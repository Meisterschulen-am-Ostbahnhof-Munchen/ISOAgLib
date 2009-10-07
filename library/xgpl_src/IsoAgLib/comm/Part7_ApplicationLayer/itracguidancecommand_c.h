/***************************************************************************
      itracguidancecommand_c.h  - working on guidance command data;
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
#ifndef ISOAGLIBITRACGUIDANCECOMMAND_C_H
#define ISOAGLIBITRACGUIDANCECOMMAND_C_H

#include "ibasetypes.h"
#include "impl/tracguidancecommand_c.h"


namespace IsoAgLib {

/** stores, updates  and delivers guidance command data information;
    Derive from BaseCommon_c some fundamental funktionality for all base data
    Derive from Scheduler_Task_c to register in Scheduler_c for timeEvent trigger
    Derive from CANCustomer to register FilterBox'es in CanIo_c to receive CAN messages
    Derive from SINGLETON to create a Singleton which manages one global accessible singleton
    per IsoAgLib instance (if only one IsoAgLib instance is defined in application config, no overhead is produced).
  */
class iTracGuidanceCommand_c :
private __IsoAgLib::TracGuidanceCommand_c {
 public:
  // Public methods
  /**
      config the iTracGuidanceCommand_c object after init -> set pointer to isoName and
      config send/receive of different base msg types
      @param apc_isoName pointer to the ISOName variable of the responsible member instance (pointer enables automatic value update if var val is changed)
      @param at_identMode set mode to either IsoAgLib::IdentModeImplement or IsoAgLib::IdentModeTractor
      @return true -> configuration was successfull
    */
  bool config (const iIsoName_c* apc_isoName, IsoAgLib::IdentMode_t at_identMode = IsoAgLib::IdentModeImplement, uint16_t aui16_suppressMask = 0)
  { return TracGuidanceCommand_c::config_base (apc_isoName, at_identMode, aui16_suppressMask ); }

  /* ******************************************* */
  /** \name Set Values for periodic send on BUS  */
  /*@{*/
  /** set desired course curvature over ground that a machine's steering system is required to achieve
      @param  i16_val  desired course curvature over ground
    */
  void setCurvatureCmd(int16_t i16_val) {TracGuidanceCommand_c::setCurvatureCmd( i16_val );}

  /** set whether the guidance system is attempting to control steering with this command
      @param  t_val  is guidance system attempting to control steering
    */
  void setCurvatureCmdStatus(IsoAgLib::IsoSteerFlag_t t_val) {TracGuidanceCommand_c::setCurvatureCmdStatus( t_val );}

  /** get currently selected curvature commander source address
      @param  ui8_sourceAddress  source address of curvature commander
      */
  void setCommander(uint8_t ui8_sourceAddress) {TracGuidanceCommand_c::setCommander( ui8_sourceAddress );}
  /*@}*/

  /* ****************************************************** */
  /** \name Retrieve Values which are sent from other ECUs  */
  /*@{*/
  /** set desired course curvature over ground that a machine's steering system is required to achieve
      @return  desired course curvature over ground
    */
  int16_t curvatureCmd() {return TracGuidanceCommand_c::curvatureCmd();}

  /** set whether the guidance system is attempting to control steering with this command
      @return  is guidance system attempting to control steering
    */
  IsoAgLib::IsoSteerFlag_t curvatureCmdStatus() {return TracGuidanceCommand_c::curvatureCmdStatus();}

  /** get currently selected curvature commander source address
      @return  source address of curvature commander
      */
  uint8_t getCommander() {return TracGuidanceCommand_c::getCommander();}
  /*@}*/

 private:
  /** allow getITracGuidanceInstance() access to shielded traccert class.
    otherwise __IsoAgLib::getTracGuidanceInstance() wouldn't be accepted by compiler
  */
  #if defined(PRT_INSTANCE_CNT) && (PRT_INSTANCE_CNT > 1)
  friend iTracGuidanceCommand_c& getITracGuidanceCommandInstance(uint8_t aui8_instance);
  #else
  friend iTracGuidanceCommand_c& getITracGuidanceCommandInstance(void);
  #endif

};

#if defined(PRT_INSTANCE_CNT) && (PRT_INSTANCE_CNT > 1)
/** C-style function, to get access to the unique iTracGuidance_c singleton instance
  * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
  */
inline iTracGuidanceCommand_c& getITracGuidanceCommandInstance(uint8_t aui8_instance = 0)
{ return static_cast<iTracGuidanceCommand_c&>(__IsoAgLib::getTracGuidanceCommandInstance(aui8_instance));}
#else
/** C-style function, to get access to the unique iTracGuidance_c singleton instance */
inline iTracGuidanceCommand_c& getITracGuidanceCommandInstance(void)
{ return static_cast<iTracGuidanceCommand_c&>(__IsoAgLib::getTracGuidanceCommandInstance());}
#endif

} // end of namespace IsoAgLib

#endif
