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

namespace __IsoAgLib {
// ****************************************************************************************
// ********************************* ProcDataLocal_c *********************************
// ****************************************************************************************

/**
  constructor which can set all element vars

  possible errors:
      * Err_c::badAlloc not enough memory to insert first  MeasureProgLocal
  @param rui8_lis optional LIS code of this instance
  @param rc_gtp optional GETY_POS code of Process-Data
  @param rui8_wert optional WERT code of this instance
  @param rui8_inst optional INST code of this instance
  @param rui8_zaehlnum optional ZAEHLNUM code of this instance
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
ProcDataLocal_c::ProcDataLocal_c(uint8_t rui8_lis, GetyPos_c rc_gtp, uint8_t rui8_wert,
      uint8_t rui8_inst, uint8_t rui8_zaehlnum, uint8_t rui8_pri, GetyPos_c rc_ownerGtp,
      GetyPos_c *rpc_gtp, bool rb_cumulativeValue
      #ifdef USE_EEPROM_IO
      , uint16_t rui16_eepromAdr
      #endif
      , IsoAgLib::ProcessDataChangeHandler_c *rpc_processDataChangeHandler
      , int ri_singletonVecKey
      )
    : ProcDataLocalBase_c(rui8_lis, rc_gtp, rui8_wert, rui8_inst, rui8_zaehlnum, rui8_pri, rc_ownerGtp, rpc_gtp,
                                rb_cumulativeValue
                                #ifdef USE_EEPROM_IO
                                , rui16_eepromAdr
                                #endif
                                , rpc_processDataChangeHandler
                                , ri_singletonVecKey)
      , c_measureprog( this )
      , c_setpoint( this )
{
}

/**
  initialise this ProcDataLocal_c instance to a well defined initial state

  possible errors:
      * Err_c::badAlloc not enough memory to insert first  MeasureProgLocal
  @param rui8_lis optional LIS code of this instance
  @param rc_gtp optional GETY_POS code of Process-Data
  @param rui8_wert optional WERT code of this instance
  @param rui8_inst optional INST code of this instance
  @param rui8_zaehlnum optional ZAEHLNUM code of this instance
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
void ProcDataLocal_c::init(uint8_t rui8_lis, GetyPos_c rc_gtp, uint8_t rui8_wert,
      uint8_t rui8_inst, uint8_t rui8_zaehlnum, uint8_t rui8_pri, GetyPos_c rc_ownerGtp,
      GetyPos_c *rpc_gtp, bool rb_cumulativeValue
      #ifdef USE_EEPROM_IO
      , uint16_t rui16_eepromAdr
      #endif
      , IsoAgLib::ProcessDataChangeHandler_c *rpc_processDataChangeHandler
      , int ri_singletonVecKey
      )
{
  ProcDataLocalBase_c::init(rui8_lis, rc_gtp, rui8_wert, rui8_inst, rui8_zaehlnum, rui8_pri, rc_ownerGtp, rpc_gtp, rb_cumulativeValue,
      #ifdef USE_EEPROM_IO
      rui16_eepromAdr,
      #endif
      rpc_processDataChangeHandler,
      ri_singletonVecKey );
  c_setpoint.init( this );
  c_measureprog.init( this );
}

/** default destructor which has nothing to do */
ProcDataLocal_c::~ProcDataLocal_c(){
}


#ifdef USE_EEPROM_IO
/**
  set the eeprom adr for the value, read in value from EEPROM

  possible errors:
      * dependent error in EEPROMIO_c on problems during read
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
  set the masterVal from main application independent from any measure progs
  @param ri32_val new measure value
*/
void ProcDataLocal_c::setMasterVal(int32_t ri32_val){
  ProcDataLocalBase_c::setMasterVal( ri32_val );
  c_measureprog.setGlobalVal( ri32_val );
}
/**
  increment the value -> update the local and the measuring programs values
  @param ri32_val size of increment of master value
*/
void ProcDataLocal_c::incrMasterVal(int32_t ri32_val){
  ProcDataLocalBase_c::incrMasterVal( ri32_val );
  c_measureprog.setGlobalVal( ri32_val );
}
#ifdef USE_FLOAT_DATA_TYPE
/**
  set the masterVal from main application independent from any measure progs
  @param rf_val new measure value
*/
void ProcDataLocal_c::setMasterVal(float rf_val){
  ProcDataLocalBase_c::setMasterVal( rf_val );
  c_measureprog.setGlobalVal( rf_val );
}
/**
  increment the value -> update the local and the measuring programs values
  @param ri32_val size of increment of master value
*/
void ProcDataLocal_c::incrMasterVal(float rf_val){
  ProcDataLocalBase_c::incrMasterVal( rf_val );
  c_measureprog.setGlobalVal( rf_val );
}
#endif

/**
  perform periodic actions
  delete all running measure programs of members which are >3sec inactive
  deletion of outdated setpoints is managed by SetpointLocal_c::timeEvent
  @return true -> all planned executions performed
*/
bool ProcDataLocal_c::timeEvent( void ){
  if ( Scheduler_c::getAvailableExecTime() == 0 ) return false;

  // perform time event activities for base class
  if ( ! ProcDataLocalBase_c::timeEvent() ) return false;

  if ( ! c_measureprog.timeEvent() ) return false;
  if ( ! c_setpoint.timeEvent() ) return false;
  return true;
}

#ifdef USE_EEPROM_IO
/**
  called from MeasureProg item -> if this item is first in list
  reset eeprom val

  possible errors:
      * dependent error in EEPROMIO_c on problems during read
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
  c_setpoint.processMsg();
  // call base class processMsg to detect if this is a setpoint cmd
  // to reset the measurement value
  ProcDataLocalBase_c::processSetpoint();
}

/** process a measure prog message for local process data */
void ProcDataLocal_c::processProg(){
  c_measureprog.processProg();
}




} // end of namespace __IsoAgLib
