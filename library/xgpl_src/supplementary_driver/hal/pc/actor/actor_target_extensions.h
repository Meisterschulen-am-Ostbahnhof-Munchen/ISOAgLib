/***************************************************************************
                          actor_target_extensions.h - header for PC specific
                                                extensions for the HAL
                                                for actor
                             -------------------
    begin                : Sat Jan 01 2003
    copyright            : (C) 2003 by Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 * This file is part of the "IsoAgLib", an object oriented program library *
 * to serve as a software layer between application specific program and   *
 * communication protocol details. By providing simple function calls for  *
 * jobs like starting a measuring program for a process data value on a    *
 * remote ECU, the main program has not to deal with single CAN telegram   *
 * formatting. This way communication problems between ECU's which use     *
 * this library should be prevented.                                       *
 * Everybody and every company is invited to use this library to make a    *
 * working plug and play standard out of the printed protocol standard.    *
 *                                                                         *
 * Copyright (C) 1999 - 2004 Dipl.-Inform. Achim Spangler                 *
 *                                                                         *
 * The IsoAgLib is free software; you can redistribute it and/or modify it *
 * under the terms of the GNU General Public License as published          *
 * by the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This library is distributed in the hope that it will be useful, but     *
 * WITHOUT ANY WARRANTY; without even the implied warranty of              *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       *
 * General Public License for more details.                                *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with IsoAgLib; if not, write to the Free Software Foundation,     *
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA           *
 *                                                                         *
 * As a special exception, if other files instantiate templates or use     *
 * macros or inline functions from this file, or you compile this file and *
 * link it with other works to produce a work based on this file, this file*
 * does not by itself cause the resulting work to be covered by the GNU    *
 * General Public License. However the source code for this file must still*
 * be made available in accordance with section (3) of the                 *
 * GNU General Public License.                                             *
 *                                                                         *
 * This exception does not invalidate any other reasons why a work based on*
 * this file might be covered by the GNU General Public License.           *
 *                                                                         *
 * Alternative licenses for IsoAgLib may be arranged by contacting         *
 * the main author Achim Spangler by a.spangler@osb-ag:de                  *
 ***************************************************************************/
#ifndef _HAL_PC_ACTOR_TARGET_EXTENSIONS_H_
#define _HAL_PC_ACTOR_TARGET_EXTENSIONS_H_

#include <IsoAgLib/hal/pc/config.h>
#include <IsoAgLib/hal/pc/typedef.h>
namespace __HAL {
/* ******************************************************** */
/**
 * \name Actor_c Specific Functions
 */
/* ******************************************************** */
/*@{*/
/** --- GLOBAL TYPDEFINITIONS --- */
typedef struct   {
  uint16_t wOutputValue;              /* aktual output value */
  uint16_t wMaxOutput;                /* highest pwm puls width value */
  uint32_t dwOutputFreq;             /* pwm frequence */
  tTime TSetTime;                 /* time of output setting */
} tOutput;

/* --- GLOBAL VARIABLES --- */
/* --- PROTOTYPEN --- */
int16_t  setPwmFreq(uint8_t bOutputGroup, uint32_t dwFrequency);  /* define the frequency of the pwm signal */
int16_t  setDigout(uint8_t aui8_channel, uint16_t wPWMValue);          /* set pwm value 0 ... 100 %*/


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
