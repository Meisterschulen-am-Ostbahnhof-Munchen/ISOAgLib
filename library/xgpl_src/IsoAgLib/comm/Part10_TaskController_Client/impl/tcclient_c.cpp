/*
  process_c.cpp: central managing instance for all process data
    informations in the system

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include "process_c.h"
#include <IsoAgLib/comm/impl/isobus_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isofiltermanager_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isoitem_c.h>
#include <IsoAgLib/util/iassert.h>

// @TODO : check if TCisAlive ? if TC status not sent for 6 seconds, stop sending measurment (call stopMeasurement)
// @TODO : manage TOTALS

namespace __IsoAgLib {

Process_c &getProcessInstance( uint8_t aui8_instance )
{
  MACRO_MULTITON_GET_INSTANCE_BODY(Process_c, PRT_INSTANCE_CNT, aui8_instance);
}

DevPropertyHandler_c& Process_c::getDevPropertyHandlerInstance( void )
{
  return mc_devPropertyHandler;
}

void
Process_c::init()
{
  isoaglib_assert (!initialized());

  getSchedulerInstance().registerClient( this );
  __IsoAgLib::getIsoMonitorInstance4Comm().registerControlFunctionStateHandler( mt_handler );
  m_lastActiveTaskTC = false;

  mc_devPropertyHandler.init();

  mpc_procDataHandler = NULL;

  // receive PROCESS_DATA_PGN messages which are addressed to GLOBAL
  const uint32_t cui32_filter = (((PROCESS_DATA_PGN) | 0xFF) << 8);
  if (!getIsoBusInstance4Comm().existFilter( mt_customer, IsoAgLib::iMaskFilter_c( (0x3FFFF00UL), cui32_filter) ) )
    getIsoBusInstance4Comm().insertFilter( mt_customer, IsoAgLib::iMaskFilter_c( (0x3FFFF00UL), cui32_filter ), 8, true);

  setTimePeriod(200);

  mpc_iter = c_arrClientC1.begin();

  setInitialized();
}

void Process_c::close()
{
  isoaglib_assert (initialized());

  getSchedulerInstance().unregisterClient( this );
  getIsoMonitorInstance4Comm().deregisterControlFunctionStateHandler( mt_handler );

  setClosed();
};

bool Process_c::timeEvent() {

  mc_devPropertyHandler.timeEvent();

  uint16_t ui16_nextTimePeriod = 0;

  if ( mpc_iter != c_arrClientC1.begin() )
    ui16_nextTimePeriod = 20;

  // call the time event for all local data
  for ( ;( mpc_iter != c_arrClientC1.end() ); mpc_iter++ )
  {
    if ( Scheduler_Task_c::getAvailableExecTime() == 0 )
    { // no time left, stop loop
      ui16_nextTimePeriod = 20;
      break;
    }

    // process next ProcData_c
    (*mpc_iter)->timeEvent( ui16_nextTimePeriod );
  }

  if ( mpc_iter == c_arrClientC1.end() )
    mpc_iter = c_arrClientC1.begin();

  if (ui16_nextTimePeriod)
  {
    if (ui16_nextTimePeriod < 20)
    { // skip small values
      ui16_nextTimePeriod = 20;
    }
    else if (ui16_nextTimePeriod > 200)
    { // limit large values (for Alive sending)
      ui16_nextTimePeriod = 200;
    }
    Scheduler_Task_c::setTimePeriod(ui16_nextTimePeriod); // + Scheduler_Task_c::getEarlierInterval());
  }

  return true;
};

void Process_c::resetTimerPeriod( void )
{
  getSchedulerInstance().changeTimePeriodAndResortTask(this, 100);
}

bool Process_c::processMsg( const CanPkg_c& arc_data )
{
  ProcessPkg_c pkg( arc_data, getMultitonInst() );

  if (pkg.getMonitorItemForSA() == NULL)
  { // SA = 0xFE  =>  don't handle such messages, we need to have a sender
    return true;
  }

  // check for sender isoName
  const IsoName_c& c_isoNameSender = pkg.getMonitorItemForSA()->isoName();
  if ( mc_isoNameTC.isSpecified() )
  {
    if ( mc_isoNameTC != c_isoNameSender ) // this is not the TC we are talking with !
      return true;
  }

  // process TC status message (for local instances)
  if ( pkg.men_command == ProcessPkg_c::taskControllerStatus )
  {
    processTcStatusMsg(pkg[4], c_isoNameSender);

    mc_devPropertyHandler.updateTcStateReceived(pkg[4]);
    mc_devPropertyHandler.setTcSourceAddress(pkg.isoSa());

    return true;
  }

  // use remoteType_t for the remote item
  const IsoAgLib::ProcData::remoteType_t ecuType = getProcessInstance4Comm().getTypeFromISOName( pkg.getMonitorItemForSA()->isoName() );

  // first check if this is a device property message -> then DevPropertyHandler_c should process this msg
  if ( ( pkg.men_command == ProcessPkg_c::requestConfiguration )
    || ( pkg.men_command == ProcessPkg_c::configurationResponse )
    || ( pkg.men_command == ProcessPkg_c::nack ) )
  {
    if (mc_devPropertyHandler.processMsg( pkg ))
      return true;
  }

  // no forther processing of NACK messages
  if ( pkg.men_command == ProcessPkg_c::nack )
    return true;

  // ignore other working set task message
  if ( pkg.men_command == ProcessPkg_c::workingsetMasterMaintenance )
  {
    // @TODO probably respond with NACK if it is addressed to us, otherwise just ignore
    return true;
  }

  // ignore broadcast message
  if (pkg.getMonitorItemForDA() == NULL)
  {
    return false;
  }

  // use isoName from corresponding monitor item for checks
  const IsoName_c& c_isoNameReceiver = pkg.getMonitorItemForDA()->isoName();

  bool b_elementFound = false;
  ProcData_c* pd = procData( pkg.mui16_DDI, pkg.mui16_element, c_isoNameReceiver, b_elementFound);
  if ( pd != NULL )
  { // there exists an appropriate process data item -> let the item process the msg
    pd->processMsg( pkg, ecuType );
  }
  else
  {
    // element exists but DDI not present -> DDI not supported by element
    sendNack( pkg.getMonitorItemForSA()->isoName(),
              pkg.getMonitorItemForDA()->isoName(),
              pkg.mui16_DDI,
              pkg.mui16_element,
              b_elementFound ? IsoAgLib::ProcData::NackDDINoSupportedByElement : IsoAgLib::ProcData::NackInvalidElementNumber );
  }

  return true;
}

void Process_c::sendNack( const IsoName_c& ac_da,
                          const IsoName_c& ac_sa,
                          int16_t a_ddi,
                          int16_t a_element,
                          IsoAgLib::ProcData::nackResponse_t a_errorcodes) const
{
  isoaglib_assert(a_errorcodes != IsoAgLib::ProcData::NackReserved1);
  isoaglib_assert(a_errorcodes != IsoAgLib::ProcData::NackReserved2);
  isoaglib_assert(a_errorcodes != IsoAgLib::ProcData::NackUndefined);

  CanPkgExt_c pkg;
  pkg.setISONameForDA(ac_da);
  pkg.setISONameForSA(ac_sa);
  pkg.setIsoPri(3);
  pkg.setIsoPgn(PROCESS_DATA_PGN);

  const uint8_t ui8_cmd = 0xd;
  pkg.setUint8Data(0, (ui8_cmd & 0xf) | ( (a_element & 0xf) << 4) );
  pkg.setUint8Data(1, a_element >> 4 );
  pkg.setUint8Data(2, a_ddi & 0x00FF );
  pkg.setUint8Data(3, (a_ddi & 0xFF00) >> 8 );
  pkg.setUint8Data(4, a_errorcodes);
  pkg.setUint8Data(5, 0xFF);
  pkg.setUint8Data(6, 0xFF);
  pkg.setUint8Data(7, 0xFF);
  pkg.setLen(8);

  getIsoBusInstance4Comm() << pkg;
}

ProcData_c* Process_c::procData( uint16_t aui16_DDI, uint16_t aui16_element, const IsoName_c& acrc_isoNameReceiver, bool& elementFound)
{
  elementFound = false;
  for ( cacheTypeC1_t pc_iter = c_arrClientC1.begin();
        pc_iter != c_arrClientC1.end();
        pc_iter++ )
  {
    if ( (*pc_iter)->isoName() != acrc_isoNameReceiver )
      continue;

    if ( (*pc_iter)->element() == aui16_element )
    {
      elementFound = true;
      if ( (*pc_iter)->DDI() == aui16_DDI )
      {
        return *pc_iter;
      }
    }
  }
  
  return NULL;
}

void
Process_c::reactOnIsoItemModification (ControlFunctionStateHandler_c::IsoItemModification_t at_action, IsoItem_c const& acrc_isoItem)
{
  switch (at_action)
  {
    case ControlFunctionStateHandler_c::AddToMonitorList:
      if (acrc_isoItem.itemState (IState_c::Local))
      { // local IsoItem_c has finished adr claim
        getIsoFilterManagerInstance4Comm().insertIsoFilter(   IsoFilter_s (mt_customer, IsoAgLib::iMaskFilter_c( (0x3FFFF00UL), ((PROCESS_DATA_PGN) << 8) ), &acrc_isoItem.isoName(), NULL, 8 ), true);
      }
      // else: don't care for remote IsoItem_c
      break;

    case ControlFunctionStateHandler_c::RemoveFromMonitorList:
      if (acrc_isoItem.itemState (IState_c::Local))
      { // local IsoItem_c has gone (i.e. IdentItem has gone, too.
        getIsoFilterManagerInstance4Comm().removeIsoFilter(  IsoFilter_s (mt_customer, IsoAgLib::iMaskFilter_c( (0x3FFFF00UL), ((PROCESS_DATA_PGN) << 8) ), &acrc_isoItem.isoName(), NULL, 8 ));
      }
      else
      {
        IsoAgLib::ProcData::remoteType_t ecuType = getTypeFromISOName(acrc_isoItem.isoName());
        if ( ecuType != IsoAgLib::ProcData::remoteTypeUndefined )
          stopRunningMeasurement( ecuType );
      }
      // else: don't care for other remote IsoItem_c
      break;

    default:
      break;
  }
}

bool Process_c::processTcStatusMsg(uint8_t ui8_tcStatus, const __IsoAgLib::IsoName_c& sender)
{
  // @TODO logger not supported yet

  if (mc_isoNameTC.isUnspecified())
  {
    mc_isoNameTC = sender;
    if (mpc_procDataHandler)
    { // call handler function if handler class is registered
      mpc_procDataHandler->processTcConnected(IsoAgLib::ProcData::remoteTypeTaskControl, mc_isoNameTC.toConstIisoName_c() );
    }
  }

  const IsoAgLib::ProcData::remoteType_t ecuType = getTypeFromISOName( sender );
  const bool activeTask = ui8_tcStatus & 0x1;

  if (activeTask != m_lastActiveTaskTC)
  {
    if (activeTask)
    {
      if (mpc_procDataHandler)
        mpc_procDataHandler->processTaskStarted(ecuType);
    }
    else
    {
      stopRunningMeasurement(ecuType);
      if (mpc_procDataHandler)
        mpc_procDataHandler->processTaskStopped(ecuType);
    }
  }

  m_lastActiveTaskTC = activeTask;

  return true;
}

#if DEBUG_SCHEDULER
const char*
Process_c::getTaskName() const
{ return "Process_c"; }
#endif

void
Process_c::updateEarlierAndLatestInterval(){
  mui16_earlierInterval = 0;
  mui16_latestInterval   =  ( getTimePeriod() / 2) ;
}

const IsoName_c& Process_c::getISONameFromType( IsoAgLib::ProcData::remoteType_t ecuType ) const
{
  if (IsoAgLib::ProcData::remoteTypeTaskControl == ecuType)
    return mc_isoNameTC;
#ifdef USE_DATALOGGER
  else if (IsoAgLib::ProcData::remoteTypeLogger == ecuType)
    return mc_isoNameLogger;
#endif

  return IsoName_c::IsoNameUnspecified();
}

IsoAgLib::ProcData::remoteType_t Process_c::getTypeFromISOName( const IsoName_c& isoName ) const
{
  if ( isoName == mc_isoNameTC )
    return IsoAgLib::ProcData::remoteTypeTaskControl;
#ifdef USE_DATALOGGER
  else if ( isoName == mc_isoNameLogger )
    return IsoAgLib::ProcData::remoteTypeLogger;
#endif

  return IsoAgLib::ProcData::remoteTypeUndefined;
}

void Process_c::stopRunningMeasurement(IsoAgLib::ProcData::remoteType_t ecuType)
{
  for ( cacheTypeC1_t pc_iter = c_arrClientC1.begin(); pc_iter != c_arrClientC1.end(); pc_iter++ )
  {
    (*pc_iter)->stopRunningMeasurement(ecuType);
  }
}

} // end of namespace __IsoAgLib
