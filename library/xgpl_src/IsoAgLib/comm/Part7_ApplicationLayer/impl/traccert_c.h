/***************************************************************************
                 tracguidance_c.cpp - stores, updates and delivers all certification
                                  data information from CanCustomer_c
                                  derived for CAN sending and receiving
                                  interaction;
                                  from BaseCommon_c derived for
                                  interaction with other IsoAgLib objects
                             -------------------
    begin                 Fri Apr 07 2000
    copyright            : (C) 2000 - 2009 by Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 * This file is part of the "IsoAgLib", an object oriented program library *
 * to serve as a software layer between application specific program and   *
 * communication protocol details. By providing simple function calls for  *
 * jobs like starting a measuring program for a process data value on a    *
 * remote ECU, the main program has not to deal with single CAN telegram   *
 * formatting. This way communication problems between ECU's which use     *
 * this library should be prevented.                                       *
 * Everybody and every company is invited to use this library to make a    *
 * working plug and play standard out of the printed protocol standard.    *
 *                                                                         *
 * Copyright (C) 2000 - 2009 Dipl.-Inform. Achim Spangler                  *
 *                                                                         *
 * The IsoAgLib is free software; you can redistribute it and/or modify it *
 * under the terms of the GNU General Public License as published          *
 * by the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This library is distributed in the hope that it will be useful, but     *
 * WITHOUT ANY WARRANTY; without even the implied warranty of              *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       *
 * General Public License for more details.                                *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with IsoAgLib; if not, write to the Free Software Foundation,     *
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA           *
 *                                                                         *
 * As a special exception, if other files instantiate templates or use     *
 * macros or inline functions from this file, or you compile this file and *
 * link it with other works to produce a work based on this file, this file*
 * does not by itself cause the resulting work to be covered by the GNU    *
 * General Public License. However the source code for this file must still*
 * be made available in accordance with section (3) of the                 *
 * GNU General Public License.                                             *
 *                                                                         *
 * This exception does not invalidate any other reasons why a work based on*
 * this file might be covered by the GNU General Public License.           *
 *                                                                         *
 * Alternative licenses for IsoAgLib may be arranged by contacting         *
 * the main author Achim Spangler by a.spangler@osb-ag:de                  *
 ***************************************************************************/

 /**************************************************************************
 *                                                                         *
 *     ###    !!!    ---    ===    IMPORTANT    ===    ---    !!!    ###   *
 * Each software module, which accesses directly elements of this file,    *
 * is considered to be an extension of IsoAgLib and is thus covered by the *
 * GPL license. Applications must use only the interface definition out-   *
 * side :impl: subdirectories. Never access direct elements of __IsoAgLib  *
 * and __HAL namespaces from applications which shouldnt be affected by    *
 * the license. Only access their interface counterparts in the IsoAgLib   *
 * and HAL namespaces. Contact a.spangler@osb-ag:de in case your applicat- *
 * ion really needs access to a part of an internal namespace, so that the *
 * interface might be extended if your request is accepted.                *
 *                                                                         *
 * Definition of direct access:                                            *
 * - Instantiation of a variable with a datatype from internal namespace   *
 * - Call of a (member-) function                                          *
 * Allowed is:                                                             *
 * - Instatiation of a variable with a datatype from interface namespace,  *
 *   even if this is derived from a base class inside an internal namespace*
 * - Call of member functions which are defined in the interface class     *
 *   definition ( header )                                                 *
 *                                                                         *
 * Pairing of internal and interface classes:                              *
 * - Internal implementation in an :impl: subdirectory                     *
 * - Interface in the parent directory of the corresponding internal class *
 * - Interface class name IsoAgLib::iFoo_c maps to the internal class      *
 *   __IsoAgLib::Foo_c                                                     *
 *                                                                         *
 * AS A RULE: Use only classes with names beginning with small letter :i:  *
 ***************************************************************************/
#ifndef TRAC_CERT_C_H
#define TRAC_CERT_C_H

#include "basecommon_c.h"
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isorequestpgnhandler_c.h>


namespace __IsoAgLib {

typedef struct
{
  /** certification reference number assigned by certification lab */
  uint16_t ui16_certReferenceNumber : 16;
  /** manufacture code of the laboratory that performed the certification test */
  uint16_t ui16_certLabId : 11;
  /** year of the test protocol to which the certification test was performed */
  uint8_t ui8_isobusCertYear : 6;
  /** revision of the certification test performed */
  uint8_t ui8_isobusCertRevision : 3;
  /** approving body for the certification laboratory */
  uint8_t ui8_isobusCertLabType : 3;
  /** indicate the type of certification test performed */
  uint8_t ui8_minimumECU : 1;
  /** indicate the type of certification test performed */
  uint8_t ui8_TECUClass1 : 1;
  /** indicate the type of certification test performed */
  uint8_t ui8_TECUClass2 : 1;
  /** indicate the type of certification test performed */
  uint8_t ui8_TECUClass3 : 1;
  /** indicate the type of certification test performed */
  uint8_t ui8_Class3ECU : 1;
  /** indicate the type of certification test performed */
  uint8_t ui8_VirtualTerminal : 1;
  /** indicate the type of certification test performed */
  uint8_t ui8_VTWorkingSetMaster : 1;
  /** indicate the type of certification test performed */
  uint8_t ui8_VTWorkingSetMember : 1;
  /** indicate the type of certification test performed */
  uint8_t ui8_TaskController : 1;
  /** indicate the type of certification test performed */
  uint8_t ui8_TCWorkingSetMaster : 1;
  /** indicate the type of certification test performed */
  uint8_t ui8_TCWorkingSetMember : 1;
  /** indicate the type of certification test performed */
  uint8_t ui8_FileServer : 1;
  /** indicate the type of certification test performed */
  uint8_t ui8_GPSReceiver : 1;
} certificationBitData_t;

class TracCert_c;
typedef SINGLETON_DERIVED(TracCert_c,BaseCommon_c) SingletonTracCert_c;
  /** stores, updates  and delivers all moving data information;
      Derive from BaseCommon_c some fundamental funktionality for all base data
      Derive from Scheduler_Task_c to register in Scheduler_c for timeEvent trigger
      Derive from CANCustomer to register FilterBox'es in CanIo_c to receive CAN messages
      Derive from SINGLETON to create a Singleton which manages one global accessible singleton
      per IsoAgLib instance (if only one IsoAgLib instance is defined in application config, no overhead is produced).
    */
class TracCert_c : public SingletonTracCert_c
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
  /** config the TracCert_c object after init -> set pointer to isoName and
      config send/receive of a moving msg type
      @param apc_isoName pointer to the ISOName variable of the responsible member instance (pointer enables automatic value update if var val is changed)
      @param at_identMode either IsoAgLib::IdentModeImplement or IsoAgLib::IdentModeTractor
    */
  virtual bool config_base (const IsoName_c* apc_isoName, IsoAgLib::IdentMode_t at_identMode, uint16_t aui16_suppressMask = 0);

  /** destructor for TracGuidance_c which has nothing to do */
  virtual ~TracCert_c() { BaseCommon_c::close();}
	
#if 0
		// This part is moved to the diagnostic PGN handling under Part_12
  bool processMsgRequestPGN (uint32_t aui32_pgn, IsoItem_c* apc_isoItemSender, IsoItem_c* apc_isoItemReceiver);

  /** send guidance data
    @see  TracCert_c::processMsgRequestPGN
    @see  CanIo_c::operator<<
  */
  void isoSendMessage( );

  /* ******************************************* */
  /** \name Set Values for periodic send on BUS  */
  /*@{*/
  /** set certification reference number assigned by certification lab
      @param ui16_val  certification reference number
    */
  void setCertReferenceNumber(const uint16_t ui16_val) {m_certData.ui16_certReferenceNumber = ui16_val;}

  /** set manufacture code of the laboratory that performed the certification test
      @param ui16_val  certification laboratory id
    */
  void setCertLabID(const uint16_t ui16_val) { m_certData.ui16_certLabId = ui16_val; }

  /** set year of the test protocol to which the certification test was performed
      @param ui16_val  year of test protocol
    */
  void setIsobusCertYear(const uint16_t ui16_val) {m_certData.ui8_isobusCertYear = ui16_val - 2000;}

  /** set revision of the certification test performed
      @param t_val  revision of certification test
    */
  void setIsobusCertRevision(const IsoAgLib::IsoCertRevisionFlag_t t_val) {m_certData.ui8_isobusCertRevision = t_val;}

  /** set approving body for the certification laboratory
      @param t_val  certification laboratory type
    */
  void setIsobusCertLabType(const IsoAgLib::IsoCertLabTypeFlag_t t_val) {m_certData.ui8_isobusCertLabType = t_val;}

  /** set type of certification test performed
      @param t_val  type of certification test
    */
  void setMinimumECU(const IsoAgLib::IsoCertTypeFlag_t t_val) {m_certData.ui8_minimumECU = t_val;}

  /** set type of certification test performed
      @param t_val  type of certification test
    */
  void setTECUClass1(const IsoAgLib::IsoCertTypeFlag_t t_val) {m_certData.ui8_TECUClass1 = t_val;}

  /** set type of certification test performed
      @param t_val  type of certification test
    */
  void setTECUClass2(const IsoAgLib::IsoCertTypeFlag_t t_val) {m_certData.ui8_TECUClass2 = t_val;}

  /** set type of certification test performed
      @param t_val  type of certification test
    */
  void setECUClass3(const IsoAgLib::IsoCertTypeFlag_t t_val) {m_certData.ui8_TECUClass3 = t_val;}

  /** set type of certification test performed
      @param t_val  type of certification test
    */
  void setClass3ECU(const IsoAgLib::IsoCertTypeFlag_t t_val) {m_certData.ui8_Class3ECU = t_val;}

  /** set type of certification test performed
      @param t_val  type of certification test
    */
  void setVirtualTerminal(const IsoAgLib::IsoCertTypeFlag_t t_val) {m_certData.ui8_VirtualTerminal = t_val;}

  /** set type of certification test performed
      @param t_val  type of certification test
    */
  void setVTWorkingSetMaster(const IsoAgLib::IsoCertTypeFlag_t t_val) {m_certData.ui8_VTWorkingSetMaster = t_val;}

  /** set type of certification test performed
      @param t_val  type of certification test
    */
  void setVTWorkingSetMember(const IsoAgLib::IsoCertTypeFlag_t t_val) {m_certData.ui8_VTWorkingSetMember = t_val;}

  /** set type of certification test performed
      @param t_val  type of certification test
    */
  void setTaskController(const IsoAgLib::IsoCertTypeFlag_t t_val) {m_certData.ui8_TaskController = t_val;}

  /** set type of certification test performed
      @param t_val  type of certification test
    */
  void setTCWorkingSetMaster(const IsoAgLib::IsoCertTypeFlag_t t_val) {m_certData.ui8_TCWorkingSetMaster = t_val;}

  /** set type of certification test performed
      @param t_val  type of certification test
    */
  void setMTCWorkingSetMember(const IsoAgLib::IsoCertTypeFlag_t t_val) {m_certData.ui8_TCWorkingSetMember = t_val;}

  /** set type of certification test performed
      @param t_val  type of certification test
    */
  void setFileServer(const IsoAgLib::IsoCertTypeFlag_t t_val) {m_certData.ui8_FileServer = t_val;}

  /** set type of certification test performed
      @param t_val  type of certification test
    */
  void setPSReceiver(const IsoAgLib::IsoCertTypeFlag_t t_val) {m_certData.ui8_GPSReceiver = t_val;}
  /*@}*/

#endif

  /* ****************************************************** */
  /** \name Retrieve Values which are sent from other ECUs  */
  /*@{*/
  /** get certification reference number assigned by certification lab
      @return  certification reference number
    */
  uint16_t certReferenceNumber()const {return m_certData.ui16_certReferenceNumber;}

  /** get manufacture code of the laboratory that performed the certification test
      @return  certification laboratory id
    */
  uint16_t certLabID() const { return m_certData.ui16_certLabId; }

  /** get year of the test protocol to which the certification test was performed
      @return  year of test protocol
    */
  uint16_t isobusCertYear() const {return m_certData.ui8_isobusCertYear + 2000;}

  /** get revision of the certification test performed
      @return  revision of certification test
    */
  IsoAgLib::IsoCertRevisionFlag_t isobusCertRevision() const
  {return  IsoAgLib::IsoCertRevisionFlag_t(m_certData.ui8_isobusCertRevision);}

  /** get approving body for the certification laboratory
      @return  certification laboratory type
    */
  IsoAgLib::IsoCertLabTypeFlag_t isobusCertLabType() const
  {return IsoAgLib::IsoCertLabTypeFlag_t(m_certData.ui8_isobusCertLabType);}

  /** get type of certification test performed
      @return  type of certification test
    */
  IsoAgLib::IsoCertTypeFlag_t minimumECU() const {return IsoAgLib::IsoCertTypeFlag_t(m_certData.ui8_minimumECU);}

  /** get type of certification test performed
      @return  type of certification test
    */
  IsoAgLib::IsoCertTypeFlag_t TECUClass1() const {return IsoAgLib::IsoCertTypeFlag_t(m_certData.ui8_TECUClass1);}

  /** get type of certification test performed
      @return  type of certification test
    */
  IsoAgLib::IsoCertTypeFlag_t TECUClass2( ) const {return IsoAgLib::IsoCertTypeFlag_t(m_certData.ui8_TECUClass2);}

  /** get type of certification test performed
      @return  type of certification test
    */
  IsoAgLib::IsoCertTypeFlag_t TECUClass3()const {return IsoAgLib::IsoCertTypeFlag_t(m_certData.ui8_TECUClass3);}

  /** get type of certification test performed
      @return  type of certification test
    */
  IsoAgLib::IsoCertTypeFlag_t Class3ECU() const {return IsoAgLib::IsoCertTypeFlag_t(m_certData.ui8_Class3ECU);}

  /** get type of certification test performed
      @return  type of certification test
    */
  IsoAgLib::IsoCertTypeFlag_t VirtualTerminal() const {return IsoAgLib::IsoCertTypeFlag_t(m_certData.ui8_VirtualTerminal);}

  /** get type of certification test performed
      @return  type of certification test
    */
  IsoAgLib::IsoCertTypeFlag_t VTWorkingSetMaster() const {return IsoAgLib::IsoCertTypeFlag_t(m_certData.ui8_VTWorkingSetMaster);}

  /** get type of certification test performed
      @return  type of certification test
    */
  IsoAgLib::IsoCertTypeFlag_t VTWorkingSetMember() const {return IsoAgLib::IsoCertTypeFlag_t(m_certData.ui8_VTWorkingSetMember);}

  /** get type of certification test performed
      @return  type of certification test
    */
  IsoAgLib::IsoCertTypeFlag_t TaskController() const {return IsoAgLib::IsoCertTypeFlag_t(m_certData.ui8_TaskController);}

  /** get type of certification test performed
      @return  type of certification test
    */
  IsoAgLib::IsoCertTypeFlag_t TCWorkingSetMaster() const {return IsoAgLib::IsoCertTypeFlag_t(m_certData.ui8_TCWorkingSetMaster);}

  /** get type of certification test performed
      @return  type of certification test
    */
  IsoAgLib::IsoCertTypeFlag_t TCWorkingSetMember() const {return IsoAgLib::IsoCertTypeFlag_t(m_certData.ui8_TCWorkingSetMember);}

  /** get type of certification test performed
      @return  type of certification test
    */
  IsoAgLib::IsoCertTypeFlag_t FileServer() const {return IsoAgLib::IsoCertTypeFlag_t(m_certData.ui8_FileServer);}

  /** get type of certification test performed
      @return  type of certification test
    */
  IsoAgLib::IsoCertTypeFlag_t GPSReceiver() const {return IsoAgLib::IsoCertTypeFlag_t(m_certData.ui8_GPSReceiver);}
  /*@}*/

  ///  Used for Debugging Tasks in Scheduler_c
  virtual const char* getTaskName() const;

 private:
  // Private methods
  friend class SINGLETON_DERIVED(TracCert_c,BaseCommon_c);
  /** HIDDEN constructor for a TracCert_c object instance which can optional
      set the configuration for send/receive for a moving msg
      NEVER instantiate a variable of type TracCert_c within application
      only access TracCert_c via getTracCertInstance() or getTracCertInstance( int riLbsBusNr ) in case more than one ISO11783 BUS is used for IsoAgLib
    */
  TracCert_c() {}

  /** check if filter boxes shall be created - create only ISO filters based
      on active local idents which has already claimed an address
      --> avoid to much Filter Boxes
    */
  virtual void checkCreateReceiveFilter( );

  /** process a ISO11783 base information PGN
      @pre  sender of message is existent in monitor list
      @see  CanPkgExt_c::resolveSendingInformation()
    */
  virtual bool processMsg();

 private:
    // Private attributes
    /** struct of certification data */
    certificationBitData_t m_certData;
};

  #if defined(PRT_INSTANCE_CNT) && (PRT_INSTANCE_CNT > 1)
  /** C-style function, to get access to the unique TracCert_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an m_index must be given to select the wanted BUS
    */
  TracCert_c& getTracCertInstance(uint8_t aui8_instance = 0);
  #else
  /** C-style function, to get access to the unique TracCert_c singleton instance */
  TracCert_c& getTracCertInstance(void);
  #endif

}

#endif
