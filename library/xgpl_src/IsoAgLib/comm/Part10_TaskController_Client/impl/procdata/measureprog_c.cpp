/*
  measureprog_c.cpp: object for managing local measure programs

  (C) Copyright 2009 - 2013 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#include "measureprog_c.h"
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/tcclient_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/procdata/procdata_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/tcclientconnection_c.h>

#if defined(USE_BASE) || defined(USE_TRACTOR_MOVE)
#include <IsoAgLib/comm/Part7_ApplicationLayer/impl/tracmove_c.h>
#endif


namespace __IsoAgLib {

  static const int32_t sci32_stopValTimeInterval = 0;
  static const int32_t sci32_stopValDistanceInterval = 0;
  static const int32_t sci32_stopValOnChange = 0;
  static const int32_t sci32_stopValThresholdMaximum = -2147483647L; // Standard specifies (-2^31+1) instead of (-2^31)
  static const int32_t sci32_stopValThresholdMinimum = 2147483647L;

  MeasureProg_c::MeasureProg_c( TcClientConnection_c* c, ProcData_c& procdata )
      : SchedulerTask_c( 100, false ), m_value( 0 ), m_connection( c ), m_procdata(procdata) {
    m_procdata.getMeasurement().addMeasureProgRef( *this );
  }


  MeasureProg_c::~MeasureProg_c() {
    m_procdata.getMeasurement().removeMeasureProgRef( *this );
  }


  void MeasureProg_c::processMeasurementMsg( ProcessPkg_c::CommandType_t command, int32_t pdValue ) {

    switch ( command ) {
      case ProcessPkg_c::measurementDistanceValueStart:
      case ProcessPkg_c::measurementTimeValueStart:
      case ProcessPkg_c::measurementChangeThresholdValueStart:
      case ProcessPkg_c::measurementMinimumThresholdValueStart:
      case ProcessPkg_c::measurementMaximumThresholdValueStart:
        // measurementCommand_t and CommandType_t are unified for all measurement types
        if( !startMeasurement( IsoAgLib::ProcData::MeasurementCommand_t( command ), pdValue ) ) {
          isoaglib_assert( m_connection );
          m_connection->sendNack( m_procdata.DDI(), m_procdata.element(), IsoAgLib::ProcData::NackTriggerMethodNotSupported );
        }
        break;

      default:
        isoaglib_assert( !"Method shall not be called for this Process command" );
        break;
    }
  }


  void MeasureProg_c::processRequestMsg() {
    isoaglib_assert( m_connection );
    m_connection->sendProcMsg( m_procdata.DDI(), m_procdata.element(), m_value );
  }


  void MeasureProg_c::processSetMsg( int32_t pdValue ) {
    isoaglib_assert( m_connection );
    m_procdata.getSetpoint().processMsg(m_procdata, *m_connection, pdValue);
  }


  void MeasureProg_c::setValue( int32_t ai32_val ) {

    m_value = ai32_val;
    const int32_t i32_time = System_c::getTime();

    // now check if one subprog triggers
    for ( MeasureSubprogContainerIterator_t pc_iter = m_measureSubprog.begin(); pc_iter != m_measureSubprog.end(); ++pc_iter ) {

      bool triggeredIncrement = false;

      switch ( pc_iter->type() ) {
        case IsoAgLib::ProcData::MeasurementCommandOnChange:
          triggeredIncrement = pc_iter->updateTrigger( ai32_val );
          break;
        case IsoAgLib::ProcData::MeasurementCommandTimeProp:
        case IsoAgLib::ProcData::MeasurementCommandMinimumThreshold:
        case IsoAgLib::ProcData::MeasurementCommandDistProp:
        case IsoAgLib::ProcData::MeasurementCommandMaximumThreshold:
          break;
      }


      // if triggeredIncrement == true the registered values should be sent
      if ( triggeredIncrement ) {
        // send the registered values
        if ( !minMaxLimitsPassed( ai32_val ) ) {
          // omit this value send
          continue;
        }
        // if at least one send try had success reset triggeredIncrement
        isoaglib_assert( m_connection );
        m_connection->sendProcMsg( m_procdata.DDI(), m_procdata.element(), ai32_val );
      }
    }
  }


  void MeasureProg_c::timeEvent() {
    const int32_t i32_time = System_c::getTime();

    for ( MeasureSubprogContainerIterator_t pc_iter = m_measureSubprog.begin(); pc_iter != m_measureSubprog.end(); ++pc_iter ) {
      bool triggeredIncrement = false;
      int32_t i32_nextTimePeriod = 0;
      switch ( pc_iter->type() ) {
        case IsoAgLib::ProcData::MeasurementCommandTimeProp:
          triggeredIncrement = pc_iter->updateTrigger( i32_time );
          i32_nextTimePeriod = pc_iter->nextTriggerTime( m_procdata, i32_time );
          break;
        case IsoAgLib::ProcData::MeasurementCommandDistProp: {
#if defined(USE_BASE) || defined(USE_TRACTOR_MOVE)
            int32_t i32_distTheor = getTracMoveInstance(m_procdata.identItem().getMultitonInst()).distTheor();
            triggeredIncrement = pc_iter->updateTrigger(i32_distTheor);
#else
            int32_t i32_distTheor = 0;
#endif
            i32_nextTimePeriod = pc_iter->nextTriggerTime( m_procdata, i32_distTheor );
          }
          break;
        case IsoAgLib::ProcData::MeasurementCommandOnChange:
        case IsoAgLib::ProcData::MeasurementCommandMinimumThreshold:
        case IsoAgLib::ProcData::MeasurementCommandMaximumThreshold:
          break;
      } // switch

      if ( i32_nextTimePeriod > 0 ) {
        setPeriod( i32_nextTimePeriod, false );
      }

      // if triggeredIncrement == true the registered values should be sent
      if ( triggeredIncrement ) {
        if ( !minMaxLimitsPassed( m_value ) ) {
          // omit this value send
          continue;
        }
        isoaglib_assert( m_connection );
        m_connection->sendProcMsg( m_procdata.DDI(), m_procdata.element(), m_value );
      }
    }
  }


  bool MeasureProg_c::minMaxLimitsPassed( int32_t value ) const {
    // no threshold -> skip
    if ( m_thresholdInfo.empty() )
      return true;

    bool b_checkMin = false;
    bool b_checkMax = false;
    int32_t i32_maxVal = 0;
    int32_t i32_minVal = 0;

    for ( ThresholdInfoContainerConstIterator_t iterThreshold = m_thresholdInfo.begin(); iterThreshold != m_thresholdInfo.end(); ++iterThreshold ) {
      switch ( iterThreshold->en_type ) {
        case IsoAgLib::ProcData::MeasurementCommandMaximumThreshold:
          b_checkMax = true;
          i32_maxVal = iterThreshold->i32_threshold;
          break;

        case IsoAgLib::ProcData::MeasurementCommandMinimumThreshold:
          b_checkMin = true;
          i32_minVal = iterThreshold->i32_threshold;
          break;

        default:
          ;
      }
    }

    if ( b_checkMin && b_checkMax && ( i32_maxVal < i32_minVal ) && ( ( i32_maxVal >= value ) || ( i32_minVal <= value ) ) )
      return true;

    if ( ( b_checkMin && i32_minVal > value ) ||
         ( b_checkMax && i32_maxVal < value ) )
      return false;

    return true;
  }


  MeasureSubprog_c& MeasureProg_c::addSubprog( IsoAgLib::ProcData::MeasurementCommand_t ren_type, int32_t ai32_increment ) {
    // if subprog with this type exist, update only value
    MeasureSubprogContainerIterator_t pc_subprog = m_measureSubprog.end();
    for ( pc_subprog = m_measureSubprog.begin(); pc_subprog != m_measureSubprog.end(); ++pc_subprog ) {
      if ( pc_subprog->type() == ren_type ) {
        // TODO improve set of period: search period in list of subprog (time interval or 50ms if distance found)
        pc_subprog->setIncrement( ai32_increment );
        return *pc_subprog;
      }
    }

    m_measureSubprog.push_front( MeasureSubprog_c( ren_type, ai32_increment ) );

    if( ! isRegistered() ) {
      switch( ren_type ) {
          case IsoAgLib::ProcData::MeasurementCommandTimeProp:
          case IsoAgLib::ProcData::MeasurementCommandDistProp:
            getSchedulerInstance().registerTask( *this, 0 );
            // TODO improve set of period: search period in list of subprog (time interval or 50ms if distance found)
            setPeriod(50,true);
            break;
          default:
            break;
      }
    }
    return *( m_measureSubprog.begin() );
  }


  bool MeasureProg_c::startMeasurement( IsoAgLib::ProcData::MeasurementCommand_t ren_type, int32_t ai32_increment ) {
    bool b_validTriggerMethod = false;

    switch ( ren_type ) {
      case IsoAgLib::ProcData::MeasurementCommandTimeProp: // time proportional
        if ( IsoAgLib::ProcData::isMethodSet( m_procdata.triggerMethod(), IsoAgLib::ProcData::TimeInterval ) ) {
          if ( ai32_increment == sci32_stopValTimeInterval ) {
            stopMeasurement( ren_type );
          } else {
            MeasureSubprog_c& subprog = addSubprog( ren_type, __IsoAgLib::abs( ai32_increment ) );
            subprog.start( System_c::getTime() );
          }
          isoaglib_assert( m_connection );
          m_connection->sendProcMsg( m_procdata.DDI(), m_procdata.element(), m_value );
          b_validTriggerMethod = true;
        }
        break;

      case IsoAgLib::ProcData::MeasurementCommandDistProp: // distance proportional
#if defined(USE_BASE) || defined(USE_TRACTOR_MOVE) // if no distance available, NACK will be sent
        if ( IsoAgLib::ProcData::isMethodSet( ac_processData.triggerMethod(), IsoAgLib::ProcData::DistInterval ) ) {
          if ( ai32_increment == sci32_stopValDistanceInterval ) {
            stopMeasurement( ren_type );
          } else {
            MeasureSubprog_c& subprog = addSubprog(ren_type, ai32_increment);
            subprog.start(int32_t(getTracMoveInstance(ac_processData.identItem().getMultitonInst()).distTheor()));
          }
          isoaglib_assert( m_connection );
          m_connection->sendProcMsg( ac_processData.DDI(), ac_processData.element(), value );
          b_validTriggerMethod = true;
        }
#endif
        break;

      case IsoAgLib::ProcData::MeasurementCommandOnChange: // change threshold proportional
        if ( IsoAgLib::ProcData::isMethodSet( m_procdata.triggerMethod(), IsoAgLib::ProcData::OnChange ) ) {
          if ( ai32_increment == sci32_stopValOnChange ) {
            stopMeasurement( ren_type );
          } else {
            MeasureSubprog_c& subprog = addSubprog( ren_type, ai32_increment );
            subprog.start( m_value );
          }
          isoaglib_assert( m_connection );
          m_connection->sendProcMsg( m_procdata.DDI(), m_procdata.element(), m_value );
          b_validTriggerMethod = true;
        }
        break;

      case IsoAgLib::ProcData::MeasurementCommandMaximumThreshold: // change threshold proportional
        if ( IsoAgLib::ProcData::isMethodSet( m_procdata.triggerMethod(), IsoAgLib::ProcData::ThresholdLimit ) ) {
          if ( ai32_increment == sci32_stopValThresholdMaximum ) {
            stopMeasurement( ren_type );
          } else {
            MeasureSubprog_c& subprog = addSubprog( ren_type, ai32_increment );
            subprog.start();
            const ThresholdInfo_s s_thresholdInfo = {ren_type, subprog.increment()};
            m_thresholdInfo.push_front( s_thresholdInfo );
          }
          // Note : do not send value when a threshold is set
          b_validTriggerMethod = true;
        }
        break;

      case IsoAgLib::ProcData::MeasurementCommandMinimumThreshold: // change threshold proportional
        if ( IsoAgLib::ProcData::isMethodSet( m_procdata.triggerMethod(), IsoAgLib::ProcData::ThresholdLimit ) ) {
          if ( ai32_increment == sci32_stopValThresholdMinimum ) {
            stopMeasurement( ren_type );
          } else {
            MeasureSubprog_c& subprog = addSubprog( ren_type, ai32_increment );
            subprog.start();
            const ThresholdInfo_s s_thresholdInfo = {ren_type, subprog.increment()};
            m_thresholdInfo.push_front( s_thresholdInfo );
          }
          // Note : do not send value when a threshold is set
          b_validTriggerMethod = true;
        }
        break;

      default:
        isoaglib_assert( !"method should not be called with this command" );
        break;
    }

    return b_validTriggerMethod;
  }


  void MeasureProg_c::stopMeasurement( IsoAgLib::ProcData::MeasurementCommand_t ren_type ) {
    for ( MeasureSubprogContainerIterator_t pc_iter = m_measureSubprog.begin(); pc_iter != m_measureSubprog.end(); ) {
      if ( pc_iter->type() == ren_type )
        pc_iter = m_measureSubprog.erase( pc_iter );
      else
        ++pc_iter;
    }

    // additionally cleanup corresponding threshold info
    if ( ( IsoAgLib::ProcData::MeasurementCommandMinimumThreshold == ren_type ) || ( IsoAgLib::ProcData::MeasurementCommandMaximumThreshold == ren_type ) ) {
      for ( ThresholdInfoContainerIterator_t iterThreshold = m_thresholdInfo.begin(); iterThreshold != m_thresholdInfo.end(); ) {
        if ( iterThreshold->en_type == ren_type )
          iterThreshold = m_thresholdInfo.erase( iterThreshold );
        else
          ++iterThreshold;
      }
    }

    if( isRegistered() ) {
        for (MeasureSubprogContainerIterator_t pc_iter = m_measureSubprog.begin(); pc_iter != m_measureSubprog.end(); ++pc_iter) {
          if ( pc_iter->type() == IsoAgLib::ProcData::MeasurementCommandTimeProp || IsoAgLib::ProcData::MeasurementCommandDistProp )
          {
            // TODO improve set of period: search period in list of subprog (time interval or 50ms if distance found)
            return;
          }
        }
        getSchedulerInstance().deregisterTask(*this);
    }
  }


  void MeasureProg_c::stopAllMeasurements() {
    m_measureSubprog.clear();
    m_thresholdInfo.clear();

    if( isRegistered() ) {
        getSchedulerInstance().deregisterTask(*this);
    }
  }

}
