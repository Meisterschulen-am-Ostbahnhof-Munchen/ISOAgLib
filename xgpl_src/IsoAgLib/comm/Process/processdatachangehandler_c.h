/***************************************************************************
                          processdatachangehandler_c.h -
                                   base class for application handler
                                   classes, with handler methods which are called
                                   after each change access on process data
                                   from external systems
                             -------------------
    begin                : Thu Jul 24 2003
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
 * starting a measuring program for a process data value on a Remote ECU,  *
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
#ifndef PROCESSDATACHANGEHANDLER_H
#define PROCESSDATACHANGEHANDLER_H

#include <IsoAgLib/util/igetypos_c.h>

// predeclare __IsoAgLib_c::ProcDataBase_c
namespace __IsoAgLib {
  class ProcDataBase_c;
}

// Begin Namespace IsoAgLib
namespace IsoAgLib {
  // predeclare the possible event source classes
  class iProcDataLocal_c;
  class iProcDataLocalSimpleMeasure_c;
  class iProcDataLocalSimpleSetpoint_c;
  class iProcDataLocalSimpleSetpointSimpleMeasure_c;
  class iProcDataRemote_c;
  class iProcDataRemoteSimpleMeasure_c;
  class iProcDataRemoteSimpleSetpoint_c;
  class iProcDataRemoteSimpleSetpointSimpleMeasure_c;
  class iProcDataRemoteSimpleSetpointSimpleMeasureCombined_c;

/** as the handler functions are called by classes of different types, the functions
    deliver a pointer to the calling instance as variable of type EventSource_c.
    This class provides suitable conversion operators to references of the wanted
    type.
  */
class EventSource_c {
 public:
  EventSource_c( __IsoAgLib::ProcDataBase_c* rpc_src ) : pc_src(rpc_src) {};
  iProcDataLocal_c* makeIProcDataLocal( void );

  iProcDataLocalSimpleMeasure_c* makeIProcDataLocalSimpleMeasure( void );

  iProcDataLocalSimpleSetpoint_c* makeIProcDataLocalSimpleSetpoint( void );

  iProcDataLocalSimpleSetpointSimpleMeasure_c* makeIProcDataLocalSimpleSetpointSimpleMeasure( void );

  iProcDataRemote_c* makeIProcDataRemote( void );

  iProcDataRemoteSimpleMeasure_c* makeIProcDataRemoteSimpleMeasure( void );

  iProcDataRemoteSimpleSetpoint_c* makeIProcDataRemoteSimpleSetpoint( void );

  iProcDataRemoteSimpleSetpointSimpleMeasure_c* makeIProcDataRemoteSimpleSetpointSimpleMeasure( void );

  iProcDataRemoteSimpleSetpointSimpleMeasureCombined_c* makeIProcDataRemoteSimpleSetpointSimpleMeasureCombined( void );
 private:
  __IsoAgLib::ProcDataBase_c* pc_src;
};

/** Base class for application classes, with handler methods for different change events.
    This class allows all derived application classes to define a handler function which
    is called after the receive of a CAN message which caused a value change.
    This avoids polling process data for changes, so that reaction is quicker with less
    computing power.
    This is especially useful for:<ul>
    <li>local process data to react on received setpoint (either accept, reject and start realising)
    <li>remote process data to react on new received measurement value
    </ul>.
    An example is described in \ref ProcDataPage .
*/
class ProcessDataChangeHandler_c {
 public:
 virtual ~ProcessDataChangeHandler_c(){};
 /** react on new received setpoint for local process data
   * (remote system which wants to control the local system dependent on the setpoint
   *  sent a new setpoint value)
   * @param rc_src general event source class, which provides conversion functions to get needed event source class
   * @param ri32_val new value, which caused the event (for immediate access)
   * @param rc_callerGetyPos GetyPos of calling device - i.e. which sent new setpoint
   * @return true -> handler class reacted on change event
   */
 virtual bool processSetpointSet( EventSource_c rc_src, int32_t ri32_val, const iGetyPos_c& rc_callerGetyPos, bool rb_change );

 /** react on measurement reset from external system for local process data
   * @param rc_src general event source class, which provides conversion functions to get needed event source class
   * @param ri32_val new value, which caused the event (for immediate access)
   * @param rc_callerGetyPos GetyPos of calling device - i.e. which sent new setpoint
   * @return true -> handler class reacted on change event
   */
 virtual bool processMeasurementReset( EventSource_c rc_src, int32_t ri32_val, const iGetyPos_c& rc_callerGetyPos );

 /** react on new received measurement update for remote process data
   * (remote system which manages the process data sent new value on request or
   *  during active measurement programm)
   * @param rc_src general event source class, which provides conversion functions to get needed event source class
   * @param ri32_val new value, which caused the event (for immediate access)
   * @param rc_callerGetyPos GetyPos of calling device - i.e. which sent new setpoint
   * @return true -> handler class reacted on change event
   */
 virtual bool processMeasurementUpdate( EventSource_c rc_src, int32_t ri32_val, const iGetyPos_c& rc_callerGetyPos, bool rb_change );

#ifdef USE_FLOAT_DATA_TYPE
 /** react on new received measurement update for remote process data
   * (remote system which manages the process data sent new value on request or
   *  during active measurement programm)
   * @param rc_src general event source class, which provides conversion functions to get needed event source class
   * @param rf_val new value, which caused the event (for immediate access)
   * @param rc_callerGetyPos GetyPos of calling device - i.e. which sent new setpoint
   * @return true -> handler class reacted on change event
   */
 virtual bool processMeasurementUpdate( EventSource_c rc_src, float rf_val, const iGetyPos_c& rc_callerGetyPos, bool rb_change );
#endif

 /** react on received setpoint ACK or NACK upon previous setpoint set for remote process data
   * (remote system which manages the process data, local or other system sent previously a
   *  new setpoint; commanded manager of process data sent the response with ACK/NACK)
   * @param rc_src general event source class, which provides conversion functions to get needed event source class
   * @param ri32_val new value, which caused the event (for immediate access)
   * @param rc_callerGetyPos GetyPos of calling device - i.e. which sent new setpoint
   * @return true -> handler class reacted on change event
   */
 virtual bool processSetpointResponse( EventSource_c rc_src, int32_t ri32_val, const iGetyPos_c& rc_callerGetyPos );
};
}
#endif
