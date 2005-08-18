/***************************************************************************
                          procdataremotebase_c.h  - managing of remote
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
#ifndef PROCDATA_REMOTE_BASE_H
#define PROCDATA_REMOTE_BASE_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include "../../impl/procdatabase_c.h"

// Begin Namespace IsoAgLib
namespace __IsoAgLib {

/**
  Common base class for all remote process data independent from the
  individual feature set.
  This class is responsible for deduction of dynamic member adresses ( %i.e. SA for ISO 11783 )
  dependend on device type of managing ECU and this local ECU ( which interacts with the remote ECU )
  ( \sa ProcDataRemoteBase_c::var2empfSend ).
 */
class ProcDataRemoteBase_c : public ProcDataBase_c
{
 public:
  /**
    constructor which can set all element vars
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
  ProcDataRemoteBase_c(
#ifdef USE_ISO_11783
                       const IsoAgLib::ElementDDI_s* ps_elementDDI = NULL,
#endif
#ifdef USE_DIN_9684
                       uint8_t rui8_lis = 0xFF, uint8_t rui8_wert = 0, uint8_t rui8_inst = 0, uint8_t rui8_zaehlnum = 0xFF,
#endif
                       const GetyPos_c& rc_gtp = GetyPos_c::GetyPosInitialProcessData,
                       uint8_t rui8_pri = 2, const GetyPos_c& rc_ownerGtp = GetyPos_c::GetyPosUnspecified,
                       const GetyPos_c* rpc_commanderGtp = NULL,
                       IsoAgLib::ProcessDataChangeHandler_c *rpc_processDataChangeHandler = NULL,
                       int ri_singletonVecKey = 0)
  { init(
#ifdef USE_ISO_11783
         ps_elementDDI,
#endif
#ifdef USE_DIN_9684
         rui8_lis, rui8_wert, rui8_inst, rui8_zaehlnum,
#endif
         rc_gtp, rui8_pri, rc_ownerGtp, rpc_commanderGtp,
         rpc_processDataChangeHandler, ri_singletonVecKey);};
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
  void init(
#ifdef USE_ISO_11783
            const IsoAgLib::ElementDDI_s* ps_elementDDI,
#endif
#ifdef USE_DIN_9684
            uint8_t rui8_lis = 0xFF, uint8_t rui8_wert = 0, uint8_t rui8_inst = 0, uint8_t rui8_zaehlnum = 0xFF,
#endif
            const GetyPos_c& rc_gtp = GetyPos_c::GetyPosInitialProcessData,
            uint8_t rui8_pri = 2, const GetyPos_c& rc_ownerGtp = GetyPos_c::GetyPosUnspecified,
            const GetyPos_c* rpc_commanderGtp = NULL,
            IsoAgLib::ProcessDataChangeHandler_c *rpc_processDataChangeHandler = NULL,
            int ri_singletonVecKey = 0);

   /**
    assignment operator for this object
    @param rrefc_src source instance
    @return reference to source instance for cmd like "prog1 = prog2 = prog3;"
  */
  const ProcDataRemoteBase_c& operator=(const ProcDataRemoteBase_c& rrefc_src);

  /**
    copy constructor for IsoAgLibProcDataRemote
    @param rrefc_src source instance
  */
   ProcDataRemoteBase_c(const ProcDataRemoteBase_c& rrefc_src);

  /** default destructor which has nothing to do */
  ~ProcDataRemoteBase_c();


  /**
    deliver the commanderGtp (GETY_POS of local member)
    @return GETY_POS used for sending commands to remote owner member
  */
  virtual const GetyPos_c& commanderGtp()const{return (pc_gtp != NULL)?*pc_gtp:GetyPos_c::GetyPosUnspecified;};
  /**
    set the pointer to the commander ident gtp
    @param rpbgtp pointer to GETY_POS var of local member used for
                sending commands to remote owner member
  */
  virtual void setCommanderGtp(const GetyPos_c* rpc_gtp);

  /**
    perform periodic actions
    ProcDataRemoteBase_c::timeEvent has nothing to do
    -> this function must only be defined as base for derived variants which
    uses simple measurements and thus doesn't need a time event
    @return true -> all planned executions performed
  */
  virtual bool timeEvent( void );

private: // Private methods
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
  virtual bool var2empfSend(uint8_t rui8_pri, uint8_t rb_var, uint8_t &b_empf, uint8_t &b_send
  #ifdef USE_ISO_11783
    , IState_c::itemState_t &en_msgProto
  #endif
    ) const;


private: // Private attributes
  /** pointer to the gtp of the local ident, which acts as commanding member */
  const GetyPos_c* pc_gtp;
};

}
#endif
