/***************************************************************************
                          setpointbase_c.cpp - base class for management
                                             of setpoints
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

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include "setpointbase_c.h"
#include "setpointregister_c.h"
#include "../../impl/process_c.h"
#include "../../impl/processcmd_c.h"

namespace __IsoAgLib {

/**
  default constructor which can set all member vars
  @param apc_processData optional pointer to containing ProcessData instance
*/
SetpointBase_c::SetpointBase_c(
    ProcDataBase_c *const apc_processData)
    : ProcessElementBase_c( apc_processData){
}

/**
  default constructor which can set all member vars
  @param arc_processData optional reference to containing ProcessData instance
*/
SetpointBase_c::SetpointBase_c(
    ProcDataBase_c &arc_processData)
    : ProcessElementBase_c( arc_processData){
}

/**
  assignement from another object
  @param acrc_src source SetpointBase_c instance
  @return reference to source for cmd like "setp1 = setp2 = setp3;"
*/
const SetpointBase_c& SetpointBase_c::operator=(const SetpointBase_c& acrc_src){
  // call the base class operator
  ProcessElementBase_c::operator=(acrc_src);

  // return reference
  return acrc_src;
}

/**
  copy constructor for SetpointBase
  @param acrc_src source SetpointBase_c instance
*/
SetpointBase_c::SetpointBase_c( const SetpointBase_c& acrc_src)
  : ProcessElementBase_c( acrc_src){
}

/** default destructor which has nothing to do */
SetpointBase_c::~SetpointBase_c(){
}

/**
  init this item after the containing object item
  was inserted in a list completely
  @param apc_data pointer to containing ProcessData instance
*/
void SetpointBase_c::init( ProcDataBase_c* apc_data )
{
  set( apc_data );
}

/**
  perform periodic actions
  @return true -> all planned executions performed
*/
bool SetpointBase_c::timeEvent( void ){
  return true;
}

/** process a setpoint message */
void SetpointBase_c::processMsg(){
  // check if its a request for actual setpoint
  if (getProcessInstance4Comm().data().mc_processCmd.checkIsRequest())
  {
    processRequest();
  }
  else
  { // set setpoint value
    processSet();
  }
}

/**
  send the values of an setpoint entry; if wanted
  the values can be overridden with a special value

  possible errors:
      * dependant error in ProcDataBase_c commander of this setpoint isn't found in Monitor List
      * dependant error in CanIo_c on CAN send problems
  @param acrc_src reference to SetpointRegister_c with registered setpoints
  @param ab_override true -> override registered setpoint with ai32_overrideVal
  @param ai32_overrideVal value which can override registered setpoint on ab_override == true
*/
void SetpointBase_c::sendSetpointVals( const SetpointRegister_c& acrc_src,
                                       bool ab_override, int32_t ai32_overrideVal) const
{
  int32_t i32_value = 0;
  bool b_isCmd = false;
  ProcessCmd_c::ValueGroup_t en_valueGroup = ProcessCmd_c::noValue;

  if ((ai32_overrideVal == SETPOINT_RELEASE_COMMAND)
   || (ai32_overrideVal == SETPOINT_ERROR_COMMAND)
      ) b_isCmd = true;

  if (acrc_src.existExact())
  { // exact setpoint exist
    en_valueGroup = ProcessCmd_c::exactValue;
    i32_value = (ab_override) ? ai32_overrideVal : acrc_src.exact();
  }
  else if (acrc_src.existMin())
  { // min setpoint exist
    en_valueGroup = ProcessCmd_c::minValue;
    i32_value = (ab_override) ? ai32_overrideVal : acrc_src.min();
  }
  else if (acrc_src.existMax())
  { // max setpoint exist
     en_valueGroup = ProcessCmd_c::maxValue;
     i32_value = (ab_override) ? ai32_overrideVal : acrc_src.max();
  }
  else if (acrc_src.existDefault())
  { // default setpoint exist
     en_valueGroup = ProcessCmd_c::defaultValue;
     i32_value = (ab_override) ? ai32_overrideVal : acrc_src.getDefault();
  }

  if (en_valueGroup != ProcessCmd_c::noValue)
  {
     // prepare general command in process pkg
     getProcessInstance4Comm().data().mc_processCmd.setValues(true /* isSetpoint */, false /* isRequest */,
                                                              en_valueGroup, ProcessCmd_c::setValue);

     pprocessData()->sendValISOName( acrc_src.isoName(), i32_value);
  }
}


} // end of namespace __IsoAgLib
