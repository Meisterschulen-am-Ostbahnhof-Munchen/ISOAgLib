/***************************************************************************
                          hal_can_interface.cpp - CAN interface between
                                        BIOS/OS and IsoAgLib to concentrate
                                        CAN handling abstraction within
                                        one module
                             -------------------
    begin                : Wed Jun 29 2001
    copyright            : (C) 2001 - 2004 Dipl.-Inform. Achim Spangler
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

/* **********************************************************
 * The module halCanInterface implements a standard
 * interface for the CAN interactions of the IsoAgLib
 * This way the the adoption of the IsoAgLib can be
 * better restricted to the various BIOS specific the
 * directories xgpl_src/IsoAgLib/hal/xx/can/ .
 * ********************************************************** */

#include "../typedef.h"
#include "../config.h"
#include "../errcodes.h"
#include "hal_can_interface.h"
#include "../system/system_target_extensions.h"

#include <IsoAgLib/driver/can/impl/ident_c.h>
#include <IsoAgLib/util/impl/canpkg_c.h>
#include <IsoAgLib/hal/mitron167/system/system.h>

#include <supplementary_driver/hal/rs232.h>


#define TASKING
#define SRVWDT _srvwdt()

#define TRUE 1
#define FALSE 0

extern "C" {
  /** include the BIOS specific header with the part for CAN into __HAL */
  #include <commercial_BIOS/bios_mitron167/CanDriver/CAN.H>
  #include <C166.h>
  #include <reg167cs.h>
  #include "canr_16x.h"							/* CAN register definitions		*/

  /** some prototypes from Mitron CAN example */
  unsigned char CAN1TxReady;
  unsigned char CAN2TxReady;


/* ************************************* */
/* **** Some Modul Global Variables **** */
/* ************************************* */
static uint16_t ui16_lastCanBusState[CAN_BUS_CNT];
static int32_t i32_cinterfBeginBusWarnOff[CAN_BUS_CNT];
static int32_t i32_cinterfBeginBit1err[CAN_BUS_CNT];

/** report if active running IRQ controlled send process is active */
static bool b_runningIrqSendProcess[CAN_BUS_CNT] = { false, false };

static int32_t i32_cinterfLastSuccSend[CAN_BUS_CNT];
static int32_t i32_cinterfLastSuccReceive[CAN_BUS_CNT];

/** array of 100msec. timeslice conters of received and sent msg per BUS [uint8_t] */
static uint16_t gwCinterfBusLoad[CAN_BUS_CNT][10];
/** actual index in gwBusLoad */
static uint8_t gb_cinterfBusLoadSlice[CAN_BUS_CNT];

static uint32_t sui32_lastIdent;

__IsoAgLib::Ident_c c_cinterfIdent;

/** define a struct which allows combination with timestamp */
typedef struct {
  CAN_buf_Seed_def data;
  int32_t          timestamp;
} canBufferElement_t;

typedef canBufferElement_t*  canBuffer_t;

static uint32_t sui32_irqCnt = 0;
static uint32_t sui32_txIrqCnt = 0;
static uint32_t sui32_rxIrqCnt = 0;

/** define one static CAN HW structure obj for direct send */
static CAN_buf_Seed_def ss_tempSendObj;
/** Buffer of CAN messages */
static canBuffer_t arr_canBuffer[2][15] =
{ { (canBuffer_t)NULL, (canBuffer_t)NULL, (canBuffer_t)NULL, (canBuffer_t)NULL, (canBuffer_t)NULL, (canBuffer_t)NULL, (canBuffer_t)NULL,
    (canBuffer_t)NULL, (canBuffer_t)NULL, (canBuffer_t)NULL, (canBuffer_t)NULL, (canBuffer_t)NULL, (canBuffer_t)NULL, (canBuffer_t)NULL, (canBuffer_t)NULL },
 { (canBuffer_t)NULL, (canBuffer_t)NULL, (canBuffer_t)NULL, (canBuffer_t)NULL, (canBuffer_t)NULL, (canBuffer_t)NULL, (canBuffer_t)NULL,
   (canBuffer_t)NULL, (canBuffer_t)NULL, (canBuffer_t)NULL, (canBuffer_t)NULL, (canBuffer_t)NULL, (canBuffer_t)NULL, (canBuffer_t)NULL, (canBuffer_t)NULL } };

/** lock state
	@todo directly use CAN controller register to lock a MsgObj */
static bool b_canBufferLock[2][15] =
{ { false, false, false, false, false, false, false,
   false, false, false, false, false, false, false, false },
  { false, false, false, false, false, false, false,
   false, false, false, false, false, false, false, false } };

/** standardID or extended ID */
static uint8_t ui8_canBufferXtd[2][15] =
{ { 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0 } };

/** direction and open/close state per MSgObj: 0==RX, 1==TX, 0xFF==closed */
static uint8_t ui8_canBufferDirectionState[2][15] =
 { { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
     0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
   { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
     0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF } };

/** allocated size of buffer per MsgObj */
static uint16_t ui16_canBufferMaxSize[2][15] =
{ { 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0 } };
/** amount of currently stored elements */
static uint16_t ui16_canBufferSize[2][15] =
{ { 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0 } };

/** read position in ringbuffer */
static canBufferElement_t* pt_canBufferRead[2][15] =
{ { arr_canBuffer[0][0], arr_canBuffer[0][1], arr_canBuffer[0][2], arr_canBuffer[0][3],
    arr_canBuffer[0][4], arr_canBuffer[0][5], arr_canBuffer[0][6], arr_canBuffer[0][7],
    arr_canBuffer[0][8], arr_canBuffer[0][9], arr_canBuffer[0][10], arr_canBuffer[0][11],
    arr_canBuffer[0][12], arr_canBuffer[0][13], arr_canBuffer[0][14] },
  { arr_canBuffer[1][0], arr_canBuffer[1][1], arr_canBuffer[1][2], arr_canBuffer[1][3],
    arr_canBuffer[1][4], arr_canBuffer[1][5], arr_canBuffer[1][6], arr_canBuffer[1][7],
    arr_canBuffer[1][8], arr_canBuffer[1][9], arr_canBuffer[1][10], arr_canBuffer[1][11],
    arr_canBuffer[1][12], arr_canBuffer[1][13], arr_canBuffer[1][14] } };

/** write position in ringbuffer */
static canBufferElement_t* pt_canBufferWrite[2][15] =
{ { arr_canBuffer[0][0], arr_canBuffer[0][1], arr_canBuffer[0][2], arr_canBuffer[0][3],
    arr_canBuffer[0][4], arr_canBuffer[0][5], arr_canBuffer[0][6], arr_canBuffer[0][7],
    arr_canBuffer[0][8], arr_canBuffer[0][9], arr_canBuffer[0][10], arr_canBuffer[0][11],
    arr_canBuffer[0][12], arr_canBuffer[0][13], arr_canBuffer[0][14] },
  { arr_canBuffer[1][0], arr_canBuffer[1][1], arr_canBuffer[1][2], arr_canBuffer[1][3],
    arr_canBuffer[1][4], arr_canBuffer[1][5], arr_canBuffer[1][6], arr_canBuffer[1][7],
    arr_canBuffer[1][8], arr_canBuffer[1][9], arr_canBuffer[1][10], arr_canBuffer[1][11],
    arr_canBuffer[1][12], arr_canBuffer[1][13], arr_canBuffer[1][14] } };

/** write position in ringbuffer */
static canBufferElement_t* pt_canBufferBegin[2][15] =
{ { arr_canBuffer[0][0], arr_canBuffer[0][1], arr_canBuffer[0][2], arr_canBuffer[0][3],
    arr_canBuffer[0][4], arr_canBuffer[0][5], arr_canBuffer[0][6], arr_canBuffer[0][7],
    arr_canBuffer[0][8], arr_canBuffer[0][9], arr_canBuffer[0][10], arr_canBuffer[0][11],
    arr_canBuffer[0][12], arr_canBuffer[0][13], arr_canBuffer[0][14] },
  { arr_canBuffer[1][0], arr_canBuffer[1][1], arr_canBuffer[1][2], arr_canBuffer[1][3],
    arr_canBuffer[1][4], arr_canBuffer[1][5], arr_canBuffer[1][6], arr_canBuffer[1][7],
    arr_canBuffer[1][8], arr_canBuffer[1][9], arr_canBuffer[1][10], arr_canBuffer[1][11],
    arr_canBuffer[1][12], arr_canBuffer[1][13], arr_canBuffer[1][14] } };

/** write position in ringbuffer */
static canBufferElement_t* pt_canBufferEnd[2][15] =
{ { arr_canBuffer[0][0], arr_canBuffer[0][1], arr_canBuffer[0][2], arr_canBuffer[0][3],
    arr_canBuffer[0][4], arr_canBuffer[0][5], arr_canBuffer[0][6], arr_canBuffer[0][7],
    arr_canBuffer[0][8], arr_canBuffer[0][9], arr_canBuffer[0][10], arr_canBuffer[0][11],
    arr_canBuffer[0][12], arr_canBuffer[0][13], arr_canBuffer[0][14] },
  { arr_canBuffer[1][0], arr_canBuffer[1][1], arr_canBuffer[1][2], arr_canBuffer[1][3],
    arr_canBuffer[1][4], arr_canBuffer[1][5], arr_canBuffer[1][6], arr_canBuffer[1][7],
    arr_canBuffer[1][8], arr_canBuffer[1][9], arr_canBuffer[1][10], arr_canBuffer[1][11],
    arr_canBuffer[1][12], arr_canBuffer[1][13], arr_canBuffer[1][14] } };

/** if Application accesses the buffer -> set LOCK -> if IRQ a READ operation of the
    App on a receive buffer _must_ be retried as long as one process is not interrupted.
    On a send buffer, the sending IRQ can regard the buffer as empty, if
    the lock is set, and the current reported size is < 2 - avoid reading of a not
    aet complete written message */
static bool b_appLock[2][15] =
{ { false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false },
 { false, false, false, false, false, false, false,
   false, false, false, false, false, false, false, false } };

/** store last used Baudrate */
static uint8_t ui8_currentBaudrate[2] = { 0, 0 };

/** low level helper function to check if buffer is full */
bool isCanRingBufferFull( uint8_t rui8_busNr, uint8_t rui8_msgObj ) {
  if ( ( rui8_busNr > 1 ) || ( rui8_msgObj> 14 ) ) return true;
  return ( ui16_canBufferSize[rui8_busNr][rui8_msgObj] < ui16_canBufferMaxSize[rui8_busNr][rui8_msgObj] )?false:true;
}
/** low level helper function to check if buffer is empty */
bool isCanRingBufferEmpty( uint8_t rui8_busNr, uint8_t rui8_msgObj ) {
  if ( ( rui8_busNr > 1 ) || ( rui8_msgObj> 14 ) ) return true;
  return ( ui16_canBufferSize[rui8_busNr][rui8_msgObj] == 0 )?true:false;
}
/** low level helper function to get current count of elements in buffer */
uint16_t getCanRingBufferSize( uint8_t rui8_busNr, uint8_t rui8_msgObj ) {
  if ( ( rui8_busNr > 1 ) || ( rui8_msgObj> 14 ) ) return 0;
  return ui16_canBufferSize[rui8_busNr][rui8_msgObj];
}
/** low level helper function to get current count of elements in buffer */
uint16_t getCanRingBufferMaxSize( uint8_t rui8_busNr, uint8_t rui8_msgObj ) {
  if ( ( rui8_busNr > 1 ) || ( rui8_msgObj> 14 ) ) return 0;
  return ui16_canBufferMaxSize[rui8_busNr][rui8_msgObj];
}
/** low level helper function to reset internal state of buffer:
    - set read and write position to begin of buffer
    - set size to 0
 */
uint16_t resetCanRingBuffer( uint8_t rui8_busNr, uint8_t rui8_msgObj ) {
  if ( ( rui8_busNr > 1 ) || ( rui8_msgObj> 14 ) ) return 0;
  _atomic( 4 );
  // set read and write pointer to first pointed CAN-Obj
  // in selected buffer <rui8_busNr, rui8_msgObj>
  pt_canBufferRead[rui8_busNr][rui8_msgObj]
    = pt_canBufferWrite[rui8_busNr][rui8_msgObj]
    = pt_canBufferBegin[rui8_busNr][rui8_msgObj]
    = &(arr_canBuffer[rui8_busNr][rui8_msgObj][0]);

  _atomic( 4 );
  const uint16_t cui16_lastItem
    = (ui16_canBufferMaxSize[rui8_busNr][rui8_msgObj] - 1);
  pt_canBufferEnd[rui8_busNr][rui8_msgObj]
    = &(arr_canBuffer[rui8_busNr][rui8_msgObj][cui16_lastItem]);

  ui16_canBufferSize[rui8_busNr][rui8_msgObj] = 0;
  return 1;
}

/** low level helper function to write can data */
canBufferElement_t* writeDataToCanRingBuffer( uint8_t rui8_busNr, uint8_t rui8_msgObj, bool rb_force = false ) {
  if ( ( rui8_busNr > 1 ) || ( rui8_msgObj> 14 ) ) return NULL;
  // first check if buffer is already full and no overwrite is forced
  if ( ( isCanRingBufferFull( rui8_busNr, rui8_msgObj ) ) && ( ! rb_force ) ) return NULL;

  // make first validate check
  if ( ( pt_canBufferWrite[rui8_busNr][rui8_msgObj] < pt_canBufferBegin[rui8_busNr][rui8_msgObj] )
    || ( pt_canBufferWrite[rui8_busNr][rui8_msgObj] > pt_canBufferEnd[rui8_busNr][rui8_msgObj]   ) )
  { // integrity of write pointer is corrupted -> reset
    resetCanRingBuffer( rui8_busNr, rui8_msgObj );
  }
  // now either everything was already correct, or a freshly reseted buffer is available
  // it's granted that write pointer does point to CAN-Obj in buffer of selected MsgObj

  _atomic( 4 );
  // first increase size to avoid problems with combined access
  ui16_canBufferSize[rui8_busNr][rui8_msgObj]++;
  // now derive write pointer and increment pointer
  canBufferElement_t* ps_tempWrite = pt_canBufferWrite[rui8_busNr][rui8_msgObj]++;

  // update read pos if this write is forced durign buffer-full condition ( oldest not-yet read msg is overwritten by this call )
  if ( isCanRingBufferFull( rui8_busNr, rui8_msgObj ) ) {
    // use operator++ to shorten time critical increasing access (IRQ)
    _atomic( 4 );
    pt_canBufferRead[rui8_busNr][rui8_msgObj]++;
    if ( pt_canBufferRead[rui8_busNr][rui8_msgObj] == pt_canBufferEnd[rui8_busNr][rui8_msgObj] )
      pt_canBufferRead[rui8_busNr][rui8_msgObj] = pt_canBufferBegin[rui8_busNr][rui8_msgObj];
  }
  // now check if write position must be reset to Begin
  _atomic( 4 );
  if ( pt_canBufferWrite[rui8_busNr][rui8_msgObj] == pt_canBufferEnd[rui8_busNr][rui8_msgObj] )
    pt_canBufferWrite[rui8_busNr][rui8_msgObj] = pt_canBufferBegin[rui8_busNr][rui8_msgObj];

  // now return pointer to CAN buffer element, where new data can be stored
  return ps_tempWrite;
}

/** low level helper function to read can data */
canBufferElement_t* readDataFromCanRingBuffer( uint8_t rui8_busNr, uint8_t rui8_msgObj ) {
  if ( ( rui8_busNr > 1 ) || ( rui8_msgObj> 14 ) ) return NULL;
  // first check if buffer is empty
  if ( isCanRingBufferEmpty( rui8_busNr, rui8_msgObj ) ) return NULL;

  // make first validate check
  if ( ( pt_canBufferRead[rui8_busNr][rui8_msgObj] < pt_canBufferBegin[rui8_busNr][rui8_msgObj] )
    || ( pt_canBufferRead[rui8_busNr][rui8_msgObj] > pt_canBufferEnd[rui8_busNr][rui8_msgObj]   ) )
  { // integrity of write pointer is corrupted -> reset
    resetCanRingBuffer( rui8_busNr, rui8_msgObj );
    // there's nothin to read from fresh reset buffer
    return NULL;
  }
  // now everything was already correct
  // it's granted that read pointer does point to CAN-Obj in buffer of selected MsgObj

  // now derive read pointer
  return pt_canBufferRead[rui8_busNr][rui8_msgObj];
}

/** low level helper function to read can data */
canBufferElement_t* readPopDataFromCanRingBuffer( uint8_t rui8_busNr, uint8_t rui8_msgObj ) {
  if ( ( rui8_busNr > 1 ) || ( rui8_msgObj> 14 ) ) return NULL;
  // first check if buffer is empty
  if ( isCanRingBufferEmpty( rui8_busNr, rui8_msgObj ) ) return NULL;

  // make first validate check
  if ( ( pt_canBufferRead[rui8_busNr][rui8_msgObj] < pt_canBufferBegin[rui8_busNr][rui8_msgObj] )
    || ( pt_canBufferRead[rui8_busNr][rui8_msgObj] > pt_canBufferEnd[rui8_busNr][rui8_msgObj]   ) )
  { // integrity of write pointer is corrupted -> reset
    resetCanRingBuffer( rui8_busNr, rui8_msgObj );
    // there's nothin to read from fresh reset buffer
    return NULL;
  }
  // now everything was already correct
  // it's granted that read pointer does point to CAN-Obj in buffer of selected MsgObj

  _atomic( 4 );
  // first decrease size to avoid problems with combined access
  ui16_canBufferSize[rui8_busNr][rui8_msgObj]--;
  // now derive read pointer and increment pointer after read access
  canBufferElement_t* ps_tempRead = pt_canBufferRead[rui8_busNr][rui8_msgObj]++;

  // now check if read position must be reset to Begin
  _atomic( 4 );
  if ( pt_canBufferRead[rui8_busNr][rui8_msgObj] == pt_canBufferEnd[rui8_busNr][rui8_msgObj] )
    pt_canBufferRead[rui8_busNr][rui8_msgObj] = pt_canBufferBegin[rui8_busNr][rui8_msgObj];
  // now return pointer to CAN buffer element, where new data can be read
  return ps_tempRead;
}

void popDataFromCanRingBuffer( uint8_t rui8_busNr, uint8_t rui8_msgObj ) {
  if ( ( rui8_busNr > 1 ) || ( rui8_msgObj> 14 ) ) return;
  // first check if buffer is empty
  if ( isCanRingBufferEmpty( rui8_busNr, rui8_msgObj ) ) return;

  // make first validate check
  if ( ( pt_canBufferRead[rui8_busNr][rui8_msgObj] < pt_canBufferBegin[rui8_busNr][rui8_msgObj] )
    || ( pt_canBufferRead[rui8_busNr][rui8_msgObj] > pt_canBufferEnd[rui8_busNr][rui8_msgObj]   ) )
  { // integrity of write pointer is corrupted -> reset
    resetCanRingBuffer( rui8_busNr, rui8_msgObj );
    // there's nothin to read from fresh reset buffer
    return;
  }
  // now everything was already correct
  // it's granted that read pointer does point to CAN-Obj in buffer of selected MsgObj

  _atomic( 4 );
  // first decrease size to avoid problems with combined access
  ui16_canBufferSize[rui8_busNr][rui8_msgObj]--;
  // now derive read pointer and increment pointer after read access
  canBufferElement_t* ps_tempRead = pt_canBufferRead[rui8_busNr][rui8_msgObj]++;

  // now check if read position must be reset to Begin
  _atomic( 4 );
  if ( pt_canBufferRead[rui8_busNr][rui8_msgObj] == pt_canBufferEnd[rui8_busNr][rui8_msgObj] )
    pt_canBufferRead[rui8_busNr][rui8_msgObj] = pt_canBufferBegin[rui8_busNr][rui8_msgObj];
}

} // end extern "C"

namespace __HAL {

/* ******************************************************* */
/* ***************** Status Checking ********************* */
/* ******************************************************* */

/* ******************** */
/* ***Global Per BUS*** */
/* ******************** */

/**
  update the CAN BUS state parameters for
  WARN, OFF and Bit1Err
  @param rui8_busNr number of bus to check
*/
void updateCanStateTimestamps(uint8_t rui8_busNr)
{
  if ( rui8_busNr > 1 ) return;
  const uint16_t ui16_canState = ui16_lastCanBusState[rui8_busNr];
  // check if WARN bit is set in CAN control status register
  if ((ui16_canState & CanStateWarnOrOff) != 0)
  { // set error state time interval begin
    if (i32_cinterfBeginBusWarnOff[rui8_busNr] < 0)
      i32_cinterfBeginBusWarnOff[rui8_busNr] = HAL::getTime();
  }
  else
  { // resets error time begin
    i32_cinterfBeginBusWarnOff[rui8_busNr] = -1;
  }
  // check if LEC state reports bit1error
  if ((ui16_canState & CanStateLecErrMask) == CanStateBit1Err)
  {  // set error state time interval begin
    if (i32_cinterfBeginBit1err[rui8_busNr] < 0)
      i32_cinterfBeginBit1err[rui8_busNr] = HAL::getTime();
  }
  else
  {  // resets error time begin
    i32_cinterfBeginBit1err[rui8_busNr] = -1;
  }
}

/**
  test if the CAN BUS is in WARN state
  @param rui8_busNr number of the BUS to check (default 0)
  @return true == CAN BUS is in WARN state, else normal operation
*/
bool can_stateGlobalWarn(uint8_t rui8_busNr)
{
  if ( rui8_busNr > 1 ) return true;
  updateCanStateTimestamps(rui8_busNr);
  // check if WARN bit is set in CAN control status register
  return (ui16_lastCanBusState[rui8_busNr] & CanStateWarn)?true:false;
}

/**
  test if the CAN BUS is in OFF state
  @param rui8_busNr number of the BUS to check (default 0)
  @return true == CAN BUS is in OFF state, else normal operation
*/
bool can_stateGlobalOff(uint8_t rui8_busNr)
{
  if ( rui8_busNr > 1 ) return true;
  updateCanStateTimestamps(rui8_busNr);
  // check if OFF bit is set in CAN control status register
  return (ui16_lastCanBusState[rui8_busNr] & CanStateOff)?true:false;
}

/**
  test if the CAN BUS is in Blocked state, which can be a sign
  for CAN controllers which are configured with different baudrates;
  this is the case if neither succesfull sent nor received msg
  is detcted AND CAN controller is in WARN or OFF state
  (the time since last succ. send/rec and the time of WARN/OFF
   can be defined with CONFIG_CAN_MAX_CAN_ERR_TIME_BEFORE_SLOWERING
   in the application specific config file isoaglib_config
   -> should not be to short to avoid false alarm)
  @param rui8_busNr number of the BUS to check (default 0)
  @return true == CAN BUS is in blocked state, else normal operation
*/
bool can_stateGlobalBlocked(uint8_t rui8_busNr)
{
  if ( rui8_busNr > 1 ) return true;
  bool b_busBlocked = true;
  const int32_t i32_now = HAL::getTime();
  // sett b_busBlocked to false, if sign for
  // correct work was detected
  if ((i32_now - i32_cinterfLastSuccSend[rui8_busNr])
      < CONFIG_CAN_MAX_SEND_WAIT_TIME)
    b_busBlocked = false;
  // check if successful receive was detected
  if ((i32_now - i32_cinterfLastSuccReceive[rui8_busNr])
      < CONFIG_CAN_MAX_SEND_WAIT_TIME)
    b_busBlocked = false;

  // check if WARN or ERR was detected
  updateCanStateTimestamps(rui8_busNr);
  if (
      (i32_cinterfBeginBusWarnOff[rui8_busNr] < 0)
    ||((i32_now - i32_cinterfBeginBusWarnOff[rui8_busNr])
        < CONFIG_CAN_MAX_CAN_ERR_TIME_BEFORE_SLOWERING)
      )
  { // no WARN or OFF state is active for defined time
    b_busBlocked = false;
  }
  if (
      (i32_cinterfBeginBit1err[rui8_busNr] < 0)
    ||((i32_now - i32_cinterfBeginBit1err[rui8_busNr])
        < CONFIG_CAN_MAX_CAN_ERR_TIME_BEFORE_SLOWERING)
      )
  { // no Bit1Error state is active for defined time
    b_busBlocked = false;
  }
  return b_busBlocked;
}

/**
  deliver the baudrate of the CAN BUS in [kbaud]
  @param rui8_busNr number of the BUS to check (default 0)
  @return BUS load of the last second [kbaud]
*/
int32_t can_stateGlobalBusload(uint8_t rui8_busNr)
{
  if ( rui8_busNr > 1 ) return 0;
  int32_t i32_baudrate = 0;
  for (uint8_t ui8_ind = 0; ui8_ind < 10; ui8_ind++)
  {
    i32_baudrate += (gwCinterfBusLoad[rui8_busNr][ui8_ind] * 8);
  }
  return i32_baudrate;
}

/**
  check if send try of this MsgObj caused an Bit1Error
  (occurs if >1 controller try to send msg with same ident)
  ==> ISO 11783 forces stop of retries in this case for
      adress claim
      @param rui8_busNr number of the BUS to check
      @return true -> Bit1Err occured
*/
bool can_stateGlobalBit1err(uint8_t rui8_busNr)
{
  if ( rui8_busNr > 1 ) return true;
  // if bit1err timestamp is -1 no actual Bit1Err
  // check if WARN or ERR was detected
  updateCanStateTimestamps(rui8_busNr);
  return (i32_cinterfBeginBit1err[rui8_busNr] < 0)?false:true;
}

/* ***************************** */
/* ***Specific for one MsgObj*** */
/* ***************************** */

/**
  deliver the timestamp of last successfull CAN send action
  @param rui8_busNr number of the BUS to check  [0..1]
  @param rui8_msgobjNr number of the MsgObj to check [0..13]
  @return timestamp of last successful send
          OR -1 if rui8_msgObjNr corresponds to no valid send obj
*/
int32_t can_stateMsgobjTxok(uint8_t rui8_busNr, uint8_t rui8_msgobjNr)
{
  if ( ( rui8_busNr > 1 ) || ( rui8_msgobjNr> 14 ) ) return 0;
  return i32_cinterfLastSuccSend[rui8_busNr];
}

/**
  check if a send MsgObj can't send msgs from buffer to the
  BUS (detecetd by comparing the inactive time with
  CONFIG_CAN_MAX_SEND_WAIT_TIME (defined in isoaglib_config)
  @param rui8_busNr number of the BUS to check  [0..1]
  @param rui8_msgobjNr number of the MsgObj to check [0..13]
  @return true -> longer than CONFIG_CAN_MAX_SEND_WAIT_TIME no msg sent on BUS
*/
bool can_stateMsgobjSendproblem(uint8_t rui8_busNr, uint8_t rui8_msgobjNr)
{
  if ( ( rui8_busNr > 1 ) || ( rui8_msgobjNr> 14 ) ) return true;
  const int32_t i32_now = HAL::getTime();
  if (((i32_now - can_stateMsgobjTxok(rui8_busNr, rui8_msgobjNr)) > CONFIG_CAN_MAX_SEND_WAIT_TIME)
    && ( can_stateMsgobjBuffercnt(rui8_busNr, rui8_msgobjNr) > 0 ))
    return true;
  else return false;
}

/**
  test if buffer of a MsgObj is full (e.g. no more
  msg can be put into buffer (important for TX objects))
  @param rui8_busNr number of the BUS to check
  @param rui8_msgobjNr number of the MsgObj to check [0..13]
  @return true -> buffer is full -> no further can_send allowed
*/
bool can_stateMsgobjOverflow(uint8_t rui8_busNr, uint8_t rui8_msgobjNr)
{
  if ( ( rui8_busNr > 1 ) || ( rui8_msgobjNr> 14 ) ) return true;
  return ( can_stateMsgobjFreecnt( rui8_busNr, rui8_msgobjNr ) == 0 )?true:false;
}
/**
  deliver amount of messages in buffer
  (interesting for RX objects)
  @param rui8_busNr number of the BUS to check
  @param rui8_msgobjNr number of the MsgObj to check
  @return number of messages in buffer or negative error code:
    HAL_CONFIG_ERR == BUS not initialised
    HAL_RANGE_ERR == wrong BUS or MsgObj number
*/
int16_t can_stateMsgobjBuffercnt(uint8_t rui8_busNr, uint8_t rui8_msgobjNr)
{
  if ( ( rui8_busNr > 1 ) || ( rui8_msgobjNr> 14 ) ) return 0;
  return getCanRingBufferSize( rui8_busNr, rui8_msgobjNr );
}

/**
  deliver amount of messages which can be placed in buffer
  (interesting for TX objects)
  @param rui8_busNr number of the BUS to check
  @param rui8_msgobjNr number of the MsgObj to check
  @return number of messages which can be placed in buffer or negative error code:
    HAL_CONFIG_ERR == BUS not initialised
    HAL_RANGE_ERR == wrong BUS or MsgObj number
*/
int16_t can_stateMsgobjFreecnt(uint8_t rui8_busNr, uint8_t rui8_msgobjNr)
{
  if ( ( rui8_busNr > 1 ) || ( rui8_msgobjNr> 14 ) ) return 0;
  if ( getCanRingBufferMaxSize( rui8_busNr, rui8_msgobjNr ) > getCanRingBufferSize( rui8_busNr, rui8_msgobjNr ) )
    return ( getCanRingBufferMaxSize( rui8_busNr, rui8_msgobjNr ) - getCanRingBufferSize( rui8_busNr, rui8_msgobjNr ) );
  else return 0;
}

/* ***************************************************** */
/* ***************** Configuration ********************* */
/* ***************************************************** */

/* ******************** */
/* ***Global Per BUS*** */
/* ******************** */

/**
  init CAN BUS with given baudrate and masks
  (11bit, 29bit and last msg mask)
  @param rui8_busNr number of the BUS to init (default 0)
  @param rb_baudrate baudrate in [kbaud]
  @param rui16_maskStd 11bit global mask
  @param rui32_maskExt 29bit global mask
  @param rui32_maskLastmsg mask of last CAN msg
  @return HAL_NO_ERR == no error;
          HAL_RANGE_ERR == wrong BUS nr or wrong baudrate;
          HAL_WARN_ERR == BUS previously initialised - no problem if only masks had to be changed
*/
int16_t can_configGlobalInit(uint8_t rui8_busNr, uint16_t rb_baudrate, uint16_t rui16_maskStd, uint32_t rui32_maskExt, uint32_t rui32_maskLastmsg)
{
  if ( rui8_busNr > 1 ) {
    char tempString[80];
    sprintf( tempString, "\n\rVerlasse can_configGlobalInit() wegen RangeErr durch BusNr %hd\n\r", rui8_busNr );
    put_rs232String( (const uint8_t *) tempString );
	  int32_t i32_waitEnd = HAL::getTime() + 1000;
	  while ( i32_waitEnd > HAL::getTime() ) _nop();
    return HAL_RANGE_ERR;
  }
  // init variables
  const int32_t i32_now = HAL::getTime();
  ui8_currentBaudrate[rui8_busNr] =  rb_baudrate;
  i32_cinterfBeginBusWarnOff[rui8_busNr] = -1;
  i32_cinterfBeginBit1err[rui8_busNr] = -1;
  i32_cinterfLastSuccSend[rui8_busNr] = i32_now;
  i32_cinterfLastSuccReceive[rui8_busNr] = i32_now;
  for (uint8_t ui8_ind = 0; ui8_ind < 15; ui8_ind++) {
    ui8_canBufferDirectionState[rui8_busNr][ui8_ind] = 0xFF;
	  ui8_canBufferXtd[rui8_busNr][ui8_ind] = 0;
	  ui8_canBufferDirectionState[rui8_busNr][ui8_ind] = 0xFF;
	  ui16_canBufferMaxSize[rui8_busNr][ui8_ind] = 0;
    resetCanRingBuffer( rui8_busNr, ui8_ind);
	  b_appLock[rui8_busNr][ui8_ind] = false;
  }

  gb_cinterfBusLoadSlice[rui8_busNr] = 0;
  b_runningIrqSendProcess[rui8_busNr] = false;
  CNAMESPACE::memset((gwCinterfBusLoad[rui8_busNr]),0,10);
  #if 0
  char tempString[80];
  sprintf( tempString, "\n\n\rGCo %hd, %hd, 0x%x, 0x%lx\n\r",
           rui8_busNr, rb_baudrate, rui16_maskStd, rui32_maskExt );
  put_rs232String( (const uint8_t *) tempString );
  int32_t i32_waitEnd = HAL::getTime() + 500;
  while ( i32_waitEnd > HAL::getTime() ) tempString[0] = tempString[1] + 1;
  #endif

  // avoid problems with to big values for masks
  #if 0
  const uint16_t cui16_shortMask = ( rui16_maskStd & 0x7FF );
  const uint32_t cui32_longMask = ( rui32_maskExt & 0x1FFFFFFF );
  uint16_t ui16_buildShortMask = ( ( cui16_shortMask & 0x7 ) << 13 ) | ( ( cui16_shortMask >> 3 ) & 0xFF );
  uint32_t ui32_buildLongMaskLower = ( ( cui32_longMask & 0x1F ) << 11 ) | ( ( cui32_longMask >> 5 ) & 0xFF );
  uint32_t ui32_buildLongMaskUpper = ( ( ( cui32_longMask << 3 ) & 0xFF0000UL ) >> 8 ) | ( ( ( cui32_longMask << 3 ) & 0xFF000000UL ) >> 24 );
  const uint16_t cui16_longMaskLower = ( ui32_buildLongMaskLower & 0xFFFF );
  const uint16_t cui16_longMaskUpper = ( ui32_buildLongMaskUpper & 0xFFFF );
	#else
  const uint16_t cui16_shortMask = ( rui16_maskStd & 0x7FF );
  const uint32_t cui32_longMask = ( rui32_maskExt & 0x1FFFFFFF );
  uint16_t ui16_temp = 0;
  ui16_temp  = ( ( cui16_shortMask & 0x007 ) << 13 ); // ID 18..20
  ui16_temp += ( ( cui16_shortMask & 0x7F8 ) >>  3 ); // ID 21..28
  const uint16_t cui16_buildShortMask = ui16_temp;

  ui16_temp  = ( cui32_longMask & 0x001FE000 ) >>  5; // ID 13..20
  ui16_temp += ( cui32_longMask & 0x1FE00000 ) >> 21; // ID 21..28
  const uint16_t cui16_longMaskUpper = ui16_temp;

  ui16_temp  = ( cui32_longMask & 0x0000001F ) << 11; // ID 4..0
  ui16_temp += ( cui32_longMask & 0x00001FE0 ) >> 5; //  ID 12..5
  const uint16_t cui16_longMaskLower = ui16_temp;
  #endif

  // deactivate all IRQ to inhibit distortion by RS232 or other IRQ
  IEN = 0;      // defined in reg167cs.h
  // now config BUS
  if ( rui8_busNr == 0 ) {
    // option bits: ErrInfoIrq := 0,  StatusInfoIrq := 0, CanIrq := 1
    // deactivate ITQ and state
    XP0IC = 0x0000;
	  // set control register to config mode
	  C1CSR = ( C1CSR | 0x41 ); // set CCE and INIT to indicate config
    // set 11bit mask for CAN1 in register
    C1GMS = cui16_buildShortMask;
    // set upper part of 29bit mask
    C1UGML = cui16_longMaskUpper;
    // set lower part of 29 bit mask
    C1LGML = cui16_longMaskLower;
    // reinit CAN with new settings
    CAN1_Init( ui8_currentBaudrate[0], EIE_BIT, SIE_BIT, IE_BIT );
    // double set the mask values, as it's not shure where
    // Mitron CAN init function takes the mask values
    // set 11bit mask for CAN1 in register
    C1GMS = cui16_buildShortMask;
    // set upper part of 29bit mask
    C1UGML = cui16_longMaskUpper;
    // set lower part of 29 bit mask
    C1LGML = cui16_longMaskLower;
	  // finish config with clear of CCE and INIT to indicate config
	  // ( higher 4-Bit nibble of lower byte must be set to 0
	  //   as it contains TestMode Bit and read-only bit despite of CCE )
	  C1CSR = ( C1CSR & 0xFF0E );
    CAN1TxReady = TRUE;
    ///  -----------------------------------------------------------------------
    ///  Configuration of the used CAN1 Interrupts:
    ///  -----------------------------------------------------------------------
    ///  - CAN1 service request node configuration:
    ///  - CAN1 interrupt priority level (ILVL) = 11
    ///  - CAN1 interrupt group level (GLVL) = 3
    XP0IC = 0x0073;
  }
  else {
    XP1IC =  0x0000;
	  // set control register to config mode
	  C2CSR = ( C2CSR | 0x41 ); // set CCE and INIT to indicate config
    // set 11bit mask for CAN2 in register
    C2GMS = cui16_buildShortMask;
    // set upper part of 29bit mask
    C2UGML = cui16_longMaskUpper;
    // set lower part of 29 bit mask
    C2LGML = cui16_longMaskLower;
    // reinit CAN with new settings
    CAN2_Init( ui8_currentBaudrate[1], EIE_BIT, SIE_BIT, IE_BIT );
    // double set the mask values, as it's not shure where
    // Mitron CAN init function takes the mask values
    // set 11bit mask for CAN2 in register
    C2GMS = cui16_buildShortMask;
    // set upper part of 29bit mask
    C2UGML = cui16_longMaskUpper;
    // set lower part of 29 bit mask
    C2LGML = cui16_longMaskLower;
	  // finish config with clear of CCE and INIT to indicate config
	  // ( higher 4-Bit nibble of lower byte must be set to 0
	  //   as it contains TestMode Bit and read-only bit despite of CCE )
	  C2CSR = ( C2CSR & 0xFF0E );
    CAN2TxReady = TRUE;
    ///  -----------------------------------------------------------------------
    ///  Configuration of the used CAN2 Interrupts:
    ///  -----------------------------------------------------------------------
    ///  - CAN2 service request node configuration:
    ///  - CAN2 interrupt priority level (ILVL) = 11
    ///  - CAN2 interrupt group level (GLVL) = 2
    XP1IC = 0x0072;
  }
  // activate all IRQ again
  IEN = 1;      // defined in reg167cs.h

  return HAL_NO_ERR;
}

/**
  set the global masks
  (11bit, 29bit and last msg mask)
  @param rui8_busNr number of the BUS to config (default 0)
  @param rui16_maskStd 11bit global mask
  @param rui32_maskExt 29bit global mask
  @param rui32_maskLastmsg mask of last CAN msg
  @return HAL_NO_ERR == no error;
          HAL_RANGE_ERR == wrong BUS nr
*/
int16_t can_configGlobalMask(uint8_t rui8_busNr, uint16_t rui16_maskStd, uint32_t rui32_maskExt, uint32_t rui32_maskLastmsg)
{
  if ( rui8_busNr > 1 ) {
    put_rs232String( "Verlasse can_configGlobalMask mit RangeError\n\r" );
    return HAL_RANGE_ERR;
  }
  b_runningIrqSendProcess[rui8_busNr] = false;
  // set close state for all MsgObj as base config of BUS causes invalidation of all active MsgObj
  for (uint8_t ui8_ind = 0; ui8_ind < 15; ui8_ind++) {
    ui8_canBufferDirectionState[rui8_busNr][ui8_ind] = 0xFF;
    b_appLock[rui8_busNr][ui8_ind] = false;
    resetCanRingBuffer( rui8_busNr, ui8_ind);

    // close MsgObjs - retry till MsgObj is really deleted
    if ( rui8_busNr == 0 ) {
     while ( CAN1_DelMsgObj( ui8_ind ) == 0 ) __HAL::wdTriggern();
    }
    else {
     while ( CAN2_DelMsgObj( ui8_ind ) == 0 ) __HAL::wdTriggern();
    }
  }
  // avoid problems with to big values for masks
  #if 0
  const uint16_t cui16_shortMask = ( rui16_maskStd & 0x7FF );
  const uint32_t cui32_longMask = ( rui32_maskExt & 0x1FFFFFFF );
  uint16_t ui16_buildShortMask = ( ( cui16_shortMask & 0x7 ) << 13 ) | ( ( cui16_shortMask >> 3 ) & 0xFF );
  uint32_t ui32_buildLongMaskLower = ( ( cui32_longMask & 0x1F ) << 11 ) | ( ( cui32_longMask >> 5 ) & 0xFF );
  uint32_t ui32_buildLongMaskUpper = ( ( ( cui32_longMask << 3 ) & 0xFF0000UL ) >> 8 ) | ( ( ( cui32_longMask << 3 ) & 0xFF000000UL ) >> 24 );
  const uint16_t cui16_longMaskLower = ( ui32_buildLongMaskLower & 0xFFFF );
  const uint16_t cui16_longMaskUpper = ( ui32_buildLongMaskUpper & 0xFFFF );
	#else
  const uint16_t cui16_shortMask = ( rui16_maskStd & 0x7FF );
  const uint32_t cui32_longMask = ( rui32_maskExt & 0x1FFFFFFF );
  uint16_t ui16_temp = 0;
  ui16_temp  = ( ( cui16_shortMask & 0x007 ) << 13 ); // ID 18..20
  ui16_temp += ( ( cui16_shortMask & 0x7F8 ) >>  3 ); // ID 21..28
  const uint16_t cui16_buildShortMask = ui16_temp;

  ui16_temp  = ( cui32_longMask & 0x001FE000 ) >>  5; // ID 13..20
  ui16_temp += ( cui32_longMask & 0x1FE00000 ) >> 21; // ID 21..28
  const uint16_t cui16_longMaskUpper = ui16_temp;

  ui16_temp  = ( cui32_longMask & 0x0000001F ) << 11; // ID 4..0
  ui16_temp += ( cui32_longMask & 0x00001FE0 ) >> 5; //  ID 12..5
  const uint16_t cui16_longMaskLower = ui16_temp;
  #endif
  // deactivate all IRQ to inhibit distortion by RS232 or other IRQ
  IEN = 0;      // defined in reg167cs.h
  if ( rui8_busNr == 0 ) {
    #if 0
    put_rs232String( "can_configGlobalMask on Bus 0\n\r" );
    #endif
    // deactivate ITQ and state
    XP0IC = 0x0000;
	  // set control register to config mode
	  C1CSR = ( C1CSR | 0x41 ); // set CCE and INIT to indicate config
    CAN1TxReady = FALSE;
    // set 11bit mask for CAN1 in register
    C1GMS = cui16_buildShortMask;
    // set upper part of 29bit mask
    C1UGML = cui16_longMaskUpper;
    // set lower part of 29 bit mask
    C1LGML = cui16_longMaskLower;
    // reinit CAN with new settings
    CAN1_Init( ui8_currentBaudrate[0], EIE_BIT, SIE_BIT, IE_BIT );
    // double set the mask values, as it's not shure where
    // Mitron CAN init function takes the mask values
    // set 11bit mask for CAN1 in register
    C1GMS = cui16_buildShortMask;
    // set upper part of 29bit mask
    C1UGML = cui16_longMaskUpper;
    // set lower part of 29 bit mask
    C1LGML = cui16_longMaskLower;
	  // finish config with clear of CCE and INIT to indicate config
	  // ( higher 4-Bit nibble of lower byte must be set to 0
	  //   as it contains TestMode Bit and read-only bit despite of CCE )
	  C1CSR = ( C1CSR & 0xFF0E );
    CAN1TxReady = TRUE;
    ///  -----------------------------------------------------------------------
    ///  Configuration of the used CAN1 Interrupts:
    ///  -----------------------------------------------------------------------
    ///  - CAN1 service request node configuration:
    ///  - CAN1 interrupt priority level (ILVL) = 11
    ///  - CAN1 interrupt group level (GLVL) = 3
    XP0IC = 0x0073;
  }
  else if ( rui8_busNr == 1 ) {
    #if 0
    put_rs232String( "can_configGlobalMask on Bus 1\n\r" );
    #endif
    // deactivate ITQ and state
    XP1IC =  0x0000;
	  // set control register to config mode
	  C2CSR = ( C2CSR | 0x41 ); // set CCE and INIT to indicate config
    CAN2TxReady = FALSE;
    // set 11bit mask for CAN2 in register
    C2GMS = cui16_buildShortMask;
    // set upper part of 29bit mask
    C2UGML = cui16_longMaskUpper;
    // set lower part of 29 bit mask
    C2LGML = cui16_longMaskLower;
    // reinit CAN with new settings
    CAN2_Init( ui8_currentBaudrate[1], EIE_BIT, SIE_BIT, IE_BIT );
    // double set the mask values, as it's not shure where
    // Mitron CAN init function takes the mask values
    // set 11bit mask for CAN2 in register
    C2GMS = cui16_buildShortMask;
    // set upper part of 29bit mask
    C2UGML = cui16_longMaskUpper;
    // set lower part of 29 bit mask
    C2LGML = cui16_longMaskLower;
	  // finish config with clear of CCE and INIT to indicate config
	  // ( higher 4-Bit nibble of lower byte must be set to 0
	  //   as it contains TestMode Bit and read-only bit despite of CCE )
	  C2CSR = ( C2CSR & 0xFF0E );
    CAN2TxReady = TRUE;
    ///  -----------------------------------------------------------------------
    ///  Configuration of the used CAN2 Interrupts:
    ///  -----------------------------------------------------------------------
    ///  - CAN2 service request node configuration:
    ///  - CAN2 interrupt priority level (ILVL) = 11
    ///  - CAN2 interrupt group level (GLVL) = 2
    XP1IC = 0x0072;
  }
  // activate all IRQ again
  IEN = 1;      // defined in reg167cs.h

  return HAL_NO_ERR;
}

/**
  close CAN BUS
  (important to close BUS before re-init with other baudrate)
  @param rui8_busNr number of the BUS to close (default 0)
  @return HAL_NO_ERR == no error;
          HAL_RANGE_ERR == wrong BUS nr;
          HAL_CONFIG_ERR == BUS previously not initialised
*/
int16_t can_configGlobalClose(uint8_t rui8_busNr)
{ // no function found in CAN driver
  // deactivate all IRQ to inhibit distortion by RS232 or other IRQ
  IEN = 0;      // defined in reg167cs.h

  // set close state for all MsgObj as base config of BUS causes invalidation of all active MsgObj
  #if 0
  put_rs232String( "GCl\n\r" );
  #endif
  for (uint8_t ui8_ind = 0; ui8_ind < 15; ui8_ind++) {
    if ( ui8_canBufferDirectionState[rui8_busNr][ui8_ind] == 0xFF ) continue;
    ui8_canBufferDirectionState[rui8_busNr][ui8_ind] = 0xFF;
    b_appLock[rui8_busNr][ui8_ind] = false;
    resetCanRingBuffer( rui8_busNr, ui8_ind );

    // close MsgObjs
    if ( rui8_busNr == 0 ) {
     while ( CAN1_DelMsgObj( ui8_ind ) == 0 ) __HAL::wdTriggern();
    }
    else {
     while ( CAN2_DelMsgObj( ui8_ind ) == 0 ) __HAL::wdTriggern();
    }
  }
  b_runningIrqSendProcess[rui8_busNr] = false;
  // activate all IRQ again
  IEN = 1;      // defined in reg167cs.h
  return HAL_NO_ERR;
}


/* ***************************** */
/* ***Specific for one MsgObj*** */
/* ***************************** */

/**
  config a MsgObj
  (buffer size is defined locally in implementation of this interface;
  all received msgs should get a time stamp;
  class __IsoAgLib::Ident_c has ident and type 11/29bit)
  @param rui8_busNr number of the BUS to config
  @param rui8_msgobjNr number of the MsgObj to config
  @param rrefc_ident filter ident of this MsgObj
  @param rb_rxtx 0==RX receive; 1==TX transmit
  @return HAL_NO_ERR == no error;
          C_BUSY == this MsgObj is already used
          HAL_CONFIG_ERR == BUS not initialised or error during buffer allocation
          HAL_RANGE_ERR == wrong BUS or MsgObj number
*/
int16_t can_configMsgobjInit(uint8_t rui8_busNr, uint8_t rui8_msgobjNr, __IsoAgLib::Ident_c& rrefc_ident, uint8_t rb_rxtx)
{ // check for input vars
  if ( ( rui8_busNr > 1 ) || ( rui8_msgobjNr> 14 ) ) {
    char tempString[80];
    sprintf( tempString, "\n\rVerlasse can_configMsgobjInit() wegen RangeErr durch BusNr %hd, MsgObjNr %hd\n\r", rui8_busNr, rui8_msgobjNr );
    put_rs232String( (const uint8_t *) tempString );
	int32_t i32_waitEnd = HAL::getTime() + 1000;
	while ( i32_waitEnd > HAL::getTime() ) ;
    return HAL_RANGE_ERR;
  }
  uint32_t ui32_ident = rrefc_ident.ident();
  ui8_canBufferXtd[rui8_busNr][rui8_msgobjNr] = (rrefc_ident.identType() == __IsoAgLib::Ident_c::BothIdent)
                    ?DEFAULT_IDENT_TYPE
                    :rrefc_ident.identType();

  uint8_t ui8_bufferSize = CONFIG_CAN_STD_LOAD_REC_BUF_SIZE_MIN;
	// unlock - if previously locked
	b_canBufferLock[rui8_busNr][rui8_msgobjNr] = false;

	// deactivate all IRQ to inhibit distortion by RS232 or other IRQ
  IEN = 0;      // defined in reg167cs.h
  // deactivate CAN BUS durign re-config
  if ( rui8_busNr == 0 ) {
    CAN1TxReady = FALSE;
    XP0IC = 0x0000;
  }
  else {
    CAN2TxReady = FALSE;
    XP1IC = 0x0000;
  }

  // check if MsgObj is already open, so that it must be closed first
  if ( ui8_canBufferDirectionState[rui8_busNr][rui8_msgobjNr] != 0xFF ) {
    // it's at the moment open
    if ( rui8_busNr == 0 ) {
     while ( CAN1_DelMsgObj( rui8_msgobjNr ) == 0 ) __HAL::wdTriggern();
    }
    else {
     while ( CAN2_DelMsgObj( rui8_msgobjNr ) == 0 ) __HAL::wdTriggern();
    }
  }
  // store wanted direction
  ui8_canBufferDirectionState[rui8_busNr][rui8_msgobjNr] = rb_rxtx;


  if (rb_rxtx == 0)
  { // receive
		ui8_bufferSize = CONFIG_CAN_STD_LOAD_REC_BUF_SIZE_MIN;
		const uint32_t highLoadCheckList[] = CONFIG_CAN_HIGH_LOAD_IDENT_LIST ;
		for ( uint8_t ind = 0; ind < CONFIG_CAN_HIGH_LOAD_IDENT_CNT; ind++ )
		{
			if ( highLoadCheckList[ind] == pt_config->dwId )
			{
				ui8_bufferSize = CONFIG_CAN_HIGH_LOAD_REC_BUF_SIZE_MIN;
				break;
			}
		}
  }
  else
  { // send
    ui8_bufferSize = CONFIG_CAN_SEND_BUFFER_SIZE;
  }
  // if buffer is previously of different size - but yet allocated, delete
  if ( ( arr_canBuffer[rui8_busNr][rui8_msgobjNr] != NULL               )
    && ( ui16_canBufferMaxSize[rui8_busNr][rui8_msgobjNr] < ui8_bufferSize ) ) {
    // currently existing buffer is smaller than now wanted -> delete old too small
    delete [] ( arr_canBuffer[rui8_busNr][rui8_msgobjNr] );
    arr_canBuffer[rui8_busNr][rui8_msgobjNr] = NULL;
  }
  if ( arr_canBuffer[rui8_busNr][rui8_msgobjNr] == NULL ) {
    // create buffer
    arr_canBuffer[rui8_busNr][rui8_msgobjNr] = new canBufferElement_t[ui8_bufferSize];
    if ( arr_canBuffer[rui8_busNr][rui8_msgobjNr] == NULL ) return HAL_OVERFLOW_ERR;
    ui16_canBufferMaxSize[rui8_busNr][rui8_msgobjNr] = ui8_bufferSize;
  }
  #if 0
  char tempString[80];
  sprintf( tempString, "MsgInit B%hd O%hd RxTx%hd X%hd Id0x%lx\n\r",
           rui8_busNr, rui8_msgobjNr, rb_rxtx, ui8_canBufferXtd[rui8_busNr][rui8_msgobjNr],
           ui32_ident );
  put_rs232String( (const uint8_t *) tempString );
  int32_t i32_waitEnd = HAL::getTime() + 500;
  while ( i32_waitEnd > HAL::getTime() ) tempString[0] = tempString[1] + 1;
  #endif
  if ( rui8_busNr == 0 ) {
    CAN1_DefineMsgObj(rui8_msgobjNr,  rb_rxtx, CAN_INT_ON, ui8_canBufferXtd[rui8_busNr][rui8_msgobjNr], ui32_ident, 8);
    CAN1TxReady = TRUE;
    XP0IC = 0x0073;
  }
  else {
    CAN2_DefineMsgObj(rui8_msgobjNr,  rb_rxtx, CAN_INT_ON, ui8_canBufferXtd[rui8_busNr][rui8_msgobjNr], ui32_ident, 8);
    CAN2TxReady = TRUE;
    XP1IC = 0x0072;
  }
  // activate all IRQ again
  IEN = 1;      // defined in reg167cs.h

  // reset AppLock flag
  b_appLock[rui8_busNr][rui8_msgobjNr] = false;
  resetCanRingBuffer( rui8_busNr, rui8_msgobjNr );

  return HAL_NO_ERR;
}

/**
  change the Ident_c of an already initialised MsgObj
  (class __IsoAgLib::Ident_c has ident and type 11/29bit)
  @param rui8_busNr number of the BUS to config
  @param rui8_msgobjNr number of the MsgObj to config
  @param rrefc_ident filter ident of this MsgObj
  @return HAL_NO_ERR == no error;
          HAL_CONFIG_ERR == BUS not initialised or ident can't be changed
          HAL_RANGE_ERR == wrong BUS or MsgObj number
*/
int16_t can_configMsgobjChgid(uint8_t rui8_busNr, uint8_t rui8_msgobjNr, __IsoAgLib::Ident_c& rrefc_ident)
{
  // check for input vars
  if ( ( rui8_busNr > 1 ) || ( rui8_msgobjNr> 14 ) ) return HAL_RANGE_ERR;
  if ( ui8_canBufferDirectionState[rui8_busNr][rui8_msgobjNr] == 0xFF ) return HAL_CONFIG_ERR;
  ui8_canBufferXtd[rui8_busNr][rui8_msgobjNr] = (rrefc_ident.identType() == __IsoAgLib::Ident_c::BothIdent)
                                                ?DEFAULT_IDENT_TYPE
                                                :rrefc_ident.identType();

  #if 0
  char tempString[80];
  sprintf( tempString, "MsgChgid B%hd M%hd X%hd, Id0x%lx\n\r",
           rui8_busNr, rui8_msgobjNr,
           ui8_canBufferXtd[rui8_busNr][rui8_msgobjNr],
           rrefc_ident.ident() );
  put_rs232String( (const uint8_t *) tempString );
  int32_t i32_waitEnd = HAL::getTime() + 500;
  while ( i32_waitEnd > HAL::getTime() ) tempString[0] = tempString[1] + 1;
  #endif
	// unlock - if previously locked
	b_canBufferLock[rui8_busNr][rui8_msgobjNr] = false;

	// deactivate all IRQ to inhibit distortion by RS232 or other IRQ
  IEN = 0;      // defined in reg167cs.h
  if ( rui8_busNr == 0 ) {
    CAN1TxReady = FALSE;
    while ( CAN1_DelMsgObj( rui8_msgobjNr ) == 0 ) __HAL::wdTriggern();
    XP0IC = 0x0000;
    CAN1_DefineMsgObj(rui8_msgobjNr,  ui8_canBufferDirectionState[rui8_busNr][rui8_msgobjNr], CAN_INT_ON, ui8_canBufferXtd[rui8_busNr][rui8_msgobjNr], rrefc_ident.ident(), 8);
    CAN1TxReady = TRUE;
    XP0IC = 0x0073;
  }
  else {
    CAN2TxReady = FALSE;
    while ( CAN2_DelMsgObj( rui8_msgobjNr ) == 0 ) __HAL::wdTriggern();
    XP1IC = 0x0000;
    CAN2_DefineMsgObj(rui8_msgobjNr,  ui8_canBufferDirectionState[rui8_busNr][rui8_msgobjNr], CAN_INT_ON, ui8_canBufferXtd[rui8_busNr][rui8_msgobjNr], rrefc_ident.ident(), 8);
    CAN2TxReady = TRUE;
    XP1IC = 0x0072;
  }
  // activate all IRQ again
  IEN = 1;      // defined in reg167cs.h

  // reset AppLock flag
  b_appLock[rui8_busNr][rui8_msgobjNr] = false;
  resetCanRingBuffer( rui8_busNr, rui8_msgobjNr );

  return HAL_NO_ERR;
}

/**
  change the the send rate for one MsgObj by setting the minimum
  pause time between two messages [msec.]
  @param rui8_busNr number of the BUS to config
  @param rui8_msgobjNr number of the MsgObj to config
  @param rui16_minSendPause minimum send pause between two sent messages [msec.]
  @return HAL_NO_ERR == no error;
          HAL_CONFIG_ERR == BUS not initialised or ident can't be changed
          HAL_RANGE_ERR == wrong BUS or MsgObj number
*/
int16_t can_configMsgobjSendpause(uint8_t rui8_busNr, uint8_t rui8_msgobjNr, uint16_t rui16_minSend)
{
  return HAL_NO_ERR;
}

/**
  close a MsgObj
  @param rui8_busNr number of the BUS to config
  @param rui8_msgobjNr number of the MsgObj to config
  @return HAL_NO_ERR == no error;
          HAL_CONFIG_ERR == BUS not initialised, MsgObj previously not used or buffer memory not freed
          HAL_RANGE_ERR == wrong BUS or MsgObj number
*/
int16_t can_configMsgobjClose(uint8_t rui8_busNr, uint8_t rui8_msgobjNr)
{
  // check for input vars
  if ( ( rui8_busNr > 1 ) || ( rui8_msgobjNr> 14 ) ) return HAL_RANGE_ERR;
	// unlock - if previously locked
	b_canBufferLock[rui8_busNr][rui8_msgobjNr] = false;
  #if 0
  char tempString[80];
  sprintf( tempString, "MsgClose B%hd M%hd\n\r",
           rui8_busNr, rui8_msgobjNr );
  put_rs232String( (const uint8_t *) tempString );
  int32_t i32_waitEnd = HAL::getTime() + 500;
  while ( i32_waitEnd > HAL::getTime() ) tempString[0] = tempString[1] + 1;
  #endif

  // deactivate all IRQ to inhibit distortion by RS232 or other IRQ
  IEN = 0;      // defined in reg167cs.h
  if ( rui8_busNr == 0 ) {
    CAN1TxReady = FALSE;
    while ( CAN1_DelMsgObj( rui8_msgobjNr ) == 0 ) __HAL::wdTriggern();
    XP0IC = 0x0000;
    CAN1TxReady = TRUE;
    XP0IC = 0x0073;
  }
  else {
    CAN2TxReady = FALSE;
    while ( CAN2_DelMsgObj( rui8_msgobjNr ) == 0 ) __HAL::wdTriggern();
    XP1IC = 0x0000;
    CAN2TxReady = TRUE;
    XP1IC = 0x0072;
  }
  // activate all IRQ again
  IEN = 1;      // defined in reg167cs.h

  // reset AppLock flag
  b_appLock[rui8_busNr][rui8_msgobjNr] = false;
  ui8_canBufferDirectionState[rui8_busNr][rui8_msgobjNr] = 0xFF;
  resetCanRingBuffer( rui8_busNr, rui8_msgobjNr );

  return HAL_NO_ERR;
}

/* ***************************************************** */
/* ***************** Use of MsgObj ********************* */
/* ***************************************************** */

/**
  update the CAN BUS load statistic
  @param rui8_busNr BUS number to update
  @param rb_dlc length ot the detected (send or receive) message
*/
void updateCanBusLoad(uint8_t rui8_busNr, uint8_t rb_dlc)
{
  if ( rui8_busNr > 1 ) return;
  int32_t i32_now = HAL::getTime();
  uint8_t b_newSlice = ((i32_now / 100)%10);
  if (gb_cinterfBusLoadSlice[rui8_busNr] != b_newSlice)
  {
    gwCinterfBusLoad[rui8_busNr][b_newSlice] = rb_dlc;
    gb_cinterfBusLoadSlice[rui8_busNr] = b_newSlice;
  }
  else
  {
    gwCinterfBusLoad[rui8_busNr][b_newSlice] += rb_dlc;
  }
}

/**
  send a message via a MsgObj;
  CANPkg_c (or derived object) must provide (virtual)
  functions:
  * MASK_TYPE ident() -> deliver ident value
  * __IsoAgLib::Ident_c::identType_t identType() -> deliver type of ident
  * void getData(MASK_TYPE& reft_ident, uint8_t& refui8_identType,
                 uint8_t& refb_dlcTarget, uint8_t* pb_dataTarget)
    -> put DLC in referenced ref_dlc and insert data in uint8_t string pb_data
  @param rui8_busNr number of the BUS to config
  @param rui8_msgobjNr number of the MsgObj to config
  @param rpc_data pointer to CANPkg_c instance with data to send
  @return HAL_NO_ERR == no error;
          HAL_CONFIG_ERR == BUS not initialised, MsgObj is no send object
          HAL_NOACT_ERR == BUS OFF
          HAL_OVERFLOW_ERR == send buffer overflowed
          HAL_RANGE_ERR == wrong BUS or MsgObj number
*/
int16_t can_useMsgobjSend(uint8_t rui8_busNr, uint8_t rui8_msgobjNr, __IsoAgLib::CANPkg_c* rpc_data)
{
  if ( ( rui8_busNr > 1 ) || ( rui8_msgobjNr> 14 ) ) return HAL_RANGE_ERR;
  // a receive MsgObj is registered with 0xFF in corresponding ui8_cinterfLastSendBufCnt
  if ( ui8_canBufferDirectionState[rui8_busNr][rui8_msgobjNr] != 1 ) return HAL_CONFIG_ERR;
  CAN_buf_Seed_def* p_tempSend = NULL;

  // direct access driver HW if buffer is empty
  if ( isCanRingBufferEmpty( rui8_busNr, rui8_msgobjNr ) ) {
    if ( rui8_busNr == 0 ) {
      // try to lock HW for send - mark successful preperation by writing address of ss_tempSendObj into p_tempSend
      if ( CAN1_RequestMsgObj( rui8_msgobjNr ) != 0 ) p_tempSend = &ss_tempSendObj;
    }
    else {
      // try to lock HW for send - mark successful preperation in b_doDirectSend
      if ( CAN2_RequestMsgObj( rui8_msgobjNr ) != 0 ) p_tempSend = &ss_tempSendObj;
    }
  }
  if ( p_tempSend == NULL ) {
    #if 0
    char tempString[80];
	  sprintf( tempString, "Buffer Send with Bus: %hd, MsgObj: %hd, MaxSize: %d, Size: %d, Read: %d, Write: %d, IRQ-Cnt: %lu, TX-IRQ: %lu, RX-IRQ: %lu, State: %x\n\r",
	  rui8_busNr, rui8_msgobjNr,
    ui16_canBufferMaxSize[rui8_busNr][rui8_msgobjNr],
	  ui16_canBufferSize[rui8_busNr][rui8_msgobjNr],
	  ui16_canBufferRead[rui8_busNr][rui8_msgobjNr],
	  ui16_canBufferWrite[rui8_busNr][rui8_msgobjNr],
	  sui32_irqCnt,
	  sui32_txIrqCnt,
	  sui32_rxIrqCnt,
	  ui16_lastCanBusState[rui8_busNr] );
    put_rs232String( (const uint8_t *) tempString );
	#endif
    // first if buffer is full
    if ( isCanRingBufferFull( rui8_busNr, rui8_msgobjNr ) ) {
	  #if 0
	  put_rs232String( "Send Buffer Full\n\r" );
	  #endif
      return HAL_OVERFLOW_ERR;
	}

    // set AppLock, so that IRQ function can avoid reading a not complete written msg
    // ( this could cause trouble, if the current to be written msg is the first msg in a
    //   previously empty buffer ) - if the buffer contains more elements, a parallel access
    //  causes no problems
    b_appLock[rui8_busNr][rui8_msgobjNr] = true;

    // retrieve pointer to buffer, where next data can be inserted ( don't force overwrite )
    canBufferElement_t* ps_element = writeDataToCanRingBuffer( rui8_busNr, rui8_msgobjNr, false );

    if ( ps_element == NULL ) {
      // maybe meanwhile buffer is full -> exit
      b_appLock[rui8_busNr][rui8_msgobjNr] = false;
	  #if 0
      put_rs232String( "Insert in buffer faulted\n\r" );
	  #endif
      return HAL_OVERFLOW_ERR;
    }
    else {
      // the Mitron CAN structure content is in the element field data
      p_tempSend = &( ps_element->data );
	  }
  }
  // now insert to be sent data
  // CANPkgExt_c::getData transforms flag data to ident and 8byte string
  uint8_t ui8_type;
  rpc_data->getData(p_tempSend->id, ui8_type, p_tempSend->bytes, p_tempSend->databytes);
  // pt_send->dwId = rpc_data->ident();
  // if (rpc_data->identType() == 1)
  // CANPkg_c::ident() and CANPkg_c::identType() changed to static
  p_tempSend->id = __IsoAgLib::CANPkg_c::ident();
  if (ui8_type == 1)
  { // extended 29bit ident
    updateCanBusLoad(rui8_busNr, (p_tempSend->bytes + 4));
  }
  else
  { // standard 11bit ident
    updateCanBusLoad(rui8_busNr, (p_tempSend->bytes + 2));
  }

  // check if p_tempSend was written with address of ss_tempSendObj for direct write
  if ( p_tempSend != &ss_tempSendObj ) {
    // buffered write detected
    // buffer contains now ready data -> release lock
    b_appLock[rui8_busNr][rui8_msgobjNr] = false;
    // check if currently a send irq process is running which could be now restarted
    if ( b_runningIrqSendProcess[rui8_busNr] == false ) {
      canBufferElement_t* ps_element = NULL;
      // only retry to start send, if corresponding MsgObj is ready to send
      if ( rui8_busNr == 0 ) {
        if ( CAN1_RequestMsgObj( rui8_msgobjNr ) != 0 )
          ps_element = readPopDataFromCanRingBuffer( 0, rui8_msgobjNr );
      }
      else {
        if ( CAN2_RequestMsgObj( rui8_msgobjNr ) != 0 )
          ps_element = readPopDataFromCanRingBuffer( 1, rui8_msgobjNr );
      }
      if ( ps_element != NULL ) {
        // start send the first msg in buffer
        b_runningIrqSendProcess[rui8_busNr] = true;
        p_tempSend = &( ps_element->data );
        if ( rui8_busNr == 0 ) {
          CAN1_WriteMsgObjData( rui8_msgobjNr, p_tempSend );
          CAN1_Transmit( rui8_msgobjNr );
		}
		else {
          CAN2_WriteMsgObjData( rui8_msgobjNr, p_tempSend );
          CAN2_Transmit( rui8_msgobjNr );
		}
      }
    }
  }
  else {
    // direct send detected
    b_runningIrqSendProcess[rui8_busNr] = true;
    if ( rui8_busNr == 0 ) {
      CAN1_WriteMsgObjData( rui8_msgobjNr, p_tempSend );
      CAN1_Transmit( rui8_msgobjNr );
    }
    else {
      CAN2_WriteMsgObjData( rui8_msgobjNr, p_tempSend );
      CAN2_Transmit( rui8_msgobjNr );
    }
  }

  return HAL_NO_ERR;
}

/**
  get the ident of a received message to decide about the further
  processing before the whole data string is retreived
  @param rui8_busNr number of the BUS to config
  @param rui8_msgobjNr number of the MsgObj to config
  @param reflIdent reference to the var, where the ident should be inserted
  @return error code
  HAL_NO_ERR == No problem
  HAL_CONFIG_ERR == BUS not initialised, MsgObj is no RX object
  HAL_NOACT_ERR == BUS OFF
  HAL_OVERFLOW_ERR == send buffer overflowed
  HAL_RANGE_ERR == wrong BUS or MsgObj number
  HAL_WARN_ERR == BUS WARN or no received message
*/
int32_t can_useMsgobjReceivedIdent(uint8_t rui8_busNr, uint8_t rui8_msgobjNr, int32_t &reflIdent)
{
  if ( ( rui8_busNr > 1 ) || ( rui8_msgobjNr> 14 ) ) return HAL_RANGE_ERR;
  if ( isCanRingBufferEmpty( rui8_busNr, rui8_msgobjNr ) ) return HAL_WARN_ERR;
  // a receive MsgObj is registered with 0 in corresponding ui8_canBufferDirectionState
  if ( ui8_canBufferDirectionState[rui8_busNr][rui8_msgobjNr] != 0 ) return HAL_CONFIG_ERR;
  // now write ident of MsgObj at position readPos
  reflIdent = pt_canBufferRead[rui8_busNr][rui8_msgobjNr]->data.id;
  #if 0
  char temp[50];
  if ( ( reflIdent & 0x700 ) == 0x200 ) {
    sprintf( temp, "Id0x%lx T%ld O%hd P%hd\n\r",
           reflIdent, arr_canBuffer[rui8_busNr][rui8_msgobjNr][readPos].timestamp,
		   rui8_msgobjNr, readPos );
    put_rs232String( (const uint8_t *) temp );
  }
  #endif
  return HAL_NO_ERR;
}

/**
	transfer front element in buffer into the pointed CANPkg_c;
	DON'T clear this item from buffer.
	@see can_useMsgobjPopFront for explicit clear of this front item
  functions:
  * void setIdent(MASK_TYPE rt_ident, Ident_c::identType_t rt_type)
    -> set ident rrefc_ident of received msg in CANPkg_c
  * uint8_t setDataString(uint8_t* rpb_data, uint8_t rb_dlc)
    -> set DLC in CANPkg_c from rb_dlc and insert data from uint8_t string rpb_data
  * void setTime(int32_t ri32_time) -> set receive time
  @param rui8_busNr number of the BUS to config
  @param rui8_msgobjNr number of the MsgObj to config
  @param rpc_data pointer to CANPkg_c instance with data to send
  @return HAL_NO_ERR == no error;
          HAL_CONFIG_ERR == BUS not initialised, MsgObj is no RX object
          HAL_NOACT_ERR == BUS OFF
          HAL_OVERFLOW_ERR == send buffer overflowed
          HAL_RANGE_ERR == wrong BUS or MsgObj number
          HAL_WARN_ERR == BUS WARN or no received message
*/
int16_t can_useMsgobjGet(uint8_t rui8_busNr, uint8_t rui8_msgobjNr, __IsoAgLib::CANPkg_c* rpc_data)
{
  if ( ( rui8_busNr > 1 ) || ( rui8_msgobjNr> 14 ) ) return HAL_RANGE_ERR;
  if ( isCanRingBufferEmpty( rui8_busNr, rui8_msgobjNr ) ) return HAL_WARN_ERR;
  // a receive MsgObj is registered with 0 in corresponding ui8_canBufferDirectionState
  if ( ui8_canBufferDirectionState[rui8_busNr][rui8_msgobjNr] != 0 ) return HAL_CONFIG_ERR;

  // repeat send tries till the AppLock isn't reset by IRQ function
  do {
    b_appLock[rui8_busNr][rui8_msgobjNr] = true;
    // TEST
    int32_t i32_test;
    can_useMsgobjReceivedIdent( rui8_busNr, rui8_msgobjNr, i32_test );
    // TEST
    // retrieve pointer with data to read
    canBufferElement_t *pt_element = readDataFromCanRingBuffer( rui8_busNr, rui8_msgobjNr );
    // check if valid data was returned
    if ( pt_element == NULL ) return HAL_WARN_ERR;
    // now read data from pointed element of buffer
    __IsoAgLib::CANPkg_c::setIdent(uint32_t( pt_element->data.id ), __IsoAgLib::Ident_c::identType_t(ui8_canBufferXtd[rui8_busNr][rui8_msgobjNr]));
    rpc_data->setDataString(pt_element->data.databytes, pt_element->data.bytes);
	rpc_data->setTime( pt_element->timestamp );
	#if 0
	char temp[50];
	if ( ( pt_element->data.id & 0x700 ) == 0x200 ) {
	  sprintf( temp, "Time: %lu:\tLastIdent: %lx\tIdent:%luDLC: %hu\tData: %hx %hx %hx %hx %hx %hx %hx %hx\n\r",
	          pt_element->timestamp,
			  sui32_lastIdent,
			  pt_element->data.id,
              pt_element->data.bytes,
			  pt_element->data.databytes[0],
			  pt_element->data.databytes[1],
			  pt_element->data.databytes[2],
			  pt_element->data.databytes[3],
			  pt_element->data.databytes[4],
			  pt_element->data.databytes[5],
			  pt_element->data.databytes[6],
			  pt_element->data.databytes[7] );
      put_rs232String( (const uint8_t *) temp );
	}
  #elif 0
	static char temp[80];
	if ( ( pt_element->data.id & 0x700 ) == 0x200 ) {
	  sprintf( temp, "Process at Time: %lu:\tLastIdent: 0x%lx\tIdent:%luDLC: %hu - ResultIdent: 0x%lx\n\r",
	          pt_element->timestamp,
			  sui32_lastIdent,
			  pt_element->data.id,
        pt_element->data.bytes,
        __IsoAgLib::CANPkg_c::ident()    );
      put_rs232String( (const uint8_t *) temp );
	}
	#endif
  } while ( b_appLock[rui8_busNr][rui8_msgobjNr] == false );
  // now flag can be cleared as data is complete read
  b_appLock[rui8_busNr][rui8_msgobjNr] = false;

  return HAL_NO_ERR;
}

/**
	Either register the currenct front item of buffer as not relevant,
	or just pop the front item, as it was processed.
	This explicit pop is needed, as one CAN message shall be served to
	several CANCustomer_c instances, as long as one of them indicates a
	succesfull process of the received message.
  @param rui8_busNr number of the BUS to config
  @param rui8_msgobjNr number of the MsgObj to config
*/
void can_useMsgobjPopFront(uint8_t rui8_busNr, uint8_t rui8_msgobjNr)
{ // simply increment read position without further interpretation
  popDataFromCanRingBuffer( rui8_busNr, rui8_msgobjNr );
}


/**
  clear th buffer of a MsgObj (e.g. to stop sending retries)
  @param rui8_busNr number of the BUS to config
  @param rui8_msgobjNr number of the MsgObj to config
  @return HAL_NO_ERR == no error;
          HAL_CONFIG_ERR == BUS not initialised
          HAL_RANGE_ERR == wrong BUS or MsgObj number
*/
int16_t can_useMsgobjClear(uint8_t rui8_busNr, uint8_t rui8_msgobjNr)
{
  if ( ( rui8_busNr > 1 ) || ( rui8_msgobjNr> 14 ) ) return HAL_RANGE_ERR;
  // a receive MsgObj is registered with 0 in corresponding ui8_canBufferDirectionState
  if ( ui8_canBufferDirectionState[rui8_busNr][rui8_msgobjNr] != 1 ) return HAL_CONFIG_ERR;
  // reset buffer state
  resetCanRingBuffer( rui8_busNr, rui8_msgobjNr );
  // close - open MsgObj
  return HAL_NO_ERR;
}
} // end of namespace

extern "C" {

//****************************************************************************
// Function      interrupt (XP0INT) void CAN1_Isr(void)
// Level
// Project
// Library
// File          CanISR.C
// SW-env.       Tasking C166
// HW-env.       Infineon C167 series microprocessor
//
//----------------------------------------------------------------------------
// Description   This is the interrupt service routine for the CAN1 controller.
//                Depending on the configuration it is executed if:
//                - the busoff or the error warning status is reached
//                  (EIE is set)
//                - a message has been sent or received successfully or a bus
//                  error occurred (SIE is set)
//                - the bit INTPND (interrupt pending) in one of the message
//                  object control-registers is set (at Tx or Rx)
//
//
//                NOTE: You have to add application specific
//                code to this function. There is for message object 1 and 15
//				  example code written, which can be modified  by user to suit
//				  the application.
//
//----------------------------------------------------------------------------
// Returnvalue   none
//
//----------------------------------------------------------------------------
// Parameters    none
//
//----------------------------------------------------------------------------
//
// VERS   DATE      EDITOR  CHANGE
// 1.0    22.10.02  SR      Initial version
//
//****************************************************************************

interrupt (XP0INT) _using(CAN1_ISR) void CAN1_Isr(void)
{
  unsigned int IntID;
  const int32_t ci32_now = HAL::getTime();
  canBufferElement_t* pt_tempElement;
  uint8_t ui8_msgObjInd;

  sui32_irqCnt++;

  while ((IntID = (C1IR & 0x00ff)))
  {
    ui16_lastCanBusState[0] = C1CSR;
	// clear higher byte - preserve config bits
    C1CSR = ( ui16_lastCanBusState[0] & 0xFF );
    if ( (IntID & 0x00ff) == 1 ) {
      // Status Change Interrupt
      // The CAN controller has updated (not necessarily changed)
      // the status in the Control Register.
      if (ui16_lastCanBusState[0] & CanStateOff)  // if BOFF
      { // Indicates when the CAN controller is in busoff state.
        C1CSR = (C1CSR & 0xfffe);    // recover from BOFF (clear INIT)
      }
      if (ui16_lastCanBusState[0] & CanStateWarn)  // if EWRN
      { // Indicates that the least one of the error counters in the
        // EML has reached the error warning limit of 96.
        if ( i32_cinterfBeginBusWarnOff[0] == -1 ) {
          // first WARN event of this BUS
          i32_cinterfBeginBusWarnOff[0] = ci32_now;
        }
      }
      else {
        // no WARN condition -> clear warn timestamp
        i32_cinterfBeginBusWarnOff[0] = -1;
      }
      // continue with next readout of possibly pending IRQ
      continue;
    }
    else if ( (IntID & 0x00ff) == 2 ) ui8_msgObjInd = 14;
    else ui8_msgObjInd = (IntID & 0x00ff) - 3;
    // interprete IRQ type: TXOK or RXOK for MsgObj ui8_msgObjInd
    if (ui16_lastCanBusState[0] & CanStateRxOk) {
	  sui32_rxIrqCnt++;
      i32_cinterfLastSuccReceive[0] = ci32_now;
      // if receive interrupt... (RXOK=1)
      if( CAN1_MsgLost( ui8_msgObjInd ) ) {
        // maybe do some special action on lost msg - but mainly cause clear of register bit
      }
      if ( ( CAN1_NewData( ui8_msgObjInd ) ) && ( ! b_canBufferLock[0][ui8_msgObjInd] ) ) {
        // The CAN controller has stored a new message
        // into this object - and this msgObj is not marked as locked
        if ( isCanRingBufferFull( 0, ui8_msgObjInd ) ) {
          // clear so that potentially interrupted read action of App can detect
          // that in overflowe condition, the next data must be read
          b_appLock[0][ui8_msgObjInd] = false;
        }
        pt_tempElement = writeDataToCanRingBuffer( 0, ui8_msgObjInd, true );
        CAN1_GetMsgObj( ui8_msgObjInd , &( pt_tempElement->data ) );
        pt_tempElement->timestamp = ci32_now;
				sui32_lastIdent = pt_tempElement->data.id;
      }
      CAN1_ReleaseObj( ui8_msgObjInd );
    }
    if ( ui16_lastCanBusState[0] & CanStateTxOk ) {
      // TXOK IRQ
	  sui32_txIrqCnt++;
      i32_cinterfLastSuccSend[0] = ci32_now;
      CAN1TxReady = TRUE;
      b_runningIrqSendProcess[0] = false;
	  CAN1_ReleaseObj( ui8_msgObjInd );
      // check if same MsgObj can't be used to send succeeding msg
      // this can be the case if:
      // + buffer is complete empty
      // + buffer contains <= 1 msgs where App has activated lock to
      //    indicate that the write of the single obj may be currently
      //    written, so that the data is not yet valid for send
      if ( ( isCanRingBufferEmpty( 0, ui8_msgObjInd )         )
        || ( ( b_appLock[0][ui8_msgObjInd] == true          )
          && ( getCanRingBufferSize(0, ui8_msgObjInd ) <= 1 ) ) ) {
		// MsgObj of successful send has no more msg to send in buffer -> clear INTPND
		CAN1_ReleaseObj( ui8_msgObjInd );
        // search in other configureg send buffer for other waiting msgs
        pt_tempElement = NULL;
        for ( ui8_msgObjInd = 0; ui8_msgObjInd < 15; ui8_msgObjInd++ ) {
          // check if MsgObj buffer is TX and contains msgs to send
          if ( ( ui8_canBufferDirectionState[0][ui8_msgObjInd] == 1 )
            && ( ! isCanRingBufferEmpty( 0, ui8_msgObjInd )         )
            && ( ( getCanRingBufferSize(0, ui8_msgObjInd ) > 1 ) || ( b_appLock[0][ui8_msgObjInd] == false ) )
            && ( CAN1_RequestMsgObj( ui8_msgObjInd )                ) ) {
            // MsgObj in has data to send
            pt_tempElement = readPopDataFromCanRingBuffer( 0, ui8_msgObjInd );
            break;
          }
        }
      }
      else if ( CAN1_RequestMsgObj( ui8_msgObjInd ) ) {
        // retrieve for other waiting msgs in this buffer
        pt_tempElement = readPopDataFromCanRingBuffer( 0, ui8_msgObjInd );
      }
      else {
        pt_tempElement = NULL; // no continuing send is possible
      }
      // if pt_tempElement != NULL -> trigger pointed data to send
      if ( pt_tempElement != NULL ) {
          // clear MsgObj
          CAN1TxReady = FALSE;
   		  b_runningIrqSendProcess[0] = true;
          CAN1_WriteMsgObjData( ui8_msgObjInd, & ( pt_tempElement->data ) );
          CAN1_Transmit( ui8_msgObjInd );
      }
    } // end handle TXOK
  }    // end while
}      // end ISR



//****************************************************************************
// Function      interrupt (XP1INT) void CAN2_Isr(void)
// level         device hander
// project
// library
// file          CanISR.C
// SW-env.       Tasking C166
// HW-env.       Infineon C167 series microprocessor
//
//----------------------------------------------------------------------------
// Description    This is the interrupt service routine for the CAN2 controller.
//                Depending on the configuration it is executed if:
//                - the busoff or the error warning status is reached
//                  (EIE is set)
//                - a message has been sent or received successfully or a bus
//                  error occurred (SIE is set)
//                - the bit INTPND (interrupt pending) in one of the message
//                  object control-registers is set (at Tx or Rx)
//
//
//                Please note that you have to add application specific
//                code to this function.
//
//----------------------------------------------------------------------------
// Returnvalue   none
//
//----------------------------------------------------------------------------
// Parameters    none
//
//----------------------------------------------------------------------------
// VERS   DATE      EDITOR  CHANGE
// 1.0    22.10.02  SR      Initial version
//
//****************************************************************************

interrupt (XP1INT) _using(CAN2_ISR) void CAN2_Isr(void)
{
  unsigned int IntID;
  const int32_t ci32_now = HAL::getTime();
  canBufferElement_t* pt_tempElement;
  uint8_t ui8_msgObjInd;

  sui32_irqCnt++;

  while ((IntID = (C2IR & 0x00ff)))
  {
    ui16_lastCanBusState[1] = C2CSR;
	// clear higher byte - preserve config bits
    C2CSR = ( ui16_lastCanBusState[1] & 0xFF );
    if ( (IntID & 0x00ff) == 1 ) {
      // Status Change Interrupt
      // The CAN controller has updated (not necessarily changed)
      // the status in the Control Register.
      if (ui16_lastCanBusState[1] & CanStateOff)  // if BOFF
      { // Indicates when the CAN controller is in busoff state.
        C2CSR = (C2CSR & 0xfffe);    // recover from BOFF (clear INIT)
      }
      if (ui16_lastCanBusState[1] & CanStateWarn)  // if EWRN
      { // Indicates that the least one of the error counters in the
        // EML has reached the error warning limit of 96.
        if ( i32_cinterfBeginBusWarnOff[1] == -1 ) {
          // first WARN event of this BUS
          i32_cinterfBeginBusWarnOff[1] = ci32_now;
        }
      }
      else {
        // no WARN condition -> clear warn timestamp
        i32_cinterfBeginBusWarnOff[1] = -1;
      }
      // continue with next readout of possibly pending IRQ
      continue;
    }
    else if ( (IntID & 0x00ff) == 2 ) ui8_msgObjInd = 14;
    else ui8_msgObjInd = (IntID & 0x00ff) - 3;
    // interprete IRQ type: TXOK or RXOK for MsgObj ui8_msgObjInd
    if (ui16_lastCanBusState[1] & CanStateRxOk) {
	  sui32_rxIrqCnt++;
      i32_cinterfLastSuccReceive[1] = ci32_now;
      // if receive interrupt... (RXOK=1)
      if( CAN2_MsgLost( ui8_msgObjInd ) ) {
        // maybe do some special action on lost msg - but mainly cause clear of register bit
      }
      if ( ( CAN2_NewData( ui8_msgObjInd ) ) && ( ! b_canBufferLock[1][ui8_msgObjInd] ) ) {
        // The CAN controller has stored a new message
        // into this object - and this msgObj is not marked as locked
        if ( isCanRingBufferFull( 1, ui8_msgObjInd ) ) {
          // clear so that potentially interrupted read action of App can detect
          // that in overflowe condition, the next data must be read
          b_appLock[1][ui8_msgObjInd] = false;
        }
        pt_tempElement = writeDataToCanRingBuffer( 1, ui8_msgObjInd, true );
        CAN2_GetMsgObj( ui8_msgObjInd , &( pt_tempElement->data ) );
        pt_tempElement->timestamp = ci32_now;
      }
      CAN2_ReleaseObj( ui8_msgObjInd );
    }
    if ( ui16_lastCanBusState[1] & CanStateTxOk ) {
      // TXOK IRQ
	  sui32_txIrqCnt++;
      i32_cinterfLastSuccSend[1] = ci32_now;
      CAN2TxReady = TRUE;
      b_runningIrqSendProcess[1] = false;
	  CAN2_ReleaseObj( ui8_msgObjInd );
      // check if same MsgObj can't be used to send succeeding msg
      // this can be the case if:
      // + buffer is complete empty
      // + buffer contains <= 1 msgs where App has activated lock to
      //    indicate that the write of the single obj may be currently
      //    written, so that the data is not yet valid for send
      if ( ( isCanRingBufferEmpty( 1, ui8_msgObjInd )         )
        || ( ( b_appLock[1][ui8_msgObjInd] == true          )
          && ( getCanRingBufferSize(1, ui8_msgObjInd ) <= 1 ) ) ) {
		// MsgObj of successful send has no more msg to send in buffer -> clear INTPND
		CAN2_ReleaseObj( ui8_msgObjInd );
        // search in other configureg send buffer for other waiting msgs
        pt_tempElement = NULL;
        for ( ui8_msgObjInd = 0; ui8_msgObjInd < 15; ui8_msgObjInd++ ) {
          // check if MsgObj buffer is TX and contains msgs to send
          if ( ( ui8_canBufferDirectionState[1][ui8_msgObjInd] == 1 )
            && ( ! isCanRingBufferEmpty( 1, ui8_msgObjInd )         )
            && ( ( getCanRingBufferSize(1, ui8_msgObjInd ) > 1 ) || ( b_appLock[1][ui8_msgObjInd] == false ) )
            && ( CAN2_RequestMsgObj( ui8_msgObjInd )                ) ) {
            // MsgObj in has data to send
            pt_tempElement = readPopDataFromCanRingBuffer( 1, ui8_msgObjInd );
            break;
          }
        }
      }
      else if ( CAN2_RequestMsgObj( ui8_msgObjInd ) ) {
        // retrieve for other waiting msgs in this buffer
        CAN2_ReleaseObj( ui8_msgObjInd );
        pt_tempElement = readPopDataFromCanRingBuffer( 1, ui8_msgObjInd );
      }
      else {
        CAN2_ReleaseObj( ui8_msgObjInd );
        pt_tempElement = NULL; // no continuing send is possible
      }
      // if pt_tempElement != NULL -> trigger pointed data to send
      if ( pt_tempElement != NULL ) {
          // clear MsgObj
          CAN2TxReady = FALSE;
   		  b_runningIrqSendProcess[1] = true;
          CAN2_WriteMsgObjData( ui8_msgObjInd, & ( pt_tempElement->data ) );
          CAN2_Transmit( ui8_msgObjInd );
      }
    } // end handle TXOK
  }    // end while
}      // end ISR

}
