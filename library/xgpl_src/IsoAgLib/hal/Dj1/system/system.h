/*
  system.h: definition of Hardware Abstraction Layer for central
    system functions for DICKEY-john hardware

  (C) Copyright 2009 - 2013 by OSB AG and developing partners

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
  inline void openSystem()
  {
    __HAL::DjBios_SysOpen();
    __HAL::DjBios_AnalogInit(10); /* Allow HAL::getAdcUbat() to work */
  };


  inline void closeSystem()
  {
    __HAL::DjBios_SysClose();
  };


  inline bool isSystemOpened()
  {
    return ( __HAL::DjBios_SysIsOpen() == __HAL::BIOS_YES );
  };


  inline void initWatchdog( void* ) {}


  inline void closeWatchdog() {}


  inline void triggerWatchdog()
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


  inline int16_t getAdcUbat()
  {
    return ( __HAL::DjBios_AnalogGetVoltsFilter(10) );
  };



  inline int16_t getAdc_u85()
  {
    return 8500;
  };


  inline bool getOn_offSwitch()
  {
    return (__HAL::DjBios_IgnOn() == __HAL::BIOS_TRUE );
  };


  inline void powerHold( bool ab_on )
  {
    __HAL::DjBios_PowerHold ( ab_on ? __HAL::BIOS_TRUE : __HAL::BIOS_FALSE );
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
