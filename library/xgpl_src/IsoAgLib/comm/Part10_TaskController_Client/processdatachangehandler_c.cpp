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


bool
ProcessDataChangeHandler_c::processSetpointSet( EventSource_c eventSource, uint16_t ddi, int32_t value, const iIsoName_c& caller, bool change ) {
  // empty body -> allow application class to just implement some of the possible handler methods
  (void)eventSource;
  (void)ddi;
  (void)value;
  (void)caller;
  (void)change;
  return false;
}


bool
ProcessDataChangeHandler_c::processMeasurementReset( EventSource_c eventSource, uint16_t ddi, int32_t value, const iIsoName_c& caller ) {
  // empty body -> allow application class to just implement some of the possible handler methods
  (void)eventSource;
  (void)ddi;
  (void)value;
  (void)caller;
  return false;
}


bool
ProcessDataChangeHandler_c::processMeasurementUpdate( EventSource_c eventSource, uint16_t ddi, int32_t value, const iIsoName_c& caller, bool change ) {
  // empty body -> allow application class to just implement some of the possible handler methods
  (void)eventSource;
  (void)ddi;
  (void)value;
  (void)caller;
  (void)change;
  return false;
}


bool
ProcessDataChangeHandler_c::processSetpointResponse( EventSource_c eventSource, uint16_t ddi, int32_t value, const iIsoName_c& caller ) {
  // empty body -> allow application class to just implement some of the possible handler methods
  (void)eventSource;
  (void)ddi;
  (void)value;
  (void)caller;
  return false;
}


bool
ProcessDataChangeHandler_c::processDefaultLoggingStart( EventSource_c eventSource, uint16_t ddi, int32_t value, const iIsoName_c& caller ) {
  // empty body -> allow application class to just implement some of the possible handler methods
  (void)eventSource;
  (void)ddi;
  (void)value;
  (void)caller;
  return false;
}


bool
ProcessDataChangeHandler_c::processTcStatusMessage(bool taskRunning, const iIsoName_c& caller )
{
  // empty body -> allow application class to just implement some of the possible handler methods
  (void)taskRunning;
  (void)caller;
  return false;
}

} // end of namespace IsoAgLib
