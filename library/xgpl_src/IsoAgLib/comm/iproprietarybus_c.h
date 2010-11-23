/*
  iproprietarybus.h - Interface access class for the proprietary CAN-busses

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

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
#if 1 < CAN_INSTANCE_CNT
  MACRO_MULTITON_CONTRIBUTION();
#else
  MACRO_SINGLETON_CONTRIBUTION();
#endif
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
    @param ren_identType 
    @param aui8_minObjNr optional minimum number for hardware CAN
           message object (important for sharing CAN controller with
         other tasks) (default by define in isoaglib_config.h)
    @param aui8_maxObjNr optional maximum number for hardware CAN
           message object (default by define in isoaglib_config.h)
    @return true -> correct initialisation without errors
  */
  inline bool init(
    uint8_t aui8_busNumber,
    uint16_t aui16_bitrate = DEFAULT_BITRATE,
    iIdent_c::identType_t ren_identType = DEFAULT_CONFIG_IDENT_TYPE,
    uint8_t aui8_minObjNr = CONFIG_CAN_DEFAULT_MIN_OBJ_NR,
    uint8_t aui8_maxObjNr = CONFIG_CAN_DEFAULT_MAX_OBJ_NR)
  { return __IsoAgLib::getCanInstance4Prop().init (aui8_busNumber, aui16_bitrate, ren_identType, aui8_minObjNr, aui8_maxObjNr); }

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
      (default DEFAULT_IDENT_TYPE set in isoaglib_config.h)
    @return number of msgs which fit into send buffer
  */
  uint8_t sendCanFreecnt(iIdent_c::identType_t ren_identType = DEFAULT_IDENT_TYPE)
  { return __IsoAgLib::getCanInstance4Prop().sendCanFreecnt(ren_identType); }

  /**
    clear the send buffer
    @param ren_identType type of searched ident: standard 11bit or extended 29bit
      (default DEFAULT_IDENT_TYPE set in isoaglib_config.h)
  */
  void sendCanClearbuf() { __IsoAgLib::getCanInstance4Prop().sendCanClearbuf(); }

  /** set the new maximum send delay
      @param ai32_maxSendDelay new maximum send delay in milli-seconds
   */
  void setMaxSendDelay (int32_t ai32_maxSendDelay)
  { __IsoAgLib::getCanInstance4Prop().setMaxSendDelay (ai32_maxSendDelay); }

  /**
    test if a FilterBox_c definition already exist
    (version expecial for standard ident, chosen at compile time)
    @param ar_customer reference to the processing class ( the same filter setting can be registered by different consuming classes )
    @param aui32_mask individual mask for this filter box
    @param aui32_filter individual filter
    @param ren_identType type of searched ident: standard 11bit or extended 29bit
      (default DEFAULT_IDENT_TYPE set in isoaglib_config.h)
    @return true -> same FilterBox_c already exist
  */
  bool existFilter(
    const IsoAgLib::iCanCustomer_c & ar_customer,
    uint16_t aui32_mask, uint16_t aui32_filter,
    iIdent_c::identType_t ren_identType = DEFAULT_IDENT_TYPE)
  { return __IsoAgLib::getCanInstance4Prop().existFilter(ar_customer, aui32_mask, aui32_filter, ren_identType, NULL); }

  /**
    test if a FilterBox_c definition already exist
    (version expecial for extended ident, chosen at compile time)
    @param ar_customer reference to the processing class ( the same filter setting can be registered by different consuming classes )
    @param aui32_mask individual mask for this filter box
    @param aui32_filter individual filter
    @param ren_identType type of searched ident: standard 11bit or extended 29bit
      (default DEFAULT_IDENT_TYPE set in isoaglib_config.h)
    @return true -> same FilterBox_c already exist
  */
  bool existFilter(
    const IsoAgLib::iCanCustomer_c & ar_customer,
    uint32_t aui32_mask, uint32_t aui32_filter,
    iIdent_c::identType_t ren_identType = DEFAULT_IDENT_TYPE)
  { return __IsoAgLib::getCanInstance4Prop().existFilter( ar_customer, aui32_mask, aui32_filter, ren_identType, NULL); }

  /**
    test if a FilterBox_c definition already exist
    (version with comper items as Ident_c class instances, chosen by compiler)
    @param ar_customer reference to the processing class ( the same filter setting can be registered by different consuming classes )
    @param ac_compMask individual mask for this filter box
    @param ac_compFilter individual filter
    @return true -> same FilterBox_c already exist
  */
  bool existFilter(
    const IsoAgLib::iCanCustomer_c & ar_customer,
    const iIdent_c& ac_compMask, const iIdent_c& ac_compFilter)
  { return __IsoAgLib::getCanInstance4Prop().existFilter(ar_customer, ac_compMask, ac_compFilter, NULL); }

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
    @param at_mask individual mask for this filter box
    @param at_filter individual filter
    @param ab_reconfigImmediate true -> all Filter objects are reconfigured
           to according CAN hardware MsgObj after creating this filter
    @param at_identType ident type of the created ident: standard 11bit or extended 29bit
      (default DEFAULT_IDENT_TYPE set in isoaglib_config.h)
    @return true -> inserting and if wanted reconfiguration are
          performed without errors
    @exception badAlloc
  */
  bool insertFilter(
    IsoAgLib::iCanCustomer_c& ar_customer,
    MASK_TYPE at_mask, MASK_TYPE at_filter,
    bool ab_reconfigImmediate = true,
    const iIdent_c::identType_t at_identType = DEFAULT_IDENT_TYPE)
  { /// @todo Add DLC force here, too.
    return NULL != __IsoAgLib::getCanInstance4Prop().insertFilter(
        ar_customer,
        at_mask, at_filter,
        ab_reconfigImmediate,
        at_identType);
  }

  /**
    reconfigure the MsgObj after insert/delete of FilterBox
  */
  bool reconfigureMsgObj() {return __IsoAgLib::getCanInstance4Prop().reconfigureMsgObj(); }

  /**
    delete a FilerBox definition
    @param ar_customer reference to the processing class ( the same filter setting can be registered by different consuming classes )
    @param aui32_mask individual mask for this filter box
    @param aui32_filter individual filter
    @param at_identType ident type of the deleted ident: standard 11bit or extended 29bit
        (defualt DEFAULT_IDENT_TYPE defined in isoaglib_config.h)
    @return true -> FilterBox_c found and deleted
  */
  bool deleteFilter(
    const IsoAgLib::iCanCustomer_c& ar_customer,
    MASK_TYPE aui32_mask, MASK_TYPE aui32_filter,
    const iIdent_c::identType_t at_identType = DEFAULT_IDENT_TYPE)
  {
    return __IsoAgLib::getCanInstance4Prop().deleteFilter(
      ar_customer, aui32_mask, aui32_filter, at_identType);
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

  /// For now also work with iCanPkgExt, because it has the flags2string stuff included.
  /// NOTE: This needs to be separated into ISO / flags2strings somehow FINALLY!!!
  iProprietaryBus_c& operator<< (iCanPkgExt_c& acrc_src)
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
