/*
  actor_target_extensions.h:
    header for PC specific extensions for the HAL for actor

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef _HAL_PC_ACTOR_TARGET_EXTENSIONS_H_
#define _HAL_PC_ACTOR_TARGET_EXTENSIONS_H_

#include <IsoAgLib/isoaglib_config.h>
#include <IsoAgLib/hal/pc/typedef.h>

namespace __HAL {
/* ******************************************************** */
/**
 * \name Actor_c Specific Functions
 */
/* ******************************************************** */
/*@{*/
/** --- GLOBAL TYPDEFINITIONS --- */
#if 0
// doesn't seem to be needed in PC case, can be removed if everything still compiles...
typedef struct   {
  uint16_t wOutputValue;              /* aktual output value */
  uint16_t wMaxOutput;                /* highest pwm puls width value */
  uint32_t dwOutputFreq;             /* pwm frequence */
  tTime TSetTime;                 /* time of output setting */
} tOutput;
#endif

/* --- GLOBAL VARIABLES --- */
/* --- PROTOTYPEN --- */
int16_t  setPwmFreq(uint8_t bOutputGroup, uint32_t dwFrequency);  /* define the frequency of the pwm signal */
int16_t  setDigout(uint8_t bOutputNo, uint16_t wPWMValue);          /* set pwm value 0 ... 100 %*/

  /* switch main relais on or off*/
  void setMainRelais( bool on );

  /**
    deliver channel number for checking/requesting of
    ADC value at pwm input
    for call of __HAL:: functions (differences mostly caused
    by multiplexers)
    @param channel number from interval [0..maxNo]
    @return according channel number for __HAL call
  */
  inline uint8_t getPwmCurrentCheckNr(uint8_t ab_channel)
    {return (42 - (2 * ab_channel));};

  /**
    deliver channel number for checking/requesting of
    ADC value at pwm output
    for call of __HAL:: functions (differences mostly caused
    by multiplexers)
    @param channel number from interval [0..maxNo]
    @return according channel number for __HAL call
  */
  inline uint8_t getPwmoutAdcCheckNr(uint8_t ab_channel)
    {if (ab_channel < 4) return (43 - (ab_channel * 2));
    if (ab_channel == 4) return 44;
    if (ab_channel < 8) return (47 - ab_channel);
    if (85 - ab_channel) return (85 - ab_channel);
    return 0;
    };

/*@}*/

} // End of name space __HAL
#endif
