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
#include "ISO11783/iisoitem_c.h"
#include "ISO11783/iisoname_c.h"

namespace __IsoAgLib {
  class IsoTerminal_c;
  class DevPropertyHandler_c;
}

// Begin Namespace IsoAgLib
namespace IsoAgLib {
// predeclare iIsoTerminal_c, as this class is allowed to access private ( hidden )
// elements of this class exclusively ( see friend declaration at end of this file )
class iIsoTerminal_c;

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
  IsoAgLib::iIsoName_c c_myType( 2, 0 );
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
class iIdentItem_c : private __IsoAgLib::IdentItem_c  {
public:
  /**
    default constructor, which can optionally start address claim for this identity, if enough information
    is provided with the parameters (at least apc_isoName, apb_name
    @param apc_isoName           optional pointer to the ISOName variable of this identity, which is resident somewhere else (f.e. main() task)
    @param ai8_slaveCount       amount of attached slave devices; default -1 == no master state;
                                in case an address claim for the slave devices shall be sent by this ECU, they
                                must get their own IdentItem_c instance ( then with default value -1 for ai8_slaveCount )
    @param apc_slaveIsoNameList pointer to list of IsoName_c values, where the slave devices are defined.
                                IsoAgLib will then send the needed "master indicates its slaves" messages on BUS
    @param ai_singletonVecKey   optional key for selection of IsoAgLib instance (default 0)
  */
  iIdentItem_c (uint16_t aui16_saEepromAdr = 0xFFFF, int ai_singletonVecKey = 0)
    : IdentItem_c (aui16_saEepromAdr, ai_singletonVecKey) {}


  /**
    constructor for ISO identity, which starts address claim for this identity
    @param aui8_indGroup        select the industry group, 2 == agriculture
    @param ab_func              function code of the member (25 = network interconnect)
    @param aui16_manufCode      11bit manufactor code
    @param aui32_serNo          21bit serial number
    @param ab_preferredSa       preselected source adress (SA) of the ISO item (fixed SA or last time
                                SA for self conf ISO device) (default 254 for free self-conf)
    @param aui16_saEepromAdr    EEPROM adress, where the used source adress is stored for self_conf members
                                (default 0xFFFF for NO EEPROM store)
    @param ab_funcInst          function instance of this member (default 0)
    @param ab_ecuInst           ECU instance of this member (default 0)
    @param ab_selfConf          true -> this member as a self configurable source adress
    @param ai8_slaveCount       amount of attached slave devices; default -1 == no master state;
                                in case an address claim for the slave devices shall be sent by this ECU, they
                                must get their own IdentItem_c instance ( then with default value -1 for ai8_slaveCount )
    @param apc_slaveIsoNameList pointer to list of iIsoName_c values, where the slave devices are defined.
                                IsoAgLib will then send the needed "master indicates its slaves" messages on BUS
    @param ai_singletonVecKey   optional key for selection of IsoAgLib instance (default 0)
  */
  iIdentItem_c (
    uint8_t aui8_indGroup, uint8_t aui8_devClass, uint8_t aui8_devClassInst, uint8_t ab_func,
    uint16_t aui16_manufCode, uint32_t aui32_serNo, uint8_t ab_preferredSa = 254, uint16_t aui16_eepromAdr = 0xFFFF,
    uint8_t ab_funcInst = 0, uint8_t ab_ecuInst = 0, bool ab_selfConf = true,
    #ifdef USE_WORKING_SET
    int8_t ai8_slaveCount = -1, const iIsoName_c* apc_slaveIsoNameList = NULL,
    #endif
    int ai_singletonVecKey = 0)
    : IdentItem_c (aui8_indGroup, aui8_devClass, aui8_devClassInst, ab_func, aui16_manufCode, aui32_serNo,
                   ab_preferredSa, aui16_eepromAdr, ab_funcInst, ab_ecuInst, ab_selfConf,
                 #ifdef USE_WORKING_SET
                   ai8_slaveCount, apc_slaveIsoNameList,
                 #endif
                   ai_singletonVecKey) {}

  /** init function for later start of address claim of an ISO identity (this can be only called once upon a default-constructed object)
      @param aui8_indGroup        select the industry group, 2 == agriculture
      @param ab_func              function code of the member (25 = network interconnect)
      @param aui16_manufCode      11bit manufactor code
      @param aui32_serNo          21bit serial number
      @param aui8_preferredSa     preferred source adress (SA) of the ISO item (fixed SA or last time
                                  SA for self conf ISO device) (default 254 for no special wish)
      @param aui16_saEepromAdr    EEPROM adress, where the used IsoName / SA / flags are stored
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
    int8_t ai8_slaveCount = -1, const iIsoName_c* apc_slaveIsoNameList = NULL,
    #endif
    int ai_singletonVecKey = 0)
  { IdentItem_c::init (aui8_indGroup, aui8_devClass, aui8_devClassInst, ab_func, aui16_manufCode,
                       aui32_serNo, ab_preferredSa, aui16_saEepromAdr, ab_funcInst, ab_ecuInst, ab_selfConf,
                       #ifdef USE_WORKING_SET
                       ai8_slaveCount, apc_slaveIsoNameList,
                       #endif
                       ai_singletonVecKey);
  }



  /** deliver pointer to IsoItem_c in IsoMonitor_c
      @return NULL -> either no ISO item or not yet registered in IsoMonitor_c
    */
  iIsoItem_c* getIsoItem( void ) const { return static_cast<iIsoItem_c*>(IdentItem_c::getIsoItem()); }

  /** send address release for DIN identities and perform other stop actions at end of lifetime
    * of a local ident
    */
  void close( void ) { IdentItem_c::close();}

  /** check if the ident has claimed address */
  bool isClaimedAddress( void ) const { return IdentItem_c::isClaimedAddress(); }

  /**
    retreive the actual ISOName value of this ident item
    @return ISOName code of this ident item instance
  */
  const iIsoName_c& isoName() const {return IdentItem_c::isoName().toConstIisoName_c(); }

  /**
    retrieve a POINTER to the ISOName of this ident item
    you can ONLY modify this item if an address is not yet claimed!
    i.e. in this case you'd get a NULL-pointer
    ATTENTION: So ALWAYS check for a NULL pointer return value before accessing!!
    @return NULL if this ident item has already claimed an address on the ISOBUS
            or
            a POINTER to the ISOName of this ident item!
   */
  iIsoName_c* modifyableIsoNameBeforeAddressClaimed() { if (!isClaimedAddress()) return &modifyableIsoName().toIisoName_c();
                                                        else return NULL; }

private:
  friend class iIsoTerminal_c;
  friend class iVtClientServerCommunication_c;
  friend class iDevPropertyHandler_c;
  friend class __IsoAgLib::IsoTerminal_c;
};

}// End Namespace IsoAgLib
#endif
