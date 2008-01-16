/***************************************************************************
    actor_target_extension.cpp - __HAL namespace layer for AMS5

    -------------------
    date                 : 18.06.2007
    copyright            : (c) 2007 GESAS GmbH
    email                : stefan.klueh@gesas:de
    type                 : Module
 ***************************************************************************/


#include "actor_target_extensions.h"
#include <../commercial_BIOS/bios_ams5/ams_bios.h>

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
   uint16_t getMaxDigout(uint8_t aui8_channel)
   {
      if (aui8_channel < 32)   // static digital outputs (A6833)
      {
         return 1;
      }
      else
      {
         switch (aui8_channel)
         {
            case 32:
            case 33:
            case 34:
               return 65535;   // PWM value for analog output

               case 35:
               case 36:
               return 3;       // motor H-bridge state

               default:
               return 0;
            }
      }
   }

   /**
      set outputs by channel

      No AMS5-BIOS channel is a "PWM" channel!
      Use symbolic output channels form "config.h"!

      @param aui8_channel channel number of output
      @param wValue range depends on channel!
      @return HAL_NO_ERR
              HAL_RANGE_ERR  value is out of range
              HAL_CONFIG_ERR channel is not availabel 
   */
   int16_t setDigout(uint8_t aui8_channel, uint16_t wValue)
   {
      return AMSBIOS::io_setOutput(aui8_channel, wValue);
   }

   /*@}*/
}
