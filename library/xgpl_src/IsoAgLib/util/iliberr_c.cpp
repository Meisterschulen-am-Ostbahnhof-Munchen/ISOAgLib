/*
  iliberr_c.cpp: central object for error management

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include "iliberr_c.h"
#include "config.h"
#include <IsoAgLib/hal/hal_system.h>
#include <algorithm>


#if DEBUG_REGISTERERROR
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

struct UpdateErrorObserver_s : public STL_NAMESPACE::unary_function< iErrorObserver_c *, void > {
  void operator()(iErrorObserver_c *pc_errorObserver) {
    pc_errorObserver->error(mt_errorType, mt_errLocation);
  }
  UpdateErrorObserver_s(
      iErrorObserver_c::ErrorType_t at_errorType,
      iErrorObserver_c::ErrorLocation_t at_errLocation) :
    mt_errorType(at_errorType),
    mt_errLocation(at_errLocation) {}
  iErrorObserver_c::ErrorType_t mt_errorType;
  iErrorObserver_c::ErrorLocation_t mt_errLocation;
};

/** register an error
  * @param at_errType type of occured error
  * @param at_errLocation location of error
  */
void iLibErr_c::registerError( iLibErr_c::iLibErrTypes_t at_errType, iLibErrLocations_t at_errLocation )
{
  if ((at_errLocation != AllErrLocations) && (at_errType != AllErrTypes)) {
    errTypeAtLoc [at_errLocation].set (at_errType);
  }

  STL_NAMESPACE::for_each(c_arrClientC1.begin(), c_arrClientC1.end(), UpdateErrorObserver_s(at_errType, at_errLocation));

#if DEBUG_REGISTERERROR
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
