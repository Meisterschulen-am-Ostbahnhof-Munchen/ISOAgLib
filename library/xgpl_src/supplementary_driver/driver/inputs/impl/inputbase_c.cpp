/*
  inputbase_c.cpp:
    implementation file for InputBase_c, a base class for AnalogI_c
    and Digital_I

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include "inputbase_c.h"
#include "inputs_c.h"


// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

InputBase_c::InputBase_c(uint8_t aui8_channelNr, inputType_t ren_inputType)
{
  // store the channel number (software side
  ui8_channelNr = aui8_channelNr;
  en_inputType = ren_inputType;
}


void
InputBase_c::init(uint8_t aui8_channelNr, inputType_t ren_inputType )
{ // store the channel number (software side
  ui8_channelNr = aui8_channelNr;
  en_inputType = ren_inputType;
}

InputBase_c::~InputBase_c()
{
}


int32_t
InputBase_c::valLong()
{ // as far as not overloaded otherwise answer simply val()
  return val();
}


} // end of namespace __IsoAgLib
