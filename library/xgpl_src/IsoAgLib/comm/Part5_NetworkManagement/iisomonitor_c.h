/***************************************************************************
                          iiso_monitor.h - object for monitoring members
                                             (list of iIsoItem_c)
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
#ifndef IISO_MONITOR_H
#define IISO_MONITOR_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include "impl/isomonitor_c.h"
#include "iisoitem_c.h"

// Begin Namespace IsoAgLib
namespace IsoAgLib {

class iScheduler_c;

/** this object manages a monitor list of all
  ISO members including inserting and administration of local own members.
  @short Manager for members of Scheduler_c (IsoItem_c)
  @see ISOItem
  @author Dipl.-Inform. Achim Spangler
*/
class iIsoMonitor_c : private __IsoAgLib::IsoMonitor_c
{
public:
  /** initialisation for IsoMonitor_c which can store optional pointer to central Scheduler_c instance */
  void init( void ) { IsoMonitor_c::init(); }

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

  /** check if member is in member list with wanted ISOName,
    adapt instance if member with claimed address with other device class inst exist
    @param rc_isoName ISOName to search (-> it's updated if member with claimed address with other dev class inst is found)
    @return true -> member with claimed address with given DEVCLASS found (and rc_isoName has now its ISOName)
  */
  bool isoDevClass2ISONameClaimedAddress(iIsoName_c &rc_isoName)
	{ return IsoMonitor_c::isoDevClass2ISONameClaimedAddress( rc_isoName);}

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

  /** deliver member item with given ISOName, set pointed bool var to true on success
    and set a Member Array Iterator to the result
    @param acrc_isoName searched ISOName
    @param pb_success bool pointer to store the success (true on success)
    @param ab_forceClaimedAddress (default:false)
    @return reference to the searched item
  */
  iIsoItem_c& isoMemberISOName(const iIsoName_c& acrc_isoName, bool *const pb_success, bool ab_forceClaimedAddress = false )
  { return static_cast<iIsoItem_c&>(IsoMonitor_c::isoMemberISOName( acrc_isoName, pb_success, ab_forceClaimedAddress ));}

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

  /** command switching to and from special service / diagnostic mode.
      setting the flag mc_serviceTool controls appropriate handling
    */
  void setDiagnosticMode( const iIsoName_c& acrc_serviceTool = iIsoName_c::iIsoNameUnspecified())
  { IsoMonitor_c::setDiagnosticMode( acrc_serviceTool );}

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

/** this typedef is only for some time to provide backward compatibility at API level */
typedef iIsoMonitor_c iISOMonitor_c;

}
#endif
