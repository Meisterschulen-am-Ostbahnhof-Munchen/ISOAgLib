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

/** Common base class for all remote process data independent from the
  individual feature set.
  This class is responsible for deduction of dynamic member adresses ( %i.e. SA for ISO 11783 )
  dependend on device type of managing ECU and this local ECU ( which interacts with the remote ECU )
 */
class ProcDataRemoteBase_c : public ProcDataBase_c
{
 public:
  /** constructor which can set all element vars
    ISO parameter
    @param ps_elementDDI optional pointer to array of structure IsoAgLib::ElementDDI_s which contains DDI, element, isSetpoint and ValueGroup
                         (array is terminated by ElementDDI_s.ui16_element == 0xFFFF)
    common parameter
    @param rc_isoName optional ISOName code of this instance
    @param rc_ownerISOName optional ISOName of the owner
    @param rpc_commanderISOName pointer to updated ISOName variable of commander
    @param rpc_processDataChangeHandler optional pointer to handler class of application
    @param ri_singletonVecKey optional key for selection of IsoAgLib instance (default 0)
  */
  ProcDataRemoteBase_c( const IsoAgLib::ElementDDI_s* ps_elementDDI = NULL, uint16_t rui16_element = 0xFFFF,
                        const ISOName_c& rc_isoName = ISOName_c::ISONameInitialProcessData(),
                        const ISOName_c& rc_ownerISOName = ISOName_c::ISONameUnspecified(),
                        const ISOName_c* rpc_commanderISOName = NULL,
                        IsoAgLib::ProcessDataChangeHandler_c *rpc_processDataChangeHandler = NULL,
                        int ri_singletonVecKey = 0)
  {
    init( ps_elementDDI, rui16_element, rc_isoName, rc_ownerISOName, rpc_commanderISOName,
          rpc_processDataChangeHandler, ri_singletonVecKey);
  }

  /** initialise this ProcDataRemoteBase_c instance to a well defined initial state
      ISO parameter
      @param ps_elementDDI optional pointer to array of structure IsoAgLib::ElementDDI_s which contains DDI, element, isSetpoint and ValueGroup
                          (array is terminated by ElementDDI_s.ui16_element == 0xFFFF)
      common parameter
      @param rc_isoName optional ISOName code of this instance
      @param rc_ownerISOName optional ISOName of the owner
      @param rpc_commanderISOName pointer to updated ISOName variable of commander
      @param rpc_processDataChangeHandler optional pointer to handler class of application
      @param ri_singletonVecKey optional key for selection of IsoAgLib instance (default 0)
    */
  void init(  const IsoAgLib::ElementDDI_s* ps_elementDDI, uint16_t rui16_element,
              const ISOName_c& rc_isoName = ISOName_c::ISONameInitialProcessData(),
              const ISOName_c& rc_ownerISOName = ISOName_c::ISONameUnspecified(),
              const ISOName_c* rpc_commanderISOName = NULL,
              IsoAgLib::ProcessDataChangeHandler_c *rpc_processDataChangeHandler = NULL,
              int ri_singletonVecKey = 0);

  /** assignment operator for this object
      @param rrefc_src source instance
      @return reference to source instance for cmd like "prog1 = prog2 = prog3;"
    */
  const ProcDataRemoteBase_c& operator=(const ProcDataRemoteBase_c& rrefc_src);

  /** copy constructor for IsoAgLibProcDataRemote
      @param rrefc_src source instance
    */
   ProcDataRemoteBase_c(const ProcDataRemoteBase_c& rrefc_src);

  /** default destructor which has nothing to do */
  ~ProcDataRemoteBase_c();

  /** deliver the commanderISOName (ISOName of local member)
    @return ISOName used for sending commands to remote owner member
  */
  virtual const ISOName_c& commanderISOName() const { return (pc_isoName != NULL) ? (*pc_isoName):(ISOName_c::ISONameUnspecified()); }

  /** set the pointer to the commander ident isoName
    @param rpbisoName pointer to ISOName var of local member used for
                sending commands to remote owner member
  */
  virtual void setCommanderISOName(const ISOName_c* rpc_isoName);

  /** perform periodic actions
    ProcDataRemoteBase_c::timeEvent has nothing to do
    -> this function must only be defined as base for derived variants which
    uses simple measurements and thus doesn't need a time event
    @param pui16_nextTimePeriod calculated new time period, based on current measure progs (only for local proc data)
    @return true -> all planned executions performed
  */
  virtual bool timeEvent( uint16_t *pui16_nextTimePeriod = NULL );

    /** send the given int32_t value with variable ISOName rc_varISOName;
  set the int32_t value with conversion (according to central data type) in message
  string and set data format flags corresponding to central data type of this process data
  (other parameter fixed by ident of process data)
    possible errors:
   * Err_c::elNonexistent one of resolved EMPF/SEND isn't registered with claimed address in Monitor
   * dependant error in CANIO_c on CAN send problems

  @param rc_varISOName variable ISOName
  @param ri32_val int32_t value to send
  @param en_valueGroup: min/max/exact/default
  @param en_command
  @return true -> sendIntern set successful EMPF and SEND
     */
  bool sendValISOName(const ISOName_c& rc_varISOName, int32_t ri32_val = 0) const;

#ifdef USE_FLOAT_DATA_TYPE
  /** send the given float value with variable ISOName rc_varISOName;
    set the float value with conversion (according to central data type) in message
    string and set data format flags corresponding to central data type of this process data
    (other parameter fixed by ident of process data)
    possible errors:
 * Err_c::elNonexistent one of resolved EMPF/SEND isn't registered with claimed address in Monitor
 * dependant error in CANIO_c on CAN send problems

      @param rc_varISOName variable ISOName
      @param rb_pd PD code for the msg
      @param rb_mod MOD code for the msg
      @param ri32_val float value to send
      @return true -> sendIntern set successful EMPF and SEND
   */
  bool sendValISOName(const ISOName_c& rc_varISOName, float rf_val = 0.0F) const;
#endif

private: // Private methods
  void setRemoteSendFlags (const ISOName_c& rc_varISOName) const;

private: // Private attributes
  /** pointer to the isoName of the local ident, which acts as commanding member */
  const ISOName_c* pc_isoName;
};

}
#endif
