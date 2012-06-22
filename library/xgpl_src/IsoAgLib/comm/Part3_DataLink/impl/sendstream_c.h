/*
  sendstream_c.h: transfer of a stream of multi message data

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef SEND_STREAM_H
#define SEND_STREAM_H

#include <IsoAgLib/isoaglib_config.h>
#include <IsoAgLib/util/config.h>
#include <IsoAgLib/util/impl/singleton.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isoname_c.h>
#include <IsoAgLib/comm/Part3_DataLink/impl/canpkgext_c.h>
#include <IsoAgLib/driver/system/impl/system_c.h>

#include "multisendeventhandler_c.h"
#include "../imultisendstreamer_c.h"

#include <list>
#include <functional>

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

// forward declarations
class MultiSend_c;

class SendStream_c : public ClientBase
{
public:
  enum sendState_t   { AwaitCts, SendData, SendPauseTillCts, AwaitEndofmsgack };
  enum sendSuccess_t { SendSuccess, SendAborted, Running };
  enum msgType_t     { IsoTP, IsoETP, IsoTPbroadcast, NmeaFastPacket };

public:
  // This c'tor does NOT REALLY "initialize "anything, use "init(...)" directly after construction!!!!
  SendStream_c (MultiSend_c& arc_multiSend MULTITON_INST_PARAMETER_DEF_WITH_COMMA )
    : MULTITON_PARENT_CONSTRUCTOR 
      mui32_pgn (0)
    , mc_isoNameReceiver()
    , mc_isoNameSender()
    , mpc_multiSendEventHandler(NULL)
    #if defined (ENABLE_MULTIPACKET_VARIANT_FAST_PACKET)
    , mui8_FpSequenceCounter (0)
    #endif
    , mi32_timestampToWaitTo (0)
    , mui32_dataBufferOffset (0)
    , mui32_dataSize (0)
    , mhpbui8_data (NULL)
    , men_sendState (AwaitCts) // dummy init state
    , men_sendSuccess (Running) // dummy init state
    , men_msgType (IsoTP) // dummy init state
    , mpc_mss (NULL)
    , mui32_packetNrRequestedInLastCts (0)
    , mui8_packetsLeftToSendInBurst (0)
    , mui8_packetsSentInThisBurst (0)
    , mrc_multiSend (arc_multiSend)
  {}

  /**
    call "init" directly after construction!
  */
  void init (const IsoName_c& acrc_isoNameSender,
             const IsoName_c& acrc_isoNameReceiver,
             const HUGE_MEM uint8_t* rhpb_data,
             uint32_t aui32_dataSize,
             uint32_t aui32_pgn,
             IsoAgLib::iMultiSendStreamer_c* apc_mss,
             msgType_t ren_msgType,
             MultiSendEventHandler_c* apc_multiSendEventHandler);

  /**
    perform periodical actions
    @return true -> remove me, I'm finished!
  */
  bool timeEvent (uint8_t aui8_pkgCnt);

  /**
    start processing of a process msg
    that matches this SendStream
  */
  bool processMsg( const CanPkgExt_c& arc_data );

  int32_t getNextTriggerTime() const { return mi32_timestampToWaitTo; }

  /**
    check if time delay is reached
    @return true -> time delay is over
  */
  bool timeHasCome() const {return (System_c::getTime() - mi32_timestampToWaitTo) >= 0; }

  /**
    check if current message is complete
  */
  bool isCompleteBurst() const { return (mui8_packetsLeftToSendInBurst == 0); }

  /**
    check if send of all data is complete
  */
  bool isCompleteData() const { return (mui32_dataBufferOffset >= mui32_dataSize); }

  /**
    deliver the data size in byte
    @return data size in uint32_t for RTS
  */
  uint32_t getDataSize() const { return mui32_dataSize; }

  bool isFinished() const { return (men_sendSuccess == SendAborted) || (men_sendSuccess == SendSuccess); }

  sendSuccess_t getSendSuccess() const { return men_sendSuccess; }

  /**
    abort the multipacket send stream
  */
  void abortSend ();

  bool matchSaDa (const IsoName_c& acrc_sa, const IsoName_c& acrc_da) const { return (acrc_sa == mc_isoNameSender) && (acrc_da == mc_isoNameReceiver); }

  MultiSendEventHandler_c* getMultiSendEventHandler() const { return mpc_multiSendEventHandler; }

  uint32_t pgn() const { return mui32_pgn;}
  const IsoName_c& receiver() const { return mc_isoNameReceiver; }
  const IsoName_c& sender() const { return mc_isoNameSender; }

private: // methods
  /**
    send an ISO message -> set the ident (depending on ab_data) and initiate sending to CAN
    @param ab_data true -> use the (E)TP_DATA_TRANSFER_PGN
                   false-> use the (E)TP_CONN_MANAGE_PGN
                   --Value is not of interest in Fast-Packet sending!--
  */
  void sendPacketIso( bool ab_data, MultiSendPkg_c& arc_data );

#if defined (ENABLE_MULTIPACKET_VARIANT_FAST_PACKET)
  /**
    send a FP message -> set the ident and initiate sending to CAN
  */
  void sendPacketFp();
#endif

  /**
    calculate the actual sequence number and
    calculate the amount of data bytes which must be placed in new CAN pkg
    @param ui8_nettoDataCnt amount of data which should be sent within this msg
  */
  void prepareSendMsg (uint8_t &ui8_nettoDataCnt);

  void retriggerIn (int32_t i32_timeDelta) { mi32_timestampToWaitTo = System_c::getTime() + i32_timeDelta;}

  void switchToState (sendState_t ren_sendState, int32_t i32_timeDelta) { men_sendState = ren_sendState; retriggerIn (i32_timeDelta); }

  const SendStream_c& operator= (const SendStream_c&); // Only private declaration, no implementation: Forbid assignment of SendStream_c instances.

  void notifySender(sendSuccess_t );

private: // attributes
/// Initialized on init(...)
  uint32_t mui32_pgn;
  IsoName_c mc_isoNameReceiver;
  IsoName_c mc_isoNameSender;

  MultiSendEventHandler_c* mpc_multiSendEventHandler;

  #if defined (ENABLE_MULTIPACKET_VARIANT_FAST_PACKET)
  uint8_t mui8_FpSequenceCounter;
  #endif

  /** timestamp for time control */
  int32_t mi32_timestampToWaitTo;

  /** data counter for data to send
      will be set on CTS and incremented after each sent packet. */
  uint32_t mui32_dataBufferOffset;

  /** size of the data complete */
  uint32_t mui32_dataSize;

  /** standard delay between two sent packets (between 50 and 200)
  uint16_t ui16_delay;
  __-> now using maxDelay from MultiSend, because not everybody can have its own delay, we'll have to take the max. */

  /** pointer to the data */
  const HUGE_MEM uint8_t* mhpbui8_data;

  sendState_t men_sendState;

  sendSuccess_t men_sendSuccess;

  /** are we broadcast, iso, extended, fast packet? */
  msgType_t men_msgType;

  /** pointer to an IsoAgLib::MultiSendStreamer_c class which streams out parts of the stream step by step */
  IsoAgLib::iMultiSendStreamer_c* mpc_mss;

  /** save ... from last CTS so we can see if the CTS was resent... */
  uint32_t mui32_packetNrRequestedInLastCts;

  /** cnt of pkg left to send in this burst */
  uint8_t mui8_packetsLeftToSendInBurst;

  /** cnt of pkg sent since the last DPO (ETP) - now also used to TP */
  uint8_t mui8_packetsSentInThisBurst;

/// Back reference to MultiSend_c for setting the MAX of all delays, this can only be managed here...
  MultiSend_c& mrc_multiSend;
}; // end class SendStream_c

} // End Namespace __IsoAgLib
#endif
