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

#include "fsserverinstance_c.h"
#include "fsclientservercommunication_c.h"

#include <vector>
#include <list>

#include <IsoAgLib/hal/hal_typedef.h>

#include <IsoAgLib/util/impl/singleton.h>
#include <IsoAgLib/scheduler/impl/schedulertask_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/iidentitem_c.h>

#include "fsserverinstance_c.h"
#include "fscommand_c.h"
#include "../ifsclient_c.h"

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

class FsManager_c;
typedef SINGLETON_DERIVED(FsManager_c,Scheduler_Task_c) SingletonFsManager_c;

/** central IsoAgLib terminal management object */
class FsManager_c : public SingletonFsManager_c
{
  public:

/**
  * Get a Fileserver by its source address.
  * @param ui8_SA the source address of the fileserver
  * @return FsServerInstance_c the fileserver with the desired source address, or NULL of no fileserver is found.
  */
    FsServerInstance_c *getFileServerBySA(uint8_t ui8_SA);

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
   virtual void reactOnIsoItemModification (IsoItemModification_t /*at_action*/, IsoItem_c const& /*acrc_isoItem*/);

  private:
    friend class SINGLETON_DERIVED(FsManager_c,Scheduler_Task_c);

    /** constructor is private, so singleton has to be used */
    FsManager_c();

/**
  * requests the properties for a fileserver according to ISO-Standard. If no fileserver client managed by the manager has a valid
  * source address this request can't be performed.
  * @param rui8_SA the source address of the desired fileserver
  * @return true if request was performed, flase if reqeust cannot be performed.
  */
    bool requestFsProperties(FsServerInstance_c &pc_fsInstance);

    /**
      * initialize directly after the singleton instance is created.
      * this is called from singleton.h and should NOT be called from the user again.
      * users please use init(...) instead.
      */
    void singletonInit();

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
    std::list<FsCommand_c *> l_commands;

    /**
      * boolean used to save it fileserveinstances without requested properties exist
      */
    bool b_fileserverWituoutProperties;

    /**
      * Deregister a fileserver from all client server communications that use it.
      * @param rc_fileserver the filerserver that shall be removed
      */
    void removeFileserverFromUsingClients(FsServerInstance_c &rc_fileserver);
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
