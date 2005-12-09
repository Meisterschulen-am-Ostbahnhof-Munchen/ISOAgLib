/***************************************************************************
                          setpointlocal_c.cpp - object for managing setpoints
                                              commanded from remote to local
                                              process data objects
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
#include "../../../impl/process_c.h"
#include "../../../processdatachangehandler_c.h"
#include <IsoAgLib/driver/system/impl/system_c.h>

#if defined(DEBUG) || defined(DEBUG_HEAP_USEAGE)
  #include <supplementary_driver/driver/rs232/impl/rs232io_c.h>
#endif

#ifdef DEBUG_HEAP_USEAGE
static uint16_t sui16_setpointLocalTotal = 0;
#endif

namespace __IsoAgLib {
/**
  initialise this SetpointLocal_c to a well defined starting condition
  @param rpc_processData pointer to containing ProcessData instance
*/
void SetpointLocal_c::init( ProcDataBase_c *const rpc_processData )
{
  SetpointBase_c::init( rpc_processData );
  // set pc_registerCache to a well defined value
  pc_registerCache = vec_register.begin();
  #ifdef DEBUG_HEAP_USEAGE
  sui16_setpointLocalTotal -= ( vec_register.size() * ( sizeof(SetpointRegister_c) + 2 * sizeof(SetpointRegister_c*) ) );
  #endif
  vec_register.clear();
  pc_master = vec_register.end();

  b_allowedDeltaPercent = 0;
  b_staticMaster = false;

  #ifdef DEBUG_HEAP_USEAGE
  getRs232Instance()
    << "sizeof(SetpointRegister_c) = " << sizeof(SetpointRegister_c)
    << " Bytes\r\n";
  #endif

}

/**
  assginment from another object
  @param rrefc_src source SetpointLocal_c instance
  @return reference to source for cmd like "setp1 = setp2 = setp3;"
*/
const SetpointLocal_c& SetpointLocal_c::operator=( const SetpointLocal_c& rrefc_src ){
  // first call base assignment operator
  SetpointBase_c::operator=(rrefc_src);

  assignFromSource( rrefc_src );

  // return reference
  return *this;
}

/**
  copy constructor for SetpointLocal
  @param rrefc_src source SetpointLocal_c instance
*/
SetpointLocal_c::SetpointLocal_c( const SetpointLocal_c& rrefc_src )
  : SetpointBase_c( rrefc_src){
  assignFromSource( rrefc_src );
}
/** base function for assignment of element vars for copy constructor and operator= */
void SetpointLocal_c::assignFromSource( const SetpointLocal_c& rrefc_src )
{
  // now copy element vars
  i32_setpointMaxAllowed = rrefc_src.i32_setpointMaxAllowed;
  i32_setpointMinAllowed = rrefc_src.i32_setpointMinAllowed;
  b_staticMaster = rrefc_src.b_staticMaster;
  vec_register = rrefc_src.vec_register;


  if (vec_register.size() < rrefc_src.vec_register.size())
  { // not all items copied
    getLbsErrInstance().registerError( LibErr_c::BadAlloc, LibErr_c::LbsProcess );
  }
  #ifdef DEBUG_HEAP_USEAGE
  else
  {
    sui16_setpointLocalTotal += ( vec_prog().size() * ( sizeof(SetpointRegister_c) + 2 * sizeof(SetpointRegister_c*) ) );

    getRs232Instance()
      << "SetLReg T: " << sui16_setpointLocalTotal << ", Node: " << ( sizeof(SetpointRegister_c) + 2 * sizeof(SetpointRegister_c*) ) << "\r\n";
  }
  #endif


  // pc_registerCache is a pointer, which must be copied relative to vec_register.begin()
  // the distance operator needs a const_iterator
  Vec_SetpointRegister::const_iterator pc_iter = rrefc_src.pc_registerCache;
  pc_registerCache = vec_register.begin();
  STL_NAMESPACE::advance( pc_registerCache, STL_NAMESPACE::distance( rrefc_src.vec_register.begin(), pc_iter));

  // copy master element vars
  for (pc_master = vec_register.begin(); pc_master != vec_register.end();pc_master++)
  {
    if (pc_master->master()) break;
  }

  b_allowedDeltaPercent = rrefc_src.b_allowedDeltaPercent;
}

/** default destructor which has nothing to do */
SetpointLocal_c::~SetpointLocal_c(){
  #ifdef DEBUG_HEAP_USEAGE
  sui16_setpointLocalTotal -= ( vec_register.size() * ( sizeof(SetpointRegister_c) + 2 * sizeof(SetpointRegister_c*) ) );

  getRs232Instance()
  << "SetLReg T: " << sui16_setpointLocalTotal << ", Node: " << ( sizeof(SetpointRegister_c) + 2 * sizeof(SetpointRegister_c*) ) << "\r\n";
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

  if (pc_registerCache == vec_register.end())
    pc_registerCache = vec_register.begin();

  while (pc_registerCache != vec_register.end())
  {
    if ( ( ((existMaster()) && (pc_registerCache->devKey() == masterConst().devKey()))
        || ((existMaster()) && (masterConst().devKey() == 0xFF) )
        || ((pc_registerCache->master()) )
         )
      && (!pc_registerCache->handled())
       )
    {
      i8_result = 1;
      break;
    } // if

    if (i8_result < 0)
    { // first run with check for old cache
      i8_result = 0;
      pc_registerCache = vec_register.begin();
    } else
      pc_registerCache++;

  } // for
  return ( i8_result > 0 )?true:false;
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
    getLbsErrInstance().registerError( LibErr_c::ElNonexistent, LibErr_c::LbsProcess );
  }
  return *pc_registerCache;
}

/**
  if there exist a new unhandled setpoint from the master setpoint sender;
  accept the new value with this command
  otherwise the old value is still used and answered

  possible errors:
      * Err_c::elNonexistent no master setpoint found
  @see existUnhandledMaster
  @see unhandledMaster
  @param rb_accept true -> new setpoint of master is accepted -> send positive notify
*/
void SetpointLocal_c::acceptNewMaster( bool rb_accept){
  if (existMaster())
  { // a master setpoint exist
    if (existUnhandledMaster())
    {
      if (rb_accept)
      { // accept
        // if handled: copy valid data (existMin(), existDefault() ...) from pc_registerCache (pointer to newly created SetpointRegister_c)
        // and release pc_registerCache and keep pc_master!
        // (e.g. different from new received unhandled one)
        if (pc_master->handled())
        {
          if (pc_registerCache->existExact())
            pc_master->setExact(pc_registerCache->exact());
          if (pc_registerCache->existMin())
            pc_master->setMin(pc_registerCache->min());
          if (pc_registerCache->existMax())
            pc_master->setMax(pc_registerCache->max());
          if (pc_registerCache->existDefault())
            pc_master->setDefault(pc_registerCache->getDefault());

          vec_register.erase( pc_registerCache );
          #ifdef DEBUG_HEAP_USEAGE
          sui16_setpointLocalTotal -= ( 1 * ( sizeof(SetpointRegister_c) + 2 * sizeof(SetpointRegister_c*) ) );

          getRs232Instance()
            << "SetLReg T: " << sui16_setpointLocalTotal << ", Node: " << ( sizeof(SetpointRegister_c) + 2 * sizeof(SetpointRegister_c*) ) << "\r\n";
          #endif
        }
      }
      else
      { // don't accept
        pc_registerCache->setValid( false);
      }
    }
  }
  else
  { // no master setpoint
    getLbsErrInstance().registerError( LibErr_c::ElNonexistent, LibErr_c::LbsProcess );
  }
}

/**
  set the master setpoint manually
  (in some cases remote systems request informations
   through process data setpoints)
  @param ri32_val wanted setpoint value
*/
void SetpointLocal_c::setMasterMeasurementVal( int32_t ri32_val)
{
  if (!existMaster())
  { // create register entry for master value
    const uint16_t cui16_oldSize = vec_register.size();
    vec_register.push_front();
    if ( cui16_oldSize >= vec_register.size() )
    { // out-of-memory
      getLbsErrInstance().registerError( LibErr_c::BadAlloc, LibErr_c::LbsProcess );
      return;
    }
    #ifdef DEBUG_HEAP_USEAGE
    else
    {
      sui16_setpointLocalTotal += ( 1 * ( sizeof(SetpointRegister_c) + 2 * sizeof(SetpointRegister_c*) ) );

      getRs232Instance()
        << "SetLReg T: " << sui16_setpointLocalTotal << ", Node: " << ( sizeof(SetpointRegister_c) + 2 * sizeof(SetpointRegister_c*) ) << "\r\n";
    }
    #endif
    pc_master = vec_register.begin();
  }
  master().setExact( ri32_val);
  master().setHandled( true);
  master().setValid( true);
  master().setDevKey( 0xFF);

#ifdef USE_FLOAT_DATA_TYPE
  setValType( i32_val);
#endif
}

#ifdef USE_FLOAT_DATA_TYPE
/**
  set the master setpoint manually
  (in some cases remote systems request informations
   through process data setpoints)
  @param rf_val wanted setpoint value
*/
void SetpointLocal_c::setMasterMeasurementVal( float rf_val)
{
  int32_t i32_temp = (*(int32_t*)(&rf_val));
  setMasterMeasurementVal( i32_temp);
  setValType( float_val);
}
#endif

/**
  deliver the count of unhandled setpoints
  @see unhandledInd
  @return count of unhandled received setpoints
*/
uint8_t SetpointLocal_c::unhandledCnt()const{
  uint8_t b_result = 0;
  for (Vec_SetpointRegisterConstIterator pc_iter = vec_register.begin();
       pc_iter != vec_register.end();
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
  deliver the rui8_ind of the unhandled setpoints
  @param rui8_ind position of the wanted setpoint entry in list of unhandled setpoints
    (first entry: rui8_ind == 0!!)

  possible errors:
      * Err_c::range there are less than rui8_ind unhandled setpoints found
  @see unhandledCnt
  @return wanted unhandled setpoint
*/
SetpointRegister_c& SetpointLocal_c::unhandledInd( uint8_t rui8_ind){
  uint8_t b_counter = 0;
  for (pc_registerCache = vec_register.begin();
       pc_registerCache != vec_register.end();
       pc_registerCache++)
  {
    if (!(pc_registerCache->handled()))b_counter++;

    // check if this was the rui8_ind item of the unhandled
    if (b_counter == rui8_ind) break;
  }
  // check if enough unhandled items found
  if (b_counter != rui8_ind)
  { // rui8_ind was too big
    getLbsErrInstance().registerError( LibErr_c::Range, LibErr_c::LbsProcess );
    pc_registerCache = vec_register.begin();
  }

  // now the pointer points to the wanted unhandled setpoint entry
  // (or to another valid entry if the amount of
  //  unhandled setpoints is lower than rui8_ind)
  return *pc_registerCache;
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
      * dependant error in CANIO_c on CAN send problems
*/
void SetpointLocal_c::answerAllUnhandled(){
  // now send NO_VAL_32S for all other unhandled setpoints
  for (Vec_SetpointRegisterIterator pc_iter = vec_register.begin();
       pc_iter != vec_register.end();
       pc_iter++)
  { // send NO_VAL_32S to all unhandled not master setpoint entries
    if (!(pc_iter->handled()))
    { // first set the item to handled
      pc_iter->setHandled( true, System_c::getTime());
      // if no master -> send NO_VAL_32S
      if ( (!(pc_iter->master()))
        && ( (!(existMaster())) || (pc_iter->devKey() != master().devKey()))
         )
      { // item is no master and is not sent from actual master
        // send ignore msg
        sendSetpointVals(*pc_iter, true, NO_VAL_32S);
      }
      else
      { // actual item is newly accepted master
        // set iterator for actual master to actual list item
        if (pc_iter->master()) pc_master = pc_iter;
        // send the newly accepted values as positive response
        sendSetpointVals( master());
      } // else item is master
    }
  }
}

/**
  check if the given measuremet value is correct for the actual
  master setpoint;
  @param ri32_val measured value
  @param rb_sendIfError true -> if actual value exceeds setpoint limits
         the actual value is sent as notification (default true)
  @return 0 -> correct; (n<0) -> measurement is delta n to low;
          (n>0) -> measurement is delta n to high
*/
int32_t SetpointLocal_c::checkMeasurement( int32_t ri32_val, bool rb_sendIfError) {
  int32_t i32_delta = 0;

  // store the actual valid state
  bool b_oldValid,
        b_actualValid = true;
  if (rb_sendIfError) b_actualValid = rb_sendIfError;
  uint8_t b_deviationPercent;

  if (existMaster())
  { // master value exists
    b_oldValid = masterConst().valid();

    // now check if new measurement is valid
    if (masterConst().existExact())
    { // exact setpoint exist
      i32_delta = (ri32_val - masterConst().exact());
      if (masterConst().exact() != 0)
        b_deviationPercent = ((CNAMESPACE::labs( i32_delta) * 100)/masterConst().exact());
      else b_deviationPercent = 100;
      b_actualValid = (b_deviationPercent < b_allowedDeltaPercent)?true:false;
    }
    else
    { // min or max
      if (masterConst().existMin())
      {
        i32_delta = (ri32_val - masterConst().min());
        b_deviationPercent = ((CNAMESPACE::labs( i32_delta) * 100)/masterConst().min());
        b_actualValid = ((i32_delta >= 0)||(b_deviationPercent < b_allowedDeltaPercent))?true:false;
      }
      if (b_actualValid && (masterConst().existMax()))
      { // the min test was if done successfull -> max limit exist -> check it
        i32_delta = (ri32_val - masterConst().max());
        b_deviationPercent = ((CNAMESPACE::labs( i32_delta) * 100)/masterConst().max());
        b_actualValid = ((i32_delta <= 0)||(b_deviationPercent < b_allowedDeltaPercent))?true:false;
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
        sendSetpointVals( masterConst(), true, SETPOINT_ERROR_COMMAND);
      }
    }
  }
  // now return the delta
  return i32_delta;
}

/**
  perform periodic actions
  (here: check if measure val is in limits; delete old handled not master setpoints and
          delete master entry if it's devKey isn't registered active any more)
  @return true -> all planned activities performed in allowed time
*/
bool SetpointLocal_c::timeEvent( void ){
  if ( Scheduler_c::getAvailableExecTime() == 0 ) return false;
  int32_t i32_time = Scheduler_c::getLastTimeEventTrigger();

  // delete all NOT-master entries handled >3sec ago, or
  // delete master entries where devKey of caller is inactive >3sec
  bool b_repeat=true;
  while (b_repeat && (!vec_register.empty()) ) {
    b_repeat = false;
    for (Vec_SetpointRegister::iterator pc_iter = vec_register.begin();
        pc_iter != vec_register.end(); pc_iter++)
    { // check handled time
      if (!pc_iter->master())
      {
        if ( ((i32_time - pc_iter->lastHandledTime()) > 3000)
          && (pc_iter->lastHandledTime() > 0)
          )
        { // pc_iter points to old handled (>3sec) item which is NOT the master -> delete it
          // and it was already handled (default time 0 after creation)
          vec_register.erase( pc_iter);
          #ifdef DEBUG_HEAP_USEAGE
          sui16_setpointLocalTotal -= ( 1 * ( sizeof(SetpointRegister_c) + 2 * sizeof(SetpointRegister_c*) ) );

          getRs232Instance()
            << "SetLReg T: " << sui16_setpointLocalTotal << ", Node: " << ( sizeof(SetpointRegister_c) + 2 * sizeof(SetpointRegister_c*) ) << "\r\n";
          #endif
          b_repeat = true;
          break; // old was: delete max one item per timeEvent, cause of reordering of list
        }
      }
      else if (!b_staticMaster)
      { // pc_iter is master -> check if devKey is valid
        const DevKey_c& c_testDevKey = pc_iter->devKey();
        if ( (!getSystemMgmtInstance4Comm().existMemberDevKey( c_testDevKey, true))
          #if  defined( USE_DIN_9687 )
          || ( (getSystemMgmtInstance4Comm().memberDevKey( c_testDevKey, true).lastedTime() > 3000)
              #if defined( USE_ISO_11783 )
              (getSystemMgmtInstance4Comm().memberDevKey( c_testDevKey, true).itemState( IState_c::Din))
              #endif
            )
          #endif
          )
        { // devKey of caller not in Monitor-List or inactive since >3sec -> delete entry
          vec_register.erase( pc_iter);
          pc_master = vec_register.end(); // register that no acive master defined
          #ifdef DEBUG_HEAP_USEAGE
          sui16_setpointLocalTotal -= ( 1 * ( sizeof(SetpointRegister_c) + 2 * sizeof(SetpointRegister_c*) ) );

          getRs232Instance()
            << "SetLReg T: " << sui16_setpointLocalTotal << ", Node: " << ( sizeof(SetpointRegister_c) + 2 * sizeof(SetpointRegister_c*) ) << "\r\n";
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
  send a sub-setpoint (selected by MOD) to a specified target (selected by GPT)
  @param GeneralCommand_c::ValueGroup_t min/max/exact/default code of the value type to send
  @param rc_targetDevKey DevKey of target
  @param ren_type optional PRI specifier of the message (default Proc_c::Target )
  @param en_valueGroup: min/max/exact/default
  @param en_command
  @return true -> successful sent
*/
bool SetpointLocal_c::sendSetpointMod(const DevKey_c& rc_targetDevKey,
                                      Proc_c::progType_t ren_progType,
                                      GeneralCommand_c::ValueGroup_t en_valueGroup,
                                      GeneralCommand_c::CommandType_t en_command) const {
  // prepare general command in process pkg
  getProcessInstance4Comm().data().c_generalCommand.setValues(true /* isSetpoint */, false, /* isRequest */
                                                              en_valueGroup, en_command);
  #ifdef USE_FLOAT_DATA_TYPE
  if (valType() == i32_val) {
  #endif
    return processDataConst().sendValDevKey(ren_progType, rc_targetDevKey, masterConst().valMod(en_valueGroup));
  #ifdef USE_FLOAT_DATA_TYPE
  }
  else {
    return processDataConst().sendValDevKey( ren_progType,rc_targetDevKey, masterConst().valModFloat(en_valueGroup));
  }
  #endif
}


/**
  process a setpoint request for local process data
*/
void SetpointLocal_c::processRequest() const {
  ProcessPkg_c& c_pkg = getProcessInstance4Comm().data();

  // check if master setpoint is defined
  bool b_existMaster = existMaster();

  if (b_existMaster)
  {
    // use the values in general command which are already set
    sendSetpointMod(c_pkg.memberSend().devKey(), Proc_c::progType_t( c_pkg.pri() ), c_pkg.c_generalCommand.getValueGroup(), GeneralCommand_c::setValue );
  }
}

/**
  process a setpoint set for local process data
*/
void SetpointLocal_c::processSet(){
  ProcessPkg_c& c_pkg = getProcessInstance4Comm().data();
  Vec_SetpointRegisterIterator pc_callerIter = vec_register.begin();

  // detect if something was changed
  bool b_change = false;

  const DevKey_c& c_callerDevKey = c_pkg.memberSend().devKey();
  for (pc_callerIter = vec_register.begin(); pc_callerIter != vec_register.end(); pc_callerIter++)
  { // check if c_callerDevKey already set the item at ui8_callerIndex
    // ignore item of actual acepted master, as this should be handled as new
    // item
    if (pc_callerIter->devKey() == c_callerDevKey)
       // @todo: disable check?
       if (!pc_callerIter->master())
         break;
  }
  // check if caller was found
  if (pc_callerIter == vec_register.end())
  { // caller didn't set setpoint previous to this -> create item
    if (c_pkg.isSpecCmd( static_cast<proc_specCmd_t>(setpointReleaseCmd|setpointErrCmd)) == false)
    {
      const uint16_t cui16_oldSize = vec_register.size();
      vec_register.push_front( SetpointRegister_c( c_callerDevKey));
      if ( cui16_oldSize >= vec_register.size() )
      { // out-of-memory
        getLbsErrInstance().registerError( LibErr_c::BadAlloc, LibErr_c::LbsProcess );
        return;
      }
      #ifdef DEBUG_HEAP_USEAGE
      else
      {
        sui16_setpointLocalTotal += ( 1 * ( sizeof(SetpointRegister_c) + 2 * sizeof(SetpointRegister_c*) ) );

        getRs232Instance()
          << "SetLReg T: " << sui16_setpointLocalTotal << ", Node: " << ( sizeof(SetpointRegister_c) + 2 * sizeof(SetpointRegister_c*) ) << "\r\n";
      }
      #endif
      pc_callerIter = vec_register.begin();
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
  // check if setpoint is released -  SETPOINT_RELEASE_COMMAND
  if (c_pkg.isSpecCmd( setpointReleaseCmd))
  { // check if the to be deleted item is the actual master
    if ((existMaster()) && (master().devKey() == c_callerDevKey))
    { // if this item was master setpoint, set pc_masterCache to end()
      pc_master = vec_register.end();
      b_change = true;
    }
    // delete the SetpointRegister_c item
    vec_register.erase( pc_callerIter);
    #ifdef DEBUG_HEAP_USEAGE
    sui16_setpointLocalTotal -= ( 1 * ( sizeof(SetpointRegister_c) + 2 * sizeof(SetpointRegister_c*) ) );

    getRs232Instance()
      << "SetLReg T: " << sui16_setpointLocalTotal << ", Node: " << ( sizeof(SetpointRegister_c) + 2 * sizeof(SetpointRegister_c*) ) << "\r\n";
    #endif

    // prepare general command in process pkg
    getProcessInstance4Comm().data().c_generalCommand.setValues(true /* isSetpoint */, false, /* isRequest */
                                                                GeneralCommand_c::exactValue,
                                                                GeneralCommand_c::setValue);
    // notify the caller
    processData().sendValDevKey( c_pkg.pri(),
                c_pkg.memberSend().devKey() ,
                SETPOINT_RELEASE_COMMAND);
  }
  else
  {
    #ifdef USE_FLOAT_DATA_TYPE
    if (processData().valType() == i32_val)
    { // get the int32_t data val; let it convert, if needed
      setValType( i32_val);
    #endif
      int32_t i32_val = processData().pkgDataLong();
      // now set according to mod the suitable setpoint type  value
      // simply set the new setpoint vlue
      if ( pc_callerIter->valMod( c_pkg.c_generalCommand.getValueGroup() ) != i32_val ) b_change = true;
      pc_callerIter->setValMod( i32_val, c_pkg.c_generalCommand.getValueGroup());
    #ifdef USE_FLOAT_DATA_TYPE
    }
    else
    { // get the int32_t data val; let it convert, if needed
      setValType( float_val);
      float f_val = processData().pkgDataFloat();
      // now set according to mod the suitable setpoint type  value
      // simply set the new setpoint vlue
      if ( pc_callerIter->valModFloat( c_pkg.c_generalCommand.getValueGroup() ) != f_val ) b_change = true;
      pc_callerIter->setValMod( f_val, c_pkg.c_generalCommand.getValueGroup() );
    }
    #endif
  }
  // call handler function if handler class is registered
  if ( processDataConst().getProcessDataChangeHandler() != NULL )
    processDataConst().getProcessDataChangeHandler()->processSetpointSet( pprocessData(), processData().pkgDataLong(), c_pkg.memberSend().devKey(), b_change );
}

} // end of namespace __IsoAgLib
