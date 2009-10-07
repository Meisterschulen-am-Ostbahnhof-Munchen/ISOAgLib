/***************************************************************************
                itracaux_c.h  - working on auxiliary valve data
                                stores, updates  and
                                delivers all base data informations
                                from CanCustomer_c derived for CAN
                                sending and receiving interaction;
                                from BaseCommon_c derived for
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
#ifndef ITRACAUX_C_H
#define ITRACAUX_C_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include <IsoAgLib/comm/Part5_NetworkManagement/iisoname_c.h>
#include "ibasetypes.h"
#include "impl/tracaux_c.h"


// Begin Namespace IsoAgLib
namespace IsoAgLib {
   /** stores, updates  and delivers all auxiliary data information;
      Derive from BaseCommon_c some fundamental funktionality for all base data
      Derive from Scheduler_Task_c to register in Scheduler_c for timeEvent trigger
      Derive from CANCustomer to register FilterBox'es in CanIo_c to receive CAN messages
      Derive from SINGLETON to create a Singleton which manages one global accessible singleton
      per IsoAgLib instance (if only one IsoAgLib instance is defined in application config, no overhead is produced).
    */
  class iTracAux_c : private __IsoAgLib::TracAux_c {
  public:
    // Public methods
  /**
      config the iTracAux_c object after init -> set pointer to isoName and
      config send/receive of different base msg types
      @param apc_isoName pointer to the ISOName variable of the responsible member instance (pointer enables automatic value update if var val is changed)
      @param at_identMode set mode to either IsoAgLib::IdentModeImplement or IsoAgLib::IdentModeTractor
      @return true -> configuration was successfull
    */
  bool config (const iIsoName_c* apc_isoName, IsoAgLib::IdentMode_t at_identMode = IsoAgLib::IdentModeImplement, uint16_t aui16_suppressMask = 0)
  { return TracAux_c::config_base (apc_isoName, at_identMode, aui16_suppressMask ); }

  /* ******************************************* */
  /** \name Set Values for periodic send on BUS  */
  /*@{*/
  /** set value reported by the controller of flow through the extended port of auxiliary valve of a tractor
      which could be based on the commandes of the valve
      possible errors:
        * data range error
      @param valveNumber  auxiliary valve number (range from 0 to 15)
      @param t_val        value to store
      @return             true -> data was set; false -> data range error
    */
  bool setExtendPortEstFlow(const uint8_t valveNumber, const int8_t t_val)
  {return TracAux_c::setExtendPortEstFlow(valveNumber, t_val); }

  /** set value reported by the controller of flow through the retract port of auxiliary valve of a tractor
      which could be based on the commandes of the valve
      possible errors:
        * data range error
      @param valveNumber  auxiliary valve number (range from 0 to 15)
      @param t_val        value to store
      @return             true -> data was set; false -> data range error
    */
  bool setRetractPortEstFlow(const uint8_t valveNumber, const int8_t t_val)
  {return TracAux_c::setRetractPortEstFlow(valveNumber, t_val);}

  /** set measured state of the fail save mode of auxiliary valve
      possible errors:
        * data range error
      @param valveNumber  auxiliary valve number (range from 0 to 15)
      @param t_val        value to store
      @return             true -> data was set; false -> data range error
    */
  bool setEstFailSaveMode(const uint8_t valveNumber, const IsoAuxFlag_t t_val)
  {return TracAux_c::setEstFailSaveMode(valveNumber, t_val);}

  /** set measured state of the auxiliary valve
      possible errors:
        * data range error
      @param valveNumber  auxiliary valve number (range from 0 to 15)
      @param t_val        value to store
      @return             true -> data was set; false -> data range error
    */
  bool setEstValveState(const uint8_t valveNumber, const IsoAuxFlagExtended_t t_val)
  {return TracAux_c::setEstValveState(valveNumber, t_val);}

  /** set measured flow through the extend port auxiliary valve of a tractor, expressed as a percentage of full flow
      possible errors:
        * data range error
      @param valveNumber  auxiliary valve number (range from 0 to 15)
      @param t_val        value to store
      @return             true -> data was set; false -> data range error
    */
  bool setExtendPortMeasuredFlow(const uint8_t valveNumber, const int8_t t_val)
  {return TracAux_c::setExtendPortMeasuredFlow(valveNumber, t_val);}

  /** set measured flow through the retract port auxiliary valve of a tractor, expressed as a percentage of full flow
      possible errors:
        * data range error
      @param valveNumber  auxiliary valve number (range from 0 to 15)
      @param t_val        value to store
      @return             true -> data was set; false -> data range error
    */
  bool setRetractPortMeasuredFlow(const uint8_t valveNumber, const int8_t t_val)
  {return TracAux_c::setRetractPortMeasuredFlow(valveNumber, t_val);}

  /** set measured nominal pressure at the extend port of auxiliary valve of a tractor
      possible errors:
        * data range error
      @param valveNumber  auxiliary valve number (range from 0 to 15)
      @param t_val        value to store
      @return             true -> data was set; false -> data range error
    */
  bool setExtendPortPressure(const uint8_t valveNumber, const uint32_t t_val)
  {return TracAux_c::setExtendPortPressure(valveNumber, t_val);}

  /** set measured nominal pressure at the retract port of auxiliary valve of a tractor
      possible errors:
        * data range error
      @param valveNumber  auxiliary valve number (range from 0 to 15)
      @param t_val        value to store
      @return             true -> data was set; false -> data range error
    */
  bool setRetractPortPressure(const uint8_t valveNumber, const uint32_t t_val)
  {return TracAux_c::setRetractPortPressure(valveNumber, t_val);}

  /** set measured nominal pressure at the return port of auxiliary valve of a tractor
      possible errors:
        * data range error
      @param valveNumber  auxiliary valve number (range from 0 to 15)
      @param t_val        value to store
      @return             true -> data was set; false -> data range error
    */
  bool setReturnPortPressure(const uint8_t valveNumber, const uint16_t t_val)
  {return TracAux_c::setReturnPortPressure(valveNumber, t_val);}

  /** set command for setting the flow through the extend or retract port of auxiliary valve or a tractor, expressed as a
      percentage of full flow
      possible errors:
        * data range error
      @param valveNumber  auxiliary valve number (range from 0 to 15)
      @param t_val        value to store
      @return             true -> data was set; false -> data range error
    */
  bool setCmdPortFlow(const uint8_t valveNumber, const uint8_t t_val) {return TracAux_c::setCmdPortFlow(valveNumber, t_val);}

  /** set command for setting the fail save mode of auxiliary valve
      possible errors:
        * data range error
      @param valveNumber  auxiliary valve number (range from 0 to 15)
      @param t_val        value to store
      @return             true -> data was set; false -> data range error
    */
  bool setCmdFailSaveMode(const uint8_t valveNumber, const IsoAuxFlag_t t_val)
  {return TracAux_c::setCmdFailSaveMode(valveNumber, t_val);}

  /** set command for setting the auxiliary valve state
      possible errors:
        * data range error
      @param valveNumber  auxiliary valve number (range from 0 to 15)
      @param t_val        value to store
      @return             true -> data was set; false -> data range error
    */
  bool setCmdValveState(const uint8_t valveNumber, const IsoAuxFlagExtended_t t_val)
  {return TracAux_c::setCmdValveState(valveNumber, t_val);}
  /** set present limit status of auxiliary valve
      possible errors:
        * data range error
      @param valveNumber  auxiliary valve number (range from 0 to 15)
      @param at_val       limit status to store
      @return             true -> data was set; false -> data range error
    */
  bool setEstValveLimitStatus(const uint8_t valveNumber, const IsoAgLib::IsoLimitFlag_t at_val)
  {return TracAux_c::setEstValveLimitStatus(valveNumber, at_val);}
  /** set measured present limit status of auxiliary valve
      possible errors:
        * data range error
      @param valveNumber  auxiliary valve number (range from 0 to 15)
      @param at_val       limit status to store
      @return             true -> data was set; false -> data range error
    */
    bool setMeasuredValveLimitStatus(const uint8_t valveNumber, const IsoAgLib::IsoLimitFlag_t at_val)
    {return TracAux_c::setMeasuredValveLimitStatus(valveNumber, at_val);}
  /*@{*/

  /* ****************************************************** */
  /** \name Retrieve Values which are sent from other ECUs  */
  /*@{*/
  /** get value reported by the controller of flow through the extended port of auxiliary valve of a tractor
      which could be based on the commandes of the valve
      @param valveNumber  auxiliary valve number (range 0 to 15)
      @return             value of extended port of auxiliary valve number valveNumber
    */
  int8_t extendPortEstFlow(const uint8_t valveNumber)const {return TracAux_c::extendPortEstFlow(valveNumber);}

  /** get value reported by the controller of flow through the retract port of auxiliary valve of a tractor
      which could be based on the commandes of the valve
      @param valveNumber  auxiliary valve number (range 0 to 15)
      @return             value of extended port of auxiliary valve number valveNumber
    */
  int8_t retractPortEstFlow(const uint8_t valveNumber)const {return TracAux_c::retractPortEstFlow(valveNumber);}

  /** get measured state of the fail save mode of auxiliary valve
      @param valveNumber  auxiliary valve number (range 0 to 15)
      @return             value of extended port of auxiliary valve number valveNumber
    */
  IsoAuxFlag_t estFailSaveMode(const uint8_t valveNumber)const {return TracAux_c::estFailSaveMode(valveNumber);}

  /** get measured state of the auxiliary valve
      @param valveNumber  auxiliary valve number (range 0 to 15)
      @return             value of extended port of auxiliary valve number valveNumber
    */
  IsoAuxFlagExtended_t estValveState(const uint8_t valveNumber)const {return TracAux_c::estValveState(valveNumber);}

  /** get measured flow through the extend port auxiliary valve of a tractor, expressed as a percentage of full flow
      @param valveNumber  auxiliary valve number (range 0 to 15)
      @return             value of extended port of auxiliary valve number valveNumber
  */
  int8_t extendPortMeasuredFlow(const uint8_t valveNumber)const {return TracAux_c::extendPortMeasuredFlow(valveNumber);}

  /** get measured flow through the retract port auxiliary valve of a tractor, expressed as a percentage of full flow
      @param valveNumber  auxiliary valve number (range 0 to 15)
      @return             value of extended port of auxiliary valve number valveNumber
    */
  int8_t retractPortMeasuredFlow(const uint8_t valveNumber)const {return TracAux_c::retractPortMeasuredFlow(valveNumber);}

  /** get measured nominal pressure at the extend port of auxiliary valve of a tractor
      @param valveNumber  auxiliary valve number (range 0 to 15)
      @return             value of extended port of auxiliary valve number valveNumber
    */
  uint32_t extendPortPressure(const uint8_t valveNumber)const {return TracAux_c::extendPortPressure(valveNumber);}

  /** get measured nominal pressure at the retract port of auxiliary valve of a tractor
      @param valveNumber  auxiliary valve number (range 0 to 15)
      @return             value of extended port of auxiliary valve number valveNumber
    */
  uint32_t retractPortPressure(const uint8_t valveNumber)const {return TracAux_c::retractPortPressure(valveNumber);}

  /** get measured nominal pressure at the return port of auxiliary valve of a tractor
      @param valveNumber  auxiliary valve number (range 0 to 15)
      @return             value of extended port of auxiliary valve number valveNumber
    */
  uint16_t returnPortPressure(const uint8_t valveNumber)const {return TracAux_c::returnPortPressure(valveNumber);}

  /** get command for setting the flow through the extend or retract port of auxiliary valve or a tractor, expressed as a
      percentage of full flow
      @param valveNumber  auxiliary valve number (range 0 to 15)
      @return             value of extended port of auxiliary valve number valveNumber
    */
  uint8_t cmdPortFlow(const uint8_t valveNumber)const {return TracAux_c::cmdPortFlow(valveNumber);}

  /** get command for setting the fail save mode of auxiliary valve
      @param valveNumber  auxiliary valve number (range 0 to 15)
      @return             value of extended port of auxiliary valve number valveNumber
    */
  IsoAuxFlag_t cmdFailSaveMode(const uint8_t valveNumber)const {return TracAux_c::cmdFailSaveMode(valveNumber);}

  /** get command for setting the auxiliary valve state
      @param valveNumber  auxiliary valve number (range 0 to 15)
      @return             value of extended port of auxiliary valve number valveNumber
    */
  IsoAuxFlagExtended_t cmdValveState(const uint8_t valveNumber)const {return TracAux_c::cmdValveState(valveNumber);}
  /** get present limit status of auxiliary valve
      @param valveNumber  auxiliary valve number (range from 0 to 15)
      @return             present limit status
    */
  IsoAgLib::IsoLimitFlag_t estValveLimitStatus(const uint8_t valveNumber)const
  {return TracAux_c::estValveLimitStatus(valveNumber);}
  /** get measured present limit status of auxiliary valve
      @param valveNumber  auxiliary valve number (range from 0 to 15)
      @return             present limit status
    */
  IsoAgLib::IsoLimitFlag_t measuredValveLimitStatus(const uint8_t valveNumber) const
  {return TracAux_c::measuredValveLimitStatus(valveNumber);}
  /*@}*/

  private:
    /** allow getITracMoveInstance() access to shielded tracgeneral class.
    otherwise __IsoAgLib::getTracMoveInstance4Comm() wouldn't be accepted by compiler
    */
    #if defined(PRT_INSTANCE_CNT) && (PRT_INSTANCE_CNT > 1)
    friend iTracAux_c& getITracAuxInstance(uint8_t aui8_instance);
    #else
    friend iTracAux_c& getITracAuxInstance(void);
    #endif

  };

  #if defined(PRT_INSTANCE_CNT) && (PRT_INSTANCE_CNT > 1)
  /** C-style function, to get access to the unique TracMove_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  inline iTracAux_c& getITracAuxInstance(uint8_t aui8_instance = 0)
  { return static_cast<iTracAux_c&>(__IsoAgLib::getTracAuxInstance(aui8_instance));};
  #else
  /** C-style function, to get access to the unique TracMove_c singleton instance */
  inline iTracAux_c& getITracAuxInstance(void)
  { return static_cast<iTracAux_c&>(__IsoAgLib::getTracAuxInstance());};
  #endif
}
#endif
