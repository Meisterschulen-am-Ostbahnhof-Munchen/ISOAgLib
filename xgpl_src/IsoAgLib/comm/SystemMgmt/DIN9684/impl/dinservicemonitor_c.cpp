/***************************************************************************
                          dinservicemonitor_c.cpp - object for monitoring of
                                               Scheduler_c services like BS, GPS
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
#include "dinservicemonitor_c.h"
#include <IsoAgLib/comm/Scheduler/impl/scheduler_c.h>
#include <IsoAgLib/util/liberr_c.h>
#include <IsoAgLib/driver/system/impl/system_c.h>
#include "dinmonitor_c.h"

#if defined(DEBUG) || defined(DEBUG_HEAP_USEAGE)
  #include <supplementary_driver/driver/rs232/impl/rs232io_c.h>
  #include <IsoAgLib/util/impl/util_funcs.h>
#endif

#ifdef DEBUG_HEAP_USEAGE
static uint16_t sui16_dinServiceItemTotal = 0;
#endif

namespace __IsoAgLib {
#if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
  /** C-style function, to get access to the unique DINServiceMonitor_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
   DINServiceMonitor_c& getDinServiceMonitorInstance( uint8_t rui8_instance ) { return DINServiceMonitor_c::instance( rui8_instance );};
#else
  /** C-style function, to get access to the unique DINServiceMonitor_c singleton instance */
   DINServiceMonitor_c& getDinServiceMonitorInstance( void ) { return DINServiceMonitor_c::instance();};
#endif

/** basic intialisation */
void DINServiceMonitor_c::init( void ) {
  #ifdef DEBUG_HEAP_USEAGE
  sui16_dinServiceItemTotal -= vec_dinService.size();
  #endif
  vec_dinService.clear();
  pc_dinServiceCache = vec_dinService.end();
  // clear state of b_alreadyClosed, so that close() is called one time
  clearAlreadyClosed();
  // register in Scheduler_c to be triggered fopr timeEvent
  getSchedulerInstance4Comm().registerClient( this );
}

/**
  HIDDEN constructor
  NEVER instantiate a variable of type DINServiceMonitor_c within application
  only access DINServiceMonitor_c via getBaseInstance() or getBaseInstance( int riLbsBusNr ) in case more than one ISO11783 or DIN9684 BUS is used for IsoAgLib
*/
DINServiceMonitor_c::DINServiceMonitor_c( void )
  : vec_dinService() {
  init();
}
/** every subsystem of IsoAgLib has explicit function for controlled shutdown
  */
void DINServiceMonitor_c::close( void ) {
  if ( ! checkAlreadyClosed() ) {
    // avoid another call
    setAlreadyClosed();
    getSchedulerInstance4Comm().unregisterClient( this );
  }
};


/**
  create a service ident, for which the library
  sends the suitable alive messages

  possible errors:
    * badAlloc on not enough memory to create the needed DINServiceItem_c instance
    * precondition on invalid GETY_POS code
    * busy another DINServiceItem_c with same GETY_POS is already in list of active services
  @param rc_gtp GETY_POS code for the service
  @param rui8_nr corresponding service no
  @return true -> service was created successfully (no equal service exist, enough memory to store new entry in DINServiceMonitor_c)
   @exception badAlloc
*/
bool DINServiceMonitor_c::createLocalService(const GetyPos_c& rc_gtp, uint8_t rui8_nr)
{
  bool b_result = false;

  /* *********************************** */
  /* perform some input parameter checks */
  /* *********************************** */

  // ckeck for defined pointers
  if ((rc_gtp.getGety() > 0) && (rui8_nr > 0x7))
  { // set precondition error on wrong service type code
    getLbsErrInstance().registerError( LibErr_c::Precondition, LibErr_c::LbsSystem );
    return false; // return with error information
  }
  uint8_t ui8_nr = (rui8_nr < 0x7)? rui8_nr:(rc_gtp.getPos());

  // check if given GETY_POS isn't already used by another service
  if (existDinServiceGtp(ui8_nr))
  { // error set busy error
    getLbsErrInstance().registerError( LibErr_c::Busy, LibErr_c::LbsSystem );
    return false; // exit function with error state
  }

  // service type code is valid and no equivalent service is active on Scheduler_c BUS

  /* *******************************/
  /* perform insert of service item */
  /* *******************************/
  // no equivalent service already exists -> can be created
  // DINServiceMonitor_c::insertService sets badAlloc if memory problem occurs
  if (insertDinService(rc_gtp))
  { // new service entry successful inserted
    b_result = true;
    dinServiceGtp(rc_gtp).setItemState
      (IState_c::itemState_t(IState_c::Local | IState_c::Active), true);
  }

  return b_result;
}
/**
  delete a local service
  @param rc_gtp getyPos code of the service
  @return true -> wanted item found and deleted
*/
bool DINServiceMonitor_c::deleteLocalService(const GetyPos_c& rc_gtp)
{
  if (existDinServiceGtp(rc_gtp) && dinServiceGtp(rc_gtp).itemState(IState_c::Local))
  { // wanted local serice exist
    deleteDinServiceGtp(rc_gtp);
    return true;
  }
  else
  {
    return false;
  }
}


/**
  check if service with given gtp exist
  @param rc_gtp GETY_POS code of the searched service
*/
bool DINServiceMonitor_c::existDinServiceGtp(const GetyPos_c& rc_gtp)
{
   bool b_result = false;
   if (!vec_dinService.empty())
   {
     if ( pc_dinServiceCache != vec_dinService.end() )
     {
      if (pc_dinServiceCache->gtp() == rc_gtp) return true;
     }
     pc_dinServiceCache = STL_NAMESPACE::find(vec_dinService.begin(), vec_dinService.end(), rc_gtp);
     b_result = ((pc_dinServiceCache != vec_dinService.end())&&(pc_dinServiceCache->gtp() == rc_gtp))?true:false;
   }
   return b_result;
};

/**
  deliver service item with given gtp

  possible errors:
    * elNonexistent on failed search

  @param rc_gtp GETY_POS code of the searched ServiceItem
  @return reference to the DINItem_c of the first active own identity
  @exception containerElementNonexistant
*/
DINServiceItem_c& DINServiceMonitor_c::dinServiceGtp(const GetyPos_c& rc_gtp)
{
  if (existDinServiceGtp(rc_gtp))
  { // return reference to the pointed ident element
    return *pc_dinServiceCache;
  }
  else
  { // no active own identity found -> set error state
    getLbsErrInstance().registerError( LibErr_c::ElNonexistent, LibErr_c::LbsSystem );

    // throw exception by constant -> if no exception configured no command is created
    THROW_CONT_EL_NONEXIST

    //return reference to the first service in the list
    return vec_dinService.front();;
  }
};


/**
  insert service with given gtp and nr

  possible errors:
    * badAlloc not enough memory to insert new DINServiceItem_c isntance
    * busy another service with same ident exists already in the list

  @param rc_gtp GETY_POS code of Service
  @return true if new item inserted
*/
bool DINServiceMonitor_c::insertDinService(const GetyPos_c& rc_gtp){
  bool b_result = false;
  if (!existDinServiceGtp(rc_gtp))
  { // update manage info
    uint8_t b_oldSize = vec_dinService.size();
    c_tempDinServiceItem.set(System_c::getTime(), rc_gtp, rc_gtp.getPos(), IState_c::Active, getDinMonitorInstance4Comm().adrvect(), getSingletonVecKey() );
    c_tempDinServiceItem.setItemState(IState_c::Service);
    vec_dinService.push_front(c_tempDinServiceItem);
    if (vec_dinService.size() > b_oldSize)
    {
      b_result = true;
      pc_dinServiceCache = (vec_dinService.begin())++;
      #ifdef DEBUG_HEAP_USEAGE
      sui16_dinServiceItemTotal++;

      getRs232Instance()
        << sui16_dinServiceItemTotal << " x DINServiceItem_c: Mal-Alloc: "
        <<  sizeSlistTWithMalloc( sizeof(DINServiceItem_c), sui16_dinServiceItemTotal )
        << "/" << sizeSlistTWithMalloc( sizeof(DINServiceItem_c), 1 )
        << ", Chunk-Alloc: "
        << sizeSlistTWithChunk( sizeof(DINServiceItem_c), sui16_dinServiceItemTotal )
        << "\r\n\r\n";
      #endif
    }
    else
    { // array didn't grow -> alloc problem
      getLbsErrInstance().registerError( LibErr_c::BadAlloc, LibErr_c::LbsSystem );
      b_result = false;
    }
  }
  else
  { // same service type already exist
    getLbsErrInstance().registerError( LibErr_c::Busy, LibErr_c::LbsSystem );
    b_result = false;
  }
  return b_result;
}

/**
  delete a service with given GETY_POS from the list

  possible errors:
    * elNonexistent no service with given ident exists

  @param rc_gtp GETY_POS of the deleted item
  @return true -> an item weas deleted
*/
bool DINServiceMonitor_c::deleteDinServiceGtp(const GetyPos_c& rc_gtp)
{
  if (existDinServiceGtp(rc_gtp))
  { // erase it from list with cpServiceCache set to be deleted item (by existDinServiceGtp)
    vec_dinService.erase(pc_dinServiceCache);
    #ifdef DEBUG_HEAP_USEAGE
    sui16_dinServiceItemTotal--;

    getRs232Instance()
      << sui16_dinServiceItemTotal << " x DINServiceItem_c: Mal-Alloc: "
      <<  sizeSlistTWithMalloc( sizeof(DINServiceItem_c), sui16_dinServiceItemTotal )
      << "/" << sizeSlistTWithMalloc( sizeof(DINServiceItem_c), 1 )
      << ", Chunk-Alloc: "
      << sizeSlistTWithChunk( sizeof(DINServiceItem_c), sui16_dinServiceItemTotal )
      << "\r\n\r\n";
    #endif
    // set cache to begin of the list
    pc_dinServiceCache = vec_dinService.begin();
    return true;
  }
  else
  { // error because elelment doesn't exist
    getLbsErrInstance().registerError( LibErr_c::ElNonexistent, LibErr_c::LbsSystem );
    return false;
  }
};

/**
  periodically event
  @param ri32_time optional timestamp (default retreived from SystemMgmt_c)
*/
bool DINServiceMonitor_c::timeEvent( void ){
  if ( Scheduler_c::getAvailableExecTime() == 0 ) return false;

  // call time event for all own identities
  bool b_repeat = true;
  while (b_repeat && (!vec_dinService.empty())) {
    b_repeat = false;
    for (ArrServiceIterator pc_dinServiceIter = vec_dinService.begin();
        pc_dinServiceIter != vec_dinService.end(); pc_dinServiceIter++)
    {
      if (pc_dinServiceIter->itemState(IState_c::Local)) pc_dinServiceIter->timeEvent();
      #if CONFIG_DIN_SERVICE_MAX_AGE > 0
      else
      { // clean every 3sec. inactive remote service items from list// clean every 3sec. inactive member items from list
        if (pc_dinServiceIter->checkTime(CONFIG_DIN_SERVICE_MAX_AGE))
        {
          ArrServiceIterator pc_iterDelete = pc_dinServiceIter;
          vec_dinService.erase(pc_iterDelete);
          #ifdef DEBUG_HEAP_USEAGE
          sui16_dinServiceItemTotal--;

          getRs232Instance()
            << sui16_dinServiceItemTotal << " x DINServiceItem_c: Mal-Alloc: "
            <<  sizeSlistTWithMalloc( sizeof(DINServiceItem_c), sui16_dinServiceItemTotal )
            << "/" << sizeSlistTWithMalloc( sizeof(DINServiceItem_c), 1 )
            << ", Chunk-Alloc: "
            << sizeSlistTWithChunk( sizeof(DINServiceItem_c), sui16_dinServiceItemTotal )
            << "\r\n\r\n";
          #endif
          b_repeat = true;
          break;
// That was the old "hacked" version
//          return true;          // return from this function, as the iterator might get problem after erase
        }
      }
      #endif
    }
  }
  return true;
};


/**
  process system msg with alive or error state
  msg from a service
  @return true -> message processed by MemberMonitor; false -> process msg by DINServiceMonitor_c
*/
bool DINServiceMonitor_c::processMsg(){
  bool b_result = false;
  // get pointer to the containing DINSystemPkg_c instance of the SystemMgmt_c instance
  DINSystemPkg_c& c_pkg = getDinMonitorInstance4Comm().data();

  // decide what to do dependent on system msg type
  switch (c_pkg.verw())
  {
    case 4: case 7: // service msg: service alive or service error state information
      if (!existDinServiceGtp(c_pkg.gtp()))
      { // insert service item
        insertDinService(c_pkg.gtp());
      }
      // let suitable DINServiceItem_c process the msg
      dinServiceGtp(c_pkg.gtp()).processMsg();
      b_result = true;
      break;
  }
  return b_result;
}

} // end of namespace __IsoAgLib
