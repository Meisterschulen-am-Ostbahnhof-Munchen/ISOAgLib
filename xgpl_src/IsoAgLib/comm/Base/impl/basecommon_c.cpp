/***************************************************************************
                          basecommon_c.h  -  base class for basa data types
                             -------------------
    begin                : Fri Apr 07 2000
    copyright            : (C) 2000 - 2004 by Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
    type                 : Header
    $LastChangedDate: 2005-01-11 10:55:57 +0100 (Di, 11 Jan 2005) $
    $LastChangedRevision: 805 $
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

#include <IsoAgLib/util/impl/cancustomer_c.h>
#include <IsoAgLib/util/impl/canpkg_c.h>
#ifdef USE_DIN_9684
  #include <IsoAgLib/comm/SystemMgmt/DIN9684/impl/dinmonitor_c.h>
#endif
#ifdef USE_ISO_11783
  #include <IsoAgLib/comm/SystemMgmt/ISO11783/impl/isomonitor_c.h>
#endif
#include "basecommon_c.h"
#include <IsoAgLib/comm/SystemMgmt/impl/systemmgmt_c.h>
#include <IsoAgLib/driver/can/impl/canio_c.h>

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

/** initialize directly after the singleton instance is created.
    this is called from singleton.h and should NOT be called from the user again.
    users please use init(...) instead.
  */
void BaseCommon_c::singletonInit()
{
  setAlreadyClosed();
  init(NULL);
}

/**
  virtual function which delivers a pointer to the CANCustomer
  specific CANPkgExt_c instance
*/
CANPkgExt_c& BaseCommon_c::dataBase()
{
  return c_data;
}

/** every subsystem of IsoAgLib has explicit function for controlled shutdown */
void BaseCommon_c::close( )
{
  if ( ! checkAlreadyClosed() ) {
    // avoid another call
    setAlreadyClosed();
    // unregister from timeEvent() call by Scheduler_c
    getSchedulerInstance4Comm().unregisterClient( this );
  }
};

/** initialise element which can't be done during construct;
    above all create the needed FilterBox_c instances
    possible errors:
      * dependant error in CANIO_c problems during insertion of new FilterBox_c entries for IsoAgLibBase
    @param rpc_devKey optional pointer to the DEV_KEY variable of the responsible member instance (pointer enables automatic value update if var val is changed)
    @param rt_identMode either IsoAgLib::IdentModeImplement or IsoAgLib::IdentModeTractor
  */
void BaseCommon_c::init(const DevKey_c* rpc_devKey, IsoAgLib::IdentMode_t rt_identMode)
{
  // first register in Scheduler_c
  getSchedulerInstance4Comm().registerClient( this );
  c_data.setSingletonKey( c_data.getSingletonVecKey() );

  #ifdef USE_DIN_9684
  if (checkAlreadyClosed())
  {
    b_dinFilterCreated = false;
  }
  #endif

  #ifdef USE_ISO_11783
  if (checkAlreadyClosed())
  {
    b_isoFilterCreated = false;
  }
  #endif

  // set configure values with call for config
  config(rpc_devKey, rt_identMode);

  // clear state of b_alreadyClosed, so that close() is called one time
  clearAlreadyClosed();
};

/** config tractor object after init --> store devKey and mode
    @param rpc_devKey pointer to the DEV_KEY variable of the responsible member instance (pointer enables automatic value update if var val is changed)
    @param rt_identMode either IsoAgLib::IdentModeImplement or IsoAgLib::IdentModeTractor
    @return true -> configuration was successfull
  */
bool BaseCommon_c::config(const DevKey_c* rpc_devKey, IsoAgLib::IdentMode_t rt_identMode)
{
  if (   rt_identMode == IsoAgLib::IdentModeTractor
      && rpc_devKey == NULL
     )
  { // the application is in tractor mode but has no valid devKey
    // IMPORTANT: if we are in tractor mode we MUST have a valid devKey otherwise the configuration makes no sense
    #ifdef DEBUG
      EXTERNAL_DEBUG_DEVICE << "CONFIG FAILURE. The config function was called with devKey == NULL and\
                                IdentModeTractor. Is is not allowed that the devKey ist NULL in combination\
                                with tractor mode." << "\n";
    #endif
    #if defined(DEBUG) && defined(SYSTEM_PC)
      abort();
    #endif
    getLbsErrInstance().registerError( LibErr_c::Precondition, LibErr_c::LbsBase );
    return false;
  }
  // set configure values
  pc_devKey = rpc_devKey; // store the pointer in any case
  setMode(rt_identMode);

  // set the timestamps to 0
  i32_lastMsgReceived = 0;

  //setSelectedDataSourceDevKey is only used in tractor mode
  if (rt_identMode == IsoAgLib::IdentModeTractor)
  {
    c_selectedDataSourceDevKey = *rpc_devKey;
  }
  else
  { //implement mode
    c_selectedDataSourceDevKey.setUnspecified();
  }
  return true;
}

/** check if a received message should be parsed */
bool BaseCommon_c::checkParseReceived(const DevKey_c& rrefc_currentSender) const
{
  return ( checkMode(IsoAgLib::IdentModeImplement) // I'm not the sender
            && ( // one of the following conditions must be true
                   (c_selectedDataSourceDevKey == rrefc_currentSender  ) // actual sender equivalent to last
                || (c_selectedDataSourceDevKey.isUnspecified()         ) // last sender has not correctly claimed address member
               )
          )?true:false;
}

/** process received moving msg and store updated value for later reading access;
    called by FilterBox_c::processMsg after receiving a msg
    possible errors:
      * LibErr_c::LbsBaseSenderConflict moving msg recevied from different member than before
    @see FilterBox_c::processMsg
    @see CANIO_c::processMsg
    @return true -> message was processed; else the received CAN message will be served to other matching CANCustomer_c
  */
bool BaseCommon_c::processMsg()
{
  #if defined(USE_ISO_11783) && defined(USE_DIN_9684)
  if (c_data.identType() == Ident_c::ExtendedIdent)
  #endif // USE_DIN_9684 && USE_ISO_11783
  #ifdef USE_ISO_11783
  { // an ISO11783 base information msg received
    return isoProcessMsg();
  }
  #endif // USE_ISO_11783
  #if defined(USE_ISO_11783) && defined(USE_DIN_9684)
  else
  #endif
  #ifdef USE_DIN_9684
  return dinProcessMsg();
  #endif
  return false;
}

  /** functions with actions, which must be performed periodically
    -> called periodically by Scheduler_c
    ==> sends base data msg if configured in the needed rates
    possible errors:
      * dependant error in CANIO_c on CAN send problems
    @see CANPkg_c::getData
    @see CANPkgExt_c::getData
    @see CANIO_c::operator<<
    @return true -> all planned activities performed in allowed time
  */
bool BaseCommon_c::timeEvent()
{
  if (Scheduler_c::getAvailableExecTime() == 0) return false;

  checkCreateReceiveFilter();
  if (Scheduler_c::getAvailableExecTime() == 0) return false;

  // check for different base data types whether the previously
  // sending node stopped sending -> other nodes can now step in
  if (  checkMode(IsoAgLib::IdentModeImplement)
        &&(lastedTimeSinceUpdate() >= TIMEOUT_SENDING_NODE )
        && (c_selectedDataSourceDevKey.isSpecified())    )
  { // the previously sending node didn't send the information for 3 seconds -> give other items a chance
    c_selectedDataSourceDevKey.setUnspecified();
  }

  if ( ( getDevKey() != NULL )
    && (!getSystemMgmtInstance4Comm().existLocalMemberDevKey(*getDevKey(), true)) )
  { // local dev key for sending is registrated, but it is not yet fully claimed
    // --> nothing to do
    return true;
  }

  // check if we are in tractor mode and have a pointer to the sending device key
  if ( checkMode(IsoAgLib::IdentModeTractor) )
  { // all tractor mode timeEvent() functions have the only target to send messages,
    // now:
    // 1) getDevKey() != NULL
    // 2) getSystemMgmtInstance4Comm().existLocalMemberDevKey(*getDevKey(), true) indicates, that a corresponding
    //    item has already performed its address claim
    // ==> we can directly call sending time event in this case
    bool b_result = true;
    #ifdef USE_DIN_9684
    if ( !dinTimeEventTracMode()) b_result = false;
    #endif
    #ifdef USE_ISO_11783
    if ( !isoTimeEventTracMode()) b_result = false;
    #endif
    return b_result;
  }
  #ifdef USE_ISO_11783
  else
  { // we are in implement mode
    // call this function also if devKey == NULL, because some functions do settings which are independent from devKey
    if ( !isoTimeEventImplMode()) return false;
  }
  #endif
  return true;
}

#ifdef USE_ISO_11783
/** process a ISO11783 base information PGN */
bool BaseCommon_c::isoProcessMsg()
{ return false;}
/** send a ISO11783 base information PGN.
  * this is only called when sending ident is configured and it has already claimed an address
  */
bool BaseCommon_c::isoTimeEventTracMode()
{ return true;}
/** send a ISO11783 base information PGN.
  * this is only called when sending ident is configured and it has already claimed an address
  */
bool BaseCommon_c::isoTimeEventImplMode()
{ return true;}

/** send a PGN request */
bool BaseCommon_c::sendPgnRequest(uint32_t ui32_requestedPGN)
{
  data().setIsoPri(6);
  // set PGN first, as this might overwrite the PS field
  data().setIsoPgn(REQUEST_PGN_MSG_PGN);

  /// if the ISOItem_c is not in the monitor list, ignore this request
  if ( getSystemMgmtInstance4Comm().existActiveLocalMember() )
  { // use the SA of the already active node
    data().setMonitorItemForSA( &getSystemMgmtInstance4Comm().getActiveLocalMember() );
  }
  else
  { // there exists no local ident which is in claimed state -> we are not allowed to send on ISOBUS
    return false;
  }

  // now check and retrieve the target
  if ( ( getSelectedDataSourceDevKeyConst().isSpecified() )
    && ( getIsoMonitorInstance4Comm().existIsoMemberDevKey( getSelectedDataSourceDevKeyConst(), true ) ) )
  { // we have a valid tractor data source, that can be asked directly
    data().setDevKeyForDA( getSelectedDataSourceDevKeyConst() );
  }
  else
  { // there is no selected tractor registered --> ask to global
    data().setIsoPs(255); // global request
  }

  // built request data string
  data().setUint32Data( 0 , ui32_requestedPGN );
  data().setLen( 3 );
  // now CANPkgExt_c has right data -> send
  getCanInstance4Comm() << data();
  return true;
}
/** check if preconditions for request for pgn are fullfilled
    @return  true -> the request for pgn can be send
  */
bool BaseCommon_c::check4ReqForPgn(uint32_t rui32_pgn, uint8_t /*rui8_sa*/, uint8_t rui8_da)
{
  if ( NULL == getDevKey() ) return false;
  if ( ! getIsoMonitorInstance4Comm().existIsoMemberDevKey( *getDevKey(), true ) ) return false;

  // now we can be sure, that we are in tractor mode, and the registered tractor device key
  // belongs to an already claimed IsoItem_c --> we are allowed to send
  if ( ( getIsoMonitorInstance4Comm().isoMemberDevKey( *getDevKey() ).nr() == rui8_da ) || ( rui8_da == 0xFF ) )
  { // the REQUEST was directed to the SA that belongs to the tractor IdentItem_c that is matched by the registrated
    // DevKey_c (getDevKey())
    return true;
  }
  else
  { // we are not allowed to send the request
    return false;
  }
}
#endif

#ifdef USE_DIN_9684
/** process a DIN9684 base information PGN */
bool BaseCommon_c::dinProcessMsg() {return false;}
/** send a DIN9684 base information PGN
  * this is only called when sending ident is configured and it has already claimed an address
  */
bool BaseCommon_c::dinTimeEventTracMode() {return true;}
#endif

}// end namespace __IsoAgLib
