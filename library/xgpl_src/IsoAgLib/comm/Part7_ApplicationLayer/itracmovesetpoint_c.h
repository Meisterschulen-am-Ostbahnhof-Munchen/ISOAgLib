/***************************************************************************
                itracmove_c.h  - working on Base Data Msg Type NMEA 200 GPS
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
#ifndef ITRACMOVESETPOINT_C_H
#define ITRACMOVESETPOINT_C_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include <IsoAgLib/comm/Part5_NetworkManagement/iisoname_c.h>
#include "ibasetypes.h"
#include "impl/tracmovesetpoint_c.h"


// Begin Namespace IsoAgLib
namespace IsoAgLib {

  /** working on Base Data Msg Type 1;
      stores, updates  and delivers all base data informations;
      Derive from Scheduler_Task_c to register in Scheduler_c for timeEvent trigger
      Derive from CANCustomer to register FilterBox'es in CanIo_c to receive CAN messages
      Derive from SINGLETON to create a Singleton which manages one global accessible singleton
      per IsoAgLib instance (if only one IsoAgLib instance is defined in application config, no overhead is produced).
    */
  class iTracMoveSetPoint_c : private __IsoAgLib::TracMoveSetPoint_c {
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
    { return TracMoveSetPoint_c::config_base (apc_isoName, at_identMode, aui16_suppressMask ); }

    /** set commanded set point value of the machine speed as measured by the selected source
        @param ui16_val  set point value of the machine speed
      */
    void setSelectedSpeedSetPointCmd(const uint16_t ui16_val)
    {TracMoveSetPoint_c::setSelectedSpeedSetPointCmd(ui16_val);}
    /** communicate maximum allowed speed to the tractor
        @param ui16_val  maximum allowed speed
      */
    void setSelectedSpeedSetPointLimit(const uint16_t ui16_val)
    {TracMoveSetPoint_c::setSelectedSpeedSetPointLimit(ui16_val);}
    /** set commanded direction of the machine
        @param t_val  commanded direction of travel
      */
    void setSelectedDirectionCmd(const IsoAgLib::IsoDirectionFlag_t t_val) {TracMoveSetPoint_c::setSelectedDirectionCmd(t_val);}

    /** get commanded set point value of the machine speed as measured by the selected source
        @return  set point value of the machine speed
      */
    uint16_t selectedSpeedSetPointCmd() const {return TracMoveSetPoint_c::selectedSpeedSetPointCmd();}
    /** get communicated maximum allowed speed to the tractor
        @return  maximum allowed speed
      */
    uint16_t selectedSpeedSetPointLimit() const {return TracMoveSetPoint_c::selectedSpeedSetPointLimit();}
    /** get commanded direction of the machine
        @return  commanded direction of travel
      */
    IsoAgLib::IsoDirectionFlag_t selectedDirectionCmd() const {return TracMoveSetPoint_c::selectedDirectionCmd();}

  private:
    /** allow getITracMoveInstance() access to shielded tracgeneral class.
    otherwise __IsoAgLib::getTracMoveInstance4Comm() wouldn't be accepted by compiler
    */
    #if defined(PRT_INSTANCE_CNT) && (PRT_INSTANCE_CNT > 1)
    friend iTracMoveSetPoint_c& getITracMoveSetPointInstance(uint8_t aui8_instance);
    #else
    friend iTracMoveSetPoint_c& getITracMoveSetPointInstance(void);
    #endif

  };

  #if defined(PRT_INSTANCE_CNT) && (PRT_INSTANCE_CNT > 1)
  /** C-style function, to get access to the unique TracMove_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  inline iTracMoveSetPoint_c& getITracMoveSetPointInstance(uint8_t aui8_instance = 0)
  { return static_cast<iTracMoveSetPoint_c&>(__IsoAgLib::getTracMoveSetPointInstance(aui8_instance));};
  #else
  /** C-style function, to get access to the unique TracMove_c singleton instance */
  inline iTracMoveSetPoint_c& getITracMoveSetPointInstance(void)
  { return static_cast<iTracMoveSetPoint_c&>(__IsoAgLib::getTracMoveSetPointInstance());};
  #endif
}
#endif
