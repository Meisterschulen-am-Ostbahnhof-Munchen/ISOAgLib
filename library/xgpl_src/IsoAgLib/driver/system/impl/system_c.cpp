/*
  system_c.cpp: central object for encapsulation of platform dependent
    elements (mostly BIOS)

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include "system_c.h"


namespace __IsoAgLib {

/** C-style function, to get access to the unique System_c singleton instance */
System_c &getSystemInstance(uint8_t aui8_instance)
{
  MACRO_MULTITON_GET_INSTANCE_BODY(System_c, 1, aui8_instance);
}


void
System_c::close()
{
  isoaglib_assert( HAL::isSystemOpened() );
  HAL::powerHold( false );
  HAL::closeSystem();
}


void 
System_c::init()
{
  isoaglib_assert( ! HAL::isSystemOpened() );

  HAL::openSystem();
  HAL::startTaskTimer();
  HAL::powerHold( true );
  initWd();
}

} // end of namespace __IsoAgLib
