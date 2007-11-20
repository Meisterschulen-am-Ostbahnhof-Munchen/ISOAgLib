/***************************************************************************
                          eepromio_c.h  -  header for EEPROM_IO_c object
                                          for communication with EEPROM
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
#ifndef EEPROM_IO_H
#define EEPROM_IO_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include <IsoAgLib/util/iliberr_c.h>
#include <IsoAgLib/util/impl/singleton.h>
#include <IsoAgLib/hal/eeprom.h>

#include <supplementary_driver/hal/rs232.h>


// workaround for 8.0Beta bug
#if defined(__TSW_CPP__) && !defined(__TSW_CPP_70__) && !defined(__TSW_CPP_756__)
  #include <stl/_string.h>
#else
  #include <string>
#endif

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {
class IsoName_c;

/**
  object for communication with the EEPROM,
  stream read/write operators for all basic types;
  avoid rewriting same values to EEPROM;
  manages operations cross segment boundaries
  @short Simple data communication with EEPROM.
  @author Dipl.-Inform. Achim Spangler
*/
class EepromIo_c : public Singleton<EepromIo_c> {
public:
  typedef STL_NAMESPACE::string cc_string;
  /** default initialisation */
  void init();
  /** every subsystem of IsoAgLib has explicit function for controlled shutdown
    */
  void close( void ){};

  /** destructor has nothing to destruct */
  ~EepromIo_c();

  // ++++++++++++++++++++++++++++++++++++
  // ++++ EEPROM managing operations ++++
  // ++++++++++++++++++++++++++++++++++++

  /**
    deliver the size of the whole EEPROM memory in bytes;
    uses BIOS function
    @return size of EEPROM in BYTE
  */
  static uint16_t eepromSize(){return (HAL::getEepromSize());};

  /**
    set the write position in EEPROM (in Byte); answer if possible
    if aui16_adress exceeds EEPROM memory ignore setting and set range

    possible errors:
        * range aui16_adress outer the limits of EEPROM memory

    @param aui16_adress position of write mark [uint8_t]
    @return true -> setting of write mark without errors
  */
  bool setp(uint16_t aui16_adress);
  /**
    set the read position in EEPROM (in Byte); answer if possible
    if aui16_adress exceeds EEPROM memory ignore setting and set range

    possible errors:
        * range aui16_adress outer the limits of EEPROM memory

    @param aui16_adress position of read mark [uint8_t]
    @return true -> setting of read mark without errors
  */
  bool setg(uint16_t aui16_adress);

  /**
    get the write position in EEPROM (in Byte)
    @return position of write mark [uint8_t]
  */
  inline uint16_t tellp(){return mui16_wPosition;};

  /**
    get the read position in EEPROM (in Byte)
    @return position of read mark [uint8_t]
  */
  inline uint16_t tellg(){return mui16_rPosition;};

  /**
    check if write position is at end of EEPROM
    (parameter specifies needed length for operation
    -> false means that enough space is there for length bytes of data)

    possible errors:
        * range if ab_setState == true and actual write position is nearer to end of EEPROM than aui8_length byte

    @param aui8_length optional size of uint8_t, which must fit into EEPROM from actual position on (default 0 -> only write mark position tested)
    @param ab_setState if true a range is set eof is reached (default false)
    @return false -> write marker is more than aui8_length uint8_t ahead end of EEPROM
  */
  bool eofp(uint8_t aui8_length = 0, bool ab_setState = false);

  /**
    check if read position is at end of EEPROM
    (parameter specifies needed length for operation
    -> false means that enough space is there for length bytes of data)

    possible errors:
        * range if ab_setState == true and actual read position is nearer to end of EEPROM than aui8_length byte

    @param aui8_length optional size of uint8_t, which must fit into EEPROM from actual position on (default 0 -> only read mark position tested)
    @param ab_setState if true a range is set eof is reached (default false)
    @return false -> read marker is more than aui8_length uint8_t ahead end of EEPROM
  */
  bool eofg(uint8_t aui8_length = 0, bool ab_setState = false);

  /* *************************************** */
  /* *** EEPROM data operation functions *** */
  /* ************** writing **************** */
  /* *************************************** */

  /**
    write a value to EEPROM from actual write position on (tellp() )
    by use of template mechanism the correct write implementation is generted
    for the given parameter type

    possible errors:
        * range writing position exceeds end of EEPROM
        * busy the EEPROM was busy with another action
        * eepromSegment low level writing caused segment error

    @see EepromIo_c::tellp
    @see EepromIo_c::setp
    @param rTemplateVal value of any type to write into EEPROM from actual write position on
    @return reference to this EepromIo_c instance (for chains like "eeprom << val1 << val2 << ... << val_n;")
  */
  template<class T>
    EepromIo_c& operator<<(const T& rTemlpateVal){return writeIntern(((const uint8_t*)(&rTemlpateVal)), sizeof(T));}
  /**
    write a text string value to EEPROM from actual write position on (tellp() )

    possible errors:
        * range writing position exceeds end of EEPROM
        * busy the EEPROM was busy with another action
        * eepromSegment low level writing caused segment error

    @see EepromIo_c::tellp
    @see EepromIo_c::setp
    @param arc_val string to write into EEPROM
    @return reference to this EepromIo_c instance (for chains like "eeprom << val1 << val2 << ... << val_n;")
  */
  inline EepromIo_c& operator<<(const cc_string& arc_val);
  /**
    write a uint8_t string value to EEPROM from actual write position on (tellp() )

    possible errors:
        * range writing position exceeds end of EEPROM
        * busy the EEPROM was busy with another action
        * eepromSegment low level writing caused segment error

    @see EepromIo_c::tellp
    @see EepromIo_c::setp
    @param apb_string string to write into EEPROM
    @param aui16_number length of the string to write
    @return reference to this EepromIo_c instance (for chains like "eeprom << val1 << val2 << ... << val_n;")
  */
  EepromIo_c& writeString(const uint8_t *const apb_string, uint16_t aui16_number);

  /* *************************************** */
  /* *** EEPROM data operation functions *** */
  /* ************** reading **************** */
  /* *************************************** */
  /**
    read a value to EEPROM from actual write position on (tellg() )
    by use of template mechanism the correct read implementation is generted
    for the given parameter type

    possible errors:
        * range reading position exceeds end of EEPROM
        * busy the EEPROM was busy with another action
        * eepromSegment low level reading caused segment error

    @see EepromIo_c::tellg
    @see EepromIo_c::setg
    @param rTemplateVal EERPOM data is read into rTemplateVal from actual read position on
    @return reference to this EepromIo_c instance (for chains like "eeprom >> val1 >> val2 >> ... >> val_n;")
  */
  template<class T>
  EepromIo_c& operator>>(T& rTemlpateVal){return readIntern(((uint8_t*)(&rTemlpateVal)), sizeof(T));}

  /**
    read operator for strings with given length; uses BIOS function

    possible errors:
        * range reading position exceeds end of EEPROM
        * busy the EEPROM was busy with another reading action
        * eepromSegment low level reading caused segment error

    @see EepromIo_c::tellg
    @see EepromIo_c::setg
    @param apb_string pointer to uint8_t string, which should be read from actual EEPROM read position on
    @param aui16_number number of uint8_t to read into string
    @return true -> read with success
  */
  bool readString(uint8_t *const apb_string, uint16_t aui16_number);

private:
// Private methods
  friend class Singleton<EepromIo_c>;
  friend EepromIo_c& operator<<(EepromIo_c& rc_stream, const IsoName_c& rc_data );
  friend EepromIo_c& operator>>(EepromIo_c& rc_stream, IsoName_c& rc_data );

  /** private constructor which prevents direct instantiation in user application
    * NEVER define instance of EepromIo_c within application
    */
  EepromIo_c( void ) {};
  /**
    initialize directly after the singleton instance is created.
    this is called from singleton.h and should NOT be called from the user again.
    users please use init(...) instead.
  */
  void singletonInit();
  /**
    set error flags dependent on BIOS return value
    @param ai16_biosReturn BIOS return value which should be translated in error state of EEPROM_IO
  */
  static void setState4BiosReturn(int16_t ai16_biosReturn);

  /**
    init for single write operation, check if write is possible; uses BIOS function

    possible errors:
        * busy other task communicates with EEPROM
        * unspecified low level SPI EEPROM communication doesn't work
        * eepromSegment low level reading caused segment error

    @return true -> EEPROM is ready for write operations
  */
  static bool writeInit();
  /**
    wait with triggering the watchdog, till the EEPROM is ready

    @return EE_READY -> EEPROM is ready for next operation
  */
  static int16_t wait_eepromReady( void );
  /**
    deliver the max number of bytes, which can be written in EEPROM from given start position on
    @param aui16_adress wanted write position
    @return amount of bytes between write position and end of EEPROM
  */
  uint16_t maxSize(uint16_t aui16_adress);

  /**
    write the template parameter value at given position to EEPROM,
    return true if successfull

    possible errors:
        * range writing exceeds EEPROM memory
        * busy other task communicates with EEPROM
        * unspecified low level SPI EEPROM communication doesn't work
        * eepromSegment low level reading caused segment error

    @param aui16_adress adress to start writing
    @param rTemplateVal value of any type to write to EEPROM
    @return true -> write successful
  */
  template<class T>
  bool write(uint16_t aui16_adress, T rTemplateVal);

  /**
    write the pointed data string to eeprom; uses BIOS function

    possible errors:
        * range writing exceeds EEPROM memory
        * busy other task communicates with EEPROM
        * unspecified low level SPI EEPROM communication doesn't work
        * eepromSegment low level reading caused segment error

    @param aui16_adress adress to start writing
    @param aui16_number amount of bytes to write to EEPROM
    @param apb_data pointer to string of uint8_t to write to EEPROM
    @return true -> write successful
  */
  bool write(uint16_t aui16_adress, uint16_t aui16_number, const uint8_t* apb_data);
  /**
    write the given data string to eeprom from actual write position
    possible errors:
        * range writing exceeds EEPROM memory
        * busy other task communicates with EEPROM
        * unspecified low level SPI EEPROM communication doesn't work
        * eepromSegment low level reading caused segment error

    @param apb_data pointer to data
    @param aui8_len lenght of data
    @return reference to this object for chains
  */
  EepromIo_c& writeIntern(const uint8_t* apb_data, uint8_t aui8_len);
  /**
    read from actual read position wanted amount of data in to pointer

    possible errors:
        * range writing exceeds EEPROM memory
        * busy other task communicates with EEPROM
        * unspecified low level SPI EEPROM communication doesn't work
        * eepromSegment low level reading caused segment error

    @param apb_data pointer to data
    @param aui8_len lenght of data
    @return reference to this object for chains
  */
  EepromIo_c& readIntern(uint8_t* apb_data, uint8_t aui8_len);

private:
// Private attributes
  /**
    segment size of EEPROM which can't be exceedecd by write actions
  */
  uint16_t mui16_segmentSize;
  /** actual read position in EEPROM */
  uint16_t mui16_rPosition;
  /** actual write position in EEPROM */
  uint16_t mui16_wPosition;
};


/* *********************************************************************** */
/* ****inline implementation of stream input/output template functions**** */
/* *********************************************************************** */



/**
  write a text string value to EEPROM from actual write position on (tellp() )

  possible errors:
      * range writing position exceeds end of EEPROM
      * busy the EEPROM was busy with another action
      * eepromSegment low level writing caused segment error

  @see __IsoAgLib::EepromIo_c::tellp
  @see __IsoAgLib::EepromIo_c::setp
  @param arc_val string to write into EEPROM
  @return reference to this EepromIo_c instance (for chains like "eeprom << val1 << val2 << ... << val_n;")
*/
inline EepromIo_c& EepromIo_c::operator<<(const cc_string& arc_val)
{ // check if enough space for string is after actual write position
  // second parameter true -> set range if end is reached
  if (!eofp(arc_val.size() * sizeof(int8_t), true))
  { // use private write function to read in correct number of bytes into data string
   write (mui16_wPosition, arc_val.size() * sizeof(int8_t),
            static_cast<const uint8_t*>(static_cast<const void*>(arc_val.c_str())));
    mui16_wPosition += (arc_val.size() * sizeof(int8_t)); //inkrement position
  }
  return *this;
};

/**
  write the template parameter value at given position to EEPROM,
  return true if successfull

  possible errors:
      * range writing exceeds EEPROM memory
      * busy other task communicates with EEPROM
      * unspecified low level SPI EEPROM communication doesn't work
      * eepromSegment low level reading caused segment error

  @param aui16_adress adress to start writing
  @param rTemplateVal value of any type to write to EEPROM
  @return true -> write successful
*/
template<class T>
bool EepromIo_c::write(uint16_t aui16_adress, T rTemplateVal)
{
  bool b_result = false;
  if ((aui16_adress + sizeof(T)) < eepromSize())
  { // write doesn't exceed EEPROM limits
    // copy to local var -> then call string write
    T tempVal = rTemplateVal;
    b_result =  write(aui16_adress, sizeof(T), static_cast<uint8_t*>(static_cast<void*>(&tempVal)));
  }
  else
  { // write action would exceed limits
    getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::Eeprom ); // changed by M.Wodok, was "eeprom"
  }
  return b_result;
};

/** C-style function, to get access to the unique EepromIo_c singleton instance */
EepromIo_c& getEepromInstance( void );
EepromIo_c& operator<<(EepromIo_c& rc_stream, const IsoName_c& rc_data );
EepromIo_c& operator>>(EepromIo_c& rc_stream, IsoName_c& rc_data );


/** this typedef is only for some time to provide backward compatibility at API level */
typedef EepromIo_c EEPROMIO_c;
}

#endif
