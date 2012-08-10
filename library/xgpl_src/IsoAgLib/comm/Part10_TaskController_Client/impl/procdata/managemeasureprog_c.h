/*
  managemeasureproglocal_c.h

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef MANAGE_MEASUREPROG_LOCAL_H
#define MANAGE_MEASUREPROG_LOCAL_H

#include <IsoAgLib/hal/hal_typedef.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/processpkg_c.h>
#include "measureproglocal_c.h"

#include <list>

// Begin Namespace IsoAgLib
namespace __IsoAgLib {
/** 
  management of a measurement valuem with possibility of measurement programs
  */
class ManageMeasureProgLocal_c : public ClientBase
{
 public:
  /** constructor */
  ManageMeasureProgLocal_c();
  
  /** destructor */
  virtual ~ManageMeasureProgLocal_c() {}

  void timeEvent( ProcDataLocal_c& ac_processData, uint16_t& rui16_nextTimePeriod );

  /** process a measure prog message for local process data */
  void processMsg( ProcDataLocal_c& ac_processData, const ProcessPkg_c& arc_data, Proc_c::remoteType_t a_ecuType );

  /** set value for all registered Measure Progs */
  void setVal( ProcDataLocal_c& ac_processData, int32_t ai32_val );

  void startDataLogging(ProcDataLocal_c& ac_processData, Proc_c::measurementCommand_t ren_type /* Proc_c::TimeProp, Proc_c::DistProp, ... */,
                        int32_t ai32_increment, const IsoName_c& ac_receiverDevice );
  /**
    stop all measurement progs in all local process instances, started with given isoName
    @param a_ecuType
  */
  void stopRunningMeasurement(ProcDataLocal_c& ac_processData, Proc_c::remoteType_t a_ecuType);

private:
  MeasureProgLocal_c& getMeasureProgLocal( Proc_c::remoteType_t a_ecuType );

private:
  /** container of objects for managing jobs of local measure programs */
  MeasureProgLocal_c mc_measureprogTC;
#ifdef USE_DATALOGGER
  MeasureProgLocal_c mc_measureprogLogger;
#endif

private:
  /** not copyable : copy constructor is only declared, never defined */
  ManageMeasureProgLocal_c(const ManageMeasureProgLocal_c&);
  /** not copyable : copy operator is only declared, never defined */
  ManageMeasureProgLocal_c& operator=(const ManageMeasureProgLocal_c&); 
};


}
#endif
