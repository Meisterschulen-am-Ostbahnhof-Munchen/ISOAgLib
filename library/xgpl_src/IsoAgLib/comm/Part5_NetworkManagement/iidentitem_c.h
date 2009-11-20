/***************************************************************************
                          iidentitem_c.h  - object for managing identity/ies
                                          of the ECU
                             -------------------
    begin                : Fri Apr 07 2000
    copyright            : (C) 2000 - 2009 by Dipl.-Inform. Achim Spangler
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
 * Copyright (C) 2000 - 2009 Dipl.-Inform. Achim Spangler                  *
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
#include "iisoitem_c.h"
#include "iisoname_c.h"

namespace __IsoAgLib {
  class IsoTerminal_c;
  class DevPropertyHandler_c;
  class ProprietaryMessageHandler_c;
  class ProprietaryMessageClient_c;
}

// Begin Namespace IsoAgLib
namespace IsoAgLib {
// predeclare iIsoTerminal_c, as this class is allowed to access private ( hidden )
// elements of this class exclusively ( see friend declaration at end of this file )
class iIsoTerminal_c;
class iProprietaryMessageHandler_c;
class iProprietaryMessageClient_c;

/**
  class for identity/ies (Control Function(s)) which are managed by the actual ECU;
  new instances start in prepare address claim state and stay there for a random and serialNo dependent time;
  then they are inserted as announcing member in the monitoring list and start announcing;

  If the IsoAgLib shall not start immediately with address claim on definition of the variable
  ( e.g. if some data must be read for definition of local identity ),
  the address claim can be started later with explicit call of iIdentItem_c::start( ).

  The IsoAgLib backend is responsible for answering all requests like
  RequestForClaimedSourceAdress ( ISO 11783 )

  The application just has to call the main trigger function Scheduler_c::timeEvent().

  After the monitor list report a completed address claim the status changes to active/claimed address;

  @short member ident item of this Control Function 
  @author Dipl.-Inform. Achim Spangler
  */
class iIdentItem_c : private __IsoAgLib::IdentItem_c  {
public:
  /**
    default constructor, which can optionally start address claim for this identity, if enough information
    is provided with the parameters (at least apc_isoName, apb_name
    @param aui16_saEepromAdr
    @param ai_singletonVecKey optional key for selection of IsoAgLib instance (default 0)
  */
  iIdentItem_c (uint16_t aui16_saEepromAdr = 0xFFFF, int ai_singletonVecKey = 0)
    : IdentItem_c (aui16_saEepromAdr, ai_singletonVecKey) {}


  /**
    constructor for ISO identity, which starts address claim for this identity
    @param aui8_indGroup        select the industry group, 2 == agriculture
    @param aui8_devClass	device class
    @param aui8_devClassInst	deveice class instance
    @param ab_func              function code of the member (25 = network interconnect)
    @param aui16_manufCode      11bit manufactor code
    @param aui32_serNo          21bit serial number
    @param aui8_preferredSa       preselected source adress (SA) of the ISO item (fixed SA or last time
                                SA for self conf ISO device) (default 254 for free self-conf)
    @param aui16_eepromAdr    EEPROM adress, where the used source adress is stored for self_conf members
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
    uint16_t aui16_manufCode, uint32_t aui32_serNo, uint8_t aui8_preferredSa = 254, uint16_t aui16_eepromAdr = 0xFFFF,
    uint8_t ab_funcInst = 0, uint8_t ab_ecuInst = 0, bool ab_selfConf = true,
    #ifdef USE_WORKING_SET
    int8_t ai8_slaveCount = -1, const iIsoName_c* apc_slaveIsoNameList = NULL,
    #endif
    int ai_singletonVecKey = 0)
    : IdentItem_c (aui8_indGroup, aui8_devClass, aui8_devClassInst, ab_func, aui16_manufCode, aui32_serNo,
                   aui8_preferredSa, aui16_eepromAdr, ab_funcInst, ab_ecuInst, ab_selfConf,
                 #ifdef USE_WORKING_SET
                   ai8_slaveCount, apc_slaveIsoNameList,
                 #endif
                   ai_singletonVecKey) {}

  /** init function for later start of address claim of an ISO identity (this can be only called once upon a default-constructed object)
      @param aui8_indGroup        select the industry group, 2 == agriculture
      @param aui8_devClass	  device class
      @param aui8_devClassInst	  deveice class instance
       @param ab_func             function code of the member (25 = network interconnect)
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
    uint32_t aui32_serNo, uint8_t aui8_preferredSa, uint16_t aui16_saEepromAdr, uint8_t ab_funcInst = 0, uint8_t ab_ecuInst = 0, bool ab_selfConf = true,
    #ifdef USE_WORKING_SET
    int8_t ai8_slaveCount = -1, const iIsoName_c* apc_slaveIsoNameList = NULL,
    #endif
    int ai_singletonVecKey = 0)
  { IdentItem_c::init (aui8_indGroup, aui8_devClass, aui8_devClassInst, ab_func, aui16_manufCode,
                       aui32_serNo, aui8_preferredSa, aui16_saEepromAdr, ab_funcInst, ab_ecuInst, ab_selfConf,
                       #ifdef USE_WORKING_SET
                       ai8_slaveCount, apc_slaveIsoNameList,
                       #endif
                       ai_singletonVecKey);
  }



  /** deliver pointer to IsoItem_c in IsoMonitor_c
      @return NULL -> either no ISO item or not yet registered in IsoMonitor_c
    */
  iIsoItem_c* getIsoItem( void ) const { return static_cast<iIsoItem_c*>(IdentItem_c::getIsoItem()); }

  /** perform stop actions at end of lifetime of a local ident
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


  /**
    Set ECU Identification fields, needed during the diagnostic procedure
    @return true if the fields were okay
            false if one field was too long (> 200 chars) or had '*' in it.
  */
  bool setEcuIdentification( const STL_NAMESPACE::string& astr_partNbr, const STL_NAMESPACE::string& astr_serialNbr, const STL_NAMESPACE::string& astr_location, const STL_NAMESPACE::string& astr_type, const STL_NAMESPACE::string& astr_manufacturerName )
  {
    return IdentItem_c::setEcuIdentification (astr_partNbr, astr_serialNbr, astr_location, astr_type, astr_manufacturerName);
  }

  /** Set SW Identification fields, needed during the diagnostic procedure
      @param astr_swId SwIdentification according to ISO 11783-12.
                       End fields with *, even the last one (and even if it is only one)
      @return true if the SW Identification was okay
              false if it was too long (> 200 chars) or empty
   */
  bool setSwIdentification( const STL_NAMESPACE::string& astr_swId)
  {
    return IdentItem_c::setSwIdentification (astr_swId);
  }

  //! Setter for the different certification message fields
  //! Parameter:
  //! @param ui16_year Certification year as in ISO 11783-7 A.29.1, must be between 2000 and 2061
  //! @param a_revision Certification revision as in ISO 11783-7 A.29.2
  //! @param a_laboratoryType Certification laboratory type as in ISO 11783-7 A.29.3
  //! @param aui16_laboratoryId Certification laboratory ID (11 bits wide) as in ISO 11783-7 A.29.4
  //! @param acrc_certificationBitMask Compliance certification type bitfield as in ISO 11783-7 A.29.5 till A.29.17
  //! @param aui16_referenceNumber Compliance certification reference number as in ISO 11783-7 A.29.18
  //! @return true all parameters were okay in range
  //!         false at least one parameter was wrong
  bool setCertificationData( uint16_t ui16_year, IsoAgLib::CertificationRevision_t a_revision, IsoAgLib::CertificationLabType_t a_laboratoryType, uint16_t aui16_laboratoryId,
                               const IsoAgLib::CertificationBitMask_t& acrc_certificationBitMask, uint16_t aui16_referenceNumber )
  {
    return IdentItem_c::setCertificationData( ui16_year , a_revision, a_laboratoryType, aui16_laboratoryId,
                                              acrc_certificationBitMask, aui16_referenceNumber );
  }

  /// Using the singletonVecKey from internal class
  SINGLETON_PAR_BASE_DEF(IdentItem_c)

private:
  friend class iFsManager_c;
  friend class iIsoTerminal_c;
  friend class iVtClientServerCommunication_c;
  friend class iDevPropertyHandler_c;
  friend class __IsoAgLib::IsoTerminal_c;
  friend class iProprietaryMessageHandler_c;
  friend class iProprietaryMessageClient_c;
  friend class __IsoAgLib::ProprietaryMessageHandler_c;
  friend class __IsoAgLib::ProprietaryMessageClient_c;
};

}// End Namespace IsoAgLib
#endif
