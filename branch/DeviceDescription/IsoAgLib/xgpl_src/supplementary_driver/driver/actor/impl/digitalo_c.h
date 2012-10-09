/***************************************************************************
                          digitalo_c.h  - header file for Digital_O_c, an object
                                         for digital actor output
                             -------------------
    begin                : Mon Oct 25 1999
    copyright            : (C) 1999 - 2004 by Dipl.-Inform. Achim Spangler
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
#ifndef DIGITAL_O_H
#define DIGITAL_O_H

#include "actorbase_c.h"

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {
/**
  Output object for simple digital output;
  @see Actor_O
  @see ActorBase
  @author Dipl.-Inform. Achim Spangler
*/
class DigitalO_c : public ActorBase_c  {
public:
  /** enum for error states of digital output */
  typedef enum { noDoutErr, dout_openErr, dout_shortcutErr } dout_err_t;
  /**
    internal called constructor for a new digital input channel which performs configuration of hardware
    (uses BIOS function)

    possible errors:
        * Err_c::range wrong output channel number
    @see ActorO_c::createDigital
    @param rui8_channel default-argument for setting hardware channel for this input
  */
  DigitalO_c(uint8_t rui8_channel = 0);
  /** init this instance
    @param rui8_channel default-argument for setting hardware channel for this input
  */
  void init(uint8_t rui8_channel);
  /**  destructor of the input object which can close explicit the hardware input */
  virtual ~DigitalO_c();
  /** compare this channel to an uint8_t value representing a dout channel
    * this function is used by the base Singleton to find a specific item
    */
  bool operator==( uint8_t rui8_channel ) const { return ( rui8_channel == channelNr() )?true:false;};
  /**
    set the output PWM frequency
    (uses BIOS function)

    possible errors:
       * Err_c::range wrong output channel number or wrong frequency
    @param rui32_val value to use as PWM frequency in [mHz]
  */
  void setFreq(uint32_t rui32_val);
  /**
    set the output PWM value
    (uses BIOS function)

    possible errors:
        * Err_c::range wrong output channel number
    @param rui16_val value to set for the output channel [0..0xffff]
  */
  void set(uint16_t rui16_val);
  /**
    switch PWM to total OFF or ON (simple switch function)
    (uses BIOS function)

    possible errors:
       * Err_c::range wrong output channel number
    @param rb_state
  */
  void set(bool rb_state);
  /**
    deliver actual set value
    @return last set value [0..0xffff]
  */
  uint16_t get()const{return ui16_value;};
  /**
    * set the allowed current limits for active state
    * @param rui16_minActiveCurrent minimal allowed current in active state
    * @param rui16_maxActiveCurrent maximum allowed current in active state
    */
  void setActiveCurrentLimits( uint16_t rui16_minActiveCurrent, uint16_t rui16_maxActiveCurrent )
  {ui16_minAllowedCurrent = rui16_minActiveCurrent; ui16_maxAllowedCurrent = rui16_maxActiveCurrent;};
  /** deliver the actual current of the digital output
    * @return current in [mA] ( if this digital out instance doesn't support current measurement, -1 is returned )
    */
  inline int16_t getDigoutCurrent( void ) const { return HAL::getDigoutCurrent( channelNr() );};
  /** check if this digital output channel is in FAIL condition
    * @return true -> channel is in fail condition
    */
  bool operator!( void ) const { return (good())?false:true;};
  /** check if this digital output channel is in GOOD condition
    * @return true -> channel is in GOOD condition
    */
  bool good( void ) const;
  /** deliver detailed error state information for this Digital Output
		* This function evaluates the current where possible, otherwise it evaluates
		* the measured voltage at the output. The latter interpretation can go wrong
		* if the PWM setting is >0 but has a very low value, so that even under normal
		* conditions the voltage with connected consuming device is lower than to open
		* connector state at low level.
    * @return dout_err_t [noDoutErr|dout_openErr|dout_shortcutErr]
    */
  dout_err_t getState( void ) const;
	/** deliver the measure voltage at the PWM output.
		Use this for application specific state evaluation for cases, where the standard
		getDigoutDiagnose function can go wrong.
		@return voltage at PWM output [mV]
	*/
  int16_t getDigoutAdc( void ) const { return HAL::getDigoutAdc( channelNr() );};
	/** deliver the max allowed PWM -> setting this value results in max output
		@return max allowed PWM for the current PWM frequency setting
	*/
  uint16_t getMaxOutputPwmFreq() const
	{ return ui16_maxOutputPwmFreq; }
	
private:
  /**
    HIDDEN! copy constructor for Digital_O
    NEVER copy a DigitalO_c around!!!!
    ONLY copy pointers to the wanted instance!!!
    ==> the copy constructor is defined as private, so that compiler
        detects this fault, and shows you this WARNING!!
    @param rrefc_src source
  */
  DigitalO_c(const DigitalO_c& rrefc_src) : ActorBase_c(rrefc_src) {};
  /**
    HIDDEN! assignment for Digital_O
    NEVER assign a DigitalO_c to another instance!!!!
    ==> the asignment is defined as private, so that compiler
        detects this fault, and shows you this WARNING!!
    @param rrefc_src source
  */
  DigitalO_c& operator=(const DigitalO_c& rrefc_src){return *this;};

  uint16_t ui16_value;
  /** minimal allowed current in active state */
  uint16_t ui16_minAllowedCurrent;
  /** maximum allowed current in active state */
  uint16_t ui16_maxAllowedCurrent;
  /** max allowed PWM freq -> setting this value results in max output */
  uint16_t ui16_maxOutputPwmFreq;
};

}
#endif
