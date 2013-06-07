/*
  measurement_c.cpp

  (C) Copyright 2009 - 2013 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include <IsoAgLib/comm/Part10_TaskController_Client/impl/procdata/measurement_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/procdata/procdata_c.h>

namespace __IsoAgLib {


  Measurement_c::Measurement_c() : m_value( 0 ), m_measureProgs() {}


  void Measurement_c::addMeasureProg( MeasureProg_c* m ) {
    isoaglib_assert( m );
    m_measureProgs.push_front( m );
  }


  void Measurement_c::removeMeasureProg( MeasureProg_c* m ) {
    for( STL_NAMESPACE::list<MeasureProg_c*>::iterator i = m_measureProgs.begin(); i != m_measureProgs.end(); ++i ) {
      if( ( *i ) == m ) {
        m_measureProgs.erase( i );
        break;
      }
    }
  }


  void Measurement_c::setMeasurementValue( ProcData_c& pd, int32_t v ) {
    m_value = v;
    for( STL_NAMESPACE::list<MeasureProg_c*>::iterator i = m_measureProgs.begin(); i != m_measureProgs.end(); ++i ) {
      ( *i )->setValue( pd, v );
    }
  }


  void Measurement_c::processMsg( ProcData_c& pd, const ProcessPkg_c& pkg ) {
    for( STL_NAMESPACE::list<MeasureProg_c*>::iterator i = m_measureProgs.begin(); i != m_measureProgs.end(); ++i ) {
      (*i)->processMsg( pd, pkg, getValue() );
    }
  }


  void Measurement_c::startDataLogging( ProcData_c& pd, IsoAgLib::ProcData::MeasurementCommand_t type, int32_t inc ) {
    for( STL_NAMESPACE::list<MeasureProg_c*>::iterator i = m_measureProgs.begin(); i != m_measureProgs.end(); ++i ) {
      (*i)->handleMeasurement( pd, type, inc, getValue() );
    }
  }


  void Measurement_c::stopRunningMeasurement() {
    for( STL_NAMESPACE::list<MeasureProg_c*>::iterator i = m_measureProgs.begin(); i != m_measureProgs.end(); ++i ) {
      (*i)->stopAllMeasurements();
    }
  }

}
