/***************************************************************************
                          procdatalocalbase_c.cpp - managing of simple
                                                       local process data object
                             -------------------
    begin                : Fri Apr 07 2000
    copyright            : (C) 2000 - 2009 by Dipl.-Inform. Achim Spangler
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

#include "procdatalocalbase_c.h"
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/process_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/processdatachangehandler_c.h>


namespace __IsoAgLib {

/**
  initialise this ProcDataBase_c instance to a well defined initial state

  possible errors:
      * Err_c::badAlloc not enough memory to insert first  MeasureProgLocal
  ISO parameter
  @param ps_elementDDI optional pointer to array of structure IsoAgLib::ElementDdi_s which contains DDI, element, isSetpoint and ValueGroup
                       (array is terminated by ElementDdi_s.ui16_element == 0xFFFF)
  @param aui16_element
  @param acrc_isoName optional ISOName code of Process-Data
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
  @param ai_multitonInst optional key for selection of IsoAgLib instance (default 0)
*/
void ProcDataLocalBase_c::init(const IsoAgLib::ElementDdi_s* ps_elementDDI, uint16_t aui16_element,
                               const IsoName_c& acrc_isoName, const IsoName_c *apc_externalOverridingIsoName,
                               bool ab_cumulativeValue,
                               IsoAgLib::ProcessDataChangeHandler_c *apc_processDataChangeHandler,
                               int ai_multitonInst
                               )
{
  ProcDataBase_c::init( ps_elementDDI, aui16_element, acrc_isoName, apc_externalOverridingIsoName,
                        apc_processDataChangeHandler, ai_multitonInst);

  mb_cumulativeValue = ab_cumulativeValue;
  mi32_masterVal = 0;

  getProcessInstance4Comm().registerLocalProcessData( this );
}

/** copy constructor */
ProcDataLocalBase_c::ProcDataLocalBase_c( const ProcDataLocalBase_c& acrc_src )
  : ProcDataBase_c( acrc_src )
{
  assignFromSource( acrc_src );

  // now register the pointer to this instance in Process_c
  getProcessInstance4Comm().registerLocalProcessData( this );
}
/** assignment operator */
const ProcDataLocalBase_c& ProcDataLocalBase_c::operator=( const ProcDataLocalBase_c& acrc_src )
{
  ProcDataBase_c::operator=( acrc_src );
  assignFromSource( acrc_src );
  return *this;
}
/** base function for assignment of element vars for copy constructor and operator= */
void ProcDataLocalBase_c::assignFromSource( const ProcDataLocalBase_c& acrc_src )
{
  mi32_masterVal = acrc_src.mi32_masterVal;
  mb_cumulativeValue = acrc_src.mb_cumulativeValue;
}


/** default destructor which has nothing to do */
ProcDataLocalBase_c::~ProcDataLocalBase_c(){
  // call unregisterLocalProcessData in last derived class because unregister does again message processing!
}


/**
  set the masterMeasurementVal from main application independent from any measure progs
  @param ai32_val new measure value
*/
void ProcDataLocalBase_c::setMasterMeasurementVal(int32_t ai32_val)
{
  mi32_masterVal = ai32_val;
}

/**
  increment the value -> update the local and the measuring programs values
  @param ai32_val size of increment of master value
*/
void ProcDataLocalBase_c::incrMasterMeasurementVal(int32_t ai32_val)
{
  mi32_masterVal += ai32_val;
}


/**
  perform periodic actions
  task for ProcDataLocal_c::timeEvent is to store the actual
  eeprom value in the defined time intervall
    @param pui16_nextTimePeriod calculated new time period, based on current measure progs (only for local proc data)
  @return true -> all planned executions performed
*/
bool ProcDataLocalBase_c::timeEvent( uint16_t* /* pui16_nextTimePeriod */)
{
  return true;
}

/**
  send a main-information (selected by value group) to a specified target (selected by ISOName)
  @param ac_targetISOName ISOName of target
  @return true -> successful sent
*/
bool ProcDataLocalBase_c::sendMasterMeasurementVal( const IsoName_c& ac_targetISOName) const {

    ProcessPkg_c pkg;
    // prepare general command in process pkg
    pkg.mc_processCmd.setValues(false /* isSetpoint */, false, /* isRequest */
                                                             ProcessCmd_c::exactValue,
                                                             ProcessCmd_c::setValue);

    return sendValISOName( pkg, ac_targetISOName, masterMeasurementVal());
}


/** process a measure prog message for local process data.
    this variant is only used for simple measurement progam management.
    derived classes with more flexible management (including measurement programs)
    use their own overloaded version.
*/
void ProcDataLocalBase_c::processProg( const ProcessPkg_c& pkg )
{
  if (pkg.senderItem() == NULL)
  { // sender with SA 0xFE is not of interest
    return;
  }

  // handle for simple measurement value
  if (pkg.mc_processCmd.checkIsRequest() &&
      // pkg.mc_processCmd.checkIsMeasure() &&  /* already checked before, we are in processProg() ! */
      pkg.mc_processCmd.getValueGroup() == ProcessCmd_c::exactValue)
  { // request for measurement value
    sendMasterMeasurementVal( pkg.senderItem()->isoName() );
  }
}


/** processing of a setpoint message.
    this base class variant checks only, if a setpoint cmd was recieved
    which wants to reset a measurement value
*/
void ProcDataLocalBase_c::processSetpoint( const ProcessPkg_c& pkg )
{
  #ifdef RESET_MEASUREMENT_WITH_ZERO_EXACT_SETPOINT
  if (pkg.dataRawCmdLong() == 0)
  { // let processProg process measurement value reset -> change PD to use std. reset
    pkg.setPd(1);
    processProg(pkg);
  }
  #endif
}


bool ProcDataLocalBase_c::sendValISOName( ProcessPkg_c& arc_pkg, const IsoName_c& ac_varISOName, int32_t ai32_val) const
{
  arc_pkg.setISONameForDA(ac_varISOName);
  arc_pkg.setISONameForSA(isoName());

  return ProcDataBase_c::sendValISOName (arc_pkg, ac_varISOName, ai32_val);
}

} // end of namespace __IsoAgLib
