/***************************************************************************
     vtserverinstance_c.cpp - class for wrapping one vtserver instance
                             -------------------
    begin                : Wed Jul 05 2006
    copyright            : (C) 2006 by Dipl.-Inf.(FH) Martina Winkler
    email                : m.winkler@osb-ag:de
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

// LOESCHE_POOL will send DeleteVersion INSTEAD of LoadVersion
//#define LOESCHE_POOL
//#define GET_VERSIONS
//#define USE_SIMPLE_AUX_RESPONSE

#include "vtserverinstance_c.h"

// #include <IsoAgLib/comm/Scheduler/impl/scheduler_c.h>
// #include <IsoAgLib/driver/can/impl/canio_c.h>
#include <IsoAgLib/hal/system.h>
// #include <IsoAgLib/comm/Multipacket/impl/multireceive_c.h>
// #include <supplementary_driver/driver/datastreams/volatilememory_c.h>
// #include <IsoAgLib/comm/SystemMgmt/ISO11783/impl/isomonitor_c.h>
#include "isoterminal_c.h"
// #include "vttypes.h"

#if defined(DEBUG) || defined(DEBUG_HEAP_USEAGE)
  #include <supplementary_driver/driver/rs232/impl/rs232io_c.h>
  #include <IsoAgLib/util/impl/util_funcs.h>
  #ifdef SYSTEM_PC
    #include <iostream>
  #else
    #include <supplementary_driver/driver/rs232/impl/rs232io_c.h>
  #endif
#endif


namespace __IsoAgLib {

  /****************************************/
 /*** VtServer Instance Implementation ***/
/****************************************/
/** default constructor, which can optional set the pointer to the containing
  Scheduler_c object instance
*/
VtServerInstance_c::VtServerInstance_c(const ISOItem_c& ref_newItem, DevKey_c c_newDevKey, ISOTerminal_c& ref_isoTerminal)
  : pcc_isoItem (&ref_newItem)
  , c_devKey (c_newDevKey)
  , refc_isoTerminal (ref_isoTerminal)
{
  /// init all variables to an initial upload state (Upload will not start before ws-announcing is due
  vtCapabilities_a.lastReceivedSoftkeys = 0; // not yet (queried and) got answer about vt's capabilities yet
  vtCapabilities_a.lastRequestedSoftkeys = 0; // not yet requested vt's capabilities yet
  vtCapabilities_a.lastReceivedHardware = 0; // not yet (queried and) got answer about vt's capabilities yet
  vtCapabilities_a.lastRequestedHardware = 0; // not yet requested vt's capabilities yet
  vtCapabilities_a.lastReceivedFont = 0; // not yet (queried and) got answer about vt's capabilities yet
  vtCapabilities_a.lastRequestedFont = 0; // not yet requested vt's capabilities yet
  vtCapabilities_a.lastReceivedVersion = 0; // interesting for NACK handling, that's why it's reset here!
  localSettings_a.lastRequested = 0; // no vt_statusMessage received yet
  localSettings_a.lastReceived = 0; // no language info received yet
}


/** default destructor, which initiate sending address release for all own identities
  @see VtServerInstance_c::~VtServerInstance_c
*/
VtServerInstance_c::~VtServerInstance_c()
{
}


/** call to check if at least one vt_statusMessage has arrived so we know if the terminal is there.
  @return true -> >= 1 vt_statusMessages have arrived -> terminal is there.
*/
bool
VtServerInstance_c::isVtActive ()
{
  if (vtState_a.lastReceived)
  {
    if (((int32_t)HAL::getTime() - (int32_t)vtState_a.lastReceived) <= 3000)
    { // comparing as int, so that in case "NOW-time > CAN-time" NO client-reload happens
      return true;
    }
    else
    {
      return false;
    }
  }
  return false;
}


/** process received vt status message
  @return true -> message was processed; else the received CAN message will be served to other matching CANCustomer_c
*/
void
VtServerInstance_c::setLatestVtStatusData()
{
  vtState_a.lastReceived = refc_isoTerminal.data().time();
  vtState_a.saOfActiveWorkingSetMaster = refc_isoTerminal.data().getUint8Data (1);
  vtState_a.dataAlarmMask = refc_isoTerminal.data().getUint8Data (2) | (refc_isoTerminal.data().getUint8Data (3) << 8);
  vtState_a.softKeyMask = refc_isoTerminal.data().getUint8Data (4) | (refc_isoTerminal.data().getUint8Data (5) << 8);
  vtState_a.busyCodes = refc_isoTerminal.data().getUint8Data (6);
  vtState_a.functionBusy = refc_isoTerminal.data().getUint8Data (7);
}



/** process received language messages
  @return true -> message was processed; else the received CAN message will be served to other matching CANCustomer_c
 */
void
VtServerInstance_c::setLocalSettings()
{
  localSettings_a.lastReceived =  refc_isoTerminal.data().time();
  localSettings_a.languageCode = (refc_isoTerminal.data().getUint8Data (0) << 8) | refc_isoTerminal.data().getUint8Data (1);
  localSettings_a.nDecimalPoint = refc_isoTerminal.data().getUint8Data (2) >> 6;
  localSettings_a.nTimeFormat =  (refc_isoTerminal.data().getUint8Data (2) >> 4) & 0x03;
  localSettings_a.dFormat =       refc_isoTerminal.data().getUint8Data (3);
  localSettings_a.uDistance =     refc_isoTerminal.data().getUint8Data (4) >> 6;
  localSettings_a.uArea =        (refc_isoTerminal.data().getUint8Data (4) >> 4) & 0x03;
  localSettings_a.uVolume =      (refc_isoTerminal.data().getUint8Data (4) >> 2) & 0x03;
  localSettings_a.uMass =         refc_isoTerminal.data().getUint8Data (4)       & 0x03;
  localSettings_a.uTemperature =  refc_isoTerminal.data().getUint8Data (5) >> 6;
  localSettings_a.uPressure =    (refc_isoTerminal.data().getUint8Data (5) >> 4) & 0x03;
  localSettings_a.uForce =       (refc_isoTerminal.data().getUint8Data (5) >> 2) & 0x03;
  localSettings_a.uUnitsSystem =  refc_isoTerminal.data().getUint8Data (5)       & 0x03;
  // The other fields are reserved. (yet ;-)
}


void
VtServerInstance_c::setVersion()
{
  vtCapabilities_a.lastReceivedVersion = HAL::getTime();
  vtCapabilities_a.iso11783version = refc_isoTerminal.data().getUint8Data (1);
}


void
VtServerInstance_c::setSoftKeyData()
{
  vtCapabilities_a.lastReceivedSoftkeys = HAL::getTime();
  vtCapabilities_a.skWidth = refc_isoTerminal.data().getUint8Data (4);
  vtCapabilities_a.skHeight = refc_isoTerminal.data().getUint8Data (5);
  vtCapabilities_a.skVirtual = refc_isoTerminal.data().getUint8Data (6);
  vtCapabilities_a.skPhysical = refc_isoTerminal.data().getUint8Data (7);
}


void
VtServerInstance_c::setTextFontData()
{
  vtCapabilities_a.lastReceivedFont = HAL::getTime();
  vtCapabilities_a.fontSizes = (refc_isoTerminal.data().getUint8Data (5) << 1) | 0x01;
  vtCapabilities_a.fontSizes += refc_isoTerminal.data().getUint8Data (6) << 8;
  vtCapabilities_a.fontTypes = refc_isoTerminal.data().getUint8Data (7);
}


void
VtServerInstance_c::setHardwareData()
{
  vtCapabilities_a.lastReceivedHardware = HAL::getTime();
  vtCapabilities_a.hwGraphicType = refc_isoTerminal.data().getUint8Data (2);
  vtCapabilities_a.hwHardware = refc_isoTerminal.data().getUint8Data (3);
  vtCapabilities_a.hwWidth = refc_isoTerminal.data().getUint8Data (4) + (refc_isoTerminal.data().getUint8Data (5) << 8);
  vtCapabilities_a.hwHeight = refc_isoTerminal.data().getUint8Data (6) + (refc_isoTerminal.data().getUint8Data (7) << 8);
}


void
VtServerInstance_c::resetVtAlive()
{
  vtState_a.lastReceived = 0;
}

} // end namespace __IsoAgLib
