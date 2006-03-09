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

#ifdef USE_DIN_GPS
  #include "gps_c.h"
#endif
#ifdef USE_DIN_9684
  #include <IsoAgLib/comm/SystemMgmt/DIN9684/impl/dinitem_c.h>
#endif

#ifdef USE_ISO_11783
  #include <IsoAgLib/comm/SystemMgmt/ISO11783/impl/isoitem_c.h>
#endif

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
  Process_c& getProcessInstance( uint8_t rui8_instance )
  { // if > 1 singleton instance is used, no static reference can be used
    return Process_c::instance( rui8_instance );
  };
#else
  /** C-style function, to get access to the unique Process_c singleton instance */
  Process_c& getProcessInstance( void )
  {
    static Process_c& c_process = Process_c::instance();
    return c_process;
  };
#endif

#if defined(USE_ISO_11783) && defined(USE_PROC_DATA_DESCRIPTION_POOL)
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
  #if defined(USE_ISO_11783)
  __IsoAgLib::getIsoMonitorInstance4Comm().registerSaClaimHandler( this );
    #ifdef USE_PROC_DATA_DESCRIPTION_POOL
    c_devPropertyHandler.init(&c_data);
    #endif
  #endif
  c_data.setSingletonKey( getSingletonVecKey() );

  // receive PROCESS_DATA_PGN messages which are addressed to GLOBAL
  uint32_t ui32_filter = ((static_cast<MASK_TYPE>(PROCESS_DATA_PGN) | static_cast<MASK_TYPE>(0xFF)) << 8);
  if (!getCanInstance4Comm().existFilter( *this, (0x1FFFF00UL), ui32_filter, Ident_c::ExtendedIdent))
  { // create FilterBox
    getCanInstance4Comm().insertFilter( *this, (0x1FFFF00UL), ui32_filter, true, Ident_c::ExtendedIdent);
  }

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

  getLbsErrInstance().registerError( LibErr_c::ElNonexistent, LibErr_c::LbsProcess );
}

/**
  if the amount of created local process data is known, then enough capacity for the
  vector with pointers to all of them can be reserved. Otherwise the vector
  will increase with several reallocations, where each reallocation triggers
  increase of capacity by factor 2 ( capacity is the amount of elements,
  which can be stored before reallocation takes place ).
  @param rui16_localProcCapacity
*/
void Process_c::localProcDataReserveCnt( uint16_t rui16_localProcCapacity )
{
  if ( c_arrClientC1.capacity() < rui16_localProcCapacity )
  { // trigger increase of capacity
    c_arrClientC1.reserve( rui16_localProcCapacity );
  }
}
/**
  if the amount of created remote process data is known, then enough capacity for the
  vector with pointers to all of them can be reserved. Otherwise the vector
  will increase with several reallocations, where each reallocation triggers
  increase of capacity by factor 2 ( capacity is the amount of elements,
  which can be stored before reallocation takes place ).
  @param rui16_remoteProcCapacity
*/
void Process_c::remoteProcDataReserveCnt( uint16_t rui16_remoteProcCapacity )
{
  if ( c_arrClientC2.capacity() < rui16_remoteProcCapacity )
  { // trigger increase of capacity
    c_arrClientC2.reserve( rui16_remoteProcCapacity );
  }
}


/**
  deliver reference to data pkg as reference to CANPkgExt_c
  to implement the base virtual function correct
*/
CANPkgExt_c& Process_c::dataBase()
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
  if ( Scheduler_c::getAvailableExecTime() == 0 ) return false;
  int32_t i32_time = Scheduler_c::getLastTimeEventTrigger();

#ifdef USE_ISO_11783
  if ( l_filtersToDeleteISO.size() > 0)
  {
    for (std::slist<uint32_t>::const_iterator iter = l_filtersToDeleteISO.begin();
         iter != l_filtersToDeleteISO.end();
         iter++)
    {
      if (getCanInstance4Comm().existFilter( *this, 0x1FF00FF, *iter, Ident_c::ExtendedIdent))
        // corresponding FilterBox_c exist -> delete it
        getCanInstance4Comm().deleteFilter( *this, 0x1FF00FF, *iter, Ident_c::ExtendedIdent);
    }
    l_filtersToDeleteISO.clear();
  }
#endif

#ifdef USE_DIN_9684
  if ( l_filtersToDeleteDIN.size() > 0)
  {
    for (std::slist<uint32_t>::const_iterator iter = l_filtersToDeleteDIN.begin();
         iter != l_filtersToDeleteDIN.end();
         iter++)
    {
      if (getCanInstance4Comm().existFilter( *this, 0x70F, *iter))
        // corresponding FilterBox_c exist -> delete it
        getCanInstance4Comm().deleteFilter( *this, 0x70F, *iter);
    }
    l_filtersToDeleteDIN.clear();
  }
#endif

#if defined(USE_ISO_11783) && defined(USE_PROC_DATA_DESCRIPTION_POOL)
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
      << "\r\n"
      << c_arrClientC2.size()
      << "(" << c_arrClientC2.capacity()
      << ") x RemoteProcData Pointer: Mal-Alloc: "
      << sizeVectorTWithMalloc( sizeof(uint16_t), c_arrClientC2.capacity() )
      << "/" << sizeof(void*)
      << ", Chunk-Alloc: "
      << sizeVectorTWithChunk( sizeof(uint16_t), c_arrClientC2.capacity() )
      #ifndef MASSERT
      << "\r\n\r\n";
      #else
      << "\r\n__mall tot:" << AllocateHeapMalloc
      << ", _mall deal tot: " << DeallocateHeapMalloc
      << "\r\n\r\n";
      #endif
  }
  #endif

  // call the time event for all local data
  for ( pc_searchCacheC1 = c_arrClientC1.begin();
       ( pc_searchCacheC1 != c_arrClientC1.end() );
       pc_searchCacheC1++ )
  { // delete item at pc_timeIter, if pc_searchCacheC1 points to pc_client
    if ( Scheduler_c::getAvailableExecTime() == 0 ) return false;
    if ( !(*pc_searchCacheC1)->timeEvent() ) b_result = false; /** @todo seemded to segfault here, although this is REALLY STRANGE! */
  }
  // call the time event for all remote data
  for ( pc_searchCacheC2 = c_arrClientC2.begin();
       ( pc_searchCacheC2 != c_arrClientC2.end() );
       pc_searchCacheC2++ )
  { // delete item at pc_timeIter, if pc_searchCacheC1 points to pc_client
    if ( Scheduler_c::getAvailableExecTime() == 0 ) return false;
    if ( !(*pc_searchCacheC2)->timeEvent() ) b_result = false;
  }
  // if local active member exist - check every second if
  // filters for targeted or partner process data should be created
  if ((i32_time - i32_lastFilterBoxTime) > 1000)  {
    i32_lastFilterBoxTime = i32_time;

    // create local Base-Proc Filter in case DIN member is active
    #ifdef USE_DIN_9684
    if ( getSystemMgmtInstance4Comm().existActiveLocalDinMember() )
    { // filter for base process data
    if (!getCanInstance4Comm().existFilter( *this, (uint16_t)(0x7C << 4),(uint16_t)(0x18 << 4) ))
      getCanInstance4Comm().insertFilter( *this, (uint16_t)(0x7C << 4),(uint16_t)(0x18 << 4), true);
    }
    #endif
  }
  // the other list elements doesn't need periodic actions
  return b_result;
};

/**
  start processing of a process msg
  ignore all invalid messages where SEND is not of a member with claimed address,
  or where EMPF isn't valid

  possible errors:
    * Err_c::elNonexistent on SEND/EMPF not registered in Monitor-List
  @return true -> message was processed; else the received CAN message will be served to other matching CANCustomer_c
*/
 bool Process_c::processMsg(){

#if defined(USE_ISO_11783) && defined(USE_PROC_DATA_DESCRIPTION_POOL)
// first check if this is a device property message -> then DevPropertyHandler_c should process this msg
if ( ( c_data.identType() == Ident_c::ExtendedIdent ) && ( ( ( c_data[0] & 0xF ) < 2 ) || ( c_data[0] >= 0xD ) ) )
{
  if (c_devPropertyHandler.processMsg()) return true;
}
#endif

#if defined(USE_DIN_GPS)
  // check if this is a message from a service
  if (
      ( (data().pri() == 3) && (data().send() == 3) && (data().lis() == 0) && (data().devClass() == 0) && (data().empf() == 0xF)  )
    ||( (data().pri() == 5) && (data().lis() == 3) && (data().wert() == 5) && (data().devClass() <= 1) )
     )
  { // messages from a GPS service are processed by GPS_c::processMsg
    return getGpsInstance4Comm().processMsg();
  }
#endif

#ifdef USE_DIN_9684
  uint8_t ui8_devClassInstData = 0xFF;
#endif
  bool b_result = false;

  #if defined(DEBUG) && defined(USE_DIN_9684)
  if ( ( data().wert() == 0 ) && ( data().inst() == 0 ) ) {
    IsoAgLib::getIrs232Instance()

        << "Process_c::processMsg() mit Alarm ACK\n";
  }
  #endif
  #ifdef USE_DIN_9684
  // try to aquire exact DEV_KEY code for the suitable Process Data Item
  // -> check for DEVCLASSINST of the owner of the handled Process Data
  if ((data().pri() == 0x2) || (data().pri() == 0x5))
  { // target and partner process data
    bool b_sendOwner = false,
         b_empfOwner = false;
    // first check if both SEND and EMPF are valid according Monitor-List
    if (!(data().existMemberSend())
      ||!(data().existMemberEmpf())
       )
    { // msg not valid as one of SEND or EMPF isn't registered with claimed address
      // not really an error - just this received messages is of no interest
      // getLbsErrInstance().registerError( LibErr_c::ElNonexistent, LibErr_c::LbsProcess );
      return false; // exit function
    }
    if (data().devClass() == ((data().memberSend()).devKey().getDevClass()) )
    { // the devClass of the sender is equivalent to the devClass of the data
      b_sendOwner = true;
    }
    if (data().devClass() == ((data().memberEmpf()).devKey().getDevClass()) )
    { // the devClass of the receiver is equivalent to the devClass of the data
      b_empfOwner = true;
    }
    if (b_sendOwner == b_empfOwner)
    { // both could be or could be not the owner of the data -> use dev class inst of the msg
      if (!getSystemMgmtInstance4Comm().existMemberDevKey( data().devKey() ) )
      { // no member with msg devClass/pos exist -> ignore pos for search
        ui8_devClassInstData = 0XFF;
      }
      else
      {
        ui8_devClassInstData = data().devClassInst();
      }
    }
    else
    { // only one could be the owner -> use its pos
      ui8_devClassInstData = (b_sendOwner)? ((data().memberSend()).devKey().getDevClassInst())
                                 : ((data().memberEmpf()).devKey().getDevClassInst());
    }
  }// if target msg
  else if (data().identType() == Ident_c::StandardIdent)
  { // for base msg the first try must be the devClass/pos of msg
    // first check if SEND is valid according Monitor-List
    if (!(data().existMemberSend()))
    { // msg not valid as SEND isn't registered with claimed address
      // not really an error - just this received messages is of no interest
      // getLbsErrInstance().registerError( LibErr_c::ElNonexistent, LibErr_c::LbsProcess );
      return false; // exit function
    }
    b_result = true;
    DINMonitor_c& c_din_monitor = getDinMonitorInstance4Comm();
    // - only if no member with this combination exist, try pos of sender
    if (!c_din_monitor.existDinMemberDevKey( data().devKey() ) )
    { // no member with get/pos of msg exist -> try devClass/pos of sender, if sender.devClass is equiv to msg.devClass
      if (data().devClass() == (c_din_monitor.dinMemberNr(data().send()).devKey().getDevClass()) )
      { // the devClass of the sender is equivalent to the devClass of the data
        ui8_devClassInstData = ((data().memberSend()).devKey().getDevClassInst());
      }
      else
      { // ignore pos for searching process data item
        ui8_devClassInstData = 0xFF;
      }
    } // if devClass/pos of msg does not exist as member
    else
    { // use devClass/pos of msg
      ui8_devClassInstData = data().devClassInst();
    }
  }
  #endif

  // decide which devClass to use for matching
  // ISO and DIN message: use devClass from corresponding monitor item for checks
  const uint8_t ui8_devClassReceiver = data().memberEmpf().devKey().getDevClass();
  // ISO only message: check for sender devClass (only in remote)
  const uint8_t ui8_devClassSender = data().memberSend().devKey().getDevClass();

  // now ui8_devClassInstData is the best guess for searching the appropriate prcess data item
  // check first for local Process Data
  if (existProcDataLocal(
#ifdef USE_ISO_11783
                         data().DDI(), data().element(),
#endif
#ifdef USE_DIN_9684
                         data().devClass(), ui8_devClassInstData, data().lis(), data().wert(), data().inst(), data().zaehlnum(),
#endif
                         ui8_devClassReceiver)
     )
  { // there exists an appropriate process data item -> let the item process the msg
    procDataLocal(
#ifdef USE_ISO_11783
                  data().DDI(), data().element(),
#endif
#ifdef USE_DIN_9684
                  data().devClass(), ui8_devClassInstData, data().lis(), data().wert(), data().inst(), data().zaehlnum(),
#endif
                  ui8_devClassReceiver
                  ).processMsg();
    b_result = true;
  }

  // now check for remote Process Data
  if (existProcDataRemote(
#ifdef USE_ISO_11783
                          data().DDI(), data().element(), ui8_devClassSender,
#endif
#ifdef USE_DIN_9684
                          data().devClass(), ui8_devClassInstData, data().lis(), data().wert(), data().inst(), data().zaehlnum(),
#endif
                          ui8_devClassReceiver
                          )
      )
  { // there exists an appropriate process data item -> let the item process the msg
    procDataRemote(
#ifdef USE_ISO_11783
                   data().DDI(), data().element(), ui8_devClassSender,
#endif
#ifdef USE_DIN_9684
                   data().devClass(), ui8_devClassInstData, data().lis(), data().wert(), data().inst(), data().zaehlnum(),
#endif
                   ui8_devClassReceiver
                   ).processMsg();
    b_result = true;
  }

  return b_result;
}

/**
  checks if a suitable ProcDataLocal_c item exist
  ISO parameter
  @param rui16_DDI
  @param rui16_element

  DIN parameter
  @param rui8_dataDevClass DEVCLASS of process data as specified in DIN process data message
  @param rui8_devClassInst optional device class inst code of searched local Process Data instance
                (only important if more DEVCLASS type members are active)
  @param rui8_lis LIS code of searched local Process Data instance
  @param rui8_wert WERT code of searched local Process Data instance
  @param rui8_inst INST code of searched local Process Data instance
  @param rui8_zaehlnum ZAEHLNUM  code of searched local Process Data instance

  @param rui8_devClassReceiver DEVCLASS code of searched local Process Data instance
  @return true -> suitable instance found
*/
bool Process_c::existProcDataLocal(
#ifdef USE_ISO_11783
                                   uint16_t rui16_DDI,
                                   uint16_t rui16_element,
#endif
#ifdef USE_DIN_9684
                                   uint8_t rui8_dataDevClass,
                                   uint8_t rui8_devClassInst,
                                   uint8_t rui8_lis,
                                   uint8_t rui8_wert,
                                   uint8_t rui8_inst,
                                   uint8_t rui8_zaehlnum,
#endif
                                   uint8_t rui8_devClassReceiver)
{
  return updateLocalCache(
#ifdef USE_ISO_11783
                          rui16_DDI, rui16_element,
#endif
#ifdef USE_DIN_9684
                          rui8_dataDevClass, rui8_devClassInst, rui8_lis, rui8_wert, rui8_inst, rui8_zaehlnum,
#endif
                          rui8_devClassReceiver);
}

  /**
    checks if a suitable ProcDataRemoteBase_c item exist
    ISO parameter
    @param rui16_DDI
    @param rui16_element

    DIN parameter
    @param rui8_dataDevClass DEVCLASS of process data as specified in DIN process data message
    @param rui8_devClassInst optional device class inst code of searched local Process Data instance
                  (only important if more DEVCLASS type members are active)
    @param rui8_lis LIS code of searched local Process Data instance
    @param rui8_wert WERT code of searched local Process Data instance
    @param rui8_inst INST code of searched local Process Data instance
    @param rui8_zaehlnum ZAEHLNUM  code of searched local Process Data instance

    common parameter
    @param rui8_devClassReceiver DEVCLASS code of searched local Process Data instance
    @return true -> suitable instance found
  */
bool Process_c::existProcDataRemote(
#ifdef USE_ISO_11783
                           uint16_t rui16_DDI,
                           uint16_t rui16_element,
                           uint8_t rui8_devClassSender,
#endif
#ifdef USE_DIN_9684
                           uint8_t rui8_dataDevClass,
                           uint8_t rui8_devClassInst,
                           uint8_t rui8_lis,
                           uint8_t rui8_wert,
                           uint8_t rui8_inst,
                           uint8_t rui8_zaehlnum,
#endif
                           uint8_t rui8_devClassReceiver)
{
 return updateRemoteCache(
#ifdef USE_ISO_11783
                          rui16_DDI, rui16_element, rui8_devClassSender,
#endif
#ifdef USE_DIN_9684
                          rui8_dataDevClass, rui8_devClassInst, rui8_lis, rui8_wert, rui8_inst, rui8_zaehlnum,
#endif
                          rui8_devClassReceiver);

}

/**
  search for suitable ProcDataLocal_c item; create on if not found AND if wanted

  possible errors:
      * Err_c::badAlloc not enough memory to add new ProcDataLocal_c
        (can cause badAlloc exception)
      * Err_c::elNonexistent if element not found and rb_doCreate == false
  ISO parameter
  @param rui16_DDI
  @param rui16_element

  DIN parameter
  @param rui8_dataDevClass DEVCLASS of process data as specified in DIN process data message
  @param rui8_devClassInst optional device class inst code of searched local Process Data instance
                (only important if more DEVCLASS type members are active)
  @param rui8_lis LIS code of searched local Process Data instance
  @param rui8_wert WERT code of searched local Process Data instance
  @param rui8_inst INST code of searched local Process Data instance
  @param rui8_zaehlnum ZAEHLNUM  code of searched local Process Data instance

  @param rui8_devClassReceiver DEVCLASS code of searched local Process Data instance
  @return reference to searched/created ProcDataLocal_c instance
*/
ProcDataLocalBase_c& Process_c::procDataLocal(
#ifdef USE_ISO_11783
                                     uint16_t rui16_DDI,
                                     uint16_t rui16_element,
#endif
#ifdef USE_DIN_9684
                                     uint8_t rui8_dataDevClass,
                                     uint8_t rui8_devClassInst,
                                     uint8_t rui8_lis,
                                     uint8_t rui8_wert,
                                     uint8_t rui8_inst,
                                     uint8_t rui8_zaehlnum,
#endif
                                     uint8_t rui8_devClassReceiver)
{
  bool b_found = updateLocalCache(
#ifdef USE_ISO_11783
                                  rui16_DDI, rui16_element,
#endif
#ifdef USE_DIN_9684
                                  rui8_dataDevClass, rui8_devClassInst, rui8_lis, rui8_wert, rui8_inst, rui8_zaehlnum,
#endif
                                  rui8_devClassReceiver);
  if (!b_found)
  { // not found and no creation wanted -> error
    getLbsErrInstance().registerError( LibErr_c::ElNonexistent, LibErr_c::LbsProcess );
  }
  return **pc_searchCacheC1;
}

  /**
    search for suitable ProcDataRemoteBase_c item; create on if not found AND if wanted

    possible errors:
        * Err_c::badAlloc not enough memory to add new ProcDataRemote_c
          (can cause badAlloc exception)
        * Err_c::elNonexistent if element not found and rb_doCreate == false
    ISO parameter
    @param rui16_DDI
    @param rui16_element

    DIN parameter
    @param rui8_dataDevClass DEVCLASS of process data as specified in DIN process data message
    @param rui8_devClassInst optional device class inst code of searched local Process Data instance
                  (only important if more DEVCLASS type members are active)
    @param rui8_lis LIS code of searched local Process Data instance
    @param rui8_wert WERT code of searched local10,64 Process Data instance
    @param rui8_inst INST code of searched local Process Data instance
    @param rui8_zaehlnum ZAEHLNUM  code of searched local Process Data instance

    common parameter
    @param rui8_devClassReceiver DEVCLASS code of searched local Process Data instance
    @return reference to searched/created ProcDataRemoteBase_c instance
    @exception badAlloc
  */
ProcDataRemoteBase_c& Process_c::procDataRemote(
#ifdef USE_ISO_11783
                                                uint16_t rui16_DDI,
                                                uint16_t rui16_element,
                                                uint8_t rui8_devClassSender,
#endif
#ifdef USE_DIN_9684
                                                uint8_t rui8_dataDevClass,
                                                uint8_t rui8_devClassInst,
                                                uint8_t rui8_lis,
                                                uint8_t rui8_wert,
                                                uint8_t rui8_inst,
                                                uint8_t rui8_zaehlnum,
#endif
                                                uint8_t rui8_devClassReceiver)
{
  bool b_found = updateRemoteCache(
#ifdef USE_ISO_11783
                                   rui16_DDI, rui16_element, rui8_devClassSender,
#endif
#ifdef USE_DIN_9684
                                   rui8_dataDevClass, rui8_devClassInst, rui8_lis, rui8_wert, rui8_inst, rui8_zaehlnum,
#endif
                                   rui8_devClassReceiver);

  if (!b_found)
  { // not found and no creation wanted -> error
    getLbsErrInstance().registerError( LibErr_c::ElNonexistent, LibErr_c::LbsProcess );
  }
  return **pc_searchCacheC2;
}

/**
  delivers count of local process data entries with similar ident
  (which differs only in _instance_ of owner)
  ISO parameter
  @param rui16_DDI
  @param rui16_element

  DIN parameter
  @param rui8_lis LIS code of searched local Process Data instance
  @param rui8_wert WERT code of searched local Process Data instance
  @param rui8_inst INST code of searched local Process Data instance
  @param rui8_zaehlnum ZAEHLNUM  code of searched local Process Data instance

  common parameter
  @param rui8_devClass DEVCLASS code of searched local Process Data instance
  @return count of similar local process data entries
*/
uint8_t Process_c::procDataLocalCnt(
#ifdef USE_ISO_11783
                                    uint16_t rui16_DDI,
                                    uint16_t rui16_element,
#endif
#ifdef USE_DIN_9684
                                    uint8_t rui8_lis,
                                    uint8_t rui8_wert,
                                    uint8_t rui8_inst,
                                    uint8_t rui8_zaehlnum,
#endif
                                    uint8_t rui8_devClass)
{
  uint8_t ui8_cnt=0;

  for ( cacheTypeC1_t pc_iter = c_arrClientC1.begin();
       ( pc_iter != c_arrClientC1.end() );
       pc_iter++ )
  { // search for all local items which match the searched identity

#if defined(USE_ISO_11783) && defined(USE_DIN_9684)
    if (data().identType() == Ident_c::StandardIdent) {
      // DIN
      if ((*pc_iter)->matchDIN(rui8_devClass, rui8_lis, rui8_wert, rui8_inst, rui8_zaehlnum, 0xFF))
        ui8_cnt++;
    } else {
      // ISO
      // don't check sender devClass => 0xFF
      if ((*pc_iter)->matchISO(rui8_devClass, 0xFF, rui16_DDI, rui16_element))
        ui8_cnt++;
    }
#else
  #ifdef USE_ISO_11783
    // don't check sender devClass => 0xFF
    if ((*pc_iter)->matchISO(rui8_devClass, 0xFF, rui16_DDI, rui16_element))
      ui8_cnt++;
  #endif
  #ifdef USE_DIN_9684
    if ((*pc_iter)->matchDIN(rui8_devClass, rui8_lis, rui8_wert, rui8_inst, rui8_zaehlnum, 0xFF))
      ui8_cnt++;
  #endif
#endif
  }
  return ui8_cnt;
}

/**
  delivers count of remote process data entries with similar ident
  (which differs only in _instance_ of owner)
  ISO parameter
  @param rui16_DDI
  @param rui16_element
  @param rui8_devClassSender devClass of the sender (used for check against ownerDevKey().getDevClass())

  DIN parameter
  @param rui8_lis LIS code of searched local Process Data instance
  @param rui8_wert WERT code of searched local Process Data instance
  @param rui8_inst INST code of searched local Process Data instance
  @param rui8_zaehlnum ZAEHLNUM  code of searched local Process Data instance

  common parameter
  @param rui8_devClass DEVCLASS code of searched local Process Data instance
  @return count of similar remote process data entries
*/
uint8_t Process_c::procDataRemoteCnt(
#ifdef USE_ISO_11783
                                     uint16_t rui16_DDI,
                                     uint16_t rui16_element,
                                     uint8_t rui8_devClassSender,
#endif
#ifdef USE_DIN_9684
                                     uint8_t rui8_lis,
                                     uint8_t rui8_wert,
                                     uint8_t rui8_inst,
                                     uint8_t rui8_zaehlnum,
#endif
                                     uint8_t rui8_devClass)
{


  uint8_t ui8_cnt=0;

  for ( cacheTypeC2_t pc_iter = c_arrClientC2.begin();
       ( pc_iter != c_arrClientC2.end() );
       pc_iter++ )
  { // search for all local items which match the searched identity
#if defined(USE_ISO_11783) && defined(USE_DIN_9684)
    if (data().identType() == Ident_c::StandardIdent) {
      // DIN
      if ((*pc_iter)->matchDIN(rui8_devClass, rui8_lis, rui8_wert, rui8_inst, rui8_zaehlnum, 0xFF))
        ui8_cnt++;
    } else {
      // ISO
      if ((*pc_iter)->matchISO(rui8_devClass, rui8_devClassSender, rui16_DDI, rui16_element))
        ui8_cnt++;
    }
#else
  #ifdef USE_ISO_11783
    if ((*pc_iter)->matchISO(rui8_devClass, rui8_devClassSender, rui16_DDI, rui16_element))
      ui8_cnt++;
  #endif
  #ifdef USE_DIN_9684
    if ((*pc_iter)->matchDIN(rui8_devClass, rui8_lis, rui8_wert, rui8_inst, rui8_zaehlnum, 0xFF))
      ui8_cnt++;
  #endif
#endif

  }
  return ui8_cnt;
}

/**
  update the cache with search for according ProcDataLocal_c item
  ISO parameter
  @param rui16_DDI
  @param rui16_element

  DIN parameter
  @param rui8_dataDevClass DEVCLASS of process data as specified in DIN process data message
  @param rui8_devClassInst optional device class inst code of searched local Process Data instance
                (only important if more DEVCLASS type members are active)
  @param rui8_lis LIS code of searched local Process Data instance
  @param rui8_wert WERT code of searched local Process Data instance
  @param rui8_inst INST code of searched local Process Data instance
  @param rui8_zaehlnum ZAEHLNUM  code of searched local Process Data instance

  @param rui8_devClassReceiver DEVCLASS code of created local Process Data instance
*/
bool Process_c::updateLocalCache(
#ifdef USE_ISO_11783
                                 uint16_t rui16_DDI,
                                 uint16_t rui16_element,
#endif
#ifdef USE_DIN_9684
                                 uint8_t rui8_dataDevClass,
                                 uint8_t rui8_devClassInst,
                                 uint8_t rui8_lis,
                                 uint8_t rui8_wert,
                                 uint8_t rui8_inst,
                                 uint8_t rui8_zaehlnum,
#endif
                                 uint8_t rui8_devClassReceiver)
{
  bool b_foundLazy = false;

  if (!c_arrClientC1.empty())
  {
    if ( pc_searchCacheC1 != c_arrClientC1.end() )
    {

#if defined(USE_ISO_11783) && defined(USE_DIN_9684)
      if (data().identType() == Ident_c::StandardIdent) {
        // DIN
        if ((*pc_searchCacheC1)->matchDIN(rui8_dataDevClass, rui8_lis, rui8_wert, rui8_inst, rui8_zaehlnum, rui8_devClassInst)) return true;
      } else {
        // ISO
        // don't check sender devClass => 0xFF
        if ((*pc_searchCacheC1)->matchISO(rui8_devClassReceiver, 0xFF, rui16_DDI, rui16_element)) return true;
      }
#else
  #ifdef USE_ISO_11783
      // don't check sender devClass => 0xFF
      if ((*pc_searchCacheC1)->matchISO(rui8_devClassReceiver, 0xFF, rui16_DDI, rui16_element)) return true;
  #endif
  #ifdef USE_DIN_9684
      if ((*pc_searchCacheC1)->matchDIN(rui8_dataDevClass, rui8_lis, rui8_wert, rui8_inst, rui8_zaehlnum, rui8_devClassInst)) return true;
  #endif
#endif
    }
    //old cache doesn't match any more -> search new
    for ( cacheTypeC1_t pc_iter = c_arrClientC1.begin();
        ( pc_iter != c_arrClientC1.end() );
        pc_iter++ )
    { // check for lazy match with INSTANCE == 0xFF (==joker)

      bool b_matched = false;

#if defined(USE_ISO_11783) && defined(USE_DIN_9684)
      if (data().identType() == Ident_c::StandardIdent) {
        // DIN
        if ((*pc_iter)->matchDIN(rui8_dataDevClass, rui8_lis, rui8_wert, rui8_inst, rui8_zaehlnum, 0xFF))
          b_matched = true;
      } else {
        // ISO
        // don't check sender devClass => 0xFF
        if ((*pc_iter)->matchISO(rui8_devClassReceiver, 0xFF, rui16_DDI, rui16_element))
          b_matched = true;
      }
#else
  #ifdef USE_ISO_11783
      // don't check sender devClass => 0xFF
      if ((*pc_iter)->matchISO(rui8_devClassReceiver, 0xFF, rui16_DDI, rui16_element))
        b_matched = true;
  #endif
  #ifdef USE_DIN_9684
      if ((*pc_iter)->matchDIN(rui8_dataDevClass, rui8_lis, rui8_wert, rui8_inst, rui8_zaehlnum, 0xFF))
        b_matched = true;
  #endif
#endif

      if (b_matched)
      { // matches at least lazy
        #ifdef USE_DIN_9684
        if ((*pc_iter)->devClassInst() == rui8_devClassInst)
        { //exact match
          b_foundLazy = true;
          pc_searchCacheC1 = pc_iter;
          // stop search
          break;
        } // exact match
        else if (b_foundLazy == false)
        { // no other lazy match before this was found
          b_foundLazy = true;
          pc_searchCacheC1 = pc_iter;
        } //first lazy match for this search
        #else
        // for ISO - no exact compare including _instance_ is possible, as this is not transfered in data message
        b_foundLazy = true;
        pc_searchCacheC1 = pc_iter;
        #endif
      } // check lazy match
    }// for
  }

  return b_foundLazy;
}

  /**
    update the cache with search for according ProcDataRemoteBase_c item
    ISO parameter
    @param rui16_DDI
    @param rui16_element

    DIN parameter
    @param rui8_dataDevClass DEVCLASS of process data as specified in DIN process data message
    @param rui8_devClassInst optional device class inst code of searched local Process Data instance
                  (only important if more DEVCLASS type members are active)
    @param rui8_lis LIS code of searched local Process Data instance
    @param rui8_wert WERT code of searched local Process Data instance
    @param rui8_inst INST code of searched local Process Data instance
    @param rui8_zaehlnum ZAEHLNUM  code of searched local Process Data instance

    common parameter
    @param rui8_devClassReceiver DEVCLASS code of searched local Process Data instance
  */
bool Process_c::updateRemoteCache(
#ifdef USE_ISO_11783
                         uint16_t rui16_DDI,
                         uint16_t rui16_element,
                         uint8_t rui8_devClassSender,
#endif
#ifdef USE_DIN_9684
                         uint8_t rui8_dataDevClass,
                         uint8_t rui8_devClassInst,
                         uint8_t rui8_lis,
                         uint8_t rui8_wert,
                         uint8_t rui8_inst,
                         uint8_t rui8_zaehlnum,
#endif
                         uint8_t rui8_devClassReceiver)
{
  bool b_foundLazy = false;
  if (!c_arrClientC2.empty())
  {
    if ( pc_searchCacheC2 != c_arrClientC2.end() )
    {
#if defined(USE_ISO_11783) && defined(USE_DIN_9684)
      if (data().identType() == Ident_c::StandardIdent) {
        // DIN
        if ((*pc_searchCacheC2)->matchDIN(rui8_dataDevClass, rui8_lis, rui8_wert, rui8_inst, rui8_zaehlnum, rui8_devClassInst)) return true;
      } else {
        // ISO
        if ((*pc_searchCacheC2)->matchISO(rui8_devClassReceiver, rui8_devClassSender, rui16_DDI, rui16_element)) return true;
      }
#else
  #ifdef USE_ISO_11783
      if ((*pc_searchCacheC2)->matchISO(rui8_devClassReceiver, rui8_devClassSender, rui16_DDI, rui16_element)) return true;
  #endif
  #ifdef USE_DIN_9684
      if ((*pc_searchCacheC2)->matchDIN(rui8_dataDevClass, rui8_lis, rui8_wert, rui8_inst, rui8_zaehlnum, rui8_devClassInst)) return true;
  #endif
#endif

    }
    //old cache doesn't match any more -> search new
    for ( cacheTypeC2_t pc_iter = c_arrClientC2.begin(); //list of remote process data
        ( pc_iter != c_arrClientC2.end() );
        pc_iter++ )
    { // check for lazy match with INSTANCE == 0xFF (==joker)
      bool b_matched = false;

#if defined(USE_ISO_11783) && defined(USE_DIN_9684)
      if (data().identType() == Ident_c::StandardIdent) {
        // DIN
        if ((*pc_iter)->matchDIN(rui8_dataDevClass, rui8_lis, rui8_wert, rui8_inst, rui8_zaehlnum, 0xFF))
          b_matched = true;
      } else {
        // ISO
        if ((*pc_iter)->matchISO(rui8_devClassReceiver, rui8_devClassSender, rui16_DDI, rui16_element))
          b_matched = true;
      }
#else
  #ifdef USE_ISO_11783
      if ((*pc_iter)->matchISO(rui8_devClassReceiver, rui8_devClassSender, rui16_DDI, rui16_element))
        b_matched = true;
  #endif
  #ifdef USE_DIN_9684
      if ((*pc_iter)->matchDIN(rui8_dataDevClass, rui8_lis, rui8_wert, rui8_inst, rui8_zaehlnum, 0xFF))
        b_matched = true;
  #endif
#endif

      if (b_matched)
      { // matches at least lazy
        #ifdef USE_DIN_9684
        if ((*pc_iter)->devClassInst() == rui8_devClassInst)
        { //exact match
          b_foundLazy = true;
          pc_searchCacheC2 = pc_iter;
          // stop search
          break;
        } // exact match
        else if (b_foundLazy == false)
        { // no other lazy match before this was found
          b_foundLazy = true;
          pc_searchCacheC2 = pc_iter;
        } //first lazy match for this search
        #else
        b_foundLazy = true;
        pc_searchCacheC2 = pc_iter;
        #endif
      } // check lazy match
    }// for
  }
  return b_foundLazy;
}

#ifdef USE_ISO_11783
bool Process_c::checkAndAddMatchingDDI2Group(uint16_t rui16_DDI, uint16_t rui_deviceElement, const DevKey_c& rc_devKey)
{
  ProcDataRemoteBase_c* pc_remoteProcessData = check4DDIGroupMatch(rui16_DDI, rui_deviceElement, rc_devKey);

  if (NULL == pc_remoteProcessData) return false;

  return pc_remoteProcessData->add2Group(rui16_DDI);
};


bool Process_c::addProprietaryDDI2Group(uint16_t rui16_DDI, uint16_t rui_deviceElement, bool b_isSetpoint, GeneralCommand_c::ValueGroup_t ddiType, const DevKey_c &rc_devKey)
{
  ProcDataRemoteBase_c* pc_remoteProcessData = check4ProprietaryDDIGroupMatch(rui_deviceElement, rc_devKey);

  if (NULL == pc_remoteProcessData) return false;

  return pc_remoteProcessData->addProprietary2Group(rui16_DDI, b_isSetpoint, ddiType);
};


ProcDataRemoteBase_c* Process_c::check4DDIGroupMatch(uint16_t rui16_DDI, uint16_t rui_deviceElement, const DevKey_c& rc_devKey)
{
  for ( cacheTypeC2_t pc_iter = c_arrClientC2.begin(); //list of remote process data
        ( pc_iter != c_arrClientC2.end() );
        pc_iter++ )
  {
    if ((*pc_iter)->check4GroupMatch(rui16_DDI, rui_deviceElement, rc_devKey))
      return *pc_iter;
  }// for

  return NULL;
}

ProcDataRemoteBase_c* Process_c::check4ProprietaryDDIGroupMatch(uint16_t rui_deviceElement, const DevKey_c &rc_devKey)
{
  for ( cacheTypeC2_t pc_iter = c_arrClientC2.begin(); //list of remote process data
        ( pc_iter != c_arrClientC2.end() );
        pc_iter++ )
  {
    if ((*pc_iter)->checkProprietary4GroupMatch(rui_deviceElement, rc_devKey))
      return *pc_iter;
  }// for

  return NULL;
};
#endif

/**
  delete FilterBox_c for receive from remote devKey if needed
  (important to delete old Filter Boxes after deletion of
  of remote device from monitor list or after re-adressclaim with different SA)
  @param rc_ownerDevKey DEVKEY code of remote owner who sent the message
  @param rui8_pri PRI code of messages with this process data instance (default 2)
  @return true -> member exist and Filter Box deleted
*/
bool Process_c::deleteRemoteFilter(const DevKey_c& rc_ownerDevKey, uint8_t
  #ifdef USE_DIN_9684
  rui8_pri
  #endif
  )
{
  bool b_result = false,
       b_found = false;
  MASK_TYPE ui32_filter;

  for ( pc_searchCacheC2 = c_arrClientC2.begin();
      ( pc_searchCacheC2 != c_arrClientC2.end() );
      pc_searchCacheC2++ )
  {
    if ((*pc_searchCacheC2)->ownerDevKey() == rc_ownerDevKey) b_found = true;
  }
  if (b_found)
  { // remote proc data has given onwerDevKey
    // -> delete according FilterBox
    #ifdef USE_DIN_9684
    if (getDinMonitorInstance4Comm().existDinMemberDevKey(rc_ownerDevKey, true))
    { // remote owner exist and has claimed address -> check if corresponding FilterBox_c exist
      uint8_t ui8_recNr = getDinMonitorInstance4Comm().dinMemberDevKey(rc_ownerDevKey, true).nr();
      ui32_filter = (ui8_recNr | (rui8_pri << 8));
      if (getCanInstance4Comm().existFilter( *this, 0x70F, ui32_filter))
      { // corresponding FilterBox_c exist -> delete it
        getCanInstance4Comm().deleteFilter( *this, 0x70F, ui32_filter);
        b_result = true;
      }
    } // owner exist with claimed address in memberMonitor
    #endif
    #ifdef USE_ISO_11783
    if (getIsoMonitorInstance4Comm().existIsoMemberDevKey(rc_ownerDevKey, true))
    { // remote owner exist and has claimed address -> check if corresponding FilterBox_c exist
      uint8_t ui8_recNr = getIsoMonitorInstance4Comm().isoMemberDevKey(rc_ownerDevKey, true).nr();
      ui32_filter = (PROCESS_DATA_PGN << 8) | ui8_recNr;
      if (getCanInstance4Comm().existFilter( *this, 0x1FF00FF, ui32_filter, Ident_c::ExtendedIdent))
      { // corresponding FilterBox_c exist -> delete it
        getCanInstance4Comm().deleteFilter( *this, 0x1FF00FF, ui32_filter, Ident_c::ExtendedIdent);
        b_result = true;
      }
    } // owner exist with claimed address in isoMonitor
    #endif
  } // not other remote proc data with ownerDevKey found
  // only reconfigure if new FilterBox_c created -> signalled by b_result == true

  if (b_result)
  {
    getCanInstance4Comm().reconfigureMsgObj();
  }
  return b_result;
}

/**
  insert FilterBox_c for receive from remote devKey if needed
  @param rc_ownerDevKey DEVKEY code of remote owner who sent the message
  @param rui8_pri PRI code of messages with this process data instance (default 2)
  @return true -> member exist and Filter Box created
 */
bool Process_c::createRemoteFilter(const DevKey_c& rc_ownerDevKey, uint8_t
#ifdef USE_DIN_9684
  rui8_pri
#endif
                                  )
{
  bool b_result = false;
  MASK_TYPE t_filter;
  #ifdef USE_DIN_9684
  if (getDinMonitorInstance4Comm().existDinMemberDevKey(rc_ownerDevKey, true))
  { // remote owner exist and has claimed address -> check if suitable FilterBox_c exist
    const uint8_t ui8_recNr = getDinMonitorInstance4Comm().dinMemberDevKey(rc_ownerDevKey, true).nr();
    t_filter = (ui8_recNr | (rui8_pri << 8));
    #ifdef USE_ISO_11783
    if (!getCanInstance4Comm().existFilter( *this, 0x70F, t_filter, Ident_c::StandardIdent))
    #else
    if (!getCanInstance4Comm().existFilter( *this, 0x70F, t_filter))
    #endif
    { // no suitable FilterBox_c exist -> create it
      getCanInstance4Comm().insertFilter( *this, 0x70F, t_filter, false);
      b_result = true;
    }
  }
  #endif
  #ifdef USE_ISO_11783
  if (getIsoMonitorInstance4Comm().existIsoMemberDevKey(rc_ownerDevKey, true))
  { // remote owner exist and has claimed address -> check if suitable FilterBox_c exist
    const uint8_t ui8_recNr = getIsoMonitorInstance4Comm().isoMemberDevKey(rc_ownerDevKey, true).nr();
    // only receive msg from ui8_recNr / rc_ownerDevKey to all other devices
    t_filter = (PROCESS_DATA_PGN << 8) | ui8_recNr;
    if (!getCanInstance4Comm().existFilter( *this, 0x1FF00FFUL, t_filter, Ident_c::ExtendedIdent))
    { // no suitable FilterBox_c exist -> create it
      getCanInstance4Comm().insertFilter( *this, 0x1FF00FFUL, t_filter, false, Ident_c::ExtendedIdent);
      b_result = true;
    }
  }
  #endif
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
bool Process_c::checkCreateRemoteReceiveFilter(const DevKey_c* rpc_checkOnlyOwner)
{
  bool b_result = false;
  const DevKey_c *pc_lastFilterDevKey = NULL;
  const DevKey_c *pc_actDevKey = NULL;
  uint8_t ui8_lastFilterPri = 0, ui8_actPri = 2;
  for ( pc_searchCacheC2 = c_arrClientC2.begin();
        ( pc_searchCacheC2 != c_arrClientC2.end() );
        pc_searchCacheC2++ )
  { // delete item at pc_timeIter, if pc_searchCacheC2 points to pc_client
    pc_actDevKey = &((*pc_searchCacheC2)->ownerDevKey());
    ui8_actPri = (*pc_searchCacheC2)->pri();
    if ( (ui8_actPri != ui8_lastFilterPri) && (*pc_actDevKey != DevKey_c::DevKeyUnspecified)
      && ( ( NULL == pc_lastFilterDevKey ) || (*pc_actDevKey != *pc_lastFilterDevKey) )
      && ( ( NULL == rpc_checkOnlyOwner  ) || (*pc_actDevKey == *rpc_checkOnlyOwner ) )
       )
    { // last FilterBox_c call with other devKey
      // -> avoid unneccessary calls with search
      pc_lastFilterDevKey = pc_actDevKey;
      ui8_lastFilterPri = ui8_actPri;
      if ( createRemoteFilter(*pc_actDevKey, ui8_actPri) ) b_result = true;
    }
  }
  return b_result;
}

/** this function is called by ISOMonitor_c when a new CLAIMED ISOItem_c is registered.
  * @param refc_devKey const reference to the item which ISOItem_c state is changed
  * @param rpc_newItem pointer to the currently corresponding ISOItem_c
  */
void Process_c::reactOnMonitorListAdd( const DevKey_c& refc_devKey, const ISOItem_c* rpc_newItem )
{ // create FilterBoxes for remote ProcessData if needed
  if ( getSystemMgmtInstance4Comm().existLocalMemberDevKey(refc_devKey) )
  { // lcoal ISOItem_c has finished adr claim
    uint32_t ui32_nr = rpc_newItem->nr();
          // only ISO msgs with own SA in PS (destination)
    uint32_t ui32_filter = ((static_cast<MASK_TYPE>(PROCESS_DATA_PGN) | static_cast<MASK_TYPE>(ui32_nr)) << 8);
    if (!getCanInstance4Comm().existFilter( *this, (0x1FFFF00UL), ui32_filter, Ident_c::ExtendedIdent))
    { // create FilterBox
      getCanInstance4Comm().insertFilter( *this, (0x1FFFF00UL), ui32_filter, true, Ident_c::ExtendedIdent);
    }
  }
  else
  { // remote ISOItem_c has finished adr claim
    checkCreateRemoteReceiveFilter( &refc_devKey );
  }
}

/** this function is called by ISOMonitor_c when a device looses its ISOItem_c.
  * @param refc_devKey const reference to the item which ISOItem_c state is changed
  * @param rui8_oldSa previously used SA which is NOW LOST -> clients which were connected to this item can react explicitly
  */
void Process_c::reactOnMonitorListRemove( const DevKey_c& refc_devKey, uint8_t rui8_oldSa )
{
  if ( getSystemMgmtInstance4Comm().existLocalMemberDevKey(refc_devKey) )
  { // lcoal ISOItem_c has lost SA
    uint32_t ui32_nr = rui8_oldSa;
          // only ISO msgs with own SA in PS (destination)
    uint32_t ui32_filter = ((static_cast<MASK_TYPE>(PROCESS_DATA_PGN) | static_cast<MASK_TYPE>(ui32_nr)) << 8);
    if (getCanInstance4Comm().existFilter( *this, (0x1FFFF00UL), ui32_filter, Ident_c::ExtendedIdent))
    { // create FilterBox
      getCanInstance4Comm().deleteFilter( *this, (0x1FFFF00UL), ui32_filter, Ident_c::ExtendedIdent);
    }
  }
  else
  { // remote ISOItem_c
    deleteRemoteFilter(refc_devKey);
  }
}
/** register pointer to a new remote process data instance
  * this function is called within construction of new remote process data instance
  */
bool Process_c::registerRemoteProcessData( ProcDataRemoteBase_c* pc_remoteClient)
{
  const bool cb_result = registerC2( pc_remoteClient );
  checkCreateRemoteReceiveFilter( &(pc_remoteClient->ownerDevKey()) );
  return cb_result;
}

/** unregister pointer to a already registered remote process data instance
  * this function is called within destruction of remote process data instance
  */
void Process_c::unregisterRemoteProcessData( ProcDataRemoteBase_c* pc_remoteClient)
{
  // check if the remote owner DEVKEY is used for any other remote proc
  const DevKey_c& c_toBeDeletedOwnerDevKey = pc_remoteClient->ownerDevKey();
  bool b_otherRemoteWithSameOwner = false;
  MASK_TYPE ui32_filter;

  for ( pc_searchCacheC2 = c_arrClientC2.begin();
      ( pc_searchCacheC2 != c_arrClientC2.end() );
      pc_searchCacheC2++ )
  {
    if ( (*pc_searchCacheC2) == pc_remoteClient ) continue;
    if ((*pc_searchCacheC2)->ownerDevKey() == c_toBeDeletedOwnerDevKey) b_otherRemoteWithSameOwner = true;
  }

  unregisterC2( pc_remoteClient );

  // set ptr to a defined position => avoid use of this pc_searchCacheC2 in deleteFilter() which is now postponed (timeEvent())
  pc_searchCacheC2 = c_arrClientC2.end();

  if ( !b_otherRemoteWithSameOwner )
  { // delete the remote filter that was created to receive messages from that owner
    #ifdef USE_DIN_9684
    if (getDinMonitorInstance4Comm().existDinMemberDevKey(c_toBeDeletedOwnerDevKey, true))
    { // remote owner exist and has claimed address -> check if corresponding FilterBox_c exist
      uint8_t ui8_recNr = getDinMonitorInstance4Comm().dinMemberDevKey(c_toBeDeletedOwnerDevKey, true).nr();
      ui32_filter = (ui8_recNr | (pc_remoteClient->pri() << 8));
      // delete corresponding FilterBox_c in timeEvent() to avoid problems when called in procdata cestructor
      l_filtersToDeleteDIN.push_front(ui32_filter);
    } // owner exist with claimed address in memberMonitor
    #endif
    #ifdef USE_ISO_11783
    if (getIsoMonitorInstance4Comm().existIsoMemberDevKey(c_toBeDeletedOwnerDevKey, true))
    { // remote owner exist and has claimed address -> check if corresponding FilterBox_c exist
      uint8_t ui8_recNr = getIsoMonitorInstance4Comm().isoMemberDevKey(c_toBeDeletedOwnerDevKey, true).nr();
      ui32_filter = (PROCESS_DATA_PGN << 8) | ui8_recNr;
      // delete corresponding FilterBox_c in timeEvent() to avoid problems when called in procdata cestructor
      l_filtersToDeleteISO.push_front(ui32_filter);
    } // owner exist with claimed address in isoMonitor
    #endif
  }

};


} // end of namespace __IsoAgLib
