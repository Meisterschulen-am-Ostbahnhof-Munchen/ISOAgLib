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
#ifdef USE_DIN_9684
  #include "../DIN9684/impl/dinitem_c.h"
#endif
#ifdef USE_ISO_11783
  #include "../ISO11783/impl/isoname_c.h"
  #include "../ISO11783/impl/isoitem_c.h"
#endif

#include "../../../util/impl/devkey_c.h"

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
  __IsoAgLib::IdentItem_c c_itemLaterAddressClaim;
  __IsoAgLib::DevKey_c c_myType( 2, 0 );
  // ...
  // now start as DIN
  c_itemLaterAddressClaim.start( &c_myType, "Hi-You" );
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
class IdentItem_c : public BaseItem_c  {
public:
  /**
    default constructor, which can optionally start address claim for this identity, if enough information
    is provided with the parameters (at least rpc_devKey, rpb_name [ ren_protoOrder if NO DIN ident shall be created)
    @param rpc_devKey optional pointer to the DEV_KEY variable of this identity, which is resident somewhere else (f.e. main() task)
    @param rpb_name optional pointer to the name of this identity (DIN or ISO)
    @param ren_protoOrder optional selection of wanted protocol type ( IState_c::DinOnly, IState_c::IsoOnly)
    @param ri8_slaveCount amount of attached slave devices; default -1 == no master state;
      in case an address claim for the slave devices shall be sent by this ECU, they
      must get their own IdentItem_c instance ( then with default value -1 for ri8_slaveCount )
    @param rpc_slaveIsoNameList pointer to list of ISOName_c values, where the slave devices are defined.
      IsoAgLib will then send the needed "master indicates its slaves" messages on BUS
    @param ri_singletonVecKey optional key for selection of IsoAgLib instance (default 0)
  */
  IdentItem_c(DevKey_c* rpc_devKey = NULL,
      const uint8_t* rpb_name = NULL
      #if defined( USE_ISO_11783 ) && defined( USE_DIN_9684 )
      ,IState_c::protoOrder_t ren_protoOrder = IState_c::DinOnly
      #endif
      #ifdef USE_WORKING_SET
      ,int8_t ri8_slaveCount = -1, const DevKey_c* rpc_slaveIsoNameList = NULL
      #endif
      , int ri_singletonVecKey = 0
      );

#if defined( USE_ISO_11783 ) && defined( USE_DIN_9684 )
  /**
    constructor for DIN + ISO identity, which starts address claim for this identity
    @param rpc_devKey pointer to the DEV_KEY variable of this identity, which is resident somewhere else (f.e. main() task)
    @param rpb_name pointer to the DIN name of this identity
    @param rpb_isoName pointer to the 64Bit ISO11783 NAME of this identity
    @param rb_wantedSa optional preselected source adress (SA) of the ISO item (fixed SA or last time
        SA for self conf ISO device) (default 254 for free self-conf)
    @param rui16_saEepromAdr optional adress in EEPROM, where the according SA is stored
        (default 0xFFFF for NO EEPROM store)
    @param ri8_slaveCount amount of attached slave devices; default -1 == no master state;
      in case an address claim for the slave devices shall be sent by this ECU, they
      must get their own IdentItem_c instance ( then with default value -1 for ri8_slaveCount )
    @param rpc_slaveIsoNameList pointer to list of ISOName_c values, where the slave devices are defined.
      IsoAgLib will then send the needed "master indicates its slaves" messages on BUS
    @param ri_singletonVecKey optional key for selection of IsoAgLib instance (default 0)
  */
  IdentItem_c(DevKey_c* rpc_devKey,
      const uint8_t* rpb_name,
      const uint8_t* rpb_isoName,
      uint8_t rb_wantedSa = 254, uint16_t rui16_saEepromAdr = 0xFFFF,
      #ifdef USE_WORKING_SET
      int8_t ri8_slaveCount = -1, const DevKey_c* rpc_slaveIsoNameList = NULL,
      #endif
      int ri_singletonVecKey = 0
      );
#endif

#ifdef USE_ISO_11783
/**
    constructor for ISO identity, which starts address claim for this identity
    @param rpc_devKey pointer to the DEV_KEY variable of this identity, which is resident somewhere else (f.e. main() task)
    @param rpb_isoName pointer to the 64Bit ISO11783 NAME of this identity
    @param rb_wantedSa optional preselected source adress (SA) of the ISO item (fixed SA or last time
        SA for self conf ISO device) (default 254 for free self-conf)
    @param rui16_saEepromAdr optional adress in EEPROM, where the according SA is stored
        (default 0xFFFF for NO EEPROM store)
    @param ri8_slaveCount amount of attached slave devices; default -1 == no master state;
      in case an address claim for the slave devices shall be sent by this ECU, they
      must get their own IdentItem_c instance ( then with default value -1 for ri8_slaveCount )
    @param rpc_slaveIsoNameList pointer to list of ISOName_c values, where the slave devices are defined.
      IsoAgLib will then send the needed "master indicates its slaves" messages on BUS
    @param ri_singletonVecKey optional key for selection of IsoAgLib instance (default 0)
  */
  IdentItem_c(DevKey_c* rpc_devKey,
      const uint8_t* rpb_isoName,
      uint8_t rb_wantedSa, uint16_t rui16_saEepromAdr = 0xFFFF,
      #ifdef USE_WORKING_SET
      int8_t ri8_slaveCount = -1, const DevKey_c* rpc_slaveIsoNameList = NULL,
      #endif
      int ri_singletonVecKey = 0
      );
  /**
    constructor for DIN + ISO identity, which starts address claim for this identity
    @param rpc_devKey pointer to the DEV_KEY variable of this identity, which is resident somewhere else (f.e. main() task)
    @param rpb_name pointer to the DIN name of this identity
    @param rb_selfConf true -> this member as a self configurable source adress
    @param rui8_indGroup select the industry group, 2 == agriculture
    @param rb_func function code of the member (25 = network interconnect)
    @param rui16_manufCode 11bit manufactor code
    @param rui32_serNo 21bit serial number
    @param rb_wantedSa preselected source adress (SA) of the ISO item (fixed SA or last time
        SA for self conf ISO device) (default 254 for free self-conf)
    @param rui16_saEepromAdr EEPROM adress, where the used source adress is stored for self_conf members
        (default 0xFFFF for NO EEPROM store)
    @param rb_funcInst function instance of this member (default 0)
    @param rb_ecuInst ECU instance of this member (default 0)
    @param ri8_slaveCount amount of attached slave devices; default -1 == no master state;
      in case an address claim for the slave devices shall be sent by this ECU, they
      must get their own IdentItem_c instance ( then with default value -1 for ri8_slaveCount )
    @param rpc_slaveIsoNameList pointer to list of ISOName_c values, where the slave devices are defined.
      IsoAgLib will then send the needed "master indicates its slaves" messages on BUS
    @param ri_singletonVecKey optional key for selection of IsoAgLib instance (default 0)
  */
  IdentItem_c(DevKey_c* rpc_devKey, const uint8_t* rpb_dinName,
    bool rb_selfConf, uint8_t rui8_indGroup, uint8_t rb_func, uint16_t rui16_manufCode,
    uint32_t rui32_serNo, uint8_t rb_wantedSa, uint16_t rui16_saEepromAdr, uint8_t rb_funcInst, uint8_t rb_ecuInst,
    #ifdef USE_WORKING_SET
    int8_t ri8_slaveCount = -1, const DevKey_c* rpc_slaveIsoNameList = NULL,
    #endif
    int ri_singletonVecKey = 0);
  /**
    constructor for ISO identity, which starts address claim for this identity
    @param rpc_devKey pointer to the DEV_KEY variable of this identity, which is resident somewhere else (f.e. main() task)
    @param rpb_name pointer to the DIN name of this identity
    @param rb_selfConf true -> this member as a self configurable source adress
    @param rui8_indGroup select the industry group, 2 == agriculture
    @param rb_func function code of the member (25 = network interconnect)
    @param rui16_manufCode 11bit manufactor code
    @param rui32_serNo 21bit serial number
    @param rb_wantedSa preselected source adress (SA) of the ISO item (fixed SA or last time
        SA for self conf ISO device) (default 254 for free self-conf)
    @param rui16_saEepromAdr EEPROM adress, where the used source adress is stored for self_conf members
        (default 0xFFFF for NO EEPROM store)
    @param rb_funcInst function instance of this member (default 0)
    @param rb_ecuInst ECU instance of this member (default 0)
    @param ri8_slaveCount amount of attached slave devices; default -1 == no master state;
      in case an address claim for the slave devices shall be sent by this ECU, they
      must get their own IdentItem_c instance ( then with default value -1 for ri8_slaveCount )
    @param rpc_slaveIsoNameList pointer to list of ISOName_c values, where the slave devices are defined.
      IsoAgLib will then send the needed "master indicates its slaves" messages on BUS
    @param ri_singletonVecKey optional key for selection of IsoAgLib instance (default 0)
  */
  IdentItem_c(DevKey_c* rpc_devKey,
    bool rb_selfConf, uint8_t rui8_indGroup, uint8_t rb_func, uint16_t rui16_manufCode,
    uint32_t rui32_serNo, uint8_t rb_wantedSa, uint16_t rui16_saEepromAdr, uint8_t rb_funcInst, uint8_t rb_ecuInst,
    #ifdef USE_WORKING_SET
    int8_t ri8_slaveCount = -1, const DevKey_c* rpc_slaveIsoNameList = NULL,
    #endif
    int ri_singletonVecKey = 0);

  /**
    set this Ident as Working Set Master, create all ISOItem_c slave items
    and set them to prepare address claim with master set to the responding master ISOItem_c
    @param rb_slaveCount how many slaves does the master have?
    @param rpc_listSlaves the DEVKEYs or whatever of all the slaves
  */
  void setToMaster (int8_t ri8_slaveCount=-1, const DevKey_c* rpc_slaveIsoNameList=NULL);
#endif

#ifdef USE_DIN_9684
  /** deliver pointer to DINItem_c in DINMonitor_c
      @return NULL -> either no DIN item or not yet registered in DINMonitor_c
    */
  DINItem_c* getDinItem( void ) const { return pc_memberItem; };
  /**
    explicit start of activity for a DIN only instance
    @param rpc_devKey optional pointer to the DEV_KEY variable of this identity, which is resident somewhere else (f.e. main() task)
    @param rpb_name optional pointer to the name of this identity (DIN or ISO)
    @param ri_singletonVecKey optional key for selection of IsoAgLib instance (default 0)
  */
  void start(DevKey_c* rpc_devKey,
      const uint8_t* rpb_name,
      int ri_singletonVecKey = 0
      );
#endif
#ifdef USE_ISO_11783
  /** deliver pointer to ISOItem_c in ISOMonitor_c
      @return NULL -> either no ISO item or not yet registered in ISOMonitor_c
    */
  ISOItem_c* getIsoItem( void ) const { return pc_isoItem; };
  /**
    explicit start of activity for DIN + ISO identity, which starts address claim for this identity
    @param rpc_devKey pointer to the DEV_KEY variable of this identity, which is resident somewhere else (f.e. main() task)
    @param rpb_name pointer to the DIN name of this identity
    @param rpb_isoName pointer to the 64Bit ISO11783 NAME of this identity
    @param rb_wantedSa optional preselected source adress (SA) of the ISO item (fixed SA or last time
        SA for self conf ISO device) (default 254 for free self-conf)
    @param rui16_saEepromAdr optional adress in EEPROM, where the according SA is stored
        (default 0xFFFF for NO EEPROM store)
    @param ri8_slaveCount amount of attached slave devices; default -1 == no master state;
      in case an address claim for the slave devices shall be sent by this ECU, they
      must get their own IdentItem_c instance ( then with default value -1 for ri8_slaveCount )
    @param rpc_slaveIsoNameList pointer to list of ISOName_c values, where the slave devices are defined.
      IsoAgLib will then send the needed "master indicates its slaves" messages on BUS
    @param ri_singletonVecKey optional key for selection of IsoAgLib instance (default 0)
  */
  void start(DevKey_c* rpc_devKey,
      const uint8_t* rpb_name,
      const uint8_t* rpb_isoName,
      uint8_t rb_wantedSa = 254, uint16_t rui16_saEepromAdr = 0xFFFF,
      #ifdef USE_WORKING_SET
      int8_t ri8_slaveCount = -1, const DevKey_c* rpc_slaveIsoNameList = NULL,
      #endif
      int ri_singletonVecKey = 0
      );
  /**
    explicit start of activity for ISO identity, which starts address claim for this identity
    @param rpc_devKey pointer to the DEV_KEY variable of this identity, which is resident somewhere else (f.e. main() task)
    @param rpb_isoName pointer to the 64Bit ISO11783 NAME of this identity
    @param rb_wantedSa optional preselected source adress (SA) of the ISO item (fixed SA or last time
        SA for self conf ISO device) (default 254 for free self-conf)
    @param rui16_saEepromAdr optional adress in EEPROM, where the according SA is stored
        (default 0xFFFF for NO EEPROM store)
    @param ri8_slaveCount amount of attached slave devices; default -1 == no master state;
      in case an address claim for the slave devices shall be sent by this ECU, they
      must get their own IdentItem_c instance ( then with default value -1 for ri8_slaveCount )
    @param rpc_slaveIsoNameList pointer to list of ISOName_c values, where the slave devices are defined.
      IsoAgLib will then send the needed "master indicates its slaves" messages on BUS
    @param ri_singletonVecKey optional key for selection of IsoAgLib instance (default 0)
  */
  void start(DevKey_c* rpc_devKey,
      const uint8_t* rpb_isoName,
      uint8_t rb_wantedSa, uint16_t rui16_saEepromAdr = 0xFFFF,
      #ifdef USE_WORKING_SET
      int8_t ri8_slaveCount = -1, const DevKey_c* rpc_slaveIsoNameList = NULL,
      #endif
      int ri_singletonVecKey = 0
      );
  /**
    explicit start of activity for DIN + ISO identity, which starts address claim for this identity
    @param rpc_devKey pointer to the DEV_KEY variable of this identity, which is resident somewhere else (f.e. main() task)
    @param rpb_name pointer to the DIN name of this identity
    @param rb_selfConf true -> this member as a self configurable source adress
    @param rui8_indGroup select the industry group, 2 == agriculture
    @param rb_func function code of the member (25 = network interconnect)
    @param rui16_manufCode 11bit manufactor code
    @param rui32_serNo 21bit serial number
    @param rb_wantedSa preselected source adress (SA) of the ISO item (fixed SA or last time
        SA for self conf ISO device) (default 254 for free self-conf)
    @param rui16_saEepromAdr EEPROM adress, where the used source adress is stored for self_conf members
        (default 0xFFFF for NO EEPROM store)
    @param rb_funcInst function instance of this member (default 0)
    @param rb_ecuInst ECU instance of this member (default 0)
    @param ri8_slaveCount amount of attached slave devices; default -1 == no master state;
      in case an address claim for the slave devices shall be sent by this ECU, they
      must get their own IdentItem_c instance ( then with default value -1 for ri8_slaveCount )
    @param rpc_slaveIsoNameList pointer to list of ISOName_c values, where the slave devices are defined.
      IsoAgLib will then send the needed "master indicates its slaves" messages on BUS
    @param ri_singletonVecKey optional key for selection of IsoAgLib instance (default 0)
  */
  void start(DevKey_c* rpc_devKey, const uint8_t* rpb_dinName,
    bool rb_selfConf, uint8_t rui8_indGroup, uint8_t rb_func, uint16_t rui16_manufCode,
    uint32_t rui32_serNo, uint8_t rb_wantedSa, uint16_t rui16_saEepromAdr, uint8_t rb_funcInst, uint8_t rb_ecuInst,
    #ifdef USE_WORKING_SET
    int8_t ri8_slaveCount = -1, const DevKey_c* rpc_slaveIsoNameList = NULL,
    #endif
    int ri_singletonVecKey = 0);
  /**
    explicit start of activity for ISO identity, which starts address claim for this identity
    @param rpc_devKey pointer to the DEV_KEY variable of this identity, which is resident somewhere else (f.e. main() task)
    @param rpb_name pointer to the DIN name of this identity
    @param rb_selfConf true -> this member as a self configurable source adress
    @param rui8_indGroup select the industry group, 2 == agriculture
    @param rb_func function code of the member (25 = network interconnect)
    @param rui16_manufCode 11bit manufactor code
    @param rui32_serNo 21bit serial number
    @param rb_wantedSa preselected source adress (SA) of the ISO item (fixed SA or last time
        SA for self conf ISO device) (default 254 for free self-conf)
    @param rui16_saEepromAdr EEPROM adress, where the used source adress is stored for self_conf members
        (default 0xFFFF for NO EEPROM store)
    @param rb_funcInst function instance of this member (default 0)
    @param rb_ecuInst ECU instance of this member (default 0)
    @param ri8_slaveCount amount of attached slave devices; default -1 == no master state;
      in case an address claim for the slave devices shall be sent by this ECU, they
      must get their own IdentItem_c instance ( then with default value -1 for ri8_slaveCount )
    @param rpc_slaveIsoNameList pointer to list of ISOName_c values, where the slave devices are defined.
      IsoAgLib will then send the needed "master indicates its slaves" messages on BUS
    @param ri_singletonVecKey optional key for selection of IsoAgLib instance (default 0)
  */
  void start(DevKey_c* rpc_devKey,
    bool rb_selfConf, uint8_t rui8_indGroup, uint8_t rb_func, uint16_t rui16_manufCode,
    uint32_t rui32_serNo, uint8_t rb_wantedSa, uint16_t rui16_saEepromAdr, uint8_t rb_funcInst, uint8_t rb_ecuInst,
    #ifdef USE_WORKING_SET
    int8_t ri8_slaveCount = -1, const DevKey_c* rpc_slaveIsoNameList = NULL,
    #endif
    int ri_singletonVecKey = 0);
#endif

  /**
   * reset the Addres Claim state by:
   * + reset IdentItem::IStat_c to IState_c::PreAddressClaim
   * + remove pointed ISOItem_c and DINItem_c nodes and the respective pointer
   * @return true -> there was an item with given DevKey_c that has been resetted to IState_c::PreAddressClaim
   */
  void restartAddressClaim();

  /** default destructor which has nothing to do */
  ~IdentItem_c();
  /** every subsystem of IsoAgLib has explicit function for controlled shutdown
    * -> IdentItem_c::close() send address release for DIN identities
    */
  void close( void );

  /**
    periodically called functions do perform
    time dependent actions

    possible errors:
        * dependant memory error in SystemMgmt_c caused by inserting item in monitor list
    @see Scheduler_c::timeEvent
    @see SystemMgmt_c::timeEvent
    @see System_c::getTime
    @return true -> all planned activities performed
  */
  bool timeEvent( void );

  /**
    retreive the actual DEV_KEY value of this ident item
    @return DEV_KEY code of this ident item instance
  */
  const DevKey_c& devKey() const {return *pc_devKey;};

  /**
    check for equality with another item
    @param rrefc_src compared IdentItem_c element
    @return true -> other item has same DEV_KEY
  */
  bool operator==(IdentItem_c& rrefc_src) const {return (*pc_devKey == *(rrefc_src.pc_devKey))?true:false;};
  /**
    check for equality with given DEV_KEY
    @param rc_devKey compared DEV_KEY
    @return true -> item has same DEV_KEY
  */
  bool operator==(const DevKey_c& rc_devKey) const {return (*pc_devKey == rc_devKey)?true:false;};
  /**
    check for difference to another item
    @param rrefc_src compared IdentItem_c element
    @return true -> other item has different DEV_KEY
  */
  bool operator!=(IdentItem_c& rrefc_src) const {return (*pc_devKey != *(rrefc_src.pc_devKey))?true:false;};
  /**
    check for difference to given DEV_KEY
    @param rc_devKey compared DEV_KEY
    @return true -> other item has different DEV_KEY
  */
  bool operator!=(const DevKey_c& rc_devKey) const {return (*pc_devKey != rc_devKey)?true:false;};
  /**
    check if this item has lower DEV_KEY than another one
    @param rrefc_src compared IdentItem_c element
    @return true -> this item has lower DEV_KEY than compared one
  */
  bool operator<(IdentItem_c& rrefc_src) const {return (*pc_devKey < *(rrefc_src.pc_devKey))?true:false;};
  /**
    check if this item has lower DEV_KEY than given DEV_KEY
    @param rc_devKey compared DEV_KEY
    @return true -> this item has lower DEV_KEY than compared one
  */
  bool operator<(const DevKey_c& rc_devKey) const {return (*pc_devKey < rc_devKey)?true:false;};
  /**
    check if given number is equal to member number of this item
    @param rui8_nr compared number
    @param ren_protoOrder select which protocol representation is compared
      (IState_c::DinOnly or IState_c::IsoOnly or IState_c::DinIso)
      (default IState_c::DinIso; this parameter is only compiled if
       USE_ISO_11783 is defined)
    @return true -> this item has same number
  */
  bool equalNr(uint8_t rui8_nr
    #if defined( USE_ISO_11783 ) && defined( USE_DIN_9684 )
    , IState_c::protoOrder_t ren_protoOrder = IState_c::DinIso
    #endif
  );

  /** check if the ident has claimed address */
  bool isClaimedAddress( void ) const { return itemState (IState_c::ClaimedAddress); }

protected:
  /**
    init local Ident Instance and set all internal values of an ident item with one function call
    @param rpc_devKey pointer to the variable with the DEV_KEY code of this item (default no timestamp setting)
    @param rpb_name DIN name (uint8_t[7] array) of this item ( NULL == only ISO )
    @param rpb_isoName potiner to 64bit ISO11783 NAME string ( NULL == only DIN )
    @param rb_wantedSa preselected source adress (SA) of the ISO item (fixed SA or last time
        SA for self conf ISO device) (default 254 for free self-conf)
    @param rui16_saEepromAdr adress in EEPROM, where the according SA is stored
        (default 0xFFFF for NO EEPROM store)
    @param ri_singletonVecKey optional key for selection of IsoAgLib instance (default 0)
  */
  void init(DevKey_c* rpc_devKey = NULL, const uint8_t* rpb_name = NULL
    #ifdef USE_ISO_11783
    , const uint8_t* rpb_isoName = NULL, uint8_t rb_wantedSa = 254, uint16_t rui16_saEepromAdr = 0xFFFF
    #endif
    , int ri_singletonVecKey = 0
  );

  /**
    calculate an individual number between [0,1000] to get an individual wait time before first
    address claim -> chance to avoid conflict with other system with same default DEVKEY
  */
  void setIndividualWait();
  /**
    periodically called functions do perform
    time dependent actions in prepare address claim state
    -> unify DevKey (Device Class / Device Class Instance)
    -> insert item in appropriate monitor lists and initiate address claim

    possible errors:
        * dependant memory error in SystemMgmt_c caused by inserting item in monitor list
    @see Scheduler_c::timeEvent
    @see SystemMgmt_c::timeEvent
    @see System_c::getTime
    @return true -> all planned activities performed
  */
  bool timeEventPreAddressClaim( void );
  /**
    periodically called functions do perform
    time dependent actions in active (address claim/claimed address) state
    -> call timeEvent for corresponding items in MemberMonitor (DIN) and ISOMonitor (ISO)
    -> initiate repeated address claim with changed Nr / DevKey if conflict with other item occured

    possible errors:
        * dependant memory error in SystemMgmt_c caused by inserting item in monitor list
    @see Scheduler_c::timeEvent
    @see SystemMgmt_c::timeEvent
    @see System_c::getTime
    @return true -> all planned activities performed
  */
  bool timeEventActive( void );
private:
// Private attributes
  /**
    HIDDEN! copy constructor for IdentItem_c
    NEVER copy a IdentItem_c around!!!!
    ONLY copy pointers to the wanted instance!!!
    ==> the copy constructor is defined as private, so that compiler
        detects this fault, and shows you this WARNING!!
    @param rrefc_src source
  */
  IdentItem_c(const IdentItem_c& rrefc_src) : BaseItem_c(rrefc_src) {};
  /**
    HIDDEN! assignment for IdentItem_c
    NEVER assign a IdentItem_c to another instance!!!!
    ==> the asignment is defined as private, so that compiler
        detects this fault, and shows you this WARNING!!
    @param rrefc_src source
  */
  IdentItem_c& operator=(const IdentItem_c& /* rrefc_src */){return *this;};
  /** pointer to devKey code of this identity */
  DevKey_c* pc_devKey;
  #ifdef USE_DIN_9684
  /** name of the ident */
  uint8_t cpName[8];
  DINItem_c* pc_memberItem;
  #endif
  #ifdef USE_ISO_11783
    ISOItem_c* pc_isoItem;
    #ifdef USE_WORKING_SET
    const DevKey_c* pc_slaveIsoNameList;
    int8_t i8_slaveCount;
    #endif
    uint8_t b_wantedSa;
    uint16_t ui16_saEepromAdr;
  #endif
};

}
#endif
