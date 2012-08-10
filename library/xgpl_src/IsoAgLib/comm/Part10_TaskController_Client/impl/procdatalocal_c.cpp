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
  mc_measureprog.setVal( *this, mi32_value );
}

void ProcDataLocal_c::incrMeasurementVal(int32_t ai32_val){
  mi32_value += ai32_val;
  mc_measureprog.setVal( *this, mi32_value );
}

void ProcDataLocal_c::timeEvent( uint16_t& rui16_nextTimePeriod ){
  mc_measureprog.timeEvent( *this, rui16_nextTimePeriod);
}

void ProcDataLocal_c::processMsg( ProcessPkg_c& pkg )
{
  isoaglib_assert( DDI() == pkg.mui16_DDI );
  
  if ( pkg.men_command == ProcessPkg_c::setValue)
  { // process setpoint command
    if (isSetPoint())
    {
      mc_setpoint.processMsg( *this, pkg );
    }
    else
    { // set value but DPD is not settable
      getProcessInstance4Comm().sendNack( pkg.getMonitorItemForSA()->isoName(),
                                          isoName(),
                                          DDI(),
                                          element(),
                                          Proc_c::NackProcessDataNotSetable);
    }
  }
  else
  { // process measurement commands
    mc_measureprog.processMsg( *this, pkg );
  }
}

bool ProcDataLocal_c::startDataLogging(Proc_c::measurementCommand_t ren_type /* Proc_c::TimeProp, Proc_c::DistProp, ... */,
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

  pkg.mui16_element = element();
  pkg.mui16_DDI = DDI();

  pkg.mi32_pdValue = ai32_val;

  // check if command is valid
  isoaglib_assert(pkg.men_command != ProcessPkg_c::commandReserved1);
  isoaglib_assert(pkg.men_command != ProcessPkg_c::commandReserved2);
  isoaglib_assert(pkg.men_command != ProcessPkg_c::commandReserved3);
  isoaglib_assert(pkg.men_command != ProcessPkg_c::commandReserved4);
  isoaglib_assert(pkg.men_command != ProcessPkg_c::CommandUndefined);

  uint8_t ui8_cmd = static_cast<uint8_t>(pkg.men_command);

  pkg.setUint8Data(0, (ui8_cmd & 0xf) | ( (pkg.mui16_element & 0xf) << 4) );
  pkg.setUint8Data(1, pkg.mui16_element >> 4 );
  pkg.setUint8Data(2, pkg.mui16_DDI & 0x00FF );
  pkg.setUint8Data(3, (pkg.mui16_DDI& 0xFF00) >> 8 );
  // for ISO the ident is directly read and written

  pkg.setInt32Data(4, pkg.mi32_pdValue);

  pkg.setLen(8);

  // send the msg
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
  pkg.men_command = ProcessPkg_c::setValue;

  sendValISOName( pkg, ac_targetISOName, measurementVal());
}

} // end of namespace __IsoAgLib
