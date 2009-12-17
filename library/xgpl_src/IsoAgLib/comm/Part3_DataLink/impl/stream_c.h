/*
  stream_c.h

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef STREAM_C_H
#define STREAM_C_H

#include "../istream_c.h"
#include <IsoAgLib/driver/can/impl/canpkg_c.h>






// Begin Namespace __IsoAgLib
namespace __IsoAgLib {


// T1/T2 here are the same as in "multireceive_c.h"
// T1/T2 here are the same as in "multireceive_c.h"
static const int32_t msci32_timeOutT1=1000;  // data -> data
static const int32_t msci32_timeOutT2=1250; // cts -> data(TP)/dpo(ETP)
static const int32_t msci32_timeOutT3=1250; // data/rts -> cts (not needed for checking here)
static const int32_t msci32_timeOutT4=1100;  // cts(0)<->cts
static const int32_t msci32_timeOutT5=1000;  // dpo -> data (ETP only, naming "T5" done by Martin)
static const int32_t msci32_timeOutFP=1000;  // FPdata <-> FPdata
static const int32_t msci32_timeNever=-1;
/** @todo SOON-178 DataLinkLayerRevision The above should be adapted to the new revision of Part 3 - Data Link Layer, or should we use the old IS timings to be correct??
  * also limit number of retries to 2 (if retries are implemented, if not, do so ;-) */


/** enum for Streaming state */
typedef enum {
  StreamInactive = 0,
  StreamHold     = 1,
  StreamRunning  = 2,
  StreamFinished = 3,
  StreamFinishedJustKept = 4,
} StreamingState_t;


/** enum for next awaited information */
typedef enum {
  AwaitNothing = 0,
  AwaitDpo     = 1,
  AwaitData    = 2,
  AwaitCtsSend = 3
} NextComing_t;


/** enum for type of stream */
typedef enum {
  StreamEcmdMASK = (1<<0),
  StreamEpgnMASK = (1<<1),
  StreamSpgnScmd        = 0, // Standard TP with Standard CommandSet
  StreamSpgnEcmdINVALID = 1, // invalid, not allowed!
  StreamEpgnScmd        = 2, // Extended TP with Standard CommandSet
  StreamEpgnEcmd        = 3
  #ifdef ENABLE_MULTIPACKET_VARIANT_FAST_PACKET
 ,StreamFastPacket      = 4 /// Fast-Packet addition: Check stream for Fast-Packet before proceeding anything!
  #endif
} StreamType_t;



//  +X2C Class 754 : Stream_c
class Stream_c : public IsoAgLib::iStream_c //, public ClientBase --> for single inheritance this is now a member variable
{

public:

  Stream_c (StreamType_t at_streamType, const IsoAgLib::ReceiveStreamIdentifier_c& ac_rsi, uint32_t aui32_msgSize SINGLETON_VEC_KEY_PARAMETER_DEF_WITH_COMMA , bool ab_skipCtsAwait=false);

  Stream_c& operator= (const Stream_c&);

  virtual ~Stream_c();

  bool timedOut();

  // Tell the stream what we wish to CTS, get as result how many can really be CTS'd.
  // Sets the internal state to expect (DPO and afterwards) the calculated amount of DATA commands next
  uint8_t expectBurst(uint8_t wishingPkgs);

  bool handleDataPacket (const Flexible8ByteString_c* apc_data);

/// Begin Additional Abstract methods handled by StreamLinear_c/StreamChunk_c
  //  Operation: insert
  virtual void insert7Bytes(const uint8_t* pui8_data)=0;
#ifdef ENABLE_MULTIPACKET_VARIANT_FAST_PACKET
  virtual void insertFirst6Bytes(const uint8_t* pui8_data)=0;
#endif

  //  Operation: getNotParsedSize
//  virtual uint32_t getNotParsedSize()=0;

  //  Operation: getNextNotParsed
//  virtual uint8_t getNextNotParsed()=0;

  //  Operation: getNotParsed
//  virtual uint8_t getNotParsed(uint32_t ui32_notParsedRelativeOffset)=0;
/// End Abstract methods handled by StreamLinear_c/StreamChunk_c


/// Begin StreamInput_c methods
  //  Operation: operator>>
  //! Parameter:
  //! @param ui8_data:
  StreamInput_c& operator>>(uint8_t& ui8_data) { ui8_data = getNextNotParsed(); return *this; };

  //  Operation: eof
//  virtual bool eof() const=0; // pass on abstract method
/// End StreamInput_c methods


  //  Operation: setDataPageOffset
  //! Parameter:
  //! @param aui32_dataPageOffset:
  bool setDataPageOffset(uint32_t aui32_dataPageOffset);

  // simple getter function!
  const IsoAgLib::ReceiveStreamIdentifier_c& getIdent() { return mc_ident; }
  StreamType_t     getStreamType()              { return mt_streamType; };
  StreamingState_t getStreamingState ()         { return mt_streamState; };
  NextComing_t     getNextComing ()             { return mt_awaitStep; };
  uint32_t getPkgNextToWrite ()           const { return mui32_pkgNextToWrite; };
  uint32_t getPkgTotalSize ()             const { return mui32_pkgTotalSize; };
  uint32_t getByteTotalSize ()            const { return mui32_byteTotalSize; };
  uint32_t getByteAlreadyReceived()       const { return mui32_byteAlreadyReceived; };
  uint32_t getBurstNumber()               const { return mui32_burstCurrent; };
  //! Provide first byte set by first call of processDataChunk... First byte containes command.
  uint8_t  getFirstByte()                 const { return mui8_streamFirstByte; };
  //! Store first byte of stream. First byte containes command.
  void     setFirstByte(uint8_t aui8_firstByte) { mui8_streamFirstByte = aui8_firstByte; };

  void setStreamFinishedJustKept() { awaitNextStep (AwaitNothing, msci32_timeNever); // no timeOut on own Send-Awaits
                                     mt_streamState = StreamFinishedJustKept; }; // from now on NOTHING more should be done with this stream!

  bool readyToSendCts();

private:

  void awaitNextStep (NextComing_t at_awaitStep, int32_t ai32_timeOut);


protected:

  SINGLETON_MEMBER_DEF

private:

  //  Attribute: mc_ident
  IsoAgLib::ReceiveStreamIdentifier_c mc_ident;

  //  Attribute: mt_streamState
  StreamingState_t mt_streamState;

  //  Attribute: mt_awaitStep
  NextComing_t mt_awaitStep;

  // Attribute mi32_delayCtsUntil
  int32_t mi32_delayCtsUntil;


/// Byte counting stuff
protected:
  //  Attribute: ui32_totalSize
  uint32_t mui32_byteTotalSize; // will be set at construction!

private:
  uint32_t mui32_byteAlreadyReceived;

/// Pkg counting stuff
  uint32_t mui32_pkgNextToWrite;      // should be initialized to 1
  uint32_t mui32_pkgTotalSize;        // calculated amount of pkgs to arrive for the given byteTotalSize!
  uint8_t   mui8_pkgRemainingInBurst; // the value requested by CTS
  uint32_t mui32_burstCurrent;        // counting the bursts, so we know if it's the first or a following!
  uint8_t   mui8_streamFirstByte;     // will be the command that it's containing. set at the first call to processDataChunk...
  uint32_t mui32_dataPageOffset;      //  Attribute: mui32_dataPageOffset: gets set when a DPO arrives...

  //  Attribute: mi32_timeoutLimit
  int32_t mi32_timeoutLimit;

  //  Attribute: t:StreamType
  StreamType_t mt_streamType;

  /// Don't forget to enhance the assignment-operator when adding new member-variables!
}; // ~X2C

} // end namespace __IsoAgLib
#endif // -X2C
