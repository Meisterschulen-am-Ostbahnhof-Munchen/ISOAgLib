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

ProcDataLocal_c::ProcDataLocal_c( uint16_t aui16_ddi, uint16_t aui16_element,
                                  const IsoName_c& acrc_isoName, const IsoName_c *apc_externalOverridingIsoName,
                                  bool ab_cumulativeValue,
                                  IsoAgLib::ProcessDataChangeHandler_c *apc_processDataChangeHandler,
                                  int ai_multitonInst
                                  )
    : ProcDataBase_c( aui16_ddi, aui16_element,
                      acrc_isoName, apc_externalOverridingIsoName, apc_processDataChangeHandler, ai_multitonInst
                     )
    , mc_measureprog( this )
    , mc_setpoint( this )
{
    init( aui16_ddi, aui16_element, acrc_isoName, apc_externalOverridingIsoName, ab_cumulativeValue
        , apc_processDataChangeHandler
        , ai_multitonInst);
}

void ProcDataLocal_c::init( uint16_t aui16_ddi, uint16_t aui16_element,
                            const IsoName_c& acrc_isoName, const IsoName_c *apc_externalOverridingIsoName,
                            bool ab_cumulativeValue,
                           IsoAgLib::ProcessDataChangeHandler_c *apc_processDataChangeHandler,
                           int ai_multitonInst
                           )
{
  ProcDataBase_c::init( aui16_ddi, aui16_element,
                        acrc_isoName, apc_externalOverridingIsoName,
                        apc_processDataChangeHandler,
                        ai_multitonInst );
  mc_setpoint.init( this );
  mc_measureprog.init( this );

  mb_cumulativeValue = ab_cumulativeValue;
  mi32_masterVal = 0;

  getProcessInstance4Comm().registerLocalProcessData( this );
}

ProcDataLocal_c::~ProcDataLocal_c(){
  // now unregister the pointer to this instance in Process_c
  getProcessInstance4Comm().unregisterLocalProcessData( this );
}

void ProcDataLocal_c::setMasterMeasurementVal(int32_t ai32_val){
  mi32_masterVal = ai32_val;
  mc_measureprog.setGlobalVal( ai32_val );
}

void ProcDataLocal_c::incrMasterMeasurementVal(int32_t ai32_val){
  mi32_masterVal += ai32_val;
  // ProcDataLocalBase_c::incrMasterMeasurementVal sets new ProcDataLocalBase_c::mi32_masterVal
  mc_measureprog.setGlobalVal( masterMeasurementVal() );
}

bool ProcDataLocal_c::timeEvent( uint16_t *pui16_nextTimePeriod ){
  if ( Scheduler_Task_c::getAvailableExecTime() == 0 ) return false;

  // perform time event activities for base class
  //if ( ! ProcDataLocalBase_c::timeEvent() ) return false;

  if ( ! mc_measureprog.timeEvent(pui16_nextTimePeriod) ) return false;
  if ( ! mc_setpoint.timeEvent() ) return false;
  return true;
}

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

//void ProcDataLocal_c::processProg( const ProcessPkg_c& pkg ){
//  mc_measureprog.processProg( pkg );
//}

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

void ProcDataLocal_c::stopRunningMeasurement(const IsoName_c& rc_isoName)
{
  mc_measureprog.stopRunningMeasurement(rc_isoName);
}


ProcDataLocal_c::ProcDataLocal_c( const ProcDataLocal_c& acrc_src )
  : ProcDataBase_c( acrc_src )
{
  assignFromSource( acrc_src );

  // now register the pointer to this instance in Process_c
  getProcessInstance4Comm().registerLocalProcessData( this );
}

const ProcDataLocal_c& ProcDataLocal_c::operator=( const ProcDataLocal_c& acrc_src )
{
  ProcDataBase_c::operator=( acrc_src );
  assignFromSource( acrc_src );
  return *this;
}

void ProcDataLocal_c::assignFromSource( const ProcDataLocal_c& acrc_src )
{
  mi32_masterVal = acrc_src.mi32_masterVal;
  mb_cumulativeValue = acrc_src.mb_cumulativeValue;
}

//ProcDataLocal_c::~ProcDataLocal_c() {
//  // call unregisterLocalProcessData in last derived class because unregister does again message processing!
//}

//void ProcDataLocal_c::setMasterMeasurementVal(int32_t ai32_val)
//{
//  mi32_masterVal = ai32_val;
//}
//
//void ProcDataLocal_c::incrMasterMeasurementVal(int32_t ai32_val)
//{
//  mi32_masterVal += ai32_val;
//}

//bool ProcDataLocal_c::timeEvent( uint16_t* /* pui16_nextTimePeriod */)
//{
//  return true;
//}

bool ProcDataLocal_c::sendMasterMeasurementVal( const IsoName_c& ac_targetISOName) const {

  ProcessPkg_c pkg;
  // prepare general command in process pkg
  pkg.mc_processCmd.setValues(false /* isSetpoint */, false, /* isRequest */
                                                           ProcessCmd_c::exactValue,
                                                           ProcessCmd_c::setValue);

  return sendValISOName( pkg, ac_targetISOName, masterMeasurementVal());
}

void ProcDataLocal_c::processProg( const ProcessPkg_c& pkg )
{
  if (pkg.senderItem() == NULL)
  { // sender with SA 0xFE is not of interest
    return;
  }

  // handle for simple measurement value
  if (pkg.mc_processCmd.checkIsRequest() &&
      // pkg.mc_processCmd.checkIsMeasure() &&  /* already checked before, we are in processProg() ! */
      pkg.mc_processCmd.getValueGroup() == ProcessCmd_c::exactValue)
  { // request for measurement value
    sendMasterMeasurementVal( pkg.senderItem()->isoName() );
  }
}

//void ProcDataLocal_c::processSetpoint( const ProcessPkg_c& pkg )
//{
//  #ifdef RESET_MEASUREMENT_WITH_ZERO_EXACT_SETPOINT
//  if (pkg.dataRawCmdLong() == 0)
//  { // let processProg process measurement value reset -> change PD to use std. reset
//    pkg.setPd(1);
//    processProg(pkg);
//  }
//  #else
//  (void)pkg;
//  #endif
//}

bool ProcDataLocal_c::sendValISOName( ProcessPkg_c& arc_pkg, const IsoName_c& ac_varISOName, int32_t ai32_val) const
{
  arc_pkg.setISONameForDA(ac_varISOName);
  arc_pkg.setISONameForSA(isoName());

  return ProcDataBase_c::sendValISOName (arc_pkg, ac_varISOName, ai32_val);
}



} // end of namespace __IsoAgLib
