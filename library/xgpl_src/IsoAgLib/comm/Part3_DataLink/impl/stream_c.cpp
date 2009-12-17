/*
  stream_c.cpp

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include "stream_c.h"
#include "multireceive_c.h"

// IsoAgLib
#include <IsoAgLib/hal/system.h>
#include <algorithm>
#ifdef DEBUG
  #ifdef SYSTEM_PC
    #include <iostream>
  #else
    #include <supplementary_driver/driver/rs232/impl/rs232io_c.h>
  #endif
#endif

// helper macros
#ifndef MACRO_minimum
#define MACRO_minimum(a,b) CNAMESPACE::min(a,b)   // (a < b) ? a : b
#endif


// Begin Namespace __IsoAgLib
namespace __IsoAgLib {



Stream_c::Stream_c (StreamType_t at_streamType, const IsoAgLib::ReceiveStreamIdentifier_c& ac_rsi, uint32_t aui32_msgSize SINGLETON_VEC_KEY_PARAMETER_DEF_WITH_COMMA , bool ab_skipCtsAwait)
  : iStream_c(),
    SINGLETON_MEMBER_CONSTRUCTOR
    mc_ident (ac_rsi)
  , mt_streamState (StreamRunning)
  , mt_awaitStep (AwaitCtsSend) // so next timeEvent will send out the CTS!
  , mi32_delayCtsUntil (msci32_timeNever) // means send out IMMEDIATELY (the initial CTS, afterwards delay some time!)
  , mui32_byteTotalSize (aui32_msgSize)
  , mui32_byteAlreadyReceived (0)
  , mui32_pkgNextToWrite (1)
  , mui32_pkgTotalSize ((aui32_msgSize + 6) / 7)
  , mui32_burstCurrent (0) // so we know that it's the first burst when calling the processBurst from the client
  , mui8_streamFirstByte (0) // meaning: not yet identified!! (when you check it, it's already set!)
  , mi32_timeoutLimit (msci32_timeNever)
  , mt_streamType (at_streamType)
 // mui8_pkgRemainingInBurst     // will be set in "expectBurst(wishingPkgs)", don't care here as mt_awaitStep == awaitCtsSend!!
{
  #ifdef ENABLE_MULTIPACKET_VARIANT_FAST_PACKET
  if (at_streamType == StreamFastPacket)
  { // other calculation for FastPacket, as the first package only has 6 netto data bytes AND first package begins with frame count 0
    mui32_pkgNextToWrite = 0;
    mui32_pkgTotalSize = (aui32_msgSize + 7) / 7;
  }

  if ((at_streamType == StreamFastPacket) ||
  #else
  if (
  #endif
   (ac_rsi.getDa() == 0xFF) || ab_skipCtsAwait)
  { // if it's FastPacket or BAM, then directly expect data to be sent! --- or if we directly wanna expect data (for fake streams..)
    expectBurst (255); // We're expecting one big burst directly now without CTS/DPO stuff!
  }
};


Stream_c::~Stream_c()
{
}


Stream_c&
Stream_c::operator= (const Stream_c& ref)
{
  SINGLETON_MEMBER_ASSIGN(ref)

  mc_ident = ref.mc_ident;
  mt_streamState = ref.mt_streamState;
  mt_awaitStep = ref.mt_awaitStep;
  mi32_delayCtsUntil = ref.mi32_delayCtsUntil;
  mui32_byteTotalSize = ref.mui32_byteTotalSize;
  mui32_byteAlreadyReceived = ref.mui32_byteAlreadyReceived;

  mui32_pkgNextToWrite = ref.mui32_pkgNextToWrite;
  mui32_pkgTotalSize = ref.mui32_pkgTotalSize;
  mui8_pkgRemainingInBurst = ref.mui8_pkgRemainingInBurst;
  mui32_burstCurrent = ref.mui32_burstCurrent;
  mui8_streamFirstByte = ref.mui8_streamFirstByte;
  mui32_dataPageOffset = ref.mui32_dataPageOffset;

  mi32_timeoutLimit = ref.mi32_timeoutLimit;
  mt_streamType = ref.mt_streamType;

  return *this;
}


void
Stream_c::awaitNextStep (NextComing_t at_awaitStep, int32_t ai32_timeOut)
{
  mt_awaitStep = at_awaitStep;
  if (at_awaitStep == AwaitCtsSend) {
    mi32_delayCtsUntil = HAL::getTime() + ai32_timeOut; // use the timeOut parameter here for the delay!!!!
    mi32_timeoutLimit = msci32_timeNever; // no timeOut on own sending...
  } else {
    mi32_timeoutLimit = (ai32_timeOut==msci32_timeNever) ? (msci32_timeNever) : (HAL::getTime()+ai32_timeOut);
  }
}


bool
Stream_c::readyToSendCts()
{
  return (mi32_delayCtsUntil == msci32_timeNever) || (HAL::getTime() >= mi32_delayCtsUntil);
}



/**
  will be directly called in sendCurrentCts
  (anyway, be sure to only call if in the right state)
  */
uint8_t
Stream_c::expectBurst(uint8_t wishingPkgs)
{
  #ifdef ENABLE_MULTIPACKET_VARIANT_FAST_PACKET
  if (mt_streamType == StreamFastPacket)
  {
    awaitNextStep (AwaitData, msci32_timeOutFP);
    mui8_pkgRemainingInBurst = mui32_pkgTotalSize; // for Fast-Packet there's only one burst!
  }
  else
  #endif
  { // Await after is CTS has timeout value of "msci32_timeOutT2=1250; // cts -> data(TP)/dpo(ETP)"
    if (mt_streamType & StreamEcmdMASK) awaitNextStep (AwaitDpo,  msci32_timeOutT2);
    else /* ----------------------- */ awaitNextStep (AwaitData, (getIdent().getDa() == 0xFF) ? msci32_timeOutT1 /* BAM */
                                                                                              : msci32_timeOutT2 /* dest-adr. */);
    // how many pkgs are missing at all? is it more than wished?
	mui8_pkgRemainingInBurst = std::min<uint32_t>( (mui32_pkgTotalSize - (mui32_pkgNextToWrite - 1)), uint32_t(wishingPkgs) );
  }

  // increase mui32_burstCurrent, the expected Burst is a next new one (of course)...
  mui32_burstCurrent++;

  return mui8_pkgRemainingInBurst;
}



// //////////////////////////////// +X2C Operation 814 : handleDataPacket
//! Parameter:
//! @param apu_data the offset given in the 8 byte data can pkg!
bool
Stream_c::handleDataPacket (const Flexible8ByteString_c* apu_data)
{ // ~X2C
  // expecting data at all?
  if (mt_awaitStep != AwaitData) {
    #ifdef DEBUG
      INTERNAL_DEBUG_DEVICE << "mt_awaitStep != AwaitData! --- mt_awaitStep ==" << mt_awaitStep << INTERNAL_DEBUG_DEVICE_ENDL;
    #endif
    return false;
  }

  bool b_pkgNumberWrong=false;
  switch (mt_streamType)
  {
    case StreamSpgnEcmdINVALID: // shouldn't occur, but catch that case anyway. (so break left out intentionally!)
    case StreamEpgnEcmd:   if ((apu_data->getUint8Data( 0 ) + mui32_dataPageOffset) != mui32_pkgNextToWrite) b_pkgNumberWrong=true; break;
    case StreamSpgnScmd:
    case StreamEpgnScmd:   if ((apu_data->getUint8Data( 0 ) /* no DPO for TP!! */) != mui32_pkgNextToWrite) b_pkgNumberWrong=true; break;
    #ifdef ENABLE_MULTIPACKET_VARIANT_FAST_PACKET
    case StreamFastPacket: if ((apu_data->getUint8Data( 0 ) & (0x1FU)) != mui32_pkgNextToWrite) b_pkgNumberWrong=true; break;
    #endif
  }

  if (b_pkgNumberWrong) {
    #ifdef DEBUG
      INTERNAL_DEBUG_DEVICE << "wrong pkg-number! ";
    #endif
    return false;
  }

  #ifdef ENABLE_MULTIPACKET_VARIANT_FAST_PACKET
  if ((mt_streamType == StreamFastPacket) && (mui32_pkgNextToWrite == 0))
  { // special FastPacket first-frame handling
    insertFirst6Bytes (apu_data->getUint8DataConstPointer(2));
    mui32_byteAlreadyReceived += 6;
  }
  else
  #endif
  {
    insert7Bytes (apu_data->getUint8DataConstPointer(1));
    mui32_byteAlreadyReceived += 7;
  }

  if (mui32_byteAlreadyReceived > mui32_byteTotalSize) mui32_byteAlreadyReceived = mui32_byteTotalSize; // cut any padded (0xFF) bytes...

  #ifdef DEBUG
    INTERNAL_DEBUG_DEVICE << "#" << mui32_pkgNextToWrite << " ";
  #endif

/// <<UPDATE_ALL>> Pkg counting stuff
  mui32_pkgNextToWrite++;
  mui8_pkgRemainingInBurst--;

  if (mui8_pkgRemainingInBurst == 0) {
    // End? or CTS for more?
    if (
        #ifdef ENABLE_MULTIPACKET_VARIANT_FAST_PACKET
        (mt_streamType == StreamFastPacket) ||
        #endif
        (mui32_pkgNextToWrite > mui32_pkgTotalSize))
    {
      // ---END--- was last packet! So
      awaitNextStep (AwaitNothing, msci32_timeNever); // no timeOut on own Send-Awaits
      mt_streamState = StreamFinished;
    } else {
      // ---CTS--- go for more!
      // Calculate the send delay
      const int32_t ci32_ctsSendDelay = __IsoAgLib::getMultiReceiveInstance4Comm().getCtsDelay();
      awaitNextStep (AwaitCtsSend, ci32_ctsSendDelay); // no timeOut on own Send-Awaits (this is automatically done in awaitNextStep) - parameter is the send-delay!
    }
    // (A complete / The last) chunk is received, handling will be done after this function returns
  } else {
    awaitNextStep (AwaitData, msci32_timeOutT1); // state "AwaitData" was already set until now, but it's about the timeOut!
  }

  return true;
} // -X2C




// //////////////////////////////// +X2C Operation 2861 : setDataPageOffset
//! Parameter:
//! @param aui32_dataPageOffset:
bool
Stream_c::setDataPageOffset(uint32_t aui32_dataPageOffset)
{ // ~X2C
  if (mt_awaitStep == AwaitDpo) {
    mui32_dataPageOffset = aui32_dataPageOffset;
    awaitNextStep (AwaitData, msci32_timeOutT5);
    #ifdef DEBUG
      INTERNAL_DEBUG_DEVICE << "DPO ";
    #endif
    return true; // was awaited!
  } else {
    #ifdef DEBUG
      INTERNAL_DEBUG_DEVICE << "\n\n DPO was not awaited at this state (" << (uint16_t) mt_awaitStep << "), please handle as error! \n ";
    #endif
    return false; // DPO was not awaited at this state, please handle as error!
  }
} // -X2C



bool
Stream_c::timedOut()
{
  bool b_result=false;

  if (mi32_timeoutLimit != msci32_timeNever) {
    if (HAL::getTime() > mi32_timeoutLimit) b_result = true;
  }

  return b_result;
}

} // end namespace __IsoAgLib
