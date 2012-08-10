/*
  iprocessdatahandler_c.h: base class for application handler
    classes, with handler methods which are called after each change
    access on process data from external systems

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef IPROCESSDATAHANDLER_H
#define IPROCESSDATAHANDLER_H

#include <IsoAgLib/comm/Part5_NetworkManagement/iisoname_c.h>

namespace IsoAgLib {

class iProcDataLocal_c;

class iProcessDataHandler_c {
public:
  virtual ~iProcessDataHandler_c() {}

  virtual void processSetpointSet( iProcDataLocal_c& procdata, int32_t value, const iIsoName_c& callerISOName, bool change ) = 0;
  virtual void processDefaultLoggingStart( const iIsoName_c& callerISOName ) = 0;
  virtual void processTcStatusMessage( bool taskRunning, const iIsoName_c& callerISOName ) = 0;
};

}

#endif
