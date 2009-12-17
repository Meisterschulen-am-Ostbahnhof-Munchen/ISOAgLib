/*
  datastreams.h:
    include dependent on used target (defined in
    IsoAgLib/isoaglib_config.h) the suitable HAL specific header for
    datastreams input and output

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
		specific header for datastreams output control.
*/
/* ************************************************************ */
#ifndef _HAL_INDEPENDEND_DATASTREAMS_H_
#define _HAL_INDEPENDEND_DATASTREAMS_H_

// include interface aplication relevant configuration settings
// #include <IsoAgLib/isoaglib_config.h>
#include <IsoAgLib/hal/config.h>

typedef enum
{
	StreamAte    = 0x01,
	StreamApp    = 0x02,
	StreamTrunc  = 0x04,
	StreamIn     = 0x08,
	StreamOut    = 0x10,
	StreamBinary = 0x20
} FileMode_t;


// now include dependent on used target the suitable header
#if defined(SYSTEM_PC)
	#include "pc/datastreams/datastreams.h"
#elif defined(SYSTEM_ESX)
//	#include "esx/datastreams/datastreams.h"
#elif defined(SYSTEM_A2)
//	#include "esx/datastreams/datastreams.h"
#elif defined(SYSTEM_ESXu)
//	#include "esxu/datastreams/datastreams.h"
#elif defined(SYSTEM_C2C)
//	#include "c2c/datastreams/datastreams.h"
#elif defined(SYSTEM_DJ1)
	#include "Dj1/datastreams/datastreams.h"
#elif defined(SYSTEM_IMI)
//	#include "imi/datastreams/datastreams.h"
#elif defined(SYSTEM_PM167)
//	#include "pm167/datastreams/datastreams.h"
#elif defined(SYSTEM_AMS5)
	#include "ams5/datastreams/datastreams.h"
#endif


#endif
