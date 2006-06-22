/***************************************************************************
                          iidentitem_c.h  - object for managing identity/ies
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
#ifndef IIDENT_ITEM_H
#define IIDENT_ITEM_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include "impl/identitem_c.h"
#include <IsoAgLib/util/idevkey_c.h>

#ifdef USE_DIN_9684
  #include "DIN9684/idinitem_c.h"
#endif

#ifdef USE_ISO_11783
  #include "ISO11783/iisoitem_c.h"
  #include "ISO11783/iisoname_c.h"
#endif

// Begin Namespace IsoAgLib
namespace IsoAgLib {
// predeclare iISOTerminal_c, as this class is allowed to access private ( hidden )
// elements of this class exclusively ( see friend declaration at end of this file )
class iISOTerminal_c;

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
  IsoAgLib::iIdentItem_c c_itemLaterAddressClaim;
  IsoAgLib::iDevKey_c c_myType( 2, 0 );
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
class iIdentItem_c : protected __IsoAgLib::IdentItem_c  {
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
    @param rpc_slaveIsoNameList pointer to list of iISOName_c values, where the slave devices are defined.
      IsoAgLib will then send the needed "master indicates its slaves" messages on BUS
    @param ri_singletonVecKey optional key for selection of IsoAgLib instance (default 0)
  */
  iIdentItem_c(iDevKey_c* rpc_devKey = NULL,
      const uint8_t* rpb_name = NULL
      #if defined( USE_ISO_11783 ) && defined( USE_DIN_9684 )
      ,IState_c::protoOrder_t ren_protoOrder = IState_c::DinOnly
      #endif
      #ifdef USE_WORKING_SET
      ,int8_t ri8_slaveCount = -1, const iDevKey_c* rpc_slaveIsoNameList = NULL
      #endif
      , int ri_singletonVecKey = 0
      ) : IdentItem_c(rpc_devKey, rpb_name
      #if defined( USE_ISO_11783 ) && defined( USE_DIN_9684 )
      , ren_protoOrder
      #endif
      #ifdef USE_WORKING_SET
      , ri8_slaveCount, rpc_slaveIsoNameList
      #endif
      , ri_singletonVecKey) {};

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
    @param rpc_slaveIsoNameList pointer to list of iISOName_c values, where the slave devices are defined.
      IsoAgLib will then send the needed "master indicates its slaves" messages on BUS
    @param ri_singletonVecKey optional key for selection of IsoAgLib instance (default 0)
  */
  iIdentItem_c(iDevKey_c* rpc_devKey,
      const uint8_t* rpb_name,
      const uint8_t* rpb_isoName,
      uint8_t rb_wantedSa = 254, uint16_t rui16_saEepromAdr = 0xFFFF,
      #ifdef USE_WORKING_SET
      int8_t ri8_slaveCount = -1, const iDevKey_c* rpc_slaveIsoNameList = NULL,
      #endif
      int ri_singletonVecKey = 0
      ) : IdentItem_c(rpc_devKey, rpb_name, rpb_isoName, rb_wantedSa, rui16_saEepromAdr,
                      #ifdef USE_WORKING_SET
                      ri8_slaveCount, rpc_slaveIsoNameList,
                      #endif
                      ri_singletonVecKey) {};
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
    @param rpc_slaveIsoNameList pointer to list of iISOName_c values, where the slave devices are defined.
      IsoAgLib will then send the needed "master indicates its slaves" messages on BUS
    @param ri_singletonVecKey optional key for selection of IsoAgLib instance (default 0)
  */
  iIdentItem_c(iDevKey_c* rpc_devKey,
      const uint8_t* rpb_isoName,
      uint8_t rb_wantedSa, uint16_t rui16_saEepromAdr = 0xFFFF,
      #ifdef USE_WORKING_SET
      int8_t ri8_slaveCount = -1, const iDevKey_c* rpc_slaveIsoNameList = NULL,
      #endif
      int ri_singletonVecKey = 0
      ) : IdentItem_c(rpc_devKey, rpb_isoName, rb_wantedSa, rui16_saEepromAdr,
                      #ifdef USE_WORKING_SET
                      ri8_slaveCount, rpc_slaveIsoNameList,
                      #endif
                      ri_singletonVecKey) {};
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
    @param rpc_slaveIsoNameList pointer to list of iISOName_c values, where the slave devices are defined.
      IsoAgLib will then send the needed "master indicates its slaves" messages on BUS
    @param ri_singletonVecKey optional key for selection of IsoAgLib instance (default 0)
  */
  iIdentItem_c(iDevKey_c* rpc_devKey, const uint8_t* rpb_dinName,
    bool rb_selfConf, uint8_t rui8_indGroup, uint8_t rb_func, uint16_t rui16_manufCode,
    uint32_t rui32_serNo, uint8_t rb_wantedSa, uint16_t rui16_saEepromAdr, uint8_t rb_funcInst, uint8_t rb_ecuInst,
    #ifdef USE_WORKING_SET
    int8_t ri8_slaveCount = -1, const iDevKey_c* rpc_slaveIsoNameList = NULL,
    #endif
    int ri_singletonVecKey = 0)
   : IdentItem_c(rpc_devKey, rpb_dinName, rb_selfConf, rui8_indGroup, rb_func, rui16_manufCode, rui32_serNo,
      rb_wantedSa, rui16_saEepromAdr, rb_funcInst, rb_ecuInst,
      #ifdef USE_WORKING_SET
      ri8_slaveCount, rpc_slaveIsoNameList,
      #endif
      ri_singletonVecKey) {};
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
    @param ri_singletonVecKey optional key for selection of IsoAgLib instance (default 0)
  */
  iIdentItem_c(iDevKey_c* rpc_devKey,
    bool rb_selfConf, uint8_t rui8_indGroup, uint8_t rb_func, uint16_t rui16_manufCode,
    uint32_t rui32_serNo, uint8_t rb_wantedSa, uint16_t rui16_saEepromAdr, uint8_t rb_funcInst, uint8_t rb_ecuInst,
    #ifdef USE_WORKING_SET
    int8_t ri8_slaveCount = -1, const iDevKey_c* rpc_slaveIsoNameList = NULL,
    #endif
    int ri_singletonVecKey = 0)
   : IdentItem_c(rpc_devKey, rb_selfConf, rui8_indGroup, rb_func, rui16_manufCode, rui32_serNo,
      rb_wantedSa, rui16_saEepromAdr, rb_funcInst, rb_ecuInst,
      #ifdef USE_WORKING_SET
      ri8_slaveCount, rpc_slaveIsoNameList,
      #endif
      ri_singletonVecKey) {};
#endif
#ifdef USE_DIN_9684
  /** deliver pointer to DINItem_c in DINMonitor_c
      @return NULL -> either no DIN item or not yet registered in DINMonitor_c
    */
  iDINItem_c* getDinItem( void ) const { return static_cast<iDINItem_c*>(IdentItem_c::getDinItem()); };
  /**
    explicit start of activity for a DIN only instance
    @param rpc_devKey optional pointer to the DEV_KEY variable of this identity, which is resident somewhere else (f.e. main() task)
    @param rpb_name optional pointer to the name of this identity (DIN or ISO)
    @param ri_singletonVecKey optional key for selection of IsoAgLib instance (default 0)
  */
  void start(iDevKey_c* rpc_devKey = NULL,
      const uint8_t* rpb_name = NULL,
      int ri_singletonVecKey = 0
      ) { IdentItem_c::start(rpc_devKey, rpb_name, ri_singletonVecKey);};
#endif
#ifdef USE_ISO_11783
  /** deliver pointer to ISOItem_c in ISOMonitor_c
      @return NULL -> either no ISO item or not yet registered in ISOMonitor_c
    */
  iISOItem_c* getIsoItem( void ) const { return static_cast<iISOItem_c*>(IdentItem_c::getIsoItem()); };
  /**
    explicit start for DIN + ISO identity, which starts address claim for this identity
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
    @param rpc_slaveIsoNameList pointer to list of iISOName_c values, where the slave devices are defined.
      IsoAgLib will then send the needed "master indicates its slaves" messages on BUS
    @param ri_singletonVecKey optional key for selection of IsoAgLib instance (default 0)
  */
  void start(iDevKey_c* rpc_devKey,
      const uint8_t* rpb_name,
      const uint8_t* rpb_isoName,
      uint8_t rb_wantedSa = 254, uint16_t rui16_saEepromAdr = 0xFFFF,
      #ifdef USE_WORKING_SET
      int8_t ri8_slaveCount = -1, const iDevKey_c* rpc_slaveIsoNameList = NULL,
      #endif
      int ri_singletonVecKey = 0
      ) { IdentItem_c::start(rpc_devKey, rpb_name, rpb_isoName, rb_wantedSa, rui16_saEepromAdr,
                             #ifdef USE_WORKING_SET
                             ri8_slaveCount, rpc_slaveIsoNameList,
                             #endif
                             ri_singletonVecKey);};
  /**
    explicit start for ISO identity, which starts address claim for this identity
    @param rpc_devKey pointer to the DEV_KEY variable of this identity, which is resident somewhere else (f.e. main() task)
    @param rpb_isoName pointer to the 64Bit ISO11783 NAME of this identity
    @param rb_wantedSa optional preselected source adress (SA) of the ISO item (fixed SA or last time
        SA for self conf ISO device) (default 254 for free self-conf)
    @param rui16_saEepromAdr optional adress in EEPROM, where the according SA is stored
        (default 0xFFFF for NO EEPROM store)
    @param ri8_slaveCount amount of attached slave devices; default -1 == no master state;
      in case an address claim for the slave devices shall be sent by this ECU, they
      must get their own IdentItem_c instance ( then with default value -1 for ri8_slaveCount )
    @param rpc_slaveIsoNameList pointer to list of iISOName_c values, where the slave devices are defined.
      IsoAgLib will then send the needed "master indicates its slaves" messages on BUS
    @param ri_singletonVecKey optional key for selection of IsoAgLib instance (default 0)
  */
  void start(iDevKey_c* rpc_devKey,
      const uint8_t* rpb_isoName,
      uint8_t rb_wantedSa, uint16_t rui16_saEepromAdr = 0xFFFF,
      #ifdef USE_WORKING_SET
      int8_t ri8_slaveCount = -1, const iDevKey_c* rpc_slaveIsoNameList = NULL,
      #endif
      int ri_singletonVecKey = 0
      ) { IdentItem_c::start(rpc_devKey, rpb_isoName, rb_wantedSa, rui16_saEepromAdr,
                             #ifdef USE_WORKING_SET
                             ri8_slaveCount, rpc_slaveIsoNameList,
                             #endif
                             ri_singletonVecKey);};
  /**
    explicit start for DIN + ISO identity, which starts address claim for this identity
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
    @param rpc_slaveIsoNameList pointer to list of iISOName_c values, where the slave devices are defined.
      IsoAgLib will then send the needed "master indicates its slaves" messages on BUS
    @param ri_singletonVecKey optional key for selection of IsoAgLib instance (default 0)
  */
  void start(iDevKey_c* rpc_devKey, const uint8_t* rpb_dinName,
    bool rb_selfConf, uint8_t rui8_indGroup, uint8_t rb_func, uint16_t rui16_manufCode,
    uint32_t rui32_serNo, uint8_t rb_wantedSa, uint16_t rui16_saEepromAdr, uint8_t rb_funcInst, uint8_t rb_ecuInst,
    #ifdef USE_WORKING_SET
    int8_t ri8_slaveCount = -1, const iDevKey_c* rpc_slaveIsoNameList = NULL,
    #endif
    int ri_singletonVecKey = 0)
   { IdentItem_c::start(rpc_devKey, rpb_dinName, rb_selfConf, rui8_indGroup, rb_func, rui16_manufCode, rui32_serNo,
      rb_wantedSa, rui16_saEepromAdr, rb_funcInst, rb_ecuInst,
      #ifdef USE_WORKING_SET
      ri8_slaveCount, rpc_slaveIsoNameList,
      #endif
      ri_singletonVecKey);};
  /**
    explicit start for ISO identity, which starts address claim for this identity
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
    @param rpc_slaveIsoNameList pointer to list of iISOName_c values, where the slave devices are defined.
      IsoAgLib will then send the needed "master indicates its slaves" messages on BUS
    @param ri_singletonVecKey optional key for selection of IsoAgLib instance (default 0)
  */
  void start(iDevKey_c* rpc_devKey,
    bool rb_selfConf, uint8_t rui8_indGroup, uint8_t rb_func, uint16_t rui16_manufCode,
    uint32_t rui32_serNo, uint8_t rb_wantedSa, uint16_t rui16_saEepromAdr, uint8_t rb_funcInst, uint8_t rb_ecuInst,
    #ifdef USE_WORKING_SET
    int8_t ri8_slaveCount = -1, const iDevKey_c* rpc_slaveIsoNameList = NULL,
    #endif
    int ri_singletonVecKey = 0)
   { IdentItem_c::start(rpc_devKey, rb_selfConf, rui8_indGroup, rb_func, rui16_manufCode, rui32_serNo,
      rb_wantedSa, rui16_saEepromAdr, rb_funcInst, rb_ecuInst,
      #ifdef USE_WORKING_SET
      ri8_slaveCount, rpc_slaveIsoNameList,
      #endif
      ri_singletonVecKey);};
#endif



  /** send address release for DIN identities and perform other stop actions at end of lifetime
    * of a local ident
    */
  void close( void ) { IdentItem_c::close();};

  /** check if the ident has claimed address */
  bool isClaimedAddress( void ) const { return IdentItem_c::isClaimedAddress(); }

  /**
    retreive the actual DEV_KEY value of this ident item
    @return DEV_KEY code of this ident item instance
  */
  const iDevKey_c& devKey() const {return static_cast<const iDevKey_c&>(IdentItem_c::devKey());};
 private:
  friend class iISOTerminal_c;

};

}
#endif
