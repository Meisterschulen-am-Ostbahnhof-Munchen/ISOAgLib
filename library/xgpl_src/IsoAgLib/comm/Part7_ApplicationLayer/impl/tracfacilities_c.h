/*
  tracfacilities_c: working on facilities data; data information from
    CanCustomer_c derived for CAN sending and receiving interaction;
    from BaseCommon_c derived for interaction with other IsoAgLib
    objects. This class handles Part 7 - B.25.3 Tractor facilities
    response message 0xFE09 in both directions (configurable)

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef TRACFACILITIES_C_H
#define TRACFACILITIES_C_H

#include <IsoAgLib/comm/Part7_ApplicationLayer/impl/basecommon_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isorequestpgnhandler_c.h>

namespace __IsoAgLib {


class TracFacilities_c;
typedef SINGLETON_DERIVED(TracFacilities_c,BaseCommon_c) SingletonTracFacilities_c;
  /** stores, updates  and delivers all Facilities data information;
      Derive from BaseCommon_c some fundamental funktionality for all base data
      Derive from Scheduler_Task_c to register in Scheduler_c for timeEvent trigger
      Derive from CANCustomer to register FilterBox'es in CanIo_c to receive CAN messages
      Derive from SINGLETON to create a Singleton which manages one global accessible singleton
      per IsoAgLib instance (if only one IsoAgLib instance is defined in application config, no overhead is produced).
    */
class TracFacilities_c : public SingletonTracFacilities_c
{
 public:// Public methods

 /** initialize directly after the singleton instance is created.
      this is called from singleton.h and should NOT be called from the user again.
      users please use init(...) instead.
    */
    void singletonInit();


  /** initialise element which can't be done during construct;
      above all create the needed FilterBox_c instances
      possible errors:
        * dependant error in CanIo_c problems during insertion of new FilterBox_c entries for IsoAgLibBase
      @param apc_isoName optional pointer to the ISOName variable of the responsible member instance (pointer enables automatic value update if var val is changed)
      @param ai_singletonVecKey singleton vector key in case PRT_INSTANCE_CNT > 1
      @param at_identMode either IsoAgLib::IdentModeImplement or IsoAgLib::IdentModeTractor
    */
  virtual void init_base (const IsoName_c*, int ai_singletonVecKey, IsoAgLib::IdentMode_t at_identMode = IsoAgLib::IdentModeImplement);
  /** config the TracFacilities_c object after init -> set pointer to isoName and
      config send/receive of a moving msg type
      @param apc_isoName pointer to the ISOName variable of the responsible member instance (pointer enables automatic value update if var val is changed)
      @param at_identMode either IsoAgLib::IdentModeImplement or IsoAgLib::IdentModeTractor
    */
  virtual bool config_base (const IsoName_c* apc_isoName, IsoAgLib::IdentMode_t at_identMode, uint16_t aui16_suppressMask = 0);

  /** destructor for TracFacilities_c which has nothing to do */
  virtual ~TracFacilities_c() { BaseCommon_c::close();}

  bool processMsgRequestPGN (uint32_t aui32_pgn, IsoItem_c* apc_isoItemSender, IsoItem_c* apc_isoItemReceiver);

  /* ******************************************* */
  /** \name Set Values for periodic send on BUS  */
  /*@{*/

  /** set tecu class with class t_val
      @param t_val  type of tecu class
    */
  void setTractorClass(const IsoAgLib::IsoTecuClassFlag_t at_val);

  /** set t_facility facility with value t_val
      @param t_facility  type of facility to be set
      @param t_val       type of facility flag
    */
  void setFacitity(const IsoAgLib::IsoFacility_t at_facility, const IsoAgLib::IsoFacilityFlag_t at_val);

  /* ****************************************************** */
  /** \name Retrieve Values which are sent from other ECUs  */
  /*@{*/
  /** get tecu class
      @return  type of tecu class
    */
  IsoAgLib::IsoTecuClassFlag_t getTractorClass() const {return IsoAgLib::IsoTecuClassFlag_t(arrui8_facilitiesBitData[0] >> 6);}

  /** get facility flag for t_facility
      @param at_facility  type of facility to be set
      @return  type of facility flag
    */
  IsoAgLib::IsoFacilityFlag_t getFacitity(const IsoAgLib::IsoFacility_t at_facility) const;

  ///  Used for Debugging Tasks in Scheduler_c
  virtual const char* getTaskName() const;

 private:
  // Private methods
  friend class SINGLETON_DERIVED(TracFacilities_c,BaseCommon_c);
  /** HIDDEN constructor for a TracFacilities_c object instance which can optional
      set the configuration for send/receive for a moving msg
      NEVER instantiate a variable of type TracFacilities_c within application
      only access TracFacilities_c via getTracFacilitiesInstance() or getTracFacilitiesInstance( int riLbsBusNr ) in case more than one ISO11783 BUS is used for IsoAgLib
    */
  TracFacilities_c();

  /** check if filter boxes shall be created - create only ISO filters based
      on active local idents which has already claimed an address
      --> avoid to much Filter Boxes
    */
  virtual void checkCreateReceiveFilter( );

  /** this is only called when sending ident is configured and it has already claimed an address
  @pre  function is only called in tractor mode
  @see  BaseCommon_c::timeEvent()
     */
  virtual bool timeEventTracMode();

  /** process a ISO11783 base information PGN
      @pre  sender of message is existent in monitor list
      @see  CanPkgExt_c::resolveSendingInformation()
    */
  virtual bool processMsg();

  /** send guidance data
  @see  TracFacilities_c::processMsgRequestPGN
  @see  CanIo_c::operator<<
   */
  void sendFacilities();

 private:
    // Private attributes
    /** struct of facilities data */
    uint8_t arrui8_facilitiesBitData[8];

    /** check if facilities already sent after address claim */
    bool mb_facilitiesOnInitSent;
};

  #if defined(PRT_INSTANCE_CNT) && (PRT_INSTANCE_CNT > 1)
  /** C-style function, to get access to the unique TracFacilities_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an m_index must be given to select the wanted BUS
    */
  TracFacilities_c& getTracFacilitiesInstance(uint8_t aui8_instance = 0);
  #else
  /** C-style function, to get access to the unique TracFacilities_c singleton instance */
  TracFacilities_c& getTracFacilitiesInstance(void);
  #endif

}

#endif
