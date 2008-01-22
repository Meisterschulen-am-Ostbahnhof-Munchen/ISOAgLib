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
}


// IsoAgLib
#include <IsoAgLib/typedef.h>
#include <IsoAgLib/util/impl/singleton.h>
#include <IsoAgLib/comm/Scheduler/impl/schedulertask_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isoname_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isomonitor_c.h>

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
                               const IsoName_c& arc_isoNameClient,
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
  void registerClient   (CanCustomer_c& arc_client, const IsoName_c& arc_isoName,
                         uint32_t aui32_pgn, uint32_t aui32_pgnMask=0x3FFFF,
                         bool mb_alsoBroadcast=false, bool ab_alsoGlobalErrors=false
                         #ifdef ENABLE_MULTIPACKET_VARIANT_FAST_PACKET
                         , bool ab_isFastPacket=false
                         #endif
                         );

  void deregisterClient (CanCustomer_c& apc_client);
  void deregisterClient (CanCustomer_c& arc_client, const IsoName_c& arc_isoName, uint32_t aui32_pgn, uint32_t aui32_pgnMask);

  //  Operation: createStream
  //! 
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

  /** this function is called by IsoMonitor_c on addition, state-change and removal of an IsoItem.
   * @param at_action enumeration indicating what happened to this IsoItem. @see IsoItemModification_en / IsoItemModification_t
   * @param arcc_isoItem reference to the (const) IsoItem which is changed (by existance or state)
   */
  void reactOnIsoItemModification (IsoItemModification_t /*at_action*/, IsoItem_c const& /*arcc_isoItem*/);

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
  //! @param apc_stream:
  void sendEndOfMessageAck(DEF_Stream_c_IMPL* apc_stream);


  bool processStreamDataChunk_ofMatchingClient (Stream_c& arc_stream, bool b_lastChunk);


  void sendConnAbort(StreamType_t at_streamType, IsoAgLib::ReceiveStreamIdentifier_c ac_streamIdent);
  void connAbortTellClient(bool ab_sendConnAbort, Stream_c* apc_stream);
  void connAbortTellClientRemoveStream(bool ab_sendConnAbort, Stream_c* apc_stream);
  void removeStream(Stream_c* apc_stream);


  void notifyError (const IsoAgLib::ReceiveStreamIdentifier_c& ac_streamIdent, uint8_t aui8_multiReceiveErrorCode);


private: // attributes

  STL_NAMESPACE::list<DEF_Stream_c_IMPL> mlist_streams;

  STL_NAMESPACE::list<MultiReceiveClientWrapper_s> mlist_clients;

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
