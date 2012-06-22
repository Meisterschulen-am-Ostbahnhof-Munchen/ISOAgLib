/*
  analogi_c.cpp:
    implementation file for AnalogI_c, an object for analog input

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include "analogi_c.h"
#include "inputs_c.h"
#include <IsoAgLib/util/impl/util_funcs.h>
#include <IsoAgLib/util/config.h>
#include <IsoAgLib/util/iliberr_c.h>


// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

AnalogI_c::AnalogI_c(uint8_t ab_channel, analogType_t ren_analogType, bool ab_useMean, bool ab_fastAdc )
  : InputBase_c(ab_channel, analog) {
  if ( ab_channel != 0xFF ) init( ab_channel, ren_analogType, ab_useMean, ab_fastAdc );
}


void
AnalogI_c::init(uint8_t ab_channel, analogType_t ren_analogType, bool ab_useMean, bool ab_fastAdc)
{
  InputBase_c::init(ab_channel, analog);
  // store given values
  en_analogType = ren_analogType;
  b_useMean = ab_useMean;
  b_fastAdc = ab_fastAdc;

  // now configure the input with BIOS call
  int16_t iResult = ( ren_analogType == voltage )
                 ?HAL::init_analoginVolt(channelNr()):HAL::init_analoginCurrent(channelNr());
  switch (iResult)
  {
    case HAL_NO_ERR:
      // everything o.k. -> set fast analogin if wanted
      if (ab_fastAdc) HAL::setFastAnalogin(true);
      // now register the valid new analog input into Inputs_c
      getInputsInstance().registerClient( this );
      break;
    case HAL_RANGE_ERR:
    case HAL_CONFIG_ERR:
      isoaglib_assert( !"init error" );
      break;
  }
}


AnalogI_c::~AnalogI_c()
{
  // unregister element from Inputs_c
  getInputsInstance().unregisterClient( this );
}


uint16_t
AnalogI_c::val()const
{
  int16_t i16_sensor;
  if (b_useMean)
  { // deliver the mean value
    if ( en_analogType == voltage ) i16_sensor =  HAL::getAdcMeanVoltage(channelNr());
    else i16_sensor =  HAL::getAdcMeanCurrent(channelNr());
  }
  else
  { // use normal value
    if ( en_analogType == voltage ) i16_sensor =  HAL::getAdcVoltage(channelNr());
    else i16_sensor =  HAL::getAdcCurrent(channelNr());
  }
  // check if error occured
  if (i16_sensor < 0)
  {
    if (i16_sensor == HAL_RANGE_ERR)
    { // error during measure -> wrong input channel no
      // return error code for 16bit input values
      return ERROR_VAL_16S;
    }
  }
  return uint16_t( i16_sensor );
}


void
AnalogI_c::setFastAdc(bool ab_useFast)
{
  b_fastAdc = ab_useFast;
  // configure the BIOS
  HAL::setFastAnalogin(ab_useFast);
}


bool
AnalogI_c::active() const
{
  int16_t i16_val = val();
  if (i16_val != ERROR_VAL_16S)
  { // no range error
    return (i16_val > 0);
  }
  return false;
}

} // end of namespace __IsoAgLib
