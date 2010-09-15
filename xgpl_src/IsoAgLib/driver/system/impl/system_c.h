/***************************************************************************
                          system_c.h  -  central object for encapsulation of
                                       platform dependent elements (mostly BIOS)
                             -------------------
    begin                : Fri Apr 07 2000
    copyright            : (C) 2000 - 2004 by Dipl.-Inform. Achim Spangler
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
 * Copyright (C) 2000 - 2004 Dipl.-Inform. Achim Spangler                  *
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

 /**************************************************************************
 *                                                                         *
 *     ###    !!!    ---    ===    IMPORTANT    ===    ---    !!!    ###   *
 * Each software module, which accesses directly elements of this file,    *
 * is considered to be an extension of IsoAgLib and is thus covered by the *
 * GPL license. Applications must use only the interface definition out-   *
 * side :impl: subdirectories. Never access direct elements of __IsoAgLib  *
 * and __HAL namespaces from applications which shouldnt be affected by    *
 * the license. Only access their interface counterparts in the IsoAgLib   *
 * and HAL namespaces. Contact a.spangler@osb-ag:de in case your applicat- *
 * ion really needs access to a part of an internal namespace, so that the *
 * interface might be extended if your request is accepted.                *
 *                                                                         *
 * Definition of direct access:                                            *
 * - Instantiation of a variable with a datatype from internal namespace   *
 * - Call of a (member-) function                                          *
 * Allowed is:                                                             *
 * - Instatiation of a variable with a datatype from interface namespace,  *
 *   even if this is derived from a base class inside an internal namespace*
 * - Call of member functions which are defined in the interface class     *
 *   definition ( header )                                                 *
 *                                                                         *
 * Pairing of internal and interface classes:                              *
 * - Internal implementation in an :impl: subdirectory                     *
 * - Interface in the parent directory of the corresponding internal class *
 * - Interface class name IsoAgLib::iFoo_c maps to the internal class      *
 *   __IsoAgLib::Foo_c                                                     *
 *                                                                         *
 * AS A RULE: Use only classes with names beginning with small letter :i:  *
 ***************************************************************************/
#ifndef SYSTEM_H
#define SYSTEM_H

#include <IsoAgLib/util/liberr_c.h>
#include <IsoAgLib/util/impl/singleton.h>
#include <IsoAgLib/hal/system.h>

namespace IsoAgLib {
typedef enum { PowerdownByExplcitCall, PowerdownOnCanEnLoss } SystemPowerdownStrategy_t;
 class iSystem_c;
};

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
  bool init( bool rb_forceReinit = false, IsoAgLib::SystemPowerdownStrategy_t rt_strategy = CONFIG_DEFAULT_POWERDOWN_STRATEGY );
	/**
		default behaviour of IsoAgLib is to activate power hold, so that
		the application can decide on its own, if a CAN_EN loss shall cause
		a power down of the target. This allows to inhibit stop of application
		on short power supply voltage low bursts.
		@param rt_strategy PowerdownByExplcitCall -> stop system only on explicit call of System_c::close()
		                   PowerdownOnCanEnLoss   -> let BIOS/OS automatically switch off on CAN_EN loss
	*/
	void setPowerdownStrategy( IsoAgLib::SystemPowerdownStrategy_t rt_strategy );
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
  static int16_t  getBatteryVoltage( void )
    {return HAL::getAdcUbat();};
  /**
    get the voltage of the external reference 8.5Volt for work of external sensors
    @return voltage at external reference [mV]
  */
  static int16_t  getExternalSensorPowerVoltage( void ) { return HAL::getAdc_u85();};
  /** control the relay which is responsible for activation of the PWM output */
  static void setRelais( bool rb_activateRelaisForPwm );

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
  static inline uint32_t serialNo( void ) {return HAL::getSerialNr();};

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
