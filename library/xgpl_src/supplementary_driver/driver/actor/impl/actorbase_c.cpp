/*
  actorbase_c.cpp:
    implementation file for ActorBase_c, a base class for Digital_O

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include "actorbase_c.h"


// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

/**
  Basic constructor for an input channel object (only internal accessed)
  @param aui8_channelNr hardware channel of this input object
*/
ActorBase_c::ActorBase_c(uint8_t aui8_channelNr){
  ui8_channelNr = aui8_channelNr;
}

/** basic destructor of sensor input object with no function at the moment (only internal accessed) */
ActorBase_c::~ActorBase_c(){
}
/**
  deliver the channel number of the output object
  @return number to use for BIOS access to this channel
*/
uint8_t ActorBase_c::channelNr() const{
  return ui8_channelNr;
}

/**
  set the output PWM value
  @param aui16_val value to set in ouput channel
*/
void ActorBase_c::set(uint16_t /* aui16_val is intentionally unused in this baseclass */ )
{  // normally this function should be abstract,
   // but the Tasking Linker needs this
}


} // end of namespace __IsoAgLib
