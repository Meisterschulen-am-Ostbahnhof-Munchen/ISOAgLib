/*
  fsmanager_c.cpp

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
// own
#include "fsmanager_c.h"

// ISOAgLib
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isomonitor_c.h>
#include <IsoAgLib/driver/can/impl/canio_c.h>
#include <IsoAgLib/util/iassert.h>
#include <IsoAgLib/comm/impl/isobus_c.h>

// STL
#include <iterator>
#include <algorithm>

#if defined(_MSC_VER)
#pragma warning( disable : 4355 )
#endif

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

/**
  * performe periodic tasks:
  * delete inactive fileservers and notify clientservercoms that have not been notified on fileservers yet.
  */
void
FsManager_c::timeEvent(void)
{
  STL_NAMESPACE::vector<FsServerInstance_c *>::iterator it_end = m_servers.m_serverInstances.end();
  for (STL_NAMESPACE::vector<FsServerInstance_c *>::iterator it_serverInstance = m_servers.m_serverInstances.begin();
       it_serverInstance != it_end;
       ++it_serverInstance)
  {
    (*it_serverInstance)->timeEvent();
  }

  for (STL_NAMESPACE::list<FsCommand_c *>::iterator it_command = m_commands.l_initializingCommands.begin();
       it_command != m_commands.l_initializingCommands.end(); )
  {
    isoaglib_assert (*it_command);
    switch ((*it_command)->getFileserver().getState())
    {
      case FsServerInstance_c::offline:
      case FsServerInstance_c::online:
        // In these cases wait until initialization is complete
        ++it_command;
        break;

      case FsServerInstance_c::usable:
      case FsServerInstance_c::unusable:
        // Finished the initialization one way or another
        delete (*it_command);
        it_command = m_commands.l_initializingCommands.erase(it_command);
        break;
    }
  }
}


void
FsManager_c::reactOnIsoItemModification (ControlFunctionStateHandler_c::iIsoItemAction_e at_action, IsoItem_c const& acrc_isoItem)
{
  // we only care for fileservers
  if (acrc_isoItem.isoName().getEcuType() != IsoName_c::ecuTypeFileServerOrPrinter)
    return;

  if (at_action == ControlFunctionStateHandler_c::AddToMonitorList)
  {
    FsServerInstance_c *pc_fsInstance = new FsServerInstance_c (acrc_isoItem, *this);
    m_servers.m_serverInstances.push_back (pc_fsInstance);
  }
  else if (at_action == ControlFunctionStateHandler_c::RemoveFromMonitorList)
  {
    for (STL_NAMESPACE::vector<FsServerInstance_c *>::iterator it_serverInstance = m_servers.m_serverInstances.begin();
         it_serverInstance != m_servers.m_serverInstances.end();
         ++it_serverInstance)
    {
      // check if FsServerInstance to delete is in list
      if (acrc_isoItem.isoName() == (*it_serverInstance)->getIsoItem().isoName())
      { // There should only be one instance for an IsoName!
        // In all cases simply remove associated entries from l_initializingCommands
        for (STL_NAMESPACE::list<FsCommand_c *>::iterator it_command = m_commands.l_initializingCommands.begin();
             it_command != m_commands.l_initializingCommands.end(); )
        {
          if (&(*it_command)->getFileserver() == (*it_serverInstance))
          { // yep, command is connected to this FileServer.
            delete (*it_command);
            it_command = m_commands.l_initializingCommands.erase(it_command);
            // normally we could break the for loop here, too.
          }
          else
          { // search next command
            ++it_command;
          }
        }

        // Set the FS to offline before removing,
        // so that any necessary cleanup will be done correctly
        (*it_serverInstance)->setState (FsServerInstance_c::offline);
        // now delete the instance
        delete (*it_serverInstance);
        // and remove it from the list of instances.
        (void) m_servers.m_serverInstances.erase (it_serverInstance);
        break;
      }
    }
  }
}


void
FsManager_c::init()
{
  isoaglib_assert (!initialized());

  m_commands.init();

  getSchedulerInstance().registerTask( *this, 0 );
  getIsoMonitorInstance4Comm().registerControlFunctionStateHandler(mc_saClaimHandler);

  setInitialized();
}


FsManager_c::FsManager_c()
  : SchedulerTask_c( 100, true )
  , mc_saClaimHandler(*this)
  , m_servers( *this )
  , m_commands( *this )
  , v_communications()
{
}


/**
  * initFsClient registers a new fileserver client. If the client has already been registered, it does not re-register
  * the client, but returnes the already created client-server-communication object.
  * @param rc_identItem the IdentItem_c of the new fileserver client.
  * @param rc_Client the new fileserver client.
  * @return a new FsClientServerCommunication_c* if the client has not been registered yet. If the fileserver has been registered,
  * the original FsClientServerCommunication_c is returned.
  */
FsClientServerCommunication_c *
FsManager_c::initFsClient(IdentItem_c &rc_identItem, IsoAgLib::iFsClient_c &rc_Client, const IsoAgLib::iFsWhitelistList &v_fsWhitelist)
{
  FsClientServerCommunication_c *c_fscscClient = NULL;

  for (STL_NAMESPACE::vector<FsClientServerCommunication_c *>::iterator it_communications = v_communications.begin();
       it_communications != v_communications.end();
       ++it_communications)
  {
    if (&(*it_communications)->getClientIdentItem() == &rc_identItem)
    {
      return (*it_communications);
    }
  }

  c_fscscClient = new FsClientServerCommunication_c(rc_identItem, rc_Client, v_fsWhitelist);

  v_communications.push_back(c_fscscClient);

  return c_fscscClient;
}


// helper struct for the ::close function
struct delete_object
{
  template <typename T>
  void operator()(T *ptr){ delete ptr; }
};


void
FsManager_c::close()
{
  isoaglib_assert (initialized());

  m_commands.close();

  getIsoMonitorInstance4Comm().deregisterControlFunctionStateHandler (mc_saClaimHandler);
  getSchedulerInstance().deregisterTask(*this);

  STL_NAMESPACE::for_each( m_commands.l_initializingCommands.begin(), m_commands.l_initializingCommands.end(), delete_object());
  STL_NAMESPACE::for_each( v_communications.begin(), v_communications.end(), delete_object());
  STL_NAMESPACE::for_each( m_servers.m_serverInstances.begin(), m_servers.m_serverInstances.end(), delete_object());

  setClosed();
}


void
FsManager_c::notifyOnFileserverStateChange(
  FsServerInstance_c &rc_fileserver,
  FsServerInstance_c::FsState_en aen_oldState)
{
  switch (rc_fileserver.getState())
  {
    case FsServerInstance_c::offline:
      if (aen_oldState != FsServerInstance_c::usable)
        // don't care about offline-dropping if the FS wasn't
        // announced as usable before at all..
        break;

      for (STL_NAMESPACE::vector<FsClientServerCommunication_c *>::iterator it_communications = v_communications.begin();
          it_communications != v_communications.end();
          ++it_communications)
      {
        (*it_communications)->notifyOnOfflineFileServer (rc_fileserver);
      }
      break;

    case FsServerInstance_c::online:
      if ( !v_communications.empty() )
      {
        FsCommand_c *pc_command = new FsCommand_c(*v_communications.front(), rc_fileserver);
        m_commands.l_initializingCommands.push_back(pc_command);
      }
      break;

    case FsServerInstance_c::usable:
      for (STL_NAMESPACE::vector<FsClientServerCommunication_c *>::iterator it_communications = v_communications.begin();
           it_communications != v_communications.end();
           ++it_communications)
      {
        (*it_communications)->notifyOnUsableFileServer (rc_fileserver);
      }
      // now possibly some FsCSC(s) have connected itself to this FS
      break;

    case FsServerInstance_c::unusable:
      // no action currently
      break;
  }
}


void FsManager_c::FsServerManager_c::processFsToGlobal( const CanPkgExt_c& pkg ) {
  for (STL_NAMESPACE::vector<FsServerInstance_c *>::iterator it = m_serverInstances.begin(); it != m_serverInstances.end(); ++it) {
    if( pkg.getMonitorItemForSA() == &(*it)->getIsoItem() ) {
      (*it)->processFsToGlobal( pkg ); 
    }
  }
}


void FsManager_c::FsCommandManager_c::init() {
  getIsoBusInstance( m_fsManager.getMultitonInst() ).insertFilter( *this, IsoAgLib::iMaskFilterType_c( 0x3FF0000, FS_TO_CLIENT_PGN, Ident_c::ExtendedIdent ), 8 ); 
}


void FsManager_c::FsCommandManager_c::close() {
  getIsoBusInstance( m_fsManager.getMultitonInst() ).deleteFilter( *this, IsoAgLib::iMaskFilterType_c( 0x3FF0000, FS_TO_CLIENT_PGN, Ident_c::ExtendedIdent ) );
}


void FsManager_c::FsCommandManager_c::processMsg( const CanPkg_c& arc_data ) {
  CanPkgExt_c pkg( arc_data, m_fsManager.getMultitonInst() );
  if( ! pkg.isValid() || ( pkg.getMonitorItemForSA() == NULL ) )
    return;


  if( pkg.getMonitorItemForDA() == NULL ) {
    m_fsManager.m_servers.processFsToGlobal( pkg );
    return;
  }

  for( STL_NAMESPACE::list<FsCommand_c*>::iterator it = l_initializingCommands.begin(); it != l_initializingCommands.end(); ++it ) {
    (*it)->processMsgIso( pkg );
  }
  return;
}

/** C-style function, to get access to the unique FsManager_c singleton instance
 * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
 */
FsManager_c &getFsManagerInstance(uint8_t aui8_instance)
{ // if > 1 singleton instance is used, no static reference can be used
  MACRO_MULTITON_GET_INSTANCE_BODY(FsManager_c, PRT_INSTANCE_CNT, aui8_instance);
}

//End namespace
}
