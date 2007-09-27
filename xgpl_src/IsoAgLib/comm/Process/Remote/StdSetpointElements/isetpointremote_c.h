/***************************************************************************
                          isetpointremote_c.h - object for commanding remote
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
#ifndef ISETPOINT_REMOTE_H
#define ISETPOINT_REMOTE_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include "impl/setpointremote_c.h"
#include "../../StdSetpointElements/isetpointregister_c.h"

// Begin Namespace IsoAgLib
namespace IsoAgLib {

/**
  object for commanding remote process data objects;
  helps checking,
  - if commanded value is accepted,
  - if other setpoint is handled as master,
  - if this item is valid or invalid (detect by actual measure value)
  @author Dipl.-Inform. Achim Spangler
*/
class iSetpointRemote_c : private __IsoAgLib::SetpointRemote_c {
public:

  /**
    command a exact setpoint; store value as commanded and send command

    possible errors:
        * dependant error in ProcessDataRemote_c if comanded remote system not found in Monitor List
        * dependant error in CanIo_c on CAN send problems
    @return new exact setpoint to command
  */
  void setExact(int32_t ri32_val) {SetpointRemote_c::setExact(ri32_val);};
  /**
    command a minimum setpoint; store value as commanded and send command

    possible errors:
        * dependant error in ProcessDataRemote_c if comanded remote system not found in Monitor List
        * dependant error in CanIo_c on CAN send problems
    @return new minimum setpoint to command
  */
  void setMin(int32_t ri32_val) {SetpointRemote_c::setMin(ri32_val);};
  /**
    command a maximum setpoint; store value as commanded and send command

    possible errors:
        * dependant error in ProcessDataRemote_c if comanded remote system not found in Monitor List
        * dependant error in CanIo_c on CAN send problems
    @return new maximum setpoint to command
  */
  void setMax(int32_t ri32_val) {SetpointRemote_c::setMax(ri32_val);};
  /**
    command a default setpoint; store value as commanded and send command

    possible errors:
        * dependant error in ProcessDataRemote_c if comanded remote system not found in Monitor List
        * dependant error in CanIo_c on CAN send problems
    @return new default setpoint to command
  */
  void setDefault(int32_t ri32_val) {SetpointRemote_c::setDefault(ri32_val);};
  /**
    request remote master setpoint - exact
  */
  void requestExact(){SetpointRemote_c::requestExact();};
  /**
    request remote master setpoint - MIN
  */
  void requestMin(){SetpointRemote_c::requestMin();};
  /**
    request remote master setpoint - MAX
  */
  void requestMax(){SetpointRemote_c::requestMax();};
  /**
    request remote master setpoint - DEFAULT
  */
  void requestDefault(){SetpointRemote_c::requestDefault();};
  /**
    check if the last commanded setpoint was accepted
    (am I am now the master)
    @return true -> the last commanded setpoint was accepted by remote system
  */
  bool accepted() {return SetpointRemote_c::accepted();};
  /**
    check if the last setpoint command was answered
    @return true -> an answer to the last command was received
  */
  bool answered() const
    {return SetpointRemote_c::answered();};
  /**
    check if an active master setpoint exist
    @return true -> a master setpoint commander exist
  */
  bool existMaster() {return SetpointRemote_c::existMaster();};
  /**
    deliver master setpoint entry
    @return reference to master setpoint
  */
  const iSetpointRegister_c& master()
    {return ((const iSetpointRegister_c&)(SetpointRemote_c::master()));};
  /**
    mark the master setpoint as released
    (send a suitable reset command)

    possible errors:
        * Err_c::precondition I'm not the master -> can't release master
        * dependant error in ProcessDataRemote_c if comanded remote system not found in Monitor List
        * dependant error in CanIo_c on CAN send problems
  */
  void releaseMaster() {SetpointRemote_c::releaseMaster();};
 /**
    check if my commanded setpoint s valid:
    a) I am the master
    b) my setpoint is valid for the actual master setpoint
    @return true -> master OR my setpoint meets the actual master setpoint
  */
  bool valid() {return SetpointRemote_c::valid();};
  /**
    check if the master setpoint is actual valid
    (e.g. the controlling ECU can control as commanded by setpoint)
    @return true -> the remote system controls the process data value as commanded
  */
  bool masterValid() const {return SetpointRemote_c::masterValid();};
 private:
  friend class iProcDataRemote_c;
  friend class iProcDataRemoteSimpleMeasure_c;
  friend class iProcDataRemoteSimpleSetpoint_c;
  friend class iProcDataRemoteSimpleSetpointSimpleMeasure_c;
};

}
#endif
