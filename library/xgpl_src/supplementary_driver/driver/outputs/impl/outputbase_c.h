/*
  outputbase_c.h:
    header file for OutputBase_c, a base class for DigitalO_c

  (C) Copyright 2009 - 2011 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef OUTPUTBASE_C_H
#define OUTPUTBASE_C_H

#include <supplementary_driver/hal/hal_outputs.h>


// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

/**
  Base Class for Outputs_c output channels;
  holds information for channel number
  delivers some basically output request methods
  @see DigitalO_c
  @see Outputs_c
  @author Dipl.-Inform. Achim Spangler
  */
class OutputBase_c {
public:
  /**
    Basic constructor for an output channel object (only internal accessed)
  */
  OutputBase_c (uint8_t aui8_channelNr);

  /**
    Init to a (new) channel
  */
  void init(uint8_t aui8_channelNr) { ui8_channelNr = aui8_channelNr;}

  /** basic destructor of output object (only internal accessed) */
  virtual ~OutputBase_c();

  /**
    set the output PWM value
    @param aui16_val value to set in ouput channel
  */
  virtual void set(uint16_t aui16_val);

  /**
    deliver the channel number of the output object
    @return number to use for BIOS access to this channel
  */
  uint8_t channelNr() const;

private:
  /** channel number of this output */
  uint8_t ui8_channelNr;
};

} // end of namespace __IsoAgLib

#endif

