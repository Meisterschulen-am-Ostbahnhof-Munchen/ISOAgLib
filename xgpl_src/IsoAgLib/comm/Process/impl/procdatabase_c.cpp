/***************************************************************************
                          procdatabase_c.cpp - base class for local
                                                     or remote process data
                                                     object
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
#include "procdatabase_c.h"
#include "process_c.h"
#include <IsoAgLib/driver/can/impl/canio_c.h>

namespace __IsoAgLib {

/**
  initialise this ProcDataBase_c instance to a well defined initial state
  @param rpc_lbs optional pointer to central Scheduler_c instance
  @param rui8_lis optional LIS code of this instance
  @param rc_gtp optional GETY_POS code of Process-Data
  @param rui8_wert optional WERT code of this instance
  @param rui8_inst optional INST code of this instance
  @param rui8_zaehlnum optional ZAEHLNUM code of this instance
  @param rui8_pri PRI code of messages with this process data instance (default 2)
  @param rc_ownerGtp optional GETY_POS of the owner
  @param rpc_gtp pointer to updated GETY_POS variable of owner
  @param rpc_processDataChangeHandler optional pointer to handler class of application
  @param ri_singletonVecKey optional key for selection of IsoAgLib instance (default 0)
  */
void ProcDataBase_c::init(uint8_t rui8_lis, GetyPos_c rc_gtp, uint8_t rui8_wert,
      uint8_t rui8_inst, uint8_t rui8_zaehlnum, uint8_t rui8_pri, GetyPos_c rc_ownerGtp, GetyPos_c *rpc_gtp,
      IsoAgLib::ProcessDataChangeHandler_c *rpc_processDataChangeHandler,
      int ri_singletonVecKey)
{
  ProcIdent_c::init(rui8_lis, rc_gtp, rui8_wert, rui8_inst, rui8_zaehlnum, rui8_pri, rc_ownerGtp, rpc_gtp);
  setSingletonKey(ri_singletonVecKey);
  i32_sendConversion = 1; // change nothing with using conversion
  ui16_internalUnit = 0;
  en_procValType = i32_val;
  pc_processDataChangeHandler = rpc_processDataChangeHandler;
}
/**
  assignment operator for this base object
  @param rrefc_src source instance
  @return reference to source instance for cmd like "prog1 = prog2 = prog3;"
*/
const ProcDataBase_c& ProcDataBase_c::operator=(const ProcDataBase_c& rrefc_src)
{ // call base class operator
  ProcIdent_c::operator=(rrefc_src);

  assignFromSource(rrefc_src);

  // return source reference
  return *this;
}

/**
  copy constructor for ProcDataBase_c
  @param rrefc_src source instance
*/
ProcDataBase_c::ProcDataBase_c(const ProcDataBase_c& rrefc_src)
   : ProcIdent_c(rrefc_src) {
  assignFromSource(rrefc_src);
}
/** base function for assignment of element vars for copy constructor and operator= */
void ProcDataBase_c::assignFromSource( const ProcDataBase_c& rrefc_src )
{ // copy element vars
  i32_sendConversion = rrefc_src.i32_sendConversion;
  ui16_internalUnit = rrefc_src.ui16_internalUnit;
  en_procValType = rrefc_src.en_procValType;
  pc_processDataChangeHandler = rrefc_src.pc_processDataChangeHandler;
}

/** default destructor which has nothing to do */
ProcDataBase_c::~ProcDataBase_c(){
}
/** helper function to get reference to process data pkg very quick */
ProcessPkg_c& ProcDataBase_c::getProcessPkg( void ) const
{
#if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
  // if more than one CAN BUS is used for IsoAgLib (DIN or ISO)
  // no single cache value can be used, as each CAN BUS get its
  // individual ProcessPkg_c (a static functional-variable would
  // be used for ALL instances of ProcDataBase_c - also from
  // different CAN BUS'es)
  return getProcessInstance4Comm().data();
#else
  static ProcessPkg_c& c_data = getProcessInstance4Comm().data();
  return c_data;
#endif
}

/**
  deliver the pkg data value as int32_t;
  if pd/mod are one of the to be converted one, return converted;
  the value is read from message in the data type format which is
  defined in the message; on different types, a conversion by assignment
  is performed -> %e.g. if message has float val 4.2 the result of this
  function is 4
  @return data value as int32_t value (converted if needed)
*/
int32_t ProcDataBase_c::pkgDataLong()const
{
  int32_t i32_result = getProcessPkg().dataLong();
  if (getProcessPkg().isConvertableVal())
    return (i32_result * sendConversion());
  else
    return i32_result;
}
#ifdef USE_ISO_11783
/**
  deliver the pkg data value as uint32_t;
  if pd/mod are one of the to be converted one, return converted;
  the value is read from message in the data type format which is
  defined in the message; on different types, a conversion by assignment
  is performed -> %e.g. if message has float val 4.2 the result of this
  function is 4
  @return data value as uint32_t value (converted if needed)
*/
uint32_t ProcDataBase_c::pkgDataUlong()const
{
  if (getProcessPkg().isConvertableVal())return (getProcessPkg().dataUlong() * sendConversion());
  else return getProcessPkg().dataUlong();
}
#endif
#ifdef USE_FLOAT_DATA_TYPE
/**
  deliver the pkg data value as float;
  if pd/mod are one of the to be converted one, return converted;
  the value is read from message in the data type format which is
  defined in the message; on different types, a conversion by assignment
  is performed -> %e.g. if message has int32_t 4 the result of this function
  is float 4.0
  @return data value as float value (converted if needed)
*/
float ProcDataBase_c::pkgDataFloat()const
{
  if (getProcessPkg().isConvertableVal())return (getProcessPkg().dataFloat() * static_cast<float>(sendConversion()));
  else return getProcessPkg().dataFloat();
}
#endif

/**
  process a message, which is adressed for this process data item;
  ProcDataBase_c::processMsg() is responsible to delegate the
  processing of setpoint and measurement messages to the appripriate
  functions processSetpoint and processProg;
  both functions are virtual, so that depending on loacl or remote
  process data the suitable reaction can be implemented
*/
void ProcDataBase_c::processMsg(){
  // check if the message is about setpoint or measure prog
  if (((getProcessInstance4Comm().data().pd() & 0x1) == 0)
      && (getProcessInstance4Comm().data().mod() < 4))
  { // pd {00, 10} with mod {000,001,010,100} are setpoint messages
    processSetpoint();
  }
  else
  { // otherwise it is an measure prog message
    processProg();
  }
}

/** perform periodic acoins */
bool ProcDataBase_c::timeEvent( void ){
  return true;
}

/**
  send the given int32_t command value with variable GETY_POS rc_varGtp;
  set the cmd value without conversion in message string and set data
  format flags corresponding to central data type of this process data
  (local: receiver; remote: sender)
  (other paramter fixed by ident of process data)

  possible errors:
      * Err_c::elNonexistent one of resolved EMPF/SEND isn't registered with claimed address in Monitor
      * dependant error in CANIO_c on CAN send problems
  @param rui8_pri PRI code for the msg
  @param rc_varGtp variable GETY_POS
  @param rb_pd PD code for the msg
  @param rb_mod MOD code for the msg
  @param ri32_cmdVal int32_t command value to send
  @return true -> sendIntern set successful EMPF and SEND
*/
bool ProcDataBase_c::sendDataRawCmdGtp(uint8_t rui8_pri, GetyPos_c rc_varGtp, uint8_t rb_pd, uint8_t rb_mod, int32_t ri32_val) const{
  bool b_result;
  // let resolvGtpSetBasicSendFlags resolv suitable EMPF, SEND codes
  // and set basic flags on success (everything but the value)
  if (resolvGtpSetBasicSendFlags(rui8_pri, rc_varGtp, rb_pd, rb_mod))
  { // now call sendIntern, if var2empfSend was successful
    // now set the 4byte value
    getProcessPkg().setDataRawCmd(ri32_val, en_procValType);

    // send the msg
    getCanInstance4Comm() << getProcessPkg();
    b_result = true;
  }
  else
  { // EMPF and/or SEND not registered with claimed address in Monitor-List
    getLbsErrInstance().registerError( LibErr_c::ElNonexistent, LibErr_c::LbsProcess );
    b_result = false;
  }
  return b_result;
}


/**
  send the given int32_t value with variable GETY_POS rc_varGtp
  (local: receiver; remote: sender)
  (other paramter fixed by ident of process data)

  possible errors:
      * Err_c::elNonexistent one of resolved EMPF/SEND isn't registered with claimed address in Monitor
      * dependant error in CANIO_c on CAN send problems
  @param rui8_pri PRI code for the msg
  @param rc_varGtp variable GETY_POS
  @param rb_pd PD code for the msg
  @param rb_mod MOD code for the msg
  @param ri32_val int32_t value to send
  @return true -> sendIntern set successful EMPF and SEND
*/
bool ProcDataBase_c::sendValGtp(uint8_t rui8_pri, GetyPos_c rc_varGtp, uint8_t rb_pd, uint8_t rb_mod, int32_t ri32_val) const {
  bool b_result;
  if (resolvGtpSetBasicSendFlags(rui8_pri, rc_varGtp, rb_pd, rb_mod))
  { // now call sendIntern, if var2empfSend was successful
    // set PD, MOD, and data with the function setPkgDataLong -> it uses conversion factor if needed
    bool b_convert = false;
    if (rb_pd == 0)
    { // setpoint value -> conversion if: exact, min or max setpoint
      if ((rb_mod==0)||(rb_mod==2)||(rb_mod==3))
      { // conversion is needed
        if ((ri32_val != static_cast<int32_t>(SETPOINT_RELEASE_COMMAND))
         && (ri32_val != static_cast<int32_t>(SETPOINT_ERROR_COMMAND))) b_convert = true;
      }
    }
    else
    { // measure value: conversion if: actual, min, max, integ, med
      if (rb_mod < 5)
      { // conversion is needed
        if ((ri32_val != static_cast<int32_t>(NO_VAL_32S))
         && (ri32_val != static_cast<int32_t>(ERROR_VAL_32S))) b_convert = true;
      }
    }
    if (b_convert) getProcessPkg().setData( static_cast<int32_t>(ri32_val/sendConversion()), en_procValType);
    else getProcessPkg().setData( ri32_val, en_procValType);

    // send the msg
    getCanInstance4Comm() << getProcessPkg();
    b_result = true;
  }
  else
  { // EMPF and/or SEND not registered with claimed address in Monitor-List
    getLbsErrInstance().registerError( LibErr_c::ElNonexistent, LibErr_c::LbsProcess );
    b_result = false;
  }
  return b_result;
}

#ifdef USE_ISO_11783
/**
  send the given uint32_t value with variable GETY_POS rc_varGtp
  (local: receiver; remote: sender)
  (other paramter fixed by ident of process data)

  possible errors:
      * Err_c::elNonexistent one of resolved EMPF/SEND isn't registered with claimed address in Monitor
      * dependant error in CANIO_c on CAN send problems
  @param rui8_pri PRI code for the msg
  @param rc_varGtp variable GETY_POS
  @param rb_pd PD code for the msg
  @param rb_mod MOD code for the msg
  @param ri32_val uint32_t value to send
  @return true -> sendIntern set successful EMPF and SEND
*/
bool ProcDataBase_c::sendValGtp(uint8_t rui8_pri, GetyPos_c rc_varGtp, uint8_t rb_pd, uint8_t rb_mod, uint32_t rulVal) const {
  bool b_result;
  if (resolvGtpSetBasicSendFlags(rui8_pri, rc_varGtp, rb_pd, rb_mod))
  { // now call sendIntern, if var2empfSend was successful
    // set PD, MOD, and data with the function setPkgDataLong -> it uses conversion factor if needed
    bool b_convert = false;
    if (rb_pd == 0)
    { // setpoint value -> conversion if: exact, min or max setpoint
      if ((rb_mod==0)||(rb_mod==2)||(rb_mod==3))
      { // conversion is needed
        if ((rulVal != static_cast<uint32_t>(SETPOINT_RELEASE_COMMAND))
         && (rulVal != static_cast<uint32_t>(SETPOINT_ERROR_COMMAND))) b_convert = true;
      }
    }
    else
    { // measure value: conversion if: actual, min, max, integ, med
      if (rb_mod < 5)
      { // conversion is needed
        if ((rulVal != static_cast<uint32_t>(NO_VAL_32S))
         && (rulVal != static_cast<uint32_t>(ERROR_VAL_32S))) b_convert = true;
      }
    }
    if (b_convert) getProcessPkg().setData( (rulVal/sendConversion()), en_procValType);
    else getProcessPkg().setData( rulVal, en_procValType);

    // send the msg
    getCanInstance4Comm() << getProcessPkg();
    b_result = true;
  }
  else
  { // EMPF and/or SEND not registered with claimed address in Monitor-List
    getLbsErrInstance().registerError( LibErr_c::ElNonexistent, LibErr_c::LbsProcess );
    b_result = false;
  }
  return b_result;
}
#endif
#ifdef USE_FLOAT_DATA_TYPE
/**
  send the given float value with variable GETY_POS rc_varGtp
  (local: receiver; remote: sender)
  (other paramter fixed by ident of process data)

  possible errors:
      * Err_c::elNonexistent one of resolved EMPF/SEND isn't registered with claimed address in Monitor
      * dependant error in CANIO_c on CAN send problems
  @param rui8_pri PRI code for the msg
  @param rc_varGtp variable GETY_POS
  @param rb_pd PD code for the msg
  @param rb_mod MOD code for the msg
  @param ri32_val float value to send
  @return true -> sendIntern set successful EMPF and SEND
*/
bool ProcDataBase_c::sendValGtp(uint8_t rui8_pri, GetyPos_c rc_varGtp, uint8_t rb_pd, uint8_t rb_mod, float rf_val) const {
  bool b_result;
  if (resolvGtpSetBasicSendFlags(rui8_pri, rc_varGtp, rb_pd, rb_mod))
  { // now call sendIntern, if var2empfSend was successful
    // set PD, MOD, and data with the function setPkgDataLong -> it uses conversion factor if needed
    bool b_convert = false;
    if (rb_pd == 0)
    { // setpoint value -> conversion if: exact, min or max setpoint
      if ((rb_mod==0)||(rb_mod==2)||(rb_mod==3))
      { // conversion is needed
        if ((rf_val != static_cast<float>(SETPOINT_RELEASE_COMMAND))
         && (rf_val != static_cast<float>(SETPOINT_ERROR_COMMAND))) b_convert = true;
      }
    }
    else
    { // measure value: conversion if: actual, min, max, integ, med
      if (rb_mod < 5)
      { // conversion is needed
        if ((rf_val != static_cast<float>(NO_VAL_32S))
         && (rf_val != static_cast<float>(ERROR_VAL_32S))) b_convert = true;
      }
    }
    if (b_convert) getProcessPkg().setData( (rf_val/sendConversion()), en_procValType);
    else getProcessPkg().setData( rf_val, en_procValType);

    // send the msg
    getCanInstance4Comm() << getProcessPkg();
    b_result = true;
  }
  else
  { // EMPF and/or SEND not registered with claimed address in Monitor-List
    getLbsErrInstance().registerError( LibErr_c::ElNonexistent, LibErr_c::LbsProcess );
    b_result = false;
  }
  return b_result;
}
#endif

/**
  resolv SEND|EMPF dependent on GETY_POS rc_varGtp
  (local: receiver; remote: sender)
  (other paramter fixed by ident of process data)
  and set basic value independent flags in ProcessPkg

  the PRI code of the message is set dependent of the ProcIdent_c PRI
  code for this process data instance, if no basic proc data (rui8_pri == 1)
  is wanted

  @param rui8_pri PRI code for the msg
  @param rc_varGtp variable GETY_POS
  @param rb_pd PD code for the msg
  @param rb_mod MOD code for the msg
  @return true -> resolvSendGtp successfully resolved EMPF and SEND
*/
bool ProcDataBase_c::resolvGtpSetBasicSendFlags(uint8_t rui8_pri, GetyPos_c rc_varGtp, uint8_t rb_pd, uint8_t rb_mod) const {
  uint8_t ui8_empf = 0xFF,
      ui8_send = 0xFF;
  bool b_result = false;
  #ifdef USE_ISO_11783
  IState_c::itemState_t en_msgProto = IState_c::Din;
  #endif

  #ifdef USE_DIN_9684
  // retreive pointer to te according DINMonitor_c class
  static DINMonitor_c& c_din_monitor = getDinMonitorInstance4Comm();

  // check is the var parameter has claimed address
  if ( (c_din_monitor.existDinMemberGtp(rc_varGtp))
    && (c_din_monitor.dinMemberGtp(rc_varGtp).itemState(IState_c::ClaimedAddress)))
  { // the member to rc_varGtp has claimed address (virtual function -> worksdependent on child type local/remote)
    b_result = var2empfSend(rui8_pri, (c_din_monitor.dinMemberGtp(rc_varGtp).nr()),
                  ui8_empf,
                  ui8_send
                  #ifdef USE_ISO_11783
                  , en_msgProto
                  #endif
                  );
  }
  #endif
  #ifdef USE_ISO_11783
  // retreive pointer to te accorisog ISOMonitor_c class
  static ISOMonitor_c& c_isoMonitor = getIsoMonitorInstance4Comm();

   // try with ISO communication
   if (!b_result)
  {
    if ((c_isoMonitor.existIsoMemberGtp(rc_varGtp))
     && (c_isoMonitor.isoMemberGtp(rc_varGtp).itemState(IState_c::ClaimedAddress)))
    { // the member to rc_varGtp has claimed address (virtual function -> worksdependent on child type local/remote)
      en_msgProto = IState_c::Iso;
      b_result = var2empfSend(rui8_pri, (c_isoMonitor.isoMemberGtp(rc_varGtp).nr()),
          ui8_empf, ui8_send, en_msgProto);
    }
    else
    { // the var paramter hasn't claimed address -> signal it by 0xFF to var2empfSend
      en_msgProto = (IState_c::itemState_t(IState_c::Din | IState_c::Iso));
  #else
    else
    { // the var paramter hasn't claimed address -> signal it by 0xFF to var2empfSend
  #endif
      b_result = var2empfSend(rui8_pri, 0xFF, ui8_empf, ui8_send
        #ifdef USE_ISO_11783
          , en_msgProto
        #endif
        );
  #ifdef USE_ISO_11783
    }
  #endif
  }

  if (b_result)
  { // resolv successful -> set basic flags in ProcessPkg
    ProcessPkg_c& c_data = getProcessPkg();
    #ifdef USE_ISO_11783
    if (en_msgProto == IState_c::Din)
    {
    #endif
      // if target message is wanted, send with PRI of this proc data message
      if (rui8_pri != 1)
      {
        c_data.setPri(pri());
        // set receiver number for target msg
        c_data.setEmpf(ui8_empf);
      }
      else c_data.setPri(1);
      c_data.setSend(ui8_send);
    #ifdef USE_ISO_11783
    }
    else
    { // the communicating devices are represented on ISO11783
      c_data.setIsoPri(3);
      c_data.setIsoPgn(PROCESS_DATA_PGN);
      c_data.setIsoPs(ui8_empf);
      c_data.setIsoSa(ui8_send);
    }
    #endif

    c_data.setPd(rb_pd);
    c_data.setMod(rb_mod);
    c_data.setLis(lis());
    c_data.setZaehlnum(zaehlnum());
    c_data.setGtp(gtp());
    c_data.setWert(wert());
    c_data.setInst(inst());
  }
  return b_result;
}

/**
  process a measure prog message
  -> fully dependent on children type local/remote
*/
void ProcDataBase_c::processProg()
{
  return;
}
/**
  virtual base for processing of a setpoint message
*/
void ProcDataBase_c::processSetpoint()
{
  return;
}


} // end of namespace __IsoAgLib
