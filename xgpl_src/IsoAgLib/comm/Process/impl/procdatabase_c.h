/***************************************************************************
                          procdatabase_c.h  - base class for local
                                              or remote process data
                                              object
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
#ifndef PROCDATA_BASE_H
#define PROCDATA_BASE_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include <IsoAgLib/typedef.h>
#include <IsoAgLib/util/config.h>
#include <IsoAgLib/comm/SystemMgmt/impl/istate_c.h>
#include "procident_c.h"
#include "processpkg_c.h"
#include "generalcommand_c.h"

namespace IsoAgLib {
  class  ProcessDataChangeHandler_c;
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
    @param ps_elementDDI optional pointer to array of structure IsoAgLib::ElementDDI_s which contains DDI, element, isSetpoint and ValueGroup
                         (array is terminated by ElementDDI_s.ui16_element == 0xFFFF)
    @param ui16_element  device element number
    common parameters:
    @param rc_isoName optional ISOName code of Process-Data
    @param rc_ownerISOName optional ISOName of the owner
    @param rpc_isoName pointer to updated ISOName variable of owner
    @param rpc_processDataChangeHandler optional pointer to handler class of application
    @param ri_singletonVecKey optional key for selection of IsoAgLib instance (default 0)
  */
  ProcDataBase_c( const IsoAgLib::ElementDDI_s* ps_elementDDI = NULL, uint16_t ui16_element = 0xFFFF,
                 const ISOName_c& rc_isoName = ISOName_c::ISONameInitialProcessData,
                 const ISOName_c& rc_ownerISOName = ISOName_c::ISONameUnspecified, const ISOName_c *rpc_isoName = NULL,
                 IsoAgLib::ProcessDataChangeHandler_c *rpc_processDataChangeHandler = NULL,
                 int ri_singletonVecKey = 0)

    : ProcIdent_c( ps_elementDDI, ui16_element, rc_isoName, rc_ownerISOName, rpc_isoName, ri_singletonVecKey)
    {
      init( ps_elementDDI, ui16_element, rc_isoName, rc_ownerISOName, rpc_isoName,
            rpc_processDataChangeHandler, ri_singletonVecKey );
    }


  /** initialise this ProcDataBase_c instance to a well defined initial state
    ISO parameters:
    @param ps_elementDDI optional pointer to array of structure IsoAgLib::ElementDDI_s which contains DDI, element, isSetpoint and ValueGroup
                         (array is terminated by ElementDDI_s.ui16_element == 0xFFFF)
    @param ui16_element  device element number
    common parameters:
    @param rc_isoName optional ISOName code of Process-Data
    @param rc_ownerISOName optional ISOName of the owner
    @param rpc_isoName pointer to updated ISOName variable of owner
    @param rpc_processDataChangeHandler optional pointer to handler class of application
    @param ri_singletonVecKey optional key for selection of IsoAgLib instance (default 0)
    */
  void init( const IsoAgLib::ElementDDI_s* ps_elementDDI, uint16_t rui16_element,
            const ISOName_c& rc_isoName = ISOName_c::ISONameInitialProcessData,
            const ISOName_c& rc_ownerISOName = ISOName_c::ISONameUnspecified, const ISOName_c *rpc_isoName = NULL,
            IsoAgLib::ProcessDataChangeHandler_c *rpc_processDataChangeHandler = NULL,
            int ri_singletonVecKey = 0);

  /** assignment operator for this base object
    @param rrefc_src source instance
    @return reference to source instance for cmd like "prog1 = prog2 = prog3;"
  */
  const ProcDataBase_c& operator=(const ProcDataBase_c& rrefc_src);

  /** copy constructor for ProcDataBase_c
    @param rrefc_src source instance
  */
  ProcDataBase_c(const ProcDataBase_c& rrefc_src);

  /** default destructor which has nothing to do */
  virtual ~ProcDataBase_c();

  /** set the pointer to the handler class
    * @param rpc_processDataChangeHandler pointer to handler class of application
    */
  void setProcessDataChangeHandler( IsoAgLib::ProcessDataChangeHandler_c *rpc_processDataChangeHandler )
   { pc_processDataChangeHandler = rpc_processDataChangeHandler; }

  /** deliver the poitner to the handler class
    * @return pointer to handler class of application (or NULL if not defined by application)
    */
  IsoAgLib::ProcessDataChangeHandler_c* getProcessDataChangeHandler( void ) const { return pc_processDataChangeHandler; }

  /** deliver the central data type of this process data
    @return proc_valType_t: i32_val, ui32_val, float_val, cmdVal
  */
  const proc_valType_t valType()const{return en_procValType;}

  /** set the central data type of this process data
    @return proc_valType_t: i32_val, ui32_val, float_val, cmdVal
  */
  void setValType(proc_valType_t ren_procValType){en_procValType = ren_procValType;}

  /** deliver the pkg data value as int32_t;
    if pd/mod are one of the to be converted one, return converted;
    the value is read from message in the data type format which is
    defined in the message; on different types, a conversion by assignment
    is performed -> %e.g. if message has float val 4.2 the result of this
    function is 4
    @return data value as int32_t value (converted if needed)
  */
  int32_t pkgDataLong()const;

  /** deliver the pkg data value as uint32_t;
    if pd/mod are one of the to be converted one, return converted;
    the value is read from message in the data type format which is
    defined in the message; on different types, a conversion by assignment
    is performed -> %e.g. if message has float val 4.2 the result of this
    function is 4
    @return data value as uint32_t value (converted if needed)
  */
  uint32_t pkgDataUlong()const;

#ifdef USE_FLOAT_DATA_TYPE
  /** deliver the pkg data value as float;
    if pd/mod are one of the to be converted one, return converted;
    the value is read from message in the data type format which is
    defined in the message; on different types, a conversion by assignment
    is performed -> %e.g. if message has int32_t 4 the result of this function
    is float 4.0
    @return data value as float value (converted if needed)
  */
  float pkgDataFloat()const;
#endif

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

  virtual const ISOName_c& commanderISOName()const{return ISOName_c::ISONameUnspecified;}


  /** deliver DDI from last received can pkg
    @return DDI
  */
  uint16_t getDDIfromCANPkg( void ) const;

protected: // Protected methods
  /** send the given int32_t value with variable ISOName rc_varISOName;
    set the int32_t value with conversion (according to central data type) in message
    string and set data format flags corresponding to central data type of this process data
    (local: receiver; remote: sender)
    (other paramter fixed by ident of process data)
    set general command before sendValISOName !
    possible errors:
        * Err_c::elNonexistent one of resolved EMPF/SEND isn't registered with claimed address in Monitor
        * dependant error in CANIO_c on CAN send problems

    @param rc_varISOName variable ISOName
    @param ri32_val int32_t value to send
    @param en_valueGroup: min/max/exact/default
    @param en_command
    @return true -> sendIntern set successful EMPF and SEND
  */
  virtual bool sendValISOName(const ISOName_c& rc_varISOName, int32_t ri32_val = 0) const;

#ifdef USE_FLOAT_DATA_TYPE
  /** send the given float value with variable ISOName rc_varISOName;
    set the float value with conversion (according to central data type) in message
    string and set data format flags corresponding to central data type of this process data
    (local: receiver; remote: sender)
    (other paramter fixed by ident of process data)
    set general command before sendValISOName !
    possible errors:
        * Err_c::elNonexistent one of resolved EMPF/SEND isn't registered with claimed address in Monitor
        * dependant error in CANIO_c on CAN send problems

    @param rc_varISOName variable ISOName
    @param rb_pd PD code for the msg
    @param rb_mod MOD code for the msg
    @param ri32_val float value to send
    @return true -> sendIntern set successful EMPF and SEND
  */
  virtual bool sendValISOName(const ISOName_c& rc_varISOName, float rf_val = 0.0F) const;
#endif

  void setBasicSendFlags() const;

  /** helper function to get reference to process data pkg very quick */
  ProcessPkg_c& getProcessPkg( void ) const;

private: // Private methods
  /** base function for assignment of element vars for copy constructor and operator= */
  void assignFromSource( const ProcDataBase_c& rrefc_src );

  /** process a measure prog message
    -> fully dependent on children type local/remote
  */
  virtual void processProg();

  /** virtual base for processing of a setpoint message */
  virtual void processSetpoint();

private: // Private attributes

  friend class IsoAgLib::EventSource_c;

  /** central data type to use for messages: i32_val, ui32_val, float_val */
  proc_valType_t en_procValType;

  /** pointer to applications handler class, with handler functions
      which shall be called on correltating change events.
      (e.g. new received setpoint for local process data
       or new received measurement value for remote process data)
    */
  IsoAgLib::ProcessDataChangeHandler_c* pc_processDataChangeHandler;
};

/** deliver the pkg data value as int32_t;
  if pd/mod are one of the to be converted one, return converted;
  the value is read from message in the data type format which is
  defined in the message; on different types, a conversion by assignment
  is performed -> %e.g. if message has float val 4.2 the result of this
  function is 4
  @return data value as int32_t value (converted if needed)
*/
inline int32_t ProcDataBase_c::pkgDataLong()const
{
  return getProcessPkg().dataLong();
}

/** deliver the pkg data value as uint32_t;
  if pd/mod are one of the to be converted one, return converted;
  the value is read from message in the data type format which is
  defined in the message; on different types, a conversion by assignment
  is performed -> %e.g. if message has float val 4.2 the result of this
  function is 4
  @return data value as uint32_t value (converted if needed)
*/
inline uint32_t ProcDataBase_c::pkgDataUlong()const
{
   return getProcessPkg().dataUlong();
}

#ifdef USE_FLOAT_DATA_TYPE
/** deliver the pkg data value as float;
  if pd/mod are one of the to be converted one, return converted;
  the value is read from message in the data type format which is
  defined in the message; on different types, a conversion by assignment
  is performed -> %e.g. if message has int32_t 4 the result of this function
  is float 4.0
  @return data value as float value (converted if needed)
*/
inline float ProcDataBase_c::pkgDataFloat()const
{
  return getProcessPkg().dataFloat();
}
#endif

}
#endif
