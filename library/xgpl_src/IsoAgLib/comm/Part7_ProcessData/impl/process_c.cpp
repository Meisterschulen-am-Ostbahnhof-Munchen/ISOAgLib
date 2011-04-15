/*
  process_c.cpp: central managing instance for all process data
    informations in the system

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include "process_c.h"
#include <IsoAgLib/comm/impl/isobus_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isofiltermanager_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isoitem_c.h>
#include <IsoAgLib/util/iassert.h>

#if DEBUG_HEAP_USEAGE
  #ifdef SYSTEM_PC
    #include <iostream>
  #else
    #include <supplementary_driver/driver/rs232/impl/rs232io_c.h>
  #endif
  #include <IsoAgLib/util/impl/util_funcs.h>
#endif

#if DEBUG_HEAP_USEAGE
static uint16_t sui16_localProcPointerTotal = 0;
static uint16_t sui16_remoteProcPointerTotal = 0;

#ifdef MASSERT
extern unsigned int AllocateHeapMalloc;
extern unsigned int DeallocateHeapMalloc;
#endif
#endif


namespace __IsoAgLib {
  /** C-style function, to get access to the unique Process_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  Process_c &getProcessInstance( uint8_t aui8_instance )
  {
    MACRO_MULTITON_GET_INSTANCE_BODY(Process_c, PRT_INSTANCE_CNT, aui8_instance);
  }

#if defined(USE_PROC_DATA_DESCRIPTION_POOL)
  DevPropertyHandler_c& Process_c::getDevPropertyHandlerInstance( void )
  {
    return mc_devPropertyHandler;
  }
#endif


void
Process_c::init()
{
  isoaglib_assert (!initialized());

  getSchedulerInstance().registerClient( this );
  mi32_lastFilterBoxTime = 0;
  mb_needCallOfCheckCreateRemoteReceiveFilter = false;
  __IsoAgLib::getIsoMonitorInstance4Comm().registerControlFunctionStateHandler( mt_handler );
  mpc_tcISOName = NULL;
  mui8_lastTcStatus = 0;
  mpc_processWsmTaskMsgHandler = NULL;

  mpc_processDataChangeHandler = NULL;

  // receive PROCESS_DATA_PGN messages which are addressed to GLOBAL
  const uint32_t cui32_filter = (((PROCESS_DATA_PGN) | 0xFF) << 8);
  if (!getIsoBusInstance4Comm().existFilter( mt_customer, IsoAgLib::iMaskFilter_c( (0x3FFFF00UL), cui32_filter) ) )
  { // create FilterBox
    getIsoBusInstance4Comm().insertFilter( mt_customer, IsoAgLib::iMaskFilter_c( (0x3FFFF00UL), cui32_filter ), true);
  }

  //  start with 200 msec timer period
  Scheduler_Task_c::setTimePeriod(200);

  setInitialized();
}

/** every subsystem of IsoAgLib has explicit function for controlled shutdown
  */
void Process_c::close()
{
  isoaglib_assert (initialized());

  getSchedulerInstance().unregisterClient( this );
  getIsoMonitorInstance4Comm().deregisterControlFunctionStateHandler( mt_handler );

  setClosed();
};


/** handler function for access to undefined client.
  * the base Singleton calls this function, if it detects an error
  */
void Process_c::registerAccessFlt( void )
{

  getILibErrInstance().registerError( iLibErr_c::ElNonexistent, iLibErr_c::Process );
}



/**
  performs periodically actions,
  here: update trusted adrVect and resort array if needed every 3sec;

  possible errors:
    * pertial error caused by one of the memberItems

  @return true -> all planned activities performed in allowed time
*/
bool Process_c::timeEvent( void ){
  bool b_result = true;
  int32_t i32_time = Scheduler_Task_c::getLastRetriggerTime();

  if ( ml_filtersToDeleteISO.size() > 0)
  {
    for (STL_NAMESPACE::list<IsoName_c>::const_iterator iter = ml_filtersToDeleteISO.begin();
         iter != ml_filtersToDeleteISO.end();
         iter++)
    {
      if (getIsoFilterManagerInstance4Comm().existIsoFilter( IsoFilter_s (mt_customer, IsoAgLib::iMaskFilter_c( 0x3FF00FFLU, (PROCESS_DATA_PGN << 8) ), NULL, &(*iter), 8)))
      { // corresponding FilterBox_c exist -> delete it
        getIsoFilterManagerInstance4Comm().removeIsoFilter(  IsoFilter_s (mt_customer, IsoAgLib::iMaskFilter_c( 0x3FF00FFLU, (PROCESS_DATA_PGN << 8) ), NULL, &(*iter), 8));
      }
    }
    ml_filtersToDeleteISO.clear();
  }
  if ( mb_needCallOfCheckCreateRemoteReceiveFilter )
  {
    mb_needCallOfCheckCreateRemoteReceiveFilter = false;
    checkCreateRemoteReceiveFilter();
  }

#if defined(USE_PROC_DATA_DESCRIPTION_POOL)
  //call DevPropertyHandler_c timeEvent
  mc_devPropertyHandler.timeEvent();
#endif

  #if DEBUG_HEAP_USEAGE
  if ( ( c_arrClientC1.capacity() != sui16_localProcPointerTotal )
    || ( c_arrClientC2.capacity() != sui16_remoteProcPointerTotal ) )
  {
    sui16_localProcPointerTotal = c_arrClientC1.capacity();
    sui16_remoteProcPointerTotal = c_arrClientC2.capacity();

    INTERNAL_DEBUG_DEVICE
      << c_arrClientC1.size()
      << "(" << c_arrClientC1.capacity()
      << ") x LocalProcData Pointer: Mal-Alloc: "
      << sizeVectorTWithMalloc( sizeof(void*), c_arrClientC1.capacity() )
      << "/" << sizeof(void*)
      << ", Chunk-Alloc: "
      << sizeVectorTWithChunk( sizeof(void*), c_arrClientC1.capacity() )
      << INTERNAL_DEBUG_DEVICE_ENDL
      << c_arrClientC2.size()
      << "(" << c_arrClientC2.capacity()
      << ") x RemoteProcData Pointer: Mal-Alloc: "
      << sizeVectorTWithMalloc( sizeof(uint16_t), c_arrClientC2.capacity() )
      << "/" << sizeof(void*)
      << ", Chunk-Alloc: "
      << sizeVectorTWithChunk( sizeof(uint16_t), c_arrClientC2.capacity() )
      #ifndef MASSERT
      << INTERNAL_DEBUG_DEVICE_NEWLINE << INTERNAL_DEBUG_DEVICE_ENDL;
      #else
      << INTERNAL_DEBUG_DEVICE_NEWLINE << "__mall tot:" << AllocateHeapMalloc
      << ", _mall deal tot: " << DeallocateHeapMalloc
      << INTERNAL_DEBUG_DEVICE_NEWLINE << INTERNAL_DEBUG_DEVICE_ENDL;
      #endif
  }
  #endif

  uint16_t ui16_nextTimePeriod = 0;

  // call the time event for all local data
  for ( cacheTypeC1_t pc_iter = c_arrClientC1.begin();
        ( pc_iter != c_arrClientC1.end() );
        pc_iter++ )
  { // delete item at pc_timeIter, if pc_searchCacheC1 points to pc_client
    if ( !(*pc_iter)->timeEvent( &ui16_nextTimePeriod ) ) b_result = false;
  }

  if (ui16_nextTimePeriod)
  {
    if (ui16_nextTimePeriod < 20)
    { // skip small values
      ui16_nextTimePeriod = 20;
    }
    if (ui16_nextTimePeriod > 200)
    { // limit large values (for Alive sending)
      ui16_nextTimePeriod = 200;
    }
    Scheduler_Task_c::setTimePeriod(ui16_nextTimePeriod); // + Scheduler_Task_c::getEarlierInterval());
  }
  // call the time event for all remote data
  for ( cacheTypeC2_t pc_iter = c_arrClientC2.begin();
        ( pc_iter != c_arrClientC2.end() );
        pc_iter++ )
  { // delete item at pc_timeIter, if pc_searchCacheC1 points to pc_client
    if ( getAvailableExecTime() == 0 ) return false;
    if ( !(*pc_iter)->timeEvent() ) b_result = false;
  }
  // if local active member exist - check every second if
  // filters for targeted or partner process data should be created
  if ((i32_time - mi32_lastFilterBoxTime) > 1000)  {
    mi32_lastFilterBoxTime = i32_time;
  }
  // the other list elements doesn't need periodic actions
  return b_result;
};


/** called when a new measurement is started */
void Process_c::resetTimerPeriod( void )
{
  getSchedulerInstance().changeTimePeriodAndResortTask(this, 100);
}


bool Process_c::processMsg( const CanPkg_c& arc_data )
{

  ProcessPkg_c pkg( arc_data, getMultitonInst() );

  // call string2flags function that was done formerly in the filterbox
  pkg.string2Flags();

  // check for invalid SA/DA
  if (pkg.getMonitorItemForSA() == NULL)
  { // SA = 0xFE  =>  don't handle such messages, we need to have a sender
    return true;
  }

#if defined(USE_PROC_DATA_DESCRIPTION_POOL)
// first check if this is a device property message -> then DevPropertyHandler_c should process this msg
if ( ( pkg.identType() == Ident_c::ExtendedIdent ) && (
     ( ( pkg.cmd() ) < 2 ) || ( pkg.cmd() == 0xD ) ) )
{
  if (mc_devPropertyHandler.processMsg( pkg )) return true;
}
#endif

  // check for sender isoName (only in remote)
  const IsoName_c& c_isoNameSender = pkg.getMonitorItemForSA()->isoName();

  // process TC status message (for local instances)
  if ( ( pkg.identType() == Ident_c::ExtendedIdent ) && (pkg.cmd() == 0xE))
  {
    // update isoName of TC
    /// @todo SOON-240 This only works until the IsoItem gets destructed!!!
    /// --> Copy the ISONAME, do not take a pointer to that ISONAME!
    mpc_tcISOName = &c_isoNameSender;
    processTcStatusMsg(pkg.getValue(), c_isoNameSender);

#ifdef USE_PROC_DATA_DESCRIPTION_POOL
    mc_devPropertyHandler.updateTcStateReceived(pkg[4]);
    mc_devPropertyHandler.setTcSourceAddress(pkg.isoSa());
#endif
    return TRUE;
  }

  // process working set task message (for remote instances (e.g. TC))
  if ( ( pkg.identType() == Ident_c::ExtendedIdent ) && (pkg.cmd() == 0xF))
  {
    processWorkingSetTaskMsg(pkg.getValue(), c_isoNameSender);
    return TRUE;
  }

  bool b_result = false;

  if (pkg.getMonitorItemForDA() == NULL)
  { // broadcast message
    return b_result;
  }

  // use isoName from corresponding monitor item for checks
  const IsoName_c& c_isoNameReceiver = pkg.getMonitorItemForDA()->isoName();

  // check first for remote Process Data
  if ( existProcDataRemote( pkg.DDI(), pkg.element(), c_isoNameSender, c_isoNameReceiver) )
  { // there exists an appropriate process data item -> let the item process the msg
    procDataRemote( pkg.DDI(), pkg.element(), c_isoNameSender, c_isoNameReceiver).processMsg( pkg );
    b_result = true;
  }
  // if not found => now check for remote Process Data
  else if ( existProcDataLocal( pkg.DDI(), pkg.element(), c_isoNameReceiver) )
  { // there exists an appropriate process data item -> let the item process the msg
    procDataLocal( pkg.DDI(), pkg.element(), c_isoNameReceiver).processMsg( pkg );
    b_result = true;
  }

  return b_result;
}

/**
  checks if a suitable ProcDataLocal_c item exist
  ISO parameter
  @param aui16_DDI
  @param aui16_element
  @param acrc_isoNameReceiver isoName code of searched local Process Data instance
  @return true -> suitable instance found
*/
bool Process_c::existProcDataLocal( uint16_t aui16_DDI, uint16_t aui16_element, const IsoName_c& acrc_isoNameReceiver)
{
  return updateLocalCache( aui16_DDI, aui16_element, acrc_isoNameReceiver);
}

/**
  checks if a suitable acrc_isoName item exist
  ISO parameter
  @param aui16_DDI
  @param aui16_element
  @param acrc_isoNameSender isoName of the sender (used for check against isoName())
  @param acrc_isoNameReceiver isoName code of searched local Process Data instance
  @return true -> suitable instance found
*/
bool Process_c::existProcDataRemote( uint16_t aui16_DDI, uint16_t aui16_element,
                                     const IsoName_c& acrc_isoNameSender, const IsoName_c& acrc_isoNameReceiver)
{
 return updateRemoteCache( aui16_DDI, aui16_element, acrc_isoNameSender, acrc_isoNameReceiver);
}

/**
  search for suitable ProcDataLocal_c item; create on if not found AND if wanted

  possible errors:
      * Err_c::badAlloc not enough memory to add new ProcDataLocal_c
        (can cause badAlloc exception)
      * Err_c::elNonexistent if element not found and ab_doCreate == false
  ISO parameter
  @param aui16_DDI
  @param aui16_element
  @param acrc_isoNameReceiver isoName code of searched local Process Data instance
  @return reference to searched/created ProcDataLocal_c instance
*/
ProcDataLocalBase_c& Process_c::procDataLocal( uint16_t aui16_DDI, uint16_t aui16_element, const IsoName_c& acrc_isoNameReceiver)
{
  bool b_found = updateLocalCache( aui16_DDI, aui16_element, acrc_isoNameReceiver);
  if (!b_found)
  { // not found and no creation wanted -> error
    getILibErrInstance().registerError( iLibErr_c::ElNonexistent, iLibErr_c::Process );
  }
  return **pc_searchCacheC1;
}

/**
  search for suitable acrc_isoName item; create on if not found AND if wanted

  possible errors:
      * Err_c::badAlloc not enough memory to add new ProcDataRemote_c
        (can cause badAlloc exception)
      * Err_c::elNonexistent if element not found and ab_doCreate == false
  ISO parameter
  @param aui16_DDI
  @param aui16_element
  @param acrc_isoNameSender isoName of the sender (used for check against isoName())
  @param acrc_isoNameReceiver isoName code of searched local Process Data instance
  @return reference to searched/created acrc_isoName instance
  @exception badAlloc
*/
ProcDataRemoteBase_c& Process_c::procDataRemote( uint16_t aui16_DDI, uint16_t aui16_element,
                                                 const IsoName_c& acrc_isoNameSender, const IsoName_c& acrc_isoNameReceiver)
{
  bool b_found = updateRemoteCache(aui16_DDI, aui16_element, acrc_isoNameSender, acrc_isoNameReceiver);
  if (!b_found)
  { // not found and no creation wanted -> error
    getILibErrInstance().registerError( iLibErr_c::ElNonexistent, iLibErr_c::Process );
  }
  return **pc_searchCacheC2;
}


/**
  update the cache with search for according ProcDataLocal_c item
  ISO parameter
  @param aui16_DDI
  @param aui16_element
  @param acrc_isoNameReceiver isoName code of created local Process Data instance
*/
bool Process_c::updateLocalCache( uint16_t aui16_DDI, uint16_t aui16_element, const IsoName_c& acrc_isoNameReceiver)
{
  bool b_foundLazy = false;

  if (!c_arrClientC1.empty())
  {
    if ( pc_searchCacheC1 != c_arrClientC1.end() )
    {
      // don't check sender devClass => 0xFF
      if ((*pc_searchCacheC1)->matchISO (IsoName_c::IsoNameUnspecified(), acrc_isoNameReceiver, aui16_DDI, aui16_element)) return true;
    }
    //old cache doesn't match any more -> search new
    for ( cacheTypeC1_t pc_iter = c_arrClientC1.begin();
        ( pc_iter != c_arrClientC1.end() );
        pc_iter++ )
    { // check for lazy match with INSTANCE == 0xFF (==joker)

      bool b_matched = false;

      // don't check sender devClass => 0xFF
      if ((*pc_iter)->matchISO(IsoName_c::IsoNameUnspecified(), acrc_isoNameReceiver, aui16_DDI, aui16_element))
        b_matched = true;

      if (b_matched)
      { // matches at least lazy
        // for ISO - no exact compare including _instance_ is possible, as this is not transfered in data message
        b_foundLazy = true;
        pc_searchCacheC1 = pc_iter;
      } // check lazy match
    }// for
  }

  return b_foundLazy;
}


/**
  update the cache with search for according acrc_isoName item
  ISO parameter
  @param aui16_DDI
  @param aui16_element
  @param acrc_isoNameSender isoName of the sender (used for check against isoName())
  @param acrc_isoNameReceiver isoName code of searched local Process Data instance
*/
bool Process_c::updateRemoteCache( uint16_t aui16_DDI, uint16_t aui16_element,
                                   const IsoName_c& acrc_isoNameSender, const IsoName_c& acrc_isoNameReceiver)
{
  bool b_foundLazy = false;
  if (!c_arrClientC2.empty())
  {
    if ( pc_searchCacheC2 != c_arrClientC2.end() )
    {
      if ((*pc_searchCacheC2)->matchISO(acrc_isoNameSender, acrc_isoNameReceiver, aui16_DDI, aui16_element)) return true;
    }
    //old cache doesn't match any more -> search new
    for ( cacheTypeC2_t pc_iter = c_arrClientC2.begin(); //list of remote process data
        ( pc_iter != c_arrClientC2.end() );
        pc_iter++ )
    { // check for lazy match with INSTANCE == 0xFF (==joker)
      bool b_matched = false;

      if ((*pc_iter)->matchISO(acrc_isoNameSender, acrc_isoNameReceiver, aui16_DDI, aui16_element))
        b_matched = true;

      if (b_matched)
      { // matches at least lazy
        b_foundLazy = true;
        pc_searchCacheC2 = pc_iter;
      } // check lazy match
    }// for
  }
  return b_foundLazy;
}


ProcDataRemoteBase_c* Process_c::addDDI2ExistingProcData(uint16_t aui16_DDI, uint16_t aui_deviceElement, const IsoName_c& acrc_isoName, ProcessCmd_c::ValueGroup_t& refen_valueGroup, bool& rb_isSetpoint)
{
  ProcDataRemoteBase_c* pc_remoteProcessData = NULL;

  for ( cacheTypeC2_t pc_iter = c_arrClientC2.begin(); //list of remote process data
        ( pc_iter != c_arrClientC2.end() );
        pc_iter++ )
  {
    if ((*pc_iter)->check4GroupMatchExisting(aui16_DDI, aui_deviceElement, acrc_isoName))
    { // DDI/elementNr already contained in acrc_isoName
      (*pc_iter)->getDDIType(aui16_DDI, refen_valueGroup, rb_isSetpoint);
      return *pc_iter;
    }
  }

  // try to find a matching group
  pc_remoteProcessData = check4DDIGroupMatch(aui16_DDI, aui_deviceElement, acrc_isoName);
  if (pc_remoteProcessData)
  {
    bool mb_isSetpoint;
    ProcessCmd_c::ValueGroup_t men_valueGroup;
    pc_remoteProcessData->getDDIType(aui16_DDI, men_valueGroup, mb_isSetpoint);
    if (pc_remoteProcessData->add2Group(aui16_DDI))
    {
      refen_valueGroup = men_valueGroup;
      rb_isSetpoint = mb_isSetpoint;
      return pc_remoteProcessData;
    }
  }
  return NULL;
}

bool Process_c::checkAndAddMatchingDDI2Group(uint16_t aui16_DDI, uint16_t aui_deviceElement, const IsoName_c& acrc_isoName)
{
  ProcDataRemoteBase_c* pc_remoteProcessData = check4DDIGroupMatch(aui16_DDI, aui_deviceElement, acrc_isoName);

  if (NULL == pc_remoteProcessData) return false;

  return pc_remoteProcessData->add2Group(aui16_DDI);
}


bool Process_c::addProprietaryDDI2Group(uint16_t aui16_DDI, uint16_t aui_deviceElement, bool mb_isSetpoint, ProcessCmd_c::ValueGroup_t ddiType, const IsoName_c &acrc_isoName)
{
  ProcDataRemoteBase_c* pc_remoteProcessData = check4ProprietaryDDIGroupMatch(aui_deviceElement, acrc_isoName);

  if (NULL == pc_remoteProcessData) return false;

  return pc_remoteProcessData->addProprietary2Group(aui16_DDI, mb_isSetpoint, ddiType);
}


ProcDataRemoteBase_c* Process_c::check4DDIGroupMatch(uint16_t aui16_DDI, uint16_t aui_deviceElement, const IsoName_c& acrc_isoName)
{
  for ( cacheTypeC2_t pc_iter = c_arrClientC2.begin(); //list of remote process data
        ( pc_iter != c_arrClientC2.end() );
        pc_iter++ )
  {
    if ((*pc_iter)->check4GroupMatch(aui16_DDI, aui_deviceElement, acrc_isoName))
      return *pc_iter;
  }// for

  return NULL;
}

bool Process_c::check4DDIExisting(uint16_t aui16_DDI, uint16_t aui_deviceElement, const IsoName_c& acrc_isoName)
{
  for ( cacheTypeC2_t pc_iter = c_arrClientC2.begin(); //list of remote process data
        ( pc_iter != c_arrClientC2.end() );
        pc_iter++ )
  {
    if ((*pc_iter)->check4GroupMatchExisting(aui16_DDI, aui_deviceElement, acrc_isoName))
      return TRUE;
  }// for

  return FALSE;
}

ProcDataRemoteBase_c* Process_c::check4ProprietaryDDIGroupMatch(uint16_t aui_deviceElement, const IsoName_c &acrc_isoName)
{
  for ( cacheTypeC2_t pc_iter = c_arrClientC2.begin(); //list of remote process data
        ( pc_iter != c_arrClientC2.end() );
        pc_iter++ )
  {
    if ((*pc_iter)->checkProprietary4GroupMatch(aui_deviceElement, acrc_isoName))
      return *pc_iter;
  }// for

  return NULL;
}

/**
  delete IsoFilterBox_c for receive from remote isoName if needed
  (important to delete old Filter Boxes after deletion of
  remote device from monitor list)
  @param acrc_isoName isoName code of remote owner who sent the message
  @return true -> member exists and IsoFilterBox deleted
*/
bool Process_c::deleteRemoteFilter(IsoName_c const& acrc_isoName)
{
  for ( cacheTypeC2_t pc_iter = c_arrClientC2.begin();
        ( pc_iter != c_arrClientC2.end() );
        pc_iter++ )
  {
    if ((*pc_iter)->isoName() == acrc_isoName)
    { // remote proc data has given onwerisoName
      // -> delete according FilterBox (after check if corresponding FilterBox_c exists)
      if (getIsoFilterManagerInstance4Comm().existIsoFilter( IsoFilter_s (mt_customer, IsoAgLib::iMaskFilter_c( 0x3FF00FFLU, (PROCESS_DATA_PGN << 8) ), NULL, &acrc_isoName, 8)))
      { // corresponding FilterBox_c exist -> delete it
        getIsoFilterManagerInstance4Comm().removeIsoFilter(  IsoFilter_s (mt_customer, IsoAgLib::iMaskFilter_c( 0x3FF00FFLU, (PROCESS_DATA_PGN << 8) ), NULL, &acrc_isoName, 8));
        // and let the caller know of the positive deletion!
        return true;
      } else
        return false; // there was a problem!
    }
  }
  return false;
}

/**
  insert FilterBox_c for receive from remote isoName if needed
  @param acrc_isoName isoName code of remote owner who sent the message
  @return true -> member exist and Filter Box created
 */
bool Process_c::createRemoteFilter(IsoName_c const& acrc_isoName)
{
  if (!getIsoFilterManagerInstance4Comm().existIsoFilter( IsoFilter_s (mt_customer, IsoAgLib::iMaskFilter_c( 0x3FF00FFUL, (PROCESS_DATA_PGN << 8) ), NULL, &acrc_isoName, 8 )))
  { // no suitable FilterBox_c exist -> create it
    getIsoFilterManagerInstance4Comm().insertIsoFilter(
        IsoFilter_s (mt_customer, IsoAgLib::iMaskFilter_c( 0x3FF00FFUL, (PROCESS_DATA_PGN << 8) ), NULL, &acrc_isoName, 8 ) );
    return true;
  }

  return false;
}


/**
  * check if any remote process data needs a new receive filter
  * @return true -> a remote filter has been created
  */
bool Process_c::checkCreateRemoteReceiveFilter()
{
  bool b_result = false;
  const IsoName_c *pc_lastFilterisoName = NULL;
  const IsoName_c *pc_actisoName = NULL;

  for ( cacheTypeC2_t pc_iter = c_arrClientC2.begin();
        ( pc_iter != c_arrClientC2.end() );
        pc_iter++ )
  {
    pc_actisoName = &((*pc_iter)->isoName());

    if ( pc_actisoName->isSpecified()
      && ( ( NULL == pc_lastFilterisoName ) || (*pc_actisoName != *pc_lastFilterisoName) )
       )
    { // last FilterBox_c call with other isoName
      // -> avoid unneccessary calls with search
      pc_lastFilterisoName = pc_actisoName;
      if ( createRemoteFilter(*pc_actisoName) ) b_result = true;
    }
  }
  return b_result;
}


/** this function is called by IsoMonitor_c on addition, state-change and removal of an IsoItem.
 * @param at_action enumeration indicating what happened to this IsoItem. @see IsoItemModification_en / IsoItemModification_t
 * @param acrc_isoItem reference to the (const) IsoItem which is changed (by existance or state)
 */
void
Process_c::reactOnIsoItemModification (ControlFunctionStateHandler_c::IsoItemModification_t at_action, IsoItem_c const& acrc_isoItem)
{
  switch (at_action)
  {
    case ControlFunctionStateHandler_c::AddToMonitorList:
      if (acrc_isoItem.itemState (IState_c::Local))
      { // local IsoItem_c has finished adr claim
        getIsoFilterManagerInstance4Comm().insertIsoFilter(   IsoFilter_s (mt_customer, IsoAgLib::iMaskFilter_c( (0x3FFFF00UL), ((PROCESS_DATA_PGN) << 8) ), &acrc_isoItem.isoName(), NULL, 8 ), true);
      }
      else
      { // remote IsoItem_c has finished adr claim
        mb_needCallOfCheckCreateRemoteReceiveFilter = true;
      }
      break;

    case ControlFunctionStateHandler_c::RemoveFromMonitorList:
      if (acrc_isoItem.itemState (IState_c::Local))
      { // local IsoItem_c has gone (i.e. IdentItem has gone, too.
        getIsoFilterManagerInstance4Comm().removeIsoFilter(  IsoFilter_s (mt_customer, IsoAgLib::iMaskFilter_c( (0x3FFFF00UL), ((PROCESS_DATA_PGN) << 8) ), &acrc_isoItem.isoName(), NULL, 8 ));
      }
      else
      { // remote IsoItem_c
        /** @todo SOON-240: change handling so that DataLinkLayer issues error, when the target
            ISONAME is not available, so that the further sending of messages to this ISONAME
            can be explicitly stopped, to avoid ongoing send tries to undefined destination.
            As soon as this is done, the removal of receive filters should be de-activated.
        */
        deleteRemoteFilter(acrc_isoItem.isoName());
      }
      break;

    default:
      break;
  }
}


/** register pointer to a new remote process data instance
  * this function is called within construction of new remote process data instance
  */
bool Process_c::registerRemoteProcessData( ProcDataRemoteBase_c* pc_remoteClient)
{
  const bool cb_result = registerC2( pc_remoteClient );
  mb_needCallOfCheckCreateRemoteReceiveFilter = true;

  return cb_result;
}


/** unregister pointer to a already registered remote process data instance
  * this function is called within destruction of remote process data instance
  */
void Process_c::unregisterRemoteProcessData( ProcDataRemoteBase_c* pc_remoteClient)
{
  // check if the remote owner isoName is used for any other remote proc
  const IsoName_c& c_toBeDeletedOwnerisoName = pc_remoteClient->isoName();
  bool b_otherRemoteWithSameOwner = false;

  for ( cacheTypeC2_t pc_iter = c_arrClientC2.begin();
        ( pc_iter != c_arrClientC2.end() );
        pc_iter++ )
  {
    if ( (*pc_iter) == pc_remoteClient ) continue;
    if ((*pc_iter)->isoName() == c_toBeDeletedOwnerisoName) b_otherRemoteWithSameOwner = true;
  }

  unregisterC2( pc_remoteClient );

  // set ptr to a defined position => avoid use of this pc_iter in deleteFilter() which is now postponed (timeEvent())
  pc_searchCacheC2 = c_arrClientC2.begin();

  if ( !b_otherRemoteWithSameOwner )
  { // delete the remote filter that was created to receive messages from that owner
    // delete corresponding FilterBox_c in timeEvent() to avoid problems when called in procdata cestructor
    ml_filtersToDeleteISO.push_front(c_toBeDeletedOwnerisoName);
  }
}

/**
  process TC status messages:
  - task status suspended: stop running measurement (started by default data logging)
  @param ui8_tcStatus
  @param rc_isoName  device key of TC
  @return TRUE
*/
bool Process_c::processTcStatusMsg(uint8_t ui8_tcStatus, const IsoName_c& rc_isoName, bool ab_skipLastTcStatus)
{
  if ((ui8_tcStatus != mui8_lastTcStatus) || ab_skipLastTcStatus)
  { // process status message only when TC status change happens
    // or ab_skipLastTcStatus is set (when set value command with value 0 for DDI 0xDFFF is received)
    if (0 == ui8_tcStatus)
    {
      for ( cacheTypeC1_t pc_iter = c_arrClientC1.begin(); pc_iter != c_arrClientC1.end(); pc_iter++ )
      {
        (*pc_iter)->stopRunningMeasurement(rc_isoName);
      }
    }
  }
  if (!ab_skipLastTcStatus)
    mui8_lastTcStatus = ui8_tcStatus;

  if (mpc_processDataChangeHandler)
  { // call handler function if handler class is registered
    mpc_processDataChangeHandler->processTcStatusMessage((1 == ui8_tcStatus) /* 1: task running */, rc_isoName.toConstIisoName_c());
  }

  return TRUE;
}


bool Process_c::processWorkingSetTaskMsg(uint8_t ui8_tcStatus, const IsoName_c& rc_isoName)
{
  if (mpc_processWsmTaskMsgHandler)
    mpc_processWsmTaskMsgHandler->processWsmTaskMessage(ui8_tcStatus, rc_isoName);
  return TRUE;
}


#if DEBUG_SCHEDULER
const char*
Process_c::getTaskName() const
{ return "Process_c"; }
#endif


//! Function set ui16_earlierInterval and
//! ui16_laterInterval that will be used by
//! getTimeToNextTrigger(retriggerType_t)
//! can be overloaded by Childclass for special condition
void
Process_c::updateEarlierAndLatestInterval(){
  mui16_earlierInterval = 0; //( ( getTimePeriod() * 3) / 4);
  mui16_latestInterval   =  ( getTimePeriod() / 2) ;
}

} // end of namespace __IsoAgLib
