/*
  sensor.h: include dependent on used target (defined in IsoAgLib/isoaglib_config.h) the suitable HAL specific header for sensor data input

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

/** \file
 * 
  * include dependent on used target (defined in
	  IsoAgLib/isoaglib_config.h) the suitable HAL
		specific header for sensor data input.
*/
/* ************************************************************ */
#ifndef _HAL_INDEPENDEND_SENSOR_H_
#define _HAL_INDEPENDEND_SENSOR_H_

// include interface aplication relevant configuration settings
// #include <IsoAgLib/isoaglib_config.h>
#include <IsoAgLib/hal/config.h>

// now include dependent on used target the suitable header
#if defined(SYSTEM_PC)
	#include "pc/sensor/sensor.h"
#elif defined(SYSTEM_ESX)
	#include "esx/sensor/sensor.h"
#elif defined(SYSTEM_ESXu)
	#include "esxu/sensor/sensor.h"
#elif defined(SYSTEM_C2C)
	#include "c2c/sensor/sensor.h"
#elif defined(SYSTEM_DJ1)
	#include "Dj1/sensor/sensor.h"
#elif defined(SYSTEM_IMI)
	#include "imi/sensor/sensor.h"
#elif defined(SYSTEM_PM167)
	//#include "pm167/sensor/sensor.h"
#elif defined(SYSTEM_AMS5)
	#include "ams5/sensor/sensor.h"
#endif


#endif
