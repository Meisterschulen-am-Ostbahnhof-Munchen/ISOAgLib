/***************************************************************************
    system.h - definition of Hardware Abstraction Layer
               for AMS5 central system functions
    -------------------
    date                 : 18.06.2007
    copyright            : (c) 2007 GESAS GmbH
    email                : stefan.klueh@gesas:de
    type                 : Header
 ***************************************************************************/

#ifndef _HAL_AMS5_SYSTEM_H_
#define _HAL_AMS5_SYSTEM_H_

#include "system_target_extensions.h"

namespace HAL
{
   /* *********************************** */
   /** \name Global System BIOS functions */
   /*@{*/

   inline int16_t printf(...){return 1;};
   inline int16_t scanf(...) {return 1;};
   //  using CNAMESPACE::sprintf;
   //  using CNAMESPACE::sscanf;

   /**
      open the system with system specific function call

      @return error state (HAL_NO_ERR == o.k.)
   */
   inline int16_t  open_system()
   {
      return __HAL::open_system();
   };

   /**
      close the system with system specific function call

      @return error state (HAL_NO_ERR == o.k.)
   */
   inline int16_t  closeSystem()
   {
      return __HAL::closeSystem();
   };

   /**
      check if open_System() has already been called
   */
   inline bool isSystemOpened(void)
   {
      return __HAL::isSystemOpened();
   };

   /**
      dummy function "configure the watchdog"

      @see wdTriggern
   */
   inline int16_t configWatchdog()
   {
      return HAL_NO_ERR;
   };

   /**
      dummy function "reset the watchdog"

      @see wdTriggern
   */
   inline int16_t  wdReset(void)
   {
      return HAL_NO_ERR;
   };

   /**
      trigger the watchdog
   */
   inline void wdTriggern(void)
   {
      return __HAL::wdTriggern();
   };

   /**
      get the system time in [ms]

      @return [ms] since start of the system
   */
   inline int32_t getTime(void)
   {
      return __HAL::getTime();
   };

   /**
      hand over the serial number to a pointer

      @return HAL_NO_ERR
   */
   inline int16_t getSnr(uint8_t *snrDat)
   {
      return __HAL::getSnr(snrDat);
   };

   /**
      serial number as long interger

      @return serial number
   */
   inline int32_t getSerialNr(int16_t* pi16_errCode)
   {
      return __HAL::getSerialNr(pi16_errCode);
   };

   /**
       dummy function "start the Task Timer"
   */
   inline void startTaskTimer (void)
   {
      return;
   };

   /**
      get the main power voltage 12V or 24V

      @return voltage of power [mV]
   */
   inline int16_t  getAdcUbat(void)
   {
      return AMS5_MAIN_POWER_VOLTAGE; /* @see config.h */
   };

   /**
      get the voltage of the external reference: 2.5V

      @return voltage at external reference [mV]
   */
   inline int16_t  getAdc_u85(void)
   {
      return AMS5_REFERENCE_VOLTAGE; /* @see config.h */
   };

   /**
      dummy function "check if D+ switch is active"

      @return HAL_CONFIG_ERR
   */
   inline int16_t  getOn_offSwitch(void)
   {
      return HAL_CONFIG_ERR;
   };

   /**
      dummy function "activate the power selfholding"
   */
   inline void stayingAlive(void)
   {
      return;
   };

   /**
      dummy function "shut off the whole system (set power down)"
      -> use closeSystem()
   */
   inline void powerDown(void)
   {
      return;
   };

   /**
      enable/disable all outputs

      @param bitState true  -> outputs enable
                      false -> outputs disable

   */
   inline void setRelais(bool bitState)
   {
      return __HAL::setOutputMask(bitState);
   };
   /*@}*/

}

#ifdef USE_MUTUAL_EXCLUSION
#include <IsoAgLib/hal/generic_utils/system/mutex_emulated.h>
#endif


#endif
