/***************************************************************************
                          procdatabase_c.h  - base class for local
                                              or remote process data
                                              object
                             -------------------
    begin                : Fri Apr 07 2000
    copyright            : (C) 2000 - 2009 by Dipl.-Inform. Achim Spangler
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
 * Copyright (C) 2000 - 2009 Dipl.-Inform. Achim Spangler                  *
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
#ifndef PROCDATA_BASE_H
#define PROCDATA_BASE_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include <IsoAgLib/typedef.h>
#include <IsoAgLib/util/config.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/istate_c.h>
#include "procident_c.h"
#include "processpkg_c.h"
#include "processcmd_c.h"

namespace IsoAgLib {
  class ProcessDataChangeHandler_c;
  class EventSource_c;
}

// Begin Namespace IsoAgLib
namespace __IsoAgLib {
/** base class for local or remote process data object;
  manages identity of a process data object -> comparing identity;
  has standard functions in mostly abstract way for derived classes
  @author Dipl.-Inform. Achim Spangler
*/
class ProcDataBase_c : public ProcIdent_c {

protected:
public:
  /** allow explicit SetpointBase_c the access to private elements */
  friend class SetpointBase_c;

  /** allow explicit MeasureProgBase_c the access to private elements */
  friend class MeasureProgBase_c;

  /** allow explicit MeasureProgRemote_c the access to private elements */
  friend class MeasureProgRemote_c;

  /** allow explicit MeasureProgLocal_c the access to private elements */
  friend class MeasureProgLocal_c;

  friend class SimpleManageSetpointRemote_c;
  friend class SimpleManageMeasureProgRemote_c;
  friend class SimpleManageSetpointLocal_c;
  friend class SetpointLocal_c;
  friend class SetpointRemote_c;

  /** constructor which can set all element vars
    ISO parameters:
    @param aps_elementDDI optional pointer to array of structure IsoAgLib::ElementDdi_s which contains DDI, element, isSetpoint and ValueGroup
                         (array is terminated by ElementDdi_s.mui16_element == 0xFFFF)
    @param aui16_element device element number
    common parameters:
    @param acrc_isoName optional ISOName code of Process-Data
    @param apc_externalOverridingIsoName pointer to updated ISOName variable
    @param apc_processDataChangeHandler optional pointer to handler class of application
    @param ai_singletonVecKey optional key for selection of IsoAgLib instance (default 0)
  */
  ProcDataBase_c( const IsoAgLib::ElementDdi_s* aps_elementDDI = NULL, uint16_t aui16_element = 0xFFFF,
                 const IsoName_c& acrc_isoName = IsoName_c::IsoNameInitialProcessData(),
                 const IsoName_c *apc_externalOverridingIsoName = NULL,
                 IsoAgLib::ProcessDataChangeHandler_c *apc_processDataChangeHandler = NULL,
                 int ai_singletonVecKey = 0)

    : ProcIdent_c( aps_elementDDI, aui16_element, acrc_isoName, apc_externalOverridingIsoName, ai_singletonVecKey)
    {
      init( aps_elementDDI, aui16_element, acrc_isoName, apc_externalOverridingIsoName,
            apc_processDataChangeHandler, ai_singletonVecKey );
    }


  /** initialise this ProcDataBase_c instance to a well defined initial state
    ISO parameters:
    @param ps_elementDDI optional pointer to array of structure IsoAgLib::ElementDdi_s which contains DDI, element, isSetpoint and ValueGroup
                         (array is terminated by ElementDdi_s.mui16_element == 0xFFFF)
    @param aui16_element  device element number
    common parameters:
    @param acrc_isoName optional ISOName code of Process-Data
    @param apc_externalOverridingIsoName pointer to updated ISOName variable
    @param apc_processDataChangeHandler optional pointer to handler class of application
    @param ai_singletonVecKey optional key for selection of IsoAgLib instance (default 0)
    */
  void init( const IsoAgLib::ElementDdi_s* ps_elementDDI, uint16_t aui16_element,
            const IsoName_c& acrc_isoName = IsoName_c::IsoNameInitialProcessData(),
            const IsoName_c *apc_externalOverridingIsoName = NULL,
            IsoAgLib::ProcessDataChangeHandler_c *apc_processDataChangeHandler = NULL,
            int ai_singletonVecKey = 0);

  /** assignment operator for this base object
    @param acrc_src source instance
    @return reference to source instance for cmd like "prog1 = prog2 = prog3;"
  */
  const ProcDataBase_c& operator=(const ProcDataBase_c& acrc_src);

  /** copy constructor for ProcDataBase_c
    @param acrc_src source instance
  */
  ProcDataBase_c(const ProcDataBase_c& acrc_src);

  /** default destructor which has nothing to do */
  virtual ~ProcDataBase_c();

  //! Retrieve value from ProcessPkg
  int32_t getPkgValue() { return getProcessPkg().getValue(); }

  /** set the pointer to the handler class
    * @param apc_processDataChangeHandler pointer to handler class of application
    */
  void setProcessDataChangeHandler( IsoAgLib::ProcessDataChangeHandler_c *apc_processDataChangeHandler )
   { mpc_processDataChangeHandler = apc_processDataChangeHandler; }

  /** deliver the poitner to the handler class
    * @return pointer to handler class of application (or NULL if not defined by application)
    */
  IsoAgLib::ProcessDataChangeHandler_c* getProcessDataChangeHandler( void ) const { return mpc_processDataChangeHandler; }

  /** process a message, which is adressed for this process data item;
    ProcDataBase_c::processMsg() is responsible to delegate the
    processing of setpoint and measurement messages to the appripriate
    functions processSetpoint and processProg;
    both functions are virtual, so that depending on loacl or remote
    process data the suitable reaction can be implemented
  */
  void processMsg();

  /** perform periodic acoins
    @param pui16_nextTimePeriod calculated new time period, based on current measure progs (only for local proc data)
    @return true -> all planned executions performed
  */
  virtual bool timeEvent( uint16_t *pui16_nextTimePeriod = NULL );

  virtual const IsoName_c& commanderISOName() const { return IsoName_c::IsoNameUnspecified(); }


  /** deliver DDI from last received can pkg
    @return DDI
  */
  uint16_t getDDIfromCANPkg( void ) const;

protected: // Protected methods
  /** send the given int32_t value with variable ISOName ac_varISOName;
    set the int32_t value with conversion (according to central data type) in message
    string and set data format flags corresponding to central data type of this process data
    (local: receiver; remote: sender)
    (other paramter fixed by ident of process data)
    set general command before sendValISOName !
    possible errors:
        * Err_c::elNonexistent one of resolved EMPF/SEND isn't registered with claimed address in Monitor
        * dependant error in CanIo_c on CAN send problems

    @param ac_varISOName variable ISOName
    @param ai32_val int32_t value to send
    @return true -> sendIntern set successful EMPF and SEND
  */
  virtual bool sendValISOName(const IsoName_c& ac_varISOName, int32_t ai32_val = 0) const;

  void setBasicSendFlags() const;

  /** helper function to get reference to process data pkg very quick */
  ProcessPkg_c& getProcessPkg( void ) const;

private: // Private methods
  /** base function for assignment of element vars for copy constructor and operator= */
  void assignFromSource( const ProcDataBase_c& acrc_src );

  /** process a measure prog message
    -> fully dependent on children type local/remote
  */
  virtual void processProg();

  /** virtual base for processing of a setpoint message */
  virtual void processSetpoint();

private: // Private attributes

  friend class IsoAgLib::EventSource_c;

  /** pointer to applications handler class, with handler functions
      which shall be called on correltating change events.
      (e.g. new received setpoint for local process data
       or new received measurement value for remote process data)
    */
  IsoAgLib::ProcessDataChangeHandler_c* mpc_processDataChangeHandler;
};

}
#endif
