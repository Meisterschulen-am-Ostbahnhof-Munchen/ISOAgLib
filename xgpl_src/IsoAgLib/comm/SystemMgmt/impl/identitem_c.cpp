/***************************************************************************
                          identitem_c.cpp - object for managing identity/ies
                                          of the ECU
                             -------------------
    begin                : Fri Apr 07 2000
    copyright            : (C) 2000 - 2004 by Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
    type                 : Source
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
#include "identitem_c.h"
#include "systemmgmt_c.h"
#ifdef USE_DIN_9684
  #include "../DIN9684/impl/dinmonitor_c.h"
#endif
#ifdef USE_ISO_11783
  #include "../ISO11783/impl/isomonitor_c.h"
#endif
#ifdef USE_ISO_TERMINAL
  #include <IsoAgLib/comm/ISO_Terminal/impl/isoterminal_c.h>
#endif
#ifdef USE_PROCESS
  #include <IsoAgLib/comm/Process/impl/process_c.h>
#endif

#include <IsoAgLib/comm/Scheduler/impl/scheduler_c.h>
#include <IsoAgLib/driver/system/impl/system_c.h>
#include <IsoAgLib/driver/can/impl/canio_c.h>

namespace __IsoAgLib {

/**
  default constructor, which can optionally start address claim for this identity, if enough information
  is provided with the parameters (at least rpc_gtp, rpb_name [ ren_protoOrder if NO DIN ident shall be created)
  @param rpc_gtp optional pointer to the GETY_POS variable of this identity, which is resident somewhere else (f.e. main() task)
  @param rpb_name optional pointer to the name of this identity (DIN or ISO)
  @param ren_protoOrder optional selection of wanted protocol type ( IState_c::DinOnly, IState_c::IsoOnly)
  @param ri_singletonVecKey optional key for selection of IsoAgLib instance (default 0)
*/
IdentItem_c::IdentItem_c(GetyPos_c* rpc_gtp,
    const uint8_t* rpb_name
    #if defined( USE_ISO_11783 ) && defined( USE_DIN_9684 )
    ,IState_c::protoOrder_t ren_protoOrder
    #endif
    #if defined( USE_ISO_11783 )
    ,int8_t ri8_slaveCount, const GetyPos_c* rpc_slaveIsoNameList
    #endif
    , int ri_singletonVecKey )
  : BaseItem_c(System_c::getTime(), IState_c::Active, ri_singletonVecKey )
  #ifdef USE_DIN_9684
    , pc_memberItem( NULL )
  #endif
  #ifdef USE_ISO_11783
    , pc_isoItem( NULL ), pc_slaveIsoNameList ( rpc_slaveIsoNameList )
    , ui16_saEepromAdr( 0xFFFF ), i8_slaveCount ( ri8_slaveCount ), b_wantedSa( 254 )
  #endif
{ // check if called with complete default parameters -> don't start address claim in this case
  if ( rpc_gtp == NULL )
  {
    pc_gtp = NULL;
    #ifdef USE_DIN_9684
    CNAMESPACE::memset(cpName,'\0',7);
    #endif
  }
  else
  { // enough information for address claim is present
    #ifdef USE_DIN_9684
    CNAMESPACE::memset(cpName,'\0',7);
    #endif

    #if (!defined(USE_ISO_11783)) && defined(USE_DIN_9684)
    init( rpc_gtp, rpb_name, ri_singletonVecKey ) ;
    #elif defined(USE_ISO_11783) && defined(USE_DIN_9684)
    if ( ren_protoOrder == IState_c::DinOnly )
      init( rpc_gtp, rpb_name, NULL, 254, 0xFFF, ri_singletonVecKey );
    else
      init(rpc_gtp, NULL, rpb_name, 254, 0xFFFF, ri_singletonVecKey );
    #elif defined( USE_ISO_11783 ) && (!defined( USE_DIN_9684 ) )
    init(rpc_gtp, NULL, rpb_name, 254, 0xFFFF, ri_singletonVecKey );
    #endif
  }
}

#if defined( USE_ISO_11783 ) && defined( USE_DIN_9684 )
/**
  constructor for DIN + ISO identity, which starts address claim for this identity
  @param rpc_gtp pointer to the GETY_POS variable of this identity, which is resident somewhere else (f.e. main() task)
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
IdentItem_c::IdentItem_c(GetyPos_c* rpc_gtp,
    const uint8_t* rpb_name,
    const uint8_t* rpb_isoName,
    uint8_t rb_wantedSa, uint16_t rui16_saEepromAdr,
    int8_t ri8_slaveCount, const GetyPos_c* rpc_slaveIsoNameList,
    int ri_singletonVecKey )
  : BaseItem_c(System_c::getTime(), IState_c::Active, ri_singletonVecKey ), pc_memberItem( NULL ),
    pc_isoItem( NULL ), pc_slaveIsoNameList ( rpc_slaveIsoNameList ), ui16_saEepromAdr( rui16_saEepromAdr ), i8_slaveCount ( ri8_slaveCount ), b_wantedSa( rb_wantedSa )
{
  CNAMESPACE::memset(cpName,'\0',7);
  init( rpc_gtp, rpb_name, rpb_isoName, rb_wantedSa, rui16_saEepromAdr, ri_singletonVecKey );
}
#endif

#ifdef USE_ISO_11783
/**
  constructor for ISO identity, which starts address claim for this identity
  @param rpc_gtp pointer to the GETY_POS variable of this identity, which is resident somewhere else (f.e. main() task)
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
IdentItem_c::IdentItem_c(GetyPos_c* rpc_gtp,
    const uint8_t* rpb_isoName,
    uint8_t rb_wantedSa, uint16_t rui16_saEepromAdr,
    int8_t ri8_slaveCount, const GetyPos_c* rpc_slaveIsoNameList,
    int ri_singletonVecKey )
  : BaseItem_c(System_c::getTime(), IState_c::Active, ri_singletonVecKey ),
    #ifdef USE_DIN_9684
    pc_memberItem( NULL ),
    #endif
    pc_isoItem( NULL ), pc_slaveIsoNameList ( rpc_slaveIsoNameList ), ui16_saEepromAdr( rui16_saEepromAdr ), i8_slaveCount ( ri8_slaveCount ), b_wantedSa( rb_wantedSa )
{
  #ifdef USE_DIN_9684
  CNAMESPACE::memset(cpName,'\0',7);
  #endif
  init(rpc_gtp, NULL, rpb_isoName, rb_wantedSa, rui16_saEepromAdr, ri_singletonVecKey );
}
/**
  constructor for DIN + ISO identity, which starts address claim for this identity
  @param rpc_gtp pointer to the GETY_POS variable of this identity, which is resident somewhere else (f.e. main() task)
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
IdentItem_c::IdentItem_c(GetyPos_c* rpc_gtp, const uint8_t* rpb_dinName,
  bool rb_selfConf, uint8_t rui8_indGroup, uint8_t rb_func, uint16_t rui16_manufCode,
  uint32_t rui32_serNo, uint8_t rb_wantedSa, uint16_t rui16_saEepromAdr, uint8_t rb_funcInst,
  uint8_t rb_ecuInst, int8_t ri8_slaveCount, const GetyPos_c* rpc_slaveIsoNameList, int ri_singletonVecKey )
  : BaseItem_c(System_c::getTime(), IState_c::Active, ri_singletonVecKey ),
    #ifdef USE_DIN_9684
    pc_memberItem( NULL ),
    #endif
    pc_isoItem( NULL ), pc_slaveIsoNameList ( rpc_slaveIsoNameList ), ui16_saEepromAdr( rui16_saEepromAdr ), i8_slaveCount ( ri8_slaveCount ), b_wantedSa( rb_wantedSa )

{
  #ifdef USE_DIN_9684
  CNAMESPACE::memset(cpName,'\0',7);
  #endif
  if ( rpc_gtp != NULL )
  {
    ISOName_c c_isoName(rb_selfConf, rui8_indGroup, rpc_gtp->getGety(), rpc_gtp->getPos(),
          rb_func, rui16_manufCode, rui32_serNo, rb_funcInst, rb_ecuInst);
    init(rpc_gtp, rpb_dinName, c_isoName.outputString(), rb_wantedSa, rui16_saEepromAdr, ri_singletonVecKey );
  }
  else
  {
    init( rpc_gtp, rpb_dinName, NULL, rb_wantedSa, rui16_saEepromAdr, ri_singletonVecKey );
  }
}
/**
  constructor for ISO identity, which starts address claim for this identity
  @param rpc_gtp pointer to the GETY_POS variable of this identity, which is resident somewhere else (f.e. main() task)
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
IdentItem_c::IdentItem_c(GetyPos_c* rpc_gtp,
  bool rb_selfConf, uint8_t rui8_indGroup, uint8_t rb_func, uint16_t rui16_manufCode,
  uint32_t rui32_serNo, uint8_t rb_wantedSa, uint16_t rui16_saEepromAdr, uint8_t rb_funcInst,
  uint8_t rb_ecuInst, int8_t ri8_slaveCount, const GetyPos_c* rpc_slaveIsoNameList, int ri_singletonVecKey )
  : BaseItem_c(System_c::getTime(), IState_c::Active, ri_singletonVecKey ),
    #ifdef USE_DIN_9684
    pc_memberItem( NULL ),
    #endif
    pc_isoItem( NULL ), pc_slaveIsoNameList ( rpc_slaveIsoNameList ), ui16_saEepromAdr( rui16_saEepromAdr ), i8_slaveCount ( ri8_slaveCount ), b_wantedSa( rb_wantedSa )
{
  #ifdef USE_DIN_9684
  CNAMESPACE::memset(cpName,'\0',7);
  #endif
  if ( rpc_gtp != NULL )
  {
    ISOName_c c_isoName(rb_selfConf, rui8_indGroup, rpc_gtp->getGety(), rpc_gtp->getPos(),
          rb_func, rui16_manufCode, rui32_serNo, rb_funcInst, rb_ecuInst);
    init(rpc_gtp, NULL, c_isoName.outputString(), rb_wantedSa, rui16_saEepromAdr, ri_singletonVecKey );
  }
  else
  {
    init( rpc_gtp, NULL, NULL, rb_wantedSa, rui16_saEepromAdr, ri_singletonVecKey );
  }
}
#endif


#ifdef USE_DIN_9684
/**
  explicit start of activity for a DIN only instance
  @param rpc_gtp optional pointer to the GETY_POS variable of this identity, which is resident somewhere else (f.e. main() task)
  @param rpb_name optional pointer to the name of this identity (DIN or ISO)
  @param ri_singletonVecKey optional key for selection of IsoAgLib instance (default 0)
*/
void IdentItem_c::start(GetyPos_c* rpc_gtp,
    const uint8_t* rpb_name,
    int ri_singletonVecKey )
{
  close(); // if this item has already claimed an address -> revert this first
  BaseItem_c::set( System_c::getTime(), ri_singletonVecKey );
  CNAMESPACE::memset(cpName,'\0',7);
  #if (!defined(USE_ISO_11783))
    init( rpc_gtp, rpb_name, ri_singletonVecKey ) ;
  #else
    init( rpc_gtp, rpb_name, NULL, 254, 0xFFF, ri_singletonVecKey );
  #endif
}
#endif
#ifdef USE_ISO_11783
/**
  explicit start  for DIN + ISO identity, which starts address claim for this identity
  @param rpc_gtp pointer to the GETY_POS variable of this identity, which is resident somewhere else (f.e. main() task)
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
void IdentItem_c::start(GetyPos_c* rpc_gtp,
    const uint8_t* rpb_name,
    const uint8_t* rpb_isoName,
    uint8_t rb_wantedSa, uint16_t rui16_saEepromAdr,
    int8_t ri8_slaveCount, const GetyPos_c* rpc_slaveIsoNameList,
    int ri_singletonVecKey )
{
  close(); // if this item has already claimed an address -> revert this first
  BaseItem_c::set( System_c::getTime(), ri_singletonVecKey );
  b_wantedSa = rb_wantedSa;
  ui16_saEepromAdr = rui16_saEepromAdr;
  i8_slaveCount = ri8_slaveCount;
  pc_slaveIsoNameList = rpc_slaveIsoNameList;
  #ifdef USE_DIN_9684
  CNAMESPACE::memset(cpName,'\0',7);
  #endif
  init( rpc_gtp, rpb_name, rpb_isoName, rb_wantedSa, rui16_saEepromAdr, ri_singletonVecKey );
}
/**
  explicit start  for ISO identity, which starts address claim for this identity
  @param rpc_gtp pointer to the GETY_POS variable of this identity, which is resident somewhere else (f.e. main() task)
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
void IdentItem_c::start(GetyPos_c* rpc_gtp,
    const uint8_t* rpb_isoName,
    uint8_t rb_wantedSa, uint16_t rui16_saEepromAdr,
    int8_t ri8_slaveCount, const GetyPos_c* rpc_slaveIsoNameList,
    int ri_singletonVecKey )
{
  close(); // if this item has already claimed an address -> revert this first
  BaseItem_c::set( System_c::getTime(), ri_singletonVecKey );
  b_wantedSa = rb_wantedSa;
  ui16_saEepromAdr = rui16_saEepromAdr;
  i8_slaveCount = ri8_slaveCount;
  pc_slaveIsoNameList = rpc_slaveIsoNameList;
  #ifdef USE_DIN_9684
  CNAMESPACE::memset(cpName,'\0',7);
  #endif
  init(rpc_gtp, NULL, rpb_isoName, rb_wantedSa, rui16_saEepromAdr, ri_singletonVecKey );
}
/**
  explicit start  for DIN + ISO identity, which starts address claim for this identity
  @param rpc_gtp pointer to the GETY_POS variable of this identity, which is resident somewhere else (f.e. main() task)
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
void IdentItem_c::start(GetyPos_c* rpc_gtp, const uint8_t* rpb_dinName,
  bool rb_selfConf, uint8_t rui8_indGroup, uint8_t rb_func, uint16_t rui16_manufCode,
  uint32_t rui32_serNo, uint8_t rb_wantedSa, uint16_t rui16_saEepromAdr, uint8_t rb_funcInst,
  uint8_t rb_ecuInst, int8_t ri8_slaveCount, const GetyPos_c* rpc_slaveIsoNameList, int ri_singletonVecKey )
{
  close(); // if this item has already claimed an address -> revert this first
  BaseItem_c::set( System_c::getTime(), ri_singletonVecKey );
  b_wantedSa = rb_wantedSa;
  ui16_saEepromAdr = rui16_saEepromAdr;
  i8_slaveCount = ri8_slaveCount;
  pc_slaveIsoNameList = rpc_slaveIsoNameList;
  #ifdef USE_DIN_9684
  CNAMESPACE::memset(cpName,'\0',7);
  #endif
  if ( rpc_gtp != NULL )
  {
    ISOName_c c_isoName(rb_selfConf, rui8_indGroup, rpc_gtp->getGety(), rpc_gtp->getPos(),
          rb_func, rui16_manufCode, rui32_serNo, rb_funcInst, rb_ecuInst);
    init(rpc_gtp, rpb_dinName, c_isoName.outputString(), rb_wantedSa, rui16_saEepromAdr, ri_singletonVecKey );
  }
  else
  {
    init( rpc_gtp, rpb_dinName, NULL, rb_wantedSa, rui16_saEepromAdr, ri_singletonVecKey );
  }
}
/**
  explicit start  for ISO identity, which starts address claim for this identity
  @param rpc_gtp pointer to the GETY_POS variable of this identity, which is resident somewhere else (f.e. main() task)
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
void IdentItem_c::start(GetyPos_c* rpc_gtp,
  bool rb_selfConf, uint8_t rui8_indGroup, uint8_t rb_func, uint16_t rui16_manufCode,
  uint32_t rui32_serNo, uint8_t rb_wantedSa, uint16_t rui16_saEepromAdr, uint8_t rb_funcInst,
  uint8_t rb_ecuInst, int8_t ri8_slaveCount, const GetyPos_c* rpc_slaveIsoNameList, int ri_singletonVecKey )
{
  close(); // if this item has already claimed an address -> revert this first
  BaseItem_c::set( System_c::getTime(), ri_singletonVecKey );
  b_wantedSa = rb_wantedSa;
  ui16_saEepromAdr = rui16_saEepromAdr;
  i8_slaveCount = ri8_slaveCount;
  pc_slaveIsoNameList = rpc_slaveIsoNameList;
  #ifdef USE_DIN_9684
  CNAMESPACE::memset(cpName,'\0',7);
  #endif
  if ( rpc_gtp != NULL )
  {
    ISOName_c c_isoName(rb_selfConf, rui8_indGroup, rpc_gtp->getGety(), rpc_gtp->getPos(),
          rb_func, rui16_manufCode, rui32_serNo, rb_funcInst, rb_ecuInst);
    init(rpc_gtp, NULL, c_isoName.outputString(), rb_wantedSa, rui16_saEepromAdr, ri_singletonVecKey );
  }
  else
  {
    init( rpc_gtp, NULL, NULL, rb_wantedSa, rui16_saEepromAdr, ri_singletonVecKey );
  }
}
#endif


/** default destructor which has nothing to do */
IdentItem_c::~IdentItem_c(){
  close();
}
/** every subsystem of IsoAgLib has explicit function for controlled shutdown
  * -> IdentItem_c::close() send address release for DIN identities
  */
void IdentItem_c::close( void )
{
  #ifdef USE_DIN_9684
  if ( (pc_memberItem != NULL)
    && (pc_memberItem->gtp() == gtp())
    && (pc_memberItem->itemState(IState_c::ClaimedAddress))
    )
    { // item has claimed address -> send unregister cmd
      // send an adress release for according number
      // pc_memberItem->sendAdressRelease();
      // delete item from memberList - and force send of address release
      getDinMonitorInstance4Comm().deleteDinMemberGtp(gtp(), true );
      pc_memberItem = NULL;
    }
  #endif
  #ifdef USE_ISO_11783
  if ( (pc_isoItem != NULL)
    && (pc_isoItem->gtp() == gtp())
    && (pc_isoItem->itemState(IState_c::ClaimedAddress))
    )
    { // item has claimed address -> send unregister cmd
      // delete item from memberList
      getIsoMonitorInstance4Comm().deleteIsoMemberGtp(gtp());
      pc_isoItem = NULL;
      clearItemState( IState_c::ClaimedAddress );
    }
  #endif
  // unregister in SystemMgmt_c
  getSystemMgmtInstance4Comm().unregisterClient( this );
}

/**
  init local Ident Instance and set all internal values of an ident item with one function call
  @param rpc_gtp pointer to the variable with the GETY_POS code of this item (default no timestamp setting)
  @param rpb_name DIN name (uint8_t[7] array) of this item ( NULL == only ISO )
  @param rpb_isoName potiner to 64bit ISO11783 NAME string ( NULL == only DIN )
  @param rb_wantedSa preselected source adress (SA) of the ISO item (fixed SA or last time
      SA for self conf ISO device) (default 254 for free self-conf)
  @param rui16_saEepromAdr adress in EEPROM, where the according SA is stored
      (default 0xFFFF for NO EEPROM store)
  @param ri_singletonVecKey optional key for selection of IsoAgLib instance (default 0)
*/
void IdentItem_c::init(GetyPos_c* rpc_gtp, const uint8_t*
    #ifdef USE_DIN_9684
    rpb_name // ifdef'd to avoid compiler warning in DIN case where this parameter is not used!
    #endif
    #ifdef USE_ISO_11783
    , const uint8_t* rpb_isoName, uint8_t rb_wantedSa, uint16_t rui16_saEepromAdr
    #endif
    , int ri_singletonVecKey
  )
{
  BaseItem_c::set( System_c::getTime(), IState_c::Active, ri_singletonVecKey );
  // register in SystemMgmt_c
  getSystemMgmtInstance4Comm().registerClient( this );

  if (rpc_gtp == NULL)
  {
    if ( pc_gtp == NULL )
    { // set precondition error
      getLbsErrInstance().registerError( LibErr_c::Precondition, LibErr_c::LbsSystem );
      return; // return with error information
    }
  }
  else pc_gtp = rpc_gtp;
  #ifdef USE_DIN_9684
  if (rpb_name != NULL)
  {
    CNAMESPACE::memmove(cpName,rpb_name,7);
    setItemState(IState_c::Din);
  }
  #endif

  #ifdef USE_ISO_11783
  if (rpb_isoName != NULL)
  {
    if ( pc_gtp != NULL )
    { // set all values from rpb_isoName with exception of device class / -instance
      const uint8_t ui8_deviceClass = pc_gtp->getGety();
      const uint8_t ui8_deviceClassInstance = pc_gtp->getPos();
      // set the ISOName_c into the pointed GTP
      pc_gtp->set( rpb_isoName );
      // store the device class / -instance values back
      pc_gtp->set( ui8_deviceClass, ui8_deviceClassInstance );
    }
    setItemState(IState_c::Iso);
  }
  if (rb_wantedSa != 254) b_wantedSa = rb_wantedSa;
  if (rui16_saEepromAdr != 0xFFFF) ui16_saEepromAdr = rui16_saEepromAdr;
  #endif

  if (pc_gtp != NULL)
  { // set state to prepare address claim
    setItemState(IState_c::itemState_t(IState_c::PreAddressClaim | IState_c::Local));
    // set time to individual wait time
    setIndividualWait();
    #ifdef USE_ISO_11783
    if ( itemState( IState_c::Iso ) )
    { // trigger a request for claimed addreses,
      // if local item is activated as ISO 11783
      getIsoMonitorInstance4Comm().sendRequestForClaimedAddress();
    }
    #endif
  }
};

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
bool IdentItem_c::timeEvent( void )
{
   if ( Scheduler_c::getAvailableExecTime() == 0 ) return false;
   if ( (!itemState(IState_c::Off)) && (checkUpdateTime(10)) )
   { // the system is not switched to off state
    // and last timed action is 1sec lasted
     // -> check for possible state switch or needed actions

    // if state is prepare address claim check if gtp unique and insert item in list
    if (itemState(IState_c::PreAddressClaim)) return timeEventPreAddressClaim();
    else return timeEventActive();
  }
  else return true;
};

/**
  periodically called functions do perform
  time dependent actions in prepare address claim state
  -> unify Gtp (Device Class / Device Class Instance)
  -> insert item in appropriate monitor lists and initiate address claim

  possible errors:
      * dependant memory error in SystemMgmt_c caused by inserting item in monitor list
  @see Scheduler_c::timeEvent
  @see SystemMgmt_c::timeEvent
  @see System_c::getTime
  @return true -> all planned activities performed
*/
bool IdentItem_c::timeEventPreAddressClaim( void ) {
  bool b_gtpSuccessfulUnified = false;
  // check if gtp is unique and change if needed (to avoid adress conflict on Scheduler_c BUS)
  #ifdef USE_DIN_9684
    #ifdef USE_ISO_11783
  if (itemState(IState_c::Din))
    #endif
    b_gtpSuccessfulUnified = getDinMonitorInstance4Comm().unifyDinGtp(*pc_gtp);
  #endif // USE_DIN_9684
  #ifdef USE_ISO_11783
  // just update pointed GTP field - NAME is updated during insert of
  // new ISO item within ISOMonitor::insertIsoMember
    #ifdef USE_DIN_9684
  if (itemState(IState_c::Iso))
    #endif
    b_gtpSuccessfulUnified = getIsoMonitorInstance4Comm().unifyIsoGtp(*pc_gtp);

  #endif // USE_ISO_11783

  #ifdef USE_DIN_9684
  if ( (itemState(IState_c::Din)) && (b_gtpSuccessfulUnified) ) {
  // insert element in list
  getDinMonitorInstance4Comm().insertDinMember(gtp(), cpName);
  pc_memberItem = &(getDinMonitorInstance4Comm().dinMemberGtp(gtp()));
  // set item as member and as own identity and overwrite old value
  pc_memberItem->setItemState
    (IState_c::itemState_t(IState_c::Member | IState_c::Local | IState_c::Din));
  // start announcing of item
  pc_memberItem->startAddressClaim(true);
  }
  #endif
  #ifdef USE_ISO_11783
  if ( (itemState(IState_c::Iso)) && (b_gtpSuccessfulUnified) )  {
    // insert element in list
    getIsoMonitorInstance4Comm().insertIsoMember(gtp(), b_wantedSa,
      IState_c::itemState_t(IState_c::Member | IState_c::Local | IState_c::Iso | IState_c::PreAddressClaim), ui16_saEepromAdr);
    pc_isoItem = &(getIsoMonitorInstance4Comm().isoMemberGtp(gtp()));
    // set item as member and as own identity and overwrite old value
    pc_isoItem->setItemState
      (IState_c::itemState_t(IState_c::Member | IState_c::Local | IState_c::Iso | IState_c::PreAddressClaim));

    // insert all slave ISOItem objects (of not yet there) and set me as their master
    setToMaster ();

    pc_isoItem->timeEvent();
}
  #endif
  // set ident_item state to claim address
  if (b_gtpSuccessfulUnified) setItemState(IState_c::AddressClaim);
  return true;
}

/**
  periodically called functions do perform
  time dependent actions in active (address claim/claimed address) state
  -> call timeEvent for corresponding items in MemberMonitor (DIN) and ISOMonitor (ISO)
  -> initiate repeated address claim with changed Nr / Gtp if conflict with other item occured

  possible errors:
      * dependant memory error in SystemMgmt_c caused by inserting item in monitor list
  @see Scheduler_c::timeEvent
  @see SystemMgmt_c::timeEvent
  @see System_c::getTime
  @param ri32_time optional timestamp to coordinate different periodic actions with one time and to shrink System_c::getTime calls
*/
bool IdentItem_c::timeEventActive( void ) {
  // only change POS and repeated address claim on adress conflicts, if following define
  // is set in masterHeader
  #ifdef CHANGE_POS_ON_CONFLICT
  // because of errors caused by terminals, which doesn't claim correctly an address
  // don't do conflict changing of POS
  if ((pc_memberItem->affectedConflictCnt() > 3) && (itemState(IState_c::Din)))
  { // repeated address claim needed
    // send an adress release for according number
    pc_memberItem->sendAdressRelease();
    // delete item from memberList
    getDinMonitorInstance4Comm().deleteDinMemberGtp(gtp());
    pc_memberItem = NULL;
    // restart with init
    init();
    // set time to individual wait time
    setIndividualWait();
  }
  else
  #endif
  // following code is used independent of setting of CHANGE_POS_ON_CONFLICT
  {
    bool b_configure = false;
    // call timeEvent for monitor list item
    #ifdef USE_DIN_9684
    if ( (pc_memberItem != NULL) && ( pc_memberItem->gtp() == gtp() ) && ( pc_memberItem->itemState(IState_c::Local) ) )
    {
      bool b_oldAddressClaimState = pc_memberItem->itemState(IState_c::ClaimedAddress);
      #ifdef USE_ISO_11783
      // check always for correct master state
      // ( some conflicts with other remote BUS nodes could cause an overwrite
      //    of the master node or of one of the slave node -> this function
      //     resets everything to a well defined master->slave state )
      if ( i8_slaveCount >= 0 ) setToMaster();
      #endif

      pc_memberItem->timeEvent();
      // check if DINItem_c reports now to have claimed an address and store it in Ident_Item
      if ( (pc_memberItem->itemState(IState_c::ClaimedAddress))
        && (!b_oldAddressClaimState) ) {
        setItemState(IState_c::ClaimedAddress);
        #ifdef USE_PROCESS
        uint16_t ui16_nr = pc_memberItem->nr();
        uint16_t ui16_filter = ((ui16_nr << 4) | 0x200);
        if (!getCanInstance4Comm().existFilter( getProcessInstance4Comm(), 0x7F0, ui16_filter, Ident_c::StandardIdent))
        { // create FilterBox
          getCanInstance4Comm().insertFilter(getProcessInstance4Comm(), 0x7F0, ui16_filter, false);
          b_configure = true;
        }
        ui16_filter = ((ui16_nr << 4) | 0x500 );
        if (!getCanInstance4Comm().existFilter( getProcessInstance4Comm(), 0x7F0, ui16_filter, Ident_c::StandardIdent))
        { // create FilterBox
          getCanInstance4Comm().insertFilter(getProcessInstance4Comm(), 0x7F0, ui16_filter, false);
          b_configure = true;
        }
        #endif
        // register switch2AddressClaim
        getSchedulerInstance4Comm().registerSwitch2AddressClaim();
        // request immediately names of other members
        System_c::triggerWd();
        getDinMonitorInstance4Comm().requestDinMemberNames();
      }
    }
    else if ( itemState(IState_c::Din) )
    { // local item is DIN but either pc_memberItem doesn't point to own item
      // or one of the states LOCAL or GTP is not satisfied
      // restart address claim
      pc_memberItem = NULL;
      #ifdef USE_ISO_11783
      if ( ( pc_isoItem != NULL ) && ( pc_isoItem->gtp() == gtp() ) && ( pc_isoItem->itemState(IState_c::Local) ) )
      { // this item is also active as ISO item and is already inserted in ISO list
        // --> delete it, to get fresh restart
        getIsoMonitorInstance4Comm().deleteIsoMemberGtp(gtp());
        pc_isoItem = NULL;
        clearItemState( IState_c::ClaimedAddress );
      }
      #endif
      setItemState(IState_c::itemState_t(IState_c::PreAddressClaim | IState_c::Local));
    }
    #endif // USE_DIN_9684
    #ifdef USE_ISO_11783
    if ( pc_isoItem != NULL )
    { // if pc_isoItem points to item with different GTP
      // -> pointer is invalid -> set to NULL so that it is searched new
      if (pc_isoItem->gtp() != gtp())
      {
        pc_isoItem = NULL;
        clearItemState( IState_c::ClaimedAddress );
      }
    }
    ISOMonitor_c& c_isoMonitor = getIsoMonitorInstance4Comm();
    if (pc_isoItem == NULL) {
      // search item in ISOMonitor
      if ( c_isoMonitor.existIsoMemberGtp(gtp()) )
        pc_isoItem = &(c_isoMonitor.isoMemberGtp(gtp()));
    }
    if (pc_isoItem != NULL)
    {
      if (pc_isoItem->itemState(IState_c::Local)) {
        bool b_oldAddressClaimState = pc_isoItem->itemState(IState_c::ClaimedAddress);
        // check always for correct master state
        // ( some conflicts with other remote BUS nodes could cause an overwrite
        //    of the master node or of one of the slave node -> this function
        //     resets everything to a well defined master->slave state )
        if ( i8_slaveCount >= 0 ) setToMaster();
        pc_isoItem->timeEvent();
        // check if DINItem_c reports now to have finished address claim and store it in Ident_Item
        if ( (pc_isoItem->itemState(IState_c::ClaimedAddress))
          && (!b_oldAddressClaimState) ) {
          // item changed from address claim to claimed address state
          // -> create local filter for processs data
          setItemState(IState_c::ClaimedAddress);
          #ifdef USE_DIN_9684
          // register switch2Announc
          getSchedulerInstance4Comm().registerSwitch2AddressClaim();
          #endif
          #ifdef USE_PROCESS
          uint32_t ui32_nr = pc_isoItem->nr();
          // only ISO msgs with own SA in PS (destination)
          uint32_t ui32_filter = ((static_cast<MASK_TYPE>(PROCESS_DATA_PGN) | static_cast<MASK_TYPE>(ui32_nr)) << 8);
          if (!getCanInstance4Comm().existFilter( getProcessInstance4Comm(), (0x1FFFF00UL), ui32_filter, Ident_c::ExtendedIdent))
          { // create FilterBox
            getCanInstance4Comm().insertFilter( getProcessInstance4Comm(), (0x1FFFF00UL), ui32_filter, false, Ident_c::ExtendedIdent);
            b_configure = true;
          }
          #endif
          #ifdef USE_ISO_TERMINAL
          {
            uint32_t ui32_nr = pc_isoItem->nr();
            // only ISO msgs with own SA in PS (destination)
            uint32_t ui32_filter = ((static_cast<MASK_TYPE>(VT_TO_ECU_PGN) | static_cast<MASK_TYPE>(ui32_nr)) << 8);
            if (!getCanInstance4Comm().existFilter( getIsoTerminalInstance(), (0x1FFFF00UL), ui32_filter, Ident_c::ExtendedIdent))
            { // create FilterBox
              getCanInstance4Comm().insertFilter( getIsoTerminalInstance(), (0x1FFFF00UL), ui32_filter, false, Ident_c::ExtendedIdent);
              b_configure = true;
            }
          }
          #endif
        }
      }
      else {
        // remote ISO item has overwritten local item
        // -> unify my POS / device class instance and insert new item
        // in monitor list of ISOMonitor
        if (c_isoMonitor.unifyIsoGtp(*pc_gtp) ) {
          // insert element in list
          c_isoMonitor.insertIsoMember(gtp(), b_wantedSa,
            IState_c::itemState_t(IState_c::Member | IState_c::Local | IState_c::Iso | IState_c::PreAddressClaim), ui16_saEepromAdr);
          pc_isoItem = &(c_isoMonitor.isoMemberGtp(gtp()));
          // set item as member and as own identity and overwrite old value
          pc_isoItem->setItemState
            (IState_c::itemState_t(IState_c::Member | IState_c::Local | IState_c::Iso | IState_c::PreAddressClaim));

          // insert all slave ISOItem objects (if not yet there) and set me as their master
          setToMaster ();

          pc_isoItem->timeEvent();
        }
      }
    }
    #endif
    if (b_configure) {
      getCanInstance4Comm().reconfigureMsgObj();
    }
  }
  return true;
}

/**
  calculate an individual number between [0,1000] to get an individual wait time before first
  address claim -> chance to avoid conflict with other system with same default GETY-POS
*/
void IdentItem_c::setIndividualWait()
{
     int32_t i32_result,
          i32_time = System_c::getTime();
     uint8_t serNo[6];
     i32_result = i32_time;
     #ifdef USE_DIN_9684
     // use the name for calculating of random value
     for (int16_t i = 0; i < 8; i++) i32_result ^= ((cpName[i] * i32_result) % 10000);
     #endif

     System_c::serialNo(serNo);
     // use the serialNo for calculating of random value
     for (int16_t j = 0; j < 6; j++) i32_result ^= ((serNo[j] * i32_result) % 10000);

     // do some calculations with GETY_POS
     i32_result *= gtp().getPos();
     i32_result += gtp().getGety();
     // use the system time
     i32_result -= (System_c::getTime() - i32_time);
     i32_result = CNAMESPACE::labs((i32_result % 1000) - (i32_result / 1000)) % 1000;
     // now set time in updateLTime -> switch to claim address state not before this time
     updateTime(i32_result+i32_time);
 };

/**
  check if given number is equal to member number of this item
  @param rui8_nr compared number
  @param ren_protoOrder select which protocol representation is compared
    (IState_c::DinOnly or IState_c::IsoOnly or IState_c::DinIso)
    (default IState_c::DinIso; this parameter is only compiled if
     USE_ISO_11783 is defined)
  @return true -> this item has same number
*/
bool IdentItem_c::equalNr(uint8_t rui8_nr
  #if defined( USE_ISO_11783 ) && defined( USE_DIN_9684 )
  , IState_c::protoOrder_t ren_protoOrder
  #endif
)
{
  bool b_result = false;

  #if (!defined(USE_ISO_11783)) && defined(USE_DIN_9684)
  if (pc_memberItem != NULL) b_result = (pc_memberItem->nr() == rui8_nr)?true:false;
  #elif defined(USE_ISO_11783) && defined(USE_DIN_9684)
  if (ren_protoOrder != IState_c::IsoOnly)
  { // try with DIN if not configured to IsoOnly
    if (pc_memberItem != NULL) b_result = (pc_memberItem->nr() == rui8_nr)?true:false;
  }
  if (ren_protoOrder != IState_c::DinOnly)
  { // try with ISO if not configured to DinOnly
    if (pc_isoItem != NULL) b_result = (pc_isoItem->nr() == rui8_nr)?true:false;
  }
  #elif defined( USE_ISO_11783 ) && (!defined( USE_DIN_9684 ) )
  if (pc_isoItem != NULL) b_result = (pc_isoItem->nr() == rui8_nr)?true:false;
  #endif
  return b_result;
};

#ifdef USE_ISO_11783
void IdentItem_c::setToMaster (int8_t ri8_slaveCount, const GetyPos_c* rpc_slaveIsoNameList)
{
  ISOItem_c* pc_slaveIsoItem;

  // if given, override list of slaves as given in constructor
  if ((ri8_slaveCount != -1) && (rpc_slaveIsoNameList != NULL)) {
    i8_slaveCount = (uint8_t) ri8_slaveCount;
    pc_slaveIsoNameList = rpc_slaveIsoNameList;
  }
  else if ( i8_slaveCount < 0 )
  { // item wasn't created with explicit config for master item
    return;
  }
  // set our own ISOItem
  pc_isoItem->setMaster (pc_isoItem);

  // If Master/Slave situation....
  if ( (i8_slaveCount != 0) && (pc_slaveIsoNameList != NULL) ) {
    // loop over all slaves
    for (uint8_t i=0; i<i8_slaveCount; i++) {
      // insert element in list
      if ( getIsoMonitorInstance4Comm().existIsoMemberGtp(pc_slaveIsoNameList[i]) ) {
        pc_slaveIsoItem = &(getIsoMonitorInstance4Comm().isoMemberGtp(pc_slaveIsoNameList[i]));
      } else
      if (getIsoMonitorInstance4Comm().insertIsoMember(pc_slaveIsoNameList[i], 0xFE,
        IState_c::itemState_t(IState_c::Member | IState_c::Iso | IState_c::PreAddressClaim), 0xFFFF)) {
        pc_slaveIsoItem = &(getIsoMonitorInstance4Comm().isoMemberGtp(pc_slaveIsoNameList[i]));
        // set item as member and as own identity and overwrite old value
        pc_slaveIsoItem->setItemState
        (IState_c::itemState_t(IState_c::Member | IState_c::Iso | IState_c::PreAddressClaim));
      } else {
        // there is neither a corresponding IsoItem_c in monitor list, nor is there the possibility to
        // create a new ( memory problem, as insert of new member failed in IsoMonitor_c::insertIsoMember()
        // failed )
        return;
      }
      // set the isoItem's master....
      pc_slaveIsoItem->setMaster (pc_isoItem);
    }
  }
}
#endif

} // end of namespace __IsoAgLib
