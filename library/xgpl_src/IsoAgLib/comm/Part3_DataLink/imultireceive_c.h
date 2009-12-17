/*
  imultireceive_c.h: reception of multi message data

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef IMULTI_RECEIVE_H
#define IMULTI_RECEIVE_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include "impl/multireceive_c.h"
#include <IsoAgLib/driver/can/icancustomer_c.h>
#include <IsoAgLib/scheduler/ischedulertask_c.h>


// Begin Namespace IsoAgLib
namespace IsoAgLib {
/**
  This class implements the various multi message data
  transfer specifications of Fieldstar, LBS+ and ISO 11783
  for reception of more than 8 bytes of data.

  @author Dipl.-Inf. Martin Wodok
*/
class iMultiReceive_c : private __IsoAgLib::MultiReceive_c  {
public:

  //  Operation: init
  void init()
    { MultiReceive_c::init(); }

  /** every subsystem of IsoAgLib has explicit function for controlled shutdown */
  void close()
    { MultiReceive_c::close(); }

  //  Operation: (de)registerClient
  void registerClient   (IsoAgLib::iCanCustomer_c& arc_client, const iIsoName_c& acrc_isoName,
                         uint32_t aui32_pgn, uint32_t aui32_pgnMask=0x3FFFF,
                         bool ab_alsoBroadcast=false, bool ab_alsoGlobalErrors=false
                         #ifdef ENABLE_MULTIPACKET_VARIANT_FAST_PACKET
                         , bool ab_isFastPacket=false
                         #endif
                         )
    { MultiReceive_c::registerClient (arc_client, acrc_isoName, aui32_pgn, aui32_pgnMask, ab_alsoBroadcast, ab_alsoGlobalErrors
                         #ifdef ENABLE_MULTIPACKET_VARIANT_FAST_PACKET
                         , ab_isFastPacket
                         #endif
                         ); }

 void registerClient   (IsoAgLib::iSchedulerTask_c& arc_client, const iIsoName_c& acrc_isoName,
                         uint32_t aui32_pgn, uint32_t aui32_pgnMask=0x3FFFF,
                         bool ab_alsoBroadcast=false, bool ab_alsoGlobalErrors=false
                         #ifdef ENABLE_MULTIPACKET_VARIANT_FAST_PACKET
                         , bool ab_isFastPacket=false
                         #endif
                         )
    { MultiReceive_c::registerClient (arc_client, acrc_isoName, aui32_pgn, aui32_pgnMask, ab_alsoBroadcast, ab_alsoGlobalErrors
                         #ifdef ENABLE_MULTIPACKET_VARIANT_FAST_PACKET
                         , ab_isFastPacket
                         #endif
                         ); }

  void deregisterClient (IsoAgLib::iCanCustomer_c* apc_client)
    { MultiReceive_c::deregisterClient (*static_cast<__IsoAgLib::CanCustomer_c*>(apc_client)); }


  void deregisterClient (IsoAgLib::iSchedulerTask_c* apc_client)
    { MultiReceive_c::deregisterClient (*static_cast<__IsoAgLib::CanCustomer_c*>(apc_client)); }

  /// Use to remove a "kept"-stream after it is gotten by "getFinishedJustKeptStream" and processed.
  void removeKeptStream (IsoAgLib::iStream_c* apc_keptStream)
    { MultiReceive_c::removeKeptStream (apc_keptStream); }

  //! return 0x00-0xFF: first byte of the stream!
  //! return 0x100: first byte not yet known!
  //! return 0x101: not a valid index!
  uint16_t getStreamFirstByte (uint32_t aui32_index) const
    { return MultiReceive_c::getStreamFirstByte (aui32_index); }

  uint32_t getStreamCount() const
    { return MultiReceive_c::getStreamCount(); }

  uint32_t getStreamCompletion1000 (uint32_t aui32_index, bool ab_checkFirstByte=false, uint8_t aui8_returnNullIfThisIsFirstByte=0x00 /*don't care if check=false*/) const
    { return MultiReceive_c::getStreamCompletion1000 (aui32_index, ab_checkFirstByte, aui8_returnNullIfThisIsFirstByte); }

  uint32_t getMaxStreamCompletion1000 (bool ab_checkFirstByte=false, uint8_t aui8_returnNullIfThisIsFirstByte=0x00 /*don't care if check=false*/) const
    { return MultiReceive_c::getMaxStreamCompletion1000 (ab_checkFirstByte, aui8_returnNullIfThisIsFirstByte); }

  bool isAtLeastOneWithFirstByte (uint8_t aui8_firstByte)
    { return MultiReceive_c::isAtLeastOneWithFirstByte (aui8_firstByte); }

  IsoAgLib::iStream_c* getFinishedJustKeptStream (IsoAgLib::iStream_c* apc_lastKeptStream)
    { return MultiReceive_c::getFinishedJustKeptStream (apc_lastKeptStream); }

  //! Override the default configured values CONFIG_MULTI_RECEIVE_CTS_DELAY_AT_SINGLE_STREAM and CONFIG_MULTI_RECEIVE_CTS_DELAY_AT_MULTI_STREAMS
  //! This function is "thread safe", it only overwrites member variables.
  //! Use only if you know what you're doing!
  //! @param ai32_ctsSendDelayOneStream time to wait (in ms) before sending out the next CTS [if only one session is running]
  //! @param ai32_ctsSendDelayMoreStreams time to wait (in ms) before sending out the next CTS [if multiple sessions running concurrently]
  void setCtsDelays (int32_t ai32_ctsSendDelayOneStream, int32_t ai32_ctsSendDelayMoreStreams)
    { MultiReceive_c::setCtsDelays (ai32_ctsSendDelayOneStream, ai32_ctsSendDelayMoreStreams); }


  private:
  /** allow getIMultiReceiveInstance() access to shielded base class.
      otherwise __IsoAgLib::getMultiReceiveInstance() wouldn't be accepted by compiler
    */
  #if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
  friend iMultiReceive_c& getIMultiReceiveInstance( uint8_t aui8_instance );
  #else
  friend iMultiReceive_c& getIMultiReceiveInstance( void );
  #endif
};
#if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
  /** C-style function, to get access to the unique MultiReceive_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  inline iMultiReceive_c& getIMultiReceiveInstance( uint8_t aui8_instance = 0 )
  { return static_cast<iMultiReceive_c&>(__IsoAgLib::getMultiReceiveInstance(aui8_instance));};
#else
  /** C-style function, to get access to the unique MultiReceive_c singleton instance */
  inline iMultiReceive_c& getIMultiReceiveInstance( void )
  { return static_cast<iMultiReceive_c&>(__IsoAgLib::getMultiReceiveInstance());};
#endif

}
#endif
