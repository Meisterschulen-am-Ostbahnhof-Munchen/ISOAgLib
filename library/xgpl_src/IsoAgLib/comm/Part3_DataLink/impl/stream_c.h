/***************************************************************************
                          stream_c.h -
                             -------------------
    class                : ::Stream_c
    project              : IsoAgLib
    begin                : Tue Jan 25 17:39:02 2005
    copyright            : (C) 2005 by Achim Spangler (a.spangler@osb-ag.de)
    email                : a.spangler@osb-ag:de
    $Id: stream_c.h 1228 2005-06-03 10:32:42Z wodok $

    $Log$
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
 * Copyright (C) 2000 - 2009 Dipl.-Inform. Achim Spangler                  *
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

#ifndef STREAM_C_H
#define STREAM_C_H

#include "../istream_c.h"
#include <IsoAgLib/driver/can/impl/canpkg_c.h>






// Begin Namespace __IsoAgLib
namespace __IsoAgLib {


// T1/T2 here are the same as in "multireceive_c.h"
// T1/T2 here are the same as in "multireceive_c.h"
static const int32_t msci32_ctsSendDelayOneStream   = CONFIG_MULTI_RECEIVE_CTS_DELAY_AT_SINGLE_STREAM; // data -> cts
static const int32_t msci32_ctsSendDelayMoreStreams = CONFIG_MULTI_RECEIVE_CTS_DELAY_AT_MULTI_STREAMS; // data -> cts
static const int32_t msci32_timeOutT1=1000;  // data -> data
static const int32_t msci32_timeOutT2=1250; // cts -> data(TP)/dpo(ETP)
static const int32_t msci32_timeOutT3=1250; // data/rts -> cts (not needed for checking here)
static const int32_t msci32_timeOutT4=1100;  // cts(0)<->cts
static const int32_t msci32_timeOutT5=1000;  // dpo -> data (ETP only, naming "T5" done by Martin)
static const int32_t msci32_timeOutFP=1000;  // FPdata <-> FPdata
static const int32_t msci32_timeNever=-1;
/** @todo SOON The above should be adapted to the new revision of Part 3 - Data Link Layer, or should we use the old IS timings to be correct??
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
