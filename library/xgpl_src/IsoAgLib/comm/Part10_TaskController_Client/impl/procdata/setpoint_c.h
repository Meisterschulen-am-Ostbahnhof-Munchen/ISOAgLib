/*
  setpointlocal_c.h

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef SETPOINT_LOCAL_H
#define SETPOINT_LOCAL_H

#include <IsoAgLib/isoaglib_config.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/processpkg_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/iprocdata.h>

namespace __IsoAgLib {

class ProcData_c;

class Setpoint_c {
public:
  Setpoint_c() : mi32_value(0) {}
  ~Setpoint_c() {}

  int32_t setpointValue() const { return mi32_value; }
  
  void processMsg( ProcData_c& ac_processData, const ProcessPkg_c& pkg, IsoAgLib::ProcData::remoteType_t a_ecuType );

private:
  int32_t mi32_value;

private:
  /** not copyable : copy constructor is only declared, never defined */
  Setpoint_c(const Setpoint_c&);
  /** not copyable : copy operator is only declared, never defined */
  Setpoint_c& operator=(const Setpoint_c&); 
};

}
#endif
