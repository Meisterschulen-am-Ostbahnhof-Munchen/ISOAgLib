/***************************************************************************
                          simpleManageSetpointRemote.cc - managing of remote
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
  @param rpc_processData optional pointer to containing ProcessData instance
*/
void SimpleManageSetpointRemote_c::init( ProcDataBase_c *const rpc_processData )
{
  ProcessElementBase_c::set( rpc_processData );
    i32_setpointMasterVal = 0;
  #ifdef SIMPLE_SETPOINT_WITH_PERCENT
    ui8_setpointPercentVal = 100;
  #endif
}
/** copy constructor */
SimpleManageSetpointRemote_c::SimpleManageSetpointRemote_c( const SimpleManageSetpointRemote_c& rrefc_src )
: ProcessElementBase_c( rrefc_src )
{
  assignFromSource( rrefc_src );
}
/** assignment operator */
const SimpleManageSetpointRemote_c& SimpleManageSetpointRemote_c::operator=( const SimpleManageSetpointRemote_c& rrefc_src )
{
  ProcessElementBase_c::operator=( rrefc_src );
  assignFromSource( rrefc_src );
  return *this;
}

/** base function for assignment of element vars for copy constructor and operator= */
void SimpleManageSetpointRemote_c::assignFromSource( const SimpleManageSetpointRemote_c& rrefc_src )
{
  i32_setpointMasterVal = rrefc_src.i32_setpointMasterVal;
  #ifdef SIMPLE_SETPOINT_WITH_PERCENT
  ui8_setpointPercentVal = rrefc_src.ui8_setpointPercentVal;
  #endif
}

/** processing of a setpoint message */
void SimpleManageSetpointRemote_c::processSetpoint()
{
  // for simple setpoint the message is process here
  ProcessPkg_c& c_pkg = getProcessInstance4Comm().data();
  if (c_pkg.pd() == 0)
  {
    bool b_change = false;
    switch (c_pkg.mod())
    {
      case 0: // exact setpoint
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
          if ( i32_setpointMasterVal != c_pkg.dataLong() ) {
            i32_setpointMasterVal = c_pkg.dataLong();
            b_change = true;
          }
          // check for Fendt Reset Cmd
          setValType(i32_val);
        }
        break;
      case 1: // percent
        #ifdef SIMPLE_SETPOINT_WITH_PERCENT
        if ( ui8_setpointPercentVal != c_pkg.dataLong() ) {
          ui8_setpointPercentVal = c_pkg.dataLong();
          b_change = true;
        }
        #endif
        break;
    }
    // call handler function if handler class is registered
    if ( processDataConst().getProcessDataChangeHandler() != NULL )
      processDataConst().getProcessDataChangeHandler()->processSetpointSet( pprocessData(), processData().pkgDataLong(), c_pkg.memberSend().gtp(), b_change );
  }
}



#ifdef SIMPLE_SETPOINT_WITH_PERCENT
/**
  deliver the actual percent setpoint
  @param rb_sendRequest true -> send request for actual value
  @return percent setpoint value
*/
uint8_t SimpleManageSetpointRemote_c::setpointPercentVal(bool rb_sendRequest)
{
  ProcDataRemoteBase_c& c_base = static_cast<ProcDataRemoteBase_c&>(processData());
  if (rb_sendRequest) c_base.sendDataRawCmdGtp(c_base.pri(), c_base.commanderGtp(), 2, 1, 0);
  return ui8_setpointPercentVal;
}
/**
  send a setpoint cmd with given percent setpoint
  @param rb_val commanded setpoint percent value
  @param rb_onlyStoreOnResponse true -> the given value is only stored if response arrives
*/
void SimpleManageSetpointRemote_c::setSetpointPercentVal(uint8_t rb_val, bool rb_onlyStoreOnResponse)
{
  ProcDataRemoteBase_c& c_base = static_cast<ProcDataRemoteBase_c&>(processData());
  c_base.sendDataRawCmdGtp(c_base.pri(), c_base.commanderGtp(), 0, 1, rb_val);
  if (!rb_onlyStoreOnResponse) ui8_setpointPercentVal = rb_val;
}
#endif

/**
  deliver the actual master setpoint
  @param rb_sendRequest true -> send request for actual value
  @return setpoint value as long
*/
int32_t SimpleManageSetpointRemote_c::setpointMasterVal(bool rb_sendRequest)
{
  setValType(i32_val);
  ProcDataRemoteBase_c& c_base = static_cast<ProcDataRemoteBase_c&>(processData());
  if (rb_sendRequest) c_base.sendDataRawCmdGtp(c_base.pri(), c_base.commanderGtp(), 2, 0, 0);
  return i32_setpointMasterVal;
}

/**
  send a setpoint cmd with given exact setpoint
  @param ri32_val commanded setpoint value as long
  @param rb_onlyStoreOnResponse true -> the given value is only stored if response arrives
*/
void SimpleManageSetpointRemote_c::setSetpointMasterVal(int32_t ri32_val, bool rb_onlyStoreOnResponse)
{
  ProcDataRemoteBase_c& c_base = static_cast<ProcDataRemoteBase_c&>(processData());
  setValType(i32_val);
  c_base.sendValGtp(c_base.pri(), c_base.commanderGtp(), 0, 0, ri32_val);
  if (!rb_onlyStoreOnResponse) i32_setpointMasterVal = ri32_val;
}
#ifdef USE_FLOAT_DATA_TYPE
/**
  deliver the actual master setpoint
  @param rb_sendRequest true -> send request for actual value
  @return setpoint value as float
*/
float SimpleManageSetpointRemote_c::setpointMasterValFloat(bool rb_sendRequest)
{
  ProcDataRemoteBase_c& c_base = static_cast<ProcDataRemoteBase_c&>(processData());
  setValType(float_val);
  if (rb_sendRequest) c_base.sendDataRawCmdGtp(c_base.pri(), c_base.commanderGtp(), 2, 0, 0);
  return f_setpointMasterVal;
}
/**
  send a setpoint cmd with given exact setpoint
  @param rf_val commanded setpoint value as float
  @param rb_onlyStoreOnResponse true -> the given value is only stored if response arrives
*/
void SimpleManageSetpointRemote_c::setSetpointMasterVal(float rf_val, bool rb_onlyStoreOnResponse)
{
  ProcDataRemoteBase_c& c_base = static_cast<ProcDataRemoteBase_c&>(processData());
  setValType(float_val);
  c_base.sendValGtp(c_base.pri(), c_base.commanderGtp(), 0, 0, rf_val);
  if (!rb_onlyStoreOnResponse) f_setpointMasterVal = rf_val;
}
#endif


} // end of namespace __IsoAgLib
