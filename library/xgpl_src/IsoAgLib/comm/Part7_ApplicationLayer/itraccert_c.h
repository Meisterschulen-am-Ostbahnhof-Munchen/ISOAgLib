/*
  itraccert_c.h: working on certification data; stores, updates and
    delivers all certification data informations from CanCustomer_c
    derived for CAN sending and receiving interaction; from
    Scheduler_Task_c derived for interaction with other IsoAgLib
    objects

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef ITRACCERT_C_H
#define ITRACCERT_C_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include <IsoAgLib/comm/Part5_NetworkManagement/iisoname_c.h>
#include "ibasetypes.h"
#include "impl/traccert_c.h"


// Begin Namespace IsoAgLib
namespace IsoAgLib {

  /** stores, updates  and delivers certification data information;
      Derive from BaseCommon_c some fundamental funktionality for all base data
      Derive from Scheduler_Task_c to register in Scheduler_c for timeEvent trigger
      Derive from CANCustomer to register FilterBox'es in CanIo_c to receive CAN messages
      Derive from SINGLETON to create a Singleton which manages one global accessible singleton
      per IsoAgLib instance (if only one IsoAgLib instance is defined in application config, no overhead is produced).
    */
  class iTracCert_c : private __IsoAgLib::TracCert_c {
  public:
    // Public methods
  /**
      config the iTracCert_c object after init -> set pointer to isoName and
      config send/receive of different base msg types
      @param apc_isoName pointer to the ISOName variable of the responsible member instance (pointer enables automatic value update if var val is changed)
      @param at_identMode can currently only be set to IsoAgLib::IdentModeImplement (  IsoAgLib::IdentModeTractor is now handled by the Part_12 diagnosticpgnhandler_c )
      @return true -> configuration was successfull
  */
  bool config (const iIsoName_c* apc_isoName, IsoAgLib::IdentMode_t at_identMode = IsoAgLib::IdentModeImplement, uint16_t aui16_suppressMask = 0)
  { return TracCert_c::config_base (apc_isoName, at_identMode, aui16_suppressMask ); }

  /* ******************************************* */
    /** \name Set Values for periodic send on BUS  */
  /*@{*/

  #if 0
  // This part is moved to the diagnostic PGN handling under Part_12
  
  /** set certification reference number assigned by certification lab
      @param ui16_val  certification reference number
    */
  void setCertReferenceNumber(const uint16_t ui16_val) {TracCert_c::setCertReferenceNumber(ui16_val);}
  /** set manufacture code of the laboratory that performed the certification test
      @param ui16_val  certification laboratory id
    */
  void setCertLabID(const uint16_t ui16_val)
  {TracCert_c::setCertLabID(ui16_val);}
  /** set year of the test protocol to which the certification test was performed
      @param ui8_val  year of test protocol
    */
  void setIsobusCertYear(const uint16_t ui8_val) {TracCert_c::setIsobusCertYear(ui8_val);}
  /** set revision of the certification test performed
      @param t_val  revision of certification test
    */
  void setIsobusCertRevision(const IsoAgLib::IsoCertRevisionFlag_t t_val) {TracCert_c::setIsobusCertRevision(t_val);}
  /** set approving body for the certification laboratory
      @param t_val  certification laboratory type
    */
  void setIsobusCertLabType(const IsoAgLib::IsoCertLabTypeFlag_t t_val) {TracCert_c::setIsobusCertLabType(t_val);}
  /** set type of certification test performed
      @param t_val  type of certification test
    */
  void setMinimumECU(const IsoAgLib::IsoCertTypeFlag_t t_val) {TracCert_c::setMinimumECU(t_val);}
  /** set type of certification test performed
      @param t_val  type of certification test
    */
  void setTECUClass1(const IsoAgLib::IsoCertTypeFlag_t t_val) {TracCert_c::setTECUClass1(t_val);}
  /** set type of certification test performed
      @param t_val  type of certification test
    */
  void setTECUClass2(const IsoAgLib::IsoCertTypeFlag_t t_val) {TracCert_c::setTECUClass2(t_val);}
  /** set type of certification test performed
      @param t_val  type of certification test
    */
  void setECUClass3(const IsoAgLib::IsoCertTypeFlag_t t_val) {TracCert_c::setECUClass3(t_val);}
  /** set type of certification test performed
      @param t_val  type of certification test
    */
  void setClass3ECU(const IsoAgLib::IsoCertTypeFlag_t t_val) {TracCert_c::setClass3ECU(t_val);}
  /** set type of certification test performed
      @param t_val  type of certification test
    */
  void setVirtualTerminal(const IsoAgLib::IsoCertTypeFlag_t t_val) {TracCert_c::setVirtualTerminal(t_val);}
  /** set type of certification test performed
      @param t_val  type of certification test
    */
  void setVTWorkingSetMaster(const IsoAgLib::IsoCertTypeFlag_t t_val) {TracCert_c::setVTWorkingSetMaster(t_val);}
  /** set type of certification test performed
      @param t_val  type of certification test
    */
  void setVTWorkingSetMember(const IsoAgLib::IsoCertTypeFlag_t t_val) {TracCert_c::setVTWorkingSetMember(t_val);}
  /** set type of certification test performed
      @param t_val  type of certification test
    */
  void setTaskController(const IsoAgLib::IsoCertTypeFlag_t t_val) {TracCert_c::setTaskController(t_val);}
  /** set type of certification test performed
      @param t_val  type of certification test
    */
  void setTCWorkingSetMaster(const IsoAgLib::IsoCertTypeFlag_t t_val) {TracCert_c::setTCWorkingSetMaster(t_val);}
  /** set type of certification test performed
      @param t_val  type of certification test
    */
  void setMTCWorkingSetMember(const IsoAgLib::IsoCertTypeFlag_t t_val) {TracCert_c::setMTCWorkingSetMember(t_val);}
  /** set type of certification test performed
      @param t_val  type of certification test
    */
  void setFileServer(const IsoAgLib::IsoCertTypeFlag_t t_val) {TracCert_c::setFileServer(t_val);}
  /** set type of certification test performed
      @param t_val  type of certification test
    */
  void setPSReceiver(const IsoAgLib::IsoCertTypeFlag_t t_val) {TracCert_c::setPSReceiver(t_val);}

#endif

  /*@}*/

  /* ****************************************************** */
  /** \name Retrieve Values which are sent from other ECUs  */
  /*@{*/
  /** get certification reference number assigned by certification lab
      @return  certification reference number
    */
  uint16_t certReferenceNumber()const {return TracCert_c::certReferenceNumber();}
  /** get manufacture code of the laboratory that performed the certification test
      @return  certification laboratory id
    */
  uint16_t certLabID() const {return TracCert_c::certLabID();}
  /** get year of the test protocol to which the certification test was performed
      @return  year of test protocol
    */
  uint16_t isobusCertYear() const {return TracCert_c::isobusCertYear();}
  /** get revision of the certification test performed
      @return  revision of certification test
    */
  IsoAgLib::IsoCertRevisionFlag_t isobusCertRevision() const {return  TracCert_c::isobusCertRevision();}
  /** get approving body for the certification laboratory
      @return  certification laboratory type
    */
  IsoAgLib::IsoCertLabTypeFlag_t isobusCertLabType() const {return TracCert_c::isobusCertLabType();}
  /** get type of certification test performed
      @return  type of certification test
    */
  IsoAgLib::IsoCertTypeFlag_t minimumECU() const {return TracCert_c::minimumECU();}
  /** get type of certification test performed
      @return  type of certification test
    */
  IsoAgLib::IsoCertTypeFlag_t TECUClass1() const {return TracCert_c::TECUClass1();}
  /** get type of certification test performed
      @return  type of certification test
    */
  IsoAgLib::IsoCertTypeFlag_t TECUClass2( ) const {return TracCert_c::TECUClass2();}
  /** get type of certification test performed
      @return  type of certification test
    */
  IsoAgLib::IsoCertTypeFlag_t TECUClass3()const {return TracCert_c::TECUClass3();}
  /** get type of certification test performed
      @return  type of certification test
    */
  IsoAgLib::IsoCertTypeFlag_t Class3ECU() const {return TracCert_c::Class3ECU();}
  /** get type of certification test performed
      @return  type of certification test
    */
  IsoAgLib::IsoCertTypeFlag_t VirtualTerminal() const {return TracCert_c::VirtualTerminal();}
  /** get type of certification test performed
      @return  type of certification test
    */
  IsoAgLib::IsoCertTypeFlag_t VTWorkingSetMaster() const {return TracCert_c::VTWorkingSetMaster();}
  /** get type of certification test performed
      @return  type of certification test
    */
  IsoAgLib::IsoCertTypeFlag_t VTWorkingSetMember() const {return TracCert_c::VTWorkingSetMember();}
  /** get type of certification test performed
      @return  type of certification test
    */
  IsoAgLib::IsoCertTypeFlag_t TaskController() const {return TracCert_c::TaskController();}
  /** get type of certification test performed
      @return  type of certification test
    */
  IsoAgLib::IsoCertTypeFlag_t TCWorkingSetMaster() const {return TracCert_c::TCWorkingSetMaster();}
  /** get type of certification test performed
      @return  type of certification test
    */
  IsoAgLib::IsoCertTypeFlag_t TCWorkingSetMember() const {return TracCert_c::TCWorkingSetMember();}
  /** get type of certification test performed
      @return  type of certification test
    */
  IsoAgLib::IsoCertTypeFlag_t FileServer() const {return TracCert_c::FileServer();}
  /** get type of certification test performed
      @return  type of certification test
    */
  IsoAgLib::IsoCertTypeFlag_t GPSReceiver() const {return TracCert_c::GPSReceiver();}
  /*@}*/

  private:
    /** allow getITracCertInstance() access to shielded traccert class.
      otherwise __IsoAgLib::getTracCerInstance() wouldn't be accepted by compiler
    */
    #if defined(PRT_INSTANCE_CNT) && (PRT_INSTANCE_CNT > 1)
    friend iTracCert_c& getITracCertInstance(uint8_t aui8_instance);
    #else
    friend iTracCert_c& getITracCertInstance(void);
    #endif

  };

  #if defined(PRT_INSTANCE_CNT) && (PRT_INSTANCE_CNT > 1)
  /** C-style function, to get access to the unique iTracCert_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  inline iTracCert_c& getITracCertInstance(uint8_t aui8_instance = 0)
  { return static_cast<iTracCert_c&>(__IsoAgLib::getTracCertInstance(aui8_instance));};
  #else
  /** C-style function, to get access to the unique iTracCert_c singleton instance */
  inline iTracCert_c& getITracCertInstance(void)
  { return static_cast<iTracCert_c&>(__IsoAgLib::getTracCertInstance());};
  #endif
}
#endif

