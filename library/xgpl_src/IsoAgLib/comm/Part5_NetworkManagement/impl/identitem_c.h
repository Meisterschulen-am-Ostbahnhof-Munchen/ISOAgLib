/*
  identitem_c.h: object for managing identity/ies of the ECU

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef IDENT_ITEM_H
#define IDENT_ITEM_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include "baseitem_c.h"
#include "isoname_c.h"
#include "isoitem_c.h"
#include <IsoAgLib/comm/Part12_DiagnosticsServices/impl/diagnosticpgnhandler_c.h>
#include <IsoAgLib/driver/system/isystem_c.h>

// forward declaration of iIdentDataStorage_c - no internal definition
namespace IsoAgLib {
  class iIdentDataStorage_c;
};

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

  class DiagnosticsServices_c;

/**
  class for identity/ies (Control Function(s)) which are managed by the actual ECU;
  new instances start in prepare address claim state and stay there for a random and serialNo dependent time;
  then they are inserted as announcing member in the monitoring list and start announcing;

  IsoAgLib starts the address claim procedure after the init function is called and the iIdentItem
  is registered at iIsoMonitor with registerIdentItem().

  The IsoAgLib backend is responsible for answering all requests like
  RequestForClaimedSourceAdress ( ISO 11783 )

  The application just has to call the main trigger function Scheduler_c::timeEvent().

  After the monitor list report a completed address claim the status changes to active/claimed address;

  @short member ident item of this Control Function
  @author Dipl.-Inform. Achim Spangler
  */
class IdentItem_c : public BaseItem_c
{

public: // methods
  /** default constructor */
  IdentItem_c ();

  // see interface
  void init ( const IsoName_c& arc_isoNameParam,
              IsoAgLib::iIdentDataStorage_c& arc_dataStorage,
              int8_t ai8_slaveCount,
              const IsoName_c* apc_slaveIsoNameList,
              bool ab_enablediagnosticsServices);


  /** IsoMonitor uses this function to activate/start this Item
    @param ai_multitonInst Activate this Item on the given IsoBus
    @return false in case this Item wasn't properly set up before
                  using some init(..)-call.
  */
  bool activate (int ai_multitonInst);

  /** IsoMonitor uses this function to deactivate/stop this Item
      For later (again) use, it needs to be initialized again.
  */
  void deactivate();


  /** deliver pointer to IsoItem_c in IsoMonitor_c
      @return NULL -> either no ISO item or not yet registered in IsoMonitor_c
    */
  IsoItem_c* getIsoItem( void ) const { return mpc_isoItem; }

  /** deliver const reference to contained IsoName_c of this IdentItem_c */
  const IsoName_c& isoName() const { return mc_isoName; }

  /** deliver reference to contained IsoName_c of this IdentItem_c
    * ATTENTION: Use wisely!
    *            Only modify an IdentItem's ISOName if an address is NOT yet claimed!!!!!
    */
  IsoName_c& modifyableIsoName() { return mc_isoName; }

  /** restart SA claim procedure.
      When ISONAME conflict gets detected between creation of corresponding IsoItem_c and sending of SA-Claim,
      a complete restart of IdentItem_c life cycle beginning with PreAddressClaim.
    */
  void restartWithPreAddressClaim( void );

  /** Go Offline by:
    * + reset IdentItem::IState_c to IState_c::Off / OffUnable
    * + remove pointed IsoItem_c node and the respective pointer
    * @param ab_explicitlyOffByUser ("Off" if explicitly called by user (true), "OffUnable" if we are unable to keep a claimed address (false))
    */
  void goOffline (bool ab_explicitlyOffByUser);

  // d'tor
  ~IdentItem_c();

  /** periodically called functions do perform
      time dependent actions

      possible errors:
          * dependant memory error in SystemMgmt_c caused by inserting item in monitor list
      @see Scheduler_c::timeEvent
      @see System_c::getTime
      @return true -> all planned activities performed
    */
  bool timeEvent( void );

  /** check for equality with another item
      @param acrc_src compared IdentItem_c element
      @return true -> other item has same ISOName
    */
  bool operator==(IdentItem_c& acrc_src) const {return (mc_isoName == acrc_src.mc_isoName); }

  /** check for equality with given ISOName
      @param acrc_isoName compared ISOName
      @return true -> item has same ISOName
    */
  bool operator==(const IsoName_c& acrc_isoName) const {return (mc_isoName == acrc_isoName); }

  /** check for difference to another item
      @param acrc_src compared IdentItem_c element
      @return true -> other item has different ISOName
    */
  bool operator!=(IdentItem_c& acrc_src) const {return (mc_isoName != acrc_src.mc_isoName); }

  /** check for difference to given ISOName
      @param acrc_isoName compared ISOName
      @return true -> other item has different ISOName
    */
  bool operator!=(const IsoName_c& acrc_isoName) const {return (mc_isoName != acrc_isoName); }

  /** check if this item has lower ISOName than another one
      @param acrc_src compared IdentItem_c element
      @return true -> this item has lower ISOName than compared one
    */
  bool operator<(IdentItem_c& acrc_src) const {return (mc_isoName < acrc_src.mc_isoName); }

  /** check if this item has lower ISOName than given ISOName
      @param acrc_isoName compared ISOName
      @return true -> this item has lower ISOName than compared one
    */
  bool operator<(const IsoName_c& acrc_isoName) const {return (mc_isoName < acrc_isoName); }

  /** check if given number is equal to member number of this item
      @param aui8_nr compared number
      @return true -> this item has same number
    */
  bool equalNr(uint8_t aui8_nr);

  /** check if the ident has claimed address */
  bool isClaimedAddress( void ) const { return (getIsoItem() != NULL) ? (getIsoItem()->itemState (IState_c::ClaimedAddress)) : (itemState (IState_c::ClaimedAddress)); }

#ifdef USE_WORKING_SET
  bool isMaster() const { return (mpvec_slaveIsoNames != NULL); }
#endif

  /** Set ECU Identification fields, needed during the diagnostic procedure */
  bool setEcuIdentification(
      const char *acstr_partNr,
      const char *acstr_serialNr,
      const char *acstr_location,
      const char *acstr_type,
      const char *acstr_manufacturerName)
  { // mpc_diagnosticPgnHandler is allocated in the constructor, so no need to be checked for NULL
    return mpc_diagnosticPgnHandler->setEcuIdentification(
        acstr_partNr,
        acstr_serialNr,
        acstr_location,
        acstr_type,
        acstr_manufacturerName);
  }

  /** Set SW Identification fields, needed during the diagnostic procedure */
  bool setSwIdentification( const char *acstr_partNbr )
  { // mpc_diagnosticPgnHandler is allocated in the constructor, so no need to be checked for NULL
    return mpc_diagnosticPgnHandler->setSwIdentification( acstr_partNbr );
  }

  DiagnosticsServices_c* getDiagnosticsServices() { return mpc_diagnosticsServices; }

  //! Setter for the different certification message fields
  //! Parameter:
  //! @param ui16_year Certification year as in ISO 11783-7 A.29.1, must be between 2000 and 2061
  //! @param a_revision Certification revision as in ISO 11783-7 A.29.2
  //! @param a_laboratoryType Certification laboratory type as in ISO 11783-7 A.29.3
  //! @param aui16_laboratoryId Certification laboratory ID (11 bits wide) as in ISO 11783-7 A.29.4
  //! @param acrc_certificationBitMask Compliance certification type bitfield (  as in ISO 11783-7 A.29.5 till A.29.17 )
  //! @param aui16_referenceNumber Compliance certification reference number  as in ISO 11783-7 A.29.18
  bool setCertificationData(
    uint16_t ui16_year, IsoAgLib::CertificationRevision_t a_revision, IsoAgLib::CertificationLabType_t a_laboratoryType,
    uint16_t aui16_laboratoryId, const IsoAgLib::CertificationBitMask_t& acrc_certificationBitMask, uint16_t aui16_referenceNumber )
  {
    // mpc_diagnosticPgnHandler is allocated in the constructor, so no need to be checked for NULL
    return mpc_diagnosticPgnHandler->setCertificationData(
      ui16_year , a_revision, a_laboratoryType, aui16_laboratoryId,
      acrc_certificationBitMask, aui16_referenceNumber );
  }

  void updateLastIsoSaRequestForThisItem() { i32_lastIsoSaRequestForThisItem = HAL::getTime(); }
  int32_t getLastIsoSaRequestForThisItem() const { return i32_lastIsoSaRequestForThisItem; }

protected: // methods

  /** calculate an individual number between [0,1000] to get an individual wait time before first
      address claim -> chance to avoid conflict with other system with same default ISOName
    */
  void setIndividualWait();

  /** periodically called functions do perform
      time dependent actions in prepare address claim state
      -> unify ISOName (Device Class / Device Class Instance)
      -> insert item in appropriate monitor lists and initiate address claim

      possible errors:
          * dependant memory error in SystemMgmt_c caused by inserting item in monitor list
      @see Scheduler_c::timeEvent
      @see SystemMgmt_c::timeEvent
      @see System_c::getTime
      @return true -> all planned activities performed
    */
  bool timeEventPreAddressClaim( void );

  /** periodically called functions do perform
      time dependent actions in active (address claim/claimed address) state
      -> call timeEvent for corresponding items in MemberMonitor/ISOMonitor
      -> initiate repeated address claim with changed Nr / ISOName if conflict with other item occured

      possible errors:
          * dependant memory error in SystemMgmt_c caused by inserting item in monitor list
      @see Scheduler_c::timeEvent
      @see SystemMgmt_c::timeEvent
      @see System_c::getTime
      @return true -> all planned activities performed
    */
  bool timeEventActive( void );


private: // methods

  /** HIDDEN! copy constructor for IdentItem_c
      NEVER copy a IdentItem_c around!!!!
      ONLY copy pointers to the wanted instance!!!
      ==> the copy constructor is defined as private, so that compiler
          detects this fault, and shows you this WARNING!!
      @param acrc_src source
    */
  IdentItem_c(const IdentItem_c& acrc_src);

  /** HIDDEN! assignment for IdentItem_c
      NEVER assign a IdentItem_c to another instance!!!!
      ==> the asignment is defined as private, so that compiler
          detects this fault, and shows you this WARNING!!
      <!--@param acrc_src source-->
    */
  IdentItem_c& operator=(const IdentItem_c& /* acrc_src */);


private: // attributes

  /** pointer to associated IsoItem */
  IsoItem_c* mpc_isoItem;


  /** pointer to a valid claim data Storage handler. If 0 not used. */
  IsoAgLib::iIdentDataStorage_c* mpc_dataStorageHandler; 

  /** configured/retrived SA of this identity */
  uint8_t mui8_sa;

  /** IsoName code of this identity */
  IsoName_c mc_isoName;

  /** Diagnostic PGN handler */
  DiagnosticPgnHandler_c* mpc_diagnosticPgnHandler;
  /** Diagnostics Services handler */
  DiagnosticsServices_c* mpc_diagnosticsServices;

  #ifdef USE_WORKING_SET
  /** pointer to a list of all slave nodes represented by their ISO-Name
   * if this pointer is != NULL, this item IS a master and the list's size is the number of associated slaves.
   * if this pointer is == NULL, this item is NOT a master.
   * It may be a slave - this can be determined by searching
   * through all Items' list for this Item's IsoName!
   */
  STL_NAMESPACE::vector<IsoName_c>* mpvec_slaveIsoNames;
  #endif

  int32_t i32_lastIsoSaRequestForThisItem;

  // protect this ident from beeing initialized more than once
  bool mb_readyForActivation;
};

}
#endif
