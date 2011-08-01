/*
  process_c.cpp: central managing instance for all process data
    informations in the system

  (C) Copyright 2009 - 2011 by OSB AG and developing partners

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

  DevPropertyHandler_c& Process_c::getDevPropertyHandlerInstance( void )
  {
    return mc_devPropertyHandler;
  }


void
Process_c::init()
{
  isoaglib_assert (!initialized());

  getSchedulerInstance().registerClient( this );
  mi32_lastFilterBoxTime = 0;
  __IsoAgLib::getIsoMonitorInstance4Comm().registerControlFunctionStateHandler( mt_handler );
  mpc_tcISOName = NULL;
  mui8_lastTcStatus = 0;
  mpc_processWsmTaskMsgHandler = NULL;

  mc_devPropertyHandler.init();

  mpc_processDataChangeHandler = NULL;

  // receive PROCESS_DATA_PGN messages which are addressed to GLOBAL
  const uint32_t cui32_filter = (((PROCESS_DATA_PGN) | 0xFF) << 8);
  if (!getIsoBusInstance4Comm().existFilter( mt_customer, IsoAgLib::iMaskFilter_c( (0x3FFFF00UL), cui32_filter) ) )
  { // create FilterBox
    getIsoBusInstance4Comm().insertFilter( mt_customer, IsoAgLib::iMaskFilter_c( (0x3FFFF00UL), cui32_filter ), 8, true);
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

  //call DevPropertyHandler_c timeEvent
  mc_devPropertyHandler.timeEvent();

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

  // check for invalid SA/DA
  if (pkg.getMonitorItemForSA() == NULL)
  { // SA = 0xFE  =>  don't handle such messages, we need to have a sender
    return true;
  }

  // first check if this is a device property message -> then DevPropertyHandler_c should process this msg
  if ( ( pkg.identType() == Ident_c::ExtendedIdent ) && (
       ( ( pkg.cmd() ) < 2 ) || ( pkg.cmd() == 0xD ) ) )
  {
    if (mc_devPropertyHandler.processMsg( pkg )) return true;
  }

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

    mc_devPropertyHandler.updateTcStateReceived(pkg[4]);
    mc_devPropertyHandler.setTcSourceAddress(pkg.isoSa());
    return true;
  }

  // process working set task message (for remote instances (e.g. TC))
  if ( ( pkg.identType() == Ident_c::ExtendedIdent ) && (pkg.cmd() == 0xF))
  {
    processWorkingSetTaskMsg(pkg.getValue(), c_isoNameSender);
    return true;
  }

  bool b_result = false;

  if (pkg.getMonitorItemForDA() == NULL)
  { // broadcast message
    return b_result;
  }

  // use isoName from corresponding monitor item for checks
  const IsoName_c& c_isoNameReceiver = pkg.getMonitorItemForDA()->isoName();

  if ( existProcDataLocal( pkg.DDI(), pkg.element(), c_isoNameReceiver) )
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
      // else: don't care for remote IsoItem_c
      break;

    case ControlFunctionStateHandler_c::RemoveFromMonitorList:
      if (acrc_isoItem.itemState (IState_c::Local))
      { // local IsoItem_c has gone (i.e. IdentItem has gone, too.
        getIsoFilterManagerInstance4Comm().removeIsoFilter(  IsoFilter_s (mt_customer, IsoAgLib::iMaskFilter_c( (0x3FFFF00UL), ((PROCESS_DATA_PGN) << 8) ), &acrc_isoItem.isoName(), NULL, 8 ));
      }
      // else: don't care for remote IsoItem_c
      break;

    default:
      break;
  }
}



/**
  process TC status messages:
  - task status suspended: stop running measurement (started by default data logging)
  @param ui8_tcStatus
  @param rc_isoName  device key of TC
  @return true
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

  return true;
}


bool Process_c::processWorkingSetTaskMsg(uint8_t ui8_tcStatus, const IsoName_c& rc_isoName)
{
  if (mpc_processWsmTaskMsgHandler)
    mpc_processWsmTaskMsgHandler->processWsmTaskMessage(ui8_tcStatus, rc_isoName);
  return true;
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
