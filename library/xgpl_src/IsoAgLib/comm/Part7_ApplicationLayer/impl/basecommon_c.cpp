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

#include <IsoAgLib/driver/can/impl/cancustomer_c.h>
#include <IsoAgLib/driver/can/impl/canpkg_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement//impl/isomonitor_c.h>
#include "basecommon_c.h"
#include <IsoAgLib/driver/can/impl/canio_c.h>

#if defined(DEBUG)
  #ifdef SYSTEM_PC
    #include <iostream>
  #else
    #include <supplementary_driver/driver/rs232/impl/rs232io_c.h>
  #endif
  #include <IsoAgLib/util/impl/util_funcs.h>
#endif


// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

/** initialize directly after the singleton instance is created.
    this is called from singleton.h and should NOT be called from the user again.
    users please use init(...) instead.
  */
void BaseCommon_c::singletonInitBase(SINGLETON_VEC_KEY_PARAMETER_DEF)
{
  setAlreadyClosed();
  init_base (NULL, 0);
  // assign singletonVecKey to mc_data - if PRT_INSTANCE_CNT > 1
  SINGLETON_MC_DATA_ASSIGN
}

/**
  virtual function which delivers a pointer to the CANCustomer
  specific CanPkgExt_c instance
*/
CanPkgExt_c& BaseCommon_c::dataBase()
{
  return mc_data;
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
      * dependant error in CanIo_c problems during insertion of new FilterBox_c entries for IsoAgLibBase
    @param apc_isoName optional pointer to the ISOName variable of the responsible member instance (pointer enables automatic value update if var val is changed)
    @param ai_singletonVecKey singleton vector key in case PRT_INSTANCE_CNT > 1
    @param at_identMode either IsoAgLib::IdentModeImplement or IsoAgLib::IdentModeTractor
  */
void BaseCommon_c::init_base (const IsoName_c* apc_isoName, int ai_singletonVecKey, IsoAgLib::IdentMode_t at_identMode)
{
  getSchedulerInstance4Comm().registerClient( this );
  mc_data.setSingletonKey( ai_singletonVecKey );

  if (checkAlreadyClosed())
  {
    mb_filterCreated = false;
  }

  // set configure values with call for config
  config_base (apc_isoName, at_identMode
                           , 0 // No individual PGN disabling
                           );

  // clear state of b_alreadyClosed, so that close() is called one time
  clearAlreadyClosed();
};

/** config tractor object after init --> store isoName and mode
    @param apc_isoName pointer to the ISOName variable of the responsible member instance (pointer enables automatic value update if var val is changed)
    @param at_identMode either IsoAgLib::IdentModeImplement or IsoAgLib::IdentModeTractor
    @return true -> configuration was successfull
  */
bool BaseCommon_c::config_base (const IsoName_c* apc_isoName, IsoAgLib::IdentMode_t at_identMode, uint16_t aui16_suppressMask)
{
  mui16_suppressMask = aui16_suppressMask;
  if (   at_identMode == IsoAgLib::IdentModeTractor
      && apc_isoName == NULL
     )
  { // the application is in tractor mode but has no valid isoName
    // IMPORTANT: if we are in tractor mode we MUST have a valid isoName otherwise the configuration makes no sense
    #ifdef DEBUG
      INTERNAL_DEBUG_DEVICE << "CONFIG FAILURE. The config function was called with isoName == NULL and\
                                IdentModeTractor. Is is not allowed that the isoName is NULL in combination\
                                with tractor mode."
                            << INTERNAL_DEBUG_DEVICE_ENDL;
    #endif
    #if defined(DEBUG) && defined(SYSTEM_PC)
      abort();
    #endif
    getILibErrInstance().registerError( iLibErr_c::Precondition, iLibErr_c::Base );
    return false;
  }
  // set configure values
  mpc_isoName = apc_isoName; // store the pointer in any case
  setMode(at_identMode);

  // set the timestamps to 0
  mi32_lastMsgReceived = 0;

  //setSelectedDataSourceISOName is only used in tractor mode
  if (at_identMode == IsoAgLib::IdentModeTractor)
  {
    mc_selectedDataSourceISOName = *apc_isoName;
  }
  else
  { //implement mode
    mc_selectedDataSourceISOName.setUnspecified();
  }
  return true;
}

/** check if a received message should be parsed */
bool BaseCommon_c::checkParseReceived(const IsoName_c& arc_currentSender) const
{
  return ( checkMode(IsoAgLib::IdentModeImplement) // I'm not the sender
            && ( // one of the following conditions must be true
                   (mc_selectedDataSourceISOName == arc_currentSender  ) // actual sender equivalent to last
                || (mc_selectedDataSourceISOName.isUnspecified()         ) // last sender has not correctly claimed address member
               )
          )?true:false;
}


/** functions with actions, which must be performed periodically
    -> called periodically by Scheduler_c
    ==> sends base data msg if configured in the needed rates
    possible errors:
      * dependant error in CanIo_c on CAN send problems
    @param ai32_demandedExecEnd optional timestamp, where timeEvent shall return execution to calling function
    @see CanPkg_c::getData
    @see CanPkgExt_c::getData
    @see CanIo_c::operator<<
    @return true -> all planned activities performed in allowed time
  */
bool BaseCommon_c::timeEvent()
{
  checkCreateReceiveFilter();

  // check for different base data types whether the previously
  // sending node stopped sending -> other nodes can now step in
  if (  checkMode(IsoAgLib::IdentModeImplement)
        &&(lastedTimeSinceUpdate() >= TIMEOUT_SENDING_NODE )
        && (mc_selectedDataSourceISOName.isSpecified())    )
  { // the previously sending node didn't send the information for 3 seconds -> give other items a chance
    mc_selectedDataSourceISOName.setUnspecified();
  }

  if ( ( getISOName() != NULL )
    && (!getIsoMonitorInstance4Comm().existLocalIsoMemberISOName(*getISOName(), true)) )
  { // local isoname for sending is registrated, but it is not yet fully claimed
    // --> nothing to do
    return true;
  }

  // check if we are in tractor mode and have a pointer to the sending isoname
  if ( checkMode(IsoAgLib::IdentModeTractor) )
  { // all tractor mode timeEvent() functions have the only target to send messages,
    // now:
    // 1) getISOName() != NULL
    // 2) getSystemMgmtInstance4Comm().existLocalMemberISOName(*getISOName(), true) indicates, that a corresponding
    //    item has already performed its address claim
    // ==> we can directly call sending time event in this case
    return timeEventTracMode();
  }
  else
  { // we are in implement mode
    // call this function also if isoName == NULL, because some functions do settings which are independent from isoName
    return timeEventImplMode();
  }
}


/** send a ISO11783 base information PGN.
  * this is only called when sending ident is configured and it has already claimed an address
  */
bool BaseCommon_c::timeEventTracMode()
{ return true;}


/** send a ISO11783 base information PGN.
  * this is only called when sending ident is configured and it has already claimed an address
  */
bool BaseCommon_c::timeEventImplMode()
{ return true;}


/** send a PGN request */
bool BaseCommon_c::sendPgnRequest(uint32_t ui32_requestedPGN)
{
  data().setIsoPri(6);
  // set PGN first, as this might overwrite the PS field
  data().setIsoPgn(REQUEST_PGN_MSG_PGN);

  /// if the IsoItem_c is not in the monitor list, ignore this request
  if ( getIsoMonitorInstance4Comm().existActiveLocalIsoMember() )
  { // use the SA of the already active node
    data().setMonitorItemForSA( &getIsoMonitorInstance4Comm().getActiveLocalIsoMember() );
  }
  else
  { // there exists no local ident which is in claimed state -> we are not allowed to send on ISOBUS
    return false;
  }

  // now check and retrieve the target
  if ( ( getSelectedDataSourceISONameConst().isSpecified() )
    && ( getIsoMonitorInstance4Comm().existIsoMemberISOName( getSelectedDataSourceISONameConst(), true ) ) )
  { // we have a valid tractor data source, that can be asked directly
    data().setISONameForDA( getSelectedDataSourceISONameConst() );
  }
  else
  { // there is no selected tractor registered --> ask to global
    data().setIsoPs(255); // global request
  }

  // built request data string
  data().setUint32Data( 0 , ui32_requestedPGN );
  data().setLen( 3 );
  // now CanPkgExt_c has right data -> send
  getCanInstance4Comm() << data();
  return true;
}


/** check if preconditions for request for pgn are fullfilled
    @return  true -> the request for pgn can be send
  */
bool BaseCommon_c::check4ReqForPgn(uint32_t /* aui32_pgn */, IsoItem_c* /*apc_isoItemSender*/, IsoItem_c* apc_isoItemReceiver)
{
  if ( NULL == getISOName() ) return false; // not configured for Send
  if ( ! getIsoMonitorInstance4Comm().existIsoMemberISOName( *getISOName(), true ) ) return false;

  // now we can be sure, that we are in tractor mode, and the registered tractor isoname
  // belongs to an already claimed IsoItem_c --> we are allowed to send
  if ( ( apc_isoItemReceiver == NULL ) ||
  ( &getIsoMonitorInstance4Comm().isoMemberISOName( *getISOName() ) == apc_isoItemReceiver ) )
  { // the REQUEST was directed to GLOBAL or to the SA that belongs to the
    // tractor IdentItem_c that is matched by the registrated IsoName_c (getISOName())
    return true;
  }
  else
  { // we are not allowed to send the request
    return false;
  }
}

}// end namespace __IsoAgLib
