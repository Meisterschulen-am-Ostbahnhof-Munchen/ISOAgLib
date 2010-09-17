/*
  target_extension_can_client_socketcan.cpp: socketcan driver implementation

  (C) Copyright 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

/*  TODO:
 *  @todo SocketCAN: think about implementation of global mask setup
 *  @todo SocketCAN: get rid of ugly debug print macros
 *  @todo SocketCAN: test with linux < 2.6.0
 *  @todo SocketCAN: implement getBusStatus function
 *  */

#include <net/if.h>
#include <sys/ioctl.h>

#include <linux/can.h>
#include <linux/can/error.h>
#include <linux/can/raw.h>
#include <linux/version.h>

#include <IsoAgLib/hal/pc/system/system.h>

#ifndef PF_CAN
#define PF_CAN 29
#endif

#ifndef AF_CAN
#define AF_CAN PF_CAN
#endif


/* prior to 2.5.x versions of socket theres a specific ioctrl for
 * setting the baudrate abailable. */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
typedef uint32_t can_baudrate_t;
#define SIOCSCANBAUDRATE (SIOCDEVPRIVATE+0)
#endif

//#define DEBUG_CAN 1
#if DEBUG_CAN
#  define DEBUG_PRINT_(args) do { printf args; fflush(0); } while (0)
#else
#  define DEBUG_PRINT_(args)
#endif

#define DEBUG_PRINT(str) DEBUG_PRINT_((str))
#define DEBUG_PRINT1(str,a) DEBUG_PRINT_((str,a))
#define DEBUG_PRINT2(str,a,b) DEBUG_PRINT_((str,a,b))
#define DEBUG_PRINT3(str,a,b,c) DEBUG_PRINT_((str,a,b,c))
#define DEBUG_PRINT4(str,a,b,c,d) DEBUG_PRINT_((str,a,b,c,d))


namespace __HAL {


  /** representation of a single msg obj */
  struct tMsgObj {
    bool     b_canBufferLock;
    bool     b_canObjConfigured;
    uint8_t  ui8_bufXtd;
    uint8_t  ui8_bMsgType;
    uint32_t ui32_filter;
    uint32_t ui32_mask;
    uint16_t ui16_size;
  };


  /** representation of a single can bus instance */
  struct canBus_s {
    canBus_s() :
        mvec_msgObj(),
        mb_busUsed( false ),
        mui16_globalMask( 0 ),
        mui32_globalMask( 0 ),
        mui32_lastMask( 0 ),
        mb_initReceived( false ),
        mi_fd( -1 ) {}

    std::vector<tMsgObj>    mvec_msgObj;
    bool                    mb_busUsed;
    uint16_t                mui16_globalMask;
    uint32_t                mui32_globalMask;
    uint32_t                mui32_lastMask;
    bool                    mb_initReceived;
    int                     mi_fd;
  };


  /** hold latest timestamp of can frame reception */
  static int32_t gsi32_lastReceiveTime = 0;
  /** information about each channel available */
  static std::vector<canBus_s> gsvec_canBus;

  /** setHandleForBus bus with range checks */
  void setHandleForBus( uint8_t ui8_bus, int ast_socket ) {
    if ( gsvec_canBus.size() <= ui8_bus )
      gsvec_canBus.resize( ui8_bus + 1 );
    gsvec_canBus[ui8_bus].mi_fd = ast_socket;
  }


  /** startup the driver */
  int16_t can_startDriver() {
    gsvec_canBus.resize( HAL_CAN_MAX_BUS_NR + 1 );
    return HAL_NO_ERR;
  }


  /** stop he driver */
  int16_t can_stopDriver() {
    DEBUG_PRINT( "can_stopDriver\n" );
    return HAL_NO_ERR;
  }


  /** initialize a can channel with give parameter
      @param bBusNumber channel number: results in interface name can<number>
      @param wGlobMask unused
      @param dwGlobMaskLastmsg unused
      @param wBitrate bitrate to set for channel
  */
  int16_t init_can( uint8_t bBusNumber, uint16_t wGlobMask, uint32_t dwGlobMask, uint32_t dwGlobMaskLastmsg, uint16_t wBitrate ) {

    DEBUG_PRINT2( "init_can bus: %d bitrate: %d\n", bBusNumber, wBitrate );

    // argument check
    if ( bBusNumber > HAL_CAN_MAX_BUS_NR ) {
      return HAL_RANGE_ERR;
    }

    if ( ! gsvec_canBus[ bBusNumber ].mb_busUsed ) {

      ( void )wBitrate; // not used for Kernel >= 2.6.

      DEBUG_PRINT1( "init_can bus: %d\n", bBusNumber );

      char fname[32];
      sprintf( fname, "can%u", bBusNumber );

      // create a socket
      int m_handle = socket( PF_CAN, SOCK_RAW, CAN_RAW );
      if ( m_handle < 0 ) {
        perror( "socket" );
        return HAL_CONFIG_ERR;
      }

      DEBUG_PRINT3( "init_can: bus: %d fname: %s m_handle: %x\n", bBusNumber, fname, m_handle );

      // get the interface
      struct ifreq ifr;
      strncpy( ifr.ifr_name, fname, IFNAMSIZ );
      int ret = ioctl( m_handle, SIOCGIFINDEX, &ifr );

      if ( ret < 0 ) {
        perror( "SIOCGIFINDEX" );
        setHandleForBus( bBusNumber, 0 );
        return HAL_CONFIG_ERR;
      }

// Setting of Bitrate not possible in >= 2.6. Kernel version
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
      // Set the baudrate.  Must be set before first use
      struct ifreq ifr_baudrate;   // Use different structure for setting the baud rate as the ioctls
      // overwrite the structure
      strcpy( ifr_baudrate.ifr_name, fname );
      can_baudrate_t *baudrate = ( can_baudrate_t * ) & ifr_baudrate.ifr_ifru;
      *baudrate = wBitrate * 1000;
      int baud_ret = ioctl( m_handle, SIOCSCANBAUDRATE, &ifr_baudrate );
      DEBUG_PRINT1( "init_can: baud_ret: %d ", baud_ret );
      if ( baud_ret < 0 ) {
        perror( "SIOCSCANBAUDRATE" );
        setHandleForBus( bBusNumber, 0 );
        return HAL_CONFIG_ERR;
      }
#endif
      // bind to the socket
      struct sockaddr_can addr;
      addr.can_family = AF_CAN;
      addr.can_ifindex = ifr.ifr_ifindex;
      int result = bind( m_handle, ( struct sockaddr* ) & addr, sizeof( addr ) );

      if ( result < 0 ) {
        perror( "bind" );
        setHandleForBus( bBusNumber, NULL );
        return HAL_CONFIG_ERR;
      }

      // enable loopback (ensure, because it may not be default in the future)
      int loopback = 1; /* 0 = disabled, 1 = enabled (default) */
      if ( -1 == setsockopt( m_handle, SOL_CAN_RAW, CAN_RAW_LOOPBACK, &loopback, sizeof( loopback ) ) ) {
        perror( "enabling loopback" );
        return HAL_CONFIG_ERR;
      }

      int recv_own_msgs = 0; /* 0 = disabled (default), 1 = enabled */
      if ( -1 == setsockopt( m_handle, SOL_CAN_RAW, CAN_RAW_RECV_OWN_MSGS, &recv_own_msgs, sizeof( recv_own_msgs ) ) ) {
        perror( "setting local echo option" );
        return HAL_CONFIG_ERR;
      }

      // set error mask
      can_err_mask_t err_mask = ( CAN_ERR_MASK );
      if ( -1 == setsockopt( m_handle, SOL_CAN_RAW, CAN_RAW_ERR_FILTER, &err_mask, sizeof( err_mask ) ) ) {
        perror( "setting error mask" );
        return HAL_CONFIG_ERR;
      }


      setHandleForBus( bBusNumber, m_handle );

      gsvec_canBus[ bBusNumber ].mui16_globalMask = wGlobMask;
      gsvec_canBus[ bBusNumber ].mui32_globalMask = dwGlobMask;
      gsvec_canBus[ bBusNumber ].mui32_lastMask = dwGlobMaskLastmsg;

      gsvec_canBus[ bBusNumber ].mb_initReceived = true;
      gsvec_canBus[ bBusNumber ].mb_busUsed = true;
    }

    return HAL_NO_ERR;
  };


  /** change global mask of channel - this function only stores the values for
   *  further usage; the data is not passed to the interface
      @param bBusNumber channel to use
      @param wGlobMask unused
      @param dwGlobMaskLastmsg unused
   */
  int16_t changeGlobalMask( uint8_t bBusNumber, uint16_t wGlobMask, uint32_t dwGlobMask, uint32_t dwGlobMaskLastmsg ) {
    DEBUG_PRINT( "changeGlobalMask\n" );

    gsvec_canBus[ bBusNumber ].mui16_globalMask = wGlobMask;
    gsvec_canBus[ bBusNumber ].mui32_globalMask = dwGlobMask;
    gsvec_canBus[ bBusNumber ].mui32_lastMask = dwGlobMaskLastmsg;

    gsvec_canBus[ bBusNumber ].mb_initReceived = true;
    gsvec_canBus[ bBusNumber ].mb_busUsed = true;

    return HAL_NO_ERR;
  };


  /** close channel
      @param bBusNumber channel to close
   */
  int16_t closeCan( uint8_t bBusNumber ) {

    DEBUG_PRINT1( "closeCan bus: %d\n", bBusNumber );

    // argument check
    if ( bBusNumber > HAL_CAN_MAX_BUS_NR ) {
      return HAL_RANGE_ERR;
    }

    if ( gsvec_canBus[ bBusNumber ].mb_busUsed ) {
      if ( 0 > close( gsvec_canBus[ bBusNumber ].mi_fd ) ) {
        perror( "close" );
      }
      setHandleForBus( bBusNumber, -1 );
      gsvec_canBus[ bBusNumber ].mb_busUsed = false;
    }

    return HAL_NO_ERR;
  };


  /** unimplemented obsoleted? */
  int16_t chgCanObjPause( uint8_t, uint8_t, uint16_t ) {
    return HAL_NO_ERR;
  }


  /** retrieve can bus state
      @param bBusNumber channel number
      @ ptStatus container for state placement
    */
  int16_t getCanBusStatus( uint8_t bBusNumber, tCanBusStatus* /* ptStatus */ ) {
    ( void )bBusNumber;
    DEBUG_PRINT1( "getCanBusStatus bus: %d\n", bBusNumber );
    return HAL_NO_ERR;
  }


  /** configure a msg object (in this enh.can-hal-case FilterBox!) - data is used during rx for consistency checks only */
  int16_t configCanObj( uint8_t bBusNumber, uint8_t bMsgObj, tCanObjConfig* ptConfig ) {

    DEBUG_PRINT2( "configCanObj bus: %d obj: %d\n", bBusNumber, bMsgObj );

    if ( bBusNumber > HAL_CAN_MAX_BUS_NR ) {
      return HAL_RANGE_ERR;
    }

    if ( bMsgObj >= gsvec_canBus[ bBusNumber ].mvec_msgObj.size() ) {
      gsvec_canBus[ bBusNumber ].mvec_msgObj.resize( bMsgObj + 1 );
    }

    gsvec_canBus[ bBusNumber ].mvec_msgObj[bMsgObj].b_canObjConfigured = true;
    gsvec_canBus[ bBusNumber ].mvec_msgObj[bMsgObj].ui8_bufXtd = ptConfig->bXtd;
    gsvec_canBus[ bBusNumber ].mvec_msgObj[bMsgObj].ui32_filter = ptConfig->dwId;
    gsvec_canBus[ bBusNumber ].mvec_msgObj[bMsgObj].ui32_mask = ptConfig->mask;
    gsvec_canBus[ bBusNumber ].mvec_msgObj[bMsgObj].ui8_bMsgType = ptConfig->bMsgType;
    gsvec_canBus[ bBusNumber ].mvec_msgObj[bMsgObj].ui16_size = ptConfig->wNumberMsgs;
    gsvec_canBus[ bBusNumber ].mvec_msgObj[bMsgObj].b_canBufferLock = false;

    return HAL_NO_ERR;
  };



  /** close a msg object - data is used during rx for consistency checks only */
  int16_t closeCanObj( uint8_t bBusNumber, uint8_t bMsgObj ) {
    DEBUG_PRINT2( "closeCanObj bus: %d obj: %d\n", bBusNumber, bMsgObj );

    if (( bBusNumber > HAL_CAN_MAX_BUS_NR ) ) return HAL_RANGE_ERR;

    gsvec_canBus[ bBusNumber ].mvec_msgObj[bMsgObj].b_canObjConfigured = false;

    gsvec_canBus[ bBusNumber ].mvec_msgObj[bMsgObj].b_canBufferLock = false;
    gsvec_canBus[ bBusNumber ].mvec_msgObj[bMsgObj].ui16_size = 0;

    // erase element if it is the last in the vector, otherwise it can stay there
    while ( gsvec_canBus[ bBusNumber ].mvec_msgObj.back().b_canObjConfigured == false )
      gsvec_canBus[ bBusNumber ].mvec_msgObj.pop_back();

    return HAL_NO_ERR;

  };


  /** reconfigure a msg object - data is used during rx for consistency checks only */
  int16_t chgCanObjId( uint8_t bBusNumber, uint8_t bMsgObj, uint32_t dwId, uint32_t mask, uint8_t bXtd ) {

    DEBUG_PRINT2( "chgCanObjId bus: %d obj: %d\n", bBusNumber, bMsgObj );
    if (( bBusNumber > HAL_CAN_MAX_BUS_NR ) ) return HAL_RANGE_ERR;

    gsvec_canBus[ bBusNumber ].mvec_msgObj[bMsgObj].b_canObjConfigured = TRUE;

    gsvec_canBus[ bBusNumber ].mvec_msgObj[bMsgObj].ui8_bufXtd = bXtd;
    gsvec_canBus[ bBusNumber ].mvec_msgObj[bMsgObj].ui32_filter = dwId;

    gsvec_canBus[ bBusNumber ].mvec_msgObj[bMsgObj].ui32_mask = mask;

    return HAL_NO_ERR;
  }
  /**
    lock a MsgObj to avoid further placement of messages into buffer.
    @param aui8_busNr number of the BUS to config
    @param aui8_msgobjNr number of the MsgObj to config
    @param ab_doLock true==lock(default); false==unlock
    @return HAL_NO_ERR == no error;
            HAL_CONFIG_ERR == BUS not initialised or ident can't be changed
            HAL_RANGE_ERR == wrong BUS or MsgObj number
         */
  int16_t lockCanObj( uint8_t aui8_busNr, uint8_t aui8_msgobjNr, bool ab_doLock ) {
    DEBUG_PRINT3( "lockCanObj bus: %d obj: %d lock: %d\n", aui8_busNr, aui8_msgobjNr, ab_doLock );

    if (( aui8_busNr > HAL_CAN_MAX_BUS_NR ) ) {
      return HAL_RANGE_ERR;
    }

    gsvec_canBus[ aui8_busNr ].mvec_msgObj[aui8_msgobjNr].b_canBufferLock = ab_doLock;

    return HAL_NO_ERR;
  }

  /**
    check if MsgObj is currently locked
    @param aui8_busNr number of the BUS to check
    @param aui8_msgobjNr number of the MsgObj to check
    @return true -> MsgObj is currently locked
  */
  bool getCanMsgObjLocked( uint8_t aui8_busNr, uint8_t aui8_msgobjNr ) {
    DEBUG_PRINT2( "getCanMsgObjLocked bus: %d obj: %d\n", aui8_busNr, aui8_msgobjNr );

    if (( aui8_busNr > HAL_CAN_MAX_BUS_NR ) ) return true;

    return gsvec_canBus[ aui8_busNr ].mvec_msgObj[aui8_msgobjNr].b_canBufferLock;
  }


  /**
    clear buffer of specific object
    @param bBusNumber channel number
    @param aui8_msgobjNr number of the MsgObj to check
  */
  int16_t clearCanObjBuf( uint8_t bBusNumber, uint8_t aui8_msgObjNr ) {
#if DEBUG_CAN
    DEBUG_PRINT2( "clearCanObjBuf bus: %d obj: %d\n", bBusNumber, aui8_msgObjNr );
#else
    ( void )aui8_msgObjNr;
#endif

    if (( bBusNumber > HAL_CAN_MAX_BUS_NR ) ) {
      return HAL_RANGE_ERR;
    }

    return HAL_NO_ERR;
  }


  /**
    get latest rx timestamp global for every channel
    @return rx timestamp in ms
  */
  int32_t can_lastReceiveTime() {
    DEBUG_PRINT( "can_lastReceiveTime\n" );
    return gsi32_lastReceiveTime;
  }

  int16_t getCanMsgBufCount( uint8_t bBusNumber, uint8_t bMsgObj ) {
    DEBUG_PRINT2( "getCanMsgBufCount %d %d\n", bBusNumber, bMsgObj );
    if ( gsvec_canBus[ bBusNumber ].mvec_msgObj.size() >= bMsgObj ) {
      return 1; /*  TODO */
    }
    return 0;
  };


  /**
    block till data is available on any opened channel or timeout occours
    @param rui16_timeoutInterval timeout in ms
  */
  bool waitUntilCanReceiveOrTimeout( uint16_t rui16_timeoutInterval ) {
    fd_set rfds;
    struct timeval s_timeout;

    FD_ZERO( &rfds );
    for ( unsigned i = 0; i < gsvec_canBus.size(); i++ ) {
      if ( gsvec_canBus[ i ].mb_busUsed ) {
        FD_SET( gsvec_canBus[ i ].mi_fd, &rfds );
      }
    }

    s_timeout.tv_sec = 0;
    s_timeout.tv_usec = rui16_timeoutInterval * 1000;

    int rc = select( FD_SETSIZE, &rfds, NULL, NULL, &s_timeout );

    return ( rc > 0 );
  };


  /** print errTest if errMask & data is true; return errMask & data */
  bool printIfError( uint32_t data, uint32_t errMask, const char* errText ) {
    if ( data & errMask ) {
      fprintf( stderr, "%d: %s ", getTime(), errText );
      return true;
    }
    return false;
  }


  /** print human readable error code on error frame reception */
  int16_t handleErrorFrame( const struct can_frame& frame ) {
    // check dlc
    if ( CAN_ERR_DLC != frame.can_dlc ) {
      fprintf( stderr, "handleErrorFrame: wrong dlc: %d!\n", frame.can_dlc );
      /*  this is something weird - check your can driver */
      return HAL_UNKNOWN_ERR;
    }

    (void) printIfError( frame.can_id, CAN_ERR_TX_TIMEOUT,            "TX timeout (by netdevice driver)" );
    (void) printIfError( frame.can_id, CAN_ERR_ACK,                   "received no ACK on transmission" );
    (void) printIfError( frame.can_id, CAN_ERR_BUSOFF,                "bus off" );
    (void) printIfError( frame.can_id, CAN_ERR_BUSERROR,              "bus error (may flood!)" );
    (void) printIfError( frame.can_id, CAN_ERR_RESTARTED,             "controller restarted" );

    /* arbitration lost in bit ... / data[0] */
    if ( printIfError( frame.can_id, CAN_ERR_LOSTARB,           "lost arbitration / data[0]: " ) ) {
      (void) printIfError( frame.can_id, CAN_ERR_LOSTARB_UNSPEC,      "unspecified\n" );
    }
    /* else bit number in bitstream */

    /* error status of CAN-controller / data[1] */
    if ( printIfError( frame.can_id, CAN_ERR_CRTL,              "controller problems / data[1]:" ) ) {
      (void) printIfError( frame.data[ 1 ], CAN_ERR_CRTL_UNSPEC,      "unspecified\n" );
      (void) printIfError( frame.data[ 1 ], CAN_ERR_CRTL_RX_OVERFLOW, "RX buffer overflow\n" );
      (void) printIfError( frame.data[ 1 ], CAN_ERR_CRTL_TX_OVERFLOW, "TX buffer overflow\n" );
      (void) printIfError( frame.data[ 1 ], CAN_ERR_CRTL_RX_WARNING,  "reached warning level for RX errors\n" );
      (void) printIfError( frame.data[ 1 ], CAN_ERR_CRTL_TX_WARNING,  "reached warning level for TX errors\n" );
      (void) printIfError( frame.data[ 1 ], CAN_ERR_CRTL_RX_PASSIVE,  "reached error passive status RX\n" );
      (void) printIfError( frame.data[ 1 ], CAN_ERR_CRTL_TX_PASSIVE,  "reached error passive status TX\n" );
    }
    /* (at least one error counter exceeds */
    /* the protocol-defined level of 127)  */

    /* error in CAN protocol (type) / data[2] */
    if ( printIfError( frame.can_id, CAN_ERR_PROT,              "protocol violations / data[2..3]: " ) ) {
      (void) printIfError( frame.data[ 2 ], CAN_ERR_PROT_UNSPEC,      "unspecified\n" );
      (void) printIfError( frame.data[ 2 ], CAN_ERR_PROT_BIT,         "single bit error\n" );
      (void) printIfError( frame.data[ 2 ], CAN_ERR_PROT_FORM,        "frame format error\n" );
      (void) printIfError( frame.data[ 2 ], CAN_ERR_PROT_STUFF,       "bit stuffing error\n" );
      (void) printIfError( frame.data[ 2 ], CAN_ERR_PROT_BIT0,        "unable to send dominant bit\n" );
      (void) printIfError( frame.data[ 2 ], CAN_ERR_PROT_BIT1,        "unable to send recessive bit\n" );
      (void) printIfError( frame.data[ 2 ], CAN_ERR_PROT_OVERLOAD,    "bus overload\n" );
      (void) printIfError( frame.data[ 2 ], CAN_ERR_PROT_ACTIVE,      "active error announcement\n" );
      (void) printIfError( frame.data[ 2 ], CAN_ERR_PROT_TX,          "error occured on transmission\n" );
      /* error in CAN protocol (location) / data[3] */
      (void) printIfError( frame.data[ 3 ], CAN_ERR_PROT_LOC_UNSPEC,  "unspecified\n" );
      (void) printIfError( frame.data[ 3 ], CAN_ERR_PROT_LOC_SOF,     "start of frame\n" );
      (void) printIfError( frame.data[ 3 ], CAN_ERR_PROT_LOC_ID28_21, "ID bits 28 - 21 (SFF: 10 - 3)\n" );
      (void) printIfError( frame.data[ 3 ], CAN_ERR_PROT_LOC_ID20_18, "ID bits 20 - 18 (SFF: 2 - 0 \n" );
      (void) printIfError( frame.data[ 3 ], CAN_ERR_PROT_LOC_SRTR,    "substitute RTR (SFF: RTR)\n" );
      (void) printIfError( frame.data[ 3 ], CAN_ERR_PROT_LOC_IDE,     "identifier extension\n" );
      (void) printIfError( frame.data[ 3 ], CAN_ERR_PROT_LOC_ID17_13, "ID bits 17-13\n" );
      (void) printIfError( frame.data[ 3 ], CAN_ERR_PROT_LOC_ID12_05, "ID bits 12-5\n" );
      (void) printIfError( frame.data[ 3 ], CAN_ERR_PROT_LOC_ID04_00, "ID bits 4-0\n" );
      (void) printIfError( frame.data[ 3 ], CAN_ERR_PROT_LOC_RTR,     "RTR\n" );
      (void) printIfError( frame.data[ 3 ], CAN_ERR_PROT_LOC_RES1,    "reserved bit 1\n" );
      (void) printIfError( frame.data[ 3 ], CAN_ERR_PROT_LOC_RES0,    "reserved bit 0\n" );
      (void) printIfError( frame.data[ 3 ], CAN_ERR_PROT_LOC_DLC,     "data length code\n" );
      (void) printIfError( frame.data[ 3 ], CAN_ERR_PROT_LOC_DATA,    "data section\n" );
      (void) printIfError( frame.data[ 3 ], CAN_ERR_PROT_LOC_CRC_SEQ, "CRC sequence\n" );
      (void) printIfError( frame.data[ 3 ], CAN_ERR_PROT_LOC_CRC_DEL, "CRC delimiter\n" );
      (void) printIfError( frame.data[ 3 ], CAN_ERR_PROT_LOC_ACK,     "ACK slot\n" );
      (void) printIfError( frame.data[ 3 ], CAN_ERR_PROT_LOC_ACK_DEL, "ACK delimiter\n" );
      (void) printIfError( frame.data[ 3 ], CAN_ERR_PROT_LOC_EOF,     "end of frame\n" );
      (void) printIfError( frame.data[ 3 ], CAN_ERR_PROT_LOC_INTERM,  "intermission\n" );
    }

    /* error status of CAN-transceiver / data[4] */
    /*                                                              CANH CANL */
    if ( printIfError( frame.can_id, CAN_ERR_TRX,                     "transceiver status  / data[4]: " ) ) {
      bool printed 
               = printIfError( frame.data[ 4 ], CAN_ERR_TRX_UNSPEC,             "0000 0000\n" );
      printed |= printIfError( frame.data[ 4 ], CAN_ERR_TRX_CANH_NO_WIRE,       "0000 0100\n" );
      printed |= printIfError( frame.data[ 4 ], CAN_ERR_TRX_CANH_SHORT_TO_BAT,  "0000 0101\n" );
      printed |= printIfError( frame.data[ 4 ], CAN_ERR_TRX_CANH_SHORT_TO_VCC,  "0000 0110\n" );
      printed |= printIfError( frame.data[ 4 ], CAN_ERR_TRX_CANH_SHORT_TO_GND,  "0000 0111\n" );
      printed |= printIfError( frame.data[ 4 ], CAN_ERR_TRX_CANL_NO_WIRE,       "0100 0000\n" );
      printed |= printIfError( frame.data[ 4 ], CAN_ERR_TRX_CANL_SHORT_TO_BAT,  "0101 0000\n" );
      printed |= printIfError( frame.data[ 4 ], CAN_ERR_TRX_CANL_SHORT_TO_VCC,  "0110 0000\n" );
      printed |= printIfError( frame.data[ 4 ], CAN_ERR_TRX_CANL_SHORT_TO_GND,  "0111 0000\n" );
      printed |= printIfError( frame.data[ 4 ], CAN_ERR_TRX_CANL_SHORT_TO_CANH, "1000 0000\n" );
      if ( ! printed ) {
        fprintf( stderr, "\n" );
      }
    }

    return HAL_UNKNOWN_ERR;
  }


// MDS NOTE: This is code is highly inefficient and needs to be optimized!
//  The receive structure should be changed so that it does not need to be copied from
//  one structure to another 3 or 4 times before sending it to the higher levels.
//  This should be done at most once!
//  There are several different CAN msg receive structures defined, and that should be
//  changed to only one through this file, and the one defined externally.
  int16_t getCanMsg( uint8_t bBusNumber, uint8_t, tReceive* ptReceive ) {

    if ( bBusNumber > HAL_CAN_MAX_BUS_NR ) {
      return HAL_RANGE_ERR;
    }

    static struct can_frame frame;
    memset( &frame, 0, sizeof( frame ) );

    if ( recv( gsvec_canBus[ bBusNumber ].mi_fd, ( char* ) &frame, sizeof( struct can_frame ), MSG_DONTWAIT ) == -1 ) {
      return HAL_UNKNOWN_ERR;
    }

    gsi32_lastReceiveTime = getTime();

    // check for err frame
    if (( CAN_ERR_FLAG & frame.can_id ) == CAN_ERR_FLAG ) {
      DEBUG_PRINT( "getCanMsg: error frame!\n" );
      return handleErrorFrame( frame );
    }

    // copy data
    ptReceive->dwId = frame.can_id & 0x7FFFFFFF;
    ptReceive->bXtd = (( frame.can_id & CAN_EFF_FLAG ) == CAN_EFF_FLAG ) ? 1 : 0;

    for ( uint8_t i_loop = 0; i_loop < gsvec_canBus[ bBusNumber ].mvec_msgObj.size(); i_loop++ ) {
      if ( ! gsvec_canBus[ bBusNumber ].mvec_msgObj[i_loop].b_canBufferLock ) {

        if (( gsvec_canBus[ bBusNumber ].mvec_msgObj[i_loop].ui8_bufXtd == ptReceive->bXtd )
            && (( ptReceive->dwId & gsvec_canBus[ bBusNumber ].mvec_msgObj[i_loop].ui32_mask ) == ( gsvec_canBus[ bBusNumber ].mvec_msgObj[i_loop].ui32_filter & gsvec_canBus[ bBusNumber ].mvec_msgObj[i_loop].ui32_mask ) ) ) {
          ptReceive->bMsgObj = i_loop;
          break;
        }
      }
    }

    ptReceive->bDlc = frame.can_dlc;
    // prevent timestamp which is in the future! (because of 10ms clock jitter)
    ptReceive->tReceiveTime.l1ms = gsi32_lastReceiveTime;

    memcpy( ptReceive->abData, frame.data, frame.can_dlc );

#if DEBUG_CAN
    DEBUG_PRINT2( "%012ld rx: %02x#", getTime(), ptReceive->dwId );
    for ( unsigned i = 0; i < frame.can_dlc; ++i ) {
      DEBUG_PRINT1( "%02x", ptReceive->abData[ i ] );
    }
    DEBUG_PRINT( "\n" );
#endif

    return HAL_NO_ERR;
  };



  int16_t sendCanMsg( uint8_t bBusNumber, uint8_t, tSend* ptSend ) {

    DEBUG_PRINT1( "sendCanMsg bus: %d\n", bBusNumber );

#if DEBUG_CAN
    DEBUG_PRINT2( "%012ld tx: %x#", getTime(), ptSend->dwId );
    for ( unsigned i = 0; i < ptSend->bDlc; ++i ) {
      DEBUG_PRINT1( "%02x", ptSend->abData[ i ] );
    }
    DEBUG_PRINT( "\n" );
#endif

    static struct can_frame frame;
    memset( &frame, 0, sizeof( struct can_frame ) );

    frame.can_id = ptSend->dwId;
    frame.can_dlc = ptSend->bDlc;
    if ( ptSend->bXtd > 0 ) {
      frame.can_id |= CAN_EFF_FLAG;
    }

    memcpy( frame.data, ptSend->abData, frame.can_dlc );

    int rc = send( gsvec_canBus[ bBusNumber ].mi_fd, ( char* ) & frame, sizeof( frame ), MSG_DONTWAIT );

    if ( -1 == rc ) {
      return HAL_UNKNOWN_ERR;
    }

    return rc;
  };


  /** TODO ON REQUEST MaxSendDelay: Change all callers of this function so that they can handle the case of returnVal<0 to interprete
   *        this as error code. THEN change this function to use negative values as error codes
   */
  int32_t getMaxSendDelay( uint8_t /* aui8_busNr */ ) {
    DEBUG_PRINT( "getMaxSendDelay\n" );
    return 0; // we don't have no error code, we can just return some error-maxDelay instead
  }

} // end namespace __HAL

// eof

