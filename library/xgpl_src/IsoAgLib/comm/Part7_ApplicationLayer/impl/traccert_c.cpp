/*
  tracguidance_c.cpp: stores, updates and delivers all certification
    data information from CanCustomer_c derived for CAN sending and
    receiving interaction; from BaseCommon_c derived for interaction
    with other IsoAgLib objects

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include "traccert_c.h"
#include <IsoAgLib/driver/can/impl/canio_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isomonitor_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isorequestpgn_c.h>


using namespace std;

namespace __IsoAgLib { // Begin Namespace __IsoAglib

  #if defined(PRT_INSTANCE_CNT) && (PRT_INSTANCE_CNT > 1)
  /** C-style function, to get access to the unique TracCert_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an m_index must be given to select the wanted BUS
    */
  TracCert_c& getTracCertInstance( uint8_t aui8_instance )
  { // if > 1 singleton instance is used, no static reference can be used
    return TracCert_c::instance( aui8_instance );
  };
  #else
  /** C-style function, to get access to the unique TracCert_c singleton instance */
  TracCert_c& getTracCertInstance( void )
  {
    static TracCert_c& c_lbsTracCert = TracCert_c::instance();
    return c_lbsTracCert;
  };
  #endif

  /** initialize directly after the singleton instance is created.
        this is called from singleton.h and should NOT be called from the user again.
        users please use init(...) instead.
      */
      void TracCert_c::singletonInit()
      { // singletonInit is called, AFTER the initializing instance() function has assigned a suitable
        // singleton vec key - this key value is NOT available at construction time!!!
        BaseCommon_c::singletonInitBase(SINGLETON_VEC_KEY);
      }

  /** initialise element which can't be done during construct;
      above all create the needed FilterBox_c instances
      possible errors:
        * dependant error in CanIo_c problems during insertion of new FilterBox_c entries for IsoAgLibBase
      @param apc_isoName optional pointer to the ISOName variable of the ersponsible member instance (pointer enables automatic value update if var val is changed)
      @param ai_singletonVecKey singleton vector key in case PRT_INSTANCE_CNT > 1
      @param at_identMode either IsoAgLib::IdentModeImplement or IsoAgLib::IdentModeTractor
    */
  void TracCert_c::init_base (const IsoName_c* apc_isoName, int /*ai_singletonVecKey*/, IsoAgLib::IdentMode_t at_identMode)
  {
    //call init for handling which is base data independent
    BaseCommon_c::init_base (apc_isoName, getSingletonVecKey(), at_identMode);
  };

  /** config the TracGuidance_c object after init -> set pointer to isoName and
      config send/receive of a moving msg type
      @param apc_isoName pointer to the ISOName variable of the responsible member instance (pointer enables automatic value update if var val is changed)
      @param at_identMode either IsoAgLib::IdentModeImplement or IsoAgLib::IdentModeTractor
      @return true -> configuration was successfull
    */
  bool TracCert_c::config_base (const IsoName_c* apc_isoName, IsoAgLib::IdentMode_t at_identMode, uint16_t aui16_suppressMask)
  {
    if (at_identMode == IsoAgLib::IdentModeTractor)
      return false; // This part is now handled in Part12_Diagnostic
      
#if 0
    //store old mode to decide to register or unregister from request for pgn
    IsoAgLib::IdentMode_t t_oldMode = getMode();
#endif

    //call config for handling which is base data independent
    //if something went wrong leave function before something is configured
    if ( !BaseCommon_c::config_base (apc_isoName, at_identMode, aui16_suppressMask) ) return false;

#if 0
    // This part is moved to the diagnostic PGN handling under Part_12
    if (t_oldMode == IsoAgLib::IdentModeImplement && at_identMode == IsoAgLib::IdentModeTractor)
    {  // a change from Implement mode to Tractor mode occured
      // create FilterBox_c for REQUEST_PGN_MSG_PGN, ISOBUS_CERTIFICATION_PGN
      getIsoRequestPgnInstance4Comm().registerPGN (*this, ISOBUS_CERTIFICATION_PGN); // request for certificate
    }

    if (t_oldMode == IsoAgLib::IdentModeTractor && at_identMode == IsoAgLib::IdentModeImplement)
    {  // a change from Tractor mode to Implement mode occured
      // unregister from request for pgn, because in implement mode no request should be answered
      getIsoRequestPgnInstance4Comm().unregisterPGN (*this, ISOBUS_CERTIFICATION_PGN);
    }
#endif

    m_certData.ui16_certReferenceNumber = 0;
    m_certData.ui16_certLabId = 0;
    m_certData.ui8_isobusCertYear = 0x3F;
    m_certData.ui8_isobusCertRevision = IsoAgLib::IsoNotAvailableRev;
    m_certData.ui8_isobusCertLabType = IsoAgLib::IsoNotAvailableCert;
    m_certData.ui8_minimumECU = m_certData.ui8_TECUClass1 = m_certData.ui8_TECUClass2 = m_certData.ui8_TECUClass3
    = m_certData.ui8_Class3ECU = m_certData.ui8_VirtualTerminal = m_certData.ui8_VTWorkingSetMaster
    = m_certData.ui8_VTWorkingSetMember = m_certData.ui8_TaskController = m_certData.ui8_TCWorkingSetMaster
    = m_certData.ui8_TCWorkingSetMember = m_certData.ui8_FileServer = m_certData.ui8_GPSReceiver = IsoAgLib::IsoNotCert;

    return true;
  }

  /** check if filter boxes shall be created - create only ISO filters based
      on active local idents which has already claimed an address
      --> avoid to much Filter Boxes
    */
  void TracCert_c::checkCreateReceiveFilter( )
  {
    IsoMonitor_c& c_isoMonitor = getIsoMonitorInstance4Comm();
    CanIo_c &c_can = getCanInstance4Comm();

    if ( ( !checkFilterCreated() ) && ( c_isoMonitor.existActiveLocalIsoMember() ) )
    { // check if needed receive filters for ISO are active
      setFilterCreated();
      // create FilterBox_c for PGN ISOBUS_CERTIFICATION_PGN, PF 254 - mask for DP, PF and PS
      // mask: (0x3FF00 << 8) filter: (ISOBUS_CERTIFICATION_PGN << 8)
      c_can.insertFilter(*this, (static_cast<MASK_TYPE>(0x3FF00) << 8),
                        (static_cast<MASK_TYPE>(ISOBUS_CERTIFICATION_PGN) << 8), true, Ident_c::ExtendedIdent);
    }
  }

#if 0
  // This part is moved to the diagnostic PGN handling under Part_12
  bool TracCert_c::processMsgRequestPGN (uint32_t aui32_pgn, IsoItem_c* apc_isoItemSender, IsoItem_c* apc_isoItemReceiver)
  {
    // check if we are allowed to send a request for pgn
    if ( ! BaseCommon_c::check4ReqForPgn(aui32_pgn, apc_isoItemSender, apc_isoItemReceiver) ) return false;

    // call TracCert_c function to send certificaton status of a controler application
    // isoSendMessage checks if this item (identified by ISOName)
    // is configured to send certification status
    isoSendMessage();
    return true;
  };
#endif

  /** process a ISO11783 base information PGN
      @pre  sender of message is existent in monitor list
      @see  CanPkgExt_c::resolveSendingInformation()
    */
  bool TracCert_c::processMsg()
  {
    // there is no need to check if sender exist in the monitor list because this is already done
    // in CanPkgExt_c -> resolveSendingInformation
    IsoName_c const& rcc_tempISOName = data().getISONameForSA();

    uint32_t const cui32_pgn = data().isoPgn();
    switch (cui32_pgn)
    {
      case ISOBUS_CERTIFICATION_PGN:
        if ( checkParseReceived (rcc_tempISOName) )
        { // sender is allowed to send
          m_certData.ui8_isobusCertRevision =  (data().getUint8Data(0)  >>  6) & 0x3u;
          m_certData.ui8_isobusCertYear =      (data().getUint8Data(0)  >>  0) & 0x3Fu;
          m_certData.ui16_certLabId = (uint16_t(data().getUint8Data(2)) << 3) |
                                              ((data().getUint8Data(1)  >> 5) & 0x7u);
          m_certData.ui8_isobusCertLabType =   (data().getUint8Data(1)  >> 1) & 0x7u;
          m_certData.ui8_minimumECU =          (data().getUint8Data(3)  >> 7) & 0x1u;
          m_certData.ui8_TECUClass1 =          (data().getUint8Data(3)  >> 6) & 0x1u;
          m_certData.ui8_TECUClass2 =          (data().getUint8Data(3)  >> 5) & 0x1u;
          m_certData.ui8_TECUClass3 =          (data().getUint8Data(3)  >> 4) & 0x1u;
          m_certData.ui8_Class3ECU =           (data().getUint8Data(3)  >> 3) & 0x1u;
          m_certData.ui8_VirtualTerminal =     (data().getUint8Data(3)  >> 2) & 0x1u;
          m_certData.ui8_VTWorkingSetMaster =  (data().getUint8Data(3)  >> 1) & 0x1u;
          m_certData.ui8_VTWorkingSetMember =  (data().getUint8Data(3)  >> 0) & 0x1u;
          m_certData.ui8_TaskController =      (data().getUint8Data(4)  >> 7) & 0x1u;
          m_certData.ui8_TCWorkingSetMaster =  (data().getUint8Data(4)  >> 6) & 0x1u;
          m_certData.ui8_TCWorkingSetMember =  (data().getUint8Data(4)  >> 5) & 0x1u;
          m_certData.ui8_FileServer =          (data().getUint8Data(4)  >> 4) & 0x1u;
          m_certData.ui8_GPSReceiver =         (data().getUint8Data(4)  >> 3) & 0x1u;
          m_certData.ui16_certReferenceNumber = data().getUint16Data(6);

          setSelectedDataSourceISOName (rcc_tempISOName);
          setUpdateTime( data().time() );
        } else
        { // there is a sender conflict
          getILibErrInstance().registerError( iLibErr_c::BaseSenderConflict, iLibErr_c::Base );
        }
      break;
    }
    return true;
  }

#if 0
  /** send guidance data
      @see  TracCert_c::processMsgRequestPGN
      @see  CanIo_c::operator<<
    */
  void TracCert_c::isoSendMessage( )
  {// there is no need to check for address claim because this is already done in the processMsgRequestPGN;
   // this function is only be called on request!!

    CanIo_c& c_can = getCanInstance4Comm();

    data().setISONameForSA( *getISOName() );
    setSelectedDataSourceISOName(*getISOName());
    data().setIdentType(Ident_c::ExtendedIdent);
    data().setIsoPri(3);
    data().setLen(8);

    data().setIsoPgn(ISOBUS_CERTIFICATION_PGN);
    uint8_t ui8_0 = 0;
    ui8_0 |= (m_certData.ui8_isobusCertRevision & 0x3u) << 6;
    ui8_0 |= m_certData.ui8_isobusCertYear              << 0;
    data().setUint8Data(0, ui8_0);
    uint8_t ui8_1 = 0;
    ui8_1 |= (m_certData.ui16_certLabId & 0x7u)         << 5;
    ui8_1 |= m_certData.ui8_isobusCertLabType           << 1;
    ui8_1 |= (m_certData.ui8_isobusCertRevision & 0x4u) >> 2;
    data().setUint8Data(1, ui8_1);
    uint8_t ui8_2 = m_certData.ui16_certLabId  >> 3;
    data().setUint8Data(2, ui8_2);
    uint8_t ui8_3 = 0;
    ui8_3 |= m_certData.ui8_minimumECU                  << 7;
    ui8_3 |= m_certData.ui8_TECUClass1                  << 6;
    ui8_3 |= m_certData.ui8_TECUClass2                  << 5;
    ui8_3 |= m_certData.ui8_TECUClass3                  << 4;
    ui8_3 |= m_certData.ui8_Class3ECU                   << 3;
    ui8_3 |= m_certData.ui8_VirtualTerminal             << 2;
    ui8_3 |= m_certData.ui8_VTWorkingSetMaster          << 1;
    ui8_3 |= m_certData.ui8_VTWorkingSetMember          << 0;
    data().setUint8Data(3, ui8_3);
    uint8_t ui8_4 = 0;
    ui8_4 |= m_certData.ui8_TaskController              << 7;
    ui8_4 |= m_certData.ui8_TCWorkingSetMaster          << 6;
    ui8_4 |= m_certData.ui8_TCWorkingSetMember          << 5;
    ui8_4 |= m_certData.ui8_FileServer                  << 4;
    ui8_4 |= m_certData.ui8_GPSReceiver                 << 3;
    data().setUint8Data(4, ui8_4);
    uint8_t ui8_5 = 0;
    data().setUint8Data(5, ui8_5);
    uint16_t ui16_6 = m_certData.ui16_certReferenceNumber;
    data().setUint16Data(6, ui16_6);

    // CanIo_c::operator<< retreives the information with the help of CanPkg_c::getData
    // then it sends the data
    c_can << data();
  }
#endif

///  Used for Debugging Tasks in Scheduler_c
const char*
TracCert_c::getTaskName() const
{   return "TracCert_c"; }

}// End Namespace __IsoAglib
