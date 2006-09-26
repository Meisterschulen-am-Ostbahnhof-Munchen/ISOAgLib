/***************************************************************************
                          iprocdatalocalsimplesetpointsimplemeasure_c.h  -
                             managing of local process data object
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
#ifndef IPROC_DATA_LOCAL_SIMPLESETPOINT_SIMPLEMEASURE_H
#define IPROC_DATA_LOCAL_SIMPLESETPOINT_SIMPLEMEASURE_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include "impl/procdatalocalsimplesetpointsimplemeasure_c.h"
#include "../SimpleSetpointElements/isimplemanagesetpointlocal_c.h"
#include "../../proc_c.h"
#include <IsoAgLib/comm/SystemMgmt/ISO11783/iisoname_c.h>
#include "../../generalcommand_c.h"

// Begin Namespace IsoAgLib
namespace IsoAgLib {
// ***************************************************************************************
// ****************** ProcDataLocalSimpleSetpointSimpleMeasure_c ******************
// ***************************************************************************************

/**
  @brief Interface for managing of local process data object with restricted measurement
  and setpoint management ( %i.e. only one-shot measurement requests;
  set setpoint, without handling of acceptance, several concurring setpoint commanding ECUs ).

  Has __IsoAgLib::SimpleManageSetpointLocal_c / IsoAgLib::iSimpleManageSetpointLocal_c
  as member for doing setpoint and measurement specific jobs.
  \n
  Projects which use this class, must include the files and succeding subdirectories
  of the following subdirectories of xgpl_src/IsoAgLib/Process:
  - \e Local/SimpleSetpointElements and
  - \e impl
  \n
  Example:
  \code
  // define local device type
  IsoAgLib::ISOName c_myISOName( 1, 0 );
  // creation of process data instance
  iProcDataLocalSimpleSetpointSimpleMeasure_c c_workState;
  // init for LIS=0, local device type/subtype=1/0, complete work width=0xFF,
  // target process data/PRI=2, pointer to my local device type ( to resolve dynamic SA at time of cmd send ),
  // load/store measurememnt data to/from EEPROM
  c_workState.init( 0, myISOName, 0x1, 0x0, 0xFF, 2, c_myISOName, &c_myISOName, false, 0x1234 );

  // update current measurement value ( real value, which can differ from commanded value )
  c_workState.setMasterMeasurementVal( 100 );
  // backend in IsoAgLib doesn't provide measurement programs ( only single one-shot value requests )

  // simply access received setpoints ( without facility to reject wrong values )
  int newVal = c_workState.setpointMasterVal();
  // set a different setpoint as the current "valid" setpoint
  // -> if other ECU requests current setpoint, it will recieve the new value as answer
  c_workState.setSetpointMasterVal( 100 );
  \endcode

  @author Dipl.-Inform. Achim Spangler
*/
class iProcDataLocalSimpleSetpointSimpleMeasure_c
: private __IsoAgLib::ProcDataLocalSimpleSetpointSimpleMeasure_c  {
private:
  friend class EventSource_c;
public:
  /**
    constructor which can set all element vars

    possible errors:
        * Err_c::badAlloc not enough memory to insert first  MeasureProgLocal
    ISO parameter
    @param ps_elementDDI optional pointer to array of structure IsoAgLib::ElementDDI_s which contains DDI, element, isSetpoint and ValueGroup
                         (array is terminated by ElementDDI_s.ui16_element == 0xFFFF)

    @param rc_isoName optional ISOName code of Process-Data
    @param rui8_pri PRI code of messages with this process data instance (default 2)
    @param rc_ownerISOName optional ISOName of the owner
    @param rpc_isoName pointer to updated ISOName variable of owner
    @param rb_cumulativeValue
             -# for process data like distance, time, area
                 the value of the measure prog data sets is updated
                 on master value update dependent on the value increment
                 since the last master value update
                 -> if a remote member resets his data set copy, datas of
                    other members aren't changed
                 -> if this data is saved in EEPROM, the main application
                    needn't take into account the initial EEPROM value, as
                     setting of the master val is independent from EEPROM
             -#  for values like speed, state, rpm aren't updated by increment,
                  -> the given master value is propagated equally to all
                      measure prog data sets
                  -> if this data is saved in EEPROM, the stored value is loaded
                     as initial master value, and is initially propagated to all
                     measure prog data sets
    @param rui16_eepromAdr optional adress where value is stored in EEPROM
    @param rpc_processDataChangeHandler optional pointer to handler class of application
    @param ri_singletonVecKey optional key for selection of IsoAgLib instance (default 0)
  */
  iProcDataLocalSimpleSetpointSimpleMeasure_c(const IsoAgLib::ElementDDI_s* ps_elementDDI = NULL,
                                              uint16_t rui16_element = 0xFFF,
                                              const iISOName_c& rc_isoName = iISOName_c::ISONameInitialProcessData,
                                              uint8_t rui8_pri = 2,
                                              const iISOName_c& rc_ownerISOName = iISOName_c::ISONameUnspecified,
                                              const iISOName_c *rpc_isoName = NULL,
                                              bool rb_cumulativeValue = false,
#ifdef USE_EEPROM_IO
                                              uint16_t rui16_eepromAdr = 0xFFFF,
#endif
                                              ProcessDataChangeHandler_c *rpc_processDataChangeHandler = NULL,
                                              int ri_singletonVecKey = 0
                                              )
    : ProcDataLocalSimpleSetpointSimpleMeasure_c(ps_elementDDI, rui16_element,
                                                 rc_isoName, rui8_pri, rc_ownerISOName, rpc_isoName, rb_cumulativeValue,
#ifdef USE_EEPROM_IO
                                                 rui16_eepromAdr,
#endif
                                                 rpc_processDataChangeHandler,
                                                 ri_singletonVecKey)
    {}

  /**
    initialise this ProcDataLocalSimpleSetpointSimpleMeasure_c
    instance to a well defined initial state

    possible errors:
        * Err_c::badAlloc not enough memory to insert first  MeasureProgLocal
    ISO parameter
    @param ps_elementDDI optional pointer to array of structure IsoAgLib::ElementDDI_s which contains DDI, element, isSetpoint and ValueGroup
                         (array is terminated by ElementDDI_s.ui16_element == 0xFFFF)

    @param rc_isoName optional ISOName code of Process-Data
    @param rui8_pri PRI code of messages with this process data instance (default 2)
    @param rc_ownerISOName optional ISOName of the owner
    @param rpc_isoName pointer to updated ISOName variable of owner
    @param rb_cumulativeValue
             -# for process data like distance, time, area
                 the value of the measure prog data sets is updated
                 on master value update dependent on the value increment
                 since the last master value update
                 -> if a remote member resets his data set copy, datas of
                    other members aren't changed
                 -> if this data is saved in EEPROM, the main application
                    needn't take into account the initial EEPROM value, as
                     setting of the master val is independent from EEPROM
             -#  for values like speed, state, rpm aren't updated by increment,
                  -> the given master value is propagated equally to all
                      measure prog data sets
                  -> if this data is saved in EEPROM, the stored value is loaded
                     as initial master value, and is initially propagated to all
                     measure prog data sets
    @param rui16_eepromAdr optional adress where value is stored in EEPROM
    @param rpc_processDataChangeHandler optional pointer to handler class of application
    @param ri_singletonVecKey optional key for selection of IsoAgLib instance (default 0)
  */
  void init(const IsoAgLib::ElementDDI_s* ps_elementDDI,
            uint16_t rui16_element,
            const iISOName_c& rc_isoName = iISOName_c::ISONameInitialProcessData,
            uint8_t rui8_pri = 2,
            const iISOName_c& rc_ownerISOName = iISOName_c::ISONameUnspecified,
            const iISOName_c *rpc_isoName = NULL,
            bool rb_cumulativeValue = false,
#ifdef USE_EEPROM_IO
            uint16_t rui16_eepromAdr = 0xFFFF,
#endif
            ProcessDataChangeHandler_c *rpc_processDataChangeHandler = NULL,
            int ri_singletonVecKey = 0
            )
  {ProcDataLocalSimpleSetpointSimpleMeasure_c::init(ps_elementDDI, rui16_element,
                                                    rc_isoName, rui8_pri, rc_ownerISOName, rpc_isoName, rb_cumulativeValue,
#ifdef USE_EEPROM_IO
                                                    rui16_eepromAdr,
#endif
                                                    rpc_processDataChangeHandler,
                                                    ri_singletonVecKey);
  }

  /** set the poitner to the handler class
    * @param rpc_processDataChangeHandler pointer to handler class of application
    */
  void setProcessDataChangeHandler( ProcessDataChangeHandler_c *rpc_processDataChangeHandler )
   { ProcDataLocalSimpleSetpointSimpleMeasure_c::setProcessDataChangeHandler( rpc_processDataChangeHandler ); }

  /** deliver the poitner to the handler class
    * @return pointer to handler class of application (or NULL if not defined by application)
    */
  IsoAgLib::ProcessDataChangeHandler_c* getProcessDataChangeHandler( void ) const
   { return ProcDataLocalSimpleSetpointSimpleMeasure_c::getProcessDataChangeHandler(); }

  /**
    deliver value PRI of messages with this
    process data instance
    @return PRI
  */
  uint8_t pri() const{return ProcDataLocalSimpleSetpointSimpleMeasure_c::pri();}

  /**
    set value PRI of messages with this
    process data instance (default value is 2 == target message)
    @param rb_val new PRI value
  */
  void setPri(uint8_t rb_val){ProcDataLocalSimpleSetpointSimpleMeasure_c::setPri(rb_val);}

  /**
    deliver value LIS (list number)
    @return LIS
  */
  uint8_t lis() const{return ProcDataLocalSimpleSetpointSimpleMeasure_c::lis();}

  /**
    deliver value DEVCLASS (machine type specific table of process data types)
    @return DEVCLASS
  */
  uint8_t devClass() const{return ProcDataLocalSimpleSetpointSimpleMeasure_c::devClass();}

  /**
    deliver value ISOName (machine type specific table of process data types)
    use everytime the _device_class_ from the ident part, and take the _instance_ from the owner
    @return ISOName
  */
  const iISOName_c& isoName() const {return static_cast<const iISOName_c&>(ProcDataLocalSimpleSetpointSimpleMeasure_c::isoName());}

  /**
    deliver value WERT (row of process data table)
    @return WERT
  */
  uint8_t wert() const{return ProcDataLocalSimpleSetpointSimpleMeasure_c::wert();}

  /**
    deliver value INST (column of process data table)
    @return INST
  */
  uint8_t inst() const{return ProcDataLocalSimpleSetpointSimpleMeasure_c::inst();}

  /**
    deliver value ZAEHLNUM (0xFF == whole working width; else parts of width)
    @return ZAEHLNUM
  */
  uint8_t zaehlnum() const{return ProcDataLocalSimpleSetpointSimpleMeasure_c::zaehlnum();}

  /**
    deliver value _instance_ (important if more than one machine with equal _device_class_ are active)
    @return POS
  */
  uint8_t devClassInst() const{return ProcDataLocalSimpleSetpointSimpleMeasure_c::devClassInst();}

  /** check if this ProcIdent_c has the given DDI as element */
  bool hasDDI( uint16_t rui16_checkDDI ) const { return ProcIdent_c::hasDDI( rui16_checkDDI );}

  /**
    deliver value DDI (only possible if only one elementDDI in list)
    @return DDI
   */
  uint16_t DDI() const { return ProcIdent_c::DDI();}

  /**
  deliver value element (only possible if only one elementDDI in list)
  @return element
   */
  uint16_t element() const  { return ProcIdent_c::element();}

  /**
    send the base value (the one that is updated via setMasterMeasurementVal() )
    to a specified target (selected by GPT)
    @param rc_targetISOName ISOName of target
    @param ren_type optional PRI specifier of the message (default Proc_c::Target )
    @return true -> successful sent
  */
  bool sendMasterMeasurementVal( const iISOName_c& rc_targetISOName, Proc_c::progType_t ren_progType = Proc_c::Target ) const
    { return ProcDataLocalSimpleSetpointSimpleMeasure_c::sendMasterMeasurementVal( rc_targetISOName, ren_progType );}

  /**
    send a exact-setpoint to a specified target (selected by GPT)
    @param rc_targetISOName ISOName of target
    @param ren_type optional PRI specifier of the message (default Proc_c::Target )
    @return true -> successful sent
  */
  bool sendMasterSetpointVal( const iISOName_c& rc_targetISOName, Proc_c::progType_t ren_progType = Proc_c::Target ) const
   { return setpointConst().sendMasterSetpointVal( rc_targetISOName, ren_progType );}

  /**
    send a sub-setpoint (selected by MOD) to a specified target (selected by GPT)
    @param rui8_mod select sub-type of setpoint
    @param rc_targetISOName ISOName of target
    @param ren_type optional PRI specifier of the message (default Proc_c::Target )
    @return true -> successful sent
  */
  bool sendSetpointMod( GeneralCommand_c::ValueGroup_t en_valueGroup, const iISOName_c& rc_targetISOName, Proc_c::progType_t ren_progType = Proc_c::Target ) const
   {
     return setpointConst().sendSetpointMod( rc_targetISOName, ren_progType, en_valueGroup, __IsoAgLib::GeneralCommand_c::setValue);
    }

  #ifdef USE_EEPROM_IO
  /**
    deliver the eeprom adr for the value
    @return configured EEPROM adress
  */
  uint16_t eepromAdr()const{return ProcDataLocalSimpleSetpointSimpleMeasure_c::eepromAdr();}

  /**
    set the eeprom adr for the value, read in value from EEPROM

    possible errors:
        * dependent error in EEPROMIO_c on problems during read
    @param rui16_eepromAdr new EEPROM adress
  */
  void setEepromAdr(uint16_t rui16_eepromAdr)
    {ProcDataLocalSimpleSetpointSimpleMeasure_c::setEepromAdr(rui16_eepromAdr);}
  #endif

  /**
    deliver the master value (central measure value of this process data;
    can differ from measure vals of measure progs, as these can be reseted
    independent)
    @return actual master value
  */
  const int32_t& masterMeasurementVal()const{return ProcDataLocalSimpleSetpointSimpleMeasure_c::masterMeasurementVal();}

  /**
    set the masterMeasurementVal from main application independent from any measure progs
    @param ri32_val new measure value
  */
  void setMasterMeasurementVal(int32_t ri32_val)
    {ProcDataLocalSimpleSetpointSimpleMeasure_c::setMasterMeasurementVal(ri32_val);}

  /**
    set the masterMeasurementVal from main application independent from any measure progs
    @param ri32_val new measure value
  */
  void setMasterMeasurementVal(int16_t ri16_val)
    {ProcDataLocalSimpleSetpointSimpleMeasure_c::setMasterMeasurementVal((int32_t)ri16_val);}

  /**
    set the masterMeasurementVal from main application independent from any measure progs
    @param ri32_val new measure value
  */
  void setMasterMeasurementVal(uint8_t rb_val)
    {ProcDataLocalSimpleSetpointSimpleMeasure_c::setMasterMeasurementVal((int32_t)rb_val);}

  /**
    increment the value -> update the local and the measuring programs values
    @param ri32_val size of increment of master value
  */
  void incrMasterMeasurementVal(int32_t ri32_val)
    {ProcDataLocalSimpleSetpointSimpleMeasure_c::incrMasterMeasurementVal(ri32_val);}

#ifdef USE_FLOAT_DATA_TYPE
  /**
    deliver the master value (central measure value of this process data;
    can differ from measure vals of measure progs, as these can be reseted
    independent) as float value
    @return actual master value
  */
  const float& masterValFloat()const
    {return ProcDataLocalSimpleSetpointSimpleMeasure_c::masterValFloat();}

  /**
    set the masterMeasurementVal from main application independent from any measure progs
    @param rf_val new measure value
  */
  void setMasterMeasurementVal(float rf_val)
    {ProcDataLocalSimpleSetpointSimpleMeasure_c::setMasterMeasurementVal(rf_val);}

  /**
    increment the value -> update the local and the measuring programs values
    @param rf_val size of increment of master value
  */
  void incrMasterMeasurementVal(float rf_val)
    {ProcDataLocalSimpleSetpointSimpleMeasure_c::incrMasterMeasurementVal(rf_val);}
#endif

  /**
    deliver the central data type of this process data
    @return proc_valType_t: i32_val, ui32_val, float_val, cmdVal
  */
  proc_valType_t valType()const
    {return ProcDataLocalSimpleSetpointSimpleMeasure_c::valType();}

  /**
    set the central data type of this process data
    @return proc_valType_t: i32_val, ui32_val, float_val, cmdVal
  */
  void setValType(proc_valType_t ren_procValType)
    {ProcDataLocalSimpleSetpointSimpleMeasure_c::setValType(ren_procValType);}

  /**
    retreive simple master setpoint
    @return actual received setpoint value
  */
  int32_t setpointMasterVal() const
    { return ProcDataLocalSimpleSetpointSimpleMeasure_c::setpointConst().setpointMasterVal();}

    #ifdef USE_FLOAT_DATA_TYPE
  /**
    retreive simple master setpoint
    @return actual received setpoint value
  */
  float setpointMasterValFloat() const
    { return ProcDataLocalSimpleSetpointSimpleMeasure_c::setpointConst().setpointMasterValFloat();}
    #endif

  /**
    set the setpoint value
    @param ri32_val new setpoint value
  */
  void setSetpointMasterVal(int32_t ri32_val)
    { ProcDataLocalSimpleSetpointSimpleMeasure_c::setpoint().setSetpointMasterVal(ri32_val);}

  #ifdef USE_FLOAT_DATA_TYPE
  /**
    set the setpoint value as float value
    @param rf_val new setpoint value
  */
  void setSetpointMasterVal(float rf_val)
    { ProcDataLocalSimpleSetpointSimpleMeasure_c::setpoint().setSetpointMasterVal(rf_val);}
  #endif

 /** deliver reference to setpoint */
  iSimpleManageSetpointLocal_c& setpoint( void )
    { return static_cast<iSimpleManageSetpointLocal_c&>
             (ProcDataLocalSimpleSetpointSimpleMeasure_c::setpoint());
    }
};

}
#endif
