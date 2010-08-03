/***************************************************************************
                          measureprogremote_c.cpp - object for managing of
                                                   remote measure programs
                             -------------------
    begin                : Fri Apr 07 2000
    copyright            : (C) 2000 - 2009 by Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
    type                 : Source
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 * This file is part of the "IsoAgLib", an object oriented program library *
 * to serve as a software layer between application specific program and   *
 * communication protocol details. By providing simple function calls for  *
 * jobs like starting a measuring program for a process data value on a    *
 * remote ECU, the main program has not to deal with single CAN telegram   *
 * formatting. This way communication problems between ECU's which use     *
 * this library should be prevented.                                       *
 * Everybody and every company is invited to use this library to make a    *
 * working plug and play standard out of the printed protocol standard.    *
 *                                                                         *
 * Copyright (C) 2000 - 2009 Dipl.-Inform. Achim Spangler                  *
 *                                                                         *
 * The IsoAgLib is free software; you can redistribute it and/or modify it *
 * under the terms of the GNU General Public License as published          *
 * by the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This library is distributed in the hope that it will be useful, but     *
 * WITHOUT ANY WARRANTY; without even the implied warranty of              *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       *
 * General Public License for more details.                                *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with IsoAgLib; if not, write to the Free Software Foundation,     *
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA           *
 *                                                                         *
 * As a special exception, if other files instantiate templates or use     *
 * macros or inline functions from this file, or you compile this file and *
 * link it with other works to produce a work based on this file, this file*
 * does not by itself cause the resulting work to be covered by the GNU    *
 * General Public License. However the source code for this file must still*
 * be made available in accordance with section (3) of the                 *
 * GNU General Public License.                                             *
 *                                                                         *
 * This exception does not invalidate any other reasons why a work based on*
 * this file might be covered by the GNU General Public License.           *
 *                                                                         *
 * Alternative licenses for IsoAgLib may be arranged by contacting         *
 * the main author Achim Spangler by a.spangler@osb-ag:de                  *
 ***************************************************************************/

 /**************************************************************************
 *                                                                         *
 *     ###    !!!    ---    ===    IMPORTANT    ===    ---    !!!    ###   *
 * Each software module, which accesses directly elements of this file,    *
 * is considered to be an extension of IsoAgLib and is thus covered by the *
 * GPL license. Applications must use only the interface definition out-   *
 * side :impl: subdirectories. Never access direct elements of __IsoAgLib  *
 * and __HAL namespaces from applications which shouldnt be affected by    *
 * the license. Only access their interface counterparts in the IsoAgLib   *
 * and HAL namespaces. Contact a.spangler@osb-ag:de in case your applicat- *
 * ion really needs access to a part of an internal namespace, so that the *
 * interface might be extended if your request is accepted.                *
 *                                                                         *
 * Definition of direct access:                                            *
 * - Instantiation of a variable with a datatype from internal namespace   *
 * - Call of a (member-) function                                          *
 * Allowed is:                                                             *
 * - Instatiation of a variable with a datatype from interface namespace,  *
 *   even if this is derived from a base class inside an internal namespace*
 * - Call of member functions which are defined in the interface class     *
 *   definition ( header )                                                 *
 *                                                                         *
 * Pairing of internal and interface classes:                              *
 * - Internal implementation in an :impl: subdirectory                     *
 * - Interface in the parent directory of the corresponding internal class *
 * - Interface class name IsoAgLib::iFoo_c maps to the internal class      *
 *   __IsoAgLib::Foo_c                                                     *
 *                                                                         *
 * AS A RULE: Use only classes with names beginning with small letter :i:  *
 ***************************************************************************/

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include "measureprogremote_c.h"
#include <IsoAgLib/driver/system/impl/system_c.h>
#include <IsoAgLib/comm/Part7_ProcessData/impl/process_c.h>
#include <IsoAgLib/comm/Part7_ProcessData/processdatachangehandler_c.h>
#include <IsoAgLib/util/impl/util_funcs.h>

namespace __IsoAgLib {
  /**
    constructor of MeasureProgRemote_c which can needed pointers to containing
    Scheduler_c and Process_c instances
    @param apc_processData optional pointer to Process_c
   */
  MeasureProgRemote_c::MeasureProgRemote_c(
      ProcDataBase_c *const apc_processData )
  : MeasureProgBase_c(apc_processData, 0, IsoName_c::IsoNameUnspecified() )
  {
    init( apc_processData );
  }

/**
  initialise this MeasureProgRemote_c instance to well defined initial condition
  Scheduler_c and Process_c instances
  @param apc_processData optional pointer to Process_c
*/
void MeasureProgRemote_c::init( ProcDataBase_c *const apc_processData )
{
  MeasureProgBase_c::init( apc_processData, int32_t(0), IsoName_c::IsoNameUnspecified() );
  mb_receiveForeignMeasurement = false;
}

/**
  assignment of MeasureProgRemote_c objects
  @param acrc_src source MeasureProgRemote_c instance
  @return reference to the source instance for commands like "prog1 = prog2 = prog3 ...;
*/
const MeasureProgRemote_c& MeasureProgRemote_c::operator=(const MeasureProgRemote_c& acrc_src){
  // call base class operator
  MeasureProgBase_c::operator=(acrc_src);

  mb_receiveForeignMeasurement = acrc_src.mb_receiveForeignMeasurement;

  // return reference to source
  return acrc_src;
}

/**
  copy constructor for MeasureProgRemote
  @param acrc_src source MeasureProgRemote_c instance
*/
MeasureProgRemote_c::MeasureProgRemote_c(const MeasureProgRemote_c& acrc_src)
  : MeasureProgBase_c(acrc_src){

  // copy element vars
  mb_receiveForeignMeasurement = acrc_src.mb_receiveForeignMeasurement;
}

/** default destructor which has nothing to do */
MeasureProgRemote_c::~MeasureProgRemote_c(){
}

/**
  start a measuring programm

  possible errors:
      * Err_c::elNonexistent no remote member with claimed address with given DEVCLASS found
      * dependant error in CAN_IO

  @param ren_doSend set process data subtype to send (Proc_c::DoNone, Proc_c::DoVal, Proc_c::DoValForExactSetpoint...)
  @return true -> command successful sent
*/
bool MeasureProgRemote_c::start(Proc_c::doSend_t ren_doSend){
  // retrieve the ren_type from the matching registered subprog
  Proc_c::type_t en_typeLocal = Proc_c::NullType;

  for (Vec_MeasureSubprog::iterator pc_subprog = mvec_measureSubprog.begin();
       pc_subprog != mvec_measureSubprog.end(); pc_subprog++)
  {
    // take type of measurement prog only from that subprog with the same en_doSend! (no combination!)
    if (pc_subprog->doSend() == ren_doSend)
      en_typeLocal = pc_subprog->type();
  }

  // now call other start with en_combinedType
  return start(en_typeLocal, ren_doSend);
}

/**
  start a measuring programm

  possible errors:
      * Err_c::elNonexistent no remote member with claimed address with given DEVCLASS found
      * dependant error in CAN_IO

  @param ren_type wanted increment type (Proc_c::TimeProp, Proc_c::DistProp, ...)
  @param ren_doSend set process data subtype to send (Proc_c::DoNone, Proc_c::DoVal, Proc_c::DoValForExactSetpoint...)
  @return true -> command successful sent
*/
bool MeasureProgRemote_c::start(Proc_c::type_t ren_type, Proc_c::doSend_t ren_doSend)
{
  bool b_sendResult = true;

  // if stored remote isoName isn't valid exit this function
  // error state are set by the function
  if (!verifySetRemoteISOName())return false;


  // send all registered increments to remote ECU
  for (Vec_MeasureSubprog::iterator pc_subprog = mvec_measureSubprog.begin();
       pc_subprog != mvec_measureSubprog.end(); pc_subprog++)
  { // send the suitable increment creating message to the remote system

    if (pc_subprog->doSend() != ren_doSend)
      continue;

    ProcessCmd_c::CommandType_t en_command = ProcessCmd_c::noCommand;

    // ISO: send command (value and start) only if program type matches!
    if ( pc_subprog->type() == ren_type )
    {

      if (ren_type == Proc_c::TimeProp)
          en_command = ProcessCmd_c::measurementTimeValueStart;

      if (ren_type == Proc_c::DistProp)
        en_command = ProcessCmd_c::measurementDistanceValueStart;

      if (ren_type == Proc_c::MaximumThreshold)
        en_command = ProcessCmd_c::measurementMaximumThresholdValueStart;

      if (ren_type == Proc_c::MinimumThreshold)
        en_command = ProcessCmd_c::measurementMinimumThresholdValueStart;

      if (ren_type == Proc_c::OnChange)
        en_command = ProcessCmd_c::measurementChangeThresholdValueStart;
    }

    if (en_command != ProcessCmd_c::noCommand)
    {
      int32_t i32_tmpValue = pc_subprog->increment();

      bool b_isSetpoint = FALSE;
      ProcessCmd_c::ValueGroup_t en_valueGroup;

      switch (ren_doSend)
      {
        case Proc_c::DoValForExactSetpoint:
          en_valueGroup = ProcessCmd_c::exactValue;
          // start measurement for a sepoint DDI in proc data instance
          b_isSetpoint = TRUE;
          break;

        case Proc_c::DoValForDefaultSetpoint:
          en_valueGroup = ProcessCmd_c::defaultValue;
          // start measurement for a sepoint DDI in proc data instance
          b_isSetpoint = TRUE;
          break;

        case Proc_c::DoValForMinSetpoint:
          // start measurement for a sepoint DDI in proc data instance
          b_isSetpoint = TRUE;
          // no break!
        case Proc_c::DoValForMinMeasurement:
          en_valueGroup = ProcessCmd_c::minValue;
          break;

        case Proc_c::DoValForMaxSetpoint:
          // start measurement for a sepoint DDI in proc data instance
          b_isSetpoint = TRUE;
          // no break!
        case Proc_c::DoValForMaxMeasurement:
          en_valueGroup = ProcessCmd_c::maxValue;
          break;

        default:
          en_valueGroup = ProcessCmd_c::exactValue;
      }

      // prepare general command in process pkg
      getProcessInstance4Comm().data().mc_processCmd.setValues(b_isSetpoint, false /* isRequest */,
                                                               en_valueGroup, en_command);
      if (!processData().sendValISOName(isoName(), i32_tmpValue))
         b_sendResult = false;
    }
  }

  // call base function for registering start state
  MeasureProgBase_c::start(ren_type, ren_doSend);

  // store actual time as last receive measure time to
  // avoid misinterpretation of to int32_t not receive data
  mi32_lastMeasureReceive = System_c::getTime();

  return b_sendResult;
}

/**
  send the stop command to the remote owner of data

  possible errors:
      * Err_c::elNonexistent no remote member with claimed address with given DEVCLASS found
      * dependant error in CAN_IO
  @param b_deleteSubProgs is only used for ISO
  @param ren_type wanted increment type (Proc_c::TimeProp, Proc_c::DistProp, ...)
  @param ren_doSend set process data subtype to stop (Proc_c::DoNone, Proc_c::DoVal, Proc_c::DoValForExactSetpoint...)
  @return true -> command successful sent
*/
bool MeasureProgRemote_c::stop(bool b_deleteSubProgs, Proc_c::type_t ren_type, Proc_c::doSend_t ren_doSend){
  bool b_result = true;

  // if stored remote isoName isn't valid exit this function
  // error state are set by the function
  if (!verifySetRemoteISOName()) return false;

  // at the moment only TimeProp and DistProp are well -> only send them

  // send stop command for each program
  for (Vec_MeasureSubprog::iterator pc_subprog = mvec_measureSubprog.begin();
        pc_subprog != mvec_measureSubprog.end();)
  {
    if (Proc_c::NullType != ren_type)
    { // when a specific type is given (e.g. TimeProp...) => do a selective stop (match subprog type and doSend)
      if ((pc_subprog->type() != ren_type) || (pc_subprog->doSend() != ren_doSend))
      {
        pc_subprog++;
        continue;
      }
    }

    ProcessCmd_c::CommandType_t en_command = ProcessCmd_c::noCommand;
    int32_t i32_stopVal = 0;

    switch (pc_subprog->type())
    {
        case Proc_c::TimeProp:
          en_command = ProcessCmd_c::measurementTimeValueStart;
          break;
        case Proc_c::DistProp:
          en_command = ProcessCmd_c::measurementDistanceValueStart;
          break;
        case Proc_c::OnChange:
          en_command = ProcessCmd_c::measurementChangeThresholdValueStart;
          i32_stopVal = Proc_c::ThresholdChangeStopVal;
          break;
        case Proc_c::MaximumThreshold:
          en_command = ProcessCmd_c::measurementMaximumThresholdValueStart;
          i32_stopVal = Proc_c::ThresholdMaximumStopVal;
          break;
        case Proc_c::MinimumThreshold:
          en_command = ProcessCmd_c::measurementMinimumThresholdValueStart;
          i32_stopVal = Proc_c::ThresholdMinimumStopVal;
          break;
        default: ;
    }

    if (en_command != ProcessCmd_c::noCommand)
    {
      bool b_isSetpoint = FALSE;
      ProcessCmd_c::ValueGroup_t en_valueGroup;

      switch (pc_subprog->doSend())
      {
        case Proc_c::DoValForExactSetpoint:
          en_valueGroup = ProcessCmd_c::exactValue;
          // start measurement for a sepoint DDI in proc data instance
          b_isSetpoint = TRUE;
          break;

        case Proc_c::DoValForDefaultSetpoint:
          en_valueGroup = ProcessCmd_c::defaultValue;
          // stop measurement for a sepoint DDI in proc data instance
          b_isSetpoint = TRUE;
          break;

        case Proc_c::DoValForMinSetpoint:
          // stop measurement for a sepoint DDI in proc data instance
          b_isSetpoint = TRUE;
          // no break!
        case Proc_c::DoValForMinMeasurement:
          en_valueGroup = ProcessCmd_c::minValue;
          break;

        case Proc_c::DoValForMaxSetpoint:
          // stop measurement for a sepoint DDI in proc data instance
          b_isSetpoint = TRUE;
          // no break!
        case Proc_c::DoValForMaxMeasurement:
          en_valueGroup = ProcessCmd_c::maxValue;
          break;

        default:
          en_valueGroup = ProcessCmd_c::exactValue;
      }

      // prepare general command in process pkg
      getProcessInstance4Comm().data().mc_processCmd.setValues(b_isSetpoint, false /* isRequest */,
                                                               en_valueGroup, en_command);
      b_result = processData().sendValISOName(isoName(), i32_stopVal);
    }

    if (b_deleteSubProgs)
      pc_subprog = mvec_measureSubprog.erase(pc_subprog);
    else
      pc_subprog++;
  }

  return b_result;
}

/**
  process msg;
  @return true -> msg is already complete edited
*/
bool MeasureProgRemote_c::processMsg()
{
  bool b_result = MeasureProgBase_c::processMsg();
  // call base function - if base function returns true, nothing else must be done
  if (!b_result)
  { // the message was a value message -> evaluate it here

    // only update vars if received msg contains data (PD==1)
    ProcessPkg_c& c_pkg = getProcessInstance4Comm().data();

    // ISO: cmd() == 3;
    if (    c_pkg.mc_processCmd.getCommand() == ProcessCmd_c::setValue
         &&
            (    (c_pkg.receiverItem() && c_pkg.receiverItem()->itemState(IState_c::Local))
              || (mb_receiveForeignMeasurement)
            )
       )
    { // write -> data from remote process data
      setValFromPkg();
      b_result = true;
    }
  }
  return b_result;
}

/**
  set according to values of the package the accoring value
  (uses function, wich converts if needed)
*/
void MeasureProgRemote_c::setValFromPkg()
{
  // convert the value if wanted
  // set timestamp of last measurement receive
  ProcessPkg_c& c_pkg = getProcessInstance4Comm().data();
  if (c_pkg.senderItem() == NULL)
  { // sender with SA 0xFE not of interest
    return;
  }

  mi32_lastMeasureReceive = c_pkg.time();

  bool b_change = false;

  const int32_t i32_new_val = c_pkg.getValue();

  switch (c_pkg.mc_processCmd.getValueGroup())
  {
    case ProcessCmd_c::exactValue: // msg contains measured val
      // set val with function, to calc delta and accel
      if ( val() != i32_new_val ) b_change = true;
      setVal(i32_new_val);
      break;
    case ProcessCmd_c::minValue: // msg contains MIN val
      if ( min() != i32_new_val ) b_change = true;
      setMin(i32_new_val);
      break;
    case ProcessCmd_c::maxValue: // msg contains MAX val
      if ( max() != i32_new_val ) b_change = true;
      setMax(i32_new_val);
      break;
    default: ;
  }

  // call handler function if handler class is registered
  if ( processDataConst().getProcessDataChangeHandler() != NULL )
    processDataConst().getProcessDataChangeHandler()->processMeasurementUpdate( pprocessData(), i32_new_val, c_pkg.senderItem()->isoName().toConstIisoName_c(), b_change );
}

/**
  set a new measure val
  @param ai32_val new val received from remote system
*/
void MeasureProgRemote_c::setVal(int32_t ai32_val){
  int32_t i32_time =  System_c::getTime();
  int32_t i32_timeDelta = i32_time - mi32_lastTime;
  mi32_lastTime = i32_time;

  int32_t i32_incr =  ai32_val - val();
  int32_t i32_oldDelta = mi32_delta;
  // claculate delta and accel in 1/s
  if (i32_timeDelta > 0)
  {
    if (__IsoAgLib::abs(i32_incr) > 100000)
    { // i32_incr very big -> first divide than mult
      mi32_delta = (i32_incr / i32_timeDelta) * 1000;
      mi32_accel = ((mi32_delta - i32_oldDelta) / i32_timeDelta) * 1000;
    }
    else
    {
      mi32_delta = (i32_incr * 1000) / i32_timeDelta;
      mi32_accel = ((mi32_delta - i32_oldDelta) * 1000) / i32_timeDelta;
    }
  }

  // set the val
  mi32_val = ai32_val;
}

/**
  init element values
  @param ai32_val starting measuring value
*/
void MeasureProgRemote_c::initVal(int32_t ai32_val){
  //first call base function
  MeasureProgBase_c::initVal(ai32_val);
}

/**
  send reset command for measure value

  possible errors:
      * Err_c::elNonexistent no remote member with claimed address with given DEVCLASS found
      * dependant error in CAN_IO
  @param ai32_val reset measure value to this value
  @return true -> command successful sent
*/
bool MeasureProgRemote_c::resetVal(int32_t ai32_val){
  // if stored remote isoName isn't valid exit this function
  // error state are set by the function
  if (!verifySetRemoteISOName())return false;

  // prepare general command in process pkg
  getProcessInstance4Comm().data().mc_processCmd.setValues(false /* isSetpoint */, false /* isRequest */,
                                                           ProcessCmd_c::exactValue,
                                                           ProcessCmd_c::measurementReset);

  return processData().sendValISOName(isoName(), ai32_val);
}


/**
  perform periodic actions --> stop measuring prog if isoName isn't active any more
  @param pui16_nextTimePeriod calculated new time period, based on current measure progs (only for local proc data)
  @return true -> all planned activities performed in available time
*/
bool MeasureProgRemote_c::timeEvent( uint16_t* /* pui16_nextTimePeriod  */)
{
  if ( (!getIsoMonitorInstance4Comm().existIsoMemberISOName(isoName(), true)) && started() )
  { // remote owner of this process data isn't active any more
    // stop measureing subprogs
    stop();
  }
  return true;
}


/**
  set if this MeasureProgRemote_c instance should store
  target/partner process data messages not direct addressed
  to a local member (default store not)
  @param ab_useForeign wanted mode (default true)
*/
void MeasureProgRemote_c::receiveForeignMeasurement(bool ab_useForeign)
{
  mb_receiveForeignMeasurement = ab_useForeign;
};


/**
  verify the stored ISOName code of the remote system
  and set Err_c::elNonexistent if this system isn't registered as ative;
  if isoName is undefied yet, retrieve it by the stored ident DEVCLASS of this process data

  possible errors:
      * Err_c::elNonexistent no remote member with claimed address with given DEVCLASS found
  @return true -> everything o.k.
*/
bool MeasureProgRemote_c::verifySetRemoteISOName()
{
  // check if proc data identity is valid and corresponding member has claimed address in monitor list
  IsoMonitor_c& c_isoMonitor = getIsoMonitorInstance4Comm();
  if ( processData().isoName().isSpecified()
    && (c_isoMonitor.existIsoMemberISOName(processData().isoName(), true)))
    return true; // change nothing and return success

  // if test was false look for member with claimed address with DEVCLASS of this process
  // data to update dev class inst of this instance
  IsoName_c c_tempISOName = processData().isoName();
  uint8_t ui8_tmpDevClassInst;
  for (ui8_tmpDevClassInst = 0; ui8_tmpDevClassInst <= 0xF; ui8_tmpDevClassInst++)
  {
    c_tempISOName.setDevClassInst( ui8_tmpDevClassInst );
    if (getIsoMonitorInstance4Comm().existIsoMemberISOName(c_tempISOName))
    {
      processData().setDevClassInst(ui8_tmpDevClassInst);
      // stop further search if this item has already claimed address
      if (getIsoMonitorInstance4Comm().isoMemberISOName(c_tempISOName).itemState(IState_c::ClaimedAddress))
      { // this item has claimed address -> use it and stop search
        return true; // return from this function with positive result
      }
    }
  } // for
  getILibErrInstance().registerError( iLibErr_c::ElNonexistent, iLibErr_c::Process );
  return false;
}

} // end of namespace __IsoAgLib
