/***************************************************************************
                          setpointlocal_c.cpp - object for managing setpoints

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
#include "setpointlocal_c.h"
#include "setpointregister_c.h"
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/processcmd_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/procdatalocal_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/process_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/processdatachangehandler_c.h>
#include <IsoAgLib/driver/system/impl/system_c.h>
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
static uint16_t sui16_setpointLocalTotal = 0;
#endif

namespace __IsoAgLib {

void SetpointLocal_c::init( ProcDataLocal_c *const apc_processData )
{
  set( apc_processData );
  // set mpc_registerCache to a well defined value
  mpc_registerCache = mvec_register.begin();
  #if DEBUG_HEAP_USEAGE
  sui16_setpointLocalTotal -= ( mvec_register.size() * ( sizeof(SetpointRegister_c) + 2 * sizeof(SetpointRegister_c*) ) );
  #endif
  mvec_register.clear();
  mpc_master = mvec_register.end();

  mb_allowedDeltaPercent = 0;
  mb_staticMaster = false;

  #if DEBUG_HEAP_USEAGE
  INTERNAL_DEBUG_DEVICE
    << "sizeof(SetpointRegister_c) = " << sizeof(SetpointRegister_c)
    << " Bytes" << INTERNAL_DEBUG_DEVICE_ENDL;
  #endif

}

const SetpointLocal_c& SetpointLocal_c::operator=( const SetpointLocal_c& acrc_src ){
  // first call base assignment operator
  ProcessElementBase_c::operator=(acrc_src);

  assignFromSource( acrc_src );

  // return reference
  return *this;
}

SetpointLocal_c::SetpointLocal_c( const SetpointLocal_c& acrc_src )
  : ProcessElementBase_c( acrc_src){
  assignFromSource( acrc_src );
}

void SetpointLocal_c::assignFromSource( const SetpointLocal_c& acrc_src )
{
  // now copy element vars
  mi32_setpointMaxAllowed = acrc_src.mi32_setpointMaxAllowed;
  mi32_setpointMinAllowed = acrc_src.mi32_setpointMinAllowed;
  mb_staticMaster = acrc_src.mb_staticMaster;
  mvec_register = acrc_src.mvec_register;


  // mpc_registerCache is a pointer, which must be copied relative to mvec_register.begin()
  // the distance operator needs a const_iterator
  Vec_SetpointRegister::const_iterator pc_iter = acrc_src.mpc_registerCache;
  mpc_registerCache = mvec_register.begin();
  STL_NAMESPACE::advance( mpc_registerCache, STL_NAMESPACE::distance( acrc_src.mvec_register.begin(), pc_iter));

  // copy master element vars
  for (mpc_master = mvec_register.begin(); mpc_master != mvec_register.end();mpc_master++)
  {
    if (mpc_master->master()) break;
  }

  mb_allowedDeltaPercent = acrc_src.mb_allowedDeltaPercent;
}

SetpointLocal_c::~SetpointLocal_c(){
  #if DEBUG_HEAP_USEAGE
  sui16_setpointLocalTotal -= ( mvec_register.size() * ( sizeof(SetpointRegister_c) + 2 * sizeof(SetpointRegister_c*) ) );

  INTERNAL_DEBUG_DEVICE
  << "SetLReg T: " << sui16_setpointLocalTotal << ", Node: " << ( sizeof(SetpointRegister_c) + 2 * sizeof(SetpointRegister_c*) ) << INTERNAL_DEBUG_DEVICE_ENDL;
  #endif
}

bool SetpointLocal_c::existUnhandledMaster() {
  int8_t i8_result = -1;

  if (mpc_registerCache == mvec_register.end())
    mpc_registerCache = mvec_register.begin();

  while (mpc_registerCache != mvec_register.end())
  {
    if ( ( ((existMaster()) && (mpc_registerCache->isoName() == masterConst().isoName()))
        || ((existMaster()) && (masterConst().isoName().isUnspecified()) )
        || ((mpc_registerCache->master()) )
         )
      && (!mpc_registerCache->handled())
       )
    {
      i8_result = 1;
      break;
    } // if

    if (i8_result < 0)
    { // first run with check for old cache
      i8_result = 0;
      mpc_registerCache = mvec_register.begin();
    } else
      mpc_registerCache++;

  } // for
  return ( i8_result > 0 );
}

SetpointRegister_c& SetpointLocal_c::unhandledMaster(){
  if (existUnhandledMaster())
  { // no unhandled master found
    IsoAgLib::getILibErrInstance().registerNonFatal( IsoAgLib::iLibErr_c::ProcDataSetpoint, getMultitonInst() );
  }
  return *mpc_registerCache;
}

void SetpointLocal_c::acceptNewMaster( bool ab_accept){
  if (existMaster())
  { // a master setpoint exist
    if (existUnhandledMaster())
    {
      if (ab_accept)
      { // accept
        // if handled: copy valid data (existMin(), existDefault() ...) from mpc_registerCache (pointer to newly created SetpointRegister_c)
        // and release mpc_registerCache and keep mpc_master!
        // (e.g. different from new received unhandled one)
        if (mpc_master->handled())
        {
          if (mpc_registerCache->existExact())
            mpc_master->setExact(mpc_registerCache->exact());
          if (mpc_registerCache->existMin())
            mpc_master->setMin(mpc_registerCache->min());
          if (mpc_registerCache->existMax())
            mpc_master->setMax(mpc_registerCache->max());
          if (mpc_registerCache->existDefault())
            mpc_master->setDefault(mpc_registerCache->getDefault());

          mvec_register.erase( mpc_registerCache );
          // set mpc_registerCache to valid value
          mpc_registerCache = mpc_master;
          #if DEBUG_HEAP_USEAGE
          sui16_setpointLocalTotal -= ( 1 * ( sizeof(SetpointRegister_c) + 2 * sizeof(SetpointRegister_c*) ) );

          INTERNAL_DEBUG_DEVICE
            << "SetLReg T: " << sui16_setpointLocalTotal << ", Node: " << ( sizeof(SetpointRegister_c) + 2 * sizeof(SetpointRegister_c*) ) << INTERNAL_DEBUG_DEVICE_ENDL;
          #endif
        }
      }
      else
      { // don't accept
        mpc_registerCache->setValid( false);
      }
    }
  }
  else
  { // no master setpoint
    IsoAgLib::getILibErrInstance().registerNonFatal( IsoAgLib::iLibErr_c::ProcDataSetpoint, getMultitonInst() );
  }
}

void SetpointLocal_c::setMasterMeasurementVal( int32_t ai32_val)
{
#ifdef USE_ANALYSE_PROCDATA_IMPL
  std::cout << __FUNCTION__ << std::endl;
#endif
  if (!existMaster())
  { // create register entry for master value
    mvec_register.push_front(SetpointRegister_c());
    #if DEBUG_HEAP_USEAGE
    sui16_setpointLocalTotal += ( 1 * ( sizeof(SetpointRegister_c) + 2 * sizeof(SetpointRegister_c*) ) );
    INTERNAL_DEBUG_DEVICE
      << "SetLReg T: " << sui16_setpointLocalTotal << ", Node: " << ( sizeof(SetpointRegister_c) + 2 * sizeof(SetpointRegister_c*) ) << INTERNAL_DEBUG_DEVICE_ENDL;
    #endif
    mpc_master = mvec_register.begin();
  }
  master().setExact( ai32_val);
  master().setHandled( true);
  master().setValid( true);
  master().setISOName(IsoName_c::IsoNameUnspecified());
}

uint8_t SetpointLocal_c::unhandledCnt()const{
  uint8_t b_result = 0;
  for (Vec_SetpointRegisterConstIterator pc_iter = mvec_register.begin();
       pc_iter != mvec_register.end();
       pc_iter++)
  {
    if (!(pc_iter->handled()))
    {
      b_result++;
    }
  }
  return b_result;
}

SetpointRegister_c& SetpointLocal_c::unhandledInd( uint8_t aui8_ind){
  uint8_t b_counter = 0;
  for (mpc_registerCache = mvec_register.begin();
       mpc_registerCache != mvec_register.end();
       mpc_registerCache++)
  {
    if (!(mpc_registerCache->handled()))b_counter++;

    // check if this was the aui8_ind item of the unhandled
    if (b_counter == aui8_ind) break;
  }
  // check if enough unhandled items found
  if (b_counter != aui8_ind)
  { // aui8_ind was too big
    IsoAgLib::getILibErrInstance().registerNonFatal( IsoAgLib::iLibErr_c::ProcDataSetpoint, getMultitonInst() );
    mpc_registerCache = mvec_register.begin();
  }

  // now the pointer points to the wanted unhandled setpoint entry
  // (or to another valid entry if the amount of
  //  unhandled setpoints is lower than aui8_ind)
  return *mpc_registerCache;
}

SetpointRegister_c& SetpointLocal_c::unhandledFirst(){
  // set result value to alowed val
  return unhandledInd( 0);
}

void SetpointLocal_c::respondAckNack(){
  // now send NO_VAL_32S for all other unhandled setpoints
  for (Vec_SetpointRegisterIterator pc_iter = mvec_register.begin();
       pc_iter != mvec_register.end();
       pc_iter++)
  { // send NO_VAL_32S to all unhandled not master setpoint entries
    if (!(pc_iter->handled()))
    { // first set the item to handled
      pc_iter->setHandled( true, System_c::getTime());
      // if no master -> send NO_VAL_32S
      if ( (!(pc_iter->master()))
        && ( (!(existMaster())) || (pc_iter->isoName() != master().isoName()))
         )
      { // item is no master and is not sent from actual master
        // send ignore msg
#ifdef USE_ANALYSE_PROCDATA_IMPL
        std::cout << __FUNCTION__ << "#1" << std::endl;
#endif
        sendSetpointVals(*pc_iter, true, NO_VAL_32S);
      }
      else
      { // actual item is newly accepted master
        // set iterator for actual master to actual list item
        if (pc_iter->master()) mpc_master = pc_iter;
        // send the newly accepted values as positive response
#ifdef USE_ANALYSE_PROCDATA_IMPL
        std::cout << __FUNCTION__ << "#2" << std::endl;
#endif
        sendSetpointVals( master());
      } // else item is master
    }
  }
}

int32_t SetpointLocal_c::checkMeasurement( int32_t ai32_val, bool ab_sendIfError) {
  int32_t i32_delta = 0;

  // store the actual valid state
  bool b_oldValid,
        b_actualValid = true;
  if (ab_sendIfError) b_actualValid = ab_sendIfError;
  uint8_t b_deviationPercent;

  if (existMaster())
  { // master value exists
    b_oldValid = masterConst().valid();

    // now check if new measurement is valid
    if (masterConst().existExact())
    { // exact setpoint exist
      i32_delta = (ai32_val - masterConst().exact());
      if (masterConst().exact() != 0)
        b_deviationPercent = ((__IsoAgLib::abs( i32_delta) * 100)/masterConst().exact());
      else b_deviationPercent = 100;
      b_actualValid = (b_deviationPercent < mb_allowedDeltaPercent);
    }
    else
    { // min or max
      if (masterConst().existMin())
      {
        i32_delta = (ai32_val - masterConst().min());
        b_deviationPercent = ((__IsoAgLib::abs( i32_delta) * 100)/masterConst().min());
        b_actualValid = ((i32_delta >= 0)||(b_deviationPercent < mb_allowedDeltaPercent));
      }
      if (b_actualValid && (masterConst().existMax()))
      { // the min test was if done successfull -> max limit exist -> check it
        i32_delta = (ai32_val - masterConst().max());
        b_deviationPercent = ((__IsoAgLib::abs( i32_delta) * 100)/masterConst().max());
        b_actualValid = ((i32_delta <= 0)||(b_deviationPercent < mb_allowedDeltaPercent));
      }
      // check for (only) percent can`t be done dependent on measurement
    }
    // now checks are done -> look if state changed
    if (b_oldValid != b_actualValid)
    { // change
      // set new state
      master().setValid( b_actualValid);
      // send notify if wanted
      if (b_actualValid)
      { // send actual limit for notify
        sendSetpointVals( masterConst());
      }
      else
      { // send error command
        //sendSetpointVals( masterConst(), true, SETPOINT_ERROR_COMMAND);
      }
    }
  }
  // now return the delta
  return i32_delta;
}

bool SetpointLocal_c::timeEvent( void ){
  if ( Scheduler_Task_c::getAvailableExecTime() == 0 ) return false;
  int32_t i32_time = Scheduler_Task_c::getLastRetriggerTime();

  // delete all NOT-master entries handled >3sec ago, or
  // delete master entries where isoName of caller is inactive >3sec
  bool b_repeat=true;
  while (b_repeat && (!mvec_register.empty()) ) {
    b_repeat = false;
    for (Vec_SetpointRegister::iterator pc_iter = mvec_register.begin();
        pc_iter != mvec_register.end(); pc_iter++)
    { // check handled time
      if (!pc_iter->master())
      {
        if ( ((i32_time - pc_iter->lastHandledTime()) > 3000)
          && (pc_iter->lastHandledTime() > 0)
          )
        { // pc_iter points to old handled (>3sec) item which is NOT the master -> delete it
          // and it was already handled (default time 0 after creation)
          mvec_register.erase( pc_iter);
          #if DEBUG_HEAP_USEAGE
          sui16_setpointLocalTotal -= ( 1 * ( sizeof(SetpointRegister_c) + 2 * sizeof(SetpointRegister_c*) ) );

          INTERNAL_DEBUG_DEVICE
            << "SetLReg T: " << sui16_setpointLocalTotal << ", Node: " << ( sizeof(SetpointRegister_c) + 2 * sizeof(SetpointRegister_c*) ) << INTERNAL_DEBUG_DEVICE_ENDL;
          #endif
          b_repeat = true;
          break; // old was: delete max one item per timeEvent, cause of reordering of list
        }
      }
      else if (!mb_staticMaster)
      { // pc_iter is master -> check if isoName is valid
        const IsoName_c& c_testISOName = pc_iter->isoName();
        if ( ( !getIsoMonitorInstance4Comm().existIsoMemberISOName( c_testISOName, true) ) )
        { // isoName of caller not in Monitor-List or inactive since >3sec -> delete entry
          mvec_register.erase( pc_iter);
          mpc_master = mvec_register.end(); // register that no acive master defined
          #if DEBUG_HEAP_USEAGE
          sui16_setpointLocalTotal -= ( 1 * ( sizeof(SetpointRegister_c) + 2 * sizeof(SetpointRegister_c*) ) );

          INTERNAL_DEBUG_DEVICE
            << "SetLReg T: " << sui16_setpointLocalTotal << ", Node: " << ( sizeof(SetpointRegister_c) + 2 * sizeof(SetpointRegister_c*) ) << INTERNAL_DEBUG_DEVICE_ENDL;
          #endif
          b_repeat = true;
          break; // delete no more entry during this run
        }
      }
    }
  }

  // check if the actual master measurement value is valid, force send of notify, if
  // valid state has changed
  checkMeasurement( processData().masterMeasurementVal(), true);

  return true;
}

void SetpointLocal_c::processMsg( const ProcessPkg_c& pkg ){
  // check if its a request for actual setpoint
  if (pkg.mc_processCmd.checkIsRequest())
  {
    processRequest(pkg);
  }
  else
  { // set setpoint value
    processSet(pkg);
  }
}

bool SetpointLocal_c::sendSetpointForGroup(const IsoName_c& ac_targetISOName,
                                           ProcessCmd_c::ValueGroup_t en_valueGroup,
                                           ProcessCmd_c::CommandType_t en_command) const
{
  ProcessPkg_c pkg;
  // prepare general command in process pkg
  pkg.mc_processCmd.setValues(true /* isSetpoint */, false, /* isRequest */
                                                           en_valueGroup, en_command);
  return processDataConst().sendValISOName( pkg, ac_targetISOName, masterConst().valForGroup(en_valueGroup));
}

void SetpointLocal_c::processRequest( const ProcessPkg_c& pkg ) const
{
  // check if master setpoint is defined
  bool b_existMaster = existMaster();

  if (b_existMaster)
  { // use the values in general command which are already set
    if (pkg.senderItem() != NULL)
    { // sender is valid
      sendSetpointForGroup (pkg.senderItem()->isoName(), pkg.mc_processCmd.getValueGroup(), ProcessCmd_c::setValue );
    }
  }
}

void SetpointLocal_c::processSet( const ProcessPkg_c& pkg )
{
  Vec_SetpointRegisterIterator pc_callerIter = mvec_register.begin();

  // detect if something was changed
  bool b_change = false;

  if (pkg.senderItem() == NULL)
  { // sender with SA 0xFE is not of interest
    return;
  }

  const IsoName_c& c_callerISOName = pkg.senderItem()->isoName();
  for (pc_callerIter = mvec_register.begin(); pc_callerIter != mvec_register.end(); pc_callerIter++)
  { // check if c_callerISOName already set the item at ui8_callerIndex
    // ignore item of actual acepted master, as this should be handled as new
    // item
    if (pc_callerIter->isoName() == c_callerISOName)
       if (!pc_callerIter->master())
         break;
  }
  // check if caller was found
  if (pc_callerIter == mvec_register.end())
  { // caller didn't set setpoint previous to this -> create item
    if (pkg.isSpecCmd( static_cast<proc_specCmd_t>(setpointReleaseCmd|setpointErrCmd)) == false)
    {
      mvec_register.push_front( SetpointRegister_c( c_callerISOName));
      #if DEBUG_HEAP_USEAGE
      sui16_setpointLocalTotal += ( 1 * ( sizeof(SetpointRegister_c) + 2 * sizeof(SetpointRegister_c*) ) );

      INTERNAL_DEBUG_DEVICE
        << "SetLReg T: " << sui16_setpointLocalTotal << ", Node: " << ( sizeof(SetpointRegister_c) + 2 * sizeof(SetpointRegister_c*) ) << INTERNAL_DEBUG_DEVICE_ENDL;
      #endif
      pc_callerIter = mvec_register.begin();
      b_change = true;
    }
    else
    { // do nothing with this command
      // because SETPOINT_RELEASE or SETPOINT_ERROR make only sense for
      // already registered setpoints
      return;
    }

    // ui8_callerIndex points to this new item
  }

  const int32_t i32_val = pkg.getValue();
  const uint32_t cui32_ddi = pkg.getValue();

  // check if setpoint is released -  SETPOINT_RELEASE_COMMAND
  if (pkg.isSpecCmd( setpointReleaseCmd))
  { // check if the to be deleted item is the actual master
    if ((existMaster()) && (master().isoName() == c_callerISOName))
    { // if this item was master setpoint, set pc_masterCache to end()
      mpc_master = mvec_register.end();
      b_change = true;
    }
    // delete the SetpointRegister_c item
    mvec_register.erase( pc_callerIter);
    #if DEBUG_HEAP_USEAGE
    sui16_setpointLocalTotal -= ( 1 * ( sizeof(SetpointRegister_c) + 2 * sizeof(SetpointRegister_c*) ) );

    INTERNAL_DEBUG_DEVICE
      << "SetLReg T: " << sui16_setpointLocalTotal << ", Node: " << ( sizeof(SetpointRegister_c) + 2 * sizeof(SetpointRegister_c*) ) << INTERNAL_DEBUG_DEVICE_ENDL;
    #endif

    ProcessPkg_c sendPkg( pkg );
    // prepare general command in process pkg
    sendPkg.mc_processCmd.setValues(true /* isSetpoint */, false, /* isRequest */
                                                             ProcessCmd_c::exactValue,
                                                             ProcessCmd_c::setValue);
    // notify the caller
    processData().sendValISOName( sendPkg, c_callerISOName, SETPOINT_RELEASE_COMMAND);
  }
  else
  {
    // now set according to mod the suitable setpoint type  value
    // simply set the new setpoint vlue
    if ( pc_callerIter->valForGroup( pkg.mc_processCmd.getValueGroup() ) != i32_val ) b_change = true;
    pc_callerIter->setValForGroup( i32_val, pkg.mc_processCmd.getValueGroup());
  }
  // call handler function if handler class is registered
  if ( processDataConst().getProcessDataChangeHandler() != NULL )
    processDataConst().getProcessDataChangeHandler()->processSetpointSet( pprocessData(), cui32_ddi, i32_val, c_callerISOName.toConstIisoName_c(), b_change );
}

void SetpointLocal_c::sendSetpointVals( const SetpointRegister_c& acrc_src,
                                       bool ab_override, int32_t ai32_overrideVal) const
{
  int32_t i32_value = 0;
//currently b_isCmd is set, but not used anywhere - just left there for reference...
//bool b_isCmd = false;
  ProcessCmd_c::ValueGroup_t en_valueGroup = ProcessCmd_c::noValue;

//if ((ai32_overrideVal == SETPOINT_RELEASE_COMMAND)
// || (ai32_overrideVal == SETPOINT_ERROR_COMMAND)
//    ) b_isCmd = true;

  if (acrc_src.existExact())
  { // exact setpoint exist
    en_valueGroup = ProcessCmd_c::exactValue;
    i32_value = (ab_override) ? ai32_overrideVal : acrc_src.exact();
  }
  else if (acrc_src.existMin())
  { // min setpoint exist
    en_valueGroup = ProcessCmd_c::minValue;
    i32_value = (ab_override) ? ai32_overrideVal : acrc_src.min();
  }
  else if (acrc_src.existMax())
  { // max setpoint exist
     en_valueGroup = ProcessCmd_c::maxValue;
     i32_value = (ab_override) ? ai32_overrideVal : acrc_src.max();
  }
  else if (acrc_src.existDefault())
  { // default setpoint exist
     en_valueGroup = ProcessCmd_c::defaultValue;
     i32_value = (ab_override) ? ai32_overrideVal : acrc_src.getDefault();
  }

  if (en_valueGroup != ProcessCmd_c::noValue)
  {
     ProcessPkg_c pkg;
     // prepare general command in process pkg
     pkg.mc_processCmd.setValues(true /* isSetpoint */, false /* isRequest */,
                                                              en_valueGroup, ProcessCmd_c::setValue);

     pprocessData()->sendValISOName( pkg, acrc_src.isoName(), i32_value);
  }
}

} // end of namespace __IsoAgLib
