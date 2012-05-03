/***************************************************************************
                          procdatalocal_c.cpp  - 

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#include "procdatalocal_c.h"
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/process_c.h>

#if defined(_MSC_VER)
#pragma warning( disable : 4355 )
#endif


namespace __IsoAgLib {

/**
  constructor which can set all element vars

  possible errors:
      * Err_c::badAlloc not enough memory to insert first  MeasureProgLocal
  ISO parameter
  @param ps_elementDDI optional pointer to array of structure IsoAgLib::ElementDdi_s which contains DDI, element, isSetpoint and ValueGroup
                       (array is terminated by ElementDdi_s.ui16_element == 0xFFFF)
  @param aui16_element

  @param acrc_isoName optional ISOName code of Process-Data
  @param apc_externalOverridingIsoName pointer to updated ISOName variable
  @param ab_cumulativeValue
           -# for process data like distance, time, area
               the value of the measure prog data sets is updated
               on master value update dependent on the value increment
               since the last master value update
               -> if a remote member resets his data set copy, datas of
                  other members aren't changed
               -> if this data is saved in EEPROM, the main application
                  needn't take into account the initial EEPROM value, as
                   setting of the master val is independent from EEPROM
           -#  for values like speed, state, rpm aren't updated by increment,
                -> the given master value is propagated equally to all
                    measure prog data sets
                -> if this data is saved in EEPROM, the stored value is loaded
                   as initial master value, and is initially propagated to all
                   measure prog data sets
  @param aui16_eepromAdr optional adress where value is stored in EEPROM
  @param apc_processDataChangeHandler optional pointer to handler class of application
  @param ai_multitonInst optional key for selection of IsoAgLib instance (default 0)
*/
ProcDataLocal_c::ProcDataLocal_c( const IsoAgLib::ElementDdi_s* ps_elementDDI, uint16_t aui16_element,
                                  const IsoName_c& acrc_isoName, const IsoName_c *apc_externalOverridingIsoName,
                                  bool ab_cumulativeValue,
                  IsoAgLib::ProcessDataChangeHandler_c *apc_processDataChangeHandler,
                  int ai_multitonInst
                  )
    : ProcDataLocalBase_c( ps_elementDDI, aui16_element,
                          acrc_isoName, apc_externalOverridingIsoName, ab_cumulativeValue,
                          apc_processDataChangeHandler,
                          ai_multitonInst)
    , mc_measureprog( this )
    , mc_setpoint( this )
{
}

/**
  initialise this ProcDataLocal_c instance to a well defined initial state

  possible errors:
      * Err_c::badAlloc not enough memory to insert first  MeasureProgLocal
  ISO parameter
  @param ps_elementDDI optional pointer to array of structure IsoAgLib::ElementDdi_s which contains DDI, element, isSetpoint and ValueGroup
                       (array is terminated by ElementDdi_s.ui16_element == 0xFFFF)
  @param aui16_element

  @param acrc_isoName optional ISOName code of Process-Data
  @param apc_externalOverridingIsoName pointer to updated ISOName variable
  @param ab_cumulativeValue
          -# for process data like distance, time, area
              the value of the measure prog data sets is updated
              on master value update dependent on the value increment
              since the last master value update
              -> if a remote member resets his data set copy, datas of
                  other members aren't changed
              -> if this data is saved in EEPROM, the main application
                  needn't take into account the initial EEPROM value, as
                  setting of the master val is independent from EEPROM
          -#  for values like speed, state, rpm aren't updated by increment,
                -> the given master value is propagated equally to all
                    measure prog data sets
                -> if this data is saved in EEPROM, the stored value is loaded
                  as initial master value, and is initially propagated to all
                  measure prog data sets
  @param aui16_eepromAdr optional adress where value is stored in EEPROM
  @param apc_processDataChangeHandler optional pointer to handler class of application
  @param ai_multitonInst optional key for selection of IsoAgLib instance (default 0)
*/
void ProcDataLocal_c::init( const IsoAgLib::ElementDdi_s* ps_elementDDI, uint16_t aui16_element,
                            const IsoName_c& acrc_isoName, const IsoName_c *apc_externalOverridingIsoName,
                            bool ab_cumulativeValue,
                           IsoAgLib::ProcessDataChangeHandler_c *apc_processDataChangeHandler,
                           int ai_multitonInst
                           )
{
  ProcDataLocalBase_c::init( ps_elementDDI, aui16_element,
                            acrc_isoName, apc_externalOverridingIsoName, ab_cumulativeValue,
                            apc_processDataChangeHandler,
                            ai_multitonInst );
  mc_setpoint.init( this );
  mc_measureprog.init( this );
}

/** destructor */
ProcDataLocal_c::~ProcDataLocal_c(){
  // now unregister the pointer to this instance in Process_c
  getProcessInstance4Comm().unregisterLocalProcessData( this );
}


/**
  set the masterMeasurementVal from main application independent from any measure progs
  @param ai32_val new measure value
*/
void ProcDataLocal_c::setMasterMeasurementVal(int32_t ai32_val){
  ProcDataLocalBase_c::setMasterMeasurementVal( ai32_val );
  mc_measureprog.setGlobalVal( ai32_val );
}
/**
  increment the value -> update the local and the measuring programs values
  @param ai32_val size of increment of master value
*/
void ProcDataLocal_c::incrMasterMeasurementVal(int32_t ai32_val){
  ProcDataLocalBase_c::incrMasterMeasurementVal( ai32_val );
  // ProcDataLocalBase_c::incrMasterMeasurementVal sets new ProcDataLocalBase_c::mi32_masterVal
  mc_measureprog.setGlobalVal( masterMeasurementVal() );
}

/**
  perform periodic actions
  delete all running measure programs of members which are >3sec inactive
  deletion of outdated setpoints is managed by SetpointLocal_c::timeEvent
  @param pui16_nextTimePeriod calculated new time period, based on current measure progs (only for local proc data)
  @return true -> all planned executions performed
*/
bool ProcDataLocal_c::timeEvent( uint16_t *pui16_nextTimePeriod ){
  if ( Scheduler_Task_c::getAvailableExecTime() == 0 ) return false;

  // perform time event activities for base class
  if ( ! ProcDataLocalBase_c::timeEvent() ) return false;

  if ( ! mc_measureprog.timeEvent(pui16_nextTimePeriod) ) return false;
  if ( ! mc_setpoint.timeEvent() ) return false;
  return true;
}


/** process a setpoint message */
void ProcDataLocal_c::processSetpoint( const ProcessPkg_c& pkg ){
  switch ( pkg.mc_processCmd.getCommand())
  {
    case ProcessCmd_c::setValue:
    case ProcessCmd_c::requestValue:
      mc_setpoint.processMsg( pkg );
      break;
    default:
      // process measurement commands even if this DDI is defined as a setpoint
      mc_measureprog.processProg( pkg );
  }
}

/** process a measure prog message for local process data */
void ProcDataLocal_c::processProg( const ProcessPkg_c& pkg ){
  mc_measureprog.processProg( pkg );
}

/**
  allow local client to actively start a measurement program
  (to react on a incoming "start" command for default data logging)
  @param ren_type measurement type: Proc_c::TimeProp, Proc_c::DistProp, ...
  @param ai32_increment
  @param apc_receiverDevice commanding ISOName
  @return true -> measurement started
*/
bool ProcDataLocal_c::startDataLogging(Proc_c::type_t ren_type /* Proc_c::TimeProp, Proc_c::DistProp, ... */,
                                       int32_t ai32_increment, const IsoName_c* apc_receiverDevice )
{
  if ( !apc_receiverDevice )
    // get isoName of TC from last TC status message
    apc_receiverDevice = getProcessInstance4Comm().getTcISOName();

  // if still no apc_receiverDevice => get it from ISO monitor list
  if ( !apc_receiverDevice )
  {  // get TC dev key (device class 0)
    const IsoItem_c& c_tcISOItem = getIsoMonitorInstance4Comm().isoMemberDevClassInd(0 /* aui8_devClass */, 0 /* aui8_ind */, true /* ab_forceClaimedAddress */);
    apc_receiverDevice = &(c_tcISOItem.isoName());
  }

  return mc_measureprog.startDataLogging(ren_type, ai32_increment, apc_receiverDevice);
}

/**
  stop all measurement progs in all local process instances, started with given isoName
  @param rc_isoName
*/
void ProcDataLocal_c::stopRunningMeasurement(const IsoName_c& rc_isoName)
{
  mc_measureprog.stopRunningMeasurement(rc_isoName);
}


} // end of namespace __IsoAgLib
