/*
  setpoint_c.cpp

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#include "setpoint_c.h"
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/process_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/procdata/procdata_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/iprocdata_c.h>

namespace __IsoAgLib {

void Setpoint_c::processMsg( ProcData_c& ac_processData, const ProcessPkg_c& pkg )
{
  isoaglib_assert(ProcessPkg_c::setValue == pkg.men_command);
  isoaglib_assert(pkg.getISONameForSA().isSpecified()); // already tested before in Process_c::processMsg
 
  const bool b_change = ( mi32_value != pkg.mi32_pdValue );
  mi32_value = pkg.mi32_pdValue;

  // call handler function if handler class is registered
  if ( ac_processData.getProcDataHandler() != NULL )
    ac_processData.getProcDataHandler()->processSetpointSet( static_cast<IsoAgLib::iProcData_c&>(ac_processData),
                                                             pkg.mi32_pdValue,
                                                             pkg.getISONameForSA().toConstIisoName_c(),
                                                             b_change );
}

}
