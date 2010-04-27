/*
  tracpto_c.cpp: working on PTO data; stores, updates and delivers all
    base data informations from CanCustomer_c derived for CAN sending
    and receiving interaction; from BaseCommon_c derived for
    interaction with other IsoAgLib objects

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isomonitor_c.h>
#include <IsoAgLib/driver/can/impl/canio_c.h>
#include "tracpto_c.h"
#include "tracgeneral_c.h"
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isorequestpgn_c.h>

using namespace std;

namespace __IsoAgLib { // Begin Namespace __IsoAgLib

  #if defined(PRT_INSTANCE_CNT) && (PRT_INSTANCE_CNT > 1)
  /** C-style function, to get access to the unique TracPTO_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  TracPTO_c& getTracPtoInstance(uint8_t aui8_instance)
  { // if > 1 singleton instance is used, no static reference can be used
    return TracPTO_c::instance(aui8_instance);
  };
  #else
  /** C-style function, to get access to the unique TracPTO_c singleton instance */
  TracPTO_c& getTracPtoInstance( void )
  {
    static TracPTO_c& c_lbsTracPto = TracPTO_c::instance();
    return c_lbsTracPto;
  };
  #endif

  /** initialize directly after the singleton instance is created.
    this is called from singleton.h and should NOT be called from the user again.
    users please use init(...) instead.
  */
  void TracPTO_c::singletonInit()
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
  void TracPTO_c::init_base (const IsoName_c* apc_isoName, int /*ai_singletonVecKey*/, IsoAgLib::IdentMode_t at_identMode)
  {
    //call init for handling which is base data independent
    BaseCommon_c::init_base (apc_isoName, getSingletonVecKey(), at_identMode);
  };

  /** config the TracPTO_c object after init -> set pointer to isoName and
      config send/receive of different base msg types
      @param apc_isoName pointer to the ISOName variable of the ersponsible member instance (pointer enables automatic value update if var val is changed)
      @param at_identMode either IsoAgLib::IdentModeImplement or IsoAgLib::IdentModeTractor
      @return true -> configuration was successfull
   */
  bool TracPTO_c::config_base (const IsoName_c* apc_isoName, IsoAgLib::IdentMode_t at_identMode, uint16_t aui16_suppressMask)
  {
    //store old mode to decide to register or unregister from request for pgn
    //and set Periode for Scheduler_c
    IsoAgLib::IdentMode_t t_oldMode = getMode();

    //call config for handling which is base data independent
    //if something went wrong leave function before something is configured
    if ( !BaseCommon_c::config_base (apc_isoName, at_identMode, aui16_suppressMask) ) return false;

    ///Set time Period for Scheduler_c
    if (at_identMode == IsoAgLib::IdentModeTractor) setTimePeriod( (uint16_t) 100);
    else  setTimePeriod( (uint16_t) TIMEOUT_TRACTOR_DATA   );

    // set the member base msg value vars to NO_VAL codes
    mt_ptoFront.ui16_pto8DigitPerRpm = mt_ptoRear.ui16_pto8DigitPerRpm = NO_VAL_16;
    // set the timestamps to 0
    mt_ptoFront.i32_lastPto = mt_ptoRear.i32_lastPto = 0;

    // un-/register to PGN
    if (t_oldMode == at_identMode)
      ; // no change, still the same mode
    else if (at_identMode == IsoAgLib::IdentModeTractor) {
      // a change from Implement mode to Tractor mode occured
      RegisterPgn_s s_register = getRegisterPgn();
      s_register(FRONT_PTO_STATE_PGN);
      s_register(REAR_PTO_STATE_PGN);
    } else {
      // a change from Tractor mode to Implement mode occured
      UnregisterPgn_s s_unregister = getUnregisterPgn();
      s_unregister(FRONT_PTO_STATE_PGN);
      s_unregister(REAR_PTO_STATE_PGN);
    }

    mt_ptoFront.t_ptoEngaged = mt_ptoRear.t_ptoEngaged
    = mt_ptoFront.t_pto1000 = mt_ptoRear.t_pto1000
    = mt_ptoFront.t_ptoEconomy = mt_ptoRear.t_ptoEconomy = IsoAgLib::IsoNotAvailable; // mark as not available
    mt_ptoFront.ui16_ptoSetPoint8DigitPerRpm = 0;
    mt_ptoRear.ui16_ptoSetPoint8DigitPerRpm = 0;
    mt_ptoFront.t_ptoEngagementReqStatus = mt_ptoRear.t_ptoEngagementReqStatus
    = mt_ptoFront.t_ptoModeReqStatus = mt_ptoRear.t_ptoModeReqStatus
    = mt_ptoFront.t_ptoEconomyModeReqStatus = mt_ptoRear.t_ptoEconomyModeReqStatus = IsoAgLib::IsoNotAvailableReq;
    mt_ptoFront.t_ptoShaftSpeedLimitStatus = mt_ptoRear.t_ptoShaftSpeedLimitStatus = IsoAgLib::IsoNotAvailableLimit;

    return true;
  };

  /** check if filter boxes shall be created - create only filters based
      on active local idents which has already claimed an address
      --> avoid to much Filter Boxes
    */
  void TracPTO_c::checkCreateReceiveFilter( )
  {
    IsoMonitor_c& c_isoMonitor = getIsoMonitorInstance4Comm();
    CanIo_c &c_can = getCanInstance4Comm();

    if ( ( !checkFilterCreated() ) && ( c_isoMonitor.existActiveLocalIsoMember() ) )
    { // check if needed receive filters for ISO are active
      setFilterCreated();
      // create FilterBox_c for PGN FRONT_PTO_STATE_PGN, PF 254 - mask for DP, PF and PS
      // mask: (0x3FFFF << 8) filter: (TIME_DATE_PGN << 8)
      c_can.insertStandardIsoFilter(*this,FRONT_PTO_STATE_PGN,false);

      // create FilterBox_c for PGN REAR_PTO_STATE_PGN, PF 254 - mask for DP, PF and PS
      // mask: (0x3FFFF << 8) filter: (TIME_DATE_PGN << 8)
      c_can.insertStandardIsoFilter(*this,REAR_PTO_STATE_PGN,true);

    }
  }

  bool TracPTO_c::processMsgRequestPGN (uint32_t aui32_pgn, IsoItem_c* apc_isoItemSender, IsoItem_c* apc_isoItemReceiver)
  {
    // check if we are allowed to send a request for pgn
    if ( ! BaseCommon_c::check4ReqForPgn(aui32_pgn, apc_isoItemSender, apc_isoItemReceiver) ) return false;

    // call TracPto_c function to send pto informtation
    // isoSendMessage checks if this item (identified by ISOName)
    // is configured to send pto information
    if ( aui32_pgn == FRONT_PTO_STATE_PGN  && (IsoAgLib::IsoActiveFlag_t)(mt_ptoFront.t_ptoEngaged) != IsoAgLib::IsoActive)
    {
      sendMessage(sendFrontPto);
    }
    if ( aui32_pgn == REAR_PTO_STATE_PGN && (IsoAgLib::IsoActiveFlag_t)mt_ptoRear.t_ptoEngaged != IsoAgLib::IsoActive)
    {
      sendMessage(sendRearPto);
    }
    return true;
  }

  /** process a ISO11783 base information PGN
      @pre  sender of message is existent in monitor list
      @see  CanPkgExt_c::resolveSendingInformation()
    */
  bool TracPTO_c::processMsg()
  {
    // there is no need to check if sender exist in the monitor list because this is already done
    // in CanPkgExt_c -> resolveSendingInformation
    IsoName_c const& rcc_tempISOName = data().getISONameForSA();

    if (((data().isoPgn() /*& 0x3FFFF*/) == FRONT_PTO_STATE_PGN) || ((data().isoPgn() /*& 0x3FFFF*/) == REAR_PTO_STATE_PGN))
    {
      const int32_t ci32_now = data().time();
      // only take values, if i am not the regular sender
      // and if actual sender isn't in conflict to previous sender
      if ( checkParseReceived( rcc_tempISOName ) )
      { // sender is allowed to send
        PtoData_t* pt_ptoData = NULL;
        if (data().isoPgn() == FRONT_PTO_STATE_PGN)
        { // front PTO
          pt_ptoData = &mt_ptoFront;
        }
        else
        { // rear PTO
          pt_ptoData = &mt_ptoRear;
        }

        pt_ptoData->i32_lastPto = ci32_now;
        pt_ptoData->ui16_pto8DigitPerRpm         = data().getUint16Data(0);
        pt_ptoData->ui16_ptoSetPoint8DigitPerRpm = data().getUint16Data(2);
        pt_ptoData->t_ptoEngaged = IsoAgLib::IsoActiveFlag_t(          (    data().getUint8Data(4) >> 6) & 3 );
        pt_ptoData->t_pto1000    = IsoAgLib::IsoActiveFlag_t(          (    data().getUint8Data(4) >> 4) & 3 );
        pt_ptoData->t_ptoEconomy = IsoAgLib::IsoActiveFlag_t(          (    data().getUint8Data(4) >> 2) & 3 );
        pt_ptoData->t_ptoEngagementReqStatus   = IsoAgLib::IsoReqFlag_t(    data().getUint8Data(4)       & 3 );
        pt_ptoData->t_ptoModeReqStatus         = IsoAgLib::IsoReqFlag_t(   (data().getUint8Data(5) >> 6) & 3 );
        pt_ptoData->t_ptoEconomyModeReqStatus  = IsoAgLib::IsoReqFlag_t(   (data().getUint8Data(5) >> 4) & 3 );
        pt_ptoData->t_ptoShaftSpeedLimitStatus = IsoAgLib::IsoLimitFlag_t( (data().getUint8Data(5) >> 1) & 0x7 );

        // set last time
        setSelectedDataSourceISOName (rcc_tempISOName);
        // update time
        pt_ptoData->i32_lastPto = data().time();
        // must be set because this is needed in basecommon_c
        setUpdateTime( pt_ptoData->i32_lastPto );

        //msg from Tractor received do tell Scheduler_c next call not until  3000ms
        changeRetriggerTimeAndResort(data().time() + TIMEOUT_TRACTOR_DATA);
      }

      else
      { // there is a sender conflict
        getILibErrInstance().registerError( iLibErr_c::BaseSenderConflict, iLibErr_c::Base );
        return false;
      }
    }
    return false;
  }

  /** send a ISO11783 base information PGN.
    * this is only called when sending ident is configured and it has already claimed an address
      @pre  function is only called in tractor mode
      @see  BaseCommon_c::timeEvent()
    */
  bool TracPTO_c::timeEventTracMode( )
  {
     ///Timeperiod of 100ms is set in ::config
    if ( mt_ptoFront.t_ptoEngaged == IsoAgLib::IsoActive  )
    { // it's time to send tractor PTO information and the FRONT PTO is engaged
      sendMessage(sendFrontPto);
    }

    if ( getAvailableExecTime() == 0) return false;

    ///Timeperiod of 100ms is set in ::config
    if ( mt_ptoRear.t_ptoEngaged == IsoAgLib::IsoActive  )
    { // it's time to send tractor PTO information and the REAR PTO is engaged
      sendMessage(sendRearPto);
    }

    return true;
  }

  /** Detect stop of PTO update from tractor -> indication for stopped PTO */
  bool TracPTO_c::timeEventImplMode()
  {
    const int32_t ci32_now = getLastRetriggerTime();
    // check for different pto data types whether the previously
    // sending node stopped sending -> other nodes can now step in
    if ( ( (ci32_now - mt_ptoFront.i32_lastPto)  >= TIMEOUT_TRACTOR_DATA
           || getSelectedDataSourceISOName().isUnspecified()
         )
         && ( (IsoAgLib::IsoActiveFlag_t)(mt_ptoFront.ui16_pto8DigitPerRpm) != 0 && (IsoAgLib::IsoActiveFlag_t)(mt_ptoFront.t_ptoEngaged) != IsoAgLib::IsoInactive)
       )
    { // TECU stoppped its PTO and doesn'T send PTO updates - as defined by ISO 11783
      // --> switch values to ZERO
      mt_ptoFront.ui16_pto8DigitPerRpm = 0;
      mt_ptoFront.t_ptoEngaged = IsoAgLib::IsoInactive;
    }
    if ( ( ( ci32_now - mt_ptoRear.i32_lastPto ) >= TIMEOUT_TRACTOR_DATA
           || (getSelectedDataSourceISOName().isUnspecified() )
         )
         && ((IsoAgLib::IsoActiveFlag_t)(mt_ptoRear.ui16_pto8DigitPerRpm) != 0 && (IsoAgLib::IsoActiveFlag_t)(mt_ptoRear.t_ptoEngaged) != IsoAgLib::IsoInactive )
       )
    { // TECU stoppped its PTO and doesn'T send PTO updates - as defined by ISO 11783
      // --> switch values to ZERO
      mt_ptoRear.ui16_pto8DigitPerRpm = 0;
      mt_ptoRear.t_ptoEngaged = IsoAgLib::IsoInactive;
    }
    return true;
  }

  /** send pto data message
      @param t_sendptodata  send pto front or pto rear
      @see  TracCert_c::processMsgRequestPGN
      @see  CanIo_c::operator<<
    */
  void TracPTO_c::sendMessage(SendPtoData_t t_sendptodata)
  {
    if ( getISOName() == NULL ) return;
    if (!getIsoMonitorInstance4Comm().existIsoMemberISOName(*getISOName(), true)) return;

    CanIo_c& c_can = getCanInstance4Comm();

    data().setISONameForSA( *getISOName() );
    data().setIdentType(Ident_c::ExtendedIdent);
    data().setIsoPri(3);
    data().setLen(8);

    setSelectedDataSourceISOName(*getISOName());

    PtoData_t* pt_ptoData = NULL;
    if (t_sendptodata == sendFrontPto)
    {
      data().setIsoPgn(FRONT_PTO_STATE_PGN);
      pt_ptoData = &mt_ptoFront;
    }
    else
    { //sendRearPto
      data().setIsoPgn(REAR_PTO_STATE_PGN);
      pt_ptoData = &mt_ptoRear;
    }

    uint8_t ui8_val;
    data().setUint16Data(0, pt_ptoData->ui16_pto8DigitPerRpm);
    data().setUint16Data(2, pt_ptoData->ui16_ptoSetPoint8DigitPerRpm);
    ui8_val =  (pt_ptoData->t_ptoEngaged             << 6);
    ui8_val |= (pt_ptoData->t_pto1000                << 4);
    ui8_val |= (pt_ptoData->t_ptoEconomy             << 2);
    ui8_val |= (pt_ptoData->t_ptoEngagementReqStatus << 0);
    data().setUint8Data(4, ui8_val);
    ui8_val = 0;
    ui8_val |= (pt_ptoData->t_ptoModeReqStatus         << 6);
    ui8_val |= (pt_ptoData->t_ptoEconomyModeReqStatus  << 4);
    ui8_val |= (pt_ptoData->t_ptoShaftSpeedLimitStatus << 1);
    data().setUint8Data(5, ui8_val);
    //reserved fields
    data().setUint16Data(6, 0xFFFF);

    // CanIo_c::operator<< retreives the information with the help of CanPkg_c::getData
    // then it sends the data
    c_can << data();

    // update time
    pt_ptoData->i32_lastPto = getLastRetriggerTime();
  }

  /** force a request for pgn for front pto state */
  bool TracPTO_c::sendRequestUpdateFront()
  {
    if ( checkMode(IsoAgLib::IdentModeImplement) )
      return BaseCommon_c::sendPgnRequest(FRONT_PTO_STATE_PGN);
    else
      return false;
  }
  /** force a request for pgn for rear pto state */
  bool TracPTO_c::sendRequestUpdateRear()
  {
    if ( checkMode(IsoAgLib::IdentModeImplement) )
      return BaseCommon_c::sendPgnRequest(REAR_PTO_STATE_PGN);
    else
      return false;
  }

/// Funktion for Debugging in Scheduler_c
const char*
TracPTO_c::getTaskName() const
{   return "TracPTO_c"; }


} // End Namespace __IsoAgLib
