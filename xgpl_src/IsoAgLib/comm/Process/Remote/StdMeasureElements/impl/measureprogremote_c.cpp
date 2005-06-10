/***************************************************************************
                          measureprogremote_c.cpp - object for managing of
                                                   remote measure programs
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
#include "measureprogremote_c.h"
#include <IsoAgLib/driver/system/impl/system_c.h>
#include "../../../impl/process_c.h"
#include "../../../processdatachangehandler_c.h"

namespace __IsoAgLib {

/**
  initialise this MeasureProgRemote_c instance to well defined initial condition
  Scheduler_c and Process_c instances
  @param rpc_processData optional pointer to Process_c
*/
void MeasureProgRemote_c::init( ProcDataBase_c *const rpc_processData )
{
  MeasureProgBase_c::init( rpc_processData, Proc_c::UndefinedProg, int32_t(0), GetyPos_c(0xF, 0xF) );
  b_receiveForeignMeasurement = false;
}

/**
  assignment of MeasureProgRemote_c objects
  @param rrefc_src source MeasureProgRemote_c instance
  @return reference to the source instance for commands like "prog1 = prog2 = prog3 ...;
*/
const MeasureProgRemote_c& MeasureProgRemote_c::operator=(const MeasureProgRemote_c& rrefc_src){
  // call base class operator
  MeasureProgBase_c::operator=(rrefc_src);

  // copy element vars
  i32_med = rrefc_src.i32_med;
  b_receiveForeignMeasurement = rrefc_src.b_receiveForeignMeasurement;

  // return reference to source
  return rrefc_src;
}

/**
  copy constructor for MeasureProgRemote
  @param rrefc_src source MeasureProgRemote_c instance
*/
MeasureProgRemote_c::MeasureProgRemote_c(const MeasureProgRemote_c& rrefc_src)
  : MeasureProgBase_c(rrefc_src){

  // copy element vars
  i32_med = rrefc_src.i32_med;
  b_receiveForeignMeasurement = rrefc_src.b_receiveForeignMeasurement;
}

/** default destructor which has nothing to do */
MeasureProgRemote_c::~MeasureProgRemote_c(){
}

/**
  start a measuring programm

  possible errors:
      * Err_c::elNonexistent no remote member with claimed address with given GETY found
      * Err_c::precondition if ren_progType is not one of the allowed Proc_c::Base, Proc_c::Target
      * dependant error in CAN_IO
  @param ren_progType wanted msg type for measure prog (Proc_c::Base, Proc_c::Target)
  @param ren_doSend set process data subtype to send (Proc_c::DoNone, Proc_c::DoVal, Proc_c::DoMed, Proc_c::DoInteg)
  @return true -> command successful sent
*/
bool MeasureProgRemote_c::start(Proc_c::progType_t ren_progType, Proc_c::doSend_t ren_doSend){
  // retrieve the ren_type from the registered subprogs
  Proc_c::type_t en_combinedType = Proc_c::NullType;
  for (Vec_MeasureSubprog::iterator pc_subprog = vec_measureSubprog.begin();
       pc_subprog != vec_measureSubprog.end(); pc_subprog++)
  { // build en_combinedType
    if (pc_subprog == vec_measureSubprog.begin()) en_combinedType = pc_subprog->type();
    else en_combinedType = Proc_c::type_t(pc_subprog->type() | en_combinedType);
  }

  // now call other start with en_combinedType
  return start(ren_progType, en_combinedType, ren_doSend);
}

/**
  start a measuring programm

  possible errors:
      * Err_c::elNonexistent no remote member with claimed address with given GETY found
      * Err_c::precondition if ren_progType is not one of the allowed Proc_c::Base, Proc_c::Target
      * dependant error in CAN_IO
  @param ren_progType wanted msg type for measure prog (Proc_c::Base, Proc_c::Target)
  @param ren_type wanted increment type (Proc_c::TimeProp, Proc_c::DistProp, Proc_c::ValIncr)
  @param ren_doSend set process data subtype to send (Proc_c::DoNone, Proc_c::DoVal, Proc_c::DoMed, Proc_c::DoInteg)
  @return true -> command successful sent
*/
bool MeasureProgRemote_c::start(Proc_c::progType_t ren_progType, Proc_c::type_t ren_type,
                        Proc_c::doSend_t ren_doSend){

  bool b_sendResult = true;
  // test ren_progType
  if ((ren_progType & Proc_c::Base | Proc_c::Target) == 0)
  { // en_progType is different from Proc_c::Base and Proc_c::Target
    getLbsErrInstance().registerError( LibErr_c::Precondition, LibErr_c::LbsProcess );
    return false;
  }
  else
  { // o.k.
    // set the program type: Proc_c::Base or Proc_c::Target
    setProgType(ren_progType);
  }

#ifndef ISO_TASK_CONTROLLER
  // if stored remote gtp isn't valid exit this function
  // error state are set by the function
  if (!verifySetRemoteGtp())return false;
  // send all registered increments to remote ECU
  for (Vec_MeasureSubprog::iterator pc_subprog = vec_measureSubprog.begin();
       pc_subprog != vec_measureSubprog.end(); pc_subprog++)
  { // send the suitable increment creating message to the remote system
    if (pc_subprog->type() == Proc_c::TimeProp)
    { // send msg with wanted type-code, gtp, pd=0, mod=4, -1*increment value
      if (!processData().sendDataRawCmdGtp(ren_progType, gtp(), 0, 4,           // -bac this generates a Measuring increment command (Data Modifier = 4) Not needed for Part new 10
          -1* pc_subprog->increment()))
            b_sendResult = false;
    }
    else if (pc_subprog->type() == Proc_c::DistProp)
    { // send msg with wanted type-code, gtp, pd=0, mod=4, increment value
      if (!processData().sendDataRawCmdGtp(ren_progType, gtp(), 0, 4,
        pc_subprog->increment()))
            b_sendResult = false;
    }
  }

  // call base function for registering start state
  MeasureProgBase_c::start(ren_progType, ren_type, ren_doSend);

  // store actual time as last receive measure time to
  // avoid misinterpretation of to int32_t not receive data
  i32_lastMeasureReceive = System_c::getTime();

  // send start command to remote ECU
  // build start comand byte
  uint8_t b_command = 0x80; // start

  if (checkType(Proc_c::DistProp)) b_command |= 0x1;
  if (checkType(Proc_c::TimeProp)) b_command |= 0x4;

  if (checkDoSend(Proc_c::DoVal)) b_command |= 0x10;
  if (checkDoSend(Proc_c::DoMed)) b_command |= 0x20;
  if (checkDoSend(Proc_c::DoInteg)) b_command |= 0x40;

  // send start command to remote ECU
  uint8_t ui8_pri = (checkProgType(Proc_c::Base))? 1:2;
  // send msg with wanted type-code, gtp, pd=0, mod=6, start command
  // return result -> true if msg claimed address for member with gtp() exist
  if (!processData().sendDataRawCmdGtp(ui8_pri, gtp(), 0, 6, b_command))        // The 6 here the data modfier. It is used in conjuction with LSB of data to mean start, stop, reset. -bac
    b_sendResult = false;

#else
  // if stored remote gtp isn't valid exit this function
  // error state are set by the function
  if (!verifySetRemoteGtp())return false;
  uint8_t b_command = 0; 
  // send all registered increments to remote ECU 
  for (Vec_MeasureSubprog::iterator pc_subprog = vec_measureSubprog.begin();
       pc_subprog != vec_measureSubprog.end(); pc_subprog++)
  { // send the suitable increment creating message to the remote system
    if (pc_subprog->type() == Proc_c::TimeProp)
    { // send msg with wanted type-code, gtp, pd=0, mod=4, -1*increment value
      if (!processData().sendDataRawCmdGtp(ren_progType, gtp(), 0, 0x4, pc_subprog->increment()))
            b_sendResult = false;
    }
    else if (pc_subprog->type() == Proc_c::DistProp)
    { // send msg with wanted type-code, gtp, pd=0, mod=4, increment value
      if (!processData().sendDataRawCmdGtp(ren_progType, gtp(), 0, 0x5, pc_subprog->increment()))
            b_sendResult = false;
    }
    else if (pc_subprog->type() == Proc_c::WithinThresholdInterval)
    { 
        if(checkDoSend(Proc_c::DoMin)) b_command |= 0x6;
        else if(checkDoSend(Proc_c::DoMax)) b_command |= 0x7;
        if (!processData().sendDataRawCmdGtp(ren_progType, gtp(), 0, b_command, pc_subprog->increment()))
            b_sendResult = false;
    }
    else if (pc_subprog->type() == Proc_c::OnChange)
    { // send msg with wanted type-code, gtp, pd=0, mod=4, increment value
      if (!processData().sendDataRawCmdGtp(ren_progType, gtp(), 0, 0x8, pc_subprog->increment()))
            b_sendResult = false;
    }
    else if (pc_subprog->type() == Proc_c::OutsideThresholdInterval)
    { 
        if(checkDoSend(Proc_c::DoMin)) b_command |= 0x9;
        else if(checkDoSend(Proc_c::DoMax)) b_command |= 0xA;
        if (!processData().sendDataRawCmdGtp(ren_progType, gtp(), 0, b_command, pc_subprog->increment()))
            b_sendResult = false;
    }
    else if (pc_subprog->type() == Proc_c::Counter)
    { // send msg with wanted type-code, gtp, pd=0, mod=4, increment value
      if (!processData().sendDataRawCmdGtp(ren_progType, gtp(), 0, 0x2, pc_subprog->increment()))
            b_sendResult = false;
    }
    if(b_sendResult == true)
    {        
       // Update the state to reflect that the task has started
       getProcessInstance4Comm().setTaskStatus(1);
    }
  }

  // call base function for registering start state
  MeasureProgBase_c::start(ren_progType, ren_type, ren_doSend);

  // store actual time as last receive measure time to
  // avoid misinterpretation of to int32_t not receive data
  i32_lastMeasureReceive = System_c::getTime();
    // Need to set Command bits properly:
   
  //TimeInterval = 1, DistanceInterval = 2, WithinThresholdInterval = 4, OutsideThresholdInterval = 8, OnChange = 16, Counter = 32
  //////if (checkType(Proc_c::TimeProp)) b_command |= 0x4;
  //////if (checkType(Proc_c::DistProp)) b_command |= 0x5;
  //////if (checkType(Proc_c::WithinThresholdInterval))
  //////{
  //////    if(checkDoSend(Proc_c::DoMin)) b_command |= 0x6;
  //////    else if(checkDoSend(Proc_c::DoMax)) b_command |= 0x7;
  //////}
  //////if (checkType(Proc_c::OnChange)) b_command |= 0x8;
  //////if (checkType(Proc_c::OutsideThresholdInterval))
  //////{
  //////    if(checkDoSend(Proc_c::DoMin)) b_command |= 0x9;
  //////    else if(checkDoSend(Proc_c::DoMax)) b_command |= 0xa;
  //////}
  //////if (checkType(Proc_c::Counter))b_command |= 0x2;

  //////uint8_t ui8_pri = (checkProgType(Proc_c::Base))? 1:2;
  ////////if (!processData().sendDataRawCmdGtp(ui8_pri, gtp(), 0, 6, b_command))        // The 6 here the data modfier. It is used in conjuction with LSB of data to mean start, stop, reset. -bac
  ////// if (!processData().sendDataRawCmdGtp(ui8_pri, gtp(), 0, b_command, 2000))        // the 2000 is the time interval. Need a way to pass this in.    
  //////    b_sendResult = false;
  ////// else
  //////     // Update the state to reflect that the task has started
  //////     getProcessInstance4Comm().setTaskStatus(1);
#endif
  return b_sendResult;
}

/**
  send the stop command to the remote owner of data

  possible errors:
      * Err_c::elNonexistent no remote member with claimed address with given GETY found
      * dependant error in CAN_IO
  @return true -> command successful sent
*/
#ifdef ISO_TASK_CONTROLLER
bool MeasureProgRemote_c::stop(bool b_deleteSubProgs){
#else
bool MeasureProgRemote_c::stop(){
#endif
  bool b_result = true;

  // if stored remote gtp isn't valid exit this function
  // error state are set by the function
  if (!verifySetRemoteGtp()) return false;

  // send stop command to remote ECU
  uint8_t ui8_pri = (checkProgType(Proc_c::Base))? 1:2;
  // at the moment only TimeProp and DistProp are well -> only send them
  
#ifdef ISO_TASK_CONTROLLER
  uint8_t b_command = 0; 
  if (checkType(Proc_c::TimeProp)) b_command |= 0x4;
  if (checkType(Proc_c::DistProp)) b_command |= 0x5;
  if (checkType(Proc_c::WithinThresholdInterval))
  {
      if(checkDoSend(Proc_c::DoMin)) b_command |= 0x6;
      else if(checkDoSend(Proc_c::DoMax)) b_command |= 0x7;
  }
  if (checkType(Proc_c::OnChange)) b_command |= 0x8;
  if (checkType(Proc_c::OutsideThresholdInterval))
  {
      if(checkDoSend(Proc_c::DoMin)) b_command |= 0x9;
      else if(checkDoSend(Proc_c::DoMax)) b_command |= 0xa;
  }
  if (checkType(Proc_c::Counter))b_command |= 0x2;
  
  if(b_command)
  {
        b_result = processData().sendDataRawCmdGtp(ui8_pri, gtp(), 0, b_command, 0);
        // Set task status to stopped.
        getProcessInstance4Comm().setTaskStatus(0);
  }
#else
  if (checkDoSend(Proc_c::doSend_t(Proc_c::DoVal | Proc_c::DoMed | Proc_c::DoInteg)))
  { // send msg with wanted type-code, gtp, pd=0, mod=6, stop command
    // return result -> true if msg claimed address for member with gtp() exist
    b_result = processData().sendDataRawCmdGtp(ui8_pri, gtp(), 0, 6, 0);
  }
#endif
  else b_result = true;

  // call base function to delete the subprogs
#ifdef ISO_TASK_CONTROLLER
  if(b_deleteSubProgs)
#endif
    MeasureProgBase_c::stop();

  return b_result;
}



/**
  deliver med val
  @param rb_sendRequest choose wether a request for value update should be
      sent (default false == send no request)
  @return actual medium value
*/
int32_t MeasureProgRemote_c::med(bool rb_sendRequest) const
{
  if(rb_sendRequest)processDataConst().sendDataRawCmdGtp(2, gtp(), 3, 4, 0);
  return i32_med;
}
#ifdef USE_FLOAT_DATA_TYPE
/**
  deliver med val as float
  @param rb_sendRequest choose wether a request for value update should be
      sent (default false == send no request)
  @return actual medium value
*/
float MeasureProgRemote_c::medFloat(bool rb_sendRequest) const
{
  if(rb_sendRequest)processDataConst().sendDataRawCmdGtp(2, gtp(), 3, 4, 0);
  return f_med;
}
#endif

/**
  process msg;
  @return true -> msg is already complete edited
*/
bool MeasureProgRemote_c::processMsg(){
  bool b_result = MeasureProgBase_c::processMsg();
  // call base function - if base function returns true, nothing else must be done
  if (!b_result)
  { // the message was a value message -> evaluate it here

    // only update vars if received msg contains data (PD==1)
    ProcessPkg_c& c_pkg = getProcessInstance4Comm().data();

#ifdef ISO_TASK_CONTROLLER
    if ((c_pkg.cmd() == 3) && (c_pkg.memberEmpf().itemState(IState_c::Local)||(b_receiveForeignMeasurement) ) )
    { // write -> data from remote process data
      setValFromPkg();
      b_result = true;
    }
#else
    // only use data if:
    // A: PD== 1
    // B: EMPF is number of local member or receiveForeignMeasurement
    //      was set
    //     or msg is base process msg
    if ((c_pkg.pd() == 1) &&
        ((c_pkg.pri() == 1)
       ||(c_pkg.memberEmpf().itemState(IState_c::Local))
       ||(b_receiveForeignMeasurement)
         )
        )
    { // write -> data from remote process data
      setValFromPkg();
      b_result = true;
    }
#endif
  }
  return b_result;
}

/**
  set according to values of the package the accoring value
  (uses function, wich converts if needed)
*/
void MeasureProgRemote_c::setValFromPkg(){
  // convert the value if wanted
  // set timestamp of last measurement receive
  ProcessPkg_c& c_pkg = getProcessInstance4Comm().data();
  i32_lastMeasureReceive = c_pkg.time();
  // get MOD code of msg
  uint8_t b_mod = c_pkg.mod();
  bool b_change = false;

#ifdef USE_FLOAT_DATA_TYPE
  if (processData().valType() != float_val)
  {
#endif
    // get the int32_t data val; let it convert, if needed
    int32_t i32_new_val = processData().pkgDataLong();
#ifdef ISO_TASK_CONTROLLER
    // set val with function, to calc delta and accel
    if ( val() != i32_new_val ) b_change = true;
    setVal(i32_new_val);
 #else
    switch (b_mod)
    {
      case 0: // msg contains measured val
        // set val with function, to calc delta and accel
        if ( val() != i32_new_val ) b_change = true;
        setVal(i32_new_val);
        break;
      case 1: // msg contains MIN val
        if ( min() != i32_new_val ) b_change = true;
        setMin(i32_new_val);
        break;
      case 2: // msg contains MAX val
        if ( max() != i32_new_val ) b_change = true;
        setMax(i32_new_val);
        break;
      case 3: // msg contains integral val
        if ( integ() != i32_new_val ) b_change = true;
        setInteg(i32_new_val);
        break;
      case 4: // msg contains medium val
        if ( med() != i32_new_val ) b_change = true;
        setMed(i32_new_val);
        break;
    }
#endif


#ifdef USE_FLOAT_DATA_TYPE
  }
  else
  {
    // get the int32_t data val; let it convert, if needed
    float f_new_val = processData().pkgDataFloat();
    switch (b_mod)
    {
      case 0: // msg contains measured val
        // set val with function, to calc delta and accel
        if ( valFloat() != f_new_val ) b_change = true;
        setVal(f_new_val);
        break;
      case 1: // msg contains MIN val
        if ( minFloat() != f_new_val ) b_change = true;
        setMin(f_new_val);
        break;
      case 2: // msg contains MAX val
        if ( maxFloat() != f_new_val ) b_change = true;
        setMax(f_new_val);
        break;
      case 3: // msg contains integral val
        if ( integFloat() != f_new_val ) b_change = true;
        setInteg(f_new_val);
        break;
      case 4: // msg contains medium val
        if ( medFloat() != f_new_val ) b_change = true;
        setMed(f_new_val);
        break;
    }
  }
#endif
  // call handler function if handler class is registered
  if ( processDataConst().getProcessDataChangeHandler() != NULL )
    processDataConst().getProcessDataChangeHandler()->processMeasurementUpdate( pprocessData(), masterVal(), c_pkg.memberSend().gtp(), b_change );
}

/**
  set a new measure val
  @param ri32_val new val received from remote system
*/
void MeasureProgRemote_c::setVal(int32_t ri32_val){
  int32_t i32_time =  System_c::getTime();
  int32_t i32_timeDelta = i32_time - i32_lastTime;
  i32_lastTime = i32_time;

  int32_t i32_incr =  ri32_val - val();
  int32_t i32_oldDelta = i32_delta;
  // claculate delta and accel in 1/s
  if (i32_timeDelta > 0)
  {
    if (CNAMESPACE::labs(i32_incr) > 100000)
    { // i32_incr very big -> first divide than mult
      i32_delta = (i32_incr / i32_timeDelta) * 1000;
      i32_accel = ((i32_delta - i32_oldDelta) / i32_timeDelta) * 1000;
    }
    else
    {
      i32_delta = (i32_incr * 1000) / i32_timeDelta;
      i32_accel = ((i32_delta - i32_oldDelta) * 1000) / i32_timeDelta;
    }
  }

  // set the val
  i32_val = ri32_val;
}

/**
  init element values
  @param ri32_val starting measuring value
*/
void MeasureProgRemote_c::initVal(int32_t ri32_val){
  //first call base function
  MeasureProgBase_c::initVal(ri32_val);

  // set medium initially to first given val
  i32_med = ri32_val;
}
#ifdef USE_FLOAT_DATA_TYPE
/**
  set a new measure val
  @param rf_val new val received from remote system
*/
void MeasureProgRemote_c::setVal(float rf_val){
  int32_t i32_time =  System_c::getTime();
  int32_t i32_timeDelta = i32_time - i32_lastTime;
  i32_lastTime = i32_time;

  float f_incr =  rf_val - valFloat();
  float f_oldDelta = f_delta;
  // claculate delta and accel in 1/s
  if (i32_timeDelta > 0)
  {
    f_delta = (f_incr / (float)i32_timeDelta) * 1000.0F;
    f_accel = ((f_delta - f_oldDelta) / (float)i32_timeDelta) * 1000.0F;
  }

  // set the val
  f_val = rf_val;
}

/**
  init element values
  @param rf_val starting measuring value
*/
void MeasureProgRemote_c::initVal(float rf_val){
  //first call base function
  MeasureProgBase_c::initVal(rf_val);

  // set medium initially to first given val
  f_med = rf_val;
}
#endif

/**
  send reset command for measure value

  possible errors:
      * Err_c::elNonexistent no remote member with claimed address with given GETY found
      * dependant error in CAN_IO
  @return true -> command successful sent
*/
bool MeasureProgRemote_c::resetVal(){
  // if stored remote gtp isn't valid exit this function
  // error state are set by the function
  if (!verifySetRemoteGtp())return false;
  // get suitable PRI code
  uint8_t ui8_pri = (checkProgType(Proc_c::Base))? 1:2;
  return processData().sendDataRawCmdGtp(ui8_pri, gtp(), 0, 6, 0x18);
}

/**
  send reset command for medium value

  possible errors:
      * Err_c::elNonexistent no remote member with claimed address with given GETY found
      * dependant error in CAN_IO
  @return true -> command successful sent
*/
bool MeasureProgRemote_c::resetMed(){
  // if stored remote gtp isn't valid exit this function
  // error state are set by the function
  if (!verifySetRemoteGtp())return false;

  // get suitable PRI code
  uint8_t ui8_pri = (checkProgType(Proc_c::Base))? 1:2;
  return processData().sendDataRawCmdGtp(ui8_pri, gtp(), 0, 6, 0x28);
}

/**
  send reset command for integral value

  possible errors:
      * Err_c::elNonexistent no remote member with claimed address with given GETY found
      * dependant error in CAN_IO
  @return true -> command successful sent
*/
bool MeasureProgRemote_c::resetInteg(){
  // if stored remote gtp isn't valid exit this function
  // error state are set by the function
  if (!verifySetRemoteGtp())return false;

  // get suitable PRI code
  uint8_t ui8_pri = (checkProgType(Proc_c::Base))? 1:2;
  return processData().sendDataRawCmdGtp(ui8_pri, gtp(), 0, 6, 0x48);
}

/**
  send reset command for minimum value

  possible errors:
      * Err_c::elNonexistent no remote member with claimed address with given GETY found
      * dependant error in CAN_IO
  @return true -> command successful sent
*/
bool MeasureProgRemote_c::resetMin(){
  // if stored remote gtp isn't valid exit this function
  // error state are set by the function
  if (!verifySetRemoteGtp())return false;

  // get suitable PRI code
  uint8_t ui8_pri = (checkProgType(Proc_c::Base))? 1:2;
  return processData().sendDataRawCmdGtp(ui8_pri, gtp(), 0, 6, 0x8);
}

/**
  send reset command for maximum value

  possible errors:
      * Err_c::elNonexistent no remote member with claimed address with given GETY found
      * dependant error in CAN_IO
  @return true -> command successful sent
*/
bool MeasureProgRemote_c::resetMax(){
  // if stored remote gtp isn't valid exit this function
  // error state are set by the function
  if (!verifySetRemoteGtp())return false;

  // get suitable PRI code
  uint8_t ui8_pri = (checkProgType(Proc_c::Base))? 1:2;
  return processData().sendDataRawCmdGtp(ui8_pri, gtp(), 0, 6, 0x8);
}

/**
  perform periodic actions --> stop measuring prog if gtp isn't active any more
  @return true -> all planned activities performed in available time
*/
bool MeasureProgRemote_c::timeEvent( void )
{
  if ( (!getSystemMgmtInstance4Comm().existMemberGtp(gtp(), true)) && started() )
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
  @param rb_useForeign wanted mode (default true)
*/
void MeasureProgRemote_c::receiveForeignMeasurement(bool rb_useForeign)
{
  b_receiveForeignMeasurement = rb_useForeign;
};


/**
  verify the stored GETY_POS code of the remote system
  and set Err_c::elNonexistent if this system isn't registered as ative;
  if gtp is undefied yet, retrieve it by the stored ident GETY of this process data

  possible errors:
      * Err_c::elNonexistent no remote member with claimed address with given GETY found
  @return true -> everything o.k.
*/
bool MeasureProgRemote_c::verifySetRemoteGtp()
{ // if ownerGtp is specified, check if it's still valid
  SystemMgmt_c &c_lbsSystem = getSystemMgmtInstance4Comm();
  if ( processData().ownerGtp().isSpecified()
    && (c_lbsSystem.existMemberGtp(processData().ownerGtp(), true)))
    return true; // change nothing and return success

  // check if proc data identity is valid and corresponding member has claimed address in monitor list
  if ( processData().gtp().isSpecified()
    && (c_lbsSystem.existMemberGtp(processData().gtp(), true)))
    return true; // change nothing and return success

  // if both tests were false look for member with claimed address with GETY of this process
  // data to update POS of this instance
  GetyPos_c c_tempGtp = processData().ownerGtp();
  uint8_t b_tempPos;
  for (b_tempPos = 0; b_tempPos < 8; b_tempPos++)
  {
    c_tempGtp.setPos( b_tempPos );
    if (getSystemMgmtInstance4Comm().existMemberGtp(c_tempGtp))
    {
      processData().setOwnerGtp(c_tempGtp); // set actual GETY_POS, because member is found in list
      processData().setPos(c_tempGtp.getPos());
      // stop further search if this item has already claimed address
      if (getSystemMgmtInstance4Comm().memberGtp(c_tempGtp).itemState(IState_c::ClaimedAddress))
      { // this item has claimed address -> use it and stop search
        return true; // return from this function with positive result
      }
    }
  } // for
  getLbsErrInstance().registerError( LibErr_c::ElNonexistent, LibErr_c::LbsProcess );
  return false;
}

} // end of namespace __IsoAgLib
