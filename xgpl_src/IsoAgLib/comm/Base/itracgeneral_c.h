/***************************************************************************
              itracgeneral_c.h  - working on Base Data Msg Type NMEA 200 GPS
                                  and Calendar; stores, updates  and
                                  delivers all base data informations
                                  from CanCustomer_c derived for CAN
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
#include <IsoAgLib/comm/SystemMgmt/ISO11783/iisoname_c.h>
#include "ibasetypes.h"
#include "impl/tracgeneral_c.h"

// Begin Namespace IsoAgLib
namespace IsoAgLib {

  /** stores, updates  and delivers all base data informations;
    Derive from BaseCommon_c some fundamental funktionality for all base data
    Derive from ElementBase_c to register in Scheduler_c for timeEvent trigger
    Derive from CANCustomer to register FilterBox'es in CanIo_c to receive CAN messages
    Derive from SINGLETON to create a Singleton which manages one global accessible singleton
    per IsoAgLib instance (if only one IsoAgLib instance is defined in application config, no overhead is produced).
  */
  class iTracGeneral_c : private __IsoAgLib::TracGeneral_c {
  public:
    // Public methods

  /**
      config the iTracGeneral_c object after init -> set pointer to isoName and
      config send/receive of different base msg types
      @param apc_isoName pointer to the ISOName variable of the responsible member instance (pointer enables automatic value update if var val is changed)
      @param at_identMode set mode to either IsoAgLib::IdentModeImplement or IsoAgLib::IdentModeTractor
      @return true -> configuration was successfull
    */
  bool config (const iIsoName_c* apc_isoName, IsoAgLib::IdentMode_t at_identMode = IsoAgLib::IdentModeImplement, uint16_t aui16_suppressMask = 0)
  { return TracGeneral_c::config_base( apc_isoName, at_identMode, aui16_suppressMask ); }


  /* ******************************************* */
  /** \name Set Values for periodic send on BUS  */
  /*@{*/
  /** set rear hitch */
  void setHitchRear(uint8_t ab_val) { TracGeneral_c::setHitchRear(ab_val);}

  /** set front hitch */
  void setHitchFront(uint8_t ab_val) { TracGeneral_c::setHitchFront(ab_val);}

  /** set front hitch draft */
  void setHitchFrontDraft(int32_t ai32_val) { TracGeneral_c::setHitchFrontDraft(ai32_val);}

  /** set rear hitch draft */
  void setHitchRearDraft(int32_t ai32_val) { TracGeneral_c::setHitchRearDraft(ai32_val);}

  /** set front hitch nominal link force */
  void setHitchFrontLowerLinkForce(int16_t ai16_val) { TracGeneral_c::setHitchFrontLowerLinkForce(ai16_val);}

  /** set rear hitch nominal link force */
  void setHitchRearLowerLinkForce(int16_t ai16_val) { TracGeneral_c::setHitchRearLowerLinkForce(ai16_val);}

  /** set the ISO key switch state of the tractor */
  void setKeySwitch(IsoAgLib::IsoActiveFlag_t at_val) { TracGeneral_c::setKeySwitch(at_val);}

  /** set the maximum power time of the tractor in [min] */
  void setMaxPowerTime(uint8_t aui8_val) { TracGeneral_c::setMaxPowerTime(aui8_val);}

  /** force maintain power from tractor */
  void forceMaintainPower(bool ab_ecuPower, bool ab_actuatorPower, IsoAgLib::IsoMaintainPower_t at_implState)
  { TracGeneral_c::forceMaintainPower(ab_ecuPower, ab_actuatorPower, at_implState);}

  /** set present limit status of the front hitch position
        @param at_val  limit status of the front hitch position
      */
  void setFrontHitchPosLimitStatus(const IsoAgLib::IsoLimitFlag_t at_val) {TracGeneral_c::setFrontHitchPosLimitStatus(at_val);}

  /** set present limit status of the rear hitch position
      @param at_val  limit status of the rear hitch position
    */
  void setRearHitchPosLimitStatus(const IsoAgLib::IsoLimitFlag_t at_val) {TracGeneral_c::setRearHitchPosLimitStatus(at_val);}

  /** set state of implement in transport state
      * @param at_val state of implement in transport state
      */
  void setMaintainPowerForImplInTransport(IsoAgLib::IsoImplTransportFlag_t at_val) { TracGeneral_c::setMaintainPowerForImplInTransport(at_val); }

    /** set state of implement in park state
      * @param at_val state of implement in park state
      */
  void setMaintainPowerForImplInPark(IsoAgLib::IsoImplParkFlag_t at_val) { TracGeneral_c::setMaintainPowerForImplInPark(at_val);}

    /** set state of implement in work
      * @param at_val state of implement in work state
      */
  void setMaintainPowerForImplInWork(IsoAgLib::IsoImplWorkFlag_t at_val) { TracGeneral_c::setMaintainPowerForImplInWork(at_val);}
  /*@}*/

  /* ****************************************************** */
  /** \name Retrieve Values which are sent from other ECUs  */
  /*@{*/
    /** get rear hitch */
  uint8_t hitchRear() const { return TracGeneral_c::hitchRear();}

  /** get front hitch */
  uint8_t hitchFront() const { return TracGeneral_c::hitchFront();}

  /** deliver front hitch draft */
  int32_t hitchFrontDraft() const { return TracGeneral_c::hitchFrontDraft();}

  /** deliver rear hitch draft */
  int32_t hitchRearDraft() const { return TracGeneral_c::hitchRearDraft();}

  /** deliver front hitch nominal link force */
  int16_t hitchFrontLowerLinkForce() const { return TracGeneral_c::hitchFrontLowerLinkForce();}

  /** deliver rear hitch nominal link force */
  int16_t hitchRearLowerLinkForce() const { return TracGeneral_c::hitchRearLowerLinkForce();}

  /** deliver the ISO key switch state of the tractor */
  IsoAgLib::IsoActiveFlag_t keySwitch() const { return TracGeneral_c:: keySwitch();}

  /** deliver the maximum power time of the tractor in [min] */
  uint8_t maxPowerTime() const { return TracGeneral_c::maxPowerTime();}

  /** deliver last receive time of maintain power request */
  int32_t lastMaintainPowerRequest() const { return TracGeneral_c::lastMaintainPowerRequest();}

  /** check whether maintenance of ECU power was requested */
  bool maintainEcuPower() const { return TracGeneral_c::maintainEcuPower();}

  /** check whether maintenance of actuator power was requested */
  bool maintainActuatorPower() const { return TracGeneral_c::maintainActuatorPower();}

  /** check whether maintenance of power
    * for implement in transport state was requested */
  IsoAgLib::IsoImplTransportFlag_t maintainPowerForImplInTransport() const { return TracGeneral_c::maintainPowerForImplInTransport();}

  /** check whether maintenance of power
    * for implement in park state was requested */
  IsoAgLib::IsoImplParkFlag_t maintainPowerForImplInPark() const { return TracGeneral_c::maintainPowerForImplInPark();}

  /** check whether maintenance of power
    * for implement in work state was requested */
  IsoAgLib::IsoImplWorkFlag_t maintainPowerForImplInWork() const { return TracGeneral_c::maintainPowerForImplInWork();}

  /** get present limit status of the front hitch position
    @return  limit status of front hitch position
  */
  IsoAgLib::IsoLimitFlag_t frontHitchPosLimitStatus()const {return TracGeneral_c::frontHitchPosLimitStatus();}

  /** get present limit status of the rear hitch position
      @return  limit status of rear hitch position
    */
  IsoAgLib::IsoLimitFlag_t rearHitchPosLimitStatus()const {return TracGeneral_c::rearHitchPosLimitStatus();}

  bool isVtLanguageReceived()   const { return TracGeneral_c::isVtLanguageReceived();}
  bool isTecuLanguageReceived() const { return TracGeneral_c::isTecuLanguageReceived();}

  const uint8_t* getVtLanguage()   const { return TracGeneral_c::getVtLanguage();}
  const uint8_t* getTecuLanguage() const { return TracGeneral_c::getTecuLanguage();}

  /** send iso language data msg*/
  void sendLanguage() { TracGeneral_c::sendLanguage();}

  /** force a request for pgn for language information */
  bool sendRequestUpdateLanguage() { return TracGeneral_c::sendRequestUpdateLanguage(); }
  /*@}*/

  private:
    /** allow getITracGeneralInstance() access to shielded tracgeneral class.
      otherwise __IsoAgLib::getTracGeneralInstance() wouldn't be accepted by compiler
    */
    #if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
    friend iTracGeneral_c& getITracGeneralInstance( uint8_t aui8_instance );
    #else
    friend iTracGeneral_c& getITracGeneralInstance( void );
    #endif

  };
  #if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
  /** C-style function, to get access to the unique Base_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  inline iTracGeneral_c& getITracGeneralInstance( uint8_t aui8_instance = 0 )
  { return static_cast<iTracGeneral_c&>(__IsoAgLib::getTracGeneralInstance(aui8_instance));}
  #else
  /** C-style function, to get access to the unique Base_c singleton instance */
  inline iTracGeneral_c& getITracGeneralInstance( void )
  { return static_cast<iTracGeneral_c&>(__IsoAgLib::getTracGeneralInstance());}
  #endif
}
#endif
