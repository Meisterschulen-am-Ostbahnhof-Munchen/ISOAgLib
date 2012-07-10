/*
  setpointregister_c.cpp: stores one setpoint with management
    informations

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include "setpointregister_c.h"

namespace __IsoAgLib {
/**
  initialise this SetpointRegister_c to a well defined starting condition
  @param acrc_isoName device key of commander of this setpoint register set
  @param ai32_exact exact setpoint value
  @param ai32_min minimum setpoint value
  @param ai32_max maximum setpoint value
  @param ai32_default default setpoint value
  @param ab_handled true -> this setpoint register nistance was handled by main application
  @param ab_master true -> this setpoint register instance represents the actual master setpoint
  @param ab_valid true -> this setpoint register instance is accepted as valid
*/
void SetpointRegister_c::init(const IsoName_c& acrc_isoName, int32_t ai32_exact, int32_t ai32_min, int32_t ai32_max, int32_t ai32_default,
        bool ab_handled, bool ab_master, bool ab_valid)
{ // direct value set to avoid special functions of equivalent set functions
  setExact(ai32_exact);
  setMin(ai32_min);
  setMax(ai32_max);
  setDefault(ai32_default);

  setISOName(acrc_isoName);
  setHandled(ab_handled, 0);
  setMaster(ab_master);
  setValid(ab_valid);
}

/**
  operator= for SetpointRegister_c class
  @param acrc_src source SetpointRegister_c instance
  @return reference to source instance for cmd like "setp1 = setp2 = setp3;"
*/
const SetpointRegister_c& SetpointRegister_c::operator=(const SetpointRegister_c& acrc_src){
  assignFromSource(acrc_src);
  return *this;
}

/**
  copy constructor for SetpointRegister_c class
  @param acrc_src source SetpointRegister_c instance
*/
SetpointRegister_c::SetpointRegister_c(const SetpointRegister_c& acrc_src){
  assignFromSource(acrc_src);
}
/** base function for assignment of element vars for copy constructor and operator= */
void SetpointRegister_c::assignFromSource( const SetpointRegister_c& acrc_src )
{ // direct value set to avoid special functions of equivalent set functions
  mi32_exactOrMin = acrc_src.mi32_exactOrMin;
  mi32_max = acrc_src.mi32_max;
  mi32_default = acrc_src.mi32_default;
  data.en_definedSetpoints = acrc_src.data.en_definedSetpoints;


  setISOName(acrc_src.isoName());
  setHandled(acrc_src.handled(), acrc_src.mi32_lastHandledTime);
  setMaster(acrc_src.master());
  setValid(acrc_src.valid());
}

/** default destructor which has nothing to do */
SetpointRegister_c::~SetpointRegister_c(){
}

/**
  compare two Setpoint Register items by value
  @param acrc_src compared instance
  @return true -> both setpoint sets are equal
*/
bool SetpointRegister_c::operator==(const SetpointRegister_c& acrc_src)const{
  return ((mi32_exactOrMin == acrc_src.mi32_exactOrMin)
        && (mi32_max == acrc_src.mi32_max)
        && (mi32_default == acrc_src.mi32_default)
        && (data.en_definedSetpoints == acrc_src.data.en_definedSetpoints)
        && (isoName() == acrc_src.isoName()))
;
}

/* ************************************ */
/* ***reading member variable access*** */
/* ************************************ */

/**
  deliver the setpoint according to the value group
  @param en_valueGroup code of wanted setpoint (exact 0, min 2, max 3, default)
  @return setpoint selected by value group
*/
int32_t SetpointRegister_c::valForGroup(ProcessCmd_c::ValueGroup_t en_valueGroup) const{
  switch (en_valueGroup)
  {
    case ProcessCmd_c::exactValue:
      return exact();
    case ProcessCmd_c::minValue:
      return min();
    case ProcessCmd_c::defaultValue:
      return getDefault();
    case ProcessCmd_c::maxValue:
    default:
      return max();
  }
}

/**
  checks if setpoint with value group en_valueGroup exists
  @param en_valueGroup value group of tested setpoint type (exact 0, min 2, max 3, default)
  @return true -> a setpoint for this valueGroup exists
*/
bool SetpointRegister_c::valueGroupExists(ProcessCmd_c::ValueGroup_t en_valueGroup) const{
  switch (en_valueGroup)
  {
    case ProcessCmd_c::exactValue:
      return ((data.en_definedSetpoints & exactType) != 0);
    case ProcessCmd_c::minValue:
      return ((data.en_definedSetpoints & minType) != 0);
    case ProcessCmd_c::defaultValue:
      return ((data.en_definedSetpoints & defaultType) != 0);
    case ProcessCmd_c::maxValue:
      return ((data.en_definedSetpoints & maxType) != 0);
    default:
      return false;
  }
}

/* ************************************ */
/* ***writing member variable access*** */
/* ************************************ */

/**
  set the exact setpoint value
  @param ai32_val new exact setpoint value
*/
void SetpointRegister_c::setExact(int32_t ai32_val)
{
  if (ai32_val != NO_VAL_32S)
  {
    mi32_exactOrMin = ai32_val;
    // clear min/max
    // set exactType
    // keep defaultType
    data.en_definedSetpoints
      = static_cast<definedSetpoints_t>(exactType | (data.en_definedSetpoints & defaultType));
  }
  else
  { // only let bits of minType, maxType, defaultType
    data.en_definedSetpoints
      = static_cast<definedSetpoints_t>(data.en_definedSetpoints & minMaxDefaultType);
  }
  // set the entry to unhandled
  setHandled(false);
};

/**
  set the minimum setpoint value
  @param ai32_val new minimum setpoint value
*/
void SetpointRegister_c::setMin(int32_t ai32_val)
{
  if (ai32_val != NO_VAL_32S)
  {
    mi32_exactOrMin = ai32_val;
    // clear exactType
    // don't change maxType and defaultType
    // set minType
    data.en_definedSetpoints
      = static_cast<definedSetpoints_t>(minType | (data.en_definedSetpoints & maxDefaultType));
  }
  else
  { // clear minType -> mask with exactMaxDefaultType
    data.en_definedSetpoints
      = static_cast<definedSetpoints_t>(data.en_definedSetpoints & exactMaxDefaultType);
  }

  // set the entry to unhandled
  setHandled(false);
};

/**
  set the maximum setpoint value
  @param ai32_val new maximum setpoint value
*/
void SetpointRegister_c::setMax(int32_t ai32_val)
{
  if (ai32_val != NO_VAL_32S)
  {
    mi32_max = ai32_val;
    // set exact to empty
    // don't change minType and defaultType
    // set maxType
    data.en_definedSetpoints
      = static_cast<definedSetpoints_t>(maxType | (data.en_definedSetpoints & minDefaultType));
  }
  else
  { // clear maxType -> mask with exactMinDefaultType
    data.en_definedSetpoints
      = static_cast<definedSetpoints_t>(data.en_definedSetpoints & exactMinDefaultType);
  }
  // set the entry to unhandled
  setHandled(false);
};

/**
  set the default setpoint value
  @param ai32_val new default setpoint value
*/
void SetpointRegister_c::setDefault(int32_t ai32_val)
{
  if (ai32_val != NO_VAL_32S)
  {
    mi32_default = ai32_val;
    // set defaultType
    // don't change minType, maxType, exactType
    data.en_definedSetpoints
      = static_cast<definedSetpoints_t>(defaultType | (data.en_definedSetpoints & exactMinMaxType));
  }
  else
  { // clear defaultType -> mask with exactMinMaxType
    data.en_definedSetpoints
      = static_cast<definedSetpoints_t>(data.en_definedSetpoints & exactMinMaxType);
  }
  // set the entry to unhandled
  setHandled(false);
};

/**
  set a limit val for type given by value group
  @param ai32_val new setpoint value
  @param en_valueGroup code of setpoint type to set (exact 0, min 2, max 3, default)
*/
void SetpointRegister_c::setValForGroup(int32_t ai32_val, ProcessCmd_c::ValueGroup_t en_valueGroup){
  switch (en_valueGroup)
  {
    case ProcessCmd_c::exactValue:
      setExact(ai32_val);
      break;
    case ProcessCmd_c::minValue:
      setMin(ai32_val);
      break;
    case ProcessCmd_c::maxValue:
      setMax(ai32_val);
      break;
    case ProcessCmd_c::defaultValue:
      setDefault(ai32_val);
      break;
    default: ;
  }
}


/**
  set the handled state; return if state was changed
  @param ab_state true -> mark this setpoint as handled
  @param ai32_handledTime timestamp for detecting the last setHandle event
  @return true -> this call caused a state change for handled state
*/
bool SetpointRegister_c::setHandled(bool ab_state, int32_t ai32_handledTime)
{
  if (ai32_handledTime >= 0)mi32_lastHandledTime = ai32_handledTime;
  if (handled() != ab_state)
  { // state was changed
    data.b_handled = ab_state;
    return true;
  }
  else
  {  //nothing has changed (f.e. -> no acknowledge must be sent to requester)
    return false;
  }
}
/**
  set the master state; return if state was changed
  @param ab_state true -> mark this setpoint as master
  @return true -> this call caused a state change for master state
*/
bool SetpointRegister_c::setMaster(bool ab_state)
{
  if (master() != ab_state)
  { // state was changed
    data.b_master = ab_state;
    return true;
  }
  else
  {  //nothing has changed (f.e. -> no acknowledge must be sent to requester)
    return false;
  }
}
/**
  set the valid state; return if state was changed
  @param ab_state true -> mark this setpoint as valid (accepted)
  @return true -> this call caused a state change for valid state
*/
bool SetpointRegister_c::setValid(bool ab_state)
{
  if (valid() != ab_state)
  { // state was changed
    data.b_valid = ab_state;
    return true;
  }
  else
  {  //nothing has changed (f.e. -> no acknowledge must be sent to requester)
    return false;
  }
}

} // end of namespace __IsoAgLib
