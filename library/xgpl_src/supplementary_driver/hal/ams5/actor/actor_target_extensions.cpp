/***************************************************************************
    actor_target_extension.cpp - __HAL namespace layer for AMS5

    -------------------
    date                 : 09.09.2008
    copyright            : (c) 2007/2008 GESAS GmbH
    email                : stefan.klueh@gesas:de
    type                 : Module
 ***************************************************************************/
/*   History:
              18.06.2007 V1.00  - first release

              09.09.2008 V1.01  - revision of function "getMaxDigout" and "setDigout":
                                  AMS5 with decentral periphery (ZMDL/LW)
*/
// ---------------------------------------------------------------------------------------------


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
 
      @param rui8_channel channel number of output
      @return max possible value
   */
   uint16_t getMaxDigout(uint8_t rui8_channel)
   {
      if (rui8_channel < 32)   // static digital outputs (A6833)
      {
         return 1;
      }
      else
      {
         switch (rui8_channel)
         {
            case 32:
            case 33:
            case 34:
               return 65535;   // PWM value for analog output

            case 35:
            case 36:
               return 3;       // motor H-bridge state

            default:
               break;
         }
      }

      if (rui8_channel < 181)   // decentral periphery (ZMDL/LW)
      {
         return 1;
      }
      else
      {
         return 0;
      }
   }

   /**
      set outputs by channel

      No AMS5-BIOS channel is a "PWM" channel!
      Use symbolic output channels form "config.h"!

      @param  rui8_channel channel number of output
      @param  wValue range depends on channel!
      @return HAL_NO_ERR
              HAL_RANGE_ERR  value is out of range
              HAL_CONFIG_ERR channel is not availabel 
   */
   int16_t setDigout(uint8_t rui8_channel, uint16_t wValue)
   {
      if (rui8_channel < 37)
      {     
         return AMSBIOS::io_setOutput(rui8_channel, wValue);
      }
      else if (rui8_channel < 181)
      {
         // output channel number: 0..12
         // Node number:           1..8
         uint8_t NodeNr    = (rui8_channel-37)/18+1;
         uint8_t ChannelNr = (rui8_channel-37)%18;
         return AMSBIOS::ioExt_setOutput(NodeNr, ChannelNr, wValue);
      }
      else
      {
         return HAL_CONFIG_ERR;
      }
   }

   /*@}*/
}
