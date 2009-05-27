/***************************************************************************
                          simplemanagesetpointlocal_c.cpp - managing of simple
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

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include "simplemanagesetpointlocal_c.h"
#include "../../../impl/process_c.h"
#include "../../../processdatachangehandler_c.h"

namespace __IsoAgLib {

// **************************************************************************************
// *************************** SimpleManageSetpointLocal_c ***************************
// **************************************************************************************


/**
  initialise this SimpleManageSetpointLocal_c instance to a well defined initial state
  @param apc_processData optional pointer to containing ProcessData instance
*/
void SimpleManageSetpointLocal_c::init( ProcDataBase_c *const apc_processData )
{
  ProcessElementBase_c::set( apc_processData );
  #if !defined(HANDLE_SETPOINT_MEASURE_EQUIVALENT)
    mi32_setpointMasterVal = 0;
  #endif
}

/** copy constructor */
SimpleManageSetpointLocal_c::SimpleManageSetpointLocal_c( const SimpleManageSetpointLocal_c& acrc_src )
: ProcessElementBase_c( acrc_src )
{
  #if !defined(HANDLE_SETPOINT_MEASURE_EQUIVALENT)
    mi32_setpointMasterVal = acrc_src.mi32_setpointMasterVal;
  #endif
}

/** assignment operator */
const SimpleManageSetpointLocal_c& SimpleManageSetpointLocal_c::operator=( const SimpleManageSetpointLocal_c& acrc_src )
{
  ProcessElementBase_c::operator=( acrc_src );
  #if !defined(HANDLE_SETPOINT_MEASURE_EQUIVALENT)
    mi32_setpointMasterVal = acrc_src.mi32_setpointMasterVal;
  #endif

  return *this;
}

/** process a setpoint message */
void SimpleManageSetpointLocal_c::processSetpoint(){
  // for simple setpoint the message is process here
  ProcessPkg_c& c_pkg = getProcessInstance4Comm().data();
  const IsoName_c& cc_senderISOName = c_pkg.memberSend().isoName();

  if (c_pkg.mc_processCmd.getCommand() == ProcessCmd_c::setValue)
  { // setpoint set
    bool b_change = false;
    switch (c_pkg.mc_processCmd.getValueGroup())
    {
      case ProcessCmd_c::exactValue: // set actual setpoint
      case ProcessCmd_c::minValue: // min -> simply set exact
      case ProcessCmd_c::maxValue: // max -> simply set exact
      case ProcessCmd_c::defaultValue: // max -> simply set exact
        if ( setpointMasterVal() != c_pkg.getValue() )
        {
          setSetpointMasterVal(c_pkg.getValue());
          b_change = true;
        }
        break;
      default:
        break;
    }
    // call handler function if handler class is registered
    if ( processDataConst().getProcessDataChangeHandler() != NULL )
      processDataConst().getProcessDataChangeHandler()->processSetpointSet( pprocessData(), c_pkg.getValue(), c_pkg.memberSend().isoName().toConstIisoName_c(), b_change );
  }
  #ifndef SIMPLE_RESPOND_ON_SET
  // if no auto-response on setpoint set is want
  // send value group dependent value on request
  // if SIMPLE_RESPOND_ON_SET is defined, the actual setpoint shall be resend
  // on receive of a new setpoint --> "else" is NOT used in this case
  else
  #endif
  {
    sendSetpointForGroup(cc_senderISOName, c_pkg.mc_processCmd.getValueGroup(), ProcessCmd_c::setValue );
  }

}

/**
  send a sub-setpoint (selected by value group) to a specified target (selected by GPT)
  @param ac_targetISOName ISOName of target
  @param en_valueGroup: min/max/exact/default
  @param en_command
  @return true -> successful sent
*/
bool SimpleManageSetpointLocal_c::sendSetpointForGroup(const IsoName_c& ac_targetISOName,
                                                       ProcessCmd_c::ValueGroup_t en_valueGroup,
                                                       ProcessCmd_c::CommandType_t en_command ) const
{
  // prepare general command in process pkg
  getProcessInstance4Comm().data().mc_processCmd.setValues(true /* isSetpoint */, false, /* isRequest */
                                                           en_valueGroup, en_command);
  return processDataConst().sendValISOName(ac_targetISOName, setpointMasterVal());
}


/**
  retreive simple master setpoint
  @return actual received setpoint value
*/
int32_t SimpleManageSetpointLocal_c::setpointMasterVal() const
{
#ifndef HANDLE_SETPOINT_MEASURE_EQUIVALENT
return mi32_setpointMasterVal;
#else // HANDLE_SETPOINT_MEASURE_EQUIVALENT
ProcDataLocalBase_c& c_localProcBase = static_cast<ProcDataLocalBase_c&>(processData());
uint32_t ui32_masterVal = c_localProcBase.masterMeasurementVal();
return mi32_masterVal;
#endif // HANDLE_SETPOINT_MEASURE_EQUIVALENT
}

/**
  set the setpoint value
  @param ai32_val new setpoint value
*/
void SimpleManageSetpointLocal_c::setSetpointMasterVal(int32_t ai32_val)
{
#ifndef HANDLE_SETPOINT_MEASURE_EQUIVALENT
mi32_setpointMasterVal = ai32_val;
#else // HANDLE_SETPOINT_MEASURE_EQUIVALENT
ProcDataLocalBase_c& c_localProcBase = static_cast<ProcDataLocalBase_c&>(processData());
c_localProcBase.setMasterMeasurementVal( ai32_val );
#endif // HANDLE_SETPOINT_MEASURE_EQUIVALENT
}


} // end of namespace __IsoAgLib
