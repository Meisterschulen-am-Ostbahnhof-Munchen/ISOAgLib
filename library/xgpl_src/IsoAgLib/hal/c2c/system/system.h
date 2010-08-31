/*
  system.h: definition of Hardware Abstraction Layer for central
    system functions for debug/simulation system on C2C

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

/** \file IsoAgLib/hal/c2c/system/system.h
 * The header <i>\<target\>/\<device\>/\<device\>.h</i> performs a name
   mapping between platform specific BIOS / OS function names
   and the function names, the IsoAgLib uses for hardware access.
   In this header the groups for error codes, global system
   functions and CAN must be adapted to a new platform.
   If a simple name mapping is not possible for a specific
   function, the module targetExtensions can be used to implement
   the needed activity. The implementation of the C2C version
   can be used to derive the meaning of the specific function.
*/
/* ************************************************************ */
#ifndef _HAL_C2C_SYSTEM_H_
#define _HAL_C2C_SYSTEM_H_

#include <cstdio>
#include "../config.h"
#include "../typedef.h"
#include "../errcodes.h"
#include "system_target_extensions.h"

#include <IsoAgLib/util/impl/util_funcs.h>

namespace __HAL {
  extern "C" {
    /** include the BIOS specific header into __HAL */
    #include <commercial_BIOS/bios_c2c/c2c10osy.h>
  }
}

/**
   namespace with layer of inline (cost NO overhead -> compiler replaces
   inline function with call to orig BIOS function)
   functions between all IsoAgLib calls for BIOS and the corresponding BIOS functions
   --> simply replace the call to the corresponding BIOS function in this header
       for adaptation to new platform
 */
namespace HAL
{
  /* *********************************** */
  /** \name Global System BIOS functions */
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
  /** check if open_System() has already been called */
  inline bool isSystemOpened( void ) { return __HAL::isSystemOpened();};

  /**
    configure the watchdog of the system with the
    settings of configC2C
    @return error state (HAL_NO_ERR == o.k.)
      or DATA_CHANGED on new values -> need call of wdReset to use new settings
    @see wdReset
  */
  inline int16_t configWatchdog()
    {return __HAL::configWatchdog();};

  /**
    reset the watchdog to use new configured watchdog settings
    @see configWd
  */
  inline int16_t  wdReset(void)
    { return __HAL::reset_wd(); }
//    {return HAL_NO_ERR;};

  /** trigger the watchdog */
  inline void wdTriggern(void)
    {__HAL::trigger_wd();};
  /**
    get the system time in [ms]
    @return [ms] since start of the system
  */
  inline int32_t getTime(void)
    {return __HAL::get_time();};

  inline int16_t getSnr(uint8_t *snrDat)
    {
    int16_t retval = __HAL::get_snr(snrDat);

#if DEBUG_HAL
//IsoAgLib::getIrs232Instance() << __HAL::get_time() << " ms - "
//<< "get_snr( "
//<< (uint16_t) *snrDat
//<< " ) returns  "
//<< retval << "\r";

#if 0
// don't use CNAMESPACE in header, doesn't always work properly
// maybe reactivate the statement above using getIrs232Instance(..)
uint8_t buf[128];
CNAMESPACE::sprintf( (char*)buf, "%u ms - get_snr( %u ) returns %i\r"
, (uint16_t)__HAL::get_time()
, (uint16_t)*snrDat
, (int16_t) retval );
HAL::put_rs232NChar( buf, CNAMESPACE::strlen( (char*)buf ), 0 /*HAL::RS232_over_can_busnum*/ );
#endif
#endif

	return retval;
    }
    inline int32_t getSerialNr(int16_t* pi16_errCode = NULL)
    {
      uint8_t uint8 [6];
      int16_t errCode = __HAL::get_snr(uint8);
      if (pi16_errCode) *pi16_errCode = errCode;
    // ESX Serial number is coded in BCD. As we only get 21 bits,
    // we can take only a part of the information transmitted here.
    //  - uint8[0] is the year of construction -> 7 bits
    //  - uint8[2] and uint8[3] a contract numering -> 14 bits
      return (__IsoAgLib::bcd2dec(uint8[2]) * 100 + __IsoAgLib::bcd2dec(uint8[3])) + (__IsoAgLib::bcd2dec(uint8[0]) << 14);
    };

  inline int16_t getLokalId(uint8_t *Dat)
    {
    int16_t retval = __HAL::get_lokal_id(Dat);

#if DEBUG_HAL
//IsoAgLib::getIrs232Instance() << __HAL::get_time() << " ms - "
//<< "getLokalId( "
//<< (uint16_t) *Dat
//<< " ) returns  "
//<< retval << "\r";

#if 0
// don't use CNAMESPACE in header, doesn't always work properly
// maybe reactivate the statement above using getIrs232Instance(..)
uint8_t buf[128];
CNAMESPACE::sprintf( (char*)buf, "%u ms - get_lokal_id( %u ) returns %i\r"
, (uint16_t)__HAL::get_time()
, (uint16_t)*Dat
, (int16_t) retval );
HAL::put_rs232NChar( buf, CNAMESPACE::strlen( (char*)buf ), 0 /*HAL::RS232_over_can_busnum*/ );
#endif
#endif

	return retval;
    }

  /**
    start the Task Timer -> time between calls of Task Manager
  */
  inline void startTaskTimer ( void )
    {
    __HAL::start_task_timer ( T_TASK_BASIC );

#if DEBUG_HAL
//IsoAgLib::getIrs232Instance() << __HAL::get_time() << " ms - "
//<< "start_task_timer( "
//<< (uint16_t) T_TASK_BASIC
//<< " )\r";

#if 0
// don't use CNAMESPACE in header, doesn't always work properly
// maybe reactivate the statement above using getIrs232Instance(..)
uint8_t buf[128];
CNAMESPACE::sprintf( (char*)buf, "%u ms - start_task_timer( %u )\r"
, (uint16_t)__HAL::get_time()
, (uint16_t)T_TASK_BASIC
);
HAL::put_rs232NChar( buf, CNAMESPACE::strlen( (char*)buf ), 0 /*HAL::RS232_over_can_busnum*/ );
#endif
#endif
    }

  /**
    init the Task Call
	This function permits cyclic and/or delayed calls of user functions. If 0 is tranferred as parameter
	for wInterval, the function call will occur only once. (For starting the tasks start task timer
	(word wBasicTick) has to be queried.)
	The ordering of the task into the interrupt system uses the transfer parameter wHandle. If a zero-pointer
	is used in the user function parameter, the function will stop when the handle is called.
	The maximum number of tasks is limited to 4.
  */
  inline int16_t initTaskCall( uint16_t wHandle, uint16_t wInterval, uint16_t wOffset, void (* pfFunction)(void) )
    {
    int16_t retval = __HAL::init_task_call( wHandle, wInterval, wOffset, pfFunction );

#if DEBUG_HAL
#if 0
// don't use CNAMESPACE in header, doesn't always work properly
// maybe reactivate the statement above using getIrs232Instance(..)
uint8_t buf[128];
CNAMESPACE::sprintf( (char*)buf, "%u ms - init_task_call( %u, %u, %u, %s ) returns %i\r"
, (uint16_t)__HAL::get_time()
, (uint16_t)wHandle
, (uint16_t)wInterval
, (uint16_t)wOffset
, ( (pfFunction) ? "pfFunction" : "NULL" )
, (int16_t) retval
);
HAL::put_rs232NChar( buf, CNAMESPACE::strlen( (char*)buf ), 0 /*HAL::RS232_over_can_busnum*/ );
#endif
#endif
	return retval;
    }
  /**
    Get Task Overload
	If a task has already been running and is called up a second time by a timer interrupt,
	a flag is set. The function get_task_overload returns the condition of this flag.
	With reset_task_overload this flag can be deleted.
  */
  inline int16_t getTaskOverload ( uint16_t /*wHandle*/ )
    {
    int16_t retval = __HAL::get_task_overload ();

#if DEBUG_HAL
#if 0
// don't use CNAMESPACE in header, doesn't always work properly
// maybe reactivate the statement above using getIrs232Instance(..)
uint8_t buf[128];
CNAMESPACE::sprintf( (char*)buf, "%u ms - get_task_overload( %u ) returns %i\r"
, (uint16_t)__HAL::get_time()
, (uint16_t)wHandle
, (int16_t) retval
);
HAL::put_rs232NChar( buf, CNAMESPACE::strlen( (char*)buf ), 0 /*HAL::RS232_over_can_busnum*/ );
#endif
#endif

	return retval;
    }
  /**
    get the main power voltage
    @return voltage of power [mV]
  */
  inline int16_t  getAdcUbat( void )
    {return (40 * __HAL::get_adc(GET_U_E));};
  /**
    get the voltage of the external reference 8.5Volt for work of external sensors
    @return voltage at external reference [mV]
  */
  inline int16_t  getAdc_u85( void )
    {return 8500;};
  /**
    check if D+/CAN_EN is active
    (if NO_CAN_EN_CHECK is defined this function return always return ON)
    @return ON(1) or OFF(0)
  */
  inline int16_t  getOn_offSwitch(void)
  {
    #if defined(NO_CAN_EN_CHECK)
      return ON;
    #else
      return __HAL::get_on_off_switch();
    #endif
  };

  /**
    activate the power selfholding to perform system
    stop (f.e. store values) actions after loss of CAN_EN
  */
  inline void stayingAlive(void)
    {__HAL::staying_alive();};

  /**
    shut off the whole system (set power down)
  */
  inline void powerDown(void)
    {__HAL::power_down();};


   inline void delay_us(unsigned int i_tm)
   {__HAL::delay_us(i_tm);}


  /**
    switch relais on or off
    @param bitState true -> Relais ON
  */
  inline void setRelais(bool bitState) {};
/*@}*/

}

#ifdef USE_MUTUAL_EXCLUSION
#include <IsoAgLib/hal/generic_utils/system/mutex_emulated.h>
#endif


#endif
