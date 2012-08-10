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
#include <IsoAgLib/util/impl/singleton.h>
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

  /**
    perform periodic actions
    delete all running measure programs of members which are >3sec inactive;
    deletion of outdated setpoints is managed by SetpointLocal_c::timeEvent
    @param pui16_nextTimePeriod calculated new time period, based on current measure progs (only for local proc data)
    @return true -> all planned executions performed
  */
  void timeEvent( ProcDataLocal_c& ac_processData, uint16_t& rui16_nextTimePeriod );

  /** process a measure prog message for local process data */
  void processMsg( ProcDataLocal_c& ac_processData, const ProcessPkg_c& arc_data );

  /** set value for all registered Measure Progs */
  void setGlobalVal( ProcDataLocal_c& ac_processData, int32_t ai32_val );

  /**
    allow local client to actively start a measurement program
    (to react on a incoming "start" command for default data logging)
    @param ren_type measurement type: Proc_c::TimeProp, Proc_c::DistProp, ...
    @param ai32_increment
    @param apc_receiverDevice commanding ISOName
    @return true -> apc_receiverDevice is set
  */
  bool startDataLogging(ProcDataLocal_c& ac_processData, Proc_c::type_t ren_type /* Proc_c::TimeProp, Proc_c::DistProp, ... */,
                        int32_t ai32_increment, const IsoName_c& ac_receiverDevice );
  /**
    stop all measurement progs in all local process instances, started with given isoName
    @param rc_isoName
  */
  void stopRunningMeasurement(ProcDataLocal_c& ac_processData, const IsoName_c& rc_isoName);

 protected:
  friend class ProcDataLocal_c;
  /**
    create a new measure prog item;
    if there is still the default initial item undefined define it
    and create no new item
    @param acrc_isoName commanding ISOName
  */
  void setupMeasureprog(const IsoName_c& acrc_isoName);
  /**
    update the programm cache, create an programm item, if wanted
    @param acrc_isoName commanding ISOName
    @param ab_createIfNotFound true -> create new item if not found
    @return true -> instance found
  */
  bool updateProgCache(const IsoName_c& acrc_isoName, bool ab_createIfNotFound);
 protected:
  /** container of objects for managing jobs of local measure programs */
  MeasureProgLocal_c mc_measureprogTC;
#ifdef USE_DATALOGGER
  MeasureProgLocal_c mc_measureprogLogger;
#endif
  /** cache iterator to measure prog */
  MeasureProgLocal_c* mpc_progCache;
private:
  /** create first default measure prog, if no measure prog in list */
  void checkInitList( void );

private:
  /** not copyable : copy constructor is only declared, never defined */
  ManageMeasureProgLocal_c(const ManageMeasureProgLocal_c&);
  /** not copyable : copy operator is only declared, never defined */
  ManageMeasureProgLocal_c& operator=(const ManageMeasureProgLocal_c&); 
};


}
#endif
