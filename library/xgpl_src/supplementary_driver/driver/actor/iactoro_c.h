/***************************************************************************
                          iactor_o.h  - header for the actor output
                                        management object
                             -------------------
    begin                : Fri Apr 07 2000
    copyright            : (C) 2000 - 2009 by Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
    type                 : Header
    $LastChangedDate$
    $LastChangedRevision$
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
 * Copyright (C) 2000 - 2009 Dipl.-Inform. Achim Spangler                  *
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
#ifndef IACTOR_O_H
#define IACTOR_O_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include "impl/actor_o.h"
#include "idigital_o.h"
#include "iactor.h"


// Begin Namespace IsoAgLib
namespace IsoAgLib {
/**
  Hardware dependent object for hardware independent controling of actor outputs.
  @see iDigital_O
  @author Dipl.-Inform. Achim Spangler
  */
class iActorO_c : private __IsoAgLib::ActorO_c {
public:
  /**
    initialisation for the actor output management which sets the allowed number
    ranges for digital ouptut channels.
    As the constructor is often not called for static instances, the init function
    is used by the Singleton base class, to set the unique instance in a well defined
    initial state

    possible errors:
        * Err_c::range given limits are not possible
    @param ab_digitalFirst smallest allowed digital output channel number (DIGITAL_OUTPUT_MIN)
    @param ab_digitalLast greatest allowed digital output channel number (DIGITAL_OUTPUT_MAX)
    @see masterHeader
  */
  void init(uint8_t ab_digitalFirst = DIGITAL_OUTPUT_MIN, uint8_t ab_digitalLast = DIGITAL_OUTPUT_MAX)
  { init(ab_digitalFirst, ab_digitalLast);};
  /** destructor for the actor output manager object */
  ~iActorO_c() {};
  /**
    set the limits for digital output channels (first setting can be done by constructor parameters)

    possible errors:
        * Err_c::range given limits are not possible
    @param ab_digitalFirst number of the smallest allowed digital input channel
    @param ab_digitalLast number of the greatest allowed digital input channel
  */
  void setDigitalLimits(uint8_t ab_digitalFirst, uint8_t ab_digitalLast)
    {ActorO_c::setDigitalLimits(ab_digitalFirst, ab_digitalLast);};

  /**
    check if digital output object to given ab_channel exist
    @see iActorO_c::createDigital
    @see iActorO_c::deleteDigital
    @param ab_channel number of the tested input channel
    @return true -> searched input object exist
  */
  bool existDigital(uint8_t ab_channel)
    {return ActorO_c::existDigital(ab_channel);};

  /**
    deliver reference to requested digital channel object to access this actor output;
    IMPORTANT: an digital output channel object with the wanted number must exist
               -> creating with createDigital and checking with existDigital
               (throw exception if exceptions are activated on compile time)

    possible errors:
        * Err_c::elNonexistant wanted digital input with given channel no does not exist
    @see iActorO_c::createDigital
    @see iActorO_c::existDigital
    @see iDigitalO_c::iDigital_O
    @param ab_channel channel of the digital sensor input
    @return reference to the wanted digital actor output channel
    @exception containerElementNonexistant
  */
  iDigitalO_c& digital(uint8_t ab_channel)
    {return static_cast<iDigitalO_c&>(ActorO_c::digital(ab_channel));};
private:
  /** allow getIactorInstance() access to shielded base class.
      otherwise __IsoAgLib::getActorInstance() wouldn't be accepted by compiler
    */
  friend iActorO_c& getIactorInstance( void );

  /** private constructor which prevents direct instantiation in user application
    * NEVER define instance of ActorO_c within application
    */
  iActorO_c( void ) : ActorO_c() {};
};
/** C-style function, to get access to the unique iActorO_c singleton instance */
inline iActorO_c& getIactorInstance( void ) { return static_cast<iActorO_c&>(__IsoAgLib::getActorInstance());};

}
#endif

