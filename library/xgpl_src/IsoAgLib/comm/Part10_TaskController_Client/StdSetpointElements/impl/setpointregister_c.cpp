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

void SetpointRegister_c::init(IsoName_c::ecuType_t ecuType, int32_t ai32_value, bool ab_valid)
{ // direct value set to avoid special functions of equivalent set functions
  setValue(ai32_value);

  setISONameType(ecuType);
  setValid(ab_valid);
}

void SetpointRegister_c::setValue(int32_t ai32_val)
{
  if (ai32_val != NO_VAL_32S) // @TODO remove
  {
    mi32_value = ai32_val;
  }
};

bool SetpointRegister_c::setValid(bool ab_state)
{
  if (valid() != ab_state)
  { // state was changed
    b_valid = ab_state;
    return true;
  }
  else
  {  //nothing has changed (f.e. -> no acknowledge must be sent to requester)
    return false;
  }
}

} // end of namespace __IsoAgLib
