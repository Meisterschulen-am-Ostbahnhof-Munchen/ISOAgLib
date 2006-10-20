/***************************************************************************
                          system.h - definition of Hardware Abstraction
                                     Layer for central system functions
                                     for debug/simulation
                                     system on ESXu
                             -------------------
    begin                : Wed Mar 15 2000
    copyright            : (C) 2000 - 2004 Dipl.-Inform. Achim Spangler
						 : This file was based on the corresponding file in
						 : the ESX HAL and modified for the ESXu HAL.
						 : These changes (C) 2004 - 2005 Michael D. Schmidt
    email                : a.spangler@osb-ag:de
						 : mike.schmidt@agcocorp:com
    type                 : Header
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
 * Copyright (C) 1999 - 2004 Dipl.-Inform. Achim Spangler                  *
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

/* ************************************************************ */
/** \file IsoAgLib/hal/esxu/system/system.h
 * The header <i>\<target\>/\<device\>/\<device\>.h</i> performs a name
   mapping between platform specific BIOS / OS function names
   and the function names, the IsoAgLib uses for hardware access.
   In this header the groups for error codes, global system
   functions and CAN must be adapted to a new platform.
   If a simple name mapping is not possible for a specific
   function, the module targetExtensions can be used to implement
   the needed activity. The implementation of the ESXu version
   can be used to derive the meaning of the specific function.
*/
/* ************************************************************ */
#ifndef _HAL_ESXu_SYSTEM_H_
#define _HAL_ESXu_SYSTEM_H_

#include "system_target_extensions.h"

#include <IsoAgLib/util/impl/util_funcs.h>

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
/*@{*/
  inline int16_t printf(...){return 1;};
  inline int16_t scanf(...) {return 1;};
  using CNAMESPACE::sprintf;
  using CNAMESPACE::sscanf;
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
    settings of configESXu
    @return error state (HAL_NO_ERR == o.k.)
      or DATA_CHANGED on new values -> need call of wdReset to use new settings
    @see wdReset
  */
  inline int16_t configWatchdog()
    {return __HAL::configWatchdog();};

  /**
	With reset_wd the watchdog can be forced to RESET (reboot) the processor.

    reset the watchdog to use new configured watchdog settings
    @see configWd

	The function reset_wd causes a new start of the ESX-micro software.
	This means that the system monitor is also initialized again and the settings of
	config_wd become effective. If no reset has taken place after 4ms, the function
	is exitted and an error message is returned.
   */
  inline void wdReset(void)
    {__HAL::reset_wd();
#if defined( DEBUG_HAL )
//IsoAgLib::getIrs232Instance() << __HAL::get_time() << " ms - "
//<< "reset_wd()\r";

uint8_t buf[128];
CNAMESPACE::sprintf( (char*)buf, "%u ms - reset_wd()\r"
, (uint16_t)__HAL::get_time()
);
#endif

    };
  /** trigger the watchdog */
  inline void wdTriggern(void)
    {__HAL::trigger_wd();
#if defined( DEBUG_HAL )
//IsoAgLib::getIrs232Instance() << __HAL::get_time() << " ms - "
//<< "trigger_wd()\r";
#endif
    };

  /**
    get the system time in [ms]
    @return [ms] since start of the system
  */
  inline int32_t getTime(void)
    {return __HAL::get_time();
#if defined( DEBUG_HAL )
//IsoAgLib::getIrs232Instance() << __HAL::get_time() << " ms - "
//<< "get_time()\r";
#endif
    };

  inline int16_t getSnr(uint8_t *snrDat)
    {
    int16_t retval = __HAL::get_snr(snrDat);

#if defined( DEBUG_HAL )
//IsoAgLib::getIrs232Instance() << __HAL::get_time() << " ms - "
//<< "get_snr( "
//<< (uint16_t) *snrDat
//<< " ) returns  "
//<< retval << "\r";

uint8_t buf[128];
CNAMESPACE::sprintf( (char*)buf, "%u ms - get_snr( %u ) returns %i\r"
, (uint16_t)__HAL::get_time()
, (uint16_t)*snrDat
, (int16_t) retval );
HAL::put_rs232NChar( buf, CNAMESPACE::strlen( (char*)buf ), 0 /*HAL::RS232_over_can_busnum*/ );
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
    return (bcd2dec(uint8[2]) * 100 + bcd2dec(uint8[3])) + (bcd2dec(uint8[0]) << 14);
  };

  inline int16_t getLokalId(uint8_t *Dat)
    {
    int16_t retval = __HAL::get_lokal_id(Dat);

#if defined( DEBUG_HAL )
//IsoAgLib::getIrs232Instance() << __HAL::get_time() << " ms - "
//<< "getLokalId( "
//<< (uint16_t) *Dat
//<< " ) returns  "
//<< retval << "\r";

uint8_t buf[128];
CNAMESPACE::sprintf( (char*)buf, "%u ms - get_lokal_id( %u ) returns %i\r"
, (uint16_t)__HAL::get_time()
, (uint16_t)*Dat
, (int16_t) retval );
HAL::put_rs232NChar( buf, CNAMESPACE::strlen( (char*)buf ), 0 /*HAL::RS232_over_can_busnum*/ );
#endif

	return retval;
    }

  /**
    start the Task Timer -> time between calls of Task Manager
  */
  inline void startTaskTimer ( void )
    {
    __HAL::start_task_timer ( T_TASK_BASIC );

#if defined( DEBUG_HAL )
//IsoAgLib::getIrs232Instance() << __HAL::get_time() << " ms - "
//<< "start_task_timer( "
//<< (uint16_t) T_TASK_BASIC
//<< " )\r";

uint8_t buf[128];
CNAMESPACE::sprintf( (char*)buf, "%u ms - start_task_timer( %u )\r"
, (uint16_t)__HAL::get_time()
, (uint16_t)T_TASK_BASIC
);
HAL::put_rs232NChar( buf, CNAMESPACE::strlen( (char*)buf ), 0 /*HAL::RS232_over_can_busnum*/ );
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

#if defined( DEBUG_HAL )
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
	return retval;
    }
  /**
    Get Task Overload
	If a task has already been running and is called up a second time by a timer interrupt,
	a flag is set. The function get_task_overload returns the condition of this flag.
	With reset_task_overload this flag can be deleted.
  */
  inline int16_t getTaskOverload ( uint16_t wHandle )
    {
    int16_t retval = __HAL::get_task_overload ( wHandle );

#if defined( DEBUG_HAL )
uint8_t buf[128];
CNAMESPACE::sprintf( (char*)buf, "%u ms - get_task_overload( %u ) returns %i\r"
, (uint16_t)__HAL::get_time()
, (uint16_t)wHandle
, (int16_t) retval
);
HAL::put_rs232NChar( buf, CNAMESPACE::strlen( (char*)buf ), 0 /*HAL::RS232_over_can_busnum*/ );
#endif

	return retval;
    }
  /**
    Reset Task Overload
	If a task has already been running and is called up a second time by a timer interrupt,
	a flag is set. The function get_task_overload returns the condition of this flag.
	With reset_task_overload this flag can be deleted.
  */
  inline int16_t resetTaskOverload ( uint16_t wHandle )
    {
    int16_t retval = __HAL::reset_task_overload ( wHandle );

#if defined( DEBUG_HAL )
uint8_t buf[128];
CNAMESPACE::sprintf( (char*)buf, "%u ms - reset_task_overload( %u ) returns %i\r"
, (uint16_t)__HAL::get_time()
, (uint16_t)wHandle
, (int16_t) retval
);
HAL::put_rs232NChar( buf, CNAMESPACE::strlen( (char*)buf ), 0 /*HAL::RS232_over_can_busnum*/ );
#endif

	return retval;
    }
  /**
    get the main power voltage
    @return voltage of power [mV]
  */
  inline int16_t  getAdcUbat( void )
    {
	__HAL::t_Sys_AnalogData t_Sys_AnalogData;
	__HAL::get_system_analogdata(&t_Sys_AnalogData);

#if defined( DEBUG_HAL )
//IsoAgLib::getIrs232Instance() << __HAL::get_time() << " ms - "
//<< "get_system_analogdata( &t_Sys_AnalogData ) " << ", wUE = " << t_Sys_AnalogData.wUE << "\r";

uint8_t buf[128];
CNAMESPACE::sprintf( (char*)buf, "%u ms - get_system_analogdata(), wUE = %u\r"
, (uint16_t)__HAL::get_time()
, (uint16_t)t_Sys_AnalogData.wUE
);
HAL::put_rs232NChar( buf, CNAMESPACE::strlen( (char*)buf ), 0 /*HAL::RS232_over_can_busnum*/ );
#endif

    return t_Sys_AnalogData.wUE;
    }
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
      int16_t retval = __HAL::get_on_off_switch();

#if defined( DEBUG_HAL )
//IsoAgLib::getIrs232Instance() << __HAL::get_time() << " ms - "
//<< "get_on_off_switch("
//<< ") returns "
//<< retval << "\r";

uint8_t buf[128];
CNAMESPACE::sprintf( (char*)buf, "%u ms - get_on_off_switch() returns %i\r"
, (uint16_t)__HAL::get_time()
, (int16_t)retval
);
#endif

	return retval;
    #endif
  };

  /**
    activate the power selfholding to perform system
    stop (f.e. store values) actions after loss of CAN_EN
  */
  inline void stayingAlive(void)
    {__HAL::staying_alive();
//IsoAgLib::getIrs232Instance() << __HAL::get_time() << " ms - "
//<< "staying_alive()\r";

#if defined( DEBUG_HAL )
uint8_t buf[64];
CNAMESPACE::sprintf( (char*)buf, "%u ms - staying_alive()\r"
, (uint16_t)__HAL::get_time()
);
#endif

    };

  /**
    shut off the whole system (set power down)
  */
  inline void powerDown(void)
    {__HAL::power_down();

#if defined( DEBUG_HAL )
//IsoAgLib::getIrs232Instance() << __HAL::get_time() << " ms - "
//<< "power_down()\r";

uint8_t buf[64];
CNAMESPACE::sprintf( (char*)buf, "%u ms - power_down()\r"
, (uint16_t)__HAL::get_time()
);
#endif

    };

  /**
    switch relais on or off
    @param bitState true -> Relais ON
  */
  inline void setRelais(bool bitState)
  {__HAL::set_relais(bitState);

#if defined( DEBUG_HAL )
//IsoAgLib::getIrs232Instance() << __HAL::get_time() << " ms - "
//<< "set_relais( "
//<< (uint16_t)bitState
//<< " )\r";

uint8_t buf[128];
CNAMESPACE::sprintf( (char*)buf, "%u ms - set_relais( %u )\r"
, (uint16_t)__HAL::get_time()
, (uint16_t)bitState
);
#endif

  };
/*@}*/


}
#endif
