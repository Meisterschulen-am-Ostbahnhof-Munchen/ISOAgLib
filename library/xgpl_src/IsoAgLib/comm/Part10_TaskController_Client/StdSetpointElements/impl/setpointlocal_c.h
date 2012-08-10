/***************************************************************************
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
#include <IsoAgLib/util/impl/singleton.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/processpkg_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/StdSetpointElements/impl/setpointregister_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/proc_c.h>

#include <list>

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {
class ProcDataLocal_c;
class SetpointRegister_c;

/**
  object for managing setpoints commanded from remote to local process data objects;
  handling of received setpoints;
  managing the master setpoint, which is used for controlling;
  help for sending correct notify;
  semi automatic and manual handling of received values
  @author Dipl.-Inform. Achim Spangler
*/
class SetpointLocal_c : public ClientBase  {
public:
  /**
    default constructor which can set needed pointers to containing objects
    @param apc_processData pointer to containing ProcessData instance
  */
  SetpointLocal_c( ) : ClientBase()
    {init();}

  /**
    initialise this SetpointLocal_c to a well defined starting condition
    @param apc_processData pointer to containing ProcessData instance
  */
  void init();

  /** default destructor which has nothing to do */
   ~SetpointLocal_c() {}

  /**
    retreive simple master setpoint
    @return actual received setpoint value (calculated with setpoint )
  */
  int32_t setpointVal() const {return mpc_master->value();}
  
  /**  process a setpoint message */
  void processMsg( ProcDataLocal_c& ac_processData, const ProcessPkg_c& pkg );

private: // Private methods
  /**
    process a setpoint set for local process data
  */
  virtual void processSet( ProcDataLocal_c& ac_processData, const ProcessPkg_c& pkg );

private: // Private attributes
  /** container of registered setpoint values */
  SetpointRegister_c m_tcRegister;
  /** SetpointRegister_c entry, which represents the actual used setpoint value */
  SetpointRegister_c* mpc_master;

private:
  /** not copyable : copy constructor is only declared, never defined */
  SetpointLocal_c(const SetpointLocal_c&);
  /** not copyable : copy operator is only declared, never defined */
  SetpointLocal_c& operator=(const SetpointLocal_c&); 
};

}
#endif
