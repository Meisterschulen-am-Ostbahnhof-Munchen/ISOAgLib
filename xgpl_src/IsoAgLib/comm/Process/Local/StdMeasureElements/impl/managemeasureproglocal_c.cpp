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
  if ( c_vec_prog.size() > 0 ) return;
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
    getLbsErrInstance().registerError( LibErr_c::BadAlloc, LibErr_c::LbsProcess );
  }
  #ifdef DEBUG_HEAP_USEAGE
  else
  {
    sui16_MeasureProgLocalTotal++;
  }
  #endif
  // point cache initially to default first element
  pc_progCache = c_vec_prog.begin();
}

/**
  initialise this ManageMeasureProgLocal_c instance to a well defined initial state
  @param rpc_processData optional pointer to containing ProcessData instance
*/
void ManageMeasureProgLocal_c::init( ProcDataBase_c *const rpc_processData )
{ // set the pointer to the corresponging process data class
  ProcessElementBase_c::set( rpc_processData );
  // as init() can be also called to re-init (bring this instance to well
  // defined starting condition, clearthe measure-prog array
  #ifdef DEBUG_HEAP_USEAGE
  sui16_MeasureProgLocalTotal -= c_vec_prog.size();
  sui16_deconstructMeasureProgLocalTotal += c_vec_prog.size();
  #endif
  c_vec_prog.clear();
  pc_progCache = NULL;
}
/** copy constructor */
ManageMeasureProgLocal_c::ManageMeasureProgLocal_c( const ManageMeasureProgLocal_c& rrefc_src )
: ProcessElementBase_c( rrefc_src )
{
  assignFromSource( rrefc_src );
}
/** assignment operator */
const ManageMeasureProgLocal_c& ManageMeasureProgLocal_c::operator=( const ManageMeasureProgLocal_c& rrefc_src )
{
  ProcessElementBase_c::operator=( rrefc_src );
  assignFromSource( rrefc_src );
  return *this;
}
/** base function for assignment of element vars for copy constructor and operator= */
void ManageMeasureProgLocal_c::assignFromSource( const ManageMeasureProgLocal_c& rrefc_src )
{ // copy dynamic array
  c_vec_prog = rrefc_src.c_vec_prog;
  // now initialise the elements
  if (vec_prog().size() < rrefc_src.constVecProg().size())
  { // not all items copied
    getLbsErrInstance().registerError( LibErr_c::BadAlloc, LibErr_c::LbsProcess );
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
      getRs232Instance()
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
  // the pc_progCache is a pointer, which must be assignet relative to the start of c_vec_prog
  pc_progCache = vec_prog().begin();

  // now init all measure progs with the correct pointer to this ProcessData item
  for (Vec_MeasureProgLocal::iterator pc_iter = vec_prog().begin();
      pc_iter != vec_prog().end(); pc_iter++)
  {
    pc_iter->set( rrefc_src.processData() );
    // if the actual initialised item is the same as the cached item from
    // source set the cache of the copy (this instance)
    if (*pc_iter == *rrefc_src.pc_progCache) pc_progCache = pc_iter;
  }
}

ManageMeasureProgLocal_c::~ManageMeasureProgLocal_c()
{
  #ifdef DEBUG_HEAP_USEAGE
  sui16_MeasureProgLocalTotal -= vec_prog().size();
  sui16_deconstructMeasureProgLocalTotal += c_vec_prog.size();

  if ( ( ( vec_prog().size() > 0 ) && ( sui16_lastPrintedMeasureProgLocalTotal != sui16_MeasureProgLocalTotal ) )
    || ( sui16_lastPrintedMeasureProgLocalTotal != sui16_MeasureProgLocalTotal                                  ) )
  {
    sui16_lastPrintedMeasureProgLocalTotal = sui16_MeasureProgLocalTotal;
    sui16_printedDeconstructMeasureProgLocalTotal = sui16_deconstructMeasureProgLocalTotal;
    getRs232Instance()
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
  @return true -> all planned executions performed
*/
bool ManageMeasureProgLocal_c::timeEvent( void ){
  SystemMgmt_c& c_lbsSystem = getSystemMgmtInstance4Comm();

  if ( Scheduler_c::getAvailableExecTime() == 0 ) return false;
  const DevKey_c *pc_callerDevKey;

  #ifdef DEBUG_HEAP_USEAGE
  if ( ( sui16_lastPrintedMeasureProgLocalTotal != sui16_MeasureProgLocalTotal  )
    || ( sui16_printedDeconstructMeasureProgLocalTotal != sui16_deconstructMeasureProgLocalTotal  ) )
  {
    sui16_lastPrintedMeasureProgLocalTotal = sui16_MeasureProgLocalTotal;
    sui16_printedDeconstructMeasureProgLocalTotal = sui16_deconstructMeasureProgLocalTotal;
    getRs232Instance()
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
  { // only one measure prog -> set it to undefined prog type if devKey inactive
    pc_callerDevKey = &(vec_prog().begin()->devKey());
    if ((!vec_prog().begin()->checkProgType(Proc_c::UndefinedProg))
      &&((!c_lbsSystem.existMemberDevKey(*pc_callerDevKey, true))
      ||(
          (c_lbsSystem.memberDevKey(*pc_callerDevKey, true).lastedTime() > 3000)
        &&(c_lbsSystem.memberDevKey(*pc_callerDevKey, true).itemState(IState_c::Din))
        )
        )
      )
    { // progType of first and only element is not default undefined
      // --> devKey should be an active member, but is inactie > 3sec
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
      { // check if this item has inactive devKey
        if ((!c_lbsSystem.existMemberDevKey(pc_iter->devKey(), true))
          ||(
            (c_lbsSystem.memberDevKey(pc_iter->devKey(), true).lastedTime() > 3000)
          &&(c_lbsSystem.memberDevKey(pc_iter->devKey(), true).itemState(IState_c::Din))
            )
          )
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
              getRs232Instance()
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
    if ( ! pc_iter->timeEvent() ) return false;
  return true;
}
/** process a measure prog message for local process data */
void ManageMeasureProgLocal_c::processProg(){
  ProcessPkg_c& c_pkg = getProcessInstance4Comm().data();
  const DevKey_c& c_callerDevKey =  c_pkg.memberSend().devKey();
  GeneralCommand_c::CommandType_t en_command = c_pkg.c_generalCommand.getCommand();

  // call updateProgCache with createIfNeeded if this is a writing action, otherwise don't create if none found
  // if ( ((c_pkg.pd() & 0x1) == 0)  => pd == 0, 2
  //  || ((c_pkg.pd() == 1) && (c_pkg.mod() == 0))
  if ( (en_command & 0x10) || /* measurement command indices are >= 0x10 < 0x20! */
       ( en_command == GeneralCommand_c::setValue)
     )
  { // it's a measuring program message -> create new item if none found
    updateProgCache(c_pkg.pri(),c_callerDevKey, true);
  }
  else
    // if ( (c_pkg.pd() != 3) || (c_pkg.mod() != 0) )  => pd == 1 || (pd == 3 && mod != 0)
    if ( !c_pkg.c_generalCommand.checkIsRequest() ||
         c_pkg.c_generalCommand.getValueGroup() != GeneralCommand_c::exactValue )
    { // use normal mechanism -> exist function if no entry found
      if (!updateProgCache(c_pkg.pri(),c_callerDevKey, false))return;
    }

  #ifdef DEBUG_HEAP_USEAGE
  // first real access - print size now if this current size not yet printed
  // ( this is needed, as ManageMeasureProgLocal_c::init causes too much
  //   prints in too small time )
  if ( sui16_lastPrintedMeasureProgLocalTotal != sui16_MeasureProgLocalTotal )
  {
    sui16_lastPrintedMeasureProgLocalTotal = sui16_MeasureProgLocalTotal;
    getRs232Instance()
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
  pc_progCache->processMsg();
}

/**
  search for suiting measureprog, if not found AND if rb_doCreate == true
  create copy from first element at end of vector

  possible errors:
      * Err_c::elNonexistent wanted measureprog doesn't exist and rb_doCreate == false
  @param rui8_pri PRI code of searched measure program
  @param rc_devKey DEVCLASS code of searched measure program
  @param rb_doCreated true -> create suitable measure program if not found
*/
MeasureProgLocal_c& ManageMeasureProgLocal_c::prog(uint8_t rui8_pri, const DevKey_c& rc_devKey, bool rb_doCreate){
  // update the prog cache
  if (!updateProgCache(rui8_pri, rc_devKey, rb_doCreate) && (!rb_doCreate))
  { // not found and no creation wanted
    getLbsErrInstance().registerError( LibErr_c::ElNonexistent, LibErr_c::LbsProcess );
  }

  // now return the cache pointed prog
  return *pc_progCache;
}

/** initialise value for all registered Measure Progs */
void ManageMeasureProgLocal_c::initGlobalVal( int32_t ri32_val )
{
  for (Vec_MeasureProgLocal::iterator pc_iter = vec_prog().begin();
      pc_iter != vec_prog().end(); pc_iter++)pc_iter->initVal(ri32_val);
}
/** set value for all registered Measure Progs */
void ManageMeasureProgLocal_c::setGlobalVal( int32_t ri32_val )
{
  checkInitList();
  for (Vec_MeasureProgLocal::iterator pc_iter = vec_prog().begin();
      pc_iter != vec_prog().end(); pc_iter++)pc_iter->setVal(ri32_val);
}
#ifdef USE_FLOAT_DATA_TYPE
/** initialise value for all registered Measure Progs */
void ManageMeasureProgLocal_c::initGlobalVal( float rf_val )
{
  for (Vec_MeasureProgLocal::iterator pc_iter = vec_prog().begin();
      pc_iter != vec_prog().end(); pc_iter++)pc_iter->initVal(rf_val);
}
/** set value for all registered Measure Progs */
void ManageMeasureProgLocal_c::setGlobalVal( float rf_val )
{
  checkInitList();
  for (Vec_MeasureProgLocal::iterator pc_iter = vec_prog().begin();
      pc_iter != vec_prog().end(); pc_iter++)pc_iter->setVal(rf_val);
}
#endif // USE_FLOAT_DATA_TYPE


/**
  create a new measure prog item;
  if there is still the default initial item undefined define it
  and create no new item

  possible errors:
      * Err_c::badAlloc not enough memory to insert new MeasureProgLocal
  @param rui8_type program type: Proc_c::Base, Proc_c::Target
  @param rc_devKey commanding DEV_KEY
*/
void ManageMeasureProgLocal_c::insertMeasureprog(uint8_t rui8_type, const DevKey_c& rc_devKey){
// only create new item if first isn't undefined
  const uint8_t b_oldSize = vec_prog().size();

  if ((b_oldSize == 0)||(vec_prog().begin()->progType() != Proc_c::UndefinedProg))
  { // creation is forced
    Vec_MeasureProgLocalIterator pc_iter = vec_prog().begin();
    // search for base item to make copy of it
    for (; pc_iter != vec_prog().end(); pc_iter++)
    { // check if its base
      if (pc_iter->progType() == 0x1)break;
    }
    if (pc_iter != vec_prog().end())
    { // insert copy from base prog item at end
      vec_prog().insert_after(vec_prog().begin(), *pc_iter);
    }
    else if (b_oldSize > 0)
    { // create copy from first list item at end
      vec_prog().insert_after(vec_prog().begin(), *(vec_prog().begin()));
    }
    else
    { // empty list --> insert new item at front of list
      vec_prog().push_front( MeasureProgLocal_c(pprocessData(), Proc_c::UndefinedProg ) );
    }
    if (b_oldSize >= vec_prog().size())
    { // array didn't grow
      getLbsErrInstance().registerError( LibErr_c::BadAlloc, LibErr_c::LbsProcess );
      return; // exit function
    }
    #ifdef DEBUG_HEAP_USEAGE
    sui16_MeasureProgLocalTotal++;

    if ( ( sui16_lastPrintedMeasureProgLocalTotal != sui16_MeasureProgLocalTotal  )
      || ( sui16_printedDeconstructMeasureProgLocalTotal != sui16_deconstructMeasureProgLocalTotal  ) )
    {
      sui16_lastPrintedMeasureProgLocalTotal = sui16_MeasureProgLocalTotal;
      sui16_printedDeconstructMeasureProgLocalTotal = sui16_deconstructMeasureProgLocalTotal;
      getRs232Instance()
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
    pc_progCache = vec_prog().begin();
    if (b_oldSize > 0) pc_progCache++;
    #ifdef USE_EEPROM_IO
    // set initial value of new item to eeprom value
    ProcDataLocalBase_c* pc_procdata =
      static_cast<ProcDataLocalBase_c*>(pprocessData());
      #ifdef USE_FLOAT_DATA_TYPE
    if (valType() != i32_val)
      pc_progCache->initVal(pc_procdata->eepromValFloat());
    else
      #endif
      pc_progCache->initVal((int32_t)pc_procdata->eepromVal());
    #endif
  } // if first item is already used
  else
  { // set cache to first undefined item
    pc_progCache = vec_prog().begin();
  }
  // set type and devKey for item
  pc_progCache->setDevKey(rc_devKey);
  pc_progCache->setProgType(rui8_type);
}

/**
  update the programm cache, create an programm item, if wanted

  possible errors:
      * Err_c::badAlloc not enough memory to insert new MeasureProgLocal
  @param rui8_type program type: Proc_c::Base, Proc_c::Target
  @param rc_devKey commanding DEV_KEY
  @param rb_createIfNotFound true -> create new item if not found
  @return true -> instance found
*/
bool ManageMeasureProgLocal_c::updateProgCache(uint8_t rui8_type, const DevKey_c& rc_devKey, bool rb_createIfNotFound)
{
  bool b_result = false;
  // insert first default element, if list is empty
  checkInitList();
  // list of measure prog is created/usable -> search
  Vec_MeasureProgLocalIterator pc_iter = vec_prog().begin();
  // update only if old cache isn't valid
  if ( (!vec_prog().empty())
    && (pc_progCache->devKey() == rc_devKey)
    && (pc_progCache->progType() == rui8_type))
  { // old is valid -> return true
    b_result =  true;
  }
  else
  { // actual cache is from another devKey and/or type -> search for new
    // check if type is target
    if (rui8_type == 0x2)
    { // target process msg
      for (pc_iter = vec_prog().begin(); pc_iter != vec_prog().end(); pc_iter++)
      { // check if devKey and type fit
        if ((pc_iter->devKey() == rc_devKey) && (pc_iter->progType() == rui8_type))
        {
          b_result = true;
          pc_progCache = pc_iter;
          break;
        } // fit
      }// for
    } // target
    if ((rui8_type == 0x1)||((rui8_type == 0x2)&&(!b_result)&& (!rb_createIfNotFound)))
    { // base process msg or search for suitable target item without success and creation wasn't wanted
      for (pc_iter = vec_prog().begin(); pc_iter != vec_prog().end(); pc_iter++)
      { // check if type is base process
        if (pc_iter->progType() == 1)
        {
          b_result = true;
          pc_progCache = pc_iter;
          break;
        } // fit
      }// for
    } // base or not found target with no item to create
    // check if still no suitable item is found
    if (!b_result)
    {
      if (!rb_createIfNotFound)
      { // if still no suitable item was found and no item should be created point to first list item
        pc_progCache = vec_prog().begin();
      }
      else
      { // no suitable item was found -> create suitable one
        insertMeasureprog(rui8_type, rc_devKey);
      } // do create if not found
    }// no suitable found
  }
  return b_result;
}

#ifdef USE_ISO_11783
/**
  allow local client to actively start a measurement program
  (to react on a incoming "start" command for default data logging)
  @param ren_type measurement type: Proc_c::TimeProp, Proc_c::DistProp, ...
  @param ren_progType program type: Proc_c::Base, Proc_c::Target
  @param ri32_increment
  @param rpc_receiverDevice commanding DEV_KEY
  @return true -> rpc_receiverDevice is set
*/
bool ManageMeasureProgLocal_c::startDataLogging(Proc_c::type_t ren_type /* Proc_c::TimeProp, Proc_c::DistProp, ... */,
                                                Proc_c::progType_t ren_progType, 
                                                int32_t ri32_increment, const DevKey_c* rpc_receiverDevice )
{
  if ( !rpc_receiverDevice )
    return FALSE;
      
  // create new item if none found
  updateProgCache(ren_progType, *rpc_receiverDevice, true);

  pc_progCache->setDevKey(*rpc_receiverDevice);
    
  if (Proc_c::TimeProp == ren_type)
    pc_progCache->addSubprog(ren_type, CNAMESPACE::labs(ri32_increment));
  else
    pc_progCache->addSubprog(ren_type, ri32_increment);

  pc_progCache->start(ren_progType, ren_type, Proc_c::DoVal);

  return TRUE;
}

/**
  stop all measurement progs in all local process instances, started with given devKey
  @param refc_devKey
*/
void ManageMeasureProgLocal_c::stopRunningMeasurement(const DevKey_c& refc_devKey)
{
  Vec_MeasureProgLocalIterator pc_iter = vec_prog().begin();
  for (pc_iter = vec_prog().begin(); pc_iter != vec_prog().end(); pc_iter++)
  { // check if devKey and type fit
    if ((pc_iter->devKey() == refc_devKey) && pc_iter->started())
    {
      pc_iter->stop();
    }
  }
}

#endif

} // end of namespace __IsoAgLib
