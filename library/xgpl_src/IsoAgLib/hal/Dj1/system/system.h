/*
  system.h: definition of Hardware Abstraction Layer for central
    system functions for DICKEY-john hardware

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

/** \file IsoAgLib/hal/Dj1/system/system.h
 * The header <i>\<target\>/\<device\>/\<device\>.h</i> performs a name
   mapping between platform specific BIOS / OS function names
   and the function names, the IsoAgLib uses for hardware access.
   In this header the groups for error codes, global system
   functions and CAN must be adopted to a new platform.
   If a simple name mapping is not possible for a specific
   function, the module targetExtensions can be used to implement
   the needed activity.
*/
/* ************************************************************ */
#ifndef _HAL_DJ1_SYSTEM_H_
#define _HAL_DJ1_SYSTEM_H_

#include <cstdio>
#include <cstring>
#include <cstdlib>

#include "../config.h"
#include "../typedef.h"
#include "../errcodes.h"

#include <IsoAgLib/util/impl/util_funcs.h>

namespace __HAL
{
  extern "C"
  {
    /** include the BIOS specific header into __HAL */
    #include <commercial_BIOS/bios_Dj1/DjBios1.h>

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
  inline int16_t  open_system ( void )
  {
    __HAL::DjBios_SysOpen();
    __HAL::DjBios_AnalogInit(10); /* Allow HAL::getAdcUbat() to work */
      return ( HAL_NO_ERR );
  };


  /**
    close the system with system specific function call
    @return error state (HAL_NO_ERR == o.k.)
  */
  inline int16_t  closeSystem ( void )
  {
    __HAL::DjBios_SysClose();
    return ( HAL_NO_ERR );
  };


  /** check if open_System() has already been called */
  inline bool isSystemOpened ( void )
  {
    return ( __HAL::DjBios_SysIsOpen() == __HAL::BIOS_YES );
  };


  /**
    configure the watchdog of the system with the
    settings of configC2C
    @return error state (HAL_NO_ERR == o.k.)
      or DATA_CHANGED on new values -> need call of wdReset to use new settings
    @see wdReset
  */
  inline int16_t configWatchdog()
  {
//jtm    return __HAL::configWatchdog();
    return ( HAL_NO_ERR );
  };


  /**
    reset the watchdog to use new configured watchdog settings
  */
  inline int16_t  wdReset ( void )
  {
    return ( HAL_NO_ERR );
//jtm    return __HAL::reset_wd();
  };


  /**
    trigger the watchdog
  */
  inline void wdTriggern ( void )
  {
    __HAL::DjBios_TaskWDSysService();
  };


  /**
    get the system time in [ms]
    @return [ms] since start of the system
  */
  inline int32_t getTime(void)
  {
    return ( (int32_t)__HAL::DjBios_TimeGetNow() );
  };



  /**
    getSerialNr - Get Serial Number, as a long integer
  */
  inline int32_t getSerialNr ( int16_t* pi16_errCode = NULL )
  {
    int32_t SerNum;
    __HAL::enum_SysStatus Stat;
    int I;

    /* try to read it three times */
    for ( I = 0; I < 3; I++)
    {
      SerNum = __HAL::DjBios_GetSerialNumber( &Stat );
      if ( Stat == __HAL::BIOS_SYS_NO_ERR )
        break;
    }

    if ( pi16_errCode != NULL )
    {
      /* translate the error */
      if ( Stat == __HAL::BIOS_SYS_NO_ERR )
      {
        *pi16_errCode = HAL_NO_ERR;
      }
      else if ( Stat == __HAL::BIOS_SYS_BAD_VAL )
      {
        *pi16_errCode = HAL_RANGE_ERR;
      }
      else if ( Stat == __HAL::BIOS_SYS_OP_FAIL )
      {
        *pi16_errCode = HAL_BUSY_ERR;
      }
      else
      {
        *pi16_errCode = HAL_UNKNOWN_ERR;
      }
    }

    return SerNum;
  };


  /**
    getSnr - Get Serial Number, an array of up to 6 characters
  */
  inline int16_t getSnr(uint8_t *snrDat)
  {
    int16_t retval = HAL_UNKNOWN_ERR;

    if ( snrDat != NULL )
    {
      int32_t SerNum = getSerialNr( &retval );

      if ( retval == HAL_NO_ERR )
      {
        /* only 6 characters truncate down to a 16-bit value */
        uint16_t SmSerNum = (uint16_t)SerNum;
        if ( CNAMESPACE::sprintf( (char*)snrDat, "%u", SmSerNum ) <= 0 )
        {
          /* sprintf() error */
          retval = HAL_UNKNOWN_ERR;
        }
      }
    }

    return retval;
  }


  /**
    Get Local ID
  */
  inline int16_t getLokalId(uint8_t *Dat)
  {
//jtm      int16_t retval = __HAL::get_lokal_id(Dat);
//jtm
//jtm    #if defined( DEBUG_HAL )
//jtm    //IsoAgLib::getIrs232Instance() << __HAL::get_time() << " ms - "
//jtm    //<< "getLokalId( "
//jtm    //<< (uint16_t) *Dat
//jtm    //<< " ) returns  "
//jtm    //<< retval << "\r";
//jtm
//jtm    uint8_t buf[128];
//jtm    CNAMESPACE::sprintf( (char*)buf, "%u ms - get_lokal_id( %u ) returns %i\r"
//jtm    , (uint16_t)__HAL::get_time()
//jtm    , (uint16_t)*Dat
//jtm    , (int16_t) retval );
//jtm    HAL::put_rs232NChar( buf, CNAMESPACE::strlen( (char*)buf ), 0 /*HAL::RS232_over_can_busnum*/ );
//jtm    #endif
//jtm
//jtm      return retval;

    return ( 0 );
  }

  /**
    start the Task Timer -> time between calls of Task Manager
  */
  inline void startTaskTimer ( void )
  {
//jtm      __HAL::start_task_timer ( T_TASK_BASIC );
//jtm
//jtm  #if defined( DEBUG_HAL )
//jtm  //IsoAgLib::getIrs232Instance() << __HAL::get_time() << " ms - "
//jtm  //<< "start_task_timer( "
//jtm  //<< (uint16_t) T_TASK_BASIC
//jtm  //<< " )\r";
//jtm
//jtm  uint8_t buf[128];
//jtm  CNAMESPACE::sprintf( (char*)buf, "%u ms - start_task_timer( %u )\r"
//jtm  , (uint16_t)__HAL::get_time()
//jtm  , (uint16_t)T_TASK_BASIC
//jtm  );
//jtm  HAL::put_rs232NChar( buf, CNAMESPACE::strlen( (char*)buf ), 0 /*HAL::RS232_over_can_busnum*/ );
//jtm  #endif

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
//jtm      int16_t retval = __HAL::init_task_call( wHandle, wInterval, wOffset, pfFunction );
//jtm
//jtm  #if defined( DEBUG_HAL )
//jtm  uint8_t buf[128];
//jtm  CNAMESPACE::sprintf( (char*)buf, "%u ms - init_task_call( %u, %u, %u, %s ) returns %i\r"
//jtm  , (uint16_t)__HAL::get_time()
//jtm  , (uint16_t)wHandle
//jtm  , (uint16_t)wInterval
//jtm  , (uint16_t)wOffset
//jtm  , ( (pfFunction) ? "pfFunction" : "NULL" )
//jtm  , (int16_t) retval
//jtm  );
//jtm  HAL::put_rs232NChar( buf, CNAMESPACE::strlen( (char*)buf ), 0 /*HAL::RS232_over_can_busnum*/ );
//jtm  #endif
//jtm    return retval;
//jtm      }
//jtm    /**
//jtm      Get Task Overload
//jtm    If a task has already been running and is called up a second time by a timer interrupt,
//jtm    a flag is set. The function get_task_overload returns the condition of this flag.
//jtm    With reset_task_overload this flag can be deleted.
//jtm    */
//jtm    inline int16_t getTaskOverload ( uint16_t /*wHandle*/ )
//jtm      {
//jtm      int16_t retval = __HAL::get_task_overload ();
//jtm
//jtm  #if defined( DEBUG_HAL )
//jtm  uint8_t buf[128];
//jtm  CNAMESPACE::sprintf( (char*)buf, "%u ms - get_task_overload( %u ) returns %i\r"
//jtm  , (uint16_t)__HAL::get_time()
//jtm  , (uint16_t)wHandle
//jtm  , (int16_t) retval
//jtm  );
//jtm  HAL::put_rs232NChar( buf, CNAMESPACE::strlen( (char*)buf ), 0 /*HAL::RS232_over_can_busnum*/ );
//jtm  #endif
//jtm
//jtm    return retval;
    return ( 0 );
  };



  /**
    get the main power voltage
    @return voltage of power [mV]
  */
  inline int16_t  getAdcUbat( void )
  {
    return ( __HAL::DjBios_AnalogGetVoltsFilter(10) );
  };



  /**
    get the voltage of the external reference 8.5Volt for work of external sensors
    @return voltage at external reference [mV]
  */
  inline int16_t  getAdc_u85( void )
  {
    return 8500;
  };



  /**
    check if Ignition Sense/CAN_EN is active
      (if NO_CAN_EN_CHECK is defined (in hal\dj1\config.h) this function return
       always return ON)
    @return ON(1) or OFF(0)
  */
  inline int16_t  getOn_offSwitch ( void )
  {
#if defined(NO_CAN_EN_CHECK)
    return ON
#else
    return ( (__HAL::DjBios_IgnOn() == __HAL::BIOS_TRUE) ? 1 : 0 );
#endif

  };


  /**
    activate the power selfholding to perform system
    stop (f.e. store values) actions after loss of CAN_EN
  */
  inline void stayingAlive ( void )
  {
    __HAL::DjBios_PowerHold ( __HAL::BIOS_TRUE );
  };


  /**
    shut off the whole system (set power down)
  */
  inline void powerDown ( void )
  {
    __HAL::DjBios_PowerHold ( __HAL::BIOS_FALSE );
  };


  /**
    switch relais on or off
    @param bitState true -> Relais ON
  */
  inline void setRelais(bool bitState)
  {
    /* ON during system Initialization and off during close */
    __HAL::DjBios_PowerHold ( bitState ? __HAL::BIOS_TRUE: __HAL::BIOS_FALSE );
  };


/*@}*/
}

#ifdef USE_MUTUAL_EXCLUSION
#include <IsoAgLib/hal/generic_utils/system/mutex_emulated.h>
#endif


#endif
