/***************************************************************************
                          isetpointlocal_c.h - object for managing setpoints
                                             commanded from remote to local
                                             process data objects
                             -------------------
    begin                : Fri Apr 07 2000
    copyright            : (C) 2000 - 2004 by Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
    type                 : Header
    $LastChangedDate$
    $LastChangedRevision$
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
#ifndef ISETPOINT_LOCAL_H
#define ISETPOINT_LOCAL_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include "impl/setpointlocal_c.h"
#include "../../StdSetpointElements/isetpointregister_c.h"

// Begin Namespace IsoAgLib
namespace IsoAgLib {

/**
  object for managing setpoints commanded from remote to local process data objects;
  handling of received setpoints;
  managing the master setpoint, which is used for controlling;
  help for sending correct notify;
  semi automatic and manual handling of received values
  @author Dipl.-Inform. Achim Spangler
*/
class iSetpointLocal_c : private __IsoAgLib::SetpointLocal_c {
public:

  /**
    set if master setpoint should be preserved even if caller
    devKey is no more active (default false)
    @param rb_static choose if master setpoint should be preserved
          if caller isn't acitve
  */
  void setStaticMaster(bool rb_static = true)
    {SetpointLocal_c::setStaticMaster(rb_static);};
  /**
    check if actual a master setpoint item is defined
    @see master
    @return true -> an active master setpoint exist
  */
  bool existMaster(){return SetpointLocal_c::existMaster();};
  /**
    deliver the actual master entry
    @see existMaster
    @return actual valid master setpoint
  */
  iSetpointRegister_c& master()
    {return ((iSetpointRegister_c&)(SetpointLocal_c::master()));};
  /**
    check if unhandled new setpoint from actual master was received
    -> check the actual master first before answering the other unhandled
    @see unhandledMaster
    @see acceptNewMaster
    @return true -> master setpoint comanding member sent new setpoint
  */
  bool existUnhandledMaster() {return SetpointLocal_c::existUnhandledMaster();};
  /**
    deliver the new sent setpoint values from the actual master;
    if no unhandled setpoint of the master exist, return the actual accepted value

    possible errors:
        * Err_c::elNonexistent no master setpoint found
    @see existUnhandledMaster
    @see acceptNewMaster
    @return reference to master setpoint register
  */
  iSetpointRegister_c& unhandledMaster()
      {return ((iSetpointRegister_c&)(SetpointLocal_c::unhandledMaster()));};
  /**
    if there exist a new unhandled setpoint from the master setpoint sender;
    accept the new value with this command
    otherwise the old value is still used and answered

    possible errors:
        * Err_c::elNonexistent no master setpoint found
    @see existUnhandledMaster
    @see unhandledMaster
    @param rb_accept true ( default ) -> new setpoint of master is accepted -> send positive notify
  */
  void acceptNewMaster(bool rb_accept = true ) {SetpointLocal_c::acceptNewMaster(rb_accept);};
  /**
    set the master setpoint manually
    (in some cases remote systems request informations
     through process data setpoints)
    @param ri32_val wanted setpoint value
  */
  void setMasterMeasurementVal(int32_t ri32_val)
    {SetpointLocal_c::setMasterMeasurementVal(ri32_val);};

  /**
    deliver the count of unhandled setpoints
    @return count of unhandled received setpoints
  */
  uint8_t unhandledCnt() {return SetpointLocal_c::unhandledCnt();};
  /**
    deliver the rui8_ind of the unhandled setpoints
    @param rui8_ind position of the wanted setpoint entry in list of unhandled setpoints
      (first entry: rui8_ind == 0!!)

    possible errors:
        * Err_c::range there are less than rui8_ind unhandled setpoints found
    @see unhandledCnt
    @return wanted unhandled setpoint
  */
  iSetpointRegister_c& unhandledInd(uint8_t rui8_ind)
    {return ((iSetpointRegister_c&)(SetpointLocal_c::unhandledInd(rui8_ind)));};
  /**
    deliver the first unhandled entry

    possible errors:
        * Err_c::elNonexistent no unhandled stepoint exist
    @see unhandledCnt
    @return reference to first unhandled new received setpoint entry
  */
  iSetpointRegister_c& unhandledFirst()
    {return ((iSetpointRegister_c&)(SetpointLocal_c::unhandledFirst()));};
  /**
    answer all unhandled setpoint enties;
    if new master setpoint was set, send the master
    the new accepted setpoint value;
    send all others NO_VAL_32S

    possible errors:
        * dependant error in ProcessDataLocal_c commander of some setpoint isn't found in Monitor List
        * dependant error in CANIO_c on CAN send problems
  */
  void respondAckNack() {SetpointLocal_c::respondAckNack();};
  /**
    set the allowed percentual deviation between actual measurement and
    existing master setpoint
    @param rb_percent allowed percantual deviation
  */
  void setAllowedDeltaPercent(uint8_t rb_percent)
    {SetpointLocal_c::setAllowedDeltaPercent(rb_percent);};
  /**
    get the allowed percentual deviation between actual measurement and
    existing master setpoint
    @return allowed percantual deviation
  */
  uint8_t allowedDeltaPercent()const
    {return SetpointLocal_c::allowedDeltaPercent();};
  /**
    check if the given measuremet value is correct for the actual
    master setpoint;
    @param ri32_val measured value
    @param rb_sendIfError true -> if actual value exceeds setpoint limits
           the actual value is sent as notification (default true)
    @return 0 -> correct; (n<0) -> measurement is delta n to low;
            (n>0) -> measurement is delta n to high
  */
  int32_t checkMeasurement(int32_t ri32_val, bool rb_sendIfError = true)
    {return SetpointLocal_c::checkMeasurement(ri32_val, rb_sendIfError);};
 private:
  friend class iProcDataLocal_c;
  friend class iProcDataLocalSimpleMeasure_c;
  friend class iProcDataLocalSimpleSetpoint_c;
  friend class iProcDataLocalSimpleSetpointSimpleMeasure_c;
};

}
#endif
