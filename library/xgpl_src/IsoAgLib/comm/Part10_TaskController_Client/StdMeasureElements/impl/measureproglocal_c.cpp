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
    men_doSend(Proc_c::DoNone),
    m_ecuType(ecutype)
{}

void MeasureProgLocal_c::start(ProcDataLocal_c& ac_processData, Proc_c::measurementCommand_t ren_type){

  // call start function of base class
  men_doSend = Proc_c::DoVal;

  //mi32_lastMasterVal = ai32_masterVal;
  bool b_sendVal = true;

  // start the given subprog items
  for (Vec_MeasureSubprogIterator pc_iter = mvec_measureSubprog.begin(); pc_iter != mvec_measureSubprog.end(); pc_iter++)
  {
    switch (pc_iter->type() & ren_type)
    {
      case Proc_c::MeasurementCommandTimeProp:
        pc_iter->start(System_c::getTime());
        break;
      case Proc_c::MeasurementCommandDistProp:
      #if defined(USE_BASE) || defined(USE_TRACTOR_MOVE)
        pc_iter->start(int32_t(getTracMoveInstance4Comm().distTheor()));
      #endif
        break;
      case Proc_c::MeasurementCommandMaximumThreshold:
      {
        pc_iter->start();
        b_sendVal = false; // do not send value when a threshold is set
        const ThresholdInfo_s s_thresholdInfo = {ren_type, pc_iter->increment()};
        mlist_thresholdInfo.push_front(s_thresholdInfo);
        break;
      }
      case Proc_c::MeasurementCommandMinimumThreshold:
      {
        pc_iter->start();
        b_sendVal = false;  // do not send value when a threshold is set
        const ThresholdInfo_s s_thresholdInfo = {ren_type, pc_iter->increment()};
        mlist_thresholdInfo.push_front(s_thresholdInfo);
        break;
      }
      case Proc_c::MeasurementCommandOnChange:
        pc_iter->start(val());
        break;
    } // switch
  } // for

  // send first values: if now without success mark for later resend with true
  if (b_sendVal)
    sendRegisteredVals(ac_processData);

  // set the timer period for process_c to a low value (maybe the new programm triggers soon)
  getProcessInstance4Comm().resetTimerPeriod();
}

void MeasureProgLocal_c::stop(ProcDataLocal_c& ac_processData)
{
  mvec_measureSubprog.clear();
  mlist_thresholdInfo.clear();
  men_doSend = Proc_c::DoNone;
}

void MeasureProgLocal_c::sendVal( ProcDataLocal_c& ac_processData, const IsoName_c& ac_targetISOName) const
{
  ProcessPkg_c pkg;
  // prepare general command in process pkg
  pkg.men_command = ProcessPkg_c::setValue;
  ac_processData.sendValISOName( pkg, ac_targetISOName, val());
}

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
      // increment
      processIncrementMsg( ac_processData, arc_data);
      // measurementCommand_t and CommandType_t are unified for all measurement types
      start(ac_processData, static_cast<Proc_c::measurementCommand_t>(en_command));
      break;
    case ProcessPkg_c::requestValue:
      sendVal( ac_processData, arc_data.getMonitorItemForSA()->isoName() );

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

void MeasureProgLocal_c::setVal(ProcDataLocal_c& ac_processData, int32_t ai32_val){
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
      sendRegisteredVals(ac_processData);
    }
  } // for
}

void MeasureProgLocal_c::sendRegisteredVals(ProcDataLocal_c& ac_processData){
  // normal measurement (no measurement on setpoint DDI)
  sendVal( ac_processData, getProcessInstance4Comm().getISONameFromType( m_ecuType ) );
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
        // calculate next timer period
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
        // calculate next timer period
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
            // *pui16_nextTimePeriod not yet set or i32_nextTimePeriod smaller => set
            && ((0 == rui16_nextTimePeriod) || (i32_nextTimePeriod < rui16_nextTimePeriod))
          )
      {
        rui16_nextTimePeriod = i32_nextTimePeriod;
      }
    }

    // if triggeredIncrement == true the registered values should be sent
    // if needed an old unsuccessfull send try is redone (deactivated!)
    if (triggeredIncrement)
    {
      if (!minMaxLimitsPassed())
      {
        // omit this value send
        continue;
      }
      sendRegisteredVals(ac_processData);
    }
  }
}

bool MeasureProgLocal_c::minMaxLimitsPassed() const
{
  // no threshold -> skip
  if (mlist_thresholdInfo.empty()) return true;

  bool b_checkMin = false;
  bool b_checkMax = false;
  int32_t i32_maxVal = 0;
  int32_t i32_minVal = 0;

  for (List_ThresholdInfoConstIterator ps_iterThreshold = mlist_thresholdInfo.begin(); ps_iterThreshold != mlist_thresholdInfo.end(); ps_iterThreshold++)
  {
    switch (ps_iterThreshold->en_type)
    {
      case Proc_c::MeasurementCommandMaximumThreshold: b_checkMax = true; i32_maxVal = ps_iterThreshold->i32_threshold; break;
      case Proc_c::MeasurementCommandMinimumThreshold: b_checkMin = true; i32_minVal = ps_iterThreshold->i32_threshold; break;
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

void MeasureProgLocal_c::addSubprog(Proc_c::measurementCommand_t ren_type, int32_t ai32_increment){

  // if subprog with this type exist, update only value
  Vec_MeasureSubprog::iterator pc_subprog = mvec_measureSubprog.end();
  for (pc_subprog = mvec_measureSubprog.begin();
       pc_subprog != mvec_measureSubprog.end(); pc_subprog++)
  {
     if (pc_subprog->type() == ren_type)
       break;
  }

  if (pc_subprog != mvec_measureSubprog.end())
  { // subprog with same type found -> update val
    pc_subprog->setIncrement(ai32_increment);
  }
  else
  { // no subprog with same type exist -> insert new one
    mvec_measureSubprog.push_front(MeasureSubprog_c(ren_type, ai32_increment MULTITON_INST_WITH_COMMA));
  }
}

void MeasureProgLocal_c::processIncrementMsg(ProcDataLocal_c& ac_processData, const ProcessPkg_c& pkg)
{
  isoaglib_assert(pkg.getMonitorItemForSA() != NULL); // already tested before in Process_c::processMsg

  const ProcessPkg_c::CommandType_t en_command = pkg.men_command;

  bool b_validTriggerMethod = false;
  const int32_t ci32_val = pkg.mi32_pdValue;

  switch (en_command)
  {
  case ProcessPkg_c::measurementTimeValueStart: // time proportional
    if ( Proc_c::isMethodSet(ac_processData.triggerMethod(), Proc_c::MethodTimeInterval) )
    {
      addSubprog(Proc_c::MeasurementCommandTimeProp, __IsoAgLib::abs(ci32_val));
      b_validTriggerMethod = true;
    }
    break;
  case ProcessPkg_c::measurementDistanceValueStart: // distance proportional
    if ( Proc_c::isMethodSet(ac_processData.triggerMethod(), Proc_c::MethodDistInterval) )
    {
      addSubprog(Proc_c::MeasurementCommandDistProp, ci32_val);
      b_validTriggerMethod = true;
    }
    break;
  case ProcessPkg_c::measurementChangeThresholdValueStart: // change threshold proportional
    if ( Proc_c::isMethodSet(ac_processData.triggerMethod(), Proc_c::MethodOnChange) )
    {
      addSubprog(Proc_c::MeasurementCommandOnChange, ci32_val);
      b_validTriggerMethod = true;
    }
    break;
  case ProcessPkg_c::measurementMaximumThresholdValueStart: // change threshold proportional
    if ( Proc_c::isMethodSet(ac_processData.triggerMethod(), Proc_c::MethodThresholdLimit) )
    {
      addSubprog(Proc_c::MeasurementCommandMaximumThreshold, ci32_val);
      b_validTriggerMethod = true;
    }
    break;
  case ProcessPkg_c::measurementMinimumThresholdValueStart: // change threshold proportional
    if ( Proc_c::isMethodSet(ac_processData.triggerMethod(), Proc_c::MethodThresholdLimit) )
    {
      addSubprog(Proc_c::MeasurementCommandMinimumThreshold, ci32_val);
      b_validTriggerMethod = true;
    }
    break;
  default:
    isoaglib_assert(!"method should not be called with this command");
    break;
  }

  if (!b_validTriggerMethod)
    getProcessInstance4Comm().sendNack( pkg.getMonitorItemForSA()->isoName(),
                                        ac_processData.isoName(),
                                        ac_processData.DDI(),
                                        ac_processData.element(),
                                        Proc_c::NackTriggerMethodNotSupported);
}

} // end of namespace __IsoAgLib
