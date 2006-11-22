/***************************************************************************
                          sensori_c.cpp  -  implmentation for the sensor input management object
                             -------------------
    begin                : Mon Oct 25 1999
    copyright            : (C) 1999 - 2004 by Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
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

 /**************************************************************************
 *                                                                         *
 *     ###    !!!    ---    ===    IMPORTANT    ===    ---    !!!    ###   *
 * Each software module, which accesses directly elements of this file,    *
 * is considered to be an extension of IsoAgLib and is thus covered by the *
 * GPL license. Applications must use only the interface definition out-   *
 * side :impl: subdirectories. Never access direct elements of __IsoAgLib  *
 * and __HAL namespaces from applications which shouldnt be affected by    *
 * the license. Only access their interface counterparts in the IsoAgLib   *
 * and HAL namespaces. Contact a.spangler@osb-ag:de in case your applicat- *
 * ion really needs access to a part of an internal namespace, so that the *
 * interface might be extended if your request is accepted.                *
 *                                                                         *
 * Definition of direct access:                                            *
 * - Instantiation of a variable with a datatype from internal namespace   *
 * - Call of a (member-) function                                          *
 * Allowed is:                                                             *
 * - Instatiation of a variable with a datatype from interface namespace,  *
 *   even if this is derived from a base class inside an internal namespace*
 * - Call of member functions which are defined in the interface class     *
 *   definition ( header )                                                 *
 *                                                                         *
 * Pairing of internal and interface classes:                              *
 * - Internal implementation in an :impl: subdirectory                     *
 * - Interface in the parent directory of the corresponding internal class *
 * - Interface class name IsoAgLib::iFoo_c maps to the internal class      *
 *   __IsoAgLib::Foo_c                                                     *
 *                                                                         *
 * AS A RULE: Use only classes with names beginning with small letter :i:  *
 ***************************************************************************/
#include "sensori_c.h"
#include <IsoAgLib/util/config.h>
#include <IsoAgLib/driver/system/impl/system_c.h>
// Begin Namespace __IsoAgLib
namespace __IsoAgLib {
/** C-style function, to get access to the unique SensorI_c singleton instance */
SensorI_c& getSensorInstance( void ) { return SensorI_c::instance();};

/**
  initialisation for the sensor input management which sets the allowed number
  ranges for analog,digital,counter input channels.
  As the constructor is often not called for static instances, the init function
  is used by the Singleton base class, to set the unique instance in a well defined
  initial state

  possible errors:
      * iLibErr_c::Range given limits are not possible
  @param rb_digitalFirst smallest allowed digital input channel number (DIGITAL_INPUT_MIN)
  @param rb_digitalLast greatest allowed digital input channel number (DIGITAL_INPUT_MAX)
  @param rb_analogFirst smallest allowed analog input channel number (ANALOG_INPUT_MIN)
  @param rb_analogLast greatest allowed analog input channel number (ANALOG_INPUT_MAX)
  @param rb_counterFirst smallest allowed counter input channel number (COUNTER_INPUT_MIN)
  @param rb_counterLast greatest allowed counter input channel number (COUNTER_INPUT_MAX)
  @see isoaglib_config.h
*/
void SensorI_c::init(uint8_t rb_digitalFirst, uint8_t rb_digitalLast, uint8_t rb_analogFirst, uint8_t rb_analogLast,
           uint8_t rb_counterFirst, uint8_t rb_counterLast)
{ // store the channel limits for dig and analog
  // verify that System is int
  getSystemInstance().init();
  setDigitalLimits(rb_digitalFirst, rb_digitalLast);
  setAnalogLimits(rb_analogFirst, rb_analogLast);
  setCounterLimits(rb_counterFirst, rb_counterLast);
}

/** destructor for the sensor input manager object */
SensorI_c::~SensorI_c(){
}

/**
  set the limits for digital input channels (first setting can be done by constructor parameters)
  possible errors:
      * iLibErr_c::Range given limits are not possible
  @param rb_digitalFirst number of the smallest allowed digital input channel
  @param rb_digitalLast number of the greatest allowed digital input channel
*/
void SensorI_c::setDigitalLimits(uint8_t rb_digitalFirst, uint8_t rb_digitalLast){
  // check if input values are correct
  if (
       (rb_digitalFirst <= DIGITAL_INPUT_MAX)
#ifdef DIGITAL_INPUT_MIN_GREATER_ZERO
     &&(rb_digitalFirst >= DIGITAL_INPUT_MIN)
     &&(rb_digitalLast >= DIGITAL_INPUT_MIN)
#endif
     &&(rb_digitalLast <= DIGITAL_INPUT_MAX)
     )
  { // correct range
    setDigitalFirst(rb_digitalFirst);
    setDigitalLast(rb_digitalLast);
  }
  else
  { // wrong range
    getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::Sensor );
  }
}

/**
  set the limits for analog input channels (first setting can be done by constructor parameters)

  possible errors:
      * iLibErr_c::Range given limits are not possible
  @param rb_analogFirst number of the smallest allowed analog input channel
  @param rb_analogLast number of the greatest allowed analog input channel
*/
void SensorI_c::setAnalogLimits(uint8_t rb_analogFirst, uint8_t rb_analogLast){
  // check if input values are correct
  if (
       (rb_analogFirst <= ANALOG_INPUT_MAX)
#ifdef ANALOG_INPUT_MIN_GREATER_ZERO
     &&(rb_analogFirst >= ANALOG_INPUT_MIN)
     &&(rb_analogLast >= ANALOG_INPUT_MIN)
#endif
     &&(rb_analogLast <= ANALOG_INPUT_MAX)
     )
  { // correct range
    setAnalogFirst(rb_analogFirst);
    setAnalogLast(rb_analogLast);
  }
  else
  { // wrong range
    getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::Sensor );
  }
}

/**
  set the limits for counter input channels (first setting can be done by constructor parameters)

  possible errors:
      * iLibErr_c::Range given limits are not possible
  @param rb_counterFirst number of the smallest allowed counter input channel
  @param rb_counterLast number of the greatest allowed counter input channel
*/
void SensorI_c::setCounterLimits(uint8_t rb_counterFirst, uint8_t rb_counterLast){
  // check if input values are correct
  if (
       (rb_counterFirst <= COUNTER_INPUT_MAX)
#ifdef COUNTER_INPUT_MIN_GREATER_ZERO
     &&(rb_counterFirst >= COUNTER_INPUT_MIN)
     &&(rb_counterLast >= COUNTER_INPUT_MIN)
#endif
     &&(rb_counterLast <= COUNTER_INPUT_MAX)
     )
  { // correct range
    setCounterFirst(rb_counterFirst);
    setCounterLast(rb_counterLast);
  }
  else
  { // wrong range
    getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::Sensor );
  }
}
/** handler function for access to undefined client.
  * the base Singleton calls this function, if it detects an error
  */
void SensorI_c::registerAccessFlt( void )
{
  getILibErrInstance().registerError( iLibErr_c::ElNonexistent, iLibErr_c::Sensor );
  // throw exception if defined to do
  THROW_CONT_EL_NONEXIST
}



} // end of namespace __IsoAgLib
