/***************************************************************************
                          iprocdataremote_c.h  - managing of remote
                                                 process data object
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
#ifndef IPROC_DATA_REMOTE_H
#define IPROC_DATA_REMOTE_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include "impl/procdataremote_c.h"
#include "../StdSetpointElements/isetpointremote_c.h"
#include "../StdMeasureElements/imeasureprogremote_c.h"

// Begin Namespace IsoAgLib
namespace IsoAgLib {

/**
  @brief Interface for managing of remote process data object with standard
  ( %i.e. no restrictions ) feature set.

  Has __IsoAgLib::SetpointRemote_c / IsoAgLib::iSetpointRemote_c and __IsoAgLib::MeasureProgRemote_c / IsoAgLib::iMeasureProgRemote_c
  as member for doing setpoint and measurement specific jobs.
  \n
  Projects which use this class, must include the files and succeding subdirectories
  of the following subdirectories of xgpl_src/IsoAgLib/Process:
  - \e Remote/StdMeasureElements,
  - \e Remote/StdSetpointElements,
  - \e StdMeasureElements,
  - \e StdSetpointElements and
  - \e impl
  \n
  Example:
  \code
  // define local device type
  IsoAgLib::DevKey c_myDevKey( 1, 0 );
  // creation of process data instance
  iProcDataRemote_c c_workState;
  // init for LIS=0, remote device type/subtype=5/0, complete work width=0xFF,
  // target process data/PRI=2, pointer to my local device type ( to resolve dynamic SA at time of cmd send )
  c_workState.init( 0, IsoAgLib::iDevKey_c( 0x5, 0 ), 0x5, 0x0, 0xFF, 2, IsoAgLib::iDevKey_c( 0x5, 0 ), &c_myDevKey );

  // request current measurement value ( real value, which can differ from commanded value ); triger update request
  int lastReceivedMeasureState = c_workState.masterMeasurementVal( true );
  // start a measuring program
  // a) trigger value update every 1000 msec.
  c_workState.prog().addSubprog(Proc_c::TimeProp, 1000);
  // b) start measure program: trigger send of current measure value ( and not MIN/MAX/AVG/ etc. )
  c_workState.prog().start(Proc_c::Target, Proc_c::TimeProp, Proc_c::DoVal);

  // request current setpoint value ( parameter true -> send request to remote ECU, false -> just deliver last received value )
  int lastReceivedState = c_workState.setpointMasterVal( false );
  // set a setpoint for remote data ( SA / nr of sending and receiving node is derived at time of send
  // -> process data can be initialized even if both ECU are not yet claimed address on BUS )
  c_workState.setSetpointMasterVal( 100 );
  // check if last setpoint set is already answered
  if ( c_workState.setpoint().answered() )
  { // check if it was accepted or rejected
    if ( c_workState.setpoint().accepted() )
    { // happy!
    }
    // otherwise check which other ECU is currently the master, and retrieve the master setpoint
    else if ( c_workState.setpoint().existMaster() )
    { // now retrieve current master setpoint
      int masterSetpoint = c_workState.setpoint().master().exact();
    }
  }
  // check if my current setpoint wish is valid in regard to the current real measurement value
  if ( c_workState.setpoint().valid() )
  { // happy! - even if we're not master, our wished value is delivered at the moment
  }
  // independent of the current master setpoint ( local or other remote ECU )
  // - check if current master setpoint is exact mapped by real measurement data
  if ( c_workState.setpoint().masterValid() )
  { // the master setpoint value is currently mapped
  }
  // if I'm the master setpoint commander, I should release the setpoint explicitly if the
  // controlo is not needed any more
  if ( c_workState.setpoint().master().devKey() == c_myDevKey )
  { // I'm the master -> release control
    c_workState.setpoint().releaseMaster();
  }
  \endcode

  @author Dipl.-Inform. Achim Spangler
*/
class iProcDataRemote_c : private __IsoAgLib::ProcDataRemote_c {

private:
  friend class EventSource_c;
  // iProcess_c::addDDI2ExistingProcData() does a static_cast __IsoAgLib::ProcDataRemote_c => iProcDataRemote_c
  // => access is needed
  friend class iProcess_c;

public:

  /**
    constructor which can set all element vars
    ISO parameter
    @param ps_elementDDI optional pointer to array of structure IsoAgLib::ElementDDI_s which contains DDI, element, isSetpoint and ValueGroup
                         (array is terminated by ElementDDI_s.ui16_element == 0xFFFF)

    DIN parameter
    @param rui8_lis optional LIS code of this instance
    @param rui8_wert optional WERT code of this instance
    @param rui8_inst optional INST code of this instance
    @param rui8_zaehlnum optional ZAEHLNUM code of this instance

    common parameter
    @param rc_devKey optional DEV_KEY code of this instance
    @param rui8_pri PRI code of messages with this process data instance (default 2)
    @param rc_ownerDevKey optional DEV_KEY of the owner
    @param rpc_commanderDevKey pointer to updated DEV_KEY variable of commander
    @param rpc_processDataChangeHandler optional pointer to handler class of application
    @param ri_singletonVecKey optional key for selection of IsoAgLib instance (default 0)
  */
  iProcDataRemote_c(
#ifdef USE_ISO_11783
                    const ElementDDI_s* ps_elementDDI = NULL,
                    uint16_t rui16_element = 0xFFFF,
#endif
#ifdef USE_DIN_9684
                    uint8_t rui8_lis = 0,
                    uint8_t rui8_wert = 0,
                    uint8_t rui8_inst = 0,
                    uint8_t rui8_zaehlnum = 0xFF,
#endif
                    const iDevKey_c& rc_devKey = iDevKey_c::DevKeyInitialProcessData,
                    uint8_t rui8_pri = 2,
                    const iDevKey_c& rc_ownerDevKey = iDevKey_c::DevKeyUnspecified,
                    const iDevKey_c* rpc_commanderDevKey = NULL,
                    ProcessDataChangeHandler_c *rpc_processDataChangeHandler = NULL,
                    int ri_singletonVecKey = 0)
  : ProcDataRemote_c(
#ifdef USE_ISO_11783
                     ps_elementDDI,
                     rui16_element,
#endif
#ifdef USE_DIN_9684
                     rui8_lis, rui8_wert, rui8_inst, rui8_zaehlnum,
#endif
                     rc_devKey, rui8_pri, rc_ownerDevKey, rpc_commanderDevKey, rpc_processDataChangeHandler, ri_singletonVecKey){};


#ifdef USE_ISO_11783
  /**
    constructor which can set all element vars
    alternative parameter list (ISO only)
    @param rui16_DDI
    @param rui16_element
    ...
  */
  iProcDataRemote_c(
                    uint16_t rui16_DDI,
                    uint16_t rui16_element,
#ifdef USE_DIN_9684
                    uint8_t rui8_lis = 0,
                    uint8_t rui8_wert = 0,
                    uint8_t rui8_inst = 0,
                    uint8_t rui8_zaehlnum = 0xFF,
#endif
                    const iDevKey_c& rc_devKey = iDevKey_c::DevKeyInitialProcessData,
                    uint8_t rui8_pri = 2,
                    const iDevKey_c& rc_ownerDevKey = iDevKey_c::DevKeyUnspecified,
                    const iDevKey_c* rpc_commanderDevKey = NULL,
                    ProcessDataChangeHandler_c *rpc_processDataChangeHandler = NULL,
                    int ri_singletonVecKey = 0)
  : ProcDataRemote_c(
                     NULL, rui16_element,
#ifdef USE_DIN_9684
                     rui8_lis, rui8_wert, rui8_inst, rui8_zaehlnum,
#endif
                     rc_devKey, rui8_pri, rc_ownerDevKey, rpc_commanderDevKey, rpc_processDataChangeHandler, ri_singletonVecKey)

  {

    const ElementDDI_s s_tmpElementDDI[2] =
    {
      // if this constructor is used => only exact measurement possible
      {rui16_DDI, false, GeneralCommand_c::exactValue},
      {0xFFFF, false, GeneralCommand_c::noValue}
    };

   ProcDataRemote_c::init(
                          s_tmpElementDDI,
                          rui16_element,
#ifdef USE_DIN_9684
                          rui8_lis, rui8_wert, rui8_inst, rui8_zaehlnum,
#endif
                          rc_devKey, rui8_pri, rc_ownerDevKey, rpc_commanderDevKey, rpc_processDataChangeHandler, ri_singletonVecKey);

  };
#endif


  /**
    initialise this ProcDataRemote_c instance to a well defined initial state
    ISO parameter
    @param ps_elementDDI optional pointer to array of structure IsoAgLib::ElementDDI_s which contains DDI, element, isSetpoint and ValueGroup
                         (array is terminated by ElementDDI_s.ui16_element == 0xFFFF)

    DIN parameter
    @param rui8_lis optional LIS code of this instance
    @param rui8_wert optional WERT code of this instance
    @param rui8_inst optional INST code of this instance
    @param rui8_zaehlnum optional ZAEHLNUM code of this instance

    common parameter
    @param rc_devKey optional DEV_KEY code of this instance
    @param rui8_pri PRI code of messages with this process data instance (default 2)
    @param rc_ownerDevKey optional DEV_KEY of the owner
    @param rpc_commanderDevKey pointer to updated DEV_KEY variable of commander
    @param rpc_processDataChangeHandler optional pointer to handler class of application
    @param ri_singletonVecKey optional key for selection of IsoAgLib instance (default 0)
  */
  void init(
#ifdef USE_ISO_11783
            const ElementDDI_s* ps_elementDDI,
            uint16_t rui16_element,
#endif
#ifdef USE_DIN_9684
            uint8_t rui8_lis = 0,
            uint8_t rui8_wert = 0,
            uint8_t rui8_inst = 0,
            uint8_t rui8_zaehlnum = 0xFF,
#endif
            const iDevKey_c& rc_devKey = iDevKey_c::DevKeyInitialProcessData,
            uint8_t rui8_pri = 2,
            const iDevKey_c& rc_ownerDevKey = iDevKey_c::DevKeyUnspecified,
            const iDevKey_c* rpc_commanderDevKey = NULL,
            ProcessDataChangeHandler_c *rpc_processDataChangeHandler = NULL,
            int ri_singletonVecKey = 0)
   {ProcDataRemote_c::init(
#ifdef USE_ISO_11783
                           ps_elementDDI,
                           rui16_element,
#endif
#ifdef USE_DIN_9684
                           rui8_lis, rui8_wert, rui8_inst, rui8_zaehlnum,
#endif
                           rc_devKey, rui8_pri, rc_ownerDevKey, rpc_commanderDevKey, rpc_processDataChangeHandler, ri_singletonVecKey);};

#ifdef USE_ISO_11783
  /**
    ISO only: initialise this ProcDataRemote_c instance to a well defined initial state
              this alternative uses DDI and element number as parameter and not pointer to list of ElementDDI_s
    ISO parameter
    @param rui16_DDI
    @param rui16_element

    DIN parameter
    @param rui8_lis optional LIS code of this instance
    @param rui8_wert optional WERT code of this instance
    @param rui8_inst optional INST code of this instance
    @param rui8_zaehlnum optional ZAEHLNUM code of this instance

    common parameter
    @param rc_devKey optional DEV_KEY code of this instance
    @param rui8_pri PRI code of messages with this process data instance (default 2)
    @param rc_ownerDevKey optional DEV_KEY of the owner
    @param rpc_commanderDevKey pointer to updated DEV_KEY variable of commander
    @param rpc_processDataChangeHandler optional pointer to handler class of application
    @param ri_singletonVecKey optional key for selection of IsoAgLib instance (default 0)
  */
  void init(
            uint16_t rui16_DDI,
            uint16_t rui16_element,
#ifdef USE_DIN_9684
            uint8_t rui8_lis = 0,
            uint8_t rui8_wert = 0,
            uint8_t rui8_inst = 0,
            uint8_t rui8_zaehlnum = 0xFF,
#endif
            const iDevKey_c& rc_devKey = iDevKey_c::DevKeyInitialProcessData,
            uint8_t rui8_pri = 2,
            const iDevKey_c& rc_ownerDevKey = iDevKey_c::DevKeyUnspecified,
            const iDevKey_c* rpc_commanderDevKey = NULL,
            ProcessDataChangeHandler_c *rpc_processDataChangeHandler = NULL,
            int ri_singletonVecKey = 0)
  {
     const ElementDDI_s s_tmpElementDDI[2] =
     {
       // if this init is used => only exact measurement possible
       {rui16_DDI, false, GeneralCommand_c::exactValue},
       {0xFFFF, false, GeneralCommand_c::noValue}
     };

     ProcDataRemote_c::init(
                            s_tmpElementDDI,
                            rui16_element,
#ifdef USE_DIN_9684
                            rui8_lis, rui8_wert, rui8_inst, rui8_zaehlnum,
#endif
                            rc_devKey, rui8_pri, rc_ownerDevKey, rpc_commanderDevKey, rpc_processDataChangeHandler, ri_singletonVecKey);
  };
#endif

  /** set the poitner to the handler class
    * @param rpc_processDataChangeHandler pointer to handler class of application
    */
  void setProcessDataChangeHandler( ProcessDataChangeHandler_c *rpc_processDataChangeHandler )
   { ProcDataRemote_c::setProcessDataChangeHandler( rpc_processDataChangeHandler ); } ;
  /** deliver the poitner to the handler class
    * @return pointer to handler class of application (or NULL if not defined by application)
    */
  IsoAgLib::ProcessDataChangeHandler_c* getProcessDataChangeHandler( void ) const
   { return ProcDataRemote_c::getProcessDataChangeHandler(); } ;

  /**
    deliver value PRI of messages with this
    process data instance
    @return PRI
  */
  uint8_t pri() const{return ProcDataRemote_c::pri();};
  /**
    set value PRI of messages with this
    process data instance (default value is 2 == target message)
    @param rb_val new PRI value
  */
  void setPri(uint8_t rb_val){ProcDataRemote_c::setPri(rb_val);};
  /**
    deliver value LIS (list number)
    @return LIS
  */
  uint8_t lis() const{return ProcDataRemote_c::lis();};
  /**
    deliver value DEVCLASS (machine type specific table of process data types)
    @return DEVCLASS
  */
  uint8_t devClass() const{return ProcDataRemote_c::devClass();};
  /**
    deliver value WERT (row of process data table)
    @return WERT
  */
  uint8_t wert() const{return ProcDataRemote_c::wert();};
  /**
    deliver value INST (column of process data table)
    @return INST
  */
  uint8_t inst() const{return ProcDataRemote_c::inst();};
  /**
    deliver value ZAEHLNUM (0xFF == whole working width; else parts of width)
    @return ZAEHLNUM
  */
  uint8_t zaehlnum() const{return ProcDataRemote_c::zaehlnum();};
  /**
    deliver value _instance_ (important if more than one machine with equal _device_class_ are active)
    @return POS
  */
  uint8_t devClassInst() const{return ProcDataRemote_c::devClassInst();};

#ifdef USE_ISO_11783
  /** check if this ProcIdent_c has the given DDI as element */
  bool hasDDI( uint16_t rui16_checkDDI ) const { return ProcIdent_c::hasDDI( rui16_checkDDI );};
  /**
    deliver elementDDI list
    @return std::list<ElementDDI_s>
  */
  const std::list<ElementDDI_s>& elementDDI()const {return ProcDataRemote_c::elementDDI();};
  /**
    deliver value DDI (only possible if only one elementDDI in list)
    @return DDI
  */
  uint16_t DDI() const{return ProcDataRemote_c::DDI();};
  /**
    deliver value element (only possible if only one elementDDI in list)
    @return element
  */
  uint16_t element() const{return ProcDataRemote_c::element();};
  /**
    deliver DDI from last received can pkg
    @return DDI
  */
  uint16_t getDDIfromCANPkg() const { return ProcDataBase_c::getDDIfromCANPkg();};

#endif

  /**
    deliver a reference to the setpoint object
    @return reference to member object with setpoint informations
  */
  iSetpointRemote_c& setpoint()
    {return static_cast<iSetpointRemote_c&>(ProcDataRemote_c::setpoint());};
  /* ******************************************** *
   * some convenience functions for master        *
   * setpoint access with NOT simple setpoint      *
   * ******************************************** */
  /**
    deliver the actual master setpoint
    @param rb_sendRequest true -> send request for actual value
    @return setpoint value as long
  */
  int32_t setpointMasterVal(bool rb_sendRequest = false) const
    {return ProcDataRemote_c::setpointMasterVal(rb_sendRequest);};
  /**
    send a setpoint cmd with given exact setpoint
    @param ri32_val commanded setpoint value as long
  */
  void setSetpointMasterVal(int32_t ri32_val)
    {ProcDataRemote_c::setSetpointMasterVal(ri32_val);};
  #ifdef USE_FLOAT_DATA_TYPE
  /**
    deliver the actual master setpoint
    @param rb_sendRequest true -> send request for actual value
    @return setpoint value as float
  */
  float setpointMasterValFloat(bool rb_sendRequest = false)
    {return ProcDataRemote_c::setpointMasterValFloat(rb_sendRequest);};
  /**
    send a setpoint cmd with given exact setpoint
    @param rf_val commanded setpoint value as float
  */
  void setSetpointMasterVal(float rf_val)
    {ProcDataRemote_c::setSetpointMasterVal(rf_val);};
  #endif
  /**
    deliver a reference to the measure prog object
    @return reference to member object with measure prog informations
  */
  iMeasureProgRemote_c& prog()
    {return static_cast<iMeasureProgRemote_c&>(ProcDataRemote_c::prog());};
  /**
    deliver actual measurement value as long
    @param rb_sendRequest true -> request for new value is sent (optional, default false)
  */
  int32_t masterMeasurementVal(bool rb_sendRequest = false)
    {return ProcDataRemote_c::masterMeasurementVal(rb_sendRequest);};
  /**
    send reset cmd for the measurement value
  */
  void resetMasterVal(){ProcDataRemote_c::resetMasterVal();};
  #ifdef USE_FLOAT_DATA_TYPE
  /**
    deliver actual measurement value as float
    @param rb_sendRequest true -> request for new value is sent (optional, default false)
  */
  float masterValFloat(bool rb_sendRequest = false)
    {return ProcDataRemote_c::masterValFloat(rb_sendRequest);};
  #endif

  /**
    deliver the commanderDevKey (DEV_KEY of local member)
    @return DEV_KEY used for sending commands to remote owner member
  */
  const iDevKey_c& commanderDevKey()const{return static_cast<const iDevKey_c&>(ProcDataRemote_c::commanderDevKey());};
  /**
    set the pointer to the commander ident devKey
    @param rpbdevKey pointer to DEV_KEY var of local member used for
                sending commands to remote owner member
  */
  void setCommanderDevKey(const __IsoAgLib::DevKey_c* rpc_devKey){ProcDataRemote_c::setCommanderDevKey(rpc_devKey);};


  /**
    deliver the central data type of this process data
    @return proc_valType_t: i32_val, ui32_val, float_val, cmdVal
  */
  proc_valType_t valType()const
    {return ProcDataRemote_c::valType();};
  /**
    set the central data type of this process data
    @return proc_valType_t: i32_val, ui32_val, float_val, cmdVal
  */
  void setValType(proc_valType_t ren_procValType)
    {ProcDataRemote_c::setValType(ren_procValType);};

};

}
#endif
