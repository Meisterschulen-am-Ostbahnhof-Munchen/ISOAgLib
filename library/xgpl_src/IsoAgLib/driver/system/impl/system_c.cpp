/*
  system_c.cpp: module for a interfacing the system hardware

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
#include "system_c.h"

#include <IsoAgLib/util/iliberr_c.h>
#include <IsoAgLib/util/impl/multiton.h>


namespace __IsoAgLib {

System_c& getSystemInstance()
{
  MACRO_SINGLETON_GET_INSTANCE_BODY(System_c);
}

void
System_c::init()
{
  isoaglib_assert( ! HAL::isSystemOpened() );
  HAL::openSystem();
  HAL::powerHold( true );
}

void
System_c::close()
{
  isoaglib_assert( HAL::isSystemOpened() );
  HAL::powerHold( false );
  HAL::closeSystem();
}

}
