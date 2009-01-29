/***************************************************************************
                          ieeprom_io.h - header for iEEPROM_IO_c object
                                         for communication with EEPROM
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
#ifndef IEEPROM_IO_H
#define IEEPROM_IO_H

#include "impl/eepromio_c.h"
#include <IsoAgLib/comm/Part5_NetworkManagement//iisoname_c.h>
#include <string>

//using STL_NAMESPACE::string;

// Begin Namespace IsoAgLib
namespace IsoAgLib {
/**
  object for communication with the EEPROM,
  stream read/write operators for all basic types;
  avoid rewriting same values to EEPROM;
  manages operations cross segment boundaries
  *@author Dipl.-Inform. Achim Spangler
*/
class iEepromIo_c : private __IsoAgLib::EepromIo_c {
public:
  /** destructor has nothing to destruct */
  ~iEepromIo_c() {};

  // ++++++++++++++++++++++++++++++++++++
  // ++++ EEPROM managing operations ++++
  // ++++++++++++++++++++++++++++++++++++

  /**
    deliver the size of the whole EEPROM memory in bytes;
    uses BIOS function (which delivers KB -> mult with 1024)
    @return size of EEPROM in BYTE
  */
  static uint16_t eepromSize(){return EepromIo_c::eepromSize();};

  /**
    set the write position in EEPROM (in Byte); answer if possible
    if aui16_adress exceeds EEPROM memory ignore setting and set Err_c::range

    possible errors:
        * Err_c::range aui16_adress outer the limits of EEPROM memory

    @param aui16_adress position of write mark [uint8_t]
    @return true -> setting of write mark without errors
  */
  bool setp(uint16_t aui16_adress) {return EepromIo_c::setp(aui16_adress);};

  /**
    set the read position in EEPROM (in Byte); answer if possible
    if aui16_adress exceeds EEPROM memory ignore setting and set Err_c::range

    possible errors:
        * Err_c::range aui16_adress outer the limits of EEPROM memory

    @param aui16_adress position of read mark [uint8_t]
    @return true -> setting of read mark without errors
  */
  bool setg(uint16_t aui16_adress) {return EepromIo_c::setg(aui16_adress);};

  /**
    get the write position in EEPROM (in Byte)
    @return position of write mark [uint8_t]
  */
  uint16_t tellp(){return EepromIo_c::tellp();};

  /**
    get the read position in EEPROM (in Byte)
    @return position of read mark [uint8_t]
  */
  uint16_t tellg(){return EepromIo_c::tellg();};

  /**
    check if write position is at end of EEPROM
    (parameter specifies lookahead (normally length of operation - 1 is passed to check if a block of data fits into)

    possible errors:
        * range if (ab_setState == true) and (current read position + lookahead) is out of EEPROM range

    @param aui16_lookahead optional uint8_t lookahead offset (default 0 -> only current write mark position tested)
    @return false -> (current position + lookahead) is a valid EEPROM address. (true -> out of EEPROM range)
  */
  bool eofp(uint16_t aui16_lookahead = 0, bool ab_setState = false)
   {return EepromIo_c::eofp(aui16_lookahead, ab_setState);};

  /**
    check if read position is at end of EEPROM
    (parameter specifies lookahead (normally length of operation - 1 is passed to check if a block of data fits into)

    possible errors:
        * range if (ab_setState == true) and (current read position + lookahead) is out of EEPROM range

    @param aui16_lookahead optional uint8_t lookahead offset (default 0 -> only current read mark position tested)
    @return false -> (current position + lookahead) is a valid EEPROM address. (true -> out of EEPROM range)
  */
  bool eofg(uint16_t aui16_lookahead = 0, bool ab_setState = false)
    {return EepromIo_c::eofg(aui16_lookahead, ab_setState);};

  /* *************************************** */
  /* *** EEPROM data operation functions *** */
  /* ************** writing **************** */
  /* *************************************** */


  /**
    write a text string value to EEPROM from actual write position on (tellp() )

    possible errors:
        * Err_c::range writing position exceeds end of EEPROM
        * Err_c::hwBusy the EEPROM was busy with another action
        * Err_c::eepromSegment low level writing caused segment error

    @see iEepromIo_c::tellp
    @see iEepromIo_c::setp
    @param arc_val string to write into EEPROM
    @return reference to this iEepromIo_c instance (for chains like "eeprom << val1 << val2 << ... << val_n;")
  */
  iEepromIo_c& operator<<(STL_NAMESPACE::string& arc_val)
    {return static_cast<iEepromIo_c&>(EepromIo_c::operator<<(arc_val));};

  /**
    write a value to EEPROM from actual write position on (tellp() )
    by use of template mechanism the correct write implementation is generted
    for the given parameter type

    possible errors:
        * Err_c::range writing position exceeds end of EEPROM
        * Err_c::hwBusy the EEPROM was busy with another action
        * Err_c::eepromSegment low level writing caused segment error

    @see iEepromIo_c::tellp
    @see iEepromIo_c::setp
    @param rTemplateVal value of any type to write into EEPROM from actual write position on
    @return reference to this iEepromIo_c instance (for chains like "eeprom << val1 << val2 << ... << val_n;")
  */
  template<class T>
  iEepromIo_c& operator<<(const T& rTemplateVal)
    {return static_cast<iEepromIo_c&>(EepromIo_c::operator<<(rTemplateVal));}

  /**
    write a uint8_t string value to EEPROM from actual write position on (tellp() )

    possible errors:
        * Err_c::range writing position exceeds end of EEPROM
        * Err_c::busy the EEPROM was busy with another action
        * Err_c::eepromSegment low level writing caused segment error

    @see EepromIo_c::tellp
    @see EepromIo_c::setp
    @param apb_string string to write into EEPROM
    @param aui16_number length of the string to write
    @return reference to this EepromIo_c instance (for chains like "eeprom << val1 << val2 << ... << val_n;")
  */
  inline iEepromIo_c& writeString(const uint8_t *const apb_string, uint16_t aui16_number)
    {return static_cast<iEepromIo_c&>(EepromIo_c::writeString(apb_string, aui16_number));};

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

    @see iEepromIo_c::tellg
    @see iEepromIo_c::setg
    @param rTemplateVal EERPOM data is read into rTemplateVal from actual read position on
    @return reference to this iEepromIo_c instance (for chains like "eeprom >> val1 >> val2 >> ... >> val_n;")
  */
  template<class T>
  iEepromIo_c& operator>>(T& rTemplateVal)
    {return static_cast<iEepromIo_c&>(EepromIo_c::operator>>(rTemplateVal));}

  /**
    read operator for strings with given length; uses BIOS function

    possible errors:
        * Err_c::range reading position exceeds end of EEPROM
        * Err_c::hwBusy the EEPROM was busy with another reading action
        * Err_c::eepromSegment low level reading caused segment error

    @see iEepromIo_c::tellg
    @see iEepromIo_c::setg
    @param apb_string pointer to uint8_t string, which should be read from actual EEPROM read position on
    @param aui16_number number of uint8_t to read into string
    @return true -> read with success
  */
  bool readString(uint8_t *const apb_string, uint16_t aui16_number)
    {return EepromIo_c::readString(apb_string, aui16_number);};
private:
  /** allow getIeepromInstance() access to shielded base class.
      otherwise __IsoAgLib::getEepromInstance() wouldn't be accepted by compiler
    */
  friend iEepromIo_c& getIeepromInstance( void );
  friend iEepromIo_c& operator<<(iEepromIo_c& rc_stream, const iIsoName_c& rc_data );
  friend iEepromIo_c& operator>>(iEepromIo_c& rc_stream, iIsoName_c& rc_data );
};

/** C-style function, to get access to the unique EepromIo_c singleton instance */
inline iEepromIo_c& getIeepromInstance( void ) { return static_cast<iEepromIo_c&>(__IsoAgLib::getEepromInstance());};
inline iEepromIo_c& operator<<(iEepromIo_c& rc_stream, const iIsoName_c& rc_data )
  { return static_cast<iEepromIo_c&>(operator<<(static_cast<__IsoAgLib::EepromIo_c&>(rc_stream), rc_data ) );};
inline iEepromIo_c& operator>>(iEepromIo_c& rc_stream, iIsoName_c& rc_data )
  { return static_cast<iEepromIo_c&>(operator>>(static_cast<__IsoAgLib::EepromIo_c&>(rc_stream), rc_data ) );};


/** this typedef is only for some time to provide backward compatibility at API level */
typedef iEepromIo_c iEEPROMIO_c;

} // end of IsoAgLibInterafce namespace
#endif
