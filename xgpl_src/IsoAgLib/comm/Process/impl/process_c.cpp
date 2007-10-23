/***************************************************************************
                          process_c.cpp - central managing instance for
                                           all process data informations in
                                           the system
                             -------------------
    begin                : Fri Apr 07 2000
    copyright            : (C) 2000 - 2004 by Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
    type                 : Source
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

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include "process_c.h"
#include <IsoAgLib/driver/can/impl/canio_c.h>
#include <IsoAgLib/util/impl/singleton.h>
//#include <IsoAgLib/driver/can/impl/ident_c.h>

#include <IsoAgLib/comm/SystemMgmt/ISO11783/impl/isoitem_c.h>

#ifdef DEBUG
  #include <supplementary_driver/driver/rs232/irs232io_c.h>
#endif


#if defined(DEBUG) || defined(DEBUG_HEAP_USEAGE)
  #include <supplementary_driver/driver/rs232/impl/rs232io_c.h>
  #include <IsoAgLib/util/impl/util_funcs.h>
#endif

#ifdef DEBUG_HEAP_USEAGE
static uint16_t sui16_localProcPointerTotal = 0;
static uint16_t sui16_remoteProcPointerTotal = 0;

#ifdef MASSERT
extern unsigned int AllocateHeapMalloc;
extern unsigned int DeallocateHeapMalloc;
#endif
#endif


namespace __IsoAgLib {
#if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
  /** C-style function, to get access to the unique Process_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  Process_c& getProcessInstance( uint8_t aui8_instance )
  { // if > 1 singleton instance is used, no static reference can be used
    return Process_c::instance( aui8_instance );
  };
#else
  /** C-style function, to get access to the unique Process_c singleton instance */
  Process_c& getProcessInstance( void )
  {
    static Process_c& c_process = Process_c::instance();
    return c_process;
  };
#endif

#if defined(USE_PROC_DATA_DESCRIPTION_POOL)
  DevPropertyHandler_c& Process_c::getDevPropertyHandlerInstance( void )
  {
    return c_devPropertyHandler;
  };
#endif

/** initialise element which can't be done during construct */
void Process_c::init()
{ // clear state of b_alreadyClosed, so that close() is called one time
  clearAlreadyClosed();
  // first register in Scheduler_c
  getSchedulerInstance4Comm().registerClient( this );
  i32_lastFilterBoxTime = 0;
  b_needCallOfCheckCreateRemoteReceiveFilter = false;
  __IsoAgLib::getIsoMonitorInstance4Comm().registerSaClaimHandler( this );
    #ifdef USE_PROC_DATA_DESCRIPTION_POOL
    c_devPropertyHandler.init(&c_data);
    #endif
  pc_tcISOName = NULL;
  ui8_lastTcStatus = 0;
  pc_processWsmTaskMsgHandler = NULL;

  pc_processDataChangeHandler = NULL;

  c_data.setSingletonKey( getSingletonVecKey() );

  // receive PROCESS_DATA_PGN messages which are addressed to GLOBAL
  uint32_t ui32_filter = ((static_cast<MASK_TYPE>(PROCESS_DATA_PGN) | static_cast<MASK_TYPE>(0xFF)) << 8);
  if (!getCanInstance4Comm().existFilter( *this, (0x3FFFF00UL), ui32_filter, Ident_c::ExtendedIdent))
  { // create FilterBox
    getCanInstance4Comm().insertFilter( *this, (0x3FFFF00UL), ui32_filter, true, Ident_c::ExtendedIdent);
  }

  //  start with 200 msec timer period
  ElementBase_c::setTimePeriod(200);
}

/** every subsystem of IsoAgLib has explicit function for controlled shutdown
  */
void Process_c::close( void ) {
  if ( ! checkAlreadyClosed() ) {
    // avoid another call
    setAlreadyClosed();
    // unregister from Scheduler_c
    getSchedulerInstance4Comm().unregisterClient( this );
  }
};

/** default destructor which has nothing to do */
Process_c::~Process_c(){
  close();
}


/** handler function for access to undefined client.
  * the base Singleton calls this function, if it detects an error
  */
void Process_c::registerAccessFlt( void )
{

  getILibErrInstance().registerError( iLibErr_c::ElNonexistent, iLibErr_c::Process );
}

/**
  if the amount of created local process data is known, then enough capacity for the
  vector with pointers to all of them can be reserved. Otherwise the vector
  will increase with several reallocations, where each reallocation triggers
  increase of capacity by factor 2 ( capacity is the amount of elements,
  which can be stored before reallocation takes place ).
  @param aui16_localProcCapacity
*/
void Process_c::localProcDataReserveCnt( uint16_t aui16_localProcCapacity )
{
  if ( c_arrClientC1.capacity() < aui16_localProcCapacity )
  { // trigger increase of capacity
    c_arrClientC1.reserve( aui16_localProcCapacity );
  }
}
/**
  if the amount of created remote process data is known, then enough capacity for the
  vector with pointers to all of them can be reserved. Otherwise the vector
  will increase with several reallocations, where each reallocation triggers
  increase of capacity by factor 2 ( capacity is the amount of elements,
  which can be stored before reallocation takes place ).
  @param aui16_remoteProcCapacity
*/
void Process_c::remoteProcDataReserveCnt( uint16_t aui16_remoteProcCapacity )
{
  if ( c_arrClientC2.capacity() < aui16_remoteProcCapacity )
  { // trigger increase of capacity
    c_arrClientC2.reserve( aui16_remoteProcCapacity );
  }
}


/**
  deliver reference to data pkg as reference to CanPkgExt_c
  to implement the base virtual function correct
*/
CanPkgExt_c& Process_c::dataBase()
{
  return c_data;
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
  int32_t i32_time = ElementBase_c::getLastRetriggerTime();

  if ( l_filtersToDeleteISO.size() > 0)
  {
    for (STL_NAMESPACE::USABLE_SLIST<uint32_t>::const_iterator iter = l_filtersToDeleteISO.begin();
         iter != l_filtersToDeleteISO.end();
         iter++)
    {
      if (getCanInstance4Comm().existFilter( *this, 0x3FF00FF, *iter, Ident_c::ExtendedIdent))
        // corresponding FilterBox_c exist -> delete it
        getCanInstance4Comm().deleteFilter( *this, 0x3FF00FF, *iter, Ident_c::ExtendedIdent);
    }
    l_filtersToDeleteISO.clear();
  }
  if ( b_needCallOfCheckCreateRemoteReceiveFilter )
  {
    b_needCallOfCheckCreateRemoteReceiveFilter = false;
    checkCreateRemoteReceiveFilter();
  }

#if defined(USE_PROC_DATA_DESCRIPTION_POOL)
  //call DevPropertyHandler_c timeEvent
  c_devPropertyHandler.timeEvent();
#endif

  #ifdef DEBUG_HEAP_USEAGE
  if ( ( c_arrClientC1.capacity() != sui16_localProcPointerTotal )
    || ( c_arrClientC2.capacity() != sui16_remoteProcPointerTotal ) )
  {
    sui16_localProcPointerTotal = c_arrClientC1.capacity();
    sui16_remoteProcPointerTotal = c_arrClientC2.capacity();

    getRs232Instance()
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
    if ( !(*pc_iter)->timeEvent( &ui16_nextTimePeriod ) ) b_result = false; /** @todo seemded to segfault here, although this is REALLY STRANGE! */
  }

  if (ui16_nextTimePeriod)
  {
    if (ui16_nextTimePeriod < 20)
    { // skip small values
      ui16_nextTimePeriod = 20;
    }
    ElementBase_c::setTimePeriod(ui16_nextTimePeriod); // + ElementBase_c::getEarlierInterval());
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
  if ((i32_time - i32_lastFilterBoxTime) > 1000)  {
    i32_lastFilterBoxTime = i32_time;
  }
  // the other list elements doesn't need periodic actions
  return b_result;
};


/** called when a new measurement is started */
void Process_c::resetTimerPeriod( void )
{
  getSchedulerInstance().changeTimePeriodAndResortTask(this, 100);
}

/**
  start processing of a process msg
  ignore all invalid messages where SEND is not of a member with claimed address,
  or where EMPF isn't valid

  possible errors:
    * Err_c::elNonexistent on SEND/EMPF not registered in Monitor-List
  @return true -> message was processed; else the received CAN message will be served to other matching CanCustomer_c
*/
bool Process_c::processMsg(){

#if defined(USE_PROC_DATA_DESCRIPTION_POOL)
// first check if this is a device property message -> then DevPropertyHandler_c should process this msg
if ( ( c_data.identType() == Ident_c::ExtendedIdent ) && (
     ( ( c_data[0] & 0xF ) < 2 ) || ( c_data[0] == 0xD ) || ( c_data[0] > 0xF ) ) )
{
  if (c_devPropertyHandler.processMsg()) return true;
}
#endif

  // process TC status message (for local instances)
  if ( ( c_data.identType() == Ident_c::ExtendedIdent ) && (c_data[0] == 0xE))
  {
    // update isoName of TC
    pc_tcISOName = &(data().memberSend().isoName());
    processTcStatusMsg(c_data.dataRawCmdLong(), data().memberSend().isoName());

#ifdef USE_PROC_DATA_DESCRIPTION_POOL
    c_devPropertyHandler.updateTcStateReceived(c_data[4]);
    c_devPropertyHandler.setTcSourceAddress(data().isoSa());
#endif
    return TRUE;
  }

  // process working set taks messag (for remote instances (e.g. TC))
  if ( ( c_data.identType() == Ident_c::ExtendedIdent ) && (c_data[0] == 0xF))
  {
    processWorkingSetTaskMsg(c_data.dataRawCmdLong(), data().memberSend().isoName());
    return TRUE;
  }

  bool b_result = false;

  // decide which isoName to use for matching
  // ISO message: use isoName from corresponding monitor item for checks
  const IsoName_c& c_isoNameReceiver = data().memberEmpf().isoName();
  // ISO only message: check for sender isoName (only in remote)
  const IsoName_c& c_isoNameSender = data().memberSend().isoName();

  // check first for local Process Data
  if ( existProcDataLocal( data().DDI(), data().element(), c_isoNameReceiver) )
  { // there exists an appropriate process data item -> let the item process the msg
    procDataLocal( data().DDI(), data().element(), c_isoNameReceiver).processMsg();
    b_result = true;
  }

  // now check for remote Process Data
  if ( existProcDataRemote( data().DDI(), data().element(), c_isoNameSender, c_isoNameReceiver) )
  { // there exists an appropriate process data item -> let the item process the msg
    procDataRemote( data().DDI(), data().element(), c_isoNameSender, c_isoNameReceiver).processMsg();
    b_result = true;
  }

  return b_result;
}

/**
  checks if a suitable ProcDataLocal_c item exist
  ISO parameter
  @param aui16_DDI
  @param aui16_element
  @param arc_isoNameReceiver isoName code of searched local Process Data instance
  @return true -> suitable instance found
*/
bool Process_c::existProcDataLocal( uint16_t aui16_DDI, uint16_t aui16_element, const IsoName_c& arc_isoNameReceiver)
{
  return updateLocalCache( aui16_DDI, aui16_element, arc_isoNameReceiver);
}

/**
  checks if a suitable ProcDataRemoteBase_c item exist
  ISO parameter
  @param aui16_DDI
  @param aui16_element
  @param arc_isoNameSender isoName of the sender (used for check against ownerisoName())
  @param arc_isoNameReceiver isoName code of searched local Process Data instance
  @return true -> suitable instance found
*/
bool Process_c::existProcDataRemote( uint16_t aui16_DDI, uint16_t aui16_element,
                                     const IsoName_c& arc_isoNameSender, const IsoName_c& arc_isoNameReceiver)
{
 return updateRemoteCache( aui16_DDI, aui16_element, arc_isoNameSender, arc_isoNameReceiver);
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
  @param arc_isoNameReceiver isoName code of searched local Process Data instance
  @return reference to searched/created ProcDataLocal_c instance
*/
ProcDataLocalBase_c& Process_c::procDataLocal( uint16_t aui16_DDI, uint16_t aui16_element, const IsoName_c& arc_isoNameReceiver)
{
  bool b_found = updateLocalCache( aui16_DDI, aui16_element, arc_isoNameReceiver);
  if (!b_found)
  { // not found and no creation wanted -> error
    getILibErrInstance().registerError( iLibErr_c::ElNonexistent, iLibErr_c::Process );
  }
  return **pc_searchCacheC1;
}

/**
  search for suitable ProcDataRemoteBase_c item; create on if not found AND if wanted

  possible errors:
      * Err_c::badAlloc not enough memory to add new ProcDataRemote_c
        (can cause badAlloc exception)
      * Err_c::elNonexistent if element not found and ab_doCreate == false
  ISO parameter
  @param aui16_DDI
  @param aui16_element
  @param arc_isoNameSender isoName of the sender (used for check against ownerisoName())
  @param arc_isoNameReceiver isoName code of searched local Process Data instance
  @return reference to searched/created ProcDataRemoteBase_c instance
  @exception badAlloc
*/
ProcDataRemoteBase_c& Process_c::procDataRemote( uint16_t aui16_DDI, uint16_t aui16_element,
                                                 const IsoName_c& arc_isoNameSender, const IsoName_c& arc_isoNameReceiver)
{
  bool b_found = updateRemoteCache(aui16_DDI, aui16_element, arc_isoNameSender, arc_isoNameReceiver);
  if (!b_found)
  { // not found and no creation wanted -> error
    getILibErrInstance().registerError( iLibErr_c::ElNonexistent, iLibErr_c::Process );
  }
  return **pc_searchCacheC2;
}


/**
  delivers count of local process data entries with similar ident
  (which differs only in _instance_ of owner)
  ISO parameter
  @param aui16_DDI
  @param aui16_element
  @param arc_isoName isoName code of searched local Process Data instance
  @return count of similar local process data entries
*/
uint8_t Process_c::procDataLocalCnt( uint16_t aui16_DDI, uint16_t aui16_element, const IsoName_c& arc_isoName)
{
  uint8_t ui8_cnt=0;

  for ( cacheTypeC1_t pc_iter = c_arrClientC1.begin();
       ( pc_iter != c_arrClientC1.end() );
       pc_iter++ )
  { // search for all local items which match the searched identity
    // don't check sender devClass => 0xFF
    if ((*pc_iter)->matchISO (IsoName_c::IsoNameUnspecified(), arc_isoName, aui16_DDI, aui16_element))
      ui8_cnt++;
  }
  return ui8_cnt;
}


/**
  delivers count of remote process data entries with similar ident
  (which differs only in _instance_ of owner)
  ISO parameter
  @param aui16_DDI
  @param aui16_element
  @param arc_isoNameSender isoName of the sender (used for check against ownerisoName())
  @param arc_isoName isoName code of searched remote Process Data instance
  @return count of similar remote process data entries
*/
uint8_t Process_c::procDataRemoteCnt( uint16_t aui16_DDI, uint16_t aui16_element,
                                      const IsoName_c& arc_isoNameSender, const IsoName_c& arc_isoName)
{
  uint8_t ui8_cnt=0;

  for ( cacheTypeC2_t pc_iter = c_arrClientC2.begin();
       ( pc_iter != c_arrClientC2.end() );
       pc_iter++ )
  { // search for all local items which match the searched identity
    if ((*pc_iter)->matchISO(arc_isoNameSender, arc_isoName, aui16_DDI, aui16_element))
      ui8_cnt++;
  }
  return ui8_cnt;
}


/**
  update the cache with search for according ProcDataLocal_c item
  ISO parameter
  @param aui16_DDI
  @param aui16_element
  @param arc_isoNameReceiver isoName code of created local Process Data instance
*/
bool Process_c::updateLocalCache( uint16_t aui16_DDI, uint16_t aui16_element, const IsoName_c& arc_isoNameReceiver)
{
  bool b_foundLazy = false;

  if (!c_arrClientC1.empty())
  {
    if ( pc_searchCacheC1 != c_arrClientC1.end() )
    {
      // don't check sender devClass => 0xFF
      if ((*pc_searchCacheC1)->matchISO (IsoName_c::IsoNameUnspecified(), arc_isoNameReceiver, aui16_DDI, aui16_element)) return true;
    }
    //old cache doesn't match any more -> search new
    for ( cacheTypeC1_t pc_iter = c_arrClientC1.begin();
        ( pc_iter != c_arrClientC1.end() );
        pc_iter++ )
    { // check for lazy match with INSTANCE == 0xFF (==joker)

      bool b_matched = false;

      // don't check sender devClass => 0xFF
      if ((*pc_iter)->matchISO(IsoName_c::IsoNameUnspecified(), arc_isoNameReceiver, aui16_DDI, aui16_element))
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
  update the cache with search for according ProcDataRemoteBase_c item
  ISO parameter
  @param aui16_DDI
  @param aui16_element
  @param arc_isoNameSender isoName of the sender (used for check against ownerisoName())
  @param arc_isoNameReceiver isoName code of searched local Process Data instance
*/
bool Process_c::updateRemoteCache( uint16_t aui16_DDI, uint16_t aui16_element,
                                   const IsoName_c& arc_isoNameSender, const IsoName_c& arc_isoNameReceiver)
{
  bool b_foundLazy = false;
  if (!c_arrClientC2.empty())
  {
    if ( pc_searchCacheC2 != c_arrClientC2.end() )
    {
      if ((*pc_searchCacheC2)->matchISO(arc_isoNameSender, arc_isoNameReceiver, aui16_DDI, aui16_element)) return true;
    }
    //old cache doesn't match any more -> search new
    for ( cacheTypeC2_t pc_iter = c_arrClientC2.begin(); //list of remote process data
        ( pc_iter != c_arrClientC2.end() );
        pc_iter++ )
    { // check for lazy match with INSTANCE == 0xFF (==joker)
      bool b_matched = false;

      if ((*pc_iter)->matchISO(arc_isoNameSender, arc_isoNameReceiver, aui16_DDI, aui16_element))
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


ProcDataRemoteBase_c* Process_c::addDDI2ExistingProcData(uint16_t aui16_DDI, uint16_t aui_deviceElement, const IsoName_c& ac_isoName, GeneralCommand_c::ValueGroup_t& refen_valueGroup, bool& rb_isSetpoint)
{
  ProcDataRemoteBase_c* pc_remoteProcessData = NULL;

  for ( cacheTypeC2_t pc_iter = c_arrClientC2.begin(); //list of remote process data
        ( pc_iter != c_arrClientC2.end() );
        pc_iter++ )
  {
    if ((*pc_iter)->check4GroupMatchExisting(aui16_DDI, aui_deviceElement, ac_isoName))
    { // DDI/elementNr already contained in ProcDataRemoteBase_c
      (*pc_iter)->getDDIType(aui16_DDI, refen_valueGroup, rb_isSetpoint);
      return *pc_iter;
    }
  }

  // try to find a matching group
  pc_remoteProcessData = check4DDIGroupMatch(aui16_DDI, aui_deviceElement, ac_isoName);
  if (pc_remoteProcessData)
  {
    bool b_isSetpoint;
    GeneralCommand_c::ValueGroup_t en_valueGroup;
    pc_remoteProcessData->getDDIType(aui16_DDI, en_valueGroup, b_isSetpoint);
    if (pc_remoteProcessData->add2Group(aui16_DDI))
    {
      refen_valueGroup = en_valueGroup;
      rb_isSetpoint = b_isSetpoint;
      return pc_remoteProcessData;
    }
  }
  return NULL;
}

bool Process_c::checkAndAddMatchingDDI2Group(uint16_t aui16_DDI, uint16_t aui_deviceElement, const IsoName_c& ac_isoName)
{
  ProcDataRemoteBase_c* pc_remoteProcessData = check4DDIGroupMatch(aui16_DDI, aui_deviceElement, ac_isoName);

  if (NULL == pc_remoteProcessData) return false;

  return pc_remoteProcessData->add2Group(aui16_DDI);
}


bool Process_c::addProprietaryDDI2Group(uint16_t aui16_DDI, uint16_t aui_deviceElement, bool b_isSetpoint, GeneralCommand_c::ValueGroup_t ddiType, const IsoName_c &ac_isoName)
{
  ProcDataRemoteBase_c* pc_remoteProcessData = check4ProprietaryDDIGroupMatch(aui_deviceElement, ac_isoName);

  if (NULL == pc_remoteProcessData) return false;

  return pc_remoteProcessData->addProprietary2Group(aui16_DDI, b_isSetpoint, ddiType);
}


ProcDataRemoteBase_c* Process_c::check4DDIGroupMatch(uint16_t aui16_DDI, uint16_t aui_deviceElement, const IsoName_c& ac_isoName)
{
  for ( cacheTypeC2_t pc_iter = c_arrClientC2.begin(); //list of remote process data
        ( pc_iter != c_arrClientC2.end() );
        pc_iter++ )
  {
    if ((*pc_iter)->check4GroupMatch(aui16_DDI, aui_deviceElement, ac_isoName))
      return *pc_iter;
  }// for

  return NULL;
}

bool Process_c::check4DDIExisting(uint16_t aui16_DDI, uint16_t aui_deviceElement, const IsoName_c& ac_isoName)
{
  for ( cacheTypeC2_t pc_iter = c_arrClientC2.begin(); //list of remote process data
        ( pc_iter != c_arrClientC2.end() );
        pc_iter++ )
  {
    if ((*pc_iter)->check4GroupMatchExisting(aui16_DDI, aui_deviceElement, ac_isoName))
      return TRUE;
  }// for

  return FALSE;
}

ProcDataRemoteBase_c* Process_c::check4ProprietaryDDIGroupMatch(uint16_t aui_deviceElement, const IsoName_c &ac_isoName)
{
  for ( cacheTypeC2_t pc_iter = c_arrClientC2.begin(); //list of remote process data
        ( pc_iter != c_arrClientC2.end() );
        pc_iter++ )
  {
    if ((*pc_iter)->checkProprietary4GroupMatch(aui_deviceElement, ac_isoName))
      return *pc_iter;
  }// for

  return NULL;
}

/**
  delete FilterBox_c for receive from remote isoName if needed
  (important to delete old Filter Boxes after deletion of
  of remote device from monitor list or after re-adressclaim with different SA)
  @param ac_ownerisoName isoName code of remote owner who sent the message
  @return true -> member exist and Filter Box deleted
*/
bool Process_c::deleteRemoteFilter(const IsoName_c& ac_ownerisoName)
{
  bool b_result = false,
       b_found = false;
  MASK_TYPE ui32_filter;

  for ( cacheTypeC2_t pc_iter = c_arrClientC2.begin();
        ( pc_iter != c_arrClientC2.end() );
        pc_iter++ )
  {
    if ((*pc_iter)->ownerISOName() == ac_ownerisoName) b_found = true;
  }
  if (b_found)
  { // remote proc data has given onwerisoName
    // -> delete according FilterBox
    if (getIsoMonitorInstance4Comm().existIsoMemberISOName(ac_ownerisoName, true))
    { // remote owner exist and has claimed address -> check if corresponding FilterBox_c exist
      uint8_t ui8_recNr = getIsoMonitorInstance4Comm().isoMemberISOName(ac_ownerisoName, true).nr();
      ui32_filter = (PROCESS_DATA_PGN << 8) | ui8_recNr;
      if (getCanInstance4Comm().existFilter( *this, 0x3FF00FF, ui32_filter, Ident_c::ExtendedIdent))
      { // corresponding FilterBox_c exist -> delete it
        getCanInstance4Comm().deleteFilter( *this, 0x3FF00FF, ui32_filter, Ident_c::ExtendedIdent);
        b_result = true;
      }
    } // owner exist with claimed address in isoMonitor
  } // not other remote proc data with ownerisoName found
  // only reconfigure if new FilterBox_c created -> signalled by b_result == true

  if (b_result)
  {
    getCanInstance4Comm().reconfigureMsgObj();
  }
  return b_result;
}

/**
  insert FilterBox_c for receive from remote isoName if needed
  @param ac_ownerisoName isoName code of remote owner who sent the message
  @return true -> member exist and Filter Box created
 */
bool Process_c::createRemoteFilter(const IsoName_c& ac_ownerisoName)
{
  bool b_result = false;
  MASK_TYPE t_filter;

  if (getIsoMonitorInstance4Comm().existIsoMemberISOName(ac_ownerisoName, true))
  { // remote owner exist and has claimed address -> check if suitable FilterBox_c exist
    const uint8_t ui8_recNr = getIsoMonitorInstance4Comm().isoMemberISOName(ac_ownerisoName, true).nr();
    // only receive msg from ui8_recNr / ac_ownerisoName to all other devices
    t_filter = (PROCESS_DATA_PGN << 8) | ui8_recNr;
    if (!getCanInstance4Comm().existFilter( *this, 0x3FF00FFUL, t_filter, Ident_c::ExtendedIdent))
    { // no suitable FilterBox_c exist -> create it
      getCanInstance4Comm().insertFilter( *this, 0x3FF00FFUL, t_filter, false, Ident_c::ExtendedIdent);
      b_result = true;
    }
  }
  // only reconfigure if new FilterBox_c created -> signalled by b_result == true
  if (b_result)
  {
    getCanInstance4Comm().reconfigureMsgObj();
  }

  return b_result;
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
    pc_actisoName = &((*pc_iter)->ownerISOName());

    if ( (*pc_actisoName).isSpecified()
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


/** this function is called by IsoMonitor_c when a new CLAIMED IsoItem_c is registered.
  * @param rc_isoName const reference to the item which IsoItem_c state is changed
  * @param apc_newItem pointer to the currently corresponding IsoItem_c
  */
void Process_c::reactOnMonitorListAdd( const IsoName_c& rc_isoName, const IsoItem_c* apc_newItem )
{ // create FilterBoxes for remote ProcessData if needed
  if ( getIsoMonitorInstance4Comm().existLocalIsoMemberISOName(rc_isoName) )
  { // lcoal IsoItem_c has finished adr claim
    uint32_t ui32_nr = apc_newItem->nr();
          // only ISO msgs with own SA in PS (destination)
    uint32_t ui32_filter = ((static_cast<MASK_TYPE>(PROCESS_DATA_PGN) | static_cast<MASK_TYPE>(ui32_nr)) << 8);
    if (!getCanInstance4Comm().existFilter( *this, (0x3FFFF00UL), ui32_filter, Ident_c::ExtendedIdent))
    { // create FilterBox
      getCanInstance4Comm().insertFilter( *this, (0x3FFFF00UL), ui32_filter, true, Ident_c::ExtendedIdent);
    }
  }
  else
  { // remote IsoItem_c has finished adr claim
    b_needCallOfCheckCreateRemoteReceiveFilter = true;
  }
}


/** this function is called by IsoMonitor_c when a device looses its IsoItem_c.
  * @param rc_isoName const reference to the item which IsoItem_c state is changed
  * @param aui8_oldSa previously used SA which is NOW LOST -> clients which were connected to this item can react explicitly
  */
void Process_c::reactOnMonitorListRemove( const IsoName_c& rc_isoName, uint8_t aui8_oldSa )
{
  if ( getIsoMonitorInstance4Comm().existLocalIsoMemberISOName(rc_isoName) )
  { // lcoal IsoItem_c has lost SA
    uint32_t ui32_nr = aui8_oldSa;
          // only ISO msgs with own SA in PS (destination)
    uint32_t ui32_filter = ((static_cast<MASK_TYPE>(PROCESS_DATA_PGN) | static_cast<MASK_TYPE>(ui32_nr)) << 8);
    if (getCanInstance4Comm().existFilter( *this, (0x3FFFF00UL), ui32_filter, Ident_c::ExtendedIdent))
    { // create FilterBox
      getCanInstance4Comm().deleteFilter( *this, (0x3FFFF00UL), ui32_filter, Ident_c::ExtendedIdent);
    }
  }
  else
  { // remote IsoItem_c
    deleteRemoteFilter(rc_isoName);
  }
}


/** register pointer to a new remote process data instance
  * this function is called within construction of new remote process data instance
  */
bool Process_c::registerRemoteProcessData( ProcDataRemoteBase_c* pc_remoteClient)
{
  const bool cb_result = registerC2( pc_remoteClient );
  b_needCallOfCheckCreateRemoteReceiveFilter = true;

  return cb_result;
}


/** unregister pointer to a already registered remote process data instance
  * this function is called within destruction of remote process data instance
  */
void Process_c::unregisterRemoteProcessData( ProcDataRemoteBase_c* pc_remoteClient)
{
  // check if the remote owner isoName is used for any other remote proc
  const IsoName_c& c_toBeDeletedOwnerisoName = pc_remoteClient->ownerISOName();
  bool b_otherRemoteWithSameOwner = false;
  MASK_TYPE ui32_filter;

  for ( cacheTypeC2_t pc_iter = c_arrClientC2.begin();
        ( pc_iter != c_arrClientC2.end() );
        pc_iter++ )
  {
    if ( (*pc_iter) == pc_remoteClient ) continue;
    if ((*pc_iter)->ownerISOName() == c_toBeDeletedOwnerisoName) b_otherRemoteWithSameOwner = true;
  }

  unregisterC2( pc_remoteClient );

  // set ptr to a defined position => avoid use of this pc_iter in deleteFilter() which is now postponed (timeEvent())
  pc_searchCacheC2 = c_arrClientC2.end() - 1;

  if ( !b_otherRemoteWithSameOwner )
  { // delete the remote filter that was created to receive messages from that owner
    if (getIsoMonitorInstance4Comm().existIsoMemberISOName(c_toBeDeletedOwnerisoName, true))
    { // remote owner exist and has claimed address -> check if corresponding FilterBox_c exist
      uint8_t ui8_recNr = getIsoMonitorInstance4Comm().isoMemberISOName(c_toBeDeletedOwnerisoName, true).nr();
      ui32_filter = (PROCESS_DATA_PGN << 8) | ui8_recNr;
      // delete corresponding FilterBox_c in timeEvent() to avoid problems when called in procdata cestructor
      l_filtersToDeleteISO.push_front(ui32_filter);
    } // owner exist with claimed address in isoMonitor
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
  if ((ui8_tcStatus != ui8_lastTcStatus) || ab_skipLastTcStatus)
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
    ui8_lastTcStatus = ui8_tcStatus;

  if (pc_processDataChangeHandler)
  { // call handler function if handler class is registered
    pc_processDataChangeHandler->processTcStatusMessage((1 == ui8_tcStatus) /* 1: task running */, rc_isoName.toConstIisoName_c());
  }

  return TRUE;
}


bool Process_c::processWorkingSetTaskMsg(uint8_t ui8_tcStatus, const IsoName_c& rc_isoName)
{
  if (pc_processWsmTaskMsgHandler)
    pc_processWsmTaskMsgHandler->processWsmTaskMessage(ui8_tcStatus, rc_isoName);
  return TRUE;
}

///  Used for Debugging Tasks in Scheduler_c
const char*
Process_c::getTaskName() const
{ return "Process_c"; }


//! Function set ui16_earlierInterval and
//! ui16_laterInterval that will be used by
//! getTimeToNextTrigger(retriggerType_t)
//! can be overloaded by Childclass for special condition
void
Process_c::updateEarlierAndLatestInterval(){
  ui16_earlierInterval = 0; //( ( getTimePeriod() * 3) / 4);
  ui16_latestInterval   =  ( getTimePeriod() / 2) ;
}

} // end of namespace __IsoAgLib
