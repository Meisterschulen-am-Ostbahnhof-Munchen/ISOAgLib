/*
  inputs_target_extensions.h: header for C2C specific extensions for
    the HAL for input

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

#ifndef _HAL_C2C_INPUTS_TARGET_EXTENSIONS_H_
#define _HAL_C2C_INPUTS_TARGET_EXTENSIONS_H_

#include <IsoAgLib/isoaglib_config.h>
#include <IsoAgLib/hal/c2c/typedef.h>
#include <IsoAgLib/hal/c2c/errcodes.h>


namespace __HAL {

int16_t init_counter(uint16_t aui16_timebase, bool ab_risingEdge);
uint32_t getCounter();
int16_t resetCounter();
uint32_t getCounterPeriod_us();
uint32_t getCounterFrequency();
uint16_t getCounterLastSignalAge();

} // __HAL

#endif
