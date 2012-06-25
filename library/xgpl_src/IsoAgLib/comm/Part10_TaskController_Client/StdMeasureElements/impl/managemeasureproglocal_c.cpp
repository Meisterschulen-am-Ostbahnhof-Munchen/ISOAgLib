/***************************************************************************
                          managemeasureproglocal_c.cpp

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#include "managemeasureproglocal_c.h"
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/process_c.h>
#include <IsoAgLib/util/impl/util_funcs.h>


#if DEBUG_HEAP_USEAGE
  #ifdef SYSTEM_PC
    #include <iostream>
  #else
    #include <supplementary_driver/driver/rs232/impl/rs232io_c.h>
  #endif
  #include <IsoAgLib/util/impl/util_funcs.h>
#endif

#if DEBUG_HEAP_USEAGE
static uint16_t sui16_MeasureProgLocalTotal = 0;
static uint16_t sui16_lastPrintedMeasureProgLocalTotal = 0;
static uint16_t sui16_deconstructMeasureProgLocalTotal = 0;
static uint16_t sui16_printedDeconstructMeasureProgLocalTotal = 0;
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
    vec_prog().push_front(MeasureProgLocal_c(pc_procdata,
                                              pc_procdata->masterMeasurementVal()));
  }
  else
  { // insert default entry wihtout connection to root proc-data class into list
    vec_prog().push_front(MeasureProgLocal_c(0) );
  }

  isoaglib_assert( ! vec_prog().empty() );

  #if DEBUG_HEAP_USEAGE
  sui16_MeasureProgLocalTotal++;
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
  #if DEBUG_HEAP_USEAGE
  sui16_MeasureProgLocalTotal -= mvecc_prog.size();
  sui16_deconstructMeasureProgLocalTotal += mvecc_prog.size();
  #endif
  mvecc_prog.clear();
  mpc_progCache = vec_prog().begin();
}
/** copy constructor */
ManageMeasureProgLocal_c::ManageMeasureProgLocal_c( const ManageMeasureProgLocal_c& acrc_src )
: ProcessElementBase_c( acrc_src )
{
  assignFromSource( acrc_src );
}
/** assignment operator */
const ManageMeasureProgLocal_c& ManageMeasureProgLocal_c::operator=( const ManageMeasureProgLocal_c& acrc_src )
{
  ProcessElementBase_c::operator=( acrc_src );
  assignFromSource( acrc_src );
  return *this;
}
/** base function for assignment of element vars for copy constructor and operator= */
void ManageMeasureProgLocal_c::assignFromSource( const ManageMeasureProgLocal_c& acrc_src )
{ // copy dynamic array
  mvecc_prog = acrc_src.mvecc_prog;
  // now initialise the elements
  // the mpc_progCache is a pointer, which must be assignet relative to the start of mvecc_prog
  mpc_progCache = vec_prog().begin();

  // now init all measure progs with the correct pointer to this ProcessData item
  for (Vec_MeasureProgLocal::iterator pc_iter = vec_prog().begin();
      pc_iter != vec_prog().end(); pc_iter++)
  {
    pc_iter->set( acrc_src.processData() );
    // if the actual initialised item is the same as the cached item from
    // source set the cache of the copy (this instance)
    if (*pc_iter == *acrc_src.mpc_progCache) mpc_progCache = pc_iter;
  }
}

ManageMeasureProgLocal_c::~ManageMeasureProgLocal_c()
{
  #if DEBUG_HEAP_USEAGE
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
      << INTERNAL_DEBUG_DEVICE_NEWLINE << INTERNAL_DEBUG_DEVICE_ENDL;
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

  #if DEBUG_HEAP_USEAGE
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
      << INTERNAL_DEBUG_DEVICE_NEWLINE << INTERNAL_DEBUG_DEVICE_ENDL;
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
    if ( ( vec_prog().begin()->getActive())
        && ( !c_isoMonitor.existIsoMemberISOName(*pc_callerISOName, true) )
       )
    { // first and only element is active
      // --> isoName should be an active member, but is inactive > 3sec
      // stop all programs and set prog type to inactive
      vec_prog().begin()->stop(); // programs stopped
      vec_prog().begin()->setActive(false);
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
          pc_iter->setActive(false);

          // erase only if array size > 1
          if (vec_prog().size() > 1)
          {
            vec_prog().erase(pc_iter);

            // reset mpc_progCache
            mpc_progCache = vec_prog().begin();

            #if DEBUG_HEAP_USEAGE
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
                << INTERNAL_DEBUG_DEVICE_NEWLINE << INTERNAL_DEBUG_DEVICE_ENDL;
            }
            #endif
            b_repeat = true;
            break; // old: cause of reordering of list delete only one item per timeEvent
          }
        }
      }
    }
  }

  // call the time event for all measure programs
  for (Vec_MeasureProgLocal::iterator pc_iter = vec_prog().begin();
       pc_iter != vec_prog().end(); pc_iter++)
    // if timeEvent of item returns false, return this function also with false
    if ( ! pc_iter->timeEvent(pui16_nextTimePeriod) ) return false;

  return true;
}

/** process a measure prog message for local process data */
void ManageMeasureProgLocal_c::processProg( const ProcessPkg_c& arc_data )
{
  if (arc_data.senderItem() == NULL)
  { // sender wit SA 0xFE is not of interest
    return;
  }

  const IsoName_c& c_callerISOName =  arc_data.senderItem()->isoName();
  ProcessCmd_c::CommandType_t en_command = arc_data.mc_processCmd.getCommand();

  // call updateProgCache with createIfNeeded if this is a writing action, otherwise don't create if none found
  if ( (en_command & 0x10) || /* measurement command indices are >= 0x10 < 0x20! */
       ( en_command == ProcessCmd_c::setValue)
     )
  { // it's a measuring program message -> create new item if none found
    updateProgCache(c_callerISOName, true);
  }
  else
    if ( !arc_data.mc_processCmd.checkIsRequest() ||
         arc_data.mc_processCmd.getValueGroup() != ProcessCmd_c::exactValue )
    { // use normal mechanism -> exist function if no entry found
      if (!updateProgCache(c_callerISOName, false))return;
    }

  #if DEBUG_HEAP_USEAGE
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
      << INTERNAL_DEBUG_DEVICE_NEWLINE << INTERNAL_DEBUG_DEVICE_ENDL;
  }
  #endif
  // now call process msg for cached item
  mpc_progCache->processMsg( arc_data );
}

/**
  search for suiting measureprog, if not found AND if ab_doCreate == true
  create copy from first element at end of vector

  possible errors:
      * Err_c::elNonexistent wanted measureprog doesn't exist and ab_doCreate == false

  @param acrc_isoName DEVCLASS code of searched measure program
  @param ab_doCreate true -> create suitable measure program if not found
*/
MeasureProgLocal_c& ManageMeasureProgLocal_c::prog(const IsoName_c& acrc_isoName, bool ab_doCreate){
  // update the prog cache
  if (!updateProgCache(acrc_isoName, ab_doCreate) && (!ab_doCreate))
  { // not found and no creation wanted
    IsoAgLib::getILibErrInstance().registerNonFatal( IsoAgLib::iLibErr_c::ProcData, getMultitonInst() );
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

/**
  create a new measure prog item;
  if there is still the inactive initial item use it
  and create no new item

  possible errors:
      * Err_c::badAlloc not enough memory to insert new MeasureProgLocal

  @param acrc_isoName commanding ISOName
*/
void ManageMeasureProgLocal_c::insertMeasureprog(const IsoName_c& acrc_isoName)
{
  Vec_MeasureProgLocalIterator pc_iter = vec_prog().begin();
  for (; pc_iter != vec_prog().end(); pc_iter++)
  { // check if it's not active
    if (!pc_iter->getActive())
      break;
  }

  if (pc_iter != vec_prog().end()) {
    // set cache to new item which is inserted / reused
    mpc_progCache = pc_iter;
  }
  else
  {
    vec_prog().push_front( MeasureProgLocal_c(pprocessData() ) );
    mpc_progCache = vec_prog().begin();
  }

  #if DEBUG_HEAP_USEAGE
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
      << INTERNAL_DEBUG_DEVICE_NEWLINE << INTERNAL_DEBUG_DEVICE_ENDL;
  }
  #endif

  // set type and isoName for item
  mpc_progCache->setISOName(acrc_isoName);

  mpc_progCache->setActive(true);
}

/**
  update the programm cache, create an programm item, if wanted

  possible errors:
      * Err_c::badAlloc not enough memory to insert new MeasureProgLocal

  @param acrc_isoName commanding ISOName
  @param ab_createIfNotFound true -> create new item if not found
  @return true -> instance found
*/
bool ManageMeasureProgLocal_c::updateProgCache(const IsoName_c& acrc_isoName, bool ab_createIfNotFound)
{
  bool b_result = false;
  // insert first default element, if list is empty
  checkInitList();
  // list of measure prog is created/usable -> search
  Vec_MeasureProgLocalIterator pc_iter = vec_prog().begin();
  // update only if old cache isn't valid
  if ( (!vec_prog().empty())
    && (mpc_progCache->isoName() == acrc_isoName) )
  { // old is valid -> return true
    b_result =  true;
  }
  else
  { // actual cache is from another isoName and/or type -> search for new
    // target process msg
    for (pc_iter = vec_prog().begin(); pc_iter != vec_prog().end(); pc_iter++)
    { // check if isoName and type fit
      if ( pc_iter->isoName() == acrc_isoName )
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
        insertMeasureprog(acrc_isoName);
      } // do create if not found
    }// no suitable found
  }
  return b_result;
}

/**
  allow local client to actively start a measurement program
  (to react on a incoming "start" command for default data logging)
  @param ren_type measurement type: Proc_c::TimeProp, Proc_c::DistProp, ...
  @param ai32_increment
  @param apc_receiverDevice commanding ISOName
  @return true -> apc_receiverDevice is set
*/
bool ManageMeasureProgLocal_c::startDataLogging(Proc_c::type_t ren_type /* Proc_c::TimeProp, Proc_c::DistProp, ... */,
                                                int32_t ai32_increment, const IsoName_c* apc_receiverDevice )
{
  if ( !apc_receiverDevice )
    return false;

  // create new item if none found
  updateProgCache(*apc_receiverDevice, true);

  mpc_progCache->setISOName(*apc_receiverDevice);

  if (Proc_c::TimeProp == ren_type)
    mpc_progCache->addSubprog(ren_type, __IsoAgLib::abs(ai32_increment));
  else
    mpc_progCache->addSubprog(ren_type, ai32_increment);

  mpc_progCache->start(ren_type, Proc_c::DoVal);

  return true;
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
