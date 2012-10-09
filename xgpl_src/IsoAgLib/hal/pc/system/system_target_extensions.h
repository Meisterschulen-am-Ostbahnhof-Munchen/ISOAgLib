/***************************************************************************
                          system_target_extensions.h - header for PC specific
                                                extensions for the HAL
                                                for central system
                             -------------------
    begin                : Wed Mar 15 2000
    copyright            : (C) 2000 - 2004 Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
    type                 : Header
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

/* ********************************************************** */
/** \file IsoAgLib/hal/pc/system/system_system_target_extensions.h
 * A module targetExtensions should be used
 * for all methods, which can't be simply
 * mapped from ECU standard BIOS to the needs of
 * IsoAgLib by mostly renaming and reordering of functions, parameters
 * and types in map_bios_namespace.h .
 * ********************************************************** */

#ifndef _HAL_PC_SYSTEM_TARGET_EXTENSIONS_H_
#define _HAL_PC_SYSTEM_TARGET_EXTENSIONS_H_

#include "../config.h"
#include "../typedef.h"
#include "../errcodes.h"
#ifndef WIN32
#include <sys/time.h>
#include <time.h>
#endif

namespace __HAL {
   #define  GET_U_C               35        /* UC (Boardspannung)   */
   #define  GET_U_EXT_8_5_V       15        /* U 8,5 V EXT */

/** \name General BIOS Extenders */
/*@{*/

/**
  open the system with system specific function call
  @return error state (C_NO_ERR == o.k.)
*/
int16_t open_system();
/**
  close the system with system specific function call
  @return error state (C_NO_ERR == o.k.)
*/
int16_t closeSystem( void );
/** check if open_System() has already been called */
bool isSystemOpened( void );

/**
  configure the watchdog of the system with the
  settings of configEsx
  @return error state (C_NO_ERR == o.k.)
    or DATA_CHANGED on new values -> need call of wdReset to use new settings
  @see wdReset
*/
int16_t configWatchdog();

int32_t getTime();
#ifndef WIN32
clock_t getStartUpTime();
#endif
int16_t getSnr(uint8_t *snrDat);               /* serial number of target */

int16_t  configWd(tWDConfig *tConfigArray); /* configuration of the system supervisor*/
void wdTriggern(void);
int16_t wdReset(void);


void startTaskTimer ( void );
int16_t  getCpuFreq(void);                 /* get the cpu frequency*/
void stayingAlive(void);                /* to activate the power selfholding*/

void powerDown(void);                   /* to deactivate the power selfholding*/
int16_t  getOn_offSwitch(void);            /* the evaluation of the on/off-switch (D+)*/

void setRelais(boolean bitState);       /* switch relais on or off*/
int16_t KeyGetByte(uint8_t *p);

/*@}*/

} //end namespace __HAL
#endif
