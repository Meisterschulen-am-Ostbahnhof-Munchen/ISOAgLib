/***************************************************************************
                          setpointlocal_c.cpp - object for managing setpoints
                                              commanded from remote to local
                                              process data objects
                             -------------------
    begin                : Fri Apr 07 2000
    copyright            : (C) 2000 - 2009 by Dipl.-Inform. Achim Spangler
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
 * Copyright (C) 2000 - 2009 Dipl.-Inform. Achim Spangler                  *
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
 * - Call of a (member-) function                                          *
 * - Instantiation of a variable in a datatype from internal namespace     *
 * Allowed is:                                                             *
 * - Instatiation of a variable in a datatype from interface namespace,    *
 *   even if this is derived from a base class inside an internal namespace*
 * - Call of member functions which are defined in the interface class     *
 *   definition ( header )                                                 *
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
#include "setpointlocal_c.h"
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
/**
  initialise this SetpointLocal_c to a well defined starting condition
  @param apc_processData pointer to containing ProcessData instance
*/
void SetpointLocal_c::init( ProcDataBase_c *const apc_processData )
{
  SetpointBase_c::init( apc_processData );
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

/**
  assginment from another object
  @param acrc_src source SetpointLocal_c instance
  @return reference to source for cmd like "setp1 = setp2 = setp3;"
*/
const SetpointLocal_c& SetpointLocal_c::operator=( const SetpointLocal_c& acrc_src ){
  // first call base assignment operator
  SetpointBase_c::operator=(acrc_src);

  assignFromSource( acrc_src );

  // return reference
  return *this;
}

/**
  copy constructor for SetpointLocal
  @param acrc_src source SetpointLocal_c instance
*/
SetpointLocal_c::SetpointLocal_c( const SetpointLocal_c& acrc_src )
  : SetpointBase_c( acrc_src){
  assignFromSource( acrc_src );
}
/** base function for assignment of element vars for copy constructor and operator= */
void SetpointLocal_c::assignFromSource( const SetpointLocal_c& acrc_src )
{
  // now copy element vars
  mi32_setpointMaxAllowed = acrc_src.mi32_setpointMaxAllowed;
  mi32_setpointMinAllowed = acrc_src.mi32_setpointMinAllowed;
  mb_staticMaster = acrc_src.mb_staticMaster;
  mvec_register = acrc_src.mvec_register;


  if (mvec_register.size() < acrc_src.mvec_register.size())
  { // not all items copied
    getILibErrInstance().registerError( iLibErr_c::BadAlloc, iLibErr_c::Process );
  }
  #if DEBUG_HEAP_USEAGE
  else
  {
    sui16_setpointLocalTotal += ( mvec_register.size() * ( sizeof(SetpointRegister_c) + 2 * sizeof(SetpointRegister_c*) ) );

    INTERNAL_DEBUG_DEVICE
      << "SetLReg T: " << sui16_setpointLocalTotal << ", Node: " << ( sizeof(SetpointRegister_c) + 2 * sizeof(SetpointRegister_c*) ) << INTERNAL_DEBUG_DEVICE_ENDL;
  }
  #endif


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

/** default destructor which has nothing to do */
SetpointLocal_c::~SetpointLocal_c(){
  #if DEBUG_HEAP_USEAGE
  sui16_setpointLocalTotal -= ( mvec_register.size() * ( sizeof(SetpointRegister_c) + 2 * sizeof(SetpointRegister_c*) ) );

  INTERNAL_DEBUG_DEVICE
  << "SetLReg T: " << sui16_setpointLocalTotal << ", Node: " << ( sizeof(SetpointRegister_c) + 2 * sizeof(SetpointRegister_c*) ) << INTERNAL_DEBUG_DEVICE_ENDL;
  #endif
}

/**
  check if unhandled new setpoint from actual master was received
  -> check the actual master first before answering the other unhandled
  @see unhandledMaster
  @see acceptNewMaster
  @return true -> master setpoint comanding member sent new setpoint
*/
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

/**
  deliver the new sent setpoint values from the actual master;
  if no unhandled setpoint of the master exist, return the actual accepted value

  possible errors:
      * Err_c::elNonexistent no master setpoint found
  @see existUnhandledMaster
  @see acceptNewMaster
  @return reference to master setpoint register
*/
SetpointRegister_c& SetpointLocal_c::unhandledMaster(){
  if (existUnhandledMaster())
  { // no unhandled master found
    getILibErrInstance().registerError( iLibErr_c::ElNonexistent, iLibErr_c::Process );
  }
  return *mpc_registerCache;
}

/**
  if there exist a new unhandled setpoint from the master setpoint sender;
  accept the new value with this command
  otherwise the old value is still used and answered

  possible errors:
      * Err_c::elNonexistent no master setpoint found
  @see existUnhandledMaster
  @see unhandledMaster
  @param ab_accept true -> new setpoint of master is accepted -> send positive notify
*/
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
    getILibErrInstance().registerError( iLibErr_c::ElNonexistent, iLibErr_c::Process );
  }
}

/**
  set the master setpoint manually
  (in some cases remote systems request informations
   through process data setpoints)
  @param ai32_val wanted setpoint value
*/
void SetpointLocal_c::setMasterMeasurementVal( int32_t ai32_val)
{
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

/**
  deliver the count of unhandled setpoints
  @see unhandledInd
  @return count of unhandled received setpoints
*/
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

/**
  deliver the aui8_ind of the unhandled setpoints
  @param aui8_ind position of the wanted setpoint entry in list of unhandled setpoints
    (first entry: aui8_ind == 0!!)

  possible errors:
      * Err_c::range there are less than aui8_ind unhandled setpoints found
  @see unhandledCnt
  @return wanted unhandled setpoint
*/
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
    getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::Process );
    mpc_registerCache = mvec_register.begin();
  }

  // now the pointer points to the wanted unhandled setpoint entry
  // (or to another valid entry if the amount of
  //  unhandled setpoints is lower than aui8_ind)
  return *mpc_registerCache;
}

/**
  deliver the first unhandled entry

  possible errors:
      * Err_c::elNonexistent no unhandled stepoint exist
  @see unhandledCnt
  @return reference to first unhandled new received setpoint entry
*/
SetpointRegister_c& SetpointLocal_c::unhandledFirst(){
  // set result value to alowed val
  return unhandledInd( 0);
}

/**
  answer all unhandled setpoint enties;
  if new master setpoint was set, send the master
  the new accepted setpoint value;
  send all others NO_VAL_32S

  possible errors:
      * dependant error in ProcDataLocal_c commander of some setpoint isn't found in Monitor List
      * dependant error in CanIo_c on CAN send problems
*/
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
        sendSetpointVals(*pc_iter, true, NO_VAL_32S);
      }
      else
      { // actual item is newly accepted master
        // set iterator for actual master to actual list item
        if (pc_iter->master()) mpc_master = pc_iter;
        // send the newly accepted values as positive response
        sendSetpointVals( master());
      } // else item is master
    }
  }
}

/**
  check if the given measuremet value is correct for the actual
  master setpoint;
  @param ai32_val measured value
  @param ab_sendIfError true -> if actual value exceeds setpoint limits
         the actual value is sent as notification (default true)
  @return 0 -> correct; (n<0) -> measurement is delta n to low;
          (n>0) -> measurement is delta n to high
*/
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

/**
  perform periodic actions
  (here: check if measure val is in limits; delete old handled not master setpoints and
          delete master entry if it's isoName isn't registered active any more)
  @return true -> all planned activities performed in allowed time
*/
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
/**
  send a sub-setpoint (selected by value group) to a specified target (selected by GPT)
  @param ac_targetISOName ISOName of target
  @param en_valueGroup: min/max/exact/default
  @param en_command
  @return true -> successful sent
*/
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


/**
  process a setpoint request for local process data
*/
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

/**
  process a setpoint set for local process data
*/
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

} // end of namespace __IsoAgLib
