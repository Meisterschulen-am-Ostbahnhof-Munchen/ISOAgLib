/*
  outputs.h: definition of HAL namespace layer for C2C

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

#ifndef _HAL_C2C_OUTPUTS_H_
#define _HAL_C2C_OUTPUTS_H_

#include <IsoAgLib/isoaglib_config.h>
#include <IsoAgLib/hal/c2c/typedef.h>
#include <IsoAgLib/hal/c2c/errcodes.h>

/* ******************************************************** */
/**
 * \name Basic BIOS limit constants
 */
/*@{*/
#define DIGITAL_OUTPUT_MIN 0
#define DIGITAL_OUTPUT_MAX 1
/*@}*/

namespace __HAL {
  extern "C" {
    /** include the BIOS specific header into __HAL */
    #include <commercial_BIOS/bios_c2c/c2c10osy.h>
  }

  /**
    deliver channel number for checking/requesting of
    ADC value at pwm output
    for call of __HAL:: functions (differences mostly caused
    by multiplexers)
    @param ab_channel number from interval [0..maxNo]
    @return according channel number for __HAL call
  */
  inline uint8_t getPwmoutAdcCheckNr(uint8_t ab_channel)
    { return ab_channel+1; }

  /**
    deliver channel number for checking/requesting of
    current output through given PWM output
    @param ab_channel number from interval [0..maxNo]
    @return according channel number for __HAL call
  */
  inline uint8_t getPwmCurrentCheckNr(uint8_t ab_channel)
    {return ab_channel;}
}


namespace HAL
{
  /* **************************** */
  /** \name Output BIOS functions */
/*@{*/

  static unsigned isCachedMaxPwm[ DIGITAL_OUTPUT_MAX-DIGITAL_OUTPUT_MIN+1 ] = {}; // all fields will be init'd to 0, meaning "false". Use this instead of bool for this reason
  static uint16_t cachedMaxPwm[ DIGITAL_OUTPUT_MAX-DIGITAL_OUTPUT_MIN+1 ];

  inline void setMainRelais(bool bitState)
  {}

  // C2C BIOS lets PWM channels OUT1, OUT2 configure individual PWM FREQ
  inline int16_t setPwmFreq(uint8_t bOutput, uint32_t dwFrequency)
  {
    isCachedMaxPwm[ bOutput ] = 0; // false

    return __HAL::set_pwm_freq(bOutput, dwFrequency);
  }

  inline uint16_t getMaxPwmDigout(uint8_t aui8_channel)
  {
    if( isCachedMaxPwm[ aui8_channel ] == 0 ) // false
    {
      __HAL::tOutput tOutputstatus;
      __HAL::get_digout_status(aui8_channel,&tOutputstatus);

      cachedMaxPwm[ aui8_channel ] = tOutputstatus.wMaxOutput;
      isCachedMaxPwm[ aui8_channel ] = 1; // true
    }

    return cachedMaxPwm[ aui8_channel ];
  }

  void setDigout(uint8_t aui8_channel, uint16_t wPWMValue);
  uint16_t getDigout( uint8_t bOutputNo );

  inline int16_t getDigoutCurrent( uint8_t aui8_channel )
  { return -1; }

  int16_t getDigoutDiagnose(uint8_t aui8_channel, uint16_t aui16_minCurrent, uint16_t aui16_maxCurrent);
  int16_t getDigoutAdc( uint8_t aui8_channel );

  /*@}*/

}

#endif
