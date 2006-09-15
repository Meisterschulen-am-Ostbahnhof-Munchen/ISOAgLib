/***************************************************************************
                          systemmgmt_c.h  - central IsoAgLib system management
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
#ifndef SYSTEM_MGMT_H
#define SYSTEM_MGMT_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include <IsoAgLib/typedef.h>
#include <IsoAgLib/util/impl/singleton.h>
#include "../ISO11783/impl/isomonitor_c.h"

#include "identitem_c.h"
#include "monitoritem_c.h"


// Begin Namespace __IsoAgLib
namespace __IsoAgLib {
class SystemMgmt_c;
typedef SINGLETON_DERIVED_CLIENT1(SystemMgmt_c, ElementBase_c, IdentItem_c, DevKey_c) SingletonSystemMgmt_c;
/**
  Central IsoAgLib system management object, which provides information retrieval
  from devices independen of their protocol type. This is achieved by requesting
  both monitor lists in ISOMonitor ( if activated in protocol ).
  As long as the protocol type of the searched member is known and is constant, the
  corresponding monitor list should be accessed instead of this global interface class.
  *@author Dipl.-Inform. Achim Spangler
  */
class SystemMgmt_c : public SingletonSystemMgmt_c  {
public:

  /**
    default destructor, which initiate sending address release for all own identities
    @see MemberMonitor::~MemberMonitor
  */
  virtual ~SystemMgmt_c();

  /**
    initialise element which can't be done during construct
    -> insert FilterBox_c for receiving Scheduler_c SystemMgmt_c manages from CAN_IO

    possible errors:
      * dependant error on memory error in CANIO_c during insert of new FilterBox_c entry
  */
  void init();

  /** every subsystem of IsoAgLib has explicit function for controlled shutdown
    */
  void close( void );

  /**
    periodically event -> call timeEvent for all  identities and parent objects
    @return true -> all planned activities performed in allowed time
  */
  bool timeEvent( void );

  /* ********************************************************** */
  /** \name Search and access of local + remote + ISO
    * Search for monitor list entries for local and remote
    * ISO. Search keys are DEV_KEY and source number.                                                   */
  /* ********************************************************** */
  /*\@{*/

  /**
    deliver amount of ISO members in monitor lists which optional (!!)
    match the condition of address claim state
    @param rb_forceClaimedAddress true -> only members with claimed address are used
          (optional, default false)
    @return amount of members with claimed address
  */
  uint8_t memberCnt(bool rb_forceClaimedAddress = false)
    {return memberDevClassCnt(0xFF, rb_forceClaimedAddress);}

  /**
      deliver the n'th ISO member in monitor list which optional (!!)
      match the condition of address claim state
      check first with memberCnt if enough members are registered
      in the Monitor-List's
      @see memberCnt

      possible errors:
        * Err_c::range there exist less than rui8_ind members with claimed address

      @param rui8_ind               position of the wanted member in the
                                    sublist of members (first item has rui8_ind == 0 !!)
                                    with the wanted property
      @param rb_forceClaimedAddress true -> only members with claimed address are used
                                    (optional, default false)
      @return reference to searched element
    */
  MonitorItem_c& memberInd(uint8_t rui8_ind, bool rb_forceClaimedAddress = false)
  { return *( (MonitorItem_c*)&memberDevClassInd(0xFF, rui8_ind, rb_forceClaimedAddress) ); }

  /**
    deliver the count of members in the ISO Monitor-Lists
    with given DEVCLASS (variable POS)
    which optional (!!) match the condition of address claim state
    @param rui8_devClass          searched DEVCLASS code
    @param rb_forceClaimedAddress true -> only members with claimed address are used
                                  (optional, default false)
    @return count of members in Monitor-List with DEVCLASS == rui8_devClass
  */
  #if ( ! defined( PRT_INSTANCE_CNT ) ) || ( PRT_INSTANCE_CNT < 2 )
  static
  #endif
  uint8_t memberDevClassCnt(uint8_t rui8_devClass, bool rb_forceClaimedAddress = false);

  /**
      deliver one of the members with specific DEVCLASS  in the ISO lists
      which optional (!!) match the condition of address claim state
      check first with memberDevClassCnt if enough members with wanted DEVCLASS and
      optional (!!) property are registered in Monitor-List
      @see memberDevClassCnt

      possible errors:
        * Err_c::range there exist less than rui8_ind members with DEVCLASS rui8_devClass

      @param rui8_devClass          searched DEVCLASS
      @param rui8_ind               position of the wanted member in the
                                    sublist of member with given DEVCLASS (first item has rui8_ind == 0 !!)
      @param rb_forceClaimedAddress true -> only members with claimed address are used
                                    (optional, default false)
      @return reference to searched element
    */
  #if ( ! defined( PRT_INSTANCE_CNT ) ) || ( PRT_INSTANCE_CNT < 2 )
  static
  #endif
  MonitorItem_c& memberDevClassInd( uint8_t rui8_devClass, uint8_t rui8_ind, bool rb_forceClaimedAddress = false );

  /**
      check for member with given member no which optional (!!) match the condition of address claim state;
      @param rui8_nr  member no to search for
      @return true -> one of the devices in the searched Monitor lists has the wanted member no
    */
  #if ( ! defined( PRT_INSTANCE_CNT ) ) || ( PRT_INSTANCE_CNT < 2 )
  static
  #endif
  bool existMemberNr(uint8_t rui8_nr);

  /**
      check for member with given member devKey which optional (!!) match the condition of address claim state;
      @param rc_devKey              member devKey to search for
      @param rb_forceClaimedAddress true -> only members with claimed address are used
                                    (optional, default false)
      @return true -> one of the devices in the searched Monitor lists has the wanted member devKey
    */
  #if ( ! defined( PRT_INSTANCE_CNT ) ) || ( PRT_INSTANCE_CNT < 2 )
  static
  #endif
  bool existMemberDevKey( const DevKey_c& rc_devKey, bool rb_forceClaimedAddress = false );

  /**
      deliver member with given member no;
      search MemberMonitor and ISOMonitor in order defined by ren_searchOrder
      (check with existMemberNr before access to not defined item)

      possible errors:
        * Err_c::elNonexistent on failed search

      @param rui8_nr member no to search for
      @return reference to iMonitorItem_c with wanted number
      @exception containerElementNonexistant
    */
  #if ( ! defined( PRT_INSTANCE_CNT ) ) || ( PRT_INSTANCE_CNT < 2 )
  static
  #endif
  MonitorItem_c& memberNr( uint8_t rui8_nr );

  /**
      check for member with given member devKey;
      search MemberMonitor and ISOMonitor in order defined by ren_searchOrder
      (check with existMemberDevKey before access to not defined item)

      possible errors:
        * Err_c::elNonexistent on failed search

      @param rc_devKey               member devKey to search fot
      @param rb_forceClaimedAddress  true -> only members with claimed address are used
                                    (optional, default false)
      @return reference to iMonitorItem_c with wanted number
      @exception containerElementNonexistant
    */
  #if ( ! defined( PRT_INSTANCE_CNT ) ) || ( PRT_INSTANCE_CNT < 2 )
  static
  #endif
  MonitorItem_c& memberDevKey( const DevKey_c& rc_devKey, bool rb_forceClaimedAddress = false );

  /**
      check if member is in one of the member lists for ISO with wanted DEV_KEY,
      adapt instance if member with claimed address with other device class inst exist
      @param refc_devKey DEV_KEY to search (-> it's updated if member with claimed address with other dev class inst is found)
      @return true -> member with claimed address with given DEVCLASS found (and refc_devKey has now its DEV_KEY)
    */
  #if ( ! defined( PRT_INSTANCE_CNT ) ) || ( PRT_INSTANCE_CNT < 2 )
  static
  #endif
  bool devClass2devKeyClaimedAddress( DevKey_c &refc_devKey );
  /*\@}*/

  /* ********************************************************** */
  /** \name Search and access local devices
    * Search monitor list selective for local ISO
    * devices provide access to the according list items.       */
  /* ********************************************************** */
  /*\@{*/


  /**
      deliver the amount of local members which matches the searched proto types
      @return amount of registered local members with wanted type
    */
  uint8_t localMemberCnt();

  /**
      deliver pointer to local member of wanted type by index
      @see localMemberCnt
      @param rui8_ind index of wanted member (first item == 0)
      @return pointer to wanted local member (NULL if no suitable MonitorItem_c found)
        (MonitorItem_c is base class of ISOItem_c which serves address, devKey, itemState)
    */
  MonitorItem_c& localMemberInd( uint8_t rui8_ind );

  /**
      deliver the amount of local ISO members
      @return amount of registered local ISO members
    */
  uint8_t localIsoMemberCnt()
    {return localMemberCnt( );}

  /**
      deliver pointer to local ISO member by index
      @see localIsoMemberCnt
      @param rui8_ind index of wanted member (first item == 0)
      @return pointer to wanted local ISO member (NULL if no suitable ISOItem_c found)
    */
  ISOItem_c& localIsoMemberInd(uint8_t rui8_ind)
    { return static_cast<ISOItem_c&>( localMemberInd( rui8_ind ) ); }

  /**
      check if one of the own local members is active with claimed address at ISO11783
      @return true -> at least one of the own iso identities is active with claimed address at ISO11783
      @see SystemMgmt_c::getActiveLocalMember
    */
  bool existActiveLocalMember();

  /**
      delivers reference to the first active local member;
      send of ISO11783 system msg demands telling a sender ident member no
      -> using the number of the first active member serves as default (f.e. for requesting other member names)
      can throw an preconditionViolation error, if none of the own identities is active/claimed address yet

      possible errors:
        * Err_c::lbsSysNoActiveLocalMember on missing own active ident

      @return reference to the MonitorItem_c of the first active local member
        (MonitorItem_c is baes class of ISOItem_c which serves
        adress, devKey, itemState)
      @exception preconditionViolation
    */
  MonitorItem_c& getActiveLocalMember();

  /**
      check if one of the own local ISO members is active with claimed address at ISO11783;
      this variant is replaced during compile time by direct call to existActiveLocalMember,
      so tha no runtime overhead is caused;
      this allows consistent naming with other ISO member oriented functions
      @return true -> at least one of the own iso identities is active with claimed address at ISO11783
      @see SystemMgmt_c::getActiveLocalIsoMember
    */
  bool existActiveLocalIsoMember()
    {return existActiveLocalMember();}

  /**
      delivers reference to the first active local ISO member;
      -> using the number of the first active member serves as default
      can throw an preconditionViolation error, if none of the own ISO identities is active/claimed address yet

      possible errors:
        * Err_c::lbsSysNoActiveLocalMember on missing own active ident

      @return reference to the IsoItem_c of the first active local member
      @exception preconditionViolation
    */
  ISOItem_c& getActiveLocalIsoMember()
    {return static_cast<ISOItem_c&>(getActiveLocalMember());}

  /**
      check for own ident with given member no
      @param rui8_nr member no to search for
      @return true -> one of the own identities has the wanted member no
    */
  bool existLocalMemberNr(uint8_t rui8_nr);

  /**
      check for own ident with given DEV_KEY
      @param rc_devKey              DEV_KEY to search for
      @param rb_forceClaimedAddress true -> only members with claimed address are used
                                    (optional, default false)
      @return true -> one of the own identities has the wanted DEV_KEY
    */
  bool existLocalMemberDevKey(const DevKey_c& rc_devKey, bool rb_forceClaimedAddress = false);

  /**
      check for own iso ident with given member no;
      this variant of existLocalMemberNr is replaced during compile time by a direct
      call to existLocalMemberNr, so that no runtime overhead is caused;
      allows consistent naming with other ISO member functions
      @param rui8_nr member no to search for
      @return true -> one of the own identities has the wanted member no
    */
  bool existLocalIsoMemberNr(uint8_t rui8_nr)
    {return existLocalMemberNr(rui8_nr);}

  /**
      check for own din ident with given DEV_KEY;
      this variant of existLocalMemberNr is replaced during compile time by a direct
      call to existLocalMemberNr, so that no runtime overhead is caused;
      allows consistent naming with other ISO member functions
      @param rc_devKey              DEV_KEY to search for
      @param rb_forceClaimedAddress true -> only members with claimed address are used
                                    (optional, default false)
      @return true -> one of the own iso identities has the wanted DEV_KEY
    */
  bool existLocalIsoMemberDevKey(const DevKey_c& rc_devKey, bool rb_forceClaimedAddress = false)
    {return existLocalMemberDevKey(rc_devKey, rb_forceClaimedAddress);}
  /*\@}*/

  /**
    * reset the Addres Claim state by:
    * + reset IdentItem::IStat_c to IState_c::PreAddressClaim
    * + remove pointed ISOItem_c nodes and the respective pointer
    * @param rc_devKey  DEV_KEY
    * @return true -> there was an item with given DevKey_c that has been resetted to IState_c::PreAddressClaim
    */
  bool restartAddressClaim( const DevKey_c& rrefc_devKey );


private:
  friend class IdentItem_c;
  /** register pointer to a new client
    * this function is called within construction of new client instance
    */
  bool registerClient( IdentItem_c* pc_client) { return registerC1( pc_client );}

  /** register pointer to a new client
    * this function is called within construction of new client instance
    */
  void unregisterClient( IdentItem_c* pc_client) { pc_activeLocalMember = NULL; unregisterC1( pc_client );}

  /** handler function for access to undefined client.
    * the base Singleton calls this function, if it detects an error
    */
  void registerAccessFlt( void ) {getLbsErrInstance().registerError( LibErr_c::ElNonexistent, LibErr_c::LbsSystem );}


  friend class SINGLETON_DERIVED(SystemMgmt_c,ElementBase_c);
  /** private constructor which prevents direct instantiation in user application
    * NEVER define instance of SystemMgmt_c within application
    */
  SystemMgmt_c();

  /**
    initialize directly after the singleton instance is created.
    this is called from singleton.h and should NOT be called from the user again.
    users please use init(...) instead.
  */
  void singletonInit();

// Private attributes

  /**
    Cache pointer to speedup access to first active
    MonitorItem_c local member after check for existance
  */
  MonitorItem_c* pc_activeLocalMember;
};
#if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
  /** C-style function, to get access to the unique SystemMgmt_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  SystemMgmt_c& getSystemMgmtInstance( uint8_t rui8_instance = 0 );
#else
  /** C-style function, to get access to the unique SystemMgmt_c singleton instance */
  SystemMgmt_c& getSystemMgmtInstance( void );
#endif

}
#endif
