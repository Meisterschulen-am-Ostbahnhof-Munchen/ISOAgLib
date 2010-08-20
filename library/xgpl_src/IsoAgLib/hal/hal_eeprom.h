/*
  hal_eeprom.h: include dependent on used target (defined in
    IsoAgLib/isoaglib_config.h) the suitable HAL specific header for
    EEPROM data storage

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

/* ************************************************************ */
/** \file IsoAgLib/hal/hal_eeprom.h
  * include dependent on used target (defined in
    IsoAgLib/isoaglib_config.h) the suitable HAL
    specific header for EEPROM data storage.
*/
/* ************************************************************ */
#ifndef _HAL_INDEPENDENT_EEPROM_H_
#define _HAL_INDEPENDENT_EEPROM_H_

#include <IsoAgLib/isoaglib_config.h>

#ifdef HAL_PATH_ISOAGLIB
#define _hal_config_header_ <HAL_PATH_ISOAGLIB/config.h>
#define _hal_typedef_header_ <HAL_PATH_ISOAGLIB/typedef.h>
#define _hal_errorcodes_header_ <HAL_PATH_ISOAGLIB/errcodes.h>

#include _hal_config_header_
#include _hal_typedef_header_
#include _hal_errorcodes_header_


namespace HAL {
  /* *************************** */
  /** \name EEPROM BIOS functions */
  /*@{*/

  /**
   deliver the EEPROM size in uint8_t
   @return EEPROM size in byte
  */
  int16_t getEepromSize( void );

  /**
   deliver the EEPROM segment size in kbyte
   @return EEPROM segment size in kbyte
  */
  int16_t getEepromSegmentSize( void );

  /**
    read amount of uint8_t in uint8_t string variable
    @param wAddress start adress to read
    @param wNumber amount of bytes to read
    @param pbByte pointer to uint8_t string, where data is inserted
    @return error state (C_NO_ERR == o.k.)
  */
  int16_t eepromRead( uint16_t wAddress,uint16_t wNumber,uint8_t *pbByte );

  /**
    write amount of uint8_t from string into EEPROM from given start adress
    @param wAddress adress of first written uint8_t in EEPROM
    @param wNumber amount of written data
    @param pbData pointer to uint8_t string which should be written into EEPROM
    @return error state (C_NO_ERR == o.k.)
  */
  int16_t eepromWrite( uint16_t wAddress,uint16_t wNumber,const uint8_t *pbData );

  /**
    set or unset set write protection of EEPROM, should be called before
    every write access
    @param bitMode sets write protection OFF or ON
    @return error state (C_NO_ERR == o.k.)
  */
  int16_t eepromWp( bool bitMode );

  /**
    check if EEPROM is ready for actions
    @return EE_READY -> ready
  */
  int16_t eepromReady( void );

  /*@}*/

#ifdef USE_CAN_EEPROM_EDITOR
  /* ********************************* */
  /** \name CAN EEPROM Editor functions */
  /*@{*/

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
  int16_t InitEEEditor( uint8_t bBus,
                        int16_t iObjNrReceiveCan, int16_t iObjNrTransmitCan,
                        uint32_t dwReceiveCanId, uint8_t bUseExtendedCAN,
                        int16_t iNumberMsgsReceive, int16_t iNumberMsgsTransmit );

  /**
    periodic call to the CAN EEEditor, to process editor msg
    @return true -> EEPROM write msg recieved, and EEPROM values changed
  */
  int16_t ProcessCANEEEditorMsg();

  /*@}*/
#elif defined(USE_RS232_EEPROM_EDITOR)
  /* *********************************** */
  /** \name RS232 EEPROM Editor functions */
  /*@{*/

  /**
    periodic call to the RS232 EEEditor, to process editor msg
    @return true -> EEPROM write msg recieved, and EEPROM values changed
  */
  int16_t ProcessRS232EEEditorMsg();
  /*@}*/
#endif

}

#  define _hal_eeprom_header_ <HAL_PATH_ISOAGLIB_EEPROM/eeprom.h>
#  include _hal_eeprom_header_

#else
// currently EEPROM is mandatory, but there are platforms without, so don't error out here.
#endif


#endif
