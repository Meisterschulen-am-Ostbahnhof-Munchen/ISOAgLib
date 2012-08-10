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
#include <IsoAgLib/comm/impl/isobus_c.h>

#if defined(_MSC_VER)
#pragma warning( disable : 4355 )
#endif


namespace __IsoAgLib {

ProcDataLocal_c::ProcDataLocal_c( uint16_t aui16_ddi, uint16_t aui16_element,
                                  const IsoName_c& acrc_isoName,
                                  bool ab_isSetpoint,
                                  uint8_t aui8_triggerMethod,
                                  bool ab_cumulativeValue,
                                  IsoAgLib::ProcessDataChangeHandler_c *apc_processDataChangeHandler,
                                  int ai_multitonInst
                                  )
    : ClientBase( ai_multitonInst ),
      mc_isoName(IsoName_c::IsoNameUnspecified())
    , mc_measureprog( *this )
    , mc_setpoint( *this )
{
      init( aui16_ddi, aui16_element, acrc_isoName, ab_isSetpoint, aui8_triggerMethod, ab_cumulativeValue
          , apc_processDataChangeHandler
          , ai_multitonInst);
}

void ProcDataLocal_c::init( uint16_t aui16_ddi, uint16_t aui16_element,
                            const IsoName_c& acrc_isoName,
                            bool ab_isSetpoint,
                            uint8_t aui8_triggerMethod,
                            bool ab_cumulativeValue,
                            IsoAgLib::ProcessDataChangeHandler_c *apc_processDataChangeHandler,
                            int ai_multitonInst
                           )
{
  setElementDDI(aui16_ddi);
  setElementNumber(aui16_element);

  mc_isoName = acrc_isoName;

  setMultitonInst(ai_multitonInst);
  mpc_processDataChangeHandler = apc_processDataChangeHandler;

  mb_cumulativeValue = ab_cumulativeValue;
  mi32_masterVal = 0;

  getProcessInstance4Comm().registerLocalProcessData( this );

  mc_setpoint.init( *this );
  mc_measureprog.init( *this );

  mb_isSetpoint = ab_isSetpoint;
  mui8_triggerMethod = aui8_triggerMethod;
}

ProcDataLocal_c::~ProcDataLocal_c(){
  // now unregister the pointer to this instance in Process_c
  getProcessInstance4Comm().unregisterLocalProcessData( this );
}

#if 0 // no copy constructor and assign operator for now
const ProcDataLocal_c& ProcDataLocal_c::operator=(const ProcDataLocal_c& acrc_src)
{ // call base class operator
  ClientBase::operator=(acrc_src);
  // now assign flags of this class
  assignFromSource(acrc_src);

  // NOT COMPLETE ?
  // return source reference
  return *this;
}

void ProcDataLocal_c::assignFromSource( const ProcDataLocal_c& acrc_src )
{ // copy element vars
  // NOT COMPLETE ?
  mc_isoName = acrc_src.mc_isoName;
  // elementDDI() returns list reference, setElementDDI() expects pointer to list
  setElementDDI(acrc_src.DDI());

  mpc_processDataChangeHandler = acrc_src.mpc_processDataChangeHandler;

  mi32_masterVal = acrc_src.mi32_masterVal;
  mb_cumulativeValue = acrc_src.mb_cumulativeValue;

  mb_isSetpoint = acrc_src.mb_cumulativeValue;
  mui8_triggerMethod = acrc_src.mui8_triggerMethod;
}

ProcDataLocal_c::ProcDataLocal_c(const ProcDataLocal_c& acrc_src)
   : ClientBase( acrc_src ),
		 mc_isoName( acrc_src.mc_isoName )
     mc_measureprog( this ),
     mc_setpoint( this )
{
  // NOT COMPLETE ?
  assignFromSource(acrc_src);

  getProcessInstance4Comm().registerLocalProcessData( this );
}
#endif

void ProcDataLocal_c::processMsg( ProcessPkg_c& pkg )
{
  isoaglib_assert( DDI() == pkg.DDI() );

  pkg.resolveCommandTypeForISO( DDI(), mb_isSetpoint );

  if (pkg.mc_processCmd.checkIsSetpoint())
    processSetpoint(pkg);
  else
    processProg(pkg);
}


void ProcDataLocal_c::setMasterMeasurementVal(int32_t ai32_val){
  mi32_masterVal = ai32_val;
  mc_measureprog.setGlobalVal( ai32_val );
}

void ProcDataLocal_c::incrMasterMeasurementVal(int32_t ai32_val){
  mi32_masterVal += ai32_val;
  mc_measureprog.setGlobalVal( masterMeasurementVal() );
}

bool ProcDataLocal_c::timeEvent( uint16_t *pui16_nextTimePeriod ){
  if ( Scheduler_Task_c::getAvailableExecTime() == 0 ) return false;

  if ( ! mc_measureprog.timeEvent(pui16_nextTimePeriod) ) return false;
  return true;
}

void ProcDataLocal_c::processSetpoint( const ProcessPkg_c& pkg ){
  switch ( pkg.mc_processCmd.getCommand())
  {
    case ProcessCmd_c::setValue:
      mc_setpoint.processMsg( pkg );
      break;
    default:
      // process measurement commands even if this DDI is defined as a setpoint
      mc_measureprog.processProg( pkg );
  }
}

void ProcDataLocal_c::processProg( const ProcessPkg_c& pkg ){
  mc_measureprog.processProg( pkg );
}
#if 0
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

void ProcDataLocal_c::processSetpoint( const ProcessPkg_c& pkg )
{
  #ifdef RESET_MEASUREMENT_WITH_ZERO_EXACT_SETPOINT
  if (pkg.dataRawCmdLong() == 0)
  { // let processProg process measurement value reset -> change PD to use std. reset
    pkg.setPd(1);
    processProg(pkg);
  }
  #else
  (void)pkg;
  #endif
}
#endif
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

bool ProcDataLocal_c::sendValISOName( ProcessPkg_c& pkg, const IsoName_c& ac_varISOName, int32_t ai32_val) const
{
  pkg.setISONameForDA(ac_varISOName);
  pkg.setISONameForSA(isoName());

  setBasicSendFlags( pkg );

  pkg.setData( ai32_val );

  // send the msg
  pkg.flags2String();
  getIsoBusInstance4Comm() << pkg;
  // check for any error during send resolve, ...
  if ( getILibErrInstance().good(IsoAgLib::iLibErr_c::CanBus, IsoAgLib::iLibErr_c::Can) )
  { // good
    return true;
  }
  else
    return false;
}

void ProcDataLocal_c::setBasicSendFlags( ProcessPkg_c& pkg ) const
{
  // the communicating devices are represented on ISO11783
  pkg.setIsoPri(3);
  pkg.setIsoPgn(PROCESS_DATA_PGN);

  pkg.set_Element(element());
  pkg.set_DDI(DDI());
}

bool ProcDataLocal_c::matchISO( const IsoName_c& acrc_isoNameReceiver,
                                uint16_t aui16_DDI,
                                uint16_t aui16_element
                                ) const
{
  // check wether current element/DDI combination matches one list element in process data element/DDI list
  if (aui16_element != element()) return false;
  if (aui16_DDI != DDI()) return false;
  if (isoName() != acrc_isoNameReceiver) return false;

  // all previous tests are positive -> answer positive match
  return true;
}

bool ProcDataLocal_c::hasDDI( uint16_t aui16_checkDDI ) const
{
  return ( aui16_checkDDI == DDI() );
}

bool ProcDataLocal_c::sendMasterMeasurementVal( const IsoName_c& ac_targetISOName) const {

  ProcessPkg_c pkg;
  // prepare general command in process pkg
  pkg.mc_processCmd.setValues(false /* isSetpoint */, false /* isRequest */, ProcessCmd_c::setValue);

  return sendValISOName( pkg, ac_targetISOName, masterMeasurementVal());
}

} // end of namespace __IsoAgLib
