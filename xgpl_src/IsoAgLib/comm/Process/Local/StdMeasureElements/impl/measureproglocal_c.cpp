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
#include <IsoAgLib/comm/Base/impl/base_c.h>

namespace __IsoAgLib {

/**
  initialise this MeasureProgLocal_c instance to a well defined initial state
  @param rpc_processData optional pointer to containing ProcDataLocal_c instance (def NULL)
  @param ren_progType optional program msg type (Proc_c::Base, Proc_c::Target; default Proc_c::UndefinedProg)
  @param ri32_masterVal optional actual central local measured value used as masterVal (def 0)
  @param ri32_initialVal optional initial value (e.g which was stored in EEPROM) (default 0)
  @param rui8_callerGtp optional GETY_POS of remote member, which caused creation of this instance (default 0xFF == no member)
*/
void MeasureProgLocal_c::init(
  ProcDataBase_c *const rpc_processData,
  Proc_c::progType_t ren_progType,
  int32_t ri32_masterVal,
  int32_t ri32_initialVal,
  GetyPos_c rc_callerGtp)
{
  MeasureProgBase_c::init( rpc_processData, ren_progType, ri32_initialVal, rc_callerGtp  );

  i32_lastMasterVal = ri32_masterVal;
  if (ri32_initialVal != 0)
  { // set medSum, medCnt to initial value read from EEPROM
    i32_medCnt = 1;
    i32_medSum = ri32_initialVal;
  }
  else
  {
    i32_medCnt = i32_medSum = 0;
  }
}

#ifdef USE_FLOAT_DATA_TYPE
/**
  initialise this MeasureProgLocal_c instance to a well defined initial state
  @param rpc_processData optional pointer to containing ProcDataLocal_c instance (def NULL)

  @param ren_progType optional program msg type (Proc_c::Base, Proc_c::Target; default Proc_c::UndefinedProg)
  @param rf_masterVal actual central local measured value used as float masterVal
  @param rf_eepromVal optional value stored in EEPROM (default 0.0)
  @param rui8_callerGtp optional GETY_POS of remote member, which caused creation of this instance (default 0xFF == no member)
*/
void MeasureProgLocal_c::init(
  ProcDataBase_c *const rpc_processData,
  Proc_c::progType_t ren_progType, float rf_masterVal,
  float rf_eepromVal, GetyPos_c rc_callerGtp)
{
  MeasureProgBase_c::init( rpc_processData, ren_progType, rf_eepromVal, rc_callerGtp  );

  f_lastMasterVal = rf_masterVal;
  if (rf_eepromVal != 0)
  { // set medSum, medCnt to initial value read from EEPROM
    i32_medCnt = 1;
    f_medSum = rf_eepromVal;
  }
  else
  {
    i32_medCnt = 0;
    f_medSum = 0.0F;
  }
}

#endif
/**
  assignment of MeasureProgLocal_c objects
  @param rrefc_src source MeasureProgLocal_c instance
  @return reference to source instance for assignment like "prog1 = prog2 = prog3;"
*/
const MeasureProgLocal_c& MeasureProgLocal_c::operator=(const MeasureProgLocal_c& rrefc_src){
  // call base class operator
  MeasureProgBase_c::operator=(rrefc_src);

  assignFromSource( rrefc_src );

  // return reference to source
  return rrefc_src;
}

/**
  copy constructor for MeasureProgLocal
  @param rrefc_src source MeasureProgLocal_c instance
*/
MeasureProgLocal_c::MeasureProgLocal_c(const MeasureProgLocal_c& rrefc_src)
   : MeasureProgBase_c(rrefc_src){
  assignFromSource( rrefc_src );
}
/** base function for assignment of element vars for copy constructor and operator= */
void MeasureProgLocal_c::assignFromSource( const MeasureProgLocal_c& rrefc_src )
{ // copy element vars
  b_triggeredIncrement = rrefc_src.b_triggeredIncrement;
  i32_lastMasterVal = rrefc_src.i32_lastMasterVal;
  i32_medCnt = rrefc_src.i32_medCnt;
  i32_medSum = rrefc_src.i32_medSum;
}


/** default destructor which has nothing to do */
MeasureProgLocal_c::~MeasureProgLocal_c(){
}

/**
  start a measuring programm with new master measurement value

  possible errors:
      * dependant error in ProcDataLocal_c if EMPF or SEND not valid
      * dependant error in CANIO_c on send problems
  @param ren_progType process msg type: Proc_c::Base, Proc_c::Target
  @param ren_type used increment types: Proc_c::TimeProp, Proc_c::DistProp, Proc_c::ValIncr
  @param ren_doSend value types to send on trigger of subprog: Proc_c::DoNone, Proc_c::DoVal, Proc_c::DoMed, Proc_c::DoInteg
  @param ri32_masterVal actual master value to start with
  @return true -> starting values sent with success
*/
bool MeasureProgLocal_c::start(Proc_c::progType_t ren_progType, Proc_c::type_t ren_type,
                        Proc_c::doSend_t ren_doSend, int32_t ri32_masterVal){
  // call start function of base class
  MeasureProgBase_c::start(ren_progType, ren_type, ren_doSend);
  i32_lastMasterVal = ri32_masterVal;

  // start the given subprog items
  for (Vec_MeasureSubprogIterator pc_iter = vec_measureSubprog.begin(); pc_iter != vec_measureSubprog.end(); pc_iter++)
  {
    switch (pc_iter->type() & ren_type)
    {
      case Proc_c::TimeProp:
        pc_iter->start(System_c::getTime());
        en_accumProp = Proc_c::AccumTime;
        break;
      #ifdef USE_BASE
			case Proc_c::DistProp:
        pc_iter->start(getBaseInstance4Comm().distTheor());
        en_accumProp = Proc_c::AccumDist;
        break;
			#endif
      case Proc_c::ValIncr:
        pc_iter->start(val());
        break;
      case Proc_c::DeltaIncr:
        pc_iter->start(i32_delta);
        break;
      case Proc_c::AccelIncr:
        pc_iter->start(i32_accel);
        break;
      case Proc_c::MedIncr:
        pc_iter->start(med());
        break;
      case Proc_c::MinIncr:
        pc_iter->start(min());
        break;
      case Proc_c::MaxIncr:
        pc_iter->start(max());
        break;
      case Proc_c::IntegIncr:
        pc_iter->start(integ());
        break;
      case Proc_c::NullType: break; // just to make compiler happy
    } // switch
  } // for

  // send first values: if now without success mark for later resend with true
  b_triggeredIncrement = (sendRegisteredVals())? false:true;

  // return if successful sent starting values
  return b_triggeredIncrement;
}
#ifdef USE_FLOAT_DATA_TYPE
/**
  start a measuring programm with new master measurement value

  possible errors:
      * dependant error in ProcDataLocal_c if EMPF or SEND not valid
      * dependant error in CANIO_c on send problems
  @param ren_progType process msg type: Proc_c::Base, Proc_c::Target
  @param ren_type used increment types: Proc_c::TimeProp, Proc_c::DistProp, Proc_c::ValIncr
  @param ren_doSend value types to send on trigger of subprog: Proc_c::DoNone, Proc_c::DoVal, Proc_c::DoMed, Proc_c::DoInteg
  @param rf_masterVal actual master value to start with
  @return true -> starting values sent with success
*/
bool MeasureProgLocal_c::start(Proc_c::progType_t ren_progType, Proc_c::type_t ren_type,
                        Proc_c::doSend_t ren_doSend, float rf_masterVal){
  // call start function of base class
  MeasureProgBase_c::start(ren_progType, ren_type, ren_doSend);
  f_lastMasterVal = rf_masterVal;

  // start the given subprog items
  for (Vec_MeasureSubprogIterator pc_iter = vec_measureSubprog.begin(); pc_iter != vec_measureSubprog.end(); pc_iter++)
  {
    switch (pc_iter->type() & ren_type)
    {
      case Proc_c::TimeProp:
        pc_iter->start(System_c::getTime());
        en_accumProp = Proc_c::AccumTime;
        break;
      #ifdef USE_BASE
			case Proc_c::DistProp:
        pc_iter->start(getBaseInstance4Comm().distTheor());
        en_accumProp = Proc_c::AccumDist;
        break;
			#endif
      case Proc_c::ValIncr:
        pc_iter->start(valFloat());
        break;
      case Proc_c::DeltaIncr:
        pc_iter->start(f_delta);
        break;
      case Proc_c::AccelIncr:
        pc_iter->start(f_accel);
        break;
      case Proc_c::MedIncr:
        pc_iter->start(medFloat());
        break;
      case Proc_c::MinIncr:
        pc_iter->start(minFloat());
        break;
      case Proc_c::MaxIncr:
        pc_iter->start(maxFloat());
        break;
      case Proc_c::IntegIncr:
        pc_iter->start(integFloat());
        break;
      case Proc_c::NullType: break; // just to make compiler happy
    } // switch
  } // for

  // send first values: if now without success mark for later resend with true
  b_triggeredIncrement = (sendRegisteredVals())? false:true;

  // return if successful sent starting values
  return b_triggeredIncrement;
}
#endif

/**
  start a measuring program without new master measurement value

  possible errors:
      * dependant error in ProcDataLocal_c if EMPF or SEND not valid
      * dependant error in CANIO_c on send problems
  @param ren_progType process msg type: Proc_c::Base, Proc_c::Target
  @param ren_type used increment types: Proc_c::TimeProp, Proc_c::DistProp, Proc_c::ValIncr
  @param ren_doSend value types to send on trigger of subprog: Proc_c::DoNone, Proc_c::DoVal, Proc_c::DoMed, Proc_c::DoInteg
  @return true -> starting values sent with success
*/
bool MeasureProgLocal_c::start(Proc_c::progType_t ren_progType, Proc_c::type_t ren_type,
                        Proc_c::doSend_t ren_doSend){
#ifdef USE_FLOAT_DATA_TYPE
  if (processData().valType() != float_val) return start(ren_progType, ren_type, ren_doSend, i32_lastMasterVal);
  else return start(ren_progType, ren_type, ren_doSend, f_lastMasterVal);
#else
  return start(ren_progType, ren_type, ren_doSend, i32_lastMasterVal);
#endif
}

/**
  stop local measuring programs -> send actual values

  possible errors:
      * dependant error in ProcDataLocal_c if EMPF or SEND not valid
      * dependant error in CANIO_c on send problems
  @return true -> stop values sent with success
*/
bool MeasureProgLocal_c::stop(){
  // send the registered values
  bool b_sendResult = sendRegisteredVals();
  // call base function
  MeasureProgBase_c::stop();

  return b_sendResult;
}

/**
  send a sub-information (selected by MOD) to a specified target (selected by GPT)
  @param rui8_mod MOD code of the value type to send
  @param rc_targetGtp GetyPos of target
  @param ren_type optional PRI specifier of the message (default Proc_c::Target )
  @return true -> successful sent
*/
bool MeasureProgLocal_c::sendValMod( GeneralCommand_c::ValueGroup_t en_valueGroup, GetyPos_c rc_targetGtp, Proc_c::progType_t ren_progType ) const {
  // prepare general command in process pkg
  getProcessInstance4Comm().data().c_generalCommand.setValues(false /* isSetpoint */, false, /* isRequest */
                                                              en_valueGroup, GeneralCommand_c::setValue);
#ifdef USE_FLOAT_DATA_TYPE
  if (processDataConst().valType() != float_val)
     return processDataConst().sendValGtp(ren_progType, rc_targetGtp, valMod(en_valueGroup));
  else return processDataConst().sendValGtp(ren_progType, rc_targetGtp, valModFloat(en_valueGroup));
#else
  return processDataConst().sendValGtp(ren_progType, rc_targetGtp, valMod(en_valueGroup));
#endif
}

/**
  process a message: reset command or value requests

  possible errors:
    * dependant error in ProcDataLocal_c if EMPF or SEND not valid
    * dependant error in CANIO_c on send problems
  @return true -> received msg processed by this instance
*/
bool MeasureProgLocal_c::processMsg(){
  bool b_result = MeasureProgBase_c::processMsg();

  // call base function - if base function returns true, nothing else must be done
  if (!b_result)
  { // only use the local vars if needed
    ProcessPkg_c& c_pkg = getProcessInstance4Comm().data();
    
    // get the int32_t data val; let it convert, if needed
    int32_t i32_val = c_pkg.dataRawCmdLong();

    // the message was a value message -> evaluate it here (DIN: pd=1)
    if (c_pkg.c_generalCommand.getCommand() == GeneralCommand_c::measurementReset)
    { // write - accept only write actions to local data only if this is reset try
      // (not standard conformant, but practised)
      // i32_val == 0 is already checked in ProcessPgk::resolveCommandType()
      // try to reset value
      resetValMod(c_pkg.c_generalCommand.getValueGroup());
      // resetted val is automatically sent
      b_result = true;
      // call handler function if handler class is registered
      if ( processDataConst().getProcessDataChangeHandler() != NULL )
        processDataConst().getProcessDataChangeHandler()->processMeasurementReset( pprocessData(), 0, c_pkg.memberSend().gtp() );
    } // write
    else
    { // read -> answer wanted value
      sendValMod( c_pkg.c_generalCommand.getValueGroup(), c_pkg.memberSend().gtp(), Proc_c::progType_t(c_pkg.pri()));
      b_result = true;
    } // read
  }
  return b_result;
}

/**
  set the measure prog value and send values if triggered to do

  possible errors:

      * dependant error in ProcDataLocal_c if EMPF or SEND not valid
      * dependant error in CANIO_c on send problems
  @param ri32_val new measure value
*/
void MeasureProgLocal_c::setVal(int32_t ri32_val){
  int32_t i32_incr =  ri32_val - i32_lastMasterVal;
  int32_t i32_time =  System_c::getTime();
  int32_t i32_timeDelta = i32_time - i32_lastTime;
  int32_t i32_oldDelta = i32_delta;
  i32_lastTime = i32_time;

  // update values:
  // for cumulative values -> update i32_val by increment
  // else take the actual given master value
  i32_lastMasterVal = ri32_val;
  if (processData().b_cumulativeValue) i32_val += i32_incr;
  else i32_val = ri32_val;

  // update min max
  if (val() > max()) setMax(val());
  if (val() < min()) setMin(val());

  // claculate delta and accel in 1/s
  if (i32_timeDelta > 0)
  { // if two calls to this function follow immediate
    i32_delta = (i32_incr * 1000) / i32_timeDelta;
    i32_accel = ((i32_delta - i32_oldDelta) * 1000) / i32_timeDelta;

  }
  // now check if one subprog triggers
  bool b_singleTest;
  for (Vec_MeasureSubprogIterator pc_iter = vec_measureSubprog.begin();
       pc_iter != vec_measureSubprog.end(); pc_iter++)
  {
    switch (pc_iter->type())
    {
      case Proc_c::TimeProp:
        b_singleTest = pc_iter->updateTrigger(i32_time);
        b_triggeredIncrement = (b_singleTest)? true : b_triggeredIncrement;
        // update med/integ
        if ((b_singleTest)&&(en_accumProp == Proc_c::AccumTime))updatePropDepVals();
        break;
      case Proc_c::DistProp:
        #ifdef USE_BASE
				b_singleTest = pc_iter->updateTrigger(getBaseInstance4Comm().distTheor());
				#endif
        b_triggeredIncrement = (b_singleTest)? true : b_triggeredIncrement;
        // update med/integ
        if ((b_singleTest)&&(en_accumProp == Proc_c::AccumDist))updatePropDepVals();
        break;

      case Proc_c::ValIncr:
        b_singleTest = pc_iter->updateTrigger(val());
        b_triggeredIncrement = (b_singleTest)? true : b_triggeredIncrement;
        break;
      case Proc_c::DeltaIncr:
        b_singleTest = pc_iter->updateTrigger(i32_delta);
        b_triggeredIncrement = (b_singleTest)? true : b_triggeredIncrement;
        break;
      case Proc_c::AccelIncr:
        b_singleTest = pc_iter->updateTrigger(i32_accel);
        b_triggeredIncrement = (b_singleTest)? true : b_triggeredIncrement;
        break;
      case Proc_c::MedIncr:
        b_singleTest = pc_iter->updateTrigger(med());
        b_triggeredIncrement = (b_singleTest)? true : b_triggeredIncrement;
        break;
      case Proc_c::MinIncr:
        b_singleTest = pc_iter->updateTrigger(min());
        b_triggeredIncrement = (b_singleTest)? true : b_triggeredIncrement;
        break;
      case Proc_c::MaxIncr:
        b_singleTest = pc_iter->updateTrigger(max());
        b_triggeredIncrement = (b_singleTest)? true : b_triggeredIncrement;
        break;
      case Proc_c::IntegIncr:
        b_singleTest = pc_iter->updateTrigger(integ());
        b_triggeredIncrement = (b_singleTest)? true : b_triggeredIncrement;
        break;
      case Proc_c::NullType: break; // just to make compiler happy
    } // switch
  } // for
  // if b_triggeredIncrement == true the registered values should be sent
  if (b_triggeredIncrement)
  { // send the registered values
    // if at least one send try had success reset b_triggeredIncrement
    if (sendRegisteredVals()) b_triggeredIncrement = false;
  }
}
#ifdef USE_FLOAT_DATA_TYPE
/**
  set the measure prog value and send values if triggered to do

  possible errors:
      * dependant error in ProcDataLocal_c if EMPF or SEND not valid
      * dependant error in CANIO_c on send problems
  @param rf_val new measure value
*/
void MeasureProgLocal_c::setVal(float rf_val){
  float f_incr =  rf_val - f_lastMasterVal;
  int32_t i32_time =  System_c::getTime();
  int32_t i32_timeDelta = i32_time - i32_lastTime;
  float f_oldDelta = f_delta;
  i32_lastTime = i32_time;

  // update values:
  // for cumulative values -> update f_val by increment
  // else take the actual given master value
  f_lastMasterVal = rf_val;
  if (processData().b_cumulativeValue) f_val += f_incr;
  else f_val = rf_val;

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
  bool b_singleTest;
  for (Vec_MeasureSubprogIterator pc_iter = vec_measureSubprog.begin();
       pc_iter != vec_measureSubprog.end(); pc_iter++)
  {
    switch (pc_iter->type())

    {
      case Proc_c::TimeProp:
        b_singleTest = pc_iter->updateTrigger(i32_time);
        b_triggeredIncrement = (b_singleTest)? true : b_triggeredIncrement;
        // update med/integ
        if ((b_singleTest)&&(en_accumProp == Proc_c::AccumTime))updatePropDepVals();
        break;
      case Proc_c::DistProp:
				#ifdef USE_BASE
        b_singleTest = pc_iter->updateTrigger(getBaseInstance4Comm().distTheor());
				#endif
        b_triggeredIncrement = (b_singleTest)? true : b_triggeredIncrement;
        // update med/integ
        if ((b_singleTest)&&(en_accumProp == Proc_c::AccumDist))updatePropDepVals();
        break;
      case Proc_c::ValIncr:
        b_singleTest = pc_iter->updateTrigger(valFloat());
        b_triggeredIncrement = (b_singleTest)? true : b_triggeredIncrement;
        break;
      case Proc_c::DeltaIncr:
        b_singleTest = pc_iter->updateTrigger(f_delta);
        b_triggeredIncrement = (b_singleTest)? true : b_triggeredIncrement;
        break;
      case Proc_c::AccelIncr:
        b_singleTest = pc_iter->updateTrigger(f_accel);
        b_triggeredIncrement = (b_singleTest)? true : b_triggeredIncrement;
        break;
      case Proc_c::MedIncr:
        b_singleTest = pc_iter->updateTrigger(medFloat());
        b_triggeredIncrement = (b_singleTest)? true : b_triggeredIncrement;
        break;
      case Proc_c::MinIncr:
        b_singleTest = pc_iter->updateTrigger(minFloat());
        b_triggeredIncrement = (b_singleTest)? true : b_triggeredIncrement;
        break;

      case Proc_c::MaxIncr:
        b_singleTest = pc_iter->updateTrigger(maxFloat());
        b_triggeredIncrement = (b_singleTest)? true : b_triggeredIncrement;
        break;
      case Proc_c::IntegIncr:
        b_singleTest = pc_iter->updateTrigger(integFloat());
        b_triggeredIncrement = (b_singleTest)? true : b_triggeredIncrement;
        break;
      case Proc_c::NullType: break; // just to make compiler happy
    } // switch
  } // for
  // if b_triggeredIncrement == true the registered values should be sent
  if (b_triggeredIncrement)
  { // send the registered values
    // if at least one send try had success reset b_triggeredIncrement
    if (sendRegisteredVals()) b_triggeredIncrement = false;
  }
}
#endif
/**
  send the values which are registered by a running mesuring program

  possible errors:
    * dependant error in ProcDataLocal_c if EMPF or SEND not valid
    * dependant error in CANIO_c on send problems
  @return true -> value send triggered and performed with success
*/
bool MeasureProgLocal_c::sendRegisteredVals(){
  bool b_success = false;

  if (checkDoSend(Proc_c::DoVal))
  {
    // call send function
    b_success = (sendValMod( GeneralCommand_c::exactValue, gtp(), en_progType))?true : b_success;
  }
  if (checkDoSend(Proc_c::DoMin))
  {
    // call send function
    b_success = (sendValMod( GeneralCommand_c::minValue, gtp(), en_progType))?true : b_success;
  }
  if (checkDoSend(Proc_c::DoMax))
  {
    // call send function
    b_success = (sendValMod( GeneralCommand_c::maxValue, gtp(), en_progType))?true : b_success;
  }
  
  if (checkDoSend(Proc_c::DoInteg))
  {
    // call send function
    b_success = (sendValMod( GeneralCommand_c::integValue, gtp(), en_progType))?true : b_success;

  }
  if (checkDoSend(Proc_c::DoMed))
  {
    // call send function
    b_success = (sendValMod( GeneralCommand_c::medValue, gtp(), en_progType))?true : b_success;
  }

  return b_success;
}

/**
  init the element vals
  @param ri32_val initial measure val
*/
void MeasureProgLocal_c::initVal(int32_t ri32_val){
  // first call the base function
  MeasureProgBase_c::initVal(ri32_val);

  i32_medSum = ri32_val;
  i32_medCnt = 1;
}
#ifdef USE_FLOAT_DATA_TYPE
/**
  init the element vals
  @param rf_val initial measure val
*/
void MeasureProgLocal_c::initVal(float rf_val){
  // first call the base function
  MeasureProgBase_c::initVal(rf_val);

  f_medSum = rf_val;
  i32_medCnt = 1;
}
#endif
/**
  reset the local value

  possible errors:
    * dependant error in ProcDataLocal_c if EMPF or SEND not valid
    * dependant error in CANIO_c on send problems
  @return true -> reseted measure val sent with success
*/
bool MeasureProgLocal_c::resetVal(){
  // send resetted val
  uint8_t ui8_pri = progType();
  bool b_sendSuccess;

  // prepare general command in process pkg
  getProcessInstance4Comm().data().c_generalCommand.setValues(false /* isSetpoint */, false, /* isRequest */
                                                              GeneralCommand_c::exactValue, GeneralCommand_c::setValue);
#ifdef USE_FLOAT_DATA_TYPE
  if (processData().valType() != float_val)
  {
#endif
    i32_val = 0;
    // DIN: pd=1, mod=0
    b_sendSuccess = processData().sendValGtp(ui8_pri, c_gtp, val());
#ifdef USE_FLOAT_DATA_TYPE
  }
  else
  {
    f_val = 0;
    // DIN: pd=1, mod=0
    b_sendSuccess = processData().sendValGtp(ui8_pri, c_gtp, valFloat());
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

/**
  reset the local intgral value

  possible errors:
    * dependant error in ProcDataLocal_c if EMPF or SEND not valid
    * dependant error in CANIO_c on send problems
  @return true -> reseted integ val sent with success
*/
bool MeasureProgLocal_c::resetInteg(){
  // send resetted val
  uint8_t ui8_pri = progType();

  // prepare general command in process pkg
  getProcessInstance4Comm().data().c_generalCommand.setValues(false /* isSetpoint */, false, /* isRequest */
                                                              GeneralCommand_c::integValue, GeneralCommand_c::setValue);
  
  // DIN: pd=1, mod=3
#ifdef USE_FLOAT_DATA_TYPE
  if (processData().valType() != float_val)
  {
#endif
  i32_integ = 0;
  return processData().sendValGtp(ui8_pri, c_gtp, integ());
#ifdef USE_FLOAT_DATA_TYPE
  }
  else
  {
    f_integ = 0;
    return processData().sendValGtp(ui8_pri, c_gtp, integFloat());
  }
#endif

}

/**
  reset the local medium value

  possible errors:
    * dependant error in ProcDataLocal_c if EMPF or SEND not valid
    * dependant error in CANIO_c on send problems
  @return true -> reseted medium val sent with success
*/
bool MeasureProgLocal_c::resetMed(){
  i32_medCnt = 0;
  // send resetted val
  uint8_t ui8_pri = progType();
  
  // prepare general command in process pkg
  getProcessInstance4Comm().data().c_generalCommand.setValues(false /* isSetpoint */, false, /* isRequest */
                                                              GeneralCommand_c::medValue, GeneralCommand_c::setValue);
  //DIN pd=1, mod=4  
#ifdef USE_FLOAT_DATA_TYPE
  if (processData().valType() != float_val)
  {
#endif
  i32_medSum = 0;
  return processData().sendValGtp(ui8_pri, c_gtp, med());
#ifdef USE_FLOAT_DATA_TYPE
  }
  else
  {
    f_medSum = 0;
    return processData().sendValGtp(ui8_pri, c_gtp, medFloat());
  }
#endif
}

/**
  reset the local min value

  possible errors:
    * dependant error in ProcDataLocal_c if EMPF or SEND not valid
    * dependant error in CANIO_c on send problems
  @return true -> reseted minimum val sent with success
*/
bool MeasureProgLocal_c::resetMin(){
  // send resetted val
  uint8_t ui8_pri = progType();
  
  // prepare general command in process pkg
  getProcessInstance4Comm().data().c_generalCommand.setValues(false /* isSetpoint */, false, /* isRequest */
                                                              GeneralCommand_c::minValue, GeneralCommand_c::setValue);

#ifdef USE_FLOAT_DATA_TYPE
  if (processData().valType() != float_val)
  {
#endif
  i32_min = 0;
  // DIN: pd=1, mod=1
  return processData().sendValGtp(ui8_pri, c_gtp, min());
#ifdef USE_FLOAT_DATA_TYPE
  }
  else
  {
    f_min = 0;
    // DIN: pd=1, mod=1
    return processData().sendValGtp(ui8_pri, c_gtp, minFloat());
  }
#endif
}

/**
  reset the local max value

  possible errors:
    * dependant error in ProcDataLocal_c if EMPF or SEND not valid
    * dependant error in CANIO_c on send problems
  @return true -> reseted maximum val sent with success
*/
bool MeasureProgLocal_c::resetMax(){

  // send resetted val
  uint8_t ui8_pri = progType();
  
  // prepare general command in process pkg
  getProcessInstance4Comm().data().c_generalCommand.setValues(false /* isSetpoint */, false, /* isRequest */
                                                              GeneralCommand_c::maxValue, GeneralCommand_c::setValue);

#ifdef USE_FLOAT_DATA_TYPE
  if (processData().valType() != float_val)
  {
#endif
  i32_max = 0;
  // DIN: pd=1, mod=2
  return processData().sendValGtp(ui8_pri, c_gtp, max());
#ifdef USE_FLOAT_DATA_TYPE
  }
  else
  {
    f_max = 0;
    // DIN: pd=1, mod=2
    return processData().sendValGtp(ui8_pri, c_gtp, maxFloat());
  }
#endif
}

/**
  periodic events
  (e.g. send value for time proportional progs)

  possible errors:
    * dependant error in ProcDataLocal_c if EMPF or SEND not valid
    * dependant error in CANIO_c on send problems
  @return true -> all planned activities performed in available time
*/
bool MeasureProgLocal_c::timeEvent( void )
{
  if ( Scheduler_c::getAvailableExecTime() == 0 ) return false;
  int32_t i32_time = Scheduler_c::getLastTimeEventTrigger();

  bool b_singleTest;

  for (Vec_MeasureSubprogIterator pc_iter = vec_measureSubprog.begin(); pc_iter != vec_measureSubprog.end(); pc_iter++)
  {
    switch (pc_iter->type())
    {
      case Proc_c::TimeProp:
        b_singleTest = pc_iter->updateTrigger(i32_time);
        // if b_singleTest is true set b_triggeredIncrement to true,
        // otherwise let it unchanged
        b_triggeredIncrement = (b_singleTest)? true : b_triggeredIncrement;
        // update med/integ
        if ((b_singleTest)&&(en_accumProp == Proc_c::AccumTime))updatePropDepVals();
        break;
      case Proc_c::DistProp:
				#ifdef USE_BASE
        b_singleTest = pc_iter->updateTrigger(getBaseInstance4Comm().distTheor());
				#endif
        b_triggeredIncrement = (b_singleTest)? true : b_triggeredIncrement;
        // update med/integ
        if ((b_singleTest)&&(en_accumProp == Proc_c::AccumTime))updatePropDepVals();
        break;
      default:
        break;
    } // switch
  } // for
  // if b_triggeredIncrement == true the registered values should be sent
  // if needed an old unsuccessfull send try is redone
  if (b_triggeredIncrement)
  { // send the registered values
    // if at least one send try had success reset b_triggeredIncrement
    if (sendRegisteredVals()) b_triggeredIncrement = false;
  }
  return true;
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
