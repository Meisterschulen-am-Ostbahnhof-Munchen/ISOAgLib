/***************************************************************************
                          lbsProcdataRemoteSimplesetpointSimplemeasureCombined.cc -
                             managing of remote process data object
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
#include "procdataremotesimplesetpointsimplemeasurecombined_c.h"
#include "../../../impl/process_c.h"
#include "../../../processdatachangehandler_c.h"

namespace __IsoAgLib {

// ***************************************************************************************
// ***************** ProcDataRemoteSimpleSetpointSimpleMeasureCombined_c ******************
// ***************************************************************************************

/**
  constructor which can set all element vars
  @param rui8_lis optional LIS code of this instance
  @param rc_gtp optional GETY_POS code of this instance
  @param rui8_wert optional WERT code of this instance
  @param rui8_inst optional INST code of this instance
  @param rui8_zaehlnum optional ZAEHLNUM code of this instance
  @param rui8_pri PRI code of messages with this process data instance (default 2)
  @param rc_ownerGtp optional GETY_POS of the owner
  @param rpc_commanderGtp pointer to updated GETY_POS variable of commander
  @param rpc_processDataChangeHandler optional pointer to handler class of application
  @param ri_singletonVecKey optional key for selection of IsoAgLib instance (default 0)
*/
ProcDataRemoteSimpleSetpointSimpleMeasureCombined_c::ProcDataRemoteSimpleSetpointSimpleMeasureCombined_c(
    uint8_t rui8_lis, GetyPos_c rc_gtp, uint8_t rui8_wert, uint8_t rui8_inst, uint8_t rui8_zaehlnum,
    uint8_t rui8_pri, GetyPos_c rc_ownerGtp, GetyPos_c* rpc_commanderGtp,
    IsoAgLib::ProcessDataChangeHandler_c *rpc_processDataChangeHandler,
    int ri_singletonVecKey)
: ProcDataRemoteBase_c(rui8_lis, rc_gtp, rui8_wert, rui8_inst, rui8_zaehlnum, rui8_pri, rc_ownerGtp, rpc_commanderGtp,
                          rpc_processDataChangeHandler, ri_singletonVecKey)
{
  #ifdef SIMPLE_SETPOINT_WITH_PERCENT
  ui8_setpointPercentVal = 100;
  #endif
  i32_masterVal = 0;
}

/**
  initialise this SimpleManageMeasureProgRemote_c instance to a well defined initial state
  @param rui8_lis optional LIS code of this instance
  @param rc_gtp optional GETY_POS code of this instance
  @param rui8_wert optional WERT code of this instance
  @param rui8_inst optional INST code of this instance
  @param rui8_zaehlnum optional ZAEHLNUM code of this instance
  @param rui8_pri PRI code of messages with this process data instance (default 2)
  @param rc_ownerGtp optional GETY_POS of the owner
  @param rpc_commanderGtp pointer to updated GETY_POS variable of commander
  @param rpc_processDataChangeHandler optional pointer to handler class of application
  @param ri_singletonVecKey optional key for selection of IsoAgLib instance (default 0)
*/
void ProcDataRemoteSimpleSetpointSimpleMeasureCombined_c::init(
    uint8_t rui8_lis, GetyPos_c rc_gtp, uint8_t rui8_wert, uint8_t rui8_inst,
    uint8_t rui8_zaehlnum, uint8_t rui8_pri, GetyPos_c rc_ownerGtp,
    GetyPos_c* rpc_commanderGtp,
    IsoAgLib::ProcessDataChangeHandler_c *rpc_processDataChangeHandler,
    int ri_singletonVecKey)
{
  ProcDataRemoteBase_c::init(rui8_lis, rc_gtp, rui8_wert, rui8_inst, rui8_zaehlnum, rui8_pri, rc_ownerGtp, rpc_commanderGtp,
                                rpc_processDataChangeHandler, ri_singletonVecKey);
  #ifdef SIMPLE_SETPOINT_WITH_PERCENT
  ui8_setpointPercentVal = 100;
  #endif
  i32_masterVal = 0;
}

/**
  assignment operator for this object
  @param rrefc_src source instance
  @return reference to source instance for cmd like "prog1 = prog2 = prog3;"
*/
const ProcDataRemoteSimpleSetpointSimpleMeasureCombined_c&
  ProcDataRemoteSimpleSetpointSimpleMeasureCombined_c::operator=(
    const ProcDataRemoteSimpleSetpointSimpleMeasureCombined_c& rrefc_src)
{
  ProcDataRemoteBase_c::operator=( rrefc_src );
  assignFromSource( rrefc_src );
  return *this;
}

/**
  copy constructor for IsoAgLibProcDataRemote
  @param rrefc_src source instance
*/
ProcDataRemoteSimpleSetpointSimpleMeasureCombined_c::ProcDataRemoteSimpleSetpointSimpleMeasureCombined_c(
  const ProcDataRemoteSimpleSetpointSimpleMeasureCombined_c& rrefc_src)
  : ProcDataRemoteBase_c( rrefc_src )
{
  assignFromSource( rrefc_src );
}
/** base function for assignment of element vars for copy constructor and operator= */
void ProcDataRemoteSimpleSetpointSimpleMeasureCombined_c::assignFromSource(
  const ProcDataRemoteSimpleSetpointSimpleMeasureCombined_c& rrefc_src )
{
  #ifdef SIMPLE_SETPOINT_WITH_PERCENT
  ui8_setpointPercentVal = rrefc_src.ui8_setpointPercentVal;
  #endif
  i32_masterVal = rrefc_src.i32_masterVal;
}

/** default destructor which has nothing to do */
ProcDataRemoteSimpleSetpointSimpleMeasureCombined_c::~ProcDataRemoteSimpleSetpointSimpleMeasureCombined_c()
{}

#ifdef SIMPLE_SETPOINT_WITH_PERCENT
/**
  deliver the actual percent setpoint
  @param rb_sendRequest true -> send request for actual value
  @return percent setpoint value
*/
uint8_t ProcDataRemoteSimpleSetpointSimpleMeasureCombined_c::setpointPercentVal(bool rb_sendRequest)
{
  if (rb_sendRequest) sendDataRawCmdGtp(pri(), commanderGtp(), 2, 1, 0);
  return ui8_setpointPercentVal;
}
/**
  send a setpoint cmd with given percent setpoint
  @param rb_val commanded setpoint percent value
  @param rb_onlyStoreOnResponse true -> the given value is only stored if response arrives
*/
void ProcDataRemoteSimpleSetpointSimpleMeasureCombined_c::setSetpointPercentVal(uint8_t rb_val, bool rb_onlyStoreOnResponse)
{
  sendDataRawCmdGtp(pri(), commanderGtp(), 0, 1, rb_val);
  if (!rb_onlyStoreOnResponse)ui8_setpointPercentVal = rb_val;
}
#endif

/**
  deliver the actual master setpoint
  @param rb_sendRequest true -> send request for actual value
  @return setpoint value as long
*/
int32_t ProcDataRemoteSimpleSetpointSimpleMeasureCombined_c::setpointMasterVal(bool rb_sendRequest)
{
  setValType(i32_val);
  if (rb_sendRequest) sendDataRawCmdGtp(pri(), commanderGtp(), 2, 0, 0);
  return i32_masterVal;
}

/**
  send a setpoint cmd with given exact setpoint
  @param ri32_val commanded setpoint value as long
  @param rb_onlyStoreOnResponse true -> the given value is only stored if response arrives
*/
void ProcDataRemoteSimpleSetpointSimpleMeasureCombined_c::setSetpointMasterVal(int32_t ri32_val, bool rb_onlyStoreOnResponse)
{
  setValType(i32_val);
  sendValGtp(pri(), commanderGtp(), 0, 0, ri32_val);
  if (!rb_onlyStoreOnResponse) i32_masterVal = ri32_val;
}
#ifdef USE_FLOAT_DATA_TYPE
/**
  deliver the actual master setpoint
  @param rb_sendRequest true -> send request for actual value
  @return setpoint value as float
*/
float ProcDataRemoteSimpleSetpointSimpleMeasureCombined_c::setpointMasterValFloat(bool rb_sendRequest)
{
  setValType(float_val);
  if (rb_sendRequest) sendDataRawCmdGtp(pri(), commanderGtp(), 2, 0, 0);
  return f_masterVal;
}
/**
  send a setpoint cmd with given exact setpoint
  @param rf_val commanded setpoint value as float
  @param rb_onlyStoreOnResponse true -> the given value is only stored if response arrives
*/
void ProcDataRemoteSimpleSetpointSimpleMeasureCombined_c::setSetpointMasterVal(float rf_val, bool rb_onlyStoreOnResponse)
{
  setValType(float_val);
  sendValGtp(pri(), commanderGtp(), 0, 0, rf_val);
  if (!rb_onlyStoreOnResponse) f_masterVal = rf_val;
}
#endif

/**
  deliver actual measurement value as long
  @param rb_sendRequest true -> request for new value is sent (optional, default false)
*/
int32_t ProcDataRemoteSimpleSetpointSimpleMeasureCombined_c::masterVal(bool rb_sendRequest)
{
  setValType(i32_val);
  if (rb_sendRequest) sendDataRawCmdGtp(pri(), commanderGtp(), 3, 0, 0);
  return i32_masterVal;
}
/**
  send reset cmd for the measurement value
*/
void ProcDataRemoteSimpleSetpointSimpleMeasureCombined_c::resetMasterVal()
{
  sendDataRawCmdGtp(pri(), commanderGtp(), 1, 0, 0);
  sendDataRawCmdGtp(pri(), commanderGtp(), 0, 6, 0x8);
  #ifdef RESET_MEASUREMENT_WITH_ZERO_EXACT_SETPOINT
  sendDataRawCmdGtp(pri(), commanderGtp(), 0, 0, 0);
  #endif
}

#ifdef USE_FLOAT_DATA_TYPE
/**
  deliver actual measurement value as float
  @param rb_sendRequest true -> request for new value is sent (optional, default false)
*/
float ProcDataRemoteSimpleSetpointSimpleMeasureCombined_c::masterValFloat(bool rb_sendRequest)
{
  setValType(float_val);
  if (rb_sendRequest) sendDataRawCmdGtp(pri(), commanderGtp(), 3, 0, 0);
  return f_masterVal;
}
#endif


/** process a setpoint message */
void ProcDataRemoteSimpleSetpointSimpleMeasureCombined_c::processSetpoint(){
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
          if ( f_masterVal != c_pkg.dataFloat() ) {
            f_masterVal = c_pkg.dataFloat();
            b_change = true;
          }
          setValType(float_val);
        }
        else
        #endif
        {
          if ( i32_masterVal != c_pkg.dataLong() ){
            i32_masterVal = c_pkg.dataLong();
            b_change = true;
          }
          setValType(i32_val);
        }
        break;
      case 1: // percent
        #ifdef SIMPLE_SETPOINT_WITH_PERCENT
        if ( ui8_setpointPercentVal != c_pkg.dataLong() ){
          ui8_setpointPercentVal = c_pkg.dataLong();
          b_change = true;
        }
        #endif
        break;
    }
    // call handler function if handler class is registered
    if ( ( getProcessDataChangeHandler() != NULL ) && ( b_change ) )
      getProcessDataChangeHandler()->processSetpointResponse( this, masterVal(), c_pkg.memberSend().gtp() );
  }
}
/** process a measure prog message for remote process data */
void ProcDataRemoteSimpleSetpointSimpleMeasureCombined_c::processProg(){
  // for simple measurement the message is process here
  ProcessPkg_c& c_pkg = getProcessInstance4Comm().data();
  if ((c_pkg.pd() == 1) && (c_pkg.mod() == 0))
  {
    bool b_change = false;
  #ifdef USE_FLOAT_DATA_TYPE
    if ( (c_pkg.valType() != i32_val)
      && ( f_masterVal != c_pkg.dataFloat() ) ) {
      f_masterVal = c_pkg.dataFloat();
      b_change = true;
    }
    else if ( i32_masterVal != c_pkg.dataLong() )
  #else
    if ( i32_masterVal != c_pkg.dataLong() )
  #endif
    {
      i32_masterVal = c_pkg.dataLong();
      b_change = true;
    }
    // call handler function if handler class is registered
    if ( getProcessDataChangeHandler() != NULL )
      getProcessDataChangeHandler()->processMeasurementUpdate( this, masterVal(), c_pkg.memberSend().gtp(), b_change );
  }
}


} // end of namespace __IsoAgLib
