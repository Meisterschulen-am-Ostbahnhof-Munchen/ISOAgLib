/*
  actor_target_extensions.cpp: source for PC specific extensions for
    the HAL for actor

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#include "actor_target_extensions.h"
#include <IsoAgLib/typedef.h>
#include <IsoAgLib/util/iliberr_c.h>
#include <IsoAgLib/hal/pc/errcodes.h>
#include <cstdio>

namespace __HAL {
/* ************************************** */
/* ****** Actor_c O BIOS functions  ******* */
/* ************************************** */
/* define the frequency of the pwm signal */
int16_t  setPwmFreq(uint8_t bOutputGroup, uint32_t dwFrequency)
{
  printf("PWM von Kanal %hi auf %i gesetzt\n", bOutputGroup, dwFrequency);
   return HAL_NO_ERR;
}
/* set pwm value 0 ... 100 %*/
int16_t  setDigout(uint8_t /* bOutputNo */, uint16_t /* wPWMValue */ )
{
  #if 0
	printf("digout fuer Kanal %i auf %i gesetzt\n", bOutputNo, wPWMValue);
	#endif
   return HAL_NO_ERR;
}

} // End of name space __HAL
