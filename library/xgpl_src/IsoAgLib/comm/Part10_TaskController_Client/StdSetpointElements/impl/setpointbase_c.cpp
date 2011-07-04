/*
  setpointbase_c.cpp: base class for management of setpoints

  (C) Copyright 2009 - 2011 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include "setpointbase_c.h"
#include "setpointregister_c.h"
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/process_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/processcmd_c.h>

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
void SetpointBase_c::processMsg( const ProcessPkg_c& pkg ){
  // check if its a request for actual setpoint
  if (pkg.mc_processCmd.checkIsRequest())
  {
    processRequest(pkg);
  }
  else
  { // set setpoint value
    processSet(pkg);
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
//currently b_isCmd is set, but not used anywhere - just left there for reference...
//bool b_isCmd = false;
  ProcessCmd_c::ValueGroup_t en_valueGroup = ProcessCmd_c::noValue;

//if ((ai32_overrideVal == SETPOINT_RELEASE_COMMAND)
// || (ai32_overrideVal == SETPOINT_ERROR_COMMAND)
//    ) b_isCmd = true;

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
     ProcessPkg_c pkg;
     // prepare general command in process pkg
     pkg.mc_processCmd.setValues(true /* isSetpoint */, false /* isRequest */,
                                                              en_valueGroup, ProcessCmd_c::setValue);

     pprocessData()->sendValISOName( pkg, acrc_src.isoName(), i32_value);
  }
}


} // end of namespace __IsoAgLib
