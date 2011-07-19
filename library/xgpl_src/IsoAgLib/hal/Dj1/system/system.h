/*
  system.h: definition of Hardware Abstraction Layer for central
    system functions for DICKEY-john hardware

  (C) Copyright 2009 - 2011 by OSB AG and developing partners

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

#ifndef _HAL_DJ1_SYSTEM_H_
#define _HAL_DJ1_SYSTEM_H_

#include <cstdio>
#include <cstring>
#include <cstdlib>

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


namespace HAL
{
  inline int16_t open_system()
  {
    __HAL::DjBios_SysOpen();
    __HAL::DjBios_AnalogInit(10); /* Allow HAL::getAdcUbat() to work */
      return ( HAL_NO_ERR );
  };


  inline int16_t closeSystem()
  {
    __HAL::DjBios_SysClose();
    return ( HAL_NO_ERR );
  };


  inline bool isSystemOpened()
  {
    return ( __HAL::DjBios_SysIsOpen() == __HAL::BIOS_YES );
  };


  inline int16_t configWatchdog()
  {
//jtm    return __HAL::configWatchdog();
    return ( HAL_NO_ERR );
  };


  inline void wdReset()
  {
//jtm    (void)__HAL::reset_wd();
  };


  inline void wdTriggern()
  {
    __HAL::DjBios_TaskWDSysService();
  };


  inline int32_t getTime()
  {
    return ( (int32_t)__HAL::DjBios_TimeGetNow() );
  };


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


  inline int16_t getLokalId(uint8_t *Dat)
  {
//jtm      int16_t retval = __HAL::get_lokal_id(Dat);
//jtm
//jtm    #if DEBUG_HAL
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


  inline void startTaskTimer()
  {
//jtm      __HAL::start_task_timer ( T_TASK_BASIC );
//jtm
//jtm  #if DEBUG_HAL
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


  inline int16_t initTaskCall( uint16_t wHandle, uint16_t wInterval, uint16_t wOffset, void (* pfFunction)(void) )
  {
//jtm      int16_t retval = __HAL::init_task_call( wHandle, wInterval, wOffset, pfFunction );
//jtm
//jtm  #if DEBUG_HAL
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
//jtm  #if DEBUG_HAL
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


  inline int16_t getAdcUbat()
  {
    return ( __HAL::DjBios_AnalogGetVoltsFilter(10) );
  };



  inline int16_t getAdc_u85()
  {
    return 8500;
  };


  inline int16_t getOn_offSwitch()
  {
#if defined(NO_CAN_EN_CHECK)
    return ON
#else
    return ( (__HAL::DjBios_IgnOn() == __HAL::BIOS_TRUE) ? 1 : 0 );
#endif
  };


  inline void stayingAlive()
  {
    __HAL::DjBios_PowerHold ( __HAL::BIOS_TRUE );
  };


  inline void powerDown()
  {
    __HAL::DjBios_PowerHold ( __HAL::BIOS_FALSE );
  };


  inline void sleep_max_ms( uint32_t ms )
  {
    // currently no sleeping on Dj1 hardware, but that's fine!
  }
}


#ifdef USE_MUTUAL_EXCLUSION
#include <IsoAgLib/hal/generic_utils/system/mutex_emulated.h>
#endif


#endif
