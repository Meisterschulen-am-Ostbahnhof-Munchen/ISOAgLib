/***************************************************************************
                          errcodes.h  - header for the ESX 
                                        error codes
                             -------------------                                         
    begin                : Wed Aug 4 1999                                           
    copyright            : THIS HEADER IS DERIVED FROM ORIGNINAL STW HEADER                         
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
#ifndef __ESX_ERRCODES_H__
#define __ESX_ERRCODES_H__

namespace HAL {
  /* include BIOS errcode definition */
  extern "C" {
    #include <commercial_BIOS/bios_esx/Xos20err.h>
  }
  /* --- map BIOS error codes to target independent HAL codes --- */
  #define HAL_NO_ERR            C_NO_ERR
  #define HAL_BUSY_ERR          C_BUSY
  #define HAL_CONFIG_ERR        C_CONFIG
  #define HAL_RANGE_ERR         C_RANGE
  #define HAL_NOACT_ERR         C_NOACT
  #define HAL_WARN_ERR          C_WARN
  #define HAL_OVERFLOW_ERR      C_OVERFLOW
  #define HAL_RS232_RECEIVE_ERR C_RD_WR
  #define HAL_UNKNOWN_ERR       C_UNKNOWN_ERR

  #define HAL_DEFAULT           C_DEFAULT
  #define HAL_RD_WR             C_RD_WR
  #define HAL_COM               C_COM
  #define HAL_CHECKSUM          C_CHECKSUM
  
  #define HAL_WD_DATA_CHANGED   DATA_CHANGED
  
  #define HAL_DIGOUT_OPEN        (int16_t)(-(20 * 0x0001))
  #define HAL_DIGOUT_SHORTCUT    (int16_t)(-(21 * 0x0001))
  #define HAL_DIGOUT_OVERTEMP    (int16_t)(-(22 * 0x0001))
  #define HAL_DIGOUT_UNDERVOLT   (int16_t)(-(23 * 0x0001))
  #define HAL_DIGOUT_OVERVOLT    (int16_t)(-(24 * 0x0001))
  #define HAL_NEW_SEND_DELAY    (int16_t)(-(25 * 0x0001))
} // end namespace __HAL
#endif

