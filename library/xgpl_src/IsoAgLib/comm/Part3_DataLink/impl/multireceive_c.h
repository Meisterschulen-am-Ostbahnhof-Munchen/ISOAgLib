/*
  multireceive_c.h

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef MULTIRECEIVE_C_H
#define MULTIRECEIVE_C_H

#define StreamLinear 0
#define StreamChunk 1

// forward declarations
namespace IsoAgLib {
  class ReceiveStreamIdentifier_c;
}


// IsoAgLib
#include <IsoAgLib/hal/hal_typedef.h>
#include <IsoAgLib/util/impl/singleton.h>
#include <IsoAgLib/scheduler/impl/schedulertask_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isoname_c.h>

// stl
#include <list>

// first line is for the include directive
// following line is for class definitions
#ifndef DEF_Stream_IMPL
  #define DEF_Stream_IMPL   StreamChunk
#endif
#ifndef DEF_Stream_c_IMPL
  #define DEF_Stream_c_IMPL StreamChunk_c
#endif

// include/define wanted Stream-Implementation
#if DEF_Stream_IMPL == StreamLinear
  #include "streamlinear_c.h"
#else
  #include "streamchunk_c.h"
#endif



// Begin Namespace __IsoAgLib
namespace __IsoAgLib {



/** struct for client definition */
class MultiReceiveClientWrapper_s : public ClientBase {
  public:
  MultiReceiveClientWrapper_s( CanCustomer_c& apc_client,
                               const IsoName_c& acrc_isoNameClient,
                               uint32_t aui32_pgn,
                               uint32_t aui32_pgnMask,
                               bool ab_alsoBroadcast,
                               bool ab_alsoGlobalErrors
                              #ifdef ENABLE_MULTIPACKET_VARIANT_FAST_PACKET
                              ,bool ab_isFastPacket
                              #endif
                              SINGLETON_VEC_KEY_PARAMETER_DEF_WITH_COMMA
                             );

  CanCustomer_c* mpc_client;
  IsoName_c mc_isoName;
  uint32_t mui32_pgn;
  uint32_t mui32_pgnMask;
  uint8_t mui8_cachedClientAddress; // kinda "cached" (normally clients register for receiving multi-packages to their own SA)
  bool mb_alsoBroadcast;
  bool mb_alsoGlobalErrors;
#ifdef ENABLE_MULTIPACKET_VARIANT_FAST_PACKET
  bool mb_isFastPacket;
#endif
};



class MultiReceive_c;
typedef SINGLETON_DERIVED(MultiReceive_c,Scheduler_Task_c) SingletonMultiReceive_c;

//  +X2C Class 192 : MultiReceive_c
//!  Stereotype: 76
class MultiReceive_c : public SingletonMultiReceive_c
{

public:
  MultiReceive_c();
  //  Operation: ~MultiReceive_c
  ~MultiReceive_c();

  //  Operation: processMsg
  bool processMsg();

  //  Operation: (de)registerClient
  void registerClient   (CanCustomer_c& arc_client, const IsoName_c& acrc_isoName,
                         uint32_t aui32_pgn, uint32_t aui32_pgnMask=0x3FFFF,
                         bool mb_alsoBroadcast=false, bool ab_alsoGlobalErrors=false
                         #ifdef ENABLE_MULTIPACKET_VARIANT_FAST_PACKET
                         , bool ab_isFastPacket=false
                         #endif
                         );

  void deregisterClient (CanCustomer_c& apc_client);
  void deregisterClient (CanCustomer_c& arc_client, const IsoName_c& acrc_isoName, uint32_t aui32_pgn, uint32_t aui32_pgnMask);

  //  Operation: createStream
  //! @param at_streamType
  //! @param ac_streamIdent
  //! @param aui32_msgSize
  Stream_c* createStream(StreamType_t at_streamType, IsoAgLib::ReceiveStreamIdentifier_c ac_streamIdent, uint32_t aui32_msgSize);

  //  Operation: getStream
  //! Parameter:
  //! @param ac_streamIdent
  Stream_c* getStream(IsoAgLib::ReceiveStreamIdentifier_c ac_streamIdent
  #ifdef ENABLE_MULTIPACKET_VARIANT_FAST_PACKET
  , bool ab_fastPacket=false
  #endif
  );

  int32_t getCtsDelay() const { return (getStreamCount() == 1) ? mi32_ctsSendDelayOneStream : mi32_ctsSendDelayMoreStreams; }

  //! Override the default configured values CONFIG_MULTI_RECEIVE_CTS_DELAY_AT_SINGLE_STREAM and CONFIG_MULTI_RECEIVE_CTS_DELAY_AT_MULTI_STREAMS
  //! This function is "thread safe", it only overwrites member variables.
  void setCtsDelays (int32_t ai32_ctsSendDelayOneStream, int32_t ai32_ctsSendDelayMoreStreams)
  { mi32_ctsSendDelayOneStream = ai32_ctsSendDelayOneStream;
    mi32_ctsSendDelayMoreStreams = ai32_ctsSendDelayMoreStreams; }

  //! This method is intentionally not placed in the interface class iMultiReceive_c,
  //! as it's only for experimental reasons (until the internal stream-scheduling is improved)
  //! This function is "thread safe", it only overwrites member variables.
  //! @param ai32_retriggerDelayForFirstCts defaults to 100 ms
  //! @param ai32_timePeriodForActiveStreams defaults to 100 ms
  void setExperimentalTimings (int32_t ai32_retriggerDelayForFirstCts, int32_t ai32_timePeriodForActiveStreams)
  { mi32_retriggerDelayForFirstCts = ai32_retriggerDelayForFirstCts;
    mi32_timePeriodForActiveStreams = ai32_timePeriodForActiveStreams; }

  /** this function is called by IsoMonitor_c on addition, state-change and removal of an IsoItem.
   * @param at_action enumeration indicating what happened to this IsoItem. @see IsoItemModification_en / IsoItemModification_t
   * @param acrc_isoItem reference to the (const) IsoItem which is changed (by existance or state)
   */
  void reactOnIsoItemModification (IsoItemModification_t /*at_action*/, IsoItem_c const& /*acrc_isoItem*/);

  /// Use to remove a "kept"-stream after it is gotten by "getFinishedJustKeptStream" and processed.
  void removeKeptStream(IsoAgLib::iStream_c* apc_keptStream);

  //  Operation: timeEvent
  bool timeEvent( void );

  //! virtual function which allows a scheduler client to define
  //! a minimum execution time, that should be saved after this item in the
  //! scheduler loop - some tasks might not be able to finish any sensible
  //! work in the default min exec time of 5msec
  virtual uint16_t getForcedMinExecTime() const;

  //  Operation: init
  void init();

  /** every subsystem of IsoAgLib has explicit function for controlled shutdown */
  void close( void );

  /**
    deliver reference to data pkg
    @return reference to the member BasePkg_c, which encapsulates the CAN send structure
  */
  CanPkgExt_c& data() { return mc_data; };

  /**
    deliver reference to data pkg as reference to CanPkgExt_c
    to implement the base virtual function correct
  */
  virtual CanPkgExt_c& dataBase();

  //! return 0x00-0xFF: first byte of the stream!
  //! return 0x100: first byte not yet known!
  //! return 0x101: not a valid index!
  uint16_t getStreamFirstByte (uint32_t ui32_index) const;
  uint32_t getStreamCount() const { return mlist_streams.size(); };

  uint32_t getStreamCompletion1000 (uint32_t ui32_index, bool b_checkFirstByte=false, uint8_t ui8_returnNullIfThisIsFirstByte=0x00 /*don't care if check=false*/) const;
  uint32_t getMaxStreamCompletion1000 (bool b_checkFirstByte=false, uint8_t ui8_returnNullIfThisIsFirstByte=0x00 /*don't care if check=false*/) const;

  bool isAtLeastOneWithFirstByte(uint8_t firstByte);

  IsoAgLib::iStream_c* getFinishedJustKeptStream (IsoAgLib::iStream_c* apc_lastKeptStream);

  ///  Used for Debugging Tasks in Scheduler_c
  virtual const char* getTaskName() const;

  enum TransferError_e {
    TransferErrorExtendedCommandOnStandardPgn = 100,
    TransferErrorAlreadyRunningStream = 101,
    TransferErrorWrongPackageAmountOrMessageSize = 102,
    TransferErrorClientRejectingStream = 103,
    TransferErrorDpoForUnknownOrUnopenedStream = 104,
    TransferErrorDpoNotAwaitedNow = 105,
    TransferErrorUnknownSafety = 107,
    TransferErrorUnknownOrInvalidCommandWithTpPgn = 108,
    TransferErrorWrongSequenceNumber = 109,
    TransferErrorStreamTimedOut = 110,
    TransferErrorNoStreamRunningForMultiPacketData = 111,
    TransferErrorBamToNonGlobalAddress = 112,
    TransferErrorBamNotTakenWrongPkgNumberOrMessageSize = 113,
    TransferErrorBamSequenceError = 114,
    TransferErrorPgnNotRequestedToReceive = 115,
    TransferErrorDpoForAStandardTpStream = 116,
    TransferErrorBamInBetweenAlreadyRunningStream = 117,
    TransferErrorFastPacketFrameButNoOpenStream = 118,
    TransferErrorFastpacketSequenceError = 119
  };

protected:

private:
  friend class SINGLETON_DERIVED(MultiReceive_c,Scheduler_Task_c);

  /**
    initialize directly after the singleton instance is created.
    this is called from singleton.h and should NOT be called from the user again.
    users please use init(...) instead.
  */
  void singletonInit();

  /** temp data where received data is put */
  CanPkgExt_c mc_data;

  //  Operation: getStream
  //! 
  //! @param aui8_sa
  //! @param aui8_da
  //! @return NULL for "doesn't exist", otherwise valid "Stream_c*"
  Stream_c* getStream(uint8_t aui8_sa, uint8_t aui8_da
  #ifdef ENABLE_MULTIPACKET_VARIANT_FAST_PACKET
  , bool ab_fastPacket
  #endif
  );

  bool anyMultiReceiveClientRegisteredForThisDa (uint8_t ui8_da);

  //  Operation: getClient
  //! Parameter:
  //! @param ac_streamIdent:
  //! @return NULL for "doesn't exist", otherwise valid "CanCustomer_c*"
  CanCustomer_c* getClient (IsoAgLib::ReceiveStreamIdentifier_c ac_streamIdent);

  //  Operation: sendCurrentCts
  //! Parameter:
  //! @param apc_stream:
  void sendCurrentCts(DEF_Stream_c_IMPL* apc_stream);

  bool finishStream (DEF_Stream_c_IMPL& rc_stream);

  //  Operation: sendEndOfMessageAck
  //! Parameter:
  //! @param apc_stream
  void sendEndOfMessageAck(DEF_Stream_c_IMPL* apc_stream);


  bool processStreamDataChunk_ofMatchingClient (Stream_c& arc_stream, bool b_lastChunk);


  void sendConnAbort(StreamType_t at_streamType, IsoAgLib::ReceiveStreamIdentifier_c ac_streamIdent);
  void connAbortTellClient(bool ab_sendConnAbort, Stream_c* apc_stream);
  void connAbortTellClientRemoveStream(bool ab_sendConnAbort, Stream_c* apc_stream);
  void removeStream(Stream_c* apc_stream);
  void notifyError (const IsoAgLib::ReceiveStreamIdentifier_c &ac_streamIdent, TransferError_e a_error);


private:
  static void notifyCanCustomerOfTransferError(
      CanCustomer_c &ar_canCustomer,
      const IsoAgLib::ReceiveStreamIdentifier_c &ac_streamIdent,
      TransferError_e a_transferError,
      bool ab_isGlobal) {
    ar_canCustomer.notificationOnMultiReceiveError(
        ac_streamIdent,
        uint8_t(a_transferError),
        ab_isGlobal);
  }


  // attributes
private:
  STL_NAMESPACE::list<DEF_Stream_c_IMPL> mlist_streams;

  STL_NAMESPACE::list<MultiReceiveClientWrapper_s> mlist_clients;

  int32_t mi32_ctsSendDelayOneStream;
  int32_t mi32_ctsSendDelayMoreStreams;

  // soon obsolete - only used for the current "bad" management of multiple streams with one schedulertask.
  int32_t mi32_retriggerDelayForFirstCts;
  int32_t mi32_timePeriodForActiveStreams;
}; // ~X2C

#if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
  /** C-style function, to get access to the unique MultiReceive_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  MultiReceive_c& getMultiReceiveInstance( uint8_t aui8_instance = 0 );
#else
  /** C-style function, to get access to the unique MultiReceive_c singleton instance */
  MultiReceive_c& getMultiReceiveInstance( void );
#endif

} // end namespace __IsoAgLib
#endif // -X2C
