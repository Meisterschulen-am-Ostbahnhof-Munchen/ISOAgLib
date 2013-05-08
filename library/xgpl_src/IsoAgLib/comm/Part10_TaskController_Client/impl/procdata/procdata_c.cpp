/*
  procdata_c.cpp

  (C) Copyright 2009 - 2013 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#include "procdata_c.h"
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/tcclient_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/iprocdatahandler_c.h>

#include <IsoAgLib/comm/impl/isobus_c.h>

#if defined(_MSC_VER)
#pragma warning( disable : 4355 )
#endif


namespace __IsoAgLib {

ProcData_c::ProcData_c( )
    : m_tcCC( NULL )
    , m_dpd( NULL )
    , m_det( NULL )
    , mpc_procDataHandler( NULL )
    , mc_measureprog()
    , mc_setpoint()
{}


void
ProcData_c::init( TcClientConnection_c& tcCC, const IsoAgLib::iDeviceObjectDpd_c& dpd, const IsoAgLib::iDeviceObjectDet_c& det, IsoAgLib::iProcDataHandler_c *procDataHandler )
{
  m_tcCC = &tcCC;
  m_dpd = &dpd;
  m_det = &det;
  mpc_procDataHandler = procDataHandler;

  m_tcCC->registerLocalProcessData( this );

  isoaglib_assert(
    (DDI() != IsoAgLib::ProcData::DefaultDataLoggingDDI) ? true : 
    IsoAgLib::ProcData::isMethodSet(triggerMethod(), IsoAgLib::ProcData::TimeInterval) & 
    IsoAgLib::ProcData::isMethodSet(triggerMethod(), IsoAgLib::ProcData::DistInterval) &
    IsoAgLib::ProcData::isMethodSet(triggerMethod(), IsoAgLib::ProcData::ThresholdLimit) &
    IsoAgLib::ProcData::isMethodSet(triggerMethod(), IsoAgLib::ProcData::OnChange) &
    IsoAgLib::ProcData::isMethodSet(triggerMethod(), IsoAgLib::ProcData::Total) );
}


void
ProcData_c::close()
{
  m_tcCC->unregisterLocalProcessData( this );
}


void
ProcData_c::setMeasurementVal(int32_t ai32_val)
{
  mc_measureprog.setVal( *this, ai32_val );
}


void
ProcData_c::incrMeasurementVal(int32_t ai32_val)
{
  int32_t i32_value = mc_measureprog.measurementValue() + ai32_val;
  mc_measureprog.setVal( *this, i32_value );
}


void
ProcData_c::timeEvent( uint16_t& rui16_nextTimePeriod )
{
  mc_measureprog.timeEvent( *this, rui16_nextTimePeriod);
}


void
ProcData_c::processMsg( const ProcessPkg_c& pkg, IsoAgLib::ProcData::RemoteType_t a_ecuType )
{
  isoaglib_assert( DDI() == pkg.mui16_DDI );
  
  if ( IsoAgLib::ProcData::DefaultDataLoggingDDI == pkg.mui16_DDI ) 
  {
    isoaglib_assert( getProcDataHandler() != NULL );
    
    // if method is request, notify the application and later respond to the request
    if ( pkg.men_command == ProcessPkg_c::requestValue)
      getProcDataHandler()->processDefaultLoggingStart(a_ecuType );
    //else // command not supported for default DDI
    //@TODO NACK ? Bit 0 = 1 = Process Data Command not supported
    //return;
  }
  
  if ( pkg.men_command == ProcessPkg_c::setValue )
  { // process setpoint command
    if (isSetPoint())
    {
      mc_setpoint.processMsg( *this, pkg, a_ecuType );
    }
    else
    { // set value but DPD is not settable
      m_tcCC->sendNack( pkg.getMonitorItemForSA()->isoName(),
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


void
ProcData_c::startDataLogging(
  IsoAgLib::ProcData::MeasurementCommand_t ren_type /* IsoAgLib::ProcData::TimeProp, IsoAgLib::ProcData::DistProp, ... */,
  int32_t ai32_increment,
  IsoAgLib::ProcData::RemoteType_t a_ecuType )
{
  isoaglib_assert( IsoAgLib::ProcData::RemoteTypeUndefined != a_ecuType );

  mc_measureprog.startDataLogging(*this, ren_type, ai32_increment, a_ecuType);
}


void
ProcData_c::stopRunningMeasurement( IsoAgLib::ProcData::RemoteType_t a_ecuType )
{
  mc_measureprog.stopRunningMeasurement( a_ecuType );
}


void
ProcData_c::sendValue( IsoAgLib::ProcData::RemoteType_t a_ecuType, int32_t ai32_val) const
{
  const IsoName_c& c_destinationISOName = m_tcCC->getTcClient().getISONameFromType( a_ecuType );

  if (!c_destinationISOName.isSpecified())
    return;

  m_tcCC->sendProcMsg( c_destinationISOName, isoName(), DDI(), element(), ai32_val );
}


void
ProcData_c::sendMeasurementVal( const IsoName_c& ac_targetISOName) const
{
  sendValue( m_tcCC->getTcClient().getTypeFromISOName( ac_targetISOName ), measurementVal() );
}

}
