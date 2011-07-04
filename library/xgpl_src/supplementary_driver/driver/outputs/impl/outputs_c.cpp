/*
  outputs_c.cpp:
    implementation for the outputs management object

  (C) Copyright 2009 - 2011 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include "outputs_c.h"

#include <IsoAgLib/util/iliberr_c.h>
#include <IsoAgLib/util/config.h>
#include <IsoAgLib/driver/system/impl/system_c.h>


// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

/** C-style function, to get access to the unique Outputs_c singleton instance */
Outputs_c &getOutputsInstance()
{
  MACRO_MULTITON_GET_INSTANCE_BODY(Outputs_c, 1, 0);
}


void
Outputs_c::init (uint8_t ab_digitalFirst, uint8_t ab_digitalLast)
{ // store the channel limits for dig and analog
  setDigitalLimits(ab_digitalFirst, ab_digitalLast);
}


Outputs_c::~Outputs_c(){
}


void
Outputs_c::setDigitalLimits(uint8_t ab_digitalFirst, uint8_t ab_digitalLast){
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
    getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::Output );
  }
}


void
Outputs_c::registerAccessFlt( void )
{
  getILibErrInstance().registerError( iLibErr_c::ElNonexistent, iLibErr_c::Output );
  // throw exception if defined to do
  THROW_CONT_EL_NONEXIST
}

} // end of namespace __IsoAgLib
