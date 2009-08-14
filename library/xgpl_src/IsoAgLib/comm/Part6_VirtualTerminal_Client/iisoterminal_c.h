/***************************************************************************
                          iisoterminal_c.h  - central ISO terminal management
                             -------------------
    begin                : Wed Jul 09 2003
    copyright            : (C) 2003 by Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
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
 * Copyright (C) 2003 - 2009 Dipl.-Inform. Achim Spangler                 *
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
  // Public methods
  /**
    initialise element which can't be done during construct

    possible errors:
  */
  void init ()
  { IsoTerminal_c::init(); }

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

/** this typedef is only for some time to provide backward compatibility at API level */
typedef iIsoTerminal_c iISOTerminal_c;

}
#endif
