/***************************************************************************
                          lbsMultiSend.cc - special class to handle GPS
                                       informations
                             -------------------
    begin                : Fri Sep 28 2001
    copyright            : (C) 2000 - 2004 by Dipl.-Inform. Achim Spangler
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
 * Copyright (C) 2000 - 2004 Dipl.-Inform. Achim Spangler                  *
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

 /**************************************************************************
 *                                                                         * 
 *     ###    !!!    ---    ===    IMPORTANT    ===    ---    !!!    ###   * 
 * Each software module, which accesses directly elements of this file,    * 
 * is considered to be an extension of IsoAgLib and is thus covered by the * 
 * GPL license. Applications must use only the interface definition out-   * 
 * side :impl: subdirectories. Never access direct elements of __IsoAgLib  * 
 * and __HAL namespaces from applications which shouldnt be affected by    * 
 * the license. Only access their interface counterparts in the IsoAgLib   * 
 * and HAL namespaces. Contact a.spangler@osb-ag:de in case your applicat- * 
 * ion really needs access to a part of an internal namespace, so that the * 
 * interface might be extended if your request is accepted.                * 
 *                                                                         * 
 * Definition of direct access:                                            * 
 * - Instantiation of a variable with a datatype from internal namespace   * 
 * - Call of a (member-) function                                          * 
 * Allowed is:                                                             * 
 * - Instatiation of a variable with a datatype from interface namespace,  * 
 *   even if this is derived from a base class inside an internal namespace* 
 * - Call of member functions which are defined in the interface class     * 
 *   definition ( header )                                                 * 
 *                                                                         * 
 * Pairing of internal and interface classes:                              * 
 * - Internal implementation in an :impl: subdirectory                     * 
 * - Interface in the parent directory of the corresponding internal class * 
 * - Interface class name IsoAgLib::iFoo_c maps to the internal class      * 
 *   __IsoAgLib::Foo_c                                                     * 
 *                                                                         * 
 * AS A RULE: Use only classes with names beginning with small letter :i:  *
 ***************************************************************************/

#define IOP_OUTPUT

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include "multisend_c.h"
#include "multisendstreamer_c.h"

#include <IsoAgLib/driver/system/impl/system_c.h>
#include <IsoAgLib/driver/can/impl/canio_c.h>
#include <IsoAgLib/comm/Scheduler/impl/scheduler_c.h>

#ifdef DEBUG
#include <supplementary_driver/driver/rs232/irs232io_c.h>
#endif

#ifdef USE_DIN_TERMINAL
  #include <IsoAgLib/comm/DIN_Terminal/impl/dinmaskupload_c.h>
#endif

#if defined( IOP_OUTPUT ) && defined( SYSTEM_PC )
  #include <fstream>
#endif

namespace __IsoAgLib {
#if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
  /** C-style function, to get access to the unique MultiSend_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  MultiSend_c& getMultiSendInstance( uint8_t rui8_instance ) { return MultiSend_c::instance( rui8_instance );};
#else
  /** C-style function, to get access to the unique MultiSend_c singleton instance */
  MultiSend_c& getMultiSendInstance( void ) { return MultiSend_c::instance();};
#endif




/** initialisation for MultiSend_c */
void MultiSend_c::init(void)
{ // clear state of b_alreadyClosed, so that close() is called one time
  clearAlreadyClosed();
  // first register in Scheduler_c
  getSchedulerInstance4Comm().registerClient( this );
  c_data.setSingletonKey( getSingletonVecKey() );

  i32_timestamp = i32_DC = i32_ack_DC = i32_pgn = i32_dataSize = 0;
  ui16_msgSize = ui16_delay = 0;
  hpb_data = NULL;
  en_sendState = Idle;
  en_sendSuccess = SendSuccess;
  pen_sendSuccessNotify = &en_sendSuccess;

  en_msgType = Din; // makes no sense anyhow...
  b_try =  b_fileCmd = b_empf = b_send = 0;
  b_abortOnTimeout = false;

  // set BYTE-ORDER of MultiSendPkg_c
  data().setByteOrder(LSB_MSB);

	// detect if a new Filter is created, so that a reconfig of the CAN_IO
	// must be done at the end of this function
	bool b_isReconfigNeeded = false;

#ifdef USE_ISO_TERMINAL
  // register to get VTStatus Messages
  uint32_t ui32_filter = (static_cast<MASK_TYPE>(TP_CONN_MANAGE_PGN) << 8);
  if (!getCanInstance4Comm().existFilter((0x1FF0000UL), ui32_filter, Ident_c::ExtendedIdent))
  { // create FilterBox
    getCanInstance4Comm().insertFilter(*this, (0x1FF0000UL), ui32_filter, false, Ident_c::ExtendedIdent);
		b_isReconfigNeeded = true;
  }
  ui32_filter = (static_cast<MASK_TYPE>(ETP_CONN_MANAGE_PGN) << 8);
  if (!getCanInstance4Comm().existFilter((0x1FF0000UL), ui32_filter, Ident_c::ExtendedIdent))
  { // create FilterBox
    getCanInstance4Comm().insertFilter(*this, (0x1FF0000UL), ui32_filter, false, Ident_c::ExtendedIdent);
		b_isReconfigNeeded = true;
  }
#endif
#ifdef USE_DIN_TERMINAL
  // create filter to receive service to broadcast member messages from LBS+
  if (!getCanInstance4Comm().existFilter((uint16_t)0x700,(uint16_t)0x700))
	{
    getCanInstance4Comm().insertFilter(*this, 0x700,0x700, true);
		b_isReconfigNeeded = true;
	}
#endif
	if ( b_isReconfigNeeded )
	{ // at least one new Filter is created -> reconfig
		// CAN_IO receive structure in CANIO_c
		getCanInstance4Comm().reconfigureMsgObj();
	}
}

/** every subsystem of IsoAgLib has explicit function for controlled shutdown
  */
void MultiSend_c::close( void ) {
  if ( ! checkAlreadyClosed() ) {
    // avoid another call
    setAlreadyClosed();
    // unregister from Scheduler_c
    getSchedulerInstance4Comm().unregisterClient( this );
  }
};

/** default destructor which has nothing to do */
MultiSend_c::~MultiSend_c(){
}

/**
  deliver reference to data pkg as reference to CANPkgExt_c
  to implement the base virtual function correct
*/
CANPkgExt_c& MultiSend_c::dataBase()
{
  return c_data;
}

#ifdef USE_DIN_TERMINAL
/**
  send a DIN multipacket message for terminal accoring to LBS+
  @param rb_send dynamic member no of sender
  @param rb_empf dynamic member no of receiver
  @param hpb_data HUGE_MEM pointer to the data
  @param ri32_dataSize size of the complete mask
  @param rui16_msgSize size of one part message (mask is parted in different messages)
  @param rb_fileCmd terminal specific file command
  @param rb_abortOnTimeout choose if transfer should be aborted on timeout
        (instead of standard resend of last message) (default false)
  @return true -> MultiSend_c was ready -> mask is spooled to target
*/
bool MultiSend_c::sendDin(uint8_t rb_send, uint8_t rb_empf, HUGE_MEM uint8_t* rhpb_data, int32_t ri32_dataSize, uint16_t rui16_msgSize, uint16_t rb_fileCmd, bool rb_abortOnTimeout)
{
  // first check if new transfer can be started - must be in Idle state
  if (en_sendState != Idle) return false;

  #if defined( DEBUG )
  IsoAgLib::getIrs232Instance() << "MultiSend_c::sendDin";
  #if defined( SYSTEM_PC )
  std::printf ("MultiSend_c::sendDin mit Laenge %d\n",ri32_dataSize );
  #endif
  #endif

  // initialise data for begin
  hpb_data = rhpb_data;
  i32_dataSize = ri32_dataSize;
  // LBS+ file uploads often demand a message size which is no
  // multiplicative from 7 -> %e.g. 1024 for Msk-Upload where 1029 is a factor of 7
  // but as only 1024 bytes have to be sent per message, and the rest must be
  // filled with 0xFF, the value 1024 is correct
  ui16_msgSize = rui16_msgSize;
  b_fileCmd = rb_fileCmd;
  b_abortOnTimeout = rb_abortOnTimeout;
  b_empf = rb_empf;
  b_send = rb_send;

  // make safe default settings for ISO specific values
  b_ext = false;
  pc_mss = NULL;

  // set initial values for DIN transfer
  en_msgType = Din;
  i32_DC = i32_ack_DC = 0;
  i32_timestamp = 0;
  setDelay(4); // DIN with short delay between single pkg - must be >= 4msec
  b_try = 1;
  en_sendState = SendRts;
  en_sendSuccess = Running;

  // now call timeEvent for first send action
  timeEvent();

  return true;
}
#endif
#ifdef USE_ISO_TERMINAL
/**
  send a ISO target multipacket message with active retrieve of data-parts to send
  @param rb_send dynamic member no of sender
  @param rb_empf dynamic member no of receiver
  @param rpc_mss allow active build of data stream parts for upload by deriving data source class
                 from IsoAgLib::MultiSendStreamer_c, which defines virtual functions to control the
                 retrieve of data to send. This is especially important for ISO_Terminal,
                 which assembles the data pool dependend on the terminal capabilities during upload
                 ( e.g. bitmap variants )
  @param ri32_pgn PGN to use for the upload
  @param rpen_sendSuccessNotify -> pointer to send state var, where the current state
          is written by MultiSend_c
  @return true -> MultiSend_c was ready -> mask is spooled to target
*/
bool MultiSend_c::sendIsoTarget(uint8_t rb_send, uint8_t rb_empf, MultiSendStreamer_c* rpc_mss, int32_t ri32_pgn, sendSuccess_t* rpen_sendSuccessNotify)
{
  return sendIsoTargetIntern(rb_send, rb_empf, NULL, rpc_mss->getStreamSize (), ri32_pgn, rpen_sendSuccessNotify, rpc_mss);
}

/**
  send a ISO target multipacket message
  @param rb_send dynamic member no of sender
  @param rb_empf dynamic member no of receiver
  @param rhpb_data HUGE_MEM pointer to the data
  @param ri32_dataSize size of the complete mask
  @param ri32_pgn PGN to use for the upload
  @param rpen_sendSuccessNotify -> pointer to send state var, where the current state
          is written by MultiSend_c
  @return true -> MultiSend_c was ready -> mask is spooled to target
*/
bool MultiSend_c::sendIsoTarget(uint8_t rb_send, uint8_t rb_empf, HUGE_MEM uint8_t* rhpb_data,
int32_t ri32_dataSize, int32_t ri32_pgn, sendSuccess_t* rpen_sendSuccessNotify)
{
  return sendIsoTargetIntern(rb_send, rb_empf, rhpb_data, ri32_dataSize, ri32_pgn, rpen_sendSuccessNotify, NULL);
}

/**
  internal function to send a ISO target multipacket message
  @param rb_send dynamic member no of sender
  @param rb_empf dynamic member no of receiver
  @param rhpb_data HUGE_MEM pointer to the data
  @param ri32_dataSize size of the complete mask
  @param ri32_pgn PGN to use for the upload
  @param rpen_sendSuccessNotify -> pointer to send state var, where the current state
          is written by MultiSend_c
  @param rpc_mss allow active build of data stream parts for upload by deriving data source class
                 from IsoAgLib::MultiSendStreamer_c, which defines virtual functions to control the
                 retrieve of data to send. This is especially important for ISO_Terminal,
                 which assembles the data pool dependend on the terminal capabilities during upload
                 ( e.g. bitmap variants )
  @return true -> MultiSend_c was ready -> mask is spooled to target
*/
bool MultiSend_c::sendIsoTargetIntern(uint8_t rb_send, uint8_t rb_empf, HUGE_MEM uint8_t* rhpb_data, int32_t ri32_dataSize, int32_t ri32_pgn, sendSuccess_t* rpen_sendSuccessNotify, MultiSendStreamer_c* rpc_mss)
{
  // first check if new transfer can be started - must be in Idle state
  if (en_sendState != Idle) return false;

  // initialise data for begin
  hpb_data = rhpb_data;
  i32_dataSize = ri32_dataSize;
  ui16_msgSize = ri32_dataSize;
  b_empf = rb_empf;
  b_send = rb_send;

  if (ri32_dataSize >= 1786) b_ext = true;
                        else b_ext = false;

  // set initial values for ISO transfer
  en_msgType = IsoTarget;
  i32_DC = i32_ack_DC = 0;
  i32_timestamp = 0;
  i32_pgn = ri32_pgn;
  setDelay(scui8_isoCanPkgDelay);
  b_try = 1;
  en_sendState = SendRts;
  en_sendSuccess = Running;
  pen_sendSuccessNotify = rpen_sendSuccessNotify;
  *pen_sendSuccessNotify = Running;
  pc_mss = rpc_mss;

  // now call timeEvent for first send action
  timeEvent();

  return true;
}

/**
  send a ISO broadcast multipacket message
  @param rb_send dynamic member no of sender
  @param rb_empf dynamic member no of receiver
  @param hpb_data HUGE_MEM pointer to the data
  @param ri32_dataSize size of the complete mask
  @return true -> MultiSend_c was ready -> mask is spooled to target
*/
bool MultiSend_c::sendIsoBroadcast(uint8_t rb_send, uint8_t rb_empf, HUGE_MEM uint8_t* rhpb_data, int32_t ri32_dataSize, int32_t ri32_pgn)
{
  // first check if new transfer can be started - must be in Idle state
  if (en_sendState != Idle) return false;

  // initialise data for begin
  hpb_data = rhpb_data;
  i32_dataSize = ri32_dataSize;
  ui16_msgSize = ri32_dataSize;
  b_empf = rb_empf;
  b_send = rb_send;

  // set initial values for ISO Broadcast
  en_msgType = IsoBroadcast;
  i32_DC = i32_ack_DC = 0;
  i32_timestamp = 0;
  i32_pgn = ri32_pgn;
  setDelay(scui8_isoCanPkgDelay);
  b_try = 1;
  en_sendState = SendRts;
  en_sendSuccess = Running;

  // now call timeEvent for first send action
  timeEvent();

  return true;
}
#endif

/**
  perform periodical actions
  @return true -> all planned activities performed in allowed time
*/
#if defined( IOP_OUTPUT ) && defined(SYSTEM_PC)
static FILE *iopFile;
static int iopSequence;
#endif

bool MultiSend_c::timeEvent( void )
{
  uint8_t ui8_nettoDataCnt;
  uint8_t ui8_pkgCnt;
  uint8_t ui8_pkgInd;
  // nothing to do if no transfer is Running
  if (en_sendSuccess != Running) return true;

  if ( Scheduler_c::getAvailableExecTime() == 0 ) {
    #if defined( DEBUG )
    IsoAgLib::getIrs232Instance() << "MultiSend_c::timeEvent --- getAvailableExecTime() == 0;\n";
    #if defined (SYSTEM_PC)
    std::printf ("MultiSend_c::timeEvent --- getAvailableExecTime() == 0;\n");
    #endif
    #endif
    return false;
  }
  int32_t i32_time = Scheduler_c::getLastTimeEventTrigger();
  // store time of last call, to get time interval between execution
  static int32_t si32_lastCall = 0;
  // only send max 1 package for first call, when execution period can't be derived
  ui8_pkgCnt = ((si32_lastCall > 0 )&&(ui16_delay > 0))?((i32_time - si32_lastCall)/ui16_delay):1;
  // update last call time
  si32_lastCall = i32_time;

  switch (en_sendState)
  {
    case SendRts:
      if ( isDelayEnd(i32_time)
        && (b_try <= 3) )
      { // send RTS command
        if (b_ext) {
          data().setData(0, static_cast<uint8_t>(eCM_RTS));
          data().setData_2ByteInteger(1, getDataSize() & 0xFFFF);
          data().setData_2ByteInteger(3, getDataSize() >> 16);
        } else {
          data().setData(0, static_cast<uint8_t>(CM_RTS));
          data().setData_2ByteInteger(1, getMsgSize());
          data().setData(3, getPkgPerMsg());
          data().setData(4, static_cast<uint8_t>(0xFF));
        }
        en_sendState = AwaitCts;
//        ui32_lastNextPacketNumberToSend = 0; // not valid, so we can check later if a CTS was resent...
        i32_timestamp = i32_time;
        ui32_offset = 0;
        b_pkgSent = 0;
        if (en_msgType == Din)
        {
          if (ui16_delay == 4) data().setData(4, static_cast<uint8_t>(0xFF));
          else data().setData(4, static_cast<uint8_t>((ui16_delay - 4)*2));
          data().setData(5, b_fileCmd);
          data().setData_2ByteInteger(6, getMsgNr());
        }
        else
        { // ISO multipacket
          #if defined( IOP_OUTPUT ) && defined( SYSTEM_PC )
          iopFile = fopen ("iop_dump.bin", "wb");
          iopSequence = 0;
          #endif
          if ( pc_mss != NULL ) pc_mss->resetDataNextStreamPart ();
          data().setData(5, static_cast<uint8_t>(i32_pgn & 0xFF));
          data().setData(6, static_cast<uint8_t>((i32_pgn >> 8) & 0xFF));
          data().setData(7, static_cast<uint8_t>(i32_pgn >> 16));
          if (en_msgType != IsoTarget) data().setData(0, static_cast<uint8_t>(CM_BAM)); // ISO_BAM cmd
          // is already set from above!!: else data().setData(0, CM_RTS); // ISO_RTS cmd
        }
        // now data fields are set -> send
        sendIntern();
      }
      break;
    case AwaitCts:
      switch (en_msgType)
      {
        case IsoBroadcast:
          if (isDelayEnd(i32_time))
          {// after delay an ISO broadcast can start send immediately
            en_sendState = SendData;
          }
          break;
        case Din:
          if (isDelayEnd(i32_time, 1000))
          { // force retry of RTS for DIN after 1sec.
            b_try += 1;
            if (b_try <= 3)
            { // do retry of RTS
              en_sendState = SendRts;
            }
            else
            { // terminate send
              abortSend();
            }
          }
          break;
        case IsoTarget:
          if (isDelayEnd(i32_time, 1250))
          { // abort send
            setSendStateIdle();
            *pen_sendSuccessNotify = en_sendSuccess = SendAborted;
          }
          return true;
      }
      break;
    case Idle:
      break;
    case AwaitEndofmsgack:
      switch (en_msgType)
      {
        case IsoTarget:
        case IsoBroadcast: // not usual - but to avoid compiler warning
          if (isDelayEnd(i32_time, 1250))
          { // abort send
            setSendStateIdle();
            *pen_sendSuccessNotify = en_sendSuccess = SendAborted;
          }
          return true;
        case Din:
          if (isDelayEnd(i32_time, 5000))
          { // force resend of last message
            if (!b_abortOnTimeout)
            {  // -> start one uint8_t after last ack byte
              i32_DC = i32_ack_DC + 1;
              en_sendState = SendData;
            }
            else
            { // abort transfer
              abortSend();
            }
          }
          break;
      }
      break;
    case SendPauseTillCts:
      if (isDelayEnd(i32_time, 500))
      { // abort send
        setSendStateIdle();
        *pen_sendSuccessNotify = en_sendSuccess = SendAborted;
      }
      break;
#if 0
    case DecideAfterSend:
      if (!isCompleteData())
      {
        if (!isCompleteMsg())
        { // its time to send next packet
          if (isDelayEnd(i32_time))
          { //! @todo check if this case is really possible!!!
            en_sendState = SendData;
          }
        }
        else
        {  // isCompleteMsg()
          switch (en_msgType)
          {
            case IsoTarget:
            case IsoBroadcast: // not usual - but to avoid compiler warning
              // wait for CTS for next part of transfer
              en_sendState = AwaitCts; // is in #if 0 block!!
              break;
            case Din:
              en_sendState = AwaitEndofmsgack;
              break;
          }
        }
      }
      else
      { // isCompleteData()
        if (en_msgType != IsoBroadcast)
        {
          en_sendState = AwaitEndofmsgack;
        }
        else
        { // ISO Broadcast
          setSendStateIdle();
          en_sendSuccess = SendSuccess;
          *pen_sendSuccessNotify = SendSuccess;
          return true;
        }
      }
      break;

#endif

    case SendData:
      uint8_t ui8_freeCnt;
      if (ui8_pkgCnt == 0) ui8_pkgCnt = 1;
      if (en_msgType == Din) ui8_freeCnt = getCanInstance4Comm().sendCanFreecnt(Ident_c::StandardIdent);
      else ui8_freeCnt = getCanInstance4Comm().sendCanFreecnt(Ident_c::ExtendedIdent);
      // send only as much pkg as fits in send buffer (with spare of 2 for other use)
      if (ui8_freeCnt < 2)
        ui8_pkgCnt = 0;
      else if (ui8_pkgCnt > (ui8_freeCnt - 2))
        ui8_pkgCnt = (ui8_freeCnt - 2);

      #if 0 // defined( SYSTEM_PC )
      // slower down if program is debugged on PC
      if ( ( en_msgType == Din ) && ( ui8_pkgCnt > 1 ) ) ui8_pkgCnt -= 1;
      #endif

      if (ui8_pkgCnt == 0){
        #if defined( DEBUG )
        IsoAgLib::getIrs232Instance() << "MultiSend_c::timeEvent --- pkgCnt == 0;\n";
        #if defined( SYSTEM_PC )
        std::printf ("MultiSend_c::timeEvent --- pkgCnt == 0;\n");
        #endif
        #endif
      }
      for (ui8_pkgInd = 0; ui8_pkgInd < ui8_pkgCnt; ui8_pkgInd++)
      {
        prepareSendMsg(ui8_nettoDataCnt);
        data().setData(0, ui32_sequenceNr & 0xFF);
        if (hpb_data != NULL) {
          data().setDataPart(hpb_data, i32_DC, ui8_nettoDataCnt);
        } else {
          pc_mss->setDataNextStreamPart (&data(), ui8_nettoDataCnt);
          #if defined( IOP_OUTPUT ) && defined( SYSTEM_PC )
          char iopBuffer [7];
          iopBuffer [0] = data().data(1);
          iopBuffer [1] = data().data(2);
          iopBuffer [2] = data().data(3);
          iopBuffer [3] = data().data(4);
          iopBuffer [4] = data().data(5);
          iopBuffer [5] = data().data(6);
          iopBuffer [6] = data().data(7);
          if (iopSequence == 0) fwrite (iopBuffer+1, 1, ui8_nettoDataCnt-1, iopFile);
                           else fwrite (iopBuffer, 1, ui8_nettoDataCnt, iopFile);
          iopSequence++;
          #endif
        }
        sendIntern();
        i32_DC += ui8_nettoDataCnt;
        b_pkgSent++; // sent on epkg so we know how much to increase the offset next time a DPO is sent
        // break if this message part is finished
        if (isCompleteData() || isCompleteMsg())
        {
#if 0
          en_sendState = DecideAfterSend;
          break;
#endif
          if (!isCompleteData())
          {
            if (isCompleteMsg())
            {  // isCompleteMsg()
              switch (en_msgType)
              {
                case IsoTarget:
                case IsoBroadcast: // not usual - but to avoid compiler warning
                  // wait for CTS for next part of transfer
                  en_sendState = AwaitCts;
                  #if defined( DEBUG )
                  IsoAgLib::getIrs232Instance() << "MultiSend_c::timeEvent --- after Sending now awaiting CTS!\n";
                  #if defined( SYSTEM_PC )
                  std::printf ("MultiSend_c::timeEvent --- after Sending now awaiting CTS!\n");
                  #endif
                  #endif
                  break;
                case Din:
                  en_sendState = AwaitEndofmsgack;
                  break;
              }
            }
          }
          else
          { // isCompleteData()
            if (en_msgType != IsoBroadcast)
            {
              #if defined( DEBUG )
              IsoAgLib::getIrs232Instance() << "MultiSend_c::timeEvent --- after complete Sending now awaiting EOMACK!\n";
              #if defined( SYSTEM_PC )
              std::printf ("MultiSend_c::timeEvent --- after complete Sending now awaiting EOMACK!\n");
              #endif
              #endif
              en_sendState = AwaitEndofmsgack;
            }
            else
            { // ISO Broadcast
              setSendStateIdle();
              *pen_sendSuccessNotify = en_sendSuccess = SendSuccess;
              return true;
            }
          }
          break;
        }
      } // for
      break;
    case SendFileEnd:
      if (en_msgType == Din)
      { // send fileEnd cmd
        data().setData(0, FileEnd);
        data().setData(1,static_cast<uint8_t>(0xFF));
        data().setData(2, static_cast<uint16_t>(0xFFFFU));
        data().setData(4, uint32_t(0xFFFFFFFFUL));
        sendIntern();
        setSendStateIdle();
        *pen_sendSuccessNotify = en_sendSuccess = SendSuccess;
        return true;
      }
      break;
  }
  return true;
};

/**
  start processing of a process msg
  ignore all invalid messages where SEND is not of a member with claimed address,
  or where EMPF isn't valid

  possible errors:
    * Err_c::elNonexistent on SEND/EMPF not registered in Monitor-List
*/

bool MultiSend_c::processMsg(){
  // don't process if no response from target of mulit packet send
  // give DINMaskUpload_c a try
  if ( (en_sendState == Idle) || (data().empf() != b_send) || (data().send() != b_empf)) {
    #ifdef USE_DIN_TERMINAL
    return getDinMaskuploadInstance4Comm().processMsg();
    #else
    return false;
    #endif
  }

  switch (data().data(0))
  {
    case eCM_CTS:
    case CM_CTS:
      // upate i32_DC, ui8_sequenceNr and b_pkgToSend
      i32_DC = read_DC();
      #if defined( DEBUG )
      IsoAgLib::getIrs232Instance() << "MultiSend_c::processMsg --- CTS received!\nFor Date from DC: " << i32_DC
        << "\n";
      #if defined( SYSTEM_PC )
      std::printf ("MultiSend_c::processMsg --- CTS received!\nFor Date from DC: %d", i32_DC );
      #endif
      #endif
      // clear send buffer
      getCanInstance4Comm().sendCanClearbuf();
      if (   (en_sendState == AwaitCts)
          || (en_sendState == SendPauseTillCts)
          || (en_sendState == AwaitEndofmsgack)
          || (en_sendState == SendData)
         )
      { // awaited (or resent-) CTS received
        i32_timestamp = System_c::getTime();
        if (en_msgType == Din)
        {
          if (data().data(4) != 0xFF) {
            setDelay(4 + (data().data(4) / 2));
          }
          // if last state was AwaitEndofmsgack then a CM_CTS means a 1sec. pause
          // change timestamp for wait to (1sec. - standardWait) ==> resulting 1sec. wait
          if (en_sendState == AwaitEndofmsgack)
          {
            i32_timestamp += (1000 - ui16_delay);
          }
        }
        else
        { // only CTS reveived on IsoTarget (not IsoBroadcast)
          ui16_msgSize = data().data(1) * 7;
          setDelay(scui8_isoCanPkgDelay);
        }
        // set counter of last acknowledged data byte
        if (en_sendState != SendData) i32_ack_DC = i32_DC - 1;

        if ( data().data(1) == 0)
        { // send pause commanded from receiver
          en_sendState = SendPauseTillCts;
        }
        else {
          if ( pc_mss != NULL )
          {
            // check if the same data as the last CTS is wanted?
            if ((uint32_t(constData().data(2)) + ((uint32_t)(constData().data(3)) << 8) + (uint32_t(constData().data(4)) << 16)) == ui32_lastNextPacketNumberToSend) {
              pc_mss->restoreDataNextStreamPart ();
            } else {
              // we're streaming on, so save this position if it should happen we resend from here...
              pc_mss->saveDataNextStreamPart ();
              if (b_ext) {
                ui32_offset += b_pkgSent;
              }
            }

            // update NextPacketNumberToSend!
            ui32_lastNextPacketNumberToSend = uint32_t(constData().data(2)) + (uint32_t(constData().data(3)) << 8) + (uint32_t(constData().data(4)) << 16);

            // send out Extended Connection Mode Data Packet Offset
            if (b_ext) {
              data().setData(0, static_cast<uint8_t>(eCM_DPO));
              data().setData(1, b_pkgToSend);
              data().setData(2, static_cast<uint8_t>(ui32_offset & 0xFF));
              data().setData(3, static_cast<uint8_t>((ui32_offset >> 8) & 0xFF));
              data().setData(4, static_cast<uint8_t>(ui32_offset >> 16));
              data().setData(5, static_cast<uint8_t>(i32_pgn & 0xFF));
              data().setData(6, static_cast<uint8_t>((i32_pgn >> 8) & 0xFF));
              data().setData(7, static_cast<uint8_t>(i32_pgn >> 16));
              ui32_sequenceNr = 0;
              b_pkgSent = 0;
              sendIntern();
            }
          } // end ( pc_mss != NULL )
          // now receiver wants to receive new data
          #if defined( DEBUG )
          IsoAgLib::getIrs232Instance() << "Start To Send Next Data Block\n";
          #if defined( SYSTEM_PC )
          std::printf ("Start To Send Next Data Block\n");
          #endif
          #endif
          en_sendState = SendData;
        } // end request to send
      } // awaited (or resent-) CTS received
      break;
    case CM_EndofMsgACK:
    case eCM_EndofMsgACK:
      if (en_sendState == AwaitEndofmsgack) {
        #if defined( DEBUG )
        IsoAgLib::getIrs232Instance() << "MultiSend_c::processMsg --- EOMACK received!\n";
        #if defined( SYSTEM_PC )
        std::printf ("MultiSend_c::processMsg --- EOMACK received!\n");
        #endif
        #endif
        // CHECK HERE IF WE'RE AWAITING AN EOMACK!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        if (en_msgType == Din)
        {
          if (data().data(4) != 0)
          { // ACK
            i32_ack_DC = readAck_DC();
            i32_DC = i32_ack_DC + 1;
            if (i32_DC < i32_dataSize)
            { // start sending next part message
              en_sendState = SendRts;
              b_try = 1;
            }
            else
            { // send fileEnd
              en_sendState = SendFileEnd;
            }
          }
          else
          { // NACK received -> resend last message (usual last 147 CAN pkg)
            // -> start one uint8_t after last ack byte
            i32_DC = i32_ack_DC + 1;
            en_sendState = SendData;
          }
        }
        else
        { // ISO target
          #if defined( IOP_OUTPUT ) && defined( SYSTEM_PC )
          fclose (iopFile);
          #endif
          setSendStateIdle();
          *pen_sendSuccessNotify = en_sendSuccess = SendSuccess;
        }
      } else { // not awaiting end of message ack
        #if defined( DEBUG )
        IsoAgLib::getIrs232Instance() << "MultiSend_c::processMsg --- EOMACK received without expecting it!\n";
        #if defined( SYSTEM_PC )
        std::printf ("MultiSend_c::processMsg --- EOMACK received without expecting it!\n");
        #endif
        #endif
      }
      break;
    case ConnAbort:
      #if defined( DEBUG )
      IsoAgLib::getIrs232Instance() << "MultiSend_c::processMsg --- ConnAbort received!\n";
      #if defined( SYSTEM_PC )
      std::printf ("MultiSend_c::processMsg --- ConnAbort received!\n");
      #endif
      #endif
      setSendStateIdle();
      *pen_sendSuccessNotify = en_sendSuccess = SendAborted;
      break;
    default:
      #ifdef USE_DIN_TERMINAL
      // give DINMaskUpload_c a try
      return getDinMaskuploadInstance4Comm().processMsg();
      #else
      return false;
      #endif
  }
  return true;
}

/**
  abort the multipacket send stream
  (important if original target isn't active any more)
*/
void MultiSend_c::abortSend()
{
  if (en_sendState != Idle)
  {
    data().setData(0, ConnAbort);
    data().setData(1, uint32_t(0xFFFFFFFFUL));
    bool b_performAbort = false;
    #ifdef USE_DIN_TERMINAL
    if (en_msgType == Din)
    { // send DIN abort msg
      data().setData(5, b_fileCmd);
      data().setData_2ByteInteger(6, getMsgNr());
      b_performAbort = true;
    }
    #endif
    #ifdef USE_ISO_TERMINAL
    if ( en_msgType != Din )
    { // send ISO abort message
      data().setData(5, static_cast<uint8_t>(i32_pgn & 0xFF));
      data().setData(6, static_cast<uint8_t>((i32_pgn >> 8) & 0xFF));
      data().setData(7, static_cast<uint8_t>(i32_pgn >> 16));
      b_performAbort = true;
    }
    #endif
    if ( b_performAbort )
    {
      sendIntern();
      setSendStateIdle();
      en_sendSuccess = SendAborted;
    }
  }
}

/**
  send a message -> set the ident and initiate sending to CAN
*/
void MultiSend_c::sendIntern()
{  // set timestamp of last send action
  i32_timestamp = System_c::getTime();
  bool b_performSend = false;
  // set identifier
  #ifdef USE_DIN_TERMINAL
  if (en_msgType == Din)
  {
    uint16_t ui16_ident = (0x700 | (b_empf << 4) | b_send);
    // data().setIdent(ui16_ident, Ident_c::StandardIdent);
    // CANPkg_c::setIdent changed to static
    CANPkg_c::setIdent(ui16_ident, Ident_c::StandardIdent);
    b_performSend = true;
  }
  #endif
  #ifdef USE_ISO_TERMINAL
  if ( en_msgType != Din )
  { // ISO
    data().setIsoPri(6);
    if (en_sendState == SendData)
    {
      if (b_ext) data().setIsoPgn(ETP_DATA_TRANSFER_PGN);
            else data().setIsoPgn(TP_DATA_TRANSFER_PGN);
    }
    else
    {
     if (b_ext) data().setIsoPgn(ETP_CONN_MANAGE_PGN);
           else data().setIsoPgn(TP_CONN_MANAGE_PGN);
    }
    data().setIsoPs(b_empf);
    data().setIsoSa(b_send);
    b_performSend = true;
  }
  #endif
  if ( !b_performSend )
  { // register error as selected protocol doesn't fit to compiled protocol
    getLbsErrInstance().registerError( LibErr_c::Precondition, LibErr_c::LbsMultipacket );
  }
  else
  {
    CANPkg_c::setLen( 8 );
    getCanInstance4Comm() << data();
  }
}

/**
  check if actual message is complete
*/
bool MultiSend_c::isCompleteMsg() const
{
  return (b_pkgToSend == 0)?true:false;
}
/**
  check if send of all data is complete
  this function returns true only if both:<ul>
  <li>all netto data sent (i32_DC >= i32_dataSize)
  <li>last message after CTS with Byte stuffing is sent
  </ul>
  @return true -> i32_DC tell that i32_dataSize ist reached
*/
bool MultiSend_c::isCompleteData() const
{
#if 0
  return ( (i32_DC >= i32_dataSize)
        && ( isCompleteMsg() )
        )?true:false;
#endif
  return ( (i32_DC >= i32_dataSize)
        && (isCompleteMsg())
        )?true:false;

}


/**
  deliver the amount of messages for IsoAgLib+ data stream
  (no problem if called for ISO)
  @return amount of messages for stored complete data size
*/
uint16_t MultiSend_c::getMsgCnt()const
{
  uint16_t ui16_result = i32_dataSize / ui16_msgSize;
  return ((i32_dataSize % ui16_msgSize) != 0)?(ui16_result+1):ui16_result;
}
/**
  deliver the amount of CAN pkg per message
  @return CAN pkg amount per message
*/
uint16_t MultiSend_c::getPkgPerMsg()const
{
  uint16_t ui16_result = ui16_msgSize / 7;
  return ((ui16_msgSize % 7) != 0)?(ui16_result+1):ui16_result;
}
/**
  deliver the message number in LBS+ data stream
  @return message number to insert in CM_RTS
*/
uint16_t MultiSend_c::getMsgNr()const
{
  return ((i32_DC / ui16_msgSize)+1);
};
/**
  read the the commanded mesage size from CTS CAN pkg
  (for IsoAgLib+ a change of ui16_msgSize is only accepted, if
  new value results in increase of Pkg amount, because
  often the commanded ui16_msgSize is not round factor of 7)
  @return commanded message size
*/
uint16_t MultiSend_c::readMsgSize()const
{
  uint16_t ui16_tempSize = ((constData().data(1) * 7) - ui16_msgSize);
  if ( (en_msgType == Din)
    && (ui16_tempSize >= ui16_msgSize)
    && (ui16_tempSize - ui16_msgSize < 7)
     )
  { // no change in amount of pkg -> deliver old value
    return ui16_msgSize;
  }
  else
  {
    return ui16_tempSize;
  }
}
/**
  read the next to be sent data position from CM_CTS
  @return new i32_DC value
*/
int32_t MultiSend_c::read_DC()
{ // take sequence nr with decrease of 1 because prepareSendMsg increment by 1
  // before first send
  if (b_ext) {
    ui32_sequenceNr = (uint32_t(constData().data(2))) + (uint32_t(constData().data(3)) << 8) + (uint32_t(constData().data(4)) << 16) - 1L;
  } else {
    ui32_sequenceNr = (constData().data(2) - 1L);
  }
  b_pkgToSend = constData().data(1);
  int32_t i32_temp_DC = ui32_sequenceNr * 7L;
  if (en_msgType == Din) i32_temp_DC += ( uint32_t(constData().getData_2ByteInteger(6) - 1) * uint32_t( ui16_msgSize ) );
  if (i32_temp_DC > i32_dataSize)
  {
    i32_temp_DC = i32_dataSize;
  }
  return i32_temp_DC;
}
/**
  read the acknowledged data amount from EndOfMsgAck
  @return amount of correct received data byte
*/
int32_t MultiSend_c::readAck_DC()
{
  ui16_msgSize = constData().getData_2ByteInteger(1);
  int32_t i32_tempAck_DC = ui16_msgSize;
  if (en_msgType == Din) i32_tempAck_DC *= constData().getData_2ByteInteger(6);
  return i32_tempAck_DC;
}


/**
  calculate the actual sequence number and
  calculate the amount of data bytes which must be placed in new CAN pkg
  @param ui8_nettoDataCnt amount of data which should be sent within this msg
*/
void MultiSend_c::prepareSendMsg(uint8_t &ui8_nettoDataCnt)
{
  ui8_nettoDataCnt = 7;
  b_pkgToSend--;
  ui32_sequenceNr++;
  if (en_msgType == Din)
  {
    // check if last pkg of message has to be sent
    // -> check if dummy 0xFF have to be inserted
    if (b_pkgToSend == 0)
    { // netto data cnt of this pkg is message size - the
      // amount of already sent uint8_t == (ui8_sequenceNr - 1) pkg's
      uint16_t ui16_pkgLastByte = ((ui32_sequenceNr - 1) * 7);
      if (ui16_pkgLastByte < ui16_msgSize) ui8_nettoDataCnt = ui16_msgSize - ui16_pkgLastByte;
      else ui8_nettoDataCnt = 0;
    }
  }

  if (i32_DC + ui8_nettoDataCnt > i32_dataSize)
  {
    ui8_nettoDataCnt = i32_dataSize - i32_DC;
  }
}


/**
  set the delay between two sent messages
  @param rui16_delay wanted delay
*/
void MultiSend_c::setDelay(uint16_t rui16_delay)
{ // if 0xFF was delay setting in CAN, this function is called
  // with (0xFF / 2) + 4 ==> 131
  // if 0 was delay setting in CAN, this function is called with
  // (0 / 2) + 4 --> 4 ==> take it
  uint16_t ui16_tempDelay = (rui16_delay != 131)?rui16_delay:4;
  if (ui16_tempDelay != ui16_delay)
  {
    ui16_delay = ui16_tempDelay;
    // be a little bit slower as the CAN sending BIOS/OS may be a little bit quicker than commanded
    getCanInstance4Comm().setSendpause(ui16_delay + 1);
  }
}

/**
  set the send state to Idle and set the CAN send pause to 0
*/
void MultiSend_c::setSendStateIdle()
{
  setDelay(0);
  en_sendState = Idle;
  // restore default uint8_t order for 2 uint8_t integers
  data().setByteOrder(LSB_MSB);
}

} // end namespace __IsoAgLib
