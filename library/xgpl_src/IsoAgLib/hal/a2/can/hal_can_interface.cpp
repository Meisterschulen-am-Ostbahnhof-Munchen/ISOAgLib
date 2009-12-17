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
 * \file hal/a2/can/hal_can_interface.cpp
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
#include "can_target_extensions.h"
#include "../system/system_target_extensions.h"

#include <IsoAgLib/driver/can/impl/ident_c.h>
#include <IsoAgLib/driver/can/impl/canpkg_c.h>

#define CONFIG_CAN_STD_LOAD_REC_BUF_SIZE_MIN 1

namespace __HAL {
extern "C" {
  /** include the BIOS specific header with the part for CAN into __HAL */
//  #include <commercial_BIOS/bios_a2/Xos20esx.h>
}

//BW, remove Xos20esx.h later
#include "can_target_extensions.h"

/* ************************************* */
/* **** Some Modul Global Variables **** */
/* ************************************* */
static tCanBusStatus t_cinterfCanState;
static tCanObjStatus t_cinterfMsgobjState;
static tCanObjConfig t_cinterfMsgobjConfig;
static tSend t_cinterfMsgobjSend;
static tReceive t_cinterfMsgobjReceive;
static bool b_cinterfBufferedReceivedMsg;

static const uint32_t cui32_maxCanBusCnt = ( HAL_CAN_MAX_BUS_NR + 1 );

static int32_t i32_cinterfBeginBusWarnOff[cui32_maxCanBusCnt];
static int32_t i32_cinterfBeginBit1err[cui32_maxCanBusCnt];
static int32_t i32_cinterfLastSuccSend[cui32_maxCanBusCnt];
static int32_t far i32_cinterfMsgobjSuccSend[cui32_maxCanBusCnt][16];
static int32_t i32_cinterfLastSuccReceive[cui32_maxCanBusCnt];
static uint8_t ui8_cinterfLastSendBufCnt[cui32_maxCanBusCnt][16];
/** array of 100msec. timeslice conters of received and sent msg per BUS [uint8_t] */
static uint16_t gwCinterfBusLoad[cui32_maxCanBusCnt][10];
/** actual index in gwBusLoad */
static uint8_t gb_cinterfBusLoadSlice[cui32_maxCanBusCnt];
__IsoAgLib::Ident_c c_cinterfIdent;

/** store size of each MsgObj - needed to answer the Free Item Cnt */
static uint8_t ui8_cinterfBufSize[cui32_maxCanBusCnt][16];

extern "C" {
/** bool array to control lock state for all MsgObj */
static bool b_canBufferLock[cui32_maxCanBusCnt][15];

/** user defined CAN IRQ Function to decide on inserting or rejecting a received CAN message.
    @param bBus but number [0,1]
    @param bOjekt Message Object number [1...14/15] (send/receive)
    @param tCanregister pointer to the CAN register corresponding to the received message
    @return 0 == don't place this message into the BIOS queue; tCanregister == trigger BIOS to place msg into queue
  */
tCanMsgReg HUGE_MEM * IsoAgLibCanHandler(byte bBus,byte bOjekt,tCanMsgReg HUGE_MEM *tCanregister)
{
  if ( b_canBufferLock[bBus][bOjekt] )
  { // this CAN message shouldn't be placed into the CAN BIOS queue
    return 0;
  }
  else
  { // place this CAN message into the BIOS CAN
    return tCanregister;
  }
}
} // extern "C"



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
  uint16_t ui16_canState;
  getCanBusStatus(aui8_busNr, &t_cinterfCanState); //BW
  ui16_canState = t_cinterfCanState.wCtrlStatusReg;
  // check if WARN bit is set in CAN control status register
  if ((ui16_canState & CanStateWarnOrOff) != 0)
  { // set error state time interval begin
    if (i32_cinterfBeginBusWarnOff[aui8_busNr] < 0)
      i32_cinterfBeginBusWarnOff[aui8_busNr] = get_time();
  }
  else
  { // resets error time begin
    i32_cinterfBeginBusWarnOff[aui8_busNr] = -1;
  }
  // check if LEC state reports bit1error
  if ((ui16_canState & CanStateLecErrMask) == CanStateBit1Err )
  {  // set error state time interval begin
    if (i32_cinterfBeginBit1err[aui8_busNr] < 0)
      i32_cinterfBeginBit1err[aui8_busNr] = get_time();
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
  updateCanStateTimestamps(aui8_busNr);
  uint16_t ui16_canState = t_cinterfCanState.wCtrlStatusReg;
  // check if WARN bit is set in CAN control status register
  return ((ui16_canState & CanStateWarn) != 0);
}

/**
  test if the CAN BUS is in OFF state
  @param aui8_busNr number of the BUS to check (default 0)
  @return true == CAN BUS is in OFF state, else normal operation
*/
bool can_stateGlobalOff(uint8_t aui8_busNr)
{
  updateCanStateTimestamps(aui8_busNr);
  uint16_t ui16_canState = t_cinterfCanState.wCtrlStatusReg;
  // check if OFF bit is set in CAN control status register
  return ((ui16_canState & CanStateOff) != 0);
}
/**
  update the timestamp of last succ receive by
  comparing the cnt of msgs in send buffers
  @param aui8_busNr number of bus to check
*/
void updateSuccSendTimestamp(uint8_t aui8_busNr)
{
  int16_t i16_actCnt;
  int32_t i32_now = get_time();
  // cnt 0xFF ist sign, that this MsgObj isn't configured for send
  // loop from 0..13 and add offset 1 in BIOS call, as BIOS starts counting with 1
  // whereas IsoAgLib starts with 0
  for (uint8_t ui8_ind = 0; ui8_ind < 14; ui8_ind++)
  {
    // test if is given obj is configured for send
    if (ui8_cinterfLastSendBufCnt[aui8_busNr][ui8_ind] == 0xFF)
      break; // end this check if first not send obj is found
    i16_actCnt = getCanMsgBufCount (aui8_busNr, (ui8_ind+1)); //BW
    if ((i16_actCnt >= 0) &&
        (i16_actCnt < ui8_cinterfLastSendBufCnt[aui8_busNr][ui8_ind])
        )
    { // since last send call at least one msg sent
      i32_cinterfMsgobjSuccSend[aui8_busNr][ui8_ind] = i32_now;
      i32_cinterfLastSuccSend[aui8_busNr] = i32_now;
      ui8_cinterfLastSendBufCnt[aui8_busNr][ui8_ind] = i16_actCnt;
    }
  }
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
  bool b_busBlocked = true;
  int32_t i32_now = get_time();
  // sett b_busBlocked to false, if sign for
  // correct work was detected
  // check if successful send was detected
  updateSuccSendTimestamp(aui8_busNr);
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
{ // if bit1err timestamp is -1 no actual Bit1Err
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
  updateSuccSendTimestamp(aui8_busNr);
  return i32_cinterfMsgobjSuccSend[aui8_busNr][aui8_msgobjNr];
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
  int32_t i32_now = get_time();
  // add offset 1 to aui8_msgobjNr as ESX BIOS starts counting with 1
  // whereas IsoAgLib starts with 0
  if (((i32_now - can_stateMsgobjTxok(aui8_busNr, aui8_msgobjNr)) > CONFIG_CAN_MAX_SEND_WAIT_TIME)
    && ( getCanMsgBufCount(aui8_busNr, (aui8_msgobjNr+1)) > 0 ))   //BW
    return true;
  else return false;
}

/**
  test if buffer of a MsgObj is full (e.g. no more
  msg can be put into buffer (important for TX objects))
  @param aui8_busNr number of the BUS to check
  @param aui8_msgobjNr number of the MsgObj to check [0..14]
  @return true -> buffer is full -> no further can_send allowed
*/
bool can_stateMsgobjOverflow(uint8_t aui8_busNr, uint8_t aui8_msgobjNr)
{ // map aui8_msgobjNr 1 to 0
  bool b_overflow = false;
  // add offset 1 to aui8_msgobjNr as ESX BIOS starts counting with 1
  // whereas IsoAgLib starts with 0
  //BW
  if (getCanObjStatus(aui8_busNr, (aui8_msgobjNr+1), &t_cinterfMsgobjState) == HAL_NO_ERR)
  {
    if (t_cinterfMsgobjState.bOverflow == 1) b_overflow = true;
    updateSuccSendTimestamp(aui8_busNr);
  }
  return b_overflow;
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
  // add offset 1 to aui8_msgobjNr as ESX BIOS starts counting with 1
  // whereas IsoAgLib starts with 0
  return getCanMsgBufCount(aui8_busNr, (aui8_msgobjNr+1));	 //BW
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
{ // add offset 1 to aui8_msgobjNr as ESX BIOS starts counting with 1
  // whereas IsoAgLib starts with 0
  int16_t i16_msgcnt = getCanMsgBufCount(aui8_busNr, (aui8_msgobjNr+1));//BW
  if (i16_msgcnt < 0)
    return i16_msgcnt;
  else
    return ui8_cinterfBufSize[aui8_busNr][aui8_msgobjNr] - i16_msgcnt;
}
/**
  check if MsgObj is currently locked
  @param aui8_busNr number of the BUS to check
  @param aui8_msgobjNr number of the MsgObj to check
  @return true -> MsgObj is currently locked
*/
bool can_stateMsgobjLocked( uint8_t aui8_busNr, uint8_t aui8_msgobjNr )
{
  if ( ( aui8_busNr >= cui32_maxCanBusCnt ) || ( aui8_msgobjNr> 14 ) ) return true;
  return b_canBufferLock[aui8_busNr][aui8_msgobjNr];
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
  // init variables
  int32_t i32_now = get_time();
  b_cinterfBufferedReceivedMsg = false;
  i32_cinterfBeginBusWarnOff[aui8_busNr] = -1;
  i32_cinterfBeginBit1err[aui8_busNr] = -1;
  i32_cinterfLastSuccSend[aui8_busNr] = i32_now;
  i32_cinterfLastSuccReceive[aui8_busNr] = i32_now;
  // cnt 0xFF ist sign, that this MsgObj isn't configured for send
  CNAMESPACE::memset((ui8_cinterfLastSendBufCnt[aui8_busNr]), 0xFF, 15);
  for (uint8_t ui8_ind = 0; ui8_ind < 15; ui8_ind++)
  {
    i32_cinterfMsgobjSuccSend[aui8_busNr][ui8_ind] = i32_now;
    b_canBufferLock[aui8_busNr][ui8_ind] = false;
  }

  gb_cinterfBusLoadSlice[aui8_busNr] = 0;
  CNAMESPACE::memset((gwCinterfBusLoad[aui8_busNr]),0,10);

  // now config BUS
  //BW
  return initCan(aui8_busNr, aui16_maskStd, aui32_maskExt, aui32_maskLastmsg, ab_baudrate);
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
  getCanBusStatus(aui8_busNr, &t_cinterfCanState); //BW
  // the STW BIOS init_can simply changes the global masks, and ignores the bitrate, when init_can is called for an already
  // configured CAN BUS
  //BW
  int16_t i16_retVal = initCan(aui8_busNr, aui16_maskStd, aui32_maskExt, aui32_maskLastmsg, t_cinterfCanState.wBitrate);
  if (i16_retVal == HAL_WARN_ERR) i16_retVal = HAL_NO_ERR;
  return i16_retVal;
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
{ //BW
  return closeCan(aui8_busNr);
}


/** wait until specified timeout or until next CAN message receive
 *  @return true -> there are CAN messages waiting for process. else: return due to timeout
 */
bool can_waitUntilCanReceiveOrTimeout( uint16_t aui16_timeoutInterval )
{
  const int32_t ci32_endWait = get_time() + aui16_timeoutInterval;
  int32_t i32_waitSlice = aui16_timeoutInterval;

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
        if ( getCanMsgBufCount(busInd, (msgInd+1)) > 0 ) return true; //BW
      }
    }
    delay_us( i32_waitSlice * 1000 );
    if ( get_time() >= ci32_endWait ) return false;
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
{
  tCanObjConfig* pt_config = &t_cinterfMsgobjConfig;
  pt_config->dwId = arc_ident.ident();

  if (arc_ident.identType() == __IsoAgLib::Ident_c::BothIdent)
    pt_config->bXtd = DEFAULT_IDENT_TYPE;
  else pt_config->bXtd = arc_ident.identType();

  if (ab_rxtx == 0)
  { // receive
    ui8_cinterfLastSendBufCnt[aui8_busNr][aui8_msgobjNr] = 0xFF;
    pt_config->bMsgType = RX;
    //BW, 4/18/06, no use for A2 //pt_config->pfIrqFunction = IsoAgLibCanHandler;
    pt_config->wNumberMsgs = CONFIG_CAN_STD_LOAD_REC_BUF_SIZE_MIN;
  }
  else
  { // send
    ui8_cinterfLastSendBufCnt[aui8_busNr][aui8_msgobjNr] = 0;
    pt_config->bMsgType = TX;
//BW    pt_config->pfIrqFunction = 0;
    pt_config->wNumberMsgs = CONFIG_CAN_SEND_BUFFER_SIZE;
  }
  ui8_cinterfBufSize[aui8_busNr][aui8_msgobjNr] = pt_config->wNumberMsgs;
  b_canBufferLock[aui8_busNr][aui8_msgobjNr] = false;
  pt_config->bTimeStamped = true;
  pt_config->wPause = 0;
  pt_config->pfIrqFunction = 0;	//BW, added according to TaskingController VC7 project


  // add offset 1 to aui8_msgobjNr as ESX BIOS starts counting with 1
  // whereas IsoAgLib starts with 0
  //BW
  return configCanObj(aui8_busNr, (aui8_msgobjNr+1), pt_config);
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
{ // add offset 1 to aui8_msgobjNr as ESX BIOS starts counting with 1
  // whereas IsoAgLib starts with 0
  if ( ( aui8_busNr >= cui32_maxCanBusCnt ) || ( aui8_msgobjNr> 14 ) ) return HAL_RANGE_ERR;
  b_canBufferLock[aui8_busNr][aui8_msgobjNr] = false;
  //BW
  return chgCanObjId(aui8_busNr, (aui8_msgobjNr+1), arc_ident.ident(), arc_ident.identType());
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
  if ( ( aui8_busNr >= cui32_maxCanBusCnt ) || ( aui8_msgobjNr> 14 ) ) return HAL_RANGE_ERR;

  #ifdef DEBUG
  char temp[30];
  STL_NAMESPACE::sprintf( temp, "Lock: %d, Bus %hd, MsgObj: %hd\r\n", ab_doLock, aui8_busNr, aui8_msgobjNr );
  __HAL::put_rs232_string( (uint8_t*)temp );
  #endif

  // store the lock state into the bool array
  b_canBufferLock[aui8_busNr][aui8_msgobjNr] = ab_doLock;

  return HAL_NO_ERR;
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
  // add offset 1 to aui8_msgobjNr as ESX BIOS starts counting with 1
  // whereas IsoAgLib starts with 0
  return chgCanObjPause(aui8_busNr, (aui8_msgobjNr+1), aui16_minSend); //BW
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
{ // add offset 1 to aui8_msgobjNr as ESX BIOS starts counting with 1
  // whereas IsoAgLib starts with 0
  if ( ( aui8_busNr >= cui32_maxCanBusCnt ) || ( aui8_msgobjNr> 14 ) ) return HAL_RANGE_ERR;
  b_canBufferLock[aui8_busNr][aui8_msgobjNr] = false;
  return closeCanObj(aui8_busNr, (aui8_msgobjNr+1));
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
  int32_t i32_now = get_time();
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
{ // check if some msg were sent from buffer

  tSend* pt_send = &t_cinterfMsgobjSend;
  uint8_t b_count = ui8_cinterfLastSendBufCnt[aui8_busNr][aui8_msgobjNr];

  updateSuccSendTimestamp(aui8_busNr);
  b_count = ui8_cinterfLastSendBufCnt[aui8_busNr][aui8_msgobjNr];
  // CanPkgExt_c::getData transforms flag data to ident and 8byte string
  apc_data->getData(pt_send->dwId, pt_send->bXtd, pt_send->bDlc, pt_send->abData);

  // pt_send->dwId = apc_data->ident();
  // if (apc_data->identType() == 1)
  // CanPkg_c::ident() and CanPkg_c::identType() changed to static
  // pt_send->dwId = __IsoAgLib::CanPkg_c::ident();
  if (pt_send->bXtd == 1)
  { // extended 29bit ident
    updateCanBusLoad(aui8_busNr, (pt_send->bDlc + 4));
    // pt_send->bXtd = 1;
  }
  else
  { // standard 11bit ident
    // pt_send->bXtd = 0;
    updateCanBusLoad(aui8_busNr, (pt_send->bDlc + 2));
  }
  // increase counter of to be sent msg in buffer
  ui8_cinterfLastSendBufCnt[aui8_busNr][aui8_msgobjNr] = b_count + 1;
  #if 0
  if ( ( pt_send->bDlc == 3 )
    && ( aui8_busNr == 1 )
    && ( pt_send->bXtd == 0 )
    ) {
    IsoAgLib::getIrs232Instance()
      << "Alive Msg?? -\n\rIdent:" << pt_send->dwId << ", MsgObj Nr: "
      << uint16_t( aui8_msgobjNr ) << "\n\r";
  }
  #endif

  #ifdef DEBUG
  static uint8_t lastSendData[CAN_BUS_CNT][2][8];
  static uint8_t lastSendLen[CAN_BUS_CNT][2];
  static uint32_t lastSendIdent[CAN_BUS_CNT][2];
  static uint8_t lastSendXtd[CAN_BUS_CNT][2];
  static int32_t lastSendTime[CAN_BUS_CNT][2];

  if ( ( lastSendLen[aui8_busNr][aui8_msgobjNr] == pt_send->bDlc )
    && ( lastSendIdent[aui8_busNr][aui8_msgobjNr] == pt_send->dwId )
    && ( lastSendXtd[aui8_busNr][aui8_msgobjNr] == pt_send->bXtd )
    && ( STL_NAMESPACE::memcmp( lastSendData[aui8_busNr][aui8_msgobjNr], pt_send->abData, pt_send->bDlc ) == 0 ) )
  { // gleich
    static char temp[100];
    const int32_t ci_deltaTime = ( __HAL::get_time() - lastSendTime[aui8_busNr][aui8_msgobjNr] );
    STL_NAMESPACE::sprintf( temp, "Same Msg at Bus %hd, MsgObj %hd, Ident: 0x%lx,  TimeDelta %ld, DLC: %hd\r\n",
      aui8_busNr, aui8_msgobjNr, pt_send->dwId,
      ci_deltaTime, pt_send->bDlc );
    while ( ( 1000 - __HAL::get_rs232_tx_buf_count() ) < STL_NAMESPACE::strlen( temp ) ) __HAL::wd_triggern();
    __HAL::put_rs232_string( (uint8_t*)temp );
    STL_NAMESPACE::sprintf( temp, "0x%hx, 0x%hx, 0x%hx, 0x%hx, 0x%hx, 0x%hx, 0x%hx, 0x%hx\r\n",
      pt_send->abData[0], pt_send->abData[1], pt_send->abData[2], pt_send->abData[3],
      pt_send->abData[4], pt_send->abData[5], pt_send->abData[6], pt_send->abData[7] );
    while ( ( 1000 - __HAL::get_rs232_tx_buf_count() ) < STL_NAMESPACE::strlen( temp ) ) __HAL::wd_triggern();
    __HAL::put_rs232_string( (uint8_t*)temp );
  }
  // copy
  STL_NAMESPACE::memcpy( lastSendData[aui8_busNr][aui8_msgobjNr], pt_send->abData, pt_send->bDlc );
  lastSendLen[aui8_busNr][aui8_msgobjNr] = pt_send->bDlc;
  lastSendIdent[aui8_busNr][aui8_msgobjNr] = pt_send->dwId;
  lastSendXtd[aui8_busNr][aui8_msgobjNr] = pt_send->bXtd;
  lastSendTime[aui8_busNr][aui8_msgobjNr] = __HAL::get_time();

  #if CAN_BUS_CNT == 1
  static uint16_t minFreeSendItem[1][2] = {0xFFFF, 0xFFFF};
  #elif CAN_BUS_CNT == 2
  static uint16_t minFreeSendItem[CAN_BUS_CNT][2] = {{0xFFFF, 0xFFFF},{0xFFFF, 0xFFFF}};
  #else
  static uint16_t minFreeSendItem[CAN_BUS_CNT][2] = {{0xFFFF, 0xFFFF},{0xFFFF, 0xFFFF}};
  if ( ( minFreeSendItem[0][0] == 0xFFFF ) && ( minFreeSendItem[1][0] == 0xFFFF )
    && ( minFreeSendItem[0][1] == 0xFFFF ) && ( minFreeSendItem[1][1] == 0xFFFF ))
  {
    for ( uint16_t ind = 0; ind < CAN_BUS_CNT; ind++ )
    {
      minFreeSendItem[ind][0] = minFreeSendItem[ind][1] = 0xFFFF;
    }
  }
  #endif
  const uint16_t freeItems = CONFIG_CAN_SEND_BUFFER_SIZE - __HAL::getCanMsgBufCount( aui8_busNr, (aui8_msgobjNr+1) ); //BW
  if ( minFreeSendItem[aui8_busNr][aui8_msgobjNr] > freeItems )
  {
    static char temp[100];
    minFreeSendItem[aui8_busNr][aui8_msgobjNr] = freeItems;
    STL_NAMESPACE::sprintf( temp, "New Min Send FreeBuf Bus %hd, MsgObj %hd, Free %d\r\n",
      aui8_busNr, aui8_msgobjNr, freeItems );
    while ( ( 1000 - __HAL::get_rs232_tx_buf_count() ) < STL_NAMESPACE::strlen( temp ) ) __HAL::wd_triggern();
    __HAL::put_rs232_string( (uint8_t*)temp );
  }
  #endif // end of DEBUG
  // add offset 1 to aui8_msgobjNr as ESX BIOS starts counting with 1
  // whereas IsoAgLib starts with 0
  return sendCanMsg(aui8_busNr, (aui8_msgobjNr+1), pt_send);
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
  tReceive* pt_receive = &t_cinterfMsgobjReceive;
  int16_t i16_retVal = HAL_NO_ERR;
  // only take new msg from BIOS buffer if not previously
  // buffered for detecting of the received ident
  if (!b_cinterfBufferedReceivedMsg)
  { // add offset 1 to aui8_msgobjNr as ESX BIOS starts counting with 1
    // whereas IsoAgLib starts with 0
    i16_retVal = getCanMsg(aui8_busNr, (aui8_msgobjNr+1), pt_receive);
  }
  if ((i16_retVal == HAL_NO_ERR) || (HAL_OVERFLOW_ERR) || (HAL_WARN_ERR))
  {
    if (pt_receive->tReceiveTime.l1ms == 0)
      i32_cinterfLastSuccReceive[aui8_busNr] = get_time();
    else
      i32_cinterfLastSuccReceive[aui8_busNr] = pt_receive->tReceiveTime.l1ms;
    b_cinterfBufferedReceivedMsg = true;
    reflIdent = pt_receive->dwId;
  }
  return i16_retVal;
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
  tReceive* pt_receive = &t_cinterfMsgobjReceive;
  int16_t i16_retVal = HAL_NO_ERR;
  // only take new msg from BIOS buffer if not previously
  // buffered for detecting of the received ident
  if (!b_cinterfBufferedReceivedMsg)
  { // add offset 1 to aui8_msgobjNr as ESX BIOS starts counting with 1
    // whereas IsoAgLib starts with 0
    i16_retVal = getCanMsg(aui8_busNr, (aui8_msgobjNr+1), pt_receive); //BW
  }

  if ((i16_retVal == HAL_NO_ERR) || (HAL_OVERFLOW_ERR) || (HAL_WARN_ERR))
  {
    b_cinterfBufferedReceivedMsg = true;
    if (pt_receive->tReceiveTime.l1ms == 0)
    {
      i32_cinterfLastSuccReceive[aui8_busNr] = get_time();
      // apc_data->setTime(i32_cinterfLastSuccReceive[aui8_busNr]);
      // CanPkg_c::setTime changed to static
      __IsoAgLib::CanPkg_c::setTime(i32_cinterfLastSuccReceive[aui8_busNr]);
    }
    else
    {
      i32_cinterfLastSuccReceive[aui8_busNr] = pt_receive->tReceiveTime.l1ms;
      // apc_data->setTime(pt_receive->tReceiveTime.l1ms);
      // CanPkg_c::setTime changed to static
      __IsoAgLib::CanPkg_c::setTime(pt_receive->tReceiveTime.l1ms);
    }

    __IsoAgLib::Ident_c::identType_t idType;
    if (pt_receive->bXtd == true)
    { // extended 29bit ident
      idType = __IsoAgLib::Ident_c::ExtendedIdent;
      updateCanBusLoad(aui8_busNr, (pt_receive->bDlc + 4));
    }
    else
    { // standard  11bit ident
      idType = __IsoAgLib::Ident_c::StandardIdent;
      updateCanBusLoad(aui8_busNr, (pt_receive->bDlc + 2));
    }
    // apc_data->setIdent(pt_receive->dwId, idType);
    // CanPkg_c::setIdent changed to static member function
    __IsoAgLib::CanPkg_c::setIdent(pt_receive->dwId, idType);
    apc_data->setDataFromString(pt_receive->abData, pt_receive->bDlc);
  }
  return i16_retVal;
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
{
  b_cinterfBufferedReceivedMsg = false;
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
  // add offset 1 to aui8_msgobjNr as ESX BIOS starts counting with 1
  // whereas IsoAgLib starts with 0
  return clearCanObjBuf(aui8_busNr, (aui8_msgobjNr+1));//BW
}
} // end of namespace
