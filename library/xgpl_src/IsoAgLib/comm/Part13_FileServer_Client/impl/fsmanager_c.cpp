/*
  fsmanager_c.cpp

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

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

// STL
#include <iterator>
#include <algorithm>


// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

/**
  * performe periodic tasks:
  * delete inactive fileservers and notify clientservercoms that have not been notified on fileservers yet.
  * @return true if all tasks where performed correctly
  */
bool
FsManager_c::timeEvent(void)
{
  STL_NAMESPACE::vector<FsServerInstance_c *>::iterator it_end = v_serverInstances.end();
  for (STL_NAMESPACE::vector<FsServerInstance_c *>::iterator it_serverInstance = v_serverInstances.begin();
       it_serverInstance != it_end;
       ++it_serverInstance)
  {
    (*it_serverInstance)->timeEvent();
  }

  for (STL_NAMESPACE::list<FsCommand_c *>::iterator it_command = l_initializingCommands.begin();
       it_command != l_initializingCommands.end(); )
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
        it_command = l_initializingCommands.erase(it_command);
        break;
    }
  }

  return true;
}


void
FsManager_c::reactOnIsoItemModification (ControlFunctionStateHandler_c::IsoItemModification_t at_action, IsoItem_c const& acrc_isoItem)
{
  // we only care for fileservers
  if (acrc_isoItem.isoName().getEcuType() != ISOName_c::ecuTypeFileServerOrPrinter)
    return;

  if (at_action == ControlFunctionStateHandler_c::AddToMonitorList)
  {
    FsServerInstance_c *pc_fsInstance = new FsServerInstance_c (acrc_isoItem, *this);
    v_serverInstances.push_back (pc_fsInstance);
  }
  else if (at_action == ControlFunctionStateHandler_c::RemoveFromMonitorList)
  {
    for (STL_NAMESPACE::vector<FsServerInstance_c *>::iterator it_serverInstance = v_serverInstances.begin();
         it_serverInstance != v_serverInstances.end();
         ++it_serverInstance)
    {
      // check if FsServerInstance to delete is in list
      if (acrc_isoItem.isoName() == (*it_serverInstance)->getIsoItem().isoName())
      { // There should only be one instance for an IsoName!
        // In all cases simply remove associated entries from l_initializingCommands
        for (STL_NAMESPACE::list<FsCommand_c *>::iterator it_command = l_initializingCommands.begin();
             it_command != l_initializingCommands.end(); )
        {
          if (&(*it_command)->getFileserver() == (*it_serverInstance))
          { // yep, command is connected to this FileServer.
            delete (*it_command);
            it_command = l_initializingCommands.erase(it_command);
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
        (void) v_serverInstances.erase (it_serverInstance);
        break;
      }
    }
  }
}


void
FsManager_c::init()
{
  if (checkAlreadyClosed())
  {
    clearAlreadyClosed();

    getSchedulerInstance().registerClient(this);
    getIsoMonitorInstance4Comm().registerControlFunctionStateHandler(mc_saClaimHandler);
  }
}


#if DEBUG_SCHEDULER
const char* FsManager_c::getTaskName() const
{ return "FsManager_c\n"; }
#endif


FsManager_c::FsManager_c()
  : mc_saClaimHandler(*this)
  , v_communications()
  , v_serverInstances()
  , l_initializingCommands()
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
FsManager_c::initFsClient(IdentItem_c &rc_identItem, IsoAgLib::iFsClient_c &rc_Client, IsoAgLib::iFsWhitelistList v_fsWhitelist)
{
  FsClientServerCommunication_c *c_fscscClient = NULL;

  for (STL_NAMESPACE::vector<FsClientServerCommunication_c *>::iterator it_communications = v_communications.begin();
       it_communications != v_communications.end();
       ++it_communications)
  {
    if ((*it_communications)->getClientIdentItem() == rc_identItem)
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
  if (!checkAlreadyClosed ())
  {
    setAlreadyClosed();

    getIsoMonitorInstance4Comm().deregisterControlFunctionStateHandler (mc_saClaimHandler);
    getSchedulerInstance().unregisterClient(this);

    STL_NAMESPACE::for_each( l_initializingCommands.begin(), l_initializingCommands.end(), delete_object());
    STL_NAMESPACE::for_each( v_communications.begin(), v_communications.end(), delete_object());
    STL_NAMESPACE::for_each( v_serverInstances.begin(), v_serverInstances.end(), delete_object());
  }
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
      for (STL_NAMESPACE::vector<FsClientServerCommunication_c *>::iterator it_communications = v_communications.begin();
          it_communications != v_communications.end();
          ++it_communications)
      {
        FsCommand_c *pc_command = new FsCommand_c(*(*it_communications), rc_fileserver);

        l_initializingCommands.push_back(pc_command);
        return;
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


/** C-style function, to get access to the unique FsManager_c singleton instance
 * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
 */
FsManager_c &getFsManagerInstance(uint8_t rui8_instance = 0)
{ // if > 1 singleton instance is used, no static reference can be used
  MACRO_MULTITON_GET_INSTANCE_BODY(FsManager_c, aui8_instance);
}

//End namespace
}
