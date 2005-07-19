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
  @param rc_gtp GET_POS of commander of this setpoint register set
  @param ri32_exact exact setpoint value
  @param ri32_min minimum setpoint value
  @param ri32_max maximum setpoint value
  @param ri32_default default setpoint value
  @param rb_handled true -> this setpoint register nistance was handled by main application
  @param rb_master true -> this setpoint register instance represents the actual master setpoint
  @param rb_valid true -> this setpoint register instance is accepted as valid
*/
void SetpointRegister_c::init(GetyPos_c rc_gtp, int32_t ri32_exact, int32_t ri32_min, int32_t ri32_max, int32_t ri32_default,
        bool rb_handled, bool rb_master, bool rb_valid)
{ // direct value set to avoid special functions of equivalent set functions
  setExact(ri32_exact);
  setMin(ri32_min);
  setMax(ri32_max);
  setDefault(ri32_default);

  setGtp(rc_gtp);
  setHandled(rb_handled, 0);
  setMaster(rb_master);
  setValid(rb_valid);
}

/**
  operator= for SetpointRegister_c class
  @param rrefc_src source SetpointRegister_c instance
  @return reference to source instance for cmd like "setp1 = setp2 = setp3;"
*/
const SetpointRegister_c& SetpointRegister_c::operator=(const SetpointRegister_c& rrefc_src){
  assignFromSource(rrefc_src);
  return *this;
}

/**
  copy constructor for SetpointRegister_c class
  @param rrefc_src source SetpointRegister_c instance
*/
SetpointRegister_c::SetpointRegister_c(const SetpointRegister_c& rrefc_src){
  assignFromSource(rrefc_src);
}
/** base function for assignment of element vars for copy constructor and operator= */
void SetpointRegister_c::assignFromSource( const SetpointRegister_c& rrefc_src )
{ // direct value set to avoid special functions of equivalent set functions
  i32_exactOrMin = rrefc_src.i32_exactOrMin;
  i32_max = rrefc_src.i32_max;
  i32_default = rrefc_src.i32_default;
  data.en_definedSetpoints = rrefc_src.data.en_definedSetpoints;


  setGtp(rrefc_src.gtp());
  setHandled(rrefc_src.handled(), rrefc_src.i32_lastHandledTime);
  setMaster(rrefc_src.master());
  setValid(rrefc_src.valid());
}

/** default destructor which has nothing to do */
SetpointRegister_c::~SetpointRegister_c(){
}

/**
  compare two Setpoint Register items by value
  @param rrefc_src compared instance
  @return true -> both setpoint sets are equal
*/
bool SetpointRegister_c::operator==(const SetpointRegister_c& rrefc_src)const{
  return ((i32_exactOrMin == rrefc_src.i32_exactOrMin)
        && (i32_max == rrefc_src.i32_max)
        && (i32_default == rrefc_src.i32_default)
        && (data.en_definedSetpoints == rrefc_src.data.en_definedSetpoints)
        && (gtp() == rrefc_src.gtp()))
        ? true:false;
}

/* ************************************ */
/* ***reading member variable access*** */
/* ************************************ */

/**
  deliver the setpoint according to the mod type
  @param en_valueGroup code of wanted setpoint (exact 0, min 2, max 3, default)
  @return setpoint selected by MOD
*/
int32_t SetpointRegister_c::valMod(GeneralCommand_c::ValueGroup_t en_valueGroup) const{
  switch (en_valueGroup)
  {
    case GeneralCommand_c::exactValue:
      return exact();
    case GeneralCommand_c::minValue:
      return min();
    case GeneralCommand_c::defaultValue:
      return getDefault();
    case GeneralCommand_c::maxValue:
    default:
      return max();
  }
}

#ifdef USE_FLOAT_DATA_TYPE
/**
  deliver the setpoint according to the mod type
  @param en_valueGroup code of wanted setpoint (exact 0, min 2, max 3, default)
  @return setpoint selected by MOD
*/
float SetpointRegister_c::valModFloat(GeneralCommand_c::ValueGroup_t en_valueGroup)const{
  int32_t i32_temp = valMod(en_valueGroup);
  return *((float*)(&i32_temp));;
}
#endif
/**
  checks if setpoint with type rb_mod exists
  @param en_valueGroup value group of tested setpoint type (exact 0, min 2, max 3, default)
  @return true -> a MOD type setpoint exist
*/
// @todo: rename
bool SetpointRegister_c::existValMod(GeneralCommand_c::ValueGroup_t en_valueGroup) const{
  switch (en_valueGroup)
  {
    case GeneralCommand_c::exactValue:
      return ((data.en_definedSetpoints & exactType) != 0)?true:false;
    case GeneralCommand_c::minValue:
      return ((data.en_definedSetpoints & minType) != 0)?true:false;
    case GeneralCommand_c::defaultValue:
      return ((data.en_definedSetpoints & defaultType) != 0)?true:false;
    case GeneralCommand_c::maxValue:
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
  @param ri32_val new exact setpoint value
*/
void SetpointRegister_c::setExact(int32_t ri32_val)
{
  if (ri32_val != NO_VAL_32S)
  {
    i32_exactOrMin = ri32_val;
    // clear min/max
    // set exactType
    data.en_definedSetpoints = exactType;
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
  @param ri32_val new minimum setpoint value
*/
void SetpointRegister_c::setMin(int32_t ri32_val)
{
  if (ri32_val != NO_VAL_32S)
  {
    i32_exactOrMin = ri32_val;
    // clear exactType
    // don't change maxType
    // set minType
    data.en_definedSetpoints
      = static_cast<definedSetpoints_t>(minType | (data.en_definedSetpoints & maxType));
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
  @param ri32_val new maximum setpoint value
*/
void SetpointRegister_c::setMax(int32_t ri32_val)
{
  if (ri32_val != NO_VAL_32S)
  {
    i32_max = ri32_val;
    // set exact to empty
    // don't change minType
    // set maxType
    data.en_definedSetpoints
      = static_cast<definedSetpoints_t>(maxType | (data.en_definedSetpoints & minType));
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
  @param ri32_val new default setpoint value
*/
void SetpointRegister_c::setDefault(int32_t ri32_val)
{
  if (ri32_val != NO_VAL_32S)
  {
    i32_default = ri32_val;
    // set defaultType
    data.en_definedSetpoints = defaultType;
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
  set a limit val for type given by rb_mod
  @param ri32_val new setpoint value
  @param en_valueGroup code of setpoint type to set (exact 0, min 2, max 3, default)
*/
void SetpointRegister_c::setValMod(int32_t ri32_val, GeneralCommand_c::ValueGroup_t en_valueGroup){
  switch (en_valueGroup)
  {
    case GeneralCommand_c::exactValue:
      setExact(ri32_val);
      break;
    case GeneralCommand_c::minValue:
      setMin(ri32_val);
      break;
    case GeneralCommand_c::maxValue:
      setMax(ri32_val);
      break;
    case GeneralCommand_c::defaultValue:
      setDefault(ri32_val);
      break;
  }
}

#ifdef USE_FLOAT_DATA_TYPE

/**
  set the exact setpoint value
  @param rf_val new exact setpoint value
*/
void SetpointRegister_c::setExact(float rf_val)
{
  int32_t i32_temp = (*(int32_t*)(&rf_val));
  setExact(i32_temp);
}
/**
  set the minimum setpoint value
  @param rf_val new minimum setpoint value
*/
void SetpointRegister_c::setMin(float rf_val)
{
  int32_t i32_temp = (*(int32_t*)(&rf_val));
  setMin(i32_temp);
}
/**
  set the maximum setpoint value
  @param rf_val new maximum setpoint value
*/
void SetpointRegister_c::setMax(float rf_val)
{
  int32_t i32_temp = (*(int32_t*)(&rf_val));
  setMax(i32_temp);
}
/**
  set the default setpoint value
  @param rf_val new default setpoint value
*/
void SetpointRegister_c::setDefault(float rf_val)
{
  int32_t i32_temp = (*(int32_t*)(&rf_val));
  setDefault(i32_temp);
}

/**
  set a limit val for type given by rb_mod
  @param rf_val new setpoint value
  @param en_valueGroup MOD code of setpoint type to set (exact 0, min 2, max 3)
*/

void SetpointRegister_c::setValMod(float rf_val, GeneralCommand_c::ValueGroup_t en_valueGroup){
  switch (en_valueGroup)
  {
    case GeneralCommand_c::exactValue:
      setExact(rf_val);
      break;
    case GeneralCommand_c::minValue:
      setMin(rf_val);
      break;
    case GeneralCommand_c::maxValue:
      setMax(rf_val);
      break;
    case GeneralCommand_c::defaultValue:
      setDefault(rf_val);
      break;
  }
}

#endif

/**
  set the handled state; return if state was changed
  @param rb_state true -> mark this setpoint as handled
  @param ri32_handledTime timestamp for detecting the last setHandle event
  @return true -> this call caused a state change for handled state
*/
bool SetpointRegister_c::setHandled(bool rb_state, int32_t ri32_handledTime)
{
  if (ri32_handledTime >= 0)i32_lastHandledTime = ri32_handledTime;
  if (handled() != rb_state)
  { // state was changed
    data.b_handled = rb_state;
    return true;
  }
  else
  {  //nothing has changed (f.e. -> no acknowledge must be sent to requester)
    return false;
  }
}
/**
  set the master state; return if state was changed
  @param rb_state true -> mark this setpoint as master
  @return true -> this call caused a state change for master state
*/
bool SetpointRegister_c::setMaster(bool rb_state)
{
  if (master() != rb_state)
  { // state was changed
    data.b_master = rb_state;
    return true;
  }
  else
  {  //nothing has changed (f.e. -> no acknowledge must be sent to requester)
    return false;
  }
}
/**
  set the valid state; return if state was changed
  @param rb_state true -> mark this setpoint as valid (accepted)
  @return true -> this call caused a state change for valid state
*/
bool SetpointRegister_c::setValid(bool rb_state)
{
  if (valid() != rb_state)
  { // state was changed
    data.b_valid = rb_state;
    return true;
  }
  else
  {  //nothing has changed (f.e. -> no acknowledge must be sent to requester)
    return false;
  }
}

} // end of namespace __IsoAgLib
