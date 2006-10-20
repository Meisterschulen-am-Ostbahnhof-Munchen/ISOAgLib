/***************************************************************************
                          isystem.h - central object for encapsulation of
                                      platform dependent elements
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
#ifndef ISYSTEM_H
#define ISYSTEM_H

#include "impl/system_c.h"

// Begin Namespace IsoAgLib
namespace IsoAgLib {

/**
  Layer class to encapsulate the hardware specific details.
  All system interaction must be done via this class (and member
  classes). To avoid runtime overhead, most functions are declared inline.
  @short Central layer object to encapsulate hardware specific details which uses
    other object to implement specific I/O operations.
  @author Dipl.-Inform. Achim Spangler
*/
class iSystem_c : private __IsoAgLib::System_c {
public:
  /** destructor which shuts down the hardware (f.e. power off) */
  ~iSystem_c(){};
  /**
    Initialize the system hardware.
    (uses BIOS function)

    possible errors:
        * Err_c::SystemOpen problem during start of system with BIOS call
        * Err_c::SystemWatchdog the System_c::init_wd call caused an error
        * Err_c::unspecified Bios calls for TaskTimer, Relais or StayAlive caused an error
    @return true -> everything without errors initialised
  */
  bool init( bool rb_forceReinit = false, SystemPowerdownStrategy_t rt_strategy = CONFIG_DEFAULT_POWERDOWN_STRATEGY ) { return System_c::init(rb_forceReinit, rt_strategy);};
	/**
		default behaviour of IsoAgLib is to activate power hold, so that
		the application can decide on its own, if a CAN_EN loss shall cause
		a power down of the target. This allows to inhibit stop of application
		on short power supply voltage low bursts.
		@param rt_strategy PowerdownByExplcitCall -> stop system only on explicit call of System_c::close()
		                   PowerdownOnCanEnLoss   -> let BIOS/OS automatically switch off on CAN_EN loss
	*/
	void setPowerdownStrategy( SystemPowerdownStrategy_t rt_strategy ) { System_c::setPowerdownStrategy( rt_strategy );};
  /** every subsystem of IsoAgLib has explicit function for controlled shutdown
    * the call of System_c::close() stimulates final shutdown of power
    */
  static void close( void ) { System_c::close();};

  /**
    init the hardware watchdog
    (uses BIOS function)

    possible errors:
        * Err_c::SystemWatchdog BIOS watchdog configuration without success
    @return true -> watchdog successful configured
  */
  static bool initWd ( void ) { return System_c::initWd();};
  /** trigger the watchdog */
  static inline void triggerWd( void ) { System_c::triggerWd();};
  /**
    deliver lasted time from start of system in msec.
    @return running time in [msec.]
  */
  static int32_t getTime(){return System_c::getTime();};

  /**
    deliver the CanEn setting -> if system goes down
    @return true -> D+ or. CAN_EN is active OR D+ should not be checked
  */
  static bool canEn() {return System_c::canEn();};
  /**
    get the main power voltage
    @return voltage of power [mV]
  */
  static int16_t  getBatteryVoltage( void ) {return System_c::getBatteryVoltage();};
  /**
    get the voltage of the external reference 8.5Volt for work of external sensors
    @return voltage at external reference [mV]
  */
  static int16_t  getExternalSensorPowerVoltage( void )
    { return System_c::getExternalSensorPowerVoltage();};

  /**
    deliver the serial nr of the device into uint8_t[6] array
    - f.e. to calculated individual wait berfore address claim
    @param snrDat pointer to 6 uint8_t array, where the serial no of the device is stored
  */
  static void serialNo(uint8_t *const snrDat){System_c::serialNo(snrDat);};
  static uint32_t serialNo( void ){return System_c::serialNo();};

private:
  /** allow getIsystemInstance() access to shielded base class.
      otherwise __IsoAgLib::getSystemInstance() wouldn't be accepted by compiler
    */
  friend iSystem_c& getIsystemInstance( void );
  /** private constructor which prevents direct instantiation in user application
    * NEVER define instance of iSystem_c within application
    */
  iSystem_c( void ) : System_c() {};
};

/** C-style function, to get access to the unique System_c singleton instance */
inline iSystem_c& getIsystemInstance( void ) { return static_cast<iSystem_c&>(__IsoAgLib::getSystemInstance());};

}

#endif
