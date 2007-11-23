/***************************************************************************
                          managemeasureproglocal_c.cpp  -  description
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
#include "managemeasureproglocal_c.h"
#include "../../../impl/process_c.h"


#if defined(DEBUG) || defined(DEBUG_HEAP_USEAGE)
  #include <supplementary_driver/driver/rs232/impl/rs232io_c.h>
  #include <IsoAgLib/util/impl/util_funcs.h>
#endif

#ifdef DEBUG_HEAP_USEAGE
static uint16_t sui16_MeasureProgLocalTotal = 0;
static uint16_t sui16_lastPrintedMeasureProgLocalTotal = 0;
static uint16_t sui16_deconstructMeasureProgLocalTotal = 0;
static uint16_t sui16_printedDeconstructMeasureProgLocalTotal = 0;

#ifdef MASSERT
extern unsigned int AllocateHeapMalloc;
extern unsigned int DeallocateHeapMalloc;
#endif
#endif

namespace __IsoAgLib {
/** create first default measure prog, if no measure prog in list */
void ManageMeasureProgLocal_c::checkInitList( void )
{
  if ( mvecc_prog.size() > 0 ) return;
  // insert default entry
  if ( pprocessData() != NULL )
  {
    ProcDataLocalBase_c* pc_procdata =
      static_cast<ProcDataLocalBase_c*>(pprocessData());
    vec_prog().push_front(MeasureProgLocal_c(pc_procdata, Proc_c::UndefinedProg,
                                              pc_procdata->masterMeasurementVal()
                                              #ifdef USE_EEPROM_IO
                                                , pc_procdata->eepromVal()
                                              #endif
                                              ));
  }
  else
  { // insert default entry wihtout connection to root proc-data class into list
    vec_prog().push_front(MeasureProgLocal_c(0, Proc_c::UndefinedProg ) );
  }
  if (vec_prog().size() < 1)
  { // first element added without success
    getILibErrInstance().registerError( iLibErr_c::BadAlloc, iLibErr_c::Process );
  }
  #ifdef DEBUG_HEAP_USEAGE
  else
  {
    sui16_MeasureProgLocalTotal++;
  }
  #endif
  // point cache initially to default first element
  mpc_progCache = mvecc_prog.begin();
}

/**
  initialise this ManageMeasureProgLocal_c instance to a well defined initial state
  @param apc_processData optional pointer to containing ProcessData instance
*/
void ManageMeasureProgLocal_c::init( ProcDataBase_c *const apc_processData )
{ // set the pointer to the corresponging process data class
  ProcessElementBase_c::set( apc_processData );
  // as init() can be also called to re-init (bring this instance to well
  // defined starting condition, clearthe measure-prog array
  #ifdef DEBUG_HEAP_USEAGE
  sui16_MeasureProgLocalTotal -= mvecc_prog.size();
  sui16_deconstructMeasureProgLocalTotal += mvecc_prog.size();
  #endif
  mvecc_prog.clear();
  mpc_progCache = vec_prog().begin();
}
/** copy constructor */
ManageMeasureProgLocal_c::ManageMeasureProgLocal_c( const ManageMeasureProgLocal_c& arc_src )
: ProcessElementBase_c( arc_src )
{
  assignFromSource( arc_src );
}
/** assignment operator */
const ManageMeasureProgLocal_c& ManageMeasureProgLocal_c::operator=( const ManageMeasureProgLocal_c& arc_src )
{
  ProcessElementBase_c::operator=( arc_src );
  assignFromSource( arc_src );
  return *this;
}
/** base function for assignment of element vars for copy constructor and operator= */
void ManageMeasureProgLocal_c::assignFromSource( const ManageMeasureProgLocal_c& arc_src )
{ // copy dynamic array
  mvecc_prog = arc_src.mvecc_prog;
  // now initialise the elements
  if (vec_prog().size() < arc_src.constVecProg().size())
  { // not all items copied
    getILibErrInstance().registerError( iLibErr_c::BadAlloc, iLibErr_c::Process );
  }
  #ifdef DEBUG_HEAP_USEAGE
  else
  {
    sui16_MeasureProgLocalTotal += vec_prog().size();

    if ( ( ( vec_prog().size() > 0 ) && ( sui16_lastPrintedMeasureProgLocalTotal != sui16_MeasureProgLocalTotal ) )
      || ( sui16_lastPrintedMeasureProgLocalTotal != sui16_MeasureProgLocalTotal                                  ) )
    {
      sui16_lastPrintedMeasureProgLocalTotal = sui16_MeasureProgLocalTotal;
      sui16_printedDeconstructMeasureProgLocalTotal = sui16_deconstructMeasureProgLocalTotal;
      INTERNAL_DEBUG_DEVICE
        << sui16_MeasureProgLocalTotal << " x MeasureProgLocal_c: Mal-Alloc: "
        <<  sizeSlistTWithMalloc( sizeof(MeasureProgLocal_c), sui16_MeasureProgLocalTotal )
        << "/" << sizeSlistTWithMalloc( sizeof(MeasureProgLocal_c), 1 )
        << ", Chunk-Alloc: "
        << sizeSlistTWithChunk( sizeof(MeasureProgLocal_c), sui16_MeasureProgLocalTotal )
        << ", Deconstruct-Cnt: " << sui16_deconstructMeasureProgLocalTotal
      #ifndef MASSERT
        << INTERNAL_DEBUG_DEVICE_NEWLINE << INTERNAL_DEBUG_DEVICE_ENDL;
      #else
        << ", __mall tot:" << AllocateHeapMalloc
        << ", _mall deal tot: " << DeallocateHeapMalloc
        << INTERNAL_DEBUG_DEVICE_ENDL;
      #endif
    }
  }
  #endif
  // the mpc_progCache is a pointer, which must be assignet relative to the start of mvecc_prog
  mpc_progCache = vec_prog().begin();

  // now init all measure progs with the correct pointer to this ProcessData item
  for (Vec_MeasureProgLocal::iterator pc_iter = vec_prog().begin();
      pc_iter != vec_prog().end(); pc_iter++)
  {
    pc_iter->set( arc_src.processData() );
    // if the actual initialised item is the same as the cached item from
    // source set the cache of the copy (this instance)
    if (*pc_iter == *arc_src.mpc_progCache) mpc_progCache = pc_iter;
  }
}

ManageMeasureProgLocal_c::~ManageMeasureProgLocal_c()
{
  #ifdef DEBUG_HEAP_USEAGE
  sui16_MeasureProgLocalTotal -= vec_prog().size();
  sui16_deconstructMeasureProgLocalTotal += mvecc_prog.size();

  if ( ( ( vec_prog().size() > 0 ) && ( sui16_lastPrintedMeasureProgLocalTotal != sui16_MeasureProgLocalTotal ) )
    || ( sui16_lastPrintedMeasureProgLocalTotal != sui16_MeasureProgLocalTotal                                  ) )
  {
    sui16_lastPrintedMeasureProgLocalTotal = sui16_MeasureProgLocalTotal;
    sui16_printedDeconstructMeasureProgLocalTotal = sui16_deconstructMeasureProgLocalTotal;
    INTERNAL_DEBUG_DEVICE
      << sui16_MeasureProgLocalTotal << " x MeasureProgLocal_c: Mal-Alloc: "
      <<  sizeSlistTWithMalloc( sizeof(MeasureProgLocal_c), sui16_MeasureProgLocalTotal )
      << "/" << sizeSlistTWithMalloc( sizeof(MeasureProgLocal_c), 1 )
      << ", Chunk-Alloc: "
      << sizeSlistTWithChunk( sizeof(MeasureProgLocal_c), sui16_MeasureProgLocalTotal )
      << ", Deconstruct-Cnt: " << sui16_deconstructMeasureProgLocalTotal
    #ifndef MASSERT
      << INTERNAL_DEBUG_DEVICE_NEWLINE << INTERNAL_DEBUG_DEVICE_ENDL;
    #else
      << ", __mall tot:" << AllocateHeapMalloc
      << ", _mall deal tot: " << DeallocateHeapMalloc
      << INTERNAL_DEBUG_DEVICE_ENDL;
    #endif
  }
  #endif
}

/**
  perform periodic actions
  delete all running measure programs of members which are >3sec inactive
  deletion of outdated setpoints is managed by SetpointLocal_c::timeEvent
  @param pui16_nextTimePeriod calculated new time period, based on current measure progs (only for local proc data)
  @return true -> all planned executions performed
*/
bool ManageMeasureProgLocal_c::timeEvent( uint16_t *pui16_nextTimePeriod ){
  IsoMonitor_c& c_isoMonitor = getIsoMonitorInstance4Comm();

  if ( Scheduler_Task_c::getAvailableExecTime() == 0 ) return false;
  const IsoName_c *pc_callerISOName;

  #ifdef DEBUG_HEAP_USEAGE
  if ( ( sui16_lastPrintedMeasureProgLocalTotal != sui16_MeasureProgLocalTotal  )
    || ( sui16_printedDeconstructMeasureProgLocalTotal != sui16_deconstructMeasureProgLocalTotal  ) )
  {
    sui16_lastPrintedMeasureProgLocalTotal = sui16_MeasureProgLocalTotal;
    sui16_printedDeconstructMeasureProgLocalTotal = sui16_deconstructMeasureProgLocalTotal;
    INTERNAL_DEBUG_DEVICE
      << sui16_MeasureProgLocalTotal << " x MeasureProgLocal_c: Mal-Alloc: "
      <<  sizeSlistTWithMalloc( sizeof(MeasureProgLocal_c), sui16_MeasureProgLocalTotal )
      << "/" << sizeSlistTWithMalloc( sizeof(MeasureProgLocal_c), 1 )
      << ", Chunk-Alloc: "
      << sizeSlistTWithChunk( sizeof(MeasureProgLocal_c), sui16_MeasureProgLocalTotal )
      << ", Deconstruct-Cnt: " << sui16_deconstructMeasureProgLocalTotal
    #ifndef MASSERT
      << INTERNAL_DEBUG_DEVICE_NEWLINE << INTERNAL_DEBUG_DEVICE_ENDL;
    #else
      << ", __mall tot:" << AllocateHeapMalloc
      << ", _mall deal tot: " << DeallocateHeapMalloc
      << INTERNAL_DEBUG_DEVICE_ENDL;
    #endif
  }
  #endif

  // delete all measure programs of >3sec inactive members
  const uint16_t cui16_size = vec_prog().size();
  if ( cui16_size == 0 )
  { // insert first default element, if list is empty
    checkInitList();
  }
  else if ( cui16_size == 1)
  { // only one measure prog -> set it to undefined prog type if isoName inactive
    pc_callerISOName = &(vec_prog().begin()->isoName());
    if ( ( !vec_prog().begin()->checkProgType(Proc_c::UndefinedProg))
        && ( !c_isoMonitor.existIsoMemberISOName(*pc_callerISOName, true) )
       )
    { // progType of first and only element is not default undefined
      // --> isoName should be an active member, but is inactie > 3sec
      // stop all programs and set prog type to default
      vec_prog().begin()->stop(); // programs stopped
      vec_prog().begin()->setProgType(Proc_c::UndefinedProg); // set to default
    }
  }
  else
  { // more than one item
    bool b_repeat = true;
    while (b_repeat && (!vec_prog().empty())) {
      b_repeat=false;
      for (Vec_MeasureProgLocal::iterator pc_iter = vec_prog().begin();
          pc_iter != vec_prog().end(); pc_iter++)
      { // check if this item has inactive isoName
        if ( !c_isoMonitor.existIsoMemberISOName(pc_iter->isoName(), true) )
        { // item isn't active any more -> stop entries and erase it
          pc_iter->stop();
          // erase only if array size > 1
          if (vec_prog().size() > 1)
          {
            vec_prog().erase(pc_iter);
            #ifdef DEBUG_HEAP_USEAGE
            sui16_MeasureProgLocalTotal--;
            sui16_deconstructMeasureProgLocalTotal++;

            if ( ( sui16_lastPrintedMeasureProgLocalTotal != sui16_MeasureProgLocalTotal  )
              || ( sui16_lastPrintedMeasureProgLocalTotal != sui16_MeasureProgLocalTotal  ) )
            {
              sui16_lastPrintedMeasureProgLocalTotal = sui16_MeasureProgLocalTotal;
              sui16_printedDeconstructMeasureProgLocalTotal = sui16_deconstructMeasureProgLocalTotal;
              INTERNAL_DEBUG_DEVICE
                << sui16_MeasureProgLocalTotal << " x MeasureProgLocal_c: Mal-Alloc: "
                <<  sizeSlistTWithMalloc( sizeof(MeasureProgLocal_c), sui16_MeasureProgLocalTotal )
                << "/" << sizeSlistTWithMalloc( sizeof(MeasureProgLocal_c), 1 )
                << ", Chunk-Alloc: "
                << sizeSlistTWithChunk( sizeof(MeasureProgLocal_c), sui16_MeasureProgLocalTotal )
                << ", Deconstruct-Cnt: " << sui16_deconstructMeasureProgLocalTotal
              #ifndef MASSERT
                << INTERNAL_DEBUG_DEVICE_NEWLINE << INTERNAL_DEBUG_DEVICE_ENDL;
              #else
                << ", __mall tot:" << AllocateHeapMalloc
                << ", _mall deal tot: " << DeallocateHeapMalloc
                << INTERNAL_DEBUG_DEVICE_ENDL;
              #endif
            }
            #endif
            b_repeat = true;
            break; // old: cause of reordering of list delete only one item per timeEvent
          }
          else pc_iter->setProgType(Proc_c::UndefinedProg);
        }
      }
    }
  }

  // call the time event for all measure programs
  for (Vec_MeasureProgLocal::iterator pc_iter = vec_prog().begin();
       pc_iter != vec_prog().end(); pc_iter++)
    // if timeEvent of item returns FALSE, return this function also with false
    if ( ! pc_iter->timeEvent(pui16_nextTimePeriod) ) return false;

  return true;
}
/** process a measure prog message for local process data */
void ManageMeasureProgLocal_c::processProg(){
  ProcessPkg_c& c_pkg = getProcessInstance4Comm().data();
  const IsoName_c& c_callerISOName =  c_pkg.memberSend().isoName();
  GeneralCommand_c::CommandType_t en_command = c_pkg.mc_generalCommand.getCommand();

  // call updateProgCache with createIfNeeded if this is a writing action, otherwise don't create if none found
  // if ( ((c_pkg.pd() & 0x1) == 0)  => pd == 0, 2
  //  || ((c_pkg.pd() == 1) && (c_pkg.mod() == 0))
  if ( (en_command & 0x10) || /* measurement command indices are >= 0x10 < 0x20! */
       ( en_command == GeneralCommand_c::setValue)
     )
  { // it's a measuring program message -> create new item if none found
    updateProgCache(c_callerISOName, true);
  }
  else
    // if ( (c_pkg.pd() != 3) || (c_pkg.mod() != 0) )  => pd == 1 || (pd == 3 && mod != 0)
    if ( !c_pkg.mc_generalCommand.checkIsRequest() ||
         c_pkg.mc_generalCommand.getValueGroup() != GeneralCommand_c::exactValue )
    { // use normal mechanism -> exist function if no entry found
      if (!updateProgCache(c_callerISOName, false))return;
    }

  #ifdef DEBUG_HEAP_USEAGE
  // first real access - print size now if this current size not yet printed
  // ( this is needed, as ManageMeasureProgLocal_c::init causes too much
  //   prints in too small time )
  if ( sui16_lastPrintedMeasureProgLocalTotal != sui16_MeasureProgLocalTotal )
  {
    sui16_lastPrintedMeasureProgLocalTotal = sui16_MeasureProgLocalTotal;
    INTERNAL_DEBUG_DEVICE
      << sui16_MeasureProgLocalTotal << " x MeasureProgLocal_c: Mal-Alloc: "
      <<  sizeSlistTWithMalloc( sizeof(MeasureProgLocal_c), sui16_MeasureProgLocalTotal )
      << "/" << sizeSlistTWithMalloc( sizeof(MeasureProgLocal_c), 1 )
      << ", Chunk-Alloc: "
      << sizeSlistTWithChunk( sizeof(MeasureProgLocal_c), sui16_MeasureProgLocalTotal )
      << ", Deconstruct-Cnt: " << sui16_deconstructMeasureProgLocalTotal
    #ifndef MASSERT
      << INTERNAL_DEBUG_DEVICE_NEWLINE << INTERNAL_DEBUG_DEVICE_ENDL;
    #else
      << ", __mall tot:" << AllocateHeapMalloc
      << ", _mall deal tot: " << DeallocateHeapMalloc
      << INTERNAL_DEBUG_DEVICE_ENDL;
    #endif
  }
  #endif
  // now call process msg for cached item
  mpc_progCache->processMsg();
}

/**
  search for suiting measureprog, if not found AND if ab_doCreate == true
  create copy from first element at end of vector

  possible errors:
      * Err_c::elNonexistent wanted measureprog doesn't exist and ab_doCreate == false

  @param ac_isoName DEVCLASS code of searched measure program
  @param ab_doCreated true -> create suitable measure program if not found
*/
MeasureProgLocal_c& ManageMeasureProgLocal_c::prog(const IsoName_c& ac_isoName, bool ab_doCreate){
  // update the prog cache
  if (!updateProgCache(ac_isoName, ab_doCreate) && (!ab_doCreate))
  { // not found and no creation wanted
    getILibErrInstance().registerError( iLibErr_c::ElNonexistent, iLibErr_c::Process );
  }

  // now return the cache pointed prog
  return *mpc_progCache;
}

/** initialise value for all registered Measure Progs */
void ManageMeasureProgLocal_c::initGlobalVal( int32_t ai32_val )
{
  for (Vec_MeasureProgLocal::iterator pc_iter = vec_prog().begin();
      pc_iter != vec_prog().end(); pc_iter++)pc_iter->initVal(ai32_val);
}
/** set value for all registered Measure Progs */
void ManageMeasureProgLocal_c::setGlobalVal( int32_t ai32_val )
{
  checkInitList();
  for (Vec_MeasureProgLocal::iterator pc_iter = vec_prog().begin();
      pc_iter != vec_prog().end(); pc_iter++)pc_iter->setVal(ai32_val);
}
#ifdef USE_FLOAT_DATA_TYPE
/** initialise value for all registered Measure Progs */
void ManageMeasureProgLocal_c::initGlobalVal( float af_val )
{
  for (Vec_MeasureProgLocal::iterator pc_iter = vec_prog().begin();
      pc_iter != vec_prog().end(); pc_iter++)pc_iter->initVal(af_val);
}
/** set value for all registered Measure Progs */
void ManageMeasureProgLocal_c::setGlobalVal( float af_val )
{
  checkInitList();
  for (Vec_MeasureProgLocal::iterator pc_iter = vec_prog().begin();
      pc_iter != vec_prog().end(); pc_iter++)pc_iter->setVal(af_val);
}
#endif // USE_FLOAT_DATA_TYPE


/**
  create a new measure prog item;
  if there is still the default initial item undefined define it
  and create no new item

  possible errors:
      * Err_c::badAlloc not enough memory to insert new MeasureProgLocal

  @param ac_isoName commanding ISOName
*/
void ManageMeasureProgLocal_c::insertMeasureprog(const IsoName_c& ac_isoName){
// only create new item if first isn't undefined
  const uint8_t b_oldSize = vec_prog().size();

  if ((b_oldSize == 0)||(vec_prog().begin()->progType() != Proc_c::UndefinedProg))
  { // creation is forced
    Vec_MeasureProgLocalIterator pc_iter = vec_prog().begin();
    Vec_MeasureProgLocalIterator pc_iterInsertLokation = vec_prog().begin();
    // search for base item to make copy of it
    for (; pc_iter != vec_prog().end(); pc_iter++)
    { // check if its base
      if (pc_iter->progType() == 0x1)break;
    }
    if (pc_iter != vec_prog().end())
    { // insert copy from base prog item at end
#ifndef DO_USE_SLIST
      pc_iterInsertLokation++;
      vec_prog().insert(pc_iterInsertLokation, *pc_iter);
#else
      vec_prog().insert_after(vec_prog().begin(), *pc_iter);
#endif
    }
    else if (b_oldSize > 0)
    { // create copy from first list item at end
#ifndef DO_USE_SLIST
      pc_iterInsertLokation++;
      vec_prog().insert(pc_iterInsertLokation, *(vec_prog().begin()));
#else
      vec_prog().insert_after(vec_prog().begin(), *(vec_prog().begin()));
#endif
    }
    else
    { // empty list --> insert new item at front of list
      vec_prog().push_front( MeasureProgLocal_c(pprocessData(), Proc_c::UndefinedProg ) );
    }
    if (b_oldSize >= vec_prog().size())
    { // array didn't grow
      getILibErrInstance().registerError( iLibErr_c::BadAlloc, iLibErr_c::Process );
      return; // exit function
    }
    #ifdef DEBUG_HEAP_USEAGE
    sui16_MeasureProgLocalTotal++;

    if ( ( sui16_lastPrintedMeasureProgLocalTotal != sui16_MeasureProgLocalTotal  )
      || ( sui16_printedDeconstructMeasureProgLocalTotal != sui16_deconstructMeasureProgLocalTotal  ) )
    {
      sui16_lastPrintedMeasureProgLocalTotal = sui16_MeasureProgLocalTotal;
      sui16_printedDeconstructMeasureProgLocalTotal = sui16_deconstructMeasureProgLocalTotal;
      INTERNAL_DEBUG_DEVICE
        << sui16_MeasureProgLocalTotal << " x MeasureProgLocal_c: Mal-Alloc: "
        <<  sizeSlistTWithMalloc( sizeof(MeasureProgLocal_c), sui16_MeasureProgLocalTotal )
        << "/" << sizeSlistTWithMalloc( sizeof(MeasureProgLocal_c), 1 )
        << ", Chunk-Alloc: "
        << sizeSlistTWithChunk( sizeof(MeasureProgLocal_c), sui16_MeasureProgLocalTotal )
        << ", Deconstruct-Cnt: " << sui16_deconstructMeasureProgLocalTotal
      #ifndef MASSERT
        << INTERNAL_DEBUG_DEVICE_NEWLINE << INTERNAL_DEBUG_DEVICE_ENDL;
      #else
        << ", __mall tot:" << AllocateHeapMalloc
        << ", _mall deal tot: " << DeallocateHeapMalloc
        << INTERNAL_DEBUG_DEVICE_ENDL;
      #endif
    }
    #endif

    // set cache to new item which is inserted
    mpc_progCache = vec_prog().begin();
    if (b_oldSize > 0) mpc_progCache++;
    #ifdef USE_EEPROM_IO
    // set initial value of new item to eeprom value
    ProcDataLocalBase_c* pc_procdata =
      static_cast<ProcDataLocalBase_c*>(pprocessData());
      #ifdef USE_FLOAT_DATA_TYPE
    if (valType() != i32_val)
      mpc_progCache->initVal(pc_procdata->eepromValFloat());
    else
      #endif
      mpc_progCache->initVal((int32_t)pc_procdata->eepromVal());
    #endif
  } // if first item is already used
  else
  { // set cache to first undefined item
    mpc_progCache = vec_prog().begin();
  }
  // set type and isoName for item
  mpc_progCache->setISOName(ac_isoName);

  /** @TODO NOW: remove progType! */
 mpc_progCache->setProgType(2);
}

/**
  update the programm cache, create an programm item, if wanted

  possible errors:
      * Err_c::badAlloc not enough memory to insert new MeasureProgLocal

  @param ac_isoName commanding ISOName
  @param ab_createIfNotFound true -> create new item if not found
  @return true -> instance found
*/
bool ManageMeasureProgLocal_c::updateProgCache(const IsoName_c& ac_isoName, bool ab_createIfNotFound)
{
  bool b_result = false;
  // insert first default element, if list is empty
  checkInitList();
  // list of measure prog is created/usable -> search
  Vec_MeasureProgLocalIterator pc_iter = vec_prog().begin();
  // update only if old cache isn't valid
  if ( (!vec_prog().empty())
    && (mpc_progCache->isoName() == ac_isoName) )
  { // old is valid -> return true
    b_result =  true;
  }
  else
  { // actual cache is from another isoName and/or type -> search for new
    // target process msg
    for (pc_iter = vec_prog().begin(); pc_iter != vec_prog().end(); pc_iter++)
    { // check if isoName and type fit
      if ( pc_iter->isoName() == ac_isoName )
      {
        b_result = true;
        mpc_progCache = pc_iter;
        break;
      } // fit
    }// for

    // check if still no suitable item is found
    if (!b_result)
    {
      if (!ab_createIfNotFound)
      { // if still no suitable item was found and no item should be created point to first list item
        mpc_progCache = vec_prog().begin();
      }
      else
      { // no suitable item was found -> create suitable one
        insertMeasureprog(ac_isoName);
      } // do create if not found
    }// no suitable found
  }
  return b_result;
}

/**
  allow local client to actively start a measurement program
  (to react on a incoming "start" command for default data logging)
  @param ren_type measurement type: Proc_c::TimeProp, Proc_c::DistProp, ...
  @param ren_progType program type: Proc_c::Base, Proc_c::Target
  @param ai32_increment
  @param apc_receiverDevice commanding ISOName
  @return true -> apc_receiverDevice is set
*/
bool ManageMeasureProgLocal_c::startDataLogging(Proc_c::type_t ren_type /* Proc_c::TimeProp, Proc_c::DistProp, ... */,
                                                int32_t ai32_increment, const IsoName_c* apc_receiverDevice )
{
  if ( !apc_receiverDevice )
    return FALSE;

  // create new item if none found
  updateProgCache(/*ren_progType,*/ *apc_receiverDevice, true);

  mpc_progCache->setISOName(*apc_receiverDevice);

  if (Proc_c::TimeProp == ren_type)
    mpc_progCache->addSubprog(ren_type, CNAMESPACE::labs(ai32_increment));
  else
    mpc_progCache->addSubprog(ren_type, ai32_increment);

  mpc_progCache->start(ren_type, Proc_c::DoVal);

  return TRUE;
}

/**
  stop all measurement progs in all local process instances, started with given isoName
  @param rc_isoName
*/
void ManageMeasureProgLocal_c::stopRunningMeasurement(const IsoName_c& rc_isoName)
{
  Vec_MeasureProgLocalIterator pc_iter = vec_prog().begin();
  for (pc_iter = vec_prog().begin(); pc_iter != vec_prog().end(); pc_iter++)
  { // check if isoName and type fit
    if ((pc_iter->isoName() == rc_isoName) && pc_iter->started())
    {
      pc_iter->stop();
    }
  }
}

} // end of namespace __IsoAgLib
