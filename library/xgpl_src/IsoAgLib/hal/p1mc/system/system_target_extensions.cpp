/***************************************************************************
                          system_target_extensions.cpp - source for P1MC specific
                                                 extensions for the HAL
                                                 for central system
                             -------------------
    begin                : May 29 2009
    copyright            : (C) 2009 Dipl.-Ing. Antoine Kandera
    email                : a.kandera@osb-ag:de
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
 * Copyright (C) 2009 Dipl.-Ing. Antoine Kandera                           *
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
 * Alternative licenses for IsoAgLib may be arranged                       *
 ***************************************************************************/

/* ********************************************************** */
 /** \file IsoAgLib/hal/p1mc/system/system_target_extensions.ccp
 * A module targetExtensions should be used
 * for all methods, which can't be simply
 * mapped from ECU standard BIOS to the needs of
 * IsoAgLib by mostly renaming and reordering of functions, parameters
 * and types in <i>\<target\>/\<device\>/\<device\>.h</i> .*/
 /* ********************************************************** */

#include "../typedef.h"
#include "system_target_extensions.h"
#include "../config.h"

#include "datatypes.h"
#include "idapp.h"
#include "datacontainer.h"

extern "C"
{
  extern U32 os_gulGetTimeMs(void);
}

namespace __HAL {

/**
  open the system with system specific function call
  @return error state (C_NO_ERR == o.k.)
*/
int16_t open_system()
{
  // set repetition time for maintask to 1 ms
  OS.ExecTimeOut = 1;
  return C_NO_ERR;
}
/**
  close the system with system specific function call
  @return error state (C_NO_ERR == o.k.)
*/
int16_t closeSystem( void )
{
  return C_NO_ERR;
}

/**
  configure the watchdog of the system with the
  settings of configImi
  @return error state (C_NO_ERR == o.k.)
    or DATA_CHANGED on new values -> need call of wdReset to use new settings
  @see wdReset
*/
int16_t configWatchdog()
{
  return 0;
}

int32_t getTime(void)
{
  return os_gulGetTimeMs();
}

int16_t  getSnr(uint8_t *snrDat)
{
  return -1;
}

void start_task_timer ( void )
{

}

void stayingAlive(void)
{

}

void powerDown(void)
{

}

void setRelais(bool bitState)
{

}

int16_t __getAdcUbat()
{
  return -1;
}

int16_t getAdc_u85()
{
  return -1;
}

} // end namespace __HAL
