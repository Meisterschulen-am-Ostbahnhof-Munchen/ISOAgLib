/***************************************************************************
                itracpto_c.h  - working on Base Data Msg Type NMEA 200 GPS
                                  and Calendar; stores, updates  and
                                  delivers all base data informations
                                  from CANCustomer_c derived for CAN
                                  sending and receiving interaction;
                                  from ElementBase_c derived for
                                  interaction with other IsoAgLib objects
                             -------------------
    begin                 Fri Apr 07 2000
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
#ifndef ITRACPTO_C_H
#define ITRACPTO_C_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include <IsoAgLib/util/igetypos_c.h>
#include "ibasetypes.h"
#include "impl/tracpto_c.h"

// Begin Namespace IsoAgLib
namespace IsoAgLib {

  /** working on Base Data Msg Type 1;
      stores, updates  and delivers all base data informations;
      Derive from ElementBase_c to register in Scheduler_c for timeEvent trigger
      Derive from CANCustomer to register FilterBox'es in CANIO_c to receive CAN messages
      Derive from SINGLETON to create a Singleton which manages one global accessible singleton
      per IsoAgLib instance (if only one IsoAgLib instance is defined in application config, no overhead is produced).
    */
  class iTracPTO_c : private __IsoAgLib::TracPTO_c {
  public:
    // Public methods
  /**
    DEPRECATED CONFIG FUNCTION - PLEASE DO NOT USE ANY MORE AS THIS FUNCTION WILL BE REMOVED SOON
    config the Base_c object after init -> set pointer to devKey and
    config send/receive of different base msg types
    @param rpc_devKey pointer to the DEV_KEY variable of the responsible member instance (pointer enables automatic value update if var val is changed)
    @param rb_implementMode implement mode (true) or tractor mode (false)!!!
  */
  void config(const iDevKey_c* rpc_devKey, const bool rb_implementMode = true)
  {
    #if __GNUC__
    #warning "deprecated, use IsoAgLib::IdentMode_t as parameter instead of bool"
    #elif defined WIN32
    #pragma message ("deprecated, use IsoAgLib::IdentMode_t as parameter instead of bool")
    #endif
    TracPTO_c::config(rpc_devKey, (rb_implementMode ? IsoAgLib::IdentModeImplement : IsoAgLib::IdentModeTractor));
  }

  /**
    config the Base_c object after init -> set pointer to devKey and
    config send/receive of different base msg types
    @param rpc_devKey pointer to the DEV_KEY variable of the responsible member instance (pointer enables automatic value update if var val is changed)
    @param rt_identMode set mode to either IsoAgLib::IdentModeImplement or IsoAgLib::IdentModeTractor
  */
  void config(const iDevKey_c* rpc_devKey, const IsoAgLib::IdentMode_t rt_identMode = IsoAgLib::IdentModeImplement)
  { TracPTO_c::config(rpc_devKey, rt_identMode ); }


/* ******************************************* */
    /** \name Set Values for periodic send on BUS  */
    /*@{*/

    /** set rear PTO
        @param ri16_val value to store as the speed of the rear PTO
      */
    void setPtoRear(const uint16_t rui16_val) { TracPTO_c::setPtoRear(rui16_val);}
    /** set front PTO
        @param ri16_val value to store as the speed of the front PTO
      */
    void setPtoFront(const uint16_t rui16_val) { TracPTO_c::setPtoFront(rui16_val);}

    #ifdef USE_ISO_11783
    /** set explicit information whether front PTO is engaged
      * @param rt_val IsoActive -> PTO is engaged
      */
    void setPtoFrontEngaged(const IsoAgLib::IsoActiveFlag_t rt_val) { TracPTO_c::setPtoFrontEngaged(rt_val);}
    /** set explicit information whether rear PTO is engaged
      * @param rt_val IsoActive -> PTO is engaged
      */
    void setPtoRearEngaged(const IsoAgLib::IsoActiveFlag_t rt_val) { TracPTO_c::setPtoRearEngaged(rt_val);}
    /** set std RPM of front PTO (1000 n/min; 540 n/min)
      * @param rt_val IsoActive == 1000 n/min; false == 540 n/min
      */
    void setPtoFront1000(const IsoAgLib::IsoActiveFlag_t rt_val) {  TracPTO_c::setPtoFront1000(rt_val);}
    /** set std RPM of rear PTO (1000 n/min; 540 n/min)
      * @param rt_val IsoActive == 1000 n/min; false == 540 n/min
      */
    void setPtoRear1000(const IsoAgLib::IsoActiveFlag_t rt_val) { TracPTO_c::setPtoRear1000(rt_val);}
    /** set economy mode of front PTO
      * @param rt_val IsoActive -> front PTO is in economy mode
      */
    void setPtoFrontEconomy(const IsoAgLib::IsoActiveFlag_t rt_val) { TracPTO_c::setPtoFrontEconomy(rt_val);}
    /** set economy mode of rear PTO
      * @param rt_val IsoActive -> rear PTO is in economy mode
      */
    void setPtoRearEconomy(const IsoAgLib::IsoActiveFlag_t rt_val) { TracPTO_c::setPtoRearEconomy(rt_val);}
        /** set measured value of the set point of the rotational speed of the front power take-off (PTO) output shaft
        @param ri16_val
      */
    void setFrontPtoSetPoint(const uint16_t ri16_val)  { TracPTO_c::setFrontPtoSetPoint(ri16_val);}
    /** set measured value of the set point of the rotational speed of the rear power take-off (PTO) output shaft
        @param ri16_val
      */
    void setRearPtoSetPoint(const uint16_t ri16_val)  { TracPTO_c::setRearPtoSetPoint(ri16_val);}
    /** set reported tractor ECU's status of front engagement
        @param rt_val  status to set
      */
    void setFrontPtoEngagementReqStatus(IsoAgLib::IsoReqFlag_t rt_val) {TracPTO_c::setFrontPtoEngagementReqStatus(rt_val);}
    /** set reported tractor ECU's status of rear engagement
        @param rt_val  status to set
      */
    void setRearPtoEngagementReqStatus(IsoAgLib::IsoReqFlag_t rt_val) {TracPTO_c::setRearPtoEngagementReqStatus(rt_val);}

    /** set reported tractor ECU's status of front mode
        @param rt_val  status to set
      */
    void setFrontPtoModeReqStatus(IsoAgLib::IsoReqFlag_t rt_val) {TracPTO_c::setFrontPtoModeReqStatus(rt_val);}

    /** set reported tractor ECU's status of rear mode
        @param rt_val  status to set
      */
    void setRearPtoModeReqStatus(IsoAgLib::IsoReqFlag_t rt_val) {TracPTO_c::setRearPtoModeReqStatus(rt_val);}

    /** set reported tractor ECU's status of front economy mode
        @param rt_val  status to set
      */
    void setFrontPtoEconomyModeReqStatus(IsoAgLib::IsoReqFlag_t rt_val) {TracPTO_c::setFrontPtoEconomyModeReqStatus(rt_val);}

    /** set reported tractor ECU's status of rear economy mode
        @param rt_val  status to set
      */
    void setRearPtoEconomyModeReqStatus(IsoAgLib::IsoReqFlag_t rt_val) {TracPTO_c::setRearPtoEconomyModeReqStatus(rt_val);}

    /** set present limit status of front pto shaft speed
        @param rt_val  status to set
      */
    void setFrontPtoShaftSpeedLimitStatus(IsoAgLib::IsoLimitFlag_t rt_val) {TracPTO_c::setFrontPtoShaftSpeedLimitStatus(rt_val);}

    /** set present limit status of rear pto shaft speed
        @param rt_val  status to set
      */
    void setRearPtoShaftSpeedLimitStatus(IsoAgLib::IsoLimitFlag_t rt_val) {TracPTO_c::setRearPtoShaftSpeedLimitStatus(rt_val);}
    #endif
    /*@}*/

    /* ****************************************************** */
    /** \name Retrieve Values which are sent from other ECUs  */
    /*@{*/

    /** get rear pto
        @return actual rpm speed of rear PTO
      */
    uint16_t ptoRear() const { return TracPTO_c::ptoRear();}
    /** get front pto
        @return actual rpm speed of front PTO
      */
    uint16_t ptoFront() const { return TracPTO_c::ptoFront();}

    #ifdef USE_ISO_11783
    /** deliver explicit information whether front PTO is engaged
      * @return IsoActive -> PTO is engaged
      */
    IsoAgLib::IsoActiveFlag_t ptoFrontEngaged() const { return TracPTO_c::ptoFrontEngaged();}
    /** deliver explicit information whether rear PTO is engaged
      * @return IsoActive -> PTO is engaged
      */
    IsoAgLib::IsoActiveFlag_t ptoRearEngaged() const { return TracPTO_c::ptoRearEngaged();}
    /** deliver std RPM of front PTO (1000 n/min; 540 n/min)
      * @return IsoActive == 1000 n/min; false == 540 n/min
      */
    IsoAgLib::IsoActiveFlag_t ptoFront1000() const { return TracPTO_c::ptoFront1000();}
    /** deliver std RPM of rear PTO (1000 n/min; 540 n/min)
      * @return IsoActive == 1000 n/min; false == 540 n/min
      */
    IsoAgLib::IsoActiveFlag_t ptoRear1000() const { return TracPTO_c::ptoRear1000();}
    /** deliver economy mode of front PTO
      * @return IsoActive -> front PTO is in economy mode
      */
    IsoAgLib::IsoActiveFlag_t ptoFrontEconomy() const { return TracPTO_c::ptoFrontEconomy();}
    /** deliver economy mode of rear PTO
      * @return IsoActive -> rear PTO is in economy mode
      */
    IsoAgLib::IsoActiveFlag_t ptoRearEconomy() const { return TracPTO_c::ptoRearEconomy();}
        /** set measured value of the set point of the rotational speed of the front power take-off (PTO) output shaft
        @return
      */
    uint16_t frontPtoSetPoint()const { return TracPTO_c::frontPtoSetPoint();};
    /** set measured value of the set point of the rotational speed of the rear power take-off (PTO) output shaft
        @return
      */
    uint16_t rearPtoSetPoint()const { return TracPTO_c::rearPtoSetPoint();}
    /** get reported tractor ECU's status of front engagement
        @return  reported status
      */
    IsoAgLib::IsoReqFlag_t frontPtoEngagementReqStatus() const {return TracPTO_c::frontPtoEngagementReqStatus();}
    /** get reported tractor ECU's status of rear engagement
        @return  reported status
      */
    IsoAgLib::IsoReqFlag_t rearPtoEngagementReqStatus() const {return TracPTO_c::rearPtoEngagementReqStatus();}
    /** get reported tractor ECU's status of front mode
         @return  reported status
      */
    IsoAgLib::IsoReqFlag_t frontPtoModeReqStatus() const {return TracPTO_c::frontPtoModeReqStatus();}
    /** get reported tractor ECU's status of rear mode
        @return  reported status
      */
    IsoAgLib::IsoReqFlag_t rearPtoModeReqStatus() const {return TracPTO_c::rearPtoModeReqStatus();}
    /** get reported tractor ECU's status of front economy mode
        @return  reported status
      */
    IsoAgLib::IsoReqFlag_t frontPtoEconomyModeReqStatus() const {return TracPTO_c::frontPtoEconomyModeReqStatus();}
    /** get reported tractor ECU's status of rear economy mode
        @return  reported status
      */
    IsoAgLib::IsoReqFlag_t rearPtoEconomyModeReqStatus()const {return TracPTO_c::rearPtoEconomyModeReqStatus();}
    /** get present limit status of front pto shaft speed
        @return  present limit status
      */
    IsoAgLib::IsoLimitFlag_t frontPtoShaftSpeedLimitStatus() const {return TracPTO_c::frontPtoShaftSpeedLimitStatus();}
    /** get present limit status of rear pto shaft speed
        @return  present limit status
      */
    IsoAgLib::IsoLimitFlag_t rearPtoShaftSpeedLimitStatus() const {return TracPTO_c::rearPtoShaftSpeedLimitStatus();}
    #endif
    /*@}*/

  private:
    /** allow getITracMoveInstance() access to shielded tracgeneral class.
      otherwise __IsoAgLib::getTracMoveInstance() wouldn't be accepted by compiler
    */
    #if defined(PRT_INSTANCE_CNT) && (PRT_INSTANCE_CNT > 1)
    friend iTracPTO_c& getITracPtoInstance(uint8_t rui8_instance);
    #else
    friend iTracPTO_c& getITracPtoInstance(void);
    #endif

  };

  #if defined(PRT_INSTANCE_CNT) && (PRT_INSTANCE_CNT > 1)
  /** C-style function, to get access to the unique TracMove_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  inline iTracPTO_c& getITracPtoInstance(uint8_t rui8_instance = 0)
  { return static_cast<iTracPTO_c&>(__IsoAgLib::getTracPtoInstance(rui8_instance));};
  #else
  /** C-style function, to get access to the unique TracMove_c singleton instance */
  inline iTracPTO_c& getITracPtoInstance(void)
  { return static_cast<iTracPTO_c&>(__IsoAgLib::getTracPtoInstance());};
  #endif
}
#endif
