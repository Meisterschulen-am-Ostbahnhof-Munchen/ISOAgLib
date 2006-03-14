/***************************************************************************
                          analogirangecheck_c.h  -  header file for Analog_I_c, an object
                                         for analog sensor input
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
#ifndef ANALOGIRANGECHECK_H
#define ANALOGIRANGECHECK_H

#include "analogi_c.h"

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {
/**
  Extend AnalogI_c by range check for input signal
  -> store [minValid..maxValid] and answer state dependent
  @see AnalogI_c
  @see SensorI_c
  @see DigitalI_c
  @see SensorBase_c
  @short AnalogIRangeCheck_c serves an easy interface to sensor signals with linear transformation.
  @author Dipl.-Inform. Achim Spangler
  */
class AnalogIRangeCheck_c : public AnalogI_c  {
public:
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
    @param rui16_minValid default min allowed value (min limit of range)
    @param rui16_maxValid default max allowed value (max limit of range)
  */
  AnalogIRangeCheck_c(uint8_t rb_channel = 0xFF, Sensor_c::analogType_t ren_analogType = Sensor_c::voltage, bool rb_useMean = false,
             bool rb_fastAdc = false, uint16_t rui16_minValid = 0, uint16_t rui16_maxValid = 0xFFFF );
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
    @param rui16_minValid default min allowed value (min limit of range)
    @param rui16_maxValid default max allowed value (max limit of range)
  */
  void init(uint8_t rb_channel, Sensor_c::analogType_t ren_analogType = Sensor_c::voltage, bool rb_useMean = false,
             bool rb_fastAdc = false, uint16_t rui16_minValid = 0, uint16_t rui16_maxValid = 0xFFFF);

  /** get validate val
    @param refb_tooLow  reference to bool value which is set dependent on ( value < minLimit )
    @param refb_tooHigh reference to bool value which is set dependent on ( value > maxLimit )
    @return sensor value
  */
  int16_t validatedVal( bool &refb_tooLow, bool &refb_tooHigh ) const;

  /** check if sensor value is valid */
  virtual bool good( void ) const;
  /** check if sensor value is in error state */
  virtual bool error( void ) const;
  /** check if sensor value is out of range */
  bool checkRangeError( void ) const;
  /** check if value is too high */
  bool checkTooHigh( void ) const;
  /** check if value is too low */
  bool checkTooLow( void ) const;
  /* set range
    @param rui16_minValid default min allowed value (min limit of range)
    @param rui16_maxValid default max allowed value (max limit of range)
  */
  void setRange( uint16_t rui16_minValid, uint16_t rui16_maxValid );

  /** deliver total valid range in mV */
  uint16_t getRange( void ) const { return ( ui16_maxValid - ui16_minValid ); } ;
  /** deliver min Limit */
  uint16_t getMinValid( void ) const { return ui16_minValid; };
  /** deliver max Limit */
  uint16_t getMaxValid( void ) const { return ui16_maxValid; };
  /** destructor which can close the hardware input channel */
  virtual ~AnalogIRangeCheck_c();
protected:
  /**
    HIDDEN! copy constructor for AnalogIRangeCheck_c
    NEVER copy a AnalogIRangeCheck_c around!!!!
    ONLY copy pointers to the wanted instance!!!
    ==> the copy constructor is defined as private, so that compiler
        detects this fault, and shows you this WARNING!!
    @param rrefc_src source
  */
  AnalogIRangeCheck_c(const AnalogIRangeCheck_c& rrefc_src) : AnalogI_c(rrefc_src) {};
  /**
    HIDDEN! assignment for AnalogIRangeCheck_c
    NEVER assign a AnalogIRangeCheck_c to another instance!!!!
    ==> the asignment is defined as private, so that compiler
        detects this fault, and shows you this WARNING!!
    @param rrefc_src source
  */
  AnalogIRangeCheck_c& operator=(const AnalogIRangeCheck_c& /* rrefc_src */){ return *this;};
private:
  /** min range limit */
  uint16_t ui16_minValid;
  /** max range limit */
  uint16_t ui16_maxValid;
};
}

#endif
