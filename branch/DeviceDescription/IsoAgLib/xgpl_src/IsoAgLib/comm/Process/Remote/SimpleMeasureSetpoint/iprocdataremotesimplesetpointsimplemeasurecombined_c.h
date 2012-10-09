/***************************************************************************
                          iprocdataremotesimplesetpointsimplemeasurecombined_c.h  -
                             managing of remote process data object
                             -------------------
    begin                : Fri Apr 07 2000
    copyright            : (C) 2000 - 2004 by Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
    type                 : Header
    $LastChangedDate$
    $LastChangedRevision$
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
 * Copyright (C) 2000 - 2004 Dipl.-Inform. Achim Spangler                  *
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
#ifndef IPROC_DATA_REMOTE_SIMPLESETPOINT_SIMPLEMEASURE_COMBINED_H
#define IPROC_DATA_REMOTE_SIMPLESETPOINT_SIMPLEMEASURE_COMBINED_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include "impl/procdataremotesimplesetpointsimplemeasurecombined_c.h"

// Begin Namespace IsoAgLib
namespace IsoAgLib {

/**
  @brief Interface for managing of remote process data object with restricted measurement
  and setpoint management ( %i.e. only one-shot measurement requests;
  set setpoint, without handling of acceptance, several concurring setpoint commanding ECUs ).
  This class can't manage different values for measurement and setpoint value, which is provided
  in contrast by IsoAgLib::iProcDataRemoteSimpleSetpointSimpleMeasure_c .

  Has __IsoAgLib::SimpleManageMeasureProgRemote_c / IsoAgLib::iSimpleManageMeasureProgRemote_c and
  __IsoAgLib::SimpleManageSetpointRemote_c / IsoAgLib::iSimpleManageSetpointRemote_c
  as member for doing setpoint and measurement specific jobs.
  \n
  Projects which use this class, must include the files and succeding subdirectories
  of the following subdirectories of xgpl_src/IsoAgLib/Process:
  - \e Remote/SimpleSetpointElements and
  - \e impl
  \n
  Example:
  \code
  // define local device type
  IsoAgLib::GetyPos c_myGtp( 1, 0 );
  // creation of process data instance
  iProcDataRemoteSimpleSetpoint_c c_workState;
  // init for LIS=0, remote device type/subtype=5/0, complete work width=0xFF,
  // target process data/PRI=2, pointer to my local device type ( to resolve dynamic SA at time of cmd send )
  c_workState.init( 0, IsoAgLib::iGetyPos_c( 0x5, 0 ), 0x5, 0x0, 0xFF, 2, IsoAgLib::iGetyPos_c( 0x5, 0 ), &c_myGtp );

  // request current measurement value ( real value, which can differ from commanded value ); triger update request
  int lastReceivedMeasureState = c_workState.masterVal( true );
  // the restricted measurement feature set doesn't provide measurement programs

  // request current setpoint value ( parameter true -> send request to remote ECU, false -> just deliver last received value )
  int lastReceivedState = c_workState.setpointMasterVal( false );
  // set a setpoint for remote data ( SA / nr of sending and receiving node is derived at time of send
  // -> process data can be initialized even if both ECU are not yet claimed address on BUS )
  c_workState.setSetpointMasterVal( 100 );
  // in spite to the standard setpoint management, this class doesn't provide
  // check for master setpoint, acceptance, etc.
  \endcode

  @author Dipl.-Inform. Achim Spangler
*/
class iProcDataRemoteSimpleSetpointSimpleMeasureCombined_c : private __IsoAgLib::ProcDataRemoteSimpleSetpointSimpleMeasureCombined_c {
private:
  friend class EventSource_c;
public:

  /**
    constructor which can set all element vars
    ISO parameter
    @param rui16_DDI optional DDI code of this instance
    @param rui16_element optional Element code of this instance

    DIN parameter
    @param rui8_lis optional LIS code of this instance
    @param rui8_wert optional WERT code of this instance
    @param rui8_inst optional INST code of this instance
    @param rui8_zaehlnum optional ZAEHLNUM code of this instance

    @param rc_gtp optional GETY_POS code of this instance
    @param rui8_pri PRI code of messages with this process data instance (default 2)
    @param rc_ownerGtp optional GETY_POS of the owner
    @param rpc_commanderGtp pointer to updated GETY_POS variable of commander
    @param rpc_processDataChangeHandler optional pointer to handler class of application
    @param ri_singletonVecKey optional key for selection of IsoAgLib instance (default 0)
  */
  iProcDataRemoteSimpleSetpointSimpleMeasureCombined_c(
#ifdef USE_ISO_11783
                                  uint16_t rui16_DDI = 0,
                                  uint16_t rui16_element = 0xFF,
#endif
#ifdef USE_DIN_9684
                                  uint8_t rui8_lis = 0xFF,
                                  uint8_t rui8_wert = 0,
                                  uint8_t rui8_inst = 0,
                                  uint8_t rui8_zaehlnum = 0xFF,
#endif
                                  iGetyPos_c rc_gtp = 0,
                                  uint8_t rui8_pri = 2,
                                  iGetyPos_c rc_ownerGtp = iGetyPos_c(0xF, 0xF),
                                  iGetyPos_c* rpc_commanderGtp = NULL,
                                  ProcessDataChangeHandler_c *rpc_processDataChangeHandler = NULL,
                                  int ri_singletonVecKey = 0)
  : ProcDataRemoteSimpleSetpointSimpleMeasureCombined_c(
#ifdef USE_ISO_11783
                                   rui16_DDI, rui16_element,
#endif
#ifdef USE_DIN_9684
                                   rui8_lis, rui8_wert, rui8_inst, rui8_zaehlnum,
#endif
                                   rc_gtp, rui8_pri, rc_ownerGtp, rpc_commanderGtp,
                                   rpc_processDataChangeHandler, ri_singletonVecKey){};

  /**
    initialise this ProcDataRemoteSimpleSetpointSimpleMeasureCombined_c instance to a well defined initial state
    ISO parameter
    @param rui16_DDI optional DDI code of this instance
    @param rui16_element optional Element code of this instance

    DIN parameter
    @param rui8_lis optional LIS code of this instance
    @param rui8_wert optional WERT code of this instance
    @param rui8_inst optional INST code of this instance
    @param rui8_zaehlnum optional ZAEHLNUM code of this instance

    @param rc_gtp optional GETY_POS code of this instance
    @param rui8_pri PRI code of messages with this process data instance (default 2)
    @param rc_ownerGtp optional GETY_POS of the owner
    @param rpc_commanderGtp pointer to updated GETY_POS variable of commander
    @param rpc_processDataChangeHandler optional pointer to handler class of application
    @param ri_singletonVecKey optional key for selection of IsoAgLib instance (default 0)
  */
  void init(
#ifdef USE_ISO_11783
            uint16_t rui16_DDI = 0,
            uint16_t rui16_element = 0xFF,
#endif
#ifdef USE_DIN_9684
            uint8_t rui8_lis = 0xFF,
            uint8_t rui8_wert = 0,
            uint8_t rui8_inst = 0,
            uint8_t rui8_zaehlnum = 0xFF,
#endif
            iGetyPos_c rc_gtp = 0,
            uint8_t rui8_pri = 2,
            iGetyPos_c rc_ownerGtp = iGetyPos_c(0xF, 0xF),
            iGetyPos_c* rpc_commanderGtp = NULL,
            ProcessDataChangeHandler_c *rpc_processDataChangeHandler = NULL,
            int ri_singletonVecKey = 0)
    {ProcDataRemoteSimpleSetpointSimpleMeasureCombined_c::init(
#ifdef USE_ISO_11783
                                                               rui16_DDI, rui16_element,
#endif
#ifdef USE_DIN_9684
                                                               rui8_lis, rui8_wert, rui8_inst, rui8_zaehlnum,
#endif
                                                               rc_gtp, rui8_pri, rc_ownerGtp, rpc_commanderGtp,
                                                               rpc_processDataChangeHandler, ri_singletonVecKey);
    };

  /** set the poitner to the handler class
    * @param rpc_processDataChangeHandler pointer to handler class of application
    */
  void setProcessDataChangeHandler( ProcessDataChangeHandler_c *rpc_processDataChangeHandler )
   { ProcDataRemoteSimpleSetpointSimpleMeasureCombined_c::setProcessDataChangeHandler( rpc_processDataChangeHandler ); } ;
  /** deliver the poitner to the handler class
    * @return pointer to handler class of application (or NULL if not defined by application)
    */
  IsoAgLib::ProcessDataChangeHandler_c* getProcessDataChangeHandler( void ) const
   { return ProcDataRemoteSimpleSetpointSimpleMeasureCombined_c::getProcessDataChangeHandler(); } ;

  /**
    deliver value PRI of messages with this
    process data instance
    @return PRI
  */
  uint8_t pri() const{return ProcDataRemoteSimpleSetpointSimpleMeasureCombined_c::pri();};
  /**
    set value PRI of messages with this
    process data instance (default value is 2 == target message)
    @param rb_val new PRI value
  */
  void setPri(uint8_t rb_val){ProcDataRemoteSimpleSetpointSimpleMeasureCombined_c::setPri(rb_val);};
  /**
    deliver value LIS (list number)
    @return LIS
  */
  uint8_t lis() const{return ProcDataRemoteSimpleSetpointSimpleMeasureCombined_c::lis();};
  /**
    deliver value GETY (machine type specific table of process data types)
    @return GETY
  */
  uint8_t gety() const{return ProcDataRemoteSimpleSetpointSimpleMeasureCombined_c::gety();};
  /**
    deliver value WERT (row of process data table)
    @return WERT
  */
  uint8_t wert() const{return ProcDataRemoteSimpleSetpointSimpleMeasureCombined_c::wert();};
  /**
    deliver value INST (column of process data table)
    @return INST
  */
  uint8_t inst() const{return ProcDataRemoteSimpleSetpointSimpleMeasureCombined_c::inst();};
  /**
    deliver value ZAEHLNUM (0xFF == whole working width; else parts of width)
    @return ZAEHLNUM
  */
  uint8_t zaehlnum() const{return ProcDataRemoteSimpleSetpointSimpleMeasureCombined_c::zaehlnum();};
  /**
    deliver value POS (important if more than one machine with equal GETY are active)
    @return POS
  */
  uint8_t pos() const{return ProcDataRemoteSimpleSetpointSimpleMeasureCombined_c::pos();};


  /* ******************************************** *
   * some convenience functions for master        *
   * setpoint access with NOT simple setpoint      *
   * ******************************************** */
  /**
    deliver the actual master setpoint
    @param rb_sendRequest true -> send request for actual value
    @return setpoint value as long
  */
  int32_t setpointMasterVal(bool rb_sendRequest = false)
    {return ProcDataRemoteSimpleSetpointSimpleMeasureCombined_c::setpointMasterVal(rb_sendRequest);};
  /**
    send a setpoint cmd with given exact setpoint
    @param ri32_val commanded setpoint value as long
  */
  void setSetpointMasterVal(int32_t ri32_val)
    {ProcDataRemoteSimpleSetpointSimpleMeasureCombined_c::setSetpointMasterVal(ri32_val);};
  #ifdef USE_FLOAT_DATA_TYPE
  /**
    deliver the actual master setpoint
    @param rb_sendRequest true -> send request for actual value
    @return setpoint value as float
  */
  float setpointMasterValFloat(bool rb_sendRequest = false)
    {return ProcDataRemoteSimpleSetpointSimpleMeasureCombined_c::setpointMasterValFloat(rb_sendRequest);};
  /**
    send a setpoint cmd with given exact setpoint
    @param rf_val commanded setpoint value as float
  */
  void setSetpointMasterVal(float rf_val)
    {ProcDataRemoteSimpleSetpointSimpleMeasureCombined_c::setSetpointMasterVal(rf_val);};
  #endif
  /**
    deliver actual measurement value as long
    @param rb_sendRequest true -> request for new value is sent (optional, default false)
  */
  int32_t masterVal(bool rb_sendRequest = false)
    {return ProcDataRemoteSimpleSetpointSimpleMeasureCombined_c::masterVal(rb_sendRequest);};
  /**
    send reset cmd for the measurement value
  */
  void resetMasterVal(){ProcDataRemoteSimpleSetpointSimpleMeasureCombined_c::resetMasterVal();};
  #ifdef USE_FLOAT_DATA_TYPE
  /**
    deliver actual measurement value as float
    @param rb_sendRequest true -> request for new value is sent (optional, default false)
  */
  float masterValFloat(bool rb_sendRequest = false)
    {return ProcDataRemoteSimpleSetpointSimpleMeasureCombined_c::masterValFloat(rb_sendRequest);};
  #endif

  /**
    deliver the commanderGtp (GETY_POS of local member)
    @return GETY_POS used for sending commands to remote owner member
  */
  iGetyPos_c commanderGtp()const
    {return ProcDataRemoteSimpleSetpointSimpleMeasureCombined_c::commanderGtp();};
  /**
    set the pointer to the commander ident gtp
    @param rpbgtp pointer to GETY_POS var of local member used for
                sending commands to remote owner member
  */
  void setCommanderGtp(iGetyPos_c* rpc_gtp)
    {ProcDataRemoteSimpleSetpointSimpleMeasureCombined_c::setCommanderGtp(rpc_gtp);};


  /**
    deliver the internal unit code, which can be requested by remote ECU
    (important if process data is local managed with different unit than published via ISO11783 or DIN9684
    BUS; interesting if value update is easier with special unit)
    @return internal unit
  */
  uint16_t internalUnit()const
    {return ProcDataRemoteSimpleSetpointSimpleMeasureCombined_c::internalUnit();};
  /**
    set the internal unit code, which can be requested by remote ECU
    (important if process data is local managed with different unit than published via ISO11783 or DIN9684
    BUS; interesting if value update is easier with special unit)
    @param rui16_internalUnit new vaue for internal unit
  */
  void setInternalUnit(uint16_t rui16_internalUnit)
      {ProcDataRemoteSimpleSetpointSimpleMeasureCombined_c::setInternalUnit(rui16_internalUnit);};

  /**
    deliver the central data type of this process data
    @return proc_valType_t: i32_val, ui32_val, float_val, cmdVal
  */
  proc_valType_t valType()const
    {return ProcDataRemoteSimpleSetpointSimpleMeasureCombined_c::valType();};
  /**
    set the central data type of this process data
    @return proc_valType_t: i32_val, ui32_val, float_val, cmdVal
  */
  void setValType(proc_valType_t ren_procValType)
    {ProcDataRemoteSimpleSetpointSimpleMeasureCombined_c::setValType(ren_procValType);};

};

}
#endif
