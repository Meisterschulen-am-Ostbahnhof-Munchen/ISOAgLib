/*
  fsmanager_c.h

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

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
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/saclaimhandler_c.h>
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

class FsManager_c;
typedef SINGLETON_DERIVED(FsManager_c,Scheduler_Task_c) SingletonFsManager_c;

/** central IsoAgLib terminal management object */
class FsManager_c : public SingletonFsManager_c
{
  public:

  class SaClaimHandlerProxy_c : public SaClaimHandler_c {
  public:
    typedef FsManager_c Owner_t;

    SaClaimHandlerProxy_c(Owner_t &art_owner) : mrt_owner(art_owner) {}

    virtual ~SaClaimHandlerProxy_c() {}

  private:
    virtual void reactOnIsoItemModification(
        IsoItemModification_t at_action,
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

    /** function used to destroy the FsManager_c object */
    void close();
    virtual ~FsManager_c();

/**
  * performe periodic tasks:
  * delete inactive fileservers and notify clientservercoms that have not been notified on fileservers yet.
  * Once a fileserver's properties have been requested, the fscommand object, taking care of the request
  * is deleted.
  * @return true if all tasks where performed correctly
  */
    bool timeEvent(void);

    /**  Used for Debugging Tasks in FsManager_c */
    const char* getTaskName() const;

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
    FsClientServerCommunication_c *initFsClient(IdentItem_c &rc_identItem, IsoAgLib::iFsClient_c &rc_Client, IsoAgLib::iFsWhitelistList v_fsWhitelist);

   /** this function is called by IsoMonitor_c on addition, state-change and removal of an IsoItem.
     <!-- @param at_action enumeration indicating what happened to this IsoItem. @see IsoItemModification_en / IsoItemModification_t
      @param acrc_isoItem reference to the (const) IsoItem which is changed (by existance or state)-->
     */
   virtual void reactOnIsoItemModification (SaClaimHandler_c::IsoItemModification_t /*at_action*/, IsoItem_c const& /*acrc_isoItem*/);

  private:
    friend class SINGLETON_DERIVED(FsManager_c,Scheduler_Task_c);

    /** constructor is private, so singleton has to be used */
    FsManager_c();

    /**
      * initialize directly after the singleton instance is created.
      * this is called from singleton.h and should NOT be called from the user again.
      * users please use init(...) instead.
      */
    void singletonInit();

  public:
    /**
      * A FsServerInstance_c will notify the Manager
      * that it has its state changed.
      */
    void notifyOnFileserverStateChange(
      FsServerInstance_c &rc_fileserver,
      FsServerInstance_c::FsState_en aen_oldState);

  private:
    // member variable instead of multiple inheritance
    Handler_t mc_saClaimHandler;

    /**
     * Vector of registered FsClientServerCommunication_c
     */
    std::vector<FsClientServerCommunication_c *> v_communications;

    /**
     * Vector of known FsServerInstance_c
     */
    std::vector<FsServerInstance_c *> v_serverInstances;

    /**
     * Vector of FsCommand_c* requesting properties for not-online FsServerInstance_cS
     */
    std::list<FsCommand_c *> l_initializingCommands;
};

#if defined(PRT_INSTANCE_CNT) && (PRT_INSTANCE_CNT > 1)
  /** C-style function, to get access to the unique FsManager_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  FsManager_c &getFsManagerInstance(uint8_t rui8_instance = 0);
#else
  /** C-style function, to get access to the unique FsManager_c singleton instance */
  FsManager_c &getFsManagerInstance(void);
#endif

//End namespace
}


#endif
