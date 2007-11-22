/***************************************************************************
                          sensorbase_c.cpp  - implementation file for
                                            SensorBase_c, a base class for
                                            AnalogI_c and Digital_I
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
#include "sensorbase_c.h"
#include "sensori_c.h"
// Begin Namespace __IsoAgLib
namespace __IsoAgLib {


/**
  Basic constructor for a input channel object (only internal accessed)
  @param aui8_channelNr hardware channel of this input object
  @param ren_inputType input type, which is needed because of the relation
         of config number and gathering number which vary dependent on input type
  @see Sensor_c::input_type
*/
SensorBase_c::SensorBase_c(uint8_t aui8_channelNr, Sensor_c::inputType_t ren_inputType){
  // store the channel number (software side
  ui8_channelNr = aui8_channelNr;
  en_inputType = ren_inputType;
}
/**
  Basic constructor for an input channel object (only internal accessed)
  @param aui8_channelNr hardware channel of this input object
  @param ren_inputType input type, which is needed because of the relation
         of config number and gathering number which vary dependent on input type
  @see Sensor_c::input_type
*/
void SensorBase_c::init(uint8_t aui8_channelNr, Sensor_c::inputType_t ren_inputType )
{ // store the channel number (software side
  ui8_channelNr = aui8_channelNr;
  en_inputType = ren_inputType;
}
/** basic destructor of sensor input object with no function at the moment (only internal accessed) */
SensorBase_c::~SensorBase_c(){
}
/**
  deliver the value from the sensor (for digital input: active->1; else->0)
  @return value of the input channel (can be raw sensor value or calculated as configured on creation)
  @see AnalogI_c::val
  @see DigitalI_c::val
*/
int32_t SensorBase_c::valLong()
{ // as far as not overloaded otherwise answer simply val()
  return val();
}


} // end of namespace __IsoAgLib
