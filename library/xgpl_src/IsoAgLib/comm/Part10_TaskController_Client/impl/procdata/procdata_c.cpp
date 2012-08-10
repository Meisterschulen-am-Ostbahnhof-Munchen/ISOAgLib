/*
  procdata_c.h

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#include "procdata_c.h"
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/identitem_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/process_c.h>
#include <IsoAgLib/comm/impl/isobus_c.h>

#if defined(_MSC_VER)
#pragma warning( disable : 4355 )
#endif


namespace __IsoAgLib {

ProcData_c::ProcData_c( )
    : mc_myIdentItem( NULL ),
      mui16_ddi( 0xFFFF ),
      mui16_element( 0xFFFF ),
      mpc_procDataHandler( NULL ),
      mc_measureprog(),
      mc_setpoint()
{
  procdataconfiguration.mb_isSetpoint = false;
  procdataconfiguration.mui8_triggerMethod = 0;
}

void ProcData_c::init( IdentItem_c& acrc_identItem,
                       uint16_t aui16_ddi,
                       uint16_t aui16_element,
                       bool ab_isSetpoint,
                       uint8_t aui8_triggerMethod,
                       IsoAgLib::iProcDataHandler_c *apc_procDataHandler)
{
  mc_myIdentItem = &acrc_identItem;

  mui16_ddi = aui16_ddi;
  mui16_element = aui16_element;
  procdataconfiguration.mb_isSetpoint = ab_isSetpoint;
  procdataconfiguration.mui8_triggerMethod = aui8_triggerMethod & 0x3F; // 6 bits

  mpc_procDataHandler = apc_procDataHandler;

  getProcessInstance( mc_myIdentItem->getMultitonInst() ).registerLocalProcessData( this );

  // "Device process data objects with a data log trigger method of type total shall be settable."
  isoaglib_assert( ( IsoAgLib::ProcData::isMethodSet(aui8_triggerMethod, IsoAgLib::ProcData::MethodTotal)
                          && (aui16_ddi != IsoAgLib::ProcData::defaultDataLoggingDDI) ) ? ab_isSetpoint : true );
}

ProcData_c::~ProcData_c() {
  isoaglib_assert( mc_myIdentItem != NULL );
  getProcessInstance( mc_myIdentItem->getMultitonInst() ).unregisterLocalProcessData( this );
}

void ProcData_c::setMeasurementVal(int32_t ai32_val) {
  mc_measureprog.setVal( *this, ai32_val );
}

void ProcData_c::incrMeasurementVal(int32_t ai32_val){
  int32_t i32_value = mc_measureprog.measurementValue() + ai32_val;
  mc_measureprog.setVal( *this, i32_value );
}

void ProcData_c::timeEvent( uint16_t& rui16_nextTimePeriod ){
  mc_measureprog.timeEvent( *this, rui16_nextTimePeriod);
}

void ProcData_c::processMsg( ProcessPkg_c& pkg, IsoAgLib::ProcData::remoteType_t a_ecuType )
{
  isoaglib_assert( DDI() == pkg.mui16_DDI );
  
  if ( IsoAgLib::ProcData::defaultDataLoggingDDI == pkg.mui16_DDI ) 
  {
    isoaglib_assert( getProcDataHandler() != NULL );
    
    // if method is request, notify the application and later respond to the request
    if ( pkg.men_command == ProcessPkg_c::requestValue)
      getProcDataHandler()->processDefaultLoggingStart(
               pkg.getMonitorItemForSA()->isoName().toConstIisoName_c() );
    //else // command not supported for default DDI
    //NACK ? Bit 0 = 1 = Process Data Command not supported
    //return;
  }
  
  if ( pkg.men_command == ProcessPkg_c::setValue )
  { // process setpoint command
    if (isSetPoint())
    {
      mc_setpoint.processMsg( *this, pkg );
    }
    else
    { // set value but DPD is not settable
      getProcessInstance( mc_myIdentItem->getMultitonInst() ).sendNack(
                                          pkg.getMonitorItemForSA()->isoName(),
                                          isoName(),
                                          DDI(),
                                          element(),
                                          IsoAgLib::ProcData::NackProcessDataNotSetable);
    }
  }
  else
  { // process measurement commands
    mc_measureprog.processMsg( *this, pkg, a_ecuType );
  }
}

void ProcData_c::startDataLogging(IsoAgLib::ProcData::measurementCommand_t ren_type /* IsoAgLib::ProcData::TimeProp, IsoAgLib::ProcData::DistProp, ... */,
                                  int32_t ai32_increment, const IsoName_c& ac_receiverDevice )
{
  isoaglib_assert( ac_receiverDevice.isSpecified() );

  mc_measureprog.startDataLogging(*this, ren_type, ai32_increment, ac_receiverDevice);
}

void ProcData_c::stopRunningMeasurement(IsoAgLib::ProcData::remoteType_t a_ecuType)
{
  mc_measureprog.stopRunningMeasurement(*this, a_ecuType);
}

void ProcData_c::sendValue( IsoAgLib::ProcData::remoteType_t a_ecuType, int32_t ai32_val) const
{
  isoaglib_assert( mc_myIdentItem != NULL );
  const IsoName_c& c_destinationISOName = getProcessInstance( mc_myIdentItem->getMultitonInst() ).getISONameFromType( a_ecuType );

  if (!c_destinationISOName.isSpecified()) return;
  
  ProcessPkg_c pkg;
  pkg.men_command = ProcessPkg_c::setValue;

  pkg.setISONameForDA(c_destinationISOName);
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
  getIsoBusInstance( mc_myIdentItem->getMultitonInst() ) << pkg;
}

void ProcData_c::sendMeasurementVal( const IsoName_c& ac_targetISOName) const {
  isoaglib_assert( mc_myIdentItem != NULL );
  sendValue( getProcessInstance( mc_myIdentItem->getMultitonInst() ).getTypeFromISOName( ac_targetISOName ), measurementVal());
}

} // end of namespace __IsoAgLib
