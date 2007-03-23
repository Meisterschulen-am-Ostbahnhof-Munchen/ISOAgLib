/***************************************************************************
                itracmove_c.h  -  stores, updates and delivers all moving
                                  data information from CANCustomer_c
                                  derived for CAN sending and receiving
                                  interaction;
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
#ifndef ITRACMOVE_C_H
#define ITRACMOVE_C_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include <IsoAgLib/comm/SystemMgmt/ISO11783/iisoname_c.h>
#include "ibasetypes.h"
#include "impl/tracmove_c.h"

// Begin Namespace IsoAgLib
namespace IsoAgLib {

  /** stores, updates  and delivers all moving data information;
      Derive from BaseCommon_c some fundamental funktionality for all base data
      Derive from ElementBase_c to register in Scheduler_c for timeEvent trigger
      Derive from CANCustomer to register FilterBox'es in CANIO_c to receive CAN messages
      Derive from SINGLETON to create a Singleton which manages one global accessible singleton
      per IsoAgLib instance (if only one IsoAgLib instance is defined in application config, no overhead is produced).
    */
  class iTracMove_c : private __IsoAgLib::TracMove_c {
  public:
    // Public methods
  /**
      config the Base_c object after init -> set pointer to isoName and
      config send/receive of different base msg types
      @param rpc_isoName pointer to the ISOName variable of the responsible member instance (pointer enables automatic value update if var val is changed)
      @param rt_identMode set mode to either IsoAgLib::IdentModeImplement or IsoAgLib::IdentModeTractor
      @return true -> configuration was successfull
    */
  bool config (const iISOName_c* rpc_isoName, IsoAgLib::IdentMode_t rt_identMode = IsoAgLib::IdentModeImplement)
  { return TracMove_c::config_base(rpc_isoName, rt_identMode ); }

  /** update selected speed with actually best available speed
      @param t_speedSrc  from which source is the speed available
    */
  void updateSpeed(IsoAgLib::SpeedSource_t rt_speedSource) {TracMove_c::updateSpeed(rt_speedSource);}

  /** update distance and direction with the actually best available distance and direction
      @param t_distanceSrc  from which source is the distance and direction available
    */
  void updateDistanceDirection(IsoAgLib::DistanceDirectionSource_t t_distanceSrc) {TracMove_c::updateDistanceDirection(t_distanceSrc);}

  /* ******************************************* */
  /** \name Set Values for periodic send on BUS  */
  /*@{*/
  /** set the value of real speed (measured by radar)
      @param ri16_val value to store as real radar measured speed
    */
  void setSpeedReal(const int32_t& ri32_val)
  {
    TracMove_c::setSpeedReal(ri32_val);
  }
  /** set the value of theoretical speed (calculated from gear)
      @param ri16_val value to store as theoretical gear calculated speed
    */
  void setSpeedTheor(const int32_t& ri32_val)
  {
    TracMove_c::setSpeedTheor(ri32_val);
  }
  /** set the real (radar measured) driven distance with int32_t val
      @param rreflVal value to store as real radar measured distance
    */
  void setDistReal(const uint32_t& rreflVal) {TracMove_c::setDistReal(rreflVal);}
  /** set the theoretical (gear calculated) driven distance with int32_t val
      @param rreflVal value to store as theoretical (gear calculated) driven distance
    */
  void setDistTheor(const uint32_t& rreflVal) {TracMove_c::setDistTheor(rreflVal);}

  /** set measured signal indicating either forward or reverse as the theoretical (gear calculated) direction of travel
      @return  direction of travel
    */
  void setDirectionTheor(IsoAgLib::IsoDirectionFlag_t t_val) {TracMove_c::setDirectionTheor(t_val);}

  /** set measured signal indicating either forward or reverse as the real (radar measured) direction of travel
      @return  direction of travel
    */
  void setDirectionReal(IsoAgLib::IsoDirectionFlag_t t_val) {TracMove_c::setDirectionReal(t_val);}

  /** set parameter which indicates whetcher the reported direction is reversed from the perspective of the operator
      @param rt_val  indicates direction (IsoInactive = not reversed; IsoActive = reversed)
    */
  void setOperatorDirectionReversed(const IsoAgLib::IsoOperatorDirectionFlag_t rt_val)
  { TracMove_c::setOperatorDirectionReversed( rt_val );}

  /** set actual distance traveled by the machine based on the value of selected machine speed
      @param ui32_val  actual distance traveled
    */
  void setSelectedDistance(const uint32_t& ui32_val) {TracMove_c::setSelectedDistance(ui32_val);}

  /** set indicated current direction of travel of the machine
      @param t_val  current direction of travel
    */
  void setSelectedDirection(const IsoAgLib::IsoDirectionFlag_t t_val) {TracMove_c::setSelectedDirection(t_val);}

  /** set current value of the speed as determined from a number of sources by the machine
      @param ui16_val  current value of speed
    */
  void setSelectedSpeed(const int32_t& i32_val) {TracMove_c::setSelectedSpeed(i32_val);}

  /** set present limit status of selected speed
      @param t_val  limit status
    */
  void setSelectedSpeedLimitStatus(const IsoAgLib::IsoLimitFlag_t t_val)
  {TracMove_c::setSelectedSpeedLimitStatus(t_val);}

  /** set indicated speed source that is currently being reported in the machine speed parameter
      @param t_val  speed source that is currently being reported
    */
  void setSelectedSpeedSource(const IsoAgLib::IsoSpeedSourceFlag_t t_val)
  {TracMove_c::setSelectedSpeedSource(t_val);}
  /*@}*/

  /* ****************************************************** */
  /** \name Retrieve Values which are sent from other ECUs  */
  /*@{*/
  /** get the value of real speed (measured by radar)
      @return actual radar measured speed value
    */
  int32_t speedReal() const {return TracMove_c::speedReal();}

  /** is looking for a valid speed value
      @return true if speed is valid otherwise false */
  bool isRealSpeedUsable() const { return TracMove_c::isRealSpeedUsable();}

  /** is looking for a missing speed value
      @return true if speed is missing otherwise false */
  bool isRealSpeedMissing() const { return  TracMove_c::isRealSpeedMissing();}

  /** is looking for a erroneous speed value
      @return true if speed is erroneous otherwise false */
  bool isRealSpeedErroneous() const { return TracMove_c::isRealSpeedErroneous();}

  /** get the value of theoretical speed (calculated from gear)
      @return theoretical gear calculated speed value
    */
  int32_t speedTheor() const {return TracMove_c::speedTheor();}

  /** is looking for a valid speed value
      @return true if speed is valid otherwise false */
  bool isTheorSpeedUsable() const { return TracMove_c::isTheorSpeedUsable();}

  /** is looking for a missing speed value
      @return true if speed is missing otherwise false */
  bool isTheorSpeedMissing() const { return  TracMove_c::isTheorSpeedMissing();}

  /** is looking for a erroneous speed value
      @return true if speed is erroneous otherwise false */
  bool isTheorSpeedErroneous() const { return TracMove_c::isTheorSpeedErroneous();}

  /** get the real driven distance with int32_t val
      @return actual radar measured driven distance value
    */
  uint32_t distReal() const {return TracMove_c::distReal();}

  /** get the real driven distance with int32_t val
      @return actual gear calculated driven distance value
    */
  uint32_t distTheor() const {return TracMove_c::distTheor();}

  /** get measured signal indicating either forward or reverse as the theoretical (gear calculated) direction of travel
      @return  direction of travel
    */
  IsoAgLib::IsoDirectionFlag_t directionTheor() {return TracMove_c::directionTheor();}

  /** get measured signal indicating either forward or reverse as the real (radar measured) direction of travel
      @return  direction of travel
    */
  IsoAgLib::IsoDirectionFlag_t directionReal() {return TracMove_c::directionReal();}

  /** get parameter which indicates whetcher the reported direction is reversed from the perspective of the operator
      @return indicates direction (IsoInactive = not reversed; IsoActive = reversed)
    */
  IsoAgLib::IsoOperatorDirectionFlag_t operatorDirectionReversed()const { return TracMove_c::operatorDirectionReversed();}

  /** get actual distance traveled by the machine based on the value of selected machine speed
      @return  actual distance traveled
    */
  uint32_t selectedDistance() const {return TracMove_c::selectedDistance();}

  /** get current direction of travel of the machine
      @return  current direction of travel
    */
  IsoAgLib::IsoDirectionFlag_t selectedDirection() const {return TracMove_c::selectedDirection();}

  /** get current value of the speed as determined from a number of sources by the machine
      @return  current value of speed
    */
  int32_t selectedSpeed() { return TracMove_c::selectedSpeed();}

  /** is looking for a valid speed value
      @return true if speed is valid otherwise false */
  bool isSelectedSpeedUsable() const { return TracMove_c::isSelectedSpeedUsable();}

  /** is looking for a missing speed value
      @return true if speed is missing otherwise false */
  bool isSelectedSpeedMissing() const { return  TracMove_c::isSelectedSpeedMissing();}

  /** is looking for a erroneous speed value
      @return true if speed is erroneous otherwise false */
  bool isSelectedSpeedErroneous() const { return TracMove_c::isSelectedSpeedErroneous();}

  /** get present limit status of selected speed
      @return  limit status
    */
  IsoAgLib::IsoLimitFlag_t selectedSpeedLimitStatus() const {return TracMove_c::selectedSpeedLimitStatus();}

  /** get speed source that is currently being reported in the machine speed parameter
      @return  speed source that is currently being reported
    */
  IsoAgLib::IsoSpeedSourceFlag_t selectedSpeedSource() const {return TracMove_c::selectedSpeedSource();}

  private:
    /** allow getITracMoveInstance() access to shielded tracmove class.
      otherwise __IsoAgLib::getTracMoveInstance() wouldn't be accepted by compiler
    */
    #if defined(PRT_INSTANCE_CNT) && (PRT_INSTANCE_CNT > 1)
    friend iTracMove_c& getITracMoveInstance(uint8_t rui8_instance);
    #else
    friend iTracMove_c& getITracMoveInstance(void);
    #endif

  };

  #if defined(PRT_INSTANCE_CNT) && (PRT_INSTANCE_CNT > 1)
  /** C-style function, to get access to the unique TracMove_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  inline iTracMove_c& getITracMoveInstance(uint8_t rui8_instance = 0)
  { return static_cast<iTracMove_c&>(__IsoAgLib::getTracMoveInstance(rui8_instance));}
  #else
  /** C-style function, to get access to the unique TracMove_c singleton instance */
  inline iTracMove_c& getITracMoveInstance(void)
  { return static_cast<iTracMove_c&>(__IsoAgLib::getTracMoveInstance());}
  #endif
}
#endif
