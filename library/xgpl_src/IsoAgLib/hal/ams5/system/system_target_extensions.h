/***************************************************************************
    system_target_extensions.h - header for AMS5 specific extensions for
                                 the HAL for central system
    -------------------
    date                 : 18.06.2007
    copyright            : (c) 2007 GESAS GmbH
    email                : stefan.klueh@gesas:de
    type                 : Header
 ***************************************************************************/

#ifndef _HAL_AMS5_SYSTEM_TARGET_EXTENSIONS_H_
#define _HAL_AMS5_SYSTEM_TARGET_EXTENSIONS_H_
   
#include "..\..\..\..\IsoAgLib\hal\ams5\typedef.h"
#include "..\..\..\..\IsoAgLib\hal\ams5\errcodes.h"
#include "..\..\..\..\IsoAgLib\hal\ams5\config.h"

namespace __HAL
{
   /* *********************************** */
   /** \name Global System BIOS functions */
   /*@{*/

   /**
      open the system with system specific function call
      @return error state (HAL_NO_ERR == o.k.)
   */
   int16_t open_system(void);

   /**
      close the system with system specific function call
      @return error state (HAL_NO_ERR == o.k.)
   */
   int16_t closeSystem(void);

   /**
      check if open_System() has already been called
   */
   bool isSystemOpened(void);

   /**
      trigger the watchdog
   */
   void wdTriggern(void);

   /**
      get the system time in [ms]
      @return [ms] since start of the system
   */
   int32_t getTime(void);

   /**
      hand over the serial number to a pointer
      @return HAL_NO_ERR
   */
   int16_t getSnr(uint8_t *snrDat);

   /**
      serial number as long interger
      @return serial number
   */
   int32_t getSerialNr(int16_t* pi16_errCode);

   /**
      serial number as long interger
      @return serial number
   */
   uint32_t getSerialNr(void);

   /**
      software output enable/disable
      @param bitState true  -> outputs enable
                      false -> outputs disable
   */
   void setOutputMask(bool bitState);

   /*@}*/
}   
#endif
