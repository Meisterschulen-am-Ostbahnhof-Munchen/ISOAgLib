/***************************************************************************
                          simplemanagesetpointremote_c.cpp - managing of remote
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
#include "simplemanagesetpointremote_c.h"
#include "../../../impl/process_c.h"
#include "../../../processdatachangehandler_c.h"

namespace __IsoAgLib {

// ***************************************************************************************
// *************************** SimpleManageSetpointRemote_c ***************************
// ***************************************************************************************


/**
  initialise this SimpleManageSetpointRemote_c instance to a well defined initial state
  @param apc_processData optional pointer to containing ProcessData instance
*/
void SimpleManageSetpointRemote_c::init( ProcDataBase_c *const apc_processData )
{
  ProcessElementBase_c::set( apc_processData );
    mi32_setpointMasterVal = 0;
}
/** copy constructor */
SimpleManageSetpointRemote_c::SimpleManageSetpointRemote_c( const SimpleManageSetpointRemote_c& arcc_src )
: ProcessElementBase_c( arcc_src )
{
  assignFromSource( arcc_src );
}
/** assignment operator */
const SimpleManageSetpointRemote_c& SimpleManageSetpointRemote_c::operator=( const SimpleManageSetpointRemote_c& arcc_src )
{
  ProcessElementBase_c::operator=( arcc_src );
  assignFromSource( arcc_src );
  return *this;
}

/** base function for assignment of element vars for copy constructor and operator= */
void SimpleManageSetpointRemote_c::assignFromSource( const SimpleManageSetpointRemote_c& arcc_src )
{
  mi32_setpointMasterVal = arcc_src.mi32_setpointMasterVal;
}

/** processing of a setpoint message */
void SimpleManageSetpointRemote_c::processSetpoint()
{
  // for simple setpoint the message is process here
  ProcessPkg_c& c_pkg = getProcessInstance4Comm().data();
  // pd = 0
  if (!c_pkg.mc_processCmd.checkIsRequest())
  {
    bool b_change = false;
    switch (c_pkg.mc_processCmd.getValueGroup())
    {
      case ProcessCmd_c::exactValue: // exact setpoint
        #ifdef USE_FLOAT_DATA_TYPE
        if (c_pkg.valType() == float_val)
        {
          if ( f_setpointMasterVal != c_pkg.dataFloat() ) {
            f_setpointMasterVal = c_pkg.dataFloat();
            b_change = true;
          }
          // check for Fendt Reset Cmd
          setValType(float_val);
        }
        else
        #endif
        {
          if ( mi32_setpointMasterVal != c_pkg.dataLong() ) {
            mi32_setpointMasterVal = c_pkg.dataLong();
            b_change = true;
          }
          // check for Fendt Reset Cmd
          setValType(i32_val);
        }
        break;
      default:
        // only exactValues allowed in SimpleSetpoint
        break;
    }
    // call handler function if handler class is registered
    if ( processDataConst().getProcessDataChangeHandler() != NULL )
      processDataConst().getProcessDataChangeHandler()->processSetpointResponse( pprocessData(), processData().pkgDataLong(), c_pkg.memberSend().isoName().toConstIisoName_c() );
  }
}




/**
  deliver the actual master setpoint
  @param ab_sendRequest true -> send request for actual value
  @return setpoint value as long
*/
int32_t SimpleManageSetpointRemote_c::setpointMasterVal(bool ab_sendRequest)
{
  setValType(i32_val);
  ProcDataRemoteBase_c& c_base = static_cast<ProcDataRemoteBase_c&>(processData());
  if (ab_sendRequest) {
    // prepare general command in process pkg
    getProcessInstance4Comm().data().mc_processCmd.setValues(true /* isSetpoint */, true /* isRequest */,
                                                             ProcessCmd_c::exactValue,
                                                             ProcessCmd_c::requestValue);

    c_base.sendValISOName(c_base.commanderISOName(), int32_t(0));
  }
  return mi32_setpointMasterVal;
}

/**
  send a setpoint cmd with given exact setpoint
  @param ai32_val commanded setpoint value as long
  @param ab_onlyStoreOnResponse true -> the given value is only stored if response arrives
*/
void SimpleManageSetpointRemote_c::setSetpointMasterVal(int32_t ai32_val, bool ab_onlyStoreOnResponse)
{
  ProcDataRemoteBase_c& c_base = static_cast<ProcDataRemoteBase_c&>(processData());
  setValType(i32_val);

  // prepare general command in process pkg
  getProcessInstance4Comm().data().mc_processCmd.setValues(true /* isSetpoint */, false /* isRequest */,
                                                           ProcessCmd_c::exactValue,
                                                           ProcessCmd_c::setValue);

  c_base.sendValISOName(c_base.commanderISOName(), ai32_val);
  if (!ab_onlyStoreOnResponse) mi32_setpointMasterVal = ai32_val;
}
#ifdef USE_FLOAT_DATA_TYPE
/**
  deliver the actual master setpoint
  @param ab_sendRequest true -> send request for actual value
  @return setpoint value as float
*/
float SimpleManageSetpointRemote_c::setpointMasterValFloat(bool ab_sendRequest)
{
  ProcDataRemoteBase_c& c_base = static_cast<ProcDataRemoteBase_c&>(processData());
  setValType(float_val);
  if (ab_sendRequest) {
    // prepare general command in process pkg
    getProcessInstance4Comm().data().mc_processCmd.setValues(true /* isSetpoint */, true /* isRequest */,
                                                             ProcessCmd_c::exactValue,
                                                             ProcessCmd_c::requestValue);

    c_base.sendValISOName(c_base.commanderISOName(), int32_t(0));
  }
  return f_setpointMasterVal;
}
/**
  send a setpoint cmd with given exact setpoint
  @param af_val commanded setpoint value as float
  @param ab_onlyStoreOnResponse true -> the given value is only stored if response arrives
*/
void SimpleManageSetpointRemote_c::setSetpointMasterVal(float af_val, bool ab_onlyStoreOnResponse)
{
  ProcDataRemoteBase_c& c_base = static_cast<ProcDataRemoteBase_c&>(processData());
  setValType(float_val);
  // prepare general command in process pkg
  getProcessInstance4Comm().data().mc_processCmd.setValues(true /* isSetpoint */, false /* isRequest */,
                                                           ProcessCmd_c::exactValue,
                                                           ProcessCmd_c::setValue);

  c_base.sendValISOName(c_base.commanderISOName(), af_val);
  if (!ab_onlyStoreOnResponse) f_setpointMasterVal = af_val;
}
#endif


} // end of namespace __IsoAgLib
