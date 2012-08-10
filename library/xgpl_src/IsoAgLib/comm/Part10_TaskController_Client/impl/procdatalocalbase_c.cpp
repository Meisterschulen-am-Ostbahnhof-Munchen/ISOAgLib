/***************************************************************************
                          procdatalocalbase_c.cpp - managing of simple

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#include "procdatalocalbase_c.h"
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/process_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/processdatachangehandler_c.h>


namespace __IsoAgLib {

void ProcDataLocalBase_c::init(uint16_t aui16_ddi, uint16_t aui16_element,
                               const IsoName_c& acrc_isoName, const IsoName_c *apc_externalOverridingIsoName,
                               bool ab_cumulativeValue,
                               IsoAgLib::ProcessDataChangeHandler_c *apc_processDataChangeHandler,
                               int ai_multitonInst
                               )
{
  ProcDataBase_c::init( aui16_ddi, aui16_element, acrc_isoName, apc_externalOverridingIsoName,
                        apc_processDataChangeHandler, ai_multitonInst);

  mb_cumulativeValue = ab_cumulativeValue;
  mi32_masterVal = 0;

  getProcessInstance4Comm().registerLocalProcessData( this );
}


ProcDataLocalBase_c::ProcDataLocalBase_c( const ProcDataLocalBase_c& acrc_src )
  : ProcDataBase_c( acrc_src )
{
  assignFromSource( acrc_src );

  // now register the pointer to this instance in Process_c
  getProcessInstance4Comm().registerLocalProcessData( this );
}

const ProcDataLocalBase_c& ProcDataLocalBase_c::operator=( const ProcDataLocalBase_c& acrc_src )
{
  ProcDataBase_c::operator=( acrc_src );
  assignFromSource( acrc_src );
  return *this;
}

void ProcDataLocalBase_c::assignFromSource( const ProcDataLocalBase_c& acrc_src )
{
  mi32_masterVal = acrc_src.mi32_masterVal;
  mb_cumulativeValue = acrc_src.mb_cumulativeValue;
}

ProcDataLocalBase_c::~ProcDataLocalBase_c(){
  // call unregisterLocalProcessData in last derived class because unregister does again message processing!
}

void ProcDataLocalBase_c::setMasterMeasurementVal(int32_t ai32_val)
{
  mi32_masterVal = ai32_val;
}

void ProcDataLocalBase_c::incrMasterMeasurementVal(int32_t ai32_val)
{
  mi32_masterVal += ai32_val;
}

bool ProcDataLocalBase_c::timeEvent( uint16_t* /* pui16_nextTimePeriod */)
{
  return true;
}

bool ProcDataLocalBase_c::sendMasterMeasurementVal( const IsoName_c& ac_targetISOName) const {

  ProcessPkg_c pkg;
  // prepare general command in process pkg
  pkg.mc_processCmd.setValues(false /* isSetpoint */, false, /* isRequest */
                                                           ProcessCmd_c::exactValue,
                                                           ProcessCmd_c::setValue);

  return sendValISOName( pkg, ac_targetISOName, masterMeasurementVal());
}

void ProcDataLocalBase_c::processProg( const ProcessPkg_c& pkg )
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

void ProcDataLocalBase_c::processSetpoint( const ProcessPkg_c& pkg )
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

bool ProcDataLocalBase_c::sendValISOName( ProcessPkg_c& arc_pkg, const IsoName_c& ac_varISOName, int32_t ai32_val) const
{
  arc_pkg.setISONameForDA(ac_varISOName);
  arc_pkg.setISONameForSA(isoName());

  return ProcDataBase_c::sendValISOName (arc_pkg, ac_varISOName, ai32_val);
}

} // end of namespace __IsoAgLib
