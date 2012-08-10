/*
  managemeasureprog_c.cpp

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#include "managemeasureprog_c.h"
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/process_c.h>

namespace __IsoAgLib {

ManageMeasureProg_c::ManageMeasureProg_c( )
: mc_measureprogTC(IsoAgLib::ProcData::remoteTypeTaskControl)
#ifdef USE_DATALOGGER
  ,mc_measureprogLogger(IsoAgLib::ProcData::remoteTypeLogger)
#endif
{
}

void ManageMeasureProg_c::timeEvent( ProcData_c& ac_processData, uint16_t& rui16_nextTimePeriod )
{
  mc_measureprogTC.timeEvent(ac_processData, rui16_nextTimePeriod, measurementValue());
#ifdef USE_DATALOGGER
  mc_measureprogLogger.timeEvent(ac_processData, rui16_nextTimePeriod, measurementValue());
#endif
}

void ManageMeasureProg_c::processMsg( ProcData_c& ac_processData, const ProcessPkg_c& arc_data, IsoAgLib::ProcData::remoteType_t a_ecuType )
{
  isoaglib_assert(arc_data.getMonitorItemForSA() != NULL); // should have been filtered earlier !
  
  // now call process msg
  getMeasureProg(a_ecuType).processMsg( ac_processData, arc_data, measurementValue() );
}

void ManageMeasureProg_c::setVal( ProcData_c& ac_processData, int32_t ai32_val )
{
  mi32_value = ai32_val;
  
  mc_measureprogTC.setVal(ac_processData, ai32_val);
#ifdef USE_DATALOGGER
  mc_measureprogLogger.setVal(ac_processData, ai32_val);
#endif
}

MeasureProg_c& ManageMeasureProg_c::getMeasureProg( IsoAgLib::ProcData::remoteType_t a_ecuType )
{
  switch (a_ecuType)
  {
  case IsoAgLib::ProcData::remoteTypeTaskControl: return mc_measureprogTC;
#ifdef USE_DATALOGGER
  case IsoAgLib::ProcData::remoteTypeLogger: return mc_measureprogLogger;
#endif
  default:
    isoaglib_assert(!"TaskController-Client : invalid ECU type");
    return mc_measureprogTC;
  }
}

void ManageMeasureProg_c::startDataLogging(ProcData_c& ac_processData, IsoAgLib::ProcData::measurementCommand_t ren_type /* IsoAgLib::ProcData::MeasurementCommandTimeProp, IsoAgLib::ProcData::DistProp, ... */,
                                                int32_t ai32_increment, IsoAgLib::ProcData::remoteType_t a_ecuType )
{
  MeasureProg_c& progCache = getMeasureProg(a_ecuType);

  // for MeasurementCommandTimeProp no negative value allowed
  isoaglib_assert( (IsoAgLib::ProcData::MeasurementCommandTimeProp != ren_type) || (ai32_increment > 0) );

  const bool started = progCache.startMeasurement(ac_processData, ren_type, ai32_increment, measurementValue());
  isoaglib_assert(started); (void)started;
}

void ManageMeasureProg_c::stopRunningMeasurement(ProcData_c& ac_processData, IsoAgLib::ProcData::remoteType_t a_ecuType)
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
