/*
  counteri_c.cpp - implementation file for CounterI_c

  (C) Copyright 2009 - 2015 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#include "counteri_c.h"

namespace __IsoAgLib {

CounterI_c::cin_err_t
CounterI_c::getState() const
{
  switch ( HAL::getCounterDiagState( channelNr() ) )
  {
    case HAL_COUNTERIN_OPEN:       return cin_openErr;
    case HAL_COUNTERIN_SHORTCUT:   return cin_shortcutErr;
    case HAL_COUNTERIN_OVERVOLT:   return cin_overvoltErr;
    case HAL_COUNTERIN_UNDERVOLT:  return cin_undervoltErr;
    default: return noCinErr;
  }
}

} // __IsoAgLib
