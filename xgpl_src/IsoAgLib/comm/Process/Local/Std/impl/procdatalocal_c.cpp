/***************************************************************************
                          procdatalocal_c.cpp  -  description
                             -------------------
    begin                : Fri Apr 07 2000
    copyright            : (C) 2000 - 2004 by Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
    type                 : Source
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
 * - Call of a (member-) function                                          *
 * - Instantiation of a variable in a datatype from internal namespace     *
 * Allowed is:                                                             *
 * - Instatiation of a variable in a datatype from interface namespace,    *
 *   even if this is derived from a base class inside an internal namespace*
 * - Call of member functions which are defined in the interface class     *
 *   definition ( header )                                                 *
 * Pairing of internal and interface classes:                              *
 * - Internal implementation in an :impl: subdirectory                     *
 * - Interface in the parent directory of the corresponding internal class *
 * - Interface class name IsoAgLib::iFoo_c maps to the internal class      *
 *   __IsoAgLib::Foo_c                                                     *
 *                                                                         *
 * AS A RULE: Use only classes with names beginning with small letter :i:  *
 ***************************************************************************/

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include "procdatalocal_c.h"
#include "../../../impl/process_c.h"

namespace __IsoAgLib {
// ****************************************************************************************
// ********************************* ProcDataLocal_c *********************************
// ****************************************************************************************

/**
  constructor which can set all element vars

  possible errors:
      * Err_c::badAlloc not enough memory to insert first  MeasureProgLocal
  ISO parameter
  @param ps_elementDDI optional pointer to array of structure IsoAgLib::ElementDdi_s which contains DDI, element, isSetpoint and ValueGroup
                       (array is terminated by ElementDdi_s.ui16_element == 0xFFFF)

  @param rc_isoName optional ISOName code of Process-Data
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
ProcDataLocal_c::ProcDataLocal_c( const IsoAgLib::ElementDdi_s* ps_elementDDI, uint16_t rui16_element,
                                  const IsoName_c& rc_isoName, const IsoName_c& rc_ownerISOName,
                                  const IsoName_c *rpc_isoName, bool rb_cumulativeValue,
#ifdef USE_EEPROM_IO
                  uint16_t rui16_eepromAdr,
#endif
                  IsoAgLib::ProcessDataChangeHandler_c *rpc_processDataChangeHandler,
                  int ri_singletonVecKey
                  )
    : ProcDataLocalBase_c( ps_elementDDI, rui16_element,
                          rc_isoName, rc_ownerISOName, rpc_isoName, rb_cumulativeValue,
#ifdef USE_EEPROM_IO
                          rui16_eepromAdr,
#endif
                          rpc_processDataChangeHandler,
                          ri_singletonVecKey)
    , c_measureprog( this )
    , c_setpoint( this )
{
}

/**
  initialise this ProcDataLocal_c instance to a well defined initial state

  possible errors:
      * Err_c::badAlloc not enough memory to insert first  MeasureProgLocal
  ISO parameter
  @param ps_elementDDI optional pointer to array of structure IsoAgLib::ElementDdi_s which contains DDI, element, isSetpoint and ValueGroup
                       (array is terminated by ElementDdi_s.ui16_element == 0xFFFF)

  @param rc_isoName optional ISOName code of Process-Data
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
void ProcDataLocal_c::init( const IsoAgLib::ElementDdi_s* ps_elementDDI, uint16_t rui16_element,
                            const IsoName_c& rc_isoName, const IsoName_c& rc_ownerISOName,
                            const IsoName_c *rpc_isoName, bool rb_cumulativeValue,
#ifdef USE_EEPROM_IO
                           uint16_t rui16_eepromAdr,
#endif
                           IsoAgLib::ProcessDataChangeHandler_c *rpc_processDataChangeHandler,
                           int ri_singletonVecKey
                           )
{
  ProcDataLocalBase_c::init( ps_elementDDI, rui16_element,
                            rc_isoName, rc_ownerISOName, rpc_isoName, rb_cumulativeValue,
#ifdef USE_EEPROM_IO
                            rui16_eepromAdr,
#endif
                            rpc_processDataChangeHandler,
                            ri_singletonVecKey );
  c_setpoint.init( this );
  c_measureprog.init( this );
}

/** destructor */
ProcDataLocal_c::~ProcDataLocal_c(){
  // now unregister the pointer to this instance in Process_c
  getProcessInstance4Comm().unregisterLocalProcessData( this );
}


#ifdef USE_EEPROM_IO
/**
  set the eeprom adr for the value, read in value from EEPROM

  possible errors:
      * dependent error in EepromIo_c on problems during read
  @param rui16_eepromAdr new EEPROM adress
*/
void ProcDataLocal_c::setEepromAdr(uint16_t rui16_eepromAdr)
{
  ProcDataLocalBase_c::setEepromAdr( rui16_eepromAdr );
  #ifdef USE_FLOAT_DATA_TYPE
  if (valType() == float_val)
  { // now init read val for all MeasureProg
    c_measureprog.initGlobalVal( eepromValFloat() );
  }
  else
  #endif
  { // now init read val for all MeasureProg
    c_measureprog.initGlobalVal( eepromVal() );
  }
}
#endif
/**
  set the masterMeasurementVal from main application independent from any measure progs
  @param ri32_val new measure value
*/
void ProcDataLocal_c::setMasterMeasurementVal(int32_t ri32_val){
  ProcDataLocalBase_c::setMasterMeasurementVal( ri32_val );
  c_measureprog.setGlobalVal( ri32_val );
}
/**
  increment the value -> update the local and the measuring programs values
  @param ri32_val size of increment of master value
*/
void ProcDataLocal_c::incrMasterMeasurementVal(int32_t ri32_val){
  ProcDataLocalBase_c::incrMasterMeasurementVal( ri32_val );
  // ProcDataLocalBase_c::incrMasterMeasurementVal sets new ProcDataLocalBase_c::i32_masterVal
  c_measureprog.setGlobalVal( masterMeasurementVal() );
}
#ifdef USE_FLOAT_DATA_TYPE
/**
  set the masterMeasurementVal from main application independent from any measure progs
  @param rf_val new measure value
*/
void ProcDataLocal_c::setMasterMeasurementVal(float rf_val){
  ProcDataLocalBase_c::setMasterMeasurementVal( rf_val );
  c_measureprog.setGlobalVal( rf_val );
}
/**
  increment the value -> update the local and the measuring programs values
  @param ri32_val size of increment of master value
*/
void ProcDataLocal_c::incrMasterMeasurementVal(float rf_val){
  ProcDataLocalBase_c::incrMasterMeasurementVal( rf_val );
  // ProcDataLocalBase_c::incrMasterMeasurementVal sets new ProcDataLocalBase_c::i32_masterVal
  c_measureprog.setGlobalVal( masterValFloat() );
}
#endif

/**
  perform periodic actions
  delete all running measure programs of members which are >3sec inactive
  deletion of outdated setpoints is managed by SetpointLocal_c::timeEvent
  @param pui16_nextTimePeriod calculated new time period, based on current measure progs (only for local proc data)
  @return true -> all planned executions performed
*/
bool ProcDataLocal_c::timeEvent( uint16_t *pui16_nextTimePeriod ){
  if ( ElementBase_c::getAvailableExecTime() == 0 ) return false;

  // perform time event activities for base class
  if ( ! ProcDataLocalBase_c::timeEvent() ) return false;

  if ( ! c_measureprog.timeEvent(pui16_nextTimePeriod) ) return false;
  if ( ! c_setpoint.timeEvent() ) return false;
  return true;
}

#ifdef USE_EEPROM_IO
/**
  called from MeasureProg item -> if this item is first in list
  reset eeprom val

  possible errors:
      * dependent error in EepromIo_c on problems during read
  @param pc_progItem MeasureProgLocal_c instance which wants to reset EEPROM val
*/
void ProcDataLocal_c::resetEeprom( MeasureProgLocal_c* pc_progItem )
{ // only reset if pointer is begin
  if (&(*c_measureprog.vec_prog().begin()) == pc_progItem)
    ProcDataLocalBase_c::resetEeprom();
}
#endif

/** process a setpoint message */
void ProcDataLocal_c::processSetpoint(){
  switch (getProcessInstance4Comm().data().c_generalCommand.getCommand())
  {
    case GeneralCommand_c::setValue:
    case GeneralCommand_c::requestValue:
      c_setpoint.processMsg();
      break;
    default:
      // process measurement commands even if this DDI is defined as a setpoint
      c_measureprog.processProg();
  }
}

/** process a measure prog message for local process data */
void ProcDataLocal_c::processProg(){
  c_measureprog.processProg();
}

/**
  allow local client to actively start a measurement program
  (to react on a incoming "start" command for default data logging)
  @param ren_type measurement type: Proc_c::TimeProp, Proc_c::DistProp, ...
  @param ri32_increment
  @param rpc_receiverDevice commanding ISOName
  @return true -> measurement started
*/
bool ProcDataLocal_c::startDataLogging(Proc_c::type_t ren_type /* Proc_c::TimeProp, Proc_c::DistProp, ... */,
                                       int32_t ri32_increment, const IsoName_c* rpc_receiverDevice )
{
  if ( !rpc_receiverDevice )
    // get isoName of TC from last TC status message
    rpc_receiverDevice = getProcessInstance4Comm().getTcISOName();

  // if still no rpc_receiverDevice => get it from ISO monitor list
  if ( !rpc_receiverDevice )
  {  // get TC dev key (device class 0)
    const IsoItem_c& c_tcISOItem = getIsoMonitorInstance4Comm().isoMemberDevClassInd(0 /* rui8_devClass */, 0 /* rui8_ind */, TRUE /* rb_forceClaimedAddress */);
    rpc_receiverDevice = &(c_tcISOItem.isoName());
  }

  return c_measureprog.startDataLogging(ren_type, ri32_increment, rpc_receiverDevice);
}

/**
  stop all measurement progs in all local process instances, started with given isoName
  @param refc_isoName
*/
void ProcDataLocal_c::stopRunningMeasurement(const IsoName_c& refc_isoName)
{
  c_measureprog.stopRunningMeasurement(refc_isoName);
}


} // end of namespace __IsoAgLib
