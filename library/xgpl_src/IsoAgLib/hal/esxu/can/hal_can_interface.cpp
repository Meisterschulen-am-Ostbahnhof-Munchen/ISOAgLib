/*
  hal_can_interface.cpp: CAN interface between BIOS/OS and IsoAgLib to
    concentrate CAN handling abstraction within one module

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

/* **********************************************************
 * The module halCanInterface implements a standard
 * interface for the CAN interactions of the IsoAgLib
 * This way the the adaptation of the IsoAgLib can be
 * better restricted to the various BIOS specific the
 * directories xgpl_src/IsoAgLib/hal/xx/can/ .
 * ********************************************************** */

#include "hal_can_interface.h"
#include "../typedef.h"
#include "../config.h"
#include "../errcodes.h"

#include <IsoAgLib/driver/can/impl/ident_c.h>
#include <IsoAgLib/driver/can/impl/canpkg_c.h>
#include <list>

#include <IsoAgLib/hal/generic_utils/can/write_central_fifo.h>
#include <IsoAgLib/hal/generic_utils/can/canfifo_c.h>


#include <IsoAgLib/driver/system/impl/system_c.h>

#if DEBUG_HEAP_USEAGE || DEBUG_CAN_BUFFER_FILLING
#ifdef SYSTEM_PC
#include <iostream>
#else
#include <supplementary_driver/driver/rs232/impl/rs232io_c.h>
#endif
#include <IsoAgLib/util/impl/util_funcs.h>
#endif


using namespace std; // simple version to avoid problems with using CNAMESPACE


namespace __HAL {
  extern "C" {
    /** include the BIOS specific header into __HAL */
#include <commercial_BIOS/bios_esxu/mos10osy.h>
  }

  /* ************************************* */
  /* **** Some Modul Global Variables **** */
  /* ************************************* */
  static tCanBusStatus t_cinterfCanState;
  static tCanObjConfig t_cinterfMsgobjConfig;
  static tSend t_cinterfMsgobjSend;
  static tCanObjStatus t_cinterfMsgobjState;


  static const uint32_t cui32_maxCanBusCnt = ( HAL_CAN_MAX_BUS_NR + 1 );

  static int32_t i32_cinterfBeginBit1err[cui32_maxCanBusCnt];
  static uint8_t ui8_cinterfLastSendBufCnt[cui32_maxCanBusCnt][16];

#ifdef USE_CAN_MEASURE_BUSLOAD

  void updateCanBusLoad( uint8_t aui8_busNr, uint8_t ab_dlc );
  /** array of 100msec. timeslice conters of received and sent msg per BUS [uint8_t] */
  static uint16_t gwCinterfBusLoad[cui32_maxCanBusCnt][10];

  /** actual index in gwBusLoad */
  static uint8_t gb_cinterfBusLoadSlice[cui32_maxCanBusCnt];

#endif


  /** store size of each MsgObj - needed to answer the Free Item Cnt */
  static uint8_t ui8_cinterfBufSize[cui32_maxCanBusCnt][16];


#ifdef USE_CAN_SEND_DELAY_MEASUREMENT
  static int32_t i32_maxSendDelay = 0;

  /**
    structure to save actual time stamp and Identifier
  */
  struct can_timeStampAndId_t {
    can_timeStampAndId_t ( int32_t i32_ttimeStamp, __IsoAgLib::Ident_c& arc_ident ): i32_timeStamp( i32_ttimeStamp ),at_ident( arc_ident ) {}
    int32_t i32_timeStamp;
    __IsoAgLib::Ident_c at_ident;
  };

  static STL_NAMESPACE::list<can_timeStampAndId_t> list_sendTimeStamps;

#endif

  extern "C" {
#ifdef USE_CAN_SEND_DELAY_MEASUREMENT
    /** user defined CAN IRQ Function
        @param bBus bus number [0,1]
        @param bOjekt Message Object number [1...14] (send)
        @param tCanregister pointer to the CAN register
        @return tCanregister
      */
    tCanMsgReg HUGE_MEM * IRQ_TriggerSend( byte bBus,byte bOjekt,tCanMsgReg HUGE_MEM *tCanregister ) {


      int32_t i32_now = get_time();

      if ( !list_sendTimeStamps.empty() ) {
        if (( i32_now - list_sendTimeStamps.front().i32_timeStamp ) > i32_maxSendDelay ) {
          i32_maxSendDelay = i32_now - list_sendTimeStamps.front().i32_timeStamp;
        }
        list_sendTimeStamps.pop_front();
      }


      return tCanregister;
    }
#endif


    /** user defined CAN IRQ Function
        @param bBus bus number [0,1]
        @param bOjekt Message Object number [1...14] (send)
        @param tCanregister pointer to the CAN register
        @return tCanregister
      */

    __HAL::tCanMsgReg HUGE_MEM * IwriteCentralCanfifo( byte bBus,byte bOjekt,__HAL::tCanMsgReg HUGE_MEM *tCanregister ) {

      int32_t i32_fbIndex = -1; /** initialization value*/
      int32_t i32_msgId = (( tCanregister->tArbit.dw & 0xF8000000 ) >> 27 ) | (( tCanregister->tArbit.dw & 0xFF0000 ) >> 11 )
                          | (( tCanregister->tArbit.dw & 0xFF00 ) << 5 ) | (( tCanregister->tArbit.dw & 0xFF ) << 21 );

      if (( tCanregister->tCfg_D0.b[0] & 0x4 ) == 0 ) { // for STD ident, the bits0..17 are marked as "don't care" in the C167 documentation
        // --> shift i32_msgId down by 18 bits
        i32_msgId >>= 18; // shift down by 18 bits, as bits0..17 are "don't care" in this case
      }

#ifdef USE_CAN_MEASURE_BUSLOAD
      if (( tCanregister->tCfg_D0.b[0] & 0x4 ) != 0 ) { // extended 29bit ident
        updateCanBusLoad( bBus, ((( tCanregister->tCfg_D0.b[0] & 0xF0 ) >> 4 ) + 4 ) );
      }
      else { // standard 11bit ident
        updateCanBusLoad( bBus, ((( tCanregister->tCfg_D0.b[0] & 0xF0 ) >> 4 ) + 2 ) );
      }
#endif

      /** if the irQTable is not valid, maybe there is a reconfiguration,
      * so put all the received message in the FIFO
      */

      if ( true == HAL::isIrqTable( bBus, bOjekt - 1 ) ) {

        /** BIOS objects starts from 1 */
        HAL::findFilterBox( bBus, bOjekt - 1 ,i32_msgId,&i32_fbIndex );

        if ( i32_fbIndex == -1 ) {/** message discarded, no FB interested **/
          return 0;
          /** exit from the switch and doesn't write in the central fifo **/
        }
      }

      bool b_ret = HAL::iFifoWrite( bBus,i32_fbIndex,i32_msgId,( void* )tCanregister );

#if DEBUG_FIFO_WRITE
      if ( !b_ret ) {
        INTERNAL_DEBUG_DEVICE << "Fifo FULL" << INTERNAL_DEBUG_DEVICE_ENDL;
      }
      if ( i32_fbIndex == -1 ) {
        INTERNAL_DEBUG_DEVICE << "message received during the reconfiguration" << INTERNAL_DEBUG_DEVICE_ENDL;
      }
#endif

      return 0;
    }

    /** user defined function to retrieve the data from tCanMsgReg  */

    void getIrqData( void* inputData,_near HAL::fifoData_s* destination,uint8_t aui8_bXtd ) {

      tCanMsgReg* tCanregister = ( tCanMsgReg* )inputData;

      destination->bXtd = (( tCanregister->tCfg_D0.b[0] & 0x4 )? 1 : 0 ) ; /**Position 001 of the Configuration Data = extended or standard**/
      destination->bDlc = (( tCanregister->tCfg_D0.b[0] & 0xF0 ) >> 4 );  /** len of the data **/
      destination->abData[0] = tCanregister->tCfg_D0.b[1];
      destination->abData[1] = tCanregister->tD1_D4.b[0];
      destination->abData[2] = tCanregister->tD1_D4.b[1];
      destination->abData[3] = tCanregister->tD1_D4.b[2];
      destination->abData[4] = tCanregister->tD1_D4.b[3];
      destination->abData[5] = tCanregister->tD5_D7.b[0];
      destination->abData[6] = tCanregister->tD5_D7.b[1];
      destination->abData[7] = tCanregister->tD5_D7.b[2];

    }

  } // extern "C"

  /* ******************************************************* */
  /* ***************** Status Checking ********************* */
  /* ******************************************************* */

  /* ******************** */
  /* ***Global Per BUS*** */
  /* ******************** */
#ifdef USE_CAN_MEASURE_BUSLOAD
  /**
    update the CAN BUS load statistic
    @param aui8_busNr BUS number to update
    @param rb_dlc length ot the detected (send or receive) message
  */
  void updateCanBusLoad( uint8_t aui8_busNr, uint8_t ab_dlc ) {
    int32_t i32_now = get_time();
    uint8_t b_newSlice = (( i32_now / 100 )%10 );
    if ( gb_cinterfBusLoadSlice[aui8_busNr] != b_newSlice ) {
      gwCinterfBusLoad[aui8_busNr][b_newSlice] = ab_dlc;
      gb_cinterfBusLoadSlice[aui8_busNr] = b_newSlice;
    }
    else {
      gwCinterfBusLoad[aui8_busNr][b_newSlice] += ab_dlc;
    }
  }
#endif

  /**
    test if the CAN BUS is in WARN state
    @param aui8_busNr number of the BUS to check (default 0)
    @return true == CAN BUS is in WARN state, else normal operation
  */
  bool can_stateGlobalWarn( uint8_t aui8_busNr ) {
    get_can_bus_status( aui8_busNr, &t_cinterfCanState );
    uint16_t ui16_canState = t_cinterfCanState.wCtrlStatusReg;
    // check if WARN bit is set in CAN control status register
    return (( ui16_canState & CanStateWarn ) != 0 );
  }

  /**
    test if the CAN BUS is in OFF state
    @param aui8_busNr number of the BUS to check (default 0)
    @return true == CAN BUS is in OFF state, else normal operation
  */
  bool can_stateGlobalOff( uint8_t aui8_busNr ) {
    get_can_bus_status( aui8_busNr, &t_cinterfCanState );
    uint16_t ui16_canState = t_cinterfCanState.wCtrlStatusReg;
    // check if OFF bit is set in CAN control status register
    return (( ui16_canState & CanStateOff ) != 0 );
  }

#ifdef USE_CAN_MEASURE_BUSLOAD

  /**
    deliver the baudrate of the CAN BUS in [kbaud]
    @param aui8_busNr number of the BUS to check (default 0)
    @return BUS load of the last second [kbaud]
  */
  int32_t can_stateGlobalBusload( uint8_t aui8_busNr ) {
    int32_t i32_baudrate = 0;
    for ( uint8_t ui8_ind = 0; ui8_ind < 10; ui8_ind++ ) {
      i32_baudrate += ( gwCinterfBusLoad[aui8_busNr][ui8_ind] * 8 );
    }
    return i32_baudrate;
  }
#endif

  /**
    check if send try of this MsgObj caused an Bit1Error
    (occurs if >1 controller try to send msg with same ident)
    ==> ISO 11783 forces stop of retries in this case for
        adress claim
        @param aui8_busNr number of the BUS to check
        @return true -> Bit1Err occured
  */
  bool can_stateGlobalBit1err( uint8_t aui8_busNr ) { // if bit1err timestamp is -1 no actual Bit1Err
    // check if WARN or ERR was detected
    get_can_bus_status( aui8_busNr, &t_cinterfCanState );
    return !( i32_cinterfBeginBit1err[aui8_busNr] < 0 );
  }

  /* ***************************** */
  /* ***Specific for one MsgObj*** */
  /* ***************************** */

  /**
    test if buffer of a MsgObj is full (e.g. no more
    msg can be put into buffer (important for TX objects))
    @param aui8_busNr number of the BUS to check
    @param aui8_msgobjNr number of the MsgObj to check [0..14]
    @return true -> buffer is full -> no further can_send allowed
  */
  bool can_stateMsgobjOverflow( uint8_t aui8_busNr, uint8_t aui8_msgobjNr ) { // map aui8_msgobjNr 1 to 0
    bool b_overflow = false;


    // add offset 1 to aui8_msgobjNr as the BIOS starts counting with 1
    // whereas IsoAgLib starts with 0

    if ( get_can_obj_status( aui8_busNr, ( aui8_msgobjNr+1 ), &t_cinterfMsgobjState ) == HAL_NO_ERR ) {
      if ( t_cinterfMsgobjState.bOverflow == 1 ) b_overflow = true;

    }
    return b_overflow;
  }


  /**
    deliver amount of messages in buffer
    (interesting for RX objects)
    @param aui8_busNr number of the BUS to check
    @param aui8_msgobjNr number of the MsgObj to check
    @return number of messages in buffer or negative error code:
      HAL_CONFIG_ERR == BUS not initialised
      HAL_RANGE_ERR == wrong BUS or MsgObj number
  */
  int16_t can_stateMsgobjBuffercnt( uint8_t aui8_busNr, uint8_t aui8_msgobjNr ) {
    // add offset 1 to aui8_msgobjNr as the BIOS starts counting with 1
    // whereas IsoAgLib starts with 0
    return get_can_msg_buf_count( aui8_busNr, ( aui8_msgobjNr+1 ) );
  }

  /**
    deliver amount of messages which can be placed in buffer
    (interesting for TX objects)
    @param aui8_busNr number of the BUS to check
    @param aui8_msgobjNr number of the MsgObj to check
    @return number of messages which can be placed in buffer or negative error code:
      HAL_CONFIG_ERR == BUS not initialised
      HAL_RANGE_ERR == wrong BUS or MsgObj number
  */
  int16_t can_stateMsgobjFreecnt( uint8_t aui8_busNr, uint8_t aui8_msgobjNr ) { // add offset 1 to aui8_msgobjNr as the BIOS starts counting with 1
    // whereas IsoAgLib starts with 0
    int16_t i16_msgcnt = get_can_msg_buf_count( aui8_busNr, ( aui8_msgobjNr+1 ) );
    if ( i16_msgcnt < 0 )
      return i16_msgcnt;
    else
      return ( ui8_cinterfBufSize[aui8_busNr][aui8_msgobjNr] - i16_msgcnt );
  }

  /* ***************************************************** */
  /* ***************** Configuration ********************* */
  /* ***************************************************** */

  /* ******************** */
  /* ***Global Per BUS*** */
  /* ******************** */

  /**
    init CAN BUS with given baudrate and masks
    (11bit, 29bit and last msg mask)
    @param aui8_busNr number of the BUS to init (default 0)
    @param ab_baudrate baudrate in [kbaud]
    @param aui16_maskStd 11bit global mask
    @param aui32_maskExt 29bit global mask
    @param aui32_maskLastmsg mask of last CAN msg
    @return HAL_NO_ERR == no error;
            HAL_RANGE_ERR == wrong BUS nr or wrong baudrate;
            HAL_WARN_ERR == BUS previously initialised - no problem if only masks had to be changed
  */
  int16_t can_configGlobalInit( uint8_t aui8_busNr, uint16_t ab_baudrate, uint16_t aui16_maskStd, uint32_t aui32_maskExt, uint32_t aui32_maskLastmsg ) {
    // init variables
    int32_t i32_now = get_time();
    i32_cinterfBeginBit1err[aui8_busNr] = -1;

#ifdef USE_CAN_MEASURE_BUSLOAD
    gb_cinterfBusLoadSlice[aui8_busNr] = 0;
    // cnt 0xFF ist sign, that this MsgObj isn't configured for send
    CNAMESPACE::memset(( gwCinterfBusLoad[aui8_busNr] ),0,10 );
#endif

    // now config BUS
    return init_can( aui8_busNr, aui16_maskStd, aui32_maskExt, aui32_maskLastmsg, ab_baudrate );
  }

  /**
    set the global masks
    (11bit, 29bit and last msg mask)
    @param aui8_busNr number of the BUS to config (default 0)
    @param aui16_maskStd 11bit global mask
    @param aui32_maskExt 29bit global mask
    @param aui32_maskLastmsg mask of last CAN msg
    @return HAL_NO_ERR == no error;
            HAL_RANGE_ERR == wrong BUS nr
  */
  int16_t can_configGlobalMask( uint8_t aui8_busNr, uint16_t aui16_maskStd, uint32_t aui32_maskExt, uint32_t aui32_maskLastmsg ) {
    get_can_bus_status( aui8_busNr, &t_cinterfCanState );
    // the STW BIOS init_can simply changes the global masks, and ignores the bitrate, when init_can is called for an already
    // configured CAN BUS
    int16_t i16_retVal = init_can( aui8_busNr, aui16_maskStd, aui32_maskExt, aui32_maskLastmsg, t_cinterfCanState.wBitrate );
    if ( i16_retVal == HAL_WARN_ERR ) i16_retVal = HAL_NO_ERR;
    return i16_retVal;
  }

  /**
    close CAN BUS
    (important to close BUS before re-init with other baudrate)
    @param aui8_busNr number of the BUS to close (default 0)
    @return HAL_NO_ERR == no error;
            HAL_RANGE_ERR == wrong BUS nr;
            HAL_CONFIG_ERR == BUS previously not initialised
  */
  int16_t can_configGlobalClose( uint8_t aui8_busNr ) {
    return close_can( aui8_busNr );
  }

#define MAX_SLEEP_SLICE 5
  /** wait until specified timeout or until next CAN message receive
   *  @return true -> there are CAN messages waiting for process. else: return due to timeout
   */
  bool can_waitUntilCanReceiveOrTimeout( uint16_t aui16_timeoutInterval ) {
    const int32_t ci32_endWait = __IsoAgLib::System_c::getTime() + aui16_timeoutInterval;
    int32_t i32_waitSlice = aui16_timeoutInterval;

    // if greater than MAX_SLEEP_SLICE msec -> divide so that about 10 slices are realized
    if ( i32_waitSlice > MAX_SLEEP_SLICE ) i32_waitSlice /= 10;
    // if still more than 10msec slice limit to MAX_SLEEP_SLICE
    if ( i32_waitSlice > MAX_SLEEP_SLICE ) i32_waitSlice = MAX_SLEEP_SLICE;


    while ( true ) {
      // check whether any message is waiting for processing in central FIFO
      // return true, as soon as at least one message is available
      for ( unsigned int busInd = 0; busInd < cui32_maxCanBusCnt; busInd++ ) {

        /** a message is available */
        if ( HAL::iFifoIsMsgAvailable( busInd ) ) return true;
      }

      delay_us( i32_waitSlice * 1000 );
      if ( __IsoAgLib::System_c::getTime() >= ci32_endWait ) return false;
    }
  }


  /* ***************************** */
  /* ***Specific for one MsgObj*** */
  /* ***************************** */

  /**
    config a MsgObj
    (buffer size is defined locally in implementation of this interface;
    all received msgs should get a time stamp;
    class __IsoAgLib::Ident_c has ident and type 11/29bit)
    @param aui8_busNr number of the BUS to config
    @param aui8_msgobjNr number of the MsgObj to config
    @param arc_ident filter ident of this MsgObj
    @param ab_rxtx 0==RX receive; 1==TX transmit
    @return HAL_NO_ERR == no error;
            C_BUSY == this MsgObj is already used
            HAL_CONFIG_ERR == BUS not initialised or error during buffer allocation
            HAL_RANGE_ERR == wrong BUS or MsgObj number
  */
  int16_t can_configMsgobjInit( uint8_t aui8_busNr, uint8_t aui8_msgobjNr, __IsoAgLib::Ident_c& arc_ident, uint8_t ab_rxtx ) {
    tCanObjConfig* pt_config = &t_cinterfMsgobjConfig;
    pt_config->dwId = arc_ident.ident();


    pt_config->bXtd = arc_ident.identType();


    if ( ab_rxtx == 0 ) { // receive

      // retrieve current global masks
      get_can_bus_status( aui8_busNr, &t_cinterfCanState );


      ui8_cinterfLastSendBufCnt[aui8_busNr][aui8_msgobjNr] = 0xFF;
      pt_config->bMsgType = RX;
      pt_config->pfIrqFunction = IwriteCentralCanfifo;
      pt_config->wNumberMsgs = 0;

    }
    else { // send
      ui8_cinterfLastSendBufCnt[aui8_busNr][aui8_msgobjNr] = 0;
      pt_config->bMsgType = TX;


#ifdef USE_CAN_SEND_DELAY_MEASUREMENT
      // user defined IRQ Function
      pt_config->pfIrqFunction = IRQ_TriggerSend;
#else
      pt_config->pfIrqFunction = 0;
#endif

      pt_config->wNumberMsgs = CONFIG_CAN_SEND_BUFFER_SIZE;
#ifdef USE_CAN_SEND_DELAY_MEASUREMENT
      // clear send timestamp list
      list_sendTimeStamps.erase( list_sendTimeStamps.begin(),list_sendTimeStamps.end() );
#endif

    }
    ui8_cinterfBufSize[aui8_busNr][aui8_msgobjNr] = pt_config->wNumberMsgs;
    pt_config->bTimeStamped = false;
    pt_config->wPause = 0;

#if DEBUG_CAN_BUFFER_FILLING
    char temp[100];
    sprintf( temp, "Init CAN MsgObj with: Bus %hd, MsgObj: %hd, Filter: 0x%lx, FIFO-Size: %d\r\n",
             aui8_busNr, aui8_msgobjNr, pt_config->dwId, pt_config->wNumberMsgs );
    INTERNAL_DEBUG_DEVICE << temp << INTERNAL_DEBUG_DEVICE_ENDL;
#endif

    // add offset 1 to aui8_msgobjNr as the BIOS starts counting with 1
    // whereas IsoAgLib starts with 0
    return config_can_obj( aui8_busNr, ( aui8_msgobjNr+1 ), pt_config );
  }



  /**
    change the the send rate for one MsgObj by setting the minimum
    pause time between two messages [msec.]
    @param aui8_busNr number of the BUS to config
    @param aui8_msgobjNr number of the MsgObj to config
    @param aui16_minSend minimum send pause between two sent messages [msec.]
    @return HAL_NO_ERR == no error;
            HAL_CONFIG_ERR == BUS not initialised or ident can't be changed
            HAL_RANGE_ERR == wrong BUS or MsgObj number
  */
  int16_t can_configMsgobjSendpause( uint8_t aui8_busNr, uint8_t aui8_msgobjNr, uint16_t aui16_minSend ) {
    // add offset 1 to aui8_msgobjNr as the BIOS starts counting with 1
    // whereas IsoAgLib starts with 0
    return chg_can_obj_pause( aui8_busNr, ( aui8_msgobjNr+1 ), aui16_minSend );
  }

  /**
    close a MsgObj
    @param aui8_busNr number of the BUS to config
    @param aui8_msgobjNr number of the MsgObj to config
    @return HAL_NO_ERR == no error;
            HAL_CONFIG_ERR == BUS not initialised, MsgObj previously not used or buffer memory not freed
            HAL_RANGE_ERR == wrong BUS or MsgObj number
  */
  int16_t can_configMsgobjClose( uint8_t aui8_busNr, uint8_t aui8_msgobjNr ) { // add offset 1 to aui8_msgobjNr as the BIOS starts counting with 1
    // whereas IsoAgLib starts with 0
    if (( aui8_busNr >= cui32_maxCanBusCnt ) || ( aui8_msgobjNr> 14 ) ) return HAL_RANGE_ERR;

    return close_can_obj( aui8_busNr, ( aui8_msgobjNr+1 ) );
  }

  /* ***************************************************** */
  /* ***************** Use of MsgObj ********************* */
  /* ***************************************************** */

#ifdef USE_CAN_SEND_DELAY_MEASUREMENT
  int32_t can_getMaxSendDelay( void ) {
    return( i32_maxSendDelay );
  }
#endif
  /**
    send a message via a MsgObj;
    CanPkg_c (or derived object) must provide (virtual)
    functions:
    * MASK_TYPE ident() -> deliver ident value
    * __IsoAgLib::Ident_c::identType_t identType() -> deliver type of ident
    * void getData(MASK_TYPE& rt_ident, uint8_t& rui8_identType,
                   uint8_t& rb_dlcTarget, uint8_t* pb_dataTarget)
      -> put DLC in referenced r_dlc and insert data in uint8_t string pb_data
    @param aui8_busNr number of the BUS to config
    @param aui8_msgobjNr number of the MsgObj to config
    @param apc_data pointer to CanPkg_c instance with data to send
    @return HAL_NO_ERR == no error;
            HAL_CONFIG_ERR == BUS not initialised, MsgObj is no send object
            HAL_NOACT_ERR == BUS OFF
            HAL_OVERFLOW_ERR == send buffer overflowed
            HAL_RANGE_ERR == wrong BUS or MsgObj number
            HAL_NEW_SEND_DELAY == new send delay is stored
  */
  int16_t can_useMsgobjSend( uint8_t aui8_busNr, uint8_t aui8_msgobjNr, __IsoAgLib::CanPkg_c* apc_data ) { // check if some msg were sent from buffer
    tSend* pt_send = &t_cinterfMsgobjSend;
    uint8_t b_count = ui8_cinterfLastSendBufCnt[aui8_busNr][aui8_msgobjNr];



#ifdef USE_CAN_SEND_DELAY_MEASUREMENT
    __IsoAgLib::Ident_c at_ident( pt_send->dwId, ( pt_send->bXtd == 1 ) ? __IsoAgLib::Ident_c::ExtendedIdent : __IsoAgLib::Ident_c::StandardIdent );
    can_timeStampAndId_t t_can_timeStampAndId( __HAL::get_time(), at_ident );
    list_sendTimeStamps.push_back( t_can_timeStampAndId );
#endif


    b_count = ui8_cinterfLastSendBufCnt[aui8_busNr][aui8_msgobjNr];
    // CanPkgExt_c::getData transforms flag data to ident and 8byte string
    apc_data->getData( pt_send->dwId, pt_send->bXtd, pt_send->bDlc, pt_send->abData );

#ifdef USE_CAN_MEASURE_BUSLOAD
    if ( pt_send->bXtd == 1 ) { // extended 29bit ident
      updateCanBusLoad( aui8_busNr, ( pt_send->bDlc + 4 ) );
      // pt_send->bXtd = 1;
    }
    else { // standard 11bit ident
      // pt_send->bXtd = 0;
      updateCanBusLoad( aui8_busNr, ( pt_send->bDlc + 2 ) );
    }
#endif

    // increase counter of to be sent msg in buffer
    ui8_cinterfLastSendBufCnt[aui8_busNr][aui8_msgobjNr] = b_count + 1;
#if 0
    if (( pt_send->bDlc == 3 )
        && ( aui8_busNr == 1 )
        && ( pt_send->bXtd == 0 )
       ) {
      IsoAgLib::getIrs232Instance()
      << "Alive Msg?? -\n\rIdent:" << pt_send->dwId << ", MsgObj Nr: "
      << uint16_t( aui8_msgobjNr ) << "\n\r";
    }
#endif

#if 0
    int16_t retval = send_can_msg( aui8_busNr, ( aui8_msgobjNr+1 ), pt_send );
    char testString[500];
    sprintf( testString, "ret = %d, send_can_msg( %d, %d, { { %ld, %d }, len=%d { %d, %d, %d, %d, %d, %d, %d, %d } } )\r"
             , ( int ) retval
             , ( int )aui8_busNr, ( int )aui8_msgobjNr
             , ( long int )pt_send->dwId, ( int )pt_send->bXtd
             , ( int )pt_send->bDlc
             , ( int )pt_send->abData[0]
             , ( int )pt_send->abData[1]
             , ( int )pt_send->abData[2]
             , ( int )pt_send->abData[3]
             , ( int )pt_send->abData[4]
             , ( int )pt_send->abData[5]
             , ( int )pt_send->abData[6]
             , ( int )pt_send->abData[7]
           );
    put_rs232_string( RS232_1, ( __HAL::byte* )testString );
    return retval;
#endif

#ifdef USE_CAN_SEND_DELAY_MEASUREMENT
    // add offset 1 to aui8_msgobjNr as ESX BIOS starts counting with 1
    // whereas IsoAgLib starts with 0
    int16_t i16_retSend = send_can_msg( aui8_busNr, ( aui8_msgobjNr+1 ), pt_send );

    /** when the send fails the IRQ function is not called. The element of the list_sendTimeStamps should be removed **/
    if ( i16_retSend != HAL_NO_ERR ) {
      if ( !list_sendTimeStamps.empty() ) {
        list_sendTimeStamps.pop_front();
      }
    }

    return i16_retSend;
#else
    // add offset 1 to aui8_msgobjNr as ESX BIOS starts counting with 1
    // whereas IsoAgLib starts with 0
    return send_can_msg( aui8_busNr, ( aui8_msgobjNr+1 ), pt_send );
#endif
  }


  /**
    clear th buffer of a MsgObj (e.g. to stop sending retries)
    @param aui8_busNr number of the BUS to config
    @param aui8_msgobjNr number of the MsgObj to config
    @return HAL_NO_ERR == no error;
            HAL_CONFIG_ERR == BUS not initialised
            HAL_RANGE_ERR == wrong BUS or MsgObj number
  */
  int16_t can_useMsgobjClear( uint8_t aui8_busNr, uint8_t aui8_msgobjNr ) {
    // add offset 1 to aui8_msgobjNr as the BIOS starts counting with 1
    // whereas IsoAgLib starts with 0
    return clear_can_obj_buf( aui8_busNr, ( aui8_msgobjNr+1 ) );
  }
} // end of namespace
