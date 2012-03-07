/*
  multisend_c.cpp: transfer of multi message data

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include "multisend_c.h"

#include <IsoAgLib/driver/system/impl/system_c.h>
#include <IsoAgLib/scheduler/impl/scheduler_c.h>
#include <IsoAgLib/comm/impl/isobus_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isofiltermanager_c.h>
#include <IsoAgLib/util/iassert.h>

#include "../imultisendstreamer_c.h"

#if DEBUG_MULTISEND || DEBUG_HEAP_USEAGE
  #ifdef SYSTEM_PC
    #include <iostream>
  #else
    #include <supplementary_driver/driver/rs232/impl/rs232io_c.h>
  #endif
#endif

#if defined(_MSC_VER)
#pragma warning( disable : 4355 )
#endif

#if DEBUG_HEAP_USEAGE
  static uint16_t sui16_lastPrintedBufferCapacity = 0;
#endif

/** @todo SOON-178 figure that one out... new ISO says we can put out head2head messages! */
static const uint8_t scui8_isoCanPkgDelay = 4;



namespace __IsoAgLib {
/** C-style function, to get access to the unique MultiSend_c singleton instance
  * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
  */
MultiSend_c &getMultiSendInstance( uint8_t aui8_instance )
{
  MACRO_MULTITON_GET_INSTANCE_BODY(MultiSend_c, PRT_INSTANCE_CNT, aui8_instance);
}

/** This is mostly used for debugging now... */
void SendUploadBase_c::set (uint8_t* apui8_buffer, uint32_t aui32_bufferSize)
{
  /// Use BUFFER - NOT MultiSendStreamer!
  vec_uploadBuffer.clear();
  vec_uploadBuffer.reserve (aui32_bufferSize);

  uint32_t i=0;
  for (; i < aui32_bufferSize; i++) {
    vec_uploadBuffer.push_back (*apui8_buffer);
    apui8_buffer++;
  }
  for (; i < 8; i++) {
    vec_uploadBuffer.push_back (0xFF);
  }

  #if DEBUG_HEAP_USEAGE
  if ( vec_uploadBuffer.capacity() != sui16_lastPrintedBufferCapacity )
  {
    sui16_lastPrintedBufferCapacity = vec_uploadBuffer.capacity();
    INTERNAL_DEBUG_DEVICE << "IsoTerminal_c Buffer-Capa: " << sui16_lastPrintedBufferCapacity << INTERNAL_DEBUG_DEVICE_ENDL;
  }
  #endif
}


/**
  Constructor used for "normal" 8-byte CAN-Pkgs!
*/
void SendUploadBase_c::set (uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, uint8_t byte5, uint8_t byte6, uint8_t byte7, uint8_t byte8)
{
  vec_uploadBuffer.clear();
  vec_uploadBuffer.reserve (8);

  vec_uploadBuffer.push_back (byte1);
  vec_uploadBuffer.push_back (byte2);
  vec_uploadBuffer.push_back (byte3);
  vec_uploadBuffer.push_back (byte4);
  vec_uploadBuffer.push_back (byte5);
  vec_uploadBuffer.push_back (byte6);
  vec_uploadBuffer.push_back (byte7);
  vec_uploadBuffer.push_back (byte8);
}

/**
  Constructor used for "ChangeChildPosition" >> 9 <<-byte CAN-Pkgs!
  -- Parameter "timeOut" only there as else the signature would be the same compared to 8byte+timeOut constructor!
  -- simply always pass "DEF_TimeOut_ChangeChildPosition"
*/
void SendUploadBase_c::set (uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, uint8_t byte5, uint8_t byte6, uint8_t byte7, uint8_t byte8, uint8_t byte9)
{
  /// Use BUFFER - NOT MultiSendStreamer!
  vec_uploadBuffer.clear();
  vec_uploadBuffer.reserve (9);

  vec_uploadBuffer.push_back (byte1);
  vec_uploadBuffer.push_back (byte2);
  vec_uploadBuffer.push_back (byte3);
  vec_uploadBuffer.push_back (byte4);
  vec_uploadBuffer.push_back (byte5);
  vec_uploadBuffer.push_back (byte6);
  vec_uploadBuffer.push_back (byte7);
  vec_uploadBuffer.push_back (byte8);
  vec_uploadBuffer.push_back (byte9);
}

const SendUploadBase_c& SendUploadBase_c::operator= (const SendUploadBase_c& r_source)
{
  vec_uploadBuffer = r_source.vec_uploadBuffer;
  return r_source;
}


SendUploadBase_c::SendUploadBase_c (const SendUploadBase_c& r_source)
  : vec_uploadBuffer (r_source.vec_uploadBuffer)
{}




  /************************************/
 /**** MultiSend_c Implementation ****/
/************************************/

// SendStream subclass implementation
/////////////////////////////////////

void
MultiSend_c::init()
{
  isoaglib_assert (!initialized());

  getSchedulerInstance().registerClient( this );
  getIsoMonitorInstance4Comm().registerControlFunctionStateHandler( mt_handler );

  #if defined(ENABLE_MULTIPACKET_VARIANT_FAST_PACKET)
  mui8_nextFpSequenceCounter = 0;
  #endif

  // Receive filters for ISO 11783 are created selectively in the SA claim handler,
  // so that only messages that are directed to a local SA are received

  setInitialized();
}


void
MultiSend_c::close()
{
  isoaglib_assert (initialized());

  getIsoMonitorInstance4Comm().deregisterControlFunctionStateHandler( mt_handler );
  getSchedulerInstance().unregisterClient( this );

  /// For right now, we do gracefully kill all interrupted stream,
  /// but normally the modules should abort thier own sending when they
  /// get stopped... @todo Check that some day, for now it's okay though.
  mlist_sendStream.clear();

  // if not empty, some modules have not properly closed down its send-streams!
  isoaglib_assert (mlist_sendStream.empty());

  setClosed();
}


/**
  @return an "in-progress" stream or NULL if none active for this sa/da-key
*/
SendStream_c*
MultiSend_c::getSendStream(const IsoName_c& acrc_isoNameSender, const IsoName_c& acrc_isoNameReceiver)
{
  for (STL_NAMESPACE::list<SendStream_c>::iterator pc_iter=mlist_sendStream.begin(); pc_iter != mlist_sendStream.end(); pc_iter++)
  {
    if (pc_iter->matchSaDa(acrc_isoNameSender, acrc_isoNameReceiver))
      return &*pc_iter;
  }
  return NULL;
}



/**
  use this function to add a new SendStream.
  IMPORTANT: Assure that the added SendStream is initialized right after this call!!
  @return reference to added SendStream ==> HAS TO BE INITIALIZED, because it may be a copy of the first (to avoid stack creation of new object)
*/
SendStream_c*
MultiSend_c::addSendStream(const IsoName_c& acrc_isoNameSender, const IsoName_c& acrc_isoNameReceiver)
{
  SendStream_c* const pc_foundStream = getSendStream(acrc_isoNameSender, acrc_isoNameReceiver);
  if (pc_foundStream)
  {
    if (!pc_foundStream->isFinished())
    {
      return NULL; // can't start a sendStream, one already active for this one..
    }
    else // use this finished one because it would be deleted anyway...
    {
      #if DEBUG_MULTISEND
      INTERNAL_DEBUG_DEVICE << "Using Stream from list which is already finished but not yet kicked from MultiSend_c::timeEvent()."<< INTERNAL_DEBUG_DEVICE_ENDL;
      #endif
      return pc_foundStream;
    }
  }
  mlist_sendStream.push_back (SendStream_c(*this MULTITON_INST_WITH_COMMA ));
  return &mlist_sendStream.back();
}



bool
MultiSend_c::sendIntern (const IsoName_c& isoNameSender, const IsoName_c& isoNameReceiver, const HUGE_MEM uint8_t* rhpb_data, uint32_t aui32_dataSize, int32_t ai32_pgn, IsoAgLib::iMultiSendStreamer_c* apc_mss, SendStream_c::msgType_t ren_msgType, MultiSendEventHandler_c* apc_multiSendEventHandler)
{
  isoaglib_assert( aui32_dataSize >= endSinglePacketSize );
  isoaglib_assert( (ren_msgType != SendStream_c::NmeaFastPacket) || (aui32_dataSize < endNmeaFastPacketSize) );
  isoaglib_assert( (ren_msgType != SendStream_c::IsoTPbroadcast) || (aui32_dataSize < beginEtpPacketSize) );
  isoaglib_assert( (ren_msgType != SendStream_c::IsoTP) || (aui32_dataSize < endTpPacketSize) );
  isoaglib_assert( (ren_msgType != SendStream_c::IsoETP) || (aui32_dataSize >= beginEtpPacketSize) );
  // end of preconditions.

  /// first check if new transfer can be started
  /// - is the sender correct?
  if( !getIsoMonitorInstance4Comm().existIsoMemberISOName( isoNameSender ) )
    return false;

  // - is the receiver correct?:
  switch (ren_msgType) {
  case SendStream_c::IsoTPbroadcast:
  case SendStream_c::NmeaFastPacket:
    // Destination must be "Unspecified"
    isoaglib_assert( isoNameReceiver.isUnspecified() );
    if( isoNameReceiver.isSpecified() )
      return false;
    break;
  default:
    // destination specific - so the receiver must be registered!
    if( !getIsoMonitorInstance4Comm().existIsoMemberISOName( isoNameReceiver ) )
      return false;
  }
  /// - check if there's already a SA/DA pair active (in this case NULL is returned!)
  /// - if not NULL is returned, it points to the newly generated stream.
  SendStream_c * const cpc_newSendStream = addSendStream( isoNameSender, isoNameReceiver );
  if (!cpc_newSendStream)
    return false;

  cpc_newSendStream->init( isoNameSender, isoNameReceiver, rhpb_data, aui32_dataSize, ai32_pgn, apc_mss, ren_msgType, apc_multiSendEventHandler );

  // let this SendStream get sorted in now...
  calcAndSetNextTriggerTime();
  return true;
}

/**
  HIDDEN constructor for a MultiSend_c object instance
  NEVER instantiate a variable of type MultiSend_c within application
  only access MultiSend_c via getMultiSendInstance() or
  getMultiSendInstance (protocolInstanceNr) in case more than one
  ISO11783 BUS is used for IsoAgLib
  */
MultiSend_c::MultiSend_c() :
  mt_handler(*this),
  mt_customer(*this)
{}


/**
  perform periodical actions
  @return true -> all planned activities performed in allowed time
*/
bool
MultiSend_c::timeEvent()
{
  if (mlist_sendStream.empty())
  { // nothing to do if no transfer is Running
    setTimePeriod (5000); // actually we could use "infinite here"
    return true;
  }

  /** @todo SOON-178 Check how we want to calculate the max nr. of packets to send
            ==> Best would be to know when the next comes.
            clip that value as we may expect incoming can-pkgs, too - so be a little polite!
  */
  const int32_t ci32_time = Scheduler_Task_c::getLastRetriggerTime();
  // store time of last call, to get time interval between execution
  static int32_t si32_lastCall = 0;
  // only send max 1 package for first call, when execution period can't be derived
  uint8_t ui8_pkgCnt = (si32_lastCall > 0 ) ? ((ci32_time - si32_lastCall)/scui8_isoCanPkgDelay) : 1;
  // update last call time
  si32_lastCall = ci32_time;

  const uint8_t cui8_pkgCntForEach = ui8_pkgCnt / mlist_sendStream.size(); // in case it gets 0 after division, it is set to 1 inside of SendStream's timeEvent().

  int32_t i32_nextRetriggerNeeded = -1; // default to: "no retriggering needed"

  // Call each SendStream_c's timeEvent()
  for (STL_NAMESPACE::list<SendStream_c>::iterator pc_iter=mlist_sendStream.begin(); pc_iter != mlist_sendStream.end();)
  { // only call a SendStream when its time has come!
    if ( pc_iter->isFinished () ||
        (pc_iter->timeHasCome() && (pc_iter->timeEvent (cui8_pkgCntForEach))) )
    { // SendStream finished
      pc_iter = mlist_sendStream.erase (pc_iter);
      #if DEBUG_MULTISEND
      INTERNAL_DEBUG_DEVICE << "Kicked SendStream because it finished (abort or success)!" << INTERNAL_DEBUG_DEVICE_ENDL;
      #endif
    }
    else
    { // SendStream not yet finished
      const int32_t ci32_nextTriggerTime = pc_iter->getNextTriggerTime();
      // needs to be triggered at the following time
      if ((i32_nextRetriggerNeeded == -1) || (ci32_nextTriggerTime < i32_nextRetriggerNeeded))
      { // no trigger yet set or this SendStream needs to come earlier!
        i32_nextRetriggerNeeded = ci32_nextTriggerTime;
      }
      pc_iter++;
    }
  }

  // ALWAYS calculate when we want to be triggered again!
  uint16_t ui16_newTimePeriod = 5000; // default: no SendStream running, idle around with 10 secs.. (actually "unlimited" would be correct - we can sleep!

  if (i32_nextRetriggerNeeded > -1) // "!= -1"
  { // HARD-timing, we need to come to action then!
    int32_t i32_delta = i32_nextRetriggerNeeded-System_c::getTime();
    if (i32_delta < 0) i32_delta = 0;
    ui16_newTimePeriod = i32_delta;
  }
  setTimePeriod (ui16_newTimePeriod);

  return true;
};


//! Function set ui16_earlierInterval and
//! ui16_laterInterval that will be used by
//! getTimeToNextTrigger(retriggerType_t)
//! can be overloaded by Childclass for special condition
void
MultiSend_c::updateEarlierAndLatestInterval()
{ /** @todo OPTIMIZE-178 improve with a flag for HARD/SOFT timing, but it's okay for right now... */
  if (getTimePeriod() <= 1250)
  { // use HARD-Timing
    mui16_earlierInterval = 0;
    mui16_latestInterval  = getTimePeriod()/2; //50; // be polite and let other tasks do their job, we can wait a little bit
    // ==> this is better than always interrupting important big tasks as WE can wait and slicing is NOT nice to handle for the big task.
    if (mui16_latestInterval > 100) mui16_latestInterval = 100;
  }
  else
  { // use SOFT-Timing (using jitter for earlier/after)
    mui16_earlierInterval = ( (getTimePeriod() * 1) / 4);
    mui16_latestInterval =     getTimePeriod();
  }
}

/**
  start processing of a process msg
  ignore all invalid messages where SEND is not of a member with claimed address,
  or where EMPF isn't valid

  possible errors:
    * Err_c::elNonexistent on SEND/EMPF not registered in Monitor-List
  @return true -> message was processed; else the received CAN message will be served to other matching CanCustomer_c
*/
bool
MultiSend_c::processMsg( const CanPkg_c& arc_data )
{
  CanPkgExt_c pkg( arc_data, getMultitonInst() );
  if( !pkg.isValid() || (pkg.getMonitorItemForSA() == NULL) )
    return true;

  SendStream_c* pc_sendStreamFound = getSendStream (pkg.getISONameForDA(), pkg.getISONameForSA()); // sa/da swapped, of course ;-) !
  if (pc_sendStreamFound)
  { // found a matching SendStream, so call its processMsg()
    const bool cb_success = pc_sendStreamFound->processMsg( pkg );
    calcAndSetNextTriggerTime();
    return cb_success;
  }
  else
  { // no matching SendStream found
    // keep next trigger time
    return false;
  }
}

/** this function should NOT be called from INSIDE of timeEvent !
    ==> Only call it from OUTSIDE functions like init(), processMsg(), addSendStream, etc.
  */
void
MultiSend_c::calcAndSetNextTriggerTime()
{
  int32_t i32_nextRetriggerNeeded = -1;
  for (STL_NAMESPACE::list<SendStream_c>::iterator pc_iter=mlist_sendStream.begin(); pc_iter != mlist_sendStream.end();)
  {
    const int32_t ci32_nextTriggerTime = pc_iter->getNextTriggerTime();

    if ((i32_nextRetriggerNeeded == -1) || (ci32_nextTriggerTime < i32_nextRetriggerNeeded))
    { // no trigger yet set or this SendStream needs to come earlier!
      i32_nextRetriggerNeeded = ci32_nextTriggerTime;
    }
    pc_iter++;
  }

  if (i32_nextRetriggerNeeded == -1)
  { // no SendStreams needs to come to action, so idle around
    i32_nextRetriggerNeeded = System_c::getTime() + 5000;
  }
  getSchedulerInstance().changeRetriggerTimeAndResort (this, i32_nextRetriggerNeeded); // no need to change the period, as we don't use it - we always calculate the next trigger time!
}

/** this function is called by IsoMonitor_c on addition, state-change and removal of an IsoItem.
 * @param at_action enumeration indicating what happened to this IsoItem. @see IsoItemModification_en / IsoItemModification_t
 * @param acrc_isoItem reference to the (const) IsoItem which is changed (by existance or state)
 */
void
MultiSend_c::reactOnIsoItemModification (ControlFunctionStateHandler_c::IsoItemModification_t at_action, IsoItem_c const& acrc_isoItem)
{
  switch (at_action)
  {
    case ControlFunctionStateHandler_c::AddToMonitorList:
      if (acrc_isoItem.itemState (IState_c::Local))
      { // local IsoItem_c has finished adr claim
        getIsoFilterManagerInstance4Comm().insertIsoFilter (IsoFilter_s (mt_customer, IsoAgLib::iMaskFilter_c( (0x3FFFF00UL),  (TP_CONN_MANAGE_PGN << 8) ), &acrc_isoItem.isoName(), NULL, 8), false);
        getIsoFilterManagerInstance4Comm().insertIsoFilter (IsoFilter_s (mt_customer, IsoAgLib::iMaskFilter_c( (0x3FFFF00UL), (ETP_CONN_MANAGE_PGN << 8) ), &acrc_isoItem.isoName(), NULL, 8), true);
      }
      break;

    case ControlFunctionStateHandler_c::RemoveFromMonitorList:
      if (acrc_isoItem.itemState (IState_c::Local))
      { // local IsoItem_c has gone (i.e. IdentItem has gone, too.
        /// @todo SOON-178 activate the reconfiguration when the second parameter in removeIsoFilter is there finally...
        getIsoFilterManagerInstance4Comm().removeIsoFilter (IsoFilter_s (mt_customer, IsoAgLib::iMaskFilter_c( (0x3FFFF00UL),  (TP_CONN_MANAGE_PGN << 8) ), &acrc_isoItem.isoName(), NULL, 8));
        getIsoFilterManagerInstance4Comm().removeIsoFilter (IsoFilter_s (mt_customer, IsoAgLib::iMaskFilter_c( (0x3FFFF00UL), (ETP_CONN_MANAGE_PGN << 8) ), &acrc_isoItem.isoName(), NULL, 8));
        /// @todo SOON-178 Maybe clean up some streams and clients?
        /// Shouldn't appear normally anyway, so don't care for right now...
      }
      break;

    default:
      break;
  }
}

/** user function for explicit abort of any running matching stream. */
void
MultiSend_c::abortSend (const IsoName_c& acrc_isoNameSender, const IsoName_c& acrc_isoNameReceiver)
{
  SendStream_c* pc_sendStream = getSendStream (acrc_isoNameSender, acrc_isoNameReceiver);
  if (pc_sendStream)
  {
    if (!pc_sendStream->isFinished()) // check added by JVB 20090916
      pc_sendStream->abortSend();
  }
  /// let timeEvent do the erasing from the list.
  /// reason: if someone starts a new send directly after aborting the current,
  /// we can use the same place in the stl-list without remove/insert!
}

void
MultiSend_c::abortSend (const MultiSendEventHandler_c& apc_multiSendEventHandler)
{
  for (STL_NAMESPACE::list<SendStream_c>::iterator pc_iter=mlist_sendStream.begin(); pc_iter != mlist_sendStream.end(); pc_iter++)
  {
    if (pc_iter->getMultiSendEventHandler() == &apc_multiSendEventHandler)
    {
      if (!pc_iter->isFinished())
        pc_iter->abortSend();
    }
  }
}

#if DEBUG_SCHEDULER
const char*
MultiSend_c::getTaskName() const
{ return "MultiSend_c()"; }
#endif


} // end namespace __IsoAgLib
