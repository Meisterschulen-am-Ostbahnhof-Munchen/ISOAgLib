/***************************************************************************
                          ieeprom_io.h - header for iEEPROM_IO_c object
                                         for communication with EEPROM
                             -------------------
    begin                : Fri Apr 07 2000
    copyright            : (C) 2000 - 2004 by Dipl.-Inform. Achim Spangler
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
 * Copyright (C) 2000 - 2004 Dipl.-Inform. Achim Spangler                  *
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
#ifndef IEEPROM_IO_H
#define IEEPROM_IO_H

#include "impl/eepromio_c.h"
#include <IsoAgLib/util/igetypos_c.h>
// #include <string>

using std::basic_string;

// Begin Namespace IsoAgLib
namespace IsoAgLib {
/**
  object for communication with the EEPROM,
  stream read/write operators for all basic types;
  avoid rewriting same values to EEPROM;
  manages operations cross segment boundaries
  *@author Dipl.-Inform. Achim Spangler
*/
class iEEPROMIO_c : private __IsoAgLib::EEPROMIO_c {
public:
  /** destructor has nothing to destruct */
  ~iEEPROMIO_c() {};

  // ++++++++++++++++++++++++++++++++++++
  // ++++ EEPROM managing operations ++++
  // ++++++++++++++++++++++++++++++++++++

  /**
    deliver the size of the whole EEPROM memory in bytes;
    uses BIOS function (which delivers KB -> mult with 1024)
    @return size of EEPROM in BYTE
  */
  static uint16_t eepromSize(){return EEPROMIO_c::eepromSize();};

  /**
    set the write position in EEPROM (in Byte); answer if possible
    if rui16_adress exceeds EEPROM memory ignore setting and set Err_c::range

    possible errors:
        * Err_c::range rui16_adress outer the limits of EEPROM memory

    @param rui16_adress position of write mark [uint8_t]
    @return true -> setting of write mark without errors
  */
  bool setp(uint16_t rui16_adress) {return EEPROMIO_c::setp(rui16_adress);};

  /**
    set the read position in EEPROM (in Byte); answer if possible
    if rui16_adress exceeds EEPROM memory ignore setting and set Err_c::range

    possible errors:
        * Err_c::range rui16_adress outer the limits of EEPROM memory

    @param rui16_adress position of read mark [uint8_t]
    @return true -> setting of read mark without errors
  */
  bool setg(uint16_t rui16_adress) {return EEPROMIO_c::setg(rui16_adress);};

  /**
    get the write position in EEPROM (in Byte)
    @return position of write mark [uint8_t]
  */
  uint16_t tellp(){return EEPROMIO_c::tellp();};

  /**
    get the read position in EEPROM (in Byte)
    @return position of read mark [uint8_t]
  */
  uint16_t tellg(){return EEPROMIO_c::tellg();};

  /**
    check if write position is at end of EEPROM
    (parameter specifies needed length for operation
    -> false means that enough space is there for length bytes of data)

    possible errors:
        * Err_c::range if rb_setState == true and actual write position is nearer to end of EEPROM than rui8_length byte

    @param rui8_length optional size of uint8_t, which must fit into EEPROM from actual position on (default 0 -> only write mark position tested)
    @return false -> write marker is more than rui8_length uint8_t ahead end of EEPROM
  */
  bool eofp(uint8_t rui8_length = 0, bool rb_setState = false)
   {return EEPROMIO_c::eofp(rui8_length, rb_setState);};

  /**
    check if read position is at end of EEPROM
    (parameter specifies needed length for operation
    -> false means that enough space is there for length bytes of data)

    possible errors:
        * Err_c::range if rb_setState == true and actual read position is nearer to end of EEPROM than rui8_length byte

    @param rui8_length optional size of uint8_t, which must fit into EEPROM from actual position on (default 0 -> only read mark position tested)
    @return false -> read marker is more than rui8_length uint8_t ahead end of EEPROM
  */
  bool eofg(uint8_t rui8_length = 0, bool rb_setState = false)
    {return EEPROMIO_c::eofg(rui8_length, rb_setState);};

  /* *************************************** */
  /* *** EEPROM data operation functions *** */
  /* ************** writing **************** */
  /* *************************************** */

  /**
    write a value to EEPROM from actual write position on (tellp() )
    by use of template mechanism the correct write implementation is generted
    for the given parameter type

    possible errors:
        * Err_c::range writing position exceeds end of EEPROM
        * Err_c::hwBusy the EEPROM was busy with another action
        * Err_c::eepromSegment low level writing caused segment error

    @see iEEPROMIO_c::tellp
    @see iEEPROMIO_c::setp
    @param rTemplateVal value of any type to write into EEPROM from actual write position on
    @return reference to this iEEPROMIO_c instance (for chains like "eeprom << val1 << val2 << ... << val_n;")
  */
  template<class T>
  iEEPROMIO_c& operator<<(const T& rTemplateVal)
    {return static_cast<iEEPROMIO_c&>(EEPROMIO_c::operator<<(rTemplateVal));}

  /**
    write a text string value to EEPROM from actual write position on (tellp() )

    possible errors:
        * Err_c::range writing position exceeds end of EEPROM
        * Err_c::hwBusy the EEPROM was busy with another action
        * Err_c::eepromSegment low level writing caused segment error

    @see iEEPROMIO_c::tellp
    @see iEEPROMIO_c::setp
    @param rrefc_val string to write into EEPROM
    @return reference to this iEEPROMIO_c instance (for chains like "eeprom << val1 << val2 << ... << val_n;")
  */
  iEEPROMIO_c& operator<<(basic_string<int8_t>& rrefc_val)
    {return static_cast<iEEPROMIO_c&>(EEPROMIO_c::operator<<(rrefc_val));};

  /**
    write a uint8_t string value to EEPROM from actual write position on (tellp() )

    possible errors:
        * Err_c::range writing position exceeds end of EEPROM
        * Err_c::busy the EEPROM was busy with another action
        * Err_c::eepromSegment low level writing caused segment error

    @see EEPROMIO_c::tellp
    @see EEPROMIO_c::setp
    @param rpb_string string to write into EEPROM
    @param rui16_number length of the string to write
    @return reference to this EEPROMIO_c instance (for chains like "eeprom << val1 << val2 << ... << val_n;")
  */
  inline iEEPROMIO_c& writeString(const uint8_t *const rpb_string, uint16_t rui16_number)
    {return static_cast<iEEPROMIO_c&>(EEPROMIO_c::writeString(rpb_string, rui16_number));};

  /* *************************************** */
  /* *** EEPROM data operation functions *** */
  /* ************** reading **************** */
  /* *************************************** */
  /**
    read a value to EEPROM from actual write position on (tellg() )
    by use of template mechanism the correct read implementation is generted
    for the given parameter type

    possible errors:
        * Err_c::range reading position exceeds end of EEPROM
        * Err_c::hwBusy the EEPROM was busy with another action
        * Err_c::eepromSegment low level reading caused segment error

    @see iEEPROMIO_c::tellg
    @see iEEPROMIO_c::setg
    @param rTemplateVal EERPOM data is read into rTemplateVal from actual read position on
    @return reference to this iEEPROMIO_c instance (for chains like "eeprom >> val1 >> val2 >> ... >> val_n;")
  */
  template<class T>
  iEEPROMIO_c& operator>>(T& rTemplateVal)
    {return static_cast<iEEPROMIO_c&>(EEPROMIO_c::operator>>(rTemplateVal));}

  /**
    read operator for strings with given length; uses BIOS function

    possible errors:
        * Err_c::range reading position exceeds end of EEPROM
        * Err_c::hwBusy the EEPROM was busy with another reading action
        * Err_c::eepromSegment low level reading caused segment error

    @see iEEPROMIO_c::tellg
    @see iEEPROMIO_c::setg
    @param rpb_string pointer to uint8_t string, which should be read from actual EEPROM read position on
    @param rui16_number number of uint8_t to read into string
    @return true -> read with success
  */
  bool readString(uint8_t *const rpb_string, uint16_t rui16_number)
    {return EEPROMIO_c::readString(rpb_string, rui16_number);};
private:
  /** allow getIeepromInstance() access to shielded base class.
      otherwise __IsoAgLib::getEepromInstance() wouldn't be accepted by compiler
    */
  friend iEEPROMIO_c& getIeepromInstance( void );
  friend iEEPROMIO_c& operator<<(iEEPROMIO_c& refc_stream, const iGetyPos_c& refc_data );
  friend iEEPROMIO_c& operator>>(iEEPROMIO_c& refc_stream, iGetyPos_c& refc_data );
};

/** C-style function, to get access to the unique EEPROMIO_c singleton instance */
inline iEEPROMIO_c& getIeepromInstance( void ) { return static_cast<iEEPROMIO_c&>(__IsoAgLib::getEepromInstance());};
inline iEEPROMIO_c& operator<<(iEEPROMIO_c& refc_stream, const iGetyPos_c& refc_data )
  { return static_cast<iEEPROMIO_c&>(operator<<(static_cast<__IsoAgLib::EEPROMIO_c&>(refc_stream), refc_data ) );};
inline iEEPROMIO_c& operator>>(iEEPROMIO_c& refc_stream, iGetyPos_c& refc_data )
  { return static_cast<iEEPROMIO_c&>(operator>>(static_cast<__IsoAgLib::EEPROMIO_c&>(refc_stream), refc_data ) );};

} // end of IsoAgLibInterafce namespace
#endif
