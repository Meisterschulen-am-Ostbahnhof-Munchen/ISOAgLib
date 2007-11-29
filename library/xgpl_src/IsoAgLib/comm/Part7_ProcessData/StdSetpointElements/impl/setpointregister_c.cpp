/***************************************************************************
                          setpointregister_c.cpp - stores one setpoint with
                                                 management informations
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
#include "setpointregister_c.h"

namespace __IsoAgLib {
/**
  initialise this SetpointRegister_c to a well defined starting condition
  @param ac_isoName device key of commander of this setpoint register set
  @param ai32_exact exact setpoint value
  @param ai32_min minimum setpoint value
  @param ai32_max maximum setpoint value
  @param ai32_default default setpoint value
  @param ab_handled true -> this setpoint register nistance was handled by main application
  @param ab_master true -> this setpoint register instance represents the actual master setpoint
  @param ab_valid true -> this setpoint register instance is accepted as valid
*/
void SetpointRegister_c::init(const IsoName_c& ac_isoName, int32_t ai32_exact, int32_t ai32_min, int32_t ai32_max, int32_t ai32_default,
        bool ab_handled, bool ab_master, bool ab_valid)
{ // direct value set to avoid special functions of equivalent set functions
  setExact(ai32_exact);
  setMin(ai32_min);
  setMax(ai32_max);
  setDefault(ai32_default);

  setISOName(ac_isoName);
  setHandled(ab_handled, 0);
  setMaster(ab_master);
  setValid(ab_valid);
}

/**
  operator= for SetpointRegister_c class
  @param arc_src source SetpointRegister_c instance
  @return reference to source instance for cmd like "setp1 = setp2 = setp3;"
*/
const SetpointRegister_c& SetpointRegister_c::operator=(const SetpointRegister_c& arc_src){
  assignFromSource(arc_src);
  return *this;
}

/**
  copy constructor for SetpointRegister_c class
  @param arc_src source SetpointRegister_c instance
*/
SetpointRegister_c::SetpointRegister_c(const SetpointRegister_c& arc_src){
  assignFromSource(arc_src);
}
/** base function for assignment of element vars for copy constructor and operator= */
void SetpointRegister_c::assignFromSource( const SetpointRegister_c& arc_src )
{ // direct value set to avoid special functions of equivalent set functions
  mi32_exactOrMin = arc_src.mi32_exactOrMin;
  mi32_max = arc_src.mi32_max;
  mi32_default = arc_src.mi32_default;
  data.en_definedSetpoints = arc_src.data.en_definedSetpoints;


  setISOName(arc_src.isoName());
  setHandled(arc_src.handled(), arc_src.mi32_lastHandledTime);
  setMaster(arc_src.master());
  setValid(arc_src.valid());
}

/** default destructor which has nothing to do */
SetpointRegister_c::~SetpointRegister_c(){
}

/**
  compare two Setpoint Register items by value
  @param arc_src compared instance
  @return true -> both setpoint sets are equal
*/
bool SetpointRegister_c::operator==(const SetpointRegister_c& arc_src)const{
  return ((mi32_exactOrMin == arc_src.mi32_exactOrMin)
        && (mi32_max == arc_src.mi32_max)
        && (mi32_default == arc_src.mi32_default)
        && (data.en_definedSetpoints == arc_src.data.en_definedSetpoints)
        && (isoName() == arc_src.isoName()))
        ? true:false;
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

#ifdef USE_FLOAT_DATA_TYPE
/**
  deliver the setpoint according to the value group
  @param en_valueGroup code of wanted setpoint (exact 0, min 2, max 3, default)
  @return setpoint selected by value group
*/
float SetpointRegister_c::valForGroupFloat(ProcessCmd_c::ValueGroup_t en_valueGroup)const{
  int32_t i32_temp = valForGroup(en_valueGroup);
  return *((float*)(&i32_temp));;
}
#endif
/**
  checks if setpoint with value group en_valueGroup exists
  @param en_valueGroup value group of tested setpoint type (exact 0, min 2, max 3, default)
  @return true -> a setpoint for this valueGroup exists
*/
bool SetpointRegister_c::valueGroupExists(ProcessCmd_c::ValueGroup_t en_valueGroup) const{
  switch (en_valueGroup)
  {
    case ProcessCmd_c::exactValue:
      return ((data.en_definedSetpoints & exactType) != 0)?true:false;
    case ProcessCmd_c::minValue:
      return ((data.en_definedSetpoints & minType) != 0)?true:false;
    case ProcessCmd_c::defaultValue:
      return ((data.en_definedSetpoints & defaultType) != 0)?true:false;
    case ProcessCmd_c::maxValue:
      return ((data.en_definedSetpoints & maxType) != 0)?true:false;
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

#ifdef USE_FLOAT_DATA_TYPE

/**
  set the exact setpoint value
  @param af_val new exact setpoint value
*/
void SetpointRegister_c::setExact(float af_val)
{
  int32_t i32_temp = (*(int32_t*)(&af_val));
  setExact(i32_temp);
}
/**
  set the minimum setpoint value
  @param af_val new minimum setpoint value
*/
void SetpointRegister_c::setMin(float af_val)
{
  int32_t i32_temp = (*(int32_t*)(&af_val));
  setMin(i32_temp);
}
/**
  set the maximum setpoint value
  @param af_val new maximum setpoint value
*/
void SetpointRegister_c::setMax(float af_val)
{
  int32_t i32_temp = (*(int32_t*)(&af_val));
  setMax(i32_temp);
}
/**
  set the default setpoint value
  @param af_val new default setpoint value
*/
void SetpointRegister_c::setDefault(float af_val)
{
  int32_t i32_temp = (*(int32_t*)(&af_val));
  setDefault(i32_temp);
}

/**
  set a limit val for type given by value group
  @param af_val new setpoint value
  @param en_valueGroup of setpoint type to set (exact 0, min 2, max 3)
*/

void SetpointRegister_c::setValForGroup(float af_val, ProcessCmd_c::ValueGroup_t en_valueGroup){
  switch (en_valueGroup)
  {
    case ProcessCmd_c::exactValue:
      setExact(af_val);
      break;
    case ProcessCmd_c::minValue:
      setMin(af_val);
      break;
    case ProcessCmd_c::maxValue:
      setMax(af_val);
      break;
    case ProcessCmd_c::defaultValue:
      setDefault(af_val);
      break;
    default: ;
  }
}

#endif

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
