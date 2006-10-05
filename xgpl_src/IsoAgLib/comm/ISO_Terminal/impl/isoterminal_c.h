/***************************************************************************
                          isoterminal_c.h  - central ISO terminal management
                             -------------------
    begin                : Wed Jul 09 2003
    copyright            : (C) 2003 by Dipl.-Inf. Martin Wodok
    email                : m.wodok@osb-ag:de
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
 * Copyright (C) 2000 - 2004 Dipl.-Inform. Achim Spangler                  *
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
#ifndef ISO_TERMINAL_H
#define ISO_TERMINAL_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include <IsoAgLib/typedef.h>
#include <IsoAgLib/util/impl/singleton.h>
#include <IsoAgLib/util/impl/cancustomer_c.h>
#include <IsoAgLib/util/impl/elementbase_c.h>
#include <IsoAgLib/util/impl/canpkgext_c.h>
#include <IsoAgLib/comm/SystemMgmt/ISO11783/impl/isomonitor_c.h>
#include <IsoAgLib/comm/ISO_Terminal/iisoterminalobjectpool_c.h>
#include <IsoAgLib/comm/SystemMgmt/impl/identitem_c.h>
#include "vtserverinstance_c.h"
#include "vtclientservercommunication_c.h"

#include <list>

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

// forward declarations
class VtClientServerCommunication_c;
class iIdentItem_c;
class ISOTerminal_c;
typedef SINGLETON_DERIVED(ISOTerminal_c,ElementBase_c) SingletonISOTerminal_c;

/** central IsoAgLib terminal management object */
class ISOTerminal_c : public SingletonISOTerminal_c, public __IsoAgLib::SaClaimHandler_c {
public:
  /** default destructor
  @see ISOTerminal_c::~ISOTerminal_c
  */
  virtual ~ISOTerminal_c();

  /** initialise element which can't be done during construct and registerIsoObjectPool
    possible errors:
  */
  void init();

  /** every subsystem of IsoAgLib has explicit function for controlled shutdown */
  void close();

  VtClientServerCommunication_c* registerIsoObjectPool (IdentItem_c& rpc_wsMasterIdentItem, IsoAgLib::iIsoTerminalObjectPool_c& rrefc_pool, char* rpc_versionLabel);

  bool deregisterIsoObjectPool (IdentItem_c& rpc_wsMasterIdentItem);

  /** periodically event -> call timeEvent for all  identities and parent objects
    @return true -> all planned activities performed in allowed time
  */
  bool timeEvent(void);

  /** function that handles incoming can messages */
  virtual bool processMsg();

  bool sendCommandForDEBUG(IsoAgLib::iIdentItem_c& rpc_wsMasterIdentItem, uint8_t* rpui8_buffer, uint32_t ui32_size);

  /** deliver reference to data pkg
    @return reference to the member ISOTerminalPkg_c, which encapsulates the CAN send structure
  */
  CANPkgExt_c& data(){return c_data;};

  /** deliver reference to data pkg as reference to CANPkgExt_c
    to implement the base virtual function correct
  */
  virtual CANPkgExt_c& dataBase();


  VtClientServerCommunication_c& getClientByID (uint8_t ui8_clientIndex) { return *vec_vtClientServerComm[ui8_clientIndex]; }

  VtClientServerCommunication_c* getClientPtrByID (uint8_t ui8_clientIndex) { return (vec_vtClientServerComm.size() > 0) ? vec_vtClientServerComm[ui8_clientIndex] : NULL; }

  bool isAnyVtAvailable() { return l_vtServerInst.size()>0; }


  /** for now allow multiple uploads
  bool getFlagForPoolUpload(const VtClientServerCommunication_c* pc_vtCSC);
  void resetFlagForPoolUpload(const VtClientServerCommunication_c* pc_vtCSC);
  */
  ////////////////////////
  // INTERFACE FUNTIONS //
  ////////////////////////
// the following define should be globally defined in the project settings...
#ifdef FAKE_VT_PROPERTIES
  void fakeVtProperties (uint16_t rui16_dimension, uint16_t rui16_skWidth, uint16_t rui16_skHeight, uint8_t rui16_colorDepth, uint16_t rui16_fontSizes);
#endif



private:
  friend class SINGLETON_DERIVED(ISOTerminal_c, ElementBase_c);
  /** private constructor which prevents direct instantiation in user application
    * NEVER define instance of ISOTerminal_c within application
    */
  ISOTerminal_c();

  /**
    initialize directly after the singleton instance is created.
    this is called from singleton.h and should NOT be called from the user again.
    users please use init(...) instead.
  */
  void singletonInit();

  /** this function is called by ISOMonitor_c when a new CLAIMED ISOItem_c is registered.
   * @param refc_isoName const reference to the item which ISOItem_c state is changed
   * @param rpc_newItem pointer to the currently corresponding ISOItem_c
   */
  void reactOnMonitorListAdd (const ISOName_c& refc_isoName, const ISOItem_c* rpc_newItem);
   /** this function is called by ISOMonitor_c when a device looses its ISOItem_c.
   * @param refc_isoName const reference to the item which ISOItem_c state is changed
   * @param rui8_oldSa previously used SA which is NOW LOST -> clients which were connected to this item can react explicitly
    */
  void reactOnMonitorListRemove (const ISOName_c& refc_isoName, uint8_t rui8_oldSa);

  // helper function to shield removal access to vtCSC-list
  void deregisterIsoObjectPoolInd (uint8_t rui8_index);

protected:
  /** for now allow parallel uploads
  bool sb_poolUploadInProgress;
  const VtClientServerCommunication_c* spc_vtcscForUpload;
  */

private: // attributes

  /** temp data where received data is put */
  CANPkgExt_c c_data;

  STL_NAMESPACE::list<VtServerInstance_c> l_vtServerInst;

  STL_NAMESPACE::vector<VtClientServerCommunication_c*> vec_vtClientServerComm;
};

#if defined(PRT_INSTANCE_CNT) && (PRT_INSTANCE_CNT > 1)
  /** C-style function, to get access to the unique Scheduler_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  ISOTerminal_c& getIsoTerminalInstance(uint8_t rui8_instance = 0);
#else
  /** C-style function, to get access to the unique Scheduler_c singleton instance */
  ISOTerminal_c& getIsoTerminalInstance(void);
#endif

}
#endif
