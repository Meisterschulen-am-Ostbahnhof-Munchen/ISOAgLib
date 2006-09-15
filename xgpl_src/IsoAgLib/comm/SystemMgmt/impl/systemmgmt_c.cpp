/***************************************************************************
                          systemmgmt_c.cpp - central IsoAgLib system management
                                          object which manages own idents
                                          and delivers information to own
                                          and other services and members
                             -------------------
    begin                : Fri Apr 07 2000
    copyright            : (C) 2000 - 2004 by Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
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
#include "systemmgmt_c.h"

#include <IsoAgLib/driver/system/impl/system_c.h>
#include <IsoAgLib/comm/Scheduler/impl/scheduler_c.h>
#ifdef USE_PROCESS
#include <IsoAgLib/comm/Process/impl/process_c.h>
#endif
#ifdef USE_TRACTOR_GENERAL
#include <IsoAgLib/comm/Base/impl/tracgeneral_c.h>
#endif
#ifdef USE_TRACTOR_MOVE
#include <IsoAgLib/comm/Base/impl/tracmove_c.h>
#endif
#ifdef USE_TRACTOR_PTO
#include <IsoAgLib/comm/Base/impl/tracpto_c.h>
#endif
#ifdef USE_TRACTOR_LIGHT
#include <IsoAgLib/comm/Base/ext/impl/traclight_c.h>
#endif
#ifdef USE_TRACTOR_AUX
#include <IsoAgLib/comm/Base/ext/impl/tracaux_c.h>
#endif
#ifdef USE_TIME_GPS
#include <IsoAgLib/comm/Base/impl/timeposgps_c.h>
#endif
#ifdef DEF_Stream_IMPL
#include <IsoAgLib/comm/Multipacket/impl/multireceive_c.h>
#include <IsoAgLib/comm/Multipacket/impl/multisend_c.h>
#endif

namespace __IsoAgLib {
#if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
  /** C-style function, to get access to the unique SystemMgmt_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  SystemMgmt_c& getSystemMgmtInstance( uint8_t rui8_instance )
  { // if > 1 singleton instance is used, no static reference can be used
    return SystemMgmt_c::instance( rui8_instance );
  };
#else
  /** C-style function, to get access to the unique SystemMgmt_c singleton instance */
  SystemMgmt_c& getSystemMgmtInstance( void )
  {
    static SystemMgmt_c& c_lbsSystem = SystemMgmt_c::instance();
    return c_lbsSystem;
  };
#endif


/**
  default constructor, which can optional set the pointer to the containing
  Scheduler_c object instance
*/
SystemMgmt_c::SystemMgmt_c()
{
}
/**
  default destructor, which initiate sending address release for all own identities
  @see IsoMonitor_c::~IsoMonitor_c
*/
SystemMgmt_c::~SystemMgmt_c(){
  close();
}

/**
  initialize directly after the singleton instance is created.
  this is called from singleton.h and should NOT be called from the user again.
  users please use init(...) instead.
  */
void SystemMgmt_c::singletonInit()
{
  init();

  // NOW INIT ONCE the core singleton classes that correspond to the compile time
  // configured features of the IsoAgLib
  #ifdef USE_PROCESS
  getProcessInstance4Comm().init();
  #endif
  #ifdef USE_TRACTOR_GENERAL
  getTracGeneralInstance4Comm().init(NULL, IsoAgLib::IdentModeImplement);
  #endif
  #ifdef USE_TRACTOR_MOVE
  getTracMoveInstance4Comm().init( NULL, IsoAgLib::IdentModeImplement );
  #endif
  #ifdef USE_TRACTOR_PTO
  getTracPtoInstance4Comm().init( NULL, IsoAgLib::IdentModeImplement );
  #endif
  #ifdef USE_TRACTOR_LIGHT
  getTracLightInstance4Comm().init(NULL, IsoAgLib::IdentModeImplement);
  #endif
  #ifdef USE_TRACTOR_AUX
  getTracAuxInstance4Comm().init(NULL, IsoAgLib::IdentModeImplement);
  #endif
  #ifdef USE_TIME_GPS
  getTimePosGpsInstance4Comm().init(NULL, IsoAgLib::IdentModeImplement);
  #endif
  #ifdef DEF_Stream_IMPL
  getMultiReceiveInstance4Comm().init();
  getMultiSendInstance4Comm().init();
  #endif
}

/**
  initialise element which can't be done during construct
  -> insert FilterBox_c for receiving Scheduler_c SystemMgmt_c manages from CAN_IO

  possible errors:
      * dependant error on memory error in CANIO_c during insert of new FilterBox_c entry
*/
void SystemMgmt_c::init()
{ // clear state of b_alreadyClosed, so that close() is called one time
  clearAlreadyClosed();
  // register in Scheduler_c to get timeEvents
  getSchedulerInstance4Comm().registerClient( this );
  pc_activeLocalMember = NULL;
}

/** every subsystem of IsoAgLib has explicit function for controlled shutdown
  */
void SystemMgmt_c::close( void )
{
  if ( ! checkAlreadyClosed() ) {
    // avoid another call
    setAlreadyClosed();
    while ( c_arrClientC1.size() > 0 )
    {
      (*c_arrClientC1.begin())->close();
    }
    getSchedulerInstance4Comm().unregisterClient( this );
  }
}

/**
  periodically event -> call timeEvent for all  identities and parent objects
  @return true -> all planned activities performed in allowed time
*/
bool SystemMgmt_c::timeEvent( void ){
  for ( std::vector<__IsoAgLib::IdentItem_c*>::iterator pc_iter = c_arrClientC1.begin(); ( pc_iter != c_arrClientC1.end() ); pc_iter++ )
  { // call timeEvent for each registered client -> if timeEvent of item returns false
    // it had to return BEFORE its planned activities were performed (because of the registered end time)
    if ( !(*pc_iter)->timeEvent() ) return false;
  }
  return true;
}

/**
  deliver the count of members in the ISO Monitor-Lists
  with given DEVCLASS (variable POS)
  which optional (!!) match the condition of address claim state
  @param rui8_devClass searched DEVCLASS code
  @param rb_forceClaimedAddress true -> only members with claimed address are used
        (optional, default false)
  @return count of members in Monitor-List with DEVCLASS == rui8_devClass
*/
uint8_t SystemMgmt_c::memberDevClassCnt(uint8_t rui8_devClass, bool rb_forceClaimedAddress)
{
  uint8_t ui8_result = 0;
  ui8_result += getIsoMonitorInstance4Comm().isoMemberDevClassCnt(rui8_devClass, rb_forceClaimedAddress);

  return ui8_result;
}

/**
    deliver one of the members with specific DEVCLASS  in the DIN and/or ISO lists
    which optional (!!) match the condition of address claim state
    check first with memberDevClassCnt if enough members with wanted DEVCLASS and
    optional (!!) property are registered in Monitor-List
    @see memberDevClassCnt

    possible errors:
      * range there exist less than rui8_ind members with DEVCLASS rui8_devClass

  @param rui8_devClass           searched DEVCLASS
  @param rui8_ind                position of the wanted member in the
                                  sublist of member with given DEVCLASS (first item has rui8_ind == 0 !!)
  @param rb_forceClaimedAddress  true -> only members with claimed address are used
                                  (optional, default false)
  @return reference to searched element
  */
MonitorItem_c& SystemMgmt_c::memberDevClassInd(uint8_t rui8_devClass, uint8_t rui8_ind, bool rb_forceClaimedAddress)
{
  return static_cast<MonitorItem_c&>(getIsoMonitorInstance4Comm().isoMemberDevClassInd(rui8_devClass, rui8_ind, rb_forceClaimedAddress));
}

/**
    check for member with given member no
      which optional (!!) match the condition of address claim state;
    search ISOMonitor_c in order defined by ren_searchOrder
    @param rui8_nr member no to search for
    @return true -> one of the devices in the searched Monitor lists has the wanted member no
  */
bool SystemMgmt_c::existMemberNr(uint8_t rui8_nr)
{
  return getIsoMonitorInstance4Comm().existIsoMemberNr(rui8_nr);
}

/**
    check for member with given member devKey which optional (!!) match the condition of address claim state;
    @param rc_devKey              member devKey to search for
    @param rb_forceClaimedAddress true -> only members with claimed address are used
                                  (optional, default false)
    @return true -> one of the devices in the searched Monitor lists has the wanted member devKey
  */
bool SystemMgmt_c::existMemberDevKey(const DevKey_c& rc_devKey, bool rb_forceClaimedAddress)
{
  return getIsoMonitorInstance4Comm().existIsoMemberDevKey(rc_devKey, rb_forceClaimedAddress);
}


/**
    deliver member with given member no;
    (check with existMemberNr before access to not defined item)

    possible errors:
      * elNonexistent on failed search

    @param rui8_nr member no to search for
    @return reference to iMonitorItem_c with wanted number
    @exception containerElementNonexistant
  */
MonitorItem_c& SystemMgmt_c::memberNr(uint8_t rui8_nr)
{
  return static_cast<MonitorItem_c&>(getIsoMonitorInstance4Comm().isoMemberNr(rui8_nr));
}

/**
    check for member with given member devKey;
    (check with existMemberDevKey before access to not defined item)

    possible errors:
      * elNonexistent on failed search

    @param rc_devKey              member devKey to search for
    @param rb_forceClaimedAddress true -> only members with claimed address are used
                                  (optional, default false)
    @return reference to iMonitorItem_c with wanted number
    @exception containerElementNonexistant
  */
MonitorItem_c& SystemMgmt_c::memberDevKey(const DevKey_c& rc_devKey, bool rb_forceClaimedAddress)
{
  return static_cast<MonitorItem_c&>(getIsoMonitorInstance4Comm().isoMemberDevKey(rc_devKey, rb_forceClaimedAddress));
}


/**
    check if member is in one of the member lists for DIN or ISO with wanted DEV_KEY,
    adapt instance if member with claimed address with other device class inst exist
    @param refc_devKey DEV_KEY to search (-> it's updated if member with claimed address with other dev class inst is found)
    @return true -> member with claimed address with given DEVCLASS found (and refc_devKey has now its DEV_KEY)
  */
bool SystemMgmt_c::devClass2devKeyClaimedAddress(DevKey_c &refc_devKey)
{
  return getIsoMonitorInstance4Comm().isoDevClass2DevKeyClaimedAddress(refc_devKey);
}


/**
    deliver the amount of local members which matches the searched proto types
    @return amount of registered local din members
  */
uint8_t SystemMgmt_c::localMemberCnt()
{
  uint8_t b_count = 0;
  for ( pc_searchCacheC1 = c_arrClientC1.begin(); ( pc_searchCacheC1 != c_arrClientC1.end() ); pc_searchCacheC1++ )
  {  // increase reult count if local ident is already registered in MemberList
    if ( existMemberDevKey( (*pc_searchCacheC1)->devKey(), false ) ) b_count++;
  }
  return b_count;
}

/**
    deliver reference to local din member by index
    @see localMemberCnt
    @param rui8_ind index of wanted member (first item == 0)
    @return reference to wanted local member MonitorItem
      (MonitorItem_c is bas class of both ISOItem_c or DINItem_c which serves
      adress, devKey, itemState)
    @return pointer to wanted local din member (NULL if no suitable DINItem_c found)
  */
MonitorItem_c& SystemMgmt_c::localMemberInd(uint8_t rui8_ind)
{
  MonitorItem_c* pc_result = NULL;
  uint8_t b_count = 0;
  for ( pc_searchCacheC1 = c_arrClientC1.begin(); ( pc_searchCacheC1 != c_arrClientC1.end() ); pc_searchCacheC1++ )
  {  // increase reult count if local ident is already registered in MemberList
    if ( existMemberDevKey( (*pc_searchCacheC1)->devKey(), false ) )
    {
      if (b_count == rui8_ind)
      { // wanted item found
        pc_result = &( memberDevKey( (*pc_searchCacheC1)->devKey(), false ) );
        break;
      }
      b_count++;
    }
  } // for
  return *pc_result;
}


/**
    check if one of the own local members is active with claimed address at ISO11783
    @return true -> at least one of the own din identities is active with claimed address at ISO11783
    @see SystemMgmt_c::getActiveLocalMember
  */
bool SystemMgmt_c::existActiveLocalMember()
{
  bool b_result = false; // set default to no success

  // check if actual cache pointer points to active ident
  if ((pc_activeLocalMember == NULL)
   || (!pc_activeLocalMember->itemState(IState_c::ClaimedAddress)))
  { // the actual cache pointer isn't correct -> search new one
    const DevKey_c *pc_useDevKey = NULL;
    MonitorItem_c* pc_monitorItem = NULL;

    for ( pc_searchCacheC1 = c_arrClientC1.begin(); ( pc_searchCacheC1 != c_arrClientC1.end() ); pc_searchCacheC1++ )
    {
      if ((*pc_searchCacheC1)->itemState(IState_c::ClaimedAddress))
      {
        if (existMemberDevKey((*pc_searchCacheC1)->devKey(), true ))
        {
          pc_monitorItem = &(memberDevKey((*pc_searchCacheC1)->devKey(), true));
          if (pc_monitorItem->itemState(IState_c::itemState_t(IState_c::ClaimedAddress | IState_c::Local)))
          {
            pc_useDevKey = &((*pc_searchCacheC1)->devKey());
            break;
          } // end if claimed address and local
        } // end if existMemberDevKey
      } // end if ident_item has claimed address
    } // searching for loop
    if (pc_useDevKey != NULL)
    { // active own identity found
      b_result = true;
      pc_activeLocalMember = pc_monitorItem;
    }
    else
    { // no active ident found -> set cache to NULL
      pc_activeLocalMember = NULL;
    }
  }
  else
  { // cache pointer correct
    b_result = true;
  }
  return b_result;
}

/**
    delivers reference to the first active local member;
    send of ISO11783 system msg demands telling a sender ident member no
    -> using the number of the first active member serves as default (f.e. for requesting other member names)
    can throw an preconditionViolation error, if none of the own identities is active/claimed address yet

    possible errors:
      * lbsSysNoActiveLocalMember on missing own active ident

    @return reference to the MonitorItem_c of the first active local member
      (MonitorItem_c is bas class of both ISOItem_c or DINItem_c which serves
      adress, devKey, itemState)
    @exception preconditionViolation
  */
MonitorItem_c& SystemMgmt_c::getActiveLocalMember()
{
  if  (existActiveLocalMember() )
  { // return reference to the pointed ident element
    return *pc_activeLocalMember;
  }
  else
  { // no active own identity found -> set error state
    getLbsErrInstance().registerError( LibErr_c::LbsSysNoActiveLocalMember, LibErr_c::LbsSystem );

    // throw exception by constant -> if no exception configured no command is created
    THROW_PRECOND_VIOLATION

    //return reference to the first ident in IsoMonitor_c
    return memberDevKey(c_arrClientC1.front()->devKey(), true);
  }
}

/**
    check for own din ident with given member no
    @param rui8_nr member no to search for
    @return true -> one of the own din identities has the wanted member no
  */
bool SystemMgmt_c::existLocalMemberNr(uint8_t rui8_nr)
{
  if (     !c_arrClientC1.empty()
       && ( pc_searchCacheC1 != c_arrClientC1.end() )
       && (*pc_searchCacheC1)->equalNr(rui8_nr )
     )
  {
    return true;
  }
  else
  {
    for (pc_searchCacheC1 = c_arrClientC1.begin();
          pc_searchCacheC1 != c_arrClientC1.end(); pc_searchCacheC1++)
    {
      if ( (*pc_searchCacheC1)->equalNr(rui8_nr) )
        break;
    }
    return (pc_searchCacheC1 != c_arrClientC1.end())?true:false;
  }
}

/**
    check for own din ident with given DEV_KEY
    @param rc_devKey DEV_KEY to search for
    @return true -> one of the own din identities has the wanted DEV_KEY
  */
bool SystemMgmt_c::existLocalMemberDevKey(const DevKey_c& rc_devKey, bool rb_forceClaimedAddress)
{
  if ( (!c_arrClientC1.empty()) && (pc_searchCacheC1 != c_arrClientC1.end()) )
  { // try to use current cache as it points to valid entry
    if ( ((*pc_searchCacheC1)->devKey() == rc_devKey )
      && (!rb_forceClaimedAddress || (*pc_searchCacheC1)->itemState(IState_c::ClaimedAddress))
        )  return true;
  }
  // if last cache is still vald the function is exited -> start new search
  for (pc_searchCacheC1 = c_arrClientC1.begin();
        pc_searchCacheC1 != c_arrClientC1.end(); pc_searchCacheC1++)
  {
    if ( ((*pc_searchCacheC1)->devKey() == rc_devKey )
      && (!rb_forceClaimedAddress || (*pc_searchCacheC1)->itemState(IState_c::ClaimedAddress))
        )  return true;
  }
  // if reaching here -> nothing found
  return false;
}

/**
  * reset the Addres Claim state by:
  * + reset IdentItem::IStat_c to IState_c::PreAddressClaim
  * + remove pointed ISOItem_c and DINItem_c nodes and the respective pointer
  * @return true -> there was an item with given DevKey_c that has been resetted to IState_c::PreAddressClaim
 */
bool SystemMgmt_c::restartAddressClaim( const DevKey_c& rrefc_devKey )
{
  if ( existLocalMemberDevKey( rrefc_devKey, false ) )
  { // there exists a local IdentItem_c with the given DevKey_c
    // -> forward the function call
    (*pc_searchCacheC1)->restartAddressClaim();
    return true;
  }
  else
  { // no local item has same DevKey_c
    return false;
  }
}


} // end namespace __IsoAgLib
