/***************************************************************************
    eeprom.h - definition of Hardware Abstraction Layer
               for AMS5 eeprom functions
    -------------------
    date                 : 18.06.2007
    copyright            : (c) 2007 GESAS GmbH
    email                : stefan.klueh@gesas:de
    type                 : Header
 ***************************************************************************/


#ifndef _HAL_AMS5_EEPROM_H_
#define _HAL_AMS5_EEPROM_H_

#include "hal_eeprom_interface.h"

namespace HAL
{
   /* *************************** */
   /** \name EEPROM BIOS functions */
   /*@{*/

   /**
      deliver the EEPROM size in byte
      @return EEPROM size in byte
   */
   inline int16_t getEepromSize(void)
   {
      return (EEPROM_SIZE);
   };

   /**
      deliver the EEPROM segment size in byte
      @return EEPROM segment size in byte
   */
   inline int16_t getEepromSegmentSize(void)
   {
      return (MAX_EEPROM_SEGMENT_SIZE);
   };

   /**
      read amount of uint8_t in uint8_t string variable
      @param wAdress start adress to read
      @param wNumber amount of bytes to read
      @param pbByte pointer to uint8_t string, where data is inserted
      @return error state (C_NO_ERR == o.k.)
   */
   inline int16_t eepromRead(uint16_t wAddress,uint16_t wNumber,uint8_t *pbByte)
   {
      return __HAL::eeprom_read(wAddress, wNumber, pbByte);
   };

   /**
      write amount of uint8_t from string into EEPROM from given start adress
      @param wAdress adress of first written uint8_t in EEPROM
      @param wNumber amount of written data
      @param pbData pointer to uint8_t string which should be written into EEPROM
      @return error state (C_NO_ERR == o.k.)
   */
   inline int16_t eepromWrite(uint16_t wAddress,uint16_t wNumber,uint8_t *pbData)
   {
      return __HAL::eeprom_write(wAddress, wNumber, pbData);
   };

   /**
      set or unset set write protection of EEPROM

      dummy function - Not Supported
   */
   inline int16_t eepromWp(bool bitMode)
   {
      return HAL_NO_ERR;
   };

   /**
      check if EEPROM is ready for actions
      @return EE_READY -> ready
   */
   inline int16_t eepromReady(void)
   {
      return __HAL::eeprom_ready();
   };

   /*@}*/

}
#endif
