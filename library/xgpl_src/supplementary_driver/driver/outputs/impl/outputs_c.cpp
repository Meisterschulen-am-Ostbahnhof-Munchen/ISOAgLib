/*
  outputs_c.cpp:
    implementation for the outputs management object

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
#include "outputs_c.h"

#include <IsoAgLib/util/iliberr_c.h>
#include <IsoAgLib/driver/system/impl/system_c.h>
#include <supplementary_driver/hal/hal_outputs.h>


namespace __IsoAgLib {

Outputs_c &
getOutputsInstance()
{
  MACRO_SINGLETON_GET_INSTANCE_BODY(Outputs_c);
}


void
Outputs_c::setMainRelais( bool ab_active )
{
  HAL::setMainRelais( ab_active );
}


} // __IsoAgLib
