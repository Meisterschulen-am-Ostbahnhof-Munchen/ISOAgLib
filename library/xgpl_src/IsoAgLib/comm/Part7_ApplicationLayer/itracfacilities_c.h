/*
  itracfacilities_c.h: stores, updates and delivers all facilities
    data information from CanCustomer_c derived for CAN sending and
    receiving interaction; from SchedulerTask_c derived for.
    interaction with other IsoAgLib objects This class handles Part 7
    - B.25.3 Tractor facilities response message 0xFE09 in both
    directions (configurable)

  (C) Copyright 2009 - 2015 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef ITRACFACILITIES_C_H
#define ITRACFACILITIES_C_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include <IsoAgLib/comm/Part5_NetworkManagement/iidentitem_c.h>
#include "ibasetypes.h"
#include "impl/tracfacilities_c.h"

// Begin Namespace IsoAgLib
namespace IsoAgLib {

  /** stores, updates  and delivers all facilities data information;
      Derive from BaseCommon_c some fundamental funktionality for all base data
      Derive from SchedulerTask_c to register in Scheduler_c for timeEvent trigger
      Derive from CANCustomer to register FilterBox'es in CanIo_c to receive CAN messages
      Derive from SINGLETON to create a Singleton which manages one global accessible singleton
      per IsoAgLib instance (if only one IsoAgLib instance is defined in application config, no overhead is produced).
    */
  class iTracFacilities_c : private __IsoAgLib::TracFacilities_c {

  public:
  /** @param apc_ident pointer to the variable of the responsible member instance. If NULL the module cannot send requests!
      @param at_identMode set mode to either IsoAgLib::IdentModeImplement or IsoAgLib::IdentModeTractor
      @return true -> configuration was successfull
    */
  bool config ( const iIdentItem_c* apc_ident, IsoAgLib::IdentMode_t at_identMode = IsoAgLib::IdentModeImplement, uint16_t aui16_suppressMask = 0 )
  { return TracFacilities_c::config_base( apc_ident, at_identMode, aui16_suppressMask ); }

  /* ********************************** */
  /** \name Set Values for send on BUS  */
  /*@{*/

  /** set tecu class with class t_val
      @param at_val  type of tecu class
    */
  void setTractorClass(const IsoAgLib::IsoTecuClassFlag_t at_val) {TracFacilities_c::setTractorClass(at_val);}

  /** set facility t_facility with value t_val
      @param at_facility  type of facility to be set
      @param at_val       type of facility flag
    */
  void setFacitity(const IsoAgLib::IsoFacility_t at_facility, const IsoAgLib::IsoFacilityFlag_t at_val)
  {TracFacilities_c::setFacitity(at_facility,at_val);};

  /* ****************************************************** */
  /** \name Retrieve Values which are sent from TECU  */
  /*@{*/
  /** get tecu class
      @return  type of tecu class
    */
  IsoAgLib::IsoTecuClassFlag_t getTractorClass() const {return TracFacilities_c::getTractorClass();}

  /** get facility flag for t_facility
      @param at_facility  type of facility to be set
      @return  type of facility flag
    */
  IsoAgLib::IsoFacilityFlag_t getFacitity(const IsoAgLib::IsoFacility_t at_facility) const
  {return TracFacilities_c::getFacitity(at_facility);};

  private:
#if ( PRT_INSTANCE_CNT == 1 )
    friend iTracFacilities_c& getITracFacilitiesInstance();
#endif
    friend iTracFacilities_c& getITracFacilitiesInstance( unsigned instance );
  };

#if ( PRT_INSTANCE_CNT == 1 )
  inline iTracFacilities_c& getITracFacilitiesInstance()
  { return static_cast<iTracFacilities_c&>(__IsoAgLib::getTracFacilitiesInstance( 0 )); }
#endif
  inline iTracFacilities_c& getITracFacilitiesInstance( unsigned instance )
  { return static_cast<iTracFacilities_c&>(__IsoAgLib::getTracFacilitiesInstance( instance )); }

}
#endif
