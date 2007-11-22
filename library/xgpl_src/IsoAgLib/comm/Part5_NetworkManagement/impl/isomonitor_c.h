/***************************************************************************
                          isomonitor_c.h - object for monitoring members
                                             (list of IsoItem_c)
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
#include <IsoAgLib/comm/Scheduler/impl/schedulertask_c.h>
#include <IsoAgLib/driver/can/impl/cancustomer_c.h>
#include <IsoAgLib/driver/system/impl/system_c.h>
#include "isosystempkg_c.h"
#include "isoitem_c.h"
#include "saclaimhandler_c.h"
#include "isorequestpgnhandler_c.h"
#include "identitem_c.h"

#include <map>

#ifdef DO_USE_SLIST
  #if defined(SYSTEM_PC) && !defined(SYSTEM_PC_VC) && !defined(SYSTEM_A1) && __GNUC__ >= 3
    #include <ext/slist>
    namespace std { using __gnu_cxx::slist;}
  #else
    #include <slist>
  #endif
#else
  #include <list>
#endif
namespace IsoAgLib { class iIsoMonitor_c;}

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

/** type of map which is used to store SaClaimHandler_c clients corresponding to a IsoName_c reference */
typedef STL_NAMESPACE::vector<SaClaimHandler_c*> SaClaimHandlerVector_t;
typedef STL_NAMESPACE::vector<SaClaimHandler_c*>::iterator SaClaimHandlerVectorIterator_t;
typedef STL_NAMESPACE::vector<SaClaimHandler_c*>::const_iterator SaClaimHandlerVectorConstIterator_t;

class IsoMonitor_c;
typedef SINGLETON_DERIVED_CLIENT1(IsoMonitor_c, Scheduler_Task_c, IdentItem_c, IsoName_c) SingletonIsoMonitor_c;
// NEU SINGLETON TYPEDEF, wenn ISOMonitor auch IdentItem_c liste verwalten soll
// typedef SINGLETON_DERIVED1(IsoMonitor_c, Scheduler_Task_c, IdentItem_c, IsoName_c) SingletonIsoMonitor_c;
/** this object manages a monitor list of all
  ISO members including inserting and administration of local own members.
  @short Manager for members of Scheduler_c (IsoItem_c)
  @see ISOItem
  @author Dipl.-Inform. Achim Spangler
*/
class IsoMonitor_c : public SingletonIsoMonitor_c
{
private:
  // private typedef alias names
  #ifdef OPTIMIZE_HEAPSIZE_IN_FAVOR_OF_SPEED
  typedef STL_NAMESPACE::USABLE_SLIST<IsoItem_c,MALLOC_TEMPLATE(IsoItem_c) > Vec_ISO;
  typedef STL_NAMESPACE::USABLE_SLIST<IsoItem_c,MALLOC_TEMPLATE(IsoItem_c) >::iterator Vec_ISOIterator;
  typedef STL_NAMESPACE::USABLE_SLIST<IsoItem_c,MALLOC_TEMPLATE(IsoItem_c) >::const_iterator Vec_ISOIteratorConst;
  #else
  typedef STL_NAMESPACE::USABLE_SLIST<IsoItem_c> Vec_ISO;
  typedef STL_NAMESPACE::USABLE_SLIST<IsoItem_c>::iterator Vec_ISOIterator;
  typedef STL_NAMESPACE::USABLE_SLIST<IsoItem_c>::const_iterator Vec_ISOIteratorConst;
  #endif

public:
  /** initialisation for IsoMonitor_c which can store optional pointer to central Scheduler_c instance */
  void init( void );

  /** every subsystem of IsoAgLib has explicit function for controlled shutdown */
  void close( void );

  /** performs periodically actions,
    possible errors:
      * partial error caused by one of the memberItems
    @return true -> all planned activities performed in allowed time
  */
  bool timeEvent( void );

  virtual bool isNetworkMgmt() const { return true; }

  /** default destructor which has nothing to do */
  virtual ~IsoMonitor_c();

  /** deliver reference to data pkg
    @return reference to the CAN communication member object mc_data (IsoSystemPkg_c)
    @see CANPkgExt
  */
  IsoSystemPkg_c& data() {return mc_data;}

  /** deliver reference to data pkg as reference to CanPkgExt_c
    to implement the base virtual function correct
  */
  virtual CanPkgExt_c& dataBase();

  /** deliver amount of ISO members in monitor list which optional (!!)
    match the condition of address claim state
    @param ab_forceClaimedAddress true -> only members with claimed address are used
          (optional, default false)
    @return amount of ISO members with claimed address
  */
  uint8_t isoMemberCnt(bool ab_forceClaimedAddress = false)
    {return isoMemberDevClassCnt(0xFF, ab_forceClaimedAddress);}

  /** deliver the n'th ISO member in monitor list which optional (!!)
    match the condition of address claim state
    check first with isoMemberCnt if enough members are registered
    in Monitor-List
    @see isoMemberCnt
    possible errors:
      * Err_c::range there exist less than aui8_ind members with claimed address
    @param aui8_ind position of the wanted member in the
                 sublist of members (first item has aui8_ind == 0 !!)
                 with the wanted property
    @param ab_forceClaimedAddress true -> only members with claimed address are used
          (optional, default false)
    @return reference to searched element
  */
  IsoItem_c& isoMemberInd(uint8_t aui8_ind, bool ab_forceClaimedAddress = false)
    {return isoMemberDevClassInd(0xFF, aui8_ind, ab_forceClaimedAddress);}

  /** deliver the count of members in the Monitor-List with given ECU-Type (which is an own IsoAgLib-definition!)
    which optional (!!) match the condition of address claim state
    @param aui8_ecuType searched ECU-Type code
    @param ab_forceClaimedAddress true -> only members with claimed address are used
          (optional, default false)
    @return count of members in Monitor-List with ECU-Type == aui8_ecuType
  */
  uint8_t isoMemberEcuTypeCnt (IsoName_c::ecuType_t a_ecuType, bool ab_forceClaimedAddress = false);

  /** deliver one of the members with specific ECU_Type (which is an own IsoAgLib-definition!)
    which optional (!!) match the condition of address claim state
    check first with isoMemberEcuTypeCnt if enough members with wanted ECU-Type and
    optional (!!) property are registered in Monitor-List
    @see isoMemberEcuTypeCnt
    possible errors:
      * Err_c::range there exist less than aui8_ind members with ECU-Type aui8_ecuType
    @param aui8_ecuType searched ECU-Type code
    @param aui8_ind position of the wanted member in the
                  sublist of member with given ECU-Type (first item has aui8_ind == 0 !!)
    @param ab_forceClaimedAddress true -> only members with claimed address are used
          (optional, default false)
    @return reference to searched element
  */
  IsoItem_c& isoMemberEcuTypeInd (IsoName_c::ecuType_t a_ecuType, uint8_t aui8_ind, bool ab_forceClaimedAddress = false);

  /** deliver the count of members in the Monitor-List with given DEVCLASS (variable POS)
    which optional (!!) match the condition of address claim state
    @param aui8_devClass searched DEVCLASS code
    @param ab_forceClaimedAddress true -> only members with claimed address are used
          (optional, default false)
    @return count of members in Monitor-List with DEVCLASS == aui8_devClass
  */
  uint8_t isoMemberDevClassCnt(uint8_t aui8_devClass, bool ab_forceClaimedAddress = false);

  /** deliver one of the members with specific DEVCLASS
    which optional (!!) match the condition of address claim state
    check first with isoMemberDevClassCnt if enough members with wanted DEVCLASS and
    optional (!!) property are registered in Monitor-List
    @see isoMemberDevClassCnt
    possible errors:
      * Err_c::range there exist less than aui8_ind members with DEVCLASS aui8_devClass
   @param aui8_devClass searched DEVCLASS
   @param aui8_ind position of the wanted member in the
                 sublist of member with given DEVCLASS (first item has aui8_ind == 0 !!)
   @param ab_forceClaimedAddress true -> only members with claimed address are used
         (optional, default false)
   @return reference to searched element
  */
  IsoItem_c& isoMemberDevClassInd(uint8_t aui8_devClass, uint8_t aui8_ind, bool ab_forceClaimedAddress = false);

  /** check if a memberItem with given ISOName exist
    which optional (!!) match the condition of address claim state
    and update local mpc_isoMemberCache
    @param ac_isoName searched ISOName
    @param ab_forceClaimedAddress true -> only members with claimed address are used
          (optional, default false)
    @return true -> searched member exist
  */
  bool existIsoMemberISOName(const IsoName_c& ac_isoName, bool ab_forceClaimedAddress = false);

  /** check if a member with given number exist
    which optional (!!) match the condition of address claim state
    and update local mpc_isoMemberCache
    @param aui8_nr searched member number
    @return true -> item found
  */
  bool existIsoMemberNr(uint8_t aui8_nr);

  /**
    check if member is in member list with wanted ISOName,
    adapt instance if member with claimed address with other device class inst exist
    @param rc_isoName ISOName to search (-> it's updated if member with claimed address with other dev class inst is found)
    @return true -> member with claimed address with given DEVCLASS found (and rc_isoName has now its ISOName)
  */
  bool isoDevClass2ISONameClaimedAddress(IsoName_c &rc_isoName);

  /** insert a new IsoItem_c in the list; with unset aui8_nr the member is initiated as
    address claim state; otherwise the given state can be given or state Active is used
    possible errors:
      * Err_c::badAlloc not enough memory to insert new IsoItem_c isntance
      * Err_c::busy another member with same ident exists already in the list
    @param ac_isoName ISOName of the member
    @param aui8_nr member number
    @param aui16_saEepromAdr EEPROM adress to store actual SA -> next boot with same adr
    @param ren_status wanted status
    @return pointer to new IsoItem_c or NULL if not succeeded
  */
  IsoItem_c* insertIsoMember(const IsoName_c& ac_isoName, uint8_t aui8_nr = 0xFF,
                     IState_c::itemState_t ren_state = IState_c::Active);


  /** deliver the amount of local members which matches the searched proto types
      @return amount of registered local members with wanted type
     */
  uint8_t localIsoMemberCnt();

  /** deliver pointer to local member of wanted type by index
      @see localMemberCnt
      @param aui8_ind index of wanted member (first item == 0)
      @return pointer to wanted local member (NULL if no suitable MonitorItem_c found)
      (MonitorItem_c is base class of IsoItem_c which serves address, isoName, itemState)
     */
  IsoItem_c& localIsoMemberInd( uint8_t aui8_ind );

  /** check if one of the own local members is active with claimed address at ISO11783
      @return true -> at least one of the own iso identities is active with claimed address at ISO11783
      @see SystemMgmt_c::getActiveLocalMember
     */
  bool existActiveLocalIsoMember();

  /** delivers reference to the first active local member;
      send of ISO11783 system msg demands telling a sender ident member no
      -> using the number of the first active member serves as default (f.e. for requesting other member names)
      can throw an preconditionViolation error, if none of the own identities is active/claimed address yet
          possible errors:
      * Err_c::lbsSysNoActiveLocalMember on missing own active ident
      @return reference to the MonitorItem_c of the first active local member
      (MonitorItem_c is baes class of IsoItem_c which serves
      adress, isoName, itemState)
      @exception preconditionViolation
     */
  IsoItem_c& getActiveLocalIsoMember();

  /** check for own ident with given member no
      @param aui8_nr member no to search for
      @return true -> one of the own identities has the wanted member no
     */
  bool existLocalIsoMemberNr (uint8_t aui8_nr);

  /** check for own ident with given ISOName
      @param ac_isoName              ISOName to search for
      @param ab_forceClaimedAddress true -> only members with claimed address are used
      (optional, default false)
      @return true -> one of the own identities has the wanted ISOName
     */
  bool existLocalIsoMemberISOName (const IsoName_c& ac_isoName, bool ab_forceClaimedAddress = false);

  /**
      * reset the Address Claim state by:
      * + reset IdentItem::IStat_c to IState_c::PreAddressClaim
      * + remove pointed IsoItem_c nodes and the respective pointer
      * @param ac_isoName  ISOName
      * @return true -> there was an item with given IsoName_c that has been resetted to IState_c::PreAddressClaim
     */
  bool restartAddressClaim( const IsoName_c& arc_isoName );

  /** register a SaClaimHandler_c */
  bool registerSaClaimHandler (SaClaimHandler_c* apc_client);

  /** deregister a SaClaimHandler */
  bool deregisterSaClaimHandler (SaClaimHandler_c* apc_client);


  /** this function is used to broadcast a ISO monitor list change to all registered clients */
  void broadcastSaAdd2Clients( const IsoName_c& ac_isoName, const IsoItem_c* apc_isoItem ) const;

  /** this function is used to broadcast a ISO monitor list change to all registered clients */
  void broadcastSaRemove2Clients( const IsoName_c& ac_isoName, uint8_t aui8_oldSa ) const;

  /**
    deliver member item with given isoName
    (check with existIsoMemberISOName before access to not defined item)
    possible errors:
      * Err_c::elNonexistent on failed search
    @param ac_isoName searched ISOName
    @return reference to searched ISOItem
     @exception containerElementNonexistant
  */
  IsoItem_c& isoMemberISOName(const IsoName_c& ac_isoName, bool ab_forceClaimedAddress = false);

  /** deliver member item with given nr
    (check with existIsoMemberNr before access to not defined item)
    possible errors:
      * Err_c::elNonexistent on failed search
    @param aui8_nr searched number
    @return reference to searched ISOItem
     @exception containerElementNonexistant
  */
  IsoItem_c& isoMemberNr(uint8_t aui8_nr);

  /** deliver member item with given ISOName, set pointed bool var to true on success
    and set a Member Array Iterator to the result
    @param ac_isoName searched ISOName
    @param pb_success bool pointer to store the success (true on success)
    @param pbc_iter optional member array iterator which points to searched IsoItem_c on success
    @return reference to the searched item
  */
  IsoItem_c& isoMemberISOName(const IsoName_c& ac_isoName, bool *const pb_success, bool ab_forceClaimedAddress = false, Vec_ISOIterator *const pbc_iter = NULL);

  /**
    delete item with specified isoName
    possible errors:
      * Err_c::elNonexistent no member with given ISOName exists
    @param ac_isoName ISOName of to be deleted member
  */
  bool deleteIsoMemberISOName(const IsoName_c& ac_isoName);

  /** delete item with specified member number
    possible errors:
      * Err_c::elNonexistent no member with given ISOName exists
    @param aui8_nr number of to be deleted member
  */
  bool deleteIsoMemberNr(uint8_t aui8_nr);

  /** check if SA of an announcing IsoItem_c is unique and deliver
    another free SA if not yet unique (else deliver its actual
    SA if unique yet)
    @param apc_isoItem pointer to announcing IsoItem_c
    @return free unique SA (if possible the actual SA of the pointed item)
      (if wanted SA is not free for NOT-self-conf item or if no free SA is available
       254 is answered -> special flag for NACK)
  */
  uint8_t unifyIsoSa(const IsoItem_c* apc_isoItem);

  /**
    change isoName if actual isoName isn't unique
    (search possible free instance to given device class)
    possible errors:
      * Err_c::busy no other device class inst code leads to unique ISOName code
    @param rc_isoName reference to ISOName var (is changed directly if needed!!)
    @return true -> referenced ISOName is now unique
  */
  bool unifyIsoISOName (IsoName_c& rc_isoName, bool ab_dontUnify);

  /** deliver timestamp of last ISO request for SA claim msg
    @return time of last Request PG for Adress Claim on BUS
  */
  int32_t lastIsoSaRequest() const {return mi32_lastSaRequest;}

  /** set timestamp of last ISO request for SA claim msg
    @param ai32_time set timestamp to ai32_time or use actual time on default

    changed by M.Wodok so that NO system time is used,
    because REQUEST_FOR_CLAIMED_ADDRESS may be processed AFTER
    ADDRESS_CLAIM, so it's NOT chronologically. So CAN-Pkg-Times
    should be used here instead!!
  */
  void setLastIsoSaRequest (int32_t ai32_time/* = -1*/) {mi32_lastSaRequest = /*(ai32_time != -1)?*/ai32_time/*:System_c::getTime()*/;}

  /** trigger a request for claimed addreses
    @param ab_force false -> send request only if no request was detected until now
    @return true -> request was sent
   */
  bool sendRequestForClaimedAddress( bool ab_force = false );

  /** get the Xth slave of the master isoItem
    @param xth tells which slave of the working set to get
    @param apc_masterItem this is the master of which we want all the slaves
  */
  IsoItem_c* getSlave (uint8_t index, IsoItem_c* apc_masterItem);

  /** get the number of slaves attached to the given working set master
    @param apc_masterItem this is the master of which we want to know how many slaves he has
  */
  uint8_t getSlaveCount (IsoItem_c* apc_masterItem);

  /** notify that a ws-master is in destruction,
    so that all slaves can be notified and can set their master to NULL,
    so they become STANDALONE IsoItems instead then (again).
    @param xth tells which slave of the working set to get
    @param apc_masterItem this is the master of which we want all the slaves
  */
  void notifyOnWsMasterLoss (IsoItem_c& arc_masterItem);

  bool processMsgRequestPGN (uint32_t aui32_pgn, IsoItem_c* apc_isoItemSender, IsoItem_c* apc_isoItemReceiver);

  ///  Operation: // Function for Debugging in Scheduler_c
  virtual const char* getTaskName() const;

  /// Function notify Scheduler_c to set new retriggerTime
  /// will be called from IdentItem_c and registerClient
  /// IsoMonitor_c.timeEvent() should be called in 50 ms
  void changeRetriggerTime() { getSchedulerInstance4Comm().changeRetriggerTimeAndResort(this,System_c::getTime() + 50 );};


  /** command switching to and from special service / diagnostic mode.
      setting the flag mc_serviceTool controls appropriate handling
    */
  void setDiagnosticMode( const IsoName_c& arc_serviceTool = IsoName_c::IsoNameUnspecified());
protected: // Protected methods
  /** process system msg with informations which are
    important for managing of members
    (this function is only called if NO conflict is detected)
    @return true -> message processed by IsoMonitor_c; false -> process msg by ServiceMonitor
  */
  bool processMsg();

  //! Function set ui16_earlierInterval and
  //! ui16_laterInterval that will be used by
  //! getTimeToNextTrigger(retriggerType_t)
  //! can be overloaded by Childclass for special condition
  virtual void updateEarlierAndLatestInterval();

private:
  friend class IdentItem_c;
  /** register pointer to a new client
    * this function is called within construction of new client instance
   */
  bool registerClient( IdentItem_c* pc_client)
  {
    /// IsoMonitor_c.timeEvent() should be called from Scheduler_c in 50 ms
    changeRetriggerTime();
    return registerC1 ( pc_client );
  }

  /** register pointer to a new client
   * this function is called within construction of new client instance
     */
  void unregisterClient( IdentItem_c* pc_client) { mpc_activeLocalMember = NULL; unregisterC1 ( pc_client ); }

  /** handler function for access to undefined client.
    * the base Singleton calls this function, if it detects an error
     */
  void registerAccessFlt( void ) {getILibErrInstance().registerError( iLibErr_c::ElNonexistent, iLibErr_c::System );}

  friend class SINGLETON_DERIVED(IsoMonitor_c,Scheduler_Task_c);

  /** constructor for IsoMonitor_c which can store optional pointer to central Scheduler_c instance */
  IsoMonitor_c( void );

  /** initialize directly after the singleton instance is created.
    this is called from singleton.h and should NOT be called from the user again.
    users please use init(...) instead.
  */
  void singletonInit();

private: // Private attributes
  friend class IsoAgLib::iIsoMonitor_c;

  /** Cache pointer to speedup access to first active
    MonitorItem_c local member after check for existance
     */
  IsoItem_c* mpc_activeLocalMember;

  /** temp data where received and to be sent data is put */
  IsoSystemPkg_c mc_data;

  /** dynamic array of memberItems for handling
      of single member informations
  */
  Vec_ISO mvec_isoMember;

  /** last time of request for adress claim */
  int32_t mi32_lastSaRequest;

  /** temporary memberItem instance for better inserting of new elements */
  IsoItem_c mc_tempIsoMemberItem;

  /** cache pointer to speed serial of access to the same ISOItem */
  Vec_ISOIterator mpc_isoMemberCache;
  /** map of SaClaimHandler_c clients that want to be informed on monitor list changes */

  SaClaimHandlerVector_t mvec_saClaimHandler;

  /** flag to indicate service / diagnostic mode, where only connections to a dedicated ECU
     should be maintained */
  IsoName_c mc_serviceTool;
};

#if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
  /** C-style function, to get access to the unique IsoMonitor_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  IsoMonitor_c& getIsoMonitorInstance( uint8_t aui8_instance = 0 );
#else
  /** C-style function, to get access to the unique IsoMonitor_c singleton instance */
  IsoMonitor_c& getIsoMonitorInstance( void );
#endif

/** this typedef is only for some time to provide backward compatibility at API level */
typedef IsoMonitor_c ISOMonitor_c;

}
#endif
