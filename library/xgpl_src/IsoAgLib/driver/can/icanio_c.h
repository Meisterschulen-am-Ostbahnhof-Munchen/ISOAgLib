/*
  ican_io.h - Interface class for the internal library object CAN_IO

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef ICAN_IO_H
#define ICAN_IO_H

//include the declaration of the internal object
#include "impl/canio_c.h"
#include <IsoAgLib/comm/Part3_DataLink/icanpkgext_c.h>
#include <IsoAgLib/driver/can/icancustomer_c.h>
#include <IsoAgLib/driver/can/icanpkg_c.h>
#include <IsoAgLib/scheduler/ischedulertask_c.h>

// Begin Namespace IsoAgLib
namespace IsoAgLib {

/** Main Object for CAN communication;
  act as interface for CanIo_c of the __IsoAgLib namespace. Manages
  dynamic amount of FilterBox_c which can have individual filter/mask settings
  (means no global mask needed).
  Inhertis virtual from Shared::Err_c to allow easy access to status checking functions.
  @author Dipl.-Inform. Achim Spangler
  */
class iCanIo_c : private __IsoAgLib::CanIo_c {
 private:
  // private typedef alias names
  typedef STL_NAMESPACE::bad_alloc bad_alloc;
  typedef __IsoAgLib::CanCustomer_c  CanCustomer_c ;
  typedef __IsoAgLib::CanPkg_c CanPkg_c;
  typedef __IsoAgLib::Ident_c Ident_c;

 public:
  /**
    Initialize the CAN hardware, and instantiate one msg object for
     sending of messages. Do configuration for BUS number, sending bitrate,
    CAN ident length, min/max hardware/BIOS Msg Obj numbers by parameters;
    called by specified constructor or external functions;
    wrong BUS and msg obj numbers are rejected and cause set of Err_c:range

    If even the aui8_busNumber parameter has the default value 0xFF,
    then the configuration settings of a previous init call are not
    changed. In this case, the CAN BUS is only reset with the old settings.
    This is enabled by the default value 0xFF for aui8_busNumber, which is
    changed to CONFIG_CAN_DEFAULT_BUS_NUMBER for the first call of init() after the constructor.
    In all other cases, the special value 0xFF is indicator for empty parameter list.

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
  inline bool init(uint8_t aui8_busNumber,
        uint16_t aui16_bitrate = DEFAULT_BITRATE,
        Ident_c::identType_t ren_identType = DEFAULT_CONFIG_IDENT_TYPE,
        uint8_t aui8_minObjNr = CONFIG_CAN_DEFAULT_MIN_OBJ_NR,
        uint8_t aui8_maxObjNr = CONFIG_CAN_DEFAULT_MAX_OBJ_NR
        ) {return CanIo_c::init(aui8_busNumber, aui16_bitrate,
          ren_identType, aui8_minObjNr, aui8_maxObjNr);};

  /** check if this CanIo_c instance is configured so that it can be used to send */
  bool isReady2Send() const { return CanIo_c::isReady2Send();};
  /**
    initiate processing of all received msg
    check all active MsgObj_c for received CAN msg and
    initiate their processing
    @return <0 --> not enough time to process all messages.
           ==0 --> no messages were received.
           >0  --> all messages are processed, number of messages  */
  int16_t processMsg() { return CanIo_c::processMsg();};

 #ifdef USE_CAN_MEASURE_BUSLOAD
  /**
    deliver actual BUS load in baud
    @return baudrate in [baud] on used CAN BUS
  */
  int16_t getBusLoad() const {return CanIo_c::getBusLoad();};
 #endif

  /** wait until specified timeout or until next CAN message receive
   *  @return true -> there are CAN messages waiting for process. else: return due to timeout
   */
  static bool waitUntilCanReceiveOrTimeout( uint16_t aui16_timeoutInterval )
  { return CanIo_c::waitUntilCanReceiveOrTimeout( aui16_timeoutInterval );};


  /**
    deliver the numbers which can be placed at the moment in the send buffer
    @param ren_identType type of searched ident: standard 11bit or extended 29bit
      (default DEFAULT_IDENT_TYPE set in isoaglib_config.h)
    @return number of msgs which fit into send buffer
  */
  uint8_t sendCanFreecnt(Ident_c::identType_t ren_identType = DEFAULT_IDENT_TYPE)
  {return CanIo_c::sendCanFreecnt(ren_identType);};
  /**
    clear the send buffer
    @param ren_identType type of searched ident: standard 11bit or extended 29bit
      (default DEFAULT_IDENT_TYPE set in isoaglib_config.h)
  */
  void sendCanClearbuf(Ident_c::identType_t ren_identType = DEFAULT_IDENT_TYPE)
  {CanIo_c::sendCanClearbuf(ren_identType);};


  /** set the new maximum send delay
      @param ai32_maxSendDelay new maximum send delay in milli-seconds
   */
  void setMaxSendDelay (int32_t ai32_maxSendDelay)
  { CanIo_c::setMaxSendDelay (ai32_maxSendDelay); }

  /** set this client to have send-priority
      @param ab_sendPrioritized enable (true) or disable (false) sending in Prioritized Mode
   */
  void setSendPriority(bool ab_sendPrioritized)
  { CanIo_c::setSendPriority(ab_sendPrioritized); }


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
  bool existFilter(const IsoAgLib::iCanCustomer_c & ar_customer,
    uint16_t aui32_mask, uint16_t aui32_filter,
    Ident_c::identType_t ren_identType = DEFAULT_IDENT_TYPE,
      ArrFilterBox::iterator* apc_iter = NULL)
  { return CanIo_c::existFilter(ar_customer, aui32_mask, aui32_filter,
    ren_identType, apc_iter);};
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
  bool existFilter(const IsoAgLib::iCanCustomer_c & ar_customer,
      uint32_t aui32_mask, uint32_t aui32_filter,
      Ident_c::identType_t ren_identType = DEFAULT_IDENT_TYPE,
      ArrFilterBox::iterator* apc_iter = NULL)
  {return CanIo_c::existFilter( ar_customer, aui32_mask, aui32_filter, ren_identType, apc_iter);};
  /**
    test if a FilterBox_c definition already exist
    (version with comper items as Ident_c class instances, chosen by compiler)
    @param ar_customer reference to the processing class ( the same filter setting can be registered by different consuming classes )
    @param ac_compMask individual mask for this filter box
    @param ac_compFilter individual filter
    @return true -> same FilterBox_c already exist
  */
  bool existFilter(const IsoAgLib::iCanCustomer_c & ar_customer,
      const Ident_c& ac_compMask, const Ident_c& ac_compFilter,
      ArrFilterBox::iterator* apc_iter = NULL)
  {return CanIo_c::existFilter(ar_customer, ac_compMask, ac_compFilter, apc_iter);};

  /**
    create a Filter Box with specified at_mask/at_filter
    on ui8_busNr of object; reconfig HW CAN MsgObj_c only if
    ab_reconfigImmediate == true -> useful for
    avoiding unneeded reconfiguration during
    sequence of FilterBox_c insertions;
    by ar_customer iCanIo_c (FilterBox_c) can start direct processing
    of received data in dedicated customer object (no search);
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
   inline bool insertFilter(IsoAgLib::iCanCustomer_c& ar_customer, MASK_TYPE at_mask,
       MASK_TYPE at_filter, bool ab_reconfigImmediate = true,
                     const Ident_c::identType_t at_identType = DEFAULT_IDENT_TYPE)
  {
    CanCustomer_c &rc_customer = ar_customer;
    return NULL != CanIo_c::insertFilter(
        rc_customer,
        at_mask,
        at_filter,
        ab_reconfigImmediate,
        at_identType);
  };

/**
    Create a Standard Iso Filter Box.It calls internally the CanIo_c::insertFilter.
    @see IsoAgLib::iCANCustomer
    @param ar_customer reference to IsoAgLib::iCanCustomer_c  which needs
           filtered messages (-> on received msg call
           ar_customer.processMsg())
    @param aui32_pgn PGN
    @param ab_reconfigImmediate true -> all Filter objects are reconfigured
           to according CAN hardware MsgObj after creating this filter
    @return true -> inserting and if wanted reconfiguration are
          performed without errors
    @exception badAlloc
*/

  bool insertStandardIsoFilter(IsoAgLib::iCanCustomer_c& ar_customer, uint32_t aui32_pgn,bool ab_reconfigImmediate)
  {
    CanCustomer_c &rc_customer = ar_customer;
    bool const cb_result = NULL != CanIo_c::insertStandardIsoFilter(
        rc_customer,
        aui32_pgn,
        ab_reconfigImmediate);
    return cb_result;
  }

  /**
    reconfigure the MsgObj after insert/delete of FilterBox
  */
  bool reconfigureMsgObj() {return CanIo_c::reconfigureMsgObj();};

  /**
    delete a FilerBox definition
    @param ar_customer reference to the processing class ( the same filter setting can be registered by different consuming classes )
    @param aui32_mask individual mask for this filter box
    @param aui32_filter individual filter
    @param at_identType ident type of the deleted ident: standard 11bit or extended 29bit
        (defualt DEFAULT_IDENT_TYPE defined in isoaglib_config.h)
    @return true -> FilterBox_c found and deleted
  */


  bool deleteFilter(const IsoAgLib::iCanCustomer_c& ar_customer,
      MASK_TYPE aui32_mask, MASK_TYPE aui32_filter,
      const Ident_c::identType_t at_identType = DEFAULT_IDENT_TYPE)
  {
    CanCustomer_c const &crc_customer = ar_customer;
    return CanIo_c::deleteFilter(
        crc_customer,
        aui32_mask,
        aui32_filter,
        at_identType);
  }


  bool deleteAllFiltersForCustomer (const IsoAgLib::iCanCustomer_c& ar_customer)
  { return CanIo_c::deleteAllFiltersForCustomer (ar_customer); }


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
    @return reference to this iCanIo_c instance ==>
          needed by commands like "c_can_io << pkg_1 << pkg_2 ... << pkg_n;"
  */
  iCanIo_c& operator<<(iCanPkg_c& acrc_src)
    {return static_cast<iCanIo_c&>(CanIo_c::operator<<(static_cast<iCanPkg_c&>(acrc_src)));};
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
    @return reference to this iCanIo_c instance ==>
          needed by commands like "c_can_io << pkg_1 << pkg_2 ... << pkg_n;"
  */
  iCanIo_c& operator<<(iCanPkgExt_c& acrc_src)
    {return static_cast<iCanIo_c&>(CanIo_c::operator<<(static_cast<__IsoAgLib::CanPkgExt_c&>(acrc_src)));};

 private:
  /** allow getIcanInstance() access to shielded base class.
      otherwise __IsoAgLib::getCanInstance() wouldn't be accepted by compiler
    */
  #if defined( CAN_INSTANCE_CNT ) && ( CAN_INSTANCE_CNT > 1 )
  friend iCanIo_c& getIcanInstance( uint8_t aui8_instance );
  #else
  friend iCanIo_c& getIcanInstance( void );
  #endif
};
#if defined( CAN_INSTANCE_CNT ) && ( CAN_INSTANCE_CNT > 1 )
  /** C-style function, to get access to the unique iCanIo_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  inline iCanIo_c& getIcanInstance( uint8_t aui8_instance = 0 )
  { return static_cast<iCanIo_c&>(__IsoAgLib::getCanInstance(aui8_instance));};
#else
  /** C-style function, to get access to the unique iCanIo_c singleton instance */
  inline iCanIo_c& getIcanInstance( void )
  { return static_cast<iCanIo_c&>(__IsoAgLib::getCanInstance());};
#endif

/** this typedef is only for some time to provide backward compatibility at API level */
typedef iCanIo_c iCANIO_c;
}
#endif
