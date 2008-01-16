/***************************************************************************
                          measureproglocal_c.cpp - object for managing local
                                                  measure progs and values
                             -------------------
    begin                : Fri Apr 07 2000
    copyright            : (C) 2000 - 2004 by Dipl.-Inform. Achim Spangler
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
 * Copyright (C) 2000 - 2004 Dipl.-Inform. Achim Spangler                  *
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
#include "measureproglocal_c.h"
#include "../../../impl/process_c.h"
#include "../../../processdatachangehandler_c.h"
#include <IsoAgLib/comm/Part7_ApplicationLayer/impl/tracmove_c.h>
//#include <IsoAgLib/comm/Part5_NetworkManagement//iisoname_c.h>

namespace __IsoAgLib {

/** initialise this MeasureProgLocal_c instance to a well defined initial state
    @param apc_processData optional pointer to containing ProcDataLocal_c instance (def NULL)
    @param ai32_masterVal optional actual central local measured value used as masterVal (def 0)
    @param ai32_initialVal optional initial value (e.g which was stored in EEPROM) (default 0)
    @param ac_callerISOName optional ISOName of remote member, which caused creation of this instance (default 0xFF == no member)
  */
void MeasureProgLocal_c::init(
  ProcDataBase_c *const apc_processData,
  int32_t ai32_masterVal,
  int32_t ai32_initialVal,
  const IsoName_c& ac_callerISOName)
{
  MeasureProgBase_c::init( apc_processData, ai32_initialVal, ac_callerISOName  );

  mi32_lastMasterVal = ai32_masterVal;
  if (ai32_initialVal != 0)
  { // set medSum, medCnt to initial value read from EEPROM
    mi32_medCnt = 1;
    mi32_medSum = ai32_initialVal;
  }
  else
  {
    mi32_medCnt = mi32_medSum = 0;
  }

  mi32_integ = 0;

  mlist_thresholdInfo.clear();
}

#ifdef USE_FLOAT_DATA_TYPE
/** initialise this MeasureProgLocal_c instance to a well defined initial state
    @param apc_processData optional pointer to containing ProcDataLocal_c instance (def NULL)
    @param af_masterVal actual central local measured value used as float masterVal
    @param af_eepromVal optional value stored in EEPROM (default 0.0)
    @param aui8_callerISOName optional ISOName of remote member, which caused creation of this instance (default 0xFF == no member)
  */
void MeasureProgLocal_c::init(
  ProcDataBase_c *const apc_processData,
  float af_masterVal,
  float af_eepromVal, const IsoName_c& ac_callerISOName)
{
  MeasureProgBase_c::init( apc_processData, af_eepromVal, ac_callerISOName  );

  f_lastMasterVal = af_masterVal;
  if (af_eepromVal != 0)
  { // set medSum, medCnt to initial value read from EEPROM
    mi32_medCnt = 1;
    f_medSum = af_eepromVal;
  }
  else
  {
    mi32_medCnt = 0;
    f_medSum = 0.0F;
  }
}
#endif


/** assignment of MeasureProgLocal_c objects
    @param arc_src source MeasureProgLocal_c instance
    @return reference to source instance for assignment like "prog1 = prog2 = prog3;"
  */
const MeasureProgLocal_c& MeasureProgLocal_c::operator=(const MeasureProgLocal_c& arc_src){
  // call base class operator
  MeasureProgBase_c::operator=(arc_src);

  assignFromSource( arc_src );

  // return reference to source
  return arc_src;
}

/** copy constructor for MeasureProgLocal
    @param arc_src source MeasureProgLocal_c instance
  */
MeasureProgLocal_c::MeasureProgLocal_c(const MeasureProgLocal_c& arc_src)
   : MeasureProgBase_c(arc_src){
  assignFromSource( arc_src );
}


/** base function for assignment of element vars for copy constructor and operator= */
void MeasureProgLocal_c::assignFromSource( const MeasureProgLocal_c& arc_src )
{ // copy element vars
  mb_triggeredIncrement = arc_src.mb_triggeredIncrement;
  mi32_lastMasterVal = arc_src.mi32_lastMasterVal;
  mi32_medCnt = arc_src.mi32_medCnt;
  mi32_medSum = arc_src.mi32_medSum;
  mi32_integ = arc_src.mi32_integ;
}


/** default destructor which has nothing to do */
MeasureProgLocal_c::~MeasureProgLocal_c(){
}

/** start a measuring programm with new master measurement value

    possible errors:
        * dependant error in ProcDataLocal_c if EMPF or SEND not valid
        * dependant error in CanIo_c on send problems

    @param ren_type used increment types: Proc_c::TimeProp, Proc_c::DistProp, ...
    @param ren_doSend value types to send on trigger of subprog: Proc_c::DoNone, Proc_c::DoVal, Proc_c::DoValForExactSetpoint...
    @param ai32_masterVal actual master value to start with
    @return true -> starting values sent with success
  */
bool MeasureProgLocal_c::start(Proc_c::type_t ren_type,
                        Proc_c::doSend_t ren_doSend, int32_t ai32_masterVal){
  // call start function of base class
  MeasureProgBase_c::start(ren_type, ren_doSend);
  mi32_lastMasterVal = ai32_masterVal;
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
        men_accumProp = Proc_c::AccumTime;
        break;
      #if defined(USE_BASE) || defined(USE_TRACTOR_MOVE)
      case Proc_c::DistProp:
        pc_iter->start(int32_t(getTracMoveInstance4Comm().distTheor()));
        men_accumProp = Proc_c::AccumDist;
        break;
      #endif
      case Proc_c::MaximumThreshold:
      {
        pc_iter->start();
        b_sendVal = FALSE; // do not send value when a threshold is set
        const ThresholdInfo_s s_thresholdInfo = {ren_type, ren_doSend, pc_iter->increment(), TRUE};
        mlist_thresholdInfo.push_front(s_thresholdInfo);
        break;
      }
      case Proc_c::MinimumThreshold:
      {
        pc_iter->start();
        b_sendVal = FALSE;  // do not send value when a threshold is set
        const ThresholdInfo_s s_thresholdInfo = {ren_type, ren_doSend, pc_iter->increment(), FALSE};
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
    mb_triggeredIncrement = (sendRegisteredVals(ren_doSend))? false:true;

  // set the timer period for process_c to a low value (maybe the new programm triggers soon)
  getProcessInstance4Comm().resetTimerPeriod();

  // return if successful sent starting values
  return mb_triggeredIncrement;
}


#ifdef USE_FLOAT_DATA_TYPE
/** start a measuring programm with new master measurement value

    possible errors:
        * dependant error in ProcDataLocal_c if EMPF or SEND not valid
        * dependant error in CanIo_c on send problems

    @param ren_type used increment types: Proc_c::TimeProp, Proc_c::DistProp, ...
    @param ren_doSend value types to send on trigger of subprog: Proc_c::DoNone, Proc_c::DoVal, Proc_c::DoValForExactSetpoint...
    @param af_masterVal actual master value to start with
    @return true -> starting values sent with success
  */
bool MeasureProgLocal_c::start(Proc_c::type_t ren_type, Proc_c::doSend_t ren_doSend, float af_masterVal)
{
  // call start function of base class
  MeasureProgBase_c::start(ren_type, ren_doSend);
  f_lastMasterVal = af_masterVal;
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
        men_accumProp = Proc_c::AccumTime;
        break;
      #if defined(USE_BASE) || defined(USE_TRACTOR_MOVE)
      case Proc_c::DistProp:
        pc_iter->start(int32_t(getTracMoveInstance4Comm().distTheor()));
        men_accumProp = Proc_c::AccumDist;
        break;
      #endif
      case Proc_c::MaximumThreshold:
      {
        pc_iter->start();
        b_sendVal = FALSE; // do not send value when a threshold is set
        const ThresholdInfo_s s_thresholdInfo = {ren_type, ren_doSend, pc_iter->increment(), TRUE};
        mlist_thresholdInfo.push_front(s_thresholdInfo);
        break;
      }
      case Proc_c::MinimumThreshold:
      {
        pc_iter->start();
        b_sendVal = FALSE;  // do not send value when a threshold is set
        const ThresholdInfo_s s_thresholdInfo = {ren_type, ren_doSend, pc_iter->increment(), FALSE};
        mlist_thresholdInfo.push_front(s_thresholdInfo);
        break;
      }
      case Proc_c::NullType: break; // just to make compiler happy
    } // switch
  } // for

  // send first values: if now without success mark for later resend with true
  if (b_sendVal)
    mb_triggeredIncrement = (sendRegisteredVals(ren_doSend))? false:true;

  // return if successful sent starting values
  return mb_triggeredIncrement;
}
#endif


/** start a measuring program without new master measurement value

    possible errors:
        * dependant error in ProcDataLocal_c if EMPF or SEND not valid
        * dependant error in CanIo_c on send problems

    @param ren_type used increment types: Proc_c::TimeProp, Proc_c::DistProp, ...
    @param ren_doSend value types to send on trigger of subprog: Proc_c::DoNone, Proc_c::DoVal, Proc_c::DoValForExactSetpoint...
    @return true -> starting values sent with success
  */
bool MeasureProgLocal_c::start(Proc_c::type_t ren_type,
                        Proc_c::doSend_t ren_doSend){
#ifdef USE_FLOAT_DATA_TYPE
  if (processData().valType() != float_val) return start(ren_type, ren_doSend, mi32_lastMasterVal);
  else return start(ren_type, ren_doSend, f_lastMasterVal);
#else
  return start(ren_type, ren_doSend, mi32_lastMasterVal);
#endif
}


/** stop local measuring programs -> send actual values

    possible errors:
        * dependant error in ProcDataLocal_c if EMPF or SEND not valid
        * dependant error in CanIo_c on send problems
    @param b_deleteSubProgs is only needed for remote ISO case (but is needed due to overloading here also)
    @param ren_type wanted increment type (Proc_c::TimeProp, Proc_c::DistProp, ...
    @param ren_doSend set process data subtype to stop (Proc_c::DoNone, Proc_c::DoVal, Proc_c::DoValForExactSetpoint...)
    @return true -> stop values sent with success
  */
bool MeasureProgLocal_c::stop(bool /* b_deleteSubProgs */, Proc_c::type_t ren_type, Proc_c::doSend_t ren_doSend){
  // send the registered values
  bool b_sendResult = TRUE;
  if ((Proc_c::MinimumThreshold != ren_type) && (Proc_c::MaximumThreshold != ren_type)
      && (Proc_c::NullType != ren_type) ) // do not send values for threshold resets and NullType
  {
    b_sendResult = sendRegisteredVals();
  }

  if (Proc_c::NullType == ren_type)
    MeasureProgBase_c::stop();
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

  return b_sendResult;
}

/** send a sub-information (selected by en_valueGroup) to a specified target (selected by GPT)
    @param en_valueGroup value group to send
    @param ac_targetISOName ISOName of target
    @return true -> successful sent
  */
bool MeasureProgLocal_c::sendValForGroup( ProcessCmd_c::ValueGroup_t en_valueGroup, const IsoName_c& ac_targetISOName) const {
  // prepare general command in process pkg
  getProcessInstance4Comm().data().mc_processCmd.setValues(false /* isSetpoint */, false, /* isRequest */
                                                              en_valueGroup, ProcessCmd_c::setValue);
#ifdef USE_FLOAT_DATA_TYPE
  if (processDataConst().valType() != float_val)
     return processDataConst().sendValISOName(ac_targetISOName, valForGroup(en_valueGroup));
  else return processDataConst().sendValISOName(ac_targetISOName, valForGroupFloat(en_valueGroup));
#else
  return processDataConst().sendValISOName(ac_targetISOName, valForGroup(en_valueGroup));
#endif
}


/** send a sub-information from the corresponding setpoint master to a specified target (selected by GPT)
    @param en_valueGroup value group to send
    @param ac_targetISOName ISOName of target
    @return true -> successful sent
  */
bool MeasureProgLocal_c::sendSetpointValForGroup( ProcessCmd_c::ValueGroup_t en_valueGroup, const IsoName_c& ac_targetISOName) const {
  // prepare general command in process pkg
  getProcessInstance4Comm().data().mc_processCmd.setValues(TRUE /* isSetpoint */, false, /* isRequest */
                                                              en_valueGroup, ProcessCmd_c::setValue);
  return processDataConst().sendValISOName(ac_targetISOName, setpointValForGroup(en_valueGroup));
}

/** deliver to en_valueGroup according setpoint from a master setpoint
    @param en_valueGroup of wanted subtype
    @return value of specified subtype
  */
int32_t MeasureProgLocal_c::setpointValForGroup(ProcessCmd_c::ValueGroup_t en_valueGroup) const {
  int32_t i32_value = 0;
  ProcDataLocalBase_c* pc_procdata = pprocessData();
  if (pc_procdata->setpointExistMaster())
  {
    switch (en_valueGroup)
    {
      case ProcessCmd_c::exactValue:
        i32_value = pc_procdata->setpointExactValue();
        break;
      case ProcessCmd_c::defaultValue:
        i32_value = pc_procdata->setpointDefaultValue();
        break;
      case ProcessCmd_c::minValue:
        i32_value = pc_procdata->setpointMinValue();
        break;
      case ProcessCmd_c::maxValue:
        i32_value = pc_procdata->setpointMaxValue();
        break;
      default:
        // wrong range
        getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::Process );
    }
  }
  return i32_value;
}


/** process a message: reset command or value requests

    possible errors:
      * dependant error in ProcDataLocal_c if EMPF or SEND not valid
      * dependant error in CanIo_c on send problems
    @return true -> received msg processed by this instance
  */
bool MeasureProgLocal_c::processMsg(){
  bool b_result = MeasureProgBase_c::processMsg();

  // call base function - if base function returns true, nothing else must be done
  if (!b_result)
  { // only use the local vars if needed
    ProcessPkg_c& c_pkg = getProcessInstance4Comm().data();

    // the message was a value message -> evaluate it here
    if ( c_pkg.mc_processCmd.getCommand() == ProcessCmd_c::setValue)
    { // write - accept only write actions to local data only if this is reset try
      // ISO: value in message contains reset value
      const int32_t ci32_val = c_pkg.dataRawCmdLong();
      resetValForGroup(c_pkg.mc_processCmd.getValueGroup(), ci32_val);

      if (Proc_c::defaultDataLoggingDDI == c_pkg.DDI())
      { // setValue command for default data logging DDI stops measurement (same as TC task status "suspended")
        getProcessInstance4Comm().processTcStatusMsg(0, c_pkg.memberSend().isoName(), TRUE /* ab_skipLastTcStatus */);
      }

      // resetted val is automatically sent
      b_result = true;
      // call handler function if handler class is registered
      if ( processDataConst().getProcessDataChangeHandler() != NULL )
        processDataConst().getProcessDataChangeHandler()->processMeasurementReset( pprocessData(), ci32_val, c_pkg.memberSend().isoName().toConstIisoName_c());
    } // write
    else
    { // read -> answer wanted value
      sendValForGroup( c_pkg.mc_processCmd.getValueGroup(), c_pkg.memberSend().isoName() );

      if ((Proc_c::defaultDataLoggingDDI == c_pkg.DDI()) &&
          (processDataConst().getProcessDataChangeHandler() != NULL ))
        // call handler function if handler class is registered
        processDataConst().getProcessDataChangeHandler()->processDefaultLoggingStart( pprocessData(), processData().pkgDataLong(), c_pkg.memberSend().isoName().toConstIisoName_c() );


      b_result = true;
    } // read
  }
  return b_result;
}


/**
  constructor which can optionally set most element vars of MeasureProgLocal
  @param apc_processData optional pointer to containing ProcDataLocal_c instance (def NULL)
  @param ai32_masterVal optional actual central local measured value used as masterVal (def 0)
  @param ai32_initialVal optional initial value (e.g which was stored in EEPROM) (default 0)
  @param ac_callerISOName optional ISOName of remote member, which caused creation of this instance (default 0xFF == no member)
*/
MeasureProgLocal_c::MeasureProgLocal_c(
  ProcDataBase_c *const apc_processData,
  int32_t ai32_masterVal,
  int32_t ai32_initialVal,
  const IsoName_c& ac_callerISOName)
: MeasureProgBase_c(apc_processData, ai32_initialVal, ac_callerISOName )
{
  init( apc_processData, ai32_masterVal, ai32_initialVal, ac_callerISOName );
}


/** set the measure prog value and send values if triggered to do

    possible errors:

        * dependant error in ProcDataLocal_c if EMPF or SEND not valid
        * dependant error in CanIo_c on send problems
    @param ai32_val new measure value
  */
void MeasureProgLocal_c::setVal(int32_t ai32_val){
  int32_t i32_incr =  ai32_val - mi32_lastMasterVal;
  int32_t i32_time =  System_c::getTime();
  int32_t i32_timeDelta = i32_time - mi32_lastTime;
  int32_t i32_oldDelta = mi32_delta;
  mi32_lastTime = i32_time;

  // update values:
  // for cumulative values -> update mi32_val by increment
  // else take the actual given master value
  mi32_lastMasterVal = ai32_val;
  if (processData().mb_cumulativeValue) mi32_val += i32_incr;
  else mi32_val = ai32_val;

  // update min max
  if (val() > max()) setMax(val());
  if (val() < min()) setMin(val());

  // claculate delta and accel in 1/s
  if (i32_timeDelta > 0)
  { // if two calls to this function follow immediate
    mi32_delta = (i32_incr * 1000) / i32_timeDelta;
    mi32_accel = ((mi32_delta - i32_oldDelta) * 1000) / i32_timeDelta;

  }

  // now check if one subprog triggers
  bool b_singleTest = false;
  for (Vec_MeasureSubprogIterator pc_iter = mvec_measureSubprog.begin();
       pc_iter != mvec_measureSubprog.end(); pc_iter++)
  {
    mb_triggeredIncrement = false;
    switch (pc_iter->type())
    {
      case Proc_c::TimeProp:
        b_singleTest = pc_iter->updateTrigger(i32_time);
        mb_triggeredIncrement = (b_singleTest)? true : mb_triggeredIncrement;
        // update med/integ
        updatePropDepVals();
        break;
      case Proc_c::DistProp:
        #if defined(USE_BASE) || defined(USE_TRACTOR_MOVE)
        b_singleTest = pc_iter->updateTrigger(int32_t(getTracMoveInstance4Comm().distTheor()));
        #endif
        mb_triggeredIncrement = (b_singleTest)? true : mb_triggeredIncrement;
        // update med/integ
        updatePropDepVals();
        break;
      case Proc_c::OnChange:
        b_singleTest = pc_iter->updateTrigger(val());
        mb_triggeredIncrement = (b_singleTest)? true : mb_triggeredIncrement;
        break;
      case Proc_c::NullType: break; // just to make compiler happy
      default: ;
    } // switch

    // if mb_triggeredIncrement == true the registered values should be sent
    if (mb_triggeredIncrement)
    { // send the registered values
      if (!minMaxLimitsPassed(pc_iter->doSend()))
      {
        // omit this value send
        mb_triggeredIncrement = false;
        continue;
      }
      // if at least one send try had success reset mb_triggeredIncrement
      if (sendRegisteredVals()) mb_triggeredIncrement = false;
    }
  } // for
}


#ifdef USE_FLOAT_DATA_TYPE
/** set the measure prog value and send values if triggered to do

    possible errors:
        * dependant error in ProcDataLocal_c if EMPF or SEND not valid
        * dependant error in CanIo_c on send problems
    @param af_val new measure value
  */
void MeasureProgLocal_c::setVal(float af_val){
  float f_incr =  af_val - f_lastMasterVal;
  int32_t i32_time =  System_c::getTime();
  int32_t i32_timeDelta = i32_time - mi32_lastTime;
  float f_oldDelta = f_delta;
  mi32_lastTime = i32_time;

  // update values:
  // for cumulative values -> update f_val by increment
  // else take the actual given master value
  f_lastMasterVal = af_val;
  if (processData().mb_cumulativeValue) f_val += f_incr;
  else f_val = af_val;

  // update min max
  if (valFloat() > maxFloat()) setMax(valFloat());
  if (valFloat() < minFloat()) setMin(valFloat());

  // claculate delta and accel in 1/s
  if (i32_timeDelta > 0)
  { // if two calls to this function follow immediate
    f_delta = (f_incr * 1000.0F) / (float)i32_timeDelta;
    f_accel = ((f_delta - f_oldDelta) * 1000.0F) / (float)i32_timeDelta;
  }

  // now check if one subprog triggers
  bool b_singleTest = false;
  for (Vec_MeasureSubprogIterator pc_iter = mvec_measureSubprog.begin();
       pc_iter != mvec_measureSubprog.end(); pc_iter++)
  {
    mb_triggeredIncrement = false;
    switch (pc_iter->type())
    {
      case Proc_c::TimeProp:
        b_singleTest = pc_iter->updateTrigger(i32_time);
        mb_triggeredIncrement = (b_singleTest)? true : mb_triggeredIncrement;
        // update med/integ
        updatePropDepVals();
        break;
      case Proc_c::DistProp:
        #if defined(USE_BASE) || defined(USE_TRACTOR_MOVE)
        b_singleTest = pc_iter->updateTrigger(int32_t(getTracMoveInstance4Comm().distTheor()));
        #endif
        mb_triggeredIncrement = (b_singleTest)? true : mb_triggeredIncrement;
        // update med/integ
        updatePropDepVals();
        break;

      case Proc_c::OnChange:
        b_singleTest = pc_iter->updateTrigger(valFloat());
        mb_triggeredIncrement = (b_singleTest)? true : mb_triggeredIncrement;
        break;
      case Proc_c::NullType: break; // just to make compiler happy
      default: ;
    } // switch

    // if mb_triggeredIncrement == true the registered values should be sent
    if (mb_triggeredIncrement)
    { // send the registered values
      if (!minMaxLimitsPassed(pc_iter->doSend()))
      {
        // omit this value send
        mb_triggeredIncrement = false;
        return;
      }

      // if at least one send try had success reset mb_triggeredIncrement
      if (sendRegisteredVals()) mb_triggeredIncrement = false;
    }
  } // for
}
#endif


/** send the values which are registered by a running mesuring program

    possible errors:
      * dependant error in ProcDataLocal_c if EMPF or SEND not valid
      * dependant error in CanIo_c on send problems
    @param ren_doSend value types to send on trigger of subprog: Proc_c::DoNone, Proc_c::DoVal, Proc_c::DoValForExactSetpoint...
    @return true -> value send triggered and performed with success
  */
bool MeasureProgLocal_c::sendRegisteredVals(Proc_c::doSend_t ren_doSend){
  bool b_success = false;

  ProcessCmd_c::ValueGroup_t en_valueGroup;

  switch (ren_doSend)
  {
    case Proc_c::DoValForDefaultSetpoint: en_valueGroup = ProcessCmd_c::defaultValue; break;
    case Proc_c::DoValForMinSetpoint:
    case Proc_c::DoValForMinMeasurement:
      en_valueGroup = ProcessCmd_c::minValue; break;
    case Proc_c::DoValForMaxSetpoint:
    case Proc_c::DoValForMaxMeasurement:
      en_valueGroup = ProcessCmd_c::maxValue; break;
    case Proc_c::DoValForExactSetpoint:   en_valueGroup = ProcessCmd_c::exactValue; break;
    default:                              en_valueGroup = ProcessCmd_c::noValue;
  }

  if (ProcessCmd_c::noValue != en_valueGroup)
    // get value from corresponding setpoint and send it
    b_success = (sendSetpointValForGroup( en_valueGroup, isoName()))?true : b_success;

  // normal measurement (no measurement on setpoint DDI)
  if (Proc_c::DoVal == ren_doSend)
    b_success = (sendValForGroup( ProcessCmd_c::exactValue, isoName()))?true : b_success;

  return b_success;
}


/** init the element vals
    @param ai32_val initial measure val
  */
void MeasureProgLocal_c::initVal(int32_t ai32_val){
  // first call the base function
  MeasureProgBase_c::initVal(ai32_val);

  mi32_medSum = mi32_integ = ai32_val;
  mi32_medCnt = 1;
}


#ifdef USE_FLOAT_DATA_TYPE
/** init the element vals
    @param af_val initial measure val
  */
void MeasureProgLocal_c::initVal(float af_val){
  // first call the base function
  MeasureProgBase_c::initVal(af_val);

  f_integ = f_medSum = af_val;
  mi32_medCnt = 1;
}
#endif


/** reset the local value

    possible errors:
      * dependant error in ProcDataLocal_c if EMPF or SEND not valid
      * dependant error in CanIo_c on send problems
    @param ai32_val reset measure value to this value
    @return true -> reseted measure val sent with success
  */
bool MeasureProgLocal_c::resetVal(int32_t ai32_val){
  // send resetted val
  bool b_sendSuccess;

  // prepare general command in process pkg
  getProcessInstance4Comm().data().mc_processCmd.setValues(false /* isSetpoint */, false, /* isRequest */
                                                              ProcessCmd_c::exactValue, ProcessCmd_c::setValue);
#ifdef USE_FLOAT_DATA_TYPE
  if (processData().valType() != float_val)
  {
#endif
    // allow reset with value
    //mi32_val = 0;
    mi32_val = ai32_val;

    b_sendSuccess = processData().sendValISOName(mc_isoName, val());
#ifdef USE_FLOAT_DATA_TYPE
  }
  else
  {
    f_val = 0;
    b_sendSuccess = processData().sendValISOName(mc_isoName, valFloat());
  }
#endif
  #ifdef USE_EEPROM_IO
  // call reset function for ProcessData -> if this prog is the first one reset eepromVal
  // processData().resetEeprom(this);
  // simply call resetEeprom(void) to get independent from ProcDataLocal_c
  // -> each active measureprog item can command a reset
  processData().resetEeprom();
  #endif
  return b_sendSuccess;
}


/** reset the local intgral value

    possible errors:
      * dependant error in ProcDataLocal_c if EMPF or SEND not valid
      * dependant error in CanIo_c on send problems
    @return true -> reseted integ val sent with success
  */
bool MeasureProgLocal_c::resetInteg(){

#ifdef USE_FLOAT_DATA_TYPE
  if (processData().valType() != float_val)
  {
#endif
    mi32_integ = 0;
#ifdef USE_FLOAT_DATA_TYPE
  }
  else
  {
    f_integ = 0;
  }
#endif
  // do not send a message to remote instance
  return true;
}


/** reset the local medium value

    possible errors:
      * dependant error in ProcDataLocal_c if EMPF or SEND not valid
      * dependant error in CanIo_c on send problems
    @return true -> reseted medium val sent with success
  */
bool MeasureProgLocal_c::resetMed(){
  mi32_medCnt = 0;

#ifdef USE_FLOAT_DATA_TYPE
  if (processData().valType() != float_val)
  {
#endif
    mi32_medSum = 0;
#ifdef USE_FLOAT_DATA_TYPE
  }
  else
  {
    f_medSum = 0;
  }
#endif
  // do not send a message to remote instance
  return true;
}


/** reset the local min value

    possible errors:
      * dependant error in ProcDataLocal_c if EMPF or SEND not valid
      * dependant error in CanIo_c on send problems
    @return true -> reseted minimum val sent with success
  */
bool MeasureProgLocal_c::resetMin(){

  // prepare general command in process pkg
  getProcessInstance4Comm().data().mc_processCmd.setValues(false /* isSetpoint */, false, /* isRequest */
                                                              ProcessCmd_c::minValue, ProcessCmd_c::setValue);

#ifdef USE_FLOAT_DATA_TYPE
  if (processData().valType() != float_val)
  {
#endif
  mi32_min = 0;
  return processData().sendValISOName(mc_isoName, min());
#ifdef USE_FLOAT_DATA_TYPE
  }
  else
  {
    f_min = 0;
    return processData().sendValISOName(mc_isoName, minFloat());
  }
#endif
}


/** reset the local max value

    possible errors:
      * dependant error in ProcDataLocal_c if EMPF or SEND not valid
      * dependant error in CanIo_c on send problems
    @return true -> reseted maximum val sent with success
  */
bool MeasureProgLocal_c::resetMax(){

  // prepare general command in process pkg
  getProcessInstance4Comm().data().mc_processCmd.setValues(false /* isSetpoint */, false, /* isRequest */
                                                              ProcessCmd_c::maxValue, ProcessCmd_c::setValue);

#ifdef USE_FLOAT_DATA_TYPE
  if (processData().valType() != float_val)
  {
#endif
  mi32_max = 0;
  return processData().sendValISOName(mc_isoName, max());
#ifdef USE_FLOAT_DATA_TYPE
  }
  else
  {
    f_max = 0;
    return processData().sendValISOName(mc_isoName, maxFloat());
  }
#endif
}


/** periodic events
    (e.g. send value for time proportional progs)

    possible errors:
      * dependant error in ProcDataLocal_c if EMPF or SEND not valid
      * dependant error in CanIo_c on send problems
    @param pui16_nextTimePeriod calculated new time period, based on current measure progs (only for local proc data)
    @return true -> all planned activities performed in available time
  */
bool MeasureProgLocal_c::timeEvent( uint16_t *pui16_nextTimePeriod )
{
  if ( Scheduler_Task_c::getAvailableExecTime() == 0 ) return false;
  int32_t i32_time = Scheduler_Task_c::getLastRetriggerTime();

  bool b_singleTest;
  int32_t i32_nextTimePeriod;
  int32_t i32_distTheor;

  for (Vec_MeasureSubprogIterator pc_iter = mvec_measureSubprog.begin(); pc_iter != mvec_measureSubprog.end(); pc_iter++)
  {
    mb_triggeredIncrement = false;
    b_singleTest = false;
    i32_nextTimePeriod = 0;
    switch (pc_iter->type())
    {
      case Proc_c::TimeProp:
        b_singleTest = pc_iter->updateTrigger(i32_time);
        // if b_singleTest is true set mb_triggeredIncrement to true,
        // otherwise let it unchanged
        mb_triggeredIncrement = (b_singleTest)? true : mb_triggeredIncrement;
        // update med/integ
        if ((b_singleTest)&&(men_accumProp == Proc_c::AccumTime))updatePropDepVals();

        // calculate next timer period
        i32_nextTimePeriod = pc_iter->nextTriggerTime(i32_time);

        break;
      case Proc_c::DistProp:
        #if defined(USE_BASE) || defined(USE_TRACTOR_MOVE)
        i32_distTheor = getTracMoveInstance4Comm().distTheor();
        b_singleTest = pc_iter->updateTrigger(i32_distTheor);
        #else
        i32_distTheor = 0;
        #endif
        mb_triggeredIncrement = (b_singleTest)? true : mb_triggeredIncrement;
        // update med/integ
        if ((b_singleTest)&&(men_accumProp == Proc_c::AccumTime))updatePropDepVals();

        // calculate next timer period
        i32_nextTimePeriod = pc_iter->nextTriggerTime(i32_distTheor);

        break;
      case Proc_c::OnChange:
        b_singleTest = pc_iter->updateTrigger(val());
        mb_triggeredIncrement = (b_singleTest)? true : mb_triggeredIncrement;
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

    // if mb_triggeredIncrement == true the registered values should be sent
    // if needed an old unsuccessfull send try is redone (deactivated!)
    if (mb_triggeredIncrement)
    {
      if (!minMaxLimitsPassed(pc_iter->doSend()))
      {
        // omit this value send
        mb_triggeredIncrement = false;
        continue;
      }
      // send the registered values
      // if at least one send try had success reset mb_triggeredIncrement
      if (sendRegisteredVals(pc_iter->doSend())) mb_triggeredIncrement = false;
    }
  }

  return true;
}


bool MeasureProgLocal_c::minMaxLimitsPassed(Proc_c::doSend_t ren_doSend) const
{
  bool b_checkMin = FALSE;
  bool b_checkMax = FALSE;
  int32_t i32_maxVal = 0;
  int32_t i32_minVal = 0;

  for (List_ThresholdInfoConstIterator ps_iterThreshold = mlist_thresholdInfo.begin(); ps_iterThreshold != mlist_thresholdInfo.end(); ps_iterThreshold++)
  {
    if (ps_iterThreshold->en_doSend == ren_doSend)
    {
      switch (ps_iterThreshold->en_type)
      {
        case Proc_c::MaximumThreshold: b_checkMax = TRUE; i32_maxVal = ps_iterThreshold->i32_threshold; break;
        case Proc_c::MinimumThreshold: b_checkMin = TRUE; i32_minVal = ps_iterThreshold->i32_threshold; break;
        default: ;
      }
    }
  }

  if ( b_checkMin && b_checkMax && (i32_maxVal < i32_minVal) && ((i32_maxVal >= val()) || (i32_minVal <= val())) )
    return TRUE;

  if ( (b_checkMin && i32_minVal > val() ) ||
       (b_checkMax && i32_maxVal < val() ) )
    return FALSE;

  return TRUE;
}


/** update proportional dependent values */
void MeasureProgLocal_c::updatePropDepVals(){
#ifdef USE_FLOAT_DATA_TYPE
  if (processData().valType() != float_val)
  {
#endif
  incrInteg(val()); // increment integral
  // update medium based on actual val and count of actual calculated vals
  incrMedSum(val());
#ifdef USE_FLOAT_DATA_TYPE
  }
  else
  {
    incrInteg(valFloat()); // increment integral
    // update medium based on actual val and count of actual calculated vals
    incrMedSum(valFloat());
  }
#endif

  incrMedCnt(); // increment medCnt to calc medium from integ
}


} // end of namespace __IsoAgLib
