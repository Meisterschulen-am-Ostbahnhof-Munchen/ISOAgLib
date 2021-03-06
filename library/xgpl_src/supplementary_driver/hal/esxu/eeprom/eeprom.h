/*
  eeprom.h: definition of HAL namesapce layer for ESXu

  (C) Copyright 2009 - 2019 by OSB AG

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Usage under Commercial License:
  Licensees with a valid commercial license may use this file
  according to their commercial license agreement. (To obtain a
  commercial license contact OSB AG via <http://isoaglib.com/en/contact>)

  Usage under GNU General Public License with exceptions for ISOAgLib:
  Alternatively (if not holding a valid commercial license)
  use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

/** \file IsoAgLib/hal/esxu/eeprom/eeprom.h
 * The header <i>\<target\>/\<device\>/\<device\>.h</i> performs a name
   mapping between platform specific BIOS / OS function names
   and the function names, the IsoAgLib uses for hardware access.
   In this header only the groups for error codes, global system
   functions and CAN must be adapted to a new platform.
   If a simple name mapping is not possible for a specific
   function, the module targetExtensions can be used to implement
   the needed activity. The implementation of the PC version
   can be used to derive the meaning of the specific function.
*/
/* ************************************************************ */

#ifndef _HAL_ESXu_EEPROM_H_
#define _HAL_ESXu_EEPROM_H_

namespace __HAL {
  extern "C" {
    /** include the BIOS specific header into __HAL */
    #include <commercial_BIOS/bios_esxu/mos10osy.h>
  }
}

#include <IsoAgLib/isoaglib_config.h>
#include <IsoAgLib/hal/esxu/typedef.h>
#include <IsoAgLib/hal/esxu/errcodes.h>

/**
   namespace with layer of inline (cost NO overhead -> compiler replaces
   inline function with call to orig BIOS function)
   functions between all IsoAgLib calls for BIOS and the corresponding BIOS functions
   --> simply replace the call to the corresponding BIOS function in this header
       for adaptation to new platform
 */
namespace HAL
{
  /* *************************** */
  /** \name EEPROM BIOS functions */
/*@{*/

  /**
   deliver the EEPROM size in uint8_t -> mult ESXu BIOS value with 1024
   @return EEPROM size in byte
  */
  inline uint32_t getEepromSize(void)
    {return (1024 * __HAL::get_eeprom_size());};

  /**
   deliver the EEPROM segment size in kbyte
   @return EEPROM segment size in kbyte
  */
  // MSCHMIDT - I think this is actually supposed to say size in bytes instead
  // MSCHMIDT - of size in kbyte.  get_eeprom_segment_size() returns size in
  // MSCHMIDT - bytes.
  // ESXu has no segment size, only get_eeprom_size() function.  So, I guess that means
  // there are no segments for this BIOS.  Set segment size to 32 like was done for the PC.
  inline uint16_t getEepromSegmentSize(void)
    { return 32; };

  /**
    read amount of uint8_t in uint8_t string variable
    @param wAddress start adress to read
    @param wNumber amount of bytes to read
    @param pbByte pointer to uint8_t string, where data is inserted
    @return error state (C_NO_ERR == o.k.)
  */
  inline int16_t eepromRead(uint16_t wAddress,uint16_t wNumber,uint8_t *pbByte)
    {return __HAL::eeprom_read(wAddress, wNumber, pbByte);};

  /**
    write amount of uint8_t from string into EEPROM from given start adress
    @param wAddress adress of first written uint8_t in EEPROM
    @param wNumber amount of written data
    @param pbData pointer to uint8_t string which should be written into EEPROM
    @return error state (C_NO_ERR == o.k.)
  */
  // MSCHMIDT - ESXu has a 4th parameter boolean bitCheckEnable
  // MSCHMIDT - This does a compare after the write to make sure it was written correctly.
  // MSCHMIDT - I'm currently setting this to FALSE.  I should probably do something different.
  inline int16_t eepromWrite(uint16_t wAddress,uint16_t wNumber,const uint8_t *pbData)
    {return __HAL::eeprom_write(wAddress, wNumber, const_cast<uint8_t *>(pbData));};

  /**
    set or unset set write protection of EEPROM, should be called before
    every write access
    @param bitMode sets write protection OFF or ON
    @return error state (C_NO_ERR == o.k.)
  */
  inline int16_t eepromWp(bool bitMode)
    {return HAL_NO_ERR;};

  /**
    check if EEPROM is ready for actions
    @return HAL_NO_ERR -> ready
  */
  inline int16_t eepromReady(void)
  {return HAL_NO_ERR; };
}
#endif
