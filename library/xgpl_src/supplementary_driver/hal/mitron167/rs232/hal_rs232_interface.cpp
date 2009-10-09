/***************************************************************************
                          hal_rs232_interface.cpp - implementation of Mitron167
                                                  RS232 interface
                             -------------------
    begin                : Wed Mar 15 2000
    copyright            : (C) 2000 - 2009 Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
    type                 : Header
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
 * Copyright (C) 1999 - 2009 Dipl.-Inform. Achim Spangler                  *
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

/* ************************************************************ */
/** \file 
 * 
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

#include <IsoAgLib/hal/mitron167/config.h>
#include <IsoAgLib/hal/mitron167/typedef.h>
#include <IsoAgLib/hal/mitron167/errcodes.h>
#include "hal_rs232_interface.h"

namespace __HAL {
extern "C" {
  /** include the BIOS specific header with the part for RS232 into __HAL */
  #include <commercial_BIOS/bios_mitron167/Rs232Driver/ser_lib.h>
  #include <C166.h>
  #include <reg167cs.h>
  #include <cstring>


static const uchar* puc_startBuffer =    __HAL::serial.rx.ascii.data;
static const uchar* puc_endBuffer   =  &(__HAL::serial.rx.ascii.data[SD_MAX_ASCII_CHARS]);

static uchar* puc_rs232BufferWrite = (uchar*) puc_startBuffer;
static uchar* puc_rs232BufferRead  = (uchar*) puc_startBuffer;

static bool b_isChannelOpen = false;

/** low level helper function to check if buffer is full */
bool stackparm isRs232RingBufferFull( void ) {
  return !( __HAL::serial.rx.ascii.count < SD_MAX_ASCII_CHARS );
}
/** low level helper function to check if buffer is empty */
bool isRs232RingBufferEmpty( void ) {
  return ( __HAL::serial.rx.ascii.count == 0 );
}
/** low level helper function to get current count of elements in buffer */
uint16_t getRs232RingBufferSize( void ) {
  return __HAL::serial.rx.ascii.count;
}
/** low level helper function to get current count of elements in buffer */
uint16_t getRs232RingBufferMaxSize( void ) {
  return SD_MAX_ASCII_CHARS;
}

/** low level helper function to write can data */
bool stackparm writeDataToRs232RingBuffer( uchar ruc_item, bool ab_force = false ) {
  // first check if buffer is already full and no overwrite is forced
  if ( ( isRs232RingBufferFull() ) && ( ! ab_force ) ) return false;

  // make some minimum validation checks
  if ( ( puc_rs232BufferWrite < puc_startBuffer ) || ( puc_rs232BufferWrite > puc_endBuffer ) )
  { // pointer is wrong -> reset
    clearRs232RxBuffer();
  }
  // shield from interruption
  _atomic( 3 );
  // now - even if buffer integrity was corrupted - everything is valid again
  *puc_rs232BufferWrite++ = ruc_item;
  // puc_rs232BufferWrite++;
  // increase size to avoid problems with combined access
  __HAL::serial.rx.ascii.count++;

  // check if roundup is needed
  if ( puc_rs232BufferWrite > puc_endBuffer ) puc_rs232BufferWrite = (uchar*) puc_startBuffer;

  // check if buffer is overflown
  if ( __HAL::serial.rx.ascii.count > SD_MAX_ASCII_CHARS )
  { // to much in buffer -> change read pointer
    // shield from interruption
    _atomic( 3 );
    puc_rs232BufferRead++;
    __HAL::serial.rx.ascii.count--;
  }

  // now return true as sign for successful insert
  return true;
}
/** low level helper function to read can data */
bool readDataFromRs232RingBuffer( uchar* puc_item ) {
  // first check if buffer is empty
  if ( isRs232RingBufferEmpty() ) return false;

  // make some minimum validation checks
  if ( ( puc_rs232BufferRead < puc_startBuffer ) || ( puc_rs232BufferRead > puc_endBuffer ) )
  { // pointer is wrong -> reset
    clearRs232RxBuffer();
    // buffer now empty -> nothing to read
    *puc_item = '\0';
    return false;
  }

  // shield from interruption
  _atomic( 4 );
  // buffer structure is valid
  *puc_item = *puc_rs232BufferRead++;
  // puc_rs232BufferRead++;
  // decrease size to avoid problems with combined access
  __HAL::serial.rx.ascii.count--;

  // check if roundup is needed
  if ( puc_rs232BufferRead > puc_endBuffer ) puc_rs232BufferRead = (uchar*) puc_startBuffer;

  // item inserted in pointer-parameter -> successful
  return true;
}

} // end extern "C"


  /**
    init the RS232 interface
    @param wBaudrate wnated Baudrate {75, 600, 1200, 2400, 4800, 9600, 19200}
          as configured in <IsoAgLib/isoaglib_config.h>
    @param bMode one of (DATA_7_BITS_EVENPARITY = 1, DATA_8_BITS_EVENPARITY = 2,
            DATA_7_BITS_ODDPARITY = 3, DATA_8_BITS_ODDPARITY = 4, DATA_8_BITS_NOPARITY = 5)
    @param bStoppbits amount of stop bits (1,2)
    @param bitSoftwarehandshake true -> use xon/xoff software handshake
    @return HAL_NO_ERR -> o.k. else one of settings incorrect
  */
  int16_t init_rs232(uint16_t wBaudrate,uint8_t bMode,uint8_t bStoppbits,bool bitSoftwarehandshake) {
    puc_rs232BufferWrite = puc_rs232BufferRead = (uchar*) puc_startBuffer;
    __HAL::serial.rx.ascii.count = 0;
    // derive preload value for baudrate
    uint16_t ui16_preLoad = 0x14; // 19200 Baud
    switch ( wBaudrate ) {
      case 19200: ui16_preLoad = 0x20;   break;
      case  9600: ui16_preLoad = 0x41;   break;
      case  4800: ui16_preLoad = 0x82;   break;
      case  2400: ui16_preLoad = 0x104;  break;
      case  1200: ui16_preLoad = 0x208;  break;
      case   600: ui16_preLoad = 0x411;  break;
      case    75: ui16_preLoad = 0x1FFF; break;
      default: return HAL_CONFIG_ERR; // wrong setting
    }

    // derive register setting for data mode
    uint16_t ui16_ctlReg = 0x8010; // base setting for: ReceiverEnableBit, BaudrateRunGeneratorBit
    if ( bStoppbits == 2 ) ui16_ctlReg |= 0x8; // Bit S0STP 0==1StopBit, 1==2StopBit
    switch ( bMode ) {
      case 1: // DATA_7_BITS_EVENPARITY
        ui16_ctlReg |= 0x23; // 0x3 == 7Bit+Par, 0x20 == ParityCheckEnableBit
        break;
      case 2: // DATA_8_BITS_EVENPARITY
        ui16_ctlReg |= 0x27; // 0x7 == 8Bit+Par, 0x20 == ParityCheckEnableBit
        break;
      case 3: // DATA_7_BITS_ODDPARITY
        ui16_ctlReg |= 0x1023; // 0x3 == 7Bit+Par, 0x20 == ParityCheckEnableBit, 0x1000 == OddParity
        break;
      case 4: // DATA_8_BITS_ODDPARITY
        ui16_ctlReg |= 0x1027; // 0x7 == 8Bit+Par, 0x20 == ParityCheckEnableBit, 0x1000 == OddParity
        break;
      case 5: // DATA_8_BITS_NOPARITY
        ui16_ctlReg |= 0x1; // 0x1 == 8Bit+NoPar
        break;
      default: return HAL_CONFIG_ERR; // wrong setting
    }
    IEN = 0;      // defined in reg167cs.h
    // call mitron RS232 Init function
    // serial_port_initialization(ui16_preLoad, TX_INT_LEVEL, RX_INT_LEVEL);
	// don't use send IRQ till Mitron ser_lib can handle binary protocol
	#if 1
    serial_port_initialization(ui16_preLoad, TX_INT_LEVEL, RX_INT_LEVEL);
	#else
    serial_port_initialization(ui16_preLoad, 0, RX_INT_LEVEL);
	// manually deactivate Tx IRQ
	S0TIC = 0;
	// set initial send-ready-state
	S0TIR = 1;
	#endif
    // additionally set control register, if other mode than 8N1 is wanted
    if ( ( bMode != 5 ) || ( bStoppbits != 1 ) ) S0CON = ui16_ctlReg;
    IEN = 1;      // defined in reg167cs.h
    // answer NoErrCondition
		b_isChannelOpen = true;
    return HAL_NO_ERR;
  }
	/** close the RS232 interface. */
	int16_t close_rs232()
	{
		b_isChannelOpen = false;
	}

  /**
    set the RS232 Baudrate
    @param wBaudrate wanted baudrate
    @return HAL_NO_ERR -> o.k. else baudrate setting incorrect
  */
  int16_t setRs232Baudrate(uint16_t wBaudrate) {
    // change Baudrate
    uint16_t ui16_preLoad = 0x14; // 19200 Baud
    switch ( wBaudrate ) {
      case 19200: ui16_preLoad = 0x20;   break;
      case  9600: ui16_preLoad = 0x41;   break;
      case  4800: ui16_preLoad = 0x82;   break;
      case  2400: ui16_preLoad = 0x104;  break;
      case  1200: ui16_preLoad = 0x208;  break;
      case   600: ui16_preLoad = 0x411;  break;
      case    75: ui16_preLoad = 0x1FFF; break;
      default: return HAL_CONFIG_ERR; // wrong setting
    }
    // use low-level register access, to avoid change of other settings
    S0BG = ui16_preLoad;
    // answer NoErrCondition
    return HAL_NO_ERR;
  }
  /**
    get the amount of data [uint8_t] in receive buffer
    @return receive buffer data byte
  */
  int16_t getRs232RxBufCount(void) {
    return getRs232RingBufferSize();
  }
  /**
    get the amount of data [uint8_t] in send buffer
    @return send buffer data byte
  */
  int16_t getRs232TxBufCount(void) {
    // the Mitron RS232 system uses a circular buffer for send -> calculate the size based on its size,push,pop settings
    if ( serial.tx.buffer.size == 0 ) return 0;
    else if ( serial.tx.buffer.push >= serial.tx.buffer.pop ) return ( serial.tx.buffer.push - serial.tx.buffer.pop );
    else return ( ( serial.tx.buffer.size - serial.tx.buffer.pop ) + serial.tx.buffer.push );
  }
  /**
    configure a receive buffer and set optional irq function pointer for receive
    @param wBuffersize wanted buffer size
    @param pFunction pointer to irq function or NULL if not wanted
  */
  int16_t configRs232RxObj(uint16_t wBuffersize,void (*pFunction)(uint8_t _huge *bByte)) {
    // not possible wihtout tricks - Mitron uses constant array for RX buffer
    return HAL_NO_ERR;
  }
    /**
    configure a send buffer and set optional irq function pointer for send
    @param wBuffersize wanted buffer size
    @param funktionAfterTransmit pointer to irq function or NULL if not wanted
    @param funktionBeforTransmit pointer to irq function or NULL if not wanted
  */
  int16_t configRs232TxObj(uint16_t wBuffersize,void (*funktionAfterTransmit)(uint8_t _huge *bByte),
                                  void (*funktionBeforTransmit)(uint8_t _huge *bByte)) {
    // not possible wihtout tricks - Mitron uses constant array for RX buffer
    return HAL_NO_ERR;
  }

 /**
    get errr code of BIOS
    @return 0=parity, 1=stopbit framing error, 2=overflow
  */
  int16_t getRs232Error(uint8_t *Errorcode) {
    // set initial state
    *Errorcode = 0;
    if ( S0PE == 1 ) *Errorcode |= 1; // set Bit0 on Parity Error
    if ( S0FE == 1 ) *Errorcode |= 2; // set Bit1 on Framing Error
    if ( serial.rx.ascii.count == SD_MAX_ASCII_CHARS ) *Errorcode |= 4; // set Bit2 if RX buffer is complete full
    return HAL_NO_ERR;
  }

  /**
    read single int8_t from receive buffer
    @param pbRead pointer to target data
    @return HAL_NO_ERR -> o.k. else buffer underflow
  */
  int16_t getRs232Char(uint8_t *pbRead) {
    // if no successful read is possible - terminating '\0' is inserted at pointer
    if ( readDataFromRs232RingBuffer( pbRead ) )
    { // valid result
      return HAL_NO_ERR;
    }
    else
    { // something went wrong ( empty buffer or some now healed integrity )
      return HAL_RANGE_ERR;
    }
  }
  /**
    read bLastChar terminated string from receive buffer
    @param pbRead pointer to target data
    @param bLastChar terminating char
    @return HAL_NO_ERR -> o.k. else buffer underflow
  */
  int16_t getRs232String(uint8_t *pbRead, uint8_t bLastChar) {
    uchar *pc_write = pbRead;
    while ( readDataFromRs232RingBuffer( pc_write ) )
    { // check for terminating condition and increment pc_write
      // increment in any case ( terminating '\0' must be written after
      // current character even if it was finishing char
      pc_write++;
      if ( *pc_write == bLastChar ) break;
    }

	  // terminate target string in any case with '\0'
	  // ( as write pointer points already to position after last copied element
	  //   the '\0' can be simply writte to current pointed position )
	  *pc_write = '\0';

    return HAL_NO_ERR;
  }

  /**
    send single uint8_t on RS232
    @param bByte data uint8_t to send
    @return HAL_NO_ERR -> o.k. else send buffer overflow
  */
  int16_t put_rs232Char(uint8_t bByte) {
    // direct call Mitron send function
	#if 1
    push_rs232_char( &serial.tx.buffer, bByte );
    flush_rs232_buffer();
	#else
	// use example code from Tasking, as Mitron Lib has problems with
	// binary protocol -> '\0' isn't sent
    while ( ! S0TIR )
	  ;
	S0TIR  = 0;
	S0TBUF = bByte;
	#endif

    return HAL_NO_ERR;
  }
  /**
    send string of n uint8_t on RS232
    @param bpWrite pointer to source data string
    @param wNumber number of data uint8_t to send
    @return HAL_NO_ERR -> o.k. else send buffer overflow
  */
  int16_t put_rs232NChar(const uint8_t *bpWrite, uint16_t wNumber) {
   #if 1
    for ( uint16_t ui16_ind = 0; ( ui16_ind < wNumber ); ui16_ind++ ) {
      // use single push char
      push_rs232_char( &serial.tx.buffer, bpWrite[ui16_ind] );
    }
    flush_rs232_buffer();
    #else
	// as long as Tx IRQ is activated because of Mitron Lib lack of sending '\0'
	// use series of single send char functions
	for ( uint16_t ui16_ind = 0; ui16_ind < wNumber; ui16_ind ) put_rs232Char( bpWrite[ui16_ind] );
    #endif
    return HAL_NO_ERR;
  }
  /**
    send '\\0' terminated string on RS232
    @param pbString pointer to '\\0' terminated (!) source data string
    @return HAL_NO_ERR -> o.k. else send buffer overflow
  */
  int16_t put_rs232String(const uint8_t *pbString) {
    // Mitron provides function for this task
	#if 1
    send_string( ( char*) pbString );
    #else
	// as long as Tx IRQ is activated because of Mitron Lib lack of sending '\0'
	// use series of single send char functions
	for ( uint16_t ui16_ind = 0; pbString[ui16_ind] != '\0'; ui16_ind ) put_rs232Char( pbString[ui16_ind] );
    #endif
    return HAL_NO_ERR;
  }

  /**
    clear receive buffer
  */
  void stackparm clearRs232RxBuffer(void) {
    // shield from interruption
    _atomic( 4 );
    puc_rs232BufferWrite = puc_rs232BufferRead = (uchar*) puc_startBuffer;
    __HAL::serial.rx.ascii.count = 0;
    serial.rx.ascii.data[0] = '\0';
  }
  /**
    clear send buffer
  */
  void clearRs232TxBuffer(void) {
    IEN = 0;      // deactivate IRQs during move of rest of buffer content
    // pop char as long as pop returns "something was popped"
    uchar c_temp;
    while ( pop_rs232_char( &serial.tx.buffer, &c_temp ) == 1 );
    IEN = 1;      // activate IRQs again, as buffer action is ready
  }
}
extern "C" {

//******************************************************************************
// this function is application dependent!!!
void handle_character(uchar ch)
{ // insert at end when bus was not closed
	if ( b_isChannelOpen ) writeDataToRs232RingBuffer( ch );
}


//*************************************************************************************************
// This function is for applications
void handle_ascii_string(schar *text)
{ // insert string
	if ( !b_isChannelOpen ) return; // do nothing if BUS was closed
  schar *psc_read = text;
  while ( writeDataToRs232RingBuffer( *psc_read ) )
  { // increment write pointer in any case
    // check if current char was limiting indicator
    if ( *psc_read == '\n' ) break;
    psc_read++;
  }
  // write terminating '\0'
  writeDataToRs232RingBuffer( '\0' );
}


}
