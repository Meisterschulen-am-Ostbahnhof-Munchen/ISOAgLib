/***************************************************************************
                          iliberr_c.cpp  - central object for error management
                             -------------------
    begin                : Thu Mar 16 2000
    copyright            : (C) 2000 - 2009 by Dipl.-Inform. Achim Spangler/Dipl.-Inf. Martin Wodok
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
 * Copyright (C) 1999 - 2009 Dipl.-Inform. Achim Spangler                  *
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
#include "iliberr_c.h"
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
  /** C-style function, to get access to the unique iLibErr_c singleton instance */
  iLibErr_c& getILibErrInstance( void ) { return IsoAgLib::getILibErrInstance();}
}

namespace IsoAgLib {
/** C-style function, to get access to the unique iLibErr_c singleton instance */
iLibErr_c& getILibErrInstance( void )
{
  static iLibErr_c& rc_errInstance = iLibErr_c::instance();
  return rc_errInstance;
}

/**
  Initialize the unique error state handler
  @return true -> everything without errors initialised
*/
bool iLibErr_c::init( void )
{
	clear( AllErrTypes, AllErrLocations );
//  ui32_lastErrorTime = 0;
  return true;
}

/** default constructor which sets the error value to noErr */
iLibErr_c::iLibErr_c() : errTypeAtLoc() // : ui32_lastErrorTime(0)
{ }

/** copy constructor which sets the error value to the err value of the source */
iLibErr_c::iLibErr_c(const iLibErr_c& acrc_src)
{
  for (int i=0; i<AllErrLocations; i++)
  { // copy all IsoaglibBitset for all locations!
    errTypeAtLoc [i] = acrc_src.errTypeAtLoc [i];
  }
}

/** register an error
  * @param at_errType type of occured error
  * @param at_errLocation location of error
  */
void iLibErr_c::registerError( iLibErr_c::iLibErrTypes_t at_errType, iLibErrLocations_t at_errLocation )
{
  if ((at_errLocation != AllErrLocations) && (at_errType != AllErrTypes)) {
    errTypeAtLoc [at_errLocation].set (at_errType);
  }

#ifdef DEBUG
  static int32_t si32_nextDebug = 0;
  static iLibErr_c::iLibErrTypes_t st_lastDebugErrType;
  static iLibErrLocations_t st_lastDebugErrLocation;
  if ( ( HAL::getTime() > si32_nextDebug )
    || ( st_lastDebugErrType != at_errType )
    || ( st_lastDebugErrLocation != at_errLocation ) )
  {
    si32_nextDebug = HAL::getTime() + 500;
    st_lastDebugErrType = at_errType;
    st_lastDebugErrLocation = at_errLocation;
    INTERNAL_DEBUG_DEVICE
      << "registerError( " << at_errType << ", " << at_errLocation << " )" << INTERNAL_DEBUG_DEVICE_ENDL;
  }
#endif
}


/**
  clear specific error information of a special location
  @param at_errLocation select for which error locations the counter shall be reset
  @param at_errType select for which error types the counter shall be reset (default reset all counters)
*/
void iLibErr_c::clear( iLibErrTypes_t at_errType, iLibErrLocations_t at_errLocation ) {
  if (at_errType == AllErrTypes)
  { // Clear all types on (specifc/all) Location(s)
    clear (at_errLocation);
  }
  else
  { // clear specific type  on (specific/all) Location(s)
    if (at_errLocation == AllErrLocations) {
      for (int i=0; i<AllErrLocations; i++) {
        errTypeAtLoc [i].reset (at_errType);
      }
    } else {
      errTypeAtLoc [at_errLocation].reset (at_errType);
    }
  }
}


/**
  clear all error information of a special location
  @param at_errLocation select for which error locations the counter shall be reset
*/
void iLibErr_c::clear( iLibErrLocations_t at_errLocation ) {
  if (at_errLocation == AllErrLocations) {
    for (int i=0; i<AllErrLocations; i++) {
      errTypeAtLoc [i].reset ();
    }
  } else {
    errTypeAtLoc [at_errLocation].reset ();
  }
}


/**
  deliver the count of registered errors
  @param at_errType optional select the error-types to count (default all error types)
  @return count of registered error types
*/
uint16_t iLibErr_c::getErrCnt( iLibErrTypes_t at_errType ) const
{
  uint16_t errors=0;
  if (at_errType == AllErrTypes) {
    for (int i=0; i<AllErrLocations; i++) {
      errors += errTypeAtLoc [i].count ();
    }
  } else {
    for (int i=0; i<AllErrLocations; i++) {
      if (errTypeAtLoc [i].test (at_errType)) errors++;
    }
  }
  return errors;
}


/**
  deliver the type of the nth error
  @param aui8_ind index of the requested error ( [0..(ErrCnt-1)] )
  @return iLibErrTypes_t
*/
iLibErr_c::iLibErrTypes_t iLibErr_c::getErrIndType( uint8_t aui8_ind ) const
{
  int16_t i16_foundInd = -1;
  if ( aui8_ind >= getErrCnt( AllErrTypes ) ) return AllErrTypes;

  for (int i=0; i<AllErrLocations; i++)
  {
    for ( int j=0; j<AllErrTypes; j++)
    {
      if ( errTypeAtLoc [i].test (j))
      {
        i16_foundInd++;
        if ( i16_foundInd == aui8_ind ) return iLibErrTypes_t(j);
      }
    }
  }

  return AllErrTypes;
}


/**
  deliver the location of the nth error
  @param aui8_ind index of the requested error ( [0..(ErrCnt-1)] )
  @return iLibErrTypes_t
*/
iLibErr_c::iLibErrLocations_t iLibErr_c::getErrIndLocation( uint8_t aui8_ind ) const
{
  int16_t i16_foundInd = -1;
  if ( aui8_ind >= getErrCnt( AllErrTypes ) ) return AllErrLocations;

  for (int i=0; i<AllErrLocations; i++)
  {
    for ( int j=0; j<AllErrTypes; j++)
    {
      if ( errTypeAtLoc [i].test (j))
      {
        i16_foundInd++;
        if ( i16_foundInd == aui8_ind ) return iLibErrLocations_t(i);
      }
    }
  }

  return AllErrLocations;
}


/**
  deliver the count of registered errors with a special error location
  @param at_errLocation location to check for errors
  @return count of registered error types
*/
uint16_t iLibErr_c::getErrCnt( iLibErrLocations_t at_errLocation ) const
{
  uint16_t errors=0;
  if (at_errLocation == AllErrLocations) {
    for (int i=0; i<AllErrLocations; i++) {
      errors += errTypeAtLoc [i].count ();
    }
  } else {
    errors += errTypeAtLoc [at_errLocation].count ();
  }
  return errors;
}


/**
  check if the error value reports correct error free state
  @param at_errType optional select the error-types to count (default all error types)
  @return true -> the defined error locations report no error
*/
bool iLibErr_c::good( iLibErrTypes_t at_errType ) const
{
  uint16_t errors=0;
  if ( at_errType == AllErrTypes ) {
    for (int i=0; i<AllErrTypes; i++) {
      errors += getErrCnt( (iLibErrTypes_t) i );
    }
  }
  else {
    errors = getErrCnt( at_errType );
  }
  return !(errors > 0 );
}


/**
  check if the error value reports correct error free state
  @param at_errLocation location to check for errors
  @return true -> the defined error locations report no error
*/
bool iLibErr_c::good( iLibErrLocations_t at_errLocation ) const
{
  uint16_t errors=0;
  if ( at_errLocation == AllErrLocations ) {
    for (int i=0; i<AllErrLocations; i++) {
      errors += getErrCnt( (iLibErrLocations_t) i );
    }
  }
  else {
    errors = getErrCnt( at_errLocation );
  }
  return !(errors > 0);
}



bool iLibErr_c::good( iLibErrTypes_t at_errType, iLibErrLocations_t at_errLocation ) const
{
  if (!good( at_errLocation ))
    return false;

  if (!good( at_errType ))
    return false;

  return true;
}


} // end of namespace IsoAgLib
