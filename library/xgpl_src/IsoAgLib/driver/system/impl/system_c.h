/*
  system_c.h: central object for encapsulation of platform dependent
    elements (mostly BIOS)

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

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
class System_c;
class System_c : public Singleton<System_c> {
public:



  /**
    Initialize the system hardware.
    (uses BIOS function)

    possible errors:
        * Err_c::SystemOpen problem during start of system with BIOS call
        * Err_c::SystemWatchdog the System_c::init_wd call caused an error
        * Err_c::unspecified Bios calls for TaskTimer, Relais or StayAlive caused an error
    @return true -> everything without errors initialised
  */
  bool init( bool ab_forceReinit = false, IsoAgLib::SystemPowerdownStrategy_t at_strategy = CONFIG_DEFAULT_POWERDOWN_STRATEGY );
	/**
		default behaviour of IsoAgLib is to activate power hold, so that
		the application can decide on its own, if a CAN_EN loss shall cause
		a power down of the target. This allows to inhibit stop of application
		on short power supply voltage low bursts.
		@param at_strategy PowerdownByExplcitCall -> stop system only on explicit call of System_c::close()
		                   PowerdownOnCanEnLoss   -> let BIOS/OS automatically switch off on CAN_EN loss
	*/
	void setPowerdownStrategy( IsoAgLib::SystemPowerdownStrategy_t at_strategy );
  /** every subsystem of IsoAgLib has explicit function for controlled shutdown
    * the call of System_c::close() stimulates final shutdown of power
    */
  static void close( void );
  /**
    destructor which shuts down the hardware (f.e. power off)
    (calls BIOS function)
  */
  ~System_c( void ) { close();};

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

  /** control the relay which is responsible for activation of the PWM output */
  static void setRelais( bool ab_activateRelaisForPwm );

  /**
    deliver the CanEn setting -> if system goes down
    (bios_namespace.h always return ON independent from D+ signal
    if NO_CAN_EN_CHECK is defined in config_esx.h)
    @return true -> D+ or. CAN_EN is active OR D+ should not be checked
  */
  static bool canEn( void );

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
  friend class Singleton< System_c >;
  friend class IsoAgLib::iSystem_c;
  /** private constructor which prevents direct instantiation in user application
    * NEVER define instance of System_c within application
    */
  System_c( void ) {};

  /**
    initialize directly after the singleton instance is created.
    this is called from singleton.h and should NOT be called from the user again.
    users please use init(...) instead.
  */
  void singletonInit();

// Private attributes
};
/** C-style function, to get access to the unique System_c singleton instance */
System_c& getSystemInstance( void );
}
#endif
