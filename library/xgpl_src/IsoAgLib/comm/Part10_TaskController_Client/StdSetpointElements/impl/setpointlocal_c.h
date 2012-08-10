/*
  setpointlocal_c.h - object for managing setpoints

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef SETPOINT_LOCAL_H
#define SETPOINT_LOCAL_H

#include <IsoAgLib/hal/hal_typedef.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/processpkg_c.h>

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {
class ProcDataLocal_c;

/**
  object for managing setpoints commanded from remote to local process data objects;
  handling of received setpoints;
*/
class SetpointLocal_c : public ClientBase  {
public:
  SetpointLocal_c( ) : ClientBase(), mi32_value(NO_VAL_32S) {}
  ~SetpointLocal_c() {}

  /**
    retreive setpoint
    @return actual received setpoint value
  */
  int32_t setpointValue() const { return mi32_value; }
  
  /**  process a setpoint message */
  void processMsg( ProcDataLocal_c& ac_processData, const ProcessPkg_c& pkg );

private: // Private attributes
  /** setpoint value */
  int32_t mi32_value;

private:
  /** not copyable : copy constructor is only declared, never defined */
  SetpointLocal_c(const SetpointLocal_c&);
  /** not copyable : copy operator is only declared, never defined */
  SetpointLocal_c& operator=(const SetpointLocal_c&); 
};

}
#endif
