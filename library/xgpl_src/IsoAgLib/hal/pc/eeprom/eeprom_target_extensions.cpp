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

/* get the segment size of the eeprom for page write access*/
int16_t getEepromSegmentSize(void){
  return 32;
};

/* get the status of eeprom*/
int16_t eepromReady(void){
  // printf("eeprom ready aufgerufen\n");
  return EE_READY;
}

/* enable or disabel write protection*/
int16_t eepromWp(boolean /* bitMode */ ){
 // printf("eepromWp mit %i aufgerufen\n", bitMode);
  return HAL_NO_ERR;
}

int16_t openDatFileAndSeek(long al_position)
{
  eepromDat = fopen(EEPROM_DAT_FILE, "r+b");
  if ( NULL == eepromDat )
  { // try to create file with creation mode
    eepromDat = fopen(EEPROM_DAT_FILE, "a+b");
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
      eepromDat = fopen("eeprom.dat", "a+b");
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
int16_t eepromWrite(uint16_t wAddress,uint16_t wNumber,const uint8_t *pbData){
  int16_t i = 0;
  short sTemp;
  int8_t c_temp;
  int32_t i32_temp;
  uint8_t* pByte;
//  printf("schreibe Daten von %i mit Daten aus Datei %s\n", wAddress, EEPROM_DAT_FILE );fflush(0);
  int16_t errCode = openDatFileAndSeek(wAddress);
  if ( errCode  != HAL_NO_ERR ) return errCode;

  switch (wNumber)
  {
    case 4:
      i32_temp = *(int32_t*)pbData;
      pByte = (uint8_t*)&i32_temp;
      for (i=0; i < wNumber; i++)
      {
        c_temp = pByte[i];
#ifdef DEBUG_EEPROM
        if ( isprint( c_temp ) ) putchar(c_temp);
#endif
        fputc(c_temp, eepromDat);
      }
//      printf(", als Zahl %i", i32_temp);
      break;
    case 2:
      sTemp = *(short*)pbData;
      pByte = (uint8_t*)&sTemp;
      for (i=0; i < wNumber; i++)
      {
#ifdef DEBUG_EEPROM
       if ( isprint( pByte[i] ) ) putchar(pByte[i]);
#endif
       fputc(pByte[i], eepromDat);
      }
//      printf(", als Zahl %hi", sTemp);
      break;
    case 1:
      c_temp = *(int8_t*)pbData;
#ifdef DEBUG_EEPROM
      if ( isprint( c_temp ) ) putchar(c_temp);
#endif
      fputc(c_temp, eepromDat);
//      printf(", als Zahl %hi oder als Text %c", c_temp, c_temp);
      break;
    default:
      for (i=0; i < wNumber; i++)
      {
#ifdef DEBUG_EEPROM
        if ( isprint( pbData[i] ) ) putchar(pbData[i]);
#endif
        fputc(pbData[i], eepromDat);
      }
      break;
  }
//  printf("\n");
  fclose(eepromDat);
  return HAL_NO_ERR;
}

/* write one uint8_t into the eeprom */
int16_t eepromWriteByte(uint16_t wAddress,uint8_t bByte){
//  printf("schreibe Daten von %i mit Daten %i\n", wAddress, uint16_t(bByte));
  int16_t errCode = openDatFileAndSeek(wAddress);
  if ( errCode  != HAL_NO_ERR ) return errCode;

  fputc(bByte, eepromDat);
  fclose(eepromDat);
  return HAL_NO_ERR;
}

/* read one or more uint8_t from the eeprom*/
int16_t eepromRead(uint16_t wAddress,uint16_t wNumber,uint8_t *pbByte){
  int16_t i;
  short sTemp;
  int8_t c_temp;
  int32_t i32_temp;
//  printf("lese Daten von %i mit Daten als text:", wAddress);
  int16_t errCode = openDatFileAndSeek(wAddress);
  if ( errCode  != HAL_NO_ERR ) return errCode;

  switch (wNumber)
  {
    case 4:
      for (i=0; i < wNumber; i++)
      {
        pbByte[i] = (uint8_t)fgetc(eepromDat);
#ifdef DEBUG_EEPROM
        putchar(pbByte[i]);
#endif
      }
      i32_temp = *(int32_t*)pbByte;
//      printf(", als Zahl %i", i32_temp);
      break;
    case 2:
      for (i=0; i < wNumber; i++)
      {
        pbByte[i] = (uint8_t)fgetc(eepromDat);
#ifdef DEBUG_EEPROM
        putchar(pbByte[i]);
#endif
      }
      sTemp = *(short*)pbByte;
//      printf(", als Zahl %hi", sTemp);
      break;
    case 1:
      pbByte[0] =  (uint8_t)fgetc(eepromDat);
#ifdef DEBUG_EEPROM
      putchar(pbByte[0]);
#endif
      c_temp = *(int8_t*)pbByte;
//      printf(", als Zahl %hi", c_temp);
      break;
    default:
      for (i=0; i < wNumber; i++)
      {
        pbByte[i] = (uint8_t)fgetc(eepromDat);
#ifdef DEBUG_EEPROM
        putchar(pbByte[i]);
#endif
      }
      break;
  }
//  printf("\n");
  fclose(eepromDat);
  return HAL_NO_ERR;
}

} // End of namespace __HAL
