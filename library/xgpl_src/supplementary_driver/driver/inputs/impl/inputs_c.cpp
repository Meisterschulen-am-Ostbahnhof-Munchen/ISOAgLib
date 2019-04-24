/*
  inputs_c.cpp - implmentation for the inputs management

  (C) Copyright 2009 - 2019 by OSB AG

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Usage under Commercial License:
  Licensees with a valid commercial license may use this file
  according to their commercial license agreement. (To obtain a
  commercial license contact OSB AG via <http://isoaglib.com/en/contact>)

  Usage under GNU General Public License with exceptions for ISOAgLib:
  Alternatively (if not holding a valid commercial license)
  use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include "inputs_c.h"
#include <IsoAgLib/util/impl/singleton.h>



namespace __IsoAgLib {


Inputs_c &
getInputsInstance()
{
  MACRO_SINGLETON_GET_INSTANCE_BODY(Inputs_c);
}


} // __IsoAgLib
