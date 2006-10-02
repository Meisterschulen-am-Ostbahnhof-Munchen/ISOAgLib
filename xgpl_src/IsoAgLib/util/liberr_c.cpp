/***************************************************************************
                          liberr_c.cpp  - central object for error management
                             -------------------
    begin                : Thu Mar 16 2000
    copyright            : (C) 2000 - 2004 by Dipl.-Inform. Achim Spangler
    email                : spangler@tec.agrar.tu-muenchen.de
    type                 : Source
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
 * This library is free software; you Can redistribute it and/or modify it *
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
#include "liberr_c.h"
#include "config.h"
#include <IsoAgLib/hal/system.h>


#ifdef DEBUG
	#ifdef SYSTEM_PC
		#include <iostream>
	#else
		#include <supplementary_driver/driver/rs232/impl/rs232io_c.h>
	#endif
#endif

// Begin Namespace IsoAgLib
namespace __IsoAgLib {
  /** C-style function, to get access to the unique LibErr_c singleton instance */
  LibErr_c& getLibErrInstance( void ) { return IsoAgLib::getLibErrInstance();}
}

namespace IsoAgLib {
/** C-style function, to get access to the unique LibErr_c singleton instance */
LibErr_c& getLibErrInstance( void )
{
  static LibErr_c& refc_errInstance = LibErr_c::instance();
  return refc_errInstance;
}

/**
  Initialize the unique error state handler
  @return true -> everything without errors initialised
*/
bool LibErr_c::init( void )
{
	clear( AllErrTypes, AllErrLocations );
//  ui32_lastErrorTime = 0;
  return true;
}

/** default constructor which sets the error value to noErr */
LibErr_c::LibErr_c() : errTypeAtLoc() // : ui32_lastErrorTime(0)
{ }

/** copy constructor which sets the error value to the err value of the source */
LibErr_c::LibErr_c(const LibErr_c& rrefc_src)
{
  for (int i=0; i<AllErrLocations; i++) {
    errTypeAtLoc [i] = rrefc_src.errTypeAtLoc [i];
  }
//  CNAMESPACE::memcpy( &errTypeBitfield, &(rrefc_src.errTypeBitfield), sizeof(errTypeBitfield) );
//  CNAMESPACE::memcpy( &errLocBitfield, &(rrefc_src.errLocBitfield), sizeof(errLocBitfield) );
}

/** register an error
  * @param rt_errType type of occured error
  * @param rt_errLocation location of error
  */
void LibErr_c::registerError( LibErr_c::LibErrTypes_t rt_errType, LibErrLocations_t rt_errLocation )
{
  if ((rt_errLocation != AllErrLocations) && (rt_errType != AllErrTypes)) {
    errTypeAtLoc [rt_errLocation] [rt_errType] = 1;
  }

#ifdef DEBUG
  static int32_t si32_nextDebug = 0;
  static LibErr_c::LibErrTypes_t st_lastDebugErrType;
  static LibErrLocations_t st_lastDebugErrLocation;
  if ( ( HAL::getTime() > si32_nextDebug )
    || ( st_lastDebugErrType != rt_errType )
    || ( st_lastDebugErrLocation != rt_errLocation ) )
  {
    si32_nextDebug = HAL::getTime() + 500;
    st_lastDebugErrType = rt_errType;
    st_lastDebugErrLocation = rt_errLocation;
    INTERNAL_DEBUG_DEVICE
      << "registerError( " << rt_errType << ", " << rt_errLocation << " )" << INTERNAL_DEBUG_DEVICE_ENDL;
  }
#endif

/*
  switch ( rt_errType ) {
  case Precondition: errTypeBitfield.Precondition++;  break;
  case BadAlloc: errTypeBitfield.BadAlloc++;        break;
  case ElNonexistent: errTypeBitfield.ElNonexistent++;    break;
  case Unspecified: errTypeBitfield.Unspecified++;    break;
  case HwConfig: errTypeBitfield.HwConfig++;        break;
  case Range: errTypeBitfield.Range++;                break;
  case Busy: errTypeBitfield.Busy++;                  break;
  case SystemOpen: errTypeBitfield.SystemOpen++;    break;
  case SystemWatchdog: errTypeBitfield.SystemWatchdog++;    break;
  case CanOff: errTypeBitfield.CanOff++;            break;
  case CanWarn: errTypeBitfield.CanWarn++;          break;
  case CanBus: errTypeBitfield.CanBus++;            break;
  case CanMsgObj: errTypeBitfield.CanMsgObj++;    break;
  case CanMem: errTypeBitfield.CanMem++;            break;
  case CanOverflow: errTypeBitfield.CanOverflow++;  break;
  case CanConflict: errTypeBitfield.CanConflict++;  break;
  case CanDiffSpeed: errTypeBitfield.CanDiffSpeed++;    break;
  case Rs232Overflow: errTypeBitfield.Rs232Overflow++;    break;
  case Rs232Underflow: errTypeBitfield.Rs232Underflow++;  break;
  case EepromSegment: errTypeBitfield.EepromSegment++;    break;
  case EepromWriteError: errTypeBitfield.EepromWriteError++;    break;
  case SysNoActiveLocalMember: errTypeBitfield.SysNoActiveLocalMember++;    break;
  case BaseSenderConflict: errTypeBitfield.BaseSenderConflict++;                break;
  case IsoTerminalOutOfMemory: errTypeBitfield.IsoTerminalOutOfMemory++;            break;
  case LbsMultiSendBusy: errTypeBitfield.LbsMultiSendBusy++;                        break;
  case AllErrTypes: break; // should not happen in work - but to make compiler happy
  }
  switch ( rt_errLocation ) {
    case Lbs: errLocBitfield.Lbs++;            break;
    case Base: errLocBitfield.Base++;  break;
    case System: errLocBitfield.System++;      break;
    case Process: errLocBitfield.Process++;    break;
    case LbsTerminal: errLocBitfield.LbsTerminal++;  break;
    case LbsMultipacket: errLocBitfield.LbsMultipacket++;    break;
    case HwSystem: errLocBitfield.HwSystem++;      break;
    case Can: errLocBitfield.Can++;            break;
    case Eeprom: errLocBitfield.Eeprom++;      break;
    case Sensor: errLocBitfield.Sensor++;      break;
    case Actor: errLocBitfield.Actor++;        break;
    case Rs232: errLocBitfield.Rs232++;        break;
    case IsoTerminal: errLocBitfield.IsoTerminal++;  break;
    case AllErrLocations: break; // should not happen in work - but to make compiler happy
  }
*/
//  ui32_lastErrorTime = HAL::getTime();
}

/**
  clear all error information of a special location
  @param rt_errType select for which error types the counter shall be reset (default reset all counters)
void LibErr_c::clear( LibErrTypes_t rt_errType )
{
  switch ( rt_errType ) {
    case AllErrTypes: CNAMESPACE::memset( &errTypeBitfield, 0, sizeof(errTypeBitfield) ); break;
    case Precondition: errTypeBitfield.Precondition = 0;  break;
    case BadAlloc: errTypeBitfield.BadAlloc = 0;        break;
    case ElNonexistent: errTypeBitfield.ElNonexistent = 0;    break;
    case Unspecified: errTypeBitfield.Unspecified = 0;    break;
    case HwConfig: errTypeBitfield.HwConfig = 0;        break;
    case Range: errTypeBitfield.Range = 0;                break;
    case Busy: errTypeBitfield.Busy = 0;                  break;
    case SystemOpen: errTypeBitfield.SystemOpen = 0;    break;
    case SystemWatchdog: errTypeBitfield.SystemWatchdog = 0;    break;
    case CanOff: errTypeBitfield.CanOff = 0;            break;
    case CanWarn: errTypeBitfield.CanWarn = 0;          break;
    case CanBus: errTypeBitfield.CanBus = 0;            break;
    case CanMsgObj: errTypeBitfield.CanMsgObj = 0;    break;
    case CanMem: errTypeBitfield.CanMem = 0;            break;
    case CanOverflow: errTypeBitfield.CanOverflow = 0;  break;
    case CanConflict: errTypeBitfield.CanConflict = 0;  break;
    case CanDiffSpeed: errTypeBitfield.CanDiffSpeed = 0;    break;
    case Rs232Overflow: errTypeBitfield.Rs232Overflow = 0;    break;
    case Rs232Underflow: errTypeBitfield.Rs232Underflow = 0;  break;
    case EepromSegment: errTypeBitfield.EepromSegment = 0;    break;
    case EepromWriteError: errTypeBitfield.EepromWriteError = 0;    break;
    case SysNoActiveLocalMember: errTypeBitfield.SysNoActiveLocalMember = 0;    break;
    case BaseSenderConflict: errTypeBitfield.BaseSenderConflict = 0;                break;
    case IsoTerminalOutOfMemory: errTypeBitfield.IsoTerminalOutOfMemory = 0;            break;
    case LbsMultiSendBusy: errTypeBitfield.LbsMultiSendBusy = 0;                        break;
  }
}
*/

/**
  clear specific error information of a special location
  @param rt_errLocation select for which error locations the counter shall be reset
  @param rt_errType select for which error types the counter shall be reset (default reset all counters)
*/
void LibErr_c::clear( LibErrTypes_t rt_errType, LibErrLocations_t rt_errLocation ) {
  if (rt_errType == AllErrTypes) {
    clear (rt_errLocation); // Clear complete Location
  } else {
    if (rt_errLocation == AllErrLocations) {
      for (int i=0; i<AllErrLocations; i++) {
        errTypeAtLoc [i].reset (rt_errType);
      }
    } else {
      errTypeAtLoc [rt_errLocation].reset (rt_errType);
    }
  }
}

/**
  clear all error information of a special location
  @param rt_errLocation select for which error locations the counter shall be reset
*/
void LibErr_c::clear( LibErrLocations_t rt_errLocation ) {
  if (rt_errLocation == AllErrLocations) {
    for (int i=0; i<AllErrLocations; i++) {
      errTypeAtLoc [i].reset ();
    }
  } else {
    errTypeAtLoc [rt_errLocation].reset ();
  }
/*
  switch ( rt_errLocation ) {
    case AllErrLocations: CNAMESPACE::memset( &errLocBitfield, 0, sizeof(errLocBitfield) ); break;
    case Lbs: errLocBitfield.Lbs = 0;            break;
    case Base: errLocBitfield.Base = 0;  break;
    case System: errLocBitfield.System = 0;      break;
    case Process: errLocBitfield.Process = 0;    break;
    case LbsTerminal: errLocBitfield.LbsTerminal = 0;  break;
    case LbsMultipacket: errLocBitfield.LbsMultipacket = 0;    break;
    case HwSystem: errLocBitfield.HwSystem = 0;      break;
    case Can: errLocBitfield.Can = 0;            break;
    case Eeprom: errLocBitfield.Eeprom = 0;      break;
    case Sensor: errLocBitfield.Sensor = 0;      break;
    case Actor: errLocBitfield.Actor = 0;        break;
    case Rs232: errLocBitfield.Rs232 = 0;        break;
    case IsoTerminal: errLocBitfield.IsoTerminal = 0; break;
  }
*/
}

/**
  deliver the count of registered errors
  @param rt_errType optional select the error-types to count (default all error types)
  @return count of registered error types
*/
uint16_t LibErr_c::getErrCnt( LibErrTypes_t rt_errType ) const
{
  uint16_t errors=0;
  if (rt_errType == AllErrTypes) {
    for (int i=0; i<AllErrLocations; i++) {
      errors += errTypeAtLoc [i].count ();
    }
  } else {
    for (int i=0; i<AllErrLocations; i++) {
      if (errTypeAtLoc [i].test (rt_errType)) errors++;
    }
  }
  return errors;
/*
  uint8_t ui8_result = 0;
  switch ( rt_errType ) {
    case AllErrTypes:
      ui8_result += Precondition;
      ui8_result += BadAlloc;
      ui8_result += ElNonexistent;
      ui8_result += Unspecified;
      ui8_result += HwConfig;
      ui8_result += Range;
      ui8_result += Busy;
      ui8_result += SystemOpen;
      ui8_result += SystemWatchdog;
      ui8_result += CanOff;
      ui8_result += CanWarn;
      ui8_result += CanBus;
      ui8_result += CanMsgObj;
      ui8_result += CanMem;
      ui8_result += CanOverflow;
      ui8_result += CanConflict;
      ui8_result += CanDiffSpeed;
      ui8_result += Rs232Overflow;
      ui8_result += Rs232Underflow;
      ui8_result += EepromSegment;
      ui8_result += EepromWriteError;
      ui8_result += SysNoActiveLocalMember;
      ui8_result += BaseSenderConflict;
      ui8_result += IsoTerminalOutOfMemory;
      ui8_result += LbsMultiSendBusy;
      break;
    case Precondition: ui8_result = errTypeBitfield.Precondition;  break;
    case BadAlloc: ui8_result = errTypeBitfield.BadAlloc;        break;
    case ElNonexistent: ui8_result = errTypeBitfield.ElNonexistent;    break;
    case Unspecified: ui8_result = errTypeBitfield.Unspecified;    break;
    case HwConfig: ui8_result = errTypeBitfield.HwConfig;        break;
    case Range: ui8_result = errTypeBitfield.Range;                break;
    case Busy: ui8_result = errTypeBitfield.Busy;                  break;
    case SystemOpen: ui8_result = errTypeBitfield.SystemOpen;    break;
    case SystemWatchdog: ui8_result = errTypeBitfield.SystemWatchdog;    break;
    case CanOff: ui8_result = errTypeBitfield.CanOff;            break;
    case CanWarn: ui8_result = errTypeBitfield.CanWarn;          break;
    case CanBus: ui8_result = errTypeBitfield.CanBus;            break;
    case CanMsgObj: ui8_result = errTypeBitfield.CanMsgObj;    break;
    case CanMem: ui8_result = errTypeBitfield.CanMem;            break;
    case CanOverflow: ui8_result = errTypeBitfield.CanOverflow;  break;
    case CanConflict: ui8_result = errTypeBitfield.CanConflict;  break;
    case CanDiffSpeed: ui8_result = errTypeBitfield.CanDiffSpeed;    break;
    case Rs232Overflow: ui8_result = errTypeBitfield.Rs232Overflow;    break;
    case Rs232Underflow: ui8_result = errTypeBitfield.Rs232Underflow;  break;
    case EepromSegment: ui8_result = errTypeBitfield.EepromSegment;    break;
    case EepromWriteError: ui8_result = errTypeBitfield.EepromWriteError;    break;
    case SysNoActiveLocalMember: ui8_result = errTypeBitfield.SysNoActiveLocalMember;  break;
    case BaseSenderConflict: ui8_result = errTypeBitfield.BaseSenderConflict;                break;
    case IsoTerminalOutOfMemory: ui8_result = errTypeBitfield.IsoTerminalOutOfMemory;       break;
    case LbsMultiSendBusy: ui8_result = errTypeBitfield.LbsMultiSendBusy;       }
  return ui8_result;
*/
}

/**
  deliver the type of the nth error
  @param rui8_ind index of the requested error ( [0..(ErrCnt-1)] )
  @return LibErrTypes_t
*/
LibErr_c::LibErrTypes_t LibErr_c::getErrIndType( uint8_t rui8_ind ) const
{
  int16_t i16_foundInd = -1;
  if ( rui8_ind >= getErrCnt( AllErrTypes ) ) return AllErrTypes;

  for (int i=0; i<AllErrLocations; i++)
  {
    for ( int j=0; j<AllErrTypes; j++)
    {
      if ( errTypeAtLoc [i].test (j))
      {
        i16_foundInd++;
        if ( i16_foundInd == rui8_ind ) return LibErrTypes_t(j);
      }
    }
  }

  return AllErrTypes;
}

/**
  deliver the location of the nth error
  @param rui8_ind index of the requested error ( [0..(ErrCnt-1)] )
  @return LibErrTypes_t
*/
LibErr_c::LibErrLocations_t LibErr_c::getErrIndLocation( uint8_t rui8_ind ) const
{
  int16_t i16_foundInd = -1;
  if ( rui8_ind >= getErrCnt( AllErrTypes ) ) return AllErrLocations;

  for (int i=0; i<AllErrLocations; i++)
  {
    for ( int j=0; j<AllErrTypes; j++)
    {
      if ( errTypeAtLoc [i].test (j))
      {
        i16_foundInd++;
        if ( i16_foundInd == rui8_ind ) return LibErrLocations_t(i);
      }
    }
  }

  return AllErrLocations;
}




/**
  deliver the count of registered errors with a special error location
  @param rt_errLocation location to check for errors
  @return count of registered error types
*/
uint16_t LibErr_c::getErrCnt( LibErrLocations_t rt_errLocation ) const
{
  uint16_t errors=0;
  if (rt_errLocation == AllErrLocations) {
    for (int i=0; i<AllErrLocations; i++) {
      errors += errTypeAtLoc [i].count ();
    }
  } else {
    errors += errTypeAtLoc [rt_errLocation].count ();
  }
  return errors;
/*
  uint8_t ui8_result = 0;
  switch ( rt_errLocation ) {
    case AllErrLocations:
      ui8_result += errLocBitfield.Lbs;
      ui8_result += errLocBitfield.Base;
      ui8_result += errLocBitfield.System;
      ui8_result += errLocBitfield.Process;
      ui8_result += errLocBitfield.LbsTerminal;
      ui8_result += errLocBitfield.LbsMultipacket;
      ui8_result += errLocBitfield.HwSystem;
      ui8_result += errLocBitfield.Can;
      ui8_result += errLocBitfield.Eeprom;
      ui8_result += errLocBitfield.Sensor;
      ui8_result += errLocBitfield.Actor;
      ui8_result += errLocBitfield.Rs232;
      ui8_result += errLocBitfield.IsoTerminal;
      break;
    case Lbs: ui8_result = errLocBitfield.Lbs;            break;
    case Base: ui8_result = errLocBitfield.Base;  break;
    case System: ui8_result = errLocBitfield.System;      break;
    case Process: ui8_result = errLocBitfield.Process;    break;
    case LbsTerminal: ui8_result = errLocBitfield.LbsTerminal;  break;
    case LbsMultipacket: ui8_result = errLocBitfield.LbsMultipacket;    break;
    case HwSystem: ui8_result = errLocBitfield.HwSystem;      break;
    case Can: ui8_result = errLocBitfield.Can;            break;
    case Eeprom: ui8_result = errLocBitfield.Eeprom;      break;
    case Sensor: ui8_result = errLocBitfield.Sensor;      break;
    case Actor: ui8_result = errLocBitfield.Actor;        break;
    case Rs232: ui8_result = errLocBitfield.Rs232;        break;
    case IsoTerminal: ui8_result = errLocBitfield.IsoTerminal;  break;
  }
  return ui8_result;
*/
}

/**
  check if the error value reports correct error free state
  @param rt_errType optional select the error-types to count (default all error types)
  @return true -> the defined error locations report no error
*/
bool LibErr_c::good( LibErrTypes_t rt_errType ) const
{
  uint16_t errors=0;
  if ( rt_errType == AllErrTypes ) {
    for (int i=0; i<AllErrTypes; i++) {
      errors += getErrCnt( (LibErrTypes_t) i );
    }
  }
  else {
    errors = getErrCnt( rt_errType );
  }
  return (errors > 0 )?false:true;
}

/**
  check if the error value reports correct error free state
  @param rt_errLocation location to check for errors
  @return true -> the defined error locations report no error
*/
bool LibErr_c::good( LibErrLocations_t rt_errLocation ) const
{
  uint16_t errors=0;
  if ( rt_errLocation == AllErrLocations ) {
    for (int i=0; i<AllErrLocations; i++) {
      errors += getErrCnt( (LibErrLocations_t) i );
    }
  }
  else {
    errors = getErrCnt( rt_errLocation );
  }
  return (errors > 0 )?false:true;
/*
  if ( rt_errLocation == AllErrLocations ) {
    uint8_t temp[sizeof(errLocBitfield)];
    CNAMESPACE::memset( temp, 0, sizeof(errLocBitfield) );
    // compare errLocBitfield with string of "\0" -> if all bytes in errLocBitfield
    // are 0 memcmp returns 0
    return ( CNAMESPACE::memcmp( &errLocBitfield, temp, sizeof(errLocBitfield) ) == 0)?true:false;
  }
  else {
    return (getErrCnt( rt_errLocation ) > 0 )?false:true;
  }
*/
}

bool LibErr_c::good( LibErrTypes_t rt_errType, LibErrLocations_t rt_errLocation ) const
{
  if (!good( rt_errLocation ))
    return false;

  if (!good( rt_errType ))
    return false;

  return true;
}


/** default destructor which has nothing to do */
LibErr_c::~LibErr_c()
{
}

} // end of namespace IsoAgLib
