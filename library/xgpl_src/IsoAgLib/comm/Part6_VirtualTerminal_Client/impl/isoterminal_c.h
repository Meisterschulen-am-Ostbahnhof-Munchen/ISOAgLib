/*
  isoterminal_c.h: central ISO terminal management

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef ISO_TERMINAL_H
#define ISO_TERMINAL_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include <IsoAgLib/hal/hal_typedef.h>
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

  VtClientServerCommunication_c* initAndRegisterIsoObjectPoolForSlave (IdentItem_c& apc_slaveIdentItem, IsoAgLib::iIsoTerminalObjectPool_c& arc_pool);

  bool deregisterIsoObjectPool (IdentItem_c& apc_wsMasterIdentItem);

  /** periodically event -> call timeEvent for all  identities and parent objects
    @return true -> all planned activities performed in allowed time
  */
  bool timeEvent(void);

  /** @todo SOON-241 Remove this - this should only be temporary and a good solution needs to be implemented! */
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

  VtClientServerCommunication_c* initAndRegisterIsoObjectPoolCommon (IdentItem_c& rc_identItem, IsoAgLib::iIsoTerminalObjectPool_c& arc_pool, char* apc_versionLabel, bool ab_isSlave);

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
