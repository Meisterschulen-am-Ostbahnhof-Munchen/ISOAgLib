/***************************************************************************
                          eeprom_target_extensions.cpp - source for PC specific
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
 * Copyright (C) 1999 - 2009 Dipl.-Inform. Achim Spangler                 *
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
/** \file hal/pc/system/system_target_extensions.cpp
 * A module targetExtensions should be used
 * for all methods, which can't be simply
 * mapped from ECU standard BIOS to the needs of
 * IsoAgLib by mostly renaming and reordering of functions, parameters
 * and types in <i>\<target\>/\<device\>/\<device\>.h</i> .
 * ********************************************************** */

#include "eeprom_target_extensions.h"
#include <cstdio>
#include <cctype>
namespace __HAL {


// These are the default names for the pc's simulated eeprom
// Add a #define in your config file if you wish to name it something else.
#ifndef EEPROM_DAT_FILE
#  ifdef WIN32
#    define EEPROM_DAT_FILE		"..\\..\\..\\simulated_io\\eeprom.dat"
#  else
#    define EEPROM_DAT_FILE		"../../../simulated_io/eeprom.dat"
#  endif
#endif


/* ***************************************** */
/* ****** EEPROM I/O BIOS functions  ******* */
/* ***************************************** */
static FILE* eepromDat;

/* get the size of the eeprom */
int16_t getEepromSize(void)
{
  return 8*1024;
};

/* get the segment size of the eeprom for page write access */
int16_t getEepromSegmentSize(void){
  return 32;
};

/* get the status of eeprom */
int16_t eepromReady(void)
{
  return EE_READY; // PC EEPROM is always ready.
}

/* enable or disable write protection */
int16_t eepromWp(boolean /* bitMode */ )
{
  // printf("eepromWp mit %i aufgerufen\n", bitMode);
  // @todo ON REQUEST Implement Write Protection for PC EEPROM?
  return HAL_NO_ERR;
}

int16_t openDatFileAndSeek(long al_position)
{
  eepromDat = fopen(EEPROM_DAT_FILE, "r+b");
  if ( NULL == eepromDat )
  { // try to create file with creation mode
    eepromDat = fopen(EEPROM_DAT_FILE, "w+b");
    // Note: Using "w+b" instead of "a+b" for the following reason:
    // (although we're only using this command for creation of the file,
    //  it's being closed and reopened with "r+b" anyway!)
    // [Mike: Appending a non-existent file in certain versions of Windows, does not fill
    // the file with 00's up to the seek position.  So, the file ends up containing
    // whatever garbage happened to exist on the disk previously.  Telling it w+b
    // instead, creates the file and fills with 00's.]
    if ( NULL != eepromDat )
    { // we have to close the file, as the normal operation works only with a "r+b" opened FILE
      // - at least on some targets
      fclose( eepromDat );
      // reopen now with "r+b"
      eepromDat = fopen(EEPROM_DAT_FILE, "r+b");
    }
  }
  if ( NULL == eepromDat )
  { // have another try with file directly in calling directory
    eepromDat = fopen("eeprom.dat", "r+b");
    if ( NULL == eepromDat )
    { // try to create file with creation mode
      eepromDat = fopen("eeprom.dat", "w+b"); // "w+b": See comment for fopen(..) above.
      if ( NULL != eepromDat )
      { // we have to close the file, as the normal operation works only with a "r+b" opened FILE
        // - at least on some targets
        fclose( eepromDat );
        // reopen now with "r+b"
        eepromDat = fopen("eeprom.dat", "r+b");
      }
    }
  }
  if ( NULL != eepromDat )
  { // seek on success to the wanted position
    fseek(eepromDat, al_position, 0);
    return HAL_NO_ERR;
  }
  else return HAL_CONFIG_ERR;
}

/* write one or more bytes into the eeprom*/
int16_t eepromWrite(uint16_t wAddress,uint16_t wNumber,const uint8_t *pbData)
{
  int16_t errCode = openDatFileAndSeek(wAddress);
  if ( errCode  != HAL_NO_ERR ) return errCode;

#ifdef DEBUG_EEPROM
  INTERNAL_DEBUG_DEVICE << "Writing data to EEPROM address " << wAddress << " - Text: ";
#endif

  for (int i=0; i < wNumber; ++i)
  {
    fputc (pbData[i], eepromDat);
#ifdef DEBUG_EEPROM
    INTERNAL_DEBUG_DEVICE << (isprint (pbData[i])) ? pbData[i] : ".";
#endif
  }

#ifdef DEBUG_EEPROM
  switch (wNumber)
  {
    case 4:
      INTERNAL_DEBUG_DEVICE << " - Number (4 byte signed): " << *(int32_t*)pbData;
      break;
    case 2:
      INTERNAL_DEBUG_DEVICE << " - Number (2 byte signed): " << *(int16_t*)pbData;
      break;
    case 1:
      INTERNAL_DEBUG_DEVICE << " - Number (1 byte signed): " << *(int8_t*)pbData;
      break;
    default:
      break;
  }
  INTERNAL_DEBUG_DEVICE << INTERNAL_DEBUG_DEVICE_ENDL;
#endif

  fclose(eepromDat);
  return HAL_NO_ERR;
}

/* write one uint8_t into the eeprom */
int16_t eepromWriteByte(uint16_t wAddress,uint8_t bByte)
{
#ifdef DEBUG_EEPROM
  INTERNAL_DEBUG_DEVICE << "Writing byte '" << uint16_t (bByte) << "' to EEPROM address " << wAddress << " - Text: ";
  INTERNAL_DEBUG_DEVICE <<  isprint(bByte) ? bByte : ".";
  INTERNAL_DEBUG_DEVICE << INTERNAL_DEBUG_DEVICE_ENDL;
#endif
  int16_t errCode = openDatFileAndSeek(wAddress);
  if ( errCode  != HAL_NO_ERR ) return errCode;

  fputc(bByte, eepromDat);
  fclose(eepromDat);
  return HAL_NO_ERR;
}

/* read one or more uint8_t from the eeprom */
int16_t eepromRead(uint16_t wAddress,uint16_t wNumber,uint8_t *pbByte)
{
  const int16_t errCode = openDatFileAndSeek(wAddress);
  if ( errCode  != HAL_NO_ERR ) return errCode;

#ifdef DEBUG_EEPROM
  INTERNAL_DEBUG_DEVICE << "Reading EEPROM data from address " << wAddress << " - Text: ";
#endif

  for (int i=0; i < wNumber; ++i)
  {
    const int16_t c = fgetc(eepromDat);
    pbByte[i] = ( c == EOF ) ? CONFIG_EEPROM_PADDING_BYTE : (const uint8_t)c;   // pad when at EOF
#ifdef DEBUG_EEPROM
    INTERNAL_DEBUG_DEVICE << (pbByte[i] >= 0x20) ? pbByte[i] : ".";
#endif
  }

#ifdef DEBUG_EEPROM
  switch (wNumber)
  {
    case 4:
      INTERNAL_DEBUG_DEVICE << " - Number (4 byte signed): " << *(int32_t*)pbByte;
      break;
    case 2:
      INTERNAL_DEBUG_DEVICE << " - Number (2 byte signed): " << *(int16_t*)pbByte;
      break;
    case 1:
      INTERNAL_DEBUG_DEVICE << " - Number (1 byte signed): " << *(int8_t*)pbByte;
      break;
    default:
      break;
  }
  INTERNAL_DEBUG_DEVICE << INTERNAL_DEBUG_DEVICE_ENDL;
#endif

  fclose(eepromDat);
  return HAL_NO_ERR;
}

} // End of namespace __HAL

