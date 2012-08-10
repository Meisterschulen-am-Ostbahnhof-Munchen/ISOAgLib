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
                                  IsoAgLib::ProcessDataChangeHandler_c *apc_processDataChangeHandler,
                                  int ai_multitonInst
                                  )
    : ClientBase( ai_multitonInst ),
      mc_isoName(IsoName_c::IsoNameUnspecified()),
      mc_measureprog(),
      mc_setpoint()
{
      init( aui16_ddi, aui16_element, acrc_isoName, ab_isSetpoint, aui8_triggerMethod,
            apc_processDataChangeHandler, ai_multitonInst);
}

void ProcDataLocal_c::init( uint16_t aui16_ddi, uint16_t aui16_element,
                            const IsoName_c& acrc_isoName,
                            bool ab_isSetpoint,
                            uint8_t aui8_triggerMethod,
                            IsoAgLib::ProcessDataChangeHandler_c *apc_processDataChangeHandler,
                            int ai_multitonInst
                           )
{
  mui16_ddi = aui16_ddi;
  mui16_element = aui16_element;
  procdataconfiguration.mb_isSetpoint = ab_isSetpoint;
  procdataconfiguration.mui8_triggerMethod = aui8_triggerMethod & 0x3F; // 6 bits
  mc_isoName = acrc_isoName;

  setMultitonInst(ai_multitonInst);
  mpc_processDataChangeHandler = apc_processDataChangeHandler;

  mi32_value = 0;

  getProcessInstance4Comm().registerLocalProcessData( this );

  // "Device process data objects with a data log trigger method of type total shall be settable."
  isoaglib_assert( Proc_c::isMethodSet(aui8_triggerMethod, Proc_c::MethodTotal) ? ab_isSetpoint : true );
}

ProcDataLocal_c::~ProcDataLocal_c(){
  // now unregister the pointer to this instance in Process_c
  getProcessInstance4Comm().unregisterLocalProcessData( this );
}

void ProcDataLocal_c::setMeasurementVal(int32_t ai32_val){
  mi32_value = ai32_val;
  mc_measureprog.setGlobalVal( *this, ai32_val );
}

void ProcDataLocal_c::incrMeasurementVal(int32_t ai32_val){
  mi32_value += ai32_val;
  mc_measureprog.setGlobalVal( *this, measurementVal() );
}

void ProcDataLocal_c::timeEvent( uint16_t& rui16_nextTimePeriod ){
  mc_measureprog.timeEvent( *this, rui16_nextTimePeriod);
}

void ProcDataLocal_c::processMsg( ProcessPkg_c& pkg )
{
  isoaglib_assert( DDI() == pkg.DDI() );

  pkg.resolveCommandTypeForISO( DDI(), procdataconfiguration.mb_isSetpoint );
  
  if ( pkg.mc_processCmd.getCommand() == ProcessCmd_c::setValue)
  { // process setpoint command
    if (pkg.mc_processCmd.checkIsSetpoint())
    {
      mc_setpoint.processMsg( *this, pkg );
    }
    else
    { // set value but DPD is not settable
      getProcessInstance4Comm().sendNack( pkg.senderItem()->isoName(),
                                          isoName(),
                                          DDI(),
                                          element(),
                                          0x10); // Bit 4 = 1 Process Data not setable // @TODO avoid magic number.
    }
  }
  else
  { // process measurement commands
    mc_measureprog.processMsg( *this, pkg );
  }
}

bool ProcDataLocal_c::startDataLogging(Proc_c::type_t ren_type /* Proc_c::TimeProp, Proc_c::DistProp, ... */,
                                       int32_t ai32_increment, const IsoName_c& ac_receiverDevice )
{
  isoaglib_assert( ac_receiverDevice.isSpecified() );

  return mc_measureprog.startDataLogging(*this, ren_type, ai32_increment, ac_receiverDevice);
}

void ProcDataLocal_c::stopRunningMeasurement(const IsoName_c& rc_isoName)
{
  mc_measureprog.stopRunningMeasurement(*this, rc_isoName);
}

void ProcDataLocal_c::sendValISOName( ProcessPkg_c& pkg, const IsoName_c& ac_varISOName, int32_t ai32_val) const
{
  pkg.setISONameForDA(ac_varISOName);
  pkg.setISONameForSA(isoName());

  pkg.setIsoPri(3);
  pkg.setIsoPgn(PROCESS_DATA_PGN);

  pkg.set_Element(element());
  pkg.set_DDI(DDI());

  pkg.setData( ai32_val );

  // send the msg
  pkg.flags2String();
  getIsoBusInstance4Comm() << pkg;
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

void ProcDataLocal_c::sendMeasurementVal( const IsoName_c& ac_targetISOName) const {

  ProcessPkg_c pkg;
  // prepare general command in process pkg
  pkg.mc_processCmd.setValues(false /* isSetpoint */, false /* isRequest */, ProcessCmd_c::setValue);

  sendValISOName( pkg, ac_targetISOName, measurementVal());
}

} // end of namespace __IsoAgLib
