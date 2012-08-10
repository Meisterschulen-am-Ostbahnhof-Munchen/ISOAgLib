/*
  measureprogbase_c.cpp: base class for measure values and programs

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include "measureprogbase_c.h"
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/process_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/processdatachangehandler_c.h>
#include <IsoAgLib/util/impl/util_funcs.h>
#include <algorithm>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/procdatalocal_c.h>

#if DEBUG_HEAP_USEAGE
  #ifdef SYSTEM_PC
    #include <iostream>
  #else
    #include <supplementary_driver/driver/rs232/impl/rs232io_c.h>
  #endif
  #include <IsoAgLib/util/impl/util_funcs.h>
#endif

#if DEBUG_HEAP_USEAGE
static uint16_t sui16_MeasureProgBaseTotal = 0;
static uint16_t sui16_printedMeasureProgBaseTotal = 0;
static uint16_t sui16_deconstructMeasureProgBaseTotal = 0;
static uint16_t sui16_printedDeconstructMeasureProgBaseTotal = 0;
#endif

namespace __IsoAgLib {

void MeasureProgBase_c::init( ProcDataLocal_c *const apc_processData,
  int32_t ai32_val,
  const IsoName_c& acrc_isoName)
{ // set the dynamic list to a well defined cleared starting condition
  #if DEBUG_HEAP_USEAGE
  static bool b_doPrint = true;
  if ( b_doPrint )
  {
    b_doPrint = false;
    INTERNAL_DEBUG_DEVICE
      << "sizeof(MeasureSubprog_c) ==  " << sizeof(MeasureSubprog_c)
      << " Bytes" << INTERNAL_DEBUG_DEVICE_ENDL;
  }
  if ( sui16_MeasureProgBaseTotal > 0 )
  {
    sui16_MeasureProgBaseTotal -= ( mvec_measureSubprog.size() * ( sizeof(MeasureSubprog_c) + 2 * sizeof(MeasureSubprog_c*) ) );
  }
  #endif

  mvec_measureSubprog.clear();
  // set the pointers in the baseClass ProcessElementBase
  set(apc_processData);
  // store the parameter init vals
  mc_isoName = acrc_isoName;
  mi32_val = ai32_val;
  mb_active = false;

  // set the rest of element vals to defined init
  men_accumProp = Proc_c::AccumNone;
  men_doSend = Proc_c::DoNone;
  //men_type = Proc_c::DistProp;

  mi32_accel = mi32_delta = mi32_lastTime = mi32_max = mi32_min = 0;

  // setting of isoName in MeasureProg is normally done via ProcDataRemote_c::timeEvent( void )
  // if start follows immedeately addSubprog, timeEvent is not called yet => do it here
  // remote: virtual ProcDataRemote::commanderISOName() can give a value different to IsoName_c::IsoNameUnspecified
  // local: virtual ProcDataLocal::commanderISOName() gives IsoName_c::IsoNameUnspecified
  if (pprocessDataConst() && pprocessDataConst()->commanderISOName().isSpecified())
    setISOName(pprocessDataConst()->commanderISOName());
}

const MeasureProgBase_c& MeasureProgBase_c::operator=(const MeasureProgBase_c& acrc_src){
  // call base class operator
  ProcessElementBase_c::operator=(acrc_src);

  assignFromSource( acrc_src );

  // return reference to source
  return acrc_src;
}

MeasureProgBase_c::MeasureProgBase_c(const MeasureProgBase_c& acrc_src)
 : ProcessElementBase_c(acrc_src)  {
  assignFromSource( acrc_src );
}

void MeasureProgBase_c::assignFromSource( const MeasureProgBase_c& acrc_src )
{ // copy element vars
  mc_isoName = acrc_src.mc_isoName;
  men_accumProp =  acrc_src.men_accumProp;
  men_doSend = acrc_src.men_doSend;
  mb_active = acrc_src.mb_active;
  mi32_accel = acrc_src.mi32_accel;
  mi32_delta = acrc_src.mi32_delta;
  mi32_lastTime = acrc_src.mi32_lastTime;
  mi32_max = acrc_src.mi32_max;
  mi32_min = acrc_src.mi32_min;
  mi32_val = acrc_src.mi32_val;
  mvec_measureSubprog = acrc_src.mvec_measureSubprog;
}

MeasureProgBase_c::~MeasureProgBase_c(){
  #if DEBUG_HEAP_USEAGE
  if ( mvec_measureSubprog.size() > 0 )
  {
    sui16_deconstructMeasureProgBaseTotal++;
    sui16_MeasureProgBaseTotal -= mvec_measureSubprog.size();
  }
  #endif
}

bool MeasureProgBase_c::addSubprog(Proc_c::type_t ren_type, int32_t ai32_increment, Proc_c::doSend_t ren_doSend){
  if (ren_type == Proc_c::TimeProp) men_accumProp = Proc_c::AccumTime;
  else if (ren_type == Proc_c::DistProp) men_accumProp = Proc_c::AccumDist;

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
    #if DEBUG_HEAP_USEAGE
    sui16_MeasureProgBaseTotal++;

    INTERNAL_DEBUG_DEVICE
      << sui16_MeasureProgBaseTotal << " x MeasureSubprog_c: Mal-Alloc: "
      <<  sizeSlistTWithMalloc( sizeof(MeasureSubprog_c), sui16_MeasureProgBaseTotal )
      << "/" << sizeSlistTWithMalloc( sizeof(MeasureSubprog_c), 1 )
      << ", Chunk-Alloc: "
      << sizeSlistTWithChunk( sizeof(MeasureSubprog_c), sui16_MeasureProgBaseTotal )
      << INTERNAL_DEBUG_DEVICE_NEWLINE << INTERNAL_DEBUG_DEVICE_ENDL;
    #endif
  }

  return true;
}

bool MeasureProgBase_c::start(Proc_c::type_t ren_type, Proc_c::doSend_t ren_doSend){
  // register values
  men_doSend = (ren_doSend != Proc_c::DoNone)?ren_doSend : men_doSend;
  if (men_doSend == Proc_c::DoNone) men_doSend = Proc_c::DoVal;
  //men_type = ren_type;
  return true;
}

bool MeasureProgBase_c::stop(bool /*b_deleteSubProgs*/, Proc_c::type_t /* ren_type */, Proc_c::doSend_t /* ren_doSend */){
  // clear the array with all subprogs -> no trigger test is done on value set
  #if DEBUG_HEAP_USEAGE
  sui16_MeasureProgBaseTotal -= mvec_measureSubprog.size();

  if ( ( sui16_MeasureProgBaseTotal != sui16_printedMeasureProgBaseTotal                     )
  || ( sui16_deconstructMeasureProgBaseTotal != sui16_printedDeconstructMeasureProgBaseTotal ) )
  {
    sui16_printedMeasureProgBaseTotal = sui16_MeasureProgBaseTotal;
    sui16_printedDeconstructMeasureProgBaseTotal = sui16_deconstructMeasureProgBaseTotal;
    INTERNAL_DEBUG_DEVICE
      << sui16_MeasureProgBaseTotal << " x MeasureSubprog_c: Mal-Alloc: "
      <<  sizeSlistTWithMalloc( sizeof(MeasureSubprog_c), sui16_MeasureProgBaseTotal )
      << "/" << sizeSlistTWithMalloc( sizeof(MeasureSubprog_c), 1 )
      << ", Chunk-Alloc: "
      << sizeSlistTWithChunk( sizeof(MeasureSubprog_c), sui16_MeasureProgBaseTotal )
      << INTERNAL_DEBUG_DEVICE_NEWLINE << INTERNAL_DEBUG_DEVICE_ENDL;
  }
  #endif
  mvec_measureSubprog.clear();
  men_doSend = Proc_c::DoNone;
  return true;
}

int32_t MeasureProgBase_c::val(bool ab_sendRequest) const
{
  if (ab_sendRequest) {
    // prepare general command in process pkg
    ProcessPkg_c pkg;
    pkg.mc_processCmd.setValues(false /* isSetpoint */, true /* isRequest */,
                                                             ProcessCmd_c::exactValue,
                                                             ProcessCmd_c::requestValue);

    processData().sendValISOName(pkg, isoName(), int32_t(0));
  }

  return mi32_val;
}

int32_t MeasureProgBase_c::min(bool ab_sendRequest) const
{
  if(ab_sendRequest) {
    // prepare general command in process pkg
    ProcessPkg_c pkg;
    pkg.mc_processCmd.setValues(false /* isSetpoint */, true /* isRequest */,
                                                             ProcessCmd_c::minValue,
                                                             ProcessCmd_c::requestValue);

    processData().sendValISOName(pkg, isoName(), int32_t(0));
  }
  return mi32_min;
}

int32_t MeasureProgBase_c::max(bool ab_sendRequest) const
{
  if (ab_sendRequest) {
    // prepare general command in process pkg
    ProcessPkg_c pkg;
    pkg.mc_processCmd.setValues(false /* isSetpoint */, true /* isRequest */,
                                                             ProcessCmd_c::maxValue,
                                                             ProcessCmd_c::requestValue);

    processData().sendValISOName( pkg, isoName(), int32_t(0));
  }
  return mi32_max;
}

void MeasureProgBase_c::initVal(int32_t ai32_val){
  #if DEBUG_HEAP_USEAGE
  if ( ( sui16_MeasureProgBaseTotal != sui16_printedMeasureProgBaseTotal                     )
  || ( sui16_deconstructMeasureProgBaseTotal != sui16_printedDeconstructMeasureProgBaseTotal ) )
  {
    sui16_printedMeasureProgBaseTotal = sui16_MeasureProgBaseTotal;
    sui16_printedDeconstructMeasureProgBaseTotal = sui16_deconstructMeasureProgBaseTotal;
    INTERNAL_DEBUG_DEVICE
      << sui16_MeasureProgBaseTotal << " x MeasureSubprog_c: Mal-Alloc: "
      <<  sizeSlistTWithMalloc( sizeof(MeasureSubprog_c), sui16_MeasureProgBaseTotal )
      << "/" << sizeSlistTWithMalloc( sizeof(MeasureSubprog_c), 1 )
      << ", Chunk-Alloc: "
      << sizeSlistTWithChunk( sizeof(MeasureSubprog_c), sui16_MeasureProgBaseTotal )
      << INTERNAL_DEBUG_DEVICE_NEWLINE << INTERNAL_DEBUG_DEVICE_ENDL;
  }
  #endif
  mi32_val = mi32_min = mi32_max = ai32_val;
}

bool MeasureProgBase_c::processMsg( const ProcessPkg_c& pkg ){
  ProcessCmd_c::CommandType_t en_command = pkg.mc_processCmd.getCommand();

  #if DEBUG_HEAP_USEAGE
  if ( ( sui16_MeasureProgBaseTotal != sui16_printedMeasureProgBaseTotal                     )
  || ( sui16_deconstructMeasureProgBaseTotal != sui16_printedDeconstructMeasureProgBaseTotal ) )
  {
    sui16_printedMeasureProgBaseTotal = sui16_MeasureProgBaseTotal;
    sui16_printedDeconstructMeasureProgBaseTotal = sui16_deconstructMeasureProgBaseTotal;
    INTERNAL_DEBUG_DEVICE
      << sui16_MeasureProgBaseTotal << " x MeasureSubprog_c: Mal-Alloc: "
      <<  sizeSlistTWithMalloc( sizeof(MeasureSubprog_c), sui16_MeasureProgBaseTotal )
      << "/" << sizeSlistTWithMalloc( sizeof(MeasureSubprog_c), 1 )
      << ", Chunk-Alloc: "
      << sizeSlistTWithChunk( sizeof(MeasureSubprog_c), sui16_MeasureProgBaseTotal )
      << INTERNAL_DEBUG_DEVICE_NEWLINE << INTERNAL_DEBUG_DEVICE_ENDL;
  }
  #endif
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
    ProcessCmd_c::ValueGroup_t en_valueGroup = pkg.mc_processCmd.getValueGroup();

    Proc_c::doSend_t en_doSendPkg = Proc_c::DoVal;  //default send data mode
    if (pkg.mc_processCmd.checkIsSetpoint())
      en_doSendPkg = Proc_c::DoValForExactSetpoint; // measurement for exact value setpoint

    switch (en_valueGroup)
    {
      case ProcessCmd_c::minValue:
        en_doSendPkg = Proc_c::DoValForMinSetpoint; // measurement for min value setpoint
        if (!pkg.mc_processCmd.checkIsSetpoint())
          en_doSendPkg = Proc_c::DoValForMinMeasurement; // measurement for min value measurement
        break;
      case ProcessCmd_c::maxValue:
        en_doSendPkg = Proc_c::DoValForMaxSetpoint; // measurement for max value setpoint
        if (!pkg.mc_processCmd.checkIsSetpoint())
          en_doSendPkg = Proc_c::DoValForMaxMeasurement; // measurement for max value measurement
        break;
      case ProcessCmd_c::defaultValue:
        en_doSendPkg = Proc_c::DoValForDefaultSetpoint; // measurement for default value setpoint
        break;
      default:
        ;
    }

    // programm controlling command
    if (// ISO
        en_command == ProcessCmd_c::measurementDistanceValueStart ||
        en_command == ProcessCmd_c::measurementTimeValueStart ||
        en_command == ProcessCmd_c::measurementChangeThresholdValueStart ||
        en_command == ProcessCmd_c::measurementMinimumThresholdValueStart ||
        en_command == ProcessCmd_c::measurementMaximumThresholdValueStart)
      // increment
      processIncrementMsg( pkg, en_doSendPkg);

    if (en_command == ProcessCmd_c::measurementStop)
       stop();

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
          start(en_typePkg, en_doSendPkg);
      }
      else
       // call MeasureProgLocal_c::stop() with true and en_typePkg != Proc_c::NullType
       // => only the appropriate MeasureSubprog_c is deleted (selective stop)
       stop(true /* b_deleteSubProgs */, en_typePkg, en_doSendPkg);
    }
  }

  return b_edited;
}

int32_t MeasureProgBase_c::valForGroup(ProcessCmd_c::ValueGroup_t en_valueGroup) const {
  int32_t i32_value = val();
  switch (en_valueGroup)
  {
    case ProcessCmd_c::exactValue:
      // set val with function, to calc delta and accel
      // i32_value = val();
      break;
    case ProcessCmd_c::minValue:
      i32_value = min();
      break;
    case ProcessCmd_c::maxValue:
      i32_value = max();
      break;
    default:
      IsoAgLib::getILibErrInstance().registerNonFatal( IsoAgLib::iLibErr_c::ProcData, getMultitonInst() );
  }

  return i32_value;
}

void MeasureProgBase_c::processIncrementMsg( const ProcessPkg_c& pkg, Proc_c::doSend_t ren_doSend)
{

  if (pkg.senderItem() == NULL)
  { // don't care for packets from SA 0xFE
    return;
  }

  // set mc_isoName to caller of prog
  mc_isoName = pkg.senderItem()->isoName();

  const int32_t ci32_val = pkg.getValue();

  if ( pkg.mc_processCmd.getCommand() == ProcessCmd_c::measurementTimeValueStart)
    // time proportional
    addSubprog(Proc_c::TimeProp, __IsoAgLib::abs(ci32_val), ren_doSend);

  if ( pkg.mc_processCmd.getCommand() == ProcessCmd_c::measurementDistanceValueStart)
    // distance proportional
    addSubprog(Proc_c::DistProp, ci32_val, ren_doSend);

  if (pkg.mc_processCmd.getCommand() == ProcessCmd_c::measurementChangeThresholdValueStart)
    // change threshold proportional
    addSubprog(Proc_c::OnChange, ci32_val, ren_doSend);

  if (pkg.mc_processCmd.getCommand() == ProcessCmd_c::measurementMaximumThresholdValueStart)
    // change threshold proportional
    addSubprog(Proc_c::MaximumThreshold, ci32_val, ren_doSend);

  if (pkg.mc_processCmd.getCommand() == ProcessCmd_c::measurementMinimumThresholdValueStart)
    // change threshold proportional
    addSubprog(Proc_c::MinimumThreshold, ci32_val, ren_doSend);
}

void MeasureProgBase_c::resetValForGroup(ProcessCmd_c::ValueGroup_t en_valueGroup, int32_t ai32_val){
    switch (en_valueGroup)
    {
      case ProcessCmd_c::exactValue:
        // set val with function, to calc delta and accel
        resetVal(ai32_val);
        break;
      case ProcessCmd_c::minValue:
        resetMin();
        break;
      case ProcessCmd_c::maxValue:
        resetMax();
        break;
      default:
        IsoAgLib::getILibErrInstance().registerNonFatal( IsoAgLib::iLibErr_c::ProcData, getMultitonInst() );
    }
}


} // end of namespace __IsoAgLib
