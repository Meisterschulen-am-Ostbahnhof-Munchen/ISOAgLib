/*
  actor_target_extensions.h:
    header for PC specific extensions for the HAL for actor

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
#ifndef _HAL_PC_OUTPUTS_TARGET_EXTENSIONS_H_
#define _HAL_PC_OUTPUTS_TARGET_EXTENSIONS_H_

#include <IsoAgLib/isoaglib_config.h>
#include <IsoAgLib/hal/pc/typedef.h>

namespace __HAL {

  int16_t  setPwmFreq(uint8_t bOutputGroup, uint32_t dwFrequency);  /* define the frequency of the pwm signal */
  void     setDigout(uint8_t bOutputNo, uint16_t wPWMValue);        /* set pwm value 0 ... 100 % [0..0xFFFF] */
  uint16_t getDigout(uint8_t bOutputNo);

  void setMainRelais( bool on );

  int16_t getDigoutDiagnose(uint8_t aui8_channel, uint16_t aui16_minCurrent, uint16_t aui16_maxCurrent );

/*@}*/

} // __HAL

#endif
