/***************************************************************************
                          setpointremote_c.cpp - object for commanding remote
                                               process data objects
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
#include "setpointremote_c.h"
#include "../../../impl/process_c.h"
#include "../../../impl/generalcommand_c.h"
#include "../../../processdatachangehandler_c.h"
#include <IsoAgLib/driver/system/impl/system_c.h>

namespace __IsoAgLib {
/**
  initialise this SetpointRemote_c to a well defined starting condition
  @param apc_processData pointer to containing ProcessData instance
*/
void SetpointRemote_c::init( ProcDataBase_c *const apc_processData )
{
  SetpointBase_c::init( apc_processData );
  i32_answeredTime = i32_commandedTime = 0;
  c_answeredMaster.setISOName( IsoName_c::IsoNameUnspecified() );
  c_answeredMe.setISOName( IsoName_c::IsoNameUnspecified() );
  c_commanded.setISOName( IsoName_c::IsoNameUnspecified() );


}

/**
  assginment from another object
  @param arc_src source SetpointRemote_c instance
  @return reference to source for cmd like "setp1 = setp2 = setp3;"
*/
const SetpointRemote_c& SetpointRemote_c::operator=( const SetpointRemote_c& arc_src ){
  //first call base function
  SetpointBase_c::operator=( arc_src );

  assignFromSource( arc_src );

  // return reference to source
  return *this;
}

/**
  copy constructor for SetpointRemote
  @param arc_src source SetpointRemote_c instance
*/
SetpointRemote_c::SetpointRemote_c( const SetpointRemote_c& arc_src)
  : SetpointBase_c( arc_src){
  assignFromSource( arc_src );
}
/** base function for assignment of element vars for copy constructor and operator= */
void SetpointRemote_c::assignFromSource( const SetpointRemote_c& arc_src )
{ // now copy the elements
  c_answeredMaster = arc_src.c_answeredMaster;
  c_answeredMe = arc_src.c_answeredMe;
  c_commanded = arc_src.c_commanded;
  i32_answeredTime = arc_src.i32_answeredTime;
  i32_commandedTime = arc_src.i32_commandedTime;
}

/** default destructor which has nothing to do */
SetpointRemote_c::~SetpointRemote_c(){
}

#ifdef USE_FLOAT_DATA_TYPE
/**
  command a exact setpoint; store value as commanded and send command

  possible errors:
      * dependant error in ProcDataRemoteBase_c if comanded remote system not found in Monitor List
      * dependant error in CanIo_c on CAN send problems
  @return new exact setpoint to command
*/
void SetpointRemote_c::setExact(float af_val)
{ // set value in c_commanded
  c_commanded.setExact( af_val);
  c_commanded.setISOName( processData().commanderISOName());

  // set time of command
  i32_commandedTime = System_c::getTime();

  // prepare general command in process pkg
  // isSetpoint = true, isRequest = false
  getProcessInstance4Comm().data().c_generalCommand.setValues(true /* isSetpoint */, false /* isRequest */,
                                                              GeneralCommand_c::exactValue,
                                                              GeneralCommand_c::setValue);

  processData().sendValISOName(processData().commanderISOName(), af_val);
}

#endif

/**
  command a exact setpoint; store value as commanded and send command

  possible errors:
      * dependant error in ProcDataRemoteBase_c if comanded remote system not found in Monitor List
      * dependant error in CanIo_c on CAN send problems
  @return new exact setpoint to command
*/
void SetpointRemote_c::setExact( int32_t ai32_val){
  // set value in c_commanded
  c_commanded.setExact( ai32_val);
  c_commanded.setISOName( processData().commanderISOName());

  // set time of command
  i32_commandedTime = System_c::getTime();

  // prepare general command in process pkg
  getProcessInstance4Comm().data().c_generalCommand.setValues(true /* isSetpoint */, false /* isRequest */,
                                                              GeneralCommand_c::exactValue,
                                                              GeneralCommand_c::setValue);

  processData().sendValISOName(processData().commanderISOName(), ai32_val);
}


/**
  command a minimum setpoint; store value as commanded and send command

  possible errors:
      * dependant error in ProcDataRemoteBase_c if comanded remote system not found in Monitor List
      * dependant error in CanIo_c on CAN send problems
  @return new minimum setpoint to command
*/
void SetpointRemote_c::setMin( int32_t ai32_val){
  // set value in c_commanded
  c_commanded.setMin( ai32_val);
  c_commanded.setISOName( processData().commanderISOName());

  // set time of command
  i32_commandedTime = System_c::getTime();

  // prepare general command in process pkg
  getProcessInstance4Comm().data().c_generalCommand.setValues(true /* isSetpoint */, false, /* isRequest */
                                                              GeneralCommand_c::minValue,
                                                              GeneralCommand_c::setValue);

  processData().sendValISOName(processData().commanderISOName(), ai32_val);
}

/**
  command a maximum setpoint; store value as commanded and send command

  possible errors:
      * dependant error in ProcDataRemoteBase_c if comanded remote system not found in Monitor List
      * dependant error in CanIo_c on CAN send problems
  @return new maximum setpoint to command
*/
void SetpointRemote_c::setMax( int32_t ai32_val){
  // set value in c_commanded
  c_commanded.setMax( ai32_val);
  c_commanded.setISOName( processData().commanderISOName());

  // set time of command
  i32_commandedTime = System_c::getTime();

  // prepare general command in process pkg
  getProcessInstance4Comm().data().c_generalCommand.setValues(true /* isSetpoint */, false, /* isRequest */
                                                              GeneralCommand_c::maxValue,
                                                              GeneralCommand_c::setValue);
  // send command to owner: PD=0, MOD=3
  processData().sendValISOName(processData().commanderISOName(), ai32_val);
}

/**
  command a default setpoint; store value as commanded and send command

  possible errors:
      * dependant error in ProcDataRemoteBase_c if comanded remote system not found in Monitor List
      * dependant error in CanIo_c on CAN send problems
  @return new default setpoint to command
*/
void SetpointRemote_c::setDefault( int32_t ai32_val){
  // set value in c_commanded
  c_commanded.setDefault( ai32_val);
  c_commanded.setISOName( processData().commanderISOName());

  // set time of command
  i32_commandedTime = System_c::getTime();

  // prepare general command in process pkg
  getProcessInstance4Comm().data().c_generalCommand.setValues(true /* isSetpoint */, false, /* isRequest */
                                                              GeneralCommand_c::defaultValue,
                                                              GeneralCommand_c::setValue);
  // send command to owner:
  processData().sendValISOName(processData().commanderISOName(), ai32_val);
}

/**
  request remote master setpoint - exact
*/
void SetpointRemote_c::requestExact() const
{
  // prepare general command in process pkg
  getProcessInstance4Comm().data().c_generalCommand.setValues(true /* isSetpoint */, true /* isRequest */,
                                                              GeneralCommand_c::exactValue,
                                                              GeneralCommand_c::requestValue);

  processDataConst().sendValISOName(processDataConst().commanderISOName(), 0);
}
/**
  request remote master setpoint - MIN
*/
void SetpointRemote_c::requestMin() const
{
  // prepare general command in process pkg
  getProcessInstance4Comm().data().c_generalCommand.setValues(true /* isSetpoint */, true /* isRequest */,
                                                              GeneralCommand_c::minValue,
                                                              GeneralCommand_c::requestValue);

  processDataConst().sendValISOName(processDataConst().commanderISOName(), 0);
}
/**
  request remote master setpoint - MAX
*/
void SetpointRemote_c::requestMax() const
{
  // prepare general command in process pkg
  getProcessInstance4Comm().data().c_generalCommand.setValues(true /* isSetpoint */, true /* isRequest */,
                                                              GeneralCommand_c::maxValue,
                                                              GeneralCommand_c::requestValue);

  processDataConst().sendValISOName(processDataConst().commanderISOName(), 0);
}
/**
  request remote master setpoint - DEFAULT
*/
void SetpointRemote_c::requestDefault() const
{
  // prepare general command in process pkg
  getProcessInstance4Comm().data().c_generalCommand.setValues(true /* isSetpoint */, true /* isRequest */,
                                                              GeneralCommand_c::defaultValue,
                                                              GeneralCommand_c::requestValue);
  processDataConst().sendValISOName(processDataConst().commanderISOName(), 0);
}

/**
  check if the last commanded setpoint was accepted
  (am I am now the master)
  @return true -> the last commanded setpoint was accepted by remote system
*/
bool SetpointRemote_c::accepted() const {
  bool b_result = false;

  if (answered())
  {
    // the last command was already answered -> check if value is identic
    if (c_answeredMe == c_commanded) b_result = true;
  }

  return b_result;
}

/**
  check if an active master setpoint exist
  @return true -> a master setpoint commander exist
*/
bool SetpointRemote_c::existMaster() const {
  return (c_answeredMaster.isoName().isSpecified());
}

/**
  mark the master setpoint as released
  (send a suitable reset command)

  possible errors:
      * Err_c::precondition I'm not the master -> can't release master
      * dependant error in ProcDataRemoteBase_c if comanded remote system not found in Monitor List
      * dependant error in CanIo_c on CAN send problems
*/
void SetpointRemote_c::releaseMaster(){
  // if my lst command was accepted as master -> send release command
  if (accepted())
  { // i am the master -> send release command
    sendSetpointVals( c_commanded, true, SETPOINT_RELEASE_COMMAND);
    // now clear the master flag
    releaseMasterIntern();
  }
  else
  { // I'm not the master
    getILibErrInstance().registerError( iLibErr_c::Precondition, iLibErr_c::Process );
  }
}

/**
  check if my commanded setpoint s valid:
  a) I am the master
  b) my setpoint is valid for the actual master setpoint
  @return true -> master OR my setpoint meets the actual master setpoint
*/
bool SetpointRemote_c::valid() const {
  bool b_min_test = true,
      b_maxTest = true;
  bool b_result =
    ((answered()
  && (c_commanded.isoName() == c_answeredMaster.isoName())
    )) ? true : false;

  if ((!b_result) && (answered()) && (existMaster()))
  { // I am not the master -> check if my setpoint is conformant with master setpoint
    if (c_commanded.existExact())
    { //  commanded an exact setpoint -> the master setpoint must be the same
      if (c_commanded.exact() == c_answeredMaster.exact()) b_result = true;
    }
    else
    {
      if (c_commanded.existMin())
      { //
        if ( ((c_answeredMaster.existExact()) && (c_commanded.min() < c_answeredMaster.exact()))
          || (
                (c_answeredMaster.existMin()) && (c_commanded.min() < c_answeredMaster.min())
              &&(c_answeredMaster.existMax()) && (c_commanded.min() < c_answeredMaster.max())
             )
           )
        { // checks are positive
          b_min_test = true;
        }
        else
        { // the min test had error
          b_min_test = false;
        }
      }
      if (c_commanded.existMax())
      { //
        if ( ((c_answeredMaster.existExact()) && (c_commanded.max() > c_answeredMaster.exact()))
          || (
                (c_answeredMaster.existMin()) && (c_commanded.max() > c_answeredMaster.min())
              &&(c_answeredMaster.existMax()) && (c_commanded.max() > c_answeredMaster.max())
             )
           )
        { // checks are positive
          b_maxTest = true;
        }
        else
        { // the max test had error
          b_maxTest = false;
        }
      }
      // the b_result is ositive, if both tests was positiv, if executed
      // (default val true -> if not tested the test is ignored)
      if (b_min_test && b_maxTest)b_result = true;
    } // else -> I commanded no exact setpoint
  } // if not master -> check for conformance with master

  // now b_result is positive, if my setpoint is valid
  return b_result;
}

/** process a setpoint request for remote process data */
void SetpointRemote_c::processRequest() const {
}

/** process a setpoint set for remote process data */
void SetpointRemote_c::processSet(){
  ProcessPkg_c& c_pkg = getProcessInstance4Comm().data();
  // take raw val
  const int32_t i32_val = c_pkg.dataLong();
  // detect if something was changed, so that the handler shall be called
  bool b_change = false;

  // retreive the isoName of send and empf
  const IsoName_c& c_empfISOName = c_pkg.memberEmpf().isoName();
  const IsoName_c& c_sendISOName = c_pkg.memberSend().isoName();

  if (c_sendISOName == processData().ownerISOName())
  { // the owner sent the value
    if (c_pkg.isSpecCmd( static_cast<proc_specCmd_t>(setpointReleaseCmd|setpointErrCmd)) == false)
    { // was a normal value
      if  ( ( c_answeredMaster.valMod( c_pkg.c_generalCommand.getValueGroup() ) != i32_val )
         || ( ! c_answeredMaster.valid() )
         || ( c_answeredMaster.isoName() != c_empfISOName )
         || ( ! c_answeredMaster.master() )
          ) b_change = true;

      bool b_changeMeasurement;
      switch (c_pkg.c_generalCommand.getValueGroup())
      { // compare new value with corresponding last value
        case GeneralCommand_c::exactValue:   b_changeMeasurement = (c_answeredMaster.exact() != i32_val); break;
        case GeneralCommand_c::defaultValue: b_changeMeasurement = (c_answeredMaster.getDefault() != i32_val); break;
        case GeneralCommand_c::minValue:     b_changeMeasurement = (c_answeredMaster.min() != i32_val); break;
        case GeneralCommand_c::maxValue:     b_changeMeasurement = (c_answeredMaster.max() != i32_val); break;
        default:                             b_changeMeasurement = FALSE;
      }
      // call processMeasurementUpdate handler function in case a measurement prog is running for this setpoint DDI
      if ( ( processDataConst().getProcessDataChangeHandler() != NULL ) )
        processDataConst().getProcessDataChangeHandler()->processMeasurementUpdate( pprocessData(), i32_val, c_pkg.memberSend().isoName().toConstIisoName_c(), b_changeMeasurement);

      c_answeredMaster.setValMod( i32_val, c_pkg.c_generalCommand.getValueGroup());
      // set the isoName of the actual master
      c_answeredMaster.setISOName( c_empfISOName);
      // set the valid flag to true
      c_answeredMaster.setValid( true);
      // set for completness themaster flag
      c_answeredMaster.setMaster( true);

    }
  }
  // check if it was a master release command
  if (((c_empfISOName == c_answeredMaster.isoName())
      ||(c_sendISOName == processData().ownerISOName()))
    &&(c_pkg.isSpecCmd( setpointReleaseCmd)) )
  { // the actual master setpoint is released
    releaseMasterIntern();
    b_change = true;
  }
  // check if it was a SETPOINT_ERROR message for the master
  if ((c_empfISOName == c_answeredMaster.isoName())
    && (c_pkg.isSpecCmd( setpointErrCmd)) )
  { // the owner (controlling) system can´t deliver the commanded setpoint -> actual invalid
    if ( c_answeredMaster.valid() ) b_change = true;
    c_answeredMaster.setValid( false);
  }

  // check if this is an answer to my last command
  if ((c_empfISOName == processData().commanderISOName())
    && (c_sendISOName == processData().ownerISOName()))
  { // the owner of the process data sent me an answer
    if (c_pkg.isSpecCmd( static_cast<proc_specCmd_t>(setpointReleaseCmd|setpointErrCmd)) == false)
    { // normal value
      if  ( ( c_answeredMe.valMod( c_pkg.c_generalCommand.getValueGroup() ) != i32_val )
         || ( ! c_answeredMe.valid() )
         || ( ! c_answeredMe.master() )
          ) b_change = true;
      // set the given value
      c_answeredMe.setValMod( i32_val, c_pkg.c_generalCommand.getValueGroup());
      // set the valid flag to true
      c_answeredMe.setValid( true);
      // i am the master if i get a normal value answered
      c_answeredMe.setMaster( true);
    }
    else
    { // error or release command
      if (c_pkg.isSpecCmd( setpointReleaseCmd)) {
        c_answeredMe.setMaster( false);
        b_change = true;
      }
      if (c_pkg.isSpecCmd( setpointErrCmd)) {
        c_answeredMe.setValid( false);
        b_change = true;
      }
    }
    // set the answer time
    i32_answeredTime = System_c::getTime();
    // set the correct isoName
    c_answeredMe.setISOName( c_empfISOName);
  }
  // call handler function if handler class is registered
  if ( ( processDataConst().getProcessDataChangeHandler() != NULL ) && ( b_change ) )
    processDataConst().getProcessDataChangeHandler()->processSetpointResponse( pprocessData(), setpointMasterVal(), c_pkg.memberSend().isoName().toConstIisoName_c() );

}

/**
  clear the master flag without checking
  if this action is allowed
*/
void SetpointRemote_c::releaseMasterIntern(){
  c_answeredMaster.setISOName( IsoName_c::IsoNameUnspecified() );
  c_answeredMaster.setMaster( false);
}


/**
  perform periodic actions
  --> reset all entries if isoName isn't active any more
  @return true -> all planned activities performed in available time
*/
bool SetpointRemote_c::timeEvent( void )
{
  if ( ElementBase_c::getAvailableExecTime() == 0 ) return false;
  const IsoName_c& c_ownerISOName = processData().ownerISOName();
  if ( !getIsoMonitorInstance4Comm().existIsoMemberISOName( c_ownerISOName, true) )
  { // remote owner of this process data isn't active any more
    // -> reset all entries
    i32_answeredTime = i32_commandedTime = 0;
    c_answeredMaster.setISOName( IsoName_c::IsoNameUnspecified() );
    c_answeredMaster.setMaster( false);
    c_answeredMe.setISOName( IsoName_c::IsoNameUnspecified() );
    c_commanded.setISOName( IsoName_c::IsoNameUnspecified() );
  }
  return true;
}


} // end of namespace __IsoAgLib
