/***************************************************************************
                          identitem_c.h  - object for managing identity/ies
                                          of the ECU
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
#ifndef IDENT_ITEM_H
#define IDENT_ITEM_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include "baseitem_c.h"
#include "../impl/isoname_c.h"
#include "../impl/isoitem_c.h"


// Begin Namespace __IsoAgLib
namespace __IsoAgLib {
/**
  class for identity/ies which are managed by the actual ECU;
  new instances start in prepare address claim state and stay there for a randomic and serialNo dependent time;
  then they are inserted as announcing member in the monitoring list of the selected protocol type and start announcing;

  If the IsoAgLib shall not start immediately with address claim on definition of the variable
  ( e.g. if some data must be read for definition of local identity ),
  the address claim can be started later with explicit call of iIdentItem_c::start( ).
  This is also useful, if the corresponding protocol is selected during runtime.
  Example:
  \code
  bool     b_selfConf = true;
  uint8_t  ui8_indGroup = 2,
           ui8_func = 25,
           ui8_wantedSa = 128,
           ui8_funcInst = 0,
           ui8_ecuInst = 0,
           ui8_deviceClass = 2,
           ui8_deviceClassInst = 0;
  uint16_t ui16_manufCode = 0x7FF;
  uint32_t ui32_serNo = 27;

  IsoAgLib::iIdentItem_c c_isoItemLaterAddressClaim;
  // Start address claim of the local identity/member
  IsoAgLib::iIdentItem_c c_myIdent ( ui8_indGroup, 
                                     rui8_deviceClass, 
                                     ui8_deviceClassInst, 
                                     ui8_func,
                                     ui16_manufCode, 
                                     ui32_serNo); // further parameters use the defaults
  // ...
  // now start address claim
  c_isoItemLaterAddressClaim.init( ui8_indGroup, 
                                   ui8_deviceClass, 
                                   ui8_deviceClassInst, 
                                   ui8_func, 
                                   ui16_manufCode, 
                                   ui32_serNo, 
                                   ui8_wantedSa, 
                                   ui16_eepromAdrSa, 
                                   ui8_funcInst, 
                                   ui8_ecuInst );
  \endcode


  The IsoAgLib backend is responsible for answering all requests like
  RequestForClaimedSourceAdress ( ISO 11783 ) or
  RequestAllMemberNames ( DIN 9684 ).

  The application just has to call the main trigger function Scheduler_c::timeEvent().

  After the monitor list report a completed address claim the status changes to active/claimed address;

  Dependent on of defining of CHANGE_DEV_CLASS_INST_ON_CONFLICT in the project Makefile, a repeated address claim for a DIN 9684 ident is performed with different device class instance
  after some static conflicting alive messages.

  @short member ident item of this ECU
  @author Dipl.-Inform. Achim Spangler
  */
class IdentItem_c : public BaseItem_c
{

public: // methods

  /** A) default constructor:  will not do anything, create a not-yet-initialized identity. use "init" afterwards!
    * B) EEPROM address given: will read WantedSA/ISOName from EEPROM and start address claim for this identity
    * @param aui16_eepromAdr Address of EEPROM where the following 10 bytes have to be stored stored:
                             8 byte ISO-Name stored in LITTLE ENDIAN
                             1 byte preferred/last-used SA (0xFE for no preference)
                             1 byte flags, set to 0x00 initially (so the ISO-Name has a chance to change some of its
                                    instance(s) to avoid ISO-Name-conflicts on the bus BEFORE doing its initial address-claim
      @param ai_singletonVecKey optional key for selection of IsoAgLib instance, defaults to 0 at construction time!
    */
  IdentItem_c (uint16_t aui16_eepromAdr = 0xFFFF, int ai_singletonVecKey = 0);

  /** constructor for ISO identity, which starts address claim for this identity
      @param aui8_indGroup        select the industry group, 2 == agriculture
      @param ab_func              function code of the member (25 = network interconnect)
      @param aui16_manufCode      11bit manufactor code
      @param aui32_serNo          21bit serial number
      @param aui8_preferredSa     preferred source adress (SA) of the ISO item (fixed SA or last time
                                  SA for self conf ISO device) (default 254 for no special wish)
      @param aui16_eepromAdr      EEPROM adress, where the used IsoName / SA / flags are stored
                                  (default 0xFFFF for NO EEPROM store)
      @param ab_funcInst          function instance of this member (default 0)
      @param ab_ecuInst           ECU instance of this member (default 0)
      @param ab_selfConf          true -> this member as a self configurable source adress
      @param ai8_slaveCount       amount of attached slave devices; default -1 == no master state;
                                  in case an address claim for the slave devices shall be sent by this ECU, they
                                  must get their own IdentItem_c instance ( then with default value -1 for ai8_slaveCount )
      @param apc_slaveIsoNameList pointer to list of IsoName_c values, where the slave devices are defined.
                                  IsoAgLib will then send the needed "master indicates its slaves" messages on BUS
      @param ai_singletonVecKey   optional key for selection of IsoAgLib instance (default 0)
    */
  IdentItem_c(
    uint8_t aui8_indGroup, uint8_t aui8_devClass, uint8_t aui8_devClassInst, uint8_t ab_func, uint16_t aui16_manufCode,
    uint32_t aui32_serNo, uint8_t aui8_preferredSa = 254, uint16_t aui16_eepromAdr = 0xFFFF, uint8_t ab_funcInst = 0, uint8_t ab_ecuInst = 0, bool ab_selfConf = true,
    #ifdef USE_WORKING_SET
    int8_t ai8_slaveCount = -1, const IsoName_c* apc_slaveIsoNameList = NULL,
    #endif
    int ai_singletonVecKey = 0);

  /** init function for later start of address claim of an ISO identity (this can be only called once upon a default-constructed object)
      @param aui8_indGroup        select the industry group, 2 == agriculture
      @param ab_func              function code of the member (25 = network interconnect)
      @param aui16_manufCode      11bit manufactor code
      @param aui32_serNo          21bit serial number
      @param aui8_preferredSa      preferred source adress (SA) of the ISO item (fixed SA or last time
                                  SA for self conf ISO device) (default 254 for no special wish)
      @param aui16_eepromAdr      EEPROM adress, where the used IsoName / SA / flags are stored
                                  (default 0xFFFF for NO EEPROM store)
      @param ab_funcInst          function instance of this member (default 0)
      @param ab_ecuInst           ECU instance of this member (default 0)
      @param ab_selfConf          defaults to true -> this member as a self configurable source adress
      @param ai8_slaveCount       amount of attached slave devices; default -1 == no master state;
                                  in case an address claim for the slave devices shall be sent by this ECU, they
                                  must get their own IdentItem_c instance ( then with default value -1 for ai8_slaveCount )
      @param apc_slaveIsoNameList pointer to list of IsoName_c values, where the slave devices are defined.
                                  IsoAgLib will then send the needed "master indicates its slaves" messages on BUS
      @param ai_singletonVecKey   optional key for selection of IsoAgLib instance (default 0)
    */
  void init(
    uint8_t aui8_indGroup, uint8_t aui8_devClass, uint8_t aui8_devClassInst, uint8_t ab_func, uint16_t aui16_manufCode,
    uint32_t aui32_serNo, uint8_t ab_preferredSa, uint16_t aui16_saEepromAdr, uint8_t ab_funcInst = 0, uint8_t ab_ecuInst = 0, bool ab_selfConf = true,
    #ifdef USE_WORKING_SET
    int8_t ai8_slaveCount = -1, const IsoName_c* apc_slaveIsoNameList = NULL,
    #endif
    int ai_singletonVecKey = 0);


#ifdef USE_WORKING_SET
  /** set this Ident as Working Set Master, create all IsoItem_c slave items
      and set them to prepare address claim with master set to the responding master IsoItem_c
      @param ab_slaveCount how many slaves does the master have?
      @param apc_listSlaves the ISONames or whatever of all the slaves
    */
  void setToMaster (int8_t ai8_slaveCount=-1, const IsoName_c* apc_slaveIsoNameList=NULL);

  bool isMaster() const { return (mi8_slaveCount >= 0); }
#endif

  /** deliver pointer to IsoItem_c in IsoMonitor_c
      @return NULL -> either no ISO item or not yet registered in IsoMonitor_c
    */
  IsoItem_c* getIsoItem( void ) const { return mpc_isoItem; }

  /** deliver const reference to contained IsoName_c of this IdentItem_c */
  const IsoName_c& isoName() const { return mc_isoName; }

  /** deliver reference to contained IsoName_c of this IdentItem_c
    * ATTENTION: Use wisely!
    *            Only modify an IdentItem's ISOName if an address is NOT yet claimed!!!!!
    */
  IsoName_c& modifyableIsoName() { return mc_isoName; }

  /** reset the Addres Claim state by:
    * + reset IdentItem::IStat_c to IState_c::PreAddressClaim
    * + remove pointed IsoItem_c nodes and the respective pointer
    * @return true -> there was an item with given IsoName_c that has been resetted to IState_c::PreAddressClaim
    */
  void restartAddressClaim();

  /** default destructor which has nothing to do */
  ~IdentItem_c();

  /** every subsystem of IsoAgLib has explicit function for controlled shutdown
    * -> IdentItem_c::close() send address release for identities
    */
  void close( void );

  /** periodically called functions do perform
      time dependent actions

      possible errors:
          * dependant memory error in SystemMgmt_c caused by inserting item in monitor list
      @see Scheduler_c::timeEvent
      @see System_c::getTime
      @return true -> all planned activities performed
    */
  bool timeEvent( void );

  /** check for equality with another item
      @param arc_src compared IdentItem_c element
      @return true -> other item has same ISOName
    */
  bool operator==(IdentItem_c& arc_src) const {return (mc_isoName == arc_src.mc_isoName); }

  /** check for equality with given ISOName
      @param ac_isoName compared ISOName
      @return true -> item has same ISOName
    */
  bool operator==(const IsoName_c& ac_isoName) const {return (mc_isoName == ac_isoName); }

  /** check for difference to another item
      @param arc_src compared IdentItem_c element
      @return true -> other item has different ISOName
    */
  bool operator!=(IdentItem_c& arc_src) const {return (mc_isoName != arc_src.mc_isoName); }

  /** check for difference to given ISOName
      @param ac_isoName compared ISOName
      @return true -> other item has different ISOName
    */
  bool operator!=(const IsoName_c& ac_isoName) const {return (mc_isoName != ac_isoName); }

  /** check if this item has lower ISOName than another one
      @param arc_src compared IdentItem_c element
      @return true -> this item has lower ISOName than compared one
    */
  bool operator<(IdentItem_c& arc_src) const {return (mc_isoName < arc_src.mc_isoName); }

  /** check if this item has lower ISOName than given ISOName
      @param ac_isoName compared ISOName
      @return true -> this item has lower ISOName than compared one
    */
  bool operator<(const IsoName_c& ac_isoName) const {return (mc_isoName < ac_isoName); }

  /** check if given number is equal to member number of this item
      @param aui8_nr compared number
      @return true -> this item has same number
    */
  bool equalNr(uint8_t aui8_nr);

  /** check if the ident has claimed address */
  bool isClaimedAddress( void ) const { return (getIsoItem() != NULL) ? (getIsoItem()->itemState (IState_c::ClaimedAddress)) : (itemState (IState_c::ClaimedAddress)); }


protected: // methods

  /** calculate an individual number between [0,1000] to get an individual wait time before first
      address claim -> chance to avoid conflict with other system with same default ISOName
    */
  void setIndividualWait();

  /** periodically called functions do perform
      time dependent actions in prepare address claim state
      -> unify ISOName (Device Class / Device Class Instance)
      -> insert item in appropriate monitor lists and initiate address claim

      possible errors:
          * dependant memory error in SystemMgmt_c caused by inserting item in monitor list
      @see Scheduler_c::timeEvent
      @see SystemMgmt_c::timeEvent
      @see System_c::getTime
      @return true -> all planned activities performed
    */
  bool timeEventPreAddressClaim( void );

  /** periodically called functions do perform
      time dependent actions in active (address claim/claimed address) state
      -> call timeEvent for corresponding items in MemberMonitor/ISOMonitor
      -> initiate repeated address claim with changed Nr / ISOName if conflict with other item occured

      possible errors:
          * dependant memory error in SystemMgmt_c caused by inserting item in monitor list
      @see Scheduler_c::timeEvent
      @see SystemMgmt_c::timeEvent
      @see System_c::getTime
      @return true -> all planned activities performed
    */
  bool timeEventActive( void );


private: // methods

  void init (IsoName_c* apc_isoNameParam, uint8_t aui8_preferredSa, uint16_t aui16_eepromAdr,
    #ifdef USE_WORKING_SET
    int8_t ai8_slaveCount, const IsoName_c* apc_slaveIsoNameList,
    #endif
    int ai_singletonVecKey);

  /** HIDDEN! copy constructor for IdentItem_c
      NEVER copy a IdentItem_c around!!!!
      ONLY copy pointers to the wanted instance!!!
      ==> the copy constructor is defined as private, so that compiler
          detects this fault, and shows you this WARNING!!
      @param arc_src source
    */
  IdentItem_c(const IdentItem_c& arc_src) : BaseItem_c(arc_src) {}

  /** HIDDEN! assignment for IdentItem_c
      NEVER assign a IdentItem_c to another instance!!!!
      ==> the asignment is defined as private, so that compiler
          detects this fault, and shows you this WARNING!!
      @param arc_src source
    */
  IdentItem_c& operator=(const IdentItem_c& /* arc_src */){return *this; }


private: // attributes

  enum globalRunState_en { GlobalRunStateNeverClaimed = 0,
                           GlobalRunStateAlreadyClaimed = 1 };

  /** pointer to associated IsoItem */
  IsoItem_c* mpc_isoItem;

  /** "pointer" to EEPROM where the following block of data (10 bytes) are stored/retrieved */
  uint16_t mui16_eepromAdr;

  /** Global Run State of this identity */
  uint8_t mui8_globalRunState; // use globalRunState_en, but as it's ui8 we cannot directly use this type!
  /** Preferred-SA of this identity */
  uint8_t mui8_preferredSa;
  /** IsoName code of this identity */
  IsoName_c mc_isoName;

  #ifdef USE_WORKING_SET
  const IsoName_c* mpc_slaveIsoNameList;
  int8_t mi8_slaveCount;
  #endif

//  uint8_t ui8_lastUsedSa;

};

}
#endif
