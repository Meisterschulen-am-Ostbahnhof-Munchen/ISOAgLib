/***************************************************************************
                          procdataremotebase_c.cpp - managing of remote
                                                       process data object
                             -------------------
    begin                : Fri Apr 07 2000
    copyright            : (C) 2000 - 2004 by Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
    type                 : Header
    $LastChangedDate$
    $LastChangedRevision$
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
#include "procdataremotebase_c.h"
#include "../../impl/process_c.h"

namespace __IsoAgLib {

/**
  initialise this ProcDataRemoteBase_c instance to a well defined initial state
    ISO parameter
    @param ps_elementDDI optional pointer to array of structure IsoAgLib::ElementDDI_s which contains DDI, element, isSetpoint and ValueGroup
                         (array is terminated by ElementDDI_s.ui16_element == 0xFFFF)

    DIN parameter
    @param rui8_lis optional LIS code of this instance
    @param rui8_wert optional WERT code of this instance
    @param rui8_inst optional INST code of this instance
    @param rui8_zaehlnum optional ZAEHLNUM code of this instance

    common parameter
    @param rc_gtp optional GETY_POS code of this instance
    @param rui8_pri PRI code of messages with this process data instance (default 2)
    @param rc_ownerGtp optional GETY_POS of the owner
    @param rpc_commanderGtp pointer to updated GETY_POS variable of commander
    @param rpc_processDataChangeHandler optional pointer to handler class of application
    @param ri_singletonVecKey optional key for selection of IsoAgLib instance (default 0)
  */
void ProcDataRemoteBase_c::init(
#ifdef USE_ISO_11783
                                const IsoAgLib::ElementDDI_s* ps_elementDDI,
#endif
#ifdef USE_DIN_9684
                                uint8_t rui8_lis, uint8_t rui8_wert, uint8_t rui8_inst, uint8_t rui8_zaehlnum,
#endif
                                const GetyPos_c& rc_gtp,
                                uint8_t rui8_pri, const GetyPos_c& rc_ownerGtp,
                                const GetyPos_c* rpc_commanderGtp,
                                IsoAgLib::ProcessDataChangeHandler_c *rpc_processDataChangeHandler,
                                int ri_singletonVecKey)
{
  ProcDataBase_c::init(
#ifdef USE_ISO_11783
                       ps_elementDDI,
#endif
#ifdef USE_DIN_9684
                       rui8_lis, rui8_wert, rui8_inst, rui8_zaehlnum,
#endif

                       rc_gtp, rui8_pri, rc_ownerGtp, NULL, rpc_processDataChangeHandler);
  setSingletonKey( ri_singletonVecKey );
  setCommanderGtp(rpc_commanderGtp);

  // don't register proces data object, as long as it's only created with
  // default values (PRI and LIS must be in all cases different from 0xFF)
  if ( ( rui8_pri != 0xFF )
#ifdef USE_DIN_9684
       &&  ( rui8_lis != 0xFF )
#endif
      )
  { // now register the pointer to this instance in Process_c
    getProcessInstance4Comm().registerRemoteProcessData( this );
  }
}

/**
  assignment operator for this object
  @param rrefc_src source instance
  @return reference to source instance for cmd like "prog1 = prog2 = prog3;"
*/
const ProcDataRemoteBase_c& ProcDataRemoteBase_c::operator=(const ProcDataRemoteBase_c& rrefc_src){
  // call the assignment operator for the base class
  ProcDataBase_c::operator=(rrefc_src);
  pc_gtp = rrefc_src.pc_gtp;
  // return reference to source
  return *this;
}

/**
  copy constructor for IsoAgLibProcDataRemote
  @param rrefc_src source instance
*/
ProcDataRemoteBase_c::ProcDataRemoteBase_c(const ProcDataRemoteBase_c& rrefc_src)
  : ProcDataBase_c(rrefc_src)
{ // now copy the element var
  pc_gtp = rrefc_src.pc_gtp;

  // now register the pointer to this instance in Process_c
  getProcessInstance4Comm().registerRemoteProcessData( this );
}

/** default destructor which has nothing to do */
ProcDataRemoteBase_c::~ProcDataRemoteBase_c(){
  // now unregister the pointer to this instance in Process_c
  getProcessInstance4Comm().unregisterRemoteProcessData( this );
}
/**
  set the pointer to the commander ident gtp
  @param rpbgtp pointer to GETY_POS var of local member used for
              sending commands to remote owner member
*/
void ProcDataRemoteBase_c::setCommanderGtp(const GetyPos_c* rpc_gtp)
{
    pc_gtp = rpc_gtp;
};

/**
  perform periodic actions
  ProcDataRemoteBase_c::timeEvent
  -> adopt here the ownerGtp to an existing item, when DevClass/-Instance are matching, but the other fields are
     differen ( don't change anything, if there is an item with identic GTP setting
  @return true -> all planned executions performed
*/
bool ProcDataRemoteBase_c::timeEvent( void )
{
  return true;
}


/**
  virtual function which check dependent on remote/local
  if send action with given var parameter and address claim state of owner is
  allowed and resolves the appropriate numbers for sender and receiver (empf)

  possible errors:
      * Err_c::elNonexistent one of resolved EMPF/SEND isn't registered with claimed address in Monitor
  @param rui8_pri PRI code of message
  @param rb_var variable number -> send
  @param b_empf refernce to EMPF variable which is only checked for address claim state
  @param b_send refernce to SEND variable which is updated to rb_var
  @param en_msgProto protocol type to use for the message
      IState_c::Din or IState_c::Iso (only compiled and used if USE_ISO_11783 is
      configured) (default: IState_c::Din)
  @return true -> owner of process data registered as active in Monitor-List
*/
bool ProcDataRemoteBase_c::var2empfSend(uint8_t rui8_pri, uint8_t rb_var, uint8_t &b_empf, uint8_t &b_send
  #ifdef USE_ISO_11783
    , IState_c::itemState_t &en_msgProto
  #endif
) const
{ // retreive pointer to according SystemMgmt_c class
  #ifdef USE_ISO_11783
  ISOMonitor_c& c_isoMonitor = getIsoMonitorInstance4Comm();
  #endif
  bool b_result = false;
  rui8_pri += 0; // do something with rui8_pri so that compiler does not complain

  #ifdef USE_DIN_9684
  DINMonitor_c& c_din_monitor = getDinMonitorInstance4Comm();
  // check if owner has claimed address AND var paremeter has claimed address
  // the address claim check for var was done by caller of this function - 0xFF means not claimed address
  if (
      #ifdef USE_ISO_11783
      ((en_msgProto & IState_c::Din) != 0) &&
      #endif
      ( c_din_monitor.existDinMemberNr(rb_var))
    &&(c_din_monitor.existDinMemberGtp(ownerGtp(), true))
    &&(rb_var != 0xFF)
     )
  { // all check was positive -> set b_empf, b_send
    b_empf = c_din_monitor.dinMemberGtp(ownerGtp(), true).nr();
    b_send = rb_var; // for remote data the var parameter is the sender for sending
    #ifdef USE_ISO_11783
    en_msgProto = IState_c::Din;
    #endif
    b_result = true;
  }
  else
  #endif
  #if defined( USE_ISO_11783 )
  // try with ISO 11783
  if (
      ((en_msgProto & IState_c::Iso) != 0) &&
      (c_isoMonitor.existIsoMemberNr(rb_var ))
    &&(c_isoMonitor.existIsoMemberGtp(ownerGtp(), true))
    &&(rb_var != 0xFF)
     )
  { // all check was positive -> set b_empf, b_send
    b_empf = c_isoMonitor.isoMemberGtp(ownerGtp(), true).nr();
    b_send = rb_var; // for remote data the var parameter is the sender for senisog
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
