/*
  measureprog_c.cpp: object for managing connected local measure programs

  (C) Copyright 2009 - 2014 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#include "measureprog_c.h"
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/procdata/measuresubprog_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/pdconnection_c.h>

#if defined(USE_BASE) || defined(USE_TRACTOR_MOVE)
#include <IsoAgLib/comm/Part7_ApplicationLayer/impl/tracmove_c.h>
#endif


namespace __IsoAgLib {

  static const int32_t sci32_stopValTimeInterval = 0;
  static const int32_t sci32_stopValDistanceInterval = 0;
  static const int32_t sci32_stopValOnChange = 0;
  static const int32_t sci32_stopValThresholdMaximum = -2147483647L; // Standard specifies (-2^31+1) instead of (-2^31)
  static const int32_t sci32_stopValThresholdMinimum = 2147483647L;


  MeasureProg_c::MeasureProg_c( PdConnection_c& connection, PdLocal_c& pdLocal )
    : ConnectedPd_c( connection, pdLocal )
    , m_subProgOnChange( NULL )
    , m_subProgTimeProp( NULL )
    , m_subProgDistProp( NULL )
    , m_minThreshold( sci32_stopValThresholdMinimum )
    , m_maxThreshold( sci32_stopValThresholdMaximum )
  {
  }


  MeasureProg_c::~MeasureProg_c()
  {
    delete m_subProgDistProp;
    delete m_subProgOnChange;
    delete m_subProgTimeProp;
  }


  int32_t MeasureProg_c::getValue() const
  {
    return pdLocal().getMeasurement().getValue();
  }


  void MeasureProg_c::valueUpdated() {
    if( m_subProgOnChange )
        m_subProgOnChange->setValue( getValue() );
  }

  
  void MeasureProg_c::handleIncoming( int32_t value )
  {
    if ( !pdLocal().getSetpoint().isSettable() ) {
      connection().sendNack( pdBase().DDI(), pdBase().element(), NackProcessDataNotSettable );
      return;
    }

    pdLocal().getSetpoint().processMsg( pdLocal(), value );
  }


  bool MeasureProg_c::minMaxLimitsPassed() const {
    const bool checkMin = m_minThreshold != sci32_stopValThresholdMinimum;
    const bool checkMax = m_maxThreshold != sci32_stopValThresholdMaximum;

    if( checkMin )
    {
      if( checkMax )
      { // both given
        if ( m_maxThreshold < m_minThreshold )
        { // outside range
          return( ( getValue() <= m_maxThreshold ) || ( getValue() >= m_minThreshold ) );
        }
        else
        { // inside range
          return( ( getValue() >= m_minThreshold ) && ( getValue() <= m_maxThreshold ) );
        }
      }
      else
      { // only min
        return( getValue() >= m_minThreshold );
      }
    }
    else
    {
      if( checkMax )
      { // only max
        return( getValue() <= m_maxThreshold );
      }
      else
      { // no min, no max
        return true;
      }
    }
  }


  void MeasureProg_c::sendValue()
  {
    connection().sendProcMsg( IsoAgLib::ProcData::setValue, pdBase().DDI(), pdBase().element(), pdLocal().getMeasurement().getValue() );
  }


  bool MeasureProg_c::startMeasurement( IsoAgLib::ProcData::MeasurementCommand_t ren_type, int32_t ai32_increment ) {
    bool b_validTriggerMethod = false;

    switch ( ren_type ) {
      case IsoAgLib::ProcData::MeasurementCommandTimeProp: // time proportional
        if ( pdLocal().isMethodSet( IsoAgLib::ProcData::TimeInterval ) ) {
          if ( ai32_increment == sci32_stopValTimeInterval ) {
            delete m_subProgTimeProp;
            m_subProgTimeProp = NULL;
          } else {
            if( m_subProgTimeProp == NULL )
              m_subProgTimeProp = new MeasureTimeProp_c( *this );
            // 0, 100..60000 allowed per standard
            if( ai32_increment < 100 )
              ai32_increment = 100;
            m_subProgTimeProp->start( System_c::getTime(), ai32_increment );
          }          
          b_validTriggerMethod = true;
        }
        break;

      case IsoAgLib::ProcData::MeasurementCommandDistProp: // distance proportional
#if defined(USE_BASE) || defined(USE_TRACTOR_MOVE) // if no distance available, NACK will be sent
        if ( pdLocal().isMethodSet( IsoAgLib::ProcData::DistInterval ) ) {
          if ( ai32_increment == sci32_stopValDistanceInterval ) {
            delete m_subProgDistProp;
            m_subProgDistProp = NULL;
          } else {
            if( m_subProgDistProp == NULL )
              m_subProgDistProp = new MeasureDistProp_c( *this );
            if( ai32_increment < 0 )
              ai32_increment = -ai32_increment;
            const int32_t multitonInst = connection().getMultitonInst();
            m_subProgDistProp->start(int32_t(getTracMoveInstance(multitonInst).distTheor()), ai32_increment);
          }       
          b_validTriggerMethod = true;
        }
#endif
        break;

      case IsoAgLib::ProcData::MeasurementCommandOnChange: // change threshold proportional
        if ( pdLocal().isMethodSet( IsoAgLib::ProcData::OnChange ) ) {
          if ( ai32_increment == sci32_stopValOnChange ) {
            delete m_subProgOnChange;
            m_subProgOnChange = NULL;
          } else {
            if( m_subProgOnChange == NULL )
              m_subProgOnChange = new MeasureOnChange_c( *this );
            if( ai32_increment < 0 )
              ai32_increment = -ai32_increment;
            m_subProgOnChange->start( getValue(), ai32_increment );
          }      
          b_validTriggerMethod = true;
        }
        break;

      case IsoAgLib::ProcData::MeasurementCommandMaximumThreshold: // change threshold proportional
        if ( pdLocal().isMethodSet( IsoAgLib::ProcData::ThresholdLimit ) ) {
          m_maxThreshold = ai32_increment;
          b_validTriggerMethod = true;
          // TODO send process data value if necessary ?
        }
        break;

      case IsoAgLib::ProcData::MeasurementCommandMinimumThreshold: // change threshold proportional
        if ( pdLocal().isMethodSet( IsoAgLib::ProcData::ThresholdLimit ) ) {
          m_minThreshold = ai32_increment;
          b_validTriggerMethod = true;
          // TODO send process data value if necessary ?
        }
        break;
    }

    return b_validTriggerMethod;
  }


  void MeasureProg_c::stopAllMeasurements() {
    delete m_subProgDistProp;
    m_subProgDistProp = NULL;

    delete m_subProgOnChange;
    m_subProgOnChange = NULL;

    delete m_subProgTimeProp;
    m_subProgTimeProp = NULL;

    m_minThreshold = sci32_stopValThresholdMinimum;
    m_maxThreshold = sci32_stopValThresholdMaximum;
  }

}
