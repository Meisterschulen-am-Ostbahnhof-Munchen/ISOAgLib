/*
  hal_system.h: include dependent on used target (defined in
    IsoAgLib/isoaglib_config.h) the suitable HAL specific header for
    central system functions

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

/* ************************************************************ */
/** \file IsoAgLib/hal/hal_system.h
  * include dependent on used target (defined in
	  IsoAgLib/isoaglib_config.h) the suitable HAL
		specific header for central system functions.
*/
/* ************************************************************ */
#ifndef _HAL_INDEPENDENT_SYSTEM_H_
#define _HAL_INDEPENDENT_SYSTEM_H_

#include <IsoAgLib/isoaglib_config.h>


#ifdef HAL_PATH_ISOAGLIB_SYSTEM
#  define _hal_system_header_ <HAL_PATH_ISOAGLIB_SYSTEM/system.h>
#  include _hal_system_header_
#else
#  error "No HAL_PATH_ISOAGLIB_SYSTEM set. This is a mandatory module!"
#endif


#endif
