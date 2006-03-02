/***************************************************************************
                          isoterminal_c.cpp - central ISO terminal management
                             -------------------
    begin                : Wed Jul 09 2003
    copyright            : (C) 2003 by Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
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

// LOESCHE_POOL will send DeleteVersion INSTEAD of LoadVersion
//#define LOESCHE_POOL
//#define GET_VERSIONS
#define USE_SIMPLE_AUX_RESPONSE


#include "isoterminal_c.h"


#include <IsoAgLib/comm/Scheduler/impl/scheduler_c.h>
#include <IsoAgLib/driver/can/impl/canio_c.h>
#include <IsoAgLib/hal/system.h>
#include <IsoAgLib/comm/SystemMgmt/impl/systemmgmt_c.h>
#include <IsoAgLib/comm/Multipacket/impl/multireceive_c.h>
#include <supplementary_driver/driver/datastreams/volatilememory_c.h>

#include "vttypes.h"
#include "../ivtobjectpicturegraphic_c.h"
#include "../ivtobjectstring_c.h"

#if defined(DEBUG) || defined(DEBUG_HEAP_USEAGE)
  #include <supplementary_driver/driver/rs232/impl/rs232io_c.h>
  #include <IsoAgLib/util/impl/util_funcs.h>
  #ifdef SYSTEM_PC
    #include <iostream>
  #else
    #include <supplementary_driver/driver/rs232/impl/rs232io_c.h>
  #endif
#endif

#include <IsoAgLib/comm/Multipacket/impl/multisendpkg_c.h>

#ifdef DEBUG_HEAP_USEAGE
  static uint16_t sui16_lastPrintedBufferCapacity = 0;
  static uint16_t sui16_lastPrintedSendUploadQueueSize = 0;
  static uint16_t sui16_lastPrintedMaxSendUploadQueueSize = 0;
  static uint16_t sui16_sendUploadQueueSize = 0;
  static uint16_t sui16_maxSendUploadQueueSize = 0;
#endif

static const uint8_t scpui8_cmdCompareTable[(0xB4-0x92)+1] = {
/// (1<<0) means DO NOT OVERRIDE THESE COMMANDS AT ALL
/* 0x92 */ (1<<0) , //NEVER OVERRIDE THIS COMMAND
/* 0x93 */ 0 , //invalid command
/* 0x94 */ 0 , //invalid command
/* 0x95 */ 0 , //invalid command
/* 0x96 */ 0 , //invalid command
/* 0x97 */ 0 , //invalid command
/* 0x98 */ 0 , //invalid command
/* 0x99 */ 0 , //invalid command
/* 0x9A */ 0 , //invalid command
/* 0x9B */ 0 , //invalid command
/* 0x9C */ 0 , //invalid command
/* 0x9D */ 0 , //invalid command
/* 0x9E */ 0 , //invalid command
/* 0x9F */ 0 , //invalid command
/* 0xA0 */ (1<<1) | (1<<2) ,
/* 0xA1 */ (1<<1) | (1<<2) ,
/* 0xA2 */ (1<<1) | (1<<2) ,
/* 0xA3 */ (1<<0) , //NEVER OVERRIDE THIS COMMAND (Control Audio Device)
/* 0xA4 */ (1<<0) , //NEVER OVERRIDE THIS COMMAND (Set Audio Volume)
/* 0xA5 */ (1<<0) , //NEVER OVERRIDE THIS COMMAND (Change Child Location), as it's relative!!!
/* 0xA6 */ (1<<1) | (1<<2) ,
/* 0xA7 */ (1<<1) | (1<<2) ,
/* 0xA8 */ (1<<1) | (1<<2) , // Change Numeric Value (all has been done for THIS ONE originally ;-)
/* 0xA9 */ (1<<1) | (1<<2) ,
/* 0xAA */ (1<<1) | (1<<2) ,
/* 0xAB */ (1<<1) | (1<<2) ,
/* 0xAC */ (1<<1) | (1<<2) ,
/* 0xAD */ (1<<1) | (1<<2) , /** @todo Maybe change to delete in between and push_back new?! */ // (Change Active Mask)
/* 0xAE */ (1<<1) | (1<<2) | (1<<3) ,
/* 0xAF */ (1<<1) | (1<<2) | (1<<3) ,
/* 0xB0 */ (1<<1) | (1<<2) ,
/* 0xB1 */ (1<<1) | (1<<2) | (1<<3) ,
/* 0xB2 */ (1<<0) , //NEVER OVERRIDE THIS COMMAND (Delete Object Pool)
/* 0xB3 */ (1<<1) | (1<<2) ,
/* 0xB4 */ (1<<1) | (1<<2) | (1<<3) | (1<<4)  // (Change Child Position)
};


namespace __IsoAgLib {
#if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
  /** C-style function, to get access to the unique ISOTerminal_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  ISOTerminal_c& getIsoTerminalInstance( uint8_t rui8_instance )
  { // if > 1 singleton instance is used, no static reference can be used
    return ISOTerminal_c::instance( rui8_instance );
  };
#else
  /** C-style function, to get access to the unique ISOTerminal_c singleton instance */
  ISOTerminal_c& getIsoTerminalInstance( void )
  {
    static ISOTerminal_c& c_isoTerminal = ISOTerminal_c::instance();
    return c_isoTerminal;
  };
#endif

#define DEF_TimeOut_NormalCommand 1500       /* 1,5 seconds are stated in F.1 (page 96) */
#define DEF_TimeOut_EndOfObjectPool 60000
#define DEF_TimeOut_StoreVersion 60000
#define DEF_TimeOut_LoadVersion 60000
#define DEF_WaitFor_Reupload 5000
#define DEF_Retries_NormalCommands 2
#define DEF_Retries_TPCommands 2
#define DEF_TimeOut_ChangeStringValue 1500   /* 1,5 seconds are stated in F.1 (page 96) */



SendUpload_c::SendUpload_c (vtObjectString_c* rpc_objectString)
{
  mssObjectString = rpc_objectString;

  if (mssObjectString->getStreamer()->getStreamSize() < 9)
    ui8_retryCount = DEF_Retries_NormalCommands;
  else
    ui8_retryCount = DEF_Retries_TPCommands;

  ui32_uploadTimeout = DEF_TimeOut_ChangeStringValue;
}


  /*************************************/
 /**** Iso Terminal Implementation ****/
/*************************************/

void ISOTerminal_c::finishUploadCommand ()
{
  en_uploadType = UploadIdle;
  //dumpQueue(); /* to see all left queued cmds after every dequeued cmd */
  #ifdef DEBUG
  INTERNAL_DEBUG_DEVICE << "Dequeued (after success, timeout, whatever..): " << q_sendUpload.size() <<" -> ";
  #endif

  #ifdef USE_LIST_FOR_FIFO
  // queueing with list: queue::push <-> list::push_back; queue::front<->list::front; queue::pop<->list::pop_front
  q_sendUpload.pop_front();
  #else
  q_sendUpload.pop();
  #endif
  #ifdef DEBUG_HEAP_USEAGE
  sui16_sendUploadQueueSize--;
  #endif

  #ifdef DEBUG
  INTERNAL_DEBUG_DEVICE << q_sendUpload.size() << ".\n";
  #endif
}


// only being called if there IS a q_sendUpload.front()
bool ISOTerminal_c::startUploadCommand ()
{
  // Set new state
  en_uploadType = UploadCommand;
  en_uploadCommandState = UploadCommandWaitingForCommandResponse;

  // Get first element from queue
  SendUpload_c* actSend = &q_sendUpload.front();

  // Set time-out values
  ui32_uploadTimeout = actSend->ui32_uploadTimeout;
  ui32_uploadTimestamp = HAL::getTime();


   /// Use Multi or Single CAN-Pkgs?
  //////////////////////////////////

  if ((actSend->mssObjectString == NULL) && (actSend->vec_uploadBuffer.size() < 9)) {
    /// Fits into a single CAN-Pkg!
    // Shouldn't be less than 8, else we're messin around with vec_uploadBuffer!
    c_data.setExtCanPkg8 (7, 0, ECU_TO_VT_PGN>>8, vtSourceAddress, pc_wsMasterIdentItem->getIsoItem()->nr(),
                          actSend->vec_uploadBuffer [0], actSend->vec_uploadBuffer [1],
                          actSend->vec_uploadBuffer [2], actSend->vec_uploadBuffer [3],
                          actSend->vec_uploadBuffer [4], actSend->vec_uploadBuffer [5],
                          actSend->vec_uploadBuffer [6], actSend->vec_uploadBuffer [7]);
    getCanInstance4Comm() << c_data;

    // Save first byte for Response-Checking!
    ui8_commandParameter = actSend->vec_uploadBuffer [0];
  }
  else if ((actSend->mssObjectString != NULL) && (actSend->mssObjectString->getStreamer()->getStreamSize() < 9)) {
    /// Fits into a single CAN-Pkg!
    uint8_t ui8_len = actSend->mssObjectString->getStreamer()->getStreamSize();

    c_data.setExtCanPkg (7, 0, ECU_TO_VT_PGN>>8, vtSourceAddress, pc_wsMasterIdentItem->getIsoItem()->nr(), 8); // ECU->VT PGN is ALWAYS 8 Bytes!
    actSend->mssObjectString->getStreamer()->set5ByteCommandHeader (c_data.pb_data);
    int i=5;
    for (; i < ui8_len; i++) c_data.pb_data[i] = actSend->mssObjectString->getStreamer()->getStringToStream() [i-5];
    for (; i < 8;       i++) c_data.pb_data[i] = 0xFF; // pad unused bytes with "0xFF", so CAN-Pkg is of size 8!
    getCanInstance4Comm() << c_data;

    // Save first byte for Response-Checking!
    ui8_commandParameter = actSend->mssObjectString->getStreamer()->getFirstByte();
  }
  else if (actSend->mssObjectString == NULL) {
    /// Use multi CAN-Pkgs [(E)TP], doesn't fit into a single CAN-Pkg!

    // Save first byte for Response-Checking!
    ui8_commandParameter = actSend->vec_uploadBuffer [0]; // Save first byte for Response-Checking!

    return getMultiSendInstance4Comm().sendIsoTarget(pc_wsMasterIdentItem->getIsoItem()->nr(), vtSourceAddress,
           &actSend->vec_uploadBuffer.front(), actSend->vec_uploadBuffer.size(), ECU_TO_VT_PGN, en_sendSuccess);
  } else {
    // Save first byte for Response-Checking!
    ui8_commandParameter = actSend->mssObjectString->getStreamer()->getFirstByte();

    return getMultiSendInstance4Comm().sendIsoTarget(pc_wsMasterIdentItem->getIsoItem()->nr(), vtSourceAddress,
           (MultiSendStreamer_c*)actSend->mssObjectString->getStreamer(),        ECU_TO_VT_PGN, en_sendSuccess);
  }
  return true;
}


/**
  default constructor, which can optional set the pointer to the containing
  Scheduler_c object instance
*/
ISOTerminal_c::ISOTerminal_c()
{
  /// all variable initialization moved to singletonInit!
}


/**
  default destructor, which initiate sending address release for all own identities
  @see DINMonitor_c::~DINMonitor_c
*/
ISOTerminal_c::~ISOTerminal_c(){
  close();
}



void
ISOTerminal_c::singletonInit()
{
  /// Constructor inits
  en_uploadType = UploadIdle;
  localSettings_a.lastReceived = 0; // set this here for safety reasons, as Process_c will read the language from us if "lastReceived != 0"

  /// real init() code...
  // clear state of b_alreadyClosed, so that close() is called one time
  clearAlreadyClosed();
  // register in Scheduler_c to get time-events
  getSchedulerInstance4Comm().registerClient( this );

  const uint32_t cui32_filterV = (static_cast<MASK_TYPE>(VT_TO_GLOBAL_PGN) << 8);
  if (!getCanInstance4Comm().existFilter( *this, (0x1FFFF00UL), cui32_filterV, Ident_c::ExtendedIdent))
      getCanInstance4Comm().insertFilter( *this, (0x1FFFF00UL), cui32_filterV, true, Ident_c::ExtendedIdent);

  const uint32_t cui32_filterL = (static_cast<MASK_TYPE>(LANGUAGE_PGN) << 8);
  if (!getCanInstance4Comm().existFilter( *this, (0x1FFFF00UL), cui32_filterL, Ident_c::ExtendedIdent))
      getCanInstance4Comm().insertFilter( *this, (0x1FFFF00UL), cui32_filterL, true, Ident_c::ExtendedIdent);

  localSettings_a.lastReceived = 0; // no language info received yet
  vtState_a.lastReceived = 0; // no vt_statusMessage received yet
  vtSourceAddress = 254; // shouldn't be read anyway before vt_statusMessage arrived....

  pc_wsMasterIdentItem = NULL;
  c_streamer.pc_pool = NULL;
  pc_versionLabel = NULL;

  en_objectPoolState = OPNoneRegistered;
  en_uploadType = UploadIdle;
  b_receiveFilterCreated = false;

  b_vtAliveCurrent = false; // so we detect the rising edge when the VT gets connected!

  b_checkSameCommand = true;

  ui32_filterAckPGN = 0; // filter not yet inserted!

  //i32_nextWsMaintenanceMsg; needn't be set here, will be set in doStart!
}

/**
  Register the given object pool
  It will automatically be uploaded as soon as ISO_Terminal_c is connected to the VT
  and all initialization stuff has been done (Get VT Capabilities, Memory, etc.)
*/
bool ISOTerminal_c::registerIsoObjectPool (IdentItem_c* rpc_wsMasterIdentItem, IsoAgLib::iIsoTerminalObjectPool_c* rpc_pool, char* rpc_versionLabel)
{
  // No support for double initialization or ObjectPool change (yet)!
  if (en_objectPoolState != OPNoneRegistered) return false;

  if (!((en_uploadType == UploadIdle) || ((en_uploadType == UploadPool) && (en_uploadPoolState == UploadPoolFailed)) )) {
    // could not start bacause uploader is busy, so we can't set the variables, but this shouldn't really
    // happen, as the pool should be registered right after initializing the terminal...

    getLbsErrInstance().registerError( LibErr_c::LbsMultiSendBusy, LibErr_c::IsoTerminal );
    return false;
  }

  pc_wsMasterIdentItem = rpc_wsMasterIdentItem;
  c_streamer.pc_pool = rpc_pool;
  // the generated initAllObjectsOnce() has to ensure to be idempotent! (vt2iso-generated source does this!)
  c_streamer.pc_pool->initAllObjectsOnce(SINGLETON_VEC_KEY_PARAMETER_VAR);

  if (rpc_versionLabel != NULL) {
    pc_versionLabel = new (char [7+1]);
    uint32_t len = CNAMESPACE::strlen(rpc_versionLabel);
    if (len > 7) len=7;
    unsigned int i=0;
    for (; i<len; i++) pc_versionLabel [i] = rpc_versionLabel [i];
    for (; i<7; i++) pc_versionLabel [i] = 0x20; // ASCII: Space
    pc_versionLabel[i] = 0x00; // terminate with a 0x00 although not needed...
  }

  en_objectPoolState = OPRegistered; // try to upload until state == UploadedSuccessfully || CannotBeUploaded
  en_uploadType = UploadPool;          // Start Pool Uploading sequence!!
  en_uploadPoolState = UploadPoolInit; // with "UploadInit
  return true;
}


/**
  De-Register the registered object pool and versionLabel string (if one was copied)
*/
bool ISOTerminal_c::deregisterIsoObjectPool ()
{
  // If none registered deregistering is not possible
  if (en_objectPoolState == OPNoneRegistered) return false;

  // Stop all activities!
  /** @todo: has anything to be done here? can there still be activities running that would interfere when pc_poll == NULL then?
   * I think all has been done now to prevent any problems, so it should do! ... */

  // Deregister Pool and Name, IdentItem stays...
  c_streamer.pc_pool = NULL;
  en_objectPoolState = OPNoneRegistered;
  /** @todo finish Upload-stuff: a) in case of Upload Pool and b) in case of Upload Command */
  en_uploadType = UploadIdle;
  if (pc_versionLabel != NULL) {
    delete (pc_versionLabel);
    pc_versionLabel = NULL;
  }
  return true;
}


/**
  initialise element which can't be done during construct

  possible errors:
*/
bool ISOTerminal_c::init(IdentItem_c* rpc_wsMasterIdentItem, IsoAgLib::iIsoTerminalObjectPool_c* rpc_pool, char* rpc_versionLabel)
{
  return registerIsoObjectPool (rpc_wsMasterIdentItem, rpc_pool, rpc_versionLabel);
}


/** every subsystem of IsoAgLib has explicit function for controlled shutdown
  */
void ISOTerminal_c::close( void )
{
  if ( ! checkAlreadyClosed() ) {
    // avoid another call
    setAlreadyClosed();
    getSchedulerInstance4Comm().unregisterClient( this );

    uint32_t ui32_filter = (static_cast<MASK_TYPE>(VT_TO_GLOBAL_PGN) << 8);
    if (getCanInstance4Comm().existFilter(*this, (0x1FFFF00UL), ui32_filter, Ident_c::ExtendedIdent))
       getCanInstance4Comm().deleteFilter(*this, (0x1FFFF00UL), ui32_filter, Ident_c::ExtendedIdent);

    ui32_filter = (static_cast<MASK_TYPE>(LANGUAGE_PGN) << 8);
    if (getCanInstance4Comm().existFilter(*this, (0x1FFFF00UL), ui32_filter, Ident_c::ExtendedIdent))
       getCanInstance4Comm().deleteFilter(*this, (0x1FFFF00UL), ui32_filter, Ident_c::ExtendedIdent);

    /** @todo will the IsoItem get lost or get its SA changed? */
    if (ui32_filterAckPGN != 0)
    { // do only if filter could be calculated (IsoItem was needed for that) and was actually inserted!
      if (getCanInstance4Comm().existFilter(*this, (0x1FFFF00UL), ui32_filter, Ident_c::ExtendedIdent))
        getCanInstance4Comm().deleteFilter(*this, (0x1FFFF00UL), ui32_filter, Ident_c::ExtendedIdent);
    }
    /*** MultiReceive De-Registration ***/
    __IsoAgLib::getMultiReceiveInstance4Comm().deregisterClient(this);

    deregisterIsoObjectPool();
  }
}


void ISOTerminal_c::doStart ()
{
  /// First, trigger sending of WS-Announce
  pc_wsMasterIdentItem->getIsoItem()->triggerWsAnnounce();
  /** @todo /\ maybe wait 1 sec after ws-announcing? but it shouldn't matter. better send right now... */
  i32_nextWsMaintenanceMsg = 0; // send out ws maintenance message immediately after ws has been announced.
  /// Second, init all variables to an initial upload state (Upload will not start before ws-announcing is due
  vtCapabilities_a.lastReceivedSoftkeys = 0; // not yet (queried and) got answer about vt's capabilities yet
  vtCapabilities_a.lastRequestedSoftkeys = 0; // not yet requested vt's capabilities yet
  vtCapabilities_a.lastReceivedHardware = 0; // not yet (queried and) got answer about vt's capabilities yet
  vtCapabilities_a.lastRequestedHardware = 0; // not yet requested vt's capabilities yet
  vtCapabilities_a.lastReceivedFont = 0; // not yet (queried and) got answer about vt's capabilities yet
  vtCapabilities_a.lastRequestedFont = 0; // not yet requested vt's capabilities yet
  localSettings_a.lastRequested = 0;
  localSettings_a.lastReceived = 0;
  if (en_objectPoolState == OPNoneRegistered)
  { // no object pool registered, don't go into UploadPool state
    en_uploadType = UploadIdle;
  } else
  { // "OP<was>Registered" || "OP<was>UploadedSuccessfully" || "OPCannotBeUploaded" (on the previous VT: try probably other VT now)
    en_objectPoolState = OPRegistered; // try (re-)uploading, not caring if it was successfully or not on the last vt!

    en_uploadType = UploadPool;          // Start Pool Uploading sequence!!
    en_uploadPoolState = UploadPoolInit; // with "UploadInit
  }
}


void ISOTerminal_c::doStop()
{
  vtSourceAddress = 254;
  // VT has left the system - clear all queues now, don't wait until next re-entering (for memory reasons)
  #ifdef DEBUG_HEAP_USEAGE
  sui16_sendUploadQueueSize -= q_sendUpload.size();
  #endif
  #ifdef USE_LIST_FOR_FIFO
  // queueing with list: queue::push <-> list::push_back; queue::front<->list::front; queue::pop<->list::pop_front
  q_sendUpload.clear();
  #else
  while (!q_sendUpload.empty()) q_sendUpload.pop();
  #endif
  if (c_streamer.pc_pool != NULL)
  { // notify application on lost connection so it can enter a defined safe state
    c_streamer.pc_pool->eventEnterSafeState ();
  }
}


void ISOTerminal_c::checkVtStateChange()
{
  bool b_vtAliveOld = b_vtAliveCurrent;
  b_vtAliveCurrent = isVtActive();

  // react on vt alive change "false->true"
  if (!b_vtAliveOld && b_vtAliveCurrent)
  { /// OFF --> ON  ***  VT has (re-)entered the system
    #ifdef DEBUG
    INTERNAL_DEBUG_DEVICE << "\n=========================================================================="
                          << "\n=== VT has entered the system, trying to receive all Properties now... ==="
                          << "\n=== time: " << HAL::getTime() << " ==="
                          << "\n==========================================================================\n\n";

    #endif
    doStart();
  }
  else if (b_vtAliveOld && !b_vtAliveCurrent)
  { /// ON -> OFF  ***  Connection to VT lost
    #ifdef DEBUG
    INTERNAL_DEBUG_DEVICE << "\n=============================================================================="
                          << "\n=== VT has left the system, clearing queues --> eventEnterSafeState called ==="
                          << "\n=== time: " << HAL::getTime() << " ==="
                          << "\n==============================================================================\n\n";
    #endif
    doStop();
  }
}

/**
  periodically event
  @return true -> all planned activities performed in allowed time
*/
bool ISOTerminal_c::timeEvent( void )
{
  if ( Scheduler_c::getAvailableExecTime() == 0 ) return false;
  #ifdef DEBUG_HEAP_USEAGE
  if ( ( sui16_lastPrintedSendUploadQueueSize < sui16_sendUploadQueueSize )
    || ( sui16_lastPrintedMaxSendUploadQueueSize < sui16_maxSendUploadQueueSize ) )
  { // MAX amount of sui16_sendUploadQueueSize or sui16_maxSendUploadQueueSize
    sui16_lastPrintedSendUploadQueueSize = sui16_sendUploadQueueSize;
    sui16_lastPrintedMaxSendUploadQueueSize = sui16_maxSendUploadQueueSize;
    getRs232Instance()
      << "Max: " << sui16_sendUploadQueueSize << " Items in FIFO, "
      << sui16_sendUploadQueueSize << " x SendUpload_c: Mal-Alloc: "
      << sizeSlistTWithMalloc( sizeof(SendUpload_c), sui16_sendUploadQueueSize )
      << "/" << sizeSlistTWithMalloc( sizeof(SendUpload_c), 1 )
      << ", Chunk-Alloc: "
      << sizeSlistTWithChunk( sizeof(SendUpload_c), sui16_sendUploadQueueSize )
      << "\r\n\r\n";
  }
  #endif

  // do further activities only if registered ident is initialised as ISO and already successfully address-claimed...
  if ( !pc_wsMasterIdentItem ) return true;
  if ( !pc_wsMasterIdentItem->isClaimedAddress() ) return true;

/*** Filter/MultiReceive Registration Start ***/
  if ( ! b_receiveFilterCreated )
  { // register to get VTStatus Messages
    b_receiveFilterCreated = true;

    ui32_filterAckPGN = (static_cast<MASK_TYPE>(ACKNOWLEDGEMENT_PGN) | static_cast<MASK_TYPE>(pc_wsMasterIdentItem->getIsoItem()->nr())) << 8;
    if (!getCanInstance4Comm().existFilter( *this, (0x1FFFF00UL), ui32_filterAckPGN, Ident_c::ExtendedIdent))
        getCanInstance4Comm().insertFilter( *this, (0x1FFFF00UL), ui32_filterAckPGN, true, Ident_c::ExtendedIdent);

    /*** MultiReceive Registration ***/
    __IsoAgLib::getMultiReceiveInstance4Comm().registerClient(VT_TO_ECU_PGN, pc_wsMasterIdentItem->getIsoItem()->nr(), this);
  }
/*** Filter/MultiReceive Registration End ***/

 /*** Regular start is here (the above preconditions should be satisfied if system is finally set up. ***/
/*******************************************************************************************************/
  System_c::triggerWd();

  // #######################
  // ### VT Alive checks ###
  // #######################
  checkVtStateChange();

  // ### Do nothing if there's no VT active ###
  if (!isVtActive()) return true;

  // be kind and wait until WSMaster-/Slave-Announce have finished (AddressClaim is already checked above!!), do NOTHING else before.
  if ( !pc_wsMasterIdentItem->getIsoItem()->isClaimedAndWsAnnounced() ) return true;

  // Check if WS-Maintenance is needed
  if ((i32_nextWsMaintenanceMsg <= 0) || (HAL::getTime() >= i32_nextWsMaintenanceMsg))
  { // Do periodically WS-Maintenance sending (every second)
    c_data.setExtCanPkg8 (7, 0, ECU_TO_VT_PGN>>8, vtSourceAddress, pc_wsMasterIdentItem->getIsoItem()->nr(),
                          0xFF, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff);
    getCanInstance4Comm() << c_data;     // G.2: Function: 255 / 0xFF Working Set Maintenance Message

    i32_nextWsMaintenanceMsg = HAL::getTime() + 1000;
  }


  // If our IsoItem has claimed address, immediately try to get the LANGUAGE_PGN from VT/anyone ;-) (regardless of pool-upload!)
  if ((!localSettings_a.lastReceived) && ((localSettings_a.lastRequested == 0) || ((HAL::getTime()-localSettings_a.lastRequested) > 2000)))
  { // Try every 2 seconds to get the LANGUAGE_PGN, be polite to not bombard the VT...
    /** @todo Give up somewhen?? Or retry really every 2 seconds? */
    // Get Local Settings (may not be reached, when terminal is switched on after ECU, as VT sends LNAGUAGE Info on startup!
    c_data.setExtCanPkg3 (6, 0, REQUEST_PGN_MSG_PGN>>8, vtSourceAddress, pc_wsMasterIdentItem->getIsoItem()->nr(),
                          (LANGUAGE_PGN & 0xFF), ((LANGUAGE_PGN >> 8)& 0xFF), ((LANGUAGE_PGN >> 16)& 0xFF));
    getCanInstance4Comm() << c_data;      // Command: Request_PGN
    localSettings_a.lastRequested = HAL::getTime();
  }

  if (!localSettings_a.lastReceived)
    return true; // do not proceed if LANGUAGE not yet received!


  // Do nothing if no pool is registered
  if ( (en_objectPoolState == OPNoneRegistered)
    || (en_objectPoolState == OPCannotBeUploaded) ) // if it couldn't be uploaded, only disconnecting/connecting VT helps!
    return true; /** @todo is this correctly assumed? "if it couldn't be uploaded, only disconnecting/connecting VT helps!" */


  /// Now from here on the Pool's state is: "OPRegistered" or "OPUploadedSuccessfully"
  ////////////////////////////////
  /// UPLOADING --> OBJECT-POOL<--
  if (en_uploadType == UploadPool) {
    // Do TIME-OUT Checks ALWAYS!
    if ((en_uploadPoolState == UploadPoolWaitingForLoadVersionResponse)
     || (en_uploadPoolState == UploadPoolWaitingForMemoryResponse)
     || (en_uploadPoolState == UploadPoolWaitingForEOOResponse)) {
      // timedout
      if (((uint32_t) HAL::getTime()) > (ui32_uploadTimeout + ui32_uploadTimestamp)) {
        en_uploadPoolState = UploadPoolFailed;
        ui32_uploadTimestamp = HAL::getTime();
        ui32_uploadTimeout = DEF_WaitFor_Reupload; // wait 5 secs for possible reuploading...
      }
    }
    // Do TIME-OUT Checks ALWAYS!
    if (en_uploadPoolState == UploadPoolWaitingForStoreVersionResponse) {
      // timedout
      if (((uint32_t) HAL::getTime()) > (ui32_uploadTimeout + ui32_uploadTimestamp)) {
        // we couldn't store for some reason, but don't care, finalize anyway...
        #ifdef DEBUG
          INTERNAL_DEBUG_DEVICE << "StoreVersion TimedOut!\n";
        #endif
        finalizeUploading ();
      }
    }
    // Do TIME-OUT Checks ALWAYS! - Check if we expect an (OBJECTPOOL) UPLOAD to fail/finish?
    if (en_uploadPoolState == UploadPoolUploading) {
      switch (en_sendSuccess) {
        case __IsoAgLib::MultiSend_c::Running: {
          // do nothing, still wait.
        } break;
        case __IsoAgLib::MultiSend_c::SendAborted: {
          // aborted sending
          en_uploadPoolState = UploadPoolFailed;
          ui32_uploadTimestamp = HAL::getTime();
          ui32_uploadTimeout = DEF_WaitFor_Reupload; // if it was an object pool, try reuploading in 5 secs...
        } break;
        case __IsoAgLib::MultiSend_c::SendSuccess: {
          // successfully sent, so do we now have to send out the "End of Object Pool Message"?
          indicateObjectPoolCompletion (); // Send "End of Object Pool" message
        } break;
      } // switch
    }
    // Handled down below are (as they're no TIME-OUTs but INITIATIONs:
    // if (UploadPoolFailed)
    // if (UploadPoolInit)

    // ### Do we have to request (any) vt capabilities?
    if (!(vtCapabilities_a.lastReceivedFont && vtCapabilities_a.lastReceivedHardware && vtCapabilities_a.lastReceivedSoftkeys)) {
      /// Pool-Upload: PRE Phase (Get VT-Properties)
      if (!vtCapabilities_a.lastReceivedSoftkeys && ((vtCapabilities_a.lastRequestedSoftkeys == 0) || ((HAL::getTime()-vtCapabilities_a.lastRequestedSoftkeys) > 1000))) {
        // Get Number Of Soft Keys
        c_data.setExtCanPkg8 (7, 0, ECU_TO_VT_PGN>>8, vtSourceAddress, pc_wsMasterIdentItem->getIsoItem()->nr(),
                              194, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff);
        getCanInstance4Comm() << c_data;      // Command: Get Technical Data --- Parameter: Get Text Font Data
        vtCapabilities_a.lastRequestedSoftkeys = HAL::getTime();
        #ifdef DEBUG
        INTERNAL_DEBUG_DEVICE << "Requested first property (C2)...\n";
        #endif
      }
      if (vtCapabilities_a.lastReceivedSoftkeys && (!vtCapabilities_a.lastReceivedFont) && ((vtCapabilities_a.lastRequestedFont == 0) || ((HAL::getTime()-vtCapabilities_a.lastRequestedFont) > 1000))) {
        // Get Text Font Data
        c_data.setExtCanPkg8 (7, 0, ECU_TO_VT_PGN>>8, vtSourceAddress, pc_wsMasterIdentItem->getIsoItem()->nr(),
                              195, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff);
        getCanInstance4Comm() << c_data;      // Command: Get Technical Data --- Parameter: Get Text Font Data
        vtCapabilities_a.lastRequestedFont = HAL::getTime();
      }
      if (vtCapabilities_a.lastReceivedSoftkeys && vtCapabilities_a.lastReceivedFont && (!vtCapabilities_a.lastReceivedHardware) && ((vtCapabilities_a.lastRequestedHardware == 0) || ((HAL::getTime()-vtCapabilities_a.lastRequestedHardware) > 1000))) {
        // Get Hardware
        c_data.setExtCanPkg8 (7, 0, ECU_TO_VT_PGN>>8, vtSourceAddress, pc_wsMasterIdentItem->getIsoItem()->nr(),
                              199, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff);
        getCanInstance4Comm() << c_data;      // Command: Get Technical Data --- Parameter: Get Hardware
        vtCapabilities_a.lastRequestedHardware = HAL::getTime();
      }
    }
    else // if (vtCapabilities_a.lastReceivedFont && vtCapabilities_a.lastReceivedHardware && vtCapabilities_a.lastReceivedSoftkeys && localSettings_a.lastReceived)
    {
      /// Handle other than Response states now here: INITIALIZING-States!
      // - if (UploadPoolFailed)
      // - if (UploadPoolInit)
      /// Pool-Upload: MAIN Phase (Try2Load / Upload / Try2Save)
      // ### Do MAIN-Phase a) at INIT and b) <timeout> seconds after FAIL
      if ( ((en_uploadPoolState == UploadPoolFailed) && (((uint32_t) HAL::getTime()) > (ui32_uploadTimeout + ui32_uploadTimestamp)))
         || (en_uploadPoolState == UploadPoolInit) ) {
        // Do we want to try to "Load Version" or go directly to uploading?
// Added this preprocessor so loading of object pools can be prevented for development purposes
#ifdef NO_LOAD_VERSION
#else
        if (pc_versionLabel != NULL) {

          #ifdef GET_VERSIONS
            static int b_getVersionsSendTime=0;
            if (b_getVersionsSendTime == 0)
            { // send out get versions first
              c_data.setExtCanPkg8 (7, 0, ECU_TO_VT_PGN>>8, vtSourceAddress, pc_wsMasterIdentItem->getIsoItem()->nr(),
                                    223, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF);
              getCanInstance4Comm() << c_data;     // Command: Non Volatile Memory --- Parameter: Load Version
              b_getVersionsSendTime = HAL::getTime();
            }
            if ((b_getVersionsSendTime+500) > HAL::getTime())
            { // wait for answer first
              return true;
            }
          #endif


          // Try to "Non Volatile Memory - Load Version" first!
          c_data.setExtCanPkg8 (7, 0, ECU_TO_VT_PGN>>8, vtSourceAddress, pc_wsMasterIdentItem->getIsoItem()->nr(),
                               #ifdef LOESCHE_POOL
                               210, pc_versionLabel [0], pc_versionLabel [1], pc_versionLabel [2], pc_versionLabel [3], pc_versionLabel [4], pc_versionLabel [5], pc_versionLabel [6]);
                               #else
                               209, pc_versionLabel [0], pc_versionLabel [1], pc_versionLabel [2], pc_versionLabel [3], pc_versionLabel [4], pc_versionLabel [5], pc_versionLabel [6]);
                               #endif
          getCanInstance4Comm() << c_data;     // Command: Non Volatile Memory --- Parameter: Load Version
                                               //(Command: Non Volatile Memory --- Parameter: Delete Version - just a quick hack!)
        #ifdef LOESCHE_POOL
          startObjectPoolUploading ();
        #else
          // start uploading after reception of LoadVersion Response
          en_uploadPoolState = UploadPoolWaitingForLoadVersionResponse;
          ui32_uploadTimeout = DEF_TimeOut_LoadVersion;
          ui32_uploadTimestamp = HAL::getTime();
          #ifdef DEBUG
          INTERNAL_DEBUG_DEVICE << "Trying Load Version (D1)...\n";
          #endif
        #endif
        }
        else
#endif
        // NO_LOAD_VERSION
        { // Start uploading right now, no "LoadVersion" first
          startObjectPoolUploading ();
        }
      }
    }
  }

  /////////////////////////////
  /// UPLOADING --> COMMAND <--
  // Can only be done if the Object-Pool is successfully uploaded!
  if (en_objectPoolState != OPUploadedSuccessfully)
    return true;

  /// FROM HERE ON THE OBJECT-POOL >>IS<< UPLOADED SUCCESSFULLY
  /// NOW HERE THE RUNTIME COMMANDS ARE BEING HANDLED


  if (en_uploadType == UploadCommand) {
    // NO Response/timeOut for (C.2.3 Object Pool Transfer Message) "UploadObjectPool" - Only for "UploadMultiPaketCommand"
    if (en_uploadCommandState == UploadCommandWaitingForCommandResponse) {
      // Waiting for an answer - Did it time out?
      if (((uint32_t) HAL::getTime()) > (ui32_uploadTimeout + ui32_uploadTimestamp)) {
        /** @todo check if it was a MultiSend and if it's still running, then give little more waiting! or "connAbort" multisend! */
        en_uploadCommandState = UploadCommandTimedOut;
      }
    }
    // no ELSE here as the uploadCommandState may has changed above!
    /** @todo Check if "ConnAbort" comes in, in that case? retry?!! */

    // Are we in "Upload Command"-State and the last Upload failed?
    if ( (en_uploadCommandState == UploadCommandTimedOut) /* OBSOLETE: || (en_uploadCommandState == UploadCommandFailed) */) {
      if (ui8_uploadRetry > 0) {
        ui8_uploadRetry--;
        startUploadCommand();
      } else {
        // No more retries, simply finish this job and go Idle!
        finishUploadCommand(); // will pop the SendUpload, as it can't be correctly sent after <retry> times. too bad.
      }
    }
  } // UploadCommand

  // ### Is a) no Upload running and b) some Upload to do?
  if ((en_uploadType == UploadIdle) && !q_sendUpload.empty()) {
    // Set Retry & Start Uploading
    ui8_uploadRetry = (*(q_sendUpload.begin())).ui8_retryCount;
    startUploadCommand ();
  }

  return true;
}


// handle all string value between length of 9 and 259 bytes
bool ISOTerminal_c::reactOnStreamStart(IsoAgLib::ReceiveStreamIdentifier_c rc_ident, uint32_t rui32_totalLen)
{
  // if SA is not the address from the vt -> don't react on stream
  if (rc_ident.getSa()!=getVtSourceAddress ()) return false;
  //handling string value >= 9 Bytes
  if (rui32_totalLen > (4 /* H.18 byte 1-4 */ + 255 /* max string length */))
    return false;
  return true;
}

void ISOTerminal_c::reactOnAbort(IsoAgLib::iStream_c* /*rpc_stream*/)
{
  if (c_streamer.pc_pool != NULL) {
    c_streamer.pc_pool->eventStringValueAbort();
  }
}


bool ISOTerminal_c::processPartStreamDataChunk(IsoAgLib::iStream_c* rpc_stream, bool rb_isFirstChunk, bool rb_isLastChunk)
{
  if (rpc_stream->getStreamInvalid()) return false;
  if (rb_isFirstChunk)
  {
    if (rpc_stream->getFirstByte() != 0x8 ) return false; // check for command input string value H.18
    ui16_inputStringId = rpc_stream->getNextNotParsed() | (rpc_stream->getNextNotParsed() << 8 );
    ui8_inputStringLength = rpc_stream->getNextNotParsed();

    const uint16_t ui16_totalstreamsize = rpc_stream->getByteTotalSize();
    if ( ui16_totalstreamsize != (ui8_inputStringLength + 4) )
    {
      rpc_stream->setStreamInvalid();
      return false;
    }
  }
  if (c_streamer.pc_pool != NULL) {
    c_streamer.pc_pool->eventStringValue(ui16_inputStringId, ui8_inputStringLength, *rpc_stream, rpc_stream->getNotParsedSize(), rb_isFirstChunk, rb_isLastChunk);
  }
  return false;
}


/**
  call to check if at least one vt_statusMessage has arrived so we know if the terminal is there.
  @return true -> >= 1 vt_statusMessages have arrived -> terminal is there.
*/
bool ISOTerminal_c::isVtActive ()
{
  if (vtState_a.lastReceived) {
    if (((int32_t)HAL::getTime() - (int32_t)vtState_a.lastReceived) <= 3000)
    { // comparing as int, so that in case "NOW-time > CAN-time" NO client-reload happens
      return true;
    } else {
      return false;
    }
  } else {
    return false;
  }
}


/** save current send position in data source - neeed for resend on send problem
    - implementation of the abstract IsoAgLib::MultiSendStreamer_c function
  */
void ISOTerminalStreamer_c::saveDataNextStreamPart ()
{
  pc_iterObjectsStored = pc_iterObjects;
  ui32_objectStreamPositionStored = ui32_objectStreamPosition;
  uploadBufferPositionStored = uploadBufferPosition;
  uploadBufferFilledStored = uploadBufferFilled;
  for (int i=0; i<ISO_VT_UPLOAD_BUFFER_SIZE; i++) {
    uploadBufferStored [i] = uploadBuffer [i];
  }
}


/** reactivate previously stored data source position - used for resend on problem
    - implementation of the abstract IsoAgLib::MultiSendStreamer_c function
  */
void ISOTerminalStreamer_c::restoreDataNextStreamPart ()
{
  pc_iterObjects = pc_iterObjectsStored;
  ui32_objectStreamPosition = ui32_objectStreamPositionStored;
  uploadBufferPosition = uploadBufferPositionStored;
  uploadBufferFilled = uploadBufferFilledStored;
  for (int i=0; i<ISO_VT_UPLOAD_BUFFER_SIZE; i++) {
    uploadBuffer [i] = uploadBufferStored [i];
  }
}


/** set cache for data source to stream start
    - implementation of the abstract IsoAgLib::MultiSendStreamer_c function
  */
void ISOTerminalStreamer_c::resetDataNextStreamPart ()
{
  pc_iterObjects = pc_pool->getIVtObjects();
  ui32_objectStreamPosition = 0;
  uploadBufferPosition = 0;
  uploadBufferFilled = 1;
  uploadBuffer [0] = 17;

  // ! ui32_streamSize is constant and is initialized in "StartObjectPoolUploading"
  // ! pc_pool         is constant and is initialized in "StartObjectPoolUploading"

  // following should not be needed to be reset, as this set by "saveDataNextStreamPart"
  // ? ui32_objectStreamPositionStored
  // ? pc_iterObjectsStored;
  // ? uploadBufferStored [ISO_VT_UPLOAD_BUFFER_SIZE];
  // ? uploadBufferFilledStored;
  // ? uploadBufferPositionStored;
}


/** place next data to send direct into send puffer of pointed
    stream send package - MultiSendStreamer_c will send this
    puffer afterwards
    - implementation of the abstract IsoAgLib::MultiSendStreamer_c function
  */
void ISOTerminalStreamer_c::setDataNextStreamPart (MultiSendPkg_c* mspData, uint8_t bytes)
{
  while ((uploadBufferFilled-uploadBufferPosition) < bytes) {
    // copy down the rest of the buffer (we have no ring buffer here!)
    int j=uploadBufferFilled - uploadBufferPosition;
    for (int i=0; i<j; i++) {
      uploadBuffer [i] = uploadBuffer [i+uploadBufferPosition];
    }
    // adjust pointers
    uploadBufferPosition = 0;
    uploadBufferFilled = j;

    // stream some more bytes into internal ISO_VT_UPLOAD_BUFFER_SIZE byte buffer...
    uint16_t bytes2Buffer;
    while ((bytes2Buffer = ((vtObject_c*)(*pc_iterObjects))->stream (uploadBuffer+uploadBufferFilled, ISO_VT_UPLOAD_BUFFER_SIZE-uploadBufferFilled, ui32_objectStreamPosition)) == 0) {
      pc_iterObjects++;
      ui32_objectStreamPosition = 0;
    }
    uploadBufferFilled += bytes2Buffer;
    ui32_objectStreamPosition += bytes2Buffer;
  }
  mspData->setDataPart(uploadBuffer, uploadBufferPosition, bytes);
  uploadBufferPosition += bytes;
}



void ISOTerminal_c::startObjectPoolUploading ()
{
  // calculate mask_stream size NOW (added 1 byte for "Object Pool Upload Start" Command Byte)
  // because we can't do before we get the color-depth information (0xC7)
  c_streamer.ui32_streamSize = 1;
  for (uint32_t curObject=0; curObject < c_streamer.pc_pool->getNumObjects(); curObject++) {
    c_streamer.ui32_streamSize += ((vtObject_c*)c_streamer.pc_pool->getIVtObjects()[curObject])->fitTerminal ();
  }

  c_data.setExtCanPkg8(7, 0, ECU_TO_VT_PGN>>8, vtSourceAddress, pc_wsMasterIdentItem->getIsoItem()->nr(),
                       192 /* 0xC0 */, 0xff, (c_streamer.ui32_streamSize-1) & 0xFF, ((c_streamer.ui32_streamSize-1) >>  8) & 0xFF, ((c_streamer.ui32_streamSize-1) >> 16) & 0xFF, (c_streamer.ui32_streamSize-1) >> 24, 0xff, 0xff);
  getCanInstance4Comm() << c_data;     // Command: Get Technical Data --- Parameter: Get Memory Size

  // Now proceed to uploading
  en_uploadPoolState = UploadPoolWaitingForMemoryResponse;
  ui32_uploadTimeout = DEF_TimeOut_NormalCommand;
  ui32_uploadTimestamp = HAL::getTime();
}


void ISOTerminal_c::finalizeUploading ()
{
#ifdef DEBUG
  INTERNAL_DEBUG_DEVICE << "now en_objectPoolState = OPUploadedSuccessfully;\n";
#endif
  en_uploadType = UploadIdle;
  en_objectPoolState = OPUploadedSuccessfully;
  if (c_streamer.pc_pool != NULL) {
    // there should always a pc_pool, else "finalizeUploading" wouldn't be called!
    c_streamer.pc_pool->eventObjectPoolUploadedSuccessfully ();
  }
}


/** Send "End of Object Pool" message */
void ISOTerminal_c::indicateObjectPoolCompletion ()
{
  // successfully sent, so now send out the "End of Object Pool Message" and wait for response!
  c_data.setExtCanPkg8(7, 0, ECU_TO_VT_PGN>>8, vtSourceAddress, pc_wsMasterIdentItem->getIsoItem()->nr(),
                      18, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff);
  getCanInstance4Comm() << c_data;     // Command: Object Pool Transfer --- Parameter: Object Pool Ready
  en_uploadPoolState = UploadPoolWaitingForEOOResponse; // and wait for response to set en_uploadState back to UploadIdle;
  ui32_uploadTimeout = DEF_TimeOut_EndOfObjectPool; // wait 10 seconds for terminal to initialize pool!
  ui32_uploadTimestamp = HAL::getTime();
}


void
ISOTerminal_c::vtOutOfMemory()
{  // can't (up)load the pool.
  getLbsErrInstance().registerError( LibErr_c::IsoTerminalOutOfMemory, LibErr_c::IsoTerminal );
  en_uploadPoolState = UploadPoolFailed; // no timeout needed
  en_objectPoolState = OPCannotBeUploaded;
}


/** process received can messages
  @return true -> message was processed; else the received CAN message will be served to other matching CANCustomer_c
*/
bool ISOTerminal_c::processMsg()
{
//  #ifdef DEBUG
//  INTERNAL_DEBUG_DEVICE << "Incoming Message: data().isoPgn=" << data().isoPgn() << " - HAL::getTime()=" << HAL::getTime()<<" - data[0]="<<(uint16_t)data().getUint8Data (0)<<"...   ";;
//  #endif

  /** @todo check for can-pkg-length==8???? */

  uint8_t ui8_uploadCommandError; // who is interested in the errorCode anyway?
  uint8_t ui8_errByte=0; // from 1-8, or 0 for NO errorHandling, as NO user command (was intern command like C0/C2/C3/C7/etc.)

#define MACRO_setStateDependantOnError(errByte) \
  ui8_errByte = errByte;

  // VT_TO_GLOBAL is the only PGN we accept without VT being active, because it marks the VT active!!

        //////////////////////////////////////////
       // vt to global? -->VT_TO_GLOBAL_PGN<-- //
      //////////////////////////////////////////
  if ((data().isoPgn() & 0x1FFFF) == VT_TO_GLOBAL_PGN) {
    switch (data().getUint8Data (0)) {
      case 0xFE: // Command: "Status", Parameter: "VT Status Message"
        vtState_a.lastReceived = data().time();
//        #ifdef DEBUG
//        INTERNAL_DEBUG_DEVICE << "\nLast VT Status Message encountered: data().time()=" << data().time() << " - now()=" << HAL::getTime()<<".\n";;
//        #endif
        vtState_a.saOfActiveWorkingSetMaster = data().getUint8Data (1);
        vtState_a.dataAlarmMask = data().getUint8Data (2) | (data().getUint8Data (3) << 8);
        vtState_a.softKeyMask = data().getUint8Data (4) | (data().getUint8Data (5) << 8);
        vtState_a.busyCodes = data().getUint8Data (6);
        vtState_a.functionBusy = data().getUint8Data (7);
        // get source address of virtual terminal
        vtSourceAddress = data().isoSa();
        if (c_streamer.pc_pool != NULL) {
          c_streamer.pc_pool->eventVtStatusMsg();
        }
        break;
    }
    return true; // VT_TO_GLOBAL is NOT of interest for anyone else!
  }


        //////////////////////////////////////////////
       // NACK to this ECU? -->ACKNOWLEDGE_PGN<-- ///
      //////////////////////////////////////////////
  if ( pc_wsMasterIdentItem->getIsoItem() && ((data().isoPgn() & 0x1FFFF) == (uint32_t) (ACKNOWLEDGEMENT_PGN | (pc_wsMasterIdentItem->getIsoItem()->nr()))) )
  { /// on NACK do:
    // for now ignore source address which must be VT of course. (but in case a NACK comes in before the first VT Status Message
    // Check if a VT-related message was NACKed. Check embedded PGN for that
    const uint32_t cui32_pgn =  uint32_t (data().getUint8Data(5)) |
                               (uint32_t (data().getUint8Data(6)) << 8) |
                               (uint32_t (data().getUint8Data(7)) << 16);
    switch (cui32_pgn)
    {
      case ECU_TO_VT_PGN:
      case WORKING_SET_MEMBER_PGN:
      case WORKING_SET_MASTER_PGN:
        /// fake NOT-alive state of VT for now!
        vtState_a.lastReceived = 0; // set VTalive to FALSE, so the queue will be emptied, etc. down below on the state change check.

        #ifdef DEBUG
        INTERNAL_DEBUG_DEVICE << "\n==========================================================================================="
                              << "\n=== VT NACKed "<<cui32_pgn<<", starting all over again -> faking VT loss in the following: ===";
        #endif
        checkVtStateChange(); // will also notify application by "eventEnterSafeState"
        break;
    } // switch
    return true; // (N)ACK for our SA will NOT be of interest for anyone else...
  }


    /// ////////////////////////////////////////////////////////
   /// we're here now with either LANGUAGE_PGN or VT_TO_ECU_PGN
  /// ////////////////////////////////////////////////////////

        ////////////////////////
       // -->LANGUAGE_PGN<-- //
      ////////////////////////
  if ((data().isoPgn() & 0x1FFFF) == LANGUAGE_PGN) {
    localSettings_a.lastReceived =  data().time();
    localSettings_a.languageCode = (data().getUint8Data (0) << 8) | data().getUint8Data (1);
    localSettings_a.nDecimalPoint = data().getUint8Data (2) >> 6;
    localSettings_a.nTimeFormat =  (data().getUint8Data (2) >> 4) & 0x03;
    localSettings_a.dFormat =       data().getUint8Data (3);
    localSettings_a.uDistance =     data().getUint8Data (4) >> 6;
    localSettings_a.uArea =        (data().getUint8Data (4) >> 4) & 0x03;
    localSettings_a.uVolume =      (data().getUint8Data (4) >> 2) & 0x03;
    localSettings_a.uMass =         data().getUint8Data (4)       & 0x03;
    localSettings_a.uTemperature =  data().getUint8Data (5) >> 6;
    localSettings_a.uPressure =    (data().getUint8Data (5) >> 4) & 0x03;
    localSettings_a.uForce =       (data().getUint8Data (5) >> 2) & 0x03;
    localSettings_a.uUnitsSystem =  data().getUint8Data (5)       & 0x03;
    // The other fields are reserved. (yet ;-)
    if (c_streamer.pc_pool != NULL) {
      c_streamer.pc_pool->eventLanguagePgn(localSettings_a);
    }
    /** @todo return FALSE so others can react on it? Base_c ?? */
    /** @todo Use Base_c for getting the VT's language????? */
    return true;
  }


  // If VT is not active, don't react on PKGs addressed to us, as VT's not active ;)
  if (!isVtActive()) return true;

  // If no pool registered, do nothing!
  if (en_objectPoolState == OPNoneRegistered) return true;



        //////////////////////////////////////////
       // VT to this ECU? -->VT_TO_ECU_PGN<-- ///
      //////////////////////////////////////////
  if ( pc_wsMasterIdentItem->getIsoItem() && ((data().isoPgn() & 0x1FFFF) == (uint32_t) (VT_TO_ECU_PGN + pc_wsMasterIdentItem->getIsoItem()->nr())) )
  {


    switch (data().getUint8Data (0)) {
     /*************************************/
    /*** ### VT Initiated Messages ### ***/
      case 0x00: // Command: "Control Element Function", parameter "Soft Key"
      case 0x01: // Command: "Control Element Function", parameter "Button"
        if (c_streamer.pc_pool) {
          c_streamer.pc_pool->eventKeyCode (data().getUint8Data (1) /* key activation code (pressed, released, held) */,
                                 data().getUint8Data (2) | (data().getUint8Data (3) << 8) /* objID of key object */,
                                 data().getUint8Data (4) | (data().getUint8Data (5) << 8) /* objID of visible mask */,
                                 data().getUint8Data (6) /* key code */,
                                 data().getUint8Data (0) /* 0 for sk, 1 for button -- matches wasButton? boolean */ );
        }
        break;
      case 0x05: // Command: "Control Element Function", parameter "VT Change Numeric Value"
        if (c_streamer.pc_pool) {
          c_streamer.pc_pool->eventNumericValue (uint16_t( data().getUint8Data (1) ) | (uint16_t( data().getUint8Data (2) ) << 8) /* objID */,
                                      data().getUint8Data (4) /* 1 byte value */,
                                      uint32_t( data().getUint8Data (4) ) | (uint32_t( data().getUint8Data (5) ) << 8) | (uint32_t( data().getUint8Data (6) ) << 16)| (uint32_t( data().getUint8Data (7) ) << 24) /* 4 byte value */ );
        }
        break;
      case 0x08:  // Command: "Control Element Function", parameter "VT Input String Value"
        if (c_streamer.pc_pool)
        {
          if (data().getUint8Data (3) <= 4) //within a 8 byte long cmd can be only a 4 char long string
          {
            VolatileMemory_c c_vmString (((uint8_t*)data().name())+4);
            c_streamer.pc_pool->eventStringValue (uint16_t( data().getUint8Data (1) ) | (uint16_t( data().getUint8Data (2) ) << 8) /* objID */,
                                                  data().getUint8Data (3) /* total number of bytes */, c_vmString,
                                                  data().getUint8Data (3) /* total number of bytes */, true, true);
          }
        }
        break;



     /***************************************************/
    /*** ### ECU Initiated Messages (=Responses) ### ***/
      case 0x12: // Command: "End of Object Pool Transfer", parameter "Object Pool Ready Response"
        if ((en_uploadType == UploadPool) && (en_uploadPoolState == UploadPoolWaitingForEOOResponse)) {
          if (data().getUint8Data (1) == 0) {
// Added this preprocessor so storing of object pools can be prevented for development purposes
#ifdef NO_STORE_VERSION
#else
            if (pc_versionLabel != NULL) {
              // Store Version and finalize after "Store Version Response"
              c_data.setExtCanPkg8(7, 0, ECU_TO_VT_PGN>>8, vtSourceAddress, pc_wsMasterIdentItem->getIsoItem()->nr(),
                                   208 /* D0 */, pc_versionLabel [0], pc_versionLabel [1], pc_versionLabel [2], pc_versionLabel [3], pc_versionLabel [4], pc_versionLabel [5], pc_versionLabel [6]);
              getCanInstance4Comm() << c_data;     // Command: Non Volatile Memory --- Parameter: Store Version

              // Now wait for response
              en_uploadPoolState = UploadPoolWaitingForStoreVersionResponse;
              ui32_uploadTimeout = DEF_TimeOut_StoreVersion;
              ui32_uploadTimestamp = HAL::getTime();
            }
            else
#endif // NO_STORE_VERSION
            {
              // Finalize now!
              finalizeUploading ();
            }
          } else {
            en_uploadPoolState = UploadPoolFailed; // errorcode in ui8_uploadError;
            en_objectPoolState = OPCannotBeUploaded;
            ui8_uploadError = data().getUint8Data (2);
          }
        } else {
          // we should be in send mode when receiving an end of objectpool message.. ;)
        }
        break;

#ifdef USE_SIMPLE_AUX_RESPONSE
     /***************************************/
    /*** ### AUX Assignment Messages ### ***/
      case 0x20: { // Command: "Auxiliary Control", parameter "Auxiliary Assignment"
        data().setIsoPgn(ECU_TO_VT_PGN);
        data().setIsoSa (pc_wsMasterIdentItem->getIsoItem()->nr());
        data().setIsoPs (vtSourceAddress);
        getCanInstance4Comm() << c_data;
        /// For now simply respond without doing anything else with this information. simply ack the assignment!
        } break;
#endif

     /***************************************************/
    /*** ### ECU Initiated Messages (=Responses) ### ***/
      // ### Error field is also on byte 2 (index 1)
      case 0xA3: // Command: "Command", parameter "Control Audio Device Response"
      case 0xA4: // Command: "Command", parameter "Set Audio Volume Response"
      case 0xB2: // Command: "Command", parameter "Delete Object Pool Response"
        MACRO_setStateDependantOnError(2)
        break;

      // ### Error field is also on byte 4 (index 3)
      case 0xA6: // Command: "Command", parameter "Change Size Response"
      case 0xA8: // Command: "Command", parameter "Change Numeric Value Response"
      case 0xA9: // Command: "Command", parameter "Change End Point Response"
      case 0xAA: // Command: "Command", parameter "Change Font Attributes Response"
      case 0xAB: // Command: "Command", parameter "Change Line Attributes Response"
      case 0xAC: // Command: "Command", parameter "Change Fill Attributes Response"
      case 0xAD: // Command: "Command", parameter "Change Active Mask Response"
      case 0x92: // Command: "Command", parameter "ESC Response"
        MACRO_setStateDependantOnError(4)
        break;

      // ### Error field is also on byte nr. 5 (index 4)
      case 0xA0: // Command: "Command", parameter "Hide/Show Object Response" (Container)
      case 0xA1: // Command: "Command", parameter "Enable/Disable Object Response" (Input Object)
      case 0xA2: // Command: "Command", parameter "Select Input Object Response"
      case 0xA7: // Command: "Command", parameter "Change Background Colour Response"
      case 0xAF: // Command: "Command", parameter "Change Attribute Response"
      case 0xB0: // Command: "Command", parameter "Change Priority Response"
        MACRO_setStateDependantOnError(5)
        break;

      // ### Error field is also on byte 6 (index 5)
      case 0xA5: // Command: "Command", parameter "Change Child Location Response"
      case 0xAE: // Command: "Command", parameter "Change Soft Key Mask Response"
      case 0xB3: // Command: "Command", parameter "Change String Value Response"
      case 0xB4: // Command: "Command", parameter "Change Child Position Response"
        MACRO_setStateDependantOnError(6)
        break;

      // ### Error field is on byte 7 (index 6)
      case 0xB1: // Command: "Command", parameter "Change List Item Response"
        MACRO_setStateDependantOnError(7)
        break;

      case 0xC0: // Command: "Get Technical Data", parameter "Get Memory Size Response"
        iso11783version = data().getUint8Data (1);
        if ((en_uploadType == UploadPool) && (en_uploadPoolState == UploadPoolWaitingForMemoryResponse)) {
          if (data().getUint8Data (2) == 0) {
            // start uploading, there MAY BE enough memory
            en_uploadPoolState = UploadPoolUploading;
            getMultiSendInstance4Comm().sendIsoTarget(pc_wsMasterIdentItem->getIsoItem()->nr(), vtSourceAddress, &c_streamer, ECU_TO_VT_PGN, en_sendSuccess);
          } else {
            vtOutOfMemory();
          }
        }
        break;
      case 0xC2: // Command: "Get Technical Data", parameter "Get Number Of Soft Keys Response"
        vtCapabilities_a.skWidth = data().getUint8Data (4);
        vtCapabilities_a.skHeight = data().getUint8Data (5);
        vtCapabilities_a.skVirtual = data().getUint8Data (6);
        vtCapabilities_a.skPhysical = data().getUint8Data (7);
        vtCapabilities_a.lastReceivedSoftkeys = HAL::getTime();
        break;
      case 0xC3: // Command: "Get Technical Data", parameter "Get Text Font Data Response"
        vtCapabilities_a.fontSizes = (data().getUint8Data (5) << 1) | 0x01; // 'cause "6x8" is always available!
        vtCapabilities_a.fontSizes += data().getUint8Data (6) << 8; // so we leave out the "Not used" bit!!
        vtCapabilities_a.fontTypes = data().getUint8Data (7);
        vtCapabilities_a.lastReceivedFont = HAL::getTime();
        break;
      case 0xC7: // Command: "Get Technical Data", parameter "Get Hardware Response"
        vtCapabilities_a.hwGraphicType = data().getUint8Data (2);
        vtCapabilities_a.hwHardware = data().getUint8Data (3);
        vtCapabilities_a.hwWidth = data().getUint8Data (4) + (data().getUint8Data (5) << 8);
        vtCapabilities_a.hwHeight = data().getUint8Data (6) + (data().getUint8Data (7) << 8);
        vtCapabilities_a.lastReceivedHardware = HAL::getTime();
        break;
      case 0xD0: // Command: "Non Volatile Memory", parameter "Store Version Response"
        if ((en_uploadType == UploadPool) && (en_uploadPoolState == UploadPoolWaitingForStoreVersionResponse)) {
          switch (data().getUint8Data (5) & 0x0F) {
            case 0: // Successfully stored
            case 1: // Not used
            case 2: // Version label not known
            case 8: // General error
              finalizeUploading ();
              break;
            case 4: // Insufficient memory available
            default: // well....
              getLbsErrInstance().registerError( LibErr_c::IsoTerminalOutOfMemory, LibErr_c::IsoTerminal );
              finalizeUploading ();
              break;
            }
        }
        break;
      case 0xD1: // Command: "Non Volatile Memory", parameter "Load Version Response"
        if ((en_uploadType == UploadPool) && (en_uploadPoolState == UploadPoolWaitingForLoadVersionResponse)) {
          if ((data().getUint8Data (5) & 0x0F) == 0)
          { // Successfully loaded
            finalizeUploading ();
            #ifdef DEBUG
            INTERNAL_DEBUG_DEVICE << "Received Load Version Response (D1) without error...\n";
            #endif
          }
          else
          {
            if (data().getUint8Data (5) & (1<<2))
            { // Bit 2: // Insufficient memory available
              #ifdef DEBUG
              INTERNAL_DEBUG_DEVICE << "Received Load Version Response (D1) with error OutOfMem...\n";
              #endif
              vtOutOfMemory ();
            }
            else
            { // Not used
              // General error
              // Version label not known
              startObjectPoolUploading (); // Send out pool! send out "Get Technical Data - Get Memory Size", etc. etc.
              #ifdef DEBUG
              INTERNAL_DEBUG_DEVICE << "Received Load Version Response (D1) with VersionNotFound...\n";
              #endif
            }
          }
        }
        break;
    } // switch

    // Was it some command that requires queue-deletion & error processing?
    if (ui8_errByte != 0) {
      if (en_uploadType == UploadCommand) { /* if Waiting or Timedout (or Failed <shouldn't happen>) */
        if (ui8_commandParameter == data().getUint8Data(0)) {
          /* okay, right response for our current command! */
          ui8_uploadCommandError = data().getUint8Data(ui8_errByte-1);
          /// Inform user on success/error of this command
          if (c_streamer.pc_pool) c_streamer.pc_pool->eventCommandResponse (ui8_uploadCommandError, data().name()); // pass "ui8_uploadCommandError" in case it's only important if it's an error or not. get Cmd and all databytes from "data().name()"
          #ifdef DEBUG
          if (ui8_uploadCommandError != 0)
          { /* error */
            INTERNAL_DEBUG_DEVICE << ">>> Command " << (uint32_t) ui8_commandParameter<< " failed with error " << (uint32_t) ui8_uploadCommandError << "!\n";
          }
          #endif
/* OBSOLETE: no more retries on failed commands! only on time outs!
          } else {
            en_uploadCommandState = UploadCommandFailed;
          }
*/        finishUploadCommand(); // finish command no matter if "okay" or "error"...
        }
      }
    }
    /** Acknowledgment of VT->ECU Initiated Messages (Soft Key, Button, VTChangeNumericValue etc.)
       IS >>OPTIONAL<< IN THE FINAL ISO 11783, SO IT IS LEFT OUT HERE <<COMPLETELY>>
     **/
  } // VT to this ECU

  return true; /** @todo maybe change this when doing multiple vt-clients in one ecu... */
}


/**
  deliver reference to data pkg as reference to CANPkgExt_c
  to implement the base virtual function correct
*/
CANPkgExt_c& ISOTerminal_c::dataBase()
{
  return c_data;
}


/**
  @returns true if there was place in the SendUpload-Buffer (should always be the case now)
*/
bool ISOTerminal_c::sendCommand (uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, uint8_t byte5, uint8_t byte6, uint8_t byte7, uint8_t byte8, uint32_t ui32_timeout, bool b_enableReplaceOfCmd)
{
  #ifdef DEBUG
  INTERNAL_DEBUG_DEVICE << "Enqueued 8-bytes: " << q_sendUpload.size() << " -> ";
  #endif

  SendUpload_c stringForUpload (byte1, byte2, byte3, byte4, byte5, byte6, byte7, byte8, ui32_timeout);

  return queueOrReplace(stringForUpload, b_enableReplaceOfCmd);
}


/**
  special 9 byte sendCommand function for Change Child Position Command (no one else uses 9 byte commands!)
  @returns true if there was place in the SendUpload-Buffer (should always be the case now)
*/
bool ISOTerminal_c::sendCommand (uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, uint8_t byte5, uint8_t byte6, uint8_t byte7, uint8_t byte8, uint8_t byte9, uint32_t ui32_timeout, bool b_enableReplaceOfCmd)
{
  #ifdef DEBUG
  INTERNAL_DEBUG_DEVICE << "Enqueued 9-bytes: " << q_sendUpload.size() << " -> ";
  #endif

  SendUpload_c stringForUpload (byte1, byte2, byte3, byte4, byte5, byte6, byte7, byte8, byte9, ui32_timeout);

  return queueOrReplace(stringForUpload, b_enableReplaceOfCmd);
}


bool ISOTerminal_c::sendCommandForDEBUG (uint8_t* rpui8_buffer, uint32_t ui32_size)
{
  #ifdef DEBUG
  INTERNAL_DEBUG_DEVICE << "Enqueued Debug-TP-bytes: " << q_sendUpload.size() << " -> ";
  #endif

  SendUpload_c stringForUpload (rpui8_buffer, ui32_size);

  return queueOrReplace(stringForUpload, false);
}


bool ISOTerminal_c::sendCommandChangeChildPosition (IsoAgLib::iVtObject_c* rpc_object, IsoAgLib::iVtObject_c* rpc_childObject, int16_t x, int16_t y, bool b_enableReplaceOfCmd)
{
  return sendCommand (180 /* Command: Command --- Parameter: Change Child Position */,
                      rpc_object->getID() & 0xFF, rpc_object->getID() >> 8,
                      rpc_childObject->getID() & 0xFF, rpc_childObject->getID() >> 8,
                      x & 0xFF, x >> 8,
                      y & 0xFF, y >> 8,
                      DEF_TimeOut_NormalCommand, b_enableReplaceOfCmd);
}


//! should only be called with valid values ranging -127..0..128 (according to ISO!!!)
// THIS FUNCTION ADDED BY BRAD COX 26-AUG-2004 TO IMPLEMENT CHANGE CHILD LOCATION COMMAND
// //////////////////////////////// +X2C Operation BAC : sendCommandChangeChildLocation
bool ISOTerminal_c::sendCommandChangeChildLocation (IsoAgLib::iVtObject_c* rpc_object, IsoAgLib::iVtObject_c* rpc_childObject, int16_t dx, int16_t dy, bool b_enableReplaceOfCmd)
{
  return sendCommand (165 /* Command: Command --- Parameter: Change Child Location */,
                      rpc_object->getID() & 0xFF, rpc_object->getID() >> 8,
                      rpc_childObject->getID() & 0xFF, rpc_childObject->getID() >> 8,
                      dx+127, dy+127, 0xFF,
                      DEF_TimeOut_NormalCommand, b_enableReplaceOfCmd);
}

// THIS FUNCTION ADDED BY BRAD COX 26-AUG-2004 TO IMPLEMENT CHANGE SIZE COMMAND
// //////////////////////////////// +X2C Operation BAC :Operation: sendCommandChangeSize
bool ISOTerminal_c::sendCommandChangeSize(IsoAgLib::iVtObject_c* rpc_object,uint16_t newWidth, uint16_t newHeight, bool b_enableReplaceOfCmd)
{
  return sendCommand (166 /* Command: Command --- Parameter: Change Size */,
                      rpc_object->getID() & 0xFF, rpc_object->getID() >> 8,
                      newWidth & 0xFF, newWidth >> 8,
                      newHeight & 0xFF, newHeight >> 8,
                      0xFF,
                      DEF_TimeOut_NormalCommand, b_enableReplaceOfCmd);
}

// THIS FUNCTION ADDED BY BRAD COX 26-AUG-2004 TO IMPLEMENT CHANGE BACKGROUND COLOUR COMMAND
// //////////////////////////////// +X2C Operation BAC : sendCommandChangeBackgroundColour
bool ISOTerminal_c::sendCommandChangeBackgroundColour(IsoAgLib::iVtObject_c* rpc_object, uint8_t newColour, bool b_enableReplaceOfCmd)
{
  return sendCommand (167 /* Command: Command --- Parameter: Change Background Color */,
                      rpc_object->getID() & 0xFF, rpc_object->getID() >> 8,
                      newColour, 0xFF, 0xFF, 0xFF, 0xFF,
                      DEF_TimeOut_NormalCommand, b_enableReplaceOfCmd);
}

bool ISOTerminal_c::sendCommandChangeNumericValue (IsoAgLib::iVtObject_c* rpc_object, uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, bool b_enableReplaceOfCmd)
{
  return sendCommand (168 /* Command: Command --- Parameter: Change Numeric Value */,
                      rpc_object->getID() & 0xFF, rpc_object->getID() >> 8,
                      0xFF, byte1, byte2, byte3, byte4,
                      DEF_TimeOut_NormalCommand, b_enableReplaceOfCmd);
}

bool ISOTerminal_c::sendCommandChangeEndPoint(IsoAgLib::iVtObject_c* rpc_object,uint16_t newWidth, uint16_t newHeight, uint8_t newLineAttributes, bool b_enableReplaceOfCmd)
{
  return sendCommand (169 /* Command: Command --- Parameter: Change Size */,
          rpc_object->getID() & 0xFF, rpc_object->getID() >> 8,
          newWidth & 0xFF, newWidth >> 8,
          newHeight & 0xFF, newHeight >> 8,
          newLineAttributes,
          DEF_TimeOut_NormalCommand, b_enableReplaceOfCmd);
}

bool ISOTerminal_c::sendCommandChangeFontAttributes (IsoAgLib::iVtObject_c* rpc_object, uint8_t newFontColour, uint8_t newFontSize, uint8_t newFontType, uint8_t newFontStyle, bool b_enableReplaceOfCmd)
{
  return sendCommand (170 /* Command: Command --- Parameter: Change FontAttributes */,
                      rpc_object->getID() & 0xFF, rpc_object->getID() >> 8,
                      newFontColour, newFontSize, newFontType, newFontStyle, 0xFF,
                      DEF_TimeOut_NormalCommand, b_enableReplaceOfCmd);
}

bool ISOTerminal_c::sendCommandChangeLineAttributes (IsoAgLib::iVtObject_c* rpc_object, uint8_t newLineColour, uint8_t newLineWidth, uint16_t newLineArt, bool b_enableReplaceOfCmd)
{
  return sendCommand (171 /* Command: Command --- Parameter: Change LineAttributes */,
                     rpc_object->getID() & 0xFF, rpc_object->getID() >> 8,
                     newLineColour, newLineWidth, newLineArt & 0xFF, newLineArt >> 8, 0xFF,
                     DEF_TimeOut_NormalCommand, b_enableReplaceOfCmd);
}

// THIS FUNCTION ADDED BY BRAD COX 17-SEP-2004 TO IMPLEMENT CHANGE FILL ATTRIBUTES COMMAND
// //////////////////////////////// +X2C Operation BAC :Operation: sendCommandChangeSize
bool ISOTerminal_c::sendCommandChangeFillAttributes (IsoAgLib::iVtObject_c* rpc_object, uint8_t newFillType, uint8_t newFillColour, IsoAgLib::iVtObjectPictureGraphic_c* newFillPatternObject, bool b_enableReplaceOfCmd)
{
  return sendCommand (172 /* Command: Command --- Parameter: Change FillAttributes */,
                      rpc_object->getID() & 0xFF, rpc_object->getID() >> 8,
                      newFillType, newFillColour,
                      (newFillType == 3) ? newFillPatternObject->getID() & 0xFF : 0xFF,
                      (newFillType == 3) ? newFillPatternObject->getID() >> 8 : 0xFF,
                      0xFF,
                      DEF_TimeOut_NormalCommand, b_enableReplaceOfCmd);
}

bool ISOTerminal_c::sendCommandChangeSoftKeyMask (IsoAgLib::iVtObject_c* rpc_object, uint8_t maskType, uint16_t newSoftKeyMask, bool b_enableReplaceOfCmd)
{
  return sendCommand (174 /* Command: Command --- Parameter: Change Soft Key Mask */,
                      maskType,
                      rpc_object->getID() & 0xFF, rpc_object->getID() >> 8,
                      newSoftKeyMask & 0xFF, newSoftKeyMask >> 8,
                      0xFF, 0xFF,
                      DEF_TimeOut_NormalCommand, b_enableReplaceOfCmd);
}

bool ISOTerminal_c::sendCommandChangeAttribute (IsoAgLib::iVtObject_c* rpc_object, uint8_t attrId, uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, bool b_enableReplaceOfCmd)
{
  return sendCommand (175 /* Command: Command --- Parameter: Change Attribute */,
                      rpc_object->getID() & 0xFF, rpc_object->getID() >> 8,
                      attrId, byte1, byte2, byte3, byte4,
                      DEF_TimeOut_NormalCommand, b_enableReplaceOfCmd);
}

// THIS FUNCTION ADDED BY BRAD COX 26-AUG-2004 TO IMPLEMENT CHANGE PRIORITY COMMAND
// //////////////////////////////// +X2C Operation BAC :Operation: sendCommandChangePriority
bool ISOTerminal_c::sendCommandChangePriority(IsoAgLib::iVtObject_c* rpc_object, int8_t newPriority, bool b_enableReplaceOfCmd)
{
  if(newPriority < 3) {
    // only bother to send if priority is a legal value
    return sendCommand (176 /* Command: Command --- Parameter: Change Priority */,
                      rpc_object->getID() & 0xFF, rpc_object->getID() >> 8,
                      newPriority, 0xFF, 0xFF, 0xFF, 0xFF,
                      DEF_TimeOut_NormalCommand, b_enableReplaceOfCmd);
  } else {
    return false;
  }
}

bool ISOTerminal_c::sendCommandDeleteObjectPool ()
{
  return sendCommand (178 /* Command: Command --- Parameter: Delete Object Pool */,
                      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, DEF_TimeOut_NormalCommand);
}

bool ISOTerminal_c::sendCommandChangeStringValue (IsoAgLib::iVtObject_c* rpc_object, const char* rpc_newValue, uint16_t overrideSendLength, bool b_enableReplaceOfCmd)
{
  #ifdef DEBUG
  INTERNAL_DEBUG_DEVICE << "Enqueued string-ref: " << q_sendUpload.size() << " -> ";
  #endif

  SendUpload_c stringForUpload (rpc_object->getID(), rpc_newValue, overrideSendLength);

  return queueOrReplace (stringForUpload, b_enableReplaceOfCmd);
}

bool ISOTerminal_c::sendCommandChangeStringValue (IsoAgLib::iVtObjectString_c* rpc_objectString, bool b_enableReplaceOfCmd)
{
  #ifdef DEBUG
  INTERNAL_DEBUG_DEVICE << "Enqueued stringObject-mss: " << q_sendUpload.size() << " -> ";
  #endif

  SendUpload_c stringForUpload (rpc_objectString);

  return queueOrReplace (stringForUpload, b_enableReplaceOfCmd);
}


bool ISOTerminal_c::queueOrReplace(SendUpload_c& rref_sendUpload, bool b_enableReplaceOfCmd)
{
  if (!isVtActive())
  {
    #ifdef DEBUG
    INTERNAL_DEBUG_DEVICE << "--NOT ENQUEUED - VT IS NOT ACTIVE!--\n";
    #endif
  //  return false;
  }
  SendUpload_c* p_queue = NULL;
  uint8_t i = 0;
  #ifdef USE_LIST_FOR_FIFO
  #ifdef OPTIMIZE_HEAPSIZE_IN_FAVOR_OF_SPEED
  std::list<SendUpload_c,std::__malloc_alloc_template<0> >::iterator i_sendUpload;
  #else
  std::list<SendUpload_c>::iterator i_sendUpload;
  #endif
  #else
  std::queue<SendUpload_c>::iterator i_sendUpload;
  #endif
  if (b_checkSameCommand && b_enableReplaceOfCmd) {
    //get first equal command in queue
    for (i_sendUpload = q_sendUpload.begin(); (p_queue == NULL) && (i_sendUpload != q_sendUpload.end()); i_sendUpload++)
    {
      //first check if multisendstreamer is used!
      /* four cases:
         1. both use buffer
         2. both use mssObjectString
         3. mss is queued and could be replaced by buffer
         4. buffer is queued and could be replaced by mssObjectString
       */
      if ((i_sendUpload->mssObjectString == NULL) && (rref_sendUpload.mssObjectString == NULL))
      {
        if (i_sendUpload->vec_uploadBuffer[0] == rref_sendUpload.vec_uploadBuffer[0])
        {
          uint8_t ui8_offset = (rref_sendUpload.vec_uploadBuffer[0]);
          if ( (ui8_offset<0x92) || (ui8_offset > 0xB4))
          {
            //not possible by definition, but for being sure :-)
            #ifdef DEBUG
            INTERNAL_DEBUG_DEVICE << "--INVALID COMMAND! SHOULDN'T HAPPEN!!--\n";
            #endif
            return false;
          }
          //get bitmask for the corresponding command
          uint8_t ui8_bitmask = scpui8_cmdCompareTable [ui8_offset-0x92];
          if (!(ui8_bitmask & (1<<0)))
          { // go Check for overwrite...
            for (i=1;i<=7;i++)
            {
              if (((ui8_bitmask & 1<<i) !=0) && !(i_sendUpload->vec_uploadBuffer[i] == rref_sendUpload.vec_uploadBuffer[i]))
              {
                break;
              }
            }
            if (!(i<=7))
            { // loop ran through, all to-compare-bytes matched!
              p_queue = &*i_sendUpload; // so overwrite this SendUpload_c with the new value one
            }
          }
        }
      }
      if ((i_sendUpload->mssObjectString != NULL) && (rref_sendUpload.mssObjectString != NULL))
      {
        if ((*i_sendUpload).mssObjectString->getStreamer()->getFirstByte() == rref_sendUpload.mssObjectString->getStreamer()->getFirstByte())
        {
          if ((*i_sendUpload).mssObjectString->getStreamer()->getID() == rref_sendUpload.mssObjectString->getStreamer()->getID())
          {
            p_queue = &*i_sendUpload;
          }
        }
      }
      if ((i_sendUpload->mssObjectString != NULL) && (rref_sendUpload.mssObjectString == NULL))
      {
        if ((*i_sendUpload).mssObjectString->getStreamer()->getFirstByte() == rref_sendUpload.vec_uploadBuffer[0])
        {
          if ((*i_sendUpload).mssObjectString->getStreamer()->getID() == (rref_sendUpload.vec_uploadBuffer[1] | (rref_sendUpload.vec_uploadBuffer[2]<<8)))
          {
            p_queue = &*i_sendUpload;
          }
        }
      }
      if ((i_sendUpload->mssObjectString == NULL) && (rref_sendUpload.mssObjectString != NULL))
      {
        if ((*i_sendUpload).vec_uploadBuffer[0] == rref_sendUpload.mssObjectString->getStreamer()->getFirstByte())
        {
          if (((*i_sendUpload).vec_uploadBuffer[1] | (*i_sendUpload).vec_uploadBuffer[2]<<8) == rref_sendUpload.mssObjectString->getStreamer()->getID())
          {
            p_queue = &*i_sendUpload;
          }
        }
      }
    } // for
  }
  if (p_queue == NULL)
  {
    /* The SendUpload_c constructor only takes a reference, so don't change the string in the meantime!!! */
    #ifdef USE_LIST_FOR_FIFO
    // queueing with list: queue::push <-> list::push_back; queue::front<->list::front; queue::pop<->list::pop_front
    q_sendUpload.push_back (rref_sendUpload);
    #else
    q_sendUpload.push (rref_sendUpload);
    #endif
  }
  else
  {
    *p_queue = rref_sendUpload; // overloaded "operator="
  }
  #ifdef DEBUG_HEAP_USEAGE
  sui16_sendUploadQueueSize++;
  if ( sui16_sendUploadQueueSize > sui16_maxSendUploadQueueSize )
    sui16_maxSendUploadQueueSize = sui16_sendUploadQueueSize;
  #endif

  #ifdef DEBUG
  INTERNAL_DEBUG_DEVICE << q_sendUpload.size() << ".\n";
  //dumpQueue(); /* to see all enqueued cmds after every enqueued cmd */
  #endif
  /** push(...) has no return value */
  return true;
}

void ISOTerminal_c::dumpQueue()
{
  #ifdef USE_LIST_FOR_FIFO
  #ifdef OPTIMIZE_HEAPSIZE_IN_FAVOR_OF_SPEED
  std::list<SendUpload_c,std::__malloc_alloc_template<0> >::iterator i_sendUpload;
  #else
  std::list<SendUpload_c>::iterator i_sendUpload;
  #endif
  #else
  std::queue<SendUpload_c>::iterator i_sendUpload;
  #endif

  for (i_sendUpload = q_sendUpload.begin(); i_sendUpload != q_sendUpload.end(); i_sendUpload++)
  {
    if (i_sendUpload->mssObjectString == NULL)
    {
      for (uint8_t i=0; i<=7; i++)
      {
        #ifdef DEBUG
        INTERNAL_DEBUG_DEVICE << " " << (uint16_t)(i_sendUpload->vec_uploadBuffer[i]);
        #endif
      }
    }
    else
    {
      MultiSendPkg_c msp;
      int i_strSize = i_sendUpload->mssObjectString->getStreamer()->getStreamSize();
      for (int i=0; i < i_strSize; i+=7) {
        i_sendUpload->mssObjectString->getStreamer()->setDataNextStreamPart(&msp, (unsigned char) ((i_strSize - i) > 7 ? 7 : (i_strSize-i)));
        for (uint8_t i=1; i<=7; i++)
        {
          #ifdef DEBUG
          INTERNAL_DEBUG_DEVICE << " " << (uint16_t)(msp[i]);
          #endif
        }
      }
    }
  }
  #ifdef DEBUG
  INTERNAL_DEBUG_DEVICE << "\n";
  #endif
}


uint32_t ISOTerminal_c::getUploadBufferSize ()
{
  return q_sendUpload.size();
}

} // end namespace __IsoAgLib
