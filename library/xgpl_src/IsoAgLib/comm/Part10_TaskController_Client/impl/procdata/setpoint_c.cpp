/*
  setpoint_c.cpp

  (C) Copyright 2009 - 2013 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#include "setpoint_c.h"
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/tcclient_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/iprocdata_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/iprocdatahandler_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/itcclient_c.h>


namespace __IsoAgLib {

void
Setpoint_c::processMsg( ProcData_c& ac_processData, const ProcessPkg_c& pkg, IsoAgLib::ProcData::RemoteType_t a_ecuType )
{
  isoaglib_assert(ProcessPkg_c::setValue == pkg.men_command);
  isoaglib_assert(pkg.getISONameForSA().isSpecified()); // already tested before in TcClient_c::processMsg
 
  const bool b_change = ( mi32_value != pkg.mi32_pdValue );
  mi32_value = pkg.mi32_pdValue;

  // call handler function if handler class is registered
  if ( ac_processData.getProcDataHandler() != NULL )
    ac_processData.getProcDataHandler()->processSetpointSet(
	  static_cast<IsoAgLib::iProcData_c&>(ac_processData),
      pkg.mi32_pdValue, a_ecuType, b_change );
}

}
