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
#include <IsoAgLib/util/igetypos_c.h>
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
    DEPRECATED CONFIG FUNCTION - PLEASE DO NOT USE ANY MORE AS THIS FUNCTION WILL BE REMOVED SOON
    config the Base_c object after init -> set pointer to devKey and
    config send/receive of different base msg types
    @param rpc_devKey pointer to the DEV_KEY variable of the responsible member instance (pointer enables automatic value update if var val is changed)
    @param rb_implementMode implement mode (true) or tractor mode (false)!!!
  */
  void config(const iDevKey_c* rpc_devKey, bool rb_implementMode = true)
  {
    #ifdef SYSTEM_PC
    #warning "deprecated, use IsoAgLib::IdentMode_t as parameter instead of bool"
    #endif
    TracMove_c::config(rpc_devKey, (rb_implementMode ? IsoAgLib::IdentModeImplement : IsoAgLib::IdentModeTractor));
  }

  /**
    config the Base_c object after init -> set pointer to devKey and
    config send/receive of different base msg types
    @param rpc_devKey pointer to the DEV_KEY variable of the responsible member instance (pointer enables automatic value update if var val is changed)
    @param rt_identMode set mode to either IsoAgLib::IdentModeImplement or IsoAgLib::IdentModeTractor
  */
  void config(const iDevKey_c* rpc_devKey, IsoAgLib::IdentMode_t rt_identMode = IsoAgLib::IdentModeImplement)
  { TracMove_c::config(rpc_devKey, rt_identMode ); }


    /* ******************************************* */
    /** \name Set Values for periodic send on BUS  */
    /*@{*/

    /** set the value of real speed (measured by radar)
        @param ri16_val value to store as real radar measured speed
      */
    void setSpeedReal(int16_t ri16_val) {return TracMove_c::setSpeedReal(ri16_val);};
    /** set the value of theoretical speed (calculated from gear)
        @param ri16_val value to store as theoretical gear calculated speed
      */
    void setSpeedTheor(int16_t ri16_val) {return TracMove_c::setSpeedTheor(ri16_val);};
    /** set the real (radar measured) driven distance with int16_t val
        @param ri16_val value to store as real radar measured distance
      */
    void setDistReal(int16_t ri16_val) {return TracMove_c::setDistReal(ri16_val);};
    /** set the real (radar measured) driven distance with int32_t val
        @param rreflVal value to store as real radar measured distance
      */
    void setDistReal(const int32_t& rreflVal) {return TracMove_c::setDistReal(rreflVal);};
    /** set the theoretical (gear calculated) driven distance with int16_t val
        @param ri16_val value to store as theoretical (gear calculated) driven distance
      */
    void setDistTheor(int16_t ri16_val) {return TracMove_c::setDistTheor(ri16_val);};
    /** set the theoretical (gear calculated) driven distance with int32_t val
        @param rreflVal value to store as theoretical (gear calculated) driven distance
      */
    void setDistTheor(const int32_t& rreflVal) {return TracMove_c::setDistTheor(rreflVal);};
    /*@}*/

    /* ****************************************************** */
    /** \name Retrieve Values which are sent from other ECUs  */
    /*@{*/

    /** get the value of real speed (measured by radar)
        @return actual radar measured speed value
      */
    int16_t speedReal() const {return TracMove_c::speedReal();};
    /** get the value of theoretical speed (calculated from gear)
        @return theoretical gear calculated speed value
      */
    int16_t speedTheor() const {return TracMove_c::speedTheor();};

    /** get the real driven distance with int16_t val
        @return actual radar measured driven distance value
      */
    int32_t distReal() const {return TracMove_c::distReal();};
    /** get the real driven distance with int16_t val
        @return actual gear calculated driven distance value
      */
    int32_t distTheor() const {return TracMove_c::distTheor();};

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
  { return static_cast<iTracMove_c&>(__IsoAgLib::getTracMoveInstance(rui8_instance));};
  #else
  /** C-style function, to get access to the unique TracMove_c singleton instance */
  inline iTracMove_c& getITracMoveInstance(void)
  { return static_cast<iTracMove_c&>(__IsoAgLib::getTracMoveInstance());};
  #endif
}
#endif
