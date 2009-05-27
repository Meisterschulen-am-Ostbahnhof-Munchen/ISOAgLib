/***************************************************************************
                          processdatachangehandler_c.cpp -
                                   base class for application handler
                                   classes, with handler methods which are called
                                   after each change access on process data
                                   from external systems
                             -------------------
    begin                : Thu Jul 24 2003
    copyright            : (C) 2000 - 2009 by Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
    type                 : Source
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
#include "processdatachangehandler_c.h"
#include "Local/Std/iprocdatalocal_c.h"
#include "Local/SimpleMeasure/iprocdatalocalsimplemeasure_c.h"
#include "Local/SimpleSetpoint/iprocdatalocalsimplesetpoint_c.h"
#include "Local/SimpleMeasureSetpoint/iprocdatalocalsimplesetpointsimplemeasure_c.h"
#include "Remote/Std/iprocdataremote_c.h"
#include "Remote/SimpleMeasure/iprocdataremotesimplemeasure_c.h"
#include "Remote/SimpleSetpoint/iprocdataremotesimplesetpoint_c.h"
#include "Remote/SimpleMeasureSetpoint/iprocdataremotesimplesetpointsimplemeasure_c.h"
#include "Remote/SimpleMeasureSetpoint/iprocdataremotesimplesetpointsimplemeasurecombined_c.h"

namespace IsoAgLib {
iProcDataLocal_c* EventSource_c::makeIProcDataLocal( void ) {
  return static_cast<iProcDataLocal_c*>(pc_src);
}

iProcDataLocalSimpleMeasure_c* EventSource_c::makeIProcDataLocalSimpleMeasure( void ) {
  return static_cast<iProcDataLocalSimpleMeasure_c*>(pc_src);
}

iProcDataLocalSimpleSetpoint_c* EventSource_c::makeIProcDataLocalSimpleSetpoint( void ) {
 return static_cast<iProcDataLocalSimpleSetpoint_c*>(pc_src);
}

iProcDataLocalSimpleSetpointSimpleMeasure_c* EventSource_c::makeIProcDataLocalSimpleSetpointSimpleMeasure( void ) {
  return static_cast<iProcDataLocalSimpleSetpointSimpleMeasure_c*>(pc_src);
}

iProcDataRemote_c* EventSource_c::makeIProcDataRemote( void ) {
  return static_cast<iProcDataRemote_c*>(pc_src);
}

iProcDataRemoteSimpleMeasure_c* EventSource_c::makeIProcDataRemoteSimpleMeasure( void ) {
  return static_cast<iProcDataRemoteSimpleMeasure_c*>(pc_src);
}

iProcDataRemoteSimpleSetpoint_c* EventSource_c::makeIProcDataRemoteSimpleSetpoint( void ) {
  return static_cast<iProcDataRemoteSimpleSetpoint_c*>(pc_src);
}

iProcDataRemoteSimpleSetpointSimpleMeasure_c* EventSource_c::makeIProcDataRemoteSimpleSetpointSimpleMeasure( void ) {
  return static_cast<iProcDataRemoteSimpleSetpointSimpleMeasure_c*>(pc_src);
}

iProcDataRemoteSimpleSetpointSimpleMeasureCombined_c* EventSource_c::makeIProcDataRemoteSimpleSetpointSimpleMeasureCombined( void ) {
  return static_cast<iProcDataRemoteSimpleSetpointSimpleMeasureCombined_c*>(pc_src);
}


/** react on new received setpoint for local process data
  * (remote system which wants to control the local system dependent on the setpoint
  *  sent a new setpoint value)
  * @param ac_src general event source class, which provides conversion functions to get needed event source class
  * @param ai32_val new value, which caused the event (for immediate access)
  * @param ac_callerISOName ISOName of calling device - i.e. which sent new setpoint
  * @return true -> handler class reacted on change event
  */
bool ProcessDataChangeHandler_c::processSetpointSet( EventSource_c /* ac_src is intentionally unused in base class */, int32_t /* ai32_val is intentionally unused in base class */,
                                                     const iIsoName_c& /* ac_callerISOName is intentionally unused in base class */, bool /* ab_change is intentionally unused in base class */ ) {
  // empty body -> allow application class to just implement some of the possible handler methods
  return false;
}

/** react on measurement reset from external system for local process data
  * @param ac_src general event source class, which provides conversion functions to get needed event source class
  * @param ai32_val new value, which caused the event (for immediate access)
  * @param ac_callerISOName ISOName of calling device - i.e. which sent new setpoint
  * @return true -> handler class reacted on change event
  */
bool ProcessDataChangeHandler_c::processMeasurementReset( EventSource_c /* ac_src is intentionally unused in base class */, int32_t /* ai32_val is intentionally unused in base class */,
                                                          const iIsoName_c& /* ac_callerISOName is intentionally unused in base class */ ) {
  // empty body -> allow application class to just implement some of the possible handler methods
  return false;
}

/** react on new received measurement update for remote process data
  * (remote system which manages the process data sent new value on request or
  *  during active measurement programm)
  * @param ac_src general event source class, which provides conversion functions to get needed event source class
  * @param ai32_val new value, which caused the event (for immediate access)
  * @param ac_callerISOName ISOName of calling device - i.e. which sent new setpoint
  * @return true -> handler class reacted on change event
  */
bool ProcessDataChangeHandler_c::processMeasurementUpdate( EventSource_c /* ac_src is intentionally unused in base class */, int32_t /* ai32_val is intentionally unused in base class */,
                                                           const iIsoName_c& /* ac_callerISOName is intentionally unused in base class */, bool /* ab_change is intentionally unused in base class */ ) {
  // empty body -> allow application class to just implement some of the possible handler methods
  return false;
}

/** react on received setpoint ACK or NACK upon previous setpoint set for remote process data
  * (remote system which manages the process data, local or other system sent previously a
  *  new setpoint; commanded manager of process data sent the response with ACK/NACK)
  * @param ac_src general event source class, which provides conversion functions to get needed event source class
  * @param ai32_val new value, which caused the event (for immediate access)
  * @param ac_callerISOName ISOName of calling device - i.e. which sent new setpoint
  * @return true -> handler class reacted on change event
  */
bool ProcessDataChangeHandler_c::processSetpointResponse( EventSource_c /* ac_src is intentionally unused in base class */, int32_t /* ai32_val is intentionally unused in base class */,
                                                          const iIsoName_c& /* ac_callerISOName is intentionally unused in base class */ ) {
  // empty body -> allow application class to just implement some of the possible handler methods
  return false;
}

/** react on received value request for default data logging (DDI 0xDFFF)
  * (can be used to start measurement programms in local process data instances)
  * @param ac_src general event source class, which provides conversion functions to get needed event source class
  * @param ai32_val new value, which caused the event (for immediate access)
  * @param ac_callerISOName ISOName of calling device - i.e. which sent new setpoint
  * @return true -> handler class reacted on change event
  */
bool ProcessDataChangeHandler_c::processDefaultLoggingStart( EventSource_c /* ac_src is intentionally unused in base class */, int32_t /* ai32_val is intentionally unused in base class */,
                                                             const iIsoName_c& /* ac_callerISOName is intentionally unused in base class */ ) {
  // empty body -> allow application class to just implement some of the possible handler methods
  return false;
}

/** react on received value for TC status message
  * @param ab_taskRunning
  * @param ac_callerISOName ISONameof calling device - i.e. TC
  * @return true -> handler class reacted on change event
  */
bool ProcessDataChangeHandler_c::processTcStatusMessage(bool /* ab_taskRunning */, const iIsoName_c& /* ac_callerISOName */ )
{
  // empty body -> allow application class to just implement some of the possible handler methods
  return false;
}

} // end of namespace IsoAgLib
