/***************************************************************************
                             multisend_c.h - transfer of multi message data
                             -------------------
    begin                : Fri Sep 28 2001
    copyright            : (C) 2000 - 2004 by Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
    type                 : Header
    $LastChangedDate$
    $LastChangedRevision$
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

#ifndef MULTI_SEND_H
#define MULTI_SEND_H


/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include <IsoAgLib/typedef.h>
#include <IsoAgLib/util/config.h>
#include <IsoAgLib/util/impl/cancustomer_c.h>
#include <IsoAgLib/util/impl/singleton.h>
#include <IsoAgLib/util/impl/elementbase_c.h>
#include <IsoAgLib/comm/SystemMgmt/ISO11783/impl/isomonitor_c.h>
#include <IsoAgLib/comm/SystemMgmt/ISO11783/impl/isoname_c.h>

#include "multisendpkg_c.h"
#include "../imultisendstreamer_c.h"

#include <list>
#include <functional>


// forward declarations
namespace IsoAgLib {
class iMultiSend_c;
class iMultiSendStreamer_c;
}


// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

// forward declarations
class vtObjectString_c;


class SendUploadBase_c {
public:
  /**
    StringUpload constructor that initializes all fields of this class (use only for Change String Value TP Commands)
  */
  SendUploadBase_c () : ui8_retryCount(0), ui32_uploadTimeout(0) {};
  SendUploadBase_c (uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, uint8_t byte5, uint8_t byte6, uint8_t byte7, uint8_t byte8, uint8_t byte9, uint32_t rui32_timeout)
    {set(byte1, byte2, byte3, byte4, byte5, byte6, byte7, byte8, byte9, rui32_timeout);};
  SendUploadBase_c (uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, uint8_t byte5, uint8_t byte6, uint8_t byte7, uint8_t byte8, uint32_t rui32_timeout)
    {set(byte1, byte2, byte3, byte4, byte5, byte6, byte7, byte8, rui32_timeout);};
  SendUploadBase_c (uint16_t rui16_objId, const char* rpc_string, uint16_t overrideSendLength, uint8_t ui8_cmdByte = 179 /*is standard case for VT Change String Value (TP)*/)
    {set(rui16_objId, rpc_string, overrideSendLength, ui8_cmdByte);};
  SendUploadBase_c (const SendUploadBase_c& ref_source);
  SendUploadBase_c (uint8_t* rpui8_buffer, uint32_t bufferSize)
    {set(rpui8_buffer, bufferSize);};
  const SendUploadBase_c& operator= (const SendUploadBase_c& ref_source);


  void set (uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, uint8_t byte5, uint8_t byte6, uint8_t byte7, uint8_t byte8, uint8_t byte9, uint32_t rui32_timeout);
  void set (uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, uint8_t byte5, uint8_t byte6, uint8_t byte7, uint8_t byte8, uint32_t rui32_timeout);
  void set (uint16_t rui16_objId, const char* rpc_string, uint16_t overrideSendLength, uint8_t ui8_cmdByte = 179 /*is standard case for VT Change String Value (TP)*/);
  void set (uint8_t* rpui8_buffer, uint32_t bufferSize);


  /// Use either an MultiSendStreamer or a direct ui8-Buffer
  STL_NAMESPACE::vector<uint8_t> vec_uploadBuffer;  // don't use malloc_alloc for uint8_t values - here the 8byte overhead per malloc item are VERY big
  // ==> chunk allocation which can be shared among instances is alot better

  /// Retry some times?
  uint8_t ui8_retryCount;

  /// TimeOut value (relative to the time the Upload was started!
  uint32_t ui32_uploadTimeout;
};


/**
  This class implements the ISO 11783 (Extended) Transport Protocol
  for send of more the 8 bytes of data.

  @author Dipl.-Inform. Achim Spangler
  @author Dipl.-Inf. Martin Wodok
*/
class MultiSend_c;
typedef SINGLETON_DERIVED(MultiSend_c, ElementBase_c) SingletonMultiSend_c;
class MultiSend_c : public SingletonMultiSend_c
                  , public __IsoAgLib::SaClaimHandler_c
{
public:
  enum sendState_t   { AwaitCts, SendData, SendPauseTillCts, AwaitEndofmsgack };
  enum sendSuccess_t { SendSuccess, SendAborted, Running };
  enum msgType_t     { IsoTP, IsoETP, IsoTPbroadcast, NmeaFastPacket };


  class SendStream_c : public ClientBase
  {
  public:

    SendStream_c (MultiSend_c& rrefc_multiSend SINGLETON_VEC_KEY_PARAMETER_DEF_WITH_COMMA ) : SINGLETON_PARENT_CONSTRUCTOR refc_multiSend (rrefc_multiSend)
      {}; // does NOT initialize anything, use "init(...)" directly after construction!!!!
    /**
      call "init" directly after construction!
    */
    void init (const ISOName_c& rrefc_isoNameSender, const ISOName_c& rrefc_isoNameReceiver, const HUGE_MEM uint8_t* rhpb_data, uint32_t rui32_dataSize, sendSuccess_t& rpen_sendSuccessNotify, uint32_t rui32_pgn, IsoAgLib::iMultiSendStreamer_c* rpc_mss, msgType_t ren_msgType);

    /**
      perform periodical actions
      @return true -> remove me, I'm finished!
    */
    bool timeEvent (uint8_t rui8_pkgCnt);

    /**
      start processing of a process msg
      that matches this SendStream
    */
    bool processMsg();

    int32_t getNextTriggerTime() const { return i32_timestampToWaitTo; }

    /**
      check if time delay is reached
      @return true -> time delay is over
    */
    bool timeHasCome() const {return (System_c::getTime() - i32_timestampToWaitTo) >= 0; }

    /**
      check if current message is complete
    */
    bool isCompleteBurst() const { return (ui8_burstPkgYetToSend == 0); }

    /**
      check if send of all data is complete
    */
    bool isCompleteData() const { return (ui32_dataBufferOffset >= ui32_dataSize); }

    /**
      deliver the data size in byte
      @return data size in uint32_t for RTS
    */
    uint32_t getDataSize() const { return ui32_dataSize; }

    bool isFinished() const { return (*pen_sendSuccessNotify == SendAborted) || (*pen_sendSuccessNotify == SendSuccess); }

    /**
      abort the multipacket send stream
    */
    void abortSend ();

    bool matchSaDa (const ISOName_c& rrefc_sa, const ISOName_c& rrefc_da) { return (rrefc_sa == c_isoNameSender) && (rrefc_da == c_isoNameReceiver); }

//    bool matchSa (uint8_t rui8_sa) { return (rui8_sa == ui8_sender); }
//    bool matchDa (uint8_t rui8_da) { return (rui8_da == ui8_receiver); }

  private: // methods
    /**
      send a message -> set the ident and initiate sending to CAN
    */
    void sendPacket();

    /**
      calculate the actual sequence number and
      calculate the amount of data bytes which must be placed in new CAN pkg
      @param ui8_nettoDataCnt amount of data which should be sent within this msg
    */
    void prepareSendMsg (uint8_t &ui8_nettoDataCnt);

    void retriggerIn (int32_t i32_timeDelta) { i32_timestampToWaitTo = System_c::getTime() + i32_timeDelta;}

    void switchToState (sendState_t ren_sendState, int32_t i32_timeDelta) { en_sendState = ren_sendState; retriggerIn (i32_timeDelta); }

  private: // attributes
  /// Initialized on init(...)
    uint32_t ui32_pgn;
    ISOName_c c_isoNameReceiver;
    ISOName_c c_isoNameSender;

    #if defined (NMEA_2000_FAST_PACKET)
    uint8_t ui8_FpSequenceCounter;
    #endif

    /** timestamp for time control */
    int32_t i32_timestampToWaitTo;

    /** data counter for data to send */
    uint32_t ui32_dataBufferOffset;

    /** size of the data complete */
    uint32_t ui32_dataSize;

    /** standard delay between two sent packets (between 50 and 200)
    uint16_t ui16_delay;
    \__-> now using maxDelay from MultiSend, because not everybody can have its own delay, we'll have to take the max. */

    /** pointer to the data */
    const HUGE_MEM uint8_t* hpb_data;

    /** actual send state during Running process */
    sendState_t en_sendState;

    /** reference to variable that will be set upon state change */
    sendSuccess_t* pen_sendSuccessNotify;

    /** are we broadcast, iso, extended, fast packet? */
    msgType_t en_msgType;

    /** pointer to an IsoAgLib::MultiSendStreamer_c class which streams out parts of the stream step by step */
    IsoAgLib::iMultiSendStreamer_c* pc_mss;

  /// Initialized on Runtime
    /** sequence/offset number */
    uint8_t ui8_sequenceNr;
    uint32_t ui32_dataPacketOffset; // only for ETP

    /** save ... from last CTS so we can see if the CTS was resent... */
    uint32_t ui32_lastNextPacketNumberToSend;

    /** cnt of pkg left to send in this burst */
    uint8_t ui8_burstPkgYetToSend;

    /** cnt of pkg sent since the last DPO */
    uint8_t b_pkgSent;

  /// Back reference to MultiSend_c for setting the MAX of all delays, this can only be managed here...
    MultiSend_c& refc_multiSend;
  }; // end class SendStream_c


public: // methods

  /** initialisation for MultiSend_c */
  void init();
  /** every subsystem of IsoAgLib has explicit function for controlled shutdown */
  void close();

  /** default destructor which has nothing special to do (only call close!) */
  virtual ~MultiSend_c() { close(); }

  /** this function is called by ISOMonitor_c when a new CLAIMED ISOItem_c is registered.
   * @param refc_isoName const reference to the item which ISOItem_c state is changed
   * @param rpc_newItem pointer to the currently corresponding ISOItem_c
    */
  virtual void reactOnMonitorListAdd( const __IsoAgLib::ISOName_c& refc_isoName, const __IsoAgLib::ISOItem_c* rpc_newItem );

  /** this function is called by ISOMonitor_c when a device looses its ISOItem_c.
   * @param refc_isoName const reference to the item which ISOItem_c state is changed
   * @param rui8_oldSa previously used SA which is NOW LOST -> clients which were connected to this item can react explicitly
   */
  virtual void reactOnMonitorListRemove( const __IsoAgLib::ISOName_c& refc_isoName, uint8_t rui8_oldSa );


  /**
    send an ISO target multipacket message with active retrieve of data-parts to send
    @param rb_send dynamic member no of sender
    @param rb_empf dynamic member no of receiver
    @param rpc_mss allow active build of data stream parts for upload by deriving data source class
                  from IsoAgLib::iMultiSendStreamer_c, which defines virtual functions to control the
                  retrieve of data to send. This is especially important for ISO_Terminal,
                  which assembles the data pool dependent on the terminal capabilities during upload
                  ( e.g. bitmap variants )
    @param ri32_pgn PGN to use for the upload
    @param rrefen_sendSuccessNotify -> pointer to send state var, where the current state
            is written by MultiSend_c
    @return true -> MultiSend_c was ready -> mask is spooled to target
  */
  bool sendIsoTarget (const ISOName_c& rrefc_isoNameSender, const ISOName_c& rrefc_isoNameReceiver, IsoAgLib::iMultiSendStreamer_c* rpc_mss, int32_t ri32_pgn, sendSuccess_t& rrefen_sendSuccessNotify)
    { return sendIntern (rrefc_isoNameSender, rrefc_isoNameReceiver, NULL, rpc_mss->getStreamSize(), rrefen_sendSuccessNotify, ri32_pgn, rpc_mss, ((rpc_mss->getStreamSize() >= 1786) ? IsoETP : IsoTP)); }

  /**
    send an ISO target multipacket message
    @param rb_send dynamic member no of sender
    @param rb_empf dynamic member no of receiver
    @param rhpb_data HUGE_MEM pointer to the data
    @param ri32_dataSize size of the complete mask
    @param ri32_pgn PGN to use for the upload
    @param rrefen_sendSuccessNotify -> pointer to send state var, where the current state
            is written by MultiSend_c
    @return true -> MultiSend_c was ready -> mask is spooled to target
  */
  bool sendIsoTarget (const ISOName_c& rrefc_isoNameSender, const ISOName_c& rrefc_isoNameReceiver, const HUGE_MEM uint8_t* rhpb_data, uint32_t rui32_dataSize, int32_t ri32_pgn, sendSuccess_t& rrefen_sendSuccessNotify)
    { return sendIntern (rrefc_isoNameSender, rrefc_isoNameReceiver, rhpb_data, rui32_dataSize, rrefen_sendSuccessNotify, ri32_pgn, NULL, ((rui32_dataSize >= 1786) ? IsoETP : IsoTP)); }

  /**
    send an ISO broadcast multipacket message
    @param rb_send dynamic member no of sender
    @param hpb_data HUGE_MEM pointer to the data
    @param rui16_dataSize size of the complete message, limited to TP (1785 bytes) only!
    @return true -> MultiSend_c was ready
  */
  bool sendIsoBroadcast (const ISOName_c& rrefc_isoNameSender, const HUGE_MEM uint8_t* rhpb_data, uint16_t rui16_dataSize, int32_t ri32_pgn, sendSuccess_t& rrefen_sendSuccessNotify)
    { return sendIntern (rrefc_isoNameSender, ISOName_c::ISONameUnspecified, rhpb_data, rui16_dataSize, rrefen_sendSuccessNotify, ri32_pgn, NULL /* NOT "yet" supported */, IsoTPbroadcast); }
  bool sendIsoBroadcast(const ISOName_c& rrefc_isoNameSender, IsoAgLib::iMultiSendStreamer_c* rpc_mss, int32_t ri32_pgn, sendSuccess_t& rrefen_sendSuccessNotify)
    { return sendIntern(rrefc_isoNameSender, ISOName_c::ISONameUnspecified, NULL, rpc_mss->getStreamSize(), rrefen_sendSuccessNotify, ri32_pgn, rpc_mss, IsoTPbroadcast);}

  #if defined(NMEA_2000_FAST_PACKET)
  bool sendIsoFastPacket (const ISOName_c& rrefc_isoNameSender, const ISOName_c& rrefc_isoNameReceiver, HUGE_MEM uint8_t* rhpb_data, uint16_t rui16_dataSize, int32_t ri32_pgn, sendSuccess_t& rrefen_sendSuccessNotify)
    { return sendIntern (rrefc_isoNameSender, rrefc_isoNameReceiver, rhpb_data, rui16_dataSize, rrefen_sendSuccessNotify, ri32_pgn, NULL, NmeaFastPacket); }

  bool sendIsoFastPacket (const ISOName_c& rrefc_isoNameSender, const ISOName_c& rrefc_isoNameReceiver, IsoAgLib::iMultiSendStreamer_c* rpc_mss, int32_t ri32_pgn, sendSuccess_t& rrefen_sendSuccessNotify)
    { return sendIntern (rrefc_isoNameSender, rrefc_isoNameReceiver, NULL, rpc_mss->getStreamSize(), rrefen_sendSuccessNotify, ri32_pgn, rpc_mss, NmeaFastPacket); }

  bool sendIsoFastPacketBroadcast (const ISOName_c& rrefc_isoNameSender, HUGE_MEM uint8_t* rhpb_data, uint16_t rui16_dataSize, int32_t ri32_pgn, sendSuccess_t& rrefen_sendSuccessNotify)
    { return sendIntern (rrefc_isoNameSender, ISOName_c::ISONameUnspecified, rhpb_data, rui16_dataSize, rrefen_sendSuccessNotify, ri32_pgn, NULL, NmeaFastPacket); }

  bool sendIsoFastPacketBroadcast (const ISOName_c& rrefc_isoNameSender, IsoAgLib::iMultiSendStreamer_c* rpc_mss, int32_t ri32_pgn, sendSuccess_t& rrefen_sendSuccessNotify)
    { return sendIntern (rrefc_isoNameSender, ISOName_c::ISONameUnspecified, NULL, rpc_mss->getStreamSize(), rrefen_sendSuccessNotify, ri32_pgn, rpc_mss, NmeaFastPacket); }
  #endif

  /**
    set the delay between two sent messages, called by SendStream_c's timeEvent() when neccessary
    @param rui16_delay wanted delay
  */
  void setSendStreamDelay(uint16_t rui16_delay);

  /**
    deliver reference to data pkg
    @return reference to MultiSendPkg_c which handles CAN I/O of process data
  */
  MultiSendPkg_c& data() { return c_data; }

  /**
    deliver reference to data pkg as reference to CANPkgExt_c
    to implement the base virtual function correct
  */
  virtual CANPkgExt_c& dataBase() { return c_data; }

  /**
    deliver reference to data pkg for const read access
    @return const reference to MultiSendPkg_c which handles CAN I/O of process data
  */
  const MultiSendPkg_c& constData() const { return c_data; }

  /**
    start processing of a process msg: delegate to specific SendStream_c instance
  */
  bool processMsg();

  /**
    perform periodical actions
    @return true -> all planned activities performed in allowed time
  */
  bool timeEvent();

  /** user function for explicit abort of any running matching stream. */
  void abortSend (const ISOName_c& rrefc_isoNameSender, const ISOName_c& rrefc_isoNameReceiver);

  /** check if at least one multisend stream is running */
  bool isMultiSendRunning() const { return (list_sendStream.size() > 0); }

  ///  Used for Debugging Tasks in Scheduler_c
  virtual const char* getTaskName() const;

protected: // methods
  //! Function set ui16_earlierInterval and
  //! ui16_laterInterval that will be used by
  //! getTimeToNextTrigger(retriggerType_t)
  //! can be overloaded by Childclass for special condition
  virtual void updateEarlierAndLatestInterval();


private: // Private methods
  friend class SINGLETON_DERIVED(MultiSend_c, ElementBase_c);
  friend class iMultiSend_c;
  friend class SendStream_c;

  /**
    HIDDEN constructor for a MultiSend_c object instance
    NEVER instantiate a variable of type MultiSend_c within application
    only access MultiSend_c via getMultiSendInstance() or getMultiSendInstance( int riLbsBusNr )
    in case more than one ISO11783 BUS is used for IsoAgLib
    */
  MultiSend_c();

  /**
    initialize directly after the singleton instance is created.
    this is called from singleton.h and should NOT be called from the user again.
    users please use init(...) instead.
  */
  void singletonInit();

  SendStream_c* addSendStream (const ISOName_c& rrefc_isoNameSender, const ISOName_c& rrefc_isoNameReceiver);
  SendStream_c* getSendStream (const ISOName_c& rrefc_isoNameSender, const ISOName_c& rrefc_isoNameReceiver);

  /** this function should NOT be called from INSIDE of timeEvent !
      ==> Only call it from OUTSIDE functions like init(), processMsg(), addSendStream, etc.
  */
  void calcAndSetNextTriggerTime();

  /**
    internal function to send a ISO target multipacket message
    @param rrefc_isoNameSender
    @param rrefc_isoNameReceiver
    @param rb_empf dynamic member no of receiver
    @param rhpb_data HUGE_MEM pointer to the data
    @param ri32_dataSize size of the complete mask
    @param ri32_pgn PGN to use for the upload
    @param rpen_sendSuccessNotify -> pointer to send state var, where the current state
            is written by MultiSend_c
    @param rpc_mss allow active build of data stream parts for upload by deriving data source class
                  from IsoAgLib::MultiSendStreamer_c, which defines virtual functions to control the
                  retrieve of data to send. This is especially important for ISO_Terminal,
                  which assembles the data pool dependent on the terminal capabilities during upload
                  ( e.g. bitmap variants )
    @return true -> MultiSend_c was ready -> mask is spooled to target
  */
  bool sendIntern (const ISOName_c& rrefc_isoNameSender, const ISOName_c& rrefc_isoNameReceiver, const HUGE_MEM uint8_t* rhpb_data, int32_t ri32_dataSize, sendSuccess_t& rpen_sendSuccessNotify, int32_t ri32_pgn, IsoAgLib::iMultiSendStreamer_c* rpc_mss, msgType_t ren_msgType);

  #if defined(NMEA_2000_FAST_PACKET)
  uint8_t allocFpSequenceCounter() { const uint8_t cui8_returnVal = ui8_nextFpSequenceCounter;
                                     ui8_nextFpSequenceCounter = (ui8_nextFpSequenceCounter+1) & 0x7;
                                     return cui8_returnVal; }
  #endif

private: // Private attributes
  #if defined(NMEA_2000_FAST_PACKET)
  uint8_t ui8_nextFpSequenceCounter;
  #endif

  /** msg object for CAN I/O */
  MultiSendPkg_c c_data;

  std::list<SendStream_c> list_sendStream;
};


#if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
  /** C-style function, to get access to the unique MultiSend_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  MultiSend_c& getMultiSendInstance( uint8_t rui8_instance = 0 );
#else
  /** C-style function, to get access to the unique Process_c singleton instance */
  MultiSend_c& getMultiSendInstance( void );
#endif

} // End Namespace __IsoAgLib
#endif
