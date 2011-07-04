/*
  processdatachangehandler_c.cpp: base class for application handler
    classes, with handler methods which are called after each change
    access on process data from external systems

  (C) Copyright 2009 - 2011 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include "processdatachangehandler_c.h"
#include "iprocdatalocal_c.h"


namespace IsoAgLib {

iProcDataLocal_c* EventSource_c::makeIProcDataLocal( void ) {
  return static_cast<iProcDataLocal_c*>(pc_src);
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
