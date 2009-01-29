/***************************************************************************
          itracfacilities_c.h  -  stores, updates and delivers all facilities
                                  data information from CanCustomer_c
                                  derived for CAN sending and receiving
                                  interaction;
                                  from Scheduler_Task_c derived for
                                  interaction with other IsoAgLib objects

                                  This class handles Part 7 - B.25.3
                                  Tractor facilities response message 0xFE09
                                  in both directions (configurable)
                             -------------------
    begin                 Fri Nov 24 2008
    copyright            : (C) 2000 - 2009 by Dipl.-Ing. Antoine Kandera
    email                : a.kandera@osb-ag:de
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
#ifndef ITRACFACILITIES_C_H
#define ITRACFACILITIES_C_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include <IsoAgLib/comm/Part5_NetworkManagement/iisoname_c.h>
#include "ibasetypes.h"
#include "impl/tracfacilities_c.h"

// Begin Namespace IsoAgLib
namespace IsoAgLib {

  /** stores, updates  and delivers all facilities data information;
      Derive from BaseCommon_c some fundamental funktionality for all base data
      Derive from Scheduler_Task_c to register in Scheduler_c for timeEvent trigger
      Derive from CANCustomer to register FilterBox'es in CanIo_c to receive CAN messages
      Derive from SINGLETON to create a Singleton which manages one global accessible singleton
      per IsoAgLib instance (if only one IsoAgLib instance is defined in application config, no overhead is produced).
    */
  class iTracFacilities_c : private __IsoAgLib::TracFacilities_c {
  public:
    // Public methods
  /**
      config the Base_c object after init -> set pointer to isoName and
      config send/receive of different base msg types
      @param apc_isoName pointer to the ISOName variable of the responsible member instance (pointer enables automatic value update if var val is changed)
      @param at_identMode set mode to either IsoAgLib::IdentModeImplement or IsoAgLib::IdentModeTractor
      @return true -> configuration was successfull
    */
  bool config (const iIsoName_c* apc_isoName, IsoAgLib::IdentMode_t at_identMode = IsoAgLib::IdentModeImplement, uint16_t aui16_suppressMask = 0)
  { return TracFacilities_c::config_base(apc_isoName, at_identMode, aui16_suppressMask ); }

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
    /** allow getITracFacilitiesInstance() access to shielded tracfacilities class.
      otherwise __IsoAgLib::getTracFacilitiesInstance() wouldn't be accepted by compiler
    */
    #if defined(PRT_INSTANCE_CNT) && (PRT_INSTANCE_CNT > 1)
    friend iTracFacilities_c& getITracFacilitiesInstance(uint8_t aui8_instance);
    #else
    friend iTracFacilities_c& getITracFacilitiesInstance(void);
    #endif

  };

  #if defined(PRT_INSTANCE_CNT) && (PRT_INSTANCE_CNT > 1)
  /** C-style function, to get access to the unique TracFacilities_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  inline iTracFacilities_c& getITracFacilitiesInstance(uint8_t aui8_instance = 0)
  { return static_cast<iTracFacilities_c&>(__IsoAgLib::getTracFacilitiesInstance(aui8_instance));}
  #else
  /** C-style function, to get access to the unique TracFacilities_c singleton instance */
  inline iTracFacilities_c& getITracFacilitiesInstance(void)
  { return static_cast<iTracFacilities_c&>(__IsoAgLib::getTracFacilitiesInstance());}
  #endif
}
#endif
