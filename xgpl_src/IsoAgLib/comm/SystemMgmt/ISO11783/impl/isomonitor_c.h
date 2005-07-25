/***************************************************************************
                          isomonitor_c.h - object for monitoring members
                                             (list of ISOItem_c)
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
#ifndef ISO_MONITOR_H
#define ISO_MONITOR_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include <IsoAgLib/typedef.h>
#include <IsoAgLib/util/config.h>

#include <IsoAgLib/util/impl/singleton.h>
#include <IsoAgLib/util/impl/elementbase_c.h>
#include <IsoAgLib/util/impl/cancustomer_c.h>
#include <IsoAgLib/driver/system/impl/system_c.h>
#include "isosystempkg_c.h"
#include "isoitem_c.h"

#if defined(SYSTEM_PC) && !defined(SYSTEM_PC_VC) && __GNUC__ >= 3
  #include <ext/slist>
  namespace std { using __gnu_cxx::slist;};
#else
  #include <slist>
#endif
namespace IsoAgLib { class iISOMonitor_c;}

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {
class ISOMonitor_c;
typedef SINGLETON_DERIVED(ISOMonitor_c, ElementBase_c) SingletonISOMonitor_c;
/**
  this object manages a monitor list of all
  ISO members including inserting and administration of local own members.
  @short Manager for members of Scheduler_c (ISOItem_c)
  @see ISOItem
  @author Dipl.-Inform. Achim Spangler
*/
class ISOMonitor_c : public SingletonISOMonitor_c
{
private:
  // private typedef alias names
  #ifdef OPTIMIZE_HEAPSIZE_IN_FAVOR_OF_SPEED
  typedef STL_NAMESPACE::slist<ISOItem_c,STL_NAMESPACE::__malloc_alloc_template<0> > Vec_ISO;
  typedef STL_NAMESPACE::slist<ISOItem_c,STL_NAMESPACE::__malloc_alloc_template<0> >::iterator Vec_ISOIterator;
  #else
  typedef STL_NAMESPACE::slist<ISOItem_c> Vec_ISO;
  typedef STL_NAMESPACE::slist<ISOItem_c>::iterator Vec_ISOIterator;
  #endif

public:
  /** initialisation for ISOMonitor_c which can store optional pointer to central Scheduler_c instance
  */
  void init( void );
  /** every subsystem of IsoAgLib has explicit function for controlled shutdown
    */
  void close( void );
  /**
    performs periodically actions,

    possible errors:
      * partial error caused by one of the memberItems

    @return true -> all planned activities performed in allowed time
  */
  bool timeEvent( void );

  /** default destructor which has nothing to do */
  virtual ~ISOMonitor_c();

  /**
    deliver reference to data pkg
    @return reference to the CAN communication member object c_data (ISOSystemPkg_c)
    @see CANPkgExt
  */
  ISOSystemPkg_c& data() {return c_data;};
  /**
    deliver reference to data pkg as reference to CANPkgExt_c
    to implement the base virtual function correct
  */
  virtual CANPkgExt_c& dataBase();
  /**
    deliver amount of ISO members in monitor list which optional (!!)
    match the condition of address claim state
    @param rb_forceClaimedAddress true -> only members with claimed address are used
          (optional, default false)
    @return amount of ISO members with claimed address
  */
  uint8_t isoMemberCnt(bool rb_forceClaimedAddress = false)
    {return isoMemberGetyCnt(0xFF, rb_forceClaimedAddress);};
  /**
    deliver the n'th ISO member in monitor list which optional (!!)
    match the condition of address claim state
    check first with isoMemberCnt if enough members are registered
    in Monitor-List
    @see isoMemberCnt

    possible errors:
      * Err_c::range there exist less than rui8_ind members with claimed address
    @param rui8_ind position of the wanted member in the
                 sublist of members (first item has rui8_ind == 0 !!)
                 with the wanted property
    @param rb_forceClaimedAddress true -> only members with claimed address are used
          (optional, default false)
    @return reference to searched element
  */
  ISOItem_c& isoMemberInd(uint8_t rui8_ind, bool rb_forceClaimedAddress = false)
    {return isoMemberGetyInd(0xFF, rui8_ind, rb_forceClaimedAddress);};

  /**
    deliver the count of members in the Monitor-List with given GETY (variable POS)
    which optional (!!) match the condition of address claim state
    @param rb_gety searched GETY code
    @param rb_forceClaimedAddress true -> only members with claimed address are used
          (optional, default false)
    @return count of members in Monitor-List with GETY == rb_gety
  */
  uint8_t isoMemberGetyCnt(uint8_t rb_gety, bool rb_forceClaimedAddress = false);
  /**
    deliver one of the members with specific GETY
    which optional (!!) match the condition of address claim state
    check first with isoMemberGetyCnt if enough members with wanted GETY and
    optional (!!) property are registered in Monitor-List
    @see isoMemberGetyCnt

    possible errors:
      * Err_c::range there exist less than rui8_ind members with GETY rb_gety
   @param rb_gety searched GETY
   @param rui8_ind position of the wanted member in the
                 sublist of member with given GETY (first item has rui8_ind == 0 !!)
   @param rb_forceClaimedAddress true -> only members with claimed address are used
         (optional, default false)
   @return reference to searched element
  */
  ISOItem_c& isoMemberGetyInd(uint8_t rb_gety, uint8_t rui8_ind, bool rb_forceClaimedAddress = false);
  /**
    check if a memberItem with given GETY_POS exist
    which optional (!!) match the condition of address claim state
    and update local pc_isoMemberCache
    @param rc_gtp searched GETY_POS
    @param rb_forceClaimedAddress true -> only members with claimed address are used
          (optional, default false)
    @return true -> searched member exist
  */
  bool existIsoMemberGtp(const GetyPos_c& rc_gtp, bool rb_forceClaimedAddress = false);

  /**
    check if a member with given number exist
    which optional (!!) match the condition of address claim state
    and update local pc_isoMemberCache
    @param rui8_nr searched member number
    @return true -> item found
  */
  bool existIsoMemberNr(uint8_t rui8_nr);

  /**
    check if member is in member list with wanted GETY_POS,
    adopt POS if member with claimed address with other POS exist
    @param refc_gtp GETY_POS to search (-> it's updated if member with claimed address with other POS is found)
    @return true -> member with claimed address with given GETY found (and refc_gtp has now its GETY_POS)
  */
  bool isoGety2GtpClaimedAddress(GetyPos_c &refc_gtp);

  /**
    insert a new ISOItem_c in the list; with unset rui8_nr the member is initiated as
    address claim state; otherwise the given state can be given or state Active is used

    possible errors:
      * Err_c::badAlloc not enough memory to insert new ISOItem_c isntance
      * Err_c::busy another member with same ident exists already in the list

    @param rc_gtp GETY_POS of the member
    @param rui8_nr member number
    @param rui16_saEepromAdr EEPROM adress to store actual SA -> next boot with same adr
    @param ren_status wanted status
    @return true -> the ISOItem_c was inserted
  */
  bool insertIsoMember(const GetyPos_c& rc_gtp, uint8_t rui8_nr = 0xFF,
                     IState_c::itemState_t ren_state = IState_c::Active, uint16_t rui16_saEepromAdr = 0xFFFF);

  /**
    deliver member item with given gtp
    (check with existIsoMemberGtp before access to not defined item)

    possible errors:
      * Err_c::elNonexistent on failed search

    @param rc_gtp searched GETY_POS
    @return reference to searched ISOItem
     @exception containerElementNonexistant
  */
  ISOItem_c& isoMemberGtp(const GetyPos_c& rc_gtp, bool rb_forceClaimedAddress = false);

  /**
    deliver member item with given nr
    (check with existIsoMemberNr before access to not defined item)

    possible errors:
      * Err_c::elNonexistent on failed search

    @param rui8_nr searched number
    @return reference to searched ISOItem
     @exception containerElementNonexistant
  */
  ISOItem_c& isoMemberNr(uint8_t rui8_nr);

  /**
    deliver member item with given GETY_POS, set pointed bool var to true on success
    and set a Member Array Iterator to the result
    @param rc_gtp searched GETY_POS
    @param pb_success bool pointer to store the success (true on success)
    @param pbc_iter optional member array iterator which points to searched ISOItem_c on success
    @return reference to the searched item
  */
  ISOItem_c& isoMemberGtp(const GetyPos_c& rc_gtp, bool *const pb_success, bool rb_forceClaimedAddress = false, Vec_ISOIterator *const pbc_iter = NULL);

  /**
    delete item with specified gtp

    possible errors:
      * Err_c::elNonexistent no member with given GETY_POS exists

    @param rc_gtp GETY_POS of to be deleted member
  */
  bool deleteIsoMemberGtp(const GetyPos_c& rc_gtp);
  /**
    delete item with specified member number

    possible errors:
      * Err_c::elNonexistent no member with given GETY_POS exists

    @param rui8_nr number of to be deleted member
  */
  bool deleteIsoMemberNr(uint8_t rui8_nr);

  /**
    check if SA of an announcing ISOItem_c is unique and deliver
    another free SA if not yet unique (else deliver its actual
    SA if unique yet)
    @param rpc_isoItem pointer to announcing ISOItem_c
    @return free unique SA (if possible the actual SA of the pointed item)
      (if wanted SA is not free for NOT-self-conf item or if no free SA is available
       254 is answered -> special flag for NACK)
  */
  uint8_t unifyIsoSa(const ISOItem_c* rpc_isoItem);
  /**
    change gtp if actual gtp isn't unique
    (search possible free POS to given GETY)

    possible errors:
      * Err_c::busy no other POS code leads to unique GETY_POS code

    @param refc_gtp reference to GETY_POS var (is changed directly if needed!!)
    @return true -> referenced GETY_POS is now unique
  */
  bool unifyIsoGtp(GetyPos_c& refc_gtp);
  /**
    deliver timestamp of last ISO request for SA claim msg
    @return time of last Request PG for Adress Claim on BUS
  */
  int32_t lastIsoSaRequest() const {return i32_lastSaRequest;};
  /**
    set timestamp of last ISO request for SA claim msg
    @param ri32_time set timestamp to ri32_time or use actual time on default
  */
  void setLastIsoSaRequest(int32_t ri32_time/* = -1*/)
    /** changed by M.Wodok so that NO system time is used,
        because REQUEST_FOR_CLAIMED_ADDRESS may be processed AFTER
        ADDRESS_CLAIM, so it's NOT chronologically. So CAN-Pkg-Times
        should be used here instead!! */
    {i32_lastSaRequest = /*(ri32_time != -1)?*/ri32_time/*:System_c::getTime()*/;};

  /**
    trigger a request for claimed addreses
    @param rb_force false -> send request only if no request was detected until now
    @return true -> request was sent
   */
  bool sendRequestForClaimedAddress( bool rb_force = false );

  /**
    get the Xth slave of the master isoItem
    @param xth tells which slave of the working set to get
    @param rpc_masterItem this is the master of which we want all the slaves
  */
  ISOItem_c* getSlave (uint8_t index, ISOItem_c* rpc_masterItem);
  /**
    get the number of slaves attached to the given working set master
    @param rpc_masterItem this is the master of which we want to know how many slaves he has
  */
  uint8_t getSlaveCount (ISOItem_c* rpc_masterItem);

protected: // Protected methods
  /**
    process system msg with informations which are
    important for managing of members
    (this function is only called if NO conflict is detected)
    @return true -> message processed by ISOMonitor_c; false -> process msg by ServiceMonitor
  */
  bool processMsg();

private: // Private attributes
  friend class IsoAgLib::iISOMonitor_c;
  friend class SINGLETON_DERIVED(ISOMonitor_c,ElementBase_c);
  /**
    constructor for ISOMonitor_c which can store optional pointer to central Scheduler_c instance
  */
  ISOMonitor_c( void );
  /** temp data where received and to be sent data is put */
  ISOSystemPkg_c c_data;
  /**
    dynamic array of memberItems for handling
    of single member informations
  */
  Vec_ISO vec_isoMember;
  /** last time of request for adress claim */
  int32_t i32_lastSaRequest;
  /** temporary memberItem instance for better inserting of new elements */
  ISOItem_c c_tempIsoMemberItem;
  /**
    cache pointer to speed serial of access
    to the same ISOItem
  */
  Vec_ISOIterator pc_isoMemberCache;
};
#if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
  /** C-style function, to get access to the unique ISOMonitor_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  ISOMonitor_c& getIsoMonitorInstance( uint8_t rui8_instance = 0 );
#else
  /** C-style function, to get access to the unique ISOMonitor_c singleton instance */
  ISOMonitor_c& getIsoMonitorInstance( void );
#endif

}
#endif
