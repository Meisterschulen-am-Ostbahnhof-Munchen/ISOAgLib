/*
  managemeasureprog_c.h

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef MANAGEMEASUREPROG_C_H
#define MANAGEMEASUREPROG_C_H

#include <IsoAgLib/isoaglib_config.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/processpkg_c.h>
#include "measureprog_c.h"

#include <list>


namespace __IsoAgLib {

class ManageMeasureProg_c
{
 public:
  ManageMeasureProg_c();
  virtual ~ManageMeasureProg_c() {}

  void timeEvent( ProcData_c& ac_processData, uint16_t& rui16_nextTimePeriod );
  void processMsg( ProcData_c& ac_processData, const ProcessPkg_c& arc_data, IsoAgLib::ProcData::remoteType_t a_ecuType );

  void setVal( ProcData_c& ac_processData, int32_t ai32_val );

  void startDataLogging(
    ProcData_c& ac_processData,
	IsoAgLib::ProcData::measurementCommand_t ren_type /* IsoAgLib::ProcData::TimeProp, IsoAgLib::ProcData::DistProp, ... */,
    int32_t ai32_increment,
	IsoAgLib::ProcData::remoteType_t a_ecuType );

  void stopRunningMeasurement( IsoAgLib::ProcData::remoteType_t a_ecuType );

  int32_t measurementValue() const { return mi32_value; }

private:
  MeasureProg_c& getMeasureProg( IsoAgLib::ProcData::remoteType_t a_ecuType );

private:
  MeasureProg_c mc_measureprogTC;
#ifdef USE_DATALOGGER
  MeasureProg_c mc_measureprogLogger;
#endif

  int32_t mi32_value;

private:
  /** not copyable : copy constructor/operator only declared, not defined */
  ManageMeasureProg_c( const ManageMeasureProg_c& );
  ManageMeasureProg_c& operator=( const ManageMeasureProg_c& );
};

}

#endif
