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
#include "../Remote/Std/impl/procdataremote_c.h"

namespace __IsoAgLib {

/**
   initialise this ProcDataBase_c instance to a well defined initial state
   ISO parameters:
   @param ps_elementDDI optional pointer to array of structure IsoAgLib::ElementDDI_s which contains DDI, element, isSetpoint and ValueGroup
                        (array is terminated by ElementDDI_s.ui16_element == 0xFFFF)

   DIN parameters:
   @param rpc_lbs optional pointer to central Scheduler_c instance
   @param rui8_lis optional LIS code of this instance
   @param rui8_wert optional WERT code of this instance
   @param rui8_inst optional INST code of this instance
   @param rui8_zaehlnum optional ZAEHLNUM code of this instance

   common parameters:
   @param rc_gtp optional GETY_POS code of Process-Data
   @param rui8_pri PRI code of messages with this process data instance (default 2)
   @param rc_ownerGtp optional GETY_POS of the owner
   @param rpc_gtp pointer to updated GETY_POS variable of owner
   @param rpc_processDataChangeHandler optional pointer to handler class of application
   @param ri_singletonVecKey optional key for selection of IsoAgLib instance (default 0)
*/
  void ProcDataBase_c::init(
#ifdef USE_ISO_11783
                            const IsoAgLib::ElementDDI_s* ps_elementDDI,
#endif
#ifdef USE_DIN_9684
                            uint8_t rui8_lis, uint8_t rui8_wert,
                            uint8_t rui8_inst, uint8_t rui8_zaehlnum ,
#endif
                            const GetyPos_c& rc_gtp,
                            uint8_t rui8_pri, const GetyPos_c& rc_ownerGtp, const GetyPos_c *rpc_gtp,
                            IsoAgLib::ProcessDataChangeHandler_c *rpc_processDataChangeHandler,
                            int ri_singletonVecKey)
  {
  ProcIdent_c::init(
#ifdef USE_ISO_11783
                    ps_elementDDI,
#endif
#ifdef USE_DIN_9684
                    rui8_lis, rui8_wert, rui8_inst, rui8_zaehlnum,
#endif
                    rc_gtp, rui8_pri, rc_ownerGtp, rpc_gtp);

  setSingletonKey(ri_singletonVecKey);
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
  process a message, which is adressed for this process data item;
  ProcDataBase_c::processMsg() is responsible to delegate the
  processing of setpoint and measurement messages to the appripriate
  functions processSetpoint and processProg;
  both functions are virtual, so that depending on loacl or remote
  process data the suitable reaction can be implemented
*/
void ProcDataBase_c::processMsg() {

  // generalize command
  if (!getProcessInstance4Comm().data().resolveCommandType())
    // failure
    return;

  if (getProcessInstance4Comm().data().c_generalCommand.checkIsSetpoint())
    processSetpoint();
  else
    processProg();

}


/** perform periodic acoins */
bool ProcDataBase_c::timeEvent( void ){
  return true;
}


/**
  send the given int32_t value with variable GETY_POS rc_varGtp
  (local: receiver; remote: sender)
  (other paramter fixed by ident of process data)

  set general command before sendValGtp !

  possible errors:
      * Err_c::elNonexistent one of resolved EMPF/SEND isn't registered with claimed address in Monitor
      * dependant error in CANIO_c on CAN send problems
  @param rui8_pri PRI code for the msg
  @param rc_varGtp variable GETY_POS
  @param ri32_val int32_t value to send
  @return true -> sendIntern set successful EMPF and SEND
*/
bool ProcDataBase_c::sendValGtp(uint8_t rui8_pri, const GetyPos_c& rc_varGtp, int32_t ri32_val) const
{
  bool b_result;
  if (resolvGtpSetBasicSendFlags(rui8_pri, rc_varGtp))
  { // now call sendIntern, if var2empfSend was successful
    // set PD, MOD, and data with the function setPkgDataLong
    getProcessPkg().setData( ri32_val, en_procValType);

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

#ifdef USE_FLOAT_DATA_TYPE
/**
  send the given float value with variable GETY_POS rc_varGtp
  (local: receiver; remote: sender)
  (other paramter fixed by ident of process data)

  set general command before sendValGtp !

  possible errors:
      * Err_c::elNonexistent one of resolved EMPF/SEND isn't registered with claimed address in Monitor
      * dependant error in CANIO_c on CAN send problems
  @param rui8_pri PRI code for the msg
  @param rc_varGtp variable GETY_POS
  @param ri32_val float value to send
  @return true -> sendIntern set successful EMPF and SEND
*/
bool ProcDataBase_c::sendValGtp(uint8_t rui8_pri, const GetyPos_c& rc_varGtp, float rf_val) const {
  bool b_result;
  if (resolvGtpSetBasicSendFlags(rui8_pri, rc_varGtp))
  { // now call sendIntern, if var2empfSend was successful
    // set PD, MOD, and data with the function setPkgDataLong
    getProcessPkg().setData( rf_val, en_procValType);

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
bool ProcDataBase_c::resolvGtpSetBasicSendFlags(uint8_t rui8_pri, const GetyPos_c& rc_varGtp) const {
  uint8_t ui8_empf = 0xFF,
      ui8_send = 0xFF;
  bool b_result = false;
  #ifdef USE_ISO_11783
  IState_c::itemState_t en_msgProto = IState_c::Din;
  #endif

  #ifdef USE_DIN_9684
  // retreive pointer to te according DINMonitor_c class
  static DINMonitor_c& c_din_monitor = getDinMonitorInstance4Comm();

  // check is the var parameter has claimed address - the variable device is totally known by GTP: the requesting device for local proc, the commanding for remote proc
  if (c_din_monitor.existDinMemberGtp(rc_varGtp, true))
  { // the member to rc_varGtp has claimed address (virtual function -> worksdependent on child type local/remote)
    b_result = var2empfSend(rui8_pri, (c_din_monitor.dinMemberGtp(rc_varGtp, true).nr()),
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
    if (c_isoMonitor.existIsoMemberGtp(rc_varGtp, true ))
    { // the member to rc_varGtp has claimed address (virtual function -> worksdependent on child type local/remote)
      en_msgProto = IState_c::Iso;
      b_result = var2empfSend(rui8_pri, (c_isoMonitor.isoMemberGtp(rc_varGtp, true).nr()),
          ui8_empf, ui8_send, en_msgProto);
    }
  }
  #endif

  if ( !b_result )
  { // the var paramter hasn't claimed address -> signal it by 0xFF to var2empfSend
    en_msgProto = (IState_c::itemState_t(IState_c::Din | IState_c::Iso));
    b_result = var2empfSend(rui8_pri, 0xFF, ui8_empf, ui8_send
      #ifdef USE_ISO_11783
        , en_msgProto
      #endif
      );
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

    // general command is already set, use these values:

#ifdef USE_ISO_11783
    // set command in ProcessPkg::flags2string

    const GeneralCommand_c::ValueGroup_t en_valueGroup = getProcessInstance4Comm().data().c_generalCommand.getValueGroup();
    const bool b_isSetpoint = getProcessInstance4Comm().data().c_generalCommand.checkIsSetpoint();

    // @todo: in case no element/DDI fits send default values
    c_data.set_Element(0xFFFF);
    c_data.set_DDI(0);

    std::list<IsoAgLib::ElementDDI_s>::const_iterator iter_elementDDI;

    if (elementDDI().size() == 1) {
      // we have only one DDI/element pair
      // interface process data init was possibly called with parameter DDI and element and not with ElementDDI_s
      // => we don't have reliable infos about en_valueGroup and b_isSetpoint
      // => don't check for en_valueGroup and b_isSetpoint but use this single entry in list
      iter_elementDDI = elementDDI().begin();
      c_data.set_Element(iter_elementDDI->ui16_element);
      c_data.set_DDI(iter_elementDDI->ui16_DDI);
    }
    else
    {
      // get corresponding element/DDI
      for (iter_elementDDI = elementDDI().begin();
           iter_elementDDI != elementDDI().end(); iter_elementDDI++)
        if ( (iter_elementDDI->en_valueGroup == en_valueGroup) && (iter_elementDDI->b_isSetpoint == b_isSetpoint) ) {
          c_data.set_Element(iter_elementDDI->ui16_element);
          c_data.set_DDI(iter_elementDDI->ui16_DDI);
          break;
        }
    }

#endif

#ifdef USE_DIN_9684

    // set pd and mod in ProcessPkg::flags2string
    c_data.setLis(lis());
    c_data.setZaehlnum(zaehlnum());
    c_data.setWert(wert());
    c_data.setInst(inst());
    c_data.setGtp(gtp());
#endif

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

#ifdef USE_ISO_11783
/**
  delivers item state (DIN/ISO) for given gtp
  @param rc_gtp compared GETY_POS value
  @return IState_c::itemState_t
*/
IState_c::itemState_t ProcDataBase_c::getIStateForGtp( const GetyPos_c& rc_gtp )
{
  IState_c::itemState_t en_msgProto = IState_c::IstateNull;

#if defined(USE_DIN_9684) && !defined(USE_ISO_11783)
  if ( getDinMonitorInstance4Comm().existDinMemberGtp(rc_gtp, true) )
    en_msgProto = IState_c::Din;
#endif

#if !defined(USE_DIN_9684) && defined(USE_ISO_11783)
  if ( getIsoMonitorInstance4Comm().existIsoMemberGtp(rc_gtp, true) )
    en_msgProto = IState_c::Iso;
#endif

#if defined(USE_DIN_9684) && defined(USE_ISO_11783)
  if ( getSystemMgmtInstance4Comm().existMemberGtp(rc_gtp, true, IState_c::IsoOnly) )
    en_msgProto = IState_c::Iso;
  if ( getSystemMgmtInstance4Comm().existMemberGtp(rc_gtp, true, IState_c::DinOnly) )
    en_msgProto = IState_c::Din;
#endif

  return en_msgProto;
}
#endif

} // end of namespace __IsoAgLib
