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
#include "isoterminal_c.h"

#include <IsoAgLib/comm/Scheduler/impl/scheduler_c.h>
#include <IsoAgLib/driver/can/impl/canio_c.h>
#include <IsoAgLib/hal/system.h>
#include <IsoAgLib/comm/SystemMgmt/ISO11783/impl/isomonitor_c.h>
#include <IsoAgLib/comm/SystemMgmt/impl/systemmgmt_c.h>


#include "vttypes.h"
#include "../ivtobjectpicturegraphic_c.h"
#include "../ivtobjectstring_c.h"

#if defined(DEBUG) || defined(DEBUG_HEAP_USEAGE)
	#include <supplementary_driver/driver/rs232/impl/rs232io_c.h>
	#include <IsoAgLib/util/impl/util_funcs.h>
#endif

#ifdef DEBUG_HEAP_USEAGE
  static uint16_t sui16_lastPrintedBufferCapacity = 0;
  static uint16_t sui16_lastPrintedSendCommandQueueSize = 0;
  static uint16_t sui16_lastPrintedSendUploadQueueSize = 0;
  static uint16_t sui16_lastPrintedMaxSendCommandQueueSize = 0;
  static uint16_t sui16_lastPrintedMaxSendUploadQueueSize = 0;
  static uint16_t sui16_sendCommandQueueSize = 0;
  static uint16_t sui16_sendUploadQueueSize = 0;
  static uint16_t sui16_maxSendCommandQueueSize = 0;
  static uint16_t sui16_maxSendUploadQueueSize = 0;
#endif

// #define LOESCHE_POOL

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


/**
  default constructor
*/
SendCommand_c::SendCommand_c (uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, uint8_t byte5, uint8_t byte6, uint8_t byte7, uint8_t byte8, uint32_t ui32_timeout)
{
  arr_commandBuffer [0] = byte1;  arr_commandBuffer [1] = byte2;
  arr_commandBuffer [2] = byte3;  arr_commandBuffer [3] = byte4;
  arr_commandBuffer [4] = byte5;  arr_commandBuffer [5] = byte6;
  arr_commandBuffer [6] = byte7;  arr_commandBuffer [7] = byte8;
  ui32_commandTimeout = ui32_timeout;
}

/**
  StringUpload constructor that initializes all fields of this class (use only for Change String Value TP Commands)
*/
SendUpload_c::SendUpload_c (uint16_t rui16_objId, const char* rpc_string, uint16_t overrideSendLength, uint8_t rui8_retryCount)
{
  // if string is shorter than length, it's okay to send - if it's longer, we'll clip!!
  uint16_t strLen = (CNAMESPACE::strlen(rpc_string) < overrideSendLength) ? CNAMESPACE::strlen(rpc_string) : overrideSendLength;

  vec_uploadBuffer.reserve (1 +2 +2 +strLen);
  vec_uploadBuffer.push_back (179 /* 0xB3 */); /* Command: Command --- Parameter: Change String Value (TP) */
  vec_uploadBuffer.push_back (rui16_objId & 0xFF);
  vec_uploadBuffer.push_back (rui16_objId >> 8);
  vec_uploadBuffer.push_back (strLen & 0xFF);
  vec_uploadBuffer.push_back (strLen >> 8);

  for (int i=0; i<strLen; i++) {
    vec_uploadBuffer.push_back (*rpc_string);
    rpc_string++;
  }

  mssObjectString=NULL;
  ui8_retryCount = rui8_retryCount;

  #ifdef DEBUG_HEAP_USEAGE
  if ( vec_uploadBuffer.capacity() != sui16_lastPrintedBufferCapacity )
  {
    sui16_lastPrintedBufferCapacity = vec_uploadBuffer.capacity();
    getRs232Instance()
	    << "ISOTerminal_c Buffer-Capa: " << sui16_lastPrintedBufferCapacity << "\r\n";
  }
  #endif
}

/**
  StringUpload constructor for use of pointered MSS usage
*/
SendUpload_c::SendUpload_c (vtObjectString_c* rpc_objectString, uint8_t rui8_retryCount)
{
  mssObjectString = rpc_objectString;
  ui8_retryCount = rui8_retryCount;
}

const SendUpload_c& SendUpload_c::operator= (const SendUpload_c& ref_source)
{
  vec_uploadBuffer = ref_source.vec_uploadBuffer;
  ui8_retryCount = ref_source.ui8_retryCount;
  mssObjectString = ref_source.mssObjectString;
  return ref_source;
}

SendUpload_c::SendUpload_c (const SendUpload_c& ref_source) : mssObjectString (ref_source.mssObjectString), vec_uploadBuffer (ref_source.vec_uploadBuffer), ui8_retryCount (ref_source.ui8_retryCount)
{
}


  /*************************************/
 /**** Iso Terminal Implementation ****/
/*************************************/


bool ISOTerminal_c::startUpload (SendUpload_c* actSend) {
  if (actSend->mssObjectString == NULL) {
    return getMultiSendInstance().sendIsoTarget(pc_wsMasterIdentItem->getIsoItem()->nr(), vtSourceAddress, &actSend->vec_uploadBuffer.front(), actSend->vec_uploadBuffer.size(), ECU_TO_VT_PGN, &en_sendSuccess);
  } else {
    return getMultiSendInstance().sendIsoTarget(pc_wsMasterIdentItem->getIsoItem()->nr(), vtSourceAddress, (MultiSendStreamer_c*)actSend->mssObjectString, ECU_TO_VT_PGN, &en_sendSuccess);
  }
}

/**
  default constructor, which can optional set the pointer to the containing
  Scheduler_c object instance
*/
ISOTerminal_c::ISOTerminal_c()
{
}

/**
  default destructor, which initiate sending address release for all own identities
  @see DINMonitor_c::~DINMonitor_c
*/
ISOTerminal_c::~ISOTerminal_c(){
  close();
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
  
  if (!((en_uploadState == UIdle) || (en_uploadState == UFailed))) {
    // could not start bacause uploader is busy, so we can't set the variables, but this shouldn't really
    // happen, as the pool should be registered right after initializing the terminal...
    getLbsErrInstance().registerError( LibErr_c::LbsMultiSendBusy, LibErr_c::IsoTerminal );
    return false;
  }

  pc_wsMasterIdentItem = rpc_wsMasterIdentItem;
  pc_pool = rpc_pool;
  pc_pool->initAllObjectsOnce(); // the generated initAllObjectsOnce() has to ensure to be idempotent! (vt2iso-generated source does this!)

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
  pc_pool = NULL;
  en_objectPoolState = OPNoneRegistered;
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
void ISOTerminal_c::init()
{
  static bool b_alreadyInitialized = false;

  if ((!b_alreadyInitialized) || checkAlreadyClosed()) {
    // clear state of b_alreadyClosed, so that close() is called one time
    clearAlreadyClosed();
    // register in Scheduler_c to get timeEvents
    getSchedulerInstance4Comm().registerClient( this );

    vtState_a.lastReceived = 0; // no vt_statusMessage received yet
    vtSourceAddress = 254; // shouldn't be read anyway before vt_statusMessage arrived....
    
    pc_wsMasterIdentItem = NULL;
    pc_pool = NULL;
    pc_versionLabel = NULL;

    en_objectPoolState = OPNoneRegistered;
    b_receiveFilterCreated = false;
    
    vtAliveNew = false;

    // so init() only gets executed once!
    b_alreadyInitialized = true;
  }
};


/**
  initialise element which can't be done during construct

  possible errors:
*/
bool ISOTerminal_c::init(IdentItem_c* rpc_wsMasterIdentItem, IsoAgLib::iIsoTerminalObjectPool_c* rpc_pool, char* rpc_versionLabel)
{
  init();
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
    if (getCanInstance4Comm().existFilter((0x1FFFF00UL), ui32_filter, Ident_c::ExtendedIdent))
    { // delete FilterBox
      getCanInstance4Comm().deleteFilter( (0x1FFFF00UL), ui32_filter, Ident_c::ExtendedIdent);
    }
    ui32_filter = (static_cast<MASK_TYPE>(LANGUAGE_PGN) << 8);
    if (getCanInstance4Comm().existFilter((0x1FFFF00UL), ui32_filter, Ident_c::ExtendedIdent))
    { // delete FilterBox
      getCanInstance4Comm().deleteFilter( (0x1FFFF00UL), ui32_filter, Ident_c::ExtendedIdent);
    }
    deregisterIsoObjectPool();
  }
}

/**
  periodically event -> call timeEvent for all  identities and parent objects
  @return true -> all planned activities performed in allowed time
*/
bool ISOTerminal_c::timeEvent( void )
{
  CANIO_c& c_can = getCanInstance4Comm();
  ISOMonitor_c& c_isoMonitor = getIsoMonitorInstance4Comm();

  if ( Scheduler_c::getAvailableExecTime() == 0 ) return false;
  #ifdef DEBUG_HEAP_USEAGE
  if ( ( sui16_lastPrintedSendCommandQueueSize < sui16_sendCommandQueueSize )
    || ( sui16_lastPrintedMaxSendCommandQueueSize < sui16_maxSendCommandQueueSize ) )
  { // MAX amount of sui16_sendCommandQueueSize or sui16_maxSendCommandQueueSize
    sui16_lastPrintedSendCommandQueueSize = sui16_sendCommandQueueSize;
    sui16_lastPrintedMaxSendCommandQueueSize = sui16_maxSendCommandQueueSize;
    getRs232Instance()
      << "Max: " << sui16_maxSendCommandQueueSize << " Items in FIFO, "
	    << sui16_sendCommandQueueSize << " x SendCommand_c: Mal-Alloc: "
      << sizeSlistTWithMalloc( sizeof(SendCommand_c), sui16_sendCommandQueueSize )
      << "/" << sizeSlistTWithMalloc( sizeof(SendCommand_c), 1 )
      << ", Chunk-Alloc: "
      << sizeSlistTWithChunk( sizeof(SendCommand_c), sui16_sendCommandQueueSize )
      << "\r\n\r\n";
  }

  if ( ( sui16_lastPrintedSendUploadQueueSize < sui16_sendUploadQueueSize )
    || ( sui16_lastPrintedMaxSendUploadQueueSize < sui16_maxSendUploadQueueSize ) )
  { // MAX amount of sui16_sendCommandQueueSize or sui16_maxSendCommandQueueSize
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

/*** Filter Registration Start ***/
  // register Filter if at least one active ( claimed address )
  // local ISO IdentItem_c exists (so we're working on an ISO-Bus!
  if ( ! getSystemMgmtInstance4Comm().existActiveLocalIsoMember() ) return true;

  if ( ! b_receiveFilterCreated )
  { // register to get VTStatus Messages
    b_receiveFilterCreated = true;
    uint32_t ui32_filter = (static_cast<MASK_TYPE>(VT_TO_GLOBAL_PGN) << 8);
    if (!getCanInstance4Comm().existFilter((0x1FFFF00UL), ui32_filter, Ident_c::ExtendedIdent))
    { // create FilterBox
      getCanInstance4Comm().insertFilter(*this, (0x1FFFF00UL), ui32_filter, true, Ident_c::ExtendedIdent);
    }
    ui32_filter = (static_cast<MASK_TYPE>(LANGUAGE_PGN) << 8);
    if (!getCanInstance4Comm().existFilter((0x1FFFF00UL), ui32_filter, Ident_c::ExtendedIdent))
    { // create FilterBox
      getCanInstance4Comm().insertFilter(*this, (0x1FFFF00UL), ui32_filter, true, Ident_c::ExtendedIdent);
    }
  }
/*** Filter Registration End ***/

  // do further activities only if registered ident is initialised as ISO
  if ( !pc_wsMasterIdentItem ) return true;
  if ( pc_wsMasterIdentItem->getIsoItem() == NULL ) return true;
  //if ( !c_isoMonitor.existIsoMemberGtp (pc_wsMasterIdentItem->gtp (), true)) return true;

  
  
 /*** Regular start is here (the above preconditions should be satisfied if system is finally set up. ***/
/*******************************************************************************************************/  

  
  // #######################
  // ### VT Alive checks ###
  // #######################

  bool vtAliveOld=vtAliveNew;
  vtAliveNew=isVtActive();
  
  if (vtAliveOld != vtAliveNew) {
    // react on vt alive change
    if (vtAliveNew == true) {
      // VT has (re-)entered the System - init all necessary states...
      vtCapabilities_a.lastReceivedSoftkeys = 0; // not yet (queried and) got answer about vt's capabilities yet
      vtCapabilities_a.lastRequestedSoftkeys = 0; // not yet requested vt's capabilities yet
      vtCapabilities_a.lastReceivedHardware = 0; // not yet (queried and) got answer about vt's capabilities yet
      vtCapabilities_a.lastRequestedHardware = 0; // not yet requested vt's capabilities yet
      vtCapabilities_a.lastReceivedFont = 0; // not yet (queried and) got answer about vt's capabilities yet
      vtCapabilities_a.lastRequestedFont = 0; // not yet requested vt's capabilities yet
      localSettings_a.lastRequested = 0;
      localSettings_a.lastReceived = 0;
      en_uploadState = UIdle;
      en_sendCommandState = SendCommandIdle;
      if (en_objectPoolState != OPNoneRegistered) en_objectPoolState = OPRegistered; // try re-uploading, not caring if it was successfully or not on the last vt!
    } else {
      vtSourceAddress = 254;
      // VT has left the system - clear all queues now, don't wait until next re-entering (for memory reasons)
      #ifdef DEBUG_HEAP_USEAGE
      sui16_sendCommandQueueSize -= q_sendCommand.size();
      sui16_sendUploadQueueSize -= q_sendUpload.size();
      #endif
      #ifdef USE_LIST_FOR_FIFO
      // queueing with list: queue::push <-> list::push_back; queue::front<->list::front; queue::pop<->list::pop_front
      q_sendCommand.clear();
      q_sendUpload.clear();
      #else
      while (!q_sendCommand.empty()) q_sendCommand.pop();
      while (!q_sendUpload.empty()) q_sendUpload.pop();
      #endif
    }
  }

  // ### Do nothing if there's no VT active ###
  if (!isVtActive()) return true;

  
  // be kind and wait until "COMPLETE AddressClaim" is finished (WSMaster/Slaves Announce)
  if ( !pc_wsMasterIdentItem->getIsoItem()->isClaimedAddress() ) return true;

  // Do nothing if no pool is registered
  if ( en_objectPoolState == OPNoneRegistered ) return true;

  // ######################################
  // ### Do TimeOut / Finishing Actions ###
  // ######################################

  // ### Check if sendCommand timed out...
  if (en_sendCommandState == WaitingForCommandResponse) {
    // timed out?
    if (((uint32_t) HAL::getTime()) > (ui32_sendCommandTimeout + ui32_sendCommandTimestamp)) {
      en_sendCommandState = SendCommandTimedOut;
    }
  }

  // ### Check if OBJECTPOOL UPLOAD failed/finished? (these states are NOT possible when it's not an object pool!)
  if ((en_uploadState == UWaitingForLoadVersionResponse) || (en_uploadState == UWaitingForMemoryResponse) || (en_uploadState == UWaitingForEOOResponse)) {
    // timedout
    if (((uint32_t) HAL::getTime()) > (ui32_uploadTimeout + ui32_uploadTimestamp)) {
      en_uploadState = UFailed;
      ui32_uploadTimestamp = HAL::getTime();
      ui32_uploadTimeout = 5000; // wait 5 secs for possible reuploading...
    }
  }
  if (en_uploadState == UWaitingForStoreVersionResponse) {
    // timedout
    if (((uint32_t) HAL::getTime()) > (ui32_uploadTimeout + ui32_uploadTimestamp)) {
      // we couldn't store for some reason, but don't care, finalize anyway...
      finalizeUploading ();
    }
  }

  // ### Check if we expect an (OBJECTPOOL) UPLOAD to fail/finish?
  if (en_uploadState == UUploading) {
    switch (en_sendSuccess) {
      case __IsoAgLib::MultiSend_c::Running:
        // do nothing, still wait.
        break;
      case __IsoAgLib::MultiSend_c::SendAborted:
        // aborted sending
        switch (en_uploadType) {
          case UploadObjectPool:
            en_uploadState = UFailed;
            ui32_uploadTimestamp = HAL::getTime();
            ui32_uploadTimeout = 5000; // if it was an object pool, try reuploading in 5 secs...
            break;
          case UploadChangeStringValue:
            if (ui8_uploadRetry > 0) {
              // retry
              ui8_uploadRetry--;
              startUpload (&q_sendUpload.front());
            }
            break;
        }
        break;
      case __IsoAgLib::MultiSend_c::SendSuccess:
        // successfully sent, so do we now have to send out the "End of Object Pool Message"?
        switch (en_uploadType) {
          case UploadObjectPool:
            indicateObjectPoolCompletion (); // Send "End of Object Pool" message
            break;
          case UploadChangeStringValue:
            // successfully sent and nothing to wait for so go for Idle now!
            en_uploadState = UIdle; // and wait for response to set en_uploadState back to UploadIdle;
            #ifdef USE_LIST_FOR_FIFO
            // queueing with list: queue::push <-> list::push_back; queue::front<->list::front; queue::pop<->list::pop_front
            q_sendUpload.pop_front();
            #else
            q_sendUpload.pop();
            #endif
            #ifdef DEBUG_HEAP_USEAGE
            sui16_sendUploadQueueSize--;
            #endif
            break;
        }
        break;
    }
  }


  // #############################################
  // ### Initiate new Actions (Command/Upload) ###
  // #############################################
  
  // use IsoMonitor's dataPkg for this reason
  ISOSystemPkg_c& c_pkg = c_isoMonitor.data();

  // ### Do we have anything pool related to do?
  if (en_objectPoolState == OPRegistered) {
    // ### Do we have to request (any) vt capabilities?
    if (!(vtCapabilities_a.lastReceivedFont && vtCapabilities_a.lastReceivedHardware && vtCapabilities_a.lastReceivedSoftkeys && localSettings_a.lastReceived)) {
      if (!vtCapabilities_a.lastReceivedSoftkeys && ((vtCapabilities_a.lastRequestedSoftkeys == 0) || ((HAL::getTime()-vtCapabilities_a.lastRequestedSoftkeys) > 1000))) {
        // Get Number Of Soft Keys
        c_pkg.setExtCanPkg8 (7, 0, 231, vtSourceAddress, pc_wsMasterIdentItem->getIsoItem()->nr(),
                             194, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff);
        c_can << c_pkg;      // Command: Get Technical Data --- Parameter: Get Text Font Data
        vtCapabilities_a.lastRequestedSoftkeys = HAL::getTime();
      }
      if (vtCapabilities_a.lastReceivedSoftkeys && (!vtCapabilities_a.lastReceivedFont) && ((vtCapabilities_a.lastRequestedFont == 0) || ((HAL::getTime()-vtCapabilities_a.lastRequestedFont) > 1000))) {
        // Get Text Font Data
        c_pkg.setExtCanPkg8 (7, 0, 231, vtSourceAddress, pc_wsMasterIdentItem->getIsoItem()->nr(),
                             195, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff);
        c_can << c_pkg;      // Command: Get Technical Data --- Parameter: Get Text Font Data
        vtCapabilities_a.lastRequestedFont = HAL::getTime();
      }
      if (vtCapabilities_a.lastReceivedSoftkeys && vtCapabilities_a.lastReceivedFont && (!vtCapabilities_a.lastReceivedHardware) && ((vtCapabilities_a.lastRequestedHardware == 0) || ((HAL::getTime()-vtCapabilities_a.lastRequestedHardware) > 1000))) {
        // Get Hardware
        c_pkg.setExtCanPkg8 (7, 0, 231, vtSourceAddress, pc_wsMasterIdentItem->getIsoItem()->nr(),
                             199, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff);
        c_can << c_pkg;      // Command: Get Technical Data --- Parameter: Get Hardware
        vtCapabilities_a.lastRequestedHardware = HAL::getTime();
      }
      if (vtCapabilities_a.lastReceivedSoftkeys && vtCapabilities_a.lastReceivedFont && vtCapabilities_a.lastReceivedHardware && (!localSettings_a.lastReceived) && ((localSettings_a.lastRequested == 0) || ((HAL::getTime()-localSettings_a.lastRequested) > 1000))) {
        // Get Local Settings (may not be reached, when terminal is switched on after ECU, as VT sends LNAGUAGE Info on startup!
        c_pkg.setExtCanPkg3 (6, 0, 234, vtSourceAddress, pc_wsMasterIdentItem->getIsoItem()->nr(),
                             (LANGUAGE_PGN & 0xFF), ((LANGUAGE_PGN >> 8)& 0xFF), ((LANGUAGE_PGN >> 16)& 0xFF));
        c_can << c_pkg;      // Command: Request_PGN
        localSettings_a.lastRequested = HAL::getTime();
      }
    }
    else // if (vtCapabilities_a.lastReceivedFont && vtCapabilities_a.lastReceivedHardware && vtCapabilities_a.lastReceivedSoftkeys && localSettings_a.lastReceived)
    {
      // ### Do we have to start an object pool transfer, because ws has just been uploaded or do we have to resend?
      if ( ( ((en_uploadState == UFailed) && (((uint32_t) HAL::getTime()) > (ui32_uploadTimeout + ui32_uploadTimestamp)))
           || (en_uploadState == UIdle) )) {
        // Do we want to try to "Load Version" or go directly to uploading?
        if (pc_versionLabel != NULL) {
          // send out "Non Volatile Memory - Load Version"
          c_pkg.setExtCanPkg8 (7, 0, 231, vtSourceAddress, pc_wsMasterIdentItem->getIsoItem()->nr(), 
                               #ifdef LOESCHE_POOL
                               210, pc_versionLabel [0], pc_versionLabel [1], pc_versionLabel [2], pc_versionLabel [3], pc_versionLabel [4], pc_versionLabel [5], pc_versionLabel [6]);
                               #else
                               209, pc_versionLabel [0], pc_versionLabel [1], pc_versionLabel [2], pc_versionLabel [3], pc_versionLabel [4], pc_versionLabel [5], pc_versionLabel [6]);
                               #endif
          c_can << c_pkg;      // Command: Non Volatile Memory --- Parameter: Load Version
                               //(Command: Non Volatile Memory --- Parameter: Delete Version - just a quick hack!)
          // start uploading after reception of LoadVersion Response
          en_uploadState = UWaitingForLoadVersionResponse;
          ui32_uploadTimeout = 1000;
          ui32_uploadTimestamp = HAL::getTime();
        } else {
          // start uploading right now
          startObjectPoolUploading ();
        }
      }
    }
  }

  // ### Is anything busy or can we do some action now?
  if ((en_uploadState == UIdle) && (en_sendCommandState != WaitingForCommandResponse)) {
    // ### High Priority: Do we have Uploads (String, etc.) to do?
    if (!q_sendUpload.empty()) {
      en_uploadType = UploadChangeStringValue;
      en_uploadState = UUploading;
      // set retry value and start LbsMultiSend
      SendUpload_c* actSend = &q_sendUpload.front();
      ui8_uploadRetry = actSend->ui8_retryCount;

      startUpload (actSend);

      // not directly here, we need to keep the buffer until the upload completes...
     // q_sendUpload.pop();
    }
    else // ### Low Priority: If no Upload is running: Do we a) have no upload running, b) another command to go and c) don't expect a response?
    if (!q_sendCommand.empty()) {
      // now we go to state sending and set the timeout, so we can change to state TimedOut if we get no response after ui23_timeout..
      en_sendCommandState = WaitingForCommandResponse;
      SendCommand_c* actSend = &q_sendCommand.front();
      ui32_sendCommandTimeout = actSend->ui32_commandTimeout;
      ui32_sendCommandTimestamp = HAL::getTime();
      ui8_commandParameter = actSend->arr_commandBuffer [0];

      c_pkg.setExtCanPkg8 (7, 0, 231, vtSourceAddress, pc_wsMasterIdentItem->getIsoItem()->nr(), 
                           actSend->arr_commandBuffer [0], actSend->arr_commandBuffer [1], actSend->arr_commandBuffer [2], actSend->arr_commandBuffer [3], actSend->arr_commandBuffer [4], actSend->arr_commandBuffer [5], actSend->arr_commandBuffer [6], actSend->arr_commandBuffer [7]);
      c_can << c_pkg;      // Command: actSend->arr_commandBuffer [0]
      #ifdef USE_LIST_FOR_FIFO
      // queueing with list: queue::push <-> list::push_back; queue::front<->list::front; queue::pop<->list::pop_front
      q_sendCommand.pop_front();
      #else
      q_sendCommand.pop();
      #endif
      #ifdef DEBUG_HEAP_USEAGE
      sui16_sendCommandQueueSize--;
      #endif
    }
  }
  return true;
}


/**
  call to check if at least one vt_statusMessage has arrived so we know if the terminal is there.
  @return true -> >= 1 vt_statusMessages have arrived -> terminal is there.
*/
bool ISOTerminal_c::isVtActive ()
{
  /** @todo connction management expansion: recognize NACKs following the WS Maintenance Message! */
  uint32_t curTime = HAL::getTime();
  if (vtState_a.lastReceived) {
    if ((curTime-vtState_a.lastReceived) <= 3000) {
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
void ISOTerminal_c::saveDataNextStreamPart ()
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
void ISOTerminal_c::restoreDataNextStreamPart ()
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
void ISOTerminal_c::resetDataNextStreamPart ()
{
  pc_iterObjects = pc_pool->getIVtObjects();
  ui32_objectStreamPosition = 0;
  uploadBufferPosition = 0;
  uploadBufferFilled = 1;
  uploadBuffer [0] = 17;
}

/** place next data to send direct into send puffer of pointed
    stream send package - MultiSendStreamer_c will send this
    puffer afterwards
    - implementation of the abstract IsoAgLib::MultiSendStreamer_c function
  */
void ISOTerminal_c::setDataNextStreamPart (MultiSendPkg_c* mspData, uint8_t bytes)
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
  CANIO_c& c_can = getCanInstance4Comm();
  ISOMonitor_c& c_isoMonitor = getIsoMonitorInstance4Comm();
  ISOSystemPkg_c& c_pkg = c_isoMonitor.data(); // same data as in processMsg here!!

  // calculate mask_stream size NOW (added 1 byte for "Object Pool Upload Start" Command Byte)
  // because we can't do before we get the color-depth information (0xC7)
  ui32_streamSize = 1;
  for (uint32_t curObject=0; curObject < pc_pool->getNumObjects(); curObject++) {
    ui32_streamSize += ((vtObject_c*)pc_pool->getIVtObjects()[curObject])->fitTerminal ();
  }

  c_pkg.setExtCanPkg8(7, 0, 231, vtSourceAddress, pc_wsMasterIdentItem->getIsoItem()->nr(), 
                      192, 0xff, (ui32_streamSize-1) & 0xFF, ((ui32_streamSize-1) >>  8) & 0xFF, ((ui32_streamSize-1) >> 16) & 0xFF, (ui32_streamSize-1) >> 24, 0xff, 0xff);
  c_can << c_pkg;     // Command: Get Technical Data --- Parameter: Get Memory Size

  // Now start uploading
  en_uploadType = UploadObjectPool;
  en_uploadState = UWaitingForMemoryResponse;
  ui32_uploadTimeout = 1000;
  ui32_uploadTimestamp = HAL::getTime();
}


void ISOTerminal_c::finalizeUploading ()
{
  en_uploadState = UIdle; // == uploaded (successfully)
  en_objectPoolState = OPUploadedSuccessfully;
  if (pc_pool != NULL) {
    pc_pool->eventObjectPoolUploadedSuccessfully ();
  }
}

/** Send "End of Object Pool" message */
void ISOTerminal_c::indicateObjectPoolCompletion ()
{
  CANIO_c& c_can = getCanInstance4Comm();
  ISOMonitor_c& c_isoMonitor = getIsoMonitorInstance4Comm();
  ISOSystemPkg_c& c_pkg = c_isoMonitor.data(); // same data as in processMsg here!!

  // successfully sent, so now send out the "End of Object Pool Message" and wait for response!
  c_pkg.setExtCanPkg8(7, 0, 231, vtSourceAddress, pc_wsMasterIdentItem->getIsoItem()->nr(), 
                      18, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff);
  c_can << c_pkg;     // Command: Object Pool Transfer --- Parameter: Object Pool Ready
  en_uploadState = UWaitingForEOOResponse; // and wait for response to set en_uploadState back to UploadIdle;
  ui32_uploadTimeout = 10000; // wait 10 seconds for terminal to initialize pool!
  ui32_uploadTimestamp = HAL::getTime();
}


/** process received can messages */
bool ISOTerminal_c::processMsg()
{
  CANIO_c& c_can = getCanInstance4Comm();
  ISOMonitor_c& c_isoMonitor = getIsoMonitorInstance4Comm();
  ISOSystemPkg_c& c_pkg = c_isoMonitor.data(); // same data as in processMsg here!!

  bool b_result = false;

#define MACRO_setStateDependantOnError \
  if (ui8_commandParameter == data().getUint8Data(0)) { \
    /* okay, right response for our command! */ \
    if (ui8_sendCommandError == 0) { \
      /* no error */ \
      en_sendCommandState = SendCommandIdle; \
    } else { \
      /* some error */ \
      en_sendCommandState = SendCommandFailed; \
    } \
  }

  // VT_TO_GLOBAL is the only PGN we accept without VT being active, because it marks the VT active!!
        
        //////////////////////////////////////////
       // vt to global? -->VT_TO_GLOBAL_PGN<-- //
      //////////////////////////////////////////
  if ((data().isoPgn() & 0x1FFFF) == VT_TO_GLOBAL_PGN) {
    switch (data().getUint8Data (0)) {
      case 0xFE: // Command: "Status", Parameter: "VT Status Message"
        vtState_a.lastReceived = data().time();
        vtState_a.saOfActiveWorkingSetMaster = data().getUint8Data (1);
        vtState_a.dataAlarmMask = data().getUint8Data (2) | (data().getUint8Data (3) << 8);
        vtState_a.softKeyMask = data().getUint8Data (4) | (data().getUint8Data (5) << 8);
        vtState_a.busyCodes = data().getUint8Data (6);
        vtState_a.functionBusy = data().getUint8Data (7);
        // get source address of virtual terminal
        vtSourceAddress = data().isoSa();
        break;
      default:
        break;
    }
    return true;
  }
        
        
  // If VT is not active, don't react on PKGs addressed to us, as VG's not active ;)
  if (!isVtActive()) return true;

  // If no pool registered, do nothing!
  if (en_objectPoolState == OPNoneRegistered) return true;
  
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
    // The other fields are reserved.
    return true;
  }
  
        //////////////////////////////////////////
       // VT to this ECU? -->VT_TO_ECU_PGN<-- ///
      //////////////////////////////////////////
  if ( pc_wsMasterIdentItem->getIsoItem() && ((data().isoPgn() & 0x1FFFF) == (uint32_t) (VT_TO_ECU_PGN + pc_wsMasterIdentItem->getIsoItem()->nr())) ) {
    
    
    switch (data().getUint8Data (0)) {
    /*** ### VT Initiated Messages ### ***/
      case 0x00: // Command: "Control Element Function", parameter "Soft Key"
      case 0x01: // Command: "Control Element Function", parameter "Button"
        if (pc_pool) {
          pc_pool->eventKeyCode (data().getUint8Data (1) /* key activation code (pressed, released, held) */,
                                 data().getUint8Data (2) | (data().getUint8Data (3) << 8) /* objID of key object */,
                                 data().getUint8Data (4) | (data().getUint8Data (5) << 8) /* objID of visible mask */,
                                 data().getUint8Data (6) /* key code */,
                                 data().getUint8Data (0) /* 0 for sk, 1 for button -- matches wasButton? boolean */ );
        }
        b_result = true;
        break;
      case 0x05: // Command: "Control Element Function", parameter "VT Change Numeric Value"
        if (pc_pool) {
          pc_pool->eventNumericValue (uint16_t( data().getUint8Data (1) ) | (uint16_t( data().getUint8Data (2) ) << 8) /* objID */,
                                      data().getUint8Data (4) /* 1 byte value */,
                                      uint32_t( data().getUint8Data (4) ) | (uint32_t( data().getUint8Data (5) ) << 8) | (uint32_t( data().getUint8Data (6) ) << 16)| (uint32_t( data().getUint8Data (7) ) << 24) /* 4 byte value */ );
        }
        b_result = true;
        break;

    /***************************************************/
    /*** ### ECU Initiated Messages (=Responses) ### ***/
      case 0x12: // Command: "End of Object Pool Transfer", parameter "Object Pool Ready Response"
        if (en_uploadState == UWaitingForEOOResponse) {
          if (data().getUint8Data (1) == 0) {
            if (pc_versionLabel != NULL) {
              // Store Version and finalize after "Store Version Response"
              c_pkg.setExtCanPkg8(7, 0, 231, vtSourceAddress, pc_wsMasterIdentItem->getIsoItem()->nr(), 
                                  208 /* D0 */, pc_versionLabel [0], pc_versionLabel [1], pc_versionLabel [2], pc_versionLabel [3], pc_versionLabel [4], pc_versionLabel [5], pc_versionLabel [6]);
              c_can << c_pkg;     // Command: Non Volatile Memory --- Parameter: Store Version

              // Now wait for response
              en_uploadState = UWaitingForStoreVersionResponse;
              ui32_uploadTimeout = 3000;
              ui32_uploadTimestamp = HAL::getTime();
            } else {
              // Finalize now!
              finalizeUploading ();
            }
          } else {
            en_uploadState = UFailed; // errorcode in ui8_uploadError;
            en_objectPoolState = OPCannotBeUploaded;
            ui8_uploadError = data().getUint8Data (2);
          }
        } else {
          // we should be in send mode when receiving an end of objectpool message.. ;)
        }
        b_result = true;
        break;
      // ### Error field is also on byte 4 (index 3)
      case 0xA8: // Command: "Command", parameter "Change Numeric Value Response"
      case 0xAD: // Command: "Command", parameter "Change Active Mask Response"
        if (en_sendCommandState != SendCommandIdle) { // if Waiting or Timedout (or Failed <shouldn't happen>)
          ui8_sendCommandError = data().getUint8Data(3);
          MACRO_setStateDependantOnError
        }
        // don't promote response state back to Container object (for now)
        b_result = true;
        break;
      // ### Error field is also on byte nr. 5 (index 4)
      case 0xA0: // Command: "Command", parameter "Hide/Show Object Response" (Container)
      case 0xA1: // Command: "Command", parameter "Enable/Disable Object Response" (Input Object)
      case 0xA2: // Command: "Command", parameter "Select Input Object Response"
      case 0xAF: // Command: "Command", parameter "Change Attribute Response"
        if (en_sendCommandState != SendCommandIdle) { // if Waiting or Timedout (or Failed <shouldn't happen>)
          ui8_sendCommandError = data().getUint8Data(4);
          MACRO_setStateDependantOnError
        }
        b_result = true;
        break;
      // ### Error field is also on byte 6 (index 5)
      case 0xAE: // Command: "Command", parameter "Changee Soft Key Mask Response"
        if (en_sendCommandState != SendCommandIdle) { // if Waiting or Timedout (or Failed <shouldn't happen>)
          ui8_sendCommandError = data().getUint8Data(5);
          MACRO_setStateDependantOnError
        }
        b_result = true;
        break;
      // ### Error field is on byte 7 (index 6)
      case 0xB1: // Command: "Command", parameter "Change List Item Response"
        if (en_sendCommandState != SendCommandIdle) { // if Waiting or Timedout (or Failed <shouldn't happen>)
          ui8_sendCommandError = data().getUint8Data(6);
          MACRO_setStateDependantOnError
        }
        b_result = true;
        break;

      case 0xC0: // Command: "Get Technical Data", parameter "Get Memory Size Response"
        iso11783version = data().getUint8Data (1);
        if (en_uploadState == UWaitingForMemoryResponse) {
          if (data().getUint8Data (2) == 0) {
            // start uploading, there MAY BE enough memory
            en_uploadState = UUploading;
            getMultiSendInstance().sendIsoTarget(pc_wsMasterIdentItem->getIsoItem()->nr(), vtSourceAddress, this, ECU_TO_VT_PGN, &en_sendSuccess);
          } else {
            // definitely NOT enough memory
            getLbsErrInstance().registerError( LibErr_c::IsoTerminalOutOfMemory, LibErr_c::IsoTerminal );
            en_uploadState = UFailed;
            en_objectPoolState = OPCannotBeUploaded;
            // en_uploadTimestamp = HAL::getTime();
            // en_uploadTimeout = 5000; // not needed
          }
        }
        b_result = true;
        break;
      case 0xC2: // Command: "Get Technical Data", parameter "Get Number Of Soft Keys Response"
        vtCapabilities_a.skWidth = /*60;*/data().getUint8Data (4);
        vtCapabilities_a.skHeight = /*47; */data().getUint8Data (5);
        vtCapabilities_a.skVirtual = data().getUint8Data (6);
        vtCapabilities_a.skPhysical = data().getUint8Data (7);
        vtCapabilities_a.lastReceivedSoftkeys = HAL::getTime();
        b_result = true;
        break;
      case 0xC3: // Command: "Get Technical Data", parameter "Get Text Font Data Response"
        vtCapabilities_a.fontSizes = /*1023; */(data().getUint8Data (5) << 1) | 0x01; // 'cause "6x8" is always available!
        vtCapabilities_a.fontSizes += data().getUint8Data (6) << 8; // so we leave out the "Not used" bit!!
        vtCapabilities_a.fontTypes = /*22; */data().getUint8Data (7);
        vtCapabilities_a.lastReceivedFont = HAL::getTime();
        b_result = true;
        break;
      case 0xC7: // Command: "Get Technical Data", parameter "Get Hardware Response"
        vtCapabilities_a.hwGraphicType = /*0; */data().getUint8Data (2);
        vtCapabilities_a.hwHardware = data().getUint8Data (3);
        vtCapabilities_a.hwWidth = data().getUint8Data (4) + (data().getUint8Data (5) << 8);
        vtCapabilities_a.hwHeight = data().getUint8Data (6) + (data().getUint8Data (7) << 8);
        vtCapabilities_a.lastReceivedHardware = HAL::getTime();
        b_result = true;
        break;
      case 0xD0: // Command: "Non Volatile Memory", parameter "Store Version Response"
        if (en_uploadState == UWaitingForStoreVersionResponse) {
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
        b_result = true;
        break;
      case 0xD1: // Command: "Non Volatile Memory", parameter "Load Version Response"
        if (en_uploadState == UWaitingForLoadVersionResponse) {
          if ((data().getUint8Data (5) & 0x0F) == 0) {
            // Successfully loaded
            finalizeUploading ();
          } else {
/*        if ((data().getUint8Data (5) & 0x04) == 1) { case 4: // Insufficient memory available
            getLbsErrInstance().registerError( LibErr_c::IsoTerminalOutOfMemory, LibErr_c::IsoTerminal );
            en_uploadState = UFailed;
            en_objectPoolState = OPCannotBeUploaded;
            // en_uploadTimestamp = HAL::getTime();
            // en_uploadTimeout = 5000; // not needed
            break;
          }
*/
            // Not used
            // General error
            // Version label not known
            // None stored, so we have to send it! send out "Get Technical Data - Get Memory Size"
            startObjectPoolUploading ();
          }
        }
        b_result = true;
        break;
    } // switch

    /* Acknowledge VT->ECU Initiated Messages (Soft Key, Button, VTChangeNumericValue etc.) */
    if (/* always true comparison ;-) (data().getUint8Data (0) >= 0x00) && */(data().getUint8Data (0) <= 0x07)) {
      // for now only ack SoftKeyActivation Messages !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      CANPkg_c::setIdentType(Ident_c::ExtendedIdent);
      uint8_t swapTemp=data().isoPs();
      c_pkg.setIsoPs (data().isoSa());
      c_pkg.setIsoSa (swapTemp);
      c_can << c_pkg;
    }
  }

  return b_result;
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
  @returns true if there was place in the SendCommand-Buffer (should always be the case now)
*/
bool ISOTerminal_c::sendCommand (uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, uint8_t byte5, uint8_t byte6, uint8_t byte7, uint8_t byte8, uint32_t ui32_timeout)
{
  #ifdef USE_LIST_FOR_FIFO
  // queueing with list: queue::push <-> list::push_back; queue::front<->list::front; queue::pop<->list::pop_front
  q_sendCommand.push_back (SendCommand_c (byte1, byte2, byte3, byte4, byte5, byte6, byte7, byte8, ui32_timeout));
  #else
  q_sendCommand.push (SendCommand_c (byte1, byte2, byte3, byte4, byte5, byte6, byte7, byte8, ui32_timeout));
  #endif
  #ifdef DEBUG_HEAP_USEAGE
  sui16_sendCommandQueueSize++;
  if ( sui16_sendCommandQueueSize > sui16_maxSendCommandQueueSize )
    sui16_maxSendCommandQueueSize = sui16_sendCommandQueueSize;
  #endif
  return true;  // return false somewhen???????? buffer tooo full??
}


// THIS FUNCTION ADDED BY BRAD COX 26-AUG-2004 TO IMPLEMENT CHANGE CHILD LOCATION COMMAND
// //////////////////////////////// +X2C Operation BAC : sendCommandChangeChildLocation
bool ISOTerminal_c::sendCommandChangeChildLocation (IsoAgLib::iVtObject_c* rpc_object, IsoAgLib::iVtObject_c* rpc_childObject, int8_t dx, int8_t dy)
{
  return sendCommand (165 /* Command: Command --- Parameter: Change Child Location */,
                      rpc_object->getID() & 0xFF, rpc_object->getID() >> 8,
                      rpc_childObject->getID() & 0xFF, rpc_childObject->getID() >> 8,
                      dx+127, dy+127, 0xFF,
                      1000 /* timeout value */);
}

// THIS FUNCTION ADDED BY BRAD COX 26-AUG-2004 TO IMPLEMENT CHANGE SIZE COMMAND
// //////////////////////////////// +X2C Operation BAC :Operation: sendCommandChangeSize
bool ISOTerminal_c::sendCommandChangeSize(IsoAgLib::iVtObject_c* rpc_object,uint16_t newWidth, uint16_t newHeight)
{
  return sendCommand (166 /* Command: Command --- Parameter: Change Size */,
                      rpc_object->getID() & 0xFF, rpc_object->getID() >> 8,
                      newWidth & 0xFF, newWidth >> 8,
                      newHeight & 0xFF, newHeight >> 8,
                      0xFF,
                      1000 /* timeout value */);
}

// THIS FUNCTION ADDED BY BRAD COX 26-AUG-2004 TO IMPLEMENT CHANGE BACKGROUND COLOUR COMMAND
// //////////////////////////////// +X2C Operation BAC : sendCommandChangeBackgroundColour
bool ISOTerminal_c::sendCommandChangeBackgroundColour(IsoAgLib::iVtObject_c* rpc_object, uint8_t newColour)
{
  return sendCommand (167 /* Command: Command --- Parameter: Change Background Color */,
                      rpc_object->getID() & 0xFF, rpc_object->getID() >> 8,
                      newColour, 0xFF, 0xFF, 0xFF, 0xFF,
                      1000 /* timeout value */);
}

bool ISOTerminal_c::sendCommandChangeNumericValue (IsoAgLib::iVtObject_c* rpc_object, uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4)
{
  return sendCommand (168 /* Command: Command --- Parameter: Change Numeric Value */,
                      rpc_object->getID() & 0xFF, rpc_object->getID() >> 8,
                      0x00, byte1, byte2, byte3, byte4,
                      1000 /* timeout value */);
}

bool ISOTerminal_c::sendCommandChangeEndPoint(IsoAgLib::iVtObject_c* rpc_object,uint16_t newWidth, uint16_t newHeight, uint8_t newLineAttributes)
{
  return sendCommand (169 /* Command: Command --- Parameter: Change Size */,
          rpc_object->getID() & 0xFF, rpc_object->getID() >> 8,
          newWidth & 0xFF, newWidth >> 8,
          newHeight & 0xFF, newHeight >> 8,
          newLineAttributes,
          1000 /* timeout value */);
}

bool ISOTerminal_c::sendCommandChangeFontAttributes (IsoAgLib::iVtObject_c* rpc_object, uint8_t newFontColour, uint8_t newFontSize, uint8_t newFontType, uint8_t newFontStyle)
{
  return sendCommand (170 /* Command: Command --- Parameter: Change FontAttributes */,
                      rpc_object->getID() & 0xFF, rpc_object->getID() >> 8,
                      newFontColour, newFontSize, newFontType, newFontStyle, 0xFF,
                      1000 /* timeout value */);
}

bool ISOTerminal_c::sendCommandChangeLineAttributes (IsoAgLib::iVtObject_c* rpc_object, uint8_t newLineColour, uint8_t newLineWidth, uint16_t newLineArt)
{
  return sendCommand (171 /* Command: Command --- Parameter: Change LineAttributes */,
                     rpc_object->getID() & 0xFF, rpc_object->getID() >> 8,
                     newLineColour, newLineWidth, newLineArt & 0xFF, newLineArt >> 8, 0xFF,
                     1000 /* timeout value */);
}

// THIS FUNCTION ADDED BY BRAD COX 17-SEP-2004 TO IMPLEMENT CHANGE FILL ATTRIBUTES COMMAND
// //////////////////////////////// +X2C Operation BAC :Operation: sendCommandChangeSize
bool ISOTerminal_c::sendCommandChangeFillAttributes (IsoAgLib::iVtObject_c* rpc_object, uint8_t newFillType, uint8_t newFillColour, IsoAgLib::iVtObjectPictureGraphic_c* newFillPatternObject)
{
  return sendCommand (172 /* Command: Command --- Parameter: Change FillAttributes */,
                      rpc_object->getID() & 0xFF, rpc_object->getID() >> 8,
                      newFillType, newFillColour,
                      (newFillType == 3) ? newFillPatternObject->getID() & 0xFF : 0xFF,
                      (newFillType == 3) ? newFillPatternObject->getID() >> 8 : 0xFF,
                      0xFF,
                      1000 /* timeout value */);
}

bool ISOTerminal_c::sendCommandChangeSoftKeyMask (IsoAgLib::iVtObject_c* rpc_object, uint8_t maskType, uint16_t newSoftKeyMask)
{
  return sendCommand (174 /* Command: Command --- Parameter: Change Soft Key Mask */,
                      maskType,
                      rpc_object->getID() & 0xFF, rpc_object->getID() >> 8,
                      newSoftKeyMask & 0xFF, newSoftKeyMask >> 8,
                      0xFF, 0xFF,
                      1000 /* timeout value */);
}

bool ISOTerminal_c::sendCommandChangeAttribute (IsoAgLib::iVtObject_c* rpc_object, uint8_t attrId, uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4)
{
  return sendCommand (175 /* Command: Command --- Parameter: Change Attribute */,
                      rpc_object->getID() & 0xFF, rpc_object->getID() >> 8,
                      attrId, byte1, byte2, byte3, byte4,
                      1000 /* timeout value */);
}

// THIS FUNCTION ADDED BY BRAD COX 26-AUG-2004 TO IMPLEMENT CHANGE PRIORITY COMMAND
// //////////////////////////////// +X2C Operation BAC :Operation: sendCommandChangePriority
bool ISOTerminal_c::sendCommandChangePriority(IsoAgLib::iVtObject_c* rpc_object, int8_t newPriority)
{
  if(newPriority < 3) {
    // only bother to send if priority is a legal value
    return sendCommand (176 /* Command: Command --- Parameter: Change Priority */,
                      rpc_object->getID() & 0xFF, rpc_object->getID() >> 8,
                      newPriority, 0xFF, 0xFF, 0xFF, 0xFF,
                      1000 /* timeout value */);
  } else {
    return false;
  }
}

bool ISOTerminal_c::sendCommandChangeStringValue (IsoAgLib::iVtObject_c* rpc_object, const char* rpc_newValue, uint16_t overrideSendLength)
{
  /* The SendUpload_c constructor makes a copy of the string! */
  #ifdef USE_LIST_FOR_FIFO
  // queueing with list: queue::push <-> list::push_back; queue::front<->list::front; queue::pop<->list::pop_front
  q_sendUpload.push_back (SendUpload_c (rpc_object->getID(), rpc_newValue, overrideSendLength, 2));
  #else
  q_sendUpload.push (SendUpload_c (rpc_object->getID(), rpc_newValue, overrideSendLength, 2));
  #endif
  #ifdef DEBUG_HEAP_USEAGE
  sui16_sendUploadQueueSize++;
  if ( sui16_sendUploadQueueSize > sui16_maxSendUploadQueueSize )
    sui16_maxSendUploadQueueSize = sui16_sendUploadQueueSize;
  #endif
  /** push(...) has no return value */
  return true;
}

bool ISOTerminal_c::sendCommandChangeStringValue (IsoAgLib::iVtObjectString_c* rpc_objectString)
{
  /* The SendUpload_c constructor only takes a reference, so don't change the string in the meantime!!! */
  #ifdef USE_LIST_FOR_FIFO
  // queueing with list: queue::push <-> list::push_back; queue::front<->list::front; queue::pop<->list::pop_front
  q_sendUpload.push_back (SendUpload_c (rpc_objectString, 2));
  #else
  q_sendUpload.push (SendUpload_c (rpc_objectString, 2));
  #endif
  #ifdef DEBUG_HEAP_USEAGE
  sui16_sendUploadQueueSize++;
  if ( sui16_sendUploadQueueSize > sui16_maxSendUploadQueueSize )
    sui16_maxSendUploadQueueSize = sui16_sendUploadQueueSize;
  #endif
  /** push(...) has no return value */
  return true;
}

uint32_t ISOTerminal_c::getUploadBufferSize ()
{
  return q_sendUpload.size();
}

} // end namespace __IsoAgLib
