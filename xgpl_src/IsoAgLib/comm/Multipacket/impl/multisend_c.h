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
#include "multisendpkg_c.h"

#include <list>
#include <functional>

/* *************************************** */
/* ********** command defines ************ */
/* *************************************** */
#define FileStart      0
#define FileEnd        1
#define CM_RTS          16
#define CM_CTS          17
#define CM_EndofMsgACK  19
#define CM_BAM          32
#define eCM_RTS         20
#define eCM_CTS         21
#define eCM_DPO         22
#define eCM_EndofMsgACK 23
#define ConnAbort      255


// Begin Namespace IsoAgLib
namespace IsoAgLib {
/** predeclare some classes from IsoAgLib namespace */
class iMultiSend_c;
class iMultiSendStreamer_c;
}


// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

// forward declarations
class vtObjectString_c;

#ifdef USE_ISO_11783

class SendUploadBase_c {
public:
  /**
    StringUpload constructor that initializes all fields of this class (use only for Change String Value TP Commands)
  */
  SendUploadBase_c () : ui8_retryCount(0), ui32_uploadTimeout(0) {};
  SendUploadBase_c (uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, uint8_t byte5, uint8_t byte6, uint8_t byte7, uint8_t byte8, uint8_t byte9, uint32_t rui32_timeout);
  SendUploadBase_c (uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, uint8_t byte5, uint8_t byte6, uint8_t byte7, uint8_t byte8, uint32_t rui32_timeout);
  SendUploadBase_c (uint16_t rui16_objId, const char* rpc_string, uint16_t overrideSendLength, uint8_t ui8_cmdByte = 179 /*is standard case for VT Change String Value (TP)*/);
  SendUploadBase_c (const SendUploadBase_c& ref_source);
  SendUploadBase_c (uint8_t* rpui8_buffer, uint32_t bufferSize);
  const SendUploadBase_c& operator= (const SendUploadBase_c& ref_source);

  /// Use either an MultiSendStreamer or a direct ui8-Buffer
  STL_NAMESPACE::vector<uint8_t> vec_uploadBuffer;  // don't use malloc_alloc for uint8_t values - here the 8byte overhead per malloc item are VERY big
  // ==> chunk allocation which can be shared among instances is alot better

  /// Retry some times?
  uint8_t ui8_retryCount;

  /// TimeOut value (relative to the time the Upload was started!
  uint32_t ui32_uploadTimeout;
};
#endif


/**
  This class implements the various multi message data
  transfer specifications of Fieldstar, LBS+ and ISO 11783
  for send of more the 8 bytes of data.

  @author Dipl.-Inform. Achim Spangler
*/
class MultiSend_c : public SINGLETON_DERIVED(MultiSend_c, ElementBase_c), public __IsoAgLib::SaClaimHandler_c
{
public: // idle was thrown out as it's now idle if no SendStream is in the list for this specific sa/da-pair!
  enum sendState_t { /*Idle,*/ SendRts, AwaitCts, SendData, SendPauseTillCts, /* DecideAfterSend, */ AwaitEndofmsgack, SendFileEnd
                    #if defined (NMEA_2000_FAST_PACKET)
                    ,SendFpFirstFrame, SendFpDataFrame
                    #endif
  };
  enum sendSuccess_t {SendSuccess, SendAborted, Running};
  enum msgType_t {Din = 1, IsoTarget = 2, IsoBroadcast = 6};

  class SendStream_c : public ClientBase
  {
  public:

    /// Object construction
    SendStream_c(MultiSend_c& rrefc_multiSend SINGLETON_VEC_KEY_PARAMETER_DEF_WITH_COMMA ) : SINGLETON_PARENT_CONSTRUCTOR pc_multiSend (&rrefc_multiSend)
      {}; // does NOT initialize anything, use "init(...)" directly after construction!!!!
    void initIso (uint8_t rb_send, uint8_t rb_empf, const HUGE_MEM uint8_t* rhpb_data, int32_t ri32_dataSize, sendSuccess_t& rpen_sendSuccessNotify, int32_t ri32_pgn, IsoAgLib::iMultiSendStreamer_c* rpc_mss, msgType_t ren_msgType
      #if defined(NMEA_2000_FAST_PACKET)
      , bool rb_useFastPacket = false
      #endif
    );
    void initDin (uint8_t rb_send, uint8_t rb_empf, const HUGE_MEM uint8_t* rhpb_data, int32_t ri32_dataSize, uint16_t rui16_msgSize, sendSuccess_t& rpen_sendSuccessNotify, uint16_t rb_fileCmd, bool rb_abortOnTimeout);
    void init    (uint8_t rb_send, uint8_t rb_empf, const HUGE_MEM uint8_t* rhpb_data, int32_t ri32_dataSize, uint16_t rui16_msgSize, sendSuccess_t& rpen_sendSuccessNotify, IsoAgLib::iMultiSendStreamer_c* rpc_mss, msgType_t ren_msgType, bool rb_ext, uint16_t rui16_delay);

    /**
      perform periodical actions
      @return true -> remove me, I'm finished!
    */
    bool timeEvent( uint8_t rui8_pkgCnt, int32_t ri32_time );

    /**
      start processing of a process msg
      that matches this SendStream
    */
    bool processMsg();

    /**
      check if the forced time delay since last timestamp is over
      @param ri32_time actual time
      @return true -> time delay is over
    */
    bool isDelayEnd(int32_t ri32_time)const{return ((ri32_time - i32_timestamp)>= pc_multiSend->getMaxDelay())?true:false;};

    /**
      check if the forced time delay since last timestamp is over
      @param ri32_time actual time
      @param rui16_dynamicDelay individual delay
      @return true -> time delay is over
    */
    bool isDelayEnd(int32_t ri32_time, uint16_t rui16_dynamicDelay)const{return ((ri32_time - i32_timestamp)>= rui16_dynamicDelay)?true:false;};

    /**
      check if actual message is complete
    */
    bool isCompleteMsg() const;

    /**
      check if send of all data is complete
      @return true -> i32_DC tell that i32_dataSize ist reached
    */
    bool isCompleteData() const;

    /**
      deliver the amount of messages for IsoAgLib+ data stream
      (no problem if called for ISO)
      @return amount of messages for stored complete data size for TP.FileStart
    */
    uint16_t getMsgCnt()const;

    /**
      deliver the message size in byte
      @return message size in uint8_t for CM_RTS
    */
    uint16_t getMsgSize()const{return ui16_msgSize;};

    /**
      deliver the data size in byte
      @return data size in uint32_t for CM_RTS
    */
    uint32_t getDataSize()const{return i32_dataSize;};

    /**
      deliver the amunt of CAN pkg per message
      @return CAN pkg amount per message for CM_RTS
    */
    uint16_t getPkgPerMsg()const;

    /**
      deliver the message number in LBS+ data stream
      @return message number to insert in CM_RTS
    */
    uint16_t getMsgNr()const;

    /**
      read the the commanded mesage size from CTS CAN pkg
      (for IsoAgLib+ a change of ui16_msgSize is only accepted, if
      new value results in increase of Pkg amount, because
      often the commanded ui16_msgSize is not round factor of 7)
      @return commanded message size
    */
    uint16_t readMsgSize()const;

    bool isFinished() const { return (*pen_sendSuccessNotify == SendAborted) || (*pen_sendSuccessNotify == SendSuccess); };

    /**
      read the next to be sent data position from CM_CTS
      @return new i32_DC value
    */
    int32_t read_DC();

    /**
      read the acknowledged data amount from EndOfMsgAck
      @return amount of correct received data byte
    */
    int32_t readAck_DC();

    /**
      calculate the actual sequence number and
      calculate the amount of data bytes which must be placed in new CAN pkg
      @param ui8_nettoDataCnt amount of data which should be sent within this msg
    */
    void prepareSendMsg(uint8_t &ui8_nettoDataCnt);

    /**
      abort the multipacket send stream
      (important if original target isn't active any more)
    */
    void abortSend ();

    bool matchSaDa (uint8_t rui8_sa, uint8_t rui8_da) { return (rui8_sa == b_send) && (rui8_da == b_empf); };

    bool matchSa (uint8_t rui8_sa) { return (rui8_sa == b_send); };
    bool matchDa (uint8_t rui8_da) { return (rui8_da == b_empf); };

  private: // methods
    /**
      send a message -> set the ident and initiate sending to CAN
    */
    void sendIntern();

  private: // attributes
  /// Initialized on Init - DIN & ISO
    /** empf field for the CAN identifier */
    uint8_t b_empf;
    /** send field for the CAN identifier */
    uint8_t b_send;

    /** using Extended Transport Protocol? set this flag for DIN to FALSE, it is also used in the DIN case! */
    bool b_ext;
    #if defined (NMEA_2000_FAST_PACKET)
    bool b_fp;
    uint8_t ui8_FpSequenceCounter;
    uint8_t ui8_FpFrameNr;
    #endif
    /** timestamp for time control */
    int32_t i32_timestamp;
    /** data counter for data to send */
    int32_t i32_DC;
    /** data coutner for acknowledged data */
    int32_t i32_ack_DC;
    /** size of the data complete */
    int32_t i32_dataSize;
    /** size of one message (DIN: complete mask parted in amount of 'messages'; ISO identical to i32_dataSize) */
    uint16_t ui16_msgSize;
    /** standard delay between two sent packets (between 50 and 200)
    uint16_t ui16_delay;
    \__-> now using maxDelay from MultiSend, because not everybody can have its own delay, we'll have to take the max. */
    /** pointer to the data */
    const HUGE_MEM uint8_t* hpb_data;
    /** actual send state during Running process */
    sendState_t en_sendState;
    /** reference to variable that will be set upon state change */
    sendSuccess_t* pen_sendSuccessNotify;
    /** command to initiate file transfer */
    msgType_t en_msgType;
    /** decide if direct abort is wanted on timeout from receier ack */
    bool b_abortOnTimeout;
    /** DIN transfer cmd */
    uint8_t b_fileCmd;
    /** pointer to an IsoAgLib::MultiSendStreamer_c class which streams out parts of the stream step by step */
    IsoAgLib::iMultiSendStreamer_c* pc_mss;

  /// Initialized on Init - ISO-Specific
    /** ISO integrates the PGN of the multipacket message */
    int32_t i32_pgn;

  /// Initialized on Init - DIN-Specific
    /** counter for retries of sending RTS */
    uint8_t b_try;

  /// Initialized on Runtime
    /** sequence/offset number */
    uint32_t ui32_sequenceNr;
    uint32_t ui32_offset;
    /** save ... from last CTS so we can see if the CTS was resent... */
    uint32_t ui32_lastNextPacketNumberToSend;
    /** cnt of pkg to send in this msg */
    uint8_t b_pkgToSend;
    /** cnt of pkg sent since the last DPO */
    uint8_t b_pkgSent;

  /// Back reference to MultiSend_c for setting the MAX of all delays, this can only be managed here...
    MultiSend_c* pc_multiSend;
  };


public: // methods

  /** initialisation for MultiSend_c */
  void init( void );


  /** default destructor which has nothing to do */
  virtual ~MultiSend_c();

  /** every subsystem of IsoAgLib has explicit function for controlled shutdown */
  void close( void );


   /** this function is called by ISOMonitor_c when a new CLAIMED ISOItem_c is registered.
   * @param refc_devKey const reference to the item which ISOItem_c state is changed
   * @param rpc_newItem pointer to the currently corresponding ISOItem_c
    */
  virtual void reactOnMonitorListAdd( const __IsoAgLib::DevKey_c& refc_devKey, const __IsoAgLib::ISOItem_c* rpc_newItem );
   /** this function is called by ISOMonitor_c when a device looses its ISOItem_c.
   * @param refc_devKey const reference to the item which ISOItem_c state is changed
   * @param rui8_oldSa previously used SA which is NOW LOST -> clients which were connected to this item can react explicitly
    */
  virtual void reactOnMonitorListRemove( const __IsoAgLib::DevKey_c& refc_devKey, uint8_t rui8_oldSa );


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
  bool sendDin(uint8_t rb_send, const uint8_t rb_empf, const HUGE_MEM uint8_t* rhpb_data, int32_t ri32_dataSize, uint16_t rui16_msgSize, sendSuccess_t& rrefen_sendSuccessNotify, uint16_t rb_fileCmd, bool rb_abortOnTimeout = false);
  #endif
  #ifdef USE_ISO_11783
   /**
    send a ISO target multipacket message with active retrieve of data-parts to send
    @param rb_send dynamic member no of sender
    @param rb_empf dynamic member no of receiver
    @param rpc_mss allow active build of data stream parts for upload by deriving data source class
                  from IsoAgLib::MultiSendStreamer_c, which defines virtual functions to control the
                  retrieve of data to send. This is especially important for ISO_Terminal,
                  which assembles the data pool dependent on the terminal capabilities during upload
                  ( e.g. bitmap variants )
    @param ri32_pgn PGN to use for the upload
    @param rpen_sendSuccessNotify -> pointer to send state var, where the current state
            is written by MultiSend_c
    @return true -> MultiSend_c was ready -> mask is spooled to target
  */
  bool sendIsoTarget(uint8_t rb_send, uint8_t rb_empf, IsoAgLib::iMultiSendStreamer_c* rpc_mss, int32_t ri32_pgn, sendSuccess_t& rpen_sendSuccessNotify);

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
  bool sendIsoTarget(uint8_t rb_send, uint8_t rb_empf, const HUGE_MEM uint8_t* rhpb_data, int32_t ri32_dataSize, int32_t ri32_pgn, sendSuccess_t& rpen_sendSuccessNotify);
  #if defined(NMEA_2000_FAST_PACKET)
  bool sendIsoFastPacket(uint8_t rb_send, uint8_t rb_empf, HUGE_MEM uint8_t* rhpb_data, int32_t ri32_dataSize, int32_t ri32_pgn, sendSuccess_t& rpen_sendSuccessNotify);
  bool sendIsoFastPacket(uint8_t rb_send, uint8_t rb_empf, MultiSendStreamer_c* rpc_mss, int32_t ri32_pgn, sendSuccess_t& rpen_sendSuccessNotify);
#endif

  /**
    send a ISO broadcast multipacket message
    @param rb_send dynamic member no of sender
    @param rb_empf dynamic member no of receiver
    @param hpb_data HUGE_MEM pointer to the data
    @param ri32_dataSize size of the complete mask
    @param ri32_pgn pgn of the following messages that are to be grouped...
    @return true -> MultiSend_c was ready -> mask is spooled to target
  */
  bool sendIsoBroadcast(uint8_t rb_send, uint8_t rb_empf, const HUGE_MEM uint8_t* rhpb_data, int32_t ri32_dataSize, int32_t ri32_pgn, sendSuccess_t& rrefen_sendSuccessNotify);
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
  MultiSendPkg_c& data() { return c_data; };
  /**
    deliver reference to data pkg as reference to CANPkgExt_c
    to implement the base virtual function correct
  */
  virtual CANPkgExt_c& dataBase() { return c_data; };
  /**
    deliver reference to data pkg for const read access
    @return const reference to MultiSendPkg_c which handles CAN I/O of process data
  */
  const MultiSendPkg_c& constData() const { return c_data; };

  /**
    start processing of a process msg
    ignore all invalid messages where SEND is not of a member with claimed address,
    or where EMPF isn't valid

    possible errors:
      * Err_c::elNonexistent on SEND/EMPF not registered in Monitor-List
  */
  bool processMsg();

  /**
    perform periodical actions
    @return true -> all planned activities performed in allowed time
  */
  bool timeEvent( void );

  /** user function for explicit abort of any running matching stream. */
  void abortSend (uint8_t rb_send, uint8_t rb_empf);

  /** check if at least one multisend stream is running */
  bool isMultiSendRunning() const { return (list_sendStream.size() > 0)?true:false;};

private: // Private methods
  friend class SINGLETON_DERIVED(MultiSend_c, ElementBase_c);
  friend class iMultiSend_c;
  friend class SendStream_c;

  /**
    HIDDEN constructor for a MultiSend_c object instance
    NEVER instantiate a variable of type MultiSend_c within application
    only access MultiSend_c via getMultiSendInstance() or getMultiSendInstance( int riLbsBusNr )
    in case more than one ISO11783 or DIN9684 BUS is used for IsoAgLib
    */
  MultiSend_c() {};

  /**
    initialize directly after the singleton instance is created.
    this is called from singleton.h and should NOT be called from the user again.
    users please use init(...) instead.
  */
  void singletonInit();

  SendStream_c* getSendStream(uint8_t rui8_sa, uint8_t rui8_da);

  SendStream_c* addSendStream(uint8_t rui8_sa, uint8_t rui8_da);

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
                  which assembles the data pool dependent on the terminal capabilities during upload
                  ( e.g. bitmap variants )
    @return true -> MultiSend_c was ready -> mask is spooled to target
  */
  bool sendIsoIntern(uint8_t rb_send, uint8_t rb_empf, const HUGE_MEM uint8_t* rhpb_data, int32_t ri32_dataSize, sendSuccess_t& rpen_sendSuccessNotify, int32_t ri32_pgn, IsoAgLib::iMultiSendStreamer_c* rpc_mss, msgType_t ren_msgType
  #if defined(NMEA_2000_FAST_PACKET)
  , bool rb_useFastPacket = false
  #endif
  );

  /**
    set the delay between two sent messages, no max calculation, only for internal use!
    @param rui16_delay wanted delay
  */
  void setDelay(uint16_t rui16_delay);

  uint16_t getMaxDelay() const { return ui16_maxDelay; };

private: // Private attributes
  #if defined ( _MSC_VER )
    #if _MSC_VER <= 1200
      // Visual Studio VC++ 6.0 ANSI conformance is really ugly
      // it can't handle static const member variables
      // ( VC++ 7.0 is a lot more ANSI C++ standard conformant )
      enum { scui8_isoCanPkgDelay = 4 };
    #else
      static const unsigned char scui8_isoCanPkgDelay = 4;
    #endif
  #else
    static const unsigned char scui8_isoCanPkgDelay = 4;
  #endif

  /** msg object for CAN I/O */
  MultiSendPkg_c c_data;

  std::list<SendStream_c> list_sendStream;

  /** maximum of all delay-values, as we can't set the delay  */
  uint16_t ui16_maxDelay;
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

}
#endif
