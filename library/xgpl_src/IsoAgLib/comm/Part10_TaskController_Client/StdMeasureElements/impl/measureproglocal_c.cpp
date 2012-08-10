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

MeasureProgLocal_c::MeasureProgLocal_c()
  : ClientBase(),
    mi32_val(0),
    mlist_thresholdInfo(),
    mvec_measureSubprog(),
    men_doSend(Proc_c::DoNone),
    mb_active(false),
    m_ecuType(IsoName_c::ecuTypeANYOTHER)
{}

void MeasureProgLocal_c::start(ProcDataLocal_c& ac_processData, Proc_c::measurementCommand_t ren_type,
                        Proc_c::doSend_t ren_doSend){
  // call start function of base class
  men_doSend = Proc_c::DoVal;

  //mi32_lastMasterVal = ai32_masterVal;
  bool b_sendVal = true;

  // start the given subprog items
  for (Vec_MeasureSubprogIterator pc_iter = mvec_measureSubprog.begin(); pc_iter != mvec_measureSubprog.end(); pc_iter++)
  {
    if (pc_iter->doSend() != ren_doSend)
      //MeasureSubprog_c not matching!
      continue;

    switch (pc_iter->type() & ren_type)
    {
      case Proc_c::MeasurementCommandTimeProp:
        pc_iter->start(System_c::getTime());
        break;
      #if defined(USE_BASE) || defined(USE_TRACTOR_MOVE)
      case Proc_c::DistProp:
        pc_iter->start(int32_t(getTracMoveInstance4Comm().distTheor()));
        break;
      #endif
      case Proc_c::MeasurementCommandMaximumThreshold:
      {
        pc_iter->start();
        b_sendVal = false; // do not send value when a threshold is set
        const ThresholdInfo_s s_thresholdInfo = {ren_type, ren_doSend, pc_iter->increment(), true};
        mlist_thresholdInfo.push_front(s_thresholdInfo);
        break;
      }
      case Proc_c::MeasurementCommandMinimumThreshold:
      {
        pc_iter->start();
        b_sendVal = false;  // do not send value when a threshold is set
        const ThresholdInfo_s s_thresholdInfo = {ren_type, ren_doSend, pc_iter->increment(), false};
        mlist_thresholdInfo.push_front(s_thresholdInfo);
        break;
      }
      case Proc_c::MeasurementCommandOnChange:
        pc_iter->start(val());
        break;
      case Proc_c::MeasurementCommandNullType: break; // just to make compiler happy
    } // switch
  } // for

  // send first values: if now without success mark for later resend with true
  if (b_sendVal)
    sendRegisteredVals(ac_processData, ren_doSend);

  // set the timer period for process_c to a low value (maybe the new programm triggers soon)
  getProcessInstance4Comm().resetTimerPeriod();
}

void MeasureProgLocal_c::stop(ProcDataLocal_c& ac_processData, Proc_c::measurementCommand_t ren_type, Proc_c::doSend_t ren_doSend){
  // send the registered values
  if ((Proc_c::MeasurementCommandMinimumThreshold != ren_type) && (Proc_c::MeasurementCommandMaximumThreshold != ren_type)
      && (Proc_c::MeasurementCommandNullType != ren_type) ) // do not send values for threshold resets and NullType
  {
    sendRegisteredVals(ac_processData);
  }

  if (Proc_c::MeasurementCommandNullType == ren_type)
  {
    mvec_measureSubprog.clear();
    men_doSend = Proc_c::DoNone;
  }
  else
  { // ISO: clear only the appropriate MeasureSubprog from the sub prog list
    for (Vec_MeasureSubprogIterator pc_iter = mvec_measureSubprog.begin(); pc_iter != mvec_measureSubprog.end();)
    {
      if ((pc_iter->type() == ren_type) && (pc_iter->doSend() == ren_doSend))
      {
        pc_iter = mvec_measureSubprog.erase(pc_iter);
      }
      else
        pc_iter++;
    }
  }

  // cleanup corresponding threshold info
  if ((Proc_c::MeasurementCommandMaximumThreshold & ren_type) || (Proc_c::MeasurementCommandMinimumThreshold & ren_type))
  { // search corresponding threshold info
    for (List_ThresholdInfoIterator ps_iterThreshold = mlist_thresholdInfo.begin(); ps_iterThreshold != mlist_thresholdInfo.end();)
    {
      if ((ps_iterThreshold->en_type == ren_type) && (ps_iterThreshold->en_doSend == ren_doSend))
        ps_iterThreshold = mlist_thresholdInfo.erase(ps_iterThreshold);
      else
        ps_iterThreshold++;
    }
  }
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

  isoaglib_assert( en_command != ProcessPkg_c::setValue );
  isoaglib_assert(arc_data.getMonitorItemForSA() != NULL); // should have been filtered earlier !

  // --- Checking of "Proc_c::defaultDataLoggingDDI != pkg.DDI()" is a workaround for bad TCs ---
  if ( Proc_c::defaultDataLoggingDDI == arc_data.mui16_DDI )
  {
    // backup sender isoname before answering (modification during send in c_pkg !)
    const IsoName_c c_senderIsoNameOrig = arc_data.getMonitorItemForSA()->isoName();

    const int32_t ci32_val = arc_data.mi32_pdValue;
    const uint32_t cui32_ddi = arc_data.mui16_DDI;

    sendVal( ac_processData, c_senderIsoNameOrig );

    if ( ac_processData.getProcessDataChangeHandler() != NULL )
      // call handler function if handler class is registered
      ac_processData.getProcessDataChangeHandler()->processDefaultLoggingStart( &ac_processData, cui32_ddi, ci32_val, c_senderIsoNameOrig.toConstIisoName_c() );
    
    return true;
  }

  // --- Checking of "Proc_c::defaultDataLoggingDDI != pkg.DDI()" is a workaround for bad TCs ---
  if (en_command != ProcessPkg_c::requestValue)
  {
    // ISO, local
    if (en_command == ProcessPkg_c::measurementDistanceValueStart ||
        en_command == ProcessPkg_c::measurementTimeValueStart ||
        en_command == ProcessPkg_c::measurementChangeThresholdValueStart ||
        en_command == ProcessPkg_c::measurementMinimumThresholdValueStart ||
        en_command == ProcessPkg_c::measurementMaximumThresholdValueStart)
    {
      // increment
      processIncrementMsg( ac_processData, arc_data);

      Proc_c::measurementCommand_t en_typePkg = Proc_c::MeasurementCommandNullType;
      int32_t i32_dataLong = arc_data.mi32_pdValue;
      // @TODO remove scgwitch and / or unify enums
      switch (en_command) {
        case ProcessPkg_c::measurementTimeValueStart:
          en_typePkg = Proc_c::MeasurementCommandTimeProp;
          break;
        case ProcessPkg_c::measurementDistanceValueStart:
          en_typePkg = Proc_c::MeasurementCommandDistProp;
          break;
        case ProcessPkg_c::measurementChangeThresholdValueStart:
          en_typePkg = Proc_c::MeasurementCommandOnChange;
          break;
        case ProcessPkg_c::measurementMaximumThresholdValueStart:
          en_typePkg = Proc_c::MeasurementCommandMaximumThreshold;
          break;
        case ProcessPkg_c::measurementMinimumThresholdValueStart:
          en_typePkg = Proc_c::MeasurementCommandMinimumThreshold;
          break;
        default: ;
      }

      start(ac_processData, en_typePkg, Proc_c::DoVal);
    }
  }
  else  // request or default data logging
  {
    // backup sender isoname before answering (modification during send in c_pkg !)
    const IsoName_c c_senderIsoNameOrig = arc_data.getMonitorItemForSA()->isoName();
    sendVal( ac_processData, c_senderIsoNameOrig );
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
      case Proc_c::MeasurementCommandNullType: break; // just to make compiler happy
      default: ;
    } // switch

    // if triggeredIncrement == true the registered values should be sent
    if (triggeredIncrement)
    { // send the registered values
      if (!minMaxLimitsPassed(pc_iter->doSend()))
      {
        // omit this value send
        continue;
      }
      // if at least one send try had success reset triggeredIncrement
      sendRegisteredVals(ac_processData);
    }
  } // for
}

void MeasureProgLocal_c::sendRegisteredVals(ProcDataLocal_c& ac_processData, Proc_c::doSend_t ren_doSend){
  // normal measurement (no measurement on setpoint DDI)
  if (Proc_c::DoVal == ren_doSend)
    sendVal( ac_processData, getProcessInstance4Comm().getISONameFromType( m_ecuType ) );
}

void MeasureProgLocal_c::timeEvent( ProcDataLocal_c& ac_processData, uint16_t& rui16_nextTimePeriod )
{
  const int32_t i32_time = Scheduler_Task_c::getLastRetriggerTime();

  int32_t i32_nextTimePeriod;
  int32_t i32_distTheor;

  for (Vec_MeasureSubprogIterator pc_iter = mvec_measureSubprog.begin(); pc_iter != mvec_measureSubprog.end(); pc_iter++)
  {
    bool triggeredIncrement = false;
    i32_nextTimePeriod = 0;
    switch (pc_iter->type())
    {
      case Proc_c::MeasurementCommandTimeProp:
        triggeredIncrement = pc_iter->updateTrigger(i32_time);
        // calculate next timer period
        i32_nextTimePeriod = pc_iter->nextTriggerTime(i32_time);
        break;
      case Proc_c::MeasurementCommandDistProp:
        #if defined(USE_BASE) || defined(USE_TRACTOR_MOVE)
        i32_distTheor = getTracMoveInstance4Comm().distTheor();
        triggeredIncrement = pc_iter->updateTrigger(i32_distTheor);
        #else
        i32_distTheor = 0;
        #endif
        // calculate next timer period
        i32_nextTimePeriod = pc_iter->nextTriggerTime(i32_distTheor);
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
      if (!minMaxLimitsPassed(pc_iter->doSend()))
      {
        // omit this value send
        continue;
      }
      sendRegisteredVals(ac_processData, pc_iter->doSend());
    }
  }
}

bool MeasureProgLocal_c::minMaxLimitsPassed(Proc_c::doSend_t ren_doSend) const
{
  bool b_checkMin = false;
  bool b_checkMax = false;
  int32_t i32_maxVal = 0;
  int32_t i32_minVal = 0;

  for (List_ThresholdInfoConstIterator ps_iterThreshold = mlist_thresholdInfo.begin(); ps_iterThreshold != mlist_thresholdInfo.end(); ps_iterThreshold++)
  {
    if (ps_iterThreshold->en_doSend == ren_doSend)
    {
      switch (ps_iterThreshold->en_type)
      {
        case Proc_c::MeasurementCommandMaximumThreshold: b_checkMax = true; i32_maxVal = ps_iterThreshold->i32_threshold; break;
        case Proc_c::MeasurementCommandMinimumThreshold: b_checkMin = true; i32_minVal = ps_iterThreshold->i32_threshold; break;
        default: ;
      }
    }
  }

  if ( b_checkMin && b_checkMax && (i32_maxVal < i32_minVal) && ((i32_maxVal >= val()) || (i32_minVal <= val())) )
    return true;

  if ( (b_checkMin && i32_minVal > val() ) ||
       (b_checkMax && i32_maxVal < val() ) )
    return false;

  return true;
}

void MeasureProgLocal_c::addSubprog(Proc_c::measurementCommand_t ren_type, int32_t ai32_increment, Proc_c::doSend_t ren_doSend){

  // if subprog with this type exist, update only value
  Vec_MeasureSubprog::iterator pc_subprog = mvec_measureSubprog.end();
  for (pc_subprog = mvec_measureSubprog.begin();
       pc_subprog != mvec_measureSubprog.end(); pc_subprog++)
  {
     if ((pc_subprog->type() == ren_type) && (pc_subprog->doSend() == ren_doSend)) break;
  }

  if (pc_subprog != mvec_measureSubprog.end())
  { // subprog with same type found -> update val
    pc_subprog->setIncrement(ai32_increment);
  }
  else
  { // no subprog with same type exist -> insert new one
    mvec_measureSubprog.push_front(MeasureSubprog_c(ren_type, ren_doSend, ai32_increment MULTITON_INST_WITH_COMMA));
  }
}

void MeasureProgLocal_c::processIncrementMsg( ProcDataLocal_c& ac_processData, const ProcessPkg_c& pkg)
{
  Proc_c::doSend_t ren_doSend = Proc_c::DoVal;  //default send data mode

  // @TODO check if nessessary to test this here. Maybe checked already in Process
  if (pkg.getMonitorItemForSA() == NULL)
  { // don't care for packets from SA 0xFE
    return;
  }

  const ProcessPkg_c::CommandType_t en_command = pkg.men_command;

  // set mc_isoName to caller of prog
  m_ecuType = getProcessInstance4Comm().getTypeFromISOName( pkg.getMonitorItemForSA()->isoName() );

  bool b_wrongTriggerMethod = false;
  const int32_t ci32_val = pkg.mi32_pdValue;

  if ( en_command == ProcessPkg_c::measurementTimeValueStart )
  { // time proportional
    if ( Proc_c::isMethodSet(ac_processData.triggerMethod(), Proc_c::MethodTimeInterval) )
      addSubprog(Proc_c::MeasurementCommandTimeProp, __IsoAgLib::abs(ci32_val), ren_doSend);
    else
      b_wrongTriggerMethod = true;
  }
  else if ( en_command == ProcessPkg_c::measurementDistanceValueStart )
  {    // distance proportional
    if ( Proc_c::isMethodSet(ac_processData.triggerMethod(), Proc_c::MethodDistInterval) )
      addSubprog(Proc_c::MeasurementCommandDistProp, ci32_val, ren_doSend);
    else
      b_wrongTriggerMethod = true;
  }
  else if ( en_command == ProcessPkg_c::measurementChangeThresholdValueStart )
  {
    // change threshold proportional
    if ( Proc_c::isMethodSet(ac_processData.triggerMethod(), Proc_c::MethodOnChange) )
      addSubprog(Proc_c::MeasurementCommandOnChange, ci32_val, ren_doSend);
    else
      b_wrongTriggerMethod = true;
  }
  else if ( en_command == ProcessPkg_c::measurementMaximumThresholdValueStart )
  {
    // change threshold proportional
    if ( Proc_c::isMethodSet(ac_processData.triggerMethod(), Proc_c::MethodThresholdLimit) )
      addSubprog(Proc_c::MeasurementCommandMaximumThreshold, ci32_val, ren_doSend);
    else
      b_wrongTriggerMethod = true;
  }
  else if ( en_command == ProcessPkg_c::measurementMinimumThresholdValueStart )
  {
    // change threshold proportional
    if ( Proc_c::isMethodSet(ac_processData.triggerMethod(), Proc_c::MethodThresholdLimit) )
      addSubprog(Proc_c::MeasurementCommandMinimumThreshold, ci32_val, ren_doSend);
    else
      b_wrongTriggerMethod = true;
  }

  if (b_wrongTriggerMethod)
    getProcessInstance4Comm().sendNack( pkg.getMonitorItemForSA()->isoName(),
                                        ac_processData.isoName(),
                                        ac_processData.DDI(),
                                        ac_processData.element(),
                                        0x08); // Bit 3 = 1 Trigger method not supported // @TODO avoid magic number.
}

} // end of namespace __IsoAgLib
