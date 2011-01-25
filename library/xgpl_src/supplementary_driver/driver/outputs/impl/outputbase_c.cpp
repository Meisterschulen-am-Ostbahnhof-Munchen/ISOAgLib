/*
  outputbase_c.cpp:
    implementation file for OutputBase_c, a base class for DigitalO_c

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include "outputbase_c.h"


// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

OutputBase_c::OutputBase_c(uint8_t aui8_channelNr)
{
  ui8_channelNr = aui8_channelNr;
}


OutputBase_c::~OutputBase_c()
{
}


uint8_t
OutputBase_c::channelNr() const
{
  return ui8_channelNr;
}


void
OutputBase_c::set(uint16_t)
{  // normally this function should be abstract,
   // but the Tasking Linker needs this
}


} // end of namespace __IsoAgLib
