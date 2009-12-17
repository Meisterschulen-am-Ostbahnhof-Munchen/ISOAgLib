/*
  eeprom.h: include dependent on used target (defined in
    IsoAgLib/isoaglib_config.h) the suitable HAL specific header for
    EEPROM data storage

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

/** \file IsoAgLib/hal/eeprom.h
  * include dependent on used target (defined in
	  IsoAgLib/isoaglib_config.h) the suitable HAL
		specific header for EEPROM data storage.
*/
/* ************************************************************ */
#ifndef _HAL_INDEPENDEND_EEPROM_H_
#define _HAL_INDEPENDEND_EEPROM_H_

// include interface aplication relevant configuration settings
// #include <IsoAgLib/isoaglib_config.h>
#include "config.h"

// now include dependent on used target the suitable header
#define _hal_eeprom_header_ <IsoAgLib/hal/HAL_SUBDIR/eeprom/eeprom.h>
#include _hal_eeprom_header_


#endif
