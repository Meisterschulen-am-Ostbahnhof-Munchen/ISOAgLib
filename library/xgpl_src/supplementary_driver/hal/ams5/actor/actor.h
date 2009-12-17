/*
  actor.h: definition of HAL namespace layer for AMS5

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#ifndef _HAL_AMS5_ACTOR_H_
#define _HAL_AMS5_ACTOR_H_

#include "actor_target_extensions.h"


#define DIGITAL_OUTPUT_MIN 0
#define DIGITAL_OUTPUT_MAX 11

namespace HAL
{
   /* ********************************* */
   /** \name Actor_c Output BIOS functions */
   /*@{*/

   /**
      define the frequency of the pwm signal

      dummy function - Not Supported

      @param bOutputChannel PWM output channel
      @param dwFrequency PWM frequency in mHz
      @return error state: HAL_NO_ERR
   */
   inline uint16_t setPwmFreq(uint8_t bOutputChannel, uint32_t dwFrequency)
   {
      return HAL_NO_ERR;
   };

   /**
      retrieve maximal value for output channel

      No AMS5-BIOS channel is a "PWM" channel!
      Use symbolic output channels form "config.h"!
 
      @param aui8_channel channel number of output
      @return max possible value
   */
   inline uint16_t getMaxPwmDigout(uint8_t aui8_channel)
   {
      return __HAL::getMaxDigout(aui8_channel);
   };

   /**
      set outputs by channel

      No AMS5-BIOS channel is a "PWM" channel!
      Use symbolic output channels form "config.h"!

      @param aui8_channel channel number of output
      @param wPWMValue range depends on channel!
      @return HAL_NO_ERR
              HAL_RANGE_ERR  value is out of range
              HAL_CONFIG_ERR channel is not availabel 
   */
   inline int16_t setDigout(uint8_t aui8_channel, uint16_t wPWMValue)
   {
      return __HAL::setDigout(aui8_channel, wPWMValue);
   };


   /**
      deliver the actual current of the digital output

      dummy function - Not Supported

      @param aui8_channel channel to check
      @return current in [mA] ( if specified channel doesn't support current measurement, -1 is returned )
   */
   inline int16_t getDigoutCurrent(uint8_t aui8_channel)
   {
      return -1;
   };

   /**
      deliver the state of a digital output

      dummy function - Not Supported

      This function evaluates the current where possible, otherwise it evaluates
      the measured voltage at the output. The latter interpretation can go wrong
      if the PWM setting is >0 but has a very low value, so that even under normal
      conditions the voltage with connected consuming device is lower than to open
      connector state at low level.
      @param aui8_channel channel to check
      @param aui16_minCurrent minimal allowed current in [mA]
      @param aui16_maxCurrent maximum allowed current in [mA]
      @return HAL_NO_ERR, HAL_DIGOUT_OPEN, HAL_DIGOUT_SHORTCUT, HAL_DIGOUT_OVERTEMP,
              HAL_DIGOUT_UNDERVOLT, HAL_DIGOUT_OVERVOLT
   */
   inline int16_t getDigoutDiagnose(uint8_t aui8_channel, uint16_t aui16_minCurrent, uint16_t aui16_maxCurrent)
   {
      return HAL_NO_ERR;
   };


   /**
      deliver the measure voltage at the PWM output

      dummy function - Not Supported
.
      Use this for application specific state evaluation for cases, where the standard
      getDigoutDiagnose function can go wrong.
      @return voltage at PWM channel [mV]
   */
   inline int16_t getDigoutAdc(uint8_t aui8_channel)
   {
      return HAL_NO_ERR;
   };

   /*@}*/
}
#endif
