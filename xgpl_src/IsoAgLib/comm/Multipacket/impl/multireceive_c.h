/***************************************************************************
                          multireceive_c.h -
                             -------------------
    class                : ::MultiReceive_c
    project              : IsoAgLib
    begin                : Tue Jan 25 17:40:11 2005
    copyright            : (C) 2005 by Achim Spangler (a.spangler@osb-ag.de)
    email                : a.spangler@osb-ag:de
    $Id: multireceive_c.h 1258 2005-06-07 10:14:39Z wodok $

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

#ifndef MULTIRECEIVE_C_H
#define MULTIRECEIVE_C_H

#define StreamLinear 0
#define StreamChunk 1

// forward declarations
namespace IsoAgLib {
  class ReceiveStreamIdentifier_c;
  class MultiReceiveClient_c;
}


// IsoAgLib
#include "IsoAgLib/typedef.h"
#include <IsoAgLib/util/impl/singleton.h>
#include <IsoAgLib/util/impl/elementbase_c.h>
#include <IsoAgLib/util/impl/getypos_c.h>
#include <IsoAgLib/comm/SystemMgmt/ISO11783/impl/isomonitor_c.h>

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
class MultiReceiveClientWrapper_s {
 public:
  MultiReceiveClientWrapper_s( IsoAgLib::MultiReceiveClient_c* rpc_client,
                               uint8_t rui8_clientAddress,
                               uint32_t rui32_pgn,
                               bool rb_alsoBroadcast,
                               bool rb_alsoGlobalErrors
                              #ifdef NMEA_2000_FAST_PACKET
                              ,bool rb_isFastPacket
                              #endif
                              );

  IsoAgLib::MultiReceiveClient_c* pc_client;
  uint8_t ui8_clientAddress; // kinda "cached" (normally clients register for receiving multi-packages to their own SA)
  uint32_t ui32_pgn;
  bool b_alsoBroadcast;
  bool b_alsoGlobalErrors;
#ifdef NMEA_2000_FAST_PACKET
  bool b_isFastPacket;
#endif

  // the gtp is generated from the source address through iso monitor!
  __IsoAgLib::GetyPos_c c_gtp;
};



class MultiReceive_c;
typedef SINGLETON_DERIVED(MultiReceive_c,__IsoAgLib::ElementBase_c) SingletonMultiReceive_c;

//  +X2C Class 192 : MultiReceive_c
//!  Stereotype: 76
class MultiReceive_c : public SingletonMultiReceive_c, public __IsoAgLib::SaClaimHandler_c
{

public:

  //  Operation: ~MultiReceive_c
  ~MultiReceive_c();

  //  Operation: processMsg
  bool processMsg();

  //  Operation: (de)registerClient
  void registerClient   (uint32_t rui32_pgn, uint8_t rui8_clientAddress,
                         IsoAgLib::MultiReceiveClient_c* rpc_client, bool b_alsoBroadcast=false, bool rb_alsoGlobalErrors=false
                         #ifdef NMEA_2000_FAST_PACKET
                         , bool rb_isFastPacket=false
                         #endif
                         );
  void deregisterClient (IsoAgLib::MultiReceiveClient_c* rpc_client);

  //  Operation: createStream
  //! Parameter:
  //! @param rc_streamIdent:
  Stream_c* createStream(StreamType_t rt_streamType, IsoAgLib::ReceiveStreamIdentifier_c rc_streamIdent, uint32_t rui32_msgSize);

  //  Operation: getStream
  //! Parameter:
  //! @param rc_streamIdent:
  Stream_c* getStream(IsoAgLib::ReceiveStreamIdentifier_c rc_streamIdent
  #ifdef NMEA_2000_FAST_PACKET
  , bool rb_fastPacket=false
  #endif
  );

   /** this function is called by ISOMonitor_c when a new CLAIMED ISOItem_c is registered.
   * @param refc_gtp const reference to the item which ISOItem_c state is changed
   * @param rpc_newItem pointer to the currently corresponding ISOItem_c
    */
  virtual void reactOnMonitorListAdd( const __IsoAgLib::GetyPos_c& refc_gtp, const __IsoAgLib::ISOItem_c* rpc_newItem );
   /** this function is called by ISOMonitor_c when a device looses its ISOItem_c.
   * @param refc_gtp const reference to the item which ISOItem_c state is changed
   * @param rui8_oldSa previously used SA which is NOW LOST -> clients which were connected to this item can react explicitly
    */
  virtual void reactOnMonitorListRemove( const __IsoAgLib::GetyPos_c& refc_gtp, uint8_t rui8_oldSa );

  /// Use to remove a "kept"-stream after it is gotten by "getFinishedJustKeptStream" and processed.
  void removeKeptStream(IsoAgLib::iStream_c* rpc_keptStream);

  //  Operation: timeEvent
  bool timeEvent( void );

  //  Operation: init
  void init();

  /** every subsystem of IsoAgLib has explicit function for controlled shutdown */
  void close( void );

  /**
    deliver reference to data pkg
    @return reference to the member BasePkg_c, which encapsulates the CAN send structure
  */
  __IsoAgLib::CANPkgExt_c& data() { return c_data; };

  /**
    deliver reference to data pkg as reference to CANPkgExt_c
    to implement the base virtual function correct
  */
  virtual __IsoAgLib::CANPkgExt_c& dataBase();

  //! return 0x00-0xFF: first byte of the stream!
  //! return 0x100: first byte not yet known!
  //! return 0x101: not a valid index!
  uint16_t getStreamFirstByte (uint32_t ui32_index) const;
  uint32_t getStreamCount() const { return list_streams.size(); };

  uint32_t getStreamCompletion1000 (uint32_t ui32_index, bool b_checkFirstByte=false, uint8_t ui8_returnNullIfThisIsFirstByte=0x00 /*don't care if check=false*/) const;
  uint32_t getMaxStreamCompletion1000 (bool b_checkFirstByte=false, uint8_t ui8_returnNullIfThisIsFirstByte=0x00 /*don't care if check=false*/) const;

  bool isAtLeastOneWithFirstByte(uint8_t firstByte);

  IsoAgLib::iStream_c* getFinishedJustKeptStream (IsoAgLib::iStream_c* rpc_lastKeptStream);


protected:

private:
  friend class SINGLETON_DERIVED(MultiReceive_c,__IsoAgLib::ElementBase_c);

  /**
    initialize directly after the singleton instance is created.
    this is called from singleton.h and should NOT be called from the user again.
    users please use init(...) instead.
  */
  void singletonInit();

  /** temp data where received data is put */
  __IsoAgLib::CANPkgExt_c c_data;

  //  Operation: getStream
  //! Parameter:
  //! @param rc_streamIdent:
  //! @return NULL for "doesn't exist", otherwise valid "Stream_c*"
  Stream_c* getStream(uint8_t sa, uint8_t da
  #ifdef NMEA_2000_FAST_PACKET
  , bool rb_fastPacket
  #endif
  );

  bool anyMultiReceiveClientRegisteredForThisDa (uint8_t ui8_da);

  //  Operation: getClient
  //! Parameter:
  //! @param rc_streamIdent:
  //! @return NULL for "doesn't exist", otherwise valid "MultiReceiveClient_c*"
  IsoAgLib::MultiReceiveClient_c* getClient(IsoAgLib::ReceiveStreamIdentifier_c rc_streamIdent);

  //  Operation: sendCurrentCts
  //! Parameter:
  //! @param rpc_stream:
  void sendCurrentCts(DEF_Stream_c_IMPL* rpc_stream);

  //  Operation: sendEndOfMessageAck
  //! Parameter:
  //! @param rpc_stream:
  void sendEndOfMessageAck(DEF_Stream_c_IMPL* rpc_stream);


  bool processStreamDataChunk_ofMatchingClient (Stream_c* rpc_stream, bool b_lastChunk);


  void sendConnAbort(StreamType_t rt_streamType, IsoAgLib::ReceiveStreamIdentifier_c rc_streamIdent);
  void connAbortTellClient(bool rb_sendConnAbort, Stream_c* rpc_stream);
  void connAbortTellClientRemoveStream(bool rb_sendConnAbort, Stream_c* rpc_stream);
  void removeStream(Stream_c* rpc_stream);


  void notifyError (IsoAgLib::ReceiveStreamIdentifier_c& rc_streamIdent, uint8_t rui8_multiReceiveErrorCode);


private: // attributes

  std::list<DEF_Stream_c_IMPL> list_streams;

  std::list<MultiReceiveClientWrapper_s> list_clients;

}; // ~X2C

#if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
  /** C-style function, to get access to the unique MultiReceive_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  MultiReceive_c& getMultiReceiveInstance( uint8_t rui8_instance = 0 );
#else
  /** C-style function, to get access to the unique MultiReceive_c singleton instance */
  MultiReceive_c& getMultiReceiveInstance( void );
#endif

} // end namespace __IsoAgLib
#endif // -X2C
