/*
  measureprog_c.cpp: object for managing local measure programs

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#include "measureprog_c.h"
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/tcclient_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/procdata/procdata_c.h>

#if defined(USE_BASE) || defined(USE_TRACTOR_MOVE)
#include <IsoAgLib/comm/Part7_ApplicationLayer/impl/tracmove_c.h>
#endif


namespace __IsoAgLib {

MeasureProg_c::MeasureProg_c( IsoAgLib::ProcData::remoteType_t ecutype )
  : mlist_thresholdInfo()
  , mvec_measureSubprog()
  , m_ecuType(ecutype)
{
}


bool
MeasureProg_c::processMsg( ProcData_c& ac_processData, const ProcessPkg_c& pkg, int32_t value )
{
  ProcessPkg_c::CommandType_t en_command = pkg.men_command;

  isoaglib_assert( pkg.getMonitorItemForSA() != NULL ); // should have been filtered earlier!

  switch (en_command)
  {
    case ProcessPkg_c::measurementDistanceValueStart:
    case ProcessPkg_c::measurementTimeValueStart:
    case ProcessPkg_c::measurementChangeThresholdValueStart:
    case ProcessPkg_c::measurementMinimumThresholdValueStart:
    case ProcessPkg_c::measurementMaximumThresholdValueStart:
      // measurementCommand_t and CommandType_t are unified for all measurement types
      if( !startMeasurement( ac_processData, IsoAgLib::ProcData::measurementCommand_t( en_command ), pkg.mi32_pdValue, value ) )
      {
        getTcClientInstance( ac_processData.getMultitonInst() ).sendNack(
          pkg.getMonitorItemForSA()->isoName(),
          ac_processData.isoName(),
          ac_processData.DDI(),
          ac_processData.element(),
          IsoAgLib::ProcData::NackTriggerMethodNotSupported);
      }
      break;
	  
    case ProcessPkg_c::requestValue:
      ac_processData.sendValue( m_ecuType, value);
      break;
	  
    default:
      isoaglib_assert( !"Method shall not be called for this Process command" );
      break;
  }

  return true;
}


void
MeasureProg_c::setVal( ProcData_c& ac_processData, int32_t ai32_val )
{
  const int32_t i32_time =  System_c::getTime();

  // now check if one subprog triggers
  for (Vec_MeasureSubprogIterator pc_iter = mvec_measureSubprog.begin();
       pc_iter != mvec_measureSubprog.end(); pc_iter++)
  {
    bool triggeredIncrement = false;
    switch (pc_iter->type())
    {
      case IsoAgLib::ProcData::MeasurementCommandTimeProp:
        triggeredIncrement = pc_iter->updateTrigger(i32_time);
        break;

      case IsoAgLib::ProcData::MeasurementCommandDistProp:
        #if defined(USE_BASE) || defined(USE_TRACTOR_MOVE)
        triggeredIncrement = pc_iter->updateTrigger(int32_t(getTracMoveInstance( ac_processData.getMultitonInst() ).distTheor()));
        #endif
        break;

      case IsoAgLib::ProcData::MeasurementCommandOnChange:
        triggeredIncrement = pc_iter->updateTrigger(ai32_val);
        break;

      case IsoAgLib::ProcData::MeasurementCommandMinimumThreshold: break;
      case IsoAgLib::ProcData::MeasurementCommandMaximumThreshold: break;
    }

    // if triggeredIncrement == true the registered values should be sent
    if (triggeredIncrement)
    { // send the registered values
      if (!minMaxLimitsPassed(ai32_val))
      {
        // omit this value send
        continue;
      }
      // if at least one send try had success reset triggeredIncrement
      ac_processData.sendValue( m_ecuType, ai32_val);
    }
  }
}


void
MeasureProg_c::timeEvent( ProcData_c& ac_processData, uint16_t& rui16_nextTimePeriod, int32_t value )
{
  const int32_t i32_time = System_c::getTime();

  for (Vec_MeasureSubprogIterator pc_iter = mvec_measureSubprog.begin(); pc_iter != mvec_measureSubprog.end(); pc_iter++)
  {
    bool triggeredIncrement = false;
    int32_t i32_nextTimePeriod = 0;
    switch (pc_iter->type())
    {
      case IsoAgLib::ProcData::MeasurementCommandTimeProp:
        triggeredIncrement = pc_iter->updateTrigger(i32_time);
        i32_nextTimePeriod = pc_iter->nextTriggerTime(ac_processData, i32_time);
        break;

      case IsoAgLib::ProcData::MeasurementCommandDistProp:
        {
        #if defined(USE_BASE) || defined(USE_TRACTOR_MOVE)
        int32_t i32_distTheor = getTracMoveInstance(ac_processData.getMultitonInst()).distTheor();
        triggeredIncrement = pc_iter->updateTrigger(i32_distTheor);
        #else
        int32_t i32_distTheor = 0;
        #endif
        i32_nextTimePeriod = pc_iter->nextTriggerTime(ac_processData, i32_distTheor);
        }
        break;

      case IsoAgLib::ProcData::MeasurementCommandOnChange:
        triggeredIncrement = pc_iter->updateTrigger(value);
        break;
      default:
        break;
    } // switch

    if( i32_nextTimePeriod )
    {
      if ( (i32_nextTimePeriod > 0) // something valid to set
            // rui16_nextTimePeriod not yet set or i32_nextTimePeriod smaller => set
            && ((0 == rui16_nextTimePeriod) || (i32_nextTimePeriod < rui16_nextTimePeriod))
          )
      {
        rui16_nextTimePeriod = i32_nextTimePeriod;
      }
    }

    // if triggeredIncrement == true the registered values should be sent
    if (triggeredIncrement)
    {
      if (!minMaxLimitsPassed(value))
      {
        // omit this value send
        continue;
      }
      ac_processData.sendValue( m_ecuType, value);
    }
  }
}


bool
MeasureProg_c::minMaxLimitsPassed( int32_t value ) const
{
  // no threshold -> skip
  if (mlist_thresholdInfo.empty())
    return true;

  bool b_checkMin = false;
  bool b_checkMax = false;
  int32_t i32_maxVal = 0;
  int32_t i32_minVal = 0;

  for (List_ThresholdInfoConstIterator ps_iterThreshold = mlist_thresholdInfo.begin(); ps_iterThreshold != mlist_thresholdInfo.end(); ps_iterThreshold++)
  {
    switch (ps_iterThreshold->en_type)
    {
      case IsoAgLib::ProcData::MeasurementCommandMaximumThreshold:
        b_checkMax = true;
        i32_maxVal = ps_iterThreshold->i32_threshold;
        break;
		
      case IsoAgLib::ProcData::MeasurementCommandMinimumThreshold:
        b_checkMin = true;
        i32_minVal = ps_iterThreshold->i32_threshold;
        break;
		
      default: ;
    }
  }

  if ( b_checkMin && b_checkMax && (i32_maxVal < i32_minVal) && ((i32_maxVal >= value) || (i32_minVal <= value)) )
    return true;

  if ( (b_checkMin && i32_minVal > value ) ||
       (b_checkMax && i32_maxVal < value ) )
    return false;

  return true;
}


MeasureSubprog_c&
MeasureProg_c::addSubprog( IsoAgLib::ProcData::measurementCommand_t ren_type, int32_t ai32_increment )
{
  // if subprog with this type exist, update only value
  Vec_MeasureSubprog::iterator pc_subprog = mvec_measureSubprog.end();
  for (pc_subprog = mvec_measureSubprog.begin();
       pc_subprog != mvec_measureSubprog.end(); pc_subprog++)
  {
    if (pc_subprog->type() == ren_type)
    {
      pc_subprog->setIncrement(ai32_increment);
      return *pc_subprog;
    } 
  }

  mvec_measureSubprog.push_front(MeasureSubprog_c(ren_type, ai32_increment));
  return *(mvec_measureSubprog.begin());
}


bool
MeasureProg_c::startMeasurement( ProcData_c& ac_processData, IsoAgLib::ProcData::measurementCommand_t ren_type, int32_t ai32_increment, int32_t value )
{
  bool b_validTriggerMethod = false;

  switch (ren_type)
  {
  case IsoAgLib::ProcData::MeasurementCommandTimeProp: // time proportional
    if ( IsoAgLib::ProcData::isMethodSet(ac_processData.triggerMethod(), IsoAgLib::ProcData::MethodTimeInterval) )
    {
      MeasureSubprog_c& subprog = addSubprog(ren_type, __IsoAgLib::abs(ai32_increment));
      subprog.start(System_c::getTime());
      ac_processData.sendValue( m_ecuType, value);
      b_validTriggerMethod = true;
    }
    break;

  case IsoAgLib::ProcData::MeasurementCommandDistProp: // distance proportional
#if defined(USE_BASE) || defined(USE_TRACTOR_MOVE) // if no distance available, NACK will be sent
    if ( IsoAgLib::ProcData::isMethodSet(ac_processData.triggerMethod(), IsoAgLib::ProcData::MethodDistInterval) )
    {
      MeasureSubprog_c& subprog = addSubprog(ren_type, ai32_increment);
      subprog.start(int32_t(getTracMoveInstance(ac_processData.getMultitonInst()).distTheor()));
      ac_processData.sendValue( m_ecuType, value);
      b_validTriggerMethod = true;
    }
#endif
    break;

  case IsoAgLib::ProcData::MeasurementCommandOnChange: // change threshold proportional
    if ( IsoAgLib::ProcData::isMethodSet(ac_processData.triggerMethod(), IsoAgLib::ProcData::MethodOnChange) )
    {
      MeasureSubprog_c& subprog = addSubprog(ren_type, ai32_increment);
      subprog.start(value);
      ac_processData.sendValue( m_ecuType, value);
      b_validTriggerMethod = true;
    }
    break;

  case IsoAgLib::ProcData::MeasurementCommandMaximumThreshold: // change threshold proportional
    if ( IsoAgLib::ProcData::isMethodSet(ac_processData.triggerMethod(), IsoAgLib::ProcData::MethodThresholdLimit) )
    {
      MeasureSubprog_c& subprog = addSubprog(ren_type, ai32_increment);
      subprog.start();
      const ThresholdInfo_s s_thresholdInfo = {ren_type, subprog.increment()};
      mlist_thresholdInfo.push_front(s_thresholdInfo);
      // Note : do not send value when a threshold is set
      b_validTriggerMethod = true;
    }
    break;

  case IsoAgLib::ProcData::MeasurementCommandMinimumThreshold: // change threshold proportional
    if ( IsoAgLib::ProcData::isMethodSet(ac_processData.triggerMethod(), IsoAgLib::ProcData::MethodThresholdLimit) )
    {
      MeasureSubprog_c& subprog = addSubprog(ren_type, ai32_increment);
      subprog.start();
      const ThresholdInfo_s s_thresholdInfo = {ren_type, subprog.increment()};
      mlist_thresholdInfo.push_front(s_thresholdInfo);
      // Note : do not send value when a threshold is set
      b_validTriggerMethod = true;
    }
    break;

  default:
    isoaglib_assert(!"method should not be called with this command");
    break;
  }

  // set the timer period for process_c to a low value (maybe the new programm triggers soon)
  if (b_validTriggerMethod)
    getTcClientInstance( ac_processData.getMultitonInst() ).resetTimerPeriod();

  return b_validTriggerMethod;
}


void
MeasureProg_c::stopMeasurement()
{
  mvec_measureSubprog.clear();
  mlist_thresholdInfo.clear();
}

}
