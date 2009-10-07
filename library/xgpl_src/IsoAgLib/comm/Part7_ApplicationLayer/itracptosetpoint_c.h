/***************************************************************************
                itracpto_c.h  - working on Base Data Msg Type NMEA 200 GPS
                                  and Calendar; stores, updates  and
                                  delivers all base data informations
                                  from CanCustomer_c derived for CAN
                                  sending and receiving interaction;
                                  from Scheduler_Task_c derived for
                                  interaction with other IsoAgLib objects
                             -------------------
    begin                 Fri Apr 07 2000
    copyright            : (C) 2000 - 2009 by Dipl.-Inform. Achim Spangler
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
#ifndef ITRACPTOSETPOINT_C_H
#define ITRACPTOSETPOINT_C_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include <IsoAgLib/comm/Part5_NetworkManagement/iisoname_c.h>
#include "ibasetypes.h"
#include "impl/tracptosetpoint_c.h"


// Begin Namespace IsoAgLib
namespace IsoAgLib {

  /** working on pto set point data;
      stores, updates and delivers all base data informations;
      Derive from BaseCommon_c some fundamental funktionality for all base data
      Derive from Scheduler_Task_c to register in Scheduler_c for timeEvent trigger
      Derive from CANCustomer to register FilterBox'es in CanIo_c to receive CAN messages
      Derive from SINGLETON to create a Singleton which manages one global accessible singleton
      per IsoAgLib instance (if only one IsoAgLib instance is defined in application config, no overhead is produced).
    */
  class iTracPTOSetPoint_c : private __IsoAgLib::TracPTOSetPoint_c {
  public:
    // Public methods
    /** config the TracPTOSetPoint_c object after init -> set pointer to isoName and
        config send/receive of different base msg types
        @param apc_isoName pointer to the ISOName variable of the responsible member instance (pointer enables automatic value update if var val is changed)
        @param at_identMode set mode to either IsoAgLib::IdentModeImplement or IsoAgLib::IdentModeTractor
        @return true -> configuration was successfull
      */
    bool config (const iIsoName_c* apc_isoName, IsoAgLib::IdentMode_t at_identMode = IsoAgLib::IdentModeImplement, uint16_t aui16_suppressMask = 0 )
    {return TracPTOSetPoint_c::config_base (apc_isoName, at_identMode, aui16_suppressMask );};


    /* ******************************************* */
    /** \name Set Values for periodic send on BUS  */
    /*@{*/

    /** allowing the position of the front three-point hitch to be set
        @param ui8_val command value
      */
    void setFrontHitchPosCmd(const uint8_t ui8_val) {TracPTOSetPoint_c::setFrontHitchPosCmd(ui8_val);};
    /** allowing the position of the rear three-point hitch to be set
        @param ui8_val  command value
      */
    void setRearPtoSetPointCmd(const uint8_t ui8_val) {TracPTOSetPoint_c::setRearPtoSetPointCmd(ui8_val);};
    /** set rotational speed of the front power take-off (PTO) output shaft
        @param ui16_val  command value
      */
    void setFrontPtoSetPointCmd(const uint16_t ui16_val) {TracPTOSetPoint_c::setFrontPtoSetPointCmd(ui16_val);};
    /** set rotational speed of the rear power take-off (PTO) output shaft
        @param ui16_val  command value
      */
    void setRearPtoSetPointCmd(const uint16_t ui16_val) {TracPTOSetPoint_c::setRearPtoSetPointCmd(ui16_val);};
    /** set front power take-off (PTO)
        @param t_val  IsoActive -> power take-off is engaged
      */
    void setFrontPtoEngagement(const IsoAgLib::IsoActiveFlag_t t_val) {TracPTOSetPoint_c::setFrontPtoEngagement(t_val);};
    /** set rear power take-off (PTO)
        @param t_val  IsoActive -> power take-off is engaged
      */
    void setRearPtoEngagement(const IsoAgLib::IsoActiveFlag_t t_val) {TracPTOSetPoint_c::setRearPtoEngagement(t_val);};
    /** set mode of the front power take-off (PTO)
        @param t_val  IsoActive -> 1000 r/min; IsoInactive -> 540 r/min
      */
    void setFrontPtoMode(const IsoAgLib::IsoActiveFlag_t t_val) {TracPTOSetPoint_c::setFrontPtoMode(t_val);};
    /** set mode of the rear power take-off (PTO)
        @param t_val  IsoActive -> 1000 r/min; IsoInactive -> 540 r/min
      */
    void setRearPtoMode(const IsoAgLib::IsoActiveFlag_t t_val) {TracPTOSetPoint_c::setRearPtoMode(t_val);};
    /** set front power take-off (PTO) economy mode
        @param t_val  IsoActive -> pto economy mode is engaged
      */
    void setFrontPtoEconomyMode(const IsoAgLib::IsoActiveFlag_t t_val) {TracPTOSetPoint_c::setFrontPtoEconomyMode(t_val);};
    /** set rear power take-off (PTO) economy mode
        @param t_val  IsoActive -> pto economy mode is engaged
      */
    void setRearPtoEconomyMode(const IsoAgLib::IsoActiveFlag_t t_val) {TracPTOSetPoint_c::setRearPtoEconomyMode(t_val);};
    /*@}*/

    /* ****************************************************** */
    /** \name Retrieve Values which are sent from other ECUs  */
    /*@{*/

    /** get position of the front three-point hitch
        @return  current position of the three-point hitch
      */
    uint8_t frontHitchPosCmd() const {return TracPTOSetPoint_c::frontHitchPosCmd();};
    /** get position of the rear three-point hitch
        @return  current position of the three-point hitch
      */
    uint8_t rearHitchPosCmd() const {return TracPTOSetPoint_c::rearHitchPosCmd();};
    /** get rotational speed of the front power take-off (PTO) output shaft
        @return  current rotational speed of power take-off
      */
    uint16_t frontPtoSetPointCmd() const {return TracPTOSetPoint_c::frontPtoSetPointCmd();};
    /** get rotational speed of the rear power take-off (PTO) output shaft
        @return  current rotational speed of power take-off
      */
    uint16_t rearPtoSetPointCmd() const {return TracPTOSetPoint_c::rearPtoSetPointCmd();};
    /** get front power take-off (PTO)
        @return  IsoActive -> power take-off is engaged
      */
    IsoAgLib::IsoActiveFlag_t frontPtoEngagement() const {return TracPTOSetPoint_c::frontPtoEngagement();};
    /** get rear power take-off (PTO)
        @return  IsoActive -> power take-off is engaged
      */
    IsoAgLib::IsoActiveFlag_t rearPtoEngagement() const {return TracPTOSetPoint_c::rearPtoEngagement();};
    /** get mode of the front power take-off (PTO)
        @return  IsoActive -> 1000 r/min; IsoInactive -> 540 r/min
      */
    IsoAgLib::IsoActiveFlag_t frontPtoMode() const {return TracPTOSetPoint_c::frontPtoMode();};
    /** get mode of the rear power take-off (PTO)
        @return  IsoActive -> 1000 r/min; IsoInactive -> 540 r/min
      */
    IsoAgLib::IsoActiveFlag_t rearPtoMode() const {return TracPTOSetPoint_c::rearPtoMode();};
    /** get front power take-off (PTO) economy mode
        @return  IsoActive -> pto economy mode is engaged
      */
    IsoAgLib::IsoActiveFlag_t frontPtoEconomyMode() const {return TracPTOSetPoint_c::frontPtoEconomyMode();};
    /** get rear power take-off (PTO) economy mode
        @return  IsoActive -> pto economy mode is engaged
      */
    IsoAgLib::IsoActiveFlag_t rearPtoEconomyMode() const {return TracPTOSetPoint_c::rearPtoEconomyMode();};
    /*@}*/


  private:
    /** allow getITracPtoSetPointInstance() access to shielded tracgeneral class.
      otherwise __IsoAgLib::getTracPtoSetPointInstance() wouldn't be accepted by compiler
    */
    #if defined(PRT_INSTANCE_CNT) && (PRT_INSTANCE_CNT > 1)
    friend iTracPTOSetPoint_c& getITracPtoSetPointInstance(uint8_t aui8_instance);
    #else
    friend iTracPTOSetPoint_c& getITracPtoSetPointInstance(void);
    #endif

  };

  #if defined(PRT_INSTANCE_CNT) && (PRT_INSTANCE_CNT > 1)
  /** C-style function, to get access to the unique TracPTOSetPoint_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  inline iTracPTOSetPoint_c& getITracPtoSetPointInstance(uint8_t aui8_instance = 0)
  { return static_cast<iTracPTOSetPoint_c&>(__IsoAgLib::getTracPtoSetPointInstance(aui8_instance));};
  #else
  /** C-style function, to get access to the unique TracPTOSetPoint_c singleton instance */
  inline iTracPTOSetPoint_c& getITracPtoSetPointInstance(void)
  { return static_cast<iTracPTOSetPoint_c&>(__IsoAgLib::getTracPtoSetPointInstance());};
  #endif
}
#endif
