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
                                  const IsoName_c& acrc_isoName, const IsoName_c *apc_externalOverridingIsoName,
                                  bool ab_cumulativeValue,
                                  IsoAgLib::ProcessDataChangeHandler_c *apc_processDataChangeHandler,
                                  int ai_multitonInst
                                  )
    : ClientBase( ai_multitonInst ),
      mpc_externalOverridingIsoName(NULL),
		  mc_isoName(IsoName_c::IsoNameUnspecified())
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
  //ProcDataBase_c::init( aui16_ddi, aui16_element,
  //                      acrc_isoName, apc_externalOverridingIsoName,
  //                      apc_processDataChangeHandler,
  //                      ai_multitonInst );

  setElementDDI(aui16_ddi);
  setElementNumber(aui16_element);

  mpc_externalOverridingIsoName = apc_externalOverridingIsoName;

  // the ISOName of ident is best defined by pointed value of apc_externalOverridingIsoName
  if ( apc_externalOverridingIsoName != 0 ) mc_isoName = *apc_externalOverridingIsoName;
  // last choice is definition of mc_isoName by process data identiy
  else mc_isoName = acrc_isoName;

  setMultitonInst(ai_multitonInst);
  mpc_processDataChangeHandler = apc_processDataChangeHandler;

  mc_setpoint.init( this );
  mc_measureprog.init( this );

  mb_cumulativeValue = ab_cumulativeValue;
  mi32_masterVal = 0;

  getProcessInstance4Comm().registerLocalProcessData( this );
}

ProcDataLocal_c::~ProcDataLocal_c(){
  getProcessInstance4Comm().unregisterLocalProcessData( this );
}

const ProcDataLocal_c& ProcDataLocal_c::operator=(const ProcDataLocal_c& acrc_src)
{ // call base class operator
  //ProcIdent_c::operator=(acrc_src);
  ClientBase::operator=(acrc_src);
  assignFromSource(acrc_src);

  // NOT COMPLETE ?
  // return source reference
  return *this;
}

ProcDataLocal_c::ProcDataLocal_c(const ProcDataLocal_c& acrc_src)
   //: ProcIdent_c(acrc_src)
   : ClientBase( acrc_src ),
	  mpc_externalOverridingIsoName( acrc_src.mpc_externalOverridingIsoName ),
		mc_isoName( acrc_src.mc_isoName )
{
  // NOT COMPLETE ?
  assignFromSource(acrc_src);

  getProcessInstance4Comm().registerLocalProcessData( this );
}

void ProcDataLocal_c::assignFromSource( const ProcDataLocal_c& acrc_src )
{ // copy element vars
  mpc_processDataChangeHandler = acrc_src.mpc_processDataChangeHandler;
  // NOT COMPLETE ?
  mc_isoName = acrc_src.mc_isoName;
  mpc_externalOverridingIsoName = acrc_src.mpc_externalOverridingIsoName;
  // elementDDI() returns list reference, setElementDDI() expects pointer to list
  setElementDDI(acrc_src.DDI());

  mi32_masterVal = acrc_src.mi32_masterVal;
  mb_cumulativeValue = acrc_src.mb_cumulativeValue;
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


//ProcDataLocal_c::ProcDataLocal_c( const ProcDataLocal_c& acrc_src )
//  //: ClientBase( acrc_src )
//{
//  //assignFromSource( acrc_src );
//
//  // now register the pointer to this instance in Process_c
//  getProcessInstance4Comm().registerLocalProcessData( this );
//}

//const ProcDataLocal_c& ProcDataLocal_c::operator=( const ProcDataLocal_c& acrc_src )
//{
//  ProcDataBase_c::operator=( acrc_src );
//  assignFromSource( acrc_src );
//  return *this;
//}

//void ProcDataLocal_c::assignFromSource( const ProcDataLocal_c& acrc_src )
//{
//  mi32_masterVal = acrc_src.mi32_masterVal;
//  mb_cumulativeValue = acrc_src.mb_cumulativeValue;
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

bool ProcDataLocal_c::sendValISOName( ProcessPkg_c& arc_pkg, const IsoName_c& ac_varISOName, int32_t ai32_val) const
{
  arc_pkg.setISONameForDA(ac_varISOName);
  arc_pkg.setISONameForSA(isoName());

  setBasicSendFlags( arc_pkg );

  arc_pkg.setData( ai32_val );

  // send the msg
  arc_pkg.flags2String();
  getIsoBusInstance4Comm() << arc_pkg;
  // check for any error during send resolve, ...
  if ( getILibErrInstance().good(IsoAgLib::iLibErr_c::CanBus, IsoAgLib::iLibErr_c::Can) )
  { // good
    return true;
  }
  else
    return false;
  //return ProcDataBase_c::sendValISOName (arc_pkg, ac_varISOName, ai32_val);
}

// FROM FORMER BASE CLASS - functions already exist and were not calling base class functions

//void ProcDataLocal_c::processProg( const ProcessPkg_c& pkg ){
//  mc_measureprog.processProg( pkg );
//}

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

void ProcDataLocal_c::processMsg( ProcessPkg_c& pkg )
{
  isoaglib_assert( DDI() == pkg.DDI() );

  pkg.resolveCommandTypeForISO( DDI() );

  if (pkg.mc_processCmd.checkIsSetpoint())
    processSetpoint( pkg );
  else
    processProg(pkg);
}

//bool ProcDataLocal_c::timeEvent( uint16_t* /* pui16_nextTimePeriod */ )
//{
//  return true;
//}

//bool ProcDataLocal_c::sendValISOName( ProcessPkg_c& pkg, const IsoName_c& /*ac_varISOName*/, int32_t ai32_val) const
//{
//  setBasicSendFlags( pkg );
//
//  pkg.setData( ai32_val );
//
//  // send the msg
//  pkg.flags2String();
//  getIsoBusInstance4Comm() << pkg;
//  // check for any error during send resolve, ...
//  if ( getILibErrInstance().good(IsoAgLib::iLibErr_c::CanBus, IsoAgLib::iLibErr_c::Can) )
//  { // good
//    return true;
//  }
//  else
//    return false;
//}


void ProcDataLocal_c::setBasicSendFlags( ProcessPkg_c& pkg ) const
{
  // the communicating devices are represented on ISO11783
  pkg.setIsoPri(3);
  pkg.setIsoPgn(PROCESS_DATA_PGN);

  // general command is already set, use these values:
  // set command in ProcessPkg::flags2string
  const ProcessCmd_c::ValueGroup_t men_valueGroup = pkg.mc_processCmd.getValueGroup();
  const bool mb_isSetpoint = pkg.mc_processCmd.checkIsSetpoint();

  pkg.set_Element(0xFFFF);
  pkg.set_DDI(0);

  // we have only one DDI/element pair
  // interface process data init was possibly called with parameter DDI and element and not with ElementDdi_s
  // => we don't have reliable infos about men_valueGroup and mb_isSetpoint
  // => don't check for men_valueGroup and mb_isSetpoint but use this single entry in list
  pkg.set_Element(element());
  pkg.set_DDI(DDI());
}

//void ProcDataLocal_c::processProg( const ProcessPkg_c& )
//{
//  return;
//}
//
//void ProcDataLocal_c::processSetpoint( const ProcessPkg_c& )
//{
//  return;
//}

//void ProcDataBase_c::assignFromSource( const ProcIdent_c& acrc_src )
//{
//  mc_isoName = acrc_src.mc_isoName;
//  mpc_externalOverridingIsoName = acrc_src.mpc_externalOverridingIsoName;
//  // elementDDI() returns list reference, setElementDDI() expects pointer to list
//  setElementDDI(acrc_src.DDI());
//}


void ProcDataLocal_c::setExternalOverridingIsoName(const IsoName_c* apc_val)
{
  mpc_externalOverridingIsoName = apc_val;
  mc_isoName = *apc_val;
}

bool ProcDataLocal_c::matchISO( const IsoName_c& acrc_isoNameSender,
                            const IsoName_c& acrc_isoNameReceiver,
                            uint16_t aui16_DDI,
                            uint16_t aui16_element
                          ) const
{
  // check wether current element/DDI combination matches one list element in process data element/DDI list
  if (aui16_element != element()) return false;
  if (aui16_DDI != DDI()) return false;

  if (acrc_isoNameSender.isSpecified())
  { // check in remote case: check if devClass of ownerISOName in procident matches devClass of sender
    if (isoName() != acrc_isoNameSender) return false;
  }
  else
  { // check in local case: check if procident devClass matches devClass of empf
    if (isoName() != acrc_isoNameReceiver) return false;
  }

  // all previous tests are positive -> answer positive match
  return true;
}


} // end of namespace __IsoAgLib
