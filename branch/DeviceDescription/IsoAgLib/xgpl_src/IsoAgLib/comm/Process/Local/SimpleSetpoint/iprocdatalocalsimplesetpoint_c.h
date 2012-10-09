/***************************************************************************
                          iprocdatalocalsimplesetpoint_c.h  -
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
#ifndef IPROC_DATA_LOCAL_SIMPLESETPOINT_H
#define IPROC_DATA_LOCAL_SIMPLESETPOINT_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include "../SimpleSetpointElements/isimplemanagesetpointlocal_c.h"
#include "impl/procdatalocalsimplesetpoint_c.h"
#include "../StdMeasureElements/imeasureproglocal_c.h"
#include "../../proc_c.h"

// Begin Namespace IsoAgLib
namespace IsoAgLib {
// ***************************************************************************************
// ****************** ProcDataLocalSimpleSetpoint_c ******************
// ***************************************************************************************

/**
  @brief Interface for managing of local process data object with restricted setpoint
  management ( %i.e. set setpoint, without handling of acceptance, several concurring
  setpoint commanding ECUs ).

  Has __IsoAgLib::SimpleManageSetpointLocal_c / IsoAgLib::iSimpleManageSetpointLocal_c and
  __IsoAgLib::ManageMeasureProgLocal_c / IsoAgLib::iManageMeasureProgLocal_c
  as member for doing setpoint and measurement specific jobs.
  \n
  Projects which use this class, must include the files and succeding subdirectories
  of the following subdirectories of xgpl_src/IsoAgLib/Process:
  - \e Local/StdMeasureElements,
  - \e Local/SimpleSetpointElements,
  - \e StdMeasureElements and
  - \e impl
  \n
  Example:
  \code
  // define local device type
  IsoAgLib::GetyPos c_myGtp( 1, 0 );
  // creation of process data instance
  iProcDataLocalSimpleSetpoint_c c_workState;
  // init for LIS=0, local device type/subtype=1/0, complete work width=0xFF,
  // target process data/PRI=2, pointer to my local device type ( to resolve dynamic SA at time of cmd send ),
  // load/store measurememnt data to/from EEPROM
  c_workState.init( 0, myGtp, 0x1, 0x0, 0xFF, 2, c_myGtp, &c_myGtp, false, 0x1234 );

  // update current measurement value ( real value, which can differ from commanded value )
  c_workState.setMasterVal( 100 );
  // measurement programs and value requests are handled in the backend of the IsoAgLibrary

  // simply access received setpoints ( without facility to reject wrong values )
  int newVal = c_workState.setpointMasterVal();
  // set a different setpoint as the current "valid" setpoint
  // -> if other ECU requests current setpoint, it will recieve the new value as answer
  c_workState.setSetpointMasterVal( 100 );
  \endcode

  @author Dipl.-Inform. Achim Spangler
*/
class iProcDataLocalSimpleSetpoint_c
: private __IsoAgLib::ProcDataLocalSimpleSetpoint_c  {
private:
  friend class EventSource_c;
public:
  /**
    constructor which can set all element vars

    possible errors:
        * Err_c::badAlloc not enough memory to insert first  MeasureProgLocal
    ISO parameter
    @param rui16_DDI optional DDI code of this instance
    @param rui16_element optional Element code of this instance

    DIN parameter
    @param rui8_lis optional LIS code of this instance
    @param rui8_wert optional WERT code of this instance
    @param rui8_inst optional INST code of this instance
    @param rui8_zaehlnum optional ZAEHLNUM code of this instance

    @param rc_gtp optional GETY_POS code of Process-Data
    @param rui8_pri PRI code of messages with this process data instance (default 2)
    @param rc_ownerGtp optional GETY_POS of the owner
    @param rpc_gtp pointer to updated GETY_POS variable of owner
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
  iProcDataLocalSimpleSetpoint_c(
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
                                 iGetyPos_c rc_gtp = iGetyPos_c(0, 0xF), 
                                 uint8_t rui8_pri = 2,
                                 iGetyPos_c rc_ownerGtp = iGetyPos_c(0xF, 0xF),
                                 iGetyPos_c *rpc_gtp = NULL,
                                 bool rb_cumulativeValue = false,
#ifdef USE_EEPROM_IO
                                 uint16_t rui16_eepromAdr = 0xFFFF,
#endif
                                 ProcessDataChangeHandler_c *rpc_processDataChangeHandler = NULL,
                                 int ri_singletonVecKey = 0
  ) : ProcDataLocalSimpleSetpoint_c(
#ifdef USE_ISO_11783
                                   rui16_DDI, rui16_element,
#endif
#ifdef USE_DIN_9684
                                   rui8_lis, rui8_wert, rui8_inst, rui8_zaehlnum,
#endif
                                   rc_gtp, rui8_pri, rc_ownerGtp, rpc_gtp, rb_cumulativeValue,
#ifdef USE_EEPROM_IO
                                   rui16_eepromAdr,
#endif
                                   rpc_processDataChangeHandler,
                                   ri_singletonVecKey) {};

  /**
    initialise this ProcDataLocalSimpleSetpoint_c
    instance to a well defined initial state

    possible errors:
        * Err_c::badAlloc not enough memory to insert first  MeasureProgLocal
    ISO parameter
    @param rui16_DDI optional DDI code of this instance
    @param rui16_element optional Element code of this instance

    DIN parameter
    @param rui8_lis optional LIS code of this instance
    @param rui8_wert optional WERT code of this instance
    @param rui8_inst optional INST code of this instance
    @param rui8_zaehlnum optional ZAEHLNUM code of this instance

    @param rc_gtp optional GETY_POS code of Process-Data
    @param rui8_pri PRI code of messages with this process data instance (default 2)
    @param rc_ownerGtp optional GETY_POS of the owner
    @param rpc_gtp pointer to updated GETY_POS variable of owner
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
            iGetyPos_c rc_gtp = iGetyPos_c(0, 0xF), 
            uint8_t rui8_pri = 2, 
            iGetyPos_c rc_ownerGtp = iGetyPos_c(0xF, 0xF),
            iGetyPos_c *rpc_gtp = NULL, 
            bool rb_cumulativeValue = false,
#ifdef USE_EEPROM_IO
            uint16_t rui16_eepromAdr = 0xFFFF,
#endif
            ProcessDataChangeHandler_c *rpc_processDataChangeHandler = NULL,
            int ri_singletonVecKey = 0
            ) 
  {ProcDataLocalSimpleSetpoint_c::init(
#ifdef USE_ISO_11783
                                       rui16_DDI, rui16_element,
#endif
#ifdef USE_DIN_9684
                                       rui8_lis, rui8_wert, rui8_inst, rui8_zaehlnum,
#endif
                                       rc_gtp, rui8_pri, rc_ownerGtp, rpc_gtp, rb_cumulativeValue,
#ifdef USE_EEPROM_IO
                                       rui16_eepromAdr,
#endif
                                       rpc_processDataChangeHandler,
                                       ri_singletonVecKey);
  };

  /** set the poitner to the handler class
    * @param rpc_processDataChangeHandler pointer to handler class of application
    */
  void setProcessDataChangeHandler( ProcessDataChangeHandler_c *rpc_processDataChangeHandler )
   { ProcDataLocalSimpleSetpoint_c::setProcessDataChangeHandler( rpc_processDataChangeHandler ); } ;
  /** deliver the poitner to the handler class
    * @return pointer to handler class of application (or NULL if not defined by application)
    */
  IsoAgLib::ProcessDataChangeHandler_c* getProcessDataChangeHandler( void ) const
   { return ProcDataLocalSimpleSetpoint_c::getProcessDataChangeHandler(); } ;

  /**
    deliver value PRI of messages with this
    process data instance
    @return PRI
  */
  uint8_t pri() const{return ProcDataLocalSimpleSetpoint_c::pri();};
  /**
    set value PRI of messages with this
    process data instance (default value is 2 == target message)
    @param rb_val new PRI value
  */
  void setPri(uint8_t rb_val){ProcDataLocalSimpleSetpoint_c::setPri(rb_val);};
  /**
    deliver value LIS (list number)
    @return LIS
  */
  uint8_t lis() const{return ProcDataLocalSimpleSetpoint_c::lis();};
  /**
    deliver value GETY (machine type specific table of process data types)
    @return GETY
  */
  uint8_t gety() const{return ProcDataLocalSimpleSetpoint_c::gety();};
  /**
    deliver value GETY_POS (machine type specific table of process data types)
    use everytime the GETY from the ident part, and take the POS from the owner
    @return GETY_POS
  */
  iGetyPos_c gtp() const {return ProcDataLocalSimpleSetpoint_c::gtp();};
  /**
    deliver value WERT (row of process data table)
    @return WERT
  */
  uint8_t wert() const{return ProcDataLocalSimpleSetpoint_c::wert();};
  /**
    deliver value INST (column of process data table)
    @return INST
  */
  uint8_t inst() const{return ProcDataLocalSimpleSetpoint_c::inst();};
  /**
    deliver value ZAEHLNUM (0xFF == whole working width; else parts of width)
    @return ZAEHLNUM
  */
  uint8_t zaehlnum() const{return ProcDataLocalSimpleSetpoint_c::zaehlnum();};
  /**
    deliver value POS (important if more than one machine with equal GETY are active)
    @return POS
  */
  uint8_t pos() const{return ProcDataLocalSimpleSetpoint_c::pos();};


  /**
    send the base value (the one that is updated via setMasterVal() )
    to a specified target (selected by GPT)
    @param rc_targetGtp GetyPos of target
    @param ren_type optional PRI specifier of the message (default Proc_c::Target )
    @return true -> successful sent
  */
  bool sendVal( iGetyPos_c rc_targetGtp, Proc_c::progType_t ren_progType = Proc_c::Target ) const
    { return ProcDataLocalSimpleSetpoint_c::sendVal( rc_targetGtp, ren_progType );};
  /**
    send a exact-setpoint to a specified target (selected by GPT)
    @param rc_targetGtp GetyPos of target
    @param ren_type optional PRI specifier of the message (default Proc_c::Target )
    @return true -> successful sent
  */
  bool sendSetpoint( iGetyPos_c rc_targetGtp, Proc_c::progType_t ren_progType = Proc_c::Target ) const
   { return setpointConst().sendSetpoint( rc_targetGtp, ren_progType );};
  /**
    send a sub-setpoint (selected by MOD) to a specified target (selected by GPT)
    @param rui8_mod select sub-type of setpoint
    @param rc_targetGtp GetyPos of target
    @param ren_type optional PRI specifier of the message (default Proc_c::Target )
    @return true -> successful sent
  */
  bool sendSetpointMod( uint8_t rui8_mod, iGetyPos_c rc_targetGtp, Proc_c::progType_t ren_progType = Proc_c::Target ) const
   {
     // @todo: enable value groups in interface?
     __IsoAgLib::GeneralCommand_c::ValueGroup_t en_valueGroup;
     switch (rui8_mod) {
       case 2: en_valueGroup = __IsoAgLib::GeneralCommand_c::minValue; break;
       case 3: en_valueGroup = __IsoAgLib::GeneralCommand_c::maxValue; break;
       case 0:
       default:
         en_valueGroup = __IsoAgLib::GeneralCommand_c::exactValue; break;
     }
     return setpointConst().sendSetpointMod(rc_targetGtp, ren_progType, en_valueGroup, __IsoAgLib::GeneralCommand_c::setValue );
   };

  #ifdef USE_EEPROM_IO
  /**
    deliver the eeprom adr for the value
    @return configured EEPROM adress
  */
  uint16_t eepromAdr()const{return ProcDataLocalSimpleSetpoint_c::eepromAdr();};
  /**
    set the eeprom adr for the value, read in value from EEPROM

    possible errors:
        * dependent error in EEPROMIO_c on problems during read
    @param rui16_eepromAdr new EEPROM adress
  */
  void setEepromAdr(uint16_t rui16_eepromAdr)
    {ProcDataLocalSimpleSetpoint_c::setEepromAdr(rui16_eepromAdr);};
  #endif
  /**
    deliver the master value (central measure value of this process data;
    can differ from measure vals of measure progs, as these can be reseted
    independent)
    @return actual master value
  */
  const int32_t& masterVal()const{return ProcDataLocalSimpleSetpoint_c::masterVal();};
  /**
    set the masterVal from main application independent from any measure progs
    @param ri32_val new measure value
  */
  void setMasterVal(int32_t ri32_val)
    {ProcDataLocalSimpleSetpoint_c::setMasterVal(ri32_val);};
  /**
    set the masterVal from main application independent from any measure progs
    @param ri32_val new measure value
  */
  void setMasterVal(int16_t ri16_val)
    {ProcDataLocalSimpleSetpoint_c::setMasterVal((int32_t)ri16_val);};
  /**
    set the masterVal from main application independent from any measure progs
    @param ri32_val new measure value
  */
  void setMasterVal(uint8_t rb_val)
    {ProcDataLocalSimpleSetpoint_c::setMasterVal((int32_t)rb_val);};
  /**
    increment the value -> update the local and the measuring programs values
    @param ri32_val size of increment of master value
  */
  void incrMasterVal(int32_t ri32_val)
    {ProcDataLocalSimpleSetpoint_c::incrMasterVal(ri32_val);};
#ifdef USE_FLOAT_DATA_TYPE
  /**
    deliver the master value (central measure value of this process data;
    can differ from measure vals of measure progs, as these can be reseted
    independent) as float value
    @return actual master value
  */
  const float& masterValFloat()const
    {return ProcDataLocalSimpleSetpoint_c::masterValFloat();};
  /**
    set the masterVal from main application independent from any measure progs
    @param rf_val new measure value
  */
  void setMasterVal(float rf_val)
    {ProcDataLocalSimpleSetpoint_c::setMasterVal(rf_val);};
  /**
    increment the value -> update the local and the measuring programs values
    @param rf_val size of increment of master value
  */
  void incrMasterVal(float rf_val)
    {ProcDataLocalSimpleSetpoint_c::incrMasterVal(rf_val);};
#endif


  /**
    deliver the internal unit code, which can be requested by remote ECU
    (important if process data is local managed with different unit than published via ISO11783 or DIN9684
    BUS; interesting if value update is easier with special unit)
    @return internal unit
  */
  uint16_t internalUnit()const{return ProcDataLocalSimpleSetpoint_c::internalUnit();};
  /**
    set the internal unit code, which can be requested by remote ECU
    (important if process data is local managed with different unit than published via ISO11783 or DIN9684
    BUS; interesting if value update is easier with special unit)
    @param rui16_internalUnit new vaue for internal unit
  */
  void setInternalUnit(uint16_t rui16_internalUnit)
      {ProcDataLocalSimpleSetpoint_c::setInternalUnit(rui16_internalUnit);};

  /**
    deliver the central data type of this process data
    @return proc_valType_t: i32_val, ui32_val, float_val, cmdVal
  */
  proc_valType_t valType()const
    {return ProcDataLocalSimpleSetpoint_c::valType();};
  /**
    set the central data type of this process data
    @return proc_valType_t: i32_val, ui32_val, float_val, cmdVal
  */
  void setValType(proc_valType_t ren_procValType)
    {ProcDataLocalSimpleSetpoint_c::setValType(ren_procValType);};


  /**
    retreive simple master setpoint
    @return actual received setpoint value
  */
  int32_t setpointMasterVal() const
    { return ProcDataLocalSimpleSetpoint_c::setpointConst().setpointMasterVal();};
    #ifdef USE_FLOAT_DATA_TYPE
  /**
    retreive simple master setpoint
    @return actual received setpoint value
  */
  float setpointMasterValFloat() const
    { return ProcDataLocalSimpleSetpoint_c::setpointConst().setpointMasterValFloat();};
    #endif
  /**
    set the setpoint value
    @param ri32_val new setpoint value
  */
  void setSetpointMasterVal(int32_t ri32_val)
    { ProcDataLocalSimpleSetpoint_c::setpoint().setSetpointMasterVal(ri32_val);};
  #ifdef USE_FLOAT_DATA_TYPE
  /**
    set the setpoint value as float value
    @param rf_val new setpoint value
  */
  void setSetpointMasterVal(float rf_val)
    { ProcDataLocalSimpleSetpoint_c::setpoint().setSetpointMasterVal(rf_val);};
  #endif

   /**
    check if specific measureprog exist
    @param rui8_pri PRI code of searched measure program
    @param rc_gtp GETY code of searched measure program
    @return true -> found item
  */
  bool existProg(uint8_t rui8_pri, iGetyPos_c rc_gtp)
      {return ProcDataLocalSimpleSetpoint_c::existProg(rui8_pri, rc_gtp);};

  /**
    search for suiting measureprog, if not found AND if rb_doCreate == true
    create copy from first element at end of vector

    possible errors:
        * Err_c::elNonexistent wanted measureprog doesn't exist and rb_doCreate == false
    @param rui8_pri PRI code of searched measure program
    @param rc_gtp GETY code of searched measure program
    @param rb_doCreated true -> create suitable measure program if not found
  */
  iMeasureProgLocal_c& prog(uint8_t rui8_pri, iGetyPos_c rc_gtp, bool rb_doCreate)
    { return static_cast<iMeasureProgLocal_c&>(ProcDataLocalSimpleSetpoint_c::prog(rui8_pri, rc_gtp, rb_doCreate));};

  /** deliver reference to setpoint */
  iSimpleManageSetpointLocal_c& setpoint( void )
    { return static_cast<iSimpleManageSetpointLocal_c&>
             (ProcDataLocalSimpleSetpoint_c::setpoint());
    };

};

}
#endif
