/***************************************************************************
                            fsmanager_c.h
                             -------------------
    begin                : Tue Oct 01 2007
    copyright            : (C) 2007 - 2008 by Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
    type                 : Header
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
 * Copyright (C) 2007 - 2008 Dipl.-Inform. Achim Spangler                  *
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
#ifndef FSMANAGER_C_H
#define FSMANAGER_C_H

#include "fsserverinstance_c.h"
#include "fsclientservercommunication_c.h"

#include <vector>
#include <list>

#include <IsoAgLib/typedef.h>

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
    FsClientServerCommunication_c *initFsClient(IdentItem_c &rc_identItem, IsoAgLib::iFsClient_c &rc_Client, std::vector<iFsWhitelist_c *> v_fsWhitelist);

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
