/*
  hal_can_interface.cpp: CAN interface between BIOS/OS and IsoAgLib to
    concentrate CAN handling abstraction within one module

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

/* **********************************************************
 * The module halCanInterface implements a standard
 * interface for the CAN interactions of the IsoAgLib
 * This way the the adaptation of the IsoAgLib can be
 * better restricted to the various BIOS specific the
 * directories xgpl_src/IsoAgLib/hal/xx/can/ .
 * ********************************************************** */

#include "../typedef.h"
#include "../config.h"
#include "../errcodes.h"
#include "hal_can_interface.h"
#include "../system/system_target_extensions.h"

#include <IsoAgLib/driver/can/impl/ident_c.h>
#include <IsoAgLib/driver/can/impl/canpkg_c.h>
#include <IsoAgLib/hal/mitron167/system/system.h>

#include <supplementary_driver/hal/hal_rs232.h>


#define TASKING
#define SRVWDT _srvwdt()

#define TRUE 1
#define FALSE 0

extern "C" {
  /** include the BIOS specific header with the part for CAN into __HAL */
  #include <commercial_BIOS/bios_mitron167/CanDriver/CAN.H>
  #include <C166.h>
  #include <reg167cs.h>
  #include "canr_16x.h"             /* CAN register definitions   */

  /** some prototypes from Mitron CAN example */
  unsigned char CAN1TxReady;
  unsigned char CAN2TxReady;


/* ************************************* */
/* **** Some Modul Global Variables **** */
/* ************************************* */
static const uint32_t cui32_maxCanBusCnt = ( HAL_CAN_MAX_BUS_NR + 1 );

static uint16_t ui16_lastCanBusState[cui32_maxCanBusCnt];
static int32_t i32_cinterfBeginBusWarnOff[cui32_maxCanBusCnt];
static int32_t i32_cinterfBeginBit1err[cui32_maxCanBusCnt];

/** report if active running IRQ controlled send process is active */
static bool b_runningIrqSendProcess[cui32_maxCanBusCnt] = { false, false };

static int32_t i32_cinterfLastSuccSend[cui32_maxCanBusCnt];
static int32_t i32_cinterfLastSuccReceive[cui32_maxCanBusCnt];

/** array of 100msec. timeslice conters of received and sent msg per BUS [uint8_t] */
static uint16_t gwCinterfBusLoad[cui32_maxCanBusCnt][10];
/** actual index in gwBusLoad */
static uint8_t gb_cinterfBusLoadSlice[cui32_maxCanBusCnt];

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

/** lock state */
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
bool isCanRingBufferFull( uint8_t aui8_busNr, uint8_t aui8_msgObj ) {
  if ( ( aui8_busNr > 1 ) || ( aui8_msgObj> 14 ) ) return true;
  return !( ui16_canBufferSize[aui8_busNr][aui8_msgObj] < ui16_canBufferMaxSize[aui8_busNr][aui8_msgObj] );
}
/** low level helper function to check if buffer is empty */
bool isCanRingBufferEmpty( uint8_t aui8_busNr, uint8_t aui8_msgObj ) {
  if ( ( aui8_busNr > 1 ) || ( aui8_msgObj> 14 ) ) return true;
  return ( ui16_canBufferSize[aui8_busNr][aui8_msgObj] == 0 );
}
/** low level helper function to get current count of elements in buffer */
uint16_t getCanRingBufferSize( uint8_t aui8_busNr, uint8_t aui8_msgObj ) {
  if ( ( aui8_busNr > 1 ) || ( aui8_msgObj> 14 ) ) return 0;
  return ui16_canBufferSize[aui8_busNr][aui8_msgObj];
}
/** low level helper function to get current count of elements in buffer */
uint16_t getCanRingBufferMaxSize( uint8_t aui8_busNr, uint8_t aui8_msgObj ) {
  if ( ( aui8_busNr > 1 ) || ( aui8_msgObj> 14 ) ) return 0;
  return ui16_canBufferMaxSize[aui8_busNr][aui8_msgObj];
}
/** low level helper function to reset internal state of buffer:
    - set read and write position to begin of buffer
    - set size to 0
 */
uint16_t resetCanRingBuffer( uint8_t aui8_busNr, uint8_t aui8_msgObj ) {
  if ( ( aui8_busNr > 1 ) || ( aui8_msgObj> 14 ) ) return 0;
  _atomic( 4 );
  // set read and write pointer to first pointed CAN-Obj
  // in selected buffer <aui8_busNr, aui8_msgObj>
  pt_canBufferRead[aui8_busNr][aui8_msgObj]
    = pt_canBufferWrite[aui8_busNr][aui8_msgObj]
    = pt_canBufferBegin[aui8_busNr][aui8_msgObj]
    = &(arr_canBuffer[aui8_busNr][aui8_msgObj][0]);

  _atomic( 4 );
  const uint16_t cui16_lastItem
    = (ui16_canBufferMaxSize[aui8_busNr][aui8_msgObj] - 1);
  pt_canBufferEnd[aui8_busNr][aui8_msgObj]
    = &(arr_canBuffer[aui8_busNr][aui8_msgObj][cui16_lastItem]);

  ui16_canBufferSize[aui8_busNr][aui8_msgObj] = 0;
  return 1;
}

/** low level helper function to write can data */
canBufferElement_t* writeDataToCanRingBuffer( uint8_t aui8_busNr, uint8_t aui8_msgObj, bool ab_force = false ) {
  if ( ( aui8_busNr > 1 ) || ( aui8_msgObj> 14 ) ) return NULL;
  // first check if buffer is already full and no overwrite is forced
  if ( ( isCanRingBufferFull( aui8_busNr, aui8_msgObj ) ) && ( ! ab_force ) ) return NULL;

  // make first validate check
  if ( ( pt_canBufferWrite[aui8_busNr][aui8_msgObj] < pt_canBufferBegin[aui8_busNr][aui8_msgObj] )
    || ( pt_canBufferWrite[aui8_busNr][aui8_msgObj] > pt_canBufferEnd[aui8_busNr][aui8_msgObj]   ) )
  { // integrity of write pointer is corrupted -> reset
    resetCanRingBuffer( aui8_busNr, aui8_msgObj );
  }
  // now either everything was already correct, or a freshly reseted buffer is available
  // it's granted that write pointer does point to CAN-Obj in buffer of selected MsgObj

  _atomic( 4 );
  // first increase size to avoid problems with combined access
  ui16_canBufferSize[aui8_busNr][aui8_msgObj]++;
  // now derive write pointer and increment pointer
  canBufferElement_t* ps_tempWrite = pt_canBufferWrite[aui8_busNr][aui8_msgObj]++;

  // update read pos if this write is forced durign buffer-full condition ( oldest not-yet read msg is overwritten by this call )
  if ( isCanRingBufferFull( aui8_busNr, aui8_msgObj ) ) {
    // use operator++ to shorten time critical increasing access (IRQ)
    _atomic( 4 );
    pt_canBufferRead[aui8_busNr][aui8_msgObj]++;
    if ( pt_canBufferRead[aui8_busNr][aui8_msgObj] == pt_canBufferEnd[aui8_busNr][aui8_msgObj] )
      pt_canBufferRead[aui8_busNr][aui8_msgObj] = pt_canBufferBegin[aui8_busNr][aui8_msgObj];
  }
  // now check if write position must be reset to Begin
  _atomic( 4 );
  if ( pt_canBufferWrite[aui8_busNr][aui8_msgObj] == pt_canBufferEnd[aui8_busNr][aui8_msgObj] )
    pt_canBufferWrite[aui8_busNr][aui8_msgObj] = pt_canBufferBegin[aui8_busNr][aui8_msgObj];

  // now return pointer to CAN buffer element, where new data can be stored
  return ps_tempWrite;
}

/** low level helper function to read can data */
canBufferElement_t* readDataFromCanRingBuffer( uint8_t aui8_busNr, uint8_t aui8_msgObj ) {
  if ( ( aui8_busNr > 1 ) || ( aui8_msgObj> 14 ) ) return NULL;
  // first check if buffer is empty
  if ( isCanRingBufferEmpty( aui8_busNr, aui8_msgObj ) ) return NULL;

  // make first validate check
  if ( ( pt_canBufferRead[aui8_busNr][aui8_msgObj] < pt_canBufferBegin[aui8_busNr][aui8_msgObj] )
    || ( pt_canBufferRead[aui8_busNr][aui8_msgObj] > pt_canBufferEnd[aui8_busNr][aui8_msgObj]   ) )
  { // integrity of write pointer is corrupted -> reset
    resetCanRingBuffer( aui8_busNr, aui8_msgObj );
    // there's nothin to read from fresh reset buffer
    return NULL;
  }
  // now everything was already correct
  // it's granted that read pointer does point to CAN-Obj in buffer of selected MsgObj

  // now derive read pointer
  return pt_canBufferRead[aui8_busNr][aui8_msgObj];
}

/** low level helper function to read can data */
canBufferElement_t* readPopDataFromCanRingBuffer( uint8_t aui8_busNr, uint8_t aui8_msgObj ) {
  if ( ( aui8_busNr > 1 ) || ( aui8_msgObj> 14 ) ) return NULL;
  // first check if buffer is empty
  if ( isCanRingBufferEmpty( aui8_busNr, aui8_msgObj ) ) return NULL;

  // make first validate check
  if ( ( pt_canBufferRead[aui8_busNr][aui8_msgObj] < pt_canBufferBegin[aui8_busNr][aui8_msgObj] )
    || ( pt_canBufferRead[aui8_busNr][aui8_msgObj] > pt_canBufferEnd[aui8_busNr][aui8_msgObj]   ) )
  { // integrity of write pointer is corrupted -> reset
    resetCanRingBuffer( aui8_busNr, aui8_msgObj );
    // there's nothin to read from fresh reset buffer
    return NULL;
  }
  // now everything was already correct
  // it's granted that read pointer does point to CAN-Obj in buffer of selected MsgObj

  _atomic( 4 );
  // first decrease size to avoid problems with combined access
  ui16_canBufferSize[aui8_busNr][aui8_msgObj]--;
  // now derive read pointer and increment pointer after read access
  canBufferElement_t* ps_tempRead = pt_canBufferRead[aui8_busNr][aui8_msgObj]++;

  // now check if read position must be reset to Begin
  _atomic( 4 );
  if ( pt_canBufferRead[aui8_busNr][aui8_msgObj] == pt_canBufferEnd[aui8_busNr][aui8_msgObj] )
    pt_canBufferRead[aui8_busNr][aui8_msgObj] = pt_canBufferBegin[aui8_busNr][aui8_msgObj];
  // now return pointer to CAN buffer element, where new data can be read
  return ps_tempRead;
}

void popDataFromCanRingBuffer( uint8_t aui8_busNr, uint8_t aui8_msgObj ) {
  if ( ( aui8_busNr > 1 ) || ( aui8_msgObj> 14 ) ) return;
  // first check if buffer is empty
  if ( isCanRingBufferEmpty( aui8_busNr, aui8_msgObj ) ) return;

  // make first validate check
  if ( ( pt_canBufferRead[aui8_busNr][aui8_msgObj] < pt_canBufferBegin[aui8_busNr][aui8_msgObj] )
    || ( pt_canBufferRead[aui8_busNr][aui8_msgObj] > pt_canBufferEnd[aui8_busNr][aui8_msgObj]   ) )
  { // integrity of write pointer is corrupted -> reset
    resetCanRingBuffer( aui8_busNr, aui8_msgObj );
    // there's nothin to read from fresh reset buffer
    return;
  }
  // now everything was already correct
  // it's granted that read pointer does point to CAN-Obj in buffer of selected MsgObj

  _atomic( 4 );
  // first decrease size to avoid problems with combined access
  ui16_canBufferSize[aui8_busNr][aui8_msgObj]--;
  // now derive read pointer and increment pointer after read access
  canBufferElement_t* ps_tempRead = pt_canBufferRead[aui8_busNr][aui8_msgObj]++;

  // now check if read position must be reset to Begin
  _atomic( 4 );
  if ( pt_canBufferRead[aui8_busNr][aui8_msgObj] == pt_canBufferEnd[aui8_busNr][aui8_msgObj] )
    pt_canBufferRead[aui8_busNr][aui8_msgObj] = pt_canBufferBegin[aui8_busNr][aui8_msgObj];
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
  @param aui8_busNr number of bus to check
*/
void updateCanStateTimestamps(uint8_t aui8_busNr)
{
  if ( aui8_busNr > 1 ) return;
  const uint16_t ui16_canState = ui16_lastCanBusState[aui8_busNr];
  // check if WARN bit is set in CAN control status register
  if ((ui16_canState & CanStateWarnOrOff) != 0)
  { // set error state time interval begin
    if (i32_cinterfBeginBusWarnOff[aui8_busNr] < 0)
      i32_cinterfBeginBusWarnOff[aui8_busNr] = HAL::getTime();
  }
  else
  { // resets error time begin
    i32_cinterfBeginBusWarnOff[aui8_busNr] = -1;
  }
  // check if LEC state reports bit1error
  if ((ui16_canState & CanStateLecErrMask) == CanStateBit1Err)
  {  // set error state time interval begin
    if (i32_cinterfBeginBit1err[aui8_busNr] < 0)
      i32_cinterfBeginBit1err[aui8_busNr] = HAL::getTime();
  }
  else
  {  // resets error time begin
    i32_cinterfBeginBit1err[aui8_busNr] = -1;
  }
}

/**
  test if the CAN BUS is in WARN state
  @param aui8_busNr number of the BUS to check (default 0)
  @return true == CAN BUS is in WARN state, else normal operation
*/
bool can_stateGlobalWarn(uint8_t aui8_busNr)
{
  if ( aui8_busNr > 1 ) return true;
  updateCanStateTimestamps(aui8_busNr);
  // check if WARN bit is set in CAN control status register
  return (ui16_lastCanBusState[aui8_busNr] & CanStateWarn);
}

/**
  test if the CAN BUS is in OFF state
  @param aui8_busNr number of the BUS to check (default 0)
  @return true == CAN BUS is in OFF state, else normal operation
*/
bool can_stateGlobalOff(uint8_t aui8_busNr)
{
  if ( aui8_busNr > 1 ) return true;
  updateCanStateTimestamps(aui8_busNr);
  // check if OFF bit is set in CAN control status register
  return (ui16_lastCanBusState[aui8_busNr] & CanStateOff);
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
  @param aui8_busNr number of the BUS to check (default 0)
  @return true == CAN BUS is in blocked state, else normal operation
*/
bool can_stateGlobalBlocked(uint8_t aui8_busNr)
{
  if ( aui8_busNr > 1 ) return true;
  bool b_busBlocked = true;
  const int32_t i32_now = HAL::getTime();
  // sett b_busBlocked to false, if sign for
  // correct work was detected
  if ((i32_now - i32_cinterfLastSuccSend[aui8_busNr])
      < CONFIG_CAN_MAX_SEND_WAIT_TIME)
    b_busBlocked = false;
  // check if successful receive was detected
  if ((i32_now - i32_cinterfLastSuccReceive[aui8_busNr])
      < CONFIG_CAN_MAX_SEND_WAIT_TIME)
    b_busBlocked = false;

  // check if WARN or ERR was detected
  updateCanStateTimestamps(aui8_busNr);
  if (
      (i32_cinterfBeginBusWarnOff[aui8_busNr] < 0)
    ||((i32_now - i32_cinterfBeginBusWarnOff[aui8_busNr])
        < CONFIG_CAN_MAX_CAN_ERR_TIME_BEFORE_SLOWERING)
      )
  { // no WARN or OFF state is active for defined time
    b_busBlocked = false;
  }
  if (
      (i32_cinterfBeginBit1err[aui8_busNr] < 0)
    ||((i32_now - i32_cinterfBeginBit1err[aui8_busNr])
        < CONFIG_CAN_MAX_CAN_ERR_TIME_BEFORE_SLOWERING)
      )
  { // no Bit1Error state is active for defined time
    b_busBlocked = false;
  }
  return b_busBlocked;
}

/**
  deliver the baudrate of the CAN BUS in [kbaud]
  @param aui8_busNr number of the BUS to check (default 0)
  @return BUS load of the last second [kbaud]
*/
int32_t can_stateGlobalBusload(uint8_t aui8_busNr)
{
  if ( aui8_busNr > 1 ) return 0;
  int32_t i32_baudrate = 0;
  for (uint8_t ui8_ind = 0; ui8_ind < 10; ui8_ind++)
  {
    i32_baudrate += (gwCinterfBusLoad[aui8_busNr][ui8_ind] * 8);
  }
  return i32_baudrate;
}

/**
  check if send try of this MsgObj caused an Bit1Error
  (occurs if >1 controller try to send msg with same ident)
  ==> ISO 11783 forces stop of retries in this case for
      adress claim
      @param aui8_busNr number of the BUS to check
      @return true -> Bit1Err occured
*/
bool can_stateGlobalBit1err(uint8_t aui8_busNr)
{
  if ( aui8_busNr > 1 ) return true;
  // if bit1err timestamp is -1 no actual Bit1Err
  // check if WARN or ERR was detected
  updateCanStateTimestamps(aui8_busNr);
  return !(i32_cinterfBeginBit1err[aui8_busNr] < 0);
}

/* ***************************** */
/* ***Specific for one MsgObj*** */
/* ***************************** */

/**
  deliver the timestamp of last successfull CAN send action
  @param aui8_busNr number of the BUS to check  [0..1]
  @param aui8_msgobjNr number of the MsgObj to check [0..13]
  @return timestamp of last successful send
          OR -1 if aui8_msgObjNr corresponds to no valid send obj
*/
int32_t can_stateMsgobjTxok(uint8_t aui8_busNr, uint8_t aui8_msgobjNr)
{
  if ( ( aui8_busNr > 1 ) || ( aui8_msgobjNr> 14 ) ) return 0;
  return i32_cinterfLastSuccSend[aui8_busNr];
}

/**
  check if a send MsgObj can't send msgs from buffer to the
  BUS (detecetd by comparing the inactive time with
  CONFIG_CAN_MAX_SEND_WAIT_TIME (defined in isoaglib_config)
  @param aui8_busNr number of the BUS to check  [0..1]
  @param aui8_msgobjNr number of the MsgObj to check [0..13]
  @return true -> longer than CONFIG_CAN_MAX_SEND_WAIT_TIME no msg sent on BUS
*/
bool can_stateMsgobjSendproblem(uint8_t aui8_busNr, uint8_t aui8_msgobjNr)
{
  if ( ( aui8_busNr > 1 ) || ( aui8_msgobjNr> 14 ) ) return true;
  const int32_t i32_now = HAL::getTime();
  if (((i32_now - can_stateMsgobjTxok(aui8_busNr, aui8_msgobjNr)) > CONFIG_CAN_MAX_SEND_WAIT_TIME)
    && ( can_stateMsgobjBuffercnt(aui8_busNr, aui8_msgobjNr) > 0 ))
    return true;
  else return false;
}

/**
  test if buffer of a MsgObj is full (e.g. no more
  msg can be put into buffer (important for TX objects))
  @param aui8_busNr number of the BUS to check
  @param aui8_msgobjNr number of the MsgObj to check [0..13]
  @return true -> buffer is full -> no further can_send allowed
*/
bool can_stateMsgobjOverflow(uint8_t aui8_busNr, uint8_t aui8_msgobjNr)
{
  if ( ( aui8_busNr > 1 ) || ( aui8_msgobjNr> 14 ) ) return true;
  return ( can_stateMsgobjFreecnt( aui8_busNr, aui8_msgobjNr ) == 0 );
}
/**
  check if MsgObj is currently locked
  @param aui8_busNr number of the BUS to check
  @param aui8_msgobjNr number of the MsgObj to check
  @return true -> MsgObj is currently locked
*/
bool can_stateMsgobjLocked( uint8_t aui8_busNr, uint8_t aui8_msgobjNr )
{
  if ( ( aui8_busNr > 1 ) || ( aui8_msgobjNr> 14 ) ) return true;
  else if ( b_canBufferLock[aui8_busNr][aui8_msgobjNr] ) return true;
  else return false;
}

/**
  deliver amount of messages in buffer
  (interesting for RX objects)
  @param aui8_busNr number of the BUS to check
  @param aui8_msgobjNr number of the MsgObj to check
  @return number of messages in buffer or negative error code:
    HAL_CONFIG_ERR == BUS not initialised
    HAL_RANGE_ERR == wrong BUS or MsgObj number
*/
int16_t can_stateMsgobjBuffercnt(uint8_t aui8_busNr, uint8_t aui8_msgobjNr)
{
  if ( ( aui8_busNr > 1 ) || ( aui8_msgobjNr> 14 ) ) return 0;
  return getCanRingBufferSize( aui8_busNr, aui8_msgobjNr );
}

/**
  deliver amount of messages which can be placed in buffer
  (interesting for TX objects)
  @param aui8_busNr number of the BUS to check
  @param aui8_msgobjNr number of the MsgObj to check
  @return number of messages which can be placed in buffer or negative error code:
    HAL_CONFIG_ERR == BUS not initialised
    HAL_RANGE_ERR == wrong BUS or MsgObj number
*/
int16_t can_stateMsgobjFreecnt(uint8_t aui8_busNr, uint8_t aui8_msgobjNr)
{
  if ( ( aui8_busNr > 1 ) || ( aui8_msgobjNr> 14 ) ) return 0;
  if ( getCanRingBufferMaxSize( aui8_busNr, aui8_msgobjNr ) > getCanRingBufferSize( aui8_busNr, aui8_msgobjNr ) )
    return ( getCanRingBufferMaxSize( aui8_busNr, aui8_msgobjNr ) - getCanRingBufferSize( aui8_busNr, aui8_msgobjNr ) );
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
  @param aui8_busNr number of the BUS to init (default 0)
  @param ab_baudrate baudrate in [kbaud]
  @param aui16_maskStd 11bit global mask
  @param aui32_maskExt 29bit global mask
  @param aui32_maskLastmsg mask of last CAN msg
  @return HAL_NO_ERR == no error;
          HAL_RANGE_ERR == wrong BUS nr or wrong baudrate;
          HAL_WARN_ERR == BUS previously initialised - no problem if only masks had to be changed
*/
int16_t can_configGlobalInit(uint8_t aui8_busNr, uint16_t ab_baudrate, uint16_t aui16_maskStd, uint32_t aui32_maskExt, uint32_t aui32_maskLastmsg)
{
  if ( aui8_busNr > 1 ) {
    char tempString[80];
    sprintf( tempString, "\n\rVerlasse can_configGlobalInit() wegen RangeErr durch BusNr %hd\n\r", aui8_busNr );
    put_rs232String( (const uint8_t *) tempString );
    int32_t i32_waitEnd = HAL::getTime() + 1000;
    while ( i32_waitEnd > HAL::getTime() ) _nop();
    return HAL_RANGE_ERR;
  }
  // init variables
  const int32_t i32_now = HAL::getTime();
  ui8_currentBaudrate[aui8_busNr] =  ab_baudrate;
  i32_cinterfBeginBusWarnOff[aui8_busNr] = -1;
  i32_cinterfBeginBit1err[aui8_busNr] = -1;
  i32_cinterfLastSuccSend[aui8_busNr] = i32_now;
  i32_cinterfLastSuccReceive[aui8_busNr] = i32_now;
  for (uint8_t ui8_ind = 0; ui8_ind < 15; ui8_ind++) {
    ui8_canBufferDirectionState[aui8_busNr][ui8_ind] = 0xFF;
    ui8_canBufferXtd[aui8_busNr][ui8_ind] = 0;
    ui8_canBufferDirectionState[aui8_busNr][ui8_ind] = 0xFF;
    ui16_canBufferMaxSize[aui8_busNr][ui8_ind] = 0;
    resetCanRingBuffer( aui8_busNr, ui8_ind);
    b_appLock[aui8_busNr][ui8_ind] = false;
  }

  gb_cinterfBusLoadSlice[aui8_busNr] = 0;
  b_runningIrqSendProcess[aui8_busNr] = false;
  CNAMESPACE::memset((gwCinterfBusLoad[aui8_busNr]),0,10);

  // now config BUS
  if ( aui8_busNr == 0 ) {
    CAN1_Init( ui8_currentBaudrate[0], EIE_BIT, SIE_BIT, IE_BIT );
  }
  else
  {
    CAN2_Init( ui8_currentBaudrate[1], EIE_BIT, SIE_BIT, IE_BIT );
  }
  return can_configGlobalMask(aui8_busNr, aui16_maskStd, aui32_maskExt, aui32_maskLastmsg);
}

/**
  set the global masks
  (11bit, 29bit and last msg mask)
  @param aui8_busNr number of the BUS to config (default 0)
  @param aui16_maskStd 11bit global mask
  @param aui32_maskExt 29bit global mask
  @param aui32_maskLastmsg mask of last CAN msg
  @return HAL_NO_ERR == no error;
          HAL_RANGE_ERR == wrong BUS nr
*/
int16_t can_configGlobalMask(uint8_t aui8_busNr, uint16_t aui16_maskStd, uint32_t aui32_maskExt, uint32_t aui32_maskLastmsg)
{
  if ( aui8_busNr > 1 ) {
    put_rs232String( "Verlasse can_configGlobalMask mit RangeError\n\r" );
    return HAL_RANGE_ERR;
  }

  // avoid problems with to big values for masks
  const uint16_t cui16_shortMask = ( aui16_maskStd & 0x7FF );
  const uint32_t cui32_longMask = ( aui32_maskExt & 0x1FFFFFFF );
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
  // deactivate all IRQ to inhibit distortion by RS232 or other IRQ
  IEN = 0;      // defined in reg167cs.h
  if ( aui8_busNr == 0 ) {
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
  else if ( aui8_busNr == 1 ) {
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
  @param aui8_busNr number of the BUS to close (default 0)
  @return HAL_NO_ERR == no error;
          HAL_RANGE_ERR == wrong BUS nr;
          HAL_CONFIG_ERR == BUS previously not initialised
*/
int16_t can_configGlobalClose(uint8_t aui8_busNr)
{ // no function found in CAN driver
  // deactivate all IRQ to inhibit distortion by RS232 or other IRQ
  IEN = 0;      // defined in reg167cs.h

  // set close state for all MsgObj as base config of BUS causes invalidation of all active MsgObj
  #if 0
  put_rs232String( "GCl\n\r" );
  #endif
  for (uint8_t ui8_ind = 0; ui8_ind < 15; ui8_ind++) {
    if ( ui8_canBufferDirectionState[aui8_busNr][ui8_ind] == 0xFF ) continue;
    ui8_canBufferDirectionState[aui8_busNr][ui8_ind] = 0xFF;
    b_appLock[aui8_busNr][ui8_ind] = false;
    resetCanRingBuffer( aui8_busNr, ui8_ind );

    // close MsgObjs
    if ( aui8_busNr == 0 ) {
     while ( CAN1_DelMsgObj( ui8_ind ) == 0 ) __HAL::wdTriggern();
    }
    else {
     while ( CAN2_DelMsgObj( ui8_ind ) == 0 ) __HAL::wdTriggern();
    }
  }
  b_runningIrqSendProcess[aui8_busNr] = false;
  // activate all IRQ again
  IEN = 1;      // defined in reg167cs.h
  return HAL_NO_ERR;
}


/** wait until specified timeout or until next CAN message receive
 *  @return true -> there are CAN messages waiting for process. else: return due to timeout
 */
bool can_waitUntilCanReceiveOrTimeout( uint16_t aui16_timeoutInterval )
{
  const int32_t ci32_endWait = HAL::getTime() + aui16_timeoutInterval;
  int32_t i32_waitSlice aui16_timeoutInterval;

  // if greater than 50msec -> divide so that about 10 slices are realized
  if ( i32_waitSlice > 50 ) i32_waitSlice /= 10;
  // if still more than 50msec slice limit to 50
  if ( i32_waitSlice > 50 ) i32_waitSlice = 50;

  while (true)
  {
    for ( unsigned int busInd = 0; busInd < cui32_maxCanBusCnt; busInd++)
    {
      for ( unsigned int msgInd = 0; msgInd < 15; msgInd++ )
      {
        if ( getCanRingBufferSize( busInd, msgInd ) > 0 ) return true;
      }
    }
    if ( HAL::getTime() >= ci32_endWait ) return false;
  }
}

/* ***************************** */
/* ***Specific for one MsgObj*** */
/* ***************************** */

/**
  config a MsgObj
  (buffer size is defined locally in implementation of this interface;
  all received msgs should get a time stamp;
  class __IsoAgLib::Ident_c has ident and type 11/29bit)
  @param aui8_busNr number of the BUS to config
  @param aui8_msgobjNr number of the MsgObj to config
  @param arc_ident filter ident of this MsgObj
  @param ab_rxtx 0==RX receive; 1==TX transmit
  @return HAL_NO_ERR == no error;
          C_BUSY == this MsgObj is already used
          HAL_CONFIG_ERR == BUS not initialised or error during buffer allocation
          HAL_RANGE_ERR == wrong BUS or MsgObj number
*/
int16_t can_configMsgobjInit(uint8_t aui8_busNr, uint8_t aui8_msgobjNr, __IsoAgLib::Ident_c& arc_ident, uint8_t ab_rxtx)
{ // check for input vars
  if ( ( aui8_busNr > 1 ) || ( aui8_msgobjNr> 14 ) ) {
    char tempString[80];
    sprintf( tempString, "\n\rVerlasse can_configMsgobjInit() wegen RangeErr durch BusNr %hd, MsgObjNr %hd\n\r", aui8_busNr, aui8_msgobjNr );
    put_rs232String( (const uint8_t *) tempString );
  int32_t i32_waitEnd = HAL::getTime() + 1000;
  while ( i32_waitEnd > HAL::getTime() ) ;
    return HAL_RANGE_ERR;
  }
  uint32_t ui32_ident = arc_ident.ident();
  ui8_canBufferXtd[aui8_busNr][aui8_msgobjNr] = (arc_ident.identType() == __IsoAgLib::Ident_c::BothIdent)
                    ?DEFAULT_IDENT_TYPE
                    :arc_ident.identType();

  uint8_t ui8_bufferSize = CONFIG_CAN_STD_LOAD_REC_BUF_SIZE_MIN;
  // unlock - if previously locked
  b_canBufferLock[aui8_busNr][aui8_msgobjNr] = false;

  // deactivate all IRQ to inhibit distortion by RS232 or other IRQ
  IEN = 0;      // defined in reg167cs.h
  // deactivate CAN BUS durign re-config
  if ( aui8_busNr == 0 ) {
    CAN1TxReady = FALSE;
    XP0IC = 0x0000;
  }
  else {
    CAN2TxReady = FALSE;
    XP1IC = 0x0000;
  }

  // check if MsgObj is already open, so that it must be closed first
  if ( ui8_canBufferDirectionState[aui8_busNr][aui8_msgobjNr] != 0xFF ) {
    // it's at the moment open
    if ( aui8_busNr == 0 ) {
     while ( CAN1_DelMsgObj( aui8_msgobjNr ) == 0 ) __HAL::wdTriggern();
    }
    else {
     while ( CAN2_DelMsgObj( aui8_msgobjNr ) == 0 ) __HAL::wdTriggern();
    }
  }
  // store wanted direction
  ui8_canBufferDirectionState[aui8_busNr][aui8_msgobjNr] = ab_rxtx;


  if (ab_rxtx == 0)
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
  if ( ( arr_canBuffer[aui8_busNr][aui8_msgobjNr] != NULL               )
    && ( ui16_canBufferMaxSize[aui8_busNr][aui8_msgobjNr] < ui8_bufferSize ) ) {
    // currently existing buffer is smaller than now wanted -> delete old too small
    delete [] ( arr_canBuffer[aui8_busNr][aui8_msgobjNr] );
    arr_canBuffer[aui8_busNr][aui8_msgobjNr] = NULL;
  }
  if ( arr_canBuffer[aui8_busNr][aui8_msgobjNr] == NULL ) {
    // create buffer
    arr_canBuffer[aui8_busNr][aui8_msgobjNr] = new canBufferElement_t[ui8_bufferSize];
    if ( arr_canBuffer[aui8_busNr][aui8_msgobjNr] == NULL ) return HAL_OVERFLOW_ERR;
    ui16_canBufferMaxSize[aui8_busNr][aui8_msgobjNr] = ui8_bufferSize;
  }
  #if 0
  char tempString[80];
  sprintf( tempString, "MsgInit B%hd O%hd RxTx%hd X%hd Id0x%lx\n\r",
           aui8_busNr, aui8_msgobjNr, ab_rxtx, ui8_canBufferXtd[aui8_busNr][aui8_msgobjNr],
           ui32_ident );
  put_rs232String( (const uint8_t *) tempString );
  int32_t i32_waitEnd = HAL::getTime() + 500;
  while ( i32_waitEnd > HAL::getTime() ) tempString[0] = tempString[1] + 1;
  #endif
  if ( aui8_busNr == 0 ) {
    CAN1_DefineMsgObj(aui8_msgobjNr,  ab_rxtx, CAN_INT_ON, ui8_canBufferXtd[aui8_busNr][aui8_msgobjNr], ui32_ident, 8);
    CAN1TxReady = TRUE;
    XP0IC = 0x0073;
  }
  else {
    CAN2_DefineMsgObj(aui8_msgobjNr,  ab_rxtx, CAN_INT_ON, ui8_canBufferXtd[aui8_busNr][aui8_msgobjNr], ui32_ident, 8);
    CAN2TxReady = TRUE;
    XP1IC = 0x0072;
  }
  // activate all IRQ again
  IEN = 1;      // defined in reg167cs.h

  // reset AppLock flag
  b_appLock[aui8_busNr][aui8_msgobjNr] = false;
  resetCanRingBuffer( aui8_busNr, aui8_msgobjNr );

  return HAL_NO_ERR;
}

/**
  change the Ident_c of an already initialised MsgObj
  (class __IsoAgLib::Ident_c has ident and type 11/29bit)
  @param aui8_busNr number of the BUS to config
  @param aui8_msgobjNr number of the MsgObj to config
  @param arc_ident filter ident of this MsgObj
  @return HAL_NO_ERR == no error;
          HAL_CONFIG_ERR == BUS not initialised or ident can't be changed
          HAL_RANGE_ERR == wrong BUS or MsgObj number
*/
int16_t can_configMsgobjChgid(uint8_t aui8_busNr, uint8_t aui8_msgobjNr, __IsoAgLib::Ident_c& arc_ident)
{
  // check for input vars
  if ( ( aui8_busNr > 1 ) || ( aui8_msgobjNr> 14 ) ) return HAL_RANGE_ERR;
  if ( ui8_canBufferDirectionState[aui8_busNr][aui8_msgobjNr] == 0xFF ) return HAL_CONFIG_ERR;
  ui8_canBufferXtd[aui8_busNr][aui8_msgobjNr] = (arc_ident.identType() == __IsoAgLib::Ident_c::BothIdent)
                                                ?DEFAULT_IDENT_TYPE
                                                :arc_ident.identType();

  // unlock - if previously locked
  b_canBufferLock[aui8_busNr][aui8_msgobjNr] = false;

  uint32_t id = arc_ident.ident();
  uint16_t* preg_UAR = (aui8_busNr==0)?(C1UAR1+(aui8_msgobjNr*0x10)):(C2UAR1+(aui8_msgobjNr*0x10));
  uint16_t* preg_LAR = (aui8_busNr==0)?(C1LAR1+(aui8_msgobjNr*0x10)):(C2LAR1+(aui8_msgobjNr*0x10));
  uint16_t ui16_uarVal, ui16_larVal, tempVal;


  if (xtd) /* load Arbitration Registers with XTD ID: */
  {
    /* load Upper Arb. Reg.: */
    id=id<<3;
    tempVal=(unsigned int) (id>>16);
    ui16_uarVal=(tempVal<<8)+(tempVal>>8);

    /* load Lower Arb. Reg. */
    tempVal=(unsigned int) id;
    ui16_larVal=(tempVal<<8)+(tempVal>>8);
  }
  else   /* load Arbitration Registers with STD ID: */
  {
    /* load Upper Arb. Reg.: */
    tempVal=(unsigned int) id;
    tempVal=tempVal<<5;
    ui16_uarVal=(tempVal<<8)+(tempVal>>8);

    /* load Lower Arb. Reg.: */
    ui16_larVal=0x0800;
  }

  // deactivate all IRQ to inhibit distortion by RS232 or other IRQ
  IEN = 0;      // defined in reg167cs.h
  if ( aui8_busNr == 0 ) {
    // deactivate ITQ and state
    XP0IC = 0x0000;
    // set control register to config mode
    C1CSR = ( C1CSR | 0x41 ); // set CCE and INIT to indicate config
    CAN1TxReady = FALSE;
    // now set the UAR and LAR arbitration registers
    *preg_UAR = ui16_uarVal;
    *preg_LAR = ui16_larVal;
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
  else if ( aui8_busNr == 1 ) {
    // deactivate ITQ and state
    XP1IC =  0x0000;
    // set control register to config mode
    C2CSR = ( C2CSR | 0x41 ); // set CCE and INIT to indicate config
    CAN2TxReady = FALSE;
    // now set the UAR and LAR arbitration registers
    *preg_UAR = ui16_uarVal;
    *preg_LAR = ui16_larVal;
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

  // reset AppLock flag
  b_appLock[aui8_busNr][aui8_msgobjNr] = false;

  return HAL_NO_ERR;
}

/**
  lock a MsgObj to avoid further placement of messages into buffer.
  @param aui8_busNr number of the BUS to config
  @param aui8_msgobjNr number of the MsgObj to config
  @param ab_doLock true==lock(default); false==unlock
  @return HAL_NO_ERR == no error;
          HAL_CONFIG_ERR == BUS not initialised or ident can't be changed
          HAL_RANGE_ERR == wrong BUS or MsgObj number
  */
int16_t can_configMsgobjLock( uint8_t aui8_busNr, uint8_t aui8_msgobjNr, bool ab_doLock )
{
  b_canBufferLock[aui8_busNr][aui8_msgobjNr] = ab_doLock;
}

/**
  change the the send rate for one MsgObj by setting the minimum
  pause time between two messages [msec.]
  @param aui8_busNr number of the BUS to config
  @param aui8_msgobjNr number of the MsgObj to config
  @param aui16_minSend minimum send pause between two sent messages [msec.]
  @return HAL_NO_ERR == no error;
          HAL_CONFIG_ERR == BUS not initialised or ident can't be changed
          HAL_RANGE_ERR == wrong BUS or MsgObj number
*/
int16_t can_configMsgobjSendpause(uint8_t aui8_busNr, uint8_t aui8_msgobjNr, uint16_t aui16_minSend)
{
  return HAL_NO_ERR;
}

/**
  close a MsgObj
  @param aui8_busNr number of the BUS to config
  @param aui8_msgobjNr number of the MsgObj to config
  @return HAL_NO_ERR == no error;
          HAL_CONFIG_ERR == BUS not initialised, MsgObj previously not used or buffer memory not freed
          HAL_RANGE_ERR == wrong BUS or MsgObj number
*/
int16_t can_configMsgobjClose(uint8_t aui8_busNr, uint8_t aui8_msgobjNr)
{
  // check for input vars
  if ( ( aui8_busNr > 1 ) || ( aui8_msgobjNr> 14 ) ) return HAL_RANGE_ERR;
  // unlock - if previously locked
  b_canBufferLock[aui8_busNr][aui8_msgobjNr] = false;
  #if 0
  char tempString[80];
  sprintf( tempString, "MsgClose B%hd M%hd\n\r",
           aui8_busNr, aui8_msgobjNr );
  put_rs232String( (const uint8_t *) tempString );
  int32_t i32_waitEnd = HAL::getTime() + 500;
  while ( i32_waitEnd > HAL::getTime() ) tempString[0] = tempString[1] + 1;
  #endif

  // deactivate all IRQ to inhibit distortion by RS232 or other IRQ
  IEN = 0;      // defined in reg167cs.h
  if ( aui8_busNr == 0 ) {
    CAN1TxReady = FALSE;
    while ( CAN1_DelMsgObj( aui8_msgobjNr ) == 0 ) __HAL::wdTriggern();
    XP0IC = 0x0000;
    CAN1TxReady = TRUE;
    XP0IC = 0x0073;
  }
  else {
    CAN2TxReady = FALSE;
    while ( CAN2_DelMsgObj( aui8_msgobjNr ) == 0 ) __HAL::wdTriggern();
    XP1IC = 0x0000;
    CAN2TxReady = TRUE;
    XP1IC = 0x0072;
  }
  // activate all IRQ again
  IEN = 1;      // defined in reg167cs.h

  // reset AppLock flag
  b_appLock[aui8_busNr][aui8_msgobjNr] = false;
  ui8_canBufferDirectionState[aui8_busNr][aui8_msgobjNr] = 0xFF;
  resetCanRingBuffer( aui8_busNr, aui8_msgobjNr );

  return HAL_NO_ERR;
}

/* ***************************************************** */
/* ***************** Use of MsgObj ********************* */
/* ***************************************************** */

/**
  update the CAN BUS load statistic
  @param aui8_busNr BUS number to update
  @param ab_dlc length ot the detected (send or receive) message
*/
void updateCanBusLoad(uint8_t aui8_busNr, uint8_t ab_dlc)
{
  if ( aui8_busNr > 1 ) return;
  int32_t i32_now = HAL::getTime();
  uint8_t b_newSlice = ((i32_now / 100)%10);
  if (gb_cinterfBusLoadSlice[aui8_busNr] != b_newSlice)
  {
    gwCinterfBusLoad[aui8_busNr][b_newSlice] = ab_dlc;
    gb_cinterfBusLoadSlice[aui8_busNr] = b_newSlice;
  }
  else
  {
    gwCinterfBusLoad[aui8_busNr][b_newSlice] += ab_dlc;
  }
}

/**
  send a message via a MsgObj;
  CanPkg_c (or derived object) must provide (virtual)
  functions:
  * MASK_TYPE ident() -> deliver ident value
  * __IsoAgLib::Ident_c::identType_t identType() -> deliver type of ident
  * void getData(MASK_TYPE& rt_ident, uint8_t& rui8_identType,
                 uint8_t& rb_dlcTarget, uint8_t* pb_dataTarget)
    -> put DLC in referenced r_dlc and insert data in uint8_t string pb_data
  @param aui8_busNr number of the BUS to config
  @param aui8_msgobjNr number of the MsgObj to config
  @param apc_data pointer to CanPkg_c instance with data to send
  @return HAL_NO_ERR == no error;
          HAL_CONFIG_ERR == BUS not initialised, MsgObj is no send object
          HAL_NOACT_ERR == BUS OFF
          HAL_OVERFLOW_ERR == send buffer overflowed
          HAL_RANGE_ERR == wrong BUS or MsgObj number
*/
int16_t can_useMsgobjSend(uint8_t aui8_busNr, uint8_t aui8_msgobjNr, __IsoAgLib::CanPkg_c* apc_data)
{
  if ( ( aui8_busNr > 1 ) || ( aui8_msgobjNr> 14 ) ) return HAL_RANGE_ERR;
  // a receive MsgObj is registered with 0xFF in corresponding ui8_cinterfLastSendBufCnt
  if ( ui8_canBufferDirectionState[aui8_busNr][aui8_msgobjNr] != 1 ) return HAL_CONFIG_ERR;
  CAN_buf_Seed_def* p_tempSend = NULL;

  // direct access driver HW if buffer is empty
  if ( isCanRingBufferEmpty( aui8_busNr, aui8_msgobjNr ) ) {
    if ( aui8_busNr == 0 ) {
      // try to lock HW for send - mark successful preperation by writing address of ss_tempSendObj into p_tempSend
      if ( CAN1_RequestMsgObj( aui8_msgobjNr ) != 0 ) p_tempSend = &ss_tempSendObj;
    }
    else {
      // try to lock HW for send - mark successful preperation in b_doDirectSend
      if ( CAN2_RequestMsgObj( aui8_msgobjNr ) != 0 ) p_tempSend = &ss_tempSendObj;
    }
  }
  if ( p_tempSend == NULL ) {
    #if 0
    char tempString[80];
    sprintf( tempString, "Buffer Send with Bus: %hd, MsgObj: %hd, MaxSize: %d, Size: %d, Read: %d, Write: %d, IRQ-Cnt: %lu, TX-IRQ: %lu, RX-IRQ: %lu, State: %x\n\r",
    aui8_busNr, aui8_msgobjNr,
    ui16_canBufferMaxSize[aui8_busNr][aui8_msgobjNr],
    ui16_canBufferSize[aui8_busNr][aui8_msgobjNr],
    ui16_canBufferRead[aui8_busNr][aui8_msgobjNr],
    ui16_canBufferWrite[aui8_busNr][aui8_msgobjNr],
    sui32_irqCnt,
    sui32_txIrqCnt,
    sui32_rxIrqCnt,
    ui16_lastCanBusState[aui8_busNr] );
    put_rs232String( (const uint8_t *) tempString );
  #endif
    // first if buffer is full
    if ( isCanRingBufferFull( aui8_busNr, aui8_msgobjNr ) ) {
    #if 0
    put_rs232String( "Send Buffer Full\n\r" );
    #endif
      return HAL_OVERFLOW_ERR;
  }

    // set AppLock, so that IRQ function can avoid reading a not complete written msg
    // ( this could cause trouble, if the current to be written msg is the first msg in a
    //   previously empty buffer ) - if the buffer contains more elements, a parallel access
    //  causes no problems
    b_appLock[aui8_busNr][aui8_msgobjNr] = true;

    // retrieve pointer to buffer, where next data can be inserted ( don't force overwrite )
    canBufferElement_t* ps_element = writeDataToCanRingBuffer( aui8_busNr, aui8_msgobjNr, false );

    if ( ps_element == NULL ) {
      // maybe meanwhile buffer is full -> exit
      b_appLock[aui8_busNr][aui8_msgobjNr] = false;
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
  // CanPkgExt_c::getData transforms flag data to ident and 8byte string
  uint8_t ui8_type;
  apc_data->getData(p_tempSend->id, ui8_type, p_tempSend->bytes, p_tempSend->databytes);
  // pt_send->dwId = apc_data->ident();
  // if (apc_data->identType() == 1)
  // CanPkg_c::ident() and CanPkg_c::identType() changed to static
  p_tempSend->id = __IsoAgLib::CanPkg_c::ident();
  if (ui8_type == 1)
  { // extended 29bit ident
    updateCanBusLoad(aui8_busNr, (p_tempSend->bytes + 4));
  }
  else
  { // standard 11bit ident
    updateCanBusLoad(aui8_busNr, (p_tempSend->bytes + 2));
  }

  // check if p_tempSend was written with address of ss_tempSendObj for direct write
  if ( p_tempSend != &ss_tempSendObj ) {
    // buffered write detected
    // buffer contains now ready data -> release lock
    b_appLock[aui8_busNr][aui8_msgobjNr] = false;
    // check if currently a send irq process is running which could be now restarted
    if ( b_runningIrqSendProcess[aui8_busNr] == false ) {
      canBufferElement_t* ps_element = NULL;
      // only retry to start send, if corresponding MsgObj is ready to send
      if ( aui8_busNr == 0 ) {
        if ( CAN1_RequestMsgObj( aui8_msgobjNr ) != 0 )
          ps_element = readPopDataFromCanRingBuffer( 0, aui8_msgobjNr );
      }
      else {
        if ( CAN2_RequestMsgObj( aui8_msgobjNr ) != 0 )
          ps_element = readPopDataFromCanRingBuffer( 1, aui8_msgobjNr );
      }
      if ( ps_element != NULL ) {
        // start send the first msg in buffer
        b_runningIrqSendProcess[aui8_busNr] = true;
        p_tempSend = &( ps_element->data );
        if ( aui8_busNr == 0 ) {
          CAN1_WriteMsgObjData( aui8_msgobjNr, p_tempSend );
          CAN1_Transmit( aui8_msgobjNr );
    }
    else {
          CAN2_WriteMsgObjData( aui8_msgobjNr, p_tempSend );
          CAN2_Transmit( aui8_msgobjNr );
    }
      }
    }
  }
  else {
    // direct send detected
    b_runningIrqSendProcess[aui8_busNr] = true;
    if ( aui8_busNr == 0 ) {
      CAN1_WriteMsgObjData( aui8_msgobjNr, p_tempSend );
      CAN1_Transmit( aui8_msgobjNr );
    }
    else {
      CAN2_WriteMsgObjData( aui8_msgobjNr, p_tempSend );
      CAN2_Transmit( aui8_msgobjNr );
    }
  }

  return HAL_NO_ERR;
}

/**
  get the ident of a received message to decide about the further
  processing before the whole data string is retreived
  @param aui8_busNr number of the BUS to config
  @param aui8_msgobjNr number of the MsgObj to config
  @param reflIdent reference to the var, where the ident should be inserted
  @return error code
  HAL_NO_ERR == No problem
  HAL_CONFIG_ERR == BUS not initialised, MsgObj is no RX object
  HAL_NOACT_ERR == BUS OFF
  HAL_OVERFLOW_ERR == send buffer overflowed
  HAL_RANGE_ERR == wrong BUS or MsgObj number
  HAL_WARN_ERR == BUS WARN or no received message
*/
int32_t can_useMsgobjReceivedIdent(uint8_t aui8_busNr, uint8_t aui8_msgobjNr, int32_t &reflIdent)
{
  if ( ( aui8_busNr > 1 ) || ( aui8_msgobjNr> 14 ) ) return HAL_RANGE_ERR;
  if ( isCanRingBufferEmpty( aui8_busNr, aui8_msgobjNr ) ) return HAL_WARN_ERR;
  // a receive MsgObj is registered with 0 in corresponding ui8_canBufferDirectionState
  if ( ui8_canBufferDirectionState[aui8_busNr][aui8_msgobjNr] != 0 ) return HAL_CONFIG_ERR;
  // now write ident of MsgObj at position readPos
  reflIdent = pt_canBufferRead[aui8_busNr][aui8_msgobjNr]->data.id;
  #if 0
  char temp[50];
  if ( ( reflIdent & 0x700 ) == 0x200 ) {
    sprintf( temp, "Id0x%lx T%ld O%hd P%hd\n\r",
           reflIdent, arr_canBuffer[aui8_busNr][aui8_msgobjNr][readPos].timestamp,
       aui8_msgobjNr, readPos );
    put_rs232String( (const uint8_t *) temp );
  }
  #endif
  return HAL_NO_ERR;
}

/**
  transfer front element in buffer into the pointed CanPkg_c;
  DON'T clear this item from buffer.
  @see can_useMsgobjPopFront for explicit clear of this front item
  functions:
  * void setIdent(MASK_TYPE at_ident, Ident_c::identType_t at_type)
    -> set ident arc_ident of received msg in CanPkg_c
  * uint8_t setDataFromString(uint8_t* apb_data, uint8_t ab_dlc)
    -> set DLC in CanPkg_c from ab_dlc and insert data from uint8_t string apb_data
  * void setTime(int32_t ai32_time) -> set receive time
  @param aui8_busNr number of the BUS to config
  @param aui8_msgobjNr number of the MsgObj to config
  @param apc_data pointer to CanPkg_c instance with data to send
  @return HAL_NO_ERR == no error;
          HAL_CONFIG_ERR == BUS not initialised, MsgObj is no RX object
          HAL_NOACT_ERR == BUS OFF
          HAL_OVERFLOW_ERR == send buffer overflowed
          HAL_RANGE_ERR == wrong BUS or MsgObj number
          HAL_WARN_ERR == BUS WARN or no received message
*/
int16_t can_useMsgobjGet(uint8_t aui8_busNr, uint8_t aui8_msgobjNr, __IsoAgLib::CanPkg_c* apc_data)
{
  if ( ( aui8_busNr > 1 ) || ( aui8_msgobjNr> 14 ) ) return HAL_RANGE_ERR;
  if ( isCanRingBufferEmpty( aui8_busNr, aui8_msgobjNr ) ) return HAL_WARN_ERR;
  // a receive MsgObj is registered with 0 in corresponding ui8_canBufferDirectionState
  if ( ui8_canBufferDirectionState[aui8_busNr][aui8_msgobjNr] != 0 ) return HAL_CONFIG_ERR;

  // repeat send tries till the AppLock isn't reset by IRQ function
  do {
    b_appLock[aui8_busNr][aui8_msgobjNr] = true;
    // TEST
    int32_t i32_test;
    can_useMsgobjReceivedIdent( aui8_busNr, aui8_msgobjNr, i32_test );
    // TEST
    // retrieve pointer with data to read
    canBufferElement_t *pt_element = readDataFromCanRingBuffer( aui8_busNr, aui8_msgobjNr );
    // check if valid data was returned
    if ( pt_element == NULL ) return HAL_WARN_ERR;
    // now read data from pointed element of buffer
    __IsoAgLib::CanPkg_c::setIdent(uint32_t( pt_element->data.id ), __IsoAgLib::Ident_c::identType_t(ui8_canBufferXtd[aui8_busNr][aui8_msgobjNr]));
    apc_data->setDataFromString(pt_element->data.databytes, pt_element->data.bytes);
  apc_data->setTime( pt_element->timestamp );
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
        __IsoAgLib::CanPkg_c::ident()    );
      put_rs232String( (const uint8_t *) temp );
  }
  #endif
  } while ( b_appLock[aui8_busNr][aui8_msgobjNr] == false );
  // now flag can be cleared as data is complete read
  b_appLock[aui8_busNr][aui8_msgobjNr] = false;

  return HAL_NO_ERR;
}

/**
  Either register the currenct front item of buffer as not relevant,
  or just pop the front item, as it was processed.
  This explicit pop is needed, as one CAN message shall be served to
  several CanCustomer_c instances, as long as one of them indicates a
  succesfull process of the received message.
  @param aui8_busNr number of the BUS to config
  @param aui8_msgobjNr number of the MsgObj to config
*/
void can_useMsgobjPopFront(uint8_t aui8_busNr, uint8_t aui8_msgobjNr)
{ // simply increment read position without further interpretation
  if ( ( aui8_busNr > 1 ) || ( aui8_msgobjNr> 14 ) ) return;
  popDataFromCanRingBuffer( aui8_busNr, aui8_msgobjNr );
}


/**
  clear th buffer of a MsgObj (e.g. to stop sending retries)
  @param aui8_busNr number of the BUS to config
  @param aui8_msgobjNr number of the MsgObj to config
  @return HAL_NO_ERR == no error;
          HAL_CONFIG_ERR == BUS not initialised
          HAL_RANGE_ERR == wrong BUS or MsgObj number
*/
int16_t can_useMsgobjClear(uint8_t aui8_busNr, uint8_t aui8_msgobjNr)
{
  if ( ( aui8_busNr > 1 ) || ( aui8_msgobjNr> 14 ) ) return HAL_RANGE_ERR;
  // a receive MsgObj is registered with 0 in corresponding ui8_canBufferDirectionState
  if ( ui8_canBufferDirectionState[aui8_busNr][aui8_msgobjNr] != 1 ) return HAL_CONFIG_ERR;
  // reset buffer state
  resetCanRingBuffer( aui8_busNr, aui8_msgobjNr );
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
//          example code written, which can be modified  by user to suit
//          the application.
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
