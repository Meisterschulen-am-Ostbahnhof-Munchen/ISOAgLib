/***************************************************************************
                          isomonitor_c.cpp - object for monitoring members
                                              (list of ISOItem_c)
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
#include "isomonitor_c.h"
#include "isosystempkg_c.h"
#include <IsoAgLib/util/liberr_c.h>
#include <IsoAgLib/comm/Scheduler/impl/scheduler_c.h>
#include <IsoAgLib/driver/can/impl/canio_c.h>

#ifdef USE_PROCESS
	#include <IsoAgLib/comm/Process/impl/process_c.h>
#endif

#if defined(DEBUG) || defined(DEBUG_HEAP_USEAGE)
	#include <supplementary_driver/driver/rs232/impl/rs232io_c.h>
	#include <IsoAgLib/util/impl/util_funcs.h>
#endif

#ifdef DEBUG_HEAP_USEAGE
static uint16_t sui16_isoItemTotal = 0;
#endif

#ifdef SYSTEM_PC
  #include <iostream>
#endif

namespace __IsoAgLib {
#if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
  /** C-style function, to get access to the unique ISOMonitor_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  ISOMonitor_c& getIsoMonitorInstance( uint8_t rui8_instance )
  { // if > 1 singleton instance is used, no static reference can be used
    return ISOMonitor_c::instance( rui8_instance );
  };
#else
  /** C-style function, to get access to the unique ISOMonitor_c singleton instance */
  ISOMonitor_c& getIsoMonitorInstance( void )
  {
    static ISOMonitor_c& c_isoMonitor = ISOMonitor_c::instance();
    return c_isoMonitor;
  };
#endif

/**
  constructor for ISOMonitor_c which can store optional pointer to central Scheduler_c instance
  @param rpc_lb optional pointer to central Scheduler_c instance (default NULL -> the later set is needed)
*/
ISOMonitor_c::ISOMonitor_c()
  : SingletonISOMonitor_c(), vec_isoMember() {
  init();
}

/** initialisation for ISOMonitor_c which can store optional pointer to central Scheduler_c instance
*/
void ISOMonitor_c::init( void )
{
  c_data.setSingletonKey( getSingletonVecKey() );
  #ifdef DEBUG_HEAP_USEAGE
  sui16_isoItemTotal -= vec_isoMember.size();
  #endif
  vec_isoMember.clear();
  pc_isoMemberCache = vec_isoMember.end();
  i32_lastSaRequest = 0;
  c_tempIsoMemberItem.set( 0, GetyPos_c(0xF, 0xF), 0xFE, IState_c::Active,
           0xFFFF, (ISOName_c*)NULL, getSingletonVecKey() );

  // clear state of b_alreadyClosed, so that close() is called one time
  clearAlreadyClosed();
  // register in Scheduler_c to be triggered fopr timeEvent
  getSchedulerInstance4Comm().registerClient( this );

  bool b_configure = false;
  if (getCanInstance4Comm().insertFilter( *this, MASK_TYPE(static_cast<MASK_TYPE>(0x1FF00) << 8), MASK_TYPE(static_cast<MASK_TYPE>(REQUEST_PGN_MSG_PGN) << 8), false, Ident_c::ExtendedIdent))
    b_configure = true;
  if (getCanInstance4Comm().insertFilter( *this, MASK_TYPE(static_cast<MASK_TYPE>(0x1FFFF) << 8), MASK_TYPE(static_cast<MASK_TYPE>((ADRESS_CLAIM_PGN)+0xFF) << 8), false, Ident_c::ExtendedIdent))
    b_configure = true;
  if (getCanInstance4Comm().insertFilter( *this, MASK_TYPE(static_cast<MASK_TYPE>(0x1FFFF) << 8), MASK_TYPE(static_cast<MASK_TYPE>(WORKING_SET_MASTER_PGN) << 8), false, Ident_c::ExtendedIdent))
    b_configure = true;
  if (getCanInstance4Comm().insertFilter( *this, MASK_TYPE(static_cast<MASK_TYPE>(0x1FFFF) << 8), MASK_TYPE(static_cast<MASK_TYPE>(WORKING_SET_MEMBER_PGN) << 8), false, Ident_c::ExtendedIdent))
    b_configure = true;

  if (b_configure) {
    getCanInstance4Comm().reconfigureMsgObj();
  }
}


/** default destructor which has nothing to do */
ISOMonitor_c::~ISOMonitor_c(){
  close();
}

/** every subsystem of IsoAgLib has explicit function for controlled shutdown
  */
void ISOMonitor_c::close( void ) {
  if ( ! checkAlreadyClosed() ) {
    // avoid another call
    setAlreadyClosed();
    getSchedulerInstance4Comm().unregisterClient( this );

    getCanInstance4Comm().deleteFilter( *this, MASK_TYPE(static_cast<MASK_TYPE>(0x1FF00) << 8), MASK_TYPE(static_cast<MASK_TYPE>(REQUEST_PGN_MSG_PGN) << 8), Ident_c::ExtendedIdent);
    getCanInstance4Comm().deleteFilter( *this, MASK_TYPE(static_cast<MASK_TYPE>(0x1FFFF) << 8), MASK_TYPE(static_cast<MASK_TYPE>((ADRESS_CLAIM_PGN)+0xFF) << 8), Ident_c::ExtendedIdent);
    getCanInstance4Comm().deleteFilter( *this, MASK_TYPE(static_cast<MASK_TYPE>(0x1FFFF) << 8), MASK_TYPE(static_cast<MASK_TYPE>(WORKING_SET_MASTER_PGN) << 8), Ident_c::ExtendedIdent);
    getCanInstance4Comm().deleteFilter( *this, MASK_TYPE(static_cast<MASK_TYPE>(0x1FFFF) << 8), MASK_TYPE(static_cast<MASK_TYPE>(WORKING_SET_MEMBER_PGN) << 8), Ident_c::ExtendedIdent);
  }
};


/**
  deliver reference to data pkg as reference to CANPkgExt_c
  to implement the base virtual function correct
*/
CANPkgExt_c& ISOMonitor_c::dataBase()
{
  return c_data;
}

/**
  performs periodically actions,

  possible errors:
    * pertial error caused by one of the memberItems

  @return true -> all planned activities performed in allowed time
*/
bool ISOMonitor_c::timeEvent( void ){
  #if CONFIG_ISO_ITEM_MAX_AGE > 0
  if ( Scheduler_c::getAvailableExecTime() == 0 ) return false;

  int32_t i32_now = Scheduler_c::getLastTimeEventTrigger();

  bool b_repeat = true;
  while ((!vec_isoMember.empty()) && b_repeat) {
    b_repeat = false;
    for(Vec_ISOIterator pc_iter = vec_isoMember.begin(); pc_iter != vec_isoMember.end(); pc_iter++)
    { // delete item, if it didn't answer longer than CONFIG_ISO_ITEM_MAX_AGE since last adress claim request
      int32_t i32_timeSinceLastRequestWithMaxAge = (i32_now - lastIsoSaRequest()) + CONFIG_ISO_ITEM_MAX_AGE;
      int32_t i32_timeSinceLastAdressClaim = pc_iter->lastedTime();
      if ((i32_timeSinceLastRequestWithMaxAge > 0)
      && (i32_timeSinceLastRequestWithMaxAge < i32_timeSinceLastAdressClaim)
      &&(!(pc_iter->itemState(IState_c::Local))))
      {
        Vec_ISOIterator pc_iterDelete = pc_iter;
        vec_isoMember.erase(pc_iterDelete);
        #ifdef DEBUG_HEAP_USEAGE
        sui16_isoItemTotal--;

        getRs232Instance()
	        << sui16_isoItemTotal << " x ISOItem_c: Mal-Alloc: "
          <<  sizeSlistTWithMalloc( sizeof(ISOItem_c), sui16_isoItemTotal )
          << "/" << sizeSlistTWithMalloc( sizeof(ISOItem_c), 1 )
          << ", Chunk-Alloc: "
          << sizeSlistTWithChunk( sizeof(ISOItem_c), sui16_isoItemTotal )
          << "\r\n\r\n";
        #endif
        pc_iter = vec_isoMember.begin();
        b_repeat = true;
        // break and start it all over again (because of pc_iter changed to begin(), which won't go with the following pc_iter++ if empty!
        break;
      }
    }
  }
  #endif
  return true;
}

/**
  deliver the count of members in the Monitor-List with given GETY (variable POS)
  which optional (!!) match the condition of address claim state
  @param rb_gety searched GETY code
  @param rb_forceClaimedAddress true -> only members with claimed address are used
        (optional, default false)
  @return count of members in Monitor-List with GETY == rb_gety
*/
uint8_t ISOMonitor_c::isoMemberGetyCnt(uint8_t rb_gety, bool rb_forceClaimedAddress)
{
  uint8_t b_result = 0;
  for (Vec_ISOIterator pc_iter = vec_isoMember.begin() ; pc_iter != vec_isoMember.end(); pc_iter++)
  {
//    cerr << "Tested Member Gety: " << int( pc_iter->gtp().getGety() ) << endl;
    if ( ( ((pc_iter->gtp().getGety()) == rb_gety) || (rb_gety == 0xFF))
      && (!rb_forceClaimedAddress || pc_iter->itemState(IState_c::ClaimedAddress)) )
    {
      b_result++;
      pc_isoMemberCache = pc_iter; // set member cache to member  with searched gety
    }
  }
  return b_result;
}

/**
  deliver one of the members with specific GETY
  which optional (!!) match the condition of address claim state
  check first with isoMemberGetyCnt if enough members with wanted GETY and
  optional (!!) property are registered in Monitor-List
  @see isoMemberGetyCnt

  possible errors:
    * range there exist less than rui8_ind members with GETY rb_gety
 @param rb_gety searched GETY
 @param rui8_ind position of the wanted member in the
               sublist of member with given GETY (first item has rui8_ind == 0 !!)
 @param rb_forceClaimedAddress true -> only members with claimed address are used
       (optional, default false)
 @return reference to searched element
*/
ISOItem_c& ISOMonitor_c::isoMemberGetyInd(uint8_t rb_gety, uint8_t rui8_ind, bool rb_forceClaimedAddress)
{
  int8_t c_cnt = -1;
  for (Vec_ISOIterator pc_iter  = vec_isoMember.begin() ; pc_iter != vec_isoMember.end(); pc_iter++)
  {
    if ( ( ((pc_iter->gtp().getGety()) == rb_gety) || (rb_gety == 0xFF))
      && (!rb_forceClaimedAddress || pc_iter->itemState(IState_c::ClaimedAddress)) )
    {
      c_cnt++;
      if (c_cnt == rui8_ind)
      {
        pc_isoMemberCache = pc_iter; // set member cache to member  with searched gety
        break; //searched Item found (first element has 0)break; //searched Item found (first element has 0)
      }
    }
  }
  // check if rui8_ind was in correct range
  if (rui8_ind != c_cnt)
  { // wrong range of rui8_ind
    getLbsErrInstance().registerError( LibErr_c::Range, LibErr_c::LbsSystem );
  }
  return *pc_isoMemberCache;
}

/**
  check if a memberItem with given GETY_POS exist
  which optional (!!) match the condition of address claim state
  and update local pc_isoMemberCache
  @param rc_gtp searched GETY_POS
  @param rb_forceClaimedAddress true -> only members with claimed address are used
        (optional, default false)
  @return true -> searched member exist
*/
bool ISOMonitor_c::existIsoMemberGtp(GetyPos_c rc_gtp, bool rb_forceClaimedAddress)
{
  if (!vec_isoMember.empty() && (pc_isoMemberCache != vec_isoMember.end()))
  {
    if ((pc_isoMemberCache->gtp() == rc_gtp )&&(!rb_forceClaimedAddress || pc_isoMemberCache->itemState(IState_c::ClaimedAddress)))  return true;
  }
  for (pc_isoMemberCache = vec_isoMember.begin();
       pc_isoMemberCache != vec_isoMember.end();
       pc_isoMemberCache++)
  { if (pc_isoMemberCache->gtp() == rc_gtp) break;};
  return ((pc_isoMemberCache != vec_isoMember.end())
        &&(pc_isoMemberCache->gtp() == rc_gtp)
        &&(!rb_forceClaimedAddress || pc_isoMemberCache->itemState(IState_c::ClaimedAddress))
        );
};

/**
  check if a member with given number exist
  which optional (!!) match the condition of address claim state
  and update local pc_isoMemberCache
  @param rui8_nr searched member number
  @param rb_forceClaimedAddress true -> only members with claimed address are used
        (optional, default false)
  @return true -> item found
*/
bool ISOMonitor_c::existIsoMemberNr(uint8_t rui8_nr, bool rb_forceClaimedAddress)
{
  if (!vec_isoMember.empty() && (pc_isoMemberCache != vec_isoMember.end()))
  {
    if (( pc_isoMemberCache->nr() == rui8_nr )&&(!rb_forceClaimedAddress || pc_isoMemberCache->itemState(IState_c::ClaimedAddress)))  return true;
  }
  uint8_t b_size = vec_isoMember.size();
  uint8_t b_act = 0;
  for (pc_isoMemberCache = vec_isoMember.begin();
       pc_isoMemberCache != vec_isoMember.end();
       pc_isoMemberCache++)
  {
    if (b_act == b_size) break;
    if (pc_isoMemberCache->equalNr(rui8_nr))
    {
      break;
    }
    b_act += 1;
  }
  return ( (pc_isoMemberCache != vec_isoMember.end())
         &&(!rb_forceClaimedAddress || pc_isoMemberCache->itemState(IState_c::ClaimedAddress))
        );
};


/**
  check if member is in member list with wanted GETY_POS,
  adopt POS if member with claimed address with other POS exist
  @param refc_gtp GETY_POS to search (-> it's updated if member with claimed address with other POS is found)
  @return true -> member with claimed address with given GETY found (and refc_gtp has now its GETY_POS)
*/
bool ISOMonitor_c::isoGety2GtpClaimedAddress(GetyPos_c &refc_gtp)
{
    bool b_result = false;
    if (!(existIsoMemberGtp(refc_gtp)))
    { // no item with GETY_POS found -> adapt POS
      // search for member with claimed address with same GETY
      if (isoMemberGetyCnt(refc_gtp.getGety(), true) > 0)
      { // member with claimed address with other POS exist
        refc_gtp = isoMemberGetyInd(refc_gtp.getGety(), 0, true).gtp();
        b_result = true;
      }
    }
    else
    { // member with given GTP exist
      if (isoMemberGtp(refc_gtp).itemState(IState_c::ClaimedAddress)) b_result = true;
    }
    return b_result;
}


/**
  insert a new ISOItem_c in the list; with unset rui8_nr the member is initiated as
  address claim state; otherwise the given state can be given or state Active is used

  possible errors:
    * badAlloc not enough memory to insert new ISOItem_c isntance
    * busy another member with same ident exists already in the list

  @param rc_gtp GETY_POS of the member
  @param rpui8_name ISO11783 64-bit NAME as pointer to 8 uint8_t string
  @param rui8_nr member number
  @param rui16_saEepromAdr EEPROM adress to store actual SA -> next boot with same adr
  @param ren_status wanted status
  @return true -> the ISOItem_c was inserted
*/
bool ISOMonitor_c::insertIsoMember(GetyPos_c rc_gtp, const uint8_t* rpui8_name,
      uint8_t rui8_nr, IState_c::itemState_t ren_state, uint16_t rui16_saEepromAdr)
{
  bool b_result = true;

  // check if another ISOItem_c with same GETY_POS already exist
  if (existIsoMemberGtp(rc_gtp))
  { // another member with same GETY_POS found
    getLbsErrInstance().registerError( LibErr_c::Busy, LibErr_c::LbsSystem );
    b_result = false; // don't insert
  }

  if (b_result)
  { // the insert is allowed
    // prepare temp item with wanted data
    c_tempIsoMemberItem.set(System_c::getTime(), rc_gtp, rui8_nr,
        IState_c::itemState_t(ren_state | IState_c::Member | IState_c::Iso | IState_c::Active),
        rui16_saEepromAdr, rpui8_name, getSingletonVecKey() );

    // now insert element
    const uint8_t b_oldSize = vec_isoMember.size();
    vec_isoMember.push_front(c_tempIsoMemberItem);
    pc_isoMemberCache = vec_isoMember.begin();
    if (vec_isoMember.size() <= b_oldSize)
    { // array didn't grow
      getLbsErrInstance().registerError( LibErr_c::BadAlloc, LibErr_c::LbsSystem );
      b_result = false;
    }
    #ifdef DEBUG_HEAP_USEAGE
    else
    {
      sui16_isoItemTotal++;

      getRs232Instance()
	      << sui16_isoItemTotal << " x ISOItem_c: Mal-Alloc: "
        <<  sizeSlistTWithMalloc( sizeof(ISOItem_c), sui16_isoItemTotal )
        << "/" << sizeSlistTWithMalloc( sizeof(ISOItem_c), 1 )
        << ", Chunk-Alloc: "
        << sizeSlistTWithChunk( sizeof(ISOItem_c), sui16_isoItemTotal )
        << "\r\n\r\n";
    }
    #endif
    vec_isoMember.sort(); // resort the list

  }
  return b_result;
};

/**
  deliver member item with given gtp
  (check with existIsoMemberGtp before access to not defined item)

  possible errors:
    * elNonexistent on failed search

  @param rc_gtp searched GETY_POS
  @return reference to searched ISOItem
  @exception containerElementNonexistant
*/
ISOItem_c& ISOMonitor_c::isoMemberGtp(GetyPos_c rc_gtp)
{
  if (existIsoMemberGtp(rc_gtp))
  { // no error
    return static_cast<ISOItem_c&>(*pc_isoMemberCache);
  }
  else
  { // wanted element not found
    getLbsErrInstance().registerError( LibErr_c::ElNonexistent, LibErr_c::LbsSystem );

    // throw exception by constant -> if no exception configured no command is created
    THROW_CONT_EL_NONEXIST

    //return reference to first element as fallback
    return vec_isoMember.front();
  }
};

/**
  deliver member item with given nr
  (check with existIsoMemberNr before access to not defined item)

  possible errors:
    * elNonexistent on failed search

  @param rui8_nr searched number
  @return reference to searched ISOItem
  @exception containerElementNonexistant
*/
ISOItem_c& ISOMonitor_c::isoMemberNr(uint8_t rui8_nr)
{
  if (existIsoMemberNr(rui8_nr))
  { // no error
    return static_cast<ISOItem_c&>(*pc_isoMemberCache);
  }
  else
  { // wanted element not found
    getLbsErrInstance().registerError( LibErr_c::ElNonexistent, LibErr_c::LbsSystem );

    // throw exception by constant -> if no exception configured no command is created
    THROW_CONT_EL_NONEXIST

    //return reference to first element as fallback
    return vec_isoMember.front();
  }
};

/**
  deliver member item with given GETY_POS, set pointed bool var to true on success
  and set a Member Array Iterator to the result
  @param rc_gtp searched GETY_POS
  @param pb_success bool pointer to store the success (true on success)
  @param pbc_iter optional member array iterator which points to searched ISOItem_c on success
  @return reference to the searched item
*/
ISOItem_c& ISOMonitor_c::isoMemberGtp(GetyPos_c rc_gtp, bool *const pb_success, Vec_ISOIterator *const pbc_iter)
{
  *pb_success = (existIsoMemberGtp(rc_gtp))?true:false;

  if (pbc_iter != NULL)
  {
    *pbc_iter = pc_isoMemberCache;
  }
  return static_cast<ISOItem_c&>(*pc_isoMemberCache);
};

/**
  delete item with specified gtp

  possible errors:
    * elNonexistent no member with given GETY_POS exists

  @param rc_gtp GETY_POS of to be deleted member
*/
bool ISOMonitor_c::deleteIsoMemberGtp(GetyPos_c rc_gtp)
{
  if (existIsoMemberGtp(rc_gtp))
  { // set correct state

    // Are we deleting a WorkingSetMaster?
    if ((*pc_isoMemberCache).getMaster() != NULL) {
      // Tell all WorkingSet-Slaves that they're no more Slaves but only standalones...
      for (Vec_ISO::iterator pc_iter=vec_isoMember.begin(); pc_iter != vec_isoMember.end(); pc_iter++)
      {
        if ((*pc_iter).getMaster() == &(*pc_isoMemberCache)) (*pc_iter).setMaster(NULL);
      }
    }
  
    // erase it from list (existIsoMemberGtp sets pc_isoMemberCache to the wanted item)
    vec_isoMember.erase(pc_isoMemberCache);
    #ifdef DEBUG_HEAP_USEAGE
    sui16_isoItemTotal--;

    getRs232Instance()
      << sui16_isoItemTotal << " x ISOItem_c: Mal-Alloc: "
      <<  sizeSlistTWithMalloc( sizeof(ISOItem_c), sui16_isoItemTotal )
      << "/" << sizeSlistTWithMalloc( sizeof(ISOItem_c), 1 )
      << ", Chunk-Alloc: "
      << sizeSlistTWithChunk( sizeof(ISOItem_c), sui16_isoItemTotal )
      << "\r\n\r\n";
    #endif
    pc_isoMemberCache = vec_isoMember.begin();

    return true;
  }
  else
  { // to be deleted member GETY_POS does not exist
    getLbsErrInstance().registerError( LibErr_c::ElNonexistent, LibErr_c::LbsSystem );
    return false;
  }
};

/**
  delete item with specified member number

  possible errors:
    * elNonexistent no member with given GETY_POS exists

  @param rui8_nr SA of to be deleted member
*/
bool ISOMonitor_c::deleteIsoMemberNr(uint8_t rui8_nr)
{
  if (existIsoMemberNr(rui8_nr))
  { // use deleteIsoMemberGtp
    return deleteIsoMemberGtp(pc_isoMemberCache->gtp());
  }
  else
  { // to be deleted member number does not exist
    getLbsErrInstance().registerError( LibErr_c::ElNonexistent, LibErr_c::LbsSystem );
    return false;
  }
};


/**
  change gtp if actual gtp isn't unique
  (search possible free POS to given GETY)

  possible errors:
    * busy no other POS code leads to unique GETY_POS code

  @param refc_gtp reference to GETY_POS var (is changed directly if needed!!)
  @return true -> referenced GETY_POS is now unique
*/
bool ISOMonitor_c::unifyIsoGtp(GetyPos_c& refc_gtp){
  bool b_result = true;
  GetyPos_c c_tempGtp = refc_gtp;
  if (existIsoMemberGtp(refc_gtp))
  { // gtp exist -> search new with changed POS
    b_result = false; // refc_gtp isn't unique
    // store the pos part of given gtp
    int16_t tempPos = (refc_gtp.getPos()),
        diff = 1;
    for (; diff < 8; diff++)
    {
      if (tempPos + diff < 8)
      {  // (tempPos + diff) would be an allowed POS code
        c_tempGtp.setPos( tempPos + diff );
        if (!(existIsoMemberGtp(c_tempGtp)))
        {  // (tempPos + diff) can't be found in list -> it is unique
          refc_gtp.setPos( tempPos + diff );
          b_result = true;
          break;
        }
      }
      if (tempPos - diff >= 0)
      { // (tempPos - diff) would be an allowed POS code
        c_tempGtp.setPos( tempPos - diff );
        if (!(existIsoMemberGtp(c_tempGtp)))
        {  // (tempPos - diff) can't be found in list -> it is unique
          refc_gtp.setPos( tempPos - diff );
          b_result = true;
          break;
        }
      }
    }
  }
  if (!b_result) getLbsErrInstance().registerError( LibErr_c::Busy, LibErr_c::LbsSystem );
  return b_result;
};
/**
  check if SA of an announcing ISOItem_c is unique and deliver
  another free SA if not yet unique (else deliver its actual
  SA if unique yet)
  @param rpc_isoItem pointer to announcing ISOItem_c
  @return free unique SA (if possible the actual SA of the pointed item)
    (if wanted SA is not free for NOT-self-conf item or if no free SA is available
     254 is answered -> special flag for NACK)
*/
uint8_t ISOMonitor_c::unifyIsoSa(const ISOItem_c* rpc_isoItem)
{
  uint8_t b_wishSa = rpc_isoItem->nr();
  bool b_free = true;

  if ((b_wishSa < 128) && (rpc_isoItem->selfConf() == 1))
  { // false SA adress -> correct
    b_wishSa = 128;
  }

  if (b_wishSa != 254)
  {
    for (Vec_ISOIterator pc_iterItem = vec_isoMember.begin();
          pc_iterItem != vec_isoMember.end(); pc_iterItem++)
    {
      if ((pc_iterItem->nr() == b_wishSa)
        &&(b_wishSa != 254)
        &&(&(*pc_iterItem) != rpc_isoItem)
         )
      { // same SA, rpc_isoItem had special SA wish and actual
        // item is different from calling ISOItem
        b_free = false;
        break;
      }
    }
    if (b_free) return b_wishSa;
  }
  if (!(rpc_isoItem->selfConf()))
  { // wanted fixed SA not free -> signal this with 254
    return 254;
  }
  // now wished SA isn't free or no SA wished
  b_free = true;
  for (b_wishSa = 128; b_wishSa <= 207; b_wishSa++)
  { // try all possible self conf adresses
    for (Vec_ISOIterator pc_iterItem = vec_isoMember.begin();
          pc_iterItem != vec_isoMember.end(); pc_iterItem++)
    {
      if (pc_iterItem->nr() == b_wishSa)
      { // the tried SA is already used by this item
        // -> break this search loop and try another b_wishSa
        b_free = false;
        break;
      }
    }
    if (b_free) break; // break search loop
    else b_free = true; // now next b_wishSa to try
  }
  // if b_free is true -> b_wishSa has a free number
  if (b_free && (b_wishSa != 208))
  { // free SA found
    return b_wishSa;
  }
  else
  { // no free SA found -> return 254 as signal
    return 254;
  }
}

/**
	trigger a request for claimed addreses
	@param rb_force false -> send request only if no request was detected until now
	@return true -> request was sent
	*/
bool ISOMonitor_c::sendRequestForClaimedAddress( bool rb_force )
{ // trigger an initial request for claimed address
	// ( only if no request was detected )
	if ( ( lastIsoSaRequest() != 0 ) && ( ! rb_force ) )
	{ // at least one request was already detected
		return false;
	}
	// now it's needed to send
//  int32_t i32_time = Scheduler_c::getLastTimeEventTrigger();
  int32_t i32_time = System_c::getTime();

//  getRs232Instance() << "_time in sendReq4AdrCl: " << HAL::getTime() <<"_";

	data().setIsoPri(6);
	// PGN is equivalent to definition of PF and DP in this case
	//data().setIsoPgn(REQUEST_PGN_MSG_PGN);
	data().setIsoDp(0);
	data().setIsoPf(234);
	data().setIsoPs(255); // global request
	data().setIsoSa(254); // special flag for "no SA yet"
	// built request data string
	uint8_t pb_requestString[4];
	pb_requestString[0] = (ADRESS_CLAIM_PGN & 0xFF);
	pb_requestString[1] = ((ADRESS_CLAIM_PGN >> 8)& 0xFF);
	pb_requestString[2] = ((ADRESS_CLAIM_PGN >> 16)& 0xFF);
	data().setDataString(pb_requestString, 3);
	// now ISOSystemPkg_c has right data -> send
	getCanInstance4Comm() << data();
	// store adress claim request time
	setLastIsoSaRequest(i32_time);
	return true;
}


/**
  process system msg with informations which are
  important for managing of members
  @return true -> message processed by ISOMonitor_c; false -> process msg by ServiceMonitor
*/
bool ISOMonitor_c::processMsg(){
  bool b_processed = false;
  int32_t i32_reqPgn;

  ISOItem_c *pc_item = NULL,
            *pc_itemMaster = NULL,
            *pc_localItemWithSameSa = NULL;

  // Handle DESTINATION PGNs
  switch ((data().isoPgn() & 0x1FF00))
  {
    case ADRESS_CLAIM_PGN: // adress claim
      // if local item has same SA -> let it process the msg first
      // for suitable reaction
      if (existIsoMemberNr(data().isoSa()))
      { // ISOItem_c with same SA exist
        pc_item = &(isoMemberNr(data().isoSa()));
        if (pc_item->itemState(IState_c::Local))
        { // local item has same SA -> let it process msg
          pc_localItemWithSameSa = pc_item;
        }
      }

      // if no item has same GTP -> insert new item
      if (existIsoMemberGtp(data().gtp()) == false)
      { // create new item from received msg
        // (ISOMonitor_c::insertIsoMember set state bits for member and ISO
        //  additionally)
        insertIsoMember(data().gtp(), data().name(), data().isoSa(),
          IState_c::itemState_t(IState_c::ClaimedAddress));
        if ( isoMemberNr(data().isoSa()).processMsg() ) b_processed = true;
      }
      else
      { // item with same GTP exist -> let it process
        if ( isoMemberGtp(data().gtp()).processMsg() ) b_processed = true;
      }

      // after processing the received msg by item with same GTP
      if (pc_localItemWithSameSa != NULL)
      { // let local item with same SA process msg for suitable reaction
        if ( pc_localItemWithSameSa->processMsg() ) b_processed = true;
      }
      break;
    case REQUEST_PGN_MSG_PGN:   // request for PGN
      i32_reqPgn = (
                    (static_cast<int32_t>(data().operator[](0)))
                  | (static_cast<int32_t>(data().operator[](1)) << 8)
                  | (static_cast<int32_t>(data().operator[](2)) << 16)
                  );
      switch (i32_reqPgn)
      {
        case ADRESS_CLAIM_PGN: // request for adress claim
          // update time of last adress claim request
           b_processed = true;
           setLastIsoSaRequest();
           // don't break because default handling is true for
           // adress claim request, too
        default:
          // if isoPs is 255 let all local item answer
          if (data().isoPs() == 255)
          {
            for (Vec_ISOIterator pc_iterItem = vec_isoMember.begin();
                  pc_iterItem != vec_isoMember.end(); pc_iterItem++)
            { // let pc_iterItem process if local
              if (pc_iterItem->itemState(IState_c::Local))
                return pc_iterItem->processMsg();
            }
          }
          else
          { // check if item with SA == isoPs exist and let it process
            // if local
            if (existIsoMemberNr(data().isoPs()))
            { // check if local
              if (isoMemberNr(data().isoPs()).itemState(IState_c::Local))
                return isoMemberNr(data().isoPs()).processMsg();
            }
          }
        break;
      }
      break;
#ifdef USE_PROCESS
    case PROCESS_DATA_PGN:
      static_cast<__IsoAgLib::CANPkg_c&>(getProcessInstance4Comm().data())
        = static_cast<__IsoAgLib::CANPkg_c&>(data());
      getProcessInstance4Comm().data().string2Flags();
      return getProcessInstance4Comm().processMsg();
#endif
  } // end switch for DESTINATION pgn
  
  
  // Handle NON-DESTINATION PGNs
  switch ((data().isoPgn() /* & 0x1FFFF */ )) // isoPgn is already "& 0x1FFFF" !
  {
    case WORKING_SET_MASTER_PGN: // working set master
      b_processed = true;
      if (existIsoMemberNr(data().isoSa()))
      { // ISOItem_c with same SA has to exist!
        pc_itemMaster = &(isoMemberNr(data().isoSa()));
        pc_itemMaster->setMaster (pc_itemMaster); // set item as mas
        // IGNORE THAT WE GOT x MEMBERS FOR NOW
        // LATER WE HAVE TO BE SURE THAT ALL THOSE x MEMBER DEFINITIONS REALLY ARRIVED!!
      }
      else
      {
        // shouldn't happen!
      }
      break;
    case WORKING_SET_MEMBER_PGN: // working set member
      if (existIsoMemberNr(data().isoSa()))
      { // ISOItem_c with same SA exists (THE SA IS THE MASTER!)
        pc_itemMaster = &(isoMemberNr(data().isoSa()));
        // the working set master places the NAME field of each children
        // in the data part of this message type
        pc_item = &(isoMemberGtp(data().gtp()));
        pc_item->setMaster (pc_itemMaster); // set master on this isoItem
        b_processed = true;
      }
      else
      {
        // shouldn't happen!
      }
      break;
    default:
      break;
  } // end switch for NON-DESTINATION pgn
  
  return b_processed; // return if msg was processed by ISOMonitor_c
}


uint8_t ISOMonitor_c::getSlaveCount (ISOItem_c* rpc_masterItem)
{
  uint8_t slaveCount;
  ISOItem_c* pc_iterMaster;
  // iterate through all items and check if it has the master set to us
  // and is not the master itself!
  slaveCount = 0;
  for(Vec_ISOIterator pc_iter = vec_isoMember.begin(); pc_iter != vec_isoMember.end(); pc_iter++)
  {
    pc_iterMaster = pc_iter->getMaster ();
    if ((& (*pc_iter) != rpc_masterItem) && (pc_iterMaster == rpc_masterItem)) {
      slaveCount++;
    }
  }
  return slaveCount;
}


ISOItem_c* ISOMonitor_c::getSlave (uint8_t index, ISOItem_c* rpc_masterItem)
{
  uint8_t slaveCount;
  ISOItem_c* pc_iterMaster;
  // iterate through all items and check if it the (desired) Xth item
  // if so, break and return it, else return NULL
  slaveCount = 0;
  for(Vec_ISOIterator pc_iter = vec_isoMember.begin(); pc_iter != vec_isoMember.end(); pc_iter++)
  {
    pc_iterMaster = pc_iter->getMaster ();
    if ((& (*pc_iter) != rpc_masterItem) && (pc_iterMaster == rpc_masterItem)) {
      if (slaveCount == index) return & (*pc_iter);
      slaveCount++;
    }
  }
  return NULL;
}



} // end of namespace __IsoAgLib
