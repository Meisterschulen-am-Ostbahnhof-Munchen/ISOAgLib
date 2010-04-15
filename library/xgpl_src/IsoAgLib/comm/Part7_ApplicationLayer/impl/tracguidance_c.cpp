/*
  tracguidance_c.cpp: stores, updates and delivers all guidance data
    information from CanCustomer_c derived for CAN sending and
    receiving interaction; from BaseCommon_c derived for interaction
    with other IsoAgLib objects

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#include <IsoAgLib/driver/can/impl/canio_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isomonitor_c.h>
#include "tracguidancecommand_c.h"
#include "tracguidance_c.h"
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isorequestpgn_c.h>

using namespace std;

namespace __IsoAgLib { // Begin Namespace __IsoAglib

  #if defined(PRT_INSTANCE_CNT) && (PRT_INSTANCE_CNT > 1)
  /** C-style function, to get access to the unique TracGuidance_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an m_index must be given to select the wanted BUS
    */
  TracGuidance_c& getTracGuidanceInstance( uint8_t aui8_instance )
  { // if > 1 singleton instance is used, no static reference can be used
    return TracGuidance_c::instance( aui8_instance );
  };
  #else
  /** C-style function, to get access to the unique TracGuidance_c singleton instance */
  TracGuidance_c& getTracGuidanceInstance( void )
  {
    static TracGuidance_c& c_lbsTracGuidance = TracGuidance_c::instance();
    return c_lbsTracGuidance;
  };
  #endif

   /** initialize directly after the singleton instance is created.
        this is called from singleton.h and should NOT be called from the user again.
        users please use init(...) instead.
      */
      void TracGuidance_c::singletonInit()
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
  void TracGuidance_c::init_base (const IsoName_c* apc_isoName, int /*ai_singletonVecKey*/, IsoAgLib::IdentMode_t at_identMode)
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
  bool TracGuidance_c::config_base (const IsoName_c* apc_isoName, IsoAgLib::IdentMode_t at_identMode, uint16_t aui16_suppressMask)
  {
    IsoAgLib::IdentMode_t const ct_oldMode = getMode();
    //call config for handling which is base data independent
    //if something went wrong leave function before something is configured
    if ( !BaseCommon_c::config_base (apc_isoName, at_identMode, aui16_suppressMask) ) return false;

    ///Set time Period for Scheduler_c
    if (at_identMode == IsoAgLib::IdentModeTractor) setTimePeriod( (uint16_t) 100);
    else  setTimePeriod( (uint16_t) TIMEOUT_TRACTOR_DATA   );

    mui16_estCurvature = 32128;
    mt_requestResetCmdStatus = IsoAgLib::IsoNotAvailableReset ;
    mt_steeringInputPosStatus = IsoAgLib::IsoNotAvailableSteerPos ;
    mt_steeringSystemReadiness = IsoAgLib::IsoNotAvailableSteerReadiness ;
    mt_mechanicalSystemLogout = IsoAgLib::IsoNotAvailable ;

    // un-/register to PGN
    if (ct_oldMode == at_identMode)
      ; // no change, still the same mode
    else if (at_identMode == IsoAgLib::IdentModeTractor)
      // a change from Implement mode to Tractor mode occured
      RegisterPgn_s(this)(GUIDANCE_MACHINE_STATUS);
    else
      // a change from Tractor mode to Implement mode occured
      UnregisterPgn_s(this)(GUIDANCE_MACHINE_STATUS);

    return true;
  }

  /** check if filter boxes shall be created - create only ISO filters based
      on active local idents which has already claimed an address
      --> avoid to much Filter Boxes
    */
  void TracGuidance_c::checkCreateReceiveFilter( )
  {
    IsoMonitor_c& c_isoMonitor = getIsoMonitorInstance4Comm();
    CanIo_c &c_can = getCanInstance4Comm();

    if ( ( !checkFilterCreated() ) && ( c_isoMonitor.existActiveLocalIsoMember() ) )
    { // check if needed receive filters for ISO are active
      setFilterCreated();

      // create FilterBox_c for PGN GUIDANCE_MACHINE_STATUS, PF 254 - mask for DP, PF and PS
      // mask: (0x3FF00 << 8) filter: (GUIDANCE_MACHINE_STATUS << 8)
      c_can.insertFilter(*this, (static_cast<MASK_TYPE>(0x3FF00) << 8),
                        (static_cast<MASK_TYPE>(GUIDANCE_MACHINE_STATUS) << 8), true, Ident_c::ExtendedIdent);
    }
  }

  /** send a ISO11783 guidance information PGN.
    * this is only called when sending ident is configured and it has already claimed an address
      @pre  function is only called in tractor mode
      @see  BaseCommon_c::timeEvent()
    */
  bool TracGuidance_c::timeEventTracMode( )
  { ///Timeperiod of 100ms is set in ::config
    isoSendMessageTracMode();

    return true;
  }

  /** process a ISO11783 base information PGN
      @pre  sender of message is existent in monitor list
      @see  CanPkgExt_c::resolveSendingInformation()
    */
  bool TracGuidance_c::processMsg()
  {
    switch (data().isoPgn() & 0x3FF00)
    {
      case GUIDANCE_MACHINE_STATUS:
        // there is no need to check if sender exist in the monitor list because this is already done
        // in CanPkgExt_c -> resolveSendingInformation
        IsoName_c const& rcc_tempISOName = data().getISONameForSA();

        if ( checkParseReceived( rcc_tempISOName ) )
        {
          mui16_estCurvature = data().getUint16Data(0);
          mt_requestResetCmdStatus = IsoAgLib::IsoResetFlag_t           ( (data().getUint16Data(2) >> 6) & 0x3);
          mt_steeringInputPosStatus = IsoAgLib::IsoSteerPosFlag_t       ( (data().getUint8Data(2) >> 4) & 0x3);
          mt_steeringSystemReadiness = IsoAgLib::IsoSteerReadinessFlag_t( (data().getUint8Data(2) >> 2) & 0x3);
          mt_mechanicalSystemLogout = IsoAgLib::IsoActiveFlag_t         ( (data().getUint8Data(2) >> 0) & 0x3);

          setSelectedDataSourceISOName (rcc_tempISOName);
          setUpdateTime( data().time() );
        }
        else
        { // there is a sender conflict
          getILibErrInstance().registerError( iLibErr_c::BaseSenderConflict, iLibErr_c::Base );
          return false;
        }
        break;
    }
    return true;
  }

  /** send guidance data
      @see  CanIo_c::operator<<
    */
  void TracGuidance_c::isoSendMessageTracMode( )
  { // there is no need to check for address claim in tractor mode because this is already done in the timeEvent
    // function isoSendEstimatedMeasuredof base class BaseCommon_c

    CanIo_c& c_can = getCanInstance4Comm();

    setSelectedDataSourceISOName(*getISOName());

    // retrieve the actual dynamic sender no of the member with the registered isoName
    data().setIdentType(Ident_c::ExtendedIdent);
    data().setIsoPri(3);
    data().setLen(8);

    data().setISONameForSA( *getISOName() );
    if ( getIsoMonitorInstance4Comm().existIsoMemberNr( getTracGuidanceCommandInstance().getCommander() ) )
      data().setISONameForDA( getIsoMonitorInstance4Comm().isoMemberNr( getTracGuidanceCommandInstance().getCommander() ).isoName() );
    else
      data().setISONameForDA( IsoName_c::IsoNameUnspecified() );

    data().setIsoPgn(GUIDANCE_MACHINE_STATUS);
    uint8_t ui8_temp = 0;
    data().setUint16Data(0, mui16_estCurvature);
    ui8_temp |= (mt_requestResetCmdStatus   << 6);
    ui8_temp |= (mt_steeringInputPosStatus  << 4);
    ui8_temp |= (mt_steeringSystemReadiness << 2);
    ui8_temp |= (mt_mechanicalSystemLogout  << 0);
    data().setUint8Data(2, ui8_temp);
    //reserved fields
    data().setUint32Data(3, 0xFFFFFFFFUL);
    data().setUint8Data(7, 0xFF);

    //CanIo_c::operator<< retreives the information with the help of CanPkg_c::getData
    // then it sends the data
    c_can << data();
  }

///  Used for Debugging Tasks in Scheduler_c
const char*
TracGuidance_c::getTaskName() const
{   return "TracGuidance_c"; }

bool TracGuidance_c::processMsgRequestPGN (uint32_t aui32_pgn, IsoItem_c* apc_isoItemSender, IsoItem_c* apc_isoItemReceiver)
{
  bool const b_allowed = BaseCommon_c::check4ReqForPgn(aui32_pgn, apc_isoItemSender, apc_isoItemReceiver);
  if (!b_allowed)
    return false;
  if (GUIDANCE_MACHINE_STATUS != aui32_pgn)
    return false;
  isoSendMessageTracMode();
  return true;
}

}// End Namespace __IsoAglib
