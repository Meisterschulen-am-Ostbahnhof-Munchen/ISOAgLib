/***************************************************************************
                          idinservicemonitor_c.h  - object for monitoring of
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
#ifndef ISERVICE_MONITOR_H
#define ISERVICE_MONITOR_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include "impl/dinservicemonitor_c.h"
#include "idinserviceitem_c.h"

// Begin Namespace IsoAgLib
namespace IsoAgLib {
class iDINServiceMonitor_c;
#if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
  /** C-style function, to get access to the unique DINServiceMonitor_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  iDINServiceMonitor_c& getIdinServiceMonitorInstance( uint8_t rui8_instance = 0 )
  { return static_cast<iDINServiceMonitor_c&>(DINServiceMonitor_c::getDinServiceMonitorInstance(rui8_instance));}; 
#else
  /** C-style function, to get access to the unique DINServiceMonitor_c singleton instance */
  iDINServiceMonitor_c& getIdinServiceMonitorInstance( void )
  { return static_cast<iDINServiceMonitor_c&>(DINServiceMonitor_c::getDinServiceMonitorInstance());}; 
#endif
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
class iDINServiceMonitor_c : private __IsoAgLib::DINServiceMonitor_c
{
public:
  /**
    create a service ident, for which the library
    sends the suitable alive messages
  
    possible errors:
      * Err_c::badAlloc on not enough memory to create the needed DINServiceItem_c instance
      * Err_c::precondition on invalid GETY_POS code
      * Err_c::busy another DINServiceItem_c with same GETY_POS is already in list of active services
    @param rc_gtp GETY_POS code for the service
    @param rui8_nr corresponding service no
    @return true -> service was created successfully (no equal service exist, 
        enough memory to store new entry in ServiceMonitor)
     @exception badAlloc preconditionViolation
  */
  bool createLocalService(iGetyPos_c rc_gtp, uint8_t rui8_nr= 0xFF) 
	{ return DINServiceMonitor_c::createLocalService( rc_gtp, rui8_nr );};

  /**
    create a service ident, for which the library
    sends the suitable alive messages;
    this variant has only different name and is replaced during compile
    time by call to createLocalService; this is only for allowing
    an alternative consistent naming scheme with the other functions of ServiceMonitor
  
    possible errors:
      * Err_c::badAlloc on not enough memory to create the needed DINServiceItem_c instance
      * Err_c::precondition on invalid GETY_POS code
      * Err_c::busy another DINServiceItem_c with same GETY_POS is already in list of active services
    @param rc_gtp GETY_POS code for the service
    @param rui8_nr corresponding service no
    @return true -> service was created successfully (no equal service exist, 
        enough memory to store new entry in ServiceMonitor)
     @exception badAlloc preconditionViolation
  */
  bool createLocalDinService(iGetyPos_c rc_gtp, uint8_t rui8_nr= 0xFF)
	{ return DINServiceMonitor_c::createLocalDinService( rc_gtp, rui8_nr );};
  
  /**
    delete a local service
    @param rc_gtp GETY_POS code of the service
    @return true -> wanted item found and deleted
  */  
  bool deleteLocalService(iGetyPos_c rc_gtp) 
	{ return DINServiceMonitor_c::deleteLocalService( rc_gtp);};

  /**
    delete a local service;
    this variant deletes a DIN service; this is replaced during compile
    time by a direct call to deleteLocalMember, so that no runtime
    overhead is caused
    @param rc_gtp GETY_POS code of the service
    @return true -> wanted item found and deleted
  */  
  bool deleteLocalDinService(iGetyPos_c rc_gtp)
	{ return DINServiceMonitor_c::deleteLocalDinService( rc_gtp);};

  /**
    check if service with given gtp exist
    @param rc_gtp GETY_POS code of the searched service
  */
  bool existDinServiceGtp(iGetyPos_c rc_gtp) 
	{ return DINServiceMonitor_c::existDinServiceGtp( rc_gtp);};

  /**
    deliver service item with given gtp

    possible errors:
      * Err_c::elNonexistent on failed search

    @param rc_gtp GETY_POS code of the searched ServiceItem
    @return reference to the searched DINServiceItem_c or first item, if search failed
     @exception containerElementNonexistant
  */
  iDINServiceItem_c& dinServiceGtp(iGetyPos_c rc_gtp) 
	{ return static_cast<iDINServiceMonitor_c&>(DINServiceMonitor_c::dinServiceGtp( rc_gtp));};

  /**
    check if service with given service number exist
    (use existDinServiceGtp as gtp and number are equivalent for services)
    @param rui8_nr searched service number
  */
  bool existDinServiceNr(uint8_t rui8_nr) { return DINServiceMonitor_c::existDinServiceNr(rui8_nr);};

  /**
    deliver service item with given nr
    (use din_serviceGtp as gtp and service number are equivalent for services)

    possible errors:
      * Err_c::elNonexistent on failed search

    @param rui8_nr searched service number
    @return reference to the searched DINServiceItem_c or first item, if search failed
     @exception containerElementNonexistant
  */
  iDINServiceItem_c& dinServiceNr(uint8_t rui8_nr)
	{ return static_cast<iDINServiceMonitor_c&>(DINServiceMonitor_c::dinServiceNr(rui8_nr));};

  /**
    insert service with given gtp and nr

    possible errors:
      * Err_c::badAlloc not enough memory to insert new DINServiceItem_c isntance
      * Err_c::busy another service with same ident exists already in the list

    @param rc_gtp GETY_POS code of Service
    @return true if new item inserted
  */
  bool insertDinService(iGetyPos_c rc_gtp) 
	{ return DINServiceMonitor_c::insertDinService( rc_gtp);};

  /**
    delete a service with given GETY_POS from the list

    possible errors:
      * Err_c::elNonexistent no service with given ident exists

    @param rc_gtp GETY_POS of the deleted item
    @return true -> an item weas deleted
  */
  bool deleteDinServiceGtp(iGetyPos_c rc_gtp) 
	{ return DINServiceMonitor_c::deleteDinServiceGtp( rc_gtp);};
  /**
    delete a service with given service number from the list
    (uses deleteDinServiceGtp as GETY_POS and number of services are equal)

    possible errors:
      * Err_c::elNonexistent no service with given ident exists

    @param rc_gtp GETY_POS of the deleted item
    @return true -> an item weas deleted
  */
   bool deleteDinServiceNr(uint8_t rui8_nr) { return DINServiceMonitor_c::deleteDinServiceNr(rui8_nr);};
private:
  /**
    HIDDEN constructor for a iDINServiceMonitor_c object instance which can optional
    set the configuration for send/receive for base msg type 1,2
    and calendar
    NEVER instantiate a variable of type iDINServiceMonitor_c within application
    only access iDINServiceMonitor_c via getBaseInstance() or getBaseInstance( int riLbsBusNr ) in case more than one ISO11783 or DIN9684 BUS is used for IsoAgLib
    */
  iDINServiceMonitor_c() : DINServiceMonitor_c() {};
};

}
#endif
