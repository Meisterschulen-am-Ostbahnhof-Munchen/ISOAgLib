/*
  processdatachangehandler_c.h: base class for application handler
    classes, with handler methods which are called after each change
    access on process data from external systems

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef PROCESSDATACHANGEHANDLER_H
#define PROCESSDATACHANGEHANDLER_H

#include <IsoAgLib/comm/Part5_NetworkManagement/iisoname_c.h>


// predeclare __IsoAgLib_c::ProcDataBase_c
namespace __IsoAgLib {
  class ProcDataBase_c;
}

// Begin Namespace IsoAgLib
namespace IsoAgLib {
  // predeclare the possible event source classes
  class iProcDataLocal_c;

/** as the handler functions are called by classes of different types, the functions
    deliver a pointer to the calling instance as variable of type EventSource_c.
    This class provides suitable conversion operators to references of the wanted
    type.
  */
class EventSource_c {
 public:
  EventSource_c( __IsoAgLib::ProcDataBase_c* apc_src ) : pc_src(apc_src) {};
  iProcDataLocal_c* makeIProcDataLocal( void );


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
   * @param ac_src general event source class, which provides conversion functions to get needed event source class
   * @param ai32_val new value, which caused the event (for immediate access)
   * @param ac_callerISOName ISOName of calling device - i.e. which sent new setpoint
   * @return true -> handler class reacted on change event
   */
 virtual bool processSetpointSet( EventSource_c ac_src, uint16_t rui16_ddi, int32_t ai32_val, const iIsoName_c& ac_callerISOName, bool ab_change ) = 0;

 /** react on measurement reset from external system for local process data
   * @param ac_src general event source class, which provides conversion functions to get needed event source class
   * @param ai32_val new value, which caused the event (for immediate access)
   * @param ac_callerISOName ISOName of calling device - i.e. which sent new setpoint
   * @return true -> handler class reacted on change event
   */
 virtual bool processMeasurementReset( EventSource_c ac_src, uint16_t rui16_ddi, int32_t ai32_val, const iIsoName_c& ac_callerISOName ) = 0;

 /** react on new received measurement update for remote process data
   * (remote system which manages the process data sent new value on request or
   *  during active measurement programm)
   * @param ac_src general event source class, which provides conversion functions to get needed event source class
   * @param ai32_val new value, which caused the event (for immediate access)
   * @param ac_callerISOName ISOName of calling device - i.e. which sent new setpoint
   * @return true -> handler class reacted on change event
   */
 virtual bool processMeasurementUpdate( IsoAgLib::EventSource_c ac_src, uint16_t rui16_ddi, int32_t ai32_val, const iIsoName_c& ac_callerISOName, bool ab_change ) = 0;

 /** react on received setpoint ACK or NACK upon previous setpoint set for remote process data
   * (remote system which manages the process data, local or other system sent previously a
   *  new setpoint; commanded manager of process data sent the response with ACK/NACK)
   * @param ac_src general event source class, which provides conversion functions to get needed event source class
   * @param ai32_val new value, which caused the event (for immediate access)
   * @param ac_callerISOName ISOName of calling device - i.e. which sent new setpoint
   * @return true -> handler class reacted on change event
   */
 virtual bool processSetpointResponse( EventSource_c ac_src, uint16_t rui16_ddi, int32_t ai32_val, const iIsoName_c& ac_callerISOName ) = 0;

 /** react on received value request for default data logging (DDI 0xDFFF)
   * (can be used to start measurement programms in local process data instances)
   * @param ac_src general event source class, which provides conversion functions to get needed event source class
   * @param ai32_val new value, which caused the event (for immediate access)
   * @param ac_callerISOName ISOName of calling device - i.e. which sent new setpoint
   * @return true -> handler class reacted on change event
   */
 virtual bool processDefaultLoggingStart( EventSource_c ac_src, uint16_t rui16_ddi, int32_t ai32_val, const iIsoName_c& ac_callerISOName ) = 0;

 /** react on received value for TC status message
   * @param ab_taskRunning
   * @param ac_callerISOName ISONameof calling device - i.e. TC
   * @return true -> handler class reacted on change event
   */
 virtual bool processTcStatusMessage(bool ab_taskRunning, const iIsoName_c& ac_callerISOName ) = 0;
};
}
#endif
