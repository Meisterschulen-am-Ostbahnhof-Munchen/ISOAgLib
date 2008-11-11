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
#include <IsoAgLib/driver/can/impl/cancustomer_c.h>
#include <IsoAgLib/scheduler/impl/schedulertask_c.h>
#include <IsoAgLib/comm/Part3_DataLink/impl/canpkgext_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isomonitor_c.h>
#include <IsoAgLib/comm/Part6_VirtualTerminal_Client/iisoterminalobjectpool_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/identitem_c.h>
#include "vtserverinstance_c.h"
#include "vtclientservercommunication_c.h"

#include <list>

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

// forward declarations
class VtClientServerCommunication_c;
class iIdentItem_c;
class IsoTerminal_c;
typedef SINGLETON_DERIVED(IsoTerminal_c,Scheduler_Task_c) SingletonIsoTerminal_c;

/** central IsoAgLib terminal management object */
class IsoTerminal_c : public SingletonIsoTerminal_c {
public:
  /** default destructor
  @see IsoTerminal_c::~IsoTerminal_c
  */
  virtual ~IsoTerminal_c();

  /** initialise element which can't be done during construct and registerIsoObjectPool
    possible errors:
  */
  void init();

  /** every subsystem of IsoAgLib has explicit function for controlled shutdown */
  void close();

  VtClientServerCommunication_c* initAndRegisterIsoObjectPool (IdentItem_c& apc_wsMasterIdentItem, IsoAgLib::iIsoTerminalObjectPool_c& arc_pool, char* apc_versionLabel);

  bool deregisterIsoObjectPool (IdentItem_c& apc_wsMasterIdentItem);

  /** periodically event -> call timeEvent for all  identities and parent objects
    @return true -> all planned activities performed in allowed time
  */
  bool timeEvent(void);

  /** @todo Remove this - this should only be temporary and a good solution needs to be implemented! */
  void TEMPORARYSOLUTION_setTimePeriod(uint16_t aui16_timePeriod) { setTimePeriod (aui16_timePeriod); }

  /** function that handles incoming can messages */
  virtual bool processMsg();

  bool sendCommandForDEBUG(IsoAgLib::iIdentItem_c& apc_wsMasterIdentItem, uint8_t* apui8_buffer, uint32_t ui32_size);

  /** deliver reference to data pkg
    @return reference to the member IsoTerminalPkg_c, which encapsulates the CAN send structure
  */
  CanPkgExt_c& data(){return mc_data;};

  /** deliver reference to data pkg as reference to CanPkgExt_c
    to implement the base virtual function correct
  */
  virtual CanPkgExt_c& dataBase();


  VtClientServerCommunication_c& getClientByID (uint8_t ui8_clientIndex) { return *mvec_vtClientServerComm[ui8_clientIndex]; }

  VtClientServerCommunication_c* getClientPtrByID (uint8_t ui8_clientIndex) { return (!mvec_vtClientServerComm.empty()) ? mvec_vtClientServerComm[ui8_clientIndex] : NULL; }

  bool isAnyVtAvailable() { return !ml_vtServerInst.empty(); }

  ////////////////////////
  // INTERFACE FUNTIONS //
  ////////////////////////
// the following define should be globally defined in the project settings...
#ifdef FAKE_VT_PROPERTIES
  void fakeVtProperties (uint16_t aui16_dimension, uint16_t aui16_skWidth, uint16_t aui16_skHeight, uint8_t aui16_colorDepth, uint16_t aui16_fontSizes);
#endif

 ///  Used for Debugging Tasks in Scheduler_c
 virtual const char* getTaskName() const;

private:
  friend class SINGLETON_DERIVED(IsoTerminal_c, Scheduler_Task_c);
  /** private constructor which prevents direct instantiation in user application
    * NEVER define instance of IsoTerminal_c within application
    */
  IsoTerminal_c();

  /**
    initialize directly after the singleton instance is created.
    this is called from singleton.h and should NOT be called from the user again.
    users please use init(...) instead.
  */
  void singletonInit();

  /** this function is called by IsoMonitor_c on addition, state-change and removal of an IsoItem.
   * @param at_action enumeration indicating what happened to this IsoItem. @see IsoItemModification_en / IsoItemModification_t
   * @param acrc_isoItem reference to the (const) IsoItem which is changed (by existance or state)
   */
  virtual void reactOnIsoItemModification (IsoItemModification_t at_action, IsoItem_c const& acrc_isoItem);

  // helper function to shield removal access to vtCSC-list
  void deregisterIsoObjectPoolInd (uint8_t aui8_index);

protected:

private: // attributes

  /** temp data where received data is put */
  CanPkgExt_c mc_data;

  STL_NAMESPACE::list<VtServerInstance_c> ml_vtServerInst;

  STL_NAMESPACE::vector<VtClientServerCommunication_c*> mvec_vtClientServerComm;
};

#if defined(PRT_INSTANCE_CNT) && (PRT_INSTANCE_CNT > 1)
  /** C-style function, to get access to the unique Scheduler_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  IsoTerminal_c& getIsoTerminalInstance(uint8_t aui8_instance = 0);
#else
  /** C-style function, to get access to the unique Scheduler_c singleton instance */
  IsoTerminal_c& getIsoTerminalInstance(void);
#endif

/** this typedef is only for some time to provide backward compatibility at API level */
typedef IsoTerminal_c ISOTerminal_c;

}
#endif
