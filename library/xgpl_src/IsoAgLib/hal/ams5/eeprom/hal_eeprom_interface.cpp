/*
  hal_eeprom_interface.cpp: definition of AMS5 eeprom functions

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

/** \file      hal_eeprom_interface.cpp
    \brief     definition of AMS5 eeprom functions
*/

#include "hal_eeprom_interface.h"
#include <../commercial_BIOS/bios_ams5/ams_bios.h>

namespace __HAL
{
   /* *************************** */
   /** \name EEPROM BIOS functions */
   /*@{*/

   /**
      read amount of uint8_t in uint8_t string variable
      @param wAdress start adress to read
      @param wNumber amount of bytes to read
      @param pbByte pointer to uint8_t string, where data is inserted
      @return error state (C_NO_ERR == o.k.)
   */
   int16_t eeprom_read(uint16_t wAddress,uint16_t wNumber, uint8_t *pbByte)
   {
      if ( AMSBIOS::eeprom_isReady() )
      {
         return AMSBIOS::eeprom_readData(wAddress, wNumber, pbByte);
      }
      else
      {
         return HAL_BUSY_ERR;
      }
   }

   /**
      write amount of uint8_t from string into EEPROM from given start adress
      @param wAdress adress of first written uint8_t in EEPROM
      @param wNumber amount of written data
      @param pbData pointer to uint8_t string which should be written into EEPROM
      @return error state (C_NO_ERR == o.k.)
   */
   int16_t eeprom_write(uint16_t wAddress,uint16_t wNumber, uint8_t *pbData)
   {
      if ( AMSBIOS::eeprom_isReady() )
      {
         return AMSBIOS::eeprom_writeData(wAddress, (uint8_t)wNumber, pbData);
      }
      else
      {
         return HAL_BUSY_ERR;
      }
   }

   /**
      check if EEPROM is ready for actions
      @return HAL_NO_ERR   -> ready
              HAL_BUSY_ERR -> busy
   */
   int16_t eeprom_ready(void)
   {
      if ( AMSBIOS::eeprom_isReady() )
      {
         return HAL_NO_ERR;
      }
      else
      {
         return HAL_BUSY_ERR;
      }
   }
   /*@}*/
}
