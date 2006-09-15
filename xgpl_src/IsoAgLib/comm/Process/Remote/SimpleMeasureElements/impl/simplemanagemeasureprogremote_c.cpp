/***************************************************************************
                          simplemanagemeasureprogremote_c.cpp - managing of remote
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
#include "simplemanagemeasureprogremote_c.h"
#include "../../../impl/process_c.h"
#include "../../../processdatachangehandler_c.h"

namespace __IsoAgLib {

// ***************************************************************************************
// ************************* SimpleManageMeasureProgRemote_c **************************
// ***************************************************************************************

/**
  initialise this SimpleManageMeasureProgRemote_c instance to a well defined initial state
  @param rpc_processData optional pointer to containing ProcessData instance
*/
void SimpleManageMeasureProgRemote_c::init( ProcDataBase_c *const rpc_processData )
{
  ProcessElementBase_c::set( rpc_processData );
  i32_masterVal = 0;
}
/** copy constructor */
SimpleManageMeasureProgRemote_c::SimpleManageMeasureProgRemote_c( const SimpleManageMeasureProgRemote_c& rrefc_src )
: ProcessElementBase_c( rrefc_src ),
  i32_masterVal( rrefc_src.i32_masterVal )
{
}
/** assignment operator */
const SimpleManageMeasureProgRemote_c& SimpleManageMeasureProgRemote_c::operator=( const SimpleManageMeasureProgRemote_c& rrefc_src )
{
  ProcessElementBase_c::operator=( rrefc_src );
  i32_masterVal = rrefc_src.i32_masterVal;
  return *this;
}

/**
  deliver actual measurement value as long
  @param rb_sendRequest true -> request for new value is sent (optional, default false)
*/
int32_t SimpleManageMeasureProgRemote_c::masterMeasurementVal(bool rb_sendRequest)
{
  setValType(i32_val);
  ProcDataRemoteBase_c& c_base = static_cast<ProcDataRemoteBase_c&>(processData());
  if (rb_sendRequest) {
    // prepare general command in process pkg
    getProcessInstance4Comm().data().c_generalCommand.setValues(false /* isSetpoint */, true /* isRequest */,
                                                                GeneralCommand_c::exactValue,
                                                                GeneralCommand_c::requestValue);

    c_base.sendValDevKey(c_base.pri(), c_base.commanderDevKey(), 0);
  }
  return i32_masterVal;
}
/**
  send reset cmd for the measurement value
*/
void SimpleManageMeasureProgRemote_c::resetMasterVal()
{
  ProcDataRemoteBase_c& c_base = static_cast<ProcDataRemoteBase_c&>(processData());
  // prepare general command in process pkg
  getProcessInstance4Comm().data().c_generalCommand.setValues(false /* isSetpoint */, false /* isRequest */,
                                                              GeneralCommand_c::exactValue,
                                                              GeneralCommand_c::setValue);
  c_base.sendValDevKey(c_base.pri(), c_base.commanderDevKey(), 0);

  // prepare general command in process pkg
  getProcessInstance4Comm().data().c_generalCommand.setValues(false /* isSetpoint */, false /* isRequest */,
                                                              GeneralCommand_c::exactValue,
                                                              GeneralCommand_c::measurementReset);
  c_base.sendValDevKey(c_base.pri(), c_base.commanderDevKey(), 0x8);
  #ifdef RESET_MEASUREMENT_WITH_ZERO_EXACT_SETPOINT
    // prepare general command in process pkg
    getProcessInstance4Comm().data().c_generalCommand.setValues(true /* isSetpoint */, false /* isRequest */,
                                                                GeneralCommand_c::exactValue,
                                                                GeneralCommand_c::setValue);
    c_base.sendValDevKey(c_base.pri(), c_base.commanderDevKey(), 0);
  #endif
}

#ifdef USE_FLOAT_DATA_TYPE
/**
  deliver actual measurement value as float
  @param rb_sendRequest true -> request for new value is sent (optional, default false)
*/
float SimpleManageMeasureProgRemote_c::masterValFloat(bool rb_sendRequest)
{
  setValType(float_val);
  ProcDataRemoteBase_c& c_base = static_cast<ProcDataRemoteBase_c&>(processData());
  if (rb_sendRequest) {
    // prepare general command in process pkg
    getProcessInstance4Comm().data().c_generalCommand.setValues(false /* isSetpoint */, true /* isRequest */,
                                                                GeneralCommand_c::exactValue,
                                                                GeneralCommand_c::requestValue);

    c_base.sendValDevKey(c_base.pri(), c_base.commanderDevKey(), 0);
  }
  return f_masterVal;
}
#endif

/** process a measure prog message for remote process data */
void SimpleManageMeasureProgRemote_c::processProg()
{ // for simple measurement the message is process here
  ProcessPkg_c& c_pkg = getProcessInstance4Comm().data();
  bool b_change = false;

  if (c_pkg.c_generalCommand.getCommand() == GeneralCommand_c::setValue)
  {
  #ifdef USE_FLOAT_DATA_TYPE
    if ( ( c_pkg.valType() == float_val)
      && ( f_masterVal != c_pkg.dataFloat() ) ) {
      b_change = true;
      f_masterVal = c_pkg.dataFloat();
    }
    else if ( i32_masterVal != c_pkg.dataLong() ) {
  #else
    if ( i32_masterVal != c_pkg.dataLong() ) {
  #endif
      i32_masterVal = c_pkg.dataLong();
      b_change = true;
    }
  }
  // call handler function if handler class is registered
  if ( processDataConst().getProcessDataChangeHandler() != NULL )
    processDataConst().getProcessDataChangeHandler()->processMeasurementUpdate( pprocessData(), processData().pkgDataLong(), c_pkg.memberSend().devKey(), b_change );
}



} // end of namespace __IsoAgLib
