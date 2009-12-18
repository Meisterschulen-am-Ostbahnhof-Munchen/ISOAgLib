/*
  actorbase_c.h:
    header file for Actor_Base_c, a base class for Digital_O

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef ACTOR_BASE_H
#define ACTOR_BASE_H

#include "actor_c.h"
#include <IsoAgLib/util/iliberr_c.h>
#include <supplementary_driver/hal/hal_actor.h>

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

/**
  Base Class for Actor_c output channels;
  holds information for channel number
  delivers some basically output request methods
  @see Digital_O
  @see Actor_O
  @see Actor
  @author Dipl.-Inform. Achim Spangler
  */
class ActorBase_c {
public:
  /**
    Basic constructor for an input channel object (only internal accessed)
    @param aui8_channelNr hardware channel of this input object
  */
  ActorBase_c(uint8_t aui8_channelNr);
  /**
    Init the ActorBase object to a (new) channel
    @param aui8_channelNr hardware channel of this input object
  */
  void init(uint8_t aui8_channelNr) { ui8_channelNr = aui8_channelNr;};
  /** basic destructor of sensor input object with no function at the moment (only internal accessed) */
  virtual ~ActorBase_c();
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
// Private attributes
  /** channel number of this output */
  uint8_t ui8_channelNr;
};
} // end of namespace __IsoAgLib

#endif

