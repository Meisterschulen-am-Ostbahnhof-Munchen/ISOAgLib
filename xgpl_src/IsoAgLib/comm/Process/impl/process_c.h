/***************************************************************************
                          process_c.h  - central managing instance for
                                           all process data informations in
                                           the system
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
#ifndef PROCESS_H
#define PROCESS_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include <functional>

#include <IsoAgLib/typedef.h>
#include <IsoAgLib/util/config.h>
#include <IsoAgLib/util/impl/cancustomer_c.h>
#include <IsoAgLib/util/impl/singleton.h>
#include <IsoAgLib/util/impl/elementbase_c.h>
#include "../Local/impl/procdatalocalbase_c.h"
#include "../Remote/impl/procdataremotebase_c.h"
#include "processpkg_c.h"

#if defined(SYSTEM_PC) && !defined(SYSTEM_PC_VC) && !defined(SYSTEM_A1)
  #include <ext/slist>
  namespace std { using __gnu_cxx::slist;};
#else
  #include <slist>
#endif

namespace IsoAgLib { class iProcess_c;}

// Begin Namespace IsoAgLib
namespace __IsoAgLib {
class Process_c;
typedef SINGLETON_DERIVED_CLIENT2(Process_c, ElementBase_c, ProcDataLocalBase_c, ProcIdent_c, ProcDataRemoteBase_c, ProcIdent_c ) SingletonProcess_c;

/**
  Central managing instance for all process data
  informations in the system

  <b>Basic Rules for Matching of Received Messages)</b>
  1) primary match with LIS, GETY, ZAEHLNUM, WERT, INST
  2) distinguishing of more hits by POS
  \n
  <b>Acceptance Guidlines for Received Messages)</b>
  1) EMPF must fit to local member, SEND must be of existing member with claimed address
  2) special addition for Remote Process Data:
       alternatively SEND fit to owner of some Remote Data used to
       detect setpoints sent by owner of Remote Process Data
       -> possible to detect if local commander gets master or not
       (measure values are ignored by rule 2 unless MeasureProgRemote_c::receiveForeignMeasurement()
        was called for this ProcessDataRemote instance)
  \n
  The most functions of (i)Process_c are only relevant for the internal
  implementation, so that the interface to the API is quite small.
  Some of the internal tasks are:
  - handle list of pointers to all process data instances in the application
  - distribute the periodic timeEvent() call to all local process data instances,
    so that %e.g. value can be sent for measurement programs
  - forward incoming CAN messages to the appropriate process data to update values
    or to register measure progs, setpoints, etc.

  An overall description of Process Data management in IsoAgLib can be found in
  \ref ProcDataPage .

  @author Dipl.-Inform. Achim Spangler
*/
class Process_c : public SingletonProcess_c
{
public:
  /** initialisation for Process_c
  */
  void init( void );
  /** every subsystem of IsoAgLib has explicit function for controlled shutdown
    */
  void close( void );

  /** default destructor which has nothing to do */
  virtual ~Process_c();

  /**
    deliver reference to data pkg
    @return reference to ProcessPkg_c which handles CAN I/O of process data
  */
  ProcessPkg_c& data(){return c_data;};
  /**
    deliver reference to data pkg as reference to CANPkgExt_c
    to implement the base virtual function correct
  */
  virtual CANPkgExt_c& dataBase();

  /**
    start processing of a process msg
    ignore all invalid messages where SEND is not of a member with claimed address,
    or where EMPF isn't valid

    possible errors:
      * Err_c::elNonexistent on SEND/EMPF not registered in Monitor-List
  */
  bool processMsg();

	/**
  	if the amount of created local process data is known, then enough capacity for the
    vector with pointers to all of them can be reserved. Otherwise the vector
    will increase with several reallocations, where each reallocation triggers
    increase of capacity by factor 2 ( capacity is the amount of elements,
    which can be stored before reallocation takes place ).
    @param rui16_localProcCapacity
  */
  void localProcDataReserveCnt( uint16_t rui16_localProcCapacity );
	/**
  	if the amount of created remote process data is known, then enough capacity for the
    vector with pointers to all of them can be reserved. Otherwise the vector
    will increase with several reallocations, where each reallocation triggers
    increase of capacity by factor 2 ( capacity is the amount of elements,
    which can be stored before reallocation takes place ).
    @param rui16_remoteProcCapacity
  */
  void remoteProcDataReserveCnt( uint16_t rui16_remoteProcCapacity );

  /**
    checks if a suitable ProcDataLocalBase_c item exist
    @param rui8_lis LIS code of searched local Process Data instance
    @param rui8_gety GETY code of searched local Process Data instance
    @param rui8_wert WERT code of searched local Process Data instance
    @param rui8_inst INST code of searched local Process Data instance
    @param rui8_zaehlnum ZAEHLNUM  code of searched local Process Data instance
    @param rui8_pos optional POS code of searched local Process Data instance
                  (only important if more GETY type members are active)
    @param rui8_pri PRI code of messages with this process data instance (default 2)
    @return true -> suitable instance found
  */
  bool existProcDataLocal(uint8_t rui8_lis, uint8_t rui8_gety, uint8_t rui8_wert, uint8_t rui8_inst,
          uint8_t rui8_zaehlnum, uint8_t rui8_pos = 0xFF, uint8_t rui8_pri = 2);
  
#ifdef ISO_TASK_CONTROLLER
  bool existProcDataRemote(uint8_t rui8_gety, uint16_t rui16_DDI, uint16_t rui16_element, uint8_t rui8_pos = 0xFF, uint8_t rui8_pri = 2);
#else
  /**
    checks if a suitable ProcDataRemoteBase_c item exist
    @param rui8_lis LIS code of searched remote Process Data instance
    @param rui8_gety GETY code of searched remote Process Data instance
    @param rui8_wert WERT code of searched remote Process Data instance
    @param rui8_inst INST code of searched remote Process Data instance
    @param rui8_zaehlnum ZAEHLNUM  code of searched remote Process Data instance
    @param rui8_pos optional POS code of searched remote Process Data instance
                  (only important if more GETY type members are active)
    @param rui8_pri PRI code of messages with this process data instance (default 2)
    @return true -> suitable instance found
  */
  bool existProcDataRemote(uint8_t rui8_lis, uint8_t rui8_gety, uint8_t rui8_wert, uint8_t rui8_inst,
          uint8_t rui8_zaehlnum, uint8_t rui8_pos = 0xFF, uint8_t rui8_pri = 2);
#endif

  
  /**
    search for suitable ProcDataLocalBase_c item; create on if not found AND if wanted

    possible errors:
        * Err_c::badAlloc not enough memory to add new ProcDataLocal_c
          (can cause badAlloc exception)
        * Err_c::elNonexistent if element not found and rb_doCreate == false
    @param rui8_lis LIS code of searched local Process Data instance
    @param rui8_gety GETY code of searched local Process Data instance
    @param rui8_wert WERT code of searched local Process Data instance
    @param rui8_inst INST code of searched local Process Data instance
    @param rui8_zaehlnum ZAEHLNUM  code of searched local Process Data instance
    @param rui8_pos POS code of searched local Process Data instance
    @param rui8_pri PRI code of messages with this process data instance (default 2)
    @return reference to searched/created ProcDataLocalBase_c instance
  */
  ProcDataLocalBase_c& procDataLocal(uint8_t rui8_lis, uint8_t rui8_gety, uint8_t rui8_wert,
      uint8_t rui8_inst, uint8_t rui8_zaehlnum, uint8_t rui8_pos = 0xFF, uint8_t rui8_pri = 2 );

  /**
    search for suitable ProcDataRemoteBase_c item; create on if not found AND if wanted

    possible errors:
        * Err_c::badAlloc not enough memory to add new ProcDataRemote_c
          (can cause badAlloc exception)
        * Err_c::elNonexistent if element not found and rb_doCreate == false
    @param rui8_lis LIS code of searched remote Process Data instance
    @param rui8_gety GETY code of searched remote Process Data instance
    @param rui8_wert WERT code of searched remote Process Data instance
    @param rui8_inst INST code of searched remote Process Data instance
    @param rui8_zaehlnum ZAEHLNUM  code of searched remote Process Data instance
    @param rui8_pos POS code of searched remote Process Data instance
    @param rui8_pri PRI code of messages with this process data instance (default 2)
    @return reference to searched/created ProcDataRemoteBase_c instance
    @exception badAlloc
  */
#ifdef ISO_TASK_CONTROLLER
ProcDataRemoteBase_c& Process_c::procDataRemote(uint8_t rui8_gety, uint16_t rui16_DDI, uint8_t rui16_element, uint8_t rui8_pos = 0xFF, uint8_t rui8_pri = 2);
#else
  ProcDataRemoteBase_c& procDataRemote(uint8_t rui8_lis, uint8_t rui8_gety, uint8_t rui8_wert,
        uint8_t rui8_inst, uint8_t rui8_zaehlnum, uint8_t rui8_pos = 0xFF, uint8_t rui8_pri = 2 );
#endif

  /**
    delivers count of local process data entries with similar ident
    (which differs only in POS of owner)
    @param rui8_lis LIS code of searched local Process Data instance
    @param rui8_gety GETY code of searched local Process Data instance
    @param rui8_wert WERT code of searched local Process Data instance
    @param rui8_inst INST code of searched local Process Data instance
    @param rui8_zaehlnum ZAEHLNUM  code of searched local Process Data instance
    @param rui8_pri PRI code of messages with this process data instance (default 2)
    @return count of similar local process data entries
  */
  uint8_t procDataLocalCnt(uint8_t rui8_lis, uint8_t rui8_gety, uint8_t rui8_wert, uint8_t rui8_inst,
                           uint8_t rui8_zaehlnum, uint8_t rui8_pri = 2);
  /**
    delivers count of remote process data entries with similar ident
    (which differs only in POS of owner)
    @param rui8_lis LIS code of searched remote Process Data instance
    @param rui8_gety GETY code of searched remote Process Data instance
    @param rui8_wert WERT code of searched remote Process Data instance
    @param rui8_inst INST code of searched remote Process Data instance
    @param rui8_zaehlnum ZAEHLNUM  code of searched remote Process Data instance
    @param rui8_pri PRI code of messages with this process data instance (default 2)
    @return count of similar remote process data entries
  */
  uint8_t procDataRemoteCnt(uint8_t rui8_lis, uint8_t rui8_gety, uint8_t rui8_wert, uint8_t rui8_inst,
                            uint8_t rui8_zaehlnum, uint8_t rui8_pri = 2);

  /**
    performs periodically actions

    @return true -> all planned activities performed in allowed time
  */
  bool timeEvent( void );
  /** handler function for access to undefined client.
    * the base Singleton calls this function, if it detects an error
    */
  void registerAccessFlt( void );
  /** register pointer to a new local process data instance
    * this function is called within construction of new local process data instance
    */
  bool registerLocalProcessData( ProcDataLocalBase_c* pc_localClient)
    { return registerC1( pc_localClient );};
  /** unregister pointer to a already registered local process data instance
    * this function is called within destruction of local process data instance
    */
  void unregisterLocalProcessData( ProcDataLocalBase_c* pc_localClient)
    { unregisterC1( pc_localClient );};
  /** register pointer to a new remote process data instance
    * this function is called within construction of new remote process data instance
    */
  bool registerRemoteProcessData( ProcDataRemoteBase_c* pc_remoteClient)
    { return registerC2( pc_remoteClient );};
  /** unregister pointer to a already registered remote process data instance
    * this function is called within destruction of remote process data instance
    */
  void unregisterRemoteProcessData( ProcDataRemoteBase_c* pc_remoteClient)
    { unregisterC2( pc_remoteClient );};

#ifdef ISO_TASK_CONTROLLER
   void setTaskStatus(uint8_t taskStatus);
#endif
private: // Private methods
  /**
    update the cache with search for according ProcDataLocalBase_c item
    @param rui8_lis LIS code of created local Process Data instance
    @param rui8_gety GETY code of created local Process Data instance
    @param rui8_wert WERT code of created local Process Data instance
    @param rui8_inst INST code of created local Process Data instance
    @param rui8_zaehlnum ZAEHLNUM  code of created local Process Data instance
    @param rui8_pos optinal POS code of created local Process Data instance
      (default not used for search)
    @param rui8_pri PRI code of messages with this process data instance (default 2)
  */
  bool updateLocalCache(uint8_t rui8_lis, uint8_t rui8_gety, uint8_t rui8_wert, uint8_t rui8_inst,
                          uint8_t rui8_zaehlnum, uint8_t rui8_pos = 0xFF, uint8_t rui8_pri = 2);
  /**
    update the cache with search for according ProcDataRemoteBase_c item
    @param rui8_lis LIS code of created remote Process Data instance
    @param rui8_gety GETY code of created remote Process Data instance
    @param rui8_wert WERT code of created remote Process Data instance
    @param rui8_inst INST code of created remote Process Data instance
    @param rui8_zaehlnum ZAEHLNUM  code of created remote Process Data instance
    @param rui8_pos POS code of created remote Process Data instance
      (default not used for search)
    @param rui8_pri PRI code of messages with this process data instance (default 2)
  */
#ifdef ISO_TASK_CONTROLLER
  bool updateRemoteCache(uint8_t rui8_gety, uint16_t rui16_DDI, uint16_t rui16_element, uint8_t rui8_pos, uint8_t rui8_pri);
#else
  bool updateRemoteCache(uint8_t rui8_lis, uint8_t rui8_gety, uint8_t rui8_wert, uint8_t rui8_inst,
                           uint8_t rui8_zaehlnum, uint8_t rui8_pos = 0xFF, uint8_t rui8_pri = 2);
#endif

  /**
    insert FilterBox_c for receive from remote gtp if needed
    @param rc_ownerGtp GTP code of remote owner who sent the message
    @param rui8_pri PRI code of messages with this process data instance (default 2)
    @return true -> member exist and Filter Box created
  */
  bool createRemoteFilter(GetyPos_c rc_ownerGtp, uint8_t rui8_pri = 2);
  /**
    delete FilterBox_c for receive from remote gtp if needed
    (important to delete old Filter Boxes after deletion of
     ProcDataRemoteBase_c - called from within deleteProcDataRemote
     - only delete FilterBox_c if NO other remote proc data with same
     ownerGtp exist)
    @param rc_ownerGtp GTP code of remote owner who sent the message
    @param rui8_pri PRI code of messages with this process data instance (default 2)
    @return true -> member exist and Filter Box deleted
  */
  bool deleteRemoteFilter(GetyPos_c rc_ownerGtp, uint8_t rui8_pri = 2);


private: // Private attributes
  friend class SINGLETON_DERIVED(Process_c,ElementBase_c);
  friend class IsoAgLib::iProcess_c;
  /**
    HIDDEN constructor for a Process_c object instance
    NEVER instantiate a variable of type Process_c within application
    only access Process_c via getProcessInstance() or getProcessInstance( int riLbsBusNr ) in case more than one ISO11783 or DIN9684 BUS is used for IsoAgLib
    */
  Process_c() { init(); };

  /** msg object for CAN I/O */
  ProcessPkg_c c_data;
  /** last timestamp with FilterBox_c check */
  int32_t i32_lastFilterBoxTime;

#ifdef ISO_TASK_CONTROLLER
  int32_t i32_lastTaskStatusTime;
  uint8_t ui8_runningTaskWithSa;
  uint8_t ui8_taskStatus;
#endif
};
#if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
  /** C-style function, to get access to the unique Process_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  Process_c& getProcessInstance( uint8_t rui8_instance = 0 );
#else
  /** C-style function, to get access to the unique Process_c singleton instance */
  Process_c& getProcessInstance( void );
#endif

}
#endif
