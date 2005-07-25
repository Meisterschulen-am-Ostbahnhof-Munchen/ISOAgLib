/***************************************************************************
                          procdatalocalbase_c.cpp - managing of simple
                                                       local process data object
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
 * - Call of a (member-) function                                          *
 * - Instantiation of a variable in a datatype from internal namespace     *
 * Allowed is:                                                             *
 * - Instatiation of a variable in a datatype from interface namespace,    *
 *   even if this is derived from a base class inside an internal namespace*
 * - Call of member functions which are defined in the interface class     *
 *   definition ( header )                                                 *
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
#include "procdatalocalbase_c.h"
#include "../../impl/process_c.h"
#include "../../processdatachangehandler_c.h"

#ifdef USE_EEPROM_IO
  #include <IsoAgLib/driver/eeprom/impl/eepromio_c.h>
#endif

namespace __IsoAgLib {

/**
  initialise this ProcDataBase_c instance to a well defined initial state

  possible errors:
      * Err_c::badAlloc not enough memory to insert first  MeasureProgLocal
  ISO parameter
  @param rui16_DDI optional DDI code of this instance
  @param rui16_element optional Element code of this instance

  DIN parameter
  @param rui8_lis optional LIS code of this instance
  @param rui8_wert optional WERT code of this instance
  @param rui8_inst optional INST code of this instance
  @param rui8_zaehlnum optional ZAEHLNUM code of this instance

  @param rc_gtp optional GETY_POS code of Process-Data
  @param rui8_pri PRI code of messages with this process data instance (default 2)
  @param rc_ownerGtp optional GETY_POS of the owner
  @param rpc_gtp pointer to updated GETY_POS variable of owner
  @param rb_cumulativeValue
          -# for process data like distance, time, area
              the value of the measure prog data sets is updated
              on master value update dependent on the value increment
              since the last master value update
              -> if a remote member resets his data set copy, datas of
                  other members aren't changed
              -> if this data is saved in EEPROM, the main application
                  needn't take into account the initial EEPROM value, as
                  setting of the master val is independent from EEPROM
          -#  for values like speed, state, rpm aren't updated by increment,
                -> the given master value is propagated equally to all
                    measure prog data sets
                -> if this data is saved in EEPROM, the stored value is loaded
                  as initial master value, and is initially propagated to all
                  measure prog data sets
  @param rui16_eepromAdr optional adress where value is stored in EEPROM
  @param rpc_processDataChangeHandler optional pointer to handler class of application
  @param ri_singletonVecKey optional key for selection of IsoAgLib instance (default 0)
*/
void ProcDataLocalBase_c::init(
#ifdef USE_ISO_11783
                               uint16_t rui16_DDI, uint16_t rui16_element,
#endif
#ifdef USE_DIN_9684
                               uint8_t rui8_lis, uint8_t rui8_wert, uint8_t rui8_inst, uint8_t rui8_zaehlnum,
#endif
                               const GetyPos_c& rc_gtp, uint8_t rui8_pri, const GetyPos_c& rc_ownerGtp,
                               const GetyPos_c *rpc_gtp, bool rb_cumulativeValue,
#ifdef USE_EEPROM_IO
                               uint16_t rui16_eepromAdr,
#endif // USE_EEPROM_IO
                               IsoAgLib::ProcessDataChangeHandler_c *rpc_processDataChangeHandler,
                               int ri_singletonVecKey
                               )
{
  ProcDataBase_c::init(
#ifdef USE_ISO_11783
                       rui16_DDI, rui16_element,
#endif
#ifdef USE_DIN_9684
                       rui8_lis, rui8_wert, rui8_inst, rui8_zaehlnum,
#endif
                       rc_gtp, rui8_pri, rc_ownerGtp, rpc_gtp,
                       rpc_processDataChangeHandler, ri_singletonVecKey);

  b_cumulativeValue = rb_cumulativeValue;
#ifdef USE_EEPROM_IO
  setEepromAdr(rui16_eepromAdr);
  i32_lastEepromStore = 0;
#endif // USE_EEPROM_IO
  i32_masterVal = 0;

  // don't register proces data object, as long as it's only created with
  // default values (PRI and LIS must be in all cases different from 0xFF)
  if ( ( rui8_pri != 0xFF )
#ifdef USE_DIN_9684
       && ( rui8_lis != 0xFF )
#endif
      )
  { // now register the pointer to this instance in Process_c
   getProcessInstance4Comm().registerLocalProcessData( this );
  }
}
/** copy constructor */
ProcDataLocalBase_c::ProcDataLocalBase_c( const ProcDataLocalBase_c& rrefc_src )
  : ProcDataBase_c( rrefc_src )
{
  assignFromSource( rrefc_src );

  // now register the pointer to this instance in Process_c
  getProcessInstance4Comm().registerLocalProcessData( this );
}
/** assignment operator */
const ProcDataLocalBase_c& ProcDataLocalBase_c::operator=( const ProcDataLocalBase_c& rrefc_src )
{
  ProcDataBase_c::operator=( rrefc_src );
  assignFromSource( rrefc_src );
  return *this;
}
/** base function for assignment of element vars for copy constructor and operator= */
void ProcDataLocalBase_c::assignFromSource( const ProcDataLocalBase_c& rrefc_src )
{
  i32_masterVal = rrefc_src.i32_masterVal;
#ifdef USE_EEPROM_IO
  i32_eepromVal = rrefc_src.i32_eepromVal;
  i32_lastEepromStore = rrefc_src.i32_lastEepromStore;
  ui16_eepromAdr = rrefc_src.ui16_eepromAdr;
#endif // USE_EEPROM_IO
  b_cumulativeValue = rrefc_src.b_cumulativeValue;
}


/** default destructor which has nothing to do */
ProcDataLocalBase_c::~ProcDataLocalBase_c(){
  // now unregister the pointer to this instance in Process_c
  getProcessInstance4Comm().unregisterLocalProcessData( this );
}

#ifdef USE_EEPROM_IO
/**
  set the eeprom adr for the value, read in value from EEPROM

  possible errors:
      * dependent error in EEPROMIO_c on problems during read
  @param rui16_eepromAdr new EEPROM adress
*/
void ProcDataLocalBase_c::setEepromAdr(uint16_t rui16_eepromAdr)
{
  ui16_eepromAdr = rui16_eepromAdr;
  #ifdef USE_FLOAT_DATA_TYPE
  if (valType() == float_val)
  {
    if (ui16_eepromAdr < 0xFFFF)
    { // valid adress -> read in value
      // set read position
      getEepromInstance().setg(rui16_eepromAdr);
      // read data from eeprom
      float f_temp;
      getEepromInstance() >> f_temp;
      f_eepromVal = f_temp;
      // decide if eeprom val should be assigned as starting masterVal
      if (!b_cumulativeValue)
      { // value is NOT cumulative -> use eeprom as starting master,
        // else for cumulative values master value can be set independent from EEPROM
        // ==> for cumulative values the communicated value is sum of masterVal AND EEPROM val
        f_masterVal = f_eepromVal;
      }
    }
    else
    { // for undefined adr set value 0
      f_eepromVal = 0.0F;
    }
  }
  else
  #endif // USE_FLOAT_DATA_TYPE
  {
    if (ui16_eepromAdr < 0xFFFF)
    { // valid adress -> read in value
      // set read position
      getEepromInstance().setg(rui16_eepromAdr);
      // read data from eeprom
      int32_t i32_temp;
      getEepromInstance() >> i32_temp;
      i32_eepromVal = i32_temp;
      // decide if eeprom val should be assigned as starting masterVal
      if (!b_cumulativeValue)
      { // value is NOT cumulative -> use eeprom as starting master,
        // else for cumulative values master value can be set independent from EEPROM
        // ==> for cumulative values the communicated value is sum of masterVal AND EEPROM val
        i32_masterVal = i32_eepromVal;
      }
    }
    else
    { // for undefined adr set value 0
      i32_eepromVal = 0;
    }
  }
}
#endif // USE_EEPROM_IO

/**
  set the masterVal from main application independent from any measure progs
  @param ri32_val new measure value
*/
void ProcDataLocalBase_c::setMasterVal(int32_t ri32_val){
  // set the local values
  setValType(i32_val);
#ifdef USE_EEPROM_IO
  if (b_cumulativeValue) i32_eepromVal += (ri32_val - i32_masterVal);
  else i32_eepromVal = ri32_val;
#endif // USE_EEPROM_IO
  i32_masterVal = ri32_val;
}

/**
  increment the value -> update the local and the measuring programs values
  @param ri32_val size of increment of master value
*/
void ProcDataLocalBase_c::incrMasterVal(int32_t ri32_val){
  // set the local values
  setValType(i32_val);
#ifdef USE_EEPROM_IO
  i32_eepromVal += ri32_val;
#endif // USE_EEPROM_IO
  i32_masterVal += ri32_val;
}

#ifdef USE_FLOAT_DATA_TYPE
/**
  set the masterVal from main application independent from any measure progs
  @param rf_val new measure value
*/
void ProcDataLocalBase_c::setMasterVal(float rf_val){
  // set the local values
  setValType(float_val);
  #ifdef USE_EEPROM_IO
  if (b_cumulativeValue) f_eepromVal += (rf_val - f_masterVal);
  else f_eepromVal = rf_val;
  #endif // USE_EEPROM_IO
  f_masterVal = rf_val;
}

/**
  increment the value -> update the local and the measuring programs values
  @param ri32_val size of increment of master value
*/
void ProcDataLocalBase_c::incrMasterVal(float rf_val){
  // set the local values
  setValType(float_val);
  #ifdef USE_EEPROM_IO
  f_eepromVal += rf_val;
  #endif // USE_EEPROM_IO
  f_masterVal += rf_val;
}

#endif // USE_FLOAT_DATA_TYPE

/**
  perform periodic actions
  task for ProcDataLocal_c::timeEvent is to store the actual
  eeprom value in the defined time intervall
  @return true -> all planned executions performed
*/
bool ProcDataLocalBase_c::timeEvent( void ){
  if ( Scheduler_c::getAvailableExecTime() == 0 ) return false;
  #ifdef USE_EEPROM_IO
  // check if eeprom value should be stored
  int32_t i32_time = Scheduler_c::getLastTimeEventTrigger();
  if ((i32_time - i32_lastEepromStore > CONFIG_PROC_STORE_EEPROM_INTERVAL) && (ui16_eepromAdr != 0xFFFF))
  { // store needed
    getEepromInstance().setp(ui16_eepromAdr);
    #ifdef USE_FLOAT_DATA_TYPE
    if (valType() == float_val)
    {
      getEepromInstance() << f_eepromVal;
    }
    else
    #endif // USE_FLOAT_DATA_TYPE
    {
      getEepromInstance() << i32_eepromVal;
    }
    i32_lastEepromStore = i32_time;
  }
  #endif // USE_EEPROM_IO
  return true;
}

/**
  send a main-information (selected by MOD) to a specified target (selected by GTP)
  @param rc_targetGtp GetyPos of target
  @param ren_type optional PRI specifier of the message (default Proc_c::Target )
  @return true -> successful sent
*/
bool ProcDataLocalBase_c::sendVal( const GetyPos_c& rc_targetGtp, Proc_c::progType_t ren_progType ) const {

    // prepare general command in process pkg
    getProcessInstance4Comm().data().c_generalCommand.setValues(false /* isSetpoint */, false, /* isRequest */
                                                                GeneralCommand_c::exactValue,
                                                                GeneralCommand_c::setValue);
    // DIN: pd=1, mod=0
    #if defined(USE_EEPROM_IO) && defined(USE_FLOAT_DATA_TYPE)
    if (valType() == i32_val) return sendValGtp(ren_progType, rc_targetGtp, eepromVal());
    else return sendValGtp(ren_progType, rc_targetGtp, eepromValFloat());
    #elif !defined(USE_EEPROM_IO) && defined(USE_FLOAT_DATA_TYPE)
    if (valType() == i32_val) return sendValGtp(ren_progType, rc_targetGtp, masterVal());
    else return sendValGtp(ren_progType, rc_targetGtp, masterValFloat());
    #elif defined(USE_EEPROM_IO) && !defined(USE_FLOAT_DATA_TYPE)
    return sendValGtp(ren_progType, rc_targetGtp, eepromVal());
    #elif !defined(USE_EEPROM_IO) && !defined(USE_FLOAT_DATA_TYPE)
    return sendValGtp(ren_progType, rc_targetGtp, masterVal());
    #endif
}

/** process a measure prog message for local process data.
    this variant is only used for simple measurement progam management.
    derived classes with more flexible management (including measurement programs)
    use their own overloaded version.
*/
void ProcDataLocalBase_c::processProg(){
  ProcessPkg_c& c_pkg = getProcessInstance4Comm().data();
  // handle for simple measurement value
  // DIN: pd=3, mod=0
  if (c_pkg.c_generalCommand.checkIsRequest() &&
      // c_pkg.c_generalCommand.checkIsMeasure() &&  /* already checked before, we are in processProg() ! */
      c_pkg.c_generalCommand.getValueGroup() == GeneralCommand_c::exactValue)
  { // request for measurement value
    sendVal( c_pkg.memberSend().gtp(), Proc_c::progType_t(c_pkg.pri()) );
  }
  else
  { // DIN: PD == 0 and 1 only accepted for reset cmd (pd==1 && mod==0 || pd==0 and mod==6
    if (c_pkg.c_generalCommand.getCommand() == GeneralCommand_c::measurementReset)
    { // measurement reset cmd
      #ifdef USE_EEPROM_IO
      resetEeprom();
      #else
      setMasterVal(int32_t(0));
      #endif
      // now send result of reset action
      sendVal( c_pkg.memberSend().gtp(), Proc_c::progType_t(c_pkg.pri()) );
      // call handler function if handler class is registered
      if ( getProcessDataChangeHandler() != NULL )
        getProcessDataChangeHandler()->processMeasurementReset( this, 0, c_pkg.memberSend().gtp() );
    }
  }
}

/** processing of a setpoint message.
    this base class variant checks only, if a setpoint cmd was recieved
    which wants to reset a measurement value (this is wrongly used by some
    DIN 9684 implementations)
*/
void ProcDataLocalBase_c::processSetpoint()
{
  #ifdef RESET_MEASUREMENT_WITH_ZERO_EXACT_SETPOINT
  if (getProcessInstance4Comm().data().dataRawCmdLong() == 0)
  { // let processProg process measurement value reset -> change PD to use std. reset
    getProcessInstance4Comm().data().setPd(1);
    processProg();
  }
  #endif
}

#ifdef USE_EEPROM_IO
/**
  called from MeasureProg item -> if this item is first in list
  reset eeprom val

  possible errors:
      * dependent error in EEPROMIO_c on problems during read
  @param pc_progItem MeasureProgLocal_c instance which wants to reset EEPROM val
*/
void ProcDataLocalBase_c::resetEeprom( void ){
  // now eeprom val can be reset
  #ifdef USE_FLOAT_DATA_TYPE
  if (valType() == float_val) f_eepromVal = 0.0F;
  else
  #endif // USE_FLOAT_DATA_TYPE
    i32_eepromVal = 0;
  if (ui16_eepromAdr != 0xFFFF)
  {   // write new val to EEPROM
    getEepromInstance().setp(ui16_eepromAdr);

    #ifdef USE_FLOAT_DATA_TYPE
    if (valType() == float_val) getEepromInstance() << f_eepromVal;
    else
    #endif // USE_FLOAT_DATA_TYPE
      getEepromInstance() << i32_eepromVal;
  }
}
#endif // USE_EEPROM_IO


/**
  virtual function which check dependent on remote/local
  if send action with given var parameter and address claim state of owner is
  allowed and resolves the appropriate numbers for sender and receiver (empf)

  possible errors:
      * Err_c::elNonexistent one of resolved EMPF/SEND isn't registered with claimed address in Monitor
  @param rui8_pri PRI code of message
  @param rb_var variable number -> empf
  @param b_empf refernce to EMPF variable which is updated to rb_var
  @param b_send refernce to SEND variable which is only check for address claim state
  @param en_msgProto protocol type to use for the message
      IState_c::Din or IState_c::Iso (only compiled and used if USE_ISO_11783 is
      configured) (default: IState_c::Din)
  @return true -> owner of process data registered as active in Monitor-List
*/
bool ProcDataLocalBase_c::var2empfSend(uint8_t rui8_pri, uint8_t rb_var, uint8_t &b_empf, uint8_t &b_send
  #ifdef USE_ISO_11783
    , IState_c::itemState_t &en_msgProto
  #endif // USE_ISO_11783
) const
{ // retreive pointer to according SystemMgmt_c class
  bool b_result = false;
  #ifdef USE_ISO_11783
  uint8_t b_msgProto = static_cast<uint8_t>(en_msgProto);
  ISOMonitor_c& c_isoMonitor = getIsoMonitorInstance4Comm();
  #endif // USE_ISO_11783

  #ifdef USE_DIN_9684
  DINMonitor_c& c_din_monitor = getDinMonitorInstance4Comm();
  // check if owner has claimed address AND (var paremeter has claimed address for target messages)
  // the address claim check for var was done by caller of this function - 0xFF means not claimed address
  if (
  #ifdef USE_ISO_11783
      ((b_msgProto & (uint8_t)IState_c::Din) != 0) &&
  #endif // USE_ISO_11783
      (c_din_monitor.existDinMemberNr(rb_var))
    &&(c_din_monitor.existDinMemberGtp(ownerGtp(), true))
    &&( (rb_var != 0xFF)
      ||(rui8_pri == 1)
      )
     )
  { // all check was positive -> set b_empf, b_send
    b_empf = rb_var; // for locel data the var parameter is the receiver for sending
    b_send = c_din_monitor.dinMemberGtp(ownerGtp(), true).nr();
    #ifdef USE_ISO_11783
    en_msgProto = IState_c::Din;
    #endif // USE_ISO_11783
    b_result = true;
  }
  else
  #endif
  #if defined( USE_ISO_11783 )
  // try with ISO 11783
  if (
        ((b_msgProto & (uint8_t)IState_c::Iso) != 0) &&
        ( c_isoMonitor.existIsoMemberNr(rb_var))
      &&(c_isoMonitor.existIsoMemberGtp(ownerGtp(), true))
      &&( (rb_var != 0xFF)
        ||(rui8_pri == 1)
        )
       )
  { // all check was positive -> set b_empf, b_send
    b_empf = rb_var; // for locel data the var parameter is the receiver for senisog
    b_send = c_isoMonitor.isoMemberGtp(ownerGtp(), true).nr();
    en_msgProto = IState_c::Iso;
    b_result = true;
  }
  else
  #endif // USE_ISO_11783
  { // one of EMPF or SEND not registered as having claimed address in monior-list
    getLbsErrInstance().registerError( LibErr_c::ElNonexistent, LibErr_c::LbsProcess );
  }

  return b_result;
}

} // end of namespace __IsoAgLib
