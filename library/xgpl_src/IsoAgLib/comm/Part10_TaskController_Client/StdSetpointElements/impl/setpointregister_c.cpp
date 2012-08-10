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

void SetpointRegister_c::init(const IsoName_c& acrc_isoName, int32_t ai32_value, bool ab_valid)
{ // direct value set to avoid special functions of equivalent set functions
  setValue(ai32_value);

  setISOName(acrc_isoName);
  setValid(ab_valid);
}

const SetpointRegister_c& SetpointRegister_c::operator=(const SetpointRegister_c& acrc_src){
  assignFromSource(acrc_src);
  return *this;
}

SetpointRegister_c::SetpointRegister_c(const SetpointRegister_c& acrc_src){
  assignFromSource(acrc_src);
}
/** base function for assignment of element vars for copy constructor and operator= */
void SetpointRegister_c::assignFromSource( const SetpointRegister_c& acrc_src )
{ // direct value set to avoid special functions of equivalent set functions
  mi32_exactOrMin = acrc_src.mi32_exactOrMin;
  mi32_max = acrc_src.mi32_max;
  mi32_default = acrc_src.mi32_default;

  setISOName(acrc_src.isoName());
  setValid(acrc_src.valid());
}

SetpointRegister_c::~SetpointRegister_c(){
}

bool SetpointRegister_c::operator==(const SetpointRegister_c& acrc_src)const{
  return ((mi32_exactOrMin == acrc_src.mi32_exactOrMin)
        && (mi32_max == acrc_src.mi32_max)
        && (mi32_default == acrc_src.mi32_default)
        && (isoName() == acrc_src.isoName()))
;
}

void SetpointRegister_c::setValue(int32_t ai32_val)
{
  if (ai32_val != NO_VAL_32S)
  {
    mi32_exactOrMin = ai32_val;
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
