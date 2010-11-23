/*
  tracaux_c.cpp: working on Base Data Msg Type; stores, updates and
    delivers all base data informations from CanCustomer_c derived for
    CAN sending and receiving interaction; from ElementTracAux_c
    derived for interaction with other IsoAgLib objects

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include "tracaux_c.h"
#include <IsoAgLib/comm/impl/isobus_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isomonitor_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isorequestpgn_c.h>
#include <IsoAgLib/util/iassert.h>


namespace {

uint32_t const gcui32_pgnMask16consecutive = 0x3FFF0uL;

} //namespace


namespace __IsoAgLib {

  /** C-style function, to get access to the unique TracAux_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an m_index must be given to select the wanted BUS
    */
  TracAux_c &getTracAuxInstance( uint8_t aui8_instance )
  { // if > 1 singleton instance is used, no static reference can be used
    MACRO_MULTITON_GET_INSTANCE_BODY(TracAux_c, PRT_INSTANCE_CNT, aui8_instance);
  }

  /** config the TracAux_c object after init -> set pointer to isoName and
      config send/receive of different base msg types
      @param rpc_isoName pointer to the ISOName variable of the responsible member instance (pointer enables automatic value update if var val is changed)
      @param rt_identMode either IsoAgLib::IdentModeImplement or IsoAgLib::IdentModeTractor
      @return true -> configuration was successfull
    */
  bool TracAux_c::config_base(const IsoName_c* rpc_isoName, IsoAgLib::IdentMode_t at_identMode, uint16_t rui16_suppressMask)
  {
    //store old mode to decide to register or unregister to request for pgn
    IsoAgLib::IdentMode_t t_oldMode = getMode();

    //call config for handling which is base data independent
    //if something went wrong leave function before something is configured
    if ( !BaseCommon_c::config_base (rpc_isoName, at_identMode, rui16_suppressMask) ) return false;

    if ( at_identMode == IsoAgLib::IdentModeImplement )
    {
      // set Time Period in ms for Scheduler_c
      setTimePeriod( (uint16_t) 100 );
    }
    else
    { //IdentModeTractor
      // set Time Period in ms for Scheduler_c
      setTimePeriod( (uint16_t) 1000 );
    }

    // un-/register to PGN
    if (t_oldMode == at_identMode)
      ; // no change, still the same mode
    else if (at_identMode == IsoAgLib::IdentModeTractor) {
      // a change from Implement mode to Tractor mode occured
      RegisterPgn_s s_register = getRegisterPgn();
      s_register(AUX_VALVE_0_ESTIMATED_FLOW, gcui32_pgnMask16consecutive);
      s_register(AUX_VALVE_0_MEASURED_FLOW, gcui32_pgnMask16consecutive);
      s_register(AUX_VALVE_0_COMMAND, gcui32_pgnMask16consecutive);
    } else {
      // a change from Tractor mode to Implement mode occured
      UnregisterPgn_s s_unregister = getUnregisterPgn();
      s_unregister(AUX_VALVE_0_ESTIMATED_FLOW, gcui32_pgnMask16consecutive);
      s_unregister(AUX_VALVE_0_MEASURED_FLOW, gcui32_pgnMask16consecutive);
      s_unregister(AUX_VALVE_0_COMMAND, gcui32_pgnMask16consecutive);
    }

    // set configure values
    mi32_lastMsgReceivedCmd = 0;

    for (int i = 0; i < nrOfValves; i++)
    {
      marr_auxFlag[i].ui8_estimatedActivated = false;
      marr_auxFlag[i].ui8_measuredActivated = false;
      marr_auxFlag[i].ui8_commandActivated = false;

      marr_valve[i].ui8_extendPortEstFlow = 125;
      marr_valve[i].ui8_retractPortEstFlow = 125;
      marr_valve[i].ui8_estFailSaveMode = IsoAgLib::IsoNotAvailableAux;
      marr_valve[i].ui8_estValveState = IsoAgLib::IsoNotAvailableAuxExt;
      marr_valve[i].ui8_extendPortMeasuredFlow = 125;
      marr_valve[i].ui8_retractPortMeasuredFlow = 125;
      marr_valve[i].ui16_extendPortPressure = 0;
      marr_valve[i].ui16_retractPortPressure = 0;
      marr_valve[i].ui8_returnPortPressure = 0;
      marr_valve[i].ui8_cmdPortFlow = 0;
      marr_valve[i].ui8_cmdFailSaveMode = IsoAgLib::IsoNotAvailableAux;
      marr_valve[i].ui8_cmdValveState = IsoAgLib::IsoNotAvailableAuxExt;
      marr_valve[i].ui8_estValveLimitStatus = IsoAgLib::IsoNotAvailableLimit;
      marr_valve[i].ui8_measuredValveLimitStatus = IsoAgLib::IsoNotAvailableLimit;
    }
    return true;
  }

  /** process a ISO11783 base information PGN
      @pre  sender of message is existent in monitor list
      @see  CanPkgExt_c::resolveSendingInformation()
    */
  bool TracAux_c::processMsg()
  {
    // there is no need to check if sender exist in the monitor list because this is already done
    // in CanPkgExt_c -> resolveSendingInformation
    IsoName_c const& rcc_tempISOName = data().getISONameForSA();

    unsigned int valveNumber = 15;
    const int32_t ci32_now = data().time();

    switch (data().isoPgn() /*& 0x3FFFF*/) // don't need to &, as this is the complete PGN anyway...
    {
      case AUX_VALVE_0_ESTIMATED_FLOW:
        valveNumber--;
      case AUX_VALVE_1_ESTIMATED_FLOW:
        valveNumber--;
      case AUX_VALVE_2_ESTIMATED_FLOW:
        valveNumber--;
      case AUX_VALVE_3_ESTIMATED_FLOW:
        valveNumber--;
      case AUX_VALVE_4_ESTIMATED_FLOW:
        valveNumber--;
      case AUX_VALVE_5_ESTIMATED_FLOW:
        valveNumber--;
      case AUX_VALVE_6_ESTIMATED_FLOW:
        valveNumber--;
      case AUX_VALVE_7_ESTIMATED_FLOW:
        valveNumber--;
      case AUX_VALVE_8_ESTIMATED_FLOW:
        valveNumber--;
      case AUX_VALVE_9_ESTIMATED_FLOW:
        valveNumber--;
      case AUX_VALVE_10_ESTIMATED_FLOW:
        valveNumber--;
      case AUX_VALVE_11_ESTIMATED_FLOW:
        valveNumber--;
      case AUX_VALVE_12_ESTIMATED_FLOW:
        valveNumber--;
      case AUX_VALVE_13_ESTIMATED_FLOW:
        valveNumber--;
      case AUX_VALVE_14_ESTIMATED_FLOW:
        valveNumber--;
      case AUX_VALVE_15_ESTIMATED_FLOW:
        if ( checkParseReceived( rcc_tempISOName ) )
        { // sender is allowed to send
          marr_valve[valveNumber].ui8_extendPortEstFlow = data().getUint8Data(0);
          marr_valve[valveNumber].ui8_retractPortEstFlow = data().getUint8Data(1);
          marr_valve[valveNumber].ui8_estFailSaveMode = ( (data().getUint8Data(2) >> 6) & 3 );
          marr_valve[valveNumber].ui8_estValveState = ( data().getUint8Data(2) & 0xF );
          marr_valve[valveNumber].ui8_estValveLimitStatus = ( data().getUint8Data(3) >> 5);

          setSelectedDataSourceISOName (rcc_tempISOName);
          setUpdateTime( ci32_now );
        }
        else
        { // there is a sender conflict
          getILibErrInstance().registerError( iLibErr_c::BaseSenderConflict, iLibErr_c::Base );
          return false;
        }
        break;
      case AUX_VALVE_0_MEASURED_FLOW:
        valveNumber--;
      case AUX_VALVE_1_MEASURED_FLOW:
        valveNumber--;
      case AUX_VALVE_2_MEASURED_FLOW:
        valveNumber--;
      case AUX_VALVE_3_MEASURED_FLOW:
        valveNumber--;
      case AUX_VALVE_4_MEASURED_FLOW:
        valveNumber--;
      case AUX_VALVE_5_MEASURED_FLOW:
        valveNumber--;
      case AUX_VALVE_6_MEASURED_FLOW:
        valveNumber--;
      case AUX_VALVE_7_MEASURED_FLOW:
        valveNumber--;
      case AUX_VALVE_8_MEASURED_FLOW:
        valveNumber--;
      case AUX_VALVE_9_MEASURED_FLOW:
        valveNumber--;
      case AUX_VALVE_10_MEASURED_FLOW:
        valveNumber--;
      case AUX_VALVE_11_MEASURED_FLOW:
        valveNumber--;
      case AUX_VALVE_12_MEASURED_FLOW:
        valveNumber--;
      case AUX_VALVE_13_MEASURED_FLOW:
        valveNumber--;
      case AUX_VALVE_14_MEASURED_FLOW:
        valveNumber--;
      case AUX_VALVE_15_MEASURED_FLOW:
        if ( checkParseReceived( rcc_tempISOName ) )
        { // sender is allowed to send
          marr_valve[valveNumber].ui8_extendPortMeasuredFlow = data().getUint8Data(0);
          marr_valve[valveNumber].ui8_retractPortMeasuredFlow = data().getUint8Data(1);
          marr_valve[valveNumber].ui16_extendPortPressure =  ( static_cast<uint16_t>(data().getUint8Data(2)) +
                                                             ( static_cast<uint16_t>(data().getUint8Data(3)) << 8 ) );
          marr_valve[valveNumber].ui16_retractPortPressure = ( static_cast<uint16_t>(data().getUint8Data(4)) +
                                                             ( static_cast<uint16_t>(data().getUint8Data(5)) << 8 ) );
          marr_valve[valveNumber].ui8_returnPortPressure = data().getUint8Data(6);
          marr_valve[valveNumber].ui8_measuredValveLimitStatus = ( data().getUint8Data(7) >> 5 );

          setSelectedDataSourceISOName (rcc_tempISOName);
          setUpdateTime( ci32_now );
        }
        else
        { // there is a sender conflict
          getILibErrInstance().registerError( iLibErr_c::BaseSenderConflict, iLibErr_c::Base );
          return false;
        }
        break;
      case AUX_VALVE_0_COMMAND:
        valveNumber--;
      case AUX_VALVE_1_COMMAND:
        valveNumber--;
      case AUX_VALVE_2_COMMAND:
        valveNumber--;
      case AUX_VALVE_3_COMMAND:
        valveNumber--;
      case AUX_VALVE_4_COMMAND:
        valveNumber--;
      case AUX_VALVE_5_COMMAND:
        valveNumber--;
      case AUX_VALVE_6_COMMAND:
        valveNumber--;
      case AUX_VALVE_7_COMMAND:
        valveNumber--;
      case AUX_VALVE_8_COMMAND:
        valveNumber--;
      case AUX_VALVE_9_COMMAND:
        valveNumber--;
      case AUX_VALVE_10_COMMAND:
        valveNumber--;
      case AUX_VALVE_11_COMMAND:
        valveNumber--;
      case AUX_VALVE_12_COMMAND:
        valveNumber--;
      case AUX_VALVE_13_COMMAND:
        valveNumber--;
      case AUX_VALVE_14_COMMAND:
        valveNumber--;
      case AUX_VALVE_15_COMMAND:
        if ( checkMode(IsoAgLib::IdentModeTractor) )
        {
          marr_valve[valveNumber].ui8_cmdPortFlow = data().getUint8Data(0);
          marr_valve[valveNumber].ui8_cmdFailSaveMode = ( ( data().getUint8Data(2) >> 6) & 3);
          marr_valve[valveNumber].ui8_cmdValveState = ( data().getUint8Data(2) & 0xF );
        }
        break;
    }
    return true;
  }

  /** send a ISO11783 marr_valve information PGN in tractor mode
    * this is only called when sending ident is configured and it has already claimed an address
      @pre  function is only called in tractor mode
      @see  BaseCommon_c::timeEvent()
    */
  bool TracAux_c::timeEventTracMode( )
  {
    ///Timeperiod of 1000ms is set in ::config for TracMode
    isoSendEstimatedMeasured();

    if ( getAvailableExecTime() == 0 ) return false;

    return true;
  }

  /** send a ISO11783 marr_valve information PGN in implement mode
    * this is only called when sending ident is configured and it has already claimed an address
      @pre  function is only called in implement mode
      @see  BaseCommon_c::timeEvent()
    */
  bool TracAux_c::timeEventImplMode( )
  {
    ///Timeperiod of 100ms is set in ::config for ImplMode
    isoSendCommand();

    if ( getAvailableExecTime() == 0 ) return false;

    return true;
  }

  void TracAux_c::prepareSendingEstimatedMeasured()
  {
    setSelectedDataSourceISOName(*getISOName());

    data().setISONameForSA( *getISOName() );
    data().setIdentType(Ident_c::ExtendedIdent);
    data().setIsoPri(3);
    data().setLen(8);    
  }

  void TracAux_c::isoSendEstimated(uint32_t aui32_pgn)
  {
    isoaglib_assert(AUX_VALVE_0_ESTIMATED_FLOW <= aui32_pgn);
    isoaglib_assert(aui32_pgn <= AUX_VALVE_15_ESTIMATED_FLOW);
    prepareSendingEstimatedMeasured();
    data().setIsoPgn(aui32_pgn);
    IsoAuxValveData_t &rt_valve = marr_valve[aui32_pgn - AUX_VALVE_0_ESTIMATED_FLOW];
    data().setUint8Data(0, rt_valve.ui8_extendPortEstFlow);
    data().setUint8Data(1, rt_valve.ui8_retractPortEstFlow);
    uint8_t ui8_temp = 0;
    ui8_temp |= rt_valve.ui8_estValveState;
    ui8_temp |= (rt_valve.ui8_estFailSaveMode << 6);
    data().setUint8Data(2, ui8_temp);
    ui8_temp = 0;
    ui8_temp |= (rt_valve.ui8_estValveLimitStatus << 5);
    data().setUint8Data(3, ui8_temp );
    // Reserved fields
    data().setUint32Data(4, 0xFFFFFFFF);

    getIsoBusInstance4Comm() << data();
  }

  void TracAux_c::isoSendMeasured(uint32_t aui32_pgn)
  {
    isoaglib_assert(AUX_VALVE_0_MEASURED_FLOW <= aui32_pgn);
    isoaglib_assert(aui32_pgn <= AUX_VALVE_15_MEASURED_FLOW);
    prepareSendingEstimatedMeasured();

    data().setIsoPgn(aui32_pgn);
    IsoAuxValveData_t &rt_valve = marr_valve[aui32_pgn - AUX_VALVE_0_MEASURED_FLOW];
    data().setUint8Data(0, rt_valve.ui8_extendPortMeasuredFlow);
    data().setUint8Data(1, rt_valve.ui8_retractPortMeasuredFlow);
    data().setUint8Data(2, rt_valve.ui16_extendPortPressure & 0xFF);
    data().setUint8Data(3, rt_valve.ui16_extendPortPressure >> 8);
    data().setUint8Data(4, rt_valve.ui16_retractPortPressure & 0xFF);
    data().setUint8Data(5, rt_valve.ui16_retractPortPressure >> 8);
    data().setUint8Data(6, rt_valve.ui8_returnPortPressure);
    uint8_t ui8_temp = 0;
    ui8_temp |= (rt_valve.ui8_measuredValveLimitStatus << 5);
    data().setUint8Data(7, ui8_temp);
    // Reserved fields

    getIsoBusInstance4Comm() << data();
  }

  /** send estimated and measured messages (only tractor mode)
      @pre client has already claimed an address
      @see CanIo_c::operator<<
    */
  void TracAux_c::isoSendEstimatedMeasured()
  {
    // there is no need to check for address claim because this is already done in the timeEvent
    // function of base class BaseCommon_c
    for (int i = 0; i < nrOfValves; ++i) {
      if (marr_auxFlag[i].ui8_estimatedActivated)
        isoSendEstimated(AUX_VALVE_0_ESTIMATED_FLOW + i);
      if (marr_auxFlag[i].ui8_measuredActivated)
        isoSendMeasured(AUX_VALVE_0_MEASURED_FLOW + i);
    }
  }

  TracAux_c::SendCommand_e TracAux_c::isoSendCommand(uint32_t aui32_pgn)
  {
    isoaglib_assert(AUX_VALVE_0_COMMAND <= aui32_pgn);
    isoaglib_assert(aui32_pgn <= AUX_VALVE_15_COMMAND);
    // as BaseCommon_c timeEvent() checks only for adr claimed state in TractorMode, we have to perform those checks here,
    // as we reach this function mostly for ImplementMode, where getISOName() might report NULL at least during init time
    if ( ( NULL == getISOName() ) || ( ! getIsoMonitorInstance4Comm().existIsoMemberISOName( *getISOName(), true ) ) )
      return CommandNotSent;

    data().setISONameForSA( *getISOName() );
    data().setIdentType(Ident_c::ExtendedIdent);
    data().setIsoPri(3);
    data().setLen(8);

    data().setIsoPgn(aui32_pgn);

    IsoAuxValveData_t &rt_valve = marr_valve[aui32_pgn - AUX_VALVE_0_COMMAND];
    data().setUint8Data(0, rt_valve.ui8_cmdPortFlow);
    uint8_t ui8_temp = 0;
    ui8_temp |= rt_valve.ui8_cmdValveState;
    ui8_temp |= (rt_valve.ui8_cmdFailSaveMode << 6);
    data().setUint8Data(2, ui8_temp);
    // Reserved fields
    data().setUint8Data(1, 0xFF);
    data().setUint32Data(3, 0xFFFFFFFF);
    data().setUint8Data(7, 0xFF);

    getIsoBusInstance4Comm() << data();
    return CommandSent;
  }

  /** send command messages (only implement mode)
      @pre client has already claimed an address
      @see CanIo_c::operator<<
    */
  void TracAux_c::isoSendCommand()
  { // as BaseCommon_c timeEvent() checks only for adr claimed state in TractorMode, we have to perform those checks here,
    // as we reach this function mostly for ImplementMode, where getISOName() might report NULL at least during init time
    if ( ( NULL == getISOName() ) || ( ! getIsoMonitorInstance4Comm().existIsoMemberISOName( *getISOName(), true ) ) )
      return;

    IsoBus_c& c_can = getIsoBusInstance4Comm();

    data().setISONameForSA( *getISOName() );
    data().setIdentType(Ident_c::ExtendedIdent);
    data().setIsoPri(3);
    data().setLen(8);

     //send all active marr_valve commands
    for (int i = 0; i < nrOfValves; i++)
    {
      if (marr_auxFlag[i].ui8_commandActivated)
      {
        data().setIsoPgn(AUX_VALVE_0_COMMAND | i);

        data().setUint8Data(0, marr_valve[i].ui8_cmdPortFlow);
        uint8_t ui8_temp = 0;
                ui8_temp |= marr_valve[i].ui8_cmdValveState;
                ui8_temp |= (marr_valve[i].ui8_cmdFailSaveMode << 6);
        data().setUint8Data(2, ui8_temp);
        // Reserved fields
        data().setUint8Data(1, 0xFF);
        data().setUint32Data(3, 0xFFFFFFFF);
        data().setUint8Data(7, 0xFF);

        c_can << data();
      }
    }
  }

  /** check if filter boxes shall be created - create only ISO filters based
      on active local idents which has already claimed an address
      --> avoid to much Filter Boxes
    */
  void TracAux_c::checkCreateReceiveFilter()
  {
    IsoMonitor_c& c_isoMonitor = getIsoMonitorInstance4Comm();
    IsoBus_c &c_can = getIsoBusInstance4Comm();

    if ((!checkFilterCreated()) && (c_isoMonitor.existActiveLocalIsoMember()))
    { // check if needed receive filters for ISO are active
      setFilterCreated();
      // insert Filter now with 0x3FFF0 so that all 16 AUX_VALVEs are registered at once!
      c_can.insertFilter(*this, (static_cast<MASK_TYPE>(0x3FFF0) << 8),
                        (static_cast<MASK_TYPE>(AUX_VALVE_0_ESTIMATED_FLOW) << 8), false);
      c_can.insertFilter(*this, (static_cast<MASK_TYPE>(0x3FFF0) << 8),
                        (static_cast<MASK_TYPE>(AUX_VALVE_0_MEASURED_FLOW) << 8), false);
      c_can.insertFilter(*this, (static_cast<MASK_TYPE>(0x3FFF0) << 8),
                        (static_cast<MASK_TYPE>(AUX_VALVE_0_COMMAND) << 8), true);
    }
  }

  /** set value reported by the controller of flow through the extended port of auxiliary marr_valve of a tractor
      which could be based on the commandes of the marr_valve
      possible errors:
        * data range error
      @param valveNumber  auxiliary marr_valve number (range from 0 to 15)
      @param t_val        value to store
      @return             true -> data was set; false -> data range error
    */
  bool TracAux_c::setExtendPortEstFlow(uint8_t valveNumber, int8_t t_val)
  {
    if( !marr_auxFlag[valveNumber].ui8_estimatedActivated )
      activateEstimated(valveNumber);

    if (   t_val >= a21_2to5_MinRange
        && t_val <= a21_2to5_MaxRange
        && valveNumber < nrOfValves
       )
      marr_valve[valveNumber].ui8_extendPortEstFlow = (t_val + a21_2to5_Offset);
    else
    {
      // wrong data range
      getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::Base );
      return false;
    }
    return true;
  }

  /** set value reported by the controller of flow through the retract port of auxiliary marr_valve of a tractor
      which could be based on the commandes of the marr_valve
      possible errors:
          * data range error
      @param valveNumber  auxiliary marr_valve number (range from 0 to 15)
      @param t_val        value to store
      @return             true -> data was set; false -> data range error

  */
  bool TracAux_c::setRetractPortEstFlow(uint8_t valveNumber, int8_t t_val)
  {
    if( !marr_auxFlag[valveNumber].ui8_estimatedActivated )
      activateEstimated(valveNumber);

    if (   t_val >= a21_2to5_MinRange
        && t_val <= a21_2to5_MaxRange
        && valveNumber < nrOfValves
       )
      marr_valve[valveNumber].ui8_retractPortEstFlow = (t_val + a21_2to5_Offset);
    else
    {
      // wrong data range
      getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::Base );
      return false;
    }
    return true;
  }

  /** set measured state of the fail save mode of auxiliary marr_valve
      possible errors:
        * data range error
      @param valveNumber  auxiliary marr_valve number (range from 0 to 15)
      @param t_val        value to store
      @return             true -> data was set; false -> data range error
    */
  bool TracAux_c::setEstFailSaveMode(uint8_t valveNumber, IsoAgLib::IsoAuxFlag_t t_val)
  {
    if( !marr_auxFlag[valveNumber].ui8_estimatedActivated )
      activateEstimated(valveNumber);

    if ( valveNumber < nrOfValves )
      marr_valve[valveNumber].ui8_estFailSaveMode = t_val;
    else
    {
      // wrong data range
      getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::Base );
      return false;
    }
    return true;
  }

  /** set measured state of the auxiliary marr_valve
      possible errors:
        * data range error
      @param valveNumber  auxiliary marr_valve number (range from 0 to 15)
      @param t_val        value to store
      @return             true -> data was set; false -> data range error
    */
  bool TracAux_c::setEstValveState(uint8_t valveNumber, IsoAgLib::IsoAuxFlagExtended_t t_val)
  {
    if( !marr_auxFlag[valveNumber].ui8_estimatedActivated )
      activateEstimated(valveNumber);

    if ( valveNumber < nrOfValves )
      marr_valve[valveNumber].ui8_estValveState = t_val;
    else
    {
      // wrong data range
      getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::Base );
      return false;
    }
    return true;
  }

  /** set measured flow through the extend port auxiliary marr_valve of a tractor, expressed as a percentage of full flow
      possible errors:
        * data range error
      @param valveNumber  auxiliary marr_valve number (range from 0 to 15)
      @param t_val        value to store
      @return             true -> data was set; false -> data range error
    */
  bool TracAux_c::setExtendPortMeasuredFlow(uint8_t valveNumber, int8_t t_val)
  {
    if( !marr_auxFlag[valveNumber].ui8_measuredActivated )
      activateMeasured(valveNumber);

    if (   t_val >= a21_2to5_MinRange
        && t_val <= a21_2to5_MaxRange
        && valveNumber < nrOfValves
       )
      marr_valve[valveNumber].ui8_extendPortMeasuredFlow = (t_val + a21_2to5_Offset);
    else
    {
      // wrong data range
      getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::Base );
      return false;
    }
    return true;
  }

  /** set measured flow through the retract port auxiliary marr_valve of a tractor, expressed as a percentage of full flow
      possible errors:
        * data range error
      @param valveNumber  auxiliary marr_valve number (range from 0 to 15)
      @param t_val        value to store
      @return             true -> data was set; false -> data range error
    */
  bool TracAux_c::setRetractPortMeasuredFlow(uint8_t valveNumber, int8_t t_val)
  {
    if( !marr_auxFlag[valveNumber].ui8_measuredActivated )
      activateMeasured(valveNumber);

    if (   t_val >= a21_2to5_MinRange
        && t_val <= a21_2to5_MaxRange
        && valveNumber < nrOfValves
       )
      marr_valve[valveNumber].ui8_retractPortMeasuredFlow = (t_val + a21_2to5_Offset);
    else
    {
      // wrong data range
      getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::Base );
      return false;
    }
    return true;
  }

  /** set measured nominal pressure at the extend port of auxiliary marr_valve of a tractor
      possible errors:
        * data range error
      @param valveNumber  auxiliary marr_valve number (range from 0 to 15)
      @param t_val        value to store
      @return             true -> data was set; false -> data range error
    */
  bool TracAux_c::setExtendPortPressure(uint8_t valveNumber, uint32_t t_val)
  {
    if( !marr_auxFlag[valveNumber].ui8_measuredActivated )
      activateMeasured(valveNumber);

    if (   t_val <= a21_7to8_MaxRange
        && valveNumber < nrOfValves
       )
      marr_valve[valveNumber].ui16_extendPortPressure = (t_val / a21_7to8_Resolution);
    else
    {
      // wrong data range
      getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::Base );
      return false;
    }
    return true;
  }

  /** set measured nominal pressure at the retract port of auxiliary marr_valve of a tractor
      possible errors:
        * data range error
      @param valveNumber  auxiliary marr_valve number (range from 0 to 15)
      @param t_val        value to store
      @return             true -> data was set; false -> data range error
    */
  bool TracAux_c::setRetractPortPressure(uint8_t valveNumber, uint32_t t_val)
  {
    if( !marr_auxFlag[valveNumber].ui8_measuredActivated )
      activateMeasured(valveNumber);

    if (   t_val <= a21_7to8_MaxRange
        && valveNumber < nrOfValves
       )
      marr_valve[valveNumber].ui16_retractPortPressure = (t_val / a21_7to8_Resolution);
    else
    {
      // wrong data range
      getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::Base );
      return false;
    }
    return true;
  }

  /** set measured nominal pressure at the return port of auxiliary marr_valve of a tractor
      possible errors:
        * data range error
      @param valveNumber  auxiliary marr_valve number (range from 0 to 15)
      @param t_val        value to store
      @return             true -> data was set; false -> data range error
    */
  bool TracAux_c::setReturnPortPressure(uint8_t valveNumber, uint16_t t_val)
  {
    if( !marr_auxFlag[valveNumber].ui8_measuredActivated )
      activateMeasured(valveNumber);

    if (   t_val <= a21_9_MaxRange
        && valveNumber < nrOfValves
       )
      marr_valve[valveNumber].ui8_returnPortPressure = (t_val / a21_9_Resolution);
    else
    {
      // wrong data range
      getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::Base );
      return false;
    }
    return true;
  }

  /** set command for setting the flow through the extend or retract port of auxiliary marr_valve or a tractor, expressed as a
      percentage of full flow
      possible errors:
        * data range error
      @param valveNumber  auxiliary marr_valve number (range from 0 to 15)
      @param t_val        value to store
      @return             true -> data was set; false -> data range error
    */
  bool TracAux_c::setCmdPortFlow(uint8_t valveNumber, uint8_t t_val)
  {
    if( !marr_auxFlag[valveNumber].ui8_commandActivated )
      activateCmd(valveNumber);

    if (   t_val <= a21_10_MaxRange
        && valveNumber < nrOfValves
       )
      marr_valve[valveNumber].ui8_cmdPortFlow = (t_val*5) / 2;
    else
    {
      // wrong data range
      getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::Base );
      return false;
    }
    return true;
  }

  /** set command for setting the fail save mode of auxiliary marr_valve
      possible errors:
        * data range error
      @param valveNumber  auxiliary marr_valve number (range from 0 to 15)
      @param t_val        value to store
      @return             true -> data was set; false -> data range error
    */
  bool TracAux_c::setCmdFailSaveMode(uint8_t valveNumber, IsoAgLib::IsoAuxFlag_t t_val)
  {
    if( !marr_auxFlag[valveNumber].ui8_commandActivated )
      activateCmd(valveNumber);

    if ( valveNumber < nrOfValves )
      marr_valve[valveNumber].ui8_cmdFailSaveMode = t_val;
    else
    {
      // wrong data range
      getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::Base );
      return false;
    }
    return true;
  }

  /** set command for setting the auxiliary marr_valve state
      possible errors:
        * data range error
      @param valveNumber  auxiliary marr_valve number (range from 0 to 15)
      @param t_val        value to store
      @return             true -> data was set; false -> data range error
    */
  bool TracAux_c::setCmdValveState(uint8_t valveNumber, IsoAgLib::IsoAuxFlagExtended_t t_val)
  {
    if( !marr_auxFlag[valveNumber].ui8_commandActivated )
      activateCmd(valveNumber);

    if ( valveNumber < nrOfValves )
      marr_valve[valveNumber].ui8_cmdValveState = t_val;
    else
    {
      // wrong data range
      getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::Base );
      return false;
    }
    return true;
  }

  /** set estimated present limit status of auxiliary marr_valve
    possible errors:
      * data range error
    @param valveNumber  auxiliary marr_valve number (range from 0 to 15)
    @param rt_val       limit status to store
    @return             true -> data was set; false -> data range error
  */
  bool TracAux_c::setEstValveLimitStatus(const uint8_t valveNumber, const IsoAgLib::IsoLimitFlag_t rt_val)
  {
    if( !marr_auxFlag[valveNumber].ui8_estimatedActivated )
      activateEstimated(valveNumber);

    if ( valveNumber < nrOfValves )
      marr_valve[valveNumber].ui8_estValveLimitStatus = rt_val;
    else
    {
      // wrong data range
      getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::Base );
      return false;
    }
    return true;
  }

 /** set measured present limit status of auxiliary marr_valve
    possible errors:
      * data range error
    @param valveNumber  auxiliary marr_valve number (range from 0 to 15)
    @param rt_val       limit status to store
    @return             true -> data was set; false -> data range error
  */
  bool TracAux_c::setMeasuredValveLimitStatus(const uint8_t valveNumber, const IsoAgLib::IsoLimitFlag_t rt_val)
  {
    if( !marr_auxFlag[valveNumber].ui8_measuredActivated )
      activateMeasured(valveNumber);

    if ( valveNumber < nrOfValves )
      marr_valve[valveNumber].ui8_measuredValveLimitStatus = rt_val;
    else
    {
      // wrong data range
      getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::Base );
      return false;
    }
    return true;
  }

#if DEBUG_SCHEDULER
const char*
TracAux_c::getTaskName() const
{ return "TracAux_c"; }
#endif


bool TracAux_c::processMsgRequestPGN (uint32_t aui32_pgn, IsoItem_c* apc_isoItemSender, IsoItem_c* apc_isoItemReceiver)
{
  // check if we are allowed to send a request for pgn
  if ( ! BaseCommon_c::check4ReqForPgn(aui32_pgn, apc_isoItemSender, apc_isoItemReceiver) )
    return false;

  bool const cb_matchEstimated = IsoRequestPgn_c::doMatchPgns(
      aui32_pgn,
      AUX_VALVE_0_ESTIMATED_FLOW,
      gcui32_pgnMask16consecutive);
  if (cb_matchEstimated) {
    isoSendEstimated(aui32_pgn);
    return true;
  }

  bool const cb_matchMeasured = IsoRequestPgn_c::doMatchPgns(
      aui32_pgn,
      AUX_VALVE_0_MEASURED_FLOW,
      gcui32_pgnMask16consecutive);
  if (cb_matchMeasured) {
    isoSendMeasured(aui32_pgn);
    return true;
  }

  bool const cb_matchCommand = IsoRequestPgn_c::doMatchPgns(
      aui32_pgn,
      AUX_VALVE_0_COMMAND,
      gcui32_pgnMask16consecutive);
  if (cb_matchCommand) {
    return CommandSent == isoSendCommand(aui32_pgn);
  }

  return false;
}

} // end of namespace __IsoAgLib
