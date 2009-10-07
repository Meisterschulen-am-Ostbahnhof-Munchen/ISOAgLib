/***************************************************************************
             itracguidance_c.h  - working on guidance data;
                                  stores, updates and delivers all
                                  guidance data informations from
                                  CanCustomer_c derived for CAN
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
#ifndef ITRACGUIDANCE_C_H
#define ITRACGUIDANCE_C_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include "ibasetypes.h"
#include "impl/tracguidance_c.h"


// Begin Namespace IsoAgLib
namespace IsoAgLib {

  /** stores, updates  and delivers certification data information;
      Derive from BaseCommon_c some fundamental funktionality for all base data
      Derive from Scheduler_Task_c to register in Scheduler_c for timeEvent trigger
      Derive from CANCustomer to register FilterBox'es in CanIo_c to receive CAN messages
      Derive from SINGLETON to create a Singleton which manages one global accessible singleton
      per IsoAgLib instance (if only one IsoAgLib instance is defined in application config, no overhead is produced).
    */
  class iTracGuidance_c :
  private __IsoAgLib::TracGuidance_c {
    public:
  // Public methods
  /**
      config the TracGuidance_c object after init -> set pointer to isoName and
      config send/receive of different base msg types
      @param apc_isoName pointer to the ISOName variable of the responsible member instance (pointer enables automatic value update if var val is changed)
      @param at_identMode set mode to either IsoAgLib::IdentModeImplement or IsoAgLib::IdentModeTractor
      @return true -> configuration was successfull
    */
  bool config (const iIsoName_c* apc_isoName, IsoAgLib::IdentMode_t at_identMode = IsoAgLib::IdentModeImplement, uint16_t aui16_suppressMask = 0)
  { return TracGuidance_c::config_base (apc_isoName, at_identMode, aui16_suppressMask ); }

  /* ******************************************* */
  /** \name Set Values for periodic send on BUS  */
  /*@{*/
  /** set machine steering system's estimate of the curvature of the current turn
      @param i16_val  estimated value of curvature of current turn
    */
  void setEstCurvature(const int16_t i16_val) {TracGuidance_c::setEstCurvature(i16_val);}
  /** set machine steering system request to the automatic guidance system to change curvature command status
      @param t_val  value of curvature command status
    */
  void setRequestResetCmdStatus(const IsoAgLib::IsoResetFlag_t t_val) {TracGuidance_c::setRequestResetCmdStatus(t_val);}
  /** set machine steering system indication if manual steering is in correct position for guidance to function
      @param t_val  state of manual steering position
    */
  void setSteeringInputPosStatus(const IsoAgLib::IsoSteerPosFlag_t t_val) {TracGuidance_c::setSteeringInputPosStatus(t_val);}
  /** set machine steering system indication that it is free from faults that would preclude guidance system operation
      @param t_val  status of machine steering system
    */
  void setSteeringSystemReadiness( const IsoAgLib::IsoSteerReadinessFlag_t t_val) {TracGuidance_c::setSteeringSystemReadiness(t_val);}
  /** set state of lockout switch that allows operators to disable automatic steering system functions
      @param t_val  status of lockout switch
    */
  void setMechanicalSystemLogout(const IsoAgLib::IsoActiveFlag_t t_val) {TracGuidance_c::setMechanicalSystemLogout(t_val);}
  /*@}*/

  /* ****************************************************** */
  /** \name Retrieve Values which are sent from other ECUs  */
  /*@{*/
  /** get machine steering system's estimate of the curvature of the current turn
      @return  estimated value of curvature of current turn
    */
  int16_t estCurvature() const { return TracGuidance_c::estCurvature();}
  /** get machine steering system request to the automatic guidance system to change curvature command status
      @return  request reset command status
    */
  IsoAgLib::IsoResetFlag_t requestResetCmdStatus() const { return TracGuidance_c::requestResetCmdStatus();}
  /** get machine steering system indication if manual steering is in correct position for guidance to function
      @return  state of manual steering position
    */
  IsoAgLib::IsoSteerPosFlag_t steeringInputPosStatus() const { return TracGuidance_c::steeringInputPosStatus();}
  /** get machine steering system indication that it is free from faults that would preclude guidance system operation
      @return  status of machine steering system
    */
  IsoAgLib::IsoSteerReadinessFlag_t steeringSystemReadiness() const { return TracGuidance_c::steeringSystemReadiness();}
  /** get state of lockout switch that allows operators to disable automatic steering system functions
      @return  status of lockout switch
    */
  IsoAgLib::IsoActiveFlag_t mechanicalSystemLogout() const { return TracGuidance_c::mechanicalSystemLogout();}
  /*@}*/
  private:
    /** allow getITracGuidanceInstance() access to shielded traccert class.
      otherwise __IsoAgLib::getTracGuidanceInstance() wouldn't be accepted by compiler
    */
    #if defined(PRT_INSTANCE_CNT) && (PRT_INSTANCE_CNT > 1)
    friend iTracGuidance_c& getITracGuidanceInstance(uint8_t aui8_instance);
    #else
    friend iTracGuidance_c& getITracGuidanceInstance(void);
    #endif

  };

  #if defined(PRT_INSTANCE_CNT) && (PRT_INSTANCE_CNT > 1)
  /** C-style function, to get access to the unique iTracGuidance_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  inline iTracGuidance_c& getITracGuidanceInstance(uint8_t aui8_instance = 0)
  { return static_cast<iTracGuidance_c&>(__IsoAgLib::getTracGuidanceInstance(aui8_instance));}
  #else
  /** C-style function, to get access to the unique iTracGuidance_c singleton instance */
  inline iTracGuidance_c& getITracGuidanceInstance(void)
  { return static_cast<iTracGuidance_c&>(__IsoAgLib::getTracGuidanceInstance());}
  #endif
}
#endif

