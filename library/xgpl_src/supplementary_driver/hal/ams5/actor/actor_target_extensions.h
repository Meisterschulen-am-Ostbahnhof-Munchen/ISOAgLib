/*
  actor_target_extensions.h: header for AMS5 specific extensions

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#ifndef _HAL_AMS5_ACTOR_TARGET_EXTENSIONS_H_
#define _HAL_AMS5_ACTOR_TARGET_EXTENSIONS_H_

#include <IsoAgLib/hal/ams5/typedef.h>
#include <IsoAgLib/hal/ams5/errcodes.h>

namespace __HAL
{
   /* ********************************* */
   /** \name Actor_c Output functions   */
   /*@{*/

   /**
      retrieve maximal value for output channel

      Use symbolic output channels form "config.h"!
 
      @param aui8_channel channel number of output
      @return max possible value
   */
   uint16_t getMaxDigout(uint8_t aui8_channel);

   /**
      set outputs by channel

      No AMS5-BIOS channel is a "PWM" channel!
      Use symbolic output channels form "config.h"!

      @param  aui8_channel channel number of output
      @param  wValue range depends on channel!
      @return HAL_NO_ERR
              HAL_RANGE_ERR  value is out of range
              HAL_CONFIG_ERR channel is not availabel 
   */
   int16_t setDigout(uint8_t aui8_channel, uint16_t wValue);

   /*@}*/
}

#endif
