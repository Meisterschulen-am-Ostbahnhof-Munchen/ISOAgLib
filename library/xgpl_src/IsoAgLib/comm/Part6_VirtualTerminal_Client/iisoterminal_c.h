/*
  iisoterminal_c.h: central ISO terminal management

  (C) Copyright 2009 - 2011 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef IISO_TERMINAL_H
#define IISO_TERMINAL_H

#include "impl/isoterminal_c.h"
#include "ivttypes.h"

// Begin Namespace IsoAgLib
namespace IsoAgLib {

class iScheduler_c;

/**
  central ISO11783 terminal management object

  For how to specify an ISO VT Object Pool please refer to \ref XMLspec .
  */
class iIsoTerminal_c : private __IsoAgLib::IsoTerminal_c {
public:

  /**
    register given object pool for uploading when possible.
    @param arc_wsMasterIdentItem pointer to an IdentItem_c instance for that the ISO_Terminal acts
    @param arc_pool
    @param apc_versionLabel
    <!--@param apc_allVtObjects pointer to a list which contains pointers to the objects that deinfe the object pool (and hence should be uploaded)
    @param aui32_numOfObjects amount of objects in the apc_allVtObjects list
    @param apc_versionLabel pointer to a 7-char name under which the object pool is loaded/stored (NULL for disabling non-volatile operation)
    @param apc_vtEventHandler pointer to an instance of IsoAgLib::iIsoTerminalEventHandler_c so the hook funtions for key activation, input values and successfull objectpool upload can be invoked-->
  */
  iVtClientServerCommunication_c* initAndRegisterIsoObjectPool (iIdentItem_c& arc_wsMasterIdentItem, iIsoTerminalObjectPool_c& arc_pool, char* apc_versionLabel)
  { return IsoTerminal_c::initAndRegisterIsoObjectPool (static_cast<__IsoAgLib::IdentItem_c&>(arc_wsMasterIdentItem), arc_pool, apc_versionLabel)->toInterfacePointer(); }

  /**
    initialize pool for slave but do no uploading
    @param arc_slaveIdentItem pointer to an IdentItem_c instance for that the ISO_Terminal acts
    @param arc_pool this pool does not need to have objects (slave can send object change commands via UIDs)
  */
  iVtClientServerCommunication_c* initAndRegisterIsoObjectPoolForSlave (iIdentItem_c& arc_slaveIdentItem, iIsoTerminalObjectPool_c& arc_pool)
  { return IsoTerminal_c::initAndRegisterIsoObjectPoolForSlave (static_cast<__IsoAgLib::IdentItem_c&>(arc_slaveIdentItem), arc_pool)->toInterfacePointer(); }

  bool deregisterIsoObjectPool (iIdentItem_c& arc_wsMasterIdentItem)
  { return IsoTerminal_c::deregisterIsoObjectPool (arc_wsMasterIdentItem); }

  iVtClientServerCommunication_c& getClientByID (uint8_t ui8_clientIndex)
  { return IsoTerminal_c::getClientByID (ui8_clientIndex).toInterfaceReference(); }

  iVtClientServerCommunication_c* getClientPtrByID (uint8_t ui8_clientIndex)
  { return IsoTerminal_c::getClientPtrByID (ui8_clientIndex)->toInterfacePointer(); }

  bool isAnyVtAvailable()
  { return IsoTerminal_c::isAnyVtAvailable(); }

// the following define should be globally defined in the project settings...
#ifdef FAKE_VT_PROPERTIES
  void fakeVtProperties (uint16_t aui16_dimension, uint16_t aui16_skWidth, uint16_t aui16_skHeight, uint8_t aui16_colorDepth, uint16_t aui16_fontSizes)
{ IsoTerminal_c::fakeVtProperties (aui16_dimension, aui16_skWidth, aui16_skHeight, aui16_colorDepth, aui16_fontSizes); }
#endif

 private:
  /** allow getIisoTerminalInstance() access to shielded base class.
      otherwise __IsoAgLib::getIsoTerminalInstance() wouldn't be accepted by compiler
    */
  #if defined(PRT_INSTANCE_CNT) && (PRT_INSTANCE_CNT > 1)
  friend iIsoTerminal_c& getIisoTerminalInstance (uint8_t aui8_instance);
  #else
  friend iIsoTerminal_c& getIisoTerminalInstance (void);
  #endif
  friend class iVtClientServerCommunication_c;
};

#if defined(PRT_INSTANCE_CNT) && (PRT_INSTANCE_CNT > 1)
  /** C-style function, to get access to the unique IsoTerminal_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  inline iIsoTerminal_c& getIisoTerminalInstance (uint8_t aui8_instance = 0)
  { return static_cast<iIsoTerminal_c&>(__IsoAgLib::getIsoTerminalInstance(aui8_instance)); }
#else
  /** C-style function, to get access to the unique Process_c singleton instance */
  inline iIsoTerminal_c& getIisoTerminalInstance (void)
  { return static_cast<iIsoTerminal_c&>(__IsoAgLib::getIsoTerminalInstance()); }
#endif


}
#endif
