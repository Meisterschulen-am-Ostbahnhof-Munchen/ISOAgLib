/***************************************************************************
    hal_can_interface.cpp - CAN interface between BIOS/OS and IsoAgLib 
                            to concentrate CAN handling abstraction
                            within one module
    -------------------
    date                 : 26.06.2007
    copyright            : (c) 2007 GESAS GmbH
    email                : stefan.klueh@gesas:de
    type                 : Module
 ***************************************************************************/

#include "hal_can_interface.h"
#include "../commercial_BIOS/bios_ams5/ams_bios.h"
#include <IsoAgLib/driver/can/impl/ident_c.h>
#include <IsoAgLib/util/impl/canpkg_c.h>
#include <IsoAgLib/hal/ams5/system/system.h>

#include <list>
#include <stddef.h>

//#define CONFIG_CAN_MAX_SEND_WAIT_TIME 1000l
//#define CONFIG_CAN_MAX_CAN_ERR_TIME_BEFORE_SLOWERING 1000l


/* ************************************* */
/* **** Some Modul Global Variables **** */
/* ************************************* */

// __IsoAgLib::Ident_c c_cinterfIdent;

/** array of 100msec. timeslice conters of received and sent msg per BUS [uint8_t] */
__near static uint16_t               gw_cinterfBusLoad[2][10];
   
/** actual index in gwBusLoad */
__near static uint8_t                gb_cinterfBusLoadSlice[2];

/** CAN bus error state and timestamps */
__near static canErrorStateStruct_t  gt_cinterfaceBusErrorState;

/** CAN message and configuration objekts */
__near static canMsg_t               gt_cinterfMsgobjSend;
__near static canConfigObj_t         gt_cinterfMsgobjConfig;

namespace __HAL
{
  static bool b_existNewSendDelayMax = false;
  static int32_t i32_maxSendDelay = 0;
  
  static const uint32_t cui32_maxCanBusCnt = (HAL_CAN_MAX_BUS_NR + 1);
  static uint8_t ui8_cinterfLastSendBufCnt[cui32_maxCanBusCnt][16];

  struct can_timeStampAndId_t
  {
    can_timeStampAndId_t (int32_t i32_ttimeStamp, __IsoAgLib::Ident_c& rrefc_ident): i32_timeStamp(i32_ttimeStamp),rt_ident(rrefc_ident){}
    int32_t i32_timeStamp;
    __IsoAgLib::Ident_c rt_ident;
  };
  
  static CNAMESPACE::list<can_timeStampAndId_t> list_sendTimeStamps;

   /* ************************************************** */
   /** \name Global Status Per BUS
    *  Functions for status check of global CAN BUS      */
   /* ************************************************** */
   /*@{*/

   /**
      test if the CAN BUS is in WARN state
      @param rui8_busNr number of the BUS to check (default 0)
      @return true == CAN BUS is in WARN state, else normal operation
   */
   bool can_stateGlobalWarn(uint8_t rui8_busNr)
   {
      // update CAN bus error state and timestamps
      AMSBIOS::can_getBusErrorState(rui8_busNr, &gt_cinterfaceBusErrorState);
      // check if the CAN module is placed in an error-passive state now
      return (gt_cinterfaceBusErrorState.statusReg&0x2000)?true:false;
   };

   /**
      test if the CAN BUS is in OFF state
      @param rui8_busNr number of the BUS to check (default 0)
      @return true == CAN BUS is in OFF state, else normal operation
   */
   bool can_stateGlobalOff(uint8_t rui8_busNr)
   {
      // update CAN bus error state and timestamps
      AMSBIOS::can_getBusErrorState(rui8_busNr, &gt_cinterfaceBusErrorState);
      // check if the CAN module is placed in a bus-off state now
      return (gt_cinterfaceBusErrorState.statusReg&0x4000)?true:false;
    };

   /**
      check if a send try on this BUS caused an Bit1Error
      (occurs if >1 controller try to send msg with same ident)
      ==> ISO 11783 forces stop of retries in this case for
          adress claim
          @param rui8_busNr number of the BUS to check
          @return true -> Bit1Err occured
   */
   bool can_stateGlobalBit1err(uint8_t rui8_busNr)
   {
      // update CAN bus error state and timestamps
      AMSBIOS::can_getBusErrorState(rui8_busNr, &gt_cinterfaceBusErrorState);
      return (gt_cinterfaceBusErrorState.lastSuccTx>gt_cinterfaceBusErrorState.lastBit1Err)?false:true;
   };

   /**
      test if the CAN BUS is in Blocked state, which can be a sign
      for CAN controllers which are configured with different baudrates;
      this is the case if neither succesfull sent nor received msg
      is detcted AND CAN controller is in WARN or OFF state
      (the time since last succ. send/rec and the time of WARN/OFF
      can be defined with CONFIG_CAN_MAX_CAN_ERR_TIME_BEFORE_SLOWERING
      in the application specific config file isoaglib_config
      -> should not be to short to avoid false alarm)
      @param rui8_busNr number of the BUS to check (default 0)
      @return true == CAN BUS is in blocked state, else normal operation
   */
   bool can_stateGlobalBlocked(uint8_t rui8_busNr)
   {
      bool b_busBlocked = true;
      const int32_t i32_now = AMSBIOS::sys_getSystemTimer();

      /**< update CAN bus error state and timestamps */
      AMSBIOS::can_getBusErrorState(rui8_busNr, &gt_cinterfaceBusErrorState);

      /**< set b_busBlocked to false, if sign for correct work was detected */
 
      // check if successful reception within defined time
     if ((i32_now-gt_cinterfaceBusErrorState.lastSuccTx)<CONFIG_CAN_MAX_SEND_WAIT_TIME)
         b_busBlocked = false;

      // check if successful reception within defined time
      if ((i32_now-gt_cinterfaceBusErrorState.lastSuccRx)<CONFIG_CAN_MAX_SEND_WAIT_TIME)
         b_busBlocked = false;
   
      // check if no Error-Passive State was active for defined time
      if ((i32_now-gt_cinterfaceBusErrorState.lastErrPas)>CONFIG_CAN_MAX_CAN_ERR_TIME_BEFORE_SLOWERING)
         b_busBlocked = false;

      // check if no Bus-Off State was active for defined time
      if ((i32_now-gt_cinterfaceBusErrorState.lastBusOff)>CONFIG_CAN_MAX_CAN_ERR_TIME_BEFORE_SLOWERING)
         b_busBlocked = false;

      // check if no Bit1-Error happened for defined time
      if ((i32_now-gt_cinterfaceBusErrorState.lastBit1Err)>CONFIG_CAN_MAX_CAN_ERR_TIME_BEFORE_SLOWERING)
         b_busBlocked = false;

      return b_busBlocked;
   };

   /**
      deliver the baudrate of the CAN BUS in [kbaud]
      @param rui8_busNr number of the BUS to check (default 0)
      @return BUS load of the last second [kbaud]
   */
   int32_t can_stateGlobalBusload(uint8_t rui8_busNr)
   {
      if (rui8_busNr > 1) return 0;
   
      int32_t i32_baudrate = 0;
   
      for (uint8_t ui8_ind = 0; ui8_ind < 10; ui8_ind++)
      {
         i32_baudrate += (gw_cinterfBusLoad[rui8_busNr][ui8_ind] * 8);
      }
      return i32_baudrate;
   }
   
   /**
      update the CAN BUS load statistic
      @param rui8_busNr BUS number to update
      @param rb_dlc length of the detected (send or receive) message
   */
   void updateCanBusLoad(uint8_t rui8_busNr, uint8_t rb_dlc)
   {
      if (rui8_busNr > 1) return;
   
      int32_t i32_now = AMSBIOS::sys_getSystemTimer();
      uint8_t b_newSlice = ((i32_now / 100)%10);
   
      if (gb_cinterfBusLoadSlice[rui8_busNr] != b_newSlice)
      {
         gw_cinterfBusLoad[rui8_busNr][b_newSlice] = rb_dlc;
         gb_cinterfBusLoadSlice[rui8_busNr] = b_newSlice;
      }
      else
      {
         gw_cinterfBusLoad[rui8_busNr][b_newSlice] += rb_dlc;
      }
   }
  
  int32_t can_getMaxSendDelay(uint8_t rui8_busNr)
{
  b_existNewSendDelayMax = false;
  return(i32_maxSendDelay);  
}

   /*@}*/

   /* ************************************************** */
   /** \name Specific for one MsgObj
    *  Functions for status check of a single MsgObj     */
   /* ************************************************** */
   /*@{*/
   
   /**
     deliver the timestamp of last successfull CAN send action
     @param rui8_busNr number of the BUS to check  [0..1]
     @param rui8_msgobjNr number of the MsgObj to check [0..15] is ignorred
     @return timestamp of last successful send
             OR -1 if rui8_msgObjNr corresponds to no valid send obj
   */
   int32_t can_stateMsgobjTxok(uint8_t rui8_busNr, uint8_t rui8_msgobjNr)
   {
      if ( ( rui8_busNr > 1 ) || ( rui8_msgobjNr> 15 ) ) return 0;
  
      // check direction and open/close state of MSgObj: 0==RX, 1==TX, 0xFF==closed
      if (AMSBIOS::can_getMsgObjDirState(rui8_busNr, rui8_msgobjNr) != 1)
      {
         return -1;
      }
      else
      {
         // update CAN bus error state and timestamps
         AMSBIOS::can_getBusErrorState(rui8_busNr, &gt_cinterfaceBusErrorState);
         return gt_cinterfaceBusErrorState.lastSuccTx;

         // for debug with bios sourcecode #define DEBUG_RXTX in ams_can.cpp module
         // return AMSBIOS::canLastSuccRxTx[rui8_busNr][rui8_msgobjNr];
      }
   }

   /**
     check if a send MsgObj can't send msgs from buffer to the
     BUS (detecetd by comparing the inactive time with
     CONFIG_CAN_MAX_SEND_WAIT_TIME (defined in isoaglib_config)
     @param rui8_busNr number of the BUS to check  [0..1]
     @param rui8_msgobjNr number of the MsgObj to check [0..15] is ignorred
     @return true -> longer than CONFIG_CAN_MAX_SEND_WAIT_TIME no msg sent on BUS
   */
   bool can_stateMsgobjSendproblem(uint8_t rui8_busNr, uint8_t rui8_msgobjNr)
   {
      if ( ( rui8_busNr > 1 ) || ( rui8_msgobjNr> 15 ) ) return true;
      const int32_t i32_now = AMSBIOS::sys_getSystemTimer();

      // update CAN bus error state and timestamps
      AMSBIOS::can_getBusErrorState(rui8_busNr, &gt_cinterfaceBusErrorState);

      if (  ((i32_now-gt_cinterfaceBusErrorState.lastSuccTx)>CONFIG_CAN_MAX_SEND_WAIT_TIME)
          &&(AMSBIOS::can_getRingBufferSize(rui8_busNr, rui8_msgobjNr)>0) )
           return true;
      else return false;

      // for debug with bios sourcecode #define DEBUG_RXTX in ams_can.cpp module
      // and use AMSBIOS::canLastSuccRxTx[rui8_busNr][rui8_msgobjNr];
   }
   
   /**
      test if buffer of a MsgObj is full (e.g. no more
      msg can be put into buffer (important for TX objects))
      @param rui8_busNr number of the BUS to check
      @param rui8_msgobjNr number of the MsgObj to check
      @return true -> buffer is full -> no further can_send allowed
   */
   bool can_stateMsgobjOverflow(uint8_t rui8_busNr, uint8_t rui8_msgobjNr)
   {
      return (AMSBIOS::can_isRingBufferFull( rui8_busNr, rui8_msgobjNr ) == 1 )?true:false;
   };
   
   /**
      deliver amount of messages in buffer
      (interesting for RX objects)
      @param rui8_busNr number of the BUS to check
      @param rui8_msgobjNr number of the MsgObj to check
      @return number of messages in buffer or negative error code:
              HAL_CONFIG_ERR == BUS not initialised
              HAL_RANGE_ERR == wrong BUS or MsgObj number
   */
   int16_t can_stateMsgobjBuffercnt(uint8_t rui8_busNr, uint8_t rui8_msgobjNr)
   {
      if ( ( rui8_busNr > 1 ) || ( rui8_msgobjNr> 15 ) ) return HAL_RANGE_ERR;
//      if (!AMSBIOS::canInitDone[rui8_busNr])             return HAL_CONFIG_ERR;
      return AMSBIOS::can_getRingBufferSize( rui8_busNr, rui8_msgobjNr );
   };
   
   /**
      deliver amount of messages which can be placed in buffer
      (interesting for TX objects)
      @param rui8_busNr number of the BUS to check
      @param rui8_msgobjNr number of the MsgObj to check
      @return number of messages which can be placed in buffer or negative error code:
              HAL_CONFIG_ERR == BUS not initialised
              HAL_RANGE_ERR == wrong BUS or MsgObj number
   */
   int16_t can_stateMsgobjFreecnt(uint8_t rui8_busNr, uint8_t rui8_msgobjNr)
   {
      if ( ( rui8_busNr > 1 ) || ( rui8_msgobjNr> 15 ) ) return HAL_RANGE_ERR;

      // check direction and open/close state of MSgObj: 0==RX, 1==TX, 0xFF==closed
      if (AMSBIOS::can_getMsgObjDirState(rui8_busNr, rui8_msgobjNr) == 0xFF) return HAL_CONFIG_ERR;

      return AMSBIOS::can_getMsgObjFreeCnt(rui8_busNr, rui8_msgobjNr);

   };
   
   /**
      check if MsgObj is currently locked
      @param rui8_busNr number of the BUS to check
      @param rui8_msgobjNr number of the MsgObj to check
      @return true -> MsgObj is currently locked
   */
   bool can_stateMsgobjLocked(uint8_t rui8_busNr, uint8_t rui8_msgobjNr)
   {
      return (AMSBIOS::can_isMsgObjLocked(rui8_busNr, rui8_msgobjNr) == 1 )?true:false;
   };

   /*@}*/
   
   /* ************************************************** */
   /** \name Global Configuration Per BUS
    *  Functions to configure the global CAN BUS         */
   /* ************************************************** */
   /*@{*/
   
   /**
      init CAN BUS with given baudrate and masks
      (11bit, 29bit and last msg mask)
      @param rui8_busNr number of the BUS to init (default 0)
      @param rb_baudrate baudrate in [kbaud]
      @param rui16_maskStd 11bit global mask
      @param rui32_maskExt 29bit global mask
      @param rui32_maskLastmsg mask of last CAN msg
      @return HAL_NO_ERR == no error;
              HAL_RANGE_ERR == wrong BUS nr or wrong baudrate;
              HAL_WARN_ERR == BUS previously initialised - no problem if only masks had to be changed
   */
   int16_t can_configGlobalInit(uint8_t rui8_busNr, uint16_t rb_baudrate, uint16_t rb_maskStd, uint32_t rui32_maskExt, uint32_t rui32_maskLastmsg)
   {
      uint16_t cinterfaceBaudrate;

      switch (rb_baudrate)                     /*! convert baudrate for BIOS: */
      {
         case 50:   cinterfaceBaudrate = 0;    /*! -  50 kBit */
                    break;
         case 100:  cinterfaceBaudrate = 1;    /*! - 100 kBit */
                    break;
         case 125:  cinterfaceBaudrate = 2;    /*! - 125 kBit */
                    break;
         case 250:  cinterfaceBaudrate = 3;    /*! - 250 kBit */
                    break;
         case 500:  cinterfaceBaudrate = 4;    /*! - 500 kBit */
                    break;
         case 1000: cinterfaceBaudrate = 5;    /*! -   1 MBit */
                    break;
         default:   return HAL_RANGE_ERR;
      }

      canMaskStruct_t cinterfaceAcceptanceMasks;

      // Global mask register (Slot 0 to 13)
      cinterfaceAcceptanceMasks.GM_IdType  = 1; // extended format
      cinterfaceAcceptanceMasks.GlobalMask = rui32_maskExt; 

       // Local mask A register (Slot 14)
      cinterfaceAcceptanceMasks.LMA_IdType = 1; // extended format
      cinterfaceAcceptanceMasks.LocalMaskA = rui32_maskExt; 

      // Local mask B register (Slot 15) -> mask of last CAN msg
      cinterfaceAcceptanceMasks.LMB_IdType = 1; // extended format
      cinterfaceAcceptanceMasks.LocalMaskB = rui32_maskLastmsg; 

      return AMSBIOS::can_openBus(rui8_busNr, cinterfaceBaudrate, &cinterfaceAcceptanceMasks);
   };
   

   /**
      set the global masks
      (11bit, 29bit and last msg mask)
      @param rui8_busNr number of the BUS to config (default 0)
      @param rui16_maskStd 11bit global mask
      @param rui32_maskExt 29bit global mask
      @param rui32_maskLastmsg mask of last CAN msg
      @return HAL_NO_ERR == no error;
              HAL_RANGE_ERR == wrong BUS nr
   */
   int16_t can_configGlobalMask(uint8_t rui8_busNr, uint16_t rb_maskStd, uint32_t rui32_maskExt, uint32_t rui32_maskLastmsg)
   {
      canMaskStruct_t cinterfaceAcceptanceMasks;

      // Global mask register (Slot 0 to 13)
      cinterfaceAcceptanceMasks.GM_IdType  = 1; // extended format
      cinterfaceAcceptanceMasks.GlobalMask = rui32_maskExt; 

       // Local mask A register (Slot 14)
      cinterfaceAcceptanceMasks.LMA_IdType = 1; // extended format
      cinterfaceAcceptanceMasks.LocalMaskA = rui32_maskExt; 

      // Local mask B register (Slot 15) -> mask of last CAN msg
      cinterfaceAcceptanceMasks.LMB_IdType = 1; // extended format
      cinterfaceAcceptanceMasks.LocalMaskB = rui32_maskLastmsg; 

      // if the bus is already opened, only the AcceptanceMasks will be changed 
      return AMSBIOS::can_openBus(rui8_busNr, 0, &cinterfaceAcceptanceMasks);
   };
   


   /**
      close CAN BUS
      (important to close BUS before re-init with other baudrate)
      @param rui8_busNr number of the BUS to close (default 0)
      @return HAL_NO_ERR == no error;
              HAL_RANGE_ERR == wrong BUS nr;
              HAL_CONFIG_ERR == BUS previously not initialised
   */
   int16_t can_configGlobalClose(uint8_t rui8_busNr)
   {
     return AMSBIOS::can_closeBus(rui8_busNr);      
   };
   

   /**  wait until specified timeout or until next CAN message receive
     *  @return true -> there are CAN messages waiting for process. else: return due to timeout
     */
   bool can_waitUntilCanReceiveOrTimeout(uint16_t rui16_timeoutInterval)
   {
      const int32_t ci32_endWait = AMSBIOS::sys_getSystemTimer() + rui16_timeoutInterval;
      int32_t i32_waitSlice = rui16_timeoutInterval;

      // if greater than 50msec -> divide so that about 10 slices are realized
      if ( i32_waitSlice > 50 ) i32_waitSlice /= 10;
      // if still more than 50msec slice limit to 50
      if ( i32_waitSlice > 50 ) i32_waitSlice = 50;

      while (true)
      {
         for (uint16_t busInd=0; busInd<2; busInd++)
         {
            for (uint16_t msgInd = 0; msgInd<16; msgInd++)
            {
               if ( AMSBIOS::can_getRingBufferSize( busInd, msgInd ) > 0 ) return true;
            }
         }
         if (AMSBIOS::sys_getSystemTimer() >= ci32_endWait) return false;
      }
   };

   /*@}*/
   
   /* ************************************************** */
   /** \name Configuration specific for one MsgObj
    *  Functions to configure a specific MsgObj          */
   /* ************************************************** */
   /*@{*/

   /**
      config a MsgObj
      (buffer size is defined locally in implementation of this interface;
      all received msgs should get a time stamp;
      class Ident_c has ident and type 11/29bit)
      @param rui8_busNr number of the BUS to config
      @param rui8_msgobjNr number of the MsgObj to config
      @param rrefc_ident filter ident of this MsgObj
      @param rb_rxtx 0==RX receive; 1==TX transmit
      @return HAL_NO_ERR == no error;
           C_BUSY == this MsgObj is already used
           HAL_CONFIG_ERR == BUS not initialised or error during buffer allocation
           HAL_RANGE_ERR == wrong BUS or MsgObj number
   */
   int16_t can_configMsgobjInit(uint8_t rui8_busNr, uint8_t rui8_msgobjNr, __IsoAgLib::Ident_c& rrefc_ident, uint8_t rb_rxtx)
   {
      if ( ( rui8_busNr > 1 ) || ( rui8_msgobjNr> 15 ) ) return HAL_RANGE_ERR;
//      if (!AMSBIOS::canInitDone[rui8_busNr])             return HAL_CONFIG_ERR;

      canConfigObj_pt pt_config = &gt_cinterfMsgobjConfig;

      pt_config->objNr = rui8_msgobjNr;
      pt_config->dwId    = rrefc_ident.ident();

      if (rrefc_ident.identType() == __IsoAgLib::Ident_c::BothIdent)
         pt_config->idType = DEFAULT_IDENT_TYPE;
      else
         pt_config->idType = rrefc_ident.identType();

      if (rb_rxtx == 0)
      {  // receive
         pt_config->direction = 0; // receive (0) or transmit (1)
         pt_config->bufferSize = CONFIG_CAN_STD_LOAD_REC_BUF_SIZE_MIN;
         const uint32_t highLoadCheckList[] = CONFIG_CAN_HIGH_LOAD_IDENT_LIST ;
         for ( uint8_t ind = 0; ind < CONFIG_CAN_HIGH_LOAD_IDENT_CNT; ind++ )
         {
            if ( (highLoadCheckList[ind] & CONFIG_CAN_HIGH_LOAD_IDENT_MASK) // prevent "0 == 0"
                 && ((highLoadCheckList[ind] & CONFIG_CAN_HIGH_LOAD_IDENT_MASK) == (pt_config->dwId & CONFIG_CAN_HIGH_LOAD_IDENT_MASK))
               )
               {
                   pt_config->bufferSize = CONFIG_CAN_HIGH_LOAD_REC_BUF_SIZE_MIN;
                   break;
               }
         }
      }
      else
      {  // send
         pt_config->direction = 1; // receive (0) or transmit (1)
         pt_config->bufferSize = CONFIG_CAN_SEND_BUFFER_SIZE;
      }

      pt_config->bufferLock = false;

      // config a CAN MsgObj with Config-Objekt
      return AMSBIOS::can_openMsgObj(rui8_busNr, pt_config);
   };

   
   /**
      lock a MsgObj to avoid further placement of messages into buffer.
      @param rui8_busNr number of the BUS to config
      @param rui8_msgobjNr number of the MsgObj to config
      @param rb_doLock true==lock(default); false==unlock
      @return HAL_NO_ERR == no error;
              HAL_CONFIG_ERR == BUS not initialised or ident can't be changed
              HAL_RANGE_ERR == wrong BUS or MsgObj number
     */
   int16_t can_configMsgobjLock( uint8_t rui8_busNr, uint8_t rui8_msgobjNr, bool rb_doLock )
   {
      return AMSBIOS::can_setMsgObjLockBit(rui8_busNr, rui8_msgobjNr, rb_doLock);
   };

  /**
      change the Ident_c of an already initialised MsgObj
      (class __IsoAgLib::Ident_c has ident and type 11/29bit)
      @param rui8_busNr number of the BUS to config
      @param rui8_msgobjNr number of the MsgObj to config
      @param rrefc_ident filter ident of this MsgObj
      @return HAL_NO_ERR == no error;
              HAL_CONFIG_ERR == BUS not initialised or ident can't be changed
              HAL_RANGE_ERR == wrong BUS or MsgObj number
   */
   int16_t can_configMsgobjChgid(uint8_t rui8_busNr, uint8_t rui8_msgobjNr, __IsoAgLib::Ident_c& rrefc_ident)
   {
      return AMSBIOS::can_setMsgObjRxID(rui8_busNr, rui8_msgobjNr, rrefc_ident.ident(), rrefc_ident.identType());
   };


   /**
      close a MsgObj
      @param rui8_busNr number of the BUS to config
      @param rui8_msgobjNr number of the MsgObj to config
      @return HAL_NO_ERR == no error;
              HAL_CONFIG_ERR == BUS not initialised, MsgObj previously not used or buffer memory not freed
              HAL_RANGE_ERR == wrong BUS or MsgObj number
   */
   int16_t can_configMsgobjClose(uint8_t rui8_busNr, uint8_t rui8_msgobjNr)
   {
      return AMSBIOS::can_closeMsgObj(rui8_busNr, rui8_msgobjNr);
   };

   /*@}*/

   /* ******************************************** */
   /** \name Use of MsgObj
    *  Functions to use a specific MsgObj          */
   /* ******************************************** */
   /*@{*/
   
   /**
      send a message via a MsgObj;
      CanPkg_c (or derived object) must provide (virtual)
      functions:
      * Ident_c& getIdent() -> deliver ident of msg to send
      * void getData(MASK_TYPE& reft_ident, uint8_t& refui8_identType,
                     uint8_t& refb_dlcTarget, uint8_t* pb_dataTarget)
        -> put DLC in referenced ref_dlc and insert data in uint8_t string pb_data
      @param rui8_busNr number of the BUS to config
      @param rui8_msgobjNr number of the MsgObj to config
      @param rpc_data pointer to CanPkg_c instance with data to send
      @return HAL_NO_ERR == no error;
              HAL_CONFIG_ERR == BUS not initialised, MsgObj is no send object
              HAL_NOACT_ERR == BUS OFF
              HAL_OVERFLOW_ERR == send buffer overflowed
              HAL_RANGE_ERR == wrong BUS or MsgObj number
   */
   int16_t can_useMsgobjSend(uint8_t rui8_busNr, uint8_t rui8_msgobjNr, __IsoAgLib::CanPkg_c* rpc_data)
   {
     // check if some msg were sent from buffer
      canBuffer_pt pt_send = &gt_cinterfMsgobjSend;

      uint8_t b_count = ui8_cinterfLastSendBufCnt[rui8_busNr][rui8_msgobjNr];

      __IsoAgLib::Ident_c rt_ident (pt_send->dwId, (pt_send->idType == 1) ? __IsoAgLib::Ident_c::ExtendedIdent : __IsoAgLib::Ident_c::StandardIdent);

//      can_timeStampAndId_t t_can_timeStampAndId (__HAL::get_time(), rt_ident);
      can_timeStampAndId_t t_can_timeStampAndId (HAL::getTime(), rt_ident);
      list_sendTimeStamps.push_back(t_can_timeStampAndId);

      // CanPkgExt_c::getData transforms flag data to ident and 8byte string
      rpc_data->getData(pt_send->dwId, pt_send->idType, pt_send->dlc, pt_send->data);

      // pt_send->id = rpc_data->ident();
      // if (rpc_data->identType() == 1)
      // CanPkg_c::ident() and CanPkg_c::identType() changed to static
      // pt_send->id = __IsoAgLib::CanPkg_c::ident();

      if (pt_send->idType == 1)
      { // extended 29bit ident
         updateCanBusLoad(rui8_busNr, (pt_send->dlc + 4));
         // pt_send->bXtd = 1;
      }
      else
      { // standard 11bit ident
         // pt_send->bXtd = 0;
         updateCanBusLoad(rui8_busNr, (pt_send->dlc + 2));
      }

      return AMSBIOS::can_writeMsgObjTx(rui8_busNr, rui8_msgobjNr, pt_send);
   };

  

   /**
      get the ident of a received message to decide about the further
      processing before the whole data string is retreived
      @param rui8_busNr number of the BUS to config
      @param rui8_msgobjNr number of the MsgObj to config
      @param reflIdent reference to the var, where the ident should be inserted
      @return error code
              HAL_NO_ERR == No problem
              HAL_CONFIG_ERR == BUS not initialised, MsgObj is no RX object
              HAL_NOACT_ERR == BUS OFF
              HAL_OVERFLOW_ERR == send buffer overflowed
              HAL_RANGE_ERR == wrong BUS or MsgObj number
              HAL_WARN_ERR == BUS WARN or no received message
   */
   int32_t can_useMsgobjReceivedIdent(uint8_t rui8_busNr, uint8_t rui8_msgobjNr, int32_t &reflIdent)
   {
      int16_t i16_retVal = HAL_NO_ERR;
      canBuffer_pt pt_receive = NULL;

      i16_retVal = AMSBIOS::can_readMsgObjRx(rui8_busNr, rui8_msgobjNr, &pt_receive);

      if ((i16_retVal == HAL_NO_ERR) || (HAL_OVERFLOW_ERR) || (HAL_WARN_ERR))
      {
         reflIdent = pt_receive->dwId;
      }
      return i16_retVal;
   };
   
   /**
      transfer front element in buffer into the pointed CanPkg_c;
      DON'T clear this item from buffer.
      @see can_useMsgobjPopFront for explicit clear of this front item
      functions:
      * setIdent(Ident_c& rrefc_ident)
        -> set ident rrefc_ident of received msg in CANPkg
      * uint8_t setDataFromString(uint8_t* rpb_data, uint8_t rb_dlc)
        -> set DLC in CanPkg_c from rb_dlc and insert data from uint8_t string rpb_data
      @param rui8_busNr number of the BUS to config
      @param rui8_msgobjNr number of the MsgObj to config
      @param rpc_data pointer to CanPkg_c instance with data to send
      @return HAL_NO_ERR == no error;
              HAL_CONFIG_ERR == BUS not initialised, MsgObj is no RX object
              HAL_NOACT_ERR == BUS OFF
              HAL_OVERFLOW_ERR == send buffer overflowed
              HAL_RANGE_ERR == wrong BUS or MsgObj number
              HAL_WARN_ERR == BUS WARN or no received message
   */
   int16_t can_useMsgobjGet(uint8_t rui8_busNr, uint8_t rui8_msgobjNr, __IsoAgLib::CanPkg_c* rpc_data)
   {
      int16_t i16_retVal = HAL_NO_ERR;
      canBuffer_pt pt_receive = NULL;

      i16_retVal = AMSBIOS::can_readMsgObjRx(rui8_busNr, rui8_msgobjNr, &pt_receive);

      if ((i16_retVal == HAL_NO_ERR) || (HAL_OVERFLOW_ERR) || (HAL_WARN_ERR))
      {
         __IsoAgLib::CanPkg_c::setTime(pt_receive->timestamp);
         __IsoAgLib::Ident_c::identType_t IDType;

         if (pt_receive->idType == true)
         { // extended 29bit ident
            IDType = __IsoAgLib::Ident_c::ExtendedIdent;
            updateCanBusLoad(rui8_busNr, (pt_receive->dlc + 4));
         }
         else
         { // standard  11bit ident
            IDType = __IsoAgLib::Ident_c::StandardIdent;
            updateCanBusLoad(rui8_busNr, (pt_receive->dlc + 2));
         }
         // rpc_data->setIdent(pt_receive->dwId, idType);
         // CanPkg_c::setIdent changed to static member function
         __IsoAgLib::CanPkg_c::setIdent(pt_receive->dwId, IDType);
         rpc_data->setDataFromString(pt_receive->data, pt_receive->dlc);
      }
      return i16_retVal;
   };

   /**
      Either register the currenct front item of buffer as not relevant,
      or just pop the front item, as it was processed.
      This explicit pop is needed, as one CAN message shall be served to
      several CanCustomer_c instances, as long as one of them indicates a
      succesfull process of the received message.
      @param rui8_busNr number of the BUS to config
      @param rui8_msgobjNr number of the MsgObj to config
   */
   void can_useMsgobjPopFront(uint8_t rui8_busNr, uint8_t rui8_msgobjNr)
   {
      AMSBIOS::can_popDataFromRingBuffer(rui8_busNr, rui8_msgobjNr);
   };
   

   /**
      clear the buffer of a MsgObj (e.g. to stop sending retries)
      @param rui8_busNr number of the BUS to config
      @param rui8_msgobjNr number of the MsgObj to config
      @return HAL_NO_ERR == no error;
              HAL_CONFIG_ERR == BUS not initialised
              HAL_RANGE_ERR == wrong BUS or MsgObj number
   */
   int16_t can_useMsgobjClear(uint8_t rui8_busNr, uint8_t rui8_msgobjNr)
   {
      AMSBIOS::can_resetRingBuffer(rui8_busNr, rui8_msgobjNr);
      return HAL_NO_ERR;
   };

   /*@}*/
}  // end of namespace __HAL
