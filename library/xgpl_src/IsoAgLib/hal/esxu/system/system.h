/*
  system.h: definition of Hardware Abstraction Layer for central
    system functions for debug/simulation system on ESXu

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

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

#include "../../hal_system.h"

/**
   namespace with layer of inline (cost NO overhead -> compiler replaces
   inline function with call to orig BIOS function)
   functions between all IsoAgLib calls for BIOS and the corresponding BIOS functions
   --> simply replace the call to the corresponding BIOS function in this header
       for adaptation to new platform
 */
namespace HAL {
  inline int16_t  open_system() {
    return __HAL::open_system();
  };

  inline int16_t  closeSystem() {
    return __HAL::closeSystem();
  };

  inline bool isSystemOpened( void ) {
    return __HAL::isSystemOpened();
  };

  inline int16_t configWatchdog() {
    return __HAL::configWatchdog();
  };

  inline void wdReset( void ) {
    __HAL::reset_wd();
#if DEBUG_HAL
//IsoAgLib::getIrs232Instance() << __HAL::get_time() << " ms - "
//<< "reset_wd()\r";

#if 0
// don't use CNAMESPACE in header, doesn't always work properly
// maybe reactivate the statement above using getIrs232Instance(..)
    uint8_t buf[128];
    CNAMESPACE::sprintf(( char* )buf, "%u ms - reset_wd()\r"
                        , ( uint16_t )__HAL::get_time()
                       );
#endif
#endif

  };

  inline void wdTriggern( void ) {
    __HAL::trigger_wd();
#if DEBUG_HAL
//IsoAgLib::getIrs232Instance() << __HAL::get_time() << " ms - "
//<< "trigger_wd()\r";
#endif
  };

  inline int32_t getTime( void ) {
    return __HAL::get_time();
#if DEBUG_HAL
//IsoAgLib::getIrs232Instance() << __HAL::get_time() << " ms - "
//<< "get_time()\r";
#endif
  };

  inline int16_t getSnr( uint8_t *snrDat ) {
    int16_t retval = __HAL::get_snr( snrDat );

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
    CNAMESPACE::sprintf(( char* )buf, "%u ms - get_snr( %u ) returns %i\r"
                        , ( uint16_t )__HAL::get_time()
                        , ( uint16_t )*snrDat
                        , ( int16_t ) retval );
    HAL::put_rs232NChar( buf, CNAMESPACE::strlen(( char* )buf ), 0 /*HAL::RS232_over_can_busnum*/ );
#endif
#endif

    return retval;
  }

  inline int32_t getSerialNr( int16_t* pi16_errCode ) {
    uint8_t uint8 [6];
    int16_t errCode = __HAL::get_snr( uint8 );
    if ( pi16_errCode ) *pi16_errCode = errCode;
    // ESX Serial number is coded in BCD. As we only get 21 bits,
    // we can take only a part of the information transmitted here.
    //  - uint8[0] is the year of construction -> 7 bits
    //  - uint8[2] and uint8[3] a contract numering -> 14 bits
    return ( __IsoAgLib::bcd2dec( uint8[2] ) * 100 + __IsoAgLib::bcd2dec( uint8[3] ) ) + ( __IsoAgLib::bcd2dec( uint8[0] ) << 14 );
  };

  inline void startTaskTimer( void ) {
    __HAL::start_task_timer( T_TASK_BASIC );

#if DEBUG_HAL
//IsoAgLib::getIrs232Instance() << __HAL::get_time() << " ms - "
//<< "start_task_timer( "
//<< (uint16_t) T_TASK_BASIC
//<< " )\r";

#if 0
// don't use CNAMESPACE in header, doesn't always work properly
// maybe reactivate the statement above using getIrs232Instance(..)
    uint8_t buf[128];
    CNAMESPACE::sprintf(( char* )buf, "%u ms - start_task_timer( %u )\r"
                        , ( uint16_t )__HAL::get_time()
                        , ( uint16_t )T_TASK_BASIC
                       );
    HAL::put_rs232NChar( buf, CNAMESPACE::strlen(( char* )buf ), 0 /*HAL::RS232_over_can_busnum*/ );
#endif
#endif
  }

  inline int16_t  getAdcUbat( void ) {
    __HAL::t_Sys_AnalogData t_Sys_AnalogData;
    __HAL::get_system_analogdata( &t_Sys_AnalogData );

#if DEBUG_HAL
//IsoAgLib::getIrs232Instance() << __HAL::get_time() << " ms - "
//<< "get_system_analogdata( &t_Sys_AnalogData ) " << ", wUE = " << t_Sys_AnalogData.wUE << "\r";

#if 0
// don't use CNAMESPACE in header, doesn't always work properly
// maybe reactivate the statement above using getIrs232Instance(..)
    uint8_t buf[128];
    CNAMESPACE::sprintf(( char* )buf, "%u ms - get_system_analogdata(), wUE = %u\r"
                        , ( uint16_t )__HAL::get_time()
                        , ( uint16_t )t_Sys_AnalogData.wUE
                       );
    HAL::put_rs232NChar( buf, CNAMESPACE::strlen(( char* )buf ), 0 /*HAL::RS232_over_can_busnum*/ );
#endif
#endif

    return t_Sys_AnalogData.wUE;
  }

  inline int16_t  getAdc_u85( void ) {
    return 8500;
  };

  inline int16_t  getOn_offSwitch( void ) {
#if defined(NO_CAN_EN_CHECK)
    return ON;
#else
    int16_t retval = __HAL::get_on_off_switch();

#if DEBUG_HAL
//IsoAgLib::getIrs232Instance() << __HAL::get_time() << " ms - "
//<< "get_on_off_switch("
//<< ") returns "
//<< retval << "\r";

#if 0
// don't use CNAMESPACE in header, doesn't always work properly
// maybe reactivate the statement above using getIrs232Instance(..)
    uint8_t buf[128];
    CNAMESPACE::sprintf(( char* )buf, "%u ms - get_on_off_switch() returns %i\r"
                        , ( uint16_t )__HAL::get_time()
                        , ( int16_t )retval
                       );
#endif
#endif

    return retval;
#endif
  };

  inline void stayingAlive( void ) {
    __HAL::staying_alive();

#if DEBUG_HAL
//IsoAgLib::getIrs232Instance() << __HAL::get_time() << " ms - "
//<< "staying_alive()\r";
#if 0
// don't use CNAMESPACE in header, doesn't always work properly
// maybe reactivate the statement above using getIrs232Instance(..)
    uint8_t buf[64];
    CNAMESPACE::sprintf(( char* )buf, "%u ms - staying_alive()\r"
                        , ( uint16_t )__HAL::get_time()
                       );
#endif
#endif

  };

  inline void powerDown( void ) {
    __HAL::power_down();



#if DEBUG_HAL
//IsoAgLib::getIrs232Instance() << __HAL::get_time() << " ms - "
//<< "power_down()\r";

#if 0
// don't use CNAMESPACE in header, doesn't always work properly
// maybe reactivate the statement above using getIrs232Instance(..)
    uint8_t buf[64];
    CNAMESPACE::sprintf(( char* )buf, "%u ms - power_down()\r"
                        , ( uint16_t )__HAL::get_time()
                       );
#endif
#endif

  };

  inline void setRelais( bool bitState ) {
    __HAL::set_relais( bitState );

#if DEBUG_HAL
//IsoAgLib::getIrs232Instance() << __HAL::get_time() << " ms - "
//<< "set_relais( "
//<< (uint16_t)bitState
//<< " )\r";

#if 0
// don't use CNAMESPACE in header, doesn't always work properly
// maybe reactivate the statement above using getIrs232Instance(..)
    uint8_t buf[128];
    CNAMESPACE::sprintf(( char* )buf, "%u ms - set_relais( %u )\r"
                        , ( uint16_t )__HAL::get_time()
                        , ( uint16_t )bitState
                       );
#endif
#endif

  };
  
  inline int16_t getLokalId( uint8_t *Dat ) {
    int16_t retval = __HAL::get_lokal_id( Dat );

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
    CNAMESPACE::sprintf(( char* )buf, "%u ms - get_lokal_id( %u ) returns %i\r"
                        , ( uint16_t )__HAL::get_time()
                        , ( uint16_t )*Dat
                        , ( int16_t ) retval );
    HAL::put_rs232NChar( buf, CNAMESPACE::strlen(( char* )buf ), 0 /*HAL::RS232_over_can_busnum*/ );
#endif
#endif

    return retval;
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
  inline int16_t initTaskCall( uint16_t wHandle, uint16_t wInterval, uint16_t wOffset, void ( * pfFunction )( void ) ) {
    int16_t retval = __HAL::init_task_call( wHandle, wInterval, wOffset, pfFunction );

#if DEBUG_HAL
#if 0
// don't use CNAMESPACE in header, doesn't always work properly
// maybe reactivate the statement above using getIrs232Instance(..)
    uint8_t buf[128];
    CNAMESPACE::sprintf(( char* )buf, "%u ms - init_task_call( %u, %u, %u, %s ) returns %i\r"
                        , ( uint16_t )__HAL::get_time()
                        , ( uint16_t )wHandle
                        , ( uint16_t )wInterval
                        , ( uint16_t )wOffset
                        , (( pfFunction ) ? "pfFunction" : "NULL" )
                        , ( int16_t ) retval
                       );
    HAL::put_rs232NChar( buf, CNAMESPACE::strlen(( char* )buf ), 0 /*HAL::RS232_over_can_busnum*/ );
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
  inline int16_t getTaskOverload( uint16_t wHandle ) {
    int16_t retval = __HAL::get_task_overload( wHandle );

#if DEBUG_HAL
#if 0
// don't use CNAMESPACE in header, doesn't always work properly
// maybe reactivate the statement above using getIrs232Instance(..)
    uint8_t buf[128];
    CNAMESPACE::sprintf(( char* )buf, "%u ms - get_task_overload( %u ) returns %i\r"
                        , ( uint16_t )__HAL::get_time()
                        , ( uint16_t )wHandle
                        , ( int16_t ) retval
                       );
    HAL::put_rs232NChar( buf, CNAMESPACE::strlen(( char* )buf ), 0 /*HAL::RS232_over_can_busnum*/ );
#endif
#endif

    return retval;
  }
  /**
    Reset Task Overload
  If a task has already been running and is called up a second time by a timer interrupt,
  a flag is set. The function get_task_overload returns the condition of this flag.
  With reset_task_overload this flag can be deleted.
  */
  inline int16_t resetTaskOverload( uint16_t wHandle ) {
    int16_t retval = __HAL::reset_task_overload( wHandle );

#if DEBUG_HAL
#if 0
// don't use CNAMESPACE in header, doesn't always work properly
// maybe reactivate the statement above using getIrs232Instance(..)
    uint8_t buf[128];
    CNAMESPACE::sprintf(( char* )buf, "%u ms - reset_task_overload( %u ) returns %i\r"
                        , ( uint16_t )__HAL::get_time()
                        , ( uint16_t )wHandle
                        , ( int16_t ) retval
                       );
    HAL::put_rs232NChar( buf, CNAMESPACE::strlen(( char* )buf ), 0 /*HAL::RS232_over_can_busnum*/ );
#endif
#endif

    return retval;
  }

  //TODO what does this function do? Is it used?
  inline void delay_us( unsigned int i_tm ) {
    __HAL::delay_us( i_tm );
  }
}

#ifdef USE_MUTUAL_EXCLUSION
#include <IsoAgLib/hal/generic_utils/system/mutex_emulated.h>
#endif

#endif
