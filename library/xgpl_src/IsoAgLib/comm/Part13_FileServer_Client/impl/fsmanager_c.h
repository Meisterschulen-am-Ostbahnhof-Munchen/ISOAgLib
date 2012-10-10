/*
  fsmanager_c.h

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef FSMANAGER_C_H
#define FSMANAGER_C_H

// ISOAgLib
#include <IsoAgLib/util/impl/singleton.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/iidentitem_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/cfstatehandler_c.h>
#include <IsoAgLib/scheduler/impl/schedulertask_c.h>

// own
#include "fsserverinstance_c.h"
#include "fsclientservercommunication_c.h"
#include "fscommand_c.h"
#include "../ifsclient_c.h"

// STL
#include <vector>
#include <list>


// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

/** central IsoAgLib terminal management object */
class FsManager_c : public SchedulerTask_c
{
  MACRO_MULTITON_CONTRIBUTION();
  private:
    FsManager_c();

  public:

  class SaClaimHandlerProxy_c : public ControlFunctionStateHandler_c {
    public:
      typedef FsManager_c Owner_t;

      SaClaimHandlerProxy_c(Owner_t &art_owner) : mrt_owner(art_owner) {}

      virtual ~SaClaimHandlerProxy_c() {}

    private:
      virtual void reactOnIsoItemModification(
          iIsoItemAction_e at_action,
          IsoItem_c const &acrc_isoItem)
      {
        mrt_owner.reactOnIsoItemModification(at_action, acrc_isoItem);
      }

      // SaClaimHandlerProxy_c shall not be copyable. Otherwise the
      // reference to the containing object would become invalid.
      SaClaimHandlerProxy_c(SaClaimHandlerProxy_c const &);

      SaClaimHandlerProxy_c &operator=(SaClaimHandlerProxy_c const &);

      Owner_t &mrt_owner;
  };
  typedef SaClaimHandlerProxy_c Handler_t;


  class FsServerManager_c {
    public:
      FsServerManager_c( FsManager_c& fsManager ) : m_serverInstances(), m_fsManager( fsManager ) {}

      bool processFsToGlobal( const CanPkgExt_c& pkg );

      STL_NAMESPACE::vector<FsServerInstance_c *> m_serverInstances;
      FsManager_c& m_fsManager;
  };

  class FsCommandManager_c : public CanCustomer_c {
    public:
      FsCommandManager_c( FsManager_c& fsManager ) : l_initializingCommands(), m_fsManager( fsManager ) {}
      void init();
      void close();

      virtual bool processMsg( const CanPkg_c& arc_data );

      STL_NAMESPACE::list<FsCommand_c*> l_initializingCommands;

      FsManager_c& m_fsManager;
  };


  void init();
  void close();

  virtual ~FsManager_c() {}

/**
  * performe periodic tasks:
  * delete inactive fileservers and notify clientservercoms that have not been notified on fileservers yet.
  * Once a fileserver's properties have been requested, the fscommand object, taking care of the request
  * is deleted.
  */
  void timeEvent(void);

/**
  * initFsClient registers a new fileserver client. If the client has already been registered, it does not re-register
  * the client, but returnes the already created client-server-communication object.
  * @param rc_identItem the IdentItem_c of the new fileserver client.
  * @param rc_Client the new fileserver client.
  * @param v_fsWhitelist a vector containing an or-list of desired fileserver properties. all fileservers fullfilling one of those
  *                      options will be reported.
  * @return a new FsClientServerCommunication_c* if the client has not been registered yet. If the fileserver has been registered,
  * the original FsClientServerCommunication_c is returned.
  */
  FsClientServerCommunication_c *initFsClient(IdentItem_c &rc_identItem, IsoAgLib::iFsClient_c &rc_Client, const IsoAgLib::iFsWhitelistList &v_fsWhitelist);

   virtual void reactOnIsoItemModification (ControlFunctionStateHandler_c::iIsoItemAction_e /*at_action*/, IsoItem_c const& /*acrc_isoItem*/);

  public:
    /**
      * A FsServerInstance_c will notify the Manager
      * that it has its state changed.
      */
    void notifyOnFileserverStateChange(
      FsServerInstance_c &rc_fileserver,
      FsServerInstance_c::FsState_en aen_oldState);

  private:
    Handler_t mc_saClaimHandler;
    FsServerManager_c m_servers;
    FsCommandManager_c m_commands;

    STL_NAMESPACE::vector<FsClientServerCommunication_c *> v_communications;

    friend FsManager_c &getFsManagerInstance(uint8_t rui8_instance);
};

/** C-style function, to get access to the unique FsManager_c singleton instance
 * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
 */
FsManager_c &getFsManagerInstance(uint8_t rui8_instance = 0);

//End namespace
}


#endif
