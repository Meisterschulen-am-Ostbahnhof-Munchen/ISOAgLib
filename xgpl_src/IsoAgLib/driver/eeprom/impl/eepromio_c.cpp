/***************************************************************************
                          eepromio_c.cpp -  header for EEPROMIO_c object
                                          for communication with EEPROM
                             -------------------
    begin                : Mon Oct 25 1999
    copyright            : (C) 1999 - 2004 by Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
    type                 : Source
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
#include "eepromio_c.h"
#include <IsoAgLib/hal/system.h>
#include <IsoAgLib/util/config.h>
#include <IsoAgLib/util/impl/getypos_c.h>
#include <IsoAgLib/driver/system/impl/system_c.h>
// Begin Namespace __IsoAgLib
namespace __IsoAgLib {
/** C-style function, to get access to the unique EEPROMIO_c singleton instance */
EEPROMIO_c& getEepromInstance( void ) { return EEPROMIO_c::instance();};

/*******************************************/
/** definition of public element functions */
/*******************************************/
/** default initialisation */
void EEPROMIO_c::init()
{ // set the segment size
  // verify that System is int
  getSystemInstance().init();
  ui16_segmentSize = HAL::getEepromSegmentSize();
  // set read/write positions to beginning
  ui16_rPosition = ui16_wPosition = 0;
  // make shure that system is initialized
  System_c::instance().init();
}

/** destructor has nothing to destruct */
EEPROMIO_c::~EEPROMIO_c(){
}

// ++++++++++++++++++++++++++++++++++++
// ++++ EEPROM managing operations ++++
// ++++++++++++++++++++++++++++++++++++


/**
  set the write position in EEPROM (in Byte); answer if possible
  if rui16_adress exceeds EEPROM memory ignore setting and set Err_c::range

  possible errors:
      * range rui16_adress outer the limits of EEPROM memory

  @param rui16_adress position of write mark [uint8_t]
  @return true -> setting of write mark without errors
*/
bool EEPROMIO_c::setp(uint16_t rui16_adress)
{
  if (eepromSize() > rui16_adress)
  { // wanted position is in EEPROM range
    ui16_wPosition = rui16_adress;
    return true;
  }
  else
  { // wanted position exceeds memory -> set range error
    getLbsErrInstance().registerError( LibErr_c::Range, LibErr_c::Eeprom );
    return false;
  }
};

/**
  set the read position in EEPROM (in Byte); answer if possible
  if rui16_adress exceeds EEPROM memory ignore setting and set Err_c::range

  possible errors:
      * range rui16_adress outer the limits of EEPROM memory

  @param rui16_adress position of read mark [uint8_t]
  @return true -> setting of read mark without errors
*/
bool EEPROMIO_c::setg(uint16_t rui16_adress)
{
  if (eepromSize() > rui16_adress)
  { // wanted position is in EEPROM range
    ui16_rPosition = rui16_adress;
    return true;
  }
  else
  { // wanted position exceeds memory -> set range error
    getLbsErrInstance().registerError( LibErr_c::Range, LibErr_c::Eeprom );
    return false;
  }
};

/**
  check if write position is at end of EEPROM
  (parameter specifies needed length for operation
  -> false means that enough space is there for length bytes of data)

  possible errors:
      * range if rb_setState == true and actual write position is nearer to end of EEPROM than rui8_length byte

  @param rui8_length optional size of uint8_t, which must fit into EEPROM from actual position on (default 0 -> only write mark position tested)
  @return false -> write marker is more than rui8_length uint8_t ahead end of EEPROM
*/
bool EEPROMIO_c::eofp(uint8_t rui8_length, bool rb_setState)
{ // compare (write position + length of wanted data) with size of EEPROM memory
  if ((ui16_wPosition + rui8_length) >= eepromSize())
  { // actual or end position of write access exceeds EEPROM memory
    if (rb_setState) getLbsErrInstance().registerError( LibErr_c::Range, LibErr_c::Eeprom );
    return true;  // means: End of EEPROM memory is reached
  }
  else
  {
    return false;  // means: End of EEPROM memory is NOT reached
  }
};

/**
  check if read position is at end of EEPROM
  (parameter specifies needed length for operation
  -> false means that enough space is there for length bytes of data)

  possible errors:
      * range if rb_setState == true and actual read position is nearer to end of EEPROM than rui8_length byte

  @param rui8_length optional size of uint8_t, which must fit into EEPROM from actual position on (default 0 -> only read mark position tested)
  @return false -> read marker is more than rui8_length uint8_t ahead end of EEPROM
*/
bool EEPROMIO_c::eofg(uint8_t rui8_length, bool rb_setState)
{ // compare (read position + length of wanted data) with size of EEPROM memory
  if ((ui16_rPosition + rui8_length) >= eepromSize())
  { // actual or end position of read access exceeds EEPROM memory
    if (rb_setState) getLbsErrInstance().registerError( LibErr_c::Range, LibErr_c::Eeprom );
    return true; // means: End of EEPROM memory is reached
  }
  else
  {
    return false; // means: End of EEPROM memory is NOT reached
  }
};



/* ******************************** */
/* iostream related output operator */
/* ******************************** */

/**
  write a uint8_t string value to EEPROM from actual write position on (tellp() )

  possible errors:
      * range writing position exceeds end of EEPROM
      * busy the EEPROM was busy with another action
      * eepromSegment low level writing caused segment error

  @see EEPROMIO_c::tellp
  @see EEPROMIO_c::setp
  @param rpb_string string to write into EEPROM
  @param rui16_number length of the string to write
  @return reference to this EEPROMIO_c instance (for chains like "eeprom << val1 << val2 << ... << val_n;")
*/
EEPROMIO_c& EEPROMIO_c::writeString(const uint8_t *const rpb_string, uint16_t rui16_number)
{ // check if enough space for string is after actual write position
  // second parameter true -> set Err_c::range if end is reached
  if (!eofp(rui16_number, true))
  { // use private write function to read in correct number of bytes into data string
    write (ui16_wPosition, rui16_number, rpb_string);
    ui16_wPosition += (rui16_number); //inkrement position
  }
  return *this;
};

/**
  read operator for strings with given length; uses BIOS function
  @see EEPROMIO_c::tellg
  @see EEPROMIO_c::setg
  @param rpb_string pointer to uint8_t string, which should be read from actual EEPROM read position on
  @param rui16_number number of uint8_t to read into string
  @return true -> read with success
*/
bool EEPROMIO_c::readString(uint8_t *const rpb_string, uint16_t rui16_number)
{ // check if enough space for string is after actual read position
  // second parameter true -> set Err_c::range if end is reached
  if (!eofg(rui16_number), true)
  { // enough space in EEPROM from actual position on
    // call BIOS function to check that EEPROM is ready
    setState4BiosReturn(wait_eepromReady());
    // use eepromRead BIOS function to read in correct number of bytes into data string
    // don't update read position
    int16_t i16_retVal = HAL::eepromRead(ui16_rPosition, rui16_number, rpb_string);
    setState4BiosReturn(i16_retVal); // update state dependent on return of BIOS function
    // increment position on success
    if (i16_retVal == HAL_NO_ERR)
    {
      ui16_rPosition += rui16_number;
      return true;
    }
    else return false;
  }
  else
  { // report that read access had no success because of rane error
    return false;
  }
}



/********************************************/
/** definition of private element functions */
/********************************************/


/**
  write the pointed data string to eeprom; uses BIOS function
  @param rui16_adress adress to start writing
  @param rui16_number amount of bytes to write to EEPROM
  @param rpb_data pointer to string of uint8_t to write to EEPROM
  @return true -> write successful
*/
bool EEPROMIO_c::write(uint16_t rui16_adress, uint16_t rui16_number, const uint8_t* rpb_data){
  uint16_t ui16_restNumber = rui16_number,
       ui16_actualStart = rui16_adress,
       ui16_actualSize;
  const uint8_t* pb_data = rpb_data;
  uint8_t pb_compare[MAX_EEPROM_SEGMENT_SIZE];

  /* check input data */
  if ((rui16_adress + rui16_number) > eepromSize())
  { // range error because wanted write operation exceeds EEPROM memory
    getLbsErrInstance().registerError( LibErr_c::Range, LibErr_c::Eeprom );
  }

  while (ui16_restNumber > 0)
  { // if data doesn't fit in one segment write with series of BIOS write calls
    // init for write
   if (writeInit())
   {  // init successfull
      // get data portion for this call
      // set to maximal size for this segment
      ui16_actualSize = maxSize(ui16_actualStart);
      // if needed size is smaller or equiv set ui16_actualSize to the needed
      if (ui16_actualSize >= ui16_restNumber) ui16_actualSize = ui16_restNumber;
      // now ui16_actualSize is allowed -> write BIOS

      // perform up to MAX_EEPROM_WRITE_TRY_CYCLE_CNT write tries - and register error if re-read of just written
      // value is different from wanted value
      uint8_t ui8_tryCnt = 0;
      for ( ; ui8_tryCnt < MAX_EEPROM_WRITE_TRY_CYCLE_CNT; ui8_tryCnt++ )
      { // first check if the data is different from the original data in EEPROM
        // (avoid rewrite of same data)
        // call BIOS function to check that EEPROM is ready
        setState4BiosReturn(wait_eepromReady());
        // then call BIOS function to read
        setState4BiosReturn(HAL::eepromRead (ui16_actualStart, ui16_actualSize, (uint8_t*)pb_compare));

        // compare actual data in EEPROM with given data
        if (CNAMESPACE::memcmp(pb_compare, pb_data,ui16_actualSize) != 0)
        { // old data is different -> write new data
          if (writeInit())
          { // call BIOS function
            setState4BiosReturn(HAL::eepromWrite(ui16_actualStart, ui16_actualSize, (uint8_t*)pb_data));
          }
        }
        else
        { // re-read of value delivers same value -> break try loop
          break;
        }
      }
      if ( ui8_tryCnt == MAX_EEPROM_WRITE_TRY_CYCLE_CNT )
      { // write without success, as re-read delivers always different value
        getLbsErrInstance().registerError( LibErr_c::EepromWriteError, LibErr_c::Eeprom );
        // don't try further writes
        break;
      }
      else if (getLbsErrInstance().good( LibErr_c::Eeprom  ))
      { //update the write vals
        // decrement number of uint8_t which must be written in next loop run
        ui16_restNumber -= ui16_actualSize;
        // set the EEPROM memory start position for next write loop run
        ui16_actualStart += ui16_actualSize;
        // set pointer to source data to begin of next part
        pb_data += ui16_actualSize;
      }
      else
      { // error is already stored in state var ->just exit loop
        break;
      }
    } // if write init
  } // while
  // if good()
  return (getLbsErrInstance().good( LibErr_c::Eeprom )?true:false);
}


/**
  init for single write operation, check if write is possible; uses BIOS function
  @return true -> EEPROM is ready for write operations
*/
bool EEPROMIO_c::writeInit(){
  // clear the BIOS state
  getLbsErrInstance().clear( LibErr_c::Eeprom );

  // check if eeprom is ready; call BIOS function
  if ( wait_eepromReady() != EE_READY )
  { // error -> got not ready
    getLbsErrInstance().registerError( LibErr_c::Busy, LibErr_c::Eeprom );
    return false;
  }

  // set EEPROM to writable
  setState4BiosReturn(HAL::eepromWp(OFF));

  if (getLbsErrInstance().good( LibErr_c::Eeprom ))
  { // only try writing if write protect was succesful deactivated
    // call wait_eepromReady() to test if EEPROM is ready
    setState4BiosReturn(wait_eepromReady());
  }
  // return if EEPROM is in good state with writeable and ready state
  return (getLbsErrInstance().good( LibErr_c::Eeprom )?true:false);
}
/**
  wait with triggering the watchdog, till the EEPROM is ready

  @return true -> EEPROM is ready for next operation
*/
int16_t EEPROMIO_c::wait_eepromReady( void )
{ // test if EEPROM is ready for up to 1000msec.
  // don't builf timestamps, if EEPROM immediately ready
  if(HAL::eepromReady() == EE_READY) return EE_READY;

  int16_t i16_result = HAL_BUSY_ERR;
  int32_t i32_startTime = HAL::getTime();
  while ((HAL::getTime() - i32_startTime) < 1000)
  { // call BIOS function and exit loop if EEPROM is ready
    i16_result = HAL::eepromReady();
    if( i16_result == EE_READY) break;
    // trigger watchdog
    HAL::wdTriggern();
  }
  return i16_result;
}
/**
  deliver the max number of bytes, which can be written in EEPROM from given start position on
  @param rui16_adress wanted write position
  @return amount of bytes between write position and end of EEPROM
*/
uint16_t EEPROMIO_c::maxSize(uint16_t rui16_adress){
  uint16_t ui16_segmentEnd = ((rui16_adress/ui16_segmentSize + 1)*ui16_segmentSize) -1;
  return ((ui16_segmentEnd - rui16_adress)+1);
}


/**
  set error flags dependent on BIOS return value
  @param ri16_biosReturn BIOS return value which should be translated in error state of EEPROM_IO
*/
void EEPROMIO_c::setState4BiosReturn(int16_t ri16_biosReturn)
{
  switch (ri16_biosReturn)
  {
    case HAL_NO_ERR:
      break;
    case HAL_RANGE_ERR:
      getLbsErrInstance().registerError( LibErr_c::Range, LibErr_c::Eeprom );
      break;
    case HAL_BUSY_ERR:
      getLbsErrInstance().registerError( LibErr_c::Busy, LibErr_c::Eeprom );
      break;
    case HAL_OVERFLOW_ERR:
      getLbsErrInstance().registerError( LibErr_c::EepromSegment, LibErr_c::Eeprom );
      break;
    case HAL_NOACT_ERR:
    default:
      getLbsErrInstance().registerError( LibErr_c::Unspecified, LibErr_c::Eeprom );
      break;
  }
};

/**
  write the given data string to eeprom from actual write position
  possible errors:
      * Err_c::range writing exceeds EEPROM memory
      * Err_c::busy other task communicates with EEPROM
      * Err_c::unspecified low level SPI EEPROM communication doesn't work
      * Err_c::eepromSegment low level reading caused segment error

  @param rpb_data pointer to data
  @param rui8_len lenght of data
  @return reference to this object for chains
*/
EEPROMIO_c& EEPROMIO_c::writeIntern(const uint8_t* rpb_data, uint8_t rui8_len)
{ // check if enough space for type T is after actual write position
  // second parameter true -> set Err_c::range if end is reached
  if (!eofp(rui8_len, true))
  { // use private write function to read in correct number of bytes into data string
    write (ui16_wPosition, rui8_len, rpb_data);
    ui16_wPosition += rui8_len; //inkrement position
  }
  return *this;
}
/**
  read from actual read position wanted amount of data in to pointer

  possible errors:
      * Err_c::range writing exceeds EEPROM memory
      * Err_c::busy other task communicates with EEPROM
      * Err_c::unspecified low level SPI EEPROM communication doesn't work
      * Err_c::eepromSegment low level reading caused segment error

  @param rpb_data pointer to data
  @param rui8_len lenght of data
  @return reference to this object for chains
*/
EEPROMIO_c& EEPROMIO_c::readIntern(uint8_t* rpb_data, uint8_t rui8_len) {
  // check if enough space for string is after actual read position
  // second parameter true -> set Err_c::range if end is reached
  if (!eofg(rui8_len, true))
  { // use private eepromRead function to read in correct number of bytes into data string
    // call BIOS function to check that EEPROM is ready
    setState4BiosReturn(wait_eepromReady());
    HAL::eepromRead(ui16_rPosition, rui8_len, rpb_data);
    ui16_rPosition += rui8_len; //inkrement position
  }
  return *this;
}

EEPROMIO_c& operator<<(EEPROMIO_c& refc_stream, const GetyPos_c& refc_data )
{
  refc_stream.writeIntern(refc_data.getConstName().outputString(), 8);
  return refc_stream;
}
EEPROMIO_c& operator>>(EEPROMIO_c& refc_stream, GetyPos_c& refc_data )
{
  uint8_t tempName[8];
  refc_stream.readIntern(tempName, sizeof(8));
  refc_data.set( tempName );
  return refc_stream;
}


} // end of namespace __IsoAgLib
