/*
  target_extension_can_socketcan.cpp: socketcan driver implementation

  (C) Copyright 2011 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include <net/if.h>
#include <sys/ioctl.h>

#include <linux/can.h>
#include <linux/can/error.h>
#include <linux/can/raw.h>
#include <linux/version.h>

#include <map>

#include <IsoAgLib/hal/pc/system/system.h>
#include <IsoAgLib/util/iassert.h>

#ifndef PF_CAN
#define PF_CAN 29
#endif

#ifndef AF_CAN
#define AF_CAN PF_CAN
#endif

#ifndef DEF_CAN_NETDEV_PREFIX
#define DEF_CAN_NETDEV_PREFIX "can"
#endif

#if DEBUG_CAN
#define DEBUG_PRINT(...) \
            fprintf(stderr, "%s(%d): ", __FUNCTION__, __LINE__ ); \
            fprintf(stderr, __VA_ARGS__)
#else
#  define DEBUG_PRINT(...)
#endif

/* use this define to get a very verbose output */
//#define DEBUG_CAN_VERBOSE 1

namespace __HAL {

  /** representation of a rx msg obj */
  struct rxMsgObj_s {
    rxMsgObj_s() :
        mb_lock(false),
        mb_xtd(false),
        mui32_filter(0),
        mui32_mask(0) {}
    bool     mb_lock;
    bool     mb_xtd;
    uint32_t mui32_filter;
    uint32_t mui32_mask;
  };


  /** representation of a tx msg obj */
  struct txMsgObj_s {
    txMsgObj_s() :
        mb_lock(false),
        ui8_txMsgObj(0) {}
    bool    mb_lock;        /* lock tx */
    uint8_t ui8_txMsgObj;   /* index of tx object */
  };


  /** representation of a single can bus instance */
  struct canBus_s {
    canBus_s() :
        mc_rxMsgObj(),
        mc_txMsgObj(),
        mb_initialized(false),
        mi_fd(-1) {}
    std::map<uint8_t, rxMsgObj_s> mc_rxMsgObj;      /* configured msg objects */
    std::map<uint8_t, txMsgObj_s> mc_txMsgObj;      /* configured msg objects */
    bool                          mb_initialized;   /* initialization flag */
    int                           mi_fd;            /* socket fd */
  };


  /** hold latest timestamp of can frame reception */
  static int32_t g_lastReceiveTime = 0;
  /** information about each channel available */
  static canBus_s g_bus[ HAL_CAN_MAX_BUS_NR + 1 ];
  /** maximum fd */
  static int g_fdMax = 0;
  /** fd set with opened fd for receive path */
  static fd_set g_rfds;


  /** recalculate maximum fd and set fd mask for select call */
  void recalcFd() {
    FD_ZERO(&g_rfds);
    for (uint8_t i = 0; i < HAL_CAN_MAX_BUS_NR; ++i) {
      if (g_bus[ i ].mb_initialized) {
        FD_SET(g_bus[ i ].mi_fd, &g_rfds);
        if (g_fdMax < g_bus[ i ].mi_fd) {
          g_fdMax = g_bus[ i ].mi_fd;
        }
      }
    }
  }


  /** check bus number for range */
  bool checkBusNumber(uint8_t ui8_bus) {
    return (HAL_CAN_MAX_BUS_NR >= ui8_bus);
  }


  /** set filter on bus */
  int16_t setSockFilter(const uint8_t ui8_bus) {
    DEBUG_PRINT("\n");

    isoaglib_assert( g_bus[ ui8_bus ].mb_initialized );

    if (g_bus[ ui8_bus ].mc_rxMsgObj.empty()) {
      DEBUG_PRINT("%s not msg objs for bus %d\n", __FUNCTION__, ui8_bus);
      return HAL_CONFIG_ERR;
    }

    struct can_filter filter[ g_bus[ ui8_bus ].mc_rxMsgObj.size() ];
    unsigned filterId = 0;
    for (std::map<uint8_t, rxMsgObj_s>::iterator i = g_bus[ ui8_bus ].mc_rxMsgObj.begin();
         i != g_bus[ ui8_bus ].mc_rxMsgObj.end(); ++i) {

      /* only set filter for not locked msg objects */
      if (! i->second.mb_lock) {
        filter[ filterId ].can_id = i->second.mui32_filter;
        filter[ filterId ].can_mask = i->second.mui32_mask;
        ++filterId;
      }
    }

    if (0 != filterId) {
      if (-1 == setsockopt(g_bus[ ui8_bus ].mi_fd, SOL_CAN_RAW, CAN_RAW_FILTER, &filter, filterId * sizeof(struct can_filter))) {
        perror("setsockopt");
        return HAL_CONFIG_ERR;
      }
    }

    return HAL_NO_ERR;
  }


  /** change global mask of channel;
      implementation of that functions breaks with the socketcan concept - not implemented
      @param ui8_bus channel to use
      @param ui32_globalMask
      @param ui32_globalMaskLastMsg
   */
  int16_t changeGlobalMask(uint8_t /* ui8_bus */, uint16_t /* ui16_globalMask */, uint32_t /* ui32_globalMask */, uint32_t /* ui32_globalMaskLastMsg */) {
    DEBUG_PRINT("%s\n", __FUNCTION__);
    return HAL_NO_ERR;
  };


  /** initialize a can channel with given parameter
      @param ui8_bus channel to use
      @param ui32_globalMask
      @param ui32_globalMaskLastMsg
      @param ui16_bitrate to set for channel
  */
  int16_t init_can(uint8_t ui8_bus, uint16_t ui16_globalMask, uint32_t ui32_globalMask, uint32_t ui32_globalMaskLastMsg, uint16_t /* ui16_bitrate */) {
    DEBUG_PRINT("%s: %d\n", __FUNCTION__, ui8_bus);

    /* argument check */
    if ( HAL_CAN_MAX_BUS_NR < ui8_bus) {
      return HAL_RANGE_ERR;
    }

    /* check for already initialized bus */
    if (g_bus[ ui8_bus ].mb_initialized) {
      DEBUG_PRINT("%s: bus already initialized\n", __FUNCTION__);
      return changeGlobalMask(ui8_bus, ui16_globalMask, ui32_globalMask, ui32_globalMaskLastMsg);
    }

    /*  generate device name */
    char fname[ IFNAMSIZ ];
    sprintf(fname, "%s%u", DEF_CAN_NETDEV_PREFIX, ui8_bus);

    /* create a socket */
    int fd = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (fd < 0) {
      perror("socket");
      return HAL_CONFIG_ERR;
    }

    /* get the interface index */
    struct ifreq ifr;
    strncpy(ifr.ifr_name, fname, IFNAMSIZ);
    if (-1 == ioctl(fd, SIOCGIFINDEX, &ifr)) {
      perror("SIOCGIFINDEX");
      return HAL_CONFIG_ERR;
    }

    /* bind to the socket */
    struct sockaddr_can addr;
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    if (-1 == bind(fd, (struct sockaddr*) & addr, sizeof(addr))) {
      perror("bind");
      return HAL_CONFIG_ERR;
    }

    /* enable loopback (ensure, because it may not be default in the future) */
    int loopback = 1; /* 0 = disabled, 1 = enabled (default) */
    if (-1 == setsockopt(fd, SOL_CAN_RAW, CAN_RAW_LOOPBACK, &loopback, sizeof(loopback))) {
      perror("enabling loopback");
      return HAL_CONFIG_ERR;
    }

    /* disable reception of own sent messages */
    int recv_own_msgs = 0; /* 0 = disabled (default), 1 = enabled */
    if (-1 == setsockopt(fd, SOL_CAN_RAW, CAN_RAW_RECV_OWN_MSGS, &recv_own_msgs, sizeof(recv_own_msgs))) {
      perror("setting local echo option");
      return HAL_CONFIG_ERR;
    }

    /* set error mask */
    can_err_mask_t err_mask = (CAN_ERR_MASK);
    if (-1 == setsockopt(fd, SOL_CAN_RAW, CAN_RAW_ERR_FILTER, &err_mask, sizeof(err_mask))) {
      perror("setting error mask");
      return HAL_CONFIG_ERR;
    }

    /* check for a possible new maximum fd */
    recalcFd();

    g_bus[ ui8_bus ].mi_fd = fd;
    g_bus[ ui8_bus ].mb_initialized = true;

    return HAL_NO_ERR;
  };


  /** close channel
      @param bBusNumber channel to close
   */
  int16_t closeCan(uint8_t ui8_bus) {
    DEBUG_PRINT("\n");

    isoaglib_assert( checkBusNumber( ui8_bus ) );

    /* close bus */
    if (-1 == close(g_bus[ ui8_bus ].mi_fd)) {
      perror("close");
      return HAL_UNKNOWN_ERR;
    }

    g_bus[ ui8_bus ].mb_initialized = false;
    g_bus[ ui8_bus ].mc_rxMsgObj.clear();
    g_bus[ ui8_bus ].mc_txMsgObj.clear();
    g_bus[ ui8_bus ].mi_fd = -1;

    recalcFd();

    return HAL_NO_ERR;
  };


  /** startup the driver */
  int16_t can_startDriver() {
    return HAL_NO_ERR;
  }


  /** stop he driver */
  int16_t can_stopDriver() {
    DEBUG_PRINT("\n");

#ifndef NDEBUG
    for (uint8_t i = 0; i < HAL_CAN_MAX_BUS_NR; ++i) {
      isoaglib_assert( ! g_bus[ i ].mb_initialized ); 
    }
#endif
    return HAL_NO_ERR;
  }


  /** retrieve can bus state
      @param bBusNumber channel number
      @ ptStatus container for state placement
   */
  int16_t getCanBusStatus(uint8_t /* ui8_bus */, tCanBusStatus* /* ptStatus */) {
    DEBUG_PRINT("\n");
    return HAL_NO_ERR;
  }


  /** configure a msg object (in this enh.can-hal-case FilterBox!) - data is used during rx for consistency checks only */
  int16_t configCanObj(uint8_t ui8_bus, uint8_t ui8_msgObj, tCanObjConfig* p_config) {
    DEBUG_PRINT("\n");

    isoaglib_assert( checkBusNumber( ui8_bus ) );
    isoaglib_assert( p_config );

    /* TX path */
    if (p_config->bMsgType) {

      /* Please note: the TX objects are created only for convenience and to satisfy IsoAgLib's PC target extension interface! */

      /* check number */
      if (g_bus[ ui8_bus ].mc_rxMsgObj.end() != g_bus[ ui8_bus ].mc_rxMsgObj.find(ui8_msgObj)) {
        /* number configured as rx */
        return HAL_RANGE_ERR;
      }

      txMsgObj_s obj;
      obj.mb_lock = false;
      g_bus[ui8_bus].mc_txMsgObj[ ui8_msgObj ] = obj;
      return HAL_NO_ERR;
    }
    else { /* RX path */
      /* check number */
      if (g_bus[ ui8_bus ].mc_txMsgObj.end() != g_bus[ ui8_bus ].mc_txMsgObj.find(ui8_msgObj)) {
        /* number configured as tx */
        return HAL_RANGE_ERR;
      }

      rxMsgObj_s obj;
      obj.mb_lock = false;
      obj.mb_xtd = p_config->bXtd;
      obj.mui32_filter = p_config->dwId;
      obj.mui32_mask = p_config->mask;

      g_bus[ui8_bus].mc_rxMsgObj[ ui8_msgObj ] = obj;

      return setSockFilter(ui8_bus); /* pass configured msg box filters to protocol driver */
    }
    return HAL_CONFIG_ERR;
  };



  /** close a msg object */
  int16_t closeCanObj(uint8_t ui8_bus, uint8_t ui8_msgObj) {
    DEBUG_PRINT("\n");

    isoaglib_assert( checkBusNumber( ui8_bus ) );

    /* TX path */
    std::map<uint8_t, txMsgObj_s>::iterator itx = g_bus[ ui8_bus ].mc_txMsgObj.find(ui8_msgObj);
    if (g_bus[ ui8_bus ].mc_txMsgObj.end() != itx) {
      g_bus[ ui8_bus ].mc_txMsgObj.erase(itx);
      return HAL_NO_ERR;
    }


    /*  RX path */
    std::map<uint8_t, rxMsgObj_s>::iterator irx = g_bus[ ui8_bus ].mc_rxMsgObj.find(ui8_msgObj);
    if (g_bus[ ui8_bus ].mc_rxMsgObj.end() != irx) {
      g_bus[ ui8_bus ].mc_rxMsgObj.erase(irx);
      return setSockFilter(ui8_bus);
    }

    return HAL_RANGE_ERR;
  };


  /** reconfigure a msg object */
  int16_t chgCanObjId(uint8_t ui8_bus, uint8_t ui8_msgObj, uint32_t ui32_id, uint32_t ui32_mask, uint8_t ui8_xtd) {
    DEBUG_PRINT("\n");

    isoaglib_assert( checkBusNumber( ui8_bus ) );

    std::map<uint8_t, rxMsgObj_s>::iterator i = g_bus[ ui8_bus ].mc_rxMsgObj.find(ui8_msgObj);
    if (g_bus[ ui8_bus ].mc_rxMsgObj.end() == i) {
      return HAL_RANGE_ERR;
    }

    i->second.mui32_filter = ui32_id;
    i->second.mui32_mask = ui32_mask;
    i->second.mb_xtd = (ui8_xtd == 0) ? false : true;

    return setSockFilter(ui8_bus);
  }



  /**
    lock a MsgObj to avoid further placement of messages into buffer.
    @param ui8_bus number of the BUS to config
    @param ui8_msgObj number of the MsgObj to config
    @param ab_doLock true==lock(default); false==unlock
    @return HAL_NO_ERR == no error;
            HAL_RANGE_ERR == wrong BUS or MsgObj number
         */
  int16_t lockCanObj(uint8_t ui8_bus, uint8_t ui8_msgObj, bool b_lock) {
    DEBUG_PRINT("\n");

    isoaglib_assert( checkBusNumber( ui8_bus ) );

    /* tx */
    std::map<uint8_t, txMsgObj_s>::const_iterator itx = g_bus[ ui8_bus ].mc_txMsgObj.find(ui8_msgObj);
    if (g_bus[ ui8_bus ].mc_txMsgObj.end() != itx) {
      g_bus[ ui8_bus ].mc_txMsgObj[ ui8_msgObj ].mb_lock = b_lock;
      return HAL_NO_ERR;
    }

    /* rx */
    std::map<uint8_t, rxMsgObj_s>::iterator irx = g_bus[ ui8_bus ].mc_rxMsgObj.find(ui8_msgObj);
    if (g_bus[ ui8_bus ].mc_rxMsgObj.end() != irx) {
      g_bus[ ui8_bus ].mc_rxMsgObj[ ui8_msgObj ].mb_lock = b_lock;
      return setSockFilter(ui8_bus);
    }

    return HAL_RANGE_ERR;
  }

  /**
    check if MsgObj is currently locked
    @param ui8_bus number of the BUS to check
    @param ui8_msgObj number of the MsgObj to check
    @return true -> MsgObj is currently locked
  */
  bool getCanMsgObjLocked(uint8_t ui8_bus, uint8_t ui8_msgObj) {
    DEBUG_PRINT("\n");

    isoaglib_assert( checkBusNumber( ui8_bus ) );

    /* tx */
    std::map<uint8_t, txMsgObj_s>::const_iterator itx = g_bus[ ui8_bus ].mc_txMsgObj.find(ui8_msgObj);
    if (g_bus[ ui8_bus ].mc_txMsgObj.end() != itx) {
      return itx->second.mb_lock;
    }

    /* rx */
    std::map<uint8_t, rxMsgObj_s>::const_iterator irx = g_bus[ ui8_bus ].mc_rxMsgObj.find(ui8_msgObj);
    if (g_bus[ ui8_bus ].mc_rxMsgObj.end() != irx) {
      return irx->second.mb_lock;
    }

    return false;
  }


  /**
    block till data is available on any opened channel or timeout occours
    @param ui16_timeout timeout in ms
  */
  bool waitUntilCanReceiveOrTimeout(uint16_t ui16_timeout) {
    DEBUG_PRINT("\n");

    static struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = ui16_timeout * 1000;

    fd_set rfds = g_rfds;
    /* result written to rfds is ignored! */
    int rc = select(g_fdMax + 1, &rfds, 0, 0, &timeout);

    return (rc > 0);
  };


  /** print errTest if errMask & data is true; return errMask & data */
  bool printIfError(uint32_t ui32_data, uint32_t ui32_errMask, const char* carr_errText) {
    if (ui32_data & ui32_errMask) {
      fprintf(stderr, "%d: %s ", getTime(), carr_errText);
      return true;
    }
    return false;
  }


  /** print human readable error code on error frame reception */
  int16_t handleErrorFrame(const struct can_frame& frame) {
    // check dlc
    if (CAN_ERR_DLC != frame.can_dlc) {
      fprintf(stderr, "handleErrorFrame: wrong dlc: %d!\n", frame.can_dlc);
      /*  this is something weird - check your can driver */
      return HAL_UNKNOWN_ERR;
    }

    (void) printIfError(frame.can_id, CAN_ERR_TX_TIMEOUT,            "TX timeout (by netdevice driver)");
    (void) printIfError(frame.can_id, CAN_ERR_ACK,                   "received no ACK on transmission");
    (void) printIfError(frame.can_id, CAN_ERR_BUSOFF,                "bus off");
    (void) printIfError(frame.can_id, CAN_ERR_BUSERROR,              "bus error (may flood!)");
    (void) printIfError(frame.can_id, CAN_ERR_RESTARTED,             "controller restarted");

    /* arbitration lost in bit ... / data[0] */
    if (printIfError(frame.can_id, CAN_ERR_LOSTARB,                  "lost arbitration / data[0]: ")) {
      (void) printIfError(frame.can_id, CAN_ERR_LOSTARB_UNSPEC,      "unspecified\n");
    }
    /* else bit number in bitstream */

    /* error status of CAN-controller / data[1] */
    if (printIfError(frame.can_id, CAN_ERR_CRTL,                     "controller problems / data[1]:")) {
      (void) printIfError(frame.data[ 1 ], CAN_ERR_CRTL_UNSPEC,      "unspecified\n");
      (void) printIfError(frame.data[ 1 ], CAN_ERR_CRTL_RX_OVERFLOW, "RX buffer overflow\n");
      (void) printIfError(frame.data[ 1 ], CAN_ERR_CRTL_TX_OVERFLOW, "TX buffer overflow\n");
      (void) printIfError(frame.data[ 1 ], CAN_ERR_CRTL_RX_WARNING,  "reached warning level for RX errors\n");
      (void) printIfError(frame.data[ 1 ], CAN_ERR_CRTL_TX_WARNING,  "reached warning level for TX errors\n");
      (void) printIfError(frame.data[ 1 ], CAN_ERR_CRTL_RX_PASSIVE,  "reached error passive status RX\n");
      (void) printIfError(frame.data[ 1 ], CAN_ERR_CRTL_TX_PASSIVE,  "reached error passive status TX\n");
    }
    /* (at least one error counter exceeds */
    /* the protocol-defined level of 127)  */

    /* error in CAN protocol (type) / data[2] */
    if (printIfError(frame.can_id, CAN_ERR_PROT,                     "protocol violations / data[2..3]: ")) {
      (void) printIfError(frame.data[ 2 ], CAN_ERR_PROT_UNSPEC,      "unspecified\n");
      (void) printIfError(frame.data[ 2 ], CAN_ERR_PROT_BIT,         "single bit error\n");
      (void) printIfError(frame.data[ 2 ], CAN_ERR_PROT_FORM,        "frame format error\n");
      (void) printIfError(frame.data[ 2 ], CAN_ERR_PROT_STUFF,       "bit stuffing error\n");
      (void) printIfError(frame.data[ 2 ], CAN_ERR_PROT_BIT0,        "unable to send dominant bit\n");
      (void) printIfError(frame.data[ 2 ], CAN_ERR_PROT_BIT1,        "unable to send recessive bit\n");
      (void) printIfError(frame.data[ 2 ], CAN_ERR_PROT_OVERLOAD,    "bus overload\n");
      (void) printIfError(frame.data[ 2 ], CAN_ERR_PROT_ACTIVE,      "active error announcement\n");
      (void) printIfError(frame.data[ 2 ], CAN_ERR_PROT_TX,          "error occured on transmission\n");
      /* error in CAN protocol (location) / data[3] */
      (void) printIfError(frame.data[ 3 ], CAN_ERR_PROT_LOC_UNSPEC,  "unspecified\n");
      (void) printIfError(frame.data[ 3 ], CAN_ERR_PROT_LOC_SOF,     "start of frame\n");
      (void) printIfError(frame.data[ 3 ], CAN_ERR_PROT_LOC_ID28_21, "ID bits 28 - 21 (SFF: 10 - 3)\n");
      (void) printIfError(frame.data[ 3 ], CAN_ERR_PROT_LOC_ID20_18, "ID bits 20 - 18 (SFF: 2 - 0 \n");
      (void) printIfError(frame.data[ 3 ], CAN_ERR_PROT_LOC_SRTR,    "substitute RTR (SFF: RTR)\n");
      (void) printIfError(frame.data[ 3 ], CAN_ERR_PROT_LOC_IDE,     "identifier extension\n");
      (void) printIfError(frame.data[ 3 ], CAN_ERR_PROT_LOC_ID17_13, "ID bits 17-13\n");
      (void) printIfError(frame.data[ 3 ], CAN_ERR_PROT_LOC_ID12_05, "ID bits 12-5\n");
      (void) printIfError(frame.data[ 3 ], CAN_ERR_PROT_LOC_ID04_00, "ID bits 4-0\n");
      (void) printIfError(frame.data[ 3 ], CAN_ERR_PROT_LOC_RTR,     "RTR\n");
      (void) printIfError(frame.data[ 3 ], CAN_ERR_PROT_LOC_RES1,    "reserved bit 1\n");
      (void) printIfError(frame.data[ 3 ], CAN_ERR_PROT_LOC_RES0,    "reserved bit 0\n");
      (void) printIfError(frame.data[ 3 ], CAN_ERR_PROT_LOC_DLC,     "data length code\n");
      (void) printIfError(frame.data[ 3 ], CAN_ERR_PROT_LOC_DATA,    "data section\n");
      (void) printIfError(frame.data[ 3 ], CAN_ERR_PROT_LOC_CRC_SEQ, "CRC sequence\n");
      (void) printIfError(frame.data[ 3 ], CAN_ERR_PROT_LOC_CRC_DEL, "CRC delimiter\n");
      (void) printIfError(frame.data[ 3 ], CAN_ERR_PROT_LOC_ACK,     "ACK slot\n");
      (void) printIfError(frame.data[ 3 ], CAN_ERR_PROT_LOC_ACK_DEL, "ACK delimiter\n");
      (void) printIfError(frame.data[ 3 ], CAN_ERR_PROT_LOC_EOF,     "end of frame\n");
      (void) printIfError(frame.data[ 3 ], CAN_ERR_PROT_LOC_INTERM,  "intermission\n");
    }

    /* error status of CAN-transceiver / data[4] */
    /* CANH CANL */
    if (printIfError(frame.can_id, CAN_ERR_TRX,                                "transceiver status  / data[4]: ")) {
      bool printed = false;
      printed |= printIfError(frame.data[ 4 ], CAN_ERR_TRX_UNSPEC,             "CAN_ERR_TRX_UNSPEC\n");
      printed |= printIfError(frame.data[ 4 ], CAN_ERR_TRX_CANH_NO_WIRE,       "CAN_ERR_TRX_CANH_NO_WIRE\n");
      printed |= printIfError(frame.data[ 4 ], CAN_ERR_TRX_CANH_SHORT_TO_BAT,  "CAN_ERR_TRX_CANH_SHORT_TO_BAT\n");
      printed |= printIfError(frame.data[ 4 ], CAN_ERR_TRX_CANH_SHORT_TO_VCC,  "CAN_ERR_TRX_CANH_SHORT_TO_VCC\n");
      printed |= printIfError(frame.data[ 4 ], CAN_ERR_TRX_CANH_SHORT_TO_GND,  "CAN_ERR_TRX_CANH_SHORT_TO_GND\n");
      printed |= printIfError(frame.data[ 4 ], CAN_ERR_TRX_CANL_NO_WIRE,       "CAN_ERR_TRX_CANL_NO_WIRE\n");
      printed |= printIfError(frame.data[ 4 ], CAN_ERR_TRX_CANL_SHORT_TO_BAT,  "CAN_ERR_TRX_CANL_SHORT_TO_BAT\n");
      printed |= printIfError(frame.data[ 4 ], CAN_ERR_TRX_CANL_SHORT_TO_VCC,  "CAN_ERR_TRX_CANL_SHORT_TO_VCC\n");
      printed |= printIfError(frame.data[ 4 ], CAN_ERR_TRX_CANL_SHORT_TO_GND,  "CAN_ERR_TRX_CANL_SHORT_TO_GND\n");
      printed |= printIfError(frame.data[ 4 ], CAN_ERR_TRX_CANL_SHORT_TO_CANH, "CAN_ERR_TRX_CANL_SHORT_TO_CANH\n");
      if (! printed) {
        fprintf(stderr, "\n");
      }
    }

    return HAL_UNKNOWN_ERR;
  }


  /** retrive a pending msg from bus and store in tReceive */
  int16_t getCanMsg(uint8_t ui8_bus, uint8_t, tReceive* p_receive) {
    static struct can_frame frame;

    isoaglib_assert( p_receive );
    isoaglib_assert( checkBusNumber( ui8_bus ) );


    bzero(&frame, sizeof(struct can_frame));
    bzero(p_receive, sizeof(tReceive));

    int32_t timestamp = getTime();

    if (recv(g_bus[ ui8_bus ].mi_fd, (char*) &frame, sizeof(struct can_frame), MSG_DONTWAIT) == -1) {
      /* no data */
      return HAL_UNKNOWN_ERR;
    }


    /* check for err frame */
    if ((CAN_ERR_FLAG & frame.can_id) == CAN_ERR_FLAG) {
      DEBUG_PRINT("%s: error frame!\n", __FUNCTION__);
      return handleErrorFrame(frame);
    }

    /* copy data */
    p_receive->bXtd = ((frame.can_id & CAN_EFF_FLAG) == CAN_EFF_FLAG) ? 1 : 0;
    p_receive->dwId = frame.can_id & (p_receive->bXtd ? CAN_EFF_MASK : CAN_SFF_MASK);


    /* gather msg box number. check for mask and filter first in order to avoid
     * uneeded xtd and lock flag checks, that shall be possitive in most cases on a
     * ISOBUS system. */
    bool found = false;
    for (std::map<uint8_t, rxMsgObj_s>::iterator i = g_bus[ ui8_bus ].mc_rxMsgObj.begin();
         i != g_bus[ ui8_bus ].mc_rxMsgObj.end(); ++i) {

      if ((p_receive->dwId & i->second.mui32_mask) == (i->second.mui32_filter & i->second.mui32_mask)) {

        /* check for extended/standard flag match */
        if (i->second.mb_xtd != p_receive->bXtd) {
          continue;
        }

        /* this is the correct obj - either check lock */
        if (i->second.mb_lock) {
          continue;
        }

        /* everything is ok */
        p_receive->bMsgObj = i->first;
        found = true;
        break;
      }
    }

    if (! found) {
#ifndef NDEBUG
      /* if we get here, there's a gap between the filters passed
       * to the socket and the configured msg objects. normaly this
       * should never happen so print an error even without DEBUG_CAN
       * not defined. */
      fprintf(stderr, "%012d rx: %02x#", getTime(), p_receive->dwId);
      for (unsigned i = 0; i < frame.can_dlc; ++i) {
        fprintf(stderr, "%02x", p_receive->abData[ i ]);
      }
      fprintf(stderr, " ------------ no matching msgobj found - filter setup corrupted!\n");
#endif
      return HAL_CONFIG_ERR;
    }

    g_lastReceiveTime = timestamp;
    p_receive->bDlc = frame.can_dlc;
    p_receive->tReceiveTime.l1ms = timestamp;
    memcpy(p_receive->abData, frame.data, frame.can_dlc);

#if DEBUG_CAN && DEBUG_CAN_VERBOSE
    DEBUG_PRINT("%012ld rx: %02x#", getTime(), p_receive->dwId);
    for (unsigned i = 0; i < frame.can_dlc; ++i) {
      fprintf(stderr, "%02x", p_receive->abData[ i ]);
    }
    fprintf(stderr, "\n");
#endif

    return HAL_NO_ERR;
  };



  int16_t sendCanMsg(uint8_t ui8_bus, uint8_t, tSend* p_send) {
#if DEBUG_CAN && DEBUG_CAN_VERBOSE
    DEBUG_PRINT("\n");
#endif

    if (g_bus[ui8_bus].mc_txMsgObj.empty()) {
      DEBUG_PRINT("%s: no tx object configured\n", __FUNCTION__);
      return HAL_CONFIG_ERR;
    }
    /* TODO: no possibility to check for msg obj lock */


#if DEBUG_CAN && DEBUG_CAN_VERBOSE
    DEBUG_PRINT("%012ld tx: %x#", getTime(), p_send->dwId);
    for (unsigned i = 0; i < p_send->bDlc; ++i) {
      fprintf(stderr, "%02x", p_send->abData[ i ]);
    }
    fprintf(stderr, "\n");
#endif

    static struct can_frame frame;
    bzero(&frame, sizeof(struct can_frame));

    frame.can_id = p_send->dwId;
    frame.can_dlc = p_send->bDlc;
    if (p_send->bXtd) {
      frame.can_id |= CAN_EFF_FLAG;
    }

    memcpy(frame.data, p_send->abData, frame.can_dlc);

    if (-1 == send(g_bus[ ui8_bus ].mi_fd, (char*) & frame, sizeof(struct can_frame), MSG_DONTWAIT)) {
      perror("send");
      return HAL_UNKNOWN_ERR;
    }

    return HAL_NO_ERR;
  };


  /** get latest rx timestamp global for every channel */
  int32_t can_lastReceiveTime() {
    DEBUG_PRINT("\n");
    return g_lastReceiveTime;
  }


  /** unimplemented */
  int16_t clearCanObjBuf(uint8_t, uint8_t) {
    DEBUG_PRINT("\n");
    return HAL_NO_ERR;
  }


  /** unimplemented */
  int16_t getCanMsgBufCount(uint8_t, uint8_t) {
    DEBUG_PRINT("\n");
    return 0;
  };


  /** unimplemented */
  int32_t getMaxSendDelay(uint8_t) {
    DEBUG_PRINT("\n");
    return 0;
  }


  /** unimplemented */
  int16_t chgCanObjPause(uint8_t, uint8_t, uint16_t) {
    DEBUG_PRINT("\n");
    return HAL_NO_ERR;
  }

} // end namespace __HAL

// eof
