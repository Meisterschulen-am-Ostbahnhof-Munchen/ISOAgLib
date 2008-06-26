/***************************************************************************
                          eeprom_target_extensions.h - header for PC specific
                                                extensions for the HAL
                                                for EEPROM
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
#ifndef _PC_EEPROM_TARGET_EXTENSIONS_H_
#define _PC_EEPROM_TARGET_EXTENSIONS_H_

#include "../config.h"
#include "../typedef.h"
#include "../errcodes.h"

#ifdef USE_RS232_EEPROM_EDITOR
  #include "pc_eeditor_rs232.h"
#endif
#ifdef USE_CAN_EEPROM_EDITOR
  #include "pc_eeditor_can.h"
#endif

namespace __HAL {
/* ******************************************************** */
/**
 * \name EEPROM specific functions
 */
/* ******************************************************** */

/*@{*/
/*--- GLOBAL DEFINES CONSTANTS ---*/
#define EE_READY                    0
#define EE_NOT_READY                1


/*--- GLOBAL VARIABLES ---*/
/*--- PROTOTYPES ---*/
int16_t getEepromSize(void);            /* get the size of the eeprom */
int16_t getEepromSegmentSize(void);    /* get the segment size of the eeprom for page write access*/
int16_t eepromReady(void);               /* get the status of eeprom*/
int16_t eepromWp(boolean bitMode);       /* enable or disabel write protection*/
int16_t eepromWrite(uint16_t wAddress,uint16_t wNumber,const uint8_t *pbData); /* write one or more bytes into the eeprom*/
int16_t eepromWriteByte(uint16_t wAddress,uint8_t bByte);           /* write one uint8_t into the eeprom */
int16_t eepromRead(uint16_t wAddress,uint16_t wNumber,uint8_t *pbByte);  /* read one or more uint8_t from the eeprom*/
/*@}*/

} // End of namespace __HAL
#endif
