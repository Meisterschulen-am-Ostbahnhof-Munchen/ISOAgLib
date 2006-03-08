/***************************************************************************
              itracgeneral_c.h  - working on Base Data Msg Type NMEA 200 GPS
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
#ifndef ITRACGENERAL_C_H
#define ITRACGENERAL_C_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include <IsoAgLib/util/igetypos_c.h>
#include "ibasetypes.h"
#include "impl/tracgeneral_c.h"

// Begin Namespace IsoAgLib
namespace IsoAgLib {

  /** stores, updates  and delivers all base data informations;
    Derive from BaseCommon_c some fundamental funktionality for all base data
    Derive from ElementBase_c to register in Scheduler_c for timeEvent trigger
    Derive from CANCustomer to register FilterBox'es in CANIO_c to receive CAN messages
    Derive from SINGLETON to create a Singleton which manages one global accessible singleton
    per IsoAgLib instance (if only one IsoAgLib instance is defined in application config, no overhead is produced).
  */
  class iTracGeneral_c : private __IsoAgLib::TracGeneral_c {
  public:
    // Public methods

    /** DEPRECATED CONFIG FUNCTION - PLEASE DO NOT USE ANY MORE AS THIS FUNCTION WILL BE REMOVED SOON
        initialise element which can't be done during construct;
        above all create the needed FilterBox_c instances
        possible errors:
          * dependant error in CANIO_c problems during insertion of new FilterBox_c entries for IsoAgLibBase
        @param rpc_devKey optional pointer to the DEV_KEY variable of the ersponsible member instance (pointer enables automatic value update if var val is changed)
        @param rb_implementMode implement mode (true) or tractor mode (false)
      */
    void init(const iDevKey_c* rpc_devKey, bool rb_implementMode)
    {
      #warning "deprecated, use IsoAgLib::IdentMode_t as parameter instead of bool"
      TracGeneral_c::init(rpc_devKey, (rb_implementMode ? IsoAgLib::IdentModeImplement : IsoAgLib::IdentModeTractor));
    };
    /** initialise element which can't be done during construct;
        above all create the needed FilterBox_c instances
        possible errors:
          * dependant error in CANIO_c problems during insertion of new FilterBox_c entries for IsoAgLibBase
        @param rpc_devKey optional pointer to the DEV_KEY variable of the ersponsible member instance (pointer enables automatic value update if var val is changed)
        @param rt_identMode either IsoAgLib::IdentModeImplement or IsoAgLib::IdentModeTractor
      */
    void init(const iDevKey_c* rpc_devKey = NULL, IsoAgLib::IdentMode_t rt_IdentMode = IsoAgLib::IdentModeImplement)
    {TracGeneral_c::init(rpc_devKey, rt_IdentMode);};
  /**
    DEPRECATED CONFIG FUNCTION - PLEASE DO NOT USE ANY MORE AS THIS FUNCTION WILL BE REMOVED SOON
    config the Base_c object after init -> set pointer to devKey and
    config send/receive of different base msg types
    @param rpc_devKey pointer to the DEV_KEY variable of the responsible member instance (pointer enables automatic value update if var val is changed)
    @param rb_implementMode implement mode (true) or tractor mode (false)!!!
  */
  void config(const iDevKey_c* rpc_devKey, bool rb_implementMode = true)
  {
    #warning "deprecated, use IsoAgLib::IdentMode_t as parameter instead of bool"
    TracGeneral_c::config(rpc_devKey, (rb_implementMode ? IsoAgLib::IdentModeImplement : IsoAgLib::IdentModeTractor));
  }

  /**
    config the Base_c object after init -> set pointer to devKey and
    config send/receive of different base msg types
    @param rpc_devKey pointer to the DEV_KEY variable of the responsible member instance (pointer enables automatic value update if var val is changed)
    @param rt_identMode set mode to either IsoAgLib::IdentModeImplement or IsoAgLib::IdentModeTractor
  */
  void config(const iDevKey_c* rpc_devKey, IsoAgLib::IdentMode_t rt_identMode = IsoAgLib::IdentModeImplement)
  { TracGeneral_c::config(rpc_devKey, rt_identMode ); }

    /* ******************************************* */
    /** \name Set Values for periodic send on BUS  */
    /*@{*/
    /** set rear hitch */
    void setHitchRear(uint8_t rb_val) { return TracGeneral_c::setHitchRear(rb_val);};
    /** set front hitch */
    void setHitchFront(uint8_t rb_val) { return TracGeneral_c::setHitchFront(rb_val);};

    #ifdef USE_DIN_9684
    /** set engine speed */
    void setEngine(int16_t ri16_val) { return TracGeneral_c::setEngine(ri16_val);};
    /** deliver rear left draft */
    void setRearLeftDraft(int16_t ri16_val) { return TracGeneral_c::setRearLeftDraft(ri16_val);};
    /** deliver rear right draft */
    void setRearRightDraft(int16_t ri16_val) { return TracGeneral_c::setRearRightDraft(ri16_val);};
    /** deliver rear total draft Newton */
    void setRearDraftNewton(int16_t ri16_val) { return TracGeneral_c::setRearDraftNewton(ri16_val);};
    /** deliver rear total draft Percent */
    void setRearDraftNominal(uint8_t rui8_val) { return TracGeneral_c::setRearDraftNominal(rui8_val);};
    /** deliver fuel consumption L/h */
    void setFuelRate(int16_t ri16_val) { return TracGeneral_c::setFuelRate(ri16_val);};
    /** deliver fuel temperature °C */
    void setFuelTemperature(uint8_t rui8_val) { return TracGeneral_c::setFuelTemperature(rui8_val);};
    #endif

    #ifdef USE_ISO_11783
    /** set front hitch draft */
    void setHitchFrontDraft(int32_t ri32_val) { return TracGeneral_c::setHitchFrontDraft(ri32_val);};
    /** set rear hitch draft */
    void setHitchRearDraft(int32_t ri32_val) { return TracGeneral_c::setHitchRearDraft(ri32_val);};
    /** set front hitch nominal link force */
    void setHitchFrontLowerLinkForce(int16_t ri16_val) { return TracGeneral_c::setHitchFrontLowerLinkForce(ri16_val);};
    /** set rear hitch nominal link force */
    void setHitchRearLowerLinkForce(int16_t ri16_val) { return TracGeneral_c::setHitchRearLowerLinkForce(ri16_val);};
    /** set the ISO key switch state of the tractor */
    void setKeySwitch(IsoAgLib::IsoActiveFlag_t rt_val) { return TracGeneral_c::setKeySwitch(rt_val);};
    /** set the maximum power time of the tractor in [min] */
    void setMaxPowerTime(uint8_t rui8_val) { return TracGeneral_c::setMaxPowerTime(rui8_val);};
    /** force maintain power from tractor */
    void forceMaintainPower(bool rb_ecuPower, bool rb_actuatorPower, IsoAgLib::IsoMaintainPower_t rt_implState)
    { return TracGeneral_c::forceMaintainPower(rb_ecuPower, rb_actuatorPower, rt_implState);};
    #endif
    /*@}*/

    /* ****************************************************** */
    /** \name Retrieve Values which are sent from other ECUs  */
    /*@{*/
      /** get rear hitch */
    uint8_t hitchRear() const { return TracGeneral_c::hitchRear();};
    /** get front hitch */
    uint8_t hitchFront() const { return TracGeneral_c::hitchFront();};

    #ifdef USE_DIN_9684
    /** get engine speed */
    int16_t engine() const { return TracGeneral_c::engine();};
    /** deliver rear left draft */
    int rearLeftDraft() const { return TracGeneral_c::rearLeftDraft();};
    /** deliver rear right draft */
    int rearRightDraft() const { return TracGeneral_c::rearRightDraft();};
    /** deliver rear total draft Newton */
    int rearDraftNewton() const { return TracGeneral_c::rearDraftNewton();};
    /** deliver rear total draft Percent */
    int rearDraftNominal() const { return TracGeneral_c::rearDraftNominal();};
    /** deliver fuel consumption L/h */
    int fuelRate() const { return TracGeneral_c::fuelRate();};
    /** deliver fuel temperature °C */
    int fuelTemperature() const { return TracGeneral_c::fuelTemperature();};
    #endif

    #ifdef USE_ISO_11783
    /** deliver front hitch draft */
    int32_t hitchFrontDraft() const { return TracGeneral_c::hitchFrontDraft();};
    /** deliver rear hitch draft */
    int32_t hitchRearDraft() const { return TracGeneral_c::hitchRearDraft();};
    /** deliver front hitch nominal link force */
    int16_t hitchFrontLowerLinkForce() const { return TracGeneral_c::hitchFrontLowerLinkForce();};
    /** deliver rear hitch nominal link force */
    int16_t hitchRearLowerLinkForce() const { return TracGeneral_c::hitchRearLowerLinkForce();};
    /** deliver the ISO key switch state of the tractor */
    IsoAgLib::IsoActiveFlag_t keySwitch() const { return TracGeneral_c:: keySwitch();};
    /** deliver the maximum power time of the tractor in [min] */
    uint8_t maxPowerTime() const { return TracGeneral_c::maxPowerTime();};
    /** deliver last receive time of maintain power request */
    int32_t lastMaintainPowerRequest() const { return TracGeneral_c::lastMaintainPowerRequest();};
    /** check whether maintenance of ECU power was requested */
    bool maintainEcuPower() const { return TracGeneral_c::maintainEcuPower();};
    /** check whether maintenance of actuator power was requested */
    bool maintainActuatorPower() const { return TracGeneral_c::maintainActuatorPower();};
    /** check whether maintenance of power
      * for implement in transport state was requested */
    bool maintainPowerForImplInTransport() const { return TracGeneral_c::maintainPowerForImplInTransport();};
    /** check whether maintenance of power
      * for implement in park state was requested */
    bool maintainPowerForImplInPark() const { return TracGeneral_c::maintainPowerForImplInPark();};
    /** check whether maintenance of power
      * for implement in work state was requested */
    bool maintainPowerForImplInWork() const { return TracGeneral_c::maintainPowerForImplInWork();};

    bool isVtLanguageReceived()   const { return TracGeneral_c::isVtLanguageReceived();};
    bool isTecuLanguageReceived() const { return TracGeneral_c::isTecuLanguageReceived();};

    const uint8_t* getVtLanguage()   const { return TracGeneral_c::getVtLanguage();};
    const uint8_t* getTecuLanguage() const { return TracGeneral_c::getTecuLanguage();};

    /** send iso language data msg*/
    void isoSendLanguage(const iDevKey_c& rpc_devKey) { TracGeneral_c::isoSendLanguage(rpc_devKey);};
    #endif
    /*@}*/

  private:
    /** allow getITracGeneralInstance() access to shielded tracgeneral class.
      otherwise __IsoAgLib::getTracGeneralInstance() wouldn't be accepted by compiler
    */
    #if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
    friend iTracGeneral_c& getITracGeneralInstance( uint8_t rui8_instance );
    #else
    friend iTracGeneral_c& getITracGeneralInstance( void );
    #endif

  };
  #if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
  /** C-style function, to get access to the unique Base_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  inline iTracGeneral_c& getITracGeneralInstance( uint8_t rui8_instance = 0 )
  { return static_cast<iTracGeneral_c&>(__IsoAgLib::getTracGeneralInstance(rui8_instance));};
  #else
  /** C-style function, to get access to the unique Base_c singleton instance */
  inline iTracGeneral_c& getITracGeneralInstance( void )
  { return static_cast<iTracGeneral_c&>(__IsoAgLib::getTracGeneralInstance());};
  #endif
}
#endif
