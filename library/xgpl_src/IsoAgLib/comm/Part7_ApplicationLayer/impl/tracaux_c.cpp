/***************************************************************************
                  tracaux_c.cpp - working on Base Data Msg Type;
                                  stores, updates  and delivers all base
                                  data informations from CanCustomer_c
                                  derived for CAN sending and receiving
                                  interaction;
                                  from ElementTracAux_c derived for
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
#include <IsoAgLib/driver/can/impl/canio_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isomonitor_c.h>
#include "tracaux_c.h"

using namespace std;

namespace __IsoAgLib {

  #if defined(PRT_INSTANCE_CNT) && (PRT_INSTANCE_CNT > 1)
  /** C-style function, to get access to the unique TracAux_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an m_index must be given to select the wanted BUS
    */
  TracAux_c& getTracAuxInstance( uint8_t rui8_instance )
  { // if > 1 singleton instance is used, no static reference can be used
    return TracAux_c::instance( rui8_instance );
  };
  #else
  /** C-style function, to get access to the unique TracAux_c singleton instance */
  TracAux_c& getTracAuxInstance( void )
  {
    static TracAux_c& c_lbsTracAux = TracAux_c::instance();
    return c_lbsTracAux;
  };
  #endif


     /** initialize directly after the singleton instance is created.
        this is called from singleton.h and should NOT be called from the user again.
        users please use init(...) instead.
      */
      void TracAux_c::singletonInit()
      { // singletonInit is called, AFTER the initializing instance() function has assigned a suitable
        // singleton vec key - this key value is NOT available at construction time!!!
        BaseCommon_c::singletonInitBase(SINGLETON_VEC_KEY);
      }

  /** initialise element which can't be done during construct;
      above all create the needed FilterBox_c instances
      possible errors:
        * dependant error in CanIo_c problems during insertion of new FilterBox_c entries for IsoAgLibBase
      @param rpc_isoName optional pointer to the ISOName variable of the ersponsible member instance (pointer enables automatic value update if var val is changed)
      @param ai_singletonVecKey singleton vector key in case PRT_INSTANCE_CNT > 1
      @param rt_identMode either IsoAgLib::IdentModeImplement or IsoAgLib::IdentModeTractor
    */
  void TracAux_c::init_base (const IsoName_c* rpc_isoName, int /*ai_singletonVecKey*/, IsoAgLib::IdentMode_t rt_identMode)
  {
    //call init for handling which is base data independent
    BaseCommon_c::init_base (rpc_isoName, getSingletonVecKey(), rt_identMode);
  };

  /** config the TracAux_c object after init -> set pointer to isoName and
      config send/receive of different base msg types
      @param rpc_isoName pointer to the ISOName variable of the responsible member instance (pointer enables automatic value update if var val is changed)
      @param rt_identMode either IsoAgLib::IdentModeImplement or IsoAgLib::IdentModeTractor
      @return true -> configuration was successfull
    */
  bool TracAux_c::config_base(const IsoName_c* rpc_isoName, IsoAgLib::IdentMode_t rt_identMode, uint16_t rui16_suppressMask)
  {
    //call config for handling which is base data independent
    //if something went wrong leave function before something is configured
    if ( !BaseCommon_c::config_base (rpc_isoName, rt_identMode, rui16_suppressMask) ) return false;

    if ( rt_identMode == IsoAgLib::IdentModeImplement )
    {
      // set Time Period in ms for Scheduler_c
      setTimePeriod( (uint16_t) 100 );
    }
    else
    { //IdentModeTractor
      // set Time Period in ms for Scheduler_c
      setTimePeriod( (uint16_t) 1000 );
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

  /** send estimated and measured messages (only tractor mode)
      @pre client has already claimed an address
      @see CanIo_c::operator<<
    */
  void TracAux_c::isoSendEstimatedMeasured()
  { // there is no need to check for address claim because this is already done in the timeEvent
    // function of base class BaseCommon_c

    CanIo_c& c_can = getCanInstance4Comm();
    setSelectedDataSourceISOName(*getISOName());

    data().setISONameForSA( *getISOName() );
    data().setIdentType(Ident_c::ExtendedIdent);
    data().setIsoPri(3);
    data().setLen(8);

    for (int i = 0; i < nrOfValves; i++)
    {
      if (marr_auxFlag[i].ui8_estimatedActivated)
      {
        uint8_t ui8_temp;
        data().setIsoPgn(AUX_VALVE_0_ESTIMATED_FLOW | i);

        data().setUint8Data(0, marr_valve[i].ui8_extendPortEstFlow);
        data().setUint8Data(1, marr_valve[i].ui8_retractPortEstFlow);
        ui8_temp = 0;
        ui8_temp |= marr_valve[i].ui8_estValveState;
        ui8_temp |= (marr_valve[i].ui8_estFailSaveMode << 6);
        data().setUint8Data(2, ui8_temp);
        ui8_temp = 0;
        ui8_temp |= (marr_valve[i].ui8_estValveLimitStatus << 5);
        data().setUint8Data(3, ui8_temp );
        // Reserved fields
        data().setUint32Data(4, 0xFFFFFFFF);

        // CanIo_c::operator<< retreives the information with the help of CanPkg_c::getData
        // then it sends the data
        c_can << data();
      }

      if (marr_auxFlag[i].ui8_measuredActivated)
      {
        uint8_t ui8_temp;
        data().setIsoPgn(AUX_VALVE_0_MEASURED_FLOW | i);

        data().setUint8Data(0, marr_valve[i].ui8_extendPortMeasuredFlow);
        data().setUint8Data(1, marr_valve[i].ui8_retractPortMeasuredFlow);
        data().setUint8Data(2, marr_valve[i].ui16_extendPortPressure & 0xFF);
        data().setUint8Data(3, marr_valve[i].ui16_extendPortPressure >> 8);
        data().setUint8Data(4, marr_valve[i].ui16_retractPortPressure & 0xFF);
        data().setUint8Data(5, marr_valve[i].ui16_retractPortPressure >> 8);
        data().setUint8Data(6, marr_valve[i].ui8_returnPortPressure);
        ui8_temp = 0;
        ui8_temp |= (marr_valve[i].ui8_measuredValveLimitStatus << 5);
        data().setUint8Data(7, ui8_temp);
        // Reserved fields

        // CanIo_c::operator<< retreives the information with the help of CanPkg_c::getData
        // then it sends the data
        c_can << data();
      }
    }
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

    CanIo_c& c_can = getCanInstance4Comm();

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

        // CanIo_c::operator<< retreives the information with the help of CanPkg_c::getData
        // then it sends the data
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
    CanIo_c &c_can = getCanInstance4Comm();

    if ((!checkFilterCreated()) && (c_isoMonitor.existActiveLocalIsoMember()))
    { // check if needed receive filters for ISO are active
      setFilterCreated();
      // insert Filter now with 0x3FFF0 so that all 16 AUX_VALVEs are registered at once!
      c_can.insertFilter(*this, (static_cast<MASK_TYPE>(0x3FFF0) << 8),
                        (static_cast<MASK_TYPE>(AUX_VALVE_0_ESTIMATED_FLOW) << 8), false, Ident_c::ExtendedIdent);
      c_can.insertFilter(*this, (static_cast<MASK_TYPE>(0x3FFF0) << 8),
                        (static_cast<MASK_TYPE>(AUX_VALVE_0_MEASURED_FLOW) << 8), false, Ident_c::ExtendedIdent);
      c_can.insertFilter(*this, (static_cast<MASK_TYPE>(0x3FFF0) << 8),
                        (static_cast<MASK_TYPE>(AUX_VALVE_0_COMMAND) << 8), true, Ident_c::ExtendedIdent);
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

///  Used for Debugging Tasks in Scheduler_c
const char*
TracAux_c::getTaskName() const
{   return "TracAux_c"; }

/** dummy implementation
    @todo SOON-135: add answering of requestPGN in case this object is configured for sending of these information
           - verify this also for the other TracFoo classes
  */
bool TracAux_c::processMsgRequestPGN (uint32_t /*rui32_pgn*/, IsoItem_c* /*rpc_isoItemSender*/, IsoItem_c* /*rpc_isoItemReceiver*/)
{
  return false;
}

} // end of namespace __IsoAgLib
