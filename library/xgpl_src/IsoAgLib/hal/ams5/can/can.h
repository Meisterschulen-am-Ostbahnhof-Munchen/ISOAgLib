/***************************************************************************
    can.h - definition of Hardware Abstraction Layer for CAN functions
            for AMS5
    -------------------
    date                 : 18.06.2007
    copyright            : (c) 2007 GESAS GmbH
    email                : stefan.klueh@gesas:de
    type                 : Header
 ***************************************************************************/


#ifndef _HAL_AMS5_CAN_H_
#define _HAL_AMS5_CAN_H_

#include "hal_can_interface.h"


namespace HAL
{
   /* *********************************************************** */
   /** \name CAN BIOS functions for global status checking per BUS */
   /*@{*/

   /**
      test if the CAN BUS is in WARN state
      @param aui8_busNr number of the BUS to check (default 0)
      @return true == CAN BUS is in WARN state, else normal operation
   */
   inline bool can_stateGlobalWarn(uint8_t aui8_busNr = 0)
   {
      return __HAL::can_stateGlobalWarn(aui8_busNr);
   };

   /**
      test if the CAN BUS is in OFF state
      @param aui8_busNr number of the BUS to check (default 0)
      @return true == CAN BUS is in OFF state, else normal operation
   */
   inline bool can_stateGlobalOff(uint8_t aui8_busNr = 0)
   {
      return __HAL::can_stateGlobalOff(aui8_busNr);
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
      @param aui8_busNr number of the BUS to check (default 0)
      @return true == CAN BUS is in blocked state, else normal operation
   */
   inline bool can_stateGlobalBlocked(uint8_t aui8_busNr = 0)
   {
      return __HAL::can_stateGlobalBlocked(aui8_busNr);
   };

#ifdef USE_CAN_MEASURE_BUSLOAD
   /**
      deliver the baudrate of the CAN BUS in [kbaud]
      @param aui8_busNr number of the BUS to check (default 0)
      @return BUS load of the last second [kbaud]
   */
   inline int32_t can_stateGlobalBusload(uint8_t aui8_busNr = 0)
   {
      return __HAL::can_stateGlobalBusload(aui8_busNr);
   };
#endif

   /**
      check if a send try on this BUS caused an Bit1Error
      (occurs if >1 controller try to send msg with same ident)
      ==> ISO 11783 forces stop of retries in this case for
          adress claim
      @param aui8_busNr number of the BUS to check
      @return true -> Bit1Err occured
   */
   inline bool can_stateGlobalBit1err(uint8_t aui8_busNr = 0)
   {
   return __HAL::can_stateGlobalBit1err(aui8_busNr);
   };

   /*@}*/

   /* ******************************************************************* */
   /** \name CAN BIOS functions for status checking of one specific MsgObj */
   /*@{*/

   /**
      deliver the timestamp of last successfull CAN send action
      @param aui8_busNr number of the BUS to check
      @param aui8_msgobjNr number of the MsgObj to check
      @return timestamp of last successful send
              OR -1 if aui8_msgObjNr corresponds to no valid send obj
   */
   inline int32_t can_stateMsgobjTxok(uint8_t aui8_busNr, uint8_t aui8_msgobjNr)
   {
      return __HAL::can_stateMsgobjTxok(aui8_busNr, aui8_msgobjNr);
   };


   /**
      test if buffer of a MsgObj is full (e.g. no more
      msg can be put into buffer (important for TX objects))
      @param aui8_busNr number of the BUS to check
      @param aui8_msgobjNr number of the MsgObj to check
      @return true -> buffer is full -> no further can_send allowed
   */
   inline bool can_stateMsgobjOverflow(uint8_t aui8_busNr, uint8_t aui8_msgobjNr)
   {
      return __HAL::can_stateMsgobjOverflow(aui8_busNr, aui8_msgobjNr);
   };

   /**
      deliver amount of messages in buffer
      (interesting for RX objects)
      @param aui8_busNr number of the BUS to check
      @param aui8_msgobjNr number of the MsgObj to check
      @return number of messages in buffer or negative error code:
              C_CONFIG == BUS not initialised
              C_RANGE == wrong BUS or MsgObj number
   */
   inline uint16_t can_stateMsgobjBuffercnt(uint8_t aui8_busNr, uint8_t aui8_msgobjNr)
   {
      return __HAL::can_stateMsgobjBuffercnt(aui8_busNr, aui8_msgobjNr);
   };

   /**
      deliver amount of messages which can be placed in buffer
      (interesting for TX objects)
      @param aui8_busNr number of the BUS to check
      @param aui8_msgobjNr number of the MsgObj to check
      @return number of messages which can be placed in buffer or negative error code:
              C_CONFIG == BUS not initialised
              C_RANGE == wrong BUS or MsgObj number
   */
   inline int16_t can_stateMsgobjFreecnt(uint8_t aui8_busNr, uint8_t aui8_msgobjNr)
   {
      return __HAL::can_stateMsgobjFreecnt(aui8_busNr, aui8_msgobjNr);
   };

   /*@}*/

   /* ************************************************************ */
   /** \name CAN BIOS functions for configuration of global CAN BUS */
   /*@{*/

   /**
      init CAN BUS with given baudrate and masks
      (11bit, 29bit and last msg mask)
      @param aui8_busNr number of the BUS to init (default 0)
      @param ab_baudrate baudrate in [kbaud]
      @param aui16_maskStd 11bit global mask
      @param aui32_maskExt 29bit global mask
      @param aui32_maskLastmsg mask of last CAN msg
      @return C_NO_ERR == no error;
              C_RANGE == wrong BUS nr or wrong baudrate;
              C_WARN == BUS previously initialised - no problem if only masks had to be changed
   */
   inline int16_t can_configGlobalInit(uint8_t aui8_busNr, uint16_t ab_baudrate, uint16_t ab_maskStd,
                                       uint32_t aui32_maskExt, uint32_t aui32_maskLastmsg)
   {
      return __HAL::can_configGlobalInit(aui8_busNr, ab_baudrate, ab_maskStd,
                                         aui32_maskExt, aui32_maskLastmsg);
   };

   /**
      set the global masks
      (11bit, 29bit and last msg mask)
      @param aui8_busNr number of the BUS to config (default 0)
      @param aui16_maskStd 11bit global mask
      @param aui32_maskExt 29bit global mask
      @param aui32_maskLastmsg mask of last CAN msg
      @return C_NO_ERR == no error;
              C_RANGE == wrong BUS nr
   */
   inline int16_t can_configGlobalMask(uint8_t aui8_busNr, uint16_t ab_maskStd, uint32_t aui32_maskExt, uint32_t aui32_maskLastmsg)
   {
      return __HAL::can_configGlobalMask(aui8_busNr, ab_maskStd, aui32_maskExt, aui32_maskLastmsg);
   };

   /**
      close CAN BUS
      (important to close BUS before re-init with other baudrate)
      @param aui8_busNr number of the BUS to close (default 0)
      @return C_NO_ERR == no error;
              C_RANGE == wrong BUS nr;
              C_CONFIG == BUS previously not initialised
   */
   inline int16_t can_configGlobalClose(uint8_t aui8_busNr)
   {
      return __HAL::can_configGlobalClose(aui8_busNr);
   };

   /** wait until specified timeout or until next CAN message receive
       @return true -> there are CAN messages waiting for process. else: return due to timeout
   */
   inline bool can_waitUntilCanReceiveOrTimeout(uint16_t aui16_timeoutInterval)
   {
      return __HAL::can_waitUntilCanReceiveOrTimeout(aui16_timeoutInterval);
   };
   /*@}*/

   /* ***************************************************************** */
   /** \name CAN BIOS functions for configuration of one specific MsgObj */
   /*@{*/

   /**
      config a MsgObj
      (buffer size is defined locally in implementation of this interface;
      all received msgs should get a time stamp;
      class Ident_c has ident and type 11/29bit)
      @param aui8_busNr number of the BUS to config
      @param aui8_msgobjNr number of the MsgObj to config
      @param arc_ident filter ident of this MsgObj
      @param ab_rxtx 0==RX receive; 1==TX transmit
      @return C_NO_ERR == no error;
              C_BUSY == this MsgObj is already used
              C_CONFIG == BUS not initialised or error during buffer allocation
              C_RANGE == wrong BUS or MsgObj number
   */

//#ifdef BIOSTEST
//   inline int16_t can_configMsgobjInit(uint8_t BusNr, uint8_t ObjNr, uint32_t Ident, uint8_t Type, uint8_t RxTx)
//   {
//      return __HAL::can_configMsgobjInit(BusNr, ObjNr, Ident, Type, RxTx);
//   };
//#else
   inline int16_t can_configMsgobjInit(uint8_t aui8_busNr, uint8_t aui8_msgobjNr, __IsoAgLib::Ident_c& arc_ident, uint8_t ab_rxtx)
   {
      return __HAL::can_configMsgobjInit(aui8_busNr, aui8_msgobjNr, arc_ident, ab_rxtx);
   };
//#endif

   /**
      change the the send rate for one MsgObj by setting the minimum
      pause time between two messages [msec.]
      @param aui8_busNr number of the BUS to config
      @param aui8_msgobjNr number of the MsgObj to config
      @param aui16_minSendPause minimum send pause between two sent messages [msec.]
      @return C_NO_ERR == no error;
              C_CONFIG == BUS not initialised or ident can't be changed
              C_RANGE == wrong BUS or MsgObj number
   */
   inline int16_t can_configMsgobjSendpause(uint8_t aui8_busNr, uint8_t aui8_msgobjNr, uint16_t aui16_minSend)
   {
      return HAL_CONFIG_ERR;
   };

   /**
      close a MsgObj
      @param aui8_busNr number of the BUS to config
      @param aui8_msgobjNr number of the MsgObj to config
      @return C_NO_ERR == no error;
              C_CONFIG == BUS not initialised, MsgObj previously not used or buffer memory not freed
              C_RANGE == wrong BUS or MsgObj number
   */
   inline int16_t can_configMsgobjClose(uint8_t aui8_busNr, uint8_t aui8_msgobjNr)
   {
      return __HAL::can_configMsgobjClose(aui8_busNr, aui8_msgobjNr);
   };

   /*@}*/

   /* ****************************************************** */
   /** \name CAN BIOS functions for use of on specific MsgObj */
   /*@{*/

   /**
      send a message via a MsgObj;
      CanPkg_c (or derived object) must provide (virtual)
      functions:
      * Ident_c& getIdent() -> deliver ident of msg to send
      * uint8_t getData(MASK_TYPE& rt_ident, uint8_t& rui8_identType,
                        uint8_t& rb_dlc, uint8_t* pb_data)
      -> put DLC in referenced r_dlc and insert data in uint8_t string pb_data
      @param aui8_busNr number of the BUS to config
      @param aui8_msgobjNr number of the MsgObj to config
      @param apc_data pointer to CanPkg_c instance with data to send
      @return C_NO_ERR == no error;
              C_CONFIG == BUS not initialised, MsgObj is no send object
              C_NOACT == BUS OFF
              C_OVERFLOW == send buffer overflowed
              C_RANGE == wrong BUS or MsgObj number
   */
#ifdef BIOSTEST
   /**
      send a message via a MsgObj
      for BIOS-Test only!
   */
   inline int16_t can_useMsgobjSend(uint8_t aui8_busNr, uint8_t aui8_msgobjNr, canBuffer_pt *CanMsg_pp)
   {
      return __HAL::can_useMsgobjSend(aui8_busNr, aui8_msgobjNr, CanMsg_pp);
   };
#else
   inline int16_t can_useMsgobjSend(uint8_t aui8_busNr, uint8_t aui8_msgobjNr, __IsoAgLib::CanPkg_c* apc_data)
   {
      return __HAL::can_useMsgobjSend(aui8_busNr, aui8_msgobjNr, apc_data);
   };
#endif


   /**
      clear th buffer of a MsgObj (e.g. to stop sending retries)
      @param aui8_busNr number of the BUS to config
      @param aui8_msgobjNr number of the MsgObj to config
      @return C_NO_ERR == no error;
              C_CONFIG == BUS not initialised
              C_RANGE == wrong BUS or MsgObj number
   */
   inline int16_t can_useMsgobjClear(uint8_t aui8_busNr, uint8_t aui8_msgobjNr)
   {
      return __HAL::can_useMsgobjClear(aui8_busNr, aui8_msgobjNr);
   };

   /*@}*/
}
#endif
