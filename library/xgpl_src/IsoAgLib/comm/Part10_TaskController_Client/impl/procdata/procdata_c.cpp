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
#include <IsoAgLib/comm/Part5_NetworkManagement/iisoitem_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/tcclient_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/devicepool_c.h>

#include <IsoAgLib/comm/impl/isobus_c.h>

#if defined(_MSC_VER)
#pragma warning( disable : 4355 )
#endif


namespace __IsoAgLib {

  ProcData_c::ProcData_c( )
    : m_ident( NULL )
    , m_dpd( NULL )
    , m_det( NULL )
    , m_setpointhandler( NULL )
  {}


  void ProcData_c::init( IdentItem_c& ident, const DeviceObjectDpd_c& dpd, const DeviceObjectDet_c& det, SetpointHandler_c* setpointhandler ) {
    m_ident = &ident;
    m_dpd = &dpd;
    m_det = &det;
    m_setpointhandler = setpointhandler;

    isoaglib_assert(
      ( DDI() != IsoAgLib::ProcData::DefaultDataLoggingDDI ) ? true :
      IsoAgLib::ProcData::isMethodSet( triggerMethod(), IsoAgLib::ProcData::TimeInterval ) &
      IsoAgLib::ProcData::isMethodSet( triggerMethod(), IsoAgLib::ProcData::DistInterval ) &
      IsoAgLib::ProcData::isMethodSet( triggerMethod(), IsoAgLib::ProcData::ThresholdLimit ) &
      IsoAgLib::ProcData::isMethodSet( triggerMethod(), IsoAgLib::ProcData::OnChange ) &
      IsoAgLib::ProcData::isMethodSet( triggerMethod(), IsoAgLib::ProcData::Total ) );
  }


  void ProcData_c::close() {}


  void ProcData_c::setMeasurementValue( int32_t v ) {
    m_measurement.setMeasurementValue( *this, v );
  }


  void ProcData_c::incrMeasurementValue( int32_t v ) {
    m_measurement.setMeasurementValue( *this, m_measurement.getValue() + v );
  }


  void ProcData_c::processMsg( const ProcessPkg_c& pkg ) {
    isoaglib_assert( DDI() == pkg.mui16_DDI );

    if ( pkg.men_command == ProcessPkg_c::setValue ) {
      // process setpoint command
      if ( m_dpd->propertySetpoint() ) {
        m_setpoint.processMsg( *this, pkg );
      } else {
        // set value but DPD is not settable -> NACK
        ProcessPkg_c pkg;

        const uint8_t ui8_cmd = 0xd;
        pkg.setMonitorItemForDA( pkg.getMonitorItemForSA() );
        pkg.setMonitorItemForSA( m_ident->getIsoItem() );
        pkg.setIsoPri( 3 );
        pkg.setIsoPgn( PROCESS_DATA_PGN );
        pkg.setUint8Data( 0, ( uint8_t )( ( ui8_cmd & 0xf ) | ( ( element() & 0xf ) << 4 ) ) );
        pkg.setUint8Data( 1, ( uint8_t )( element() >> 4 ) );
        pkg.setUint8Data( 2, ( uint8_t )( DDI() & 0x00FF ) );
        pkg.setUint8Data( 3, ( uint8_t )( ( DDI() & 0xFF00 ) >> 8 ) );
        pkg.setUint8Data( 4, ( uint8_t )IsoAgLib::ProcData::NackProcessDataNotSetable );
        pkg.setUint8Data( 5, 0xFF );
        pkg.setUint8Data( 6, 0xFF );
        pkg.setUint8Data( 7, 0xFF );
        pkg.setLen( 8 );

        getIsoBusInstance( m_ident->getMultitonInst() ) << pkg;
      }
    } else {
      m_measurement.processMsg( *this, pkg );
    }
  }


  void ProcData_c::startDataLogging( IsoAgLib::ProcData::MeasurementCommand_t type, int32_t inc ) {
    m_measurement.startDataLogging( *this, type, inc );
  }


  void ProcData_c::stopRunningMeasurement() {
    m_measurement.stopRunningMeasurement();
  }


  void ProcData_c::sendMeasurementVal() const {

    TcClient_c::identData_t* d = getTcClientInstance( m_ident->getMultitonInst() ).getDataFor( *m_ident );

    isoaglib_assert( d );

    ProcessPkg_c pkg;

    pkg.men_command = ProcessPkg_c::setValue;
    pkg.setMonitorItemForSA( m_ident->getIsoItem() );
    pkg.setIsoPri( 3 );
    pkg.setIsoPgn( PROCESS_DATA_PGN );
    pkg.mui16_element = element();
    pkg.mui16_DDI = DDI();
    pkg.mi32_pdValue = measurementValue();

    // check if command is valid
    isoaglib_assert( pkg.men_command != ProcessPkg_c::commandReserved1 );
    isoaglib_assert( pkg.men_command != ProcessPkg_c::commandReserved2 );
    isoaglib_assert( pkg.men_command != ProcessPkg_c::commandReserved3 );
    isoaglib_assert( pkg.men_command != ProcessPkg_c::commandReserved4 );
    isoaglib_assert( pkg.men_command != ProcessPkg_c::CommandUndefined );

    uint8_t ui8_cmd = static_cast<uint8_t>( pkg.men_command );
    pkg.setUint8Data( 0, ( ui8_cmd & 0xf ) | ( ( pkg.mui16_element & 0xf ) << 4 ) );
    pkg.setUint8Data( 1, pkg.mui16_element >> 4 );
    pkg.setUint8Data( 2, pkg.mui16_DDI & 0x00FF );
    pkg.setUint8Data( 3, ( pkg.mui16_DDI& 0xFF00 ) >> 8 );
    // for ISO the ident is directly read and written
    pkg.setInt32Data( 4, pkg.mi32_pdValue );
    pkg.setLen( 8 );


    STL_NAMESPACE::list<TcClientConnection_c*>::const_iterator it = d->connections.begin();
    while ( it != d->connections.end() ) {
      pkg.setMonitorItemForDA( ( *it )->getIdentItem().getIsoItem() );
      getIsoBusInstance( m_ident->getMultitonInst() ) << pkg;
      ++it;
    }
  }


  uint16_t ProcData_c::DDI() const {
    return m_dpd->ddi();
  }


  uint16_t ProcData_c::element() const {
    return m_det->elementNumber();
  }


  uint8_t ProcData_c::triggerMethod() const {
    return m_dpd->method();
  }

}
