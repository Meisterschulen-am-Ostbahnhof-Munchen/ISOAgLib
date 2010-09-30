/*
  basecommon_c.h:  base class for basa data types

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include <IsoAgLib/driver/can/impl/cancustomer_c.h>
#include <IsoAgLib/driver/can/impl/canpkg_c.h>
#include <IsoAgLib/comm/impl/isobus_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isomonitor_c.h>
#include "basecommon_c.h"


// Begin Namespace __IsoAgLib
namespace __IsoAgLib {


/// NOTE: Currently all BaseCommon_c-based singletons are NOT working
///       with Multiple ISOBUS Instances
///       because getSingletonVecKey is needed here but declared/defined
///       in the derived classes, which are the singletons.


void
BaseCommon_c::init()
{
  if (checkAlreadyClosed())
  {
    clearAlreadyClosed();
    SINGLETON_MC_DATA_ASSIGN

    getSchedulerInstance().registerClient(&mt_task);

    // set configure values with call for config
    config_base (NULL, IsoAgLib::IdentModeImplement, 0 /* No individual PGN disabling */);

    // now let concrete specialized classes init their part...
    init_specialized();
  }
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
  if ( ! checkAlreadyClosed() )
  { // avoid another call
    setAlreadyClosed();

    close_specialized();

    getSchedulerInstance().unregisterClient(&mt_task);
  }
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
bool BaseCommon_c::checkParseReceived(const IsoName_c& acrc_currentSender) const
{
  return ( checkMode(IsoAgLib::IdentModeImplement) // I'm not the sender
            && ( // one of the following conditions must be true
                   (mc_selectedDataSourceISOName == acrc_currentSender  ) // actual sender equivalent to last
                || (mc_selectedDataSourceISOName.isUnspecified()         ) // last sender has not correctly claimed address member
               )
          );
}


/** functions with actions, which must be performed periodically
    -> called periodically by Scheduler_c
    ==> sends base data msg if configured in the needed rates
    possible errors:
      * dependant error in CanIo_c on CAN send problems
    <!--@param ai32_demandedExecEnd optional timestamp, where timeEvent shall return execution to calling function-->
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
        &&(lastedTimeSinceUpdate() >= getTimeOut( ) )
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
  getIsoBusInstance4Comm() << data();
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
