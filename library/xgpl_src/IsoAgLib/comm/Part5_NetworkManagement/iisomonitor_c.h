/*
  iisomonitor_c.h: object for monitoring members (list of iIsoItem_c)

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef IISO_MONITOR_H
#define IISO_MONITOR_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include "impl/isomonitor_c.h"
#include "iidentitem_c.h"
#include <IsoAgLib/comm/Part5_NetworkManagement/icontrolfunctionstatehandler_c.h>


// Begin Namespace IsoAgLib
namespace IsoAgLib {


/** this object manages a monitor list of all
  ISO members including inserting and administration of local own members.
  @short Manager for members of Scheduler_c (IsoItem_c)
  @see ISOItem
  @author Dipl.-Inform. Achim Spangler
*/
class iIsoMonitor_c : private __IsoAgLib::IsoMonitor_c
{
public:
  /** register a properly set-up IdentItem.
      @return false in case the IdentItem wasn't properly activated
  */
  bool registerIdentItem( iIdentItem_c& arc_item ) { return IsoMonitor_c::registerIdentItem( arc_item ); }

  /** deregister a registered IdentItem
      @param arc_item The (registered) IdentItem
  */
  void deregisterIdentItem( iIdentItem_c& arc_item ) { IsoMonitor_c::deregisterIdentItem( arc_item ); }


  /** deliver amount of ISO members in monitor list which optional (!!)
    match the condition of address claim state
    @param ab_forceClaimedAddress true -> only members with claimed address are used
          (optional, default false)
    @return amount of ISO members with claimed address
  */
  uint8_t isoMemberCnt(bool ab_forceClaimedAddress = false)
  { return IsoMonitor_c::isoMemberCnt(ab_forceClaimedAddress); }

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
  iIsoItem_c& isoMemberInd(uint8_t aui8_ind, bool ab_forceClaimedAddress = false)
  { return static_cast<iIsoItem_c&>(IsoMonitor_c::isoMemberInd( aui8_ind, ab_forceClaimedAddress )); }

  /** deliver the count of members in the Monitor-List with given FUNCTION
    and device classe which optional (!!) match the condition of address claim state
    @param aui8_function searched FUNCTION code
    @param aui8_devClass searched DEVCLASS code
    @param ab_forceClaimedAddress true -> only members with claimed address are used
          (optional, default false)
    @return count of members in Monitor-List with FUNCTION == aui8_function and DEVCLASS == aui8_devClass
  */
  uint8_t isoMemberDevClassFuncCnt(uint8_t aui8_devClass, uint8_t aui8_function, bool ab_forceClaimedAddress = false)
  { return IsoMonitor_c::isoMemberDevClassFuncCnt( aui8_devClass, aui8_function, ab_forceClaimedAddress );}


  /** deliver the count of members in the Monitor-List with given DEVCLASS (variable POS)
    which optional (!!) match the condition of address claim state
    @param aui8_devClass searched DEVCLASS code
    @param ab_forceClaimedAddress true -> only members with claimed address are used
          (optional, default false)
    @return count of members in Monitor-List with DEVCLASS == aui8_devClass
  */
  uint8_t isoMemberDevClassCnt(uint8_t aui8_devClass, bool ab_forceClaimedAddress = false)
  { return IsoMonitor_c::isoMemberDevClassCnt( aui8_devClass, ab_forceClaimedAddress );}

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
  iIsoItem_c& isoMemberDevClassInd(uint8_t aui8_devClass, uint8_t aui8_ind, bool ab_forceClaimedAddress = false)
  { return static_cast<iIsoItem_c&>(IsoMonitor_c::isoMemberDevClassInd( aui8_devClass, aui8_ind, ab_forceClaimedAddress ));}

  /** check if a memberItem with given ISOName exist
    which optional (!!) match the condition of address claim state
    and update local mpc_isoMemberCache
    @param acrc_isoName searched ISOName
    @param ab_forceClaimedAddress true -> only members with claimed address are used
          (optional, default false)
    @return true -> searched member exist
  */
  bool existIsoMemberISOName(const iIsoName_c& acrc_isoName, bool ab_forceClaimedAddress = false)
  { return IsoMonitor_c::existIsoMemberISOName(acrc_isoName, ab_forceClaimedAddress );}

  /** check if a member with given number exist
    which optional (!!) match the condition of address claim state
    and update local mpc_isoMemberCache
    @param aui8_nr searched member number
    @return true -> item found
  */
  bool existIsoMemberNr(uint8_t aui8_nr)
  { return IsoMonitor_c::existIsoMemberNr( aui8_nr );}

  /** deliver member item with given isoName
    (check with existIsoMemberISOName before access to not defined item)
    possible errors:
      * Err_c::elNonexistent on failed search
    @param acrc_isoName searched ISOName
    @return reference to searched ISOItem
     @exception containerElementNonexistant
  */
  iIsoItem_c& isoMemberISOName(const iIsoName_c& acrc_isoName, bool ab_forceClaimedAddress = false)
    { return static_cast<iIsoItem_c&>(IsoMonitor_c::isoMemberISOName( acrc_isoName, ab_forceClaimedAddress));}

  /** deliver member item with given nr
    (check with existIsoMemberNr before access to not defined item)
    possible errors:
      * Err_c::elNonexistent on failed search
    @param aui8_nr searched number
    @return reference to searched ISOItem
     @exception containerElementNonexistant
  */
  iIsoItem_c& isoMemberNr(uint8_t aui8_nr) { return static_cast<iIsoItem_c&>(IsoMonitor_c::isoMemberNr( aui8_nr));}

  /** check if one of the own local members is active with claimed address at ISO11783
    @return true -> at least one of the own identities is active with claimed address at ISO11783
    @see SystemMgmt_c::getActiveLocalMember
  */
  bool existActiveLocalIsoMember() {return IsoMonitor_c::existActiveLocalIsoMember();}

  /** delivers reference to the first active local ISO member;
    -> using the number of the first active member serves as default
    can throw an preconditionViolation error, if none of the own ISO identities is active/claimed address yet
      possible errors:
    * Err_c::lbsSysNoActiveLocalMember on missing own active ident
    @return reference to the IsoItem_c of the first active local member
    @exception preconditionViolation
  */
  iIsoItem_c& getActiveLocalIsoMember()
  {return static_cast<iIsoItem_c&>(IsoMonitor_c::getActiveLocalIsoMember());}

  /** register an iControlFunctionStateHandler_c */
  void registerControlFunctionStateHandler(iControlFunctionStateHandler_c &arc_handler) {
    IsoMonitor_c::registerControlFunctionStateHandler( arc_handler );
  }

  /** deregister an iControlFunctionStateHandler */
  void deregisterControlFunctionStateHandler(iControlFunctionStateHandler_c &arc_handler) {
    IsoMonitor_c::deregisterControlFunctionStateHandler( arc_handler );
  }

private:
  /** allow getIisoMonitorInstance() access to shielded base class.
      otherwise __IsoAgLib::getIsoMonitorInstance() wouldn't be accepted by compiler
    */
  #if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
  friend iIsoMonitor_c& getIisoMonitorInstance( uint8_t aui8_instance );
  #else
  friend iIsoMonitor_c& getIisoMonitorInstance( void );
  #endif

  /** HIDDEN constructor for a iIsoMonitor_c object instance
    NEVER instantiate a variable of type iIsoMonitor_c within application
    only access iIsoMonitor_c via getIisoInstance() or getIisoInstance( int riLbsBusNr ) in case more than one ISO11783 BUS is used for IsoAgLib
    */
  iIsoMonitor_c() : IsoMonitor_c() {}
};

#if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
  /** C-style function, to get access to the unique IsoMonitor_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  inline iIsoMonitor_c& getIisoMonitorInstance( uint8_t aui8_instance = 0 )
  { return static_cast<iIsoMonitor_c&>(__IsoAgLib::getIsoMonitorInstance(aui8_instance));}
#else
  /** C-style function, to get access to the unique IsoMonitor_c singleton instance */
  inline iIsoMonitor_c& getIisoMonitorInstance( void )
  { return static_cast<iIsoMonitor_c&>(__IsoAgLib::getIsoMonitorInstance());}
#endif


}
#endif
