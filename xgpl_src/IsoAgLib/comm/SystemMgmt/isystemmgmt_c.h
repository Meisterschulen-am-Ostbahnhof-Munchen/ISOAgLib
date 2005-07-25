/***************************************************************************
                          isystem_c.h  - central IsoAgLib system management
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
 * Copyright (C) 2003 - 2004 Dipl.-Inform. Achim Spangler                 *
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
#ifndef ISYSTEM_MGMT_H
#define ISYSTEM_MGMT_H

#include "impl/systemmgmt_c.h"
#include "imonitoritem_c.h"
#include "istate_c.h"

#ifdef USE_DIN_9684
  #include "DIN9684/idinitem_c.h"
  #include "DIN9684/idinserviceitem_c.h"
#endif
#ifdef USE_ISO_11783
  #include "ISO11783/iisoitem_c.h"
#endif

// Begin Namespace IsoAgLib
namespace IsoAgLib {

class iScheduler_c;

/**
  Central IsoAgLib system management object, which provides information retrieval
  from devices independen of their protocol type. This is achieved by requesting
  both monitor lists in DINMonitor and ISOMontir ( if activated in protocol ).
  As long as the protocol type of the searched member is known and is constant, the
  corresponding monitor list should be accessed instead of this global interface class.
  *@author Dipl.-Inform. Achim Spangler
  */
class iSystemMgmt_c : private __IsoAgLib::SystemMgmt_c {
public:
  // Public methods
  /**
    initialise element which can't be done during construct
    -> insert FilterBox_c for receiving Scheduler_c SystemMgmt_c manages from CAN_IO

    possible errors:
      * dependant error on memory error in CANIO_c during insert of new FilterBox_c entry
  */
  void init() { SystemMgmt_c::init();};

  /* ************************************************************ */
  /** \name Search and access of local + remote  DIN + ISO member
    * Search for monitor list entries for local and remote
    * DIN and / or ISO member. Search keys are GETY_POS and source
    * number.                                                     */
  /* ************************************************************ */
  /*\@{*/

  /**
    deliver amount of DIN and ISO members in monitor lists which optional (!!)
    match the condition of address claim state
    @param rb_forceClaimedAddress true -> only members with claimed address are used
          (optional, default false)
    @return amount of DIN members with claimed address
  */
  uint8_t memberCnt(bool rb_forceClaimedAddress = false)
    {return SystemMgmt_c::memberCnt(rb_forceClaimedAddress);};
  /**
    deliver the n'th DIN and/or ISO member in monitor list which optional (!!)
    match the condition of address claim state
    check first with memberCnt if enough members are registered
    in the Monitor-List's
    @see memberCnt

    possible errors:
      * Err_c::range there exist less than rui8_ind members with claimed address
    @param rui8_ind position of the wanted member in the
                 sublist of members (first item has rui8_ind == 0 !!)
                 with the wanted property
    @param rb_forceClaimedAddress true -> only members with claimed address are used
          (optional, default false)
    @param ren_searchOrder define search order
      (IState_c::DinOnly or IState_c::IsoOnly or IState_c::DinIso or IState_c::IsoDin)
      (this parameter is only used if USE_ISO_11783 is defined, default check first din then iso)
    @return reference to searched element
  */
  iMonitorItem_c& memberInd(uint8_t rui8_ind, bool rb_forceClaimedAddress = false
  #if defined( USE_ISO_11783 ) && defined( USE_DIN_9684 )
  , IState_c::protoOrder_t ren_protoOrder = IState_c::DinIso
  #endif
  ){return *((iMonitorItem_c*)&SystemMgmt_c::memberInd(rui8_ind, rb_forceClaimedAddress
  #if defined( USE_ISO_11783 ) && defined( USE_DIN_9684 )
  , ren_protoOrder
  #endif
  ));};

  /**
    deliver the count of members in the DIN and/or ISO Monitor-Lists
    with given GETY (variable POS)
    which optional (!!) match the condition of address claim state
    @param rb_gety searched GETY code
    @param rb_forceClaimedAddress true -> only members with claimed address are used
          (optional, default false)
    @return count of members in Monitor-List with GETY == rb_gety
  */
  #if ( ! defined( PRT_INSTANCE_CNT ) ) || ( PRT_INSTANCE_CNT < 2 )
  static
  #endif
  uint8_t memberGetyCnt(uint8_t rb_gety, bool rb_forceClaimedAddress = false)
    {return SystemMgmt_c::memberGetyCnt(rb_gety, rb_forceClaimedAddress);};

  /**
    deliver one of the members with specific GETY  in the DIN and/or ISO lists
    which optional (!!) match the condition of address claim state
    check first with memberGetyCnt if enough members with wanted GETY and
    optional (!!) property are registered in Monitor-List
    @see memberGetyCnt

    possible errors:
      * Err_c::range there exist less than rui8_ind members with GETY rb_gety
   @param rb_gety searched GETY
   @param rui8_ind position of the wanted member in the
                 sublist of member with given GETY (first item has rui8_ind == 0 !!)
   @param rb_forceClaimedAddress true -> only members with claimed address are used
         (optional, default false)
   @param ren_searchOrder define search order
    (IState_c::DinOnly or IState_c::IsoOnly or IState_c::DinIso or IState_c::IsoDin)
    (this parameter is only used if USE_ISO_11783 is defined, default check first din then iso)
   @return reference to searched element
  */
  #if ( ! defined( PRT_INSTANCE_CNT ) ) || ( PRT_INSTANCE_CNT < 2 )
  static
  #endif
  iMonitorItem_c& memberGetyInd(uint8_t rb_gety, uint8_t rui8_ind, bool rb_forceClaimedAddress = false
    #if defined( USE_ISO_11783 ) && defined( USE_DIN_9684 )
    , IState_c::protoOrder_t ren_protoOrder = IState_c::DinIso
    #endif
    );

  /**
    check for member with given member no
      which optional (!!) match the condition of address claim state;
    search MemberMonitor and ISOMonitor in order defined by ren_searchOrder
    @param rui8_nr member no to search for
    @param ren_searchOrder define search order
      (IState_c::DinOnly or IState_c::IsoOnly or IState_c::DinIso or IState_c::IsoDin)
      (this parameter is only used if USE_ISO_11783 is defined, default check first din then iso)
    @return true -> one of the devices in the searched Monitor lists has the wanted member no
  */
  #if ( ! defined( PRT_INSTANCE_CNT ) ) || ( PRT_INSTANCE_CNT < 2 )
  static
  #endif
  bool existMemberNr(uint8_t rui8_nr
    #if defined( USE_ISO_11783 ) && defined( USE_DIN_9684 )
    , IState_c::protoOrder_t ren_protoOrder = IState_c::DinIso
    #endif
  ) {return SystemMgmt_c::existMemberNr(rui8_nr
    #if defined( USE_ISO_11783 ) && defined( USE_DIN_9684 )
    , ren_protoOrder
    #endif
    );};

  /**
    check for member with given member gtp
      which optional (!!) match the condition of address claim state;
    search MemberMonitor and ISOMonitor in order defined by ren_searchOrder
    @param rc_gtp member gtp to search for
    @param rb_forceClaimedAddress true -> only members with claimed address are used
          (optional, default false)
    @param ren_searchOrder define search order
      (IState_c::DinOnly or IState_c::IsoOnly or IState_c::DinIso or IState_c::IsoDin)
      (this parameter is only used if USE_ISO_11783 is defined, default check first din then iso)
    @return true -> one of the devices in the searched Monitor lists has the wanted member gtp
  */
  #if ( ! defined( PRT_INSTANCE_CNT ) ) || ( PRT_INSTANCE_CNT < 2 )
  static
  #endif
  bool existMemberGtp(const iGetyPos_c& rc_gtp, bool rb_forceClaimedAddress = false
    #if defined( USE_ISO_11783 ) && defined( USE_DIN_9684 )
    , IState_c::protoOrder_t ren_protoOrder = IState_c::DinIso
    #endif
  ) {return SystemMgmt_c::existMemberGtp(rc_gtp, rb_forceClaimedAddress
    #if defined( USE_ISO_11783 ) && defined( USE_DIN_9684 )
    , ren_protoOrder
    #endif
    );};


  /**
    deliver member with given member no;
    search MemberMonitor and ISOMonitor in order defined by ren_searchOrder
    (check with existMemberNr before access to not defined item)

    possible errors:
      * Err_c::elNonexistent on failed search

    @param rui8_nr member no to search for
    @param ren_searchOrder define search order
      (IState_c::DinOnly or IState_c::IsoOnly or IState_c::DinIso or IState_c::IsoDin)
      (this parameter is only used if USE_ISO_11783 is defined, default check first din then iso)
    @return reference to iMonitorItem_c with wanted number
     @exception containerElementNonexistant
  */
  #if ( ! defined( PRT_INSTANCE_CNT ) ) || ( PRT_INSTANCE_CNT < 2 )
  static
  #endif
  iMonitorItem_c& memberNr(uint8_t rui8_nr
    #if defined( USE_ISO_11783 ) && defined( USE_DIN_9684 )
    , IState_c::protoOrder_t ren_protoOrder = IState_c::DinIso
    #endif
  )
  {return ((iMonitorItem_c&)(SystemMgmt_c::memberNr(rui8_nr
  #if defined( USE_ISO_11783 ) && defined( USE_DIN_9684 )
  , ren_protoOrder
  #endif
  )));};

  /**
    check for member with given member gtp;
    search MemberMonitor and ISOMonitor in order defined by ren_searchOrder
    (check with existMemberGtp before access to not defined item)

    possible errors:
      * Err_c::elNonexistent on failed search

    @param rc_gtp member gtp to search for
    @param ren_searchOrder define search order
      (IState_c::DinOnly or IState_c::IsoOnly or IState_c::DinIso or IState_c::IsoDin)
      (this parameter is only used if USE_ISO_11783 is defined, default check first din then iso)
    @return reference to iMonitorItem_c with wanted number
     @exception containerElementNonexistant
  */
  #if ( ! defined( PRT_INSTANCE_CNT ) ) || ( PRT_INSTANCE_CNT < 2 )
  static
  #endif
  iMonitorItem_c& memberGtp(const iGetyPos_c& rc_gtp, bool rb_forceClaimedAddress = false
    #if defined( USE_ISO_11783 ) && defined( USE_DIN_9684 )
    , IState_c::protoOrder_t ren_protoOrder = IState_c::DinIso
    #endif
  )
  {return ((iMonitorItem_c&)(SystemMgmt_c::memberGtp(rc_gtp, rb_forceClaimedAddress
  #if defined( USE_ISO_11783 ) && defined( USE_DIN_9684 )
  , ren_protoOrder
  #endif
  )));};

  /**
    check if member is in one of the member lists for DIN or ISO with wanted GETY_POS,
    adopt POS if member with claimed address with other POS exist
    @param refb_gtp GETY_POS to search (-> it's updated if member with claimed address with other POS is found)
    @param ren_searchOrder define search order
      (IState_c::DinOnly or IState_c::IsoOnly or IState_c::DinIso or IState_c::IsoDin)
      (this parameter is only used if USE_ISO_11783 is defined, default check first din then iso)
    @return true -> member with claimed address with given GETY found (and refb_gtp has now its GETY_POS)
  */
  #if ( ! defined( PRT_INSTANCE_CNT ) ) || ( PRT_INSTANCE_CNT < 2 )
  static
  #endif
  bool gety2gtpClaimedAddress(iGetyPos_c &refc_gtp
  #if defined( USE_ISO_11783 ) && defined( USE_DIN_9684 )
  , IState_c::protoOrder_t ren_protoOrder = IState_c::DinIso
  #endif
  ){return SystemMgmt_c::gety2gtpClaimedAddress(refc_gtp
  #if defined( USE_ISO_11783 ) && defined( USE_DIN_9684 )
  , ren_protoOrder
  #endif
  );};
  /*\@}*/

  /* ********************************************************** */
  /** \name Search and access of local only DIN + ISO member
    * Search for monitor list entries for local DIN and/or ISO
    * member. Search keys are GETY_POS and source number.       */
  /* ********************************************************** */
  /*\@{*/

  /**
    check if one of the own local members is active with claimed address at ISO11783 or DIN9684
    @param ren_protoTypes select which active local member types should be checked
      (IState_c::DinOnly or IState_c::IsoOnly or IState_c::DinIso)
      (this parameter is only used if USE_ISO_11783 is defined, default check both)
    @return true -> at least one of the own din identities is active with claimed address at ISO11783 or DIN9684
    @see SystemMgmt_c::getActiveLocalMember
  */
  bool existActiveLocalMember(
    #if defined( USE_ISO_11783 ) && defined( USE_DIN_9684 )
    IState_c::protoOrder_t ren_protoOrder = IState_c::DinIso
    #endif
    )
    {return SystemMgmt_c::existActiveLocalMember(
    #if defined( USE_ISO_11783 ) && defined( USE_DIN_9684 )
    ren_protoOrder
    #endif
    );};
  /**
    delivers reference to the first active local member;
    send of ISO11783 or DIN9684 system msg demands telling a sender ident member no
    -> using the number of the first active member serves as default (f.e. for requesting other member names)
    can throw an preconditionViolation error, if none of the own identities is active/claimed address yet

    possible errors:
      * Err_c::lbsSysNoActiveLocalMember on missing own active ident
    @param ren_protoTypes select which active local member types should be checked
      (IState_c::DinOnly or IState_c::IsoOnly or IState_c::DinIso)
      (this parameter is only used if USE_ISO_11783 is defined, default check both)
    @return reference to the MonitorItem_c of the first active local member
      (MonitorItem_c is bas class of both ISOItem_c or DINItem_c which serves
       adress, gtp, itemState)
     @exception preconditionViolation
  */
  iMonitorItem_c& getActiveLocalMember(
    #if defined( USE_ISO_11783 ) && defined( USE_DIN_9684 )
    IState_c::protoOrder_t ren_protoOrder = IState_c::DinIso
    #endif
    )
    {return ((iMonitorItem_c&)(SystemMgmt_c::getActiveLocalMember(
    #if defined( USE_ISO_11783 ) && defined( USE_DIN_9684 )
    ren_protoOrder
    #endif
    )));};


  #ifdef USE_DIN_9684
  /**
    check if one of the own local DIN members is active with claimed address at ISO11783 or DIN9684;
    this variant is replaced during compile time by direct call to existActiveLocalMember,
    so tha no runtime overhead is caused;
    this allows consistent naming with other DIN member oriented functions
    @return true -> at least one of the own din identities is active with claimed address at ISO11783 or DIN9684
    @see SystemMgmt_c::getActiveLocalDinMember
  */
  bool existActiveLocalDinMember()
    {return SystemMgmt_c::existActiveLocalDinMember();};
  /**
    delivers reference to the first active local DIN member;
    send of ISO11783 or DIN9684 system msg demands telling a sender ident member no
    -> using the number of the first active member serves as default (f.e. for requesting other member names)
    can throw an preconditionViolation error, if none of the own identities is active/claimed address yet

    possible errors:
      * Err_c::lbsSysNoActiveLocalMember on missing own active ident
    @return reference to the DINItem_c of the first active local member
     @exception preconditionViolation
  */
  iDINItem_c& getActiveLocalDinMember()
    {return ((iDINItem_c&)(SystemMgmt_c::getActiveLocalDinMember()));};
  #endif

  #ifdef USE_ISO_11783
  /**
    check if one of the own local ISO members is active with claimed address at ISO11783 or DIN9684;
    this variant is replaced during compile time by direct call to existActiveLocalMember,
    so tha no runtime overhead is caused;
    this allows consistent naming with other ISO member oriented functions
    @return true -> at least one of the own iso identities is active with claimed address at ISO11783 or DIN9684
    @see SystemMgmt_c::getActiveLocalIsoMember
  */
  bool existActiveLocalIsoMember()
    {return SystemMgmt_c::existActiveLocalIsoMember();};
  /**
    delivers reference to the first active local ISO member;
    -> using the number of the first active member serves as default
    can throw an preconditionViolation error, if none of the own ISO identities is active/claimed address yet

    possible errors:
      * Err_c::lbsSysNoActiveLocalMember on missing own active ident
    @return reference to the DINItem_c of the first active local member
     @exception preconditionViolation
  */
  iISOItem_c& getActiveLocalIsoMember()
    {return static_cast<iISOItem_c&>(SystemMgmt_c::getActiveLocalIsoMember());};

  /*\@}*/

  #endif
 private:
  /** allow getISystemMgmtInstance() access to shielded base class.
      otherwise __IsoAgLib::getSystemMgmtInstance() wouldn't be accepted by compiler
    */
  #if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
  friend iSystemMgmt_c& getISystemMgmtInstance( uint8_t rui8_instance );
  #else
  friend iSystemMgmt_c& getISystemMgmtInstance( void );
  #endif
};
#if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
  /** C-style function, to get access to the unique SystemMgmt_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  inline iSystemMgmt_c& getISystemMgmtInstance( uint8_t rui8_instance = 0 )
  { return static_cast<iSystemMgmt_c&>(__IsoAgLib::getSystemMgmtInstance(rui8_instance));};
#else
  /** C-style function, to get access to the unique SystemMgmt_c singleton instance */
  inline iSystemMgmt_c& getISystemMgmtInstance( void )
  { return static_cast<iSystemMgmt_c&>(__IsoAgLib::getSystemMgmtInstance());};
#endif

}
#endif
