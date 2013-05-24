/*
  tcclient_c.cpp: central managing instance for all Task Controller-Clients

  (C) Copyright 2009 - 2013 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#include "tcclient_c.h"

#include <IsoAgLib/comm/impl/isobus_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isoitem_c.h>
#ifdef HAL_USE_SPECIFIC_FILTERS
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isofiltermanager_c.h>
#endif
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/procdata/procdata_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/idevicepool_c.h>
#include <IsoAgLib/util/iassert.h>

#include <list>

#if defined(_MSC_VER)
#pragma warning( disable : 4355 )
#endif

namespace __IsoAgLib {

TcClient_c&
getTcClientInstance( uint8_t instance )
{
  MACRO_MULTITON_GET_INSTANCE_BODY(TcClient_c, PRT_INSTANCE_CNT, instance);
}

TcClient_c::TcClient_c()
  : m_lastActiveTaskTC( false )
  , m_isoNameTC( IsoName_c::IsoNameUnspecified() )
#ifdef USE_DATALOGGER
  , m_lastActiveTaskLogger( false )
  , m_isoNameLogger( IsoName_c::IsoNameUnspecified() )
#endif
  , m_handler( *this )
  , m_customer( *this )
{
}


const TcClientConnection_c&
TcClient_c::getTcClientConnection( const IdentItem_c& identItem ) const
{
  // find matching connection via identitem
  STL_NAMESPACE::list<TcClientConnection_c*>::const_iterator it = m_tcConnections.begin();
  while (it != m_tcConnections.end())
  {
    if ((*it != NULL) && &(*it)->getIdentItem() == &identItem)
    { 
      return **it;
    }
    ++it;
  }
  isoaglib_assert(!"No matching TcClientConnection found!!!");
  return *m_tcConnections.front();
}

  
void
TcClient_c::init()
{
  isoaglib_assert (!initialized());

  __IsoAgLib::getIsoMonitorInstance4Comm().registerControlFunctionStateHandler( m_handler );
#ifdef HAL_USE_SPECIFIC_FILTERS
  getIsoBusInstance4Comm().insertFilter( m_customer, IsoAgLib::iMaskFilter_c( (0x3FFFF00UL), (PROCESS_DATA_PGN | 0xFF) << 8 ), 8 );
#else
  getIsoBusInstance4Comm().insertFilter( m_customer, IsoAgLib::iMaskFilter_c( (0x3FF0000UL), PROCESS_DATA_PGN << 8 ), 8 );
#endif

  setInitialized();
}


void
TcClient_c::close()
{
  isoaglib_assert (initialized());

#ifdef HAL_USE_SPECIFIC_FILTERS
  getIsoBusInstance4Comm().deleteFilter( m_customer, IsoAgLib::iMaskFilter_c( (0x3FFFF00UL), (PROCESS_DATA_PGN | 0xFF) << 8 ) );
#else
  getIsoBusInstance4Comm().deleteFilter( m_customer, IsoAgLib::iMaskFilter_c( (0x3FF0000UL), PROCESS_DATA_PGN << 8 ) );
#endif

  getIsoMonitorInstance4Comm().deregisterControlFunctionStateHandler( m_handler );

  setClosed();
}


TcClientConnection_c*
TcClient_c::initAndRegister( IdentItem_c& identItem, IsoAgLib::iProcDataHandler_c& procdata, IsoAgLib::iDevicePool_c& pool )
{
#ifndef NDEBUG
  for( STL_NAMESPACE::list<TcClientConnection_c*>::const_iterator it = m_tcConnections.begin();
    it != m_tcConnections.end(); ++it )
  {
    isoaglib_assert( &(*it)->getIdentItem() != &identItem );
  }
#endif

  isoaglib_assert( ! pool.isEmpty() );

  TcClientConnection_c* pc_tcCC = new TcClientConnection_c( identItem, *this, procdata, pool );
  m_tcConnections.push_back(pc_tcCC);

  return pc_tcCC;
}


bool
TcClient_c::deregister( IdentItem_c& identItem )
{
  /* what states the IdentItem could have we have to interrupt???
  * - IState_c::ClaimedAddress -> that item is Active and Member on ISOBUS
  * - !UploadType::UploadIdle -> interrupt any upload
  */
  STL_NAMESPACE::list<TcClientConnection_c*>::iterator it = m_tcConnections.begin();
  while (it != m_tcConnections.end())
  {
    if ((*it != NULL) && &(*it)->getIdentItem() == &identItem)
    { 
      delete *it;
      m_tcConnections.erase(it);
      break;
    }
    ++it;
  }

  if (it == m_tcConnections.end())
    return false; // appropriate IdentItem could not be found, so nothing was deleted
  else
    return true; // IdentItem was found and deleted
}


void
TcClient_c::processMsg( const CanPkg_c& data )
{
  ProcessPkg_c pkg( data, getMultitonInst() );

  if( ! pkg.isValid() || ( pkg.getMonitorItemForSA() == NULL ) )
    return;

  // check for sender isoName
  if ( m_isoNameTC.isSpecified() && ( m_isoNameTC != pkg.getMonitorItemForSA()->isoName() ) )
  {
    // this is not the TC we are talking with !
    return;
  }
  // @todo what to do for logger?

  if( pkg.getMonitorItemForDA() != NULL )
    processMsgNonGlobal( pkg );
  else
    processMsgGlobal( pkg );
}


void
TcClient_c::processMsgGlobal( const ProcessPkg_c& data ) 
{
  // process TC status message (for local instances)
  if ( data.men_command == ProcessPkg_c::taskControllerStatus )
  {
    processTcStatusMsg(data[4], data.getMonitorItemForSA()->isoName() );

    STL_NAMESPACE::list<TcClientConnection_c*>::const_iterator it = m_tcConnections.begin();
    while (it != m_tcConnections.end())
    {
      (*it)->updateTcStateReceived( data[4] );
      (*it)->setTcIsoItem( *data.getMonitorItemForSA() );
      ++it;
    }
  }
}


void
TcClient_c::processMsgNonGlobal( const ProcessPkg_c& pkg ) 
{
  // first check if this is a device property message -> then DevPropertyHandler_c should process this msg
  if ( ( pkg.men_command == ProcessPkg_c::requestConfiguration )
    || ( pkg.men_command == ProcessPkg_c::configurationResponse )
    || ( pkg.men_command == ProcessPkg_c::nack ) )
  {    
    STL_NAMESPACE::list<TcClientConnection_c*>::const_iterator it = m_tcConnections.begin();
    while (it != m_tcConnections.end())
    {
      (*it)->processMsgTc( pkg );
      ++it;
    }
  }
  else
  {
    // use remoteType_t for the remote item
    const IsoAgLib::ProcData::RemoteType_t ecuType = getTypeFromISOName( pkg.getMonitorItemForSA()->isoName() );

    // no forther processing of NACK messages
    if ( pkg.men_command == ProcessPkg_c::nack )
      return;

    // ignore other working set task message
    if ( pkg.men_command == ProcessPkg_c::workingsetMasterMaintenance )
    {
      // @TODO probably respond with NACK if it is addressed to us, otherwise just ignore
      return;
    }

    // use isoName from corresponding monitor item for checks
    const IsoName_c& c_isoNameReceiver = pkg.getMonitorItemForDA()->isoName();

    STL_NAMESPACE::list<TcClientConnection_c*>::const_iterator it = m_tcConnections.begin();
    while (it != m_tcConnections.end())
    {
      if ( (*it)->getIdentItem().isoName() == c_isoNameReceiver )
      {
        bool b_elementFound = false;
        ProcData_c* pd = (*it)->procData( pkg.mui16_DDI, pkg.mui16_element, c_isoNameReceiver, b_elementFound );
        if ( pd != NULL )
        { // there exists an appropriate process data item -> let the item process the msg
          pd->processMsg( pkg, ecuType );
        }
        else
        {
          // element exists but DDI not present -> DDI not supported by element
          (*it)->sendNack( pkg.getMonitorItemForSA()->isoName(),
                           pkg.getMonitorItemForDA()->isoName(),
                           pkg.mui16_DDI,
                           pkg.mui16_element,
                           b_elementFound ? IsoAgLib::ProcData::NackDDINoSupportedByElement : IsoAgLib::ProcData::NackInvalidElementNumber );
        }
      }
      ++it;
    }
  }
}


const IsoName_c&
TcClient_c::getISONameFromType( IsoAgLib::ProcData::RemoteType_t ecuType ) const
{
  if (IsoAgLib::ProcData::RemoteTypeTaskControl == ecuType)
  {
    if (isTcAvailable())
      return m_isoNameTC;
  }
#ifdef USE_DATALOGGER
  else if (IsoAgLib::ProcData::RemoteTypeLogger == ecuType)
  {
    if (isLoggerAvailable())
      return m_isoNameLogger;
  }
#endif

  return IsoName_c::IsoNameUnspecified();
}


IsoAgLib::ProcData::RemoteType_t
TcClient_c::getTypeFromISOName( const IsoName_c& isoName ) const
{
  if ( isoName == m_isoNameTC )
    return IsoAgLib::ProcData::RemoteTypeTaskControl;
#ifdef USE_DATALOGGER
  else if ( isoName == m_isoNameLogger )
    return IsoAgLib::ProcData::RemoteTypeLogger;
#endif

  return IsoAgLib::ProcData::RemoteTypeUndefined;
}


void
TcClient_c::reactOnIsoItemModification( ControlFunctionStateHandler_c::iIsoItemAction_e action, IsoItem_c const& isoItem )
{
  if( ( action == ControlFunctionStateHandler_c::RemoveFromMonitorList ) &&( ! isoItem.itemState( IState_c::Local ) ) )
  {
    IsoAgLib::ProcData::RemoteType_t ecuType = getTypeFromISOName(isoItem.isoName());
    if ( ecuType != IsoAgLib::ProcData::RemoteTypeUndefined )
    {
      STL_NAMESPACE::list<TcClientConnection_c*>::const_iterator it = m_tcConnections.begin();
      while (it != m_tcConnections.end())
      {
        (*it)->stopRunningMeasurement( ecuType );
        ++it;
      }
    }
  }

#ifdef HAL_USE_SPECIFIC_FILTERS
  // @todo Clean that up properly, move it to TC-Client connection!!!
  // This is just a simple implementation until the Dynamic TC-Client is ready!
  // Note that the filter wouldn't be remove if the IdentItem is closed AFTER the TcClient.
  // This will be redone for 2.5.4 anyway. It's fine if the application is completely closed/restarted,
  // but may result in problems if only the TC-Client would be stopped without the rest...
  // But nowone does that in 2.5.3.........
  if( isoItem.itemState( IState_c::Local ) )
  {
    switch( action )
    {
    case ControlFunctionStateHandler_c::AddToMonitorList:
      getIsoFilterManagerInstance4Comm().insertIsoFilter( IsoFilter_s (m_customer, IsoAgLib::iMaskFilter_c( 0x3FFFF00UL, (PROCESS_DATA_PGN << 8) ), &isoItem.isoName(), NULL, 8) );
      break;
    case ControlFunctionStateHandler_c::RemoveFromMonitorList:
      getIsoFilterManagerInstance4Comm().removeIsoFilter( IsoFilter_s (m_customer, IsoAgLib::iMaskFilter_c( 0x3FFFF00UL, (PROCESS_DATA_PGN << 8) ), &isoItem.isoName(), NULL, 8) );
      break;
    default:
      break;
    }
  }
#endif
}


bool
TcClient_c::processTcStatusMsg( uint8_t tcStatus, const __IsoAgLib::IsoName_c& sender )
{
  // @TODO logger not supported yet

  if (m_isoNameTC.isUnspecified())
  {
    m_isoNameTC = sender;
    STL_NAMESPACE::list<TcClientConnection_c*>::const_iterator it = m_tcConnections.begin();
    while (it != m_tcConnections.end())
    {
      (*it)->processTcConnected( IsoAgLib::ProcData::RemoteTypeTaskControl, m_isoNameTC.toConstIisoName_c() );
      ++it;
    }
  }

  const IsoAgLib::ProcData::RemoteType_t ecuType = getTypeFromISOName( sender );
  const bool activeTask = tcStatus & 0x1;

  if (activeTask != m_lastActiveTaskTC)
  {
    if (activeTask)
    {
      STL_NAMESPACE::list<TcClientConnection_c*>::const_iterator it = m_tcConnections.begin();
      while ( it != m_tcConnections.end() )
      {
        (*it)->processTaskStarted( ecuType );
        ++it;
      }
    }
    else
    {
      STL_NAMESPACE::list<TcClientConnection_c*>::const_iterator it = m_tcConnections.begin();
      while ( it != m_tcConnections.end() )
      {
        (*it)->stopRunningMeasurement( ecuType );
        (*it)->processTaskStopped( ecuType );
        ++it;
      }
    }
  }

  m_lastActiveTaskTC = activeTask;

  return true;
}


void 
TcClient_c::processChangeDesignator( IdentItem_c& ident, uint16_t objID, const char* newDesig )
{
  STL_NAMESPACE::list<TcClientConnection_c*>::const_iterator it = m_tcConnections.begin();
  while ( it != m_tcConnections.end() )
  {
    if ( &ident == &((*it)->getIdentItem()) )
      (*it)->sendCommandChangeDesignator( objID, newDesig, CNAMESPACE::strlen(newDesig) );
    ++it;
  }
}

} // __IsoAgLib
