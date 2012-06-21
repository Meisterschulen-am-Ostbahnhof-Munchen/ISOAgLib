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


bool
System_c::close()
{
  if ( !HAL::isSystemOpened() )
    return false;

  HAL::powerHold( false );
  HAL::closeSystem();

  return true;
}


bool
System_c::init()
{
  if ( HAL::isSystemOpened() )
    return false;

  if ( HAL::open_system() != HAL_NO_ERR)
  {
    getILibErrInstance().registerError( iLibErr_c::SystemOpen, iLibErr_c::HwSystem ); // something is still wrong
    return false;
  }

  // init and config the Watchdog
  if (!initWd())
    return false;

  // start the task timer (also needed for CAN)
  HAL::startTaskTimer();

  HAL::powerHold( true );

  return true;
}


bool
System_c::initWd ( void )
{
  bool b_result = true;
  // perform the watchdog config (BIOS function)
  switch(HAL::configWatchdog())
  {
    case HAL_WD_DATA_CHANGED:
      // watchdog must be restarted to use the new values
      HAL::wdReset();
      break;
    case HAL_NO_ERR:
      // do nothing because nothing changed
      break;
    default:
      // something went wrong -> system runs with default values
      getILibErrInstance().registerError( iLibErr_c::SystemWatchdog, iLibErr_c::HwSystem );
      b_result = false;
      break;
  }
  return b_result;
}

} // end of namespace __IsoAgLib
