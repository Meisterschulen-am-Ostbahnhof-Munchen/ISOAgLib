/***************************************************************************
    actor_target_extensions.h - header for AMS5 specific extensions

    -------------------
    date                 : 18.06.2007
    copyright            : (c) 2007 GESAS GmbH
    email                : stefan.klueh@gesas:de
    type                 : Header
 ***************************************************************************/

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

      @param  bOutputNo channel number of output
      @param  wPWMValue range depends on channel!
      @return HAL_NO_ERR
              HAL_RANGE_ERR  value is out of range
              HAL_CONFIG_ERR channel is not availabel 
   */
   int16_t setDigout(uint8_t bOutputNo, uint16_t wValue);

   /*@}*/
}

#endif
