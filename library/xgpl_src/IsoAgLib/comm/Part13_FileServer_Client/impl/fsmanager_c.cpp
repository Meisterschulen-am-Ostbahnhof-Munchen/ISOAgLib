#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isomonitor_c.h>
#include <IsoAgLib/driver/can/impl/canio_c.h>
#include "fsmanager_c.h"

#include <iterator>

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

  bool b_fsPropertiesRequested = false;

  std::vector<FsServerInstance_c *>::iterator it_end = v_serverInstances.end();
  for (std::vector<FsServerInstance_c *>::iterator it_serverInstance = v_serverInstances.begin();
       it_serverInstance != it_end;
       it_serverInstance++)
  {
    // check if FsServerInstance to delete is in list
    if ((*it_serverInstance)->getLastTime() != -1 && (uint32_t)(HAL::getTime () - (uint32_t)(*it_serverInstance)->getLastTime()) > (uint32_t)6000)
    {
      (*it_serverInstance)->setOffline();
      removeFileserverFromUsingClients(*(*it_serverInstance));
      requestFsProperties(*(*it_serverInstance));
      b_fsPropertiesRequested = true;
    } else if ((*it_serverInstance)->getLastTime() != -1 &&
               (*it_serverInstance)->getInitStatus() == FsServerInstance_c::removed) {
      delete (*it_serverInstance);
      v_serverInstances.erase(it_serverInstance);
    } else if (b_fileserverWituoutProperties) {
      if (!(*it_serverInstance)->getPropertiesSet()) {
        b_fsPropertiesRequested = true;
        if ((*it_serverInstance)->getPropertiesReqeusted() == -1)
        {
          if (requestFsProperties(*(*it_serverInstance)))
            (*it_serverInstance)->setPropertiesReqeusted();
        }
        else if ((uint32_t)(HAL::getTime () - (uint32_t)(*it_serverInstance)->getPropertiesReqeusted()) > (uint32_t)12000
                   && (*it_serverInstance)->getLastTime() == -1)
        {
          (*it_serverInstance)->setOffline();
          removeFileserverFromUsingClients(*(*it_serverInstance));
          requestFsProperties(*(*it_serverInstance));
        }
        else if ((uint32_t)(HAL::getTime () - (uint32_t)(*it_serverInstance)->getPropertiesReqeusted()) > (uint32_t)6000)
        {
          if (requestFsProperties(*(*it_serverInstance)))
            (*it_serverInstance)->setPropertiesReqeusted();
        }
      }
    }
  }

  b_fileserverWituoutProperties = b_fsPropertiesRequested;

  //look for clientservercom with unreported fileservers...

  for (std::vector<FsClientServerCommunication_c *>::iterator it_communications = v_communications.begin();
       it_communications != v_communications.end();
       it_communications++)
  {
    for (std::vector<FsServerInstance_c *>::iterator it_serverInstance = v_serverInstances.begin();
       it_serverInstance != it_end;
       it_serverInstance++)
    {
      if ((*it_serverInstance)->knowsVolumes())
      {
        if ((*it_serverInstance)->getInitStatus() == FsServerInstance_c::unreported) {
          (*it_communications)->notifyOnNewFileServer(*(*it_serverInstance));
          if (it_communications + 1 == v_communications.end())
          {
            (*it_serverInstance)->setReported();
          }
        } else if (!(*it_communications)->getHasBeenNotifiedOnFileServers())
        {
          (*it_communications)->notifyOnNewFileServer(*(*it_serverInstance));
        }
      }
    }
  }

  //delete commands for fileservers whoes properties have been requested or who have been removed.
  for (std::list<FsCommand_c *>::iterator it_command = l_commands.begin();
       it_command != l_commands.end();
       )
  {
    if ((*it_command) && (*it_command)->getFileserver().getInitStatus() == FsServerInstance_c::used)
    {
      delete (*it_command);
      it_command = l_commands.erase(it_command);
      continue;
    } else if ((*it_command) && (*it_command)->getFileserver().getInitStatus() == FsServerInstance_c::removed)
    {
      (*it_command)->getFileserver().resetLastTime();
      delete (*it_command);
      it_command = l_commands.erase(it_command);
      continue;
    }
    it_command++;
  }

  return true;
}

/**
  * Get a Fileserver by its source address.
  * @param ui8_SA the source address of the fileserver
  * @return FsServerInstance_c the fileserver with the desired source address, or NULL of no fileserver is found.
  */
FsServerInstance_c *FsManager_c::getFileServerBySA(uint8_t ui8_SA)
{

  for (std::vector<FsServerInstance_c *>::iterator it_serverInstance = v_serverInstances.begin();
       it_serverInstance != v_serverInstances.end();
       it_serverInstance++)
  {
    if (ui8_SA == (*it_serverInstance)->getIsoItem().nr())
    {
      return (*it_serverInstance);
    }
  }
  return NULL;
}

/** this function is called by IsoMonitor_c on addition, state-change and removal of an IsoItem.
  *   @param at_action enumeration indicating what happened to this IsoItem. @see IsoItemModification_en / IsoItemModification_t
  *   @param acrc_isoItem reference to the (const) IsoItem which is changed (by existance or state)
  */
void FsManager_c::reactOnIsoItemModification (SaClaimHandler_c::IsoItemModification_t at_action, IsoItem_c const& acrc_isoItem)
{
  if (at_action == SaClaimHandler_c::AddToMonitorList) {
    FsServerInstance_c *pc_fsInstance;

    // we only care for fileservers
    if (acrc_isoItem.isoName().getEcuType() != ISOName_c::ecuTypeFileServerOrPrinter)
    {
      return;
    }

    pc_fsInstance = new FsServerInstance_c(acrc_isoItem, *this);

    v_serverInstances.push_back(pc_fsInstance);

    //request properties and mark for fileserver without properties.
    requestFsProperties(*pc_fsInstance);
    b_fileserverWituoutProperties = true;
  } else if (at_action == SaClaimHandler_c::RemoveFromMonitorList) {
    // we only care for fileservers
    if (acrc_isoItem.isoName().getEcuType() != ISOName_c::ecuTypeFileServerOrPrinter)
    {
      return;
    }

    for (std::vector<FsServerInstance_c *>::iterator it_serverInstance = v_serverInstances.begin();
         it_serverInstance != v_serverInstances.end();
         it_serverInstance++)
    {
      // check if FsServerInstance to delete is in list
      if (acrc_isoItem.isoName() == (*it_serverInstance)->getIsoItem().isoName())
      {
        (*it_serverInstance)->setRemoved();
        return;
      }
    }
  }
}

/**
  * requests the properties for a fileserver according to ISO-Standard. If no fileserver client managed by the manager has a valid
  * source address this request can't be performed.
  * @param rui8_SA the source address of the desired fileserver
  * @return true if request was performed, flase if reqeust cannot be performed.
  */
bool FsManager_c::requestFsProperties(FsServerInstance_c &pc_fsInstance)
{
  CANPkgExt_c canpkgext;

  for (std::vector<FsClientServerCommunication_c *>::iterator it_communications = v_communications.begin();
       it_communications != v_communications.end();
       it_communications++)
  {
    if ((*it_communications)->getClientIdentItem().isClaimedAddress())
    {
      FsCommand_c *pc_command = new FsCommand_c(*(*it_communications), pc_fsInstance);
      pc_command->initFileserver();
      l_commands.push_back(pc_command);
      return true;
    }
  }

  return false;
}

/**
  * Initializes the fileserver manager. The manager is registered to the time-scheduler, the SAClaimHandler.
  */
void
FsManager_c::singletonInit()
{
  // register in Scheduler_c to get time-events
  getSchedulerInstance4Comm().registerClient(this);
  // register to get ISO monitor list changes
  getIsoMonitorInstance4Comm().registerSaClaimHandler(this);

  // register Filter in CANIO_c
  bool b_atLeastOneFilterAdded=false;

  b_atLeastOneFilterAdded |= (
                      getCanInstance4Comm().insertFilter (*this,
                                                         (0x3FF0000UL),
                                                         ((static_cast<MASK_TYPE>(FS_TO_CLIENT_PGN)<<8)),
                                                         false,
                                                         Ident_c::ExtendedIdent, 8)
                            != NULL);

  if (b_atLeastOneFilterAdded)
    getCanInstance4Comm().reconfigureMsgObj();
}

const char* FsManager_c::getTaskName() const
{
  return "FsManager_c\n";
}

FsManager_c::~FsManager_c()
{
  close();
}

FsManager_c::FsManager_c()
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
FsClientServerCommunication_c *FsManager_c::initFsClient(IdentItem_c &rc_identItem, IsoAgLib::iFsClient_c &rc_Client, std::vector<iFsWhitelist_c *> v_fsWhitelist)
{
  FsClientServerCommunication_c *c_fscscClient = NULL;

  for (std::vector<FsClientServerCommunication_c *>::iterator it_communications = v_communications.begin();
       it_communications != v_communications.end();
       it_communications++)
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

/** function used to destroy the object FsManager_c */
void
FsManager_c::close()
{
  // deregister in Scheduler_c
  getSchedulerInstance4Comm().unregisterClient(this);
  // deregister in ISOMonitor_c
  getIsoMonitorInstance4Comm().deregisterSaClaimHandler(this);

  getCanInstance4Comm().deleteFilter(*this,
                                    (0x3FF0000UL),
                                    ((static_cast<MASK_TYPE>(FS_TO_CLIENT_PGN)<<8 )),
                                    Ident_c::ExtendedIdent);

  v_communications.clear();
  v_serverInstances.clear();
}

void FsManager_c::removeFileserverFromUsingClients(FsServerInstance_c &rc_fileserver)
{
  for (std::vector<FsClientServerCommunication_c *>::iterator it_communications = v_communications.begin();
       it_communications != v_communications.end();
       it_communications++)
  {
    if ((*it_communications)->getFileserver()->getInitStatus() == FsServerInstance_c::offline)
    {
      (*it_communications)->notifyOnFileServerOffline(rc_fileserver);
    }
  }
}


#if defined(PRT_INSTANCE_CNT) && (PRT_INSTANCE_CNT > 1)
  /** C-style function, to get access to the unique FsManager_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  FsManager_c &getFsManagerInstance(uint8_t rui8_instance = 0)
  { // if > 1 singleton instance is used, no static reference can be used
    return FsManager_c::instance(rui8_instance);
  };
#else
  /** C-style function, to get access to the unique FsManager_c singleton instance */
  FsManager_c &getFsManagerInstance(void)
  {
    static FsManager_c& c_fsManager = FsManager_c::instance();
    return c_fsManager;
  };
#endif

//End namespace
}
