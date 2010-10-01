/*
  eepromio_c.cpp: header for EepromIo_c object for communication with
    EEPROM

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include "eepromio_c.h"
#include <IsoAgLib/hal/hal_system.h>
#include <IsoAgLib/util/config.h>
#include <IsoAgLib/comm/Part5_NetworkManagement//impl/isoname_c.h>
#include <IsoAgLib/driver/system/impl/system_c.h>


// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

/** C-style function, to get access to the unique EepromIo_c singleton instance */
EepromIo_c& getEepromInstance( void ) { return EepromIo_c::instance();};


/** default initialisation */
void EepromIo_c::init()
{ // set the segment size
  mui16_segmentSize = HAL::getEepromSegmentSize();
  // set read/write positions to beginning
  mui16_rPosition = mui16_wPosition = 0;
}


bool
EepromIo_c::setp(uint16_t aui16_adress)
{
  if (eepromSize() > aui16_adress)
  { // wanted position is in EEPROM range
    mui16_wPosition = aui16_adress;
    return true;
  }
  else
  { // wanted position exceeds memory -> set range error
    getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::Eeprom );
    return false;
  }
};


bool
EepromIo_c::setg(uint16_t aui16_adress)
{
  if (eepromSize() > aui16_adress)
  { // wanted position is in EEPROM range
    mui16_rPosition = aui16_adress;
    return true;
  }
  else
  { // wanted position exceeds memory -> set range error
    getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::Eeprom );
    return false;
  }
};


bool
EepromIo_c::eofp(uint16_t aui16_lookahead, bool ab_setState)
{ // compare (write position + lookahead) with size of EEPROM memory
  if ((mui16_wPosition + aui16_lookahead) >= eepromSize())
  { // position of write access + lookahead exceeds EEPROM memory
    if (ab_setState) getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::Eeprom );
    return true;  // means: End of EEPROM memory is reached
  }
  else
  {
    return false;  // means: End of EEPROM memory is NOT reached
  }
};


bool
EepromIo_c::eofg(uint16_t aui16_lookahead, bool ab_setState)
{ // compare (read position + lookahead) with size of EEPROM memory
  if ((mui16_rPosition + aui16_lookahead) >= eepromSize())
  { // position of read access + lookahead exceeds EEPROM memory
    if (ab_setState) getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::Eeprom );
    return true; // means: End of EEPROM memory is reached
  }
  else
  {
    return false; // means: End of EEPROM memory is NOT reached
  }
};


EepromIo_c&
EepromIo_c::writeString(const uint8_t *const apb_string, uint16_t aui16_number)
{ // check if enough space for string is after actual write position
  if (aui16_number > 0)
  { // second parameter true -> set Err_c::range if end is reached
    if (!eofp(aui16_number-1, true))
    { // use private write function to read in correct number of bytes into data string
      write (mui16_wPosition, aui16_number, apb_string);
      mui16_wPosition += (aui16_number); //increment position
    }
  }
  return *this;
};


bool
EepromIo_c::readString(uint8_t *const apb_string, uint16_t aui16_number)
{ // check if enough space for string is after actual read position
  if (aui16_number == 0)
  { // someone wants to read 0 bytes. Well, that's fine, just do nothing and return with success
    return true;
  }

  // second parameter true -> set Err_c::range if end is reached
  if (!eofg(aui16_number-1), true)
  { // enough space in EEPROM from actual position on
    // call BIOS function to check that EEPROM is ready
    setState4BiosReturn(wait_eepromReady());
    // use eepromRead BIOS function to read in correct number of bytes into data string
    // don't update read position
    int16_t i16_retVal = HAL::eepromRead(mui16_rPosition, aui16_number, apb_string);
    setState4BiosReturn(i16_retVal); // update state dependent on return of BIOS function
    // increment position on success
    if (i16_retVal == HAL_NO_ERR)
    {
      mui16_rPosition += aui16_number;
      return true;
    }
    else return false;
  }
  else
  { // report that read access had no success because of range error
    return false;
  }
}


bool
EepromIo_c::write(uint16_t aui16_adress, uint16_t aui16_number, const uint8_t* apb_data){
  uint16_t ui16_restNumber = aui16_number,
       ui16_actualStart = aui16_adress,
       ui16_actualSize;
  const uint8_t* pb_data = apb_data;
  uint8_t pb_compare[MAX_EEPROM_SEGMENT_SIZE];

  /* check input data */
  if ((aui16_adress + aui16_number) > eepromSize())
  { // range error because wanted write operation exceeds EEPROM memory
    getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::Eeprom );
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
        if (memcmp(pb_compare, pb_data,ui16_actualSize) != 0)
        { // old data is different -> write new data
          if (writeInit())
          { // call BIOS function
            setState4BiosReturn(HAL::eepromWrite(ui16_actualStart, ui16_actualSize, pb_data));
          }
        }
        else
        { // re-read of value delivers same value -> break try loop
          break;
        }
      }
      if ( ui8_tryCnt == MAX_EEPROM_WRITE_TRY_CYCLE_CNT )
      { // write without success, as re-read delivers always different value
        getILibErrInstance().registerError( iLibErr_c::EepromWriteError, iLibErr_c::Eeprom );
        // don't try further writes
        break;
      }
      else if (getILibErrInstance().good( iLibErr_c::Eeprom  ))
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
  return (getILibErrInstance().good( iLibErr_c::Eeprom ));
}


bool
EepromIo_c::writeInit()
{
  // clear the BIOS state
  getILibErrInstance().clear( iLibErr_c::Eeprom );

  // check if eeprom is ready; call BIOS function
  if ( wait_eepromReady() != EE_READY )
  { // error -> got not ready
    getILibErrInstance().registerError( iLibErr_c::Busy, iLibErr_c::Eeprom );
    return false;
  }

  // set EEPROM to writable
  setState4BiosReturn(HAL::eepromWp(false));

  if (getILibErrInstance().good( iLibErr_c::Eeprom ))
  { // only try writing if write protect was succesful deactivated
    // call wait_eepromReady() to test if EEPROM is ready
    setState4BiosReturn(wait_eepromReady());
  }
  // return if EEPROM is in good state with writeable and ready state
  return (getILibErrInstance().good( iLibErr_c::Eeprom ));
}


int16_t
EepromIo_c::wait_eepromReady( void )
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


uint16_t
EepromIo_c::maxSize(uint16_t aui16_adress)
{
  uint16_t ui16_segmentEnd = ((aui16_adress/mui16_segmentSize + 1)*mui16_segmentSize) -1;
  return ((ui16_segmentEnd - aui16_adress)+1);
}


void
EepromIo_c::setState4BiosReturn(int16_t ai16_biosReturn)
{
  switch (ai16_biosReturn)
  {
    case HAL_NO_ERR:
      break;
    case HAL_RANGE_ERR:
      getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::Eeprom );
      break;
    case HAL_BUSY_ERR:
      getILibErrInstance().registerError( iLibErr_c::Busy, iLibErr_c::Eeprom );
      break;
    case HAL_OVERFLOW_ERR:
      getILibErrInstance().registerError( iLibErr_c::EepromSegment, iLibErr_c::Eeprom );
      break;
    case HAL_NOACT_ERR:
    default:
      getILibErrInstance().registerError( iLibErr_c::Unspecified, iLibErr_c::Eeprom );
      break;
  }
};


EepromIo_c&
EepromIo_c::writeIntern(const uint8_t* apb_data, uint8_t aui8_len)
{ // check if enough space for type T is after actual write position
  // second parameter true -> set Err_c::range if end is reached
  if (!eofp(aui8_len, true))
  { // use private write function to read in correct number of bytes into data string
    write (mui16_wPosition, aui8_len, apb_data);
    mui16_wPosition += aui8_len; //inkrement position
  }
  return *this;
}


EepromIo_c&
EepromIo_c::readIntern(uint8_t* apb_data, uint8_t aui8_len) {
  // check if enough space for string is after actual read position
  // second parameter true -> set Err_c::range if end is reached
  if (!eofg(aui8_len, true))
  { // use private eepromRead function to read in correct number of bytes into data string
    // call BIOS function to check that EEPROM is ready
    setState4BiosReturn(wait_eepromReady());
    HAL::eepromRead(mui16_rPosition, aui8_len, apb_data);
    mui16_rPosition += aui8_len; //inkrement position
  }
  return *this;
}


EepromIo_c&
operator<<(EepromIo_c& rc_stream, const IsoName_c& rc_data )
{
  rc_stream.writeIntern(rc_data.outputString(), 8);
  return rc_stream;
}


EepromIo_c&
operator>>(EepromIo_c& rc_stream, IsoName_c& rc_data )
{
  uint8_t tempName[8];
  rc_stream.readIntern(tempName, 8);
  rc_data.inputString( tempName );
  return rc_stream;
}


} // end of namespace __IsoAgLib
