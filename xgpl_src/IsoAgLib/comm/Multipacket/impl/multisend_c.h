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
#include "multisendpkg_c.h"

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


// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

/** predeclare some classes from IsoAgLib namespace */
  class iMultiSend_c;
  class MultiSendStreamer_c;

/**
  This class implements the various multi message data
  transfer specifications of Fieldstar, LBS+ and ISO 11783
  for send of more the 8 bytes of data.

  @author Dipl.-Inform. Achim Spangler
*/
class MultiSend_c : public SINGLETON_DERIVED(MultiSend_c, ElementBase_c)  {
private:
public:
  enum sendState_t { Idle, SendRts, AwaitCts, SendData,
                      SendPauseTillCts, /* DecideAfterSend, */ AwaitEndofmsgack, SendFileEnd};
  enum sendSuccess_t {SendSuccess, SendAborted, Running};
  enum msgType_t {Din = 1, IsoTarget = 2, IsoBroadcast = 6};

  /** initialisation for MultiSend_c
  */
  void init( void );
  /** every subsystem of IsoAgLib has explicit function for controlled shutdown
    */
  void close( void );
  /** default destructor which has nothing to do */
  virtual ~MultiSend_c();

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
  bool sendDin(uint8_t rb_send, uint8_t rb_empf, HUGE_MEM uint8_t* rhpb_data, int32_t ri32_dataSize, uint16_t rui16_msgSize, uint16_t rb_fileCmd, bool rb_abortOnTimeout = false);
  #endif
  #if defined(USE_ISO_TERMINAL) || defined (USE_ISO_TERMINAL_SERVER)
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
  bool sendIsoTarget(uint8_t rb_send, uint8_t rb_empf, MultiSendStreamer_c* rpc_mss, int32_t ri32_pgn, sendSuccess_t* rpen_sendSuccessNotify);

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
  bool sendIsoTarget(uint8_t rb_send, uint8_t rb_empf, HUGE_MEM uint8_t* rhpb_data, int32_t ri32_dataSize, int32_t ri32_pgn, sendSuccess_t* rpen_sendSuccessNotify);

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
  bool sendIsoTargetIntern(uint8_t rb_send, uint8_t rb_empf, HUGE_MEM uint8_t* rhpb_data, int32_t ri32_dataSize, int32_t ri32_pgn, sendSuccess_t* rpen_sendSuccessNotify, MultiSendStreamer_c* rpc_mss);
  /**
    send a ISO broadcast multipacket message
    @param rb_send dynamic member no of sender
    @param rb_empf dynamic member no of receiver
    @param hpb_data HUGE_MEM pointer to the data
    @param ri32_dataSize size of the complete mask
    @param ri32_pgn pgn of the following messages that are to be grouped...
    @return true -> MultiSend_c was ready -> mask is spooled to target
  */
  bool sendIsoBroadcast(uint8_t rb_send, uint8_t rb_empf, HUGE_MEM uint8_t* rhpb_data, int32_t ri32_dataSize, int32_t ri32_pgn);
  #endif

  /**
    deliver reference to data pkg
    @return reference to MultiSendPkg_c which handles CAN I/O of process data
  */
  MultiSendPkg_c& data(){return c_data;};
  /**
    deliver reference to data pkg as reference to CANPkgExt_c
    to implement the base virtual function correct
  */
  virtual CANPkgExt_c& dataBase();
  /**
    deliver reference to data pkg for const read access
    @return const reference to MultiSendPkg_c which handles CAN I/O of process data
  */
  const MultiSendPkg_c& constData()const{return c_data;};

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
  /**
    abort the multipacket send stream
    (important if original target isn't active any more)
  */
  void abortSend();
  /**
    check if multipacket send is Running
    @return true -> multipacket is being sent at the moment
  */
  bool isRunning()const{return (en_sendSuccess == Running)?true:false;};
  /**
    check if multipacket send is finished with success
    @return true -> multipacket is finished with success
  */
  bool isSuccess()const{return (en_sendSuccess == SendSuccess)?true:false;};
  /**
    check if multipacket send is aborted
    @return true -> multipacket is aborted
  */
  bool isAborted()const{return (en_sendSuccess == SendAborted)?true:false;};
private: // Private methods
  friend class SINGLETON_DERIVED(MultiSend_c, ElementBase_c);
  friend class iMultiSend_c;
  /**
    HIDDEN constructor for a MultiSend_c object instance
    NEVER instantiate a variable of type MultiSend_c within application
    only access MultiSend_c via getMultiSendInstance() or getMultiSendInstance( int riLbsBusNr )
    in case more than one ISO11783 or DIN9684 BUS is used for IsoAgLib
    */
  MultiSend_c() { init(); };
  /**
    send a message -> set the ident and initiate sending to CAN
  */
  void sendIntern();
  /**
    check if the forced time delay since last timestamp is over
    @param ri32_time actual time
    @return true -> time delay is over
  */
  bool isDelayEnd(int32_t ri32_time)const{return ((ri32_time - i32_timestamp)>= ui16_delay)?true:false;};
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
    set the delay between two sent messages
    @param rui16_delay wanted delay
  */
  void setDelay(uint16_t rui16_delay);
  /**
    set the send state to Idle and set the CAN send pause to 0
  */
  void setSendStateIdle();
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
  /** using Extended Transport Protocol? */
  bool b_ext;
  /** msg object for CAN I/O */
  MultiSendPkg_c c_data;
  /** timestamp for time control */
  int32_t i32_timestamp;
  /** data counter for data to send */
  int32_t i32_DC;
  /** data coutner for acknowledged data */
  int32_t i32_ack_DC;
  /** ISO integrates the PGN of the multipacket message */
  int32_t i32_pgn;
  /** size of the data complete */
  int32_t i32_dataSize;
  /** size of one message (DIN: complete mask parted in amount of 'messages'; ISO identical to i32_dataSize) */
  uint16_t ui16_msgSize;
  /** standard delay between two sent packets (between 50 and 200) */
  uint16_t ui16_delay;
  /** pointer to the data */
  HUGE_MEM uint8_t* hpb_data;
  /** actual send state during Running process */
  sendState_t en_sendState;
  /** state: send success or aborted */
  sendSuccess_t en_sendSuccess;
  /** reference to variable that will be set upon state change */
  sendSuccess_t* pen_sendSuccessNotify;
  /** command to initiate file transfer */
  msgType_t en_msgType;
  /** counter for retries of sending RTS */
  uint8_t b_try;
  /** decide if direct abort is wanted on timeout from receier ack */
  bool b_abortOnTimeout;
  /** DIN transfer cmd */
  uint8_t b_fileCmd;
  /** empf field for the CAN identifier */
  uint8_t b_empf;
  /** send field for the CAN identifier */
  uint8_t b_send;
  /** sequence/offset number */
  uint32_t ui32_sequenceNr;
  uint32_t ui32_offset;
  /** save ... from last CTS so we can see if the CTS was resent... */
  uint32_t ui32_lastNextPacketNumberToSend;
  /** cnt of pkg to send in this msg */
  uint8_t b_pkgToSend;
  /** cnt of pkg sent since the last DPO */
  uint8_t b_pkgSent;
  /** pointer to an IsoAgLib::MultiSendStreamer_c class which streams out parts of the stream step by step */
  MultiSendStreamer_c* pc_mss;
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
