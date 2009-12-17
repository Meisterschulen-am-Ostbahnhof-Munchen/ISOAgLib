/*
  actoro_c.cpp:
    implementation for the actor output management object

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include "actoro_c.h"
#include <IsoAgLib/util/iliberr_c.h>
#include <IsoAgLib/util/config.h>
#include <IsoAgLib/driver/system/impl/system_c.h>

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {
/** C-style function, to get access to the unique ActorO_c singleton instance */
ActorO_c& getActorInstance( void ) { return ActorO_c::instance();};

/**
  initialisation for the actor output management which sets the allowed number
  ranges for digital ouput channels.
  As the constructor is often not called for static instances, the init function
  is used by the Singleton base class, to set the unique instance in a well defined
  initial state

  possible errors:
      * Err_c::range given limits are not possible
  @param ab_digitalFirst smallest allowed digital output channel number (DIGITAL_OUTPUT_MIN)
  @param ab_digitalLast greatest allowed digital output channel number (DIGITAL_OUTPUT_MAX)
  @see masterHeader
*/
void ActorO_c::init(uint8_t ab_digitalFirst, uint8_t ab_digitalLast)
{ // store the channel limits for dig and analog
  // verify that System is int
  getSystemInstance().init();
  setDigitalLimits(ab_digitalFirst, ab_digitalLast);
}

/** destructor for the actor output manager object */
ActorO_c::~ActorO_c(){
}

/**
  set the limits for digital output channels (first setting can be done by constructor parameters)

  possible errors:
      * Err_c::range given limits are not possible
  @param ab_digitalFirst number of the smallest allowed digital output channel
  @param ab_digitalLast number of the greatest allowed digital output channel
*/
void ActorO_c::setDigitalLimits(uint8_t ab_digitalFirst, uint8_t ab_digitalLast){
  // check if output values are correct
  if (
       (ab_digitalFirst <= DIGITAL_OUTPUT_MAX)
#ifdef DIGITAL_OUTPUT_MIN_GREATER_ZERO
     &&(ab_digitalFirst >= DIGITAL_OUTPUT_MIN)
     &&(ab_digitalLast >= DIGITAL_OUTPUT_MIN)
#endif
     &&(ab_digitalLast <= DIGITAL_OUTPUT_MAX)
     )
  { // correct range
    setMinDigitalLimit(ab_digitalFirst);
    setMaxDigitalLimit(ab_digitalLast);
  }
  else
  { // wrong range
    getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::Actor );
  }
}
/** handler function for access to undefined client.
  * the base Singleton calls this function, if it detects an error
  */
void ActorO_c::registerAccessFlt( void )
{
  getILibErrInstance().registerError( iLibErr_c::ElNonexistent, iLibErr_c::Actor );
  // throw exception if defined to do
  THROW_CONT_EL_NONEXIST
}

} // end of namespace __IsoAgLib
