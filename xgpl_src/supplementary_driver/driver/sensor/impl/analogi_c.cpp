/***************************************************************************
                          analogi_c.cpp -  implementation file for AnalogI_c, an
                                         object for analog sensor input
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
 * Copyright (C) 1999 - 2004  Dipl.-Inform. Achim Spangler                 *
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
#include "analogi_c.h"
#include "sensori_c.h"
#include <IsoAgLib/util/impl/util_funcs.h>
#include <IsoAgLib/util/config.h>
// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

/**
  internal called constructor which creates a new input channel,initialize the hardware and configures conversion calculation
  (uses BIOS function)

  possible errors:
      * LibErr_c::Range wrong input number
      * LibErr_c::Precondition wrong input type
  @see SensorI_c::createAnalog
  @see Sensor_c::t_analogType
  @param rb_channel default-argument for the hardware channel of the input
  @param ren_analogType default-argument for choosing Sensor_c::voltage(default) or Sensor_c::current as input type
  @param rb_useMean default-argument for setting the calculation of mean value on true (false as default)
  @param rb_fastAdc default-argument for setting fast ADC method (false as default)
*/
AnalogI_c::AnalogI_c(uint8_t rb_channel, Sensor_c::analogType_t ren_analogType, bool rb_useMean, bool rb_fastAdc )
  : SensorBase_c(rb_channel, Sensor_c::analog) {
  if ( rb_channel != 0xFF ) init( rb_channel, ren_analogType, rb_useMean, rb_fastAdc );
}
/**
  internal called constructor which creates a new input channel,initialize the hardware and configures conversion calculation
  (uses BIOS function)

  possible errors:
      * LibErr_c::Range wrong input number
      * LibErr_c::Precondition wrong input type
  @see SensorI_c::createAnalog
  @see Sensor_c::t_analogType
  @param rb_channel default-argument for the hardware channel of the input
  @param ren_analogType default-argument for choosing Sensor_c::voltage(default) or Sensor_c::current as input type
  @param rb_useMean default-argument for setting the calculation of mean value on true (false as default)
  @param rb_fastAdc default-argument for setting fast ADC method (false as default)
*/
void AnalogI_c::init(uint8_t rb_channel, Sensor_c::analogType_t ren_analogType, bool rb_useMean, bool rb_fastAdc)
{
  SensorBase_c::init(rb_channel, Sensor_c::analog);
  // store given values
  en_analogType = ren_analogType;
  b_useMean = rb_useMean;
  b_fastAdc = rb_fastAdc;

  // now configure the input with BIOS call
  int16_t iResult = ( ren_analogType == Sensor_c::voltage )
                 ?HAL::init_analoginVolt(channelNr()):HAL::init_analoginCurrent(channelNr());
  switch (iResult)
  {
    case HAL_NO_ERR:
      // everything o.k. -> set fast analogin if wanted
      if (rb_fastAdc) HAL::setFastAnalogin(true);
      // now register the valid new analog input into SensorI_c
      getSensorInstance().registerClient( this );
      break;
    case HAL_RANGE_ERR:
      // wrong input channel number
      getLbsErrInstance().registerError( LibErr_c::Range, LibErr_c::Sensor );
      break;
    case HAL_CONFIG_ERR:
      // wrong input type
      getLbsErrInstance().registerError( LibErr_c::Precondition, LibErr_c::Sensor );
      break;
  }
}
/** destructor which can close the hardware input channel */
AnalogI_c::~AnalogI_c(){
  // unregister element from SensorI_c
  getSensorInstance().unregisterClient( this );
}
/**
  get the actual sensor value with the configured linear conversion (use the configured ADC method)
  (uses BIOS function)

  @return sensor value: A) Volt [mV], or B) Ampere [mA]
*/
int16_t AnalogI_c::val()const{
  int16_t i16_sensor;
  if (b_useMean)
  { // deliver the mean value
    if ( en_analogType == Sensor_c::voltage ) i16_sensor =  HAL::getAdcMeanVoltage(channelNr());
    else i16_sensor =  HAL::getAdcMeanCurrent(channelNr());
  }
  else
  { // use normal value
    if ( en_analogType == Sensor_c::voltage ) i16_sensor =  HAL::getAdcVoltage(channelNr());
    else i16_sensor =  HAL::getAdcCurrent(channelNr());
  }
  // check if error occured
  if (i16_sensor < 0)
  {
    if (i16_sensor == HAL_RANGE_ERR)
    { // error during measure -> wrong input channel no
      getLbsErrInstance().registerError( LibErr_c::Range, LibErr_c::Sensor );
      // return error code for 16bit sensor values
      return ERROR_VAL_16S;
    }
  }
  return i16_sensor;
}
/**
  configure fast ADC gathering
  (uses BIOS function)
  @param rb_useFast default-argument for setting fast ADC (true as default)
*/
void AnalogI_c::setFastAdc(bool rb_useFast){
  b_fastAdc = rb_useFast;
  // configure the BIOS
  HAL::setFastAnalogin(rb_useFast);
}
/**
  check if value is greater than 0
  (uses BIOS function)

  possible errors:
      * LibErr_c::Range wrong input number
  @return true if sensor value is different from 0, otherwise 0
*/
bool AnalogI_c::active() const {
  int16_t i16_val = val();
  if (i16_val != ERROR_VAL_16S)
  { // no range error
    return (i16_val > 0)?true:false;
  }
  return false;
}

} // end of namespace __IsoAgLib
