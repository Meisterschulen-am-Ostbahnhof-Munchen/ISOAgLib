/***************************************************************************
                          eeprom.h - definition of HAL
                                     namesapce layer for EES
                             -------------------
    begin                : 04/20/09 11:48:49 AM CEST
    copyright            : (C) 2009 OSB-AG
    email                : m.wodok@osb-ag:de
 ***************************************************************************/

#ifndef _HAL_ESS_EEPROM_H_
#define _HAL_ESS_EEPROM_H_

#include "../config.h"
#include "../typedef.h"
#include "../errcodes.h"


namespace HAL {
  /**
   deliver the EEPROM size in uint8_t -> mult ESX BIOS value with 1024
   @return EEPROM size in byte
  */
  inline int16_t getEepromSize( void ) {
    return 0;
  };

  /**
   deliver the EEPROM segment size in kbyte
   @return EEPROM segment size in kbyte
  */
  inline int16_t getEepromSegmentSize( void ) {
    return 0;
  };

  /**
    read amount of uint8_t in uint8_t string variable
    @param wAddress start adress to read
    @param wNumber amount of bytes to read
    @param pbByte pointer to uint8_t string, where data is inserted
    @return error state (C_NO_ERR == o.k.)
  */
  inline int16_t eepromRead( uint16_t wAddress,uint16_t wNumber,uint8_t *pbByte ) {
    return HAL_UNKNOWN_ERR;
  };

  /**
    write amount of uint8_t from string into EEPROM from given start adress
    @param wAddress adress of first written uint8_t in EEPROM
    @param wNumber amount of written data
    @param pbData pointer to uint8_t string which should be written into EEPROM
    @return error state (C_NO_ERR == o.k.)
  */
  inline int16_t eepromWrite( uint16_t wAddress,uint16_t wNumber,const uint8_t *pbData ) {
    return HAL_UNKNOWN_ERR;
  };

  /**
    set or unset set write protection of EEPROM, should be called before
    every write access
    @param bitMode sets write protection OFF or ON
    @return error state (C_NO_ERR == o.k.)
  */
  inline int16_t eepromWp( bool bitMode ) {
    return HAL_UNKNOWN_ERR;
  };

  /**
    check if EEPROM is ready for actions
    @return EE_READY -> ready
  */
  inline int16_t eepromReady( void ) {
    return HAL_NO_ERR;
  };

#ifdef USE_CAN_EEPROM_EDITOR
  /**
    initialize the CAN EEPROM editor module
    @param bBus CAN BUS number to use for EEEditor
    @param iObjNrReceiveCan CAN Msg Obj number for receive of Editor msg
    @param iObjNrTransmitCan CAN Msg Obj number for send of Editor msg
    @param dwReceiveCanId CAN Ident_c to use for receive of Editor msg
    @param bUseExtendedCAN set to 0 for standard 11bit, to 1 for extended 29bit Ident
    @param iNumberMsgsReceive size of CAN receive puffer size
    @param iNumberMsgsTransmit size of CAN send puffer size
    @return HAL_NO_ERR if no error occured
  */
  inline int16_t InitEEEditor( uint8_t bBus,
                               int16_t iObjNrReceiveCan, int16_t iObjNrTransmitCan,
                               uint32_t dwReceiveCanId, uint8_t bUseExtendedCAN,
                               int16_t iNumberMsgsReceive, int16_t iNumberMsgsTransmit ) {
    return HAL_UNKNOWN_ERR;
  };

  /**
    periodic call to the CAN EEEditor, to process editor msg
    @return true -> EEPROM write msg recieved, and EEPROM values changed
  */
  inline int16_t ProcessCANEEEditorMsg() {
    return false;
  };

#elif defined(USE_RS232_EEPROM_EDITOR)
  /**
  periodic call to the RS232 EEEditor, to process editor msg
  @return true -> EEPROM write msg recieved, and EEPROM values changed
  */
  inline int16_t ProcessRS232EEEditorMsg() {
    return false;
  };
#endif

}
#endif
