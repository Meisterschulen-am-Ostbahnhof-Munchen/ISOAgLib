/***************************************************************************
                          hddIo.cpp -  source for Hdd_IO object
                                         for serial communication (Rs232)
                             -------------------
    begin                : Mon Oct 25 1999
    copyright            : (C) 1999 by Dipl.-Inform. Achim Spangler
    email                : spangler@tec.agrar.tu-muenchen.de
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
 * Copyright (C) 1999  Dipl.-Inform. Achim Spangler                        *
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
#include "hdd_io.h"
#include <string.h>

/* ************************************** */
/* import some namespaces for easy access */
/* ************************************** */
using namespace __IsoAgLib;
using namespace __IsoAgLib::nHwRS232;

/** set trigger len to flushCmd a cmd puffer */
#define CMD_PUFFER_FLUSH_SIZE 80
#define STATIC_GLOBAL_SEND_PUFF_SIZE 250
#define STATIC_GLOBAL_TEMP_STRING_SIZE 1500
#define MAX_TEMP_STRING_TO_SEND 80
#define HDD_FLUSH_INTERVAL 1000
#define FLUSH_IDLE_CMD_TIME 100

#define WAIT_AFTER_LAST_COMM_ERR 1000
#define ISSPACE_DELIM -1


static FAR_MEM byte pb_globalHddSendPuff[STATIC_GLOBAL_SEND_PUFF_SIZE];
/** temporary string for interrupted (sync_err) write access */
static FAR_MEM byte pb_tempString[STATIC_GLOBAL_TEMP_STRING_SIZE];
static char FAR_MEM pc_readFilePuffer[CONFIG_HDD_READ_PUFFER_SIZE];
static byte b_readFilePufferPos;

/*******************************************/
/** definition of public element functions */
/*******************************************/

/**
  default constructor which can set some basic configurations
  @param rwBaudrate baudrate {75, 600, 1200, 2400, 4800, 9600, 19200}
  @param ren_dataMode data mode setting of {7,8}_{N,O,E}_{1,2}
  @param rb_xonXoff use XON/XOFF sw handshake (true, false)
  @param rwSndPuf sending puffer size
  @param rwRec_puf recieving puffer size

  possible errors:
      * Err::badAlloc not enough memory for allocating the puffers
      * Err::range one of the configuration vals is not in allowed ranges
*/
RS232_IO::RS232_IO(word rwBaudrate, t_dataMode ren_dataMode, bool rb_xonXoff,  
                    word rwSndPuf, word rwRec_puf)
{
  // check the configuration informations
  bool b_baudAllowed = false,
        b_dataModeAllowed = false,
        b_configTxSuccess = false,
        b_configRxSuccess = false;
  
  // check if rwBaudrate is one of the allowed settings
  int piAllowed[] = BIOS_RS232_BAUDRATE_LIST;
  for (byte b_index = 0; b_index < BIOS_CAN_BITRATE_CNT; b_index++)
  {
    if (piAllowed[b_index] == rwBaudrate)
    { // given baudrate is in allowed list at position b_index
      b_baudAllowed = true; // store success state
      break; // exit search loop
    }
  }

  byte b_dataParityVal = (ren_dataMode >> 4);
  byte b_stopBit = (ren_dataMode & 0xF);
  if ((b_dataParityVal > 0) && (b_dataParityVal < 6))
  { // setting for data bit and parity is correct
    // now check if stop bis are set to 1 or 2
    b_dataModeAllowed = ((b_stopBit == 1) || (b_stopBit == 2))?true:false;
  }

  // store configuration values
  wBaudrate = rwBaudrate;
  en_dataMode = ren_dataMode;
  b_xon_xoff = rb_xonXoff;
  wSndPuf = rwSndPuf;
  wRec_puf = rwRec_puf;

  // set Hdd specific vars to initial vals
  c_filename = "";
  en_hddState =  off_state;
  en_hddErr = nosyncErr;
  en_eofState = noEof;
  en_cmdPuffState = inactive;
  en_sendPuffState = inactive;
  // init cmd puff
  c_puffCmd.reserve(CMD_PUFFER_FLUSH_SIZE + 30); // reserve enough memory for 10 byte
  c_puffCmd.push_back(0); // len LSB
  c_puffCmd.push_back(0); // len MSB
  c_puffCmd.push_back(0); // TAN
  c_puffCmd.push_back(24); // cmd code for writeFile
  // init send puff
  wSendPuffMaxSize = STATIC_GLOBAL_SEND_PUFF_SIZE;
  pb_sendPuff = pb_globalHddSendPuff;
  wSendPuffActSize = 0;
  
  c_puffResp.reserve(30);
  en_respPuffState = respInactive;

  lCmdSendTime = 0;
  lCmdWriteTime = 0;
  lLastFlushCmdDriveTime = 0;
  lLastCompleteResponse = 0;
  lLastSoh = 0;
  lLastWd = 0;
  lLastRespCardErr = -10000;

  b_writeHandle = 0xFF;
  b_tan = 0;

  b_readFilePufferPos = 0;
  b_lastFileWriteFlushed = true;

  wTempLen = 0;


  // set error state if one of b_baudAllowed and b_dataModeAllowed is false
  // and init hardware if everything is accepted
  if ( ((b_baudAllowed) && (b_dataModeAllowed))
     &&(HAL::init_rs232(wBaudrate, b_dataParityVal, b_stopBit, b_xon_xoff) == HAL_NO_ERR)
      )
  { // o.k.
    setState(Err::range, Err::correctState);
    // now init puffers
    if (HAL::configRs232TxObj(wSndPuf, NULL, NULL) == HAL_NO_ERR)
      b_configTxSuccess = true;
    if (HAL::configRs232RxObj(wRec_puf, NULL) == HAL_NO_ERR)
      b_configRxSuccess = true;
    if (b_configTxSuccess && b_configRxSuccess)
    { // send and receive puffer successful created
      setState(Err::badAlloc, Err::correctState);
    }
    else
    {
      setState(Err::badAlloc, Err::errorState);
    }
  }
  else
  { //wrong values given
    setState(Err::range, Err::errorState); // something is still wrong
  }
}

/** destructor has nothing to destruct */
RS232_IO::~RS232_IO(){
  // close all open puffers on hdd and close all files
  flushCmdSlotHdd();
  // call timeEvent to send cmd
  timeEventInternControl(true);
}

// ++++++++++++++++++++++++++++++++++++
// ++++ Hdd managing operations ++++
// ++++++++++++++++++++++++++++++++++++


/**
  initialise the Hdd driver
*/
void RS232_IO::init()
{
  en_hddState = init_state;
  en_hddErr = nosyncErr;
  // timeEvent controls rest
  timeEvent();
}

/**
  check if Hdd can take a new cmd into the cmd puffer (e.g. if flush() results in an
  empty cmd puffer)
  @param rb_forceIdle true -> both cmd and send puffer must be idle (optional, default false)
  @return true -> new cmd like open,close can be used
*/
bool RS232_IO::await_newCmd(bool rb_forceIdle)const
{
  if (rb_forceIdle) return ((en_cmdPuffState == inactive)&&(en_sendPuffState == inactive))?true:false;
  else return ((en_cmdPuffState == inactive)||(en_sendPuffState == inactive))?true:false;
}

/**
  set the baudrate to a new value
  @param rwBaudrate baudrate {75, 600, 1200, 2400, 4800, 9600, 19200}

  possible errors:
      * Err::range one of the configuration vals is not in allowed ranges
  @return true -> setting successful
*/
bool RS232_IO::setBaudrate(word rwBaudrate)
{
  // check the configuration informations
  bool b_baudAllowed = false;
  
  // check if rwBaudrate is one of the allowed settings
  int piAllowed[] = BIOS_RS232_BAUDRATE_LIST;
  for (byte b_index = 0; b_index < BIOS_CAN_BITRATE_CNT; b_index++)
  {
    if (piAllowed[b_index] == rwBaudrate)
    { // given baudrate is in allowed list at position b_index
      b_baudAllowed = true; // store success state
      break; // exit search loop
    }
  }
  
  // now set the baudrate if allowed
  if ((b_baudAllowed) && (HAL::setRs232Baudrate(rwBaudrate) == HAL_NO_ERR))
  { // everything o.k.
    setState(Err::range, Err::correctState);
    wBaudrate = rwBaudrate;
  }
  else
  { // wrong setting
    setState(Err::range, Err::errorState);
  }
  return b_baudAllowed;
}

/**
  set send puffer size
  @param rwSndPuf sending puffer size

  possible errors:
      * Err::badAlloc not enough memory for allocating the puffer
  @return true -> allocating of puffer successful
*/
bool RS232_IO::setSndPufferSize(word rwPufferSize)
{
  if (HAL::configRs232TxObj(rwPufferSize, NULL, NULL) == HAL_NO_ERR)
  { // send puffer successful created
    setState(Err::badAlloc, Err::correctState);
    wSndPuf = rwPufferSize;
    return true;
  }
  else
  {
    setState(Err::badAlloc, Err::errorState);
    return false;
  }
}
/**
  set receive puffer size
  @param rwRec_puf receiving puffer size

  possible errors:
      * Err::badAlloc not enough memory for allocating the puffer
  @return true -> allocating of puffer successful
*/
bool RS232_IO::setRecPufferSize(word rwPufferSize)
{
  if (HAL::configRs232RxObj(rwPufferSize, NULL) == HAL_NO_ERR)
  { // receive puffer successful created
    setState(Err::badAlloc, Err::correctState);
    wRec_puf = rwPufferSize;
    return true;
  }
  else
  {
    setState(Err::badAlloc, Err::errorState);

    return false;
  }

}

/**
  control state change of HDD and call timeEventInternControl
  for performing the needed actions
*/
void RS232_IO::timeEvent()
{  // do nothing if HDD state is off
  if (en_hddState == off_state) return;

  // perform next step dependent on error state
  if ((en_hddErr & respErr) > 0)
  { // -> do nothing if it's to early after last comm err
    if (HAL::getTime() - lLastRespCardErr < WAIT_AFTER_LAST_COMM_ERR) return;
    else en_hddErr = t_hddErr(en_hddErr & ~respErr); // clear respErr for new try
  }
  else
  {
    switch (en_hddErr)
    {
      case noErr:
        // control switch to next state if needed
        switch (en_hddState)
        {
          case init_state:
            // flush slot
            flushCmdSlotHdd();
            break;
          case flushedSlot:
            // -> now read CARD info
            getCardinfo();
            break;
          case openableState:
            // -> check if FILE name exists so that this should be REopened
            if (c_filename != "") open(NULL);
            break;
          default: // do nothing
            break;
        } // end switch for state change
        break;
      case nosyncErr: // -> call readVersionHdd to resync TAN
        resetCmdBuffers();
        readVersionHdd();
        break;
      case justSyncedTan:
        en_hddErr = noErr;
        switch (en_hddState)
        { // continue broken action
          case init_state:
            // flush slot
            flushCmdSlotHdd();
            break;
          case flushingSlot:
            flushCmdSlotHdd();
            break;
          case cardReading:
            getCardinfo();
            break;
          case openingState:
            en_hddState = openableState;
            open(NULL);
            break;
          case closingState:
            en_hddState = open_state;
            // close without flushing temp text
            close(false);
            break;
          default:
            // test if write action was interrupted
            if ( (en_hddState == open_state)
              && (wTempLen != 0)
            )
            { // let write send temp text
              write(NULL, 0);
            }
        } // switch on hddState
        break;
      case noWriteSpaceErr: // read card info
        if (HAL::getTime() - lLastRespCardErr < WAIT_AFTER_LAST_COMM_ERR) break;
        resetCmdBuffers();
        getCardinfo();
        break;
      default: // do nothing -> only avoid compiler warn
        break;
    }
  }
  // now let timeEventInternControl perfom waiting interaction steps
  timeEventInternControl();
}

/**
  let timeEventInternLoop perform actions till
  * quick important part actions are complete (e.g. SOH2STX)
  * incoming response is complete (only for short responses where first part is already received)
  * sendPuffer is inactive and no further flushable cmd is waiting IF rb_forceFinalIdle == TRUE
  @param rb_forceFinalIdle true -> force performing of all waiting cmd's
*/
void RS232_IO::timeEventInternControl(bool rb_forceFinalIdle)
{  // do nothing, if last hdd err was less than 3 sec. ago (but ignore errors in startup)
  if (en_hddErr > nosyncErr) return;

  // if sendPuff has nothing to do -> decide about automatic action
  if (en_sendPuffState == inactive)
  {
    switch (en_cmdPuffState)
    {
      case flushCmd:
        flush();
      case inactive:
        // check if temp text should be written
        if ( (en_hddState == open_state) && (wTempLen > 0) ) write(NULL, 0);
        else if (b_lastFileWriteFlushed == false)
        { // it's now time to flush written data
          #ifdef CLOSE_AFTER_WRITE_SEQUENCE 
          // close without flushing temp text
          close(false);
          #else
          flushCmdFileHdd();
          #endif
        }
        break;
      default:
        if ( ( (HAL::getTime() - lCmdWriteTime > FLUSH_IDLE_CMD_TIME)
            && (b_lastFileWriteFlushed == false)
             ) 
          || ((c_puffCmd[3] != 24) && (en_cmdPuffState == active))
            )
             flush();
        break;
    } // switch
    // if sendPuff has still nothing to do -> don't call loop
    if (en_sendPuffState != flushCmd) return;
  }

  do {
    if (error()) break;

    // check if response error
    if ( (en_sendPuffState >= CMD) && (HAL::getTime() - lLastSoh > 1000))
    { // somewhere in action chain no response -> set respErr
      en_hddErr = t_hddErr(en_hddState | respErr);
      break;
    }
  
    timeEventInternLoop();

    if ((HAL::getTime() - lLastWd) > 50)
    {
      HAL::wdTriggern();
      lLastWd = HAL::getTime();
    } // if wdTrigger needed
  } while ( ( (en_sendPuffState & quickWorkState) != 0)
         || ((en_sendPuffState == RESPData) && (getRespLen() - c_puffResp.size() < 6))
         || ((rb_forceFinalIdle == true) && (en_sendPuffState != inactive))
        );
}

/**
  check if time event is needed for further working on sent cmd
  @return true -> timeEvent should be called
*/
bool RS232_IO::needTimeEvent()const
{
  if (((en_sendPuffState != inactive) || (en_cmdPuffState == flushCmd))
      && (!(error()))) return true;
  else return false;
}


/**
  perform next single step of interaction with HDD
*/
void RS232_IO::timeEventInternLoop()
{  
  // check for both cmd puffer if something has to be done
  switch (en_sendPuffState)
  {
    case inactive:
    case active:
      break;
    case flushCmd:
      // send puffer should be sent -> start with SOH
      // only send cmd if response puffer is respInactive (ready for new cmd)
      if (en_respPuffState == respInactive) sendSoh();
      else
      { // this is irregular situation -> reset respons buffer
        en_respPuffState = respInactive;
        break;
      }
    case SOH_STX:
      // check if STX received
      if (!(soh2stx())) break;
    case CMD:
      // send cmd of this cmd puffer
      // send cmd
      send(pb_sendPuff, wSendPuffActSize);
      // record send time
      lCmdSendTime = HAL::getTime();
      // set state
      en_sendPuffState = ACK;
      break;
    case ACK:
      if (sndPufferCnt() > 0)
      { // record send time because cmd not yet sent complete
        lCmdSendTime = HAL::getTime();
        break;
      }
      // check if ACK is received
      if (!(checkAck())) break;
    case RESPLen:
      // read len of response (2 byte)
      if (!(readRespLen())) break;
    case RESPData:
      // read response -> if readRespData return true the complete
      // response is received
      if (!(readRespData())) break;

      // check TAN of response and sent cmd -> true == corresponding
      if (!(checkRespTan())) break;

      // interprete resp data
      interpreteRespData();
      break;
  }
}

/**
  send start-of-header SOH
*/
void RS232_IO::sendSoh()
{
  // flushCmd this puffer to DOS Drive -> start with SOH
  byte b_soh = 0x01;
  // send SOH
  send(&b_soh, 1);
  // switch state to SOH_STX
  en_sendPuffState = SOH_STX;
  // record send time
  lLastSoh = lCmdSendTime = HAL::getTime();
  // set response puffer to state for awaiting STX
  en_respPuffState = await_STX;
}

/**
  check if STX is received and change
  en_sendPuffState to CMD for sending
  of cmd as next action
  @return true -> STX received -> cmd can be sent now
*/
bool RS232_IO::soh2stx()
{
  // check if en_sendPuff state and respPuffState are in sync
  if (en_respPuffState != await_STX)
  { // there is a sync problem -> trigger repeat
    triggerCmdRepeat(false);
  }

  bool b_result = false;
  long lStart = HAL::getTime();
  long lLoop = lStart;
  while ( (rec_pufferCnt() == 0) && (lLoop - lStart < 300))
  { // update sendSoh time if send puffer is not empty -> SOH not sent
    lLoop = HAL::getTime();
    if (sndPufferCnt() > 0) lLastSoh = lLoop;
    if ((lLoop - lLastWd) > 50)
    {
      HAL::wdTriggern();
      lLastWd = lLoop;
    }
  }

  // check if HDD responded
  if (rec_pufferCnt() == 0)
  { // theres no response from HDD in RS232 buffer
    if ((lLoop - lLastSoh) > 1000)
    { // SOH went off from RS232 buffer more than 1sec. ago -> response error
      triggerCmdRepeat(true);
    }
    return false;
  }

  // one data in buffer -> take it and check if it is STX = 0x02
  // store in hdd state that Hdd responds to cmd
  en_hddErr = static_cast<t_hddErr>(en_hddErr & ~respErr);

  byte b_stx = 0;
  HAL::getRs232Char(&b_stx);
  if (b_stx == 0x02)
  { // STX received -> switch state to CMD
    en_sendPuffState = CMD;
    // set response puffer to state for awaiting STX
    en_respPuffState = await_ACK;
    // signal correct receive of STX
    b_result = true;
  }
  else
  { // wrong response -> wait and resend cmd
    triggerCmdRepeat(true);
  }
  return b_result;
}

/**
  check if ACK was received
  @return true -> ACK received -> receive RESP next
*/
bool RS232_IO::checkAck()
{
  bool b_result = false;
  if ((rec_pufferCnt() > 0) && (en_respPuffState == await_ACK))
  { // one data in buffer -> take it and check if it is ACK = 0x06
    // store in hdd state that Hdd responds to cmd
    en_hddErr = static_cast<t_hddErr>(en_hddErr & ~respErr);
    byte b_stx = 0;
    HAL::getRs232Char(&b_stx);
    if (b_stx == 0x06)
    { // ACK received -> switch state to RESPLen
      en_sendPuffState = RESPLen;
      // set response puffer await_RESP
      en_respPuffState = await_RESP;
      // clear response puffer
      c_puffResp.clear();
      // set success in b_result
      b_result = true;
    }
    else
    { // received byte not ACK
      triggerCmdRepeat(true);
    }
  }
  else
  { // check time 
    if ((HAL::getTime() - 400) > lCmdSendTime)
    { // ACK should have been received -> retry by setting state to flushCmd
      triggerCmdRepeat(true);
    }
  }
  return b_result;
}

/**
  read len of response (first two bytes)
  @return true -> two byte with len of response are read in response buffer
*/
bool RS232_IO::readRespLen()
{
  bool b_result = false;
  if ((rec_pufferCnt() >= 2) && (en_respPuffState == await_RESP))
  { // two byte data in buffer -> take it as response len
    // store in hdd state that Hdd responds to cmd
    en_hddErr = static_cast<t_hddErr>(en_hddErr & ~respErr);
    byte b_len_lsb = 0xFF, b_len_msb = 0xFF;
    if (HAL::getRs232Char(&b_len_lsb) == HAL_RS232_RECEIVE_ERR)
    { // receive error -> triger repeat with respErr
      triggerCmdRepeat(true);
      return false;
    }
    c_puffResp.push_back(b_len_lsb);
    if (HAL::getRs232Char(&b_len_msb) == HAL_RS232_RECEIVE_ERR)
    { // receive error -> triger repeat with respErr
      triggerCmdRepeat(true);
      return false;
    }
    c_puffResp.push_back(b_len_msb);
    // store success in b_result
    if ((b_len_lsb != 0xFF) && (b_len_msb != 0xFF))
    { // set state to RESPData
      en_sendPuffState = RESPData;
      b_result = true;
    }
  }
  else
  { // check time // old time 300
    if ((HAL::getTime() - 600) > lCmdSendTime)
    { // ACK should have been received -> retry by setting state to flushCmd
      triggerCmdRepeat(true);
    }
  }
  return b_result;
}

/**
  read the response data in response puffer
  @return true -> complete response received
*/
bool RS232_IO::readRespData()
{
  bool b_result = false;
  // stored len include the two already received len bytes
  // but excludes the last chksum byte -> (storedLen - 1) byte
  // must be read from rs232
  // -> rs232_recPufferCnt > storedLen
  if (en_respPuffState == await_RESP)
  { // response of DOS DRIVE complete in rs232 -> read in c_puffResp
    byte b_byte;
    // two byte of resp already read in puffer
    // stored len without chksum
    // -> start with b_ind at 1 at run as long as b_ind < storedLen
    word wRespLen = getRespLen();
    while ((c_puffResp.size() <= wRespLen) && (rec_pufferCnt() > 0))
    {
      if (HAL::getRs232Char(&b_byte) == HAL_RS232_RECEIVE_ERR)
      { // receive error -> triger repeat with respErr
        triggerCmdRepeat(true);
        return false;
      }
      c_puffResp.push_back(b_byte);
    }
    if (c_puffResp.size() == (getRespLen() +1))
    {
      // store in hdd state that Hdd responds to cmd
      en_hddErr = static_cast<t_hddErr>(en_hddErr & ~respErr);
      en_respPuffState = complete_RESP;
      lLastCompleteResponse = HAL::getTime();
      b_result = true;
    }
  }
  else
  { // set b_result true if already received
    if (en_respPuffState == complete_RESP) b_result = true;
  }
  return b_result;
}

/**
  compare TAN of response and TAN of last sent cmd
  @return true -> coresponding TAN
*/
bool RS232_IO::checkRespTan()
{
  if (pb_sendPuff[2] != c_puffResp[2])
  { // something went wrong with TAN management
    // -> reset TAN counters on both sides with readVersionHdd()
    // set response puffer inactive to allow send of bew cmd
    preserveWriteText(true);
    // resync TAN, getCardinfo, flush slot -> openable
    en_hddErr = nosyncErr;
    return false;
  } // end if wrong tan
  else
  { // TAN o.k.
    return true;
  }
}

/**
  interprete the received data
*/
void RS232_IO::interpreteRespData()
{
  word wFree;
  if ((c_puffResp[3] == 0) && (validateResponse()))
  { // success -> set state to inactive (ready for activation)
    // and resize to standard size of 4
    // but don't reset cmdPuffer
    resetCmdBuffers(false); 
     b_tan++;

    // interprete the answer dependent on cmd
    switch (pb_sendPuff[3])
    {
      case 2: // readVersion -> tan already set
        en_hddErr = justSyncedTan;
        break;
      case 17: // flushSlot -> continue with getCardinfo
        // cardinfo can now be read
        en_hddState = flushedSlot;
        b_writeHandle = 0xFF;
        break;
      case 16: // getCardinfo
        // check if free space is available
        wFree = c_puffResp[16] + (c_puffResp[17] << 8);
        if ((wFree > 0) && (wFree != 0xFFFF))
        { // there's free space on card for write
          en_eofState = t_eof_state(en_eofState & ~writeEof);
          en_hddState = openableState;
          en_hddErr = noErr;
        }
        else
        { // media error or no space -> 
          if (wFree == 0) en_eofState = t_eof_state(en_eofState | writeEof);
          else
          {
            en_hddErr = t_hddErr(en_hddErr | cardErr);
            en_hddState = flushedSlot; // continue with card read
          }
        }
        break;
      case 21: //open file
        // read in handle
        b_writeHandle = c_puffResp[4];

        if (b_writeHandle == 255) en_hddErr = fileNotFound;
        // set RS232_IO state to open_state
        else
        {
          en_hddState = open_state;
          // clear state bits for readFileEof and readCallEof
          // -> only let writeEof state-bit unchanged
          en_eofState = t_eof_state(en_eofState & writeEof);

          // test if write action is waiting
          if (wTempLen != 0)
          { // let write send temp text
            write(NULL, 0);
          }
        }
        break;
      case 23: // read file
        // transfer text from c_puffResp to pc_readFilePuffer
        // ignore the CHKSUM byte at the end of c_puffResp
        for (byte b_ind = 4; b_ind <   (c_puffResp.size() - 1); b_ind++)
        { // copy text to pc_readFilePuffer
          pc_readFilePuffer[b_readFilePufferPos] = c_puffResp[b_ind];
          b_readFilePufferPos++;
        }
        // terminate string
        pc_readFilePuffer[b_readFilePufferPos] = '\0';
        // check if EOF reached:
        // ignore 4 control byte at beginning and CHKSUM at the end
        if ((getRespLen() - 5) < (pb_sendPuff[6] + (pb_sendPuff[7] << 8)))
        { // less data received than wanted -> EOF reached
          en_eofState = readFileEof;
        }
      case 24: // write file
        break;
      case 25: // flushCmd file
        break;
      case 26: // close file
        en_hddState = openableState;
        b_writeHandle = 0xFF;
        break;
    }
    if (en_cmdPuffState == flushCmd)
    {
      flush();
    }
  }
  else
  { // change for retry the TAN if no protocol error (errCode < 40)
    if (c_puffResp[3] < 40)
    { // message correct received, but execution error
      b_tan++;
      
      switch (c_puffResp[3])
      {
        case 1:
          // try to open file with file already open
          if (en_hddState == openingState) en_hddState = open_state;
          // only reset sendPuff, let cmdPuff unchanged
          resetCmdBuffers(false);
          return;
        case 5: // invalid handle -> flushSlot for well defined state
          en_hddState = init_state;
          en_hddErr = noErr;
          // reset all cmd puff (including cmdPuff)
          resetCmdBuffers(true);
          break;
        case 8: // no space on device
          en_eofState = writeEof;
          // set noWriteSpaceErr on write access failure
          // let timeEvent check for memcard change
          if (pb_sendPuff[3] == 24)en_hddErr = noWriteSpaceErr;
          resetCmdBuffers(true);
          break;
        case 9: // verify error
        case 29: // WriteCardError
          // change output file name
          if (c_filename.length() < 12)c_filename.insert(7, "0");
          // increase digit a position 7 (with overflow at 10)
          else c_filename[7] =  ((((c_filename[7] - '0') + 1) % 10) + '0');
        case 23: // no card
        case 26: // unknown mem card
          // set state to re-read card-info
          en_hddErr = t_hddErr(en_hddErr | cardErr);
          en_hddState = flushedSlot; // continue with card read
          resetCmdBuffers(true);
          break;
        default: // other error -> try resend with new TAN
          pb_sendPuff[2] = b_tan;
          triggerCmdRepeat(false);
          // get checksum
          byte b_chksum = 0;
          for (word wInd = 0; wInd < (wSendPuffActSize -1); wInd++)
          {
            pb_sendPuff[wInd] = c_puffCmd[wInd];
            b_chksum ^= c_puffCmd[wInd];
          }
          pb_sendPuff[wSendPuffActSize] = b_chksum;
          break;
      }
    }
    else
    { // protocol error -> retry with same TAN (but don't register as resp err
      if (c_puffResp[3] == 42) resetCmdBuffers(true);
      else triggerCmdRepeat(false);
    }
  }
}


/**
  test if not active puffer is ready to switch to active (e.g. is inactive)
  and switch hddState to respErr if not;
  set flushCmd flag for active cmd puffer; switch active cmd puffer;
  @param rb_forceFreeCmdPuff optional force waiting till cmdPuff is
        ready for new cmd (default false)
  @return true -> cmd puffer is now ready for new command
*/
bool RS232_IO::flush(bool rb_forceFreeCmdPuff)
{  // if en_cmdPuffState is inactive nothing 
  // can be sent -> nothing can be flushed
  if (en_cmdPuffState == inactive) return true;
  if (error()) return false;
  
  if ( (en_sendPuffState != inactive) || (en_respPuffState != respInactive) )
  { // sendPuff not ready for new cmd -> call timeEventInternControl with forced wait
    if (rb_forceFreeCmdPuff)
    {
      timeEventInternControl(true);
      // recursive call possible -> check again if en_cmdPuffState is now inactive
      if (en_cmdPuffState == inactive)
      {
        return true;
      }
    }
  }

  // check if en_sendPuffState is inactive (ready for new cmd)
  if ( (en_sendPuffState == inactive) && (en_respPuffState == respInactive) )
  { // send puffer is ready -> check if enough space in send puffer
    if ((c_puffCmd.size() + 1) > wSendPuffMaxSize)
    { // greater send puffer needed
      c_puffCmd.resize(wSendPuffMaxSize - 1); // static puffer
    }
    // cmd puffer now fit into send puffer
    preparePuffer();
    // copy cmdPuff to sendPuff
    wSendPuffActSize = c_puffCmd.size();
    // get checksum
    byte b_chksum = 0;
    for (word wInd = 0; wInd < wSendPuffActSize; wInd++)
    {
      pb_sendPuff[wInd] = c_puffCmd[wInd];
      b_chksum ^= c_puffCmd[wInd];
    }
    pb_sendPuff[wSendPuffActSize] = b_chksum;
    wSendPuffActSize++;
    en_sendPuffState = flushCmd;
    c_puffCmd.resize(4); // resize cmd puffer
    c_puffCmd[3] = 24; // set writeFile as default
    en_cmdPuffState = inactive; // cmd puffer is empty and inactive
    if ((en_hddState & respErr) != 0)
      en_hddErr = static_cast<t_hddErr>(en_hddErr & ~respErr);;
    return true;
  }
  else
  { // send puffer is not ready for new cmd
    if (en_cmdPuffState != flushCmd)
    { // cmd puffe not yet triggered for flush -> trigger it now
      en_cmdPuffState = flushCmd;
    }
    return false;
  }
}

/* ******************************* */
/* ****** managing of files ****** */
/* ******************************* */

/**
  open a file on hdd; after executing this Cmd, the Hdd object switches from closeState to open_state state
  -> writing operations are allowed;
  if this RS232_IO object is NOT in closeState state, this function returns false
  @param rpb_name name of the file
  @param ren_filemode mode of the file, combination of {readMode, writeMode, create, append}, default is all of them
  @return true -> RS232_IO switched from closeState to open_state state correct
*/
bool RS232_IO::open(const char* rpb_name, t_filemode ren_filemode)
{ // check inputs
  if (rpb_name == NULL)
  {
    if (c_filename == "") return false;
  }
  else
  {
    if ( (c_filename != "") && (c_filename != rpb_name) )
    { // change of file name
      switch (en_hddState)
      {
        case open_state:
        case openingState:
          // other file open --> write temp text to this file
          timeEventInternControl(true);
          // other file is open
          // close other file (temp text is written there)
          close(true);
        case closingState:
          // wait till other file is complete closed
          timeEventInternControl(true);
        default:
          break;
      } // end switch
    }
    c_filename = rpb_name;
  }

  // check for valid file handle -> set state to open_state
  if ( (b_writeHandle != 0xFF)
    && ( (rpb_name == NULL) || (c_filename == rpb_name) )
     )
  { // reuse
    en_hddState = open_state;
    return true;
  }

  // check actual state of RS232_IO -> allowed for state openableState and closingState 
  if (t_hddState(en_hddState & ~(fileStateChanging)) == open_state)return false;

  timeEventInternControl();

  // flush cmd puffer to send puffer
  // if flush answers false the send puffer is not ready for cmd
  // but as the filename is stored, the opening is performed as soon as possible
  if (!(flush())) return true;
  
  en_hddState = openingState;
  // now cmd puffer is free to use
  // set cmd code for open_file
  en_cmdPuffState = active;
  setCmd(21);
   // set RS232_IO state to open_state
   en_hddState = openingState;
  // set year static to 2001 -> 1980 + 21
  // set day and month to 1 and 1
  // LSB of wDate to pos 4
  c_puffCmd.push_back(10785 & 0xFF);
  // MSB of wDate to pos 5
  c_puffCmd.push_back(10785 >> 8);
  // LSB of wTime to pos 6
  c_puffCmd.push_back(2081 & 0xFF);
  // MSB of wTime to pos 7
  c_puffCmd.push_back(2081 >> 8);
  // set filemode
  en_filemode = ren_filemode;
  c_puffCmd.push_back(ren_filemode);
  // set access mode
  c_puffCmd.push_back(3);
  // insert filename
  for (byte b_ind = 0; b_ind < c_filename.length(); b_ind++)
  {
    c_puffCmd.push_back(c_filename[b_ind]);
  }
  c_puffCmd.push_back('\0');
  // call flushCmd to send cmd as soon as possible
  flush();
  timeEventInternControl();
  return true;
}

/**
  send cmd to flushCmd the open file on Hdd drive
*/
void RS232_IO::flushCmdFileHdd()
{ // check actual state of RS232_IO
  if (en_hddState != open_state) return;

  // flush cmd puffer to send puffer and force waiting till cmdPuff free
  // if flush answers false then some response error occured
  if (!(flush(true))) return;

  // register that file flush is performed
  b_lastFileWriteFlushed = true;
  
  startSimpleCmd(25, b_writeHandle);
  return;
}

/**
  send cmd to flushCmd the open slot on Hdd drive
  -> write all puffer on Hdd drive to disc and close all
     open files
  -> important to call on system stop
*/
void RS232_IO::flushCmdSlotHdd()
{ // flush cmd puffer to send puffer and force waiting till cmdPuff free
  // if flush answers false then some response error occured
  if (!(flush(true))) return;


  // set RS232_IO state to closingState
  en_hddState = closingState;
  b_lastFileWriteFlushed = true;

  startSimpleCmd(17, 0);
  return;
}

/**
  close a file on Hdd
  @param rb_flushWriteText true -> flush all stored text to HDD before close of file (optional default)
  @return true -> this object switched from other state to closeState state
*/
bool RS232_IO::close(bool rb_flushWriteText)
{ // check actual state of RS232_IO -> allowed for state openableState and closingState 
  if ((en_hddState & ~(fileStateChanging)) == openableState)return false;

  // flush cmd puffer to send puffer and force waiting till cmdPuff free
  // if flush answers false then some response error occured
  if (!(flush(true))) return false;

  //  check if state is o.k. and still text should be written -> write this first
  if (rb_flushWriteText)
  {
    while ( (!await_newCmd(true)) && (en_hddState == open_state) && (wTempLen > 0) && (en_hddErr == noErr) )
    {
      write(NULL, 0);
      timeEventInternControl(true);
    }
  }

  // set RS232_IO state to closeState
  lCmdWriteTime = 0;
  en_hddState = closingState;
  b_lastFileWriteFlushed = true;
  
  startSimpleCmd(26, b_writeHandle);
  b_writeHandle = 0xFF;
  return true;
}

/**
  init cmd sequence by resetting TAN with cmd readVersion
*/
void RS232_IO::readVersionHdd()
{ // flush cmd puffer to send puffer and force waiting till cmdPuff free
  // if flush answers false then some response error occured
  if (!(flush(true))) return;

  // now cmd puffer is free to use
  // reset TAN
  b_tan = 0;
  en_hddErr = syncingTan;

  startSimpleCmd(2);
}

/**
  get card information (above all to gett free space on media)
*/
void RS232_IO::getCardinfo()
{ // flush cmd puffer to send puffer and force waiting till cmdPuff free
  // if flush answers false then some response error occured
  if (!(flush(true))) return;

  en_hddState = cardReading;

  startSimpleCmd(16, 0);
  return;
}


/* ******************************** */
/* iostream related output operator */
/* ********    sending     ******** */
/* ******************************** */

/**
  reset the internal buffers for send and
  response; on demand (default) also the cmdPuff is reset
  @param rb_resetCmd also reset cmdPuff (default = true)
*/
void RS232_IO::resetCmdBuffers(bool rb_resetCmd)
{
  if (rb_resetCmd)
  {  // preserve only the text in cmd puff
    preserveWriteText(true);
    en_cmdPuffState = inactive;
    c_puffCmd.resize(4);
  }
  // clear RS232 buffers if need
  if (rec_pufferCnt() > 0) HAL::clearRs232RxBuffer();
  if (sndPufferCnt() > 0) HAL::clearRs232TxBuffer();

  en_respPuffState = respInactive;
  en_sendPuffState = inactive;
}

/**
  if cmd or send puffer must be reset, write text should be stored
  write text in cmd puff is always stored
  @param rb_storeSendPuff true -> force to store write text in send puf also
*/
void RS232_IO::preserveWriteText(bool rb_storeSendPuff)
{
  word wPreserveLen = 0,
      wPreserveSendLen = 0,
      wPreserveCmdLen = 0;
  if ( (pb_sendPuff[3] == 24) && (wSendPuffActSize >= 10) && (rb_storeSendPuff))
  { // text_cnt = 9-pre-byte + 1-chksum -> 10
    wPreserveSendLen = wSendPuffActSize - 10;
    wPreserveLen += wPreserveSendLen;
  }
  if ((en_cmdPuffState != inactive) && (c_puffCmd.size() >= 9) && (c_puffCmd[3] == 24))
  {  // text_cnt = 9-pre-byte -> 10 (chksum not in cmd)
    wPreserveCmdLen = c_puffCmd.size() - 9;
    wPreserveLen += wPreserveCmdLen;
  }
  if (wPreserveLen > 0)
  { // there is write text to save
    word wOldLen = wTempLen;
        wTempLen += wPreserveLen;
    if (wTempLen > STATIC_GLOBAL_TEMP_STRING_SIZE)
    {
      wTempLen = STATIC_GLOBAL_TEMP_STRING_SIZE;
      wPreserveLen = (wTempLen - wOldLen);
      if (wPreserveLen < wPreserveSendLen)
      {
        wPreserveSendLen = wPreserveLen;
        wPreserveCmdLen = 0;
      }
      else
      {
        wPreserveCmdLen = wPreserveLen - wPreserveSendLen;
      }
    }

    // shift old text in puffer back, because text in send and cmd puffer
    // should be written first
    // shift [0..(wOldLen-1)] -> [wPreserveLen..wTempLen] == [wPreserveLen..(wOldLen+wPreserveLen)]
    for (int iShiftInd = (wOldLen - 1); iShiftInd >= 0; iShiftInd--)
    { // shift beginning with last byte to new end of temp string
      pb_tempString[wPreserveLen + iShiftInd] = pb_tempString[iShiftInd];
    }

    // wPreserveSendLen is ony > 0 if send puffer text should be preserved
    if (wPreserveSendLen > 0)
    { // shift send[9..(sendSize-2)] -> temp[0..(sendSize-11)] with len wPreserveSendLen
      for (word wInd = 0; wInd < wPreserveSendLen; wInd++)
      {
        pb_tempString[wInd] = pb_sendPuff[wInd + 9];
      }
    } // end if write text in send puff

    // wPreserveCmdLen is ony > 0 if send puffer text should be preserved
    if (wPreserveCmdLen > 0)
    { // shift cmd[9..(cmdSize-1)] -> temp[(wPreserveSendLen-1)..(wPreserveSendLen+wPreserveCmdLen-1)] with len wPreserveCmdLen
      for (word wInd = 0; wInd < wPreserveCmdLen; wInd++)
      {
        pb_tempString[wInd + wPreserveSendLen] = c_puffCmd[wInd + 9];
      }
    } // end if write text in cmd puff
  } // end if tempLen > 0
}


/**
  check if file can immediately be used
  (a) file is open or
  (b) can be immediately be opened with free usable cmdPuffer afterwards
  @return true -> file can be used for write and read
*/
bool RS232_IO::fileUsable()
{
  if ((en_hddState == openableState)
   && (c_filename != "")
   && (await_newCmd())
   )
  { // file can be immeadetely be opened
    lCmdWriteTime = HAL::getTime();
    open(NULL);
  }
  if ( (en_hddState != open_state)
    && ( (en_hddState != openingState) || (en_cmdPuffState != inactive) || (error()))
     )
  { // file is not ready for read access
    return false;
  }
  else return true;
}

/**
  set state to repeat send of cmd due to response error
  @param rb_respErr true -> a response error is registered (default true)
*/
void RS232_IO::triggerCmdRepeat(bool rb_respErr)
{
  en_sendPuffState = flushCmd;
  // set response puffer to state for awaiting STX
  en_respPuffState = respInactive;

  // clear RS232 buffers if need
  if (rec_pufferCnt() > 0) HAL::clearRs232RxBuffer();
  if (sndPufferCnt() > 0) HAL::clearRs232TxBuffer();

  if (rb_respErr)
  { // set hdd state to stop timeEventIntern
    en_hddErr = static_cast<t_hddErr>(en_hddErr | respErr);
    lLastRespCardErr = HAL::getTime();
  }
}

/**
  read from a file on Hdd
  @param rwLen length of data to read
  @return true -> command executed
*/
bool RS232_IO::readFile(word rwLen)
{ // check if file is in read mode
  // can be immediately be used
  // EOF not reached
  if ( ((en_filemode != readMode) && ((en_filemode & rdwr) == 0))
    || ((en_eofState & readFileEof) != 0)
    || (!(flush(true)))
    || (!(fileUsable()))
     ) return false;
  
  // now cmd puffer is free to use
  // set cmd code for closeFile
  en_cmdPuffState = active;
  setCmd(23);
  // set handle
  c_puffCmd.push_back(b_writeHandle);
  // insert empty field
  c_puffCmd.push_back(0);
  // insert LSB of length
  c_puffCmd.push_back(rwLen & 0xFF);
  // insert MSB of length
  c_puffCmd.push_back(rwLen >> 8);
  // call flushCmd to send cmd as soon as possible
  flush();
  // proceed processing of send task
  timeEventInternControl(true);
  return true;
}

/**
  perform introduction work for write call
  @return true -> write can be continued
*/
bool RS232_IO::writeInternPre()
{
  if ( ((en_eofState & writeEof) != 0)
    || (!(fileUsable()))
     ) return false;
  
  if ((en_cmdPuffState == flushCmd) || (en_hddState != open_state))
  { // cmd not ready -> store text in temp
    return false;
  }
  
  // set en_cmdPuffState to active
  if (en_cmdPuffState != inactive)
  { // if puffer is not inactive -> check if its already used for write
    if (getCmd() != 24)
    { // can write if flush can't flush actual cmd away
      if (!(flush())) return false;
    }
  }
  if (en_cmdPuffState == inactive)
  {
    en_cmdPuffState = active;
  
    // set year static to 2001 -> 1980 + 21
    // set day and month to 1 and 1
    // LSB of wDate to pos 4
    c_puffCmd.push_back(10785 & 0xFF);
    // MSB of wDate to pos 5
    c_puffCmd.push_back(10785 >> 8);
    // LSB of wTime to pos 6
    c_puffCmd.push_back(2081 & 0xFF);
    // MSB of wTime to pos 7
    c_puffCmd.push_back(2081 >> 8);
    c_puffCmd.push_back(b_writeHandle);
  }
  setCmd(24); // set cmd type to writeFile

  return true;
}

/**
  write data byte string with given length on Hdd

  possible errors:
      * Err::rs232_overflow send buffer puffer overflow during send
  @param rpb_data pointer to data string
  @param rwLen length of data string
  @return true -> successful inserted
*/
bool RS232_IO::write(const byte *const rpb_data, word rwLen)
{ // first check if write has something to do
  if ( (rwLen == 0) && (wTempLen == 0) ) return true;
  // check if data can be inserted in cmd puffer
  if (!writeInternPre())
  { // text can't be inserted in cmdPuff -> save in tempString
    for (word wInd = 0; wInd < rwLen; wInd++)
    {
      pb_tempString[wTempLen] = rpb_data[wInd];
      wTempLen++;
      if (wTempLen >= STATIC_GLOBAL_TEMP_STRING_SIZE) break;
    }
    return true;
  }

  // first insert temp text in cmdPuff
  word wFreeCntInCmd =
    ((CMD_PUFFER_FLUSH_SIZE +2) > c_puffCmd.size())?((CMD_PUFFER_FLUSH_SIZE +2) - c_puffCmd.size()):0;
  if (wTempLen > 0)
  {
    word wInd = 0,
      wLimitLoop = wTempLen;
    if (wLimitLoop > wFreeCntInCmd) wLimitLoop = wFreeCntInCmd;
    if (wLimitLoop > MAX_TEMP_STRING_TO_SEND) wLimitLoop = MAX_TEMP_STRING_TO_SEND;
    byte b_temp;
    for (; (wInd < wLimitLoop); wInd++)
    { // put data into cmd
      b_temp = pb_tempString[wInd];
      c_puffCmd.push_back(b_temp);
    }
    word wBegin = 0;
    for (; wInd < wTempLen; wInd++)
    {
      pb_tempString[wBegin] = pb_tempString[wInd];
      wBegin++;
    }
    wTempLen = wBegin;

    // add parameter text at back of temp text
    for (word wIndSave = 0; wIndSave < rwLen; wIndSave++)
    {
      pb_tempString[wTempLen] = rpb_data[wIndSave];
      wTempLen++;
      if (wTempLen >= STATIC_GLOBAL_TEMP_STRING_SIZE) break;
    }
  } // end if wTempLen > 0
  else
  { // insert input data at the end of actual active cmd puffer
    word wInd = 0,
        wLimitLoop = rwLen;
    if (wLimitLoop > wFreeCntInCmd) wLimitLoop = wFreeCntInCmd;
    if (wLimitLoop > MAX_TEMP_STRING_TO_SEND) wLimitLoop = MAX_TEMP_STRING_TO_SEND;

    // place given text in cmdPuff as long as possible
    for (; wInd < wLimitLoop; wInd++)
    {
      c_puffCmd.push_back(rpb_data[wInd]);
    }
    // now place rest of rwLen in temp string
    for (; wInd < rwLen; wInd++)
    {
      pb_tempString[wTempLen] = rpb_data[wInd];
      wTempLen++;
      if (wTempLen >= STATIC_GLOBAL_TEMP_STRING_SIZE) break;
    }
  }

  writeInternPost();

  return true;
}
/**
  perform pending work for write
*/
void RS232_IO::writeInternPost()
{
  lCmdWriteTime = HAL::getTime();
  // register that data is written to file
  b_lastFileWriteFlushed = false;

  if (c_puffCmd.size() > CMD_PUFFER_FLUSH_SIZE)
  { // max len -> flushCmd this puffer
    flush();
  }
}
/**
  send string on Hdd

  possible errors:
      * Err::rs232_overflow send buffer puffer overflow during send
  @param rc_data sent data string
  @return refernce to RS232_IO for cmd like "rs232 << data1 << data2;"
*/
RS232_IO& RS232_IO::operator<<(const basic_string<char>& rrefc_data)
{
  write(((byte*)(rrefc_data.c_str())),rrefc_data.size());
  return *this;
}

/**
  send NULL terminated string on Hdd (terminating NULL isn't sent)

  possible errors:
      * Err::rs232_overflow send buffer puffer overflow during send
  @param rpc_data pointer to NULL terminated string to send
  @return refernce to RS232_IO for cmd like "rs232 << data1 << data2;"
*/
RS232_IO& RS232_IO::operator<<(const char *const rpc_data)
{
  write((const byte*)(rpc_data),strlen(rpc_data));
  return *this;
}

/**
  send single byte on Hdd

  possible errors:
      * Err::rs232_overflow send buffer puffer overflow during send
  @param rb_data sent data byte
  @return refernce to RS232_IO for cmd like "rs232 << data1 << data2;"
*/
RS232_IO& RS232_IO::operator<<(byte rb_data)
{
  byte b_data = rb_data;
  write(&b_data, 1);
  return *this;
}

/**
  send single char on Hdd

  possible errors:
      * Err::rs232_overflow send buffer puffer overflow during send
  @param rc_data sent data char
  @return refernce to RS232_IO for cmd like "rs232 << data1 << data2;"
*/
RS232_IO& RS232_IO::operator<<(char rc_data)
{
  byte c_data = rc_data;
  write(&c_data, 1);
  return *this;
}


/**
  send word (2byte unsigned) value as text on Hdd

  possible errors:
      * Err::rs232_overflow send buffer puffer overflow during send
  @param rwData sent data byte
  @return refernce to RS232_IO for cmd like "rs232 << data1 << data2;"
*/
RS232_IO& RS232_IO::operator<<(UINT_2_BYTE rwData)
{
  char pc_data[10];
  // sprintf print value as text to byte string and terminate it with '\0'
  HAL::sprintf(pc_data, "%hu", rwData);

  return operator<<(pc_data);
}

/**
  send int (2 byte unsigned) value as text on Hdd

  possible errors:
      * Err::rs232_overflow send buffer puffer overflow during send
  @param riData sent data byte
  @return refernce to RS232_IO for cmd like "rs232 << data1 << data2;"
*/
RS232_IO& RS232_IO::operator<<(INT_2_BYTE riData)
{
  char pc_data[10];
  // sprintf print value as text to byte string and terminate it with '\0'
  HAL::sprintf(pc_data, "%hi", riData);

  return operator<<(pc_data);
}
#if defined(SYSTEM_PC)
/**
  send unsigned int value as text on Hdd

  possible errors:
      * Err::rs232_overflow send buffer puffer overflow during send
  @param rwData sent data byte
  @return refernce to RS232_IO for cmd like "rs232 << data1 << data2;"
*/
RS232_IO& RS232_IO::operator<<(unsigned int rwData)
{
  char pc_data[20];
  // sprintf print value as text to byte string and terminate it with '\0'
  HAL::sprintf(pc_data, "%hu", rwData);

  return operator<<(pc_data);
}

/**
  send int value as text on Hdd

  possible errors:
      * Err::rs232_overflow send buffer puffer overflow during send
  @param riData sent data byte
  @return refernce to RS232_IO for cmd like "rs232 << data1 << data2;"
*/
RS232_IO& RS232_IO::operator<<(int riData)
{
  char pc_data[20];
  // sprintf print value as text to byte string and terminate it with '\0'
  HAL::sprintf(pc_data, "%hi", riData);

  return operator<<(pc_data);
}
#endif
/**
  send dword (4 byte unsigned) value as text on Hdd

  possible errors:
      * Err::rs232_overflow send buffer puffer overflow during send
  @param rdwData sent data byte
  @return refernce to RS232_IO for cmd like "rs232 << data1 << data2;"
*/
RS232_IO& RS232_IO::operator<<(dword rdwData)
{
  char pc_data[20];
  // sprintf print value as text to byte string and terminate it with '\0'
  HAL::sprintf(pc_data, "%lu", rdwData);

  return operator<<(pc_data);
}

/**
  send long (4 byte signed) value as text on Hdd

  possible errors:
      * Err::rs232_overflow send buffer puffer overflow during send
  @param rlData sent data byte
  @return refernce to RS232_IO for cmd like "rs232 << data1 << data2;"
*/
RS232_IO& RS232_IO::operator<<(long rlData)
{
  char pc_data[20];
  // sprintf print value as text to byte string and terminate it with '\0'
  HAL::sprintf(pc_data, "%li", rlData);

  return operator<<(pc_data);
}

/**
  send float value as text on Hdd

  possible errors:
      * Err::rs232_overflow send buffer puffer overflow during send
  @param rlData sent data byte
  @return refernce to RS232_IO for cmd like "rs232 << data1 << data2;"
*/
RS232_IO& RS232_IO::operator<<(float rf_data)
{
  char pc_data[20];
  // sprintf print value as text to byte string and terminate it with '\0'
  HAL::sprintf(pc_data, "%f", rf_data);
  // change use float format to german
  *(strstr(pc_data, ".")) = ',';

  return operator<<(pc_data);
}

/**
  validate received response: check len and chksum
  @return true -> stored len is correct AND chksum is correct
*/
bool RS232_IO::validateResponse()
{
  // read stored len from first two bytes
  word wDeclLen = getRespLen();
  // check len (stored len excludes the chksum byte at the end)
  bool b_result = (static_cast<word>(c_puffResp.size()) == (wDeclLen + 1))?true:false;
  // check checksum - initiate with first byte
  byte b_chksum = 0;
  for (word wInd = 0; wInd < wDeclLen; wInd++)
  { // XOR the actual value with puffer value
    b_chksum ^= c_puffResp[wInd];
  }
  if (b_chksum != c_puffResp[wDeclLen])
  {
    b_result = false;
  }

  return b_result;
}

/**
  prepare one cmd puffer for send: set len and chksum fields
  @param rb_puffNr number of cmd puffer {0,1}
*/
void RS232_IO::preparePuffer()
{  
  // store puffer len
  word wLen = getLen();
  // store LSB at pos 0
  c_puffCmd[0] = (wLen & 0xFF);
  // store MSB at pos 1
  c_puffCmd[1] = (wLen >> 8);
  // set TAN
  c_puffCmd[2] = b_tan;
}


/**
  send data byte string with given length from cmd puffer on RS232

  possible errors:
      * Err::rs232_overflow send buffer puffer overflow during send
  @param rpb_data pointer to data string
  @param rwLen length of data string
*/
void RS232_IO::send(FAR_MEM byte* rpb_data, word rwLen)
{
  while (HAL::getRs232TxBufCount() + rwLen > (CONFIG_RS232_DEFAULT_SND_PUF_SIZE - 10))
  { // wait by counting from 0 to 50
    HAL::wdTriggern();
    for (byte b_w = 0; b_w < 200; b_w++)b_w++;  
  }
  HAL::wdTriggern();
  word wInd = 0;
  for (; wInd < rwLen; wInd++)
  {
    if (HAL::put_rs232Char(rpb_data[wInd]) != HAL_NO_ERR) break;
  }
  if (wInd >= rwLen)
  { // send successful
    setState(Err::rs232_overflow, Err::correctState);
  }
  else
  {
    setState(Err::rs232_overflow, Err::errorState);
  }
}

/**
  insert new cmd in cmd puffer and call timeEventIntern
  to start performation
  @param rb_cmd command
  @param rwPar optional parameter
*/
void RS232_IO::startSimpleCmd(byte rb_cmd, word rwPar)
{ // set wanted cmd code
  en_cmdPuffState = active;
  setCmd(rb_cmd);
  if (rwPar != 0xFFFF)
  { // set optional parameter
    c_puffCmd.push_back(static_cast<byte>(rwPar));
  }
  // call flushCmd to send cmd as soon as possible
  flush();
  // proceed processing of send task
  timeEventInternControl();
}

// read operators
/*
  read one line
  @param pc_line pointer to byte string for the file text line
  @param rb_maxLen maximum length to read
  @param rb_delim optional delimiter (default '\n')
  @return reference to RS232_IO
*/
RS232_IO& RS232_IO::getline(char *pc_linePointer, byte rb_maxLen, char rc_delim)
{ // check if file is opened in read mode
  byte b_fileMasterMode = en_filemode & 0x3;
  if ((b_fileMasterMode != readMode) && (b_fileMasterMode != rdwr)) return *this;

  // if no information can be read from file exit function
  if ((en_eofState & readCallEof) != 0) return *this;
  
  if ((b_readFilePufferPos < CONFIG_HDD_READ_BLOCK_SIZE) && ((en_eofState & readFileEof) == 0))
  { // text in puffer reached limit -> read more text from hdd to puffer
    readFile(CONFIG_HDD_READ_BLOCK_SIZE);
  }

  byte b_ind = 0;

  // eat beginning white space, if delim is ISSPACE_DELIM
  if (rc_delim == ISSPACE_DELIM)
  { // eat till
    while ((isspace(pc_readFilePuffer[b_ind])) && (b_ind < b_readFilePufferPos))
    {
      if ((b_readFilePufferPos - b_ind) == CONFIG_HDD_READ_BLOCK_SIZE) readFile(CONFIG_HDD_READ_BLOCK_SIZE);
      b_ind++;
    }
  }

  for (; ((b_ind < rb_maxLen) && (b_ind < b_readFilePufferPos)); b_ind++)
  { 
    if (
        (pc_readFilePuffer[b_ind] == rc_delim)
     || ((rc_delim == ISSPACE_DELIM) && (isspace(pc_readFilePuffer[b_ind])))
       )
    { // delim found
      // terminate string
      b_ind++; // increment b_ind to pop delim from read puffer
      break;
    }
    else
    {  // copy text from puffer
      pc_linePointer[b_ind] = pc_readFilePuffer[b_ind];
    }
  }

  // if read string is shorter than rb_maxLen close string with '\0'
  if (b_ind < rb_maxLen) pc_linePointer[b_ind] = '\0';
  pc_linePointer[rb_maxLen-1] = '\0';

  // now move rest of read puffer to begin of puffer
  char FAR_MEM *from = pc_readFilePuffer + b_ind,
       FAR_MEM *to = pc_readFilePuffer,
       FAR_MEM *end = (pc_readFilePuffer + b_readFilePufferPos);
  while (from != end) *to = *from;
  // test if local read buffer is empty and end of file is reached
  if ( ((en_eofState & readFileEof) != 0) && (to == end) )
  { // local buffer empty
    en_eofState = t_eof_state(en_eofState | readCallEof);
  }

  return *this;
}


/**
  read a token (eat whitespace in front of, end stop before next whitespace or at end og buf)

  possible errors:
      * Err::rs232_underflow no data other than whitespace in puffer to read in token
*/
void RS232_IO::readToken()
{
  getline(pc_token, CONFIG_HDD_MAX_TOKEN_LEN, ISSPACE_DELIM);
}

/**
  receive single byte on RS232 (ignore white spaces)

  possible errors:
      * Err::rs232_underflow receive puffer underflow during receive
  @param b_data received data byte
  @return refernce to RS232_IO for cmd like "rs232 >> data1 >> data2;"
*/
RS232_IO& RS232_IO::operator>>(byte& b_data)
{
  readToken(); // it set rs232_underflow error if no data is read
  word wVal;
  HAL::sscanf(pc_token, "%hu", &wVal);
  b_data = wVal;
  return *this;
}

/**
  receive single char on RS232 (ignore white spaces)

  possible errors:
      * Err::rs232_underflow receive puffer underflow during receive
  @param c_data received data char
  @return refernce to RS232_IO for cmd like "rs232 >> data1 >> data2;"
*/
RS232_IO& RS232_IO::operator>>(char& c_data)
{
  readToken(); // it set rs232_underflow error if no data is read
  INT_2_BYTE iVal;
  HAL::sscanf(pc_token, "%hi", &iVal);
  c_data = iVal;

  return *this;
}

/**
  receive word (2byte unsigned) value as text on RS232
  (ignore white spaces)

  possible errors:
      * Err::rs232_underflow receive puffer underflow during receive
  @param wData received data byte
  @return refernce to RS232_IO for cmd like "rs232 >> data1 >> data2;"
*/
RS232_IO& RS232_IO::operator>>(UINT_2_BYTE& wData)
{
  readToken(); // it set rs232_underflow error if no data is read
  HAL::sscanf(pc_token, "%hu", &wData);
  return *this;
}

/**
  receive int (2 byte unsigned) value as text on RS232
  (ignore white spaces)

  possible errors:
      * Err::rs232_underflow receive puffer underflow during receive
  @param iData received data byte
  @return refernce to RS232_IO for cmd like "rs232 >> data1 >> data2;"
*/
RS232_IO& RS232_IO::operator>>(INT_2_BYTE& iData)
{
  readToken(); // it set rs232_underflow error if no data is read
  HAL::sscanf(pc_token, "%hi", &iData);
  return *this;
}

/**
  receive dword (4 byte unsigned) value as text on RS232
  (ignore white spaces)

  possible errors:
      * Err::rs232_underflow receive puffer underflow during receive
  @param dwData received data byte
  @return refernce to RS232_IO for cmd like "rs232 >> data1 >> data2;"
*/
RS232_IO& RS232_IO::operator>>(dword& dwData)
{
  readToken(); // it set rs232_underflow error if no data is read
  HAL::sscanf(pc_token, "%lu", &dwData);
  return *this;
}


/**
  receive long (4 byte signed) value as text on RS232
  (ignore white spaces)

  possible errors:
      * Err::rs232_underflow receive puffer underflow during receive
  @param lData received data byte
  @return refernce to RS232_IO for cmd like "rs232 >> data1 >> data2;"
*/
RS232_IO& RS232_IO::operator>>(long& lData)
{
  readToken(); // it set rs232_underflow error if no data is read
  HAL::sscanf(pc_token, "%li", &lData);
  return *this;
}
/**
  receive float value as text on RS232
  (ignore white spaces)

  possible errors:
      * Err::rs232_underflow receive puffer underflow during receive
  @param f_data received data byte
  @return refernce to RS232_IO for cmd like "rs232 >> data1 >> data2;"
*/
RS232_IO& RS232_IO::operator>>(float& f_data)
{
  readToken(); // it set rs232_underflow error if no data is read
  HAL::sscanf(pc_token, "%f", &f_data);
  return *this;
}
