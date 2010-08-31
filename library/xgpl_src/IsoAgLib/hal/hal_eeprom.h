/*
  hal_eeprom.h: include dependent on used target (defined in
    IsoAgLib/isoaglib_config.h) the suitable HAL specific header for
    EEPROM data storage

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

/* ************************************************************ */
/** \file IsoAgLib/hal/hal_eeprom.h
  * include dependent on used target (defined in
    IsoAgLib/isoaglib_config.h) the suitable HAL
    specific header for EEPROM data storage.
*/
/* ************************************************************ */
#ifndef _HAL_INDEPENDENT_EEPROM_H_
#define _HAL_INDEPENDENT_EEPROM_H_

#include <IsoAgLib/isoaglib_config.h>


#ifdef HAL_PATH_ISOAGLIB_EEPROM
#  define _hal_eeprom_header_ <HAL_PATH_ISOAGLIB_EEPROM/eeprom.h>
#  include _hal_eeprom_header_
#else
  // currently EEPROM is mandatory, but there are platforms without, so don't error out here.
#endif


#endif
