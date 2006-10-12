/***************************************************************************
                          multisend_c.cpp - transfer of multi message data
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

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include "multisend_c.h"

#include <IsoAgLib/driver/system/impl/system_c.h>
#include <IsoAgLib/driver/can/impl/canio_c.h>
#include <IsoAgLib/comm/Scheduler/impl/scheduler_c.h>

#ifdef DEBUG
  #ifdef SYSTEM_PC
    #include <iostream>
  #else
    #include <supplementary_driver/driver/rs232/impl/rs232io_c.h>
  #endif
#endif


static const uint8_t scui8_CM_RTS = 16;
static const uint8_t scui8_CM_CTS = 17;
static const uint8_t scui8_CM_EndofMsgACK = 19;
static const uint8_t scui8_CM_BAM = 32;
static const uint8_t scui8_eCM_RTS = 20;
static const uint8_t scui8_eCM_CTS = 21;
static const uint8_t scui8_eCM_DPO = 22;
static const uint8_t scui8_eCM_EndofMsgACK = 23;
static const uint8_t scui8_CM_ConnAbort = 255;

static const uint8_t scui8_isoCanPkgDelay = 4;



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



/** @todo set the wanted sensemaking retries/timeout values here!!! */
#define DEF_TimeOut_ChangeStringValue 1500   /* 1,5 seconds are stated in F.1 (page 96) */
#define DEF_TimeOut_ChangeChildPosition 1500 /* 1,5 seconds are stated in F.1 (page 96) */
#define DEF_Retries_TPCommands 2
#define DEF_Retries_NormalCommands 2


/** This is mostly used for debugging now... */
void SendUploadBase_c::set (uint8_t* rpui8_buffer, uint32_t rui32_bufferSize)
{
  /// Use BUFFER - NOT MultiSendStreamer!
  vec_uploadBuffer.clear();
  vec_uploadBuffer.reserve (rui32_bufferSize);

  uint32_t i=0;
  for (; i < rui32_bufferSize; i++) {
    vec_uploadBuffer.push_back (*rpui8_buffer);
    rpui8_buffer++;
  }
  for (; i < 8; i++) {
    vec_uploadBuffer.push_back (0xFF);
  }

  ui8_retryCount = 0; // hacked, no retry here!!!

  ui32_uploadTimeout = DEF_TimeOut_ChangeStringValue;

  #ifdef DEBUG_HEAP_USEAGE
  if ( vec_uploadBuffer.capacity() != sui16_lastPrintedBufferCapacity )
  {
    sui16_lastPrintedBufferCapacity = vec_uploadBuffer.capacity();
    getRs232Instance() << "ISOTerminal_c Buffer-Capa: " << sui16_lastPrintedBufferCapacity << INTERNAL_DEBUG_DEVICE_ENDL;
  }
  #endif
}


/**
  >>StringUpload<< Constructors ( Copy and Reference! )
*/
void SendUploadBase_c::set (uint16_t rui16_objId, const char* rpc_string, uint16_t overrideSendLength, uint8_t ui8_cmdByte)
{
  // if string is shorter than length, it's okay to send - if it's longer, we'll clip - as client will REJECT THE STRING (FINAL ISO 11783 SAYS: "String Too Long")
  uint16_t strLen = (CNAMESPACE::strlen(rpc_string) < overrideSendLength) ? CNAMESPACE::strlen(rpc_string) : overrideSendLength;

  /// Use BUFFER - NOT MultiSendStreamer!
  vec_uploadBuffer.clear();
  vec_uploadBuffer.reserve (((5+strLen) < 8) ? 8 : (5+strLen)); // DO NOT USED an UploadBuffer < 8 as ECU->VT ALWAYS has 8 BYTES!

  vec_uploadBuffer.push_back (ui8_cmdByte ); /* Default of ui8_cmdByte is: Command: Command --- Parameter: Change String Value (TP) */
  vec_uploadBuffer.push_back (rui16_objId & 0xFF);
  vec_uploadBuffer.push_back (rui16_objId >> 8);
  vec_uploadBuffer.push_back (strLen & 0xFF);
  vec_uploadBuffer.push_back (strLen >> 8);
  int i=0;
  for (; i < strLen; i++) {
    vec_uploadBuffer.push_back (*rpc_string);
    rpc_string++;
  }
  for (; i < 3; i++) {
    // at least 3 bytes from the string have to be written, if not, fill with 0xFF, so the pkg-len is 8!
    vec_uploadBuffer.push_back (0xFF);
  }

  if ((5+strLen) < 9)
    ui8_retryCount = DEF_Retries_NormalCommands;
  else
    ui8_retryCount = DEF_Retries_TPCommands;

  ui32_uploadTimeout = DEF_TimeOut_ChangeStringValue;

  #ifdef DEBUG_HEAP_USEAGE
  if ( vec_uploadBuffer.capacity() != sui16_lastPrintedBufferCapacity )
  {
    sui16_lastPrintedBufferCapacity = vec_uploadBuffer.capacity();
    getRs232Instance() << "ISOTerminal_c Buffer-Capa: " << sui16_lastPrintedBufferCapacity << INTERNAL_DEBUG_DEVICE_ENDL;
  }
  #endif
}



/**
  Constructor used for "normal" 8-byte CAN-Pkgs!
*/
void SendUploadBase_c::set (uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, uint8_t byte5, uint8_t byte6, uint8_t byte7, uint8_t byte8, uint32_t rui32_timeout)
{
  vec_uploadBuffer.clear();
  vec_uploadBuffer.reserve (8);

  vec_uploadBuffer.push_back (byte1);
  vec_uploadBuffer.push_back (byte2);
  vec_uploadBuffer.push_back (byte3);
  vec_uploadBuffer.push_back (byte4);
  vec_uploadBuffer.push_back (byte5);
  vec_uploadBuffer.push_back (byte6);
  vec_uploadBuffer.push_back (byte7);
  vec_uploadBuffer.push_back (byte8);

  ui8_retryCount = DEF_Retries_NormalCommands;
  ui32_uploadTimeout = rui32_timeout;
}


/**
  Constructor used for "ChangeChildPosition" >> 9 <<-byte CAN-Pkgs!
  -- Parameter "timeOut" only there as else the signature would be the same compared to 8byte+timeOut constructor!
  -- simply always pass "DEF_TimeOut_ChangeChildPosition"
*/
void SendUploadBase_c::set (uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, uint8_t byte5, uint8_t byte6, uint8_t byte7, uint8_t byte8, uint8_t byte9, uint32_t rui32_timeout)
{
  /// Use BUFFER - NOT MultiSendStreamer!
  vec_uploadBuffer.clear();
  vec_uploadBuffer.reserve (9);

  vec_uploadBuffer.push_back (byte1);
  vec_uploadBuffer.push_back (byte2);
  vec_uploadBuffer.push_back (byte3);
  vec_uploadBuffer.push_back (byte4);
  vec_uploadBuffer.push_back (byte5);
  vec_uploadBuffer.push_back (byte6);
  vec_uploadBuffer.push_back (byte7);
  vec_uploadBuffer.push_back (byte8);
  vec_uploadBuffer.push_back (byte9);

  ui8_retryCount = DEF_Retries_TPCommands;
  ui32_uploadTimeout = rui32_timeout;
}


const SendUploadBase_c& SendUploadBase_c::operator= (const SendUploadBase_c& ref_source)
{
  vec_uploadBuffer = ref_source.vec_uploadBuffer;
  ui8_retryCount = ref_source.ui8_retryCount;
  ui32_uploadTimeout = ref_source.ui32_uploadTimeout;
  return ref_source;
}


SendUploadBase_c::SendUploadBase_c (const SendUploadBase_c& ref_source)
  : vec_uploadBuffer (ref_source.vec_uploadBuffer)
  , ui8_retryCount (ref_source.ui8_retryCount)
  , ui32_uploadTimeout (ref_source.ui32_uploadTimeout)
{}




  /************************************/
 /**** MultiSend_c Implementation ****/
/************************************/

// SendStream subclass implementation
/////////////////////////////////////

/**
  this init() function immediately sends out the first package (RTS or FpFirstFrame
  */
void
MultiSend_c::SendStream_c::init (const ISOName_c& rrefc_isoNameSender, const ISOName_c& rrefc_isoNameReceiver, const HUGE_MEM uint8_t* rhpb_data, uint32_t rui32_dataSize, sendSuccess_t& rrefen_sendSuccessNotify, uint32_t rui32_pgn, IsoAgLib::iMultiSendStreamer_c* rpc_mss, msgType_t ren_msgType)
{
  ui32_pgn = rui32_pgn;
  c_isoNameSender = rrefc_isoNameSender;     // copy the 8byte IsoName
  c_isoNameReceiver = rrefc_isoNameReceiver; // copy the 8byte IsoName
  hpb_data = rhpb_data;
  ui32_dataSize = rui32_dataSize;   // initialise data for begin
  pc_mss = rpc_mss;
  en_msgType = ren_msgType;
  pen_sendSuccessNotify = &rrefen_sendSuccessNotify;

  ui32_dataBufferOffset = 0;
  ui8_sequenceNr = 0; // sequence number always start with 0, so that's okay! (for both ISO and NMEA)
  b_pkgSent = 0;

  /// Do send out the first packet right now, right here!
  MultiSendPkg_c& refc_multiSendPkg = refc_multiSend.data();

#if defined (NMEA_2000_FAST_PACKET)
  if (en_msgType == NmeaFastPacket)
  { // send Fast Packet First Frame
    ui8_FpSequenceCounter = refc_multiSend.allocFpSequenceCounter();
    refc_multiSendPkg.setUint8Data(0, static_cast<uint8_t>((ui8_FpSequenceCounter << 5) & 0xE0));
    refc_multiSendPkg.setUint8Data(1, static_cast<uint8_t>(getDataSize() & 0xFF));

    uint8_t ui8_nettoCnt = 6;
    if (ui8_nettoCnt > getDataSize()) ui8_nettoCnt = getDataSize();
    if (hpb_data != NULL) {
      refc_multiSendPkg.setFastPacketDataPart(hpb_data, 0, ui8_nettoCnt, 2);
    } else {
      pc_mss->setDataNextFastPacketStreamPart (&refc_multiSendPkg, ui8_nettoCnt, 2);
    }
    ui32_dataBufferOffset = ui8_nettoCnt;
    switchToState (SendData);
  }
  else
#endif
  { // NOT NmeaFastPacket ==> some ISO protocol
    refc_multiSendPkg.setUint16Data(1, getDataSize() & 0xFFFF);                             // Byte 2+3
    refc_multiSendPkg.setUint8Data (5, static_cast<uint8_t>(ui32_pgn & 0xFF));              // Byte 6
    refc_multiSendPkg.setUint16Data(6, static_cast<uint16_t>((ui32_pgn >> 8) & 0xFFFFU));   // Byte 7+8
    if (en_msgType == IsoETP)
    {
      refc_multiSendPkg.setUint8Data (0, static_cast<uint8_t>(scui8_eCM_RTS));              // Byte 1
      refc_multiSendPkg.setUint16Data(3, getDataSize() >> 16);                              // Byte 4+5
      ui32_dataPacketOffset = 0;
      switchToState (AwaitCts);
    }
    else if (en_msgType == IsoTP)
    {
      refc_multiSendPkg.setUint8Data (0, static_cast<uint8_t>(scui8_CM_RTS));               // Byte 1
      refc_multiSendPkg.setUint8Data (3, static_cast<uint8_t>((ui32_dataSize + 6) / 7));    // Byte 4
      refc_multiSendPkg.setUint8Data (4, static_cast<uint8_t>(0xFF));                       // Byte 5
      switchToState (AwaitCts);
    }
    else if (en_msgType == IsoTPbroadcast)
    {
      refc_multiSendPkg.setUint8Data (0, static_cast<uint8_t>(scui8_CM_BAM));               // Byte 1
      refc_multiSendPkg.setUint8Data (3, static_cast<uint8_t>((ui32_dataSize + 6) / 7));    // Byte 4
      refc_multiSendPkg.setUint8Data (4, static_cast<uint8_t>(0xFF));                       // Byte 5
      ui8_burstPkgYetToSend = (ui32_dataSize + 6) / 7;
      switchToState (SendData);
    }
    ui32_lastNextPacketNumberToSend = 0xFFFFFFFF; // so the first coming CTS is definitively NO repeated burst!
    if (pc_mss) pc_mss->resetDataNextStreamPart();
  }
  sendPacket();
  *pen_sendSuccessNotify = Running;
}



void
MultiSend_c::singletonInit()
{
  setAlreadyClosed(); // so init() will init ;-)
  init();
};



/** initialisation for MultiSend_c */
void
MultiSend_c::init(void)
{ // clear state of b_alreadyClosed, so that close() is called one time
  // only init if closed (constructor "closes" it so it gets init'ed initially!
  if (checkAlreadyClosed())
  {
    // clear state of b_alreadyClosed, so that close() is called one time AND no more init()s are performed!
    clearAlreadyClosed();

    // first register in Scheduler_c
    getSchedulerInstance4Comm().registerClient( this );
    c_data.setSingletonKey( getSingletonVecKey() );

    // register to get ISO monitor list changes
    __IsoAgLib::getIsoMonitorInstance4Comm().registerSaClaimHandler( this );

    #if defined(NMEA_2000_FAST_PACKET)
    ui8_nextFpSequenceCounter = 0;
    #endif

    // Receive filters for ISO 11783 are created selectively in the SA claim handler,
    // so that only messages that are directed to a local SA are received
  }
}



/** every subsystem of IsoAgLib has explicit function for controlled shutdown
  */
void MultiSend_c::close()
{
  if (!checkAlreadyClosed())
  {
    // avoid another call
    setAlreadyClosed();

    // unregister from Scheduler_c
    getSchedulerInstance4Comm().unregisterClient( this );

    // unregister ISO monitor list changes
    __IsoAgLib::getIsoMonitorInstance4Comm().deregisterSaClaimHandler( this );
  }
}


/**
  @return an "in-progress" stream or NULL if none active for this sa/da-key
*/
MultiSend_c::SendStream_c*
MultiSend_c::getSendStream(const ISOName_c& rrefc_isoNameSender, const ISOName_c& rrefc_isoNameReceiver)
{
  for (std::list<SendStream_c>::iterator pc_iter=list_sendStream.begin(); pc_iter != list_sendStream.end(); pc_iter++)
  {
    if (pc_iter->matchSaDa(rrefc_isoNameSender, rrefc_isoNameReceiver))
      return &*pc_iter;
  }
  return NULL;
}



/**
  use this function to add a new SendStream.
  IMPORTANT: Assure that the added SendStream is initialized right after this call!!
  @return reference to added SendStream ==> HAS TO BE INITIALIZED, because it may be a copy of the first (to avoid stack creation of new object)
*/
MultiSend_c::SendStream_c*
MultiSend_c::addSendStream(const ISOName_c& rrefc_isoNameSender, const ISOName_c& rrefc_isoNameReceiver)
{
  /** @todo check if sender is unspecified!
            receiver may be - that indicated broadcast!?!?! */
  SendStream_c* const pc_foundStream = getSendStream(rrefc_isoNameSender, rrefc_isoNameReceiver);
  if (pc_foundStream)
  {
    if (!pc_foundStream->isFinished())
    {
      return NULL; // can't start a sendStream, one already active for this one..
    }
    else // use this finished one because it would be deleted anyway...
    {
      #ifdef DEBUG
      INTERNAL_DEBUG_DEVICE << "Using Stream from list which is already finished but not yet kicked from MultiSend_c::timeEvent()."<< INTERNAL_DEBUG_DEVICE_ENDL;
      #endif
      return pc_foundStream;
    }
  }

  if (list_sendStream.empty()) {
    list_sendStream.push_back (SendStream_c(*this SINGLETON_VEC_KEY_USE4CALL ));
    getCanInstance4Comm().setSendpause (scui8_isoCanPkgDelay + 1); /** @todo remove if there's no minimum between data-packets! */
  } else {
    list_sendStream.insert (list_sendStream.end(), list_sendStream.back()); // insert a copy of the first element (for performance reasons)
  }
  return &list_sendStream.back();
}






/**
  internal function to send a ISO target multipacket message
  @param rb_send dynamic member no of sender
  @param rb_empf dynamic member no of receiver
  @param rhpb_data HUGE_MEM pointer to the data
  @param ri32_dataSize size of the complete mask
  @param ri32_pgn PGN to use for the upload
  @param rrefen_sendSuccessNotify -> pointer to send state var, where the current state
          is written by MultiSend_c
  @param rpc_mss allow active build of data stream parts for upload by deriving data source class
                 from IsoAgLib::iMultiSendStreamer_c, which defines virtual functions to control the
                 retrieve of data to send. This is especially important for ISO_Terminal,
                 which assembles the data pool dependent on the terminal capabilities during upload
                 ( e.g. bitmap variants )
  @return true -> MultiSend_c was ready -> mask is spooled to target
*/
bool
MultiSend_c::sendIntern (const ISOName_c& rrefc_isoNameSender, const ISOName_c& rrefc_isoNameReceiver, const HUGE_MEM uint8_t* rhpb_data, int32_t ri32_dataSize, sendSuccess_t& rrefen_sendSuccessNotify, int32_t ri32_pgn, IsoAgLib::iMultiSendStreamer_c* rpc_mss, msgType_t ren_msgType)
{
  /// first check if new transfer can be started
  /// - is the sender correct?
  if (!getIsoMonitorInstance4Comm().existIsoMemberISOName (rrefc_isoNameSender)) return false;

  SendStream_c* pc_newSendStream;
  /// - is the receiver correct?
  if ((ren_msgType == IsoTPbroadcast) || (ren_msgType == NmeaFastPacket))
  { // no destination-isoname checks needed for broadcast messages!
    // Force destination to be "ISONameUnspecified"
    /// - check if there's already a SA/DA pair active (in this case NULL is returned!)
    /// - if not NULL is returned, it points to the newly generated stream.
    pc_newSendStream = addSendStream (rrefc_isoNameSender, ISOName_c::ISONameUnspecified);
  }
  else
  { // destination specific - so the receiver must be registered!
    if (!getIsoMonitorInstance4Comm().existIsoMemberISOName (rrefc_isoNameReceiver))
    return false;
    /// - check if there's already a SA/DA pair active (in this case NULL is returned!)
    /// - if not NULL is returned, it points to the newly generated stream.
    pc_newSendStream = addSendStream (rrefc_isoNameSender, rrefc_isoNameReceiver);
  }

  if (pc_newSendStream)
  {
    pc_newSendStream->init (rrefc_isoNameSender, rrefc_isoNameReceiver, rhpb_data, ri32_dataSize, rrefen_sendSuccessNotify, ri32_pgn, rpc_mss, ren_msgType);
    return true;
  }
  else
  {
    return false;
  }
}




/**
  @return true: stream finished
*/
bool
MultiSend_c::SendStream_c::timeEvent (uint8_t rui8_pkgCnt, int32_t ri32_time)
{
  if (isFinished ())
  { // SendStream finished in processMsg() or by user-abort, so return true for deletion of SendStream.
    return true;
  }

  MultiSendPkg_c& refc_multiSendPkg = refc_multiSend.data();
  uint8_t ui8_nettoDataCnt;
  uint8_t ui8_pkgInd;

  switch (en_sendState)
  {
    case AwaitCts:
      switch (en_msgType)
      {
        case NmeaFastPacket:
        case IsoTPbroadcast:
          // this state shouldn't be possible in these cases!
          break;
        case IsoTP: // break left out intentionally
        case IsoETP:
          if (isDelayEnd(ri32_time, 1250))
          { // abort send
            abortSend();
            return true; // FINISHED SendStream, remove it from list please!
          }
      }
      break;

    case AwaitEndofmsgack:
      switch (en_msgType)
      {
        case IsoTP: // break left out intentionally
        case IsoETP:
          if (isDelayEnd(ri32_time, 1250))
          { // abort send
            abortSend();
            return true; // FINISHED SendStream, remove it from list please!
          }
          break;
        case IsoTPbroadcast: // we shouldn't reach this state!
        case NmeaFastPacket: // we shouldn't reach this state!
          break;
      }
      break;

    case SendPauseTillCts:
      if (isDelayEnd(ri32_time, 500))
      { // abort send
        abortSend();
        return true; // FINISHED SendStream, remove it from list please!
      }
      break;

    case SendData:
    {
#if defined (NMEA_2000_FAST_PACKET)
      if (en_msgType == NmeaFastPacket)
      {
        uint8_t ui8_freeCnt = getCanInstance4Comm().sendCanFreecnt(Ident_c::ExtendedIdent);

        // send only as much pkg as fits in send buffer (with spare of 2 for other use)
        if (ui8_freeCnt < 2)
           rui8_pkgCnt = 0;
        else if (rui8_pkgCnt > (ui8_freeCnt - 2))
           rui8_pkgCnt = (ui8_freeCnt - 2);

        if (rui8_pkgCnt == 0)        {
            #if defined( DEBUG )
            INTERNAL_DEBUG_DEVICE << "MultiSend_c::timeEvent --- pkgCnt == 0;" << INTERNAL_DEBUG_DEVICE_ENDL;
            #endif
        }
        for (ui8_pkgInd = 0; ui8_pkgInd < rui8_pkgCnt; ui8_pkgInd++)
        {
          prepareSendMsg(ui8_nettoDataCnt);
          refc_multiSendPkg.setUint8Data(0, static_cast<uint8_t>(((ui8_FpSequenceCounter << 5) |(ui8_sequenceNr & 0x1F)) ) );

          if (hpb_data != NULL) {
            refc_multiSendPkg.setFastPacketDataPart(hpb_data, ui32_dataBufferOffset, ui8_nettoDataCnt, 1);
          } else {
            pc_mss->setDataNextFastPacketStreamPart (&refc_multiSendPkg, ui8_nettoDataCnt, 1);
          }
          sendPacket();
          ui32_dataBufferOffset += ui8_nettoDataCnt;
          //b_pkgSent++; // sent on epkg so we know how much to increase the offset next time a DPO is sent
          // break if this message part is finished
          if (isCompleteData())
          {
            *pen_sendSuccessNotify = SendSuccess;
            return true; // FINISHED SendStream, remove it from list please!
          }
        } // for
      }
      else
#endif
      { // NOT NmeaFastPacket - some ISO protocol!
        if (en_msgType == IsoTPbroadcast)
        { // IsoTPbroadcast forces 50ms between all packets!!
          if (!isDelayEnd (ri32_time, 50))
          { // if 50ms has not elapsed, wait on...
            break;
          }
          rui8_pkgCnt = 1; // only send 1 packet at a time, then wait 50ms.
          switchToState (SendData); // same state - but the time stamp gets updated, so we'll wait on for 50ms...
        }
        else
        { // IsoTP || IsoETP
          const uint8_t cui8_freeCnt = getCanInstance4Comm().sendCanFreecnt(Ident_c::ExtendedIdent);
          if (rui8_pkgCnt == 0) rui8_pkgCnt = 1;
          // send only as much pkg as fits in send buffer (with spare of 2 for other use)
          if (cui8_freeCnt < 2)
            rui8_pkgCnt = 0;
          else if (rui8_pkgCnt > (cui8_freeCnt - 2))
            rui8_pkgCnt = (cui8_freeCnt - 2);
        }

        for (ui8_pkgInd = 0; ui8_pkgInd < rui8_pkgCnt; ui8_pkgInd++)
        {
          prepareSendMsg (ui8_nettoDataCnt);
          refc_multiSendPkg.setUint8Data (0, ui8_sequenceNr);
          if (hpb_data != NULL) {
            refc_multiSendPkg.setDataPart (hpb_data, ui32_dataBufferOffset, ui8_nettoDataCnt);
          } else {
            pc_mss->setDataNextStreamPart (&refc_multiSendPkg, ui8_nettoDataCnt);
          }
          sendPacket();
          ui32_dataBufferOffset += ui8_nettoDataCnt;
          b_pkgSent++; // sent on epkg so we know how much to increase the offset next time a DPO is sent
          // break if this message part is finished
          if (isCompleteData())
          {
            if (en_msgType == IsoTPbroadcast)
            { // ISO Broadcast
              *pen_sendSuccessNotify = SendSuccess;
              return true; // FINISHED SendStream, remove it from list please!
            }
            else
            { // ISO Target
              #if defined( DEBUG )
              INTERNAL_DEBUG_DEVICE << "MultiSend_c::timeEvent --- after complete Sending now awaiting EOMACK!" << INTERNAL_DEBUG_DEVICE_ENDL;
              #endif
              switchToState (AwaitEndofmsgack);
              break; // stream not yet finished!
            }
          }
          // not completely finished, but maybe this burst has finished?
          if (isCompleteBurst())
          { // wait for CTS for next part of transfer
            switchToState (AwaitCts);
            #if defined( DEBUG )
            INTERNAL_DEBUG_DEVICE << "MultiSend_c::timeEvent --- after Sending now awaiting CTS!" << INTERNAL_DEBUG_DEVICE_ENDL;
            #endif
            break; // stream not yet finished!
          }
          // nothing special, keep on sending...
        } // for - sent all pkgs for now, yet some more to go, send them in a later timeEvent()
      } // Not Nmea- some ISO protocol
    } break; // case SendData
  }
  return false; // stream not yet finished!
}



/**
  perform periodical actions
  @return true -> all planned activities performed in allowed time
*/
bool
MultiSend_c::timeEvent()
{
  if ( getAvailableExecTime() == 0 ) return false;

  // nothing to do if no transfer is Running
  if (list_sendStream.empty()) return true;

  const int32_t ci32_time = ElementBase_c::getLastRetriggerTime();
  // store time of last call, to get time interval between execution
  static int32_t si32_lastCall = 0;
  // only send max 1 package for first call, when execution period can't be derived
  uint8_t ui8_pkgCnt = (si32_lastCall > 0 ) ? ((ci32_time - si32_lastCall)/scui8_isoCanPkgDelay) : 1;
  // update last call time
  si32_lastCall = ci32_time;

  const uint8_t cui8_pkgCntForEach = ui8_pkgCnt / list_sendStream.size(); // in case it gets 0 after division, it is set to 1 inside of SendStream's timeEvent().

  // Call each SendStream_c's timeEvent()
  for (std::list<SendStream_c>::iterator pc_iter=list_sendStream.begin(); pc_iter != list_sendStream.end();)
  {
    if (pc_iter->timeEvent (cui8_pkgCntForEach, ci32_time))
    { // SendStream finished
      pc_iter = list_sendStream.erase (pc_iter);
      #ifdef DEBUG
      INTERNAL_DEBUG_DEVICE << "Kicked SendStream because it finished (abort or success)!" << INTERNAL_DEBUG_DEVICE_ENDL;
      #endif
    }
    else
    { // SendStream not yet finished
      pc_iter++;
    }
  }

  if ((list_sendStream.size() == 0))
  { // (re-)set the CAN send pause to 0, because not a single SendStream is active anymore.
    getCanInstance4Comm().setSendpause (0); /** @todo remove if there's no minimum between data-packets! */
  }

  return true;
};



bool
MultiSend_c::SendStream_c::processMsg()
{
  /***/ MultiSendPkg_c&  refc_multiSendPkg = refc_multiSend.data();
  const MultiSendPkg_c& refcc_multiSendPkg = refc_multiSend.constData();

  if (isFinished() || (en_msgType == NmeaFastPacket) || (en_msgType == IsoTPbroadcast))
  { // nothing to come in for us when we're broadcasting or already done (succeeded or aborted)
    return false; // anyway, return with false here for "safety"
  }

  switch (refc_multiSendPkg.getUint8Data (0))
  {
    case scui8_eCM_CTS:
    case scui8_CM_CTS:
      if (   (en_sendState == AwaitCts)
          || (en_sendState == SendPauseTillCts)
          || (en_sendState == AwaitEndofmsgack)
          || (en_sendState == SendData)
         )
      { // awaited (or resent-) CTS received -- only possible in ISO protocol case
        ui8_burstPkgYetToSend = refcc_multiSendPkg.getUint8Data (1);
        ui32_dataBufferOffset = (( (en_msgType == IsoETP) ? ((uint32_t(refcc_multiSendPkg.getUint16Data (2))) + (uint32_t(refcc_multiSendPkg.getUint8Data (4)) << 16))
                                                          : (refcc_multiSendPkg.getUint8Data (2))
                                ) - 1UL
                                ) * 7L; // take sequence nr with -1 because prepareSendMsg increment by 1 before first send
        if (ui32_dataBufferOffset >= ui32_dataSize)
        { // request is out of buffer!
          abortSend();
          break;
        }

        if ( refc_multiSendPkg.getUint8Data(1) == 0)
        { // send pause commanded from receiver
          switchToState (SendPauseTillCts);
        }
        else
        { // really some packets requested
          // check if the same data as the last CTS is wanted?
          uint32_t ui32_pkgCTSd = uint32_t(refcc_multiSendPkg.getUint8Data(2));
          if (en_msgType == IsoETP) {
             ui32_pkgCTSd += (uint32_t(refcc_multiSendPkg.getUint16Data(3)) << 8);
          }

          if ( pc_mss != NULL )
          {
            if (ui32_pkgCTSd == ui32_lastNextPacketNumberToSend) {
              pc_mss->restoreDataNextStreamPart ();
            } else {
              // we're streaming on, so save this position if it should happen we resend from here...
              pc_mss->saveDataNextStreamPart ();
            }
          }
          // else: nothing to do here, as we have the complete buffer available and the "ui32_dataBufferOffset" has been set above

          // send out Extended Connection Mode Data Packet Offset
          if (en_msgType == IsoETP)
          {
            if ((ui32_pkgCTSd != ui32_lastNextPacketNumberToSend))
            { // increase DPO in case of no re-request of the last burst
              ui32_dataPacketOffset += b_pkgSent;
            }
            refc_multiSendPkg.setUint8Data (0, static_cast<uint8_t>(scui8_eCM_DPO));
            refc_multiSendPkg.setUint8Data (1, static_cast<uint8_t>(ui8_burstPkgYetToSend));
            refc_multiSendPkg.setUint16Data(2, static_cast<uint16_t>(ui32_dataPacketOffset & 0xFFFF));
            refc_multiSendPkg.setUint8Data (4, static_cast<uint8_t>(ui32_dataPacketOffset >> 16));
            // change order of setUint8Data(), setUint16Data() so that
            // setUint16Data() with equal position, so that a faster assignment is enabled
            refc_multiSendPkg.setUint8Data (5, static_cast<uint8_t>(ui32_pgn & 0xFF));
            refc_multiSendPkg.setUint16Data(6, static_cast<uint16_t>(ui32_pgn >> 8));
            ui8_sequenceNr = 0; // reset sequence number, as we're advancing with the DPO
            b_pkgSent = 0;
            sendPacket();
          }
          // update NextPacketNumberToSend!
          ui32_lastNextPacketNumberToSend = ui32_pkgCTSd;

          // now receiver wants to receive new data
          #if defined( DEBUG )
          INTERNAL_DEBUG_DEVICE << "Start To Send Next Data Block" << INTERNAL_DEBUG_DEVICE_ENDL;
          #endif
          switchToState (SendData);
        } // end request to send
      } // awaited (or resent-) CTS received
      // else: nothing to do on "SendRts
      break;
    case scui8_CM_EndofMsgACK:
    case scui8_eCM_EndofMsgACK:
      if (en_sendState == AwaitEndofmsgack)
      {
        #if defined( DEBUG )
        INTERNAL_DEBUG_DEVICE << "MultiSend_c::processMsg --- EOMACK received!" << INTERNAL_DEBUG_DEVICE_ENDL;
        #endif
        // Notify sender that it finished!
        *pen_sendSuccessNotify = SendSuccess; // will be kicked out after next timeEvent!
      }
      // else: not awaiting end of message ack, no action taken for this error-case in normal operation.
      getLibErrInstance().registerError( LibErr_c::MultiSendWarn, LibErr_c::MultiSend );
      break;
    case scui8_CM_ConnAbort:
      #if defined( DEBUG )
      INTERNAL_DEBUG_DEVICE << "MultiSend_c::processMsg --- ConnAbort received!" << INTERNAL_DEBUG_DEVICE_ENDL;
      #endif
      *pen_sendSuccessNotify = SendAborted; // will be kicked out after next timeEvent!
      return false; // in case a MultiSend & MultiReceive are running parallel, then this ConnAbort should be for both!
    default:
      return false;
  }
  return true;
}



/**
  start processing of a process msg
  ignore all invalid messages where SEND is not of a member with claimed address,
  or where EMPF isn't valid

  possible errors:
    * Err_c::elNonexistent on SEND/EMPF not registered in Monitor-List
  @return true -> message was processed; else the received CAN message will be served to other matching CANCustomer_c
*/
bool
MultiSend_c::processMsg()
{
  if (data().getLen() != 8)
    return true; // All corrupted (E)TP-Packages should NOT be of interest for anybody...

  SendStream_c* pc_sendStreamFound = getSendStream (data().getISONameForDA(), data().getISONameForSA()); // sa/da swapped, of course ;-) !
  if (pc_sendStreamFound)
  { // found a matching SendStream, so call its processMsg()
    return pc_sendStreamFound->processMsg();
  }
  else
  { // no matching SendStream found
    return false;
  }
}



/**
  abort the multipacket send stream
  (important if original target isn't active any more)
  IMPORTANT: After calling abortSend() please exit timeEvent() with true, so the SendStream gets deleted!!
*/
void
MultiSend_c::SendStream_c::abortSend()
{
  MultiSendPkg_c& refc_multiSendPkg = refc_multiSend.data();

  refc_multiSendPkg.setUint8Data (0, static_cast<uint8_t>(scui8_CM_ConnAbort));
  refc_multiSendPkg.setUint32Data(1, uint32_t(0xFFFFFFFFUL));
  refc_multiSendPkg.setUint8Data (5, static_cast<uint8_t>(ui32_pgn & 0xFF));
  refc_multiSendPkg.setUint16Data(6, static_cast<uint16_t>(ui32_pgn >> 8));
  switchToState (AwaitCts); /// Attention!!! - Only setting "en_sendState" to anything other than "SendData" so CONNMANAGE PGN is used instead of DATATRANSFER.
  sendPacket();
  *pen_sendSuccessNotify = SendAborted;
}


/** this function is called by ISOMonitor_c when a new CLAIMED ISOItem_c is registered.
 * @param refc_isoName const reference to the item which ISOItem_c state is changed
 * @param rpc_newItem pointer to the currently corresponding ISOItem_c
 */
void MultiSend_c::reactOnMonitorListAdd( const ISOName_c& refc_isoName, const ISOItem_c* rpc_newItem )
{
  /** @todo to be improved!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
  if ( getIsoMonitorInstance4Comm().existLocalIsoMemberISOName(refc_isoName) )
  { // local ISOItem_c has finished adr claim
    bool b_isReconfigNeeded = false;
    uint32_t ui32_nr = rpc_newItem->nr();
    // only ISO msgs with own SA in PS (destination)
    uint32_t ui32_filter = ((static_cast<MASK_TYPE>(TP_CONN_MANAGE_PGN) | static_cast<MASK_TYPE>(ui32_nr)) << 8);
    if (!getCanInstance4Comm().existFilter( *this, (0x1FFFF00UL), ui32_filter, Ident_c::ExtendedIdent))
    { // create FilterBox
      getCanInstance4Comm().insertFilter( *this, (0x1FFFF00UL), ui32_filter, true, Ident_c::ExtendedIdent);
      b_isReconfigNeeded = true;
    }

    ui32_filter = ((static_cast<MASK_TYPE>(ETP_CONN_MANAGE_PGN) | static_cast<MASK_TYPE>(ui32_nr)) << 8);
    if (!getCanInstance4Comm().existFilter( *this, (0x1FFFF00UL), ui32_filter, Ident_c::ExtendedIdent))
    { // create FilterBox
      getCanInstance4Comm().insertFilter( *this, (0x1FFFF00UL), ui32_filter, true, Ident_c::ExtendedIdent);
      b_isReconfigNeeded = true;
    }
    if ( b_isReconfigNeeded ) getCanInstance4Comm().reconfigureMsgObj();
  }

  #ifdef DEBUG
  INTERNAL_DEBUG_DEVICE << "reactOnMonitorListAdd() handles CLAIM of ISOItem_c for device with DevClass: " << int(refc_isoName.devClass())
      << ", Instance: " << int(refc_isoName.devClassInst()) << ", and manufacturer ID: " << int(refc_isoName.manufCode())
      << "NOW use SA: " << int(rpc_newItem->nr()) << INTERNAL_DEBUG_DEVICE_NEWLINE << INTERNAL_DEBUG_DEVICE_NEWLINE
      << INTERNAL_DEBUG_DEVICE_ENDL;
  #endif
  // no resurrection here as we do NOT (yet) save the isoName/isoname to our SendStream_c instances in the list...
  // this can be done later if someone thinks that makes sense...
}


/** this function is called by ISOMonitor_c when a device looses its ISOItem_c.
 * @param refc_isoName const reference to the item which ISOItem_c state is changed
 * @param rui8_oldSa previously used SA which is NOW LOST -> clients which were connected to this item can react explicitly
 */
void MultiSend_c::reactOnMonitorListRemove( const ISOName_c& refc_isoName, uint8_t rui8_oldSa )
{
/*
  if ( getIsoMonitorInstance4Comm().existLocalIsoMemberISOName(refc_isoName) )
  { // lcoal ISOItem_c has lost SA
    uint32_t ui32_nr = rui8_oldSa;
    // only ISO msgs with own SA in PS (destination)
    uint32_t ui32_filter = ((static_cast<MASK_TYPE>(TP_CONN_MANAGE_PGN) | static_cast<MASK_TYPE>(ui32_nr)) << 8);
    if (getCanInstance4Comm().existFilter( *this, (0x1FFFF00UL), ui32_filter, Ident_c::ExtendedIdent))
    { // create FilterBox
      getCanInstance4Comm().deleteFilter( *this, (0x1FFFF00UL), ui32_filter, Ident_c::ExtendedIdent);
    }
    ui32_filter = ((static_cast<MASK_TYPE>(ETP_CONN_MANAGE_PGN) | static_cast<MASK_TYPE>(ui32_nr)) << 8);
    if (getCanInstance4Comm().existFilter( *this, (0x1FFFF00UL), ui32_filter, Ident_c::ExtendedIdent))
    { // create FilterBox
      getCanInstance4Comm().deleteFilter( *this, (0x1FFFF00UL), ui32_filter, Ident_c::ExtendedIdent);
    }
  }
  #ifdef DEBUG
  INTERNAL_DEBUG_DEVICE << "reactOnMonitorListRemove() handles LOSS of ISOItem_c for device with DevClass: " << int(refc_isoName.getDevClass())
      << ", Instance: " << int(refc_isoName.getDevClassInst()) << ", and manufacturer ID: " << int(refc_isoName.getConstName().manufCode())
      << " and PREVIOUSLY used SA: " << int(rui8_oldSa) << INTERNAL_DEBUG_DEVICE_NEWLINE << INTERNAL_DEBUG_DEVICE_NEWLINE
      << INTERNAL_DEBUG_DEVICE_ENDL;
  #endif
  for (std::list<SendStream_c>::iterator pc_iter=list_sendStream.begin(); pc_iter != list_sendStream.end();)
  {
// @todo MARTIN DO TODO !!!!
//    if (pc_iter->matchSa (rui8_oldSa) || pc_iter->matchDa (rui8_oldSa))
//    { // SendStream found in list, abort and erase!
//      pc_iter->abortSend();
//      pc_iter = list_sendStream.erase (pc_iter);
//    }
//    else
//    { // SendStream not yet found
//      pc_iter++;
//    }
//  }
*/
}



/**
  send a message -> set the ident and initiate sending to CAN
*/
void
MultiSend_c::SendStream_c::sendPacket()
{
  MultiSendPkg_c& refc_multiSendPkg = refc_multiSend.data();

  // set identifier
  refc_multiSendPkg.setIsoPri (6);
  refc_multiSendPkg.setISONameForSA (c_isoNameSender);
  refc_multiSendPkg.setIdentType (Ident_c::ExtendedIdent);
  refc_multiSendPkg.setLen (8);

  switch (en_msgType)
  {
    case NmeaFastPacket: // For fast packet, the PGN itself tells us that the message is a fast packet message.
                         // Since this is a non-destination specific PGN, we do NOT need to "setIsoPs(...)" !
                         #if defined (NMEA_2000_FAST_PACKET)
                         refc_multiSendPkg.setIsoPgn (ui32_pgn);
                         #endif
                         break;
    case IsoTP:          refc_multiSendPkg.setIsoPgn ((en_sendState == SendData) ?  TP_DATA_TRANSFER_PGN :  TP_CONN_MANAGE_PGN);
                         refc_multiSendPkg.setISONameForDA (c_isoNameReceiver);
                         break;
    case IsoETP:         refc_multiSendPkg.setIsoPgn ((en_sendState == SendData) ? ETP_DATA_TRANSFER_PGN : ETP_CONN_MANAGE_PGN);
                         refc_multiSendPkg.setISONameForDA (c_isoNameReceiver);
                         break;
    case IsoTPbroadcast: refc_multiSendPkg.setIsoPgn ((en_sendState == SendData) ?  TP_DATA_TRANSFER_PGN :  TP_CONN_MANAGE_PGN);
                         refc_multiSendPkg.setGlobalDA();
                         break;
  } // switch
  getCanInstance4Comm() << refc_multiSendPkg;
}



/**
  calculate the actual sequence number and
  calculate the amount of data bytes which must be placed in new CAN pkg
  @param ui8_nettoDataCnt amount of data which should be sent within this msg
*/
void
MultiSend_c::SendStream_c::prepareSendMsg(uint8_t &ui8_nettoDataCnt)
{
  ui8_nettoDataCnt = 7;
  ui8_burstPkgYetToSend--;
  ui8_sequenceNr++;

  if (ui32_dataBufferOffset + ui8_nettoDataCnt > ui32_dataSize)
  {
    ui8_nettoDataCnt = ui32_dataSize - ui32_dataBufferOffset;
  }
}



/** user function for explicit abort of any running matching stream. */
void
MultiSend_c::abortSend (const ISOName_c& rrefc_isoNameSender, const ISOName_c& rrefc_isoNameReceiver)
{
  SendStream_c* pc_sendStream = getSendStream (rrefc_isoNameSender, rrefc_isoNameReceiver);
  if (pc_sendStream) pc_sendStream->abortSend();
  /// let timeEvent do the erasing from the list.
  /// reason: if someone starts a new send directly after aborting the current,
  /// we can use the same place in the stl-list without remove/insert!
}

///  Used for Debugging Tasks in Scheduler_c
const char*
MultiSend_c::getTaskName() const
{ return "MultiSend_c()"; }


} // end namespace __IsoAgLib
