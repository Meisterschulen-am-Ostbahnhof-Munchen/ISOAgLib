/***************************************************************************
                          measureproglocal_c.cpp - object for managing local

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include "measureproglocal_c.h"
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/process_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/processdatachangehandler_c.h>
#include <IsoAgLib/comm/Part7_ApplicationLayer/impl/tracmove_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/procdatalocal_c.h>


namespace __IsoAgLib {

MeasureProgLocal_c::MeasureProgLocal_c()
  : ClientBase(),
    mvec_measureSubprog(),
    m_ecuType(IsoName_c::ecuTypeANYOTHER)
{
  init( 0, 0 );
}

void MeasureProgLocal_c::init(
  int32_t ai32_masterVal,
  int32_t ai32_initialVal)
{
  mvec_measureSubprog.clear();
  // set the pointers in the baseClass ProcessElementBase
  //set(&ac_processData);
  // store the parameter init vals
  m_ecuType = IsoName_c::ecuTypeANYOTHER;
  //mc_isoName = ac_callerISOName;
  mi32_val = ai32_masterVal;
  mb_active = false;

  // set the rest of element vals to defined init
  men_doSend = Proc_c::DoNone;
  //men_type = Proc_c::DistProp;

  mlist_thresholdInfo.clear();
}

#if 0 // no copy constructor and assign operator for now
const MeasureProgLocal_c& MeasureProgLocal_c::operator=(const MeasureProgLocal_c& acrc_src){
  // call base class operator
  ProcessElementBase_c::operator=(acrc_src);

  assignFromSource( acrc_src );

  // return reference to source
  return acrc_src;
}

MeasureProgLocal_c::MeasureProgLocal_c(const MeasureProgLocal_c& acrc_src)
   : ProcessElementBase_c(acrc_src){
  assignFromSource( acrc_src );
}

void MeasureProgLocal_c::assignFromSource( const MeasureProgLocal_c& acrc_src )
{ // copy element vars
  mc_isoName = acrc_src.mc_isoName;
  men_doSend = acrc_src.men_doSend;
  mb_active = acrc_src.mb_active;
  mi32_val = acrc_src.mi32_val;
  mvec_measureSubprog = acrc_src.mvec_measureSubprog;
}
#endif

MeasureProgLocal_c::~MeasureProgLocal_c(){
}

void MeasureProgLocal_c::start(ProcDataLocal_c& ac_processData, Proc_c::type_t ren_type,
                        Proc_c::doSend_t ren_doSend){
  // call start function of base class
  men_doSend = (ren_doSend != Proc_c::DoNone)?ren_doSend : men_doSend;
  if (men_doSend == Proc_c::DoNone) men_doSend = Proc_c::DoVal;

  //mi32_lastMasterVal = ai32_masterVal;
  bool b_sendVal = TRUE;

  // start the given subprog items
  for (Vec_MeasureSubprogIterator pc_iter = mvec_measureSubprog.begin(); pc_iter != mvec_measureSubprog.end(); pc_iter++)
  {
    if (pc_iter->doSend() != ren_doSend)
      //MeasureSubprog_c not matching!
      continue;

    switch (pc_iter->type() & ren_type)
    {
      case Proc_c::TimeProp:
        pc_iter->start(System_c::getTime());
        break;
      #if defined(USE_BASE) || defined(USE_TRACTOR_MOVE)
      case Proc_c::DistProp:
        pc_iter->start(int32_t(getTracMoveInstance4Comm().distTheor()));
        break;
      #endif
      case Proc_c::MaximumThreshold:
      {
        pc_iter->start();
        b_sendVal = false; // do not send value when a threshold is set
        const ThresholdInfo_s s_thresholdInfo = {ren_type, ren_doSend, pc_iter->increment(), true};
        mlist_thresholdInfo.push_front(s_thresholdInfo);
        break;
      }
      case Proc_c::MinimumThreshold:
      {
        pc_iter->start();
        b_sendVal = false;  // do not send value when a threshold is set
        const ThresholdInfo_s s_thresholdInfo = {ren_type, ren_doSend, pc_iter->increment(), false};
        mlist_thresholdInfo.push_front(s_thresholdInfo);
        break;
      }
      case Proc_c::OnChange:
        pc_iter->start(val());
        break;
      case Proc_c::NullType: break; // just to make compiler happy
    } // switch
  } // for

  // send first values: if now without success mark for later resend with true
  if (b_sendVal)
    sendRegisteredVals(ac_processData, ren_doSend);

  // set the timer period for process_c to a low value (maybe the new programm triggers soon)
  getProcessInstance4Comm().resetTimerPeriod();
}

void MeasureProgLocal_c::stop(ProcDataLocal_c& ac_processData, Proc_c::type_t ren_type, Proc_c::doSend_t ren_doSend){
  // send the registered values
  if ((Proc_c::MinimumThreshold != ren_type) && (Proc_c::MaximumThreshold != ren_type)
      && (Proc_c::NullType != ren_type) ) // do not send values for threshold resets and NullType
  {
    sendRegisteredVals(ac_processData);
  }

  if (Proc_c::NullType == ren_type)
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
  if ((Proc_c::MaximumThreshold & ren_type) || (Proc_c::MinimumThreshold & ren_type))
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
  pkg.mc_processCmd.setValues(false /* isSetpoint */, false /* isRequest */, ProcessCmd_c::setValue);
  ac_processData.sendValISOName( pkg, ac_targetISOName, val());
}

bool MeasureProgLocal_c::processMsgHelper( ProcDataLocal_c& ac_processData, const ProcessPkg_c& pkg ){
  ProcessCmd_c::CommandType_t en_command = pkg.mc_processCmd.getCommand();
  bool b_edited = false;

  if (en_command == ProcessCmd_c::setValue)
    // setValue command indicates receiving of a measure value
    // => handle this in measure prog remote => return false
    // or a reset
    // => handle this in measure prog local => return false
    return false;

  // Not sure why this has problems, but it does. So, don't run it with ISO_TASK_CONTROLLER! -bac
  // check if PD==0 -> SET increment message
  // --- Checking of "Proc_c::defaultDataLoggingDDI != pkg.DDI()" is a workaround for bad TCs ---
  if ((!pkg.mc_processCmd.checkIsRequest()) && (Proc_c::defaultDataLoggingDDI != pkg.DDI()) )
  { // mark that msg already edited
    b_edited = true;

    // set en_doSendPkg (for ISO)
    Proc_c::doSend_t en_doSendPkg = Proc_c::DoVal;  //default send data mode

    // programm controlling command
    if (// ISO
        en_command == ProcessCmd_c::measurementDistanceValueStart ||
        en_command == ProcessCmd_c::measurementTimeValueStart ||
        en_command == ProcessCmd_c::measurementChangeThresholdValueStart ||
        en_command == ProcessCmd_c::measurementMinimumThresholdValueStart ||
        en_command == ProcessCmd_c::measurementMaximumThresholdValueStart)
      // increment
      processIncrementMsg( ac_processData, pkg, en_doSendPkg);

    if (en_command == ProcessCmd_c::measurementStop)
       stop(ac_processData);

    // ISO, local
    if (en_command == ProcessCmd_c::measurementDistanceValueStart ||
        en_command == ProcessCmd_c::measurementTimeValueStart ||
        en_command == ProcessCmd_c::measurementChangeThresholdValueStart ||
        en_command == ProcessCmd_c::measurementMinimumThresholdValueStart ||
        en_command == ProcessCmd_c::measurementMaximumThresholdValueStart)
    {
      Proc_c::type_t en_typePkg = Proc_c::NullType;
      int32_t i32_dataLong = pkg.getValue();
      switch (en_command) {
        case ProcessCmd_c::measurementTimeValueStart:
          en_typePkg = Proc_c::TimeProp;
          break;
        case ProcessCmd_c::measurementDistanceValueStart:
          en_typePkg = Proc_c::DistProp;
          break;
        case ProcessCmd_c::measurementChangeThresholdValueStart:
          en_typePkg = Proc_c::OnChange;
          if (Proc_c::ThresholdChangeStopVal == i32_dataLong)
            i32_dataLong = 0; // stop command
          break;
        case ProcessCmd_c::measurementMaximumThresholdValueStart:
          en_typePkg = Proc_c::MaximumThreshold;
          if (Proc_c::ThresholdMaximumStopVal == i32_dataLong)
            i32_dataLong = 0; // stop command
          break;
        case ProcessCmd_c::measurementMinimumThresholdValueStart:
          en_typePkg = Proc_c::MinimumThreshold;
          if (Proc_c::ThresholdMinimumStopVal == i32_dataLong)
            i32_dataLong = 0; // stop command
          break;
        default: ;
      }

      // if dataLong() == 0 => stop
      if (i32_dataLong != 0)
      {
        if (en_typePkg != Proc_c::NullType)
          start(ac_processData, en_typePkg, en_doSendPkg);
      }
      else
       // call MeasureProgLocal_c::stop() with TRUE and en_typePkg != Proc_c::NullType
       // => only the appropriate MeasureSubprog_c is deleted (selective stop)
       stop(ac_processData, en_typePkg, en_doSendPkg);
    }
  }

  return b_edited;
}

bool MeasureProgLocal_c::processMsg( ProcDataLocal_c& ac_processData, const ProcessPkg_c& arc_data )
{
  bool b_result = processMsgHelper( ac_processData, arc_data );

  // call base function - if base function returns true, nothing else must be done
  if (!b_result)
  {
    if (arc_data.senderItem() == NULL)
    { // sender with SA 0xFE is not of interest here!
      return true;
    }

    // backup sender isoname before answering with resetValForGroup() or sendVal() (modification during send in c_pkg !)
    const IsoName_c c_senderIsoNameOrig = arc_data.senderItem()->isoName();

    // ISO: value in message contains reset value
    const int32_t ci32_val = arc_data.getValue();
    const uint32_t cui32_ddi = arc_data.DDI();

    // the message was a value message -> evaluate it here
    if ( arc_data.mc_processCmd.getCommand() == ProcessCmd_c::setValue)
    { // write - accept only write actions to local data only if this is reset try
      // @TODO : test if allowed. Maybe if totals. Note : totals shall be setable !!
      // @TODO : If so, this piece of code belong to setpoint and not to measurement ...

      if ( (ac_processData.triggerMethod() & (0x1 << Proc_c::MethodTotal)) == 0 )
      { // it is a value set. This DDI is no setpoint and no totals -> Process Data not setable
        getProcessInstance4Comm().sendNack( arc_data.senderItem()->isoName(),
                                            ac_processData.isoName(),
                                            ac_processData.DDI(),
                                            ac_processData.element(),
                                            0x10); // Bit 4 = 1 Process Data not setable // @TODO avoid magic number.
        return true;
      }

      resetVal(ac_processData, ci32_val);

      if (Proc_c::defaultDataLoggingDDI == arc_data.DDI())
      { // setValue command for default data logging DDI stops measurement (same as TC task status "suspended")
        getProcessInstance4Comm().processTcStatusMsg(0, c_senderIsoNameOrig, true /* ab_skipLastTcStatus */);
      }

      // call handler function if handler class is registered
      if ( ac_processData.getProcessDataChangeHandler() != NULL )
        ac_processData.getProcessDataChangeHandler()->processMeasurementReset( &ac_processData, cui32_ddi, ci32_val, c_senderIsoNameOrig.toConstIisoName_c());
    } // write
    else
    { // read -> answer wanted value
      sendVal( ac_processData, c_senderIsoNameOrig );

      if ((Proc_c::defaultDataLoggingDDI == arc_data.DDI()) &&
          (ac_processData.getProcessDataChangeHandler() != NULL ))
        // call handler function if handler class is registered
        ac_processData.getProcessDataChangeHandler()->processDefaultLoggingStart( &ac_processData, cui32_ddi, ci32_val, c_senderIsoNameOrig.toConstIisoName_c() );
    } // read
  }

  return true;
}

void MeasureProgLocal_c::setVal(ProcDataLocal_c& ac_processData, int32_t ai32_val){
  int32_t i32_time =  System_c::getTime();

  // update values
  mi32_val = ai32_val;

  // now check if one subprog triggers
  bool b_singleTest = false;
  for (Vec_MeasureSubprogIterator pc_iter = mvec_measureSubprog.begin();
       pc_iter != mvec_measureSubprog.end(); pc_iter++)
  {
    bool triggeredIncrement = false;
    switch (pc_iter->type())
    {
      case Proc_c::TimeProp:
        triggeredIncrement = pc_iter->updateTrigger(i32_time);
        break;
      case Proc_c::DistProp:
        #if defined(USE_BASE) || defined(USE_TRACTOR_MOVE)
        triggeredIncrement = pc_iter->updateTrigger(int32_t(getTracMoveInstance4Comm().distTheor()));
        #endif
        break;
      case Proc_c::OnChange:
        triggeredIncrement = pc_iter->updateTrigger(val());
        break;
      case Proc_c::NullType: break; // just to make compiler happy
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

void MeasureProgLocal_c::initVal(int32_t ai32_val){
  // first call the base function
  mi32_val = ai32_val;
}

void MeasureProgLocal_c::resetVal(ProcDataLocal_c& ac_processData, int32_t ai32_val)
{
  ProcessPkg_c pkg;
  // prepare general command in process pkg
  pkg.mc_processCmd.setValues(false /* isSetpoint */, false /* isRequest */, ProcessCmd_c::setValue);
  // allow reset with value
  //mi32_val = 0;
  mi32_val = ai32_val;

  ac_processData.sendValISOName( pkg, getProcessInstance4Comm().getISONameFromType( m_ecuType ), val());
}

bool MeasureProgLocal_c::timeEvent( ProcDataLocal_c& ac_processData, uint16_t *pui16_nextTimePeriod )
{
  if ( Scheduler_Task_c::getAvailableExecTime() == 0 ) return false;
  int32_t i32_time = Scheduler_Task_c::getLastRetriggerTime();

  bool b_singleTest;
  int32_t i32_nextTimePeriod;
  int32_t i32_distTheor;

  for (Vec_MeasureSubprogIterator pc_iter = mvec_measureSubprog.begin(); pc_iter != mvec_measureSubprog.end(); pc_iter++)
  {
    bool triggeredIncrement = false;
    b_singleTest = false;
    i32_nextTimePeriod = 0;
    switch (pc_iter->type())
    {
      case Proc_c::TimeProp:
        triggeredIncrement = pc_iter->updateTrigger(i32_time);
        // calculate next timer period
        i32_nextTimePeriod = pc_iter->nextTriggerTime(i32_time);

        break;
      case Proc_c::DistProp:
        #if defined(USE_BASE) || defined(USE_TRACTOR_MOVE)
        i32_distTheor = getTracMoveInstance4Comm().distTheor();
        triggeredIncrement = pc_iter->updateTrigger(i32_distTheor);
        #else
        i32_distTheor = 0;
        #endif

        // calculate next timer period
        i32_nextTimePeriod = pc_iter->nextTriggerTime(i32_distTheor);

        break;
      case Proc_c::OnChange:
        b_singleTest = pc_iter->updateTrigger(val());
        //triggeredIncrement = (b_singleTest)? true : triggeredIncrement;
        break;
      default:
        break;
    } // switch

    if (i32_nextTimePeriod && pui16_nextTimePeriod)
    {
      if ( (i32_nextTimePeriod > 0) // something valid to set
            // *pui16_nextTimePeriod not yet set or i32_nextTimePeriod smaller => set
            && ((0 == *pui16_nextTimePeriod) || (i32_nextTimePeriod < *pui16_nextTimePeriod))
          )
      {
        *pui16_nextTimePeriod = i32_nextTimePeriod;
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

  return true;
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
        case Proc_c::MaximumThreshold: b_checkMax = true; i32_maxVal = ps_iterThreshold->i32_threshold; break;
        case Proc_c::MinimumThreshold: b_checkMin = true; i32_minVal = ps_iterThreshold->i32_threshold; break;
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

bool MeasureProgLocal_c::addSubprog(Proc_c::type_t ren_type, int32_t ai32_increment, Proc_c::doSend_t ren_doSend){

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

  return true;
}

void MeasureProgLocal_c::processIncrementMsg( ProcDataLocal_c& ac_processData, const ProcessPkg_c& pkg, Proc_c::doSend_t ren_doSend)
{
  if (pkg.senderItem() == NULL)
  { // don't care for packets from SA 0xFE
    return;
  }

  // set mc_isoName to caller of prog
  m_ecuType = getProcessInstance4Comm().getTypeFromISOName( pkg.senderItem()->isoName() );

  bool b_wrongTriggerMethod = false;
  const int32_t ci32_val = pkg.getValue();

  if ( pkg.mc_processCmd.getCommand() == ProcessCmd_c::measurementTimeValueStart)
  { // time proportional
    if ( (ac_processData.triggerMethod() & (0x1 << Proc_c::MethodTimeInterval)) != 0 )
      addSubprog(Proc_c::TimeProp, __IsoAgLib::abs(ci32_val), ren_doSend);
    else
      b_wrongTriggerMethod = true;
  }
  else if ( pkg.mc_processCmd.getCommand() == ProcessCmd_c::measurementDistanceValueStart)
  {    // distance proportional
    if ( (ac_processData.triggerMethod() & (0x1 << Proc_c::MethodDistInterval)) != 0 )
      addSubprog(Proc_c::DistProp, ci32_val, ren_doSend);
    else
      b_wrongTriggerMethod = true;
  }
  else if (pkg.mc_processCmd.getCommand() == ProcessCmd_c::measurementChangeThresholdValueStart)
  {
    // change threshold proportional
    if ( (ac_processData.triggerMethod() & (0x1 << Proc_c::MethodOnChange)) != 0 )
      addSubprog(Proc_c::OnChange, ci32_val, ren_doSend);
    else
      b_wrongTriggerMethod = true;
  }
  else if (pkg.mc_processCmd.getCommand() == ProcessCmd_c::measurementMaximumThresholdValueStart)
  {
    // change threshold proportional
    if ( (ac_processData.triggerMethod() & (0x1 << Proc_c::MethodThresholdLimit)) != 0 )
      addSubprog(Proc_c::MaximumThreshold, ci32_val, ren_doSend);
    else
      b_wrongTriggerMethod = true;
  }
  else if (pkg.mc_processCmd.getCommand() == ProcessCmd_c::measurementMinimumThresholdValueStart)
  {
    // change threshold proportional
    if ( (ac_processData.triggerMethod() & (0x1 << Proc_c::MethodThresholdLimit)) != 0 )
      addSubprog(Proc_c::MinimumThreshold, ci32_val, ren_doSend);
    else
      b_wrongTriggerMethod = true;
  }

  if (b_wrongTriggerMethod)
    getProcessInstance4Comm().sendNack( pkg.senderItem()->isoName(),
                                        ac_processData.isoName(),
                                        ac_processData.DDI(),
                                        ac_processData.element(),
                                        0x08); // Bit 3 = 1 Trigger method not supported // @TODO avoid magic number.
}

} // end of namespace __IsoAgLib
