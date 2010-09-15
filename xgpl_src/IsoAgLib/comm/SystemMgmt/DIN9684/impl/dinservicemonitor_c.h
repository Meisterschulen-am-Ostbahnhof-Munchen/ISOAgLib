/***************************************************************************
                          dinservicemonitor_c.h  - object for monitoring of
                                              Scheduler_c services like BS, GPS
                             -------------------
    begin                : Fri Apr 07 2000
    copyright            : (C) 2000 - 2004 by Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
    type                 : Header
    $LastChangedDate$
    $LastChangedRevision$
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
#ifndef SERVICE_MONITOR_H
#define SERVICE_MONITOR_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include <IsoAgLib/typedef.h>

#include <IsoAgLib/util/impl/elementbase_c.h>
#include <IsoAgLib/util/impl/singleton.h>
#include <IsoAgLib/util/config.h>
#include "dinserviceitem_c.h"

#if defined(SYSTEM_PC) && !defined(SYSTEM_PC_VC)
  #include <ext/slist>
  namespace std { using __gnu_cxx::slist;};
#else
  #include <slist>
#endif


// Begin Namespace __IsoAgLib
namespace __IsoAgLib {
/**
  this object manages a monitor list of all
  services including inserting and administration of local own services.
  @short Manager for services of Scheduler_c (DINServiceItem_c)
  Derive from ElementBase_c to register in Scheduler_c for timeEvent trigger
  Derive from SINGLETON to create a Singleton which manages one global accessible singleton
   per IsoAgLib instance (if only one IsoAgLib instance is defined in application config, no overhead is produced).
  @see ServiceItem
  @see MemberMonitor
  @author Dipl.-Inform. Achim Spangler
*/
class DINServiceMonitor_c : public SINGLETON_DERIVED(DINServiceMonitor_c, ElementBase_c)
{
private:
  #ifdef OPTIMIZE_HEAPSIZE_IN_FAVOR_OF_SPEED
  typedef STL_NAMESPACE::slist<DINServiceItem_c,STL_NAMESPACE::__malloc_alloc_template<0> > ArrService;
  typedef STL_NAMESPACE::slist<DINServiceItem_c,STL_NAMESPACE::__malloc_alloc_template<0> >::iterator ArrServiceIterator;
  #else
  typedef STL_NAMESPACE::slist<DINServiceItem_c> ArrService;
  typedef STL_NAMESPACE::slist<DINServiceItem_c>::iterator ArrServiceIterator;
  #endif

public:
  /** basic intialisation */
  void init( void );
  /** every subsystem of IsoAgLib has explicit function for controlled shutdown
    */
  void close( void );
  /**
    periodically event
    @return true -> all planned activities performed in allowed time
  */
  bool timeEvent( void );

  /** default destructor of DINServiceMonitor_c which has nothing to do */
  virtual ~DINServiceMonitor_c() {};

  /**
    create a service ident, for which the library
    sends the suitable alive messages

    possible errors:
      * Err_c::badAlloc on not enough memory to create the needed DINServiceItem_c instance
      * Err_c::precondition on invalid DEV_KEY code
      * Err_c::busy another DINServiceItem_c with same DEV_KEY is already in list of active services
    @param rc_devKey DEV_KEY code for the service
    @param rui8_nr corresponding service no
    @return true -> service was created successfully (no equal service exist,
        enough memory to store new entry in ServiceMonitor)
     @exception badAlloc preconditionViolation
  */
  bool createLocalService(const DevKey_c& rc_devKey, uint8_t rui8_nr= 0xFF);

  /**
    create a service ident, for which the library
    sends the suitable alive messages;
    this variant has only different name and is replaced during compile
    time by call to createLocalService; this is only for allowing
    an alternative consistent naming scheme with the other functions of ServiceMonitor

    possible errors:
      * Err_c::badAlloc on not enough memory to create the needed DINServiceItem_c instance
      * Err_c::precondition on invalid DEV_KEY code
      * Err_c::busy another DINServiceItem_c with same DEV_KEY is already in list of active services
    @param rc_devKey DEV_KEY code for the service
    @param rui8_nr corresponding service no
    @return true -> service was created successfully (no equal service exist,
        enough memory to store new entry in ServiceMonitor)
     @exception badAlloc preconditionViolation
  */
  bool createLocalDinService(const DevKey_c& rc_devKey, uint8_t rui8_nr= 0xFF)
    {return createLocalService(rc_devKey, rui8_nr);};

  /**
    delete a local service
    @param rc_devKey DEV_KEY code of the service
    @return true -> wanted item found and deleted
  */
  bool deleteLocalService(const DevKey_c& rc_devKey);

  /**
    delete a local service;
    this variant deletes a DIN service; this is replaced during compile
    time by a direct call to deleteLocalMember, so that no runtime
    overhead is caused
    @param rc_devKey DEV_KEY code of the service
    @return true -> wanted item found and deleted
  */
  bool deleteLocalDinService(const DevKey_c& rc_devKey) {return deleteLocalService(rc_devKey);};

  /**
    check if service with given devKey exist
    @param rc_devKey DEV_KEY code of the searched service
  */
  bool existDinServiceDevKey(const DevKey_c& rc_devKey);

  /**
    deliver service item with given devKey

    possible errors:
      * Err_c::elNonexistent on failed search

    @param rc_devKey DEV_KEY code of the searched ServiceItem
    @return reference to the searched DINServiceItem_c or first item, if search failed
     @exception containerElementNonexistant
  */
  DINServiceItem_c& dinServiceDevKey(const DevKey_c& rc_devKey);

  /**
    check if service with given service number exist
    (use existDinServiceDevKey as devKey and number are equivalent for services)
    @param rui8_nr searched service number
  */
  bool existDinServiceNr(uint8_t rui8_nr){return existDinServiceDevKey(rui8_nr);};

  /**
    deliver service item with given nr
    (use din_serviceDevKey as devKey and service number are equivalent for services)

    possible errors:
      * Err_c::elNonexistent on failed search

    @param rui8_nr searched service number
    @return reference to the searched DINServiceItem_c or first item, if search failed
     @exception containerElementNonexistant
  */
  DINServiceItem_c& dinServiceNr(uint8_t rui8_nr) {return dinServiceDevKey(rui8_nr);};

  /**
    insert service with given devKey and nr

    possible errors:
      * Err_c::badAlloc not enough memory to insert new DINServiceItem_c isntance
      * Err_c::busy another service with same ident exists already in the list

    @param rc_devKey DEV_KEY code of Service
    @return true if new item inserted
  */
  bool insertDinService(const DevKey_c& rc_devKey);

  /**
    delete a service with given DEV_KEY from the list

    possible errors:
      * Err_c::elNonexistent no service with given ident exists

    @param rc_devKey DEV_KEY of the deleted item
    @return true -> an item weas deleted
  */
  bool deleteDinServiceDevKey(const DevKey_c& rc_devKey);
  /**
    delete a service with given service number from the list
    (uses deleteDinServiceDevKey as DEV_KEY and number of services are equal)

    possible errors:
      * Err_c::elNonexistent no service with given ident exists

    @param rc_devKey DEV_KEY of the deleted item
    @return true -> an item weas deleted
  */
   bool deleteDinServiceNr(uint8_t rui8_nr){return deleteDinServiceDevKey(rui8_nr);};
  /**
    process system msg with alive or error state
    msg from a service
    @return true -> message processed by MemberMonitor; false -> process msg by DINServiceMonitor_c
  */
  bool processMsg();
private:
// private methods
  friend class SINGLETON_DERIVED(DINServiceMonitor_c, ElementBase_c);
  /**
    HIDDEN constructor
    NEVER instantiate a variable of type DINServiceMonitor_c within application
    only access DINServiceMonitor_c via getBaseInstance() or getBaseInstance( int riLbsBusNr ) in case more than one ISO11783 or DIN9684 BUS is used for IsoAgLib
  */
  DINServiceMonitor_c( void );

  /**
    initialize directly after the singleton instance is created.
    this is called from singleton.h and should NOT be called from the user again.
    users please use init(...) instead.
  */
  void singletonInit();

private: // Private attributes
  /** temp serviceItem object for better insert of new service */
  DINServiceItem_c c_tempDinServiceItem;
  /** dynamic array of serviceItems */
  ArrService vec_dinService;
  /** cache iterator to speed series of access to same service item */
  ArrServiceIterator pc_dinServiceCache;
};
#if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
  /** C-style function, to get access to the unique DINServiceMonitor_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  DINServiceMonitor_c& getDinServiceMonitorInstance( uint8_t rui8_instance = 0 );
#else
  /** C-style function, to get access to the unique DINServiceMonitor_c singleton instance */
  DINServiceMonitor_c& getDinServiceMonitorInstance( void );
#endif

}
#endif
