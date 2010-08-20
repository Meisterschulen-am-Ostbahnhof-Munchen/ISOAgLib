/*
  eeprom.h: definition of HAL namesapce layer for DjBios1

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

/** \file IsoAgLib/hal/Dj1/eeprom/eeprom.cpp
 * The header <i>\<target\>/\<device\>/\<device\>.h</i> performs a name
   mapping between platform specific BIOS / OS function names
   and the function names, the IsoAgLib uses for hardware access.
   In this header only the groups for error codes, global system
   functions and CAN must be adopted to a new platform.
   If a simple name mapping is not possible for a specific
   function, the module targetExtensions can be used to implement
   the needed activity. The implementation of the PC version
   can be used to derive the meaning of the specific function.
*/
/* ************************************************************ */

namespace __HAL {
  extern "C" {
    /** include the BIOS specific header into __HAL */
#include <commercial_BIOS/bios_Dj1/DjBios1.h>
  }
}

#ifndef __EEPROM_DJ1_H__
#define __EEPROM_DJ1_H__

/**
   namespace with layer of (cost NO overhead -> compiler replaces
   function with call to orig BIOS function)
   functions between all IsoAgLib calls for BIOS and the corresponding BIOS functions
   --> simply replace the call to the corresponding BIOS function in this header
       for adaption to new platform
 */
namespace HAL {
  /* *************************** */
  /** \name EEPROM BIOS functions */

  /**
   deliver the EEPROM size
     @return EEPROM size in bytes
  */
  inline int16_t getEepromSize( void ) {
    return ( __HAL::DjBios_EepromGetSize() );
  };


  /**
   deliver the EEPROM segment size in kbyte
     @return EEPROM segment size in kbyte
  */
  // MSCHMIDT - I think this is actually supposed to say size in bytes instead
  // MSCHMIDT - of size in kbyte.  get_eeprom_segment_size() returns size in
  // MSCHMIDT - bytes.
  inline int16_t getEepromSegmentSize( void ) {
    return ( __HAL::DjBios_EepromGetSegmentSize() );
  };


  /**
    read amount of uint8_t in uint8_t string variable
      @param wAddress start adress to read
      @param wNumber amount of bytes to read
      @param pbByte pointer to uint8_t string, where data is inserted
      @return error state (HAL_NO_ERR == o.k.)
                           HAL_RANGE_ERR == Bad address or number of bytes
                           HAL_BUSY_ERR == Write in progress
  */
  inline int16_t eepromRead( uint16_t wAddress, uint16_t wNumber, uint8_t *pbByte ) {
    int16_t RetStatus;

    switch ( __HAL::DjBios_EepromRead( wAddress, wNumber, pbByte ) ) {
      case __HAL::BIOS_EE_NO_ERR: {
          RetStatus = HAL_NO_ERR;
          break;
        }
      case __HAL::BIOS_EE_BUSY: {
          RetStatus = HAL_BUSY_ERR;
          break;
        }
      case __HAL::BIOS_EE_BAD_ADDR: {
          RetStatus = HAL_RANGE_ERR;
          break;
        }
      default: {
          RetStatus = HAL_UNKNOWN_ERR;
          break;
        }
    } /* end switch() */

    return ( RetStatus );
  };


  /**
    write amount of uint8_t from string into EEPROM from given start adress
      @param wAddress adress of first written uint8_t in EEPROM
      @param wNumber amount of written data
      @param pbData pointer to uint8_t string which should be written into EEPROM
      @return error state (HAL_NO_ERR == o.k.)
  */
  inline int16_t eepromWrite( uint16_t wAddress, uint16_t wNumber, const uint8_t *pbData ) {
    int16_t RetStatus;

    switch ( __HAL::DjBios_EepromWriteDirect( wAddress, wNumber, pbData ) ) {
      case __HAL::BIOS_EE_NO_ERR: {
          RetStatus = HAL_NO_ERR;
          break;
        }
      case __HAL::BIOS_EE_BUSY: {
          RetStatus = HAL_BUSY_ERR;
          break;
        }
      case __HAL::BIOS_EE_BAD_ADDR: {
          RetStatus = HAL_RANGE_ERR;
          break;
        }
      default: {
          RetStatus = HAL_UNKNOWN_ERR;
          break;
        }
    } /* end switch() */

    return ( RetStatus );
  };


  /**
    set or unset set write protection of EEPROM, should be called before
    every write access
    @param bitMode sets write protection OFF or ON
    @return error state (C_NO_ERR == o.k.)
  */
  inline int16_t eepromWp( bool bitMode ) {
    /* Write Protect is not used for this hardware platform */
    return HAL_NO_ERR;
  };


  /**
    check if EEPROM is ready for actions
    @return EE_READY -> ready
  */
  inline int16_t eepromReady( void ) {
    return (( __HAL::DjBios_EepromStatus() == __HAL::BIOS_EE_NO_ERR ) ? EE_READY : HAL_BUSY_ERR );
  };


#ifdef USE_CAN_EEPROM_EDITOR
  /* ********************************* */
  /** \name CAN EEPROM Editor functions */

  /**
    initialize the CAN EEPROM editor module
      @param bBus CAN BUS number to use for EEEditor
      @param iObjNrReceiveCan CAN Msg Obj number for receive of Editor msg
      @param iObjNrTransmitCan CAN Msg Obj number for send of Editor msg
      @param dwReceiveCanId CAN Ident_c to use for receive of Editor msg
      @param bUseExtendedCAN set to 0 for standard 11bit, to 1 for extended 29bit Ident
      @param iNumberMsgsReceive size of CAN receive buffer size
      @param iNumberMsgsTransmit size of CAN send buffer size
      @return HAL_NO_ERR if no error occured
  */
  inline int16_t InitEEEditor( uint8_t bBus,
                               int16_t iObjNrReceiveCan, int16_t iObjNrTransmitCan,
                               uint32_t dwReceiveCanId, uint8_t bUseExtendedCAN,
                               int16_t iNumberMsgsReceive, int16_t iNumberMsgsTransmit ) {
    return __HAL::iInitEEEditor( bBus, iObjNrReceiveCan, iObjNrTransmitCan,
                                 dwReceiveCanId, bUseExtendedCAN,
                                 iNumberMsgsReceive, iNumberMsgsTransmit );
  };


  /**
    periodic call to the CAN EEEditor, to process editor msg
    @return true -> EEPROM write msg recieved, and EEPROM values changed
  */
  inline int16_t ProcessCANEEEditorMsg() {
    return __HAL::iCallCanEEMonitor();
  };

#elif defined(USE_RS232_EEPROM_EDITOR)
  /* *********************************** */
  /** \name RS232 EEPROM Editor functions */

  /**
    periodic call to the RS232 EEEditor, to process editor msg
    @return true -> EEPROM write msg recieved, and EEPROM values changed
  */
  inline int16_t ProcessRS232EEEditorMsg() {
    return __HAL::iCallRs232EEMonitor();
  };
#endif

} /* namespace HAL */

#endif // #define __EEPROM_DJ1_H__
