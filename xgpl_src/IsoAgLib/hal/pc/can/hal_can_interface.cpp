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
 * This way the the adaptation of the IsoAgLib can be
 * better restricted to the various BIOS specific the
 * directories xgpl_src/IsoAgLib/hal/can/ .
 * ********************************************************** */

#include "hal_can_interface.h"
#include "../config.h"
#include "../errcodes.h"

#include "can_target_extensions.h"
#include "can_typedef.h"
#include "../system/system.h"
#include <IsoAgLib/driver/can/impl/ident_c.h>
#include <IsoAgLib/util/impl/canpkg_c.h>

namespace __HAL {
/* ************************************* */
/* **** Some Modul Global Variables **** */
/* ************************************* */
static tCanBusStatus t_cinterfCanState;
// static tCanObjStatus t_cinterfMsgobjState;
static tCanObjConfig t_cinterfMsgobjConfig;
static tSend t_cinterfMsgobjSend;
static tReceive t_cinterfMsgobjReceive;
static bool b_cinterfBufferedReceivedMsg;

static const uint32_t cui32_maxCanBusCnt = ( HAL_CAN_MAX_BUS_NR + 1 );

static int32_t i32_cinterfBeginBusWarnOff[cui32_maxCanBusCnt];
static int32_t i32_cinterfBeginBit1err[cui32_maxCanBusCnt];
static int32_t i32_cinterfLastSuccSend[cui32_maxCanBusCnt];
static int32_t i32_cinterfLastSuccReceive[cui32_maxCanBusCnt];
/** array of 100msec. timeslice conters of received and sent msg per BUS [uint8_t] */
static uint16_t gwCinterfBusLoad[cui32_maxCanBusCnt][10];
/** actual index in gwBusLoad */
static uint8_t gb_cinterfBusLoadSlice[cui32_maxCanBusCnt];
__IsoAgLib::Ident_c c_cinterfIdent;

typedef struct {

uint8_t ui8_cinterfLastSendBufCnt;
int32_t i32_cinterfMsgobjSuccSend;
uint8_t ui8_cinterfBufSize;

bool b_canObjConfigured;

} tHalCan;

//typedef STL_NAMESPACE::vector<tMsgObj> ArrMsgObj;
//ArrMsgObj arrMsgObj[cui32_maxCanBusCnt];
static STL_NAMESPACE::vector<tHalCan> arrHalCan[cui32_maxCanBusCnt];

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
  getCanBusStatus(aui8_busNr, &t_cinterfCanState);
  ui16_canState = t_cinterfCanState.wCtrlStatusReg;
  // check if WARN bit is set in CAN control status register
  if ((ui16_canState & CanStateWarnOrOff) != 0)
  { // set error state time interval begin
    if (i32_cinterfBeginBusWarnOff[aui8_busNr] < 0)
      i32_cinterfBeginBusWarnOff[aui8_busNr] = getTime();
  }
  else
  { // resets error time begin
    i32_cinterfBeginBusWarnOff[aui8_busNr] = -1;
  }
  // check if LEC state reports bit1error
  if ((ui16_canState & CanStateLecErrMask) == CanStateBit1Err)
  {  // set error state time interval begin
    if (i32_cinterfBeginBit1err[aui8_busNr] < 0)
      i32_cinterfBeginBit1err[aui8_busNr] = getTime();
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
  return ((ui16_canState & CanStateWarn) != 0)?true:false;
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
  return ((ui16_canState & CanStateOff) != 0)?true:false;
}
/**
  update the timestamp of last succ receive by
  comparing the cnt of msgs in send buffers
  @param aui8_busNr number of bus to check
*/
void updateSuccSendTimestamp(uint8_t aui8_busNr)
{
  int16_t i16_actCnt;
  int32_t i32_now = getTime();
  // cnt 0xFF ist sign, that this MsgObj isn't configured for send
  for (uint8_t ui8_ind = 0; ui8_ind < arrHalCan[aui8_busNr].size()-1; ui8_ind++)
  {
    // test if is given obj is configured for send
    if (arrHalCan[aui8_busNr][ui8_ind].ui8_cinterfLastSendBufCnt == 0xFF)
      break; // end this check if first not send obj is found
    i16_actCnt = getCanMsgBufCount (aui8_busNr, (ui8_ind+1));
    if ((i16_actCnt >= 0) &&
        (i16_actCnt < arrHalCan[aui8_busNr][ui8_ind].ui8_cinterfLastSendBufCnt)
        )
    { // since last send call at least one msg sent
      arrHalCan[aui8_busNr][ui8_ind].i32_cinterfMsgobjSuccSend = i32_now;
      i32_cinterfLastSuccSend[aui8_busNr] = i32_now;
      arrHalCan[aui8_busNr][ui8_ind].ui8_cinterfLastSendBufCnt = uint8_t(i16_actCnt);
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
  int32_t i32_now = getTime();
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
  return (i32_cinterfBeginBit1err[aui8_busNr] < 0)?false:true;
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
  return arrHalCan[aui8_busNr][(aui8_msgobjNr)].i32_cinterfMsgobjSuccSend;
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
  int32_t i32_now = getTime();
  if (((i32_now - can_stateMsgobjTxok(aui8_busNr, aui8_msgobjNr)) > CONFIG_CAN_MAX_SEND_WAIT_TIME)
    && ( getCanMsgBufCount(aui8_busNr, aui8_msgobjNr) > 0 ))
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
  bool b_overflow = (aui8_msgobjNr < 15)?false:true;
    updateSuccSendTimestamp(aui8_busNr);
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
  return getCanMsgBufCount(aui8_busNr, aui8_msgobjNr);
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
  int16_t i16_msgcnt = getCanMsgBufCount(aui8_busNr, aui8_msgobjNr);
  if ((i16_msgcnt == HAL_CONFIG_ERR) || (i16_msgcnt == HAL_RANGE_ERR)) return i16_msgcnt;
  else return ( arrHalCan[aui8_busNr][aui8_msgobjNr].ui8_cinterfBufSize - i16_msgcnt);
}

/**
  check if MsgObj is currently locked
  @param aui8_busNr number of the BUS to check
  @param aui8_msgobjNr number of the MsgObj to check
  @return true -> MsgObj is currently locked
*/
bool can_stateMsgobjLocked( uint8_t aui8_busNr, uint8_t aui8_msgobjNr )
{
  return getCanMsgObjLocked( aui8_busNr, aui8_msgobjNr);
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
int16_t can_configGlobalInit(uint8_t aui8_busNr, uint16_t ab_baudrate, uint16_t ab_maskStd, uint32_t aui32_maskExt, uint32_t aui32_maskLastmsg)
{
  // init variables
  int32_t i32_now = getTime();
  b_cinterfBufferedReceivedMsg = false;
  i32_cinterfBeginBusWarnOff[aui8_busNr] = -1;
  i32_cinterfBeginBit1err[aui8_busNr] = -1;
  i32_cinterfLastSuccSend[aui8_busNr] = i32_now;
  i32_cinterfLastSuccReceive[aui8_busNr] = i32_now;
  // cnt 0xFF ist sign, that this MsgObj isn't configured for send
  //memset((ui8_cinterfLastSendBufCnt[aui8_busNr]), 0xFF, 15);
#ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
  arrHalCan[aui8_busNr].resize(15);
  for (uint8_t ui8_ind = 0; ui8_ind < 15; ui8_ind++) {
      arrHalCan[aui8_busNr][ui8_ind].i32_cinterfMsgobjSuccSend = i32_now;
      arrHalCan[aui8_busNr][ui8_ind].ui8_cinterfLastSendBufCnt = 0xFF;
  }
#endif

  gb_cinterfBusLoadSlice[aui8_busNr] = 0;
  memset((gwCinterfBusLoad[aui8_busNr]),0,10);

  // now config BUS
  return init_can(aui8_busNr, ab_maskStd, aui32_maskExt, aui32_maskLastmsg, ab_baudrate);
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
int16_t can_configGlobalMask(uint8_t aui8_busNr, uint16_t ab_maskStd, uint32_t aui32_maskExt, uint32_t aui32_maskLastmsg)
{
  getCanBusStatus(aui8_busNr, &t_cinterfCanState);
  int16_t i16_retVal = changeGlobalMask(aui8_busNr, ab_maskStd, aui32_maskExt, aui32_maskLastmsg);
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
{
  arrHalCan[aui8_busNr].clear();
  return closeCan(aui8_busNr);
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
int16_t can_configMsgobjInit(uint8_t aui8_busNr,
        uint8_t aui8_msgobjNr,
        __IsoAgLib::Ident_c& arc_ident,
#ifdef SYSTEM_WITH_ENHANCED_CAN_HAL
        __IsoAgLib::Ident_c& arc_mask,
#endif
        uint8_t ab_rxtx)
{
  tCanObjConfig* pt_config = &t_cinterfMsgobjConfig;
  pt_config->dwId = arc_ident.ident();

#ifdef SYSTEM_WITH_ENHANCED_CAN_HAL
  pt_config->mask = arc_mask.ident();
#endif

#ifdef SYSTEM_WITH_ENHANCED_CAN_HAL
  // add a new element in the vector (simply resize with old size + 1)
  //arrHalCan[aui8_busNr].resize(arrHalCan[aui8_busNr].size() + 1);
  if (aui8_msgobjNr >= arrHalCan[aui8_busNr].size()) {
      // add new elements in the vector with resize
      arrHalCan[aui8_busNr].resize(aui8_msgobjNr + 1);
  } else {
      // reconfigure element
  }

  arrHalCan[aui8_busNr][aui8_msgobjNr].b_canObjConfigured = true;
  int32_t i32_now = getTime();
  arrHalCan[aui8_busNr][aui8_msgobjNr].i32_cinterfMsgobjSuccSend = i32_now;
#endif

  if (arc_ident.identType() == __IsoAgLib::Ident_c::BothIdent)
    pt_config->bXtd = DEFAULT_IDENT_TYPE;
  else pt_config->bXtd = arc_ident.identType();
  if (ab_rxtx == 0)
  { // receive
    arrHalCan[aui8_busNr][(aui8_msgobjNr)].ui8_cinterfLastSendBufCnt = 0xFF;
    pt_config->bMsgType = RX;
    pt_config->wNumberMsgs = CONFIG_CAN_STD_LOAD_REC_BUF_SIZE_MIN;
    const uint32_t highLoadCheckList[] = CONFIG_CAN_HIGH_LOAD_IDENT_LIST ;
    for ( uint8_t ind = 0; ind < CONFIG_CAN_HIGH_LOAD_IDENT_CNT; ind++ )
    {
      if ( highLoadCheckList[ind] == pt_config->dwId )
      {
        pt_config->wNumberMsgs = CONFIG_CAN_HIGH_LOAD_REC_BUF_SIZE_MIN;
        break;
      }
    }
  }
  else
  { // send
    arrHalCan[aui8_busNr][(aui8_msgobjNr)].ui8_cinterfLastSendBufCnt = 0;
    pt_config->bMsgType = TX;
    pt_config->wNumberMsgs = CONFIG_CAN_SEND_BUFFER_SIZE;
  }
  arrHalCan[aui8_busNr][aui8_msgobjNr].ui8_cinterfBufSize = uint8_t(pt_config->wNumberMsgs);
  pt_config->bTimeStamped = true;
  pt_config->wPause = 0;
  pt_config->pfIrqFunction = 0;

  return configCanObj(aui8_busNr, aui8_msgobjNr, pt_config);
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
#ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
int16_t can_configMsgobjChgid(uint8_t aui8_busNr, uint8_t aui8_msgobjNr, __IsoAgLib::Ident_c& arc_ident)
{
  return chgCanObjId(aui8_busNr, aui8_msgobjNr, arc_ident.ident(), arc_ident.identType());
}
#else
int16_t can_configMsgobjChgid(uint8_t aui8_busNr, uint8_t aui8_msgobjNr, __IsoAgLib::Ident_c& arc_ident, __IsoAgLib::Ident_c& arc_mask)
{
  return chgCanObjId(aui8_busNr, aui8_msgobjNr, arc_ident.ident(), arc_mask.ident(), arc_ident.identType());
}
#endif

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
  return lockCanObj( aui8_busNr, aui8_msgobjNr, ab_doLock );
}

/**
  change the the send rate for one MsgObj by setting the minimum
  pause time between two messages [msec.]
  @param aui8_busNr number of the BUS to config
  @param aui8_msgobjNr number of the MsgObj to config
  @param aui16_minSendPause minimum send pause between two sent messages [msec.]
  @return HAL_NO_ERR == no error;
          HAL_CONFIG_ERR == BUS not initialised or ident can't be changed
          HAL_RANGE_ERR == wrong BUS or MsgObj number
*/
int16_t can_configMsgobjSendpause(uint8_t aui8_busNr, uint8_t aui8_msgobjNr, uint16_t aui16_minSend)
{
  return chgCanObjPause(aui8_busNr, aui8_msgobjNr, aui16_minSend);
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
#ifdef SYSTEM_WITH_ENHANCED_CAN_HAL
    arrHalCan[aui8_busNr][aui8_msgobjNr].b_canObjConfigured = false;
    // erase element if it is the last in the vector, otherwise it can stay there
    while (arrHalCan[aui8_busNr].back().b_canObjConfigured == false)
        arrHalCan[aui8_busNr].pop_back();
#endif
  return closeCanObj(aui8_busNr, aui8_msgobjNr);
}


/** wait until specified timeout or until next CAN message receive
 *  @return true -> there are CAN messages waiting for process. else: return due to timeout
  */
bool can_waitUntilCanReceiveOrTimeout( uint16_t aui16_timeoutInterval )
{
  return waitUntilCanReceiveOrTimeout( aui16_timeoutInterval );
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
  int32_t i32_now = getTime();
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
  uint8_t b_count = arrHalCan[aui8_busNr][(aui8_msgobjNr)].ui8_cinterfLastSendBufCnt;

  updateSuccSendTimestamp(aui8_busNr);
  b_count = arrHalCan[aui8_busNr][(aui8_msgobjNr)].ui8_cinterfLastSendBufCnt;
  // CanPkgExt_c::getData transforms flag data to ident and 8byte string
  apc_data->getData(pt_send->dwId, pt_send->bXtd, pt_send->bDlc, pt_send->abData);
  // pt_send->dwId = apc_data->ident();
  // if (apc_data->identType() == 1)
  // CanPkg_c::ident() and CanPkg_c::identType() changed to static
  // pt_send->dwId = __IsoAgLib::CanPkg_c::ident();
  // if (__IsoAgLib::CanPkg_c::identType() == 1)
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
  arrHalCan[aui8_busNr][(aui8_msgobjNr)].ui8_cinterfLastSendBufCnt = b_count + 1;
  return sendCanMsg(aui8_busNr, aui8_msgobjNr, pt_send);
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
  {
  // to-be-processed item is not yet buttered in pt_receive -> read from driver
    i16_retVal = getCanMsg(aui8_busNr, aui8_msgobjNr, pt_receive);
  }

  if ((i16_retVal == HAL_NO_ERR) || (i16_retVal == HAL_OVERFLOW_ERR) || (i16_retVal == HAL_WARN_ERR))
  {
    if (pt_receive->tReceiveTime.l1ms == 0)
      i32_cinterfLastSuccReceive[aui8_busNr] = getTime();
    else
      i32_cinterfLastSuccReceive[aui8_busNr] = pt_receive->tReceiveTime.l1ms;
    b_cinterfBufferedReceivedMsg = true;
    reflIdent = pt_receive->dwId;
  }
  return i16_retVal;
}

#ifdef SYSTEM_WITH_ENHANCED_CAN_HAL
int32_t can_useNextMsgobjNumber(uint8_t aui8_busNr, int32_t &refMsgobjNr)
{
  tReceive* pt_receive = &t_cinterfMsgobjReceive;
  int16_t i16_retVal = HAL_NO_ERR;
  // only take new msg from BIOS buffer if not previously
  // buffered for detecting of the received ident
  if (!b_cinterfBufferedReceivedMsg)
  { // to-be-processed item is not yet buttered in pt_receive -> read from driver
    i16_retVal = getCanMsg(aui8_busNr, COMMON_MSGOBJ_IN_QUEUE , pt_receive);
  }

  if ((i16_retVal == HAL_NO_ERR) || (i16_retVal == HAL_OVERFLOW_ERR) || (i16_retVal == HAL_WARN_ERR))
  {
    if (pt_receive->tReceiveTime.l1ms == 0)
      i32_cinterfLastSuccReceive[aui8_busNr] = getTime();
    else
      i32_cinterfLastSuccReceive[aui8_busNr] = pt_receive->tReceiveTime.l1ms;
    b_cinterfBufferedReceivedMsg = true;
    refMsgobjNr = pt_receive->bMsgObj;
  }
  return i16_retVal;
}
#endif // SYSTEM_WITH_ENHANCED_CAN_HAL


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
    i16_retVal = getCanMsg(aui8_busNr, aui8_msgobjNr, pt_receive);

  if ((i16_retVal == HAL_NO_ERR) || (HAL_OVERFLOW_ERR) || (HAL_WARN_ERR))
  {
    b_cinterfBufferedReceivedMsg = true;
    if (pt_receive->tReceiveTime.l1ms == 0)
    {
      i32_cinterfLastSuccReceive[aui8_busNr] = getTime();
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
    if (pt_receive->bXtd == 1)
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
    // setIdent changed to static member function
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
  if ((aui8_msgobjNr < arrHalCan[aui8_busNr].size())&&(aui8_busNr < cui32_maxCanBusCnt))
  { // valid parameters
    b_cinterfBufferedReceivedMsg = false;
  }
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
  return clearCanObjBuf(aui8_busNr, aui8_msgobjNr);
}

int32_t can_getMaxSendDelay(uint8_t aui8_busNr)
{
  return getMaxSendDelay(aui8_busNr);
}
} // end of namespace
