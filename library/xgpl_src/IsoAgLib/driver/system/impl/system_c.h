/*
  system_c.h: central object for encapsulation of platform dependent
    elements (mostly BIOS)

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef SYSTEM_H
#define SYSTEM_H

#include <IsoAgLib/util/iliberr_c.h>
#include <IsoAgLib/util/impl/singleton.h>
#include <IsoAgLib/hal/hal_system.h>

namespace IsoAgLib {
typedef enum SystemPowerdownStrategy_en { PowerdownByExplcitCall, PowerdownOnCanEnLoss } SystemPowerdownStrategy_t;
 class iSystem_c;
}

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {
/**
  Layer class to encapsulate the hardware specific details.
  All system interaction must be done via this class (and member
  classes). To avoid runtime overhead, most functions are declared inline.
  @short Central layer object to encapsulate hardware specific details which uses
    other object to implement specific I/O operations.
  @author Dipl.-Inform. Achim Spangler
  */
class System_c {
  MACRO_SINGLETON_CONTRIBUTION();
public:

  /**
    Initialize the system hardware.
    (uses BIOS function)

    possible errors:
        * Err_c::SystemOpen problem during start of system with BIOS call
        * Err_c::SystemWatchdog the System_c::init_wd call caused an error
        * Err_c::unspecified Bios calls for TaskTimer or StayAlive caused an error
    @return true -> everything without errors initialised
           false -> init failure or already initialized
  */
  bool init( IsoAgLib::SystemPowerdownStrategy_t at_strategy = CONFIG_DEFAULT_POWERDOWN_STRATEGY );
	
  /**
		default behaviour of IsoAgLib is to activate power hold, so that
		the application can decide on its own, if a CAN_EN loss shall cause
		a power down of the target. This allows to inhibit stop of application
		on short power supply voltage low bursts.
		@param at_strategy PowerdownByExplcitCall -> stop system only on explicit call of System_c::close()
		                   PowerdownOnCanEnLoss   -> let BIOS/OS automatically switch off on CAN_EN loss
	*/
	void setPowerdownStrategy( IsoAgLib::SystemPowerdownStrategy_t at_strategy );
  
  /** Stimulate final shutdown of ECU
    */
  bool close( void );

  ~System_c( void ) {}

  /**
    init the hardware watchdog
    (uses BIOS function)

    possible errors:
        * Err_c::SystemWatchdog BIOS watchdog configuration without success
    @return true -> watchdog successful configured
  */
  static bool initWd ( void );

  /** trigger the watchdog */
  static inline void triggerWd( void ) { HAL::wdTriggern();};

  /**
    deliver lasted time from start of system in msec.
    (use BIOS function)
    @return running time in [msec.]
  */
  static inline int32_t getTime(){return HAL::getTime();};

  /**
    get the main power voltage
    @return voltage of power [mV]
  */
  static int16_t getBatteryVoltage( void )
    {return HAL::getAdcUbat();};

  /**
    get the voltage of the external reference 8.5Volt for work of external sensors
    @return voltage at external reference [mV]
  */
  static int16_t getExternalSensorPowerVoltage( void ) { return HAL::getAdc_u85();};

  /**
    @return true -> ECU's On/Off-Switch reports system being "On".
  */
  static bool switchedOn() {
    return HAL::getOn_offSwitch();
  }

  // deprecated - only for backwards compatibility.
  // NOTE: Will be removed soon, please use "switchedOn()".
  static bool canEn() { return switchedOn(); }

  /**
    deliver the serial nr of the device into uint8_t[6] array
    - f.e. to calculated individual wait berfore address claim
    (uses BIOS function)
    @param snrDat pointer to 6 uint8_t array, where the serial no of the device is stored
  */
  static inline void serialNo(uint8_t *const snrDat){HAL::getSnr(snrDat);};

  /**
  return the serial nr of the device as an uint32_t
  (uses BIOS function)
  @param
  @return serial nr of the device
   */
//  static inline uint32_t serialNo( void ) {return HAL::getSerialNr();};

protected:
private:
  friend class IsoAgLib::iSystem_c;
  /** private constructor which prevents direct instantiation in user application
    * NEVER define instance of System_c within application
    */
  System_c() {}

// Private attributes
  friend System_c &getSystemInstance(uint8_t aui8_instance);
};

/** C-style function, to get access to the unique System_c singleton instance */
System_c &getSystemInstance(uint8_t aui8_instance = 0);
}
#endif
