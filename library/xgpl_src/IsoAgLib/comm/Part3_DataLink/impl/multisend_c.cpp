/*
  multisend_c.cpp: transfer of multi message data

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include "multisend_c.h"

#include <IsoAgLib/driver/system/impl/system_c.h>
#include <IsoAgLib/scheduler/impl/scheduler_c.h>
#include <IsoAgLib/comm/impl/isobus_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isofiltermanager_c.h>

#if DEBUG_MULTISEND || DEBUG_HEAP_USEAGE
  #ifdef SYSTEM_PC
    #include <iostream>
  #else
    #include <supplementary_driver/driver/rs232/impl/rs232io_c.h>
  #endif
#endif

#include <IsoAgLib/util/iassert.h>

#if DEBUG_HEAP_USEAGE
  static uint16_t sui16_lastPrintedBufferCapacity = 0;
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

/** @todo SOON-178 figure that one out... new ISO says we can put out head2head messages! */
static const uint8_t scui8_isoCanPkgDelay = 4;



namespace __IsoAgLib {
#if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
  /** C-style function, to get access to the unique MultiSend_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  MultiSend_c& getMultiSendInstance( uint8_t aui8_instance ) { return MultiSend_c::instance( aui8_instance );};
#else
  /** C-style function, to get access to the unique MultiSend_c singleton instance */
  MultiSend_c& getMultiSendInstance( void ) { return MultiSend_c::instance();};
#endif



/** This is mostly used for debugging now... */
void SendUploadBase_c::set (uint8_t* apui8_buffer, uint32_t aui32_bufferSize)
{
  /// Use BUFFER - NOT MultiSendStreamer!
  vec_uploadBuffer.clear();
  vec_uploadBuffer.reserve (aui32_bufferSize);

  uint32_t i=0;
  for (; i < aui32_bufferSize; i++) {
    vec_uploadBuffer.push_back (*apui8_buffer);
    apui8_buffer++;
  }
  for (; i < 8; i++) {
    vec_uploadBuffer.push_back (0xFF);
  }

  #if DEBUG_HEAP_USEAGE
  if ( vec_uploadBuffer.capacity() != sui16_lastPrintedBufferCapacity )
  {
    sui16_lastPrintedBufferCapacity = vec_uploadBuffer.capacity();
    INTERNAL_DEBUG_DEVICE << "IsoTerminal_c Buffer-Capa: " << sui16_lastPrintedBufferCapacity << INTERNAL_DEBUG_DEVICE_ENDL;
  }
  #endif
}


/**
  Constructor used for "normal" 8-byte CAN-Pkgs!
*/
void SendUploadBase_c::set (uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, uint8_t byte5, uint8_t byte6, uint8_t byte7, uint8_t byte8)
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
}

/**
  Constructor used for "ChangeChildPosition" >> 9 <<-byte CAN-Pkgs!
  -- Parameter "timeOut" only there as else the signature would be the same compared to 8byte+timeOut constructor!
  -- simply always pass "DEF_TimeOut_ChangeChildPosition"
*/
void SendUploadBase_c::set (uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, uint8_t byte5, uint8_t byte6, uint8_t byte7, uint8_t byte8, uint8_t byte9)
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
}

const SendUploadBase_c& SendUploadBase_c::operator= (const SendUploadBase_c& r_source)
{
  vec_uploadBuffer = r_source.vec_uploadBuffer;
  return r_source;
}


SendUploadBase_c::SendUploadBase_c (const SendUploadBase_c& r_source)
  : vec_uploadBuffer (r_source.vec_uploadBuffer)
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
MultiSend_c::SendStream_c::init (const IsoName_c& acrc_isoNameSender, const IsoName_c& acrc_isoNameReceiver, const HUGE_MEM uint8_t* rhpb_data, uint32_t aui32_dataSize, sendSuccess_t& rpen_sendSuccessNotify, uint32_t aui32_pgn, IsoAgLib::iMultiSendStreamer_c* apc_mss, msgType_t ren_msgType)
{
  mui32_pgn = aui32_pgn;
  if ((mui32_pgn & 0x0FF00) < 0x0F000) mui32_pgn &= 0x3FF00;
  mc_isoNameSender = acrc_isoNameSender;     // copy the 8byte IsoName
  mc_isoNameReceiver = acrc_isoNameReceiver; // copy the 8byte IsoName
  mhpbui8_data = rhpb_data;
  mui32_dataSize = aui32_dataSize;   // initialise data for begin
  mpc_mss = apc_mss;
  men_msgType = ren_msgType;
  mpen_sendSuccessNotify = &rpen_sendSuccessNotify;

  mui32_dataBufferOffset = 0;
  mui8_packetsSentInThisBurst = 0;

  /// Do send out the first packet right now, right here!
  MultiSendPkg_c& rc_multiSendPkg = mrc_multiSend.data();

#if defined (ENABLE_MULTIPACKET_VARIANT_FAST_PACKET)
  if (men_msgType == NmeaFastPacket)
  { // send Fast Packet First Frame
    mui8_FpSequenceCounter = mrc_multiSend.allocFpSequenceCounter();
    rc_multiSendPkg.setUint8Data(0, static_cast<uint8_t>((mui8_FpSequenceCounter << 5) & 0xE0));
    rc_multiSendPkg.setUint8Data(1, static_cast<uint8_t>(getDataSize() & 0xFF));

    uint8_t ui8_nettoCnt = 6;
    if (ui8_nettoCnt > getDataSize()) ui8_nettoCnt = getDataSize();
    if (mhpbui8_data != NULL) {
      rc_multiSendPkg.setFastPacketDataPart(mhpbui8_data, 0, ui8_nettoCnt, 2);
    } else {
      mpc_mss->setDataNextFastPacketStreamPart (&rc_multiSendPkg, ui8_nettoCnt, 2);
    }
    mui32_dataBufferOffset += ui8_nettoCnt; // already sent out the first 6 bytes along with the first FP message.
    switchToState (SendData, 2); // go, send the rest!
    /** @todo SOON-178 maybe send out more packets right now if it's just about some.
              Why wait? If we wait, it's probably not a "FAST Packet" anymore...*/
    sendPacketFp();
  }
  else
#endif
  { // NOT NmeaFastPacket ==> some ISO protocol
    rc_multiSendPkg.setUint16Data(1, getDataSize() & 0xFFFF);                             // Byte 2+3
    rc_multiSendPkg.setUint8Data (5, static_cast<uint8_t>(mui32_pgn & 0xFF));              // Byte 6
    rc_multiSendPkg.setUint16Data(6, static_cast<uint16_t>((mui32_pgn >> 8) & 0xFFFFU));   // Byte 7+8
    if (men_msgType == IsoETP)
    {
      rc_multiSendPkg.setUint8Data (0, static_cast<uint8_t>(scui8_eCM_RTS));              // Byte 1
      rc_multiSendPkg.setUint16Data(3, getDataSize() >> 16);                              // Byte 4+5
      switchToState (AwaitCts, 1250);
    }
    else if (men_msgType == IsoTP)
    {
      rc_multiSendPkg.setUint8Data (0, static_cast<uint8_t>(scui8_CM_RTS));               // Byte 1
      rc_multiSendPkg.setUint8Data (3, static_cast<uint8_t>((mui32_dataSize + 6) / 7));    // Byte 4
      rc_multiSendPkg.setUint8Data (4, static_cast<uint8_t>(0xFF));                       // Byte 5
      switchToState (AwaitCts, 1250);
    }
    else if (men_msgType == IsoTPbroadcast)
    {
      rc_multiSendPkg.setUint8Data (0, static_cast<uint8_t>(scui8_CM_BAM));               // Byte 1
      rc_multiSendPkg.setUint8Data (3, static_cast<uint8_t>((mui32_dataSize + 6) / 7));    // Byte 4
      rc_multiSendPkg.setUint8Data (4, static_cast<uint8_t>(0xFF));                       // Byte 5
      mui8_packetsLeftToSendInBurst = (mui32_dataSize + 6) / 7;
      switchToState (AwaitCts, 50); // on broadcast, we'll have to interspace with 50ms (minimum!)
    }
#if 0
/// Disabled as long as getILibErrInstance().registerError() makes problems on the ESX
    else getILibErrInstance().registerError( iLibErr_c::Precondition, iLibErr_c::MultiSend );
#endif

    mui32_packetNrRequestedInLastCts = 1;
    if (mpc_mss) mpc_mss->resetDataNextStreamPart();
    sendPacketIso (false); // ISO never starts with data. BAM/RTS
  }
  *mpen_sendSuccessNotify = Running;
  if (men_msgType == IsoTPbroadcast)
  { // now we can switch the state to SendData
    switchToState (SendData, 50); // on broadcast, we'll have to interspace with 50ms (minimum!)
  }
}


/** initialisation for MultiSend_c */
void
MultiSend_c::init(void)
{
  if (checkAlreadyClosed())
  {
    clearAlreadyClosed();
    mc_data.setSingletonKey( getSingletonVecKey() );

    getSchedulerInstance().registerClient( this );
    __IsoAgLib::getIsoMonitorInstance4Comm().registerControlFunctionStateHandler( mt_handler );

    #if defined(ENABLE_MULTIPACKET_VARIANT_FAST_PACKET)
    mui8_nextFpSequenceCounter = 0;
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

    __IsoAgLib::getIsoMonitorInstance4Comm().deregisterControlFunctionStateHandler( mt_handler );
    getSchedulerInstance().unregisterClient( this );

    /// For right now, we do gracefully kill all interrupted stream,
    /// but normally the modules should abort thier own sending when they
    /// get stopped... @todo Check that some day, for now it's okay though.
    mlist_sendStream.clear();

    // if not empty, some modules have not properly closed down its send-streams!
    isoaglib_assert (mlist_sendStream.empty());
  }
}


/**
  @return an "in-progress" stream or NULL if none active for this sa/da-key
*/
MultiSend_c::SendStream_c*
MultiSend_c::getSendStream(const IsoName_c& acrc_isoNameSender, const IsoName_c& acrc_isoNameReceiver)
{
  for (STL_NAMESPACE::list<SendStream_c>::iterator pc_iter=mlist_sendStream.begin(); pc_iter != mlist_sendStream.end(); pc_iter++)
  {
    if (pc_iter->matchSaDa(acrc_isoNameSender, acrc_isoNameReceiver))
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
MultiSend_c::addSendStream(const IsoName_c& acrc_isoNameSender, const IsoName_c& acrc_isoNameReceiver)
{
  SendStream_c* const pc_foundStream = getSendStream(acrc_isoNameSender, acrc_isoNameReceiver);
  if (pc_foundStream)
  {
    if (!pc_foundStream->isFinished())
    {
      return NULL; // can't start a sendStream, one already active for this one..
    }
    else // use this finished one because it would be deleted anyway...
    {
      #if DEBUG_MULTISEND
      INTERNAL_DEBUG_DEVICE << "Using Stream from list which is already finished but not yet kicked from MultiSend_c::timeEvent()."<< INTERNAL_DEBUG_DEVICE_ENDL;
      #endif
      return pc_foundStream;
    }
  }

  if (mlist_sendStream.empty()) {
    mlist_sendStream.push_back (SendStream_c(*this SINGLETON_VEC_KEY_WITH_COMMA ));
  } else {
    mlist_sendStream.insert (mlist_sendStream.end(), mlist_sendStream.back()); // insert a copy of the first element (for performance reasons)
  }
  return &mlist_sendStream.back();
}



bool
MultiSend_c::sendIntern (const IsoName_c& acrc_isoNameSender, const IsoName_c& acrc_isoNameReceiver, const HUGE_MEM uint8_t* rhpb_data, uint32_t aui32_dataSize, sendSuccess_t& rpen_sendSuccessNotify, int32_t ai32_pgn, IsoAgLib::iMultiSendStreamer_c* apc_mss, msgType_t ren_msgType)
{
  isoaglib_assert( aui32_dataSize >= endSinglePacketSize );
  isoaglib_assert( (ren_msgType != NmeaFastPacket) || (aui32_dataSize < endNmeaFastPacketSize) );
  isoaglib_assert( (ren_msgType != IsoTPbroadcast) || (aui32_dataSize < beginEtpPacketSize) );
  isoaglib_assert( (ren_msgType != IsoTP) || (aui32_dataSize < endTpPacketSize) );
  isoaglib_assert( (ren_msgType != IsoETP) || (aui32_dataSize >= beginEtpPacketSize) );
  // end of preconditions.

  /// first check if new transfer can be started
  /// - is the sender correct?
  bool const cb_existSender = getIsoMonitorInstance4Comm().existIsoMemberISOName(acrc_isoNameSender);
  if (!cb_existSender)
    return false;

  IsoName_c const *p_isoNameReceiver = &acrc_isoNameReceiver;
  // - is the receiver correct?:
  switch (ren_msgType) {
  case IsoTPbroadcast:
  case NmeaFastPacket:
    // no destination-isoname checks needed for broadcast messages!
    // Force destination to be "IsoNameUnspecified"
    p_isoNameReceiver = &IsoName_c::IsoNameUnspecified();
    break;
  default:
    // destination specific - so the receiver must be registered!
    bool const b_existReceiver = getIsoMonitorInstance4Comm().existIsoMemberISOName(*p_isoNameReceiver);
    if (!b_existReceiver)
      return false;
  }
  /// - check if there's already a SA/DA pair active (in this case NULL is returned!)
  /// - if not NULL is returned, it points to the newly generated stream.
  SendStream_c * const cpc_newSendStream = addSendStream(acrc_isoNameSender, *p_isoNameReceiver);
  if (!cpc_newSendStream)
    return false;

  cpc_newSendStream->init(acrc_isoNameSender, acrc_isoNameReceiver, rhpb_data, aui32_dataSize, rpen_sendSuccessNotify, ai32_pgn, apc_mss, ren_msgType);
  // let this SendStream get sorted in now...
  calcAndSetNextTriggerTime();
  return true;
}


/**
  @return true: stream finished, it'll be erased then!
*/
bool
MultiSend_c::SendStream_c::timeEvent (uint8_t aui8_pkgCnt)
{
  MultiSendPkg_c& rc_multiSendPkg = mrc_multiSend.data();
  uint8_t ui8_nettoDataCnt;
  uint8_t ui8_pkgInd;

  switch (men_sendState)
  {
    case AwaitCts:
    case AwaitEndofmsgack:
    case SendPauseTillCts:
      switch (men_msgType)
      {
        case IsoTPbroadcast: // we shouldn't reach this state!
        case NmeaFastPacket: // we shouldn't reach this state!
          break;
        case IsoTP: // break left out intentionally
        case IsoETP:
          abortSend();
          return true; // FINISHED SendStream, remove it from list please!
      }
      break;

    case SendData:
    {
#if defined (ENABLE_MULTIPACKET_VARIANT_FAST_PACKET)
      if (men_msgType == NmeaFastPacket)
      {
        uint8_t ui8_freeCnt = getIsoBusInstance4Comm().sendCanFreecnt();

        // send only as much pkg as fits in send buffer (with spare of 2 for other use)
        if (ui8_freeCnt < 2)
           aui8_pkgCnt = 0;
        else if (aui8_pkgCnt > (ui8_freeCnt - 2))
           aui8_pkgCnt = (ui8_freeCnt - 2);

        if (aui8_pkgCnt == 0)        {
            #if DEBUG_MULTISEND
            INTERNAL_DEBUG_DEVICE << "MultiSend_c::timeEvent --- pkgCnt == 0;" << INTERNAL_DEBUG_DEVICE_ENDL;
            #endif
        }
        for (ui8_pkgInd = 0; ui8_pkgInd < aui8_pkgCnt; ui8_pkgInd++)
        {
          prepareSendMsg (ui8_nettoDataCnt);
          rc_multiSendPkg.setUint8Data(0, static_cast<uint8_t>(((mui8_FpSequenceCounter << 5) |(mui8_packetsSentInThisBurst & 0x1F)) ) );

          if (mhpbui8_data != NULL) {
            rc_multiSendPkg.setFastPacketDataPart(mhpbui8_data, mui32_dataBufferOffset, ui8_nettoDataCnt, 1);
          } else {
            mpc_mss->setDataNextFastPacketStreamPart (&rc_multiSendPkg, ui8_nettoDataCnt, 1);
          }
          sendPacketFp();
          mui32_dataBufferOffset += ui8_nettoDataCnt;
          // break if this message part is finished
          if (isCompleteData())
          {
            *mpen_sendSuccessNotify = SendSuccess;
            return true; // FINISHED SendStream, remove it from list please!
          }
        } // for
        /** @todo SOON-178 how fast must fast-packet be? send all right now? set retriggerIn to (0) or (1) ?? */
        retriggerIn (2); // if we couldn't finish now, retrigger right soon! we're FAST-PACKET!
      }
      else
#endif
      { // NOT NmeaFastPacket - some ISO protocol!
        if (men_msgType == IsoTPbroadcast)
        { // IsoTPbroadcast forces 50ms between all packets!!
          aui8_pkgCnt = 1; // only send 1 packet at a time, then wait 50ms.
        }
        else
        { // IsoTP || IsoETP
          const uint8_t cui8_freeCnt = getIsoBusInstance4Comm().sendCanFreecnt();
          if (aui8_pkgCnt == 0) aui8_pkgCnt = 1;
          // send only as much pkg as fits in send buffer (with spare of 2 for other use)
          if (cui8_freeCnt < 2)
            aui8_pkgCnt = 0;
          else if (aui8_pkgCnt > (cui8_freeCnt - 2))
            aui8_pkgCnt = (cui8_freeCnt - 2);
        }

        for (ui8_pkgInd = 0; ui8_pkgInd < aui8_pkgCnt; ui8_pkgInd++)
        {
          prepareSendMsg (ui8_nettoDataCnt);
          const uint8_t cui8_pkgNumberToSend = mui8_packetsSentInThisBurst + ((men_msgType == IsoETP) ? 0 : uint8_t(mui32_packetNrRequestedInLastCts-1));
          rc_multiSendPkg.setUint8Data (0, cui8_pkgNumberToSend);
          if (mhpbui8_data != NULL) {
            rc_multiSendPkg.setDataPart (mhpbui8_data, mui32_dataBufferOffset, ui8_nettoDataCnt);
          } else {
            mpc_mss->setDataNextStreamPart (&rc_multiSendPkg, ui8_nettoDataCnt);
          }
          sendPacketIso (true);
          mui32_dataBufferOffset += ui8_nettoDataCnt;
          // break if this message part is finished
          if (isCompleteData())
          {
            if (men_msgType == IsoTPbroadcast)
            { // ISO Broadcast
              *mpen_sendSuccessNotify = SendSuccess;
              return true; // FINISHED SendStream, remove it from list please!
            }
            else
            { // ISO Target
              #if DEBUG_MULTISEND
              INTERNAL_DEBUG_DEVICE << "MultiSend_c::timeEvent --- after complete Sending now awaiting EOMACK!" << INTERNAL_DEBUG_DEVICE_ENDL;
              #endif
              switchToState (AwaitEndofmsgack, 1250);
              return false; // stream not yet finished!
            }
          }
          // not completely finished, but maybe this burst has finished?
          if (isCompleteBurst())
          { // wait for CTS for next part of transfer
            switchToState (AwaitCts, 1250);
            #if DEBUG_MULTISEND
            INTERNAL_DEBUG_DEVICE << "MultiSend_c::timeEvent --- after Sending now awaiting CTS!" << INTERNAL_DEBUG_DEVICE_ENDL;
            #endif
            return false; // stream not yet finished!
          }
          // nothing special, keep on sending...
        } // for - sent all pkgs for now, yet some more to go, send them in a later timeEvent()

        if (men_msgType == IsoTPbroadcast)
        { // IsoTPbroadcast forces 50ms between all packets!!
          retriggerIn (50); // same state - but the time stamp gets updated, so we'll wait on for 50ms...
        }
        else
        { // IsoTP || IsoETP can send on "immediately"
          retriggerIn (3);
        }
      } // Not Nmea- some ISO protocol
    } break; // case SendData
  }
  return false; // stream not yet finished!
}

/**
  HIDDEN constructor for a MultiSend_c object instance
  NEVER instantiate a variable of type MultiSend_c within application
  only access MultiSend_c via getMultiSendInstance() or
  getMultiSendInstance (protocolInstanceNr) in case more than one
  ISO11783 BUS is used for IsoAgLib
  */
MultiSend_c::MultiSend_c() :
  mt_handler(*this),
  mt_customer(*this)
{}


/**
  perform periodical actions
  @return true -> all planned activities performed in allowed time
*/
bool
MultiSend_c::timeEvent()
{
  if (mlist_sendStream.empty())
  { // nothing to do if no transfer is Running
    setTimePeriod (5000); // actually we could use "infinite here"
    return true;
  }

  /** @todo SOON-178 Check how we want to calculate the max nr. of packets to send
            ==> Best would be to know when the next comes.
            clip that value as we may expect incoming can-pkgs, too - so be a little polite!
  */
  const int32_t ci32_time = Scheduler_Task_c::getLastRetriggerTime();
  // store time of last call, to get time interval between execution
  static int32_t si32_lastCall = 0;
  // only send max 1 package for first call, when execution period can't be derived
  uint8_t ui8_pkgCnt = (si32_lastCall > 0 ) ? ((ci32_time - si32_lastCall)/scui8_isoCanPkgDelay) : 1;
  // update last call time
  si32_lastCall = ci32_time;

  const uint8_t cui8_pkgCntForEach = ui8_pkgCnt / mlist_sendStream.size(); // in case it gets 0 after division, it is set to 1 inside of SendStream's timeEvent().

  int32_t i32_nextRetriggerNeeded = -1; // default to: "no retriggering needed"

  // Call each SendStream_c's timeEvent()
  for (STL_NAMESPACE::list<SendStream_c>::iterator pc_iter=mlist_sendStream.begin(); pc_iter != mlist_sendStream.end();)
  { // only call a SendStream when its time has come!
    if ( pc_iter->isFinished () ||
        (pc_iter->timeHasCome() && (pc_iter->timeEvent (cui8_pkgCntForEach))) )
    { // SendStream finished
      pc_iter = mlist_sendStream.erase (pc_iter);
      #if DEBUG_MULTISEND
      INTERNAL_DEBUG_DEVICE << "Kicked SendStream because it finished (abort or success)!" << INTERNAL_DEBUG_DEVICE_ENDL;
      #endif
    }
    else
    { // SendStream not yet finished
      const int32_t ci32_nextTriggerTime = pc_iter->getNextTriggerTime();
      // needs to be triggered at the following time
      if ((i32_nextRetriggerNeeded == -1) || (ci32_nextTriggerTime < i32_nextRetriggerNeeded))
      { // no trigger yet set or this SendStream needs to come earlier!
        i32_nextRetriggerNeeded = ci32_nextTriggerTime;
      }
      pc_iter++;
    }
  }

  // ALWAYS calculate when we want to be triggered again!
  uint16_t ui16_newTimePeriod = 5000; // default: no SendStream running, idle around with 10 secs.. (actually "unlimited" would be correct - we can sleep!

  if (i32_nextRetriggerNeeded > -1) // "!= -1"
  { // HARD-timing, we need to come to action then!
    int32_t i32_delta = i32_nextRetriggerNeeded-System_c::getTime();
    if (i32_delta < 0) i32_delta = 0;
    ui16_newTimePeriod = i32_delta;
  }
  setTimePeriod (ui16_newTimePeriod);

  return true;
};


//! Function set ui16_earlierInterval and
//! ui16_laterInterval that will be used by
//! getTimeToNextTrigger(retriggerType_t)
//! can be overloaded by Childclass for special condition
void
MultiSend_c::updateEarlierAndLatestInterval()
{ /** @todo OPTIMIZE-178 improve with a flag for HARD/SOFT timing, but it's okay for right now... */
  if (getTimePeriod() <= 1250)
  { // use HARD-Timing
    mui16_earlierInterval = 0;
    mui16_latestInterval  = getTimePeriod()/2; //50; // be polite and let other tasks do their job, we can wait a little bit
    // ==> this is better than always interrupting important big tasks as WE can wait and slicing is NOT nice to handle for the big task.
    if (mui16_latestInterval > 100) mui16_latestInterval = 100;
  }
  else
  { // use SOFT-Timing (using jitter for earlier/after)
    mui16_earlierInterval = ( (getTimePeriod() * 1) / 4);
    mui16_latestInterval =     getTimePeriod();
  }
}



bool
MultiSend_c::SendStream_c::processMsg()
{
  /***/ MultiSendPkg_c&  rc_multiSendPkg = mrc_multiSend.data();
  const MultiSendPkg_c& refcc_multiSendPkg = mrc_multiSend.constData();

  if (isFinished() || (men_msgType == NmeaFastPacket) || (men_msgType == IsoTPbroadcast))
  { // nothing to come in for us when we're broadcasting or already done (succeeded or aborted)
    return false; // anyway, return with false here for "safety"
  }

  switch (rc_multiSendPkg.getUint8Data (0))
  {
    case scui8_eCM_CTS:
    case scui8_CM_CTS:
      if (   (men_sendState == AwaitCts)
          || (men_sendState == SendPauseTillCts)
          || (men_sendState == AwaitEndofmsgack)
          || (men_sendState == SendData)
         )
      { // awaited (or resent-) CTS received -- only possible in ISO protocol case
        mui8_packetsLeftToSendInBurst = refcc_multiSendPkg.getUint8Data (1);

        const uint32_t cui32_packetNrRequested =
          uint32_t(refcc_multiSendPkg.getUint8Data(2)) +
          ((men_msgType == IsoTP) ? 0 : (uint32_t(refcc_multiSendPkg.getUint16Data(3)) << 8));

        mui32_dataBufferOffset = (cui32_packetNrRequested - 1UL) * 7L; // take sequence nr with -1 because prepareSendMsg increment by 1 before first send

        if (mui8_packetsLeftToSendInBurst == 0)
        { /// Send pause commanded from receiver (CTS with 0 packets requested)
          switchToState (SendPauseTillCts, 1050);
        }
        else
        { /// Really some packets requested
          // Can we deliver all the requested packets?
          if ((mui32_dataBufferOffset + (uint32_t(mui8_packetsLeftToSendInBurst-1)*7)) >= mui32_dataSize)
          { // request will be out of buffer, either already at the start or later during sending
            abortSend();
            break;
          }

          if (mpc_mss != NULL)
          { // For the MSS, it's only allowed to go back to an arbitrary point in the last burst
            // Is the receiver asking for the next available packet (that'd be next if we were continuing to stream out)?
            if (cui32_packetNrRequested == (mui32_packetNrRequestedInLastCts + mui8_packetsSentInThisBurst))
            { // then nothing to do, we're at this position already, we can just send on...
              // --> This should be the normal case!
            }
            else
            { // some retransmission is requested
              // (needn't be the beginning, but needs to be in the last burst!)
              if ( (cui32_packetNrRequested >= mui32_packetNrRequestedInLastCts)
                && (cui32_packetNrRequested < (mui32_packetNrRequestedInLastCts + mui8_packetsSentInThisBurst)) )
              { // go back to requested point in last burst.
                // first, go back to start of last burst
                mpc_mss->restoreDataNextStreamPart();
                // second, advance to the requested position
                while (mui32_packetNrRequestedInLastCts < cui32_packetNrRequested)
                { // step one packet forward.
                  const uint8_t cui8_nettoDataCnt = 7; // here, the netto data count is always 7
                  // let the packet be streamed to rc_multiSendPkg, although we're just ignoring them, just need to fast forward...
                  mpc_mss->setDataNextStreamPart(&rc_multiSendPkg, cui8_nettoDataCnt);
                  ++mui32_packetNrRequestedInLastCts; // we can alter this variable. It's being set below anyway..
                }
              }
              else
              { // requesting some other packets (earlier than the last burst or after the current position (jump forward))
                abortSend();
                break; // break out of big switch to reach "return true" for deletion of this stream.
              }
            }
            mpc_mss->saveDataNextStreamPart();
          }
          // else: it's okay if we have the complete buffer, the sender can get what he wants. his problem.

          // send out Extended Connection Mode Data Packet Offset
          if (men_msgType == IsoETP)
          {
            rc_multiSendPkg.setUint8Data (0, static_cast<uint8_t>(scui8_eCM_DPO));
            rc_multiSendPkg.setUint8Data (1, static_cast<uint8_t>(mui8_packetsLeftToSendInBurst));
            rc_multiSendPkg.setUint16Data(2, static_cast<uint16_t>((cui32_packetNrRequested-1) & 0xFFFF));
            rc_multiSendPkg.setUint8Data (4, static_cast<uint8_t>((cui32_packetNrRequested-1) >> 16));
            // change order of setUint8Data(), setUint16Data() so that
            // setUint16Data() with equal position, so that a faster assignment is enabled
            rc_multiSendPkg.setUint8Data (5, static_cast<uint8_t>(mui32_pgn & 0xFF));
            rc_multiSendPkg.setUint16Data(6, static_cast<uint16_t>(mui32_pgn >> 8));
            sendPacketIso (false); // DPO is not Data, we need the connection management pgn here...
          }
          // else if (men_msgType == IsoTP) //-> nothing special to do here...

          mui32_packetNrRequestedInLastCts = cui32_packetNrRequested;
          mui8_packetsSentInThisBurst = 0;

          // now receiver wants to receive new data
          #if DEBUG_MULTISEND
          INTERNAL_DEBUG_DEVICE << "Start To Send Next Data Block" << INTERNAL_DEBUG_DEVICE_ENDL;
          #endif
          switchToState (SendData, 5); // we'll have to trigger that we want to
        } // end request to send
      } // awaited (or resent-) CTS received
      // else: nothing to do on "SendRts"
      break;

    case scui8_CM_EndofMsgACK:
    case scui8_eCM_EndofMsgACK:
      if (men_sendState == AwaitEndofmsgack)
      {
        #if DEBUG_MULTISEND
        INTERNAL_DEBUG_DEVICE << "MultiSend_c::processMsg --- EOMACK received!" << INTERNAL_DEBUG_DEVICE_ENDL;
        #endif
        // Notify sender that it finished!
        *mpen_sendSuccessNotify = SendSuccess; // will be kicked out after next timeEvent!
        // so trigger timeEvent so it gets actually deleted - do it asap so that there's
        // no confusion about the finished stream and a possible new stream!
        retriggerIn (0); // changed from 1500 to 0 by JVB 20090916
        /** @todo SOON-178 Should we remove the finished send-stream here now immediately
            even though we're NOT iterating through the list now? */
        break;
      }
      // else: handle the same as ConnAbort (EoMACK was sent unsolicited)
#if 0
/// @todo SOON-178 reactivate as it should work now with the new iLibeErr. Please test on ESX as it made problems there.
      else
      {  // not awaiting end of message ack, no action taken for this error-case in normal operation.
        getILibErrInstance().registerError( iLibErr_c::MultiSendWarn, iLibErr_c::MultiSend );
      }
#endif
      // break left out intentionally (see "// else:" right above
    case scui8_CM_ConnAbort:
      #if DEBUG_MULTISEND
      INTERNAL_DEBUG_DEVICE << "MultiSend_c::processMsg --- ConnAbort received!" << INTERNAL_DEBUG_DEVICE_ENDL;
      #endif
      *mpen_sendSuccessNotify = SendAborted; // will be kicked out after next timeEvent!
      retriggerIn (1500);
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
  @return true -> message was processed; else the received CAN message will be served to other matching CanCustomer_c
*/
bool
MultiSend_c::processMsg()
{
  SendStream_c* pc_sendStreamFound = getSendStream (data().getISONameForDA(), data().getISONameForSA()); // sa/da swapped, of course ;-) !
  if (pc_sendStreamFound)
  { // found a matching SendStream, so call its processMsg()
    const bool cb_success = pc_sendStreamFound->processMsg();
    calcAndSetNextTriggerTime();
    return cb_success;
  }
  else
  { // no matching SendStream found
    // keep next trigger time
    return false;
  }
}

bool
MultiSend_c::sendIsoBroadcastOrSinglePacket (const IsoName_c& acrc_isoNameSender, const HUGE_MEM uint8_t* rhpb_data, uint16_t aui16_dataSize, int32_t ai32_pgn, sendSuccess_t& rpen_sendSuccessNotify)
{
  if (aui16_dataSize < beginTpPacketSize)
  {
    MultiSendPkg_c& rc_multiSendPkg = data();
    rc_multiSendPkg.setIsoPri (6);
    rc_multiSendPkg.setISONameForSA (acrc_isoNameSender);
    rc_multiSendPkg.setIdentType (Ident_c::ExtendedIdent);
    rc_multiSendPkg.setLen (uint8_t (aui16_dataSize));
    rc_multiSendPkg.setIsoPgn(ai32_pgn);
    for (unsigned ui = 0 ; ui < aui16_dataSize; ++ui)
      rc_multiSendPkg.setUint8Data (ui, rhpb_data[ui]);

    getIsoBusInstance4Comm() << rc_multiSendPkg;
    rpen_sendSuccessNotify = SendSuccess;
    return true;
  }
  else
  {
    return sendIntern(acrc_isoNameSender, IsoName_c::IsoNameUnspecified(), rhpb_data, aui16_dataSize, rpen_sendSuccessNotify, ai32_pgn, NULL /* NOT "yet" supported */, IsoTPbroadcast);
  }
}

/** this function should NOT be called from INSIDE of timeEvent !
    ==> Only call it from OUTSIDE functions like init(), processMsg(), addSendStream, etc.
  */
void
MultiSend_c::calcAndSetNextTriggerTime()
{
  int32_t i32_nextRetriggerNeeded = -1;
  for (STL_NAMESPACE::list<SendStream_c>::iterator pc_iter=mlist_sendStream.begin(); pc_iter != mlist_sendStream.end();)
  {
    const int32_t ci32_nextTriggerTime = pc_iter->getNextTriggerTime();

    if ((i32_nextRetriggerNeeded == -1) || (ci32_nextTriggerTime < i32_nextRetriggerNeeded))
    { // no trigger yet set or this SendStream needs to come earlier!
      i32_nextRetriggerNeeded = ci32_nextTriggerTime;
    }
    pc_iter++;
  }

  if (i32_nextRetriggerNeeded == -1)
  { // no SendStreams needs to come to action, so idle around
    i32_nextRetriggerNeeded = System_c::getTime() + 5000;
  }
  getSchedulerInstance().changeRetriggerTimeAndResort (this, i32_nextRetriggerNeeded); // no need to change the period, as we don't use it - we always calculate the next trigger time!
}


/**
  abort the multipacket send stream
  (important if original target isn't active any more)
  IMPORTANT: After calling abortSend() please exit timeEvent() with true, so the SendStream gets deleted!!
*/
void
MultiSend_c::SendStream_c::abortSend()
{
  MultiSendPkg_c& rc_multiSendPkg = mrc_multiSend.data();

  rc_multiSendPkg.setUint8Data (0, static_cast<uint8_t>(scui8_CM_ConnAbort));
  rc_multiSendPkg.setUint32Data(1, uint32_t(0xFFFFFFFFUL));
  rc_multiSendPkg.setUint8Data (5, static_cast<uint8_t>(mui32_pgn & 0xFF));
  rc_multiSendPkg.setUint16Data(6, static_cast<uint16_t>(mui32_pgn >> 8));

  sendPacketIso (false); // ConnAbort is not Data, we need the connection management pgn here...
  *mpen_sendSuccessNotify = SendAborted; // will cause isFinished() to report true!
}

/** this function is called by IsoMonitor_c on addition, state-change and removal of an IsoItem.
 * @param at_action enumeration indicating what happened to this IsoItem. @see IsoItemModification_en / IsoItemModification_t
 * @param acrc_isoItem reference to the (const) IsoItem which is changed (by existance or state)
 */
void
MultiSend_c::reactOnIsoItemModification (ControlFunctionStateHandler_c::IsoItemModification_t at_action, IsoItem_c const& acrc_isoItem)
{
  switch (at_action)
  {
    case ControlFunctionStateHandler_c::AddToMonitorList:
      if (acrc_isoItem.itemState (IState_c::Local))
      { // local IsoItem_c has finished adr claim
        getIsoFilterManagerInstance4Comm().insertIsoFilter (IsoFilter_s (mt_customer, (0x3FFFF00UL),  (TP_CONN_MANAGE_PGN << 8), &acrc_isoItem.isoName(), NULL, 8), false);
        getIsoFilterManagerInstance4Comm().insertIsoFilter (IsoFilter_s (mt_customer, (0x3FFFF00UL), (ETP_CONN_MANAGE_PGN << 8), &acrc_isoItem.isoName(), NULL, 8), true);
      }
      break;

    case ControlFunctionStateHandler_c::RemoveFromMonitorList:
      if (acrc_isoItem.itemState (IState_c::Local))
      { // local IsoItem_c has gone (i.e. IdentItem has gone, too.
        /// @todo SOON-178 activate the reconfiguration when the second parameter in removeIsoFilter is there finally...
        getIsoFilterManagerInstance4Comm().removeIsoFilter (IsoFilter_s (mt_customer, (0x3FFFF00UL),  (TP_CONN_MANAGE_PGN << 8), &acrc_isoItem.isoName(), NULL, 8));
        getIsoFilterManagerInstance4Comm().removeIsoFilter (IsoFilter_s (mt_customer, (0x3FFFF00UL), (ETP_CONN_MANAGE_PGN << 8), &acrc_isoItem.isoName(), NULL, 8));
        /// @todo SOON-178 Maybe clean up some streams and clients?
        /// Shouldn't appear normally anyway, so don't care for right now...
      }
      break;

    default:
      break;
  }
}


void
MultiSend_c::SendStream_c::sendPacketIso (bool ab_data)
{
  MultiSendPkg_c& rc_multiSendPkg = mrc_multiSend.data();

  // set identifier
  rc_multiSendPkg.setIsoPri (6);
  rc_multiSendPkg.setISONameForSA (mc_isoNameSender);
  rc_multiSendPkg.setIdentType (Ident_c::ExtendedIdent);
  rc_multiSendPkg.setLen (8);

  switch (men_msgType)
  {
    case IsoTP:          rc_multiSendPkg.setIsoPgn (ab_data ?  TP_DATA_TRANSFER_PGN :  TP_CONN_MANAGE_PGN);
                         rc_multiSendPkg.setISONameForDA (mc_isoNameReceiver);
                         break;
    case IsoETP:         rc_multiSendPkg.setIsoPgn (ab_data ? ETP_DATA_TRANSFER_PGN : ETP_CONN_MANAGE_PGN);
                         rc_multiSendPkg.setISONameForDA (mc_isoNameReceiver);
                         break;
    case IsoTPbroadcast: rc_multiSendPkg.setIsoPgn (ab_data ?  TP_DATA_TRANSFER_PGN :  TP_CONN_MANAGE_PGN);
                         rc_multiSendPkg.setGlobalDA();
                         break;

    case NmeaFastPacket: // all other cases are wrong here
    default:
                         isoaglib_assert (men_msgType != NmeaFastPacket);
                         return;
  } // switch
  getIsoBusInstance4Comm() << rc_multiSendPkg;
}


#if defined (ENABLE_MULTIPACKET_VARIANT_FAST_PACKET)
void
MultiSend_c::SendStream_c::sendPacketFp()
{
  // no-one should call this when the Stream is not set to FP-mode.
  isoaglib_assert (men_msgType == NmeaFastPacket);

  MultiSendPkg_c& rc_multiSendPkg = mrc_multiSend.data();

  // For fast packet, the PGN itself tells us that the message is a fast packet message.
  /// Since we currently only need/support broadcast FP-PGNs, we do NOT need to "setIsoPs(...)" !
  rc_multiSendPkg.setIsoPgn (mui32_pgn);
  /// @todo ON REQUEST-690 The FP-Messages can have different priorities,
  /// so the calling code must be able to specify the priority.
  /// Now setting the Priority to 3 as it's the default priority
  /// for the currently only NMEA message being sent out, so regard
  /// this is hard-coded workaround for the current situation.
  /// (This PGN is NMEA_GPS_POSITION_DATA_PGN)
  rc_multiSendPkg.setIsoPri (3);
  rc_multiSendPkg.setISONameForSA (mc_isoNameSender);
  rc_multiSendPkg.setIdentType (Ident_c::ExtendedIdent);
  rc_multiSendPkg.setLen (8);

  getIsoBusInstance4Comm() << rc_multiSendPkg;
}
#endif


/**
  calculate the actual sequence number and
  calculate the amount of data bytes which must be placed in new CAN pkg
  @param ui8_nettoDataCnt amount of data which should be sent within this msg
*/
void
MultiSend_c::SendStream_c::prepareSendMsg (uint8_t &ui8_nettoDataCnt)
{
  ui8_nettoDataCnt = 7;
  --mui8_packetsLeftToSendInBurst;
  ++mui8_packetsSentInThisBurst;

  if (mui32_dataBufferOffset + ui8_nettoDataCnt > mui32_dataSize)
  {
    ui8_nettoDataCnt = mui32_dataSize - mui32_dataBufferOffset;
  }
}



/** user function for explicit abort of any running matching stream. */
void
MultiSend_c::abortSend (const IsoName_c& acrc_isoNameSender, const IsoName_c& acrc_isoNameReceiver)
{
  SendStream_c* pc_sendStream = getSendStream (acrc_isoNameSender, acrc_isoNameReceiver);
  if (pc_sendStream)
  {
    if (!pc_sendStream->isFinished()) // check added by JVB 20090916
      pc_sendStream->abortSend();
  }
  /// let timeEvent do the erasing from the list.
  /// reason: if someone starts a new send directly after aborting the current,
  /// we can use the same place in the stl-list without remove/insert!
}


#if DEBUG_SCHEDULER
const char*
MultiSend_c::getTaskName() const
{ return "MultiSend_c()"; }
#endif


} // end namespace __IsoAgLib
