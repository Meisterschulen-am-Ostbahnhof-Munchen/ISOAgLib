/*
  measureproglocal_c.cpp: object for managing local

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#include "measureproglocal_c.h"
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/process_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/processdatachangehandler_c.h>
#include <IsoAgLib/comm/Part7_ApplicationLayer/impl/tracmove_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/procdatalocal_c.h>


namespace __IsoAgLib {

MeasureProgLocal_c::MeasureProgLocal_c(IsoName_c::ecuType_t ecutype)
  : ClientBase(),
    mi32_val(0),
    mlist_thresholdInfo(),
    mvec_measureSubprog(),
    m_ecuType(ecutype)
{}

bool MeasureProgLocal_c::processMsg( ProcDataLocal_c& ac_processData, const ProcessPkg_c& arc_data )
{
  ProcessPkg_c::CommandType_t en_command = arc_data.men_command;

  isoaglib_assert(arc_data.getMonitorItemForSA() != NULL); // should have been filtered earlier !

  switch (en_command)
  {
    case ProcessPkg_c::measurementDistanceValueStart:
    case ProcessPkg_c::measurementTimeValueStart:
    case ProcessPkg_c::measurementChangeThresholdValueStart:
    case ProcessPkg_c::measurementMinimumThresholdValueStart:
    case ProcessPkg_c::measurementMaximumThresholdValueStart:
      // measurementCommand_t and CommandType_t are unified for all measurement types
      processMeasurementMsg( ac_processData, arc_data, static_cast<Proc_c::measurementCommand_t>(en_command));
      break;
    case ProcessPkg_c::requestValue:
      ac_processData.sendValue( m_ecuType, val());

      if ( (Proc_c::defaultDataLoggingDDI == arc_data.mui16_DDI) 
        && ( ac_processData.getProcessDataChangeHandler() != NULL ) )
      {
         // call handler function if handler class is registered
          ac_processData.getProcessDataChangeHandler()->processDefaultLoggingStart(
              arc_data.getMonitorItemForSA()->isoName().toConstIisoName_c() );
      }
      break;
    default:
      isoaglib_assert( !"Method shall not be called for this Process command" );
      break;
  }

  return true;
}

void MeasureProgLocal_c::setVal(ProcDataLocal_c& ac_processData, int32_t ai32_val)
{
  const int32_t i32_time =  System_c::getTime();

  // update values
  mi32_val = ai32_val;

  // now check if one subprog triggers
  for (Vec_MeasureSubprogIterator pc_iter = mvec_measureSubprog.begin();
       pc_iter != mvec_measureSubprog.end(); pc_iter++)
  {
    bool triggeredIncrement = false;
    switch (pc_iter->type())
    {
      case Proc_c::MeasurementCommandTimeProp:
        triggeredIncrement = pc_iter->updateTrigger(i32_time);
        break;
      case Proc_c::MeasurementCommandDistProp:
        #if defined(USE_BASE) || defined(USE_TRACTOR_MOVE)
        triggeredIncrement = pc_iter->updateTrigger(int32_t(getTracMoveInstance4Comm().distTheor()));
        #endif
        break;
      case Proc_c::MeasurementCommandOnChange:
        triggeredIncrement = pc_iter->updateTrigger(val());
        break;
      case Proc_c::MeasurementCommandMinimumThreshold: break; // just to make compiler happy
      case Proc_c::MeasurementCommandMaximumThreshold: break; // just to make compiler happy
    } // switch

    // if triggeredIncrement == true the registered values should be sent
    if (triggeredIncrement)
    { // send the registered values
      if (!minMaxLimitsPassed())
      {
        // omit this value send
        continue;
      }
      // if at least one send try had success reset triggeredIncrement
      ac_processData.sendValue( m_ecuType, val());
    }
  } // for
}

void MeasureProgLocal_c::timeEvent( ProcDataLocal_c& ac_processData, uint16_t& rui16_nextTimePeriod )
{
  const int32_t i32_time = Scheduler_Task_c::getLastRetriggerTime();

  for (Vec_MeasureSubprogIterator pc_iter = mvec_measureSubprog.begin(); pc_iter != mvec_measureSubprog.end(); pc_iter++)
  {
    bool triggeredIncrement = false;
    int32_t i32_nextTimePeriod = 0;
    switch (pc_iter->type())
    {
      case Proc_c::MeasurementCommandTimeProp:
        triggeredIncrement = pc_iter->updateTrigger(i32_time);
        i32_nextTimePeriod = pc_iter->nextTriggerTime(i32_time);
        break;
      case Proc_c::MeasurementCommandDistProp:
        {
        #if defined(USE_BASE) || defined(USE_TRACTOR_MOVE)
        int32_t i32_distTheor = getTracMoveInstance4Comm().distTheor();
        triggeredIncrement = pc_iter->updateTrigger(i32_distTheor);
        #else
        int32_t i32_distTheor = 0;
        #endif
        i32_nextTimePeriod = pc_iter->nextTriggerTime(i32_distTheor);
        }
        break;
      case Proc_c::MeasurementCommandOnChange:
        triggeredIncrement = pc_iter->updateTrigger(val());
        break;
      default:
        break;
    } // switch

    if (i32_nextTimePeriod )
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
      if (!minMaxLimitsPassed())
      {
        // omit this value send
        continue;
      }
      ac_processData.sendValue( m_ecuType, val());
    }
  }
}

bool MeasureProgLocal_c::minMaxLimitsPassed() const
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
      case Proc_c::MeasurementCommandMaximumThreshold:
        b_checkMax = true;
        i32_maxVal = ps_iterThreshold->i32_threshold;
        break;
      case Proc_c::MeasurementCommandMinimumThreshold:
        b_checkMin = true;
        i32_minVal = ps_iterThreshold->i32_threshold;
        break;
      default: ;
    }
  }

  if ( b_checkMin && b_checkMax && (i32_maxVal < i32_minVal) && ((i32_maxVal >= val()) || (i32_minVal <= val())) )
    return true;

  if ( (b_checkMin && i32_minVal > val() ) ||
       (b_checkMax && i32_maxVal < val() ) )
    return false;

  return true;
}

MeasureSubprog_c& MeasureProgLocal_c::addSubprog(Proc_c::measurementCommand_t ren_type, int32_t ai32_increment)
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

  mvec_measureSubprog.push_front(MeasureSubprog_c(ren_type, ai32_increment MULTITON_INST_WITH_COMMA));
  return *(mvec_measureSubprog.begin());
}

bool MeasureProgLocal_c::startMeasurement(ProcDataLocal_c& ac_processData, Proc_c::measurementCommand_t ren_type, int32_t ai32_increment)
{
  bool b_validTriggerMethod = false;

  switch (ren_type)
  {
  case Proc_c::MeasurementCommandTimeProp: // time proportional
    if ( Proc_c::isMethodSet(ac_processData.triggerMethod(), Proc_c::MethodTimeInterval) )
    {
      MeasureSubprog_c& subprog = addSubprog(ren_type, __IsoAgLib::abs(ai32_increment));
      subprog.start(System_c::getTime());
      ac_processData.sendValue( m_ecuType, val());
      b_validTriggerMethod = true;
    }
    break;
  case Proc_c::MeasurementCommandDistProp: // distance proportional
#if defined(USE_BASE) || defined(USE_TRACTOR_MOVE) // if no distance available, NACK will be sent
    if ( Proc_c::isMethodSet(ac_processData.triggerMethod(), Proc_c::MethodDistInterval) )
    {
      MeasureSubprog_c& subprog = addSubprog(ren_type, ai32_increment);
      subprog.start(int32_t(getTracMoveInstance4Comm().distTheor()));
      ac_processData.sendValue( m_ecuType, val());
      b_validTriggerMethod = true;
    }
#endif
    break;
  case Proc_c::MeasurementCommandOnChange: // change threshold proportional
    if ( Proc_c::isMethodSet(ac_processData.triggerMethod(), Proc_c::MethodOnChange) )
    {
      MeasureSubprog_c& subprog = addSubprog(ren_type, ai32_increment);
      subprog.start(val());
      ac_processData.sendValue( m_ecuType, val());
      b_validTriggerMethod = true;
    }
    break;
  case Proc_c::MeasurementCommandMaximumThreshold: // change threshold proportional
    if ( Proc_c::isMethodSet(ac_processData.triggerMethod(), Proc_c::MethodThresholdLimit) )
    {
      MeasureSubprog_c& subprog = addSubprog(ren_type, ai32_increment);
      subprog.start();
      const ThresholdInfo_s s_thresholdInfo = {ren_type, subprog.increment()};
      mlist_thresholdInfo.push_front(s_thresholdInfo);
      // Note : do not send value when a threshold is set
      b_validTriggerMethod = true;
    }
    break;
  case Proc_c::MeasurementCommandMinimumThreshold: // change threshold proportional
    if ( Proc_c::isMethodSet(ac_processData.triggerMethod(), Proc_c::MethodThresholdLimit) )
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
    getProcessInstance4Comm().resetTimerPeriod();

  return b_validTriggerMethod;
}

void MeasureProgLocal_c::stopMeasurement(ProcDataLocal_c& ac_processData)
{
  mvec_measureSubprog.clear();
  mlist_thresholdInfo.clear();
}

void MeasureProgLocal_c::processMeasurementMsg(ProcDataLocal_c& ac_processData, const ProcessPkg_c& pkg, Proc_c::measurementCommand_t ren_type)
{
  isoaglib_assert(pkg.getMonitorItemForSA() != NULL); // already tested before in Process_c::processMsg

  if (!startMeasurement(ac_processData, ren_type, pkg.mi32_pdValue))
  {
    getProcessInstance4Comm().sendNack( pkg.getMonitorItemForSA()->isoName(),
                                        ac_processData.isoName(),
                                        ac_processData.DDI(),
                                        ac_processData.element(),
                                        Proc_c::NackTriggerMethodNotSupported);
  }
}

} // end of namespace __IsoAgLib
