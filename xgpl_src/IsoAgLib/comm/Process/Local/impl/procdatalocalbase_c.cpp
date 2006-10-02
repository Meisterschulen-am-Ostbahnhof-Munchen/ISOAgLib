/***************************************************************************
                          procdatalocalbase_c.cpp - managing of simple
                                                       local process data object
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
#include "procdatalocalbase_c.h"
#include "../../impl/process_c.h"
#include "../../processdatachangehandler_c.h"

#ifdef USE_EEPROM_IO
  #include <IsoAgLib/driver/eeprom/impl/eepromio_c.h>
#endif

namespace __IsoAgLib {

/**
  initialise this ProcDataBase_c instance to a well defined initial state

  possible errors:
      * Err_c::badAlloc not enough memory to insert first  MeasureProgLocal
  ISO parameter
  @param ps_elementDDI optional pointer to array of structure IsoAgLib::ElementDDI_s which contains DDI, element, isSetpoint and ValueGroup
                       (array is terminated by ElementDDI_s.ui16_element == 0xFFFF)

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
void ProcDataLocalBase_c::init(const IsoAgLib::ElementDDI_s* ps_elementDDI, uint16_t rui16_element,
                               const ISOName_c& rc_isoName, const ISOName_c& rc_ownerISOName,
                               const ISOName_c *rpc_isoName, bool rb_cumulativeValue,
#ifdef USE_EEPROM_IO
                               uint16_t rui16_eepromAdr,
#endif // USE_EEPROM_IO
                               IsoAgLib::ProcessDataChangeHandler_c *rpc_processDataChangeHandler,
                               int ri_singletonVecKey
                               )
{
  ProcDataBase_c::init( ps_elementDDI, rui16_element, rc_isoName, rc_ownerISOName, rpc_isoName,
                        rpc_processDataChangeHandler, ri_singletonVecKey);

  b_cumulativeValue = rb_cumulativeValue;
#ifdef USE_EEPROM_IO
  setEepromAdr(rui16_eepromAdr);
  i32_lastEepromStore = 0;
#endif // USE_EEPROM_IO
  i32_masterVal = 0;

  getProcessInstance4Comm().registerLocalProcessData( this );
}

/** copy constructor */
ProcDataLocalBase_c::ProcDataLocalBase_c( const ProcDataLocalBase_c& rrefc_src )
  : ProcDataBase_c( rrefc_src )
{
  assignFromSource( rrefc_src );

  // now register the pointer to this instance in Process_c
  getProcessInstance4Comm().registerLocalProcessData( this );
}
/** assignment operator */
const ProcDataLocalBase_c& ProcDataLocalBase_c::operator=( const ProcDataLocalBase_c& rrefc_src )
{
  ProcDataBase_c::operator=( rrefc_src );
  assignFromSource( rrefc_src );
  return *this;
}
/** base function for assignment of element vars for copy constructor and operator= */
void ProcDataLocalBase_c::assignFromSource( const ProcDataLocalBase_c& rrefc_src )
{
  i32_masterVal = rrefc_src.i32_masterVal;
#ifdef USE_EEPROM_IO
  i32_eepromVal = rrefc_src.i32_eepromVal;
  i32_lastEepromStore = rrefc_src.i32_lastEepromStore;
  ui16_eepromAdr = rrefc_src.ui16_eepromAdr;
#endif // USE_EEPROM_IO
  b_cumulativeValue = rrefc_src.b_cumulativeValue;
}


/** default destructor which has nothing to do */
ProcDataLocalBase_c::~ProcDataLocalBase_c(){
  // call unregisterLocalProcessData in last derived class because unregister does again message processing!
}

#ifdef USE_EEPROM_IO
/**
  set the eeprom adr for the value, read in value from EEPROM

  possible errors:
      * dependent error in EEPROMIO_c on problems during read
  @param rui16_eepromAdr new EEPROM adress
*/
void ProcDataLocalBase_c::setEepromAdr(uint16_t rui16_eepromAdr)
{
  ui16_eepromAdr = rui16_eepromAdr;
  #ifdef USE_FLOAT_DATA_TYPE
  if (valType() == float_val)
  {
    if (ui16_eepromAdr < 0xFFFF)
    { // valid adress -> read in value
      // set read position
      getEepromInstance().setg(rui16_eepromAdr);
      // read data from eeprom
      float f_temp;
      getEepromInstance() >> f_temp;
      f_eepromVal = f_temp;
      // decide if eeprom val should be assigned as starting masterMeasurementVal
      if (!b_cumulativeValue)
      { // value is NOT cumulative -> use eeprom as starting master,
        // else for cumulative values master value can be set independent from EEPROM
        // ==> for cumulative values the communicated value is sum of masterMeasurementVal AND EEPROM val
        f_masterVal = f_eepromVal;
      }
    }
    else
    { // for undefined adr set value 0
      f_eepromVal = 0.0F;
    }
  }
  else
  #endif // USE_FLOAT_DATA_TYPE
  {
    if (ui16_eepromAdr < 0xFFFF)
    { // valid adress -> read in value
      // set read position
      getEepromInstance().setg(rui16_eepromAdr);
      // read data from eeprom
      int32_t i32_temp;
      getEepromInstance() >> i32_temp;
      i32_eepromVal = i32_temp;
      // decide if eeprom val should be assigned as starting masterMeasurementVal
      if (!b_cumulativeValue)
      { // value is NOT cumulative -> use eeprom as starting master,
        // else for cumulative values master value can be set independent from EEPROM
        // ==> for cumulative values the communicated value is sum of masterMeasurementVal AND EEPROM val
        i32_masterVal = i32_eepromVal;
      }
    }
    else
    { // for undefined adr set value 0
      i32_eepromVal = 0;
    }
  }
}
#endif // USE_EEPROM_IO

/**
  set the masterMeasurementVal from main application independent from any measure progs
  @param ri32_val new measure value
*/
void ProcDataLocalBase_c::setMasterMeasurementVal(int32_t ri32_val){
  // set the local values
  setValType(i32_val);
#ifdef USE_EEPROM_IO
  if (b_cumulativeValue) i32_eepromVal += (ri32_val - i32_masterVal);
  else i32_eepromVal = ri32_val;
#endif // USE_EEPROM_IO
  i32_masterVal = ri32_val;
}

/**
  increment the value -> update the local and the measuring programs values
  @param ri32_val size of increment of master value
*/
void ProcDataLocalBase_c::incrMasterMeasurementVal(int32_t ri32_val){
  // set the local values
  setValType(i32_val);
#ifdef USE_EEPROM_IO
  i32_eepromVal += ri32_val;
#endif // USE_EEPROM_IO
  i32_masterVal += ri32_val;
}

#ifdef USE_FLOAT_DATA_TYPE
/**
  set the masterMeasurementVal from main application independent from any measure progs
  @param rf_val new measure value
*/
void ProcDataLocalBase_c::setMasterMeasurementVal(float rf_val){
  // set the local values
  setValType(float_val);
  #ifdef USE_EEPROM_IO
  if (b_cumulativeValue) f_eepromVal += (rf_val - f_masterVal);
  else f_eepromVal = rf_val;
  #endif // USE_EEPROM_IO
  f_masterVal = rf_val;
}

/**
  increment the value -> update the local and the measuring programs values
  @param ri32_val size of increment of master value
*/
void ProcDataLocalBase_c::incrMasterMeasurementVal(float rf_val){
  // set the local values
  setValType(float_val);
  #ifdef USE_EEPROM_IO
  f_eepromVal += rf_val;
  #endif // USE_EEPROM_IO
  f_masterVal += rf_val;
}
#endif // USE_FLOAT_DATA_TYPE


/**
  perform periodic actions
  task for ProcDataLocal_c::timeEvent is to store the actual
  eeprom value in the defined time intervall
  @return true -> all planned executions performed
*/
bool ProcDataLocalBase_c::timeEvent( void ){
  if ( Scheduler_c::getAvailableExecTime() == 0 ) return false;
  #ifdef USE_EEPROM_IO
  // check if eeprom value should be stored
  int32_t i32_time = Scheduler_c::getLastTimeEventTrigger();
  if ((i32_time - i32_lastEepromStore > CONFIG_PROC_STORE_EEPROM_INTERVAL) && (ui16_eepromAdr != 0xFFFF))
  { // store needed
    getEepromInstance().setp(ui16_eepromAdr);
    #ifdef USE_FLOAT_DATA_TYPE
    if (valType() == float_val)
    {
      getEepromInstance() << f_eepromVal;
    }
    else
    #endif // USE_FLOAT_DATA_TYPE
    {
      getEepromInstance() << i32_eepromVal;
    }
    i32_lastEepromStore = i32_time;
  }
  #endif // USE_EEPROM_IO
  return true;
}

/**
  send a main-information (selected by MOD) to a specified target (selected by ISOName)
  @param rc_targetISOName ISOName of target
  @return true -> successful sent
*/
bool ProcDataLocalBase_c::sendMasterMeasurementVal( const ISOName_c& rc_targetISOName) const {

    // prepare general command in process pkg
    getProcessInstance4Comm().data().c_generalCommand.setValues(false /* isSetpoint */, false, /* isRequest */
                                                                GeneralCommand_c::exactValue,
                                                                GeneralCommand_c::setValue);

    #if defined(USE_EEPROM_IO) && defined(USE_FLOAT_DATA_TYPE)
    if (valType() == i32_val) return sendValISOName(rc_targetISOName, eepromVal());
    else return sendValISOName(rc_targetISOName, eepromValFloat());
    #elif !defined(USE_EEPROM_IO) && defined(USE_FLOAT_DATA_TYPE)
    if (valType() == i32_val) return sendValISOName(rc_targetISOName, masterMeasurementVal());
    else return sendValISOName(rc_targetISOName, masterValFloat());
    #elif defined(USE_EEPROM_IO) && !defined(USE_FLOAT_DATA_TYPE)
    return sendValISOName(rc_targetISOName, eepromVal());
    #elif !defined(USE_EEPROM_IO) && !defined(USE_FLOAT_DATA_TYPE)
    return sendValISOName(rc_targetISOName, masterMeasurementVal());
    #endif
}


/** process a measure prog message for local process data.
    this variant is only used for simple measurement progam management.
    derived classes with more flexible management (including measurement programs)
    use their own overloaded version.
*/
void ProcDataLocalBase_c::processProg(){
  ProcessPkg_c& c_pkg = getProcessInstance4Comm().data();
  // handle for simple measurement value
  if (c_pkg.c_generalCommand.checkIsRequest() &&
      // c_pkg.c_generalCommand.checkIsMeasure() &&  /* already checked before, we are in processProg() ! */
      c_pkg.c_generalCommand.getValueGroup() == GeneralCommand_c::exactValue)
  { // request for measurement value
    sendMasterMeasurementVal( c_pkg.memberSend().isoName() );
  }
  else
  {
    if (c_pkg.c_generalCommand.getCommand() == GeneralCommand_c::measurementReset)
    { // measurement reset cmd
      #ifdef USE_EEPROM_IO
      resetEeprom();
      #else
      setMasterMeasurementVal(int32_t(0));
      #endif
      // now send result of reset action
      sendMasterMeasurementVal( c_pkg.memberSend().isoName() );
      // call handler function if handler class is registered
      if ( getProcessDataChangeHandler() != NULL )
        getProcessDataChangeHandler()->processMeasurementReset( this, 0, static_cast<const IsoAgLib::iISOName_c&>(c_pkg.memberSend().isoName()) );
    }
  }
}


/** processing of a setpoint message.
    this base class variant checks only, if a setpoint cmd was recieved
    which wants to reset a measurement value
*/
void ProcDataLocalBase_c::processSetpoint()
{
  #ifdef RESET_MEASUREMENT_WITH_ZERO_EXACT_SETPOINT
  if (getProcessInstance4Comm().data().dataRawCmdLong() == 0)
  { // let processProg process measurement value reset -> change PD to use std. reset
    getProcessInstance4Comm().data().setPd(1);
    processProg();
  }
  #endif
}


#ifdef USE_EEPROM_IO
/**
  called from MeasureProg item -> if this item is first in list
  reset eeprom val

  possible errors:
      * dependent error in EEPROMIO_c on problems during read
  @param pc_progItem MeasureProgLocal_c instance which wants to reset EEPROM val
*/
void ProcDataLocalBase_c::resetEeprom( void ){
  // now eeprom val can be reset
  #ifdef USE_FLOAT_DATA_TYPE
  if (valType() == float_val) f_eepromVal = 0.0F;
  else
  #endif // USE_FLOAT_DATA_TYPE
    i32_eepromVal = 0;
  if (ui16_eepromAdr != 0xFFFF)
  {   // write new val to EEPROM
    getEepromInstance().setp(ui16_eepromAdr);

    #ifdef USE_FLOAT_DATA_TYPE
    if (valType() == float_val) getEepromInstance() << f_eepromVal;
    else
    #endif // USE_FLOAT_DATA_TYPE
      getEepromInstance() << i32_eepromVal;
  }
}
#endif // USE_EEPROM_IO

bool ProcDataLocalBase_c::sendValISOName(const ISOName_c& rc_varISOName, int32_t ri32_val) const
{
  setLocalSendFlags (rc_varISOName);

  return ProcDataBase_c::sendValISOName (rc_varISOName, ri32_val);
}

#ifdef USE_FLOAT_DATA_TYPE
bool ProcDataLocalBase_c::sendValISOName(const ISOName_c& rc_varISOName, float rf_val) const
{
  setLocalSendFlags (rc_varISOName);

  return ProcDataBase_c::sendValISOName (rc_varISOName, rf_val);
}
#endif

void ProcDataLocalBase_c::setLocalSendFlags(const ISOName_c& rc_varISOName) const
{
  ProcessPkg_c& c_data = getProcessPkg();

  c_data.setISONameForDA(rc_varISOName);
  c_data.setISONameForSA(ownerISOName());
}

} // end of namespace __IsoAgLib
