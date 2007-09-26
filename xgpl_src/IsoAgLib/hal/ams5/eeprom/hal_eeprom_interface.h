/***************************************************************************
   hal_eeprom_interface.h - definition of Hardware Abstraction Layer
                            for AMS5 eeprom functions
    -------------------
    date                 : 18.06.2007
    copyright            : (c) 2007 GESAS GmbH
    email                : stefan.klueh@gesas:de
    type                 : Header
 ***************************************************************************/


#ifndef _HAL_AMS5_EEPROM_INTERFACE_H_
#define _HAL_AMS5_EEPROM_INTERFACE_H_

#include "../../../../IsoAgLib/hal/ams5/typedef.h"
#include "../../../../IsoAgLib/hal/ams5/errcodes.h"
#include "../../../../IsoAgLib/hal/ams5/config.h"

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
   int16_t eeprom_read(uint16_t wAddress,uint16_t wNumber,uint8_t *pbByte);

   /**
      write amount of uint8_t from string into EEPROM from given start adress
      @param wAdress adress of first written uint8_t in EEPROM
      @param wNumber amount of written data
      @param pbData pointer to uint8_t string which should be written into EEPROM
      @return error state (C_NO_ERR == o.k.)
   */
   int16_t eeprom_write(uint16_t wAddress,uint16_t wNumber,uint8_t *pbData);

   /**
      check if EEPROM is ready for actions
      @return EE_READY -> ready
   */
   int16_t eeprom_ready(void);

   /*@}*/

}
#endif
