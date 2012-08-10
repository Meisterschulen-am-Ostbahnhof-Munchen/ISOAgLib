/*
  managemeasureproglocal_c.cpp

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#include "managemeasureproglocal_c.h"
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/process_c.h>


namespace __IsoAgLib {

ManageMeasureProgLocal_c::ManageMeasureProgLocal_c( )
: ClientBase(),
  mc_measureprogTC(Proc_c::remoteTypeTaskControl)
#ifdef USE_DATALOGGER
  ,mc_measureprogLogger(Proc_c::remoteTypeLogger)
#endif
{
}

void ManageMeasureProgLocal_c::timeEvent( ProcDataLocal_c& ac_processData, uint16_t& rui16_nextTimePeriod )
{
  mc_measureprogTC.timeEvent(ac_processData, rui16_nextTimePeriod);
#ifdef USE_DATALOGGER
  mc_measureprogLogger.timeEvent(ac_processData, rui16_nextTimePeriod);
#endif
}

void ManageMeasureProgLocal_c::processMsg( ProcDataLocal_c& ac_processData, const ProcessPkg_c& arc_data, Proc_c::remoteType_t a_ecuType )
{
  isoaglib_assert(arc_data.getMonitorItemForSA() != NULL); // should have been filtered earlier !
  
  // now call process msg
  getMeasureProgLocal(a_ecuType).processMsg( ac_processData, arc_data );
}

void ManageMeasureProgLocal_c::setVal( ProcDataLocal_c& ac_processData, int32_t ai32_val )
{
  mc_measureprogTC.setVal(ac_processData, ai32_val);
#ifdef USE_DATALOGGER
  mc_measureprogLogger.setVal(ac_processData, ai32_val);
#endif
}

MeasureProgLocal_c& ManageMeasureProgLocal_c::getMeasureProgLocal( Proc_c::remoteType_t a_ecuType )
{
  switch (a_ecuType)
  {
  case Proc_c::remoteTypeTaskControl: return mc_measureprogTC;
#ifdef USE_DATALOGGER
  case Proc_c::remoteTypeLogger: return mc_measureprogLogger;
#endif
  default:
    isoaglib_assert(false); // should not happened
    return mc_measureprogTC;
  }
}

void ManageMeasureProgLocal_c::startDataLogging(ProcDataLocal_c& ac_processData, Proc_c::measurementCommand_t ren_type /* Proc_c::MeasurementCommandTimeProp, Proc_c::DistProp, ... */,
                                                int32_t ai32_increment, const IsoName_c& ac_remoteDevice )
{
  const Proc_c::remoteType_t ecuType = getProcessInstance4Comm().getTypeFromISOName( ac_remoteDevice );
  MeasureProgLocal_c& progCache = getMeasureProgLocal(ecuType);

  // for MeasurementCommandTimeProp no negative value allowed
  isoaglib_assert( (Proc_c::MeasurementCommandTimeProp != ren_type) || (ai32_increment > 0) );

  const bool started = progCache.startMeasurement(ac_processData, ren_type, ai32_increment);
  isoaglib_assert(started); (void)started;
}

void ManageMeasureProgLocal_c::stopRunningMeasurement(ProcDataLocal_c& ac_processData, Proc_c::remoteType_t a_ecuType)
{
  if ((mc_measureprogTC.isoNameType() == a_ecuType))
  {
    mc_measureprogTC.stopMeasurement(ac_processData);
  }
#ifdef USE_DATALOGGER
  else if ((mc_measureprogLogger.isoNameType() == a_ecuType))
  {
    mc_measureprogLogger.stopMeasurement(ac_processData);
  }
#endif
}

} // end of namespace __IsoAgLib
