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
#include "../../iprocesscmd_c.h"

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
  IsoAgLib::iIsoName c_myIsoName( 1, 0 );
  // creation of process data instance
  iProcDataLocalSimpleSetpoint_c c_workState;
  // init for LIS=0, local device type/subtype=1/0, complete work width=0xFF,
  // target process data/PRI=2, pointer to my local device type ( to resolve dynamic SA at time of cmd send ),
  // load/store measurememnt data to/from EEPROM
  c_workState.init( 0, 0, myIsoName, c_myIsoName, &c_myIsoName, false, 0x1234 );

  // update current measurement value ( real value, which can differ from commanded value )
  c_workState.setMasterMeasurementVal( 100 );
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
    @param ps_elementDDI optional pointer to array of structure IsoAgLib::ElementDdi_s which contains DDI, element, isSetpoint and ValueGroup
                         (array is terminated by ElementDdi_s.ui16_element == 0xFFFF)

    @param arcc_isoName optional ISOName code of Process-Data
    @param apc_externalOverridingIsoName pointer to updated ISOName variable
    @param ab_cumulativeValue
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
    @param aui16_eepromAdr optional adress where value is stored in EEPROM
    @param apc_processDataChangeHandler optional pointer to handler class of application
    @param ai_singletonVecKey optional key for selection of IsoAgLib instance (default 0)
  */
  iProcDataLocalSimpleSetpoint_c(const IsoAgLib::ElementDdi_s* ps_elementDDI = NULL, uint16_t aui16_element = 0xFFFF,
                                 const iIsoName_c& arcc_isoName = iIsoName_c::iIsoNameInitialProcessData(),
                                 const iIsoName_c *apc_externalOverridingIsoName = NULL,
                                 bool ab_cumulativeValue = false,
#ifdef USE_EEPROM_IO
                                 uint16_t aui16_eepromAdr = 0xFFFF,
#endif
                                 ProcessDataChangeHandler_c *apc_processDataChangeHandler = NULL,
                                 int ai_singletonVecKey = 0
  ) : ProcDataLocalSimpleSetpoint_c(ps_elementDDI, aui16_element, arcc_isoName,
                                    apc_externalOverridingIsoName, ab_cumulativeValue,
#ifdef USE_EEPROM_IO
                                    aui16_eepromAdr,
#endif
                                    apc_processDataChangeHandler,
                                    ai_singletonVecKey) {}

  /**
    initialise this ProcDataLocalSimpleSetpoint_c
    instance to a well defined initial state

    possible errors:
        * Err_c::badAlloc not enough memory to insert first  MeasureProgLocal
    ISO parameter
    @param ps_elementDDI optional pointer to array of structure IsoAgLib::ElementDdi_s which contains DDI, element, isSetpoint and ValueGroup
                         (array is terminated by ElementDdi_s.ui16_element == 0xFFFF)

    @param arcc_isoName optional ISOName code of Process-Data
    @param apc_externalOverridingIsoName pointer to updated ISOName variable
    @param ab_cumulativeValue
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
    @param aui16_eepromAdr optional adress where value is stored in EEPROM
    @param apc_processDataChangeHandler optional pointer to handler class of application
    @param ai_singletonVecKey optional key for selection of IsoAgLib instance (default 0)
  */
  void init(const IsoAgLib::ElementDdi_s* ps_elementDDI, uint16_t aui16_element,
            const iIsoName_c& arcc_isoName = iIsoName_c::iIsoNameInitialProcessData(),
            const iIsoName_c *apc_externalOverridingIsoName = NULL,
            bool ab_cumulativeValue = false,
#ifdef USE_EEPROM_IO
            uint16_t aui16_eepromAdr = 0xFFFF,
#endif
            ProcessDataChangeHandler_c *apc_processDataChangeHandler = NULL,
            int ai_singletonVecKey = 0
            )
  {ProcDataLocalSimpleSetpoint_c::init(ps_elementDDI, aui16_element,
                                       arcc_isoName, apc_externalOverridingIsoName, ab_cumulativeValue,
#ifdef USE_EEPROM_IO
                                       aui16_eepromAdr,
#endif
                                       apc_processDataChangeHandler,
                                       ai_singletonVecKey);
  }

  /** set the poitner to the handler class
    * @param apc_processDataChangeHandler pointer to handler class of application
    */
  void setProcessDataChangeHandler( ProcessDataChangeHandler_c *apc_processDataChangeHandler )
   { ProcDataLocalSimpleSetpoint_c::setProcessDataChangeHandler( apc_processDataChangeHandler ); }

  /** deliver the poitner to the handler class
    * @return pointer to handler class of application (or NULL if not defined by application)
    */
  IsoAgLib::ProcessDataChangeHandler_c* getProcessDataChangeHandler( void ) const
   { return ProcDataLocalSimpleSetpoint_c::getProcessDataChangeHandler(); }

  /**
    deliver value DEVCLASS (machine type specific table of process data types)
    @return DEVCLASS
  */
  uint8_t devClass() const{return ProcDataLocalSimpleSetpoint_c::devClass();}

  /**
    deliver value ISOName (machine type specific table of process data types)
    use everytime the _device_class_ from the ident part, and take the _instance_ from the owner
    @return ISOName
  */
  const iIsoName_c& isoName() const {return ProcDataLocalSimpleSetpoint_c::isoName().toConstIisoName_c();}

  /**
    deliver value _instance_ (important if more than one machine with equal _device_class_ are active)
    @return POS
  */
  uint8_t devClassInst() const{return ProcDataLocalSimpleSetpoint_c::devClassInst();}

  /** check if this ProcIdent_c has the given DDI as element */
  bool hasDDI( uint16_t aui16_checkDDI ) const { return ProcIdent_c::hasDDI( aui16_checkDDI );}

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
    @param ac_targetISOName ISOName of target
    @return true -> successful sent
  */
  bool sendMasterMeasurementVal( const iIsoName_c& ac_targetISOName ) const
    { return ProcDataLocalSimpleSetpoint_c::sendMasterMeasurementVal( ac_targetISOName );}

  /**
    send a exact-setpoint to a specified target (selected by GPT)
    @param ac_targetISOName ISOName of target
    @return true -> successful sent
  */
  bool sendMasterSetpointVal( const iIsoName_c& ac_targetISOName ) const
   { return setpointConst().sendMasterSetpointVal( ac_targetISOName );}

  /**
    send a sub-setpoint (selected by value group) to a specified target (selected by GPT)
    @param en_valueGroup select sub-type of setpoint
    @param ac_targetISOName ISOName of target
    @return true -> successful sent
  */
  bool sendSetpointForGroup( ProcessCmd_c::ValueGroup_t en_valueGroup, const iIsoName_c& ac_targetISOName ) const
   {
     return setpointConst().sendSetpointForGroup(ac_targetISOName, en_valueGroup, __IsoAgLib::ProcessCmd_c::setValue );
   }

  #ifdef USE_EEPROM_IO
  /**
    deliver the eeprom adr for the value
    @return configured EEPROM adress
  */
  uint16_t eepromAdr()const{return ProcDataLocalSimpleSetpoint_c::eepromAdr();}

  /**
    set the eeprom adr for the value, read in value from EEPROM

    possible errors:
        * dependent error in EepromIo_c on problems during read
    @param aui16_eepromAdr new EEPROM adress
  */
  void setEepromAdr(uint16_t aui16_eepromAdr)
    {ProcDataLocalSimpleSetpoint_c::setEepromAdr(aui16_eepromAdr);}
  #endif

  /**
    deliver the master value (central measure value of this process data;
    can differ from measure vals of measure progs, as these can be reseted
    independent)
    @return actual master value
  */
  const int32_t& masterMeasurementVal()const{return ProcDataLocalSimpleSetpoint_c::masterMeasurementVal();}

  /**
    set the masterVal from main application independent from any measure progs
    @param ai32_val new measure value
  */
  void setMasterMeasurementVal(int32_t ai32_val)
    {ProcDataLocalSimpleSetpoint_c::setMasterMeasurementVal(ai32_val);}

  /**
    set the masterVal from main application independent from any measure progs
    @param ai16_val new measure value
  */
  void setMasterMeasurementVal(int16_t ai16_val)
    {ProcDataLocalSimpleSetpoint_c::setMasterMeasurementVal((int32_t)ai16_val);}

  /**
    set the masterVal from main application independent from any measure progs
    @param ab_val new measure value
  */
  void setMasterMeasurementVal(uint8_t ab_val)
    {ProcDataLocalSimpleSetpoint_c::setMasterMeasurementVal((int32_t)ab_val);}

  /**
    increment the value -> update the local and the measuring programs values
    @param ai32_val size of increment of master value
  */
  void incrMasterMeasurementVal(int32_t ai32_val)
    {ProcDataLocalSimpleSetpoint_c::incrMasterMeasurementVal(ai32_val);}

#ifdef USE_FLOAT_DATA_TYPE
  /**
    deliver the master value (central measure value of this process data;
    can differ from measure vals of measure progs, as these can be reseted
    independent) as float value
    @return actual master value
  */
  const float& masterValFloat()const
    {return ProcDataLocalSimpleSetpoint_c::masterValFloat();}

  /**
    set the masterVal from main application independent from any measure progs
    @param af_val new measure value
  */
  void setMasterMeasurementVal(float af_val)
    {ProcDataLocalSimpleSetpoint_c::setMasterMeasurementVal(af_val);}

  /**
    increment the value -> update the local and the measuring programs values
    @param af_val size of increment of master value
  */
  void incrMasterMeasurementVal(float af_val)
    {ProcDataLocalSimpleSetpoint_c::incrMasterMeasurementVal(af_val);}
#endif


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
    {ProcDataLocalSimpleSetpoint_c::setValType(ren_procValType);}


  /**
    retreive simple master setpoint
    @return actual received setpoint value
  */
  int32_t setpointMasterVal() const
    { return ProcDataLocalSimpleSetpoint_c::setpointConst().setpointMasterVal();}

    #ifdef USE_FLOAT_DATA_TYPE
  /**
    retreive simple master setpoint
    @return actual received setpoint value
  */
  float setpointMasterValFloat() const
    { return ProcDataLocalSimpleSetpoint_c::setpointConst().setpointMasterValFloat();}
    #endif

  /**
    set the setpoint value
    @param ai32_val new setpoint value
  */
  void setSetpointMasterVal(int32_t ai32_val)
    { ProcDataLocalSimpleSetpoint_c::setpoint().setSetpointMasterVal(ai32_val);}

  #ifdef USE_FLOAT_DATA_TYPE
  /**
    set the setpoint value as float value
    @param af_val new setpoint value
  */
  void setSetpointMasterVal(float af_val)
    { ProcDataLocalSimpleSetpoint_c::setpoint().setSetpointMasterVal(af_val);}
  #endif

   /**
    check if specific measureprog exist
    @param arcc_isoName DEVCLASS code of searched measure program
    @return true -> found item
  */
  bool existProg(const iIsoName_c& arcc_isoName)
      {return ProcDataLocalSimpleSetpoint_c::existProg(arcc_isoName);}

  /**
    search for suiting measureprog, if not found AND if ab_doCreate == true
    create copy from first element at end of vector

    possible errors:
        * Err_c::elNonexistent wanted measureprog doesn't exist and ab_doCreate == false

    @param arcc_isoName DEVCLASS code of searched measure program
    @param ab_doCreate true -> create suitable measure program if not found
  */
  iMeasureProgLocal_c& prog(const iIsoName_c& arcc_isoName, bool ab_doCreate)
    { return static_cast<iMeasureProgLocal_c&>(ProcDataLocalSimpleSetpoint_c::prog(arcc_isoName, ab_doCreate));}

  /** deliver reference to setpoint */
  iSimpleManageSetpointLocal_c& setpoint( void )
    { return static_cast<iSimpleManageSetpointLocal_c&>
             (ProcDataLocalSimpleSetpoint_c::setpoint());
    }

};

}
#endif
