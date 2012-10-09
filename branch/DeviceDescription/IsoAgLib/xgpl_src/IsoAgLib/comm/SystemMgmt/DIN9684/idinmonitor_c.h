/***************************************************************************
                          idinmonitor_c.h - object for monitoring members
                                             (list of iDINItem_c)
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
#ifndef IMEMBER_MONITOR_H
#define IMEMBER_MONITOR_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include "impl/dinmonitor_c.h"
#include "idinitem_c.h"

// Begin Namespace IsoAgLib
namespace IsoAgLib {
/**
  This object manages a monitor list of all DIN 9684
  members including inserting and administration of local own members.

  Derive from ElementBase_c to register in Scheduler_c for timeEvent trigger
  Derive from CANCustomer_c to register FilterBox'es in CANIO_c to receive CAN messages
  Derive from SINGLETON to create a Singleton which manages one global accessible singleton
   per IsoAgLib instance (if only one IsoAgLib instance is defined in application config, no overhead is produced).
  @short Manager for members of Scheduler_c (DINItem_c)
  @see iDINItem_c
  @author Dipl.-Inform. Achim Spangler
*/
class iDINMonitor_c : private __IsoAgLib::DINMonitor_c
{
public:
  /** initialisation for DINMonitor_c
  */
  void init( void ) { DINMonitor_c::init();};
  /**
    sends system message, which requests all active
    members to send their names (send own name too, as protocol demands)

    possible errors:
      * Err_c::lbsSysNoActiveLocalMember on missing own active ident
    @return true -> member name command successful sent
  */
  bool requestDinMemberNames() { return DINMonitor_c::requestDinMemberNames();};
  /**
    deliver amount of DIN members in monitor list which optional (!!)
    match the condition of address claim state
    @param rb_forceClaimedAddress true -> only members with claimed address are used
          (optional, default false)
    @return amount of DIN members with claimed address
  */
  uint8_t dinMemberCnt(bool rb_forceClaimedAddress = false) { return DINMonitor_c::dinMemberCnt(rb_forceClaimedAddress);};
  /**
    deliver the n'th DIN member in monitor list which optional (!!)
    match the condition of address claim state
    check first with dinMemberCnt if enough members are registered
    in Monitor-List
    @see dinMemberCnt

    possible errors:
      * Err_c::range there exist less than rui8_ind members with claimed address
    @param rui8_ind position of the wanted member in the
                 sublist of members (first item has rui8_ind == 0 !!)
                 with the wanted property
    @param rb_forceClaimedAddress true -> only members with claimed address are used
          (optional, default false)
    @return reference to searched element
  */
  iDINItem_c& dinMemberInd(uint8_t rui8_ind, bool rb_forceClaimedAddress = false)
  { return static_cast<iDINItem_c&>(DINMonitor_c::dinMemberInd(rui8_ind, rb_forceClaimedAddress));};

  /**
    deliver the count of members in the Monitor-List with given GETY (variable POS)
    which optional (!!) match the condition of address claim state
    @param rb_gety searched GETY code
    @param rb_forceClaimedAddress true -> only members with claimed address are used
          (optional, default false)
    @return count of members in Monitor-List with GETY == rb_gety
  */
  uint8_t dinMemberGetyCnt(uint8_t rb_gety, bool rb_forceClaimedAddress = false)
  { return DINMonitor_c::dinMemberGetyCnt(rb_gety, rb_forceClaimedAddress);};
  /**
    deliver one of the members with specific GETY
    which optional (!!) match the condition of address claim state
    check first with dinMemberGetyCnt if enough members with wanted GETY and
    optional (!!) property are registered in Monitor-List
    @see dinMemberGetyCnt

    possible errors:
      * Err_c::range there exist less than rui8_ind members with GETY rb_gety
   @param rb_gety searched GETY
   @param rui8_ind position of the wanted member in the
                 sublist of member with given GETY (first item has rui8_ind == 0 !!)
   @param rb_forceClaimedAddress true -> only members with claimed address are used
         (optional, default false)
   @return reference to searched element
  */
  iDINItem_c& dinMemberGetyInd(uint8_t rb_gety, uint8_t rui8_ind, bool rb_forceClaimedAddress = false)
  { return static_cast<iDINItem_c&>(DINMonitor_c::dinMemberGetyInd(rb_gety, rui8_ind, rb_forceClaimedAddress));};
  /**
    check if a memberItem with given GETY_POS exist
    which optional (!!) match the condition of address claim state
    and update local pc_dinMemberCache
    @param rc_gtp searched GETY_POS
    @param rb_forceClaimedAddress true -> only members with claimed address are used
          (optional, default false)
    @return true -> searched member exist
  */
  bool existDinMemberGtp(iGetyPos_c rc_gtp, bool rb_forceClaimedAddress = false)
    { return DINMonitor_c::existDinMemberGtp(rc_gtp, rb_forceClaimedAddress);};

  /**
    check if a member with given number exist
    which optional (!!) match the condition of address claim state
    and update local pc_dinMemberCache
    @param rui8_nr searched member number
    @param rb_forceClaimedAddress true -> only members with claimed address are used
          (optional, default false)
    @return true -> item found
  */
  bool existDinMemberNr(uint8_t rui8_nr, bool rb_forceClaimedAddress = false)
  { return DINMonitor_c::existDinMemberNr(rui8_nr, rb_forceClaimedAddress);};

  /**
    check if member is in member list with wanted GETY_POS,
    adopt POS if member with claimed address with other POS exist
    @param refc_gtp GETY_POS to search (-> it's updated if member with claimed address with other POS is found)
    @return true -> member with claimed address with given GETY found (and refc_gtp has now its GETY_POS)
  */
  bool dinGety2GtpClaimedAddress(iGetyPos_c &refc_gtp) { return DINMonitor_c::dinGety2GtpClaimedAddress(refc_gtp);};


  /**
    deliver member item with given gtp
    (check with existDinMemberGtp before access to not defined item)

    possible errors:
      * Err_c::elNonexistent on failed search

    @param rc_gtp searched GETY_POS
    @return reference to searched MemberItem
     @exception containerElementNonexistant
  */
  iDINItem_c& dinMemberGtp(iGetyPos_c rc_gtp) { return static_cast<iDINItem_c&>(DINMonitor_c::dinMemberGtp(rc_gtp));};

  /**
    deliver member item with given nr
    (check with existDinMemberNr before access to not defined item)

    possible errors:
      * Err_c::elNonexistent on failed search

    @param rui8_nr searched number
    @return reference to searched MemberItem
     @exception containerElementNonexistant
  */
  iDINItem_c& dinMemberNr(uint8_t rui8_nr) { return static_cast<iDINItem_c&>(DINMonitor_c::dinMemberNr(rui8_nr));};

  /**
    deliver member item with given GETY_POS, set pointed bool var to true on success
    and set a Member Array Iterator to the result
    @param rc_gtp searched GETY_POS
    @param pb_success bool pointer to store the success (true on success)
    @param pbc_iter optional member array iterator which points to searched DINItem_c on success
    @return reference to the searched item
  */
  iDINItem_c& dinMemberGtp(iGetyPos_c rc_gtp, bool *const pb_success, Vec_MemberIterator *const pbc_iter = NULL)
  { return static_cast<iDINItem_c&>(DINMonitor_c::dinMemberGtp(rc_gtp, pb_success, pbc_iter));};

#ifndef EXCLUDE_RARE_DIN_SYSTEM_CMD
  /* *********************************************************** */
  /** \name Seldom used DIN 9684 system cmds
    * Some system cmds about commanding and requesting state,
    * device error etc. are seldom used -> unnecessary Flash
    * memory waste, if not needed for special cases.             */
  /* *********************************************************** */
  /*\@{*/

  /**
    starts or release stop command for specific member;
    IMPORTANT: as protocol demands the stop command is repeated till explicit stop-release function call

    possible errors:
      * Err_c::elNonexistent the commanded member GETY_POS doesn't exist in member list
      * Err_c::lbsSysNoActiveLocalMember on missing own active ident
    @param rc_gtpTarget GETY_POS of the member, which should be stopped
    @param rb_toStop true -> start sending STOP commands; false -> release STOP sending mode
    @return true -> stop command sent without errors
  */
  bool commandStop(iGetyPos_c rc_gtpTarget, bool rb_toStop = true)
  { return DINMonitor_c::commandStop(rc_gtpTarget, rb_toStop);};

  /**
    starts or release stop command for all system members;
    IMPORTANT: as protocol demands the stop command is repeated till explicit stop-release function call

    possible errors:
      * Err_c::lbsSysNoActiveLocalMember on missing own active ident for sending the command
    @param rb_toStop true -> star sending STOP commands; false -> release STOP sending mode
    @return true -> one of the own identities was active with claimed address to send the global stop correctly
  */
  bool commandGlobalStop(bool rb_toStop = true)
  { return DINMonitor_c::commandGlobalStop(rb_toStop);};

  /**
    send ISO11783 or DIN9684 system msg to command status request for another member to switch to given mode

    possible errors:
      * Err_c::elNonexistent the commanded member GETY_POS doesn't exist in member list
      * Err_c::lbsSysNoActiveLocalMember on missing own active ident
    @param rc_gtp GETY_PSO of member, which switch state to OFF
    @param ren_itemState wanted state of item
    @return true -> stop command sent without errors
  */
  bool commandItemState(iGetyPos_c rc_gtp, IState_c::itemState_t ren_itemState)
  { return DINMonitor_c::commandItemState(rc_gtp, ren_itemState);};
  /*\@}*/
#endif
private:
  /** allow getIdinMonitorInstance() access to shielded base class.
      otherwise __IsoAgLib::getDinMonitorInstance() wouldn't be accepted by compiler
    */
  #if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
  friend iDINMonitor_c& getIdinMonitorInstance( uint8_t rui8_instance );
  #else
  friend iDINMonitor_c& getIdinMonitorInstance( void );
  #endif
  /**
    HIDDEN constructor for a iDINMonitor_c object instance which can optional
    set the configuration for send/receive for base msg type 1,2
    and calendar
    NEVER instantiate a variable of type iDINMonitor_c within application
    only access iDINMonitor_c via getBaseInstance() or getBaseInstance( int riLbsBusNr ) in case more than one ISO11783 or DIN9684 BUS is used for IsoAgLib
    */
  iDINMonitor_c() : DINMonitor_c() {};
};
#if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
  /** C-style function, to get access to the unique DINMonitor_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  inline iDINMonitor_c& getIdinMonitorInstance( uint8_t rui8_instance = 0 )
  { return static_cast<iDINMonitor_c&>(__IsoAgLib::getDinMonitorInstance(rui8_instance));};
#else
  /** C-style function, to get access to the unique DINMonitor_c singleton instance */
  inline iDINMonitor_c& getIdinMonitorInstance( void )
  { return static_cast<iDINMonitor_c&>(__IsoAgLib::getDinMonitorInstance());};
#endif

}
#endif
