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
 * Copyright (C) 2003 - 2004 Dipl.-Inform. Achim Spangler                 *
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
class iISOTerminal_c : private __IsoAgLib::ISOTerminal_c {
public:
  // Public methods
  /**
    initialise element which can't be done during construct

    possible errors:
  */
  void init ()
  { ISOTerminal_c::init(); }

  /**
    register given object pool for uploading when possible.
    @param rpc_wsMasterIdentItem pointer to an IdentItem_c instance for that the ISO_Terminal acts
    @param rui32_vtObjectPoolDimension the dimension of the defined masks
    @param rpc_allVtObjects pointer to a list which contains pointers to the objects that deinfe the object pool (and hence should be uploaded)
    @param rui32_numOfObjects amount of objects in the rpc_allVtObjects list
    @param rpc_versionLabel pointer to a 7-char name under which the object pool is loaded/stored (NULL for disabling non-volatile operation)
    @param rpc_vtEventHandler pointer to an instance of IsoAgLib::iIsoTerminalEventHandler_c so the hook funtions for key activation, input values and successfull objectpool upload can be invoked
  */
  bool registerIsoObjectPool (iIdentItem_c* rpc_wsMasterIdentItem, iIsoTerminalObjectPool_c* rpc_pool, char* rpc_versionLabel)
  { return ISOTerminal_c::registerIsoObjectPool (static_cast<__IsoAgLib::IdentItem_c&>(*rpc_wsMasterIdentItem), *rpc_pool, rpc_versionLabel); }

  bool deregisterIsoObjectPool (iIdentItem_c* rpc_wsMasterIdentItem)
  { return ISOTerminal_c::deregisterIsoObjectPool (*rpc_wsMasterIdentItem); }

  __IsoAgLib::VtClientServerCommunication_c& getClientByID (uint8_t ui8_clientIndex)
  { return ISOTerminal_c::getClientByID (ui8_clientIndex); }

  __IsoAgLib::VtClientServerCommunication_c* getClientPtrByID (uint8_t ui8_clientIndex)
  { return ISOTerminal_c::getClientPtrByID (ui8_clientIndex); }

  bool isAnyVtAvailable()
  { return ISOTerminal_c::isAnyVtAvailable(); }

// the following define should be globally defined in the project settings...
#ifdef FAKE_VT_PROPERTIES
  void fakeVtProperties (uint16_t rui16_dimension, uint16_t rui16_skWidth, uint16_t rui16_skHeight, uint8_t rui16_colorDepth, uint16_t rui16_fontSizes)
{ ISOTerminal_c::fakeVtProperties (rui16_dimension, rui16_skWidth, rui16_skHeight, rui16_colorDepth, rui16_fontSizes); }
#endif

 private:
  /** allow getIisoTerminalInstance() access to shielded base class.
      otherwise __IsoAgLib::getIsoTerminalInstance() wouldn't be accepted by compiler
    */
  #if defined(PRT_INSTANCE_CNT) && (PRT_INSTANCE_CNT > 1)
  friend iISOTerminal_c& getIisoTerminalInstance (uint8_t rui8_instance);
  #else
  friend iISOTerminal_c& getIisoTerminalInstance (void);
  #endif
};

#if defined(PRT_INSTANCE_CNT) && (PRT_INSTANCE_CNT > 1)
  /** C-style function, to get access to the unique ISOTerminal_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  inline iISOTerminal_c& getIisoTerminalInstance (uint8_t rui8_instance = 0)
  { return static_cast<iISOTerminal_c&>(__IsoAgLib::getIsoTerminalInstance(rui8_instance)); }
#else
  /** C-style function, to get access to the unique Process_c singleton instance */
  inline iISOTerminal_c& getIisoTerminalInstance (void)
  { return static_cast<iISOTerminal_c&>(__IsoAgLib::getIsoTerminalInstance()); }
#endif

}
#endif
