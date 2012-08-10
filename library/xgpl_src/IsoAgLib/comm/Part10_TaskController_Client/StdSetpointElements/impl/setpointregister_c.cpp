/*
  setpointregister_c.cpp: stores one setpoint with management informations

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#include "setpointregister_c.h"

namespace __IsoAgLib {

void SetpointRegister_c::init(IsoName_c::ecuType_t ecuType, int32_t ai32_value)
{
  setValue(ai32_value);
  setISONameType(ecuType);
}

void SetpointRegister_c::setValue(int32_t ai32_val)
{
  if (ai32_val != NO_VAL_32S) // @TODO remove ?
  {
    mi32_value = ai32_val;
  }
};

} // end of namespace __IsoAgLib
