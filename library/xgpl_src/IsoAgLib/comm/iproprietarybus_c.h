/*
  iproprietarybus.h - Interface access class for the proprietary CAN-busses

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef IPROPRIETARYBUS_C_H
#define IPROPRIETARYBUS_C_H

#include <IsoAgLib/driver/can/impl/canio_c.h>
#include <IsoAgLib/driver/can/iident_c.h>
#include <IsoAgLib/driver/can/icancustomer_c.h>
#include <IsoAgLib/driver/can/icanpkg_c.h>
#include <IsoAgLib/driver/can/imaskfilter_c.h>
#include <IsoAgLib/util/impl/singleton.h>

#if 0 < PROP_INSTANCE_CNT
namespace IsoAgLib
{

/**
  @short Class managing a Proprietary CAN bus.
  @author Martin Wodok
  @see CanIo_c
*/
class iProprietaryBus_c {
  MACRO_MULTITON_CONTRIBUTION();
 public:
  /**
    Initialize the CAN hardware, and instantiate one msg object for
     sending of messages. Do configuration for BUS number, sending bitrate,
    CAN ident length, min/max hardware/BIOS Msg Obj numbers by parameters;
    called by specified constructor or external functions;
    wrong BUS and msg obj numbers are rejected and cause set of Err_c:range

    possible errors:
        * Err_c::range on undefined BUS,  msgOb_nr or sendBufferSize,
        * Err_c::hwConfig on uninitialized BUS, undef. msgType or CAN-BIOS mem-err,
        * Err_c::busy on already used sending Msg-Obj
    @param aui8_busNumber number of the CAN bus
    @param aui16_bitrate bitrate (default by define in isoaglib_config.h)
    @param aui8_minObjNr optional minimum number for hardware CAN
           message object (important for sharing CAN controller with
         other tasks) (default by define in isoaglib_config.h)
    @param aui8_maxObjNr optional maximum number for hardware CAN
           message object (default by define in isoaglib_config.h)
    @return true -> correct initialisation without errors
  */
  inline bool init(
    uint8_t aui8_busNumber,
    uint16_t aui16_bitrate,
    uint8_t aui8_minObjNr = 0,
    uint8_t aui8_maxObjNr = (CONFIG_CAN_OBJ_CNT-1))
  { return __IsoAgLib::getCanInstance4Prop().init (aui8_busNumber, aui16_bitrate, aui8_minObjNr, aui8_maxObjNr); }

  /** Close the opened Proprietary CAN-bus */
  inline void close() { return __IsoAgLib::getCanInstance4Prop().close(); }

 #ifdef USE_CAN_MEASURE_BUSLOAD
  /**
    deliver actual BUS load in baud
    @return baudrate in [baud] on used CAN BUS
  */
  int16_t getBusLoad() const { return getCanInstance4Prop().getBusLoad(); }
 #endif

  /**
    deliver the numbers which can be placed at the moment in the send buffer
    @param ren_identType type of searched ident: standard 11bit or extended 29bit
    @return number of msgs which fit into send buffer
  */
  uint8_t sendCanFreecnt()
  { return __IsoAgLib::getCanInstance4Prop().sendCanFreecnt(); }

  /**
    clear the send buffer
    @param ren_identType type of searched ident: standard 11bit or extended 29bit
  */
  void sendCanClearbuf() { __IsoAgLib::getCanInstance4Prop().sendCanClearbuf(); }

  /** set the new maximum send delay
      @param ai32_maxSendDelay new maximum send delay in milli-seconds
   */
  void setMaxSendDelay (int32_t ai32_maxSendDelay)
  { __IsoAgLib::getCanInstance4Prop().setMaxSendDelay (ai32_maxSendDelay); }

  /**
    test if a FilterBox_c definition already exist
    @param ar_customer reference to the processing class ( the same filter setting can be registered by different consuming classes )
    @param acrc_filterMask individual set of filter and mask
    @return true -> same FilterBox_c already exist
  */
  bool existFilter(
    const IsoAgLib::iCanCustomer_c & ar_customer,
    const IsoAgLib::iMaskFilterType_c& acrc_filterMask)
  { return __IsoAgLib::getCanInstance4Prop().existFilter(ar_customer, acrc_filterMask, NULL); }

  /**
    create a Filter Box with specified at_mask/at_filter
    on ui8_busNr of object; reconfig HW CAN MsgObj_c only if
    ab_reconfigImmediate == true -> useful for
    avoiding unneeded reconfiguration during
    sequence of FilterBox_c insertions;
    uses BIOS functions

    possible errors:
        * Err_c::badAlloc on not enough memory for new FilterBox
          instance or for new configured MsgObj_c's
    @see IsoAgLib::iCANCustomer
    @param ar_customer reference to IsoAgLib::iCanCustomer_c  which needs
           filtered messages (-> on received msg call
           ar_customer.processMsg())
    @param acrc_filterMask individual filter mask set
    @param ab_reconfigImmediate true -> all Filter objects are reconfigured
           to according CAN hardware MsgObj after creating this filter
    @return true -> inserting and if wanted reconfiguration are
          performed without errors
    @exception badAlloc
  */
  bool insertFilter(
    IsoAgLib::iCanCustomer_c& ar_customer,
    IsoAgLib::iMaskFilterType_c acrc_filterMask,
    int ai_dlcForce,
    bool ab_reconfigImmediate)
  {
    return ( __IsoAgLib::getCanInstance4Prop().insertFilter(
        ar_customer,
        acrc_filterMask,
        ai_dlcForce,
        ab_reconfigImmediate) != NULL);
  }

  /**
    reconfigure the MsgObj after insert/delete of FilterBox
  */
  bool reconfigureMsgObj() {return __IsoAgLib::getCanInstance4Prop().reconfigureMsgObj(); }

  /**
    delete a FilerBox definition
    @param ar_customer reference to the processing class ( the same filter setting can be registered by different consuming classes )
    @param acrc_filterMask individual filter mask combination
    @return true -> FilterBox_c found and deleted
  */
  bool deleteFilter(
    const IsoAgLib::iCanCustomer_c& ar_customer,
    IsoAgLib::iMaskFilterType_c acrc_filterMask)
  {
    return __IsoAgLib::getCanInstance4Prop().deleteFilter(
      ar_customer, acrc_filterMask);
  }


  bool deleteAllFiltersForCustomer (const IsoAgLib::iCanCustomer_c& ar_customer)
  { return __IsoAgLib::getCanInstance4Prop().deleteAllFiltersForCustomer (ar_customer); }


  /**
    function for sending data out of iCANPkg
    if send buffer is full a local loop waits till buffer has enough space
    (every 100ms the watchdog is triggered, to avoid watchdog reset)

    possible errors:
        * Err_c::hwConfig on wrong configured CAN obj,
          not init BUS or no configured send obj
        * Err_c::range on undef BUS or BIOS send obj nr
        * Err_c::can_warn on physical CAN-BUS problems
        * Err_c::can_off on physical CAN-BUS off state
    @see iCanPkg_c
    @param acrc_src iCanPkg_c which holds the to be sent data
    @return reference to this iProprietaryBus_c instance
  */
  iProprietaryBus_c& operator<< (iCanPkg_c& acrc_src)
    { (void) __IsoAgLib::getCanInstance4Prop().operator<< (acrc_src);
      return *this; }

 private:
  /** allow getIproprietaryBusInstance() access to shielded base class.
      otherwise __IsoAgLib::getProprietaryBusInstance() wouldn't be accepted by compiler
    */
  friend iProprietaryBus_c& getIProprietaryBusInstance( uint8_t aui8_instance );
};


/** C-style function, to get access to the unique singleton instance(s) */
iProprietaryBus_c &getIProprietaryBusInstance( uint8_t aui8_instance = 0 );

} // namespace IsoAgLib

#endif // 0 < PROP_INSTANCE_CNT
#endif // IPROPRIETARYBUS_C_H
