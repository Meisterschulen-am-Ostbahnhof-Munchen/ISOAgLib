/*
  outputbase_c.h - header file for OutputBase_c

  (C) Copyright 2009 - 2019 by OSB AG

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Usage under Commercial License:
  Licensees with a valid commercial license may use this file
  according to their commercial license agreement. (To obtain a
  commercial license contact OSB AG via <http://isoaglib.com/en/contact>)

  Usage under GNU General Public License with exceptions for ISOAgLib:
  Alternatively (if not holding a valid commercial license)
  use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef OUTPUTBASE_C_H
#define OUTPUTBASE_C_H

#include <IsoAgLib/isoaglib_config.h>


namespace __IsoAgLib {

/**
  Base Class for Outputs_c output channels;
  holds information for channel number
  @see DigitalO_c
  @see Outputs_c
  */
class OutputBase_c {
public:
  OutputBase_c (uint8_t a_channelNr)
    : ui8_channelNr( a_channelNr )
  {}

  virtual ~OutputBase_c()
  {}

  void setChannel(uint8_t aui8_channelNr) { ui8_channelNr = aui8_channelNr; }
  uint8_t channelNr() const { return ui8_channelNr; }

private:
  uint8_t ui8_channelNr;
};

} // __IsoAgLib

#endif

