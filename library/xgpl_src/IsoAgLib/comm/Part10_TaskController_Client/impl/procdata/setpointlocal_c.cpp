/*
  setpointlocal_c.cpp - object for managing setpoints

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#include "setpointlocal_c.h"
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/process_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/procdatalocal_c.h>

namespace __IsoAgLib {

void SetpointLocal_c::processMsg( ProcDataLocal_c& ac_processData, const ProcessPkg_c& pkg )
{
  // check if its a request for actual setpoint
  isoaglib_assert(ProcessPkg_c::setValue == pkg.men_command);
  isoaglib_assert(pkg.getMonitorItemForSA() != NULL); // already tested before in Process_c::processMsg
 
  const IsoName_c& c_callerISOName = pkg.getMonitorItemForSA()->isoName();

  const int32_t i32_val = pkg.mi32_pdValue;
  const uint32_t cui32_ddi = pkg.mui16_DDI;

  // detect if something was changed
  const bool b_change = ( mi32_value != i32_val );

  // set the new setpoint value
  if (i32_val != NO_VAL_32S )
    mi32_value = i32_val;

#ifdef DEBUG_PROCESS_DATA
  std::cout << "SetpointLocal_c::processMsg"
            << " DDI=" << (uint16_t)ac_processData.DDI()
            << " element=" << (uint16_t)ac_processData.element()
            << " setpoint=" << mi32_value
            << std::endl;
#endif

  // call handler function if handler class is registered
  if ( ac_processData.getProcessDataChangeHandler() != NULL )
    ac_processData.getProcessDataChangeHandler()->processSetpointSet( &ac_processData, i32_val, c_callerISOName.toConstIisoName_c(), b_change );
}

} // end of namespace __IsoAgLib
