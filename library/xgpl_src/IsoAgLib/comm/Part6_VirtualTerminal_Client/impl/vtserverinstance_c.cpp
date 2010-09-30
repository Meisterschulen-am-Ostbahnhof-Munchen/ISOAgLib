/*
  vtserverinstance_c.cpp: class for wrapping one vtserver instance

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include "vtserverinstance_c.h"
// necessary for interface convert operators ("up"-cast)
#include "../ivtserverinstance_c.h"

#include <IsoAgLib/hal/hal_system.h>
#include <IsoAgLib/driver/can/impl/canio_c.h>
#include "isoterminal_c.h"

#if DEBUG_HEAP_USEAGE
  #include <supplementary_driver/driver/rs232/impl/rs232io_c.h>
  #include <IsoAgLib/util/impl/util_funcs.h>
  #ifdef SYSTEM_PC
    #include <iostream>
  #else
    #include <supplementary_driver/driver/rs232/impl/rs232io_c.h>
  #endif
#endif


namespace __IsoAgLib {

/** default constructor, which can optional set the pointer to the containing
  Scheduler_c object instance
*/
VtServerInstance_c::VtServerInstance_c(const IsoItem_c& r_newItem, IsoName_c c_newISOName, IsoTerminal_c& r_isoTerminal SINGLETON_VEC_KEY_PARAMETER_DEF_WITH_COMMA)
  : ClientBase(SINGLETON_VEC_KEY_PARAMETER_USE)
  , mcpc_isoItem (&r_newItem)
  , mc_isoName (c_newISOName)
  , mrc_isoTerminal (r_isoTerminal)
{
  /// init all variables to an initial upload state (Upload will not start before ws-announcing is due
  ms_vtCapabilitiesA.lastReceivedSoftkeys = 0; // not yet (queried and) got answer about vt's capabilities yet
  ms_vtCapabilitiesA.lastRequestedSoftkeys = 0; // not yet requested vt's capabilities yet
  ms_vtCapabilitiesA.lastReceivedHardware = 0; // not yet (queried and) got answer about vt's capabilities yet
  ms_vtCapabilitiesA.lastRequestedHardware = 0; // not yet requested vt's capabilities yet
  ms_vtCapabilitiesA.lastReceivedFont = 0; // not yet (queried and) got answer about vt's capabilities yet
  ms_vtCapabilitiesA.lastRequestedFont = 0; // not yet requested vt's capabilities yet
  ms_vtCapabilitiesA.lastReceivedVersion = 0; // interesting for NACK handling, that's why it's reset here!
  ms_localSettingsA.lastRequested = 0; // no language info requested yet
  ms_localSettingsA.lastReceived = 0; // no language info received yet
  ms_vtStateA.lastReceived = 0; // no vt_statusMessage received yet
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
VtServerInstance_c::isVtActive () const
{
  if (ms_vtStateA.lastReceived)
  {
    // Using the current time for comparison checks currently, because we don't have the information
    // about the time when the last can-msg was processed or checked for (in case there's none).
    // Note: Formerly here was a check using "getIsoBusInstance4Comm().getLastProcessedCanPkgTime()",
    //       but that is only the time when a msg was really processed - so in bus of e.g. a bus
    //       disconnection a VT-off wouldn't be detected, which is a safety issue of course!
    if ((System_c::getTime() - (int32_t)ms_vtStateA.lastReceived) <= 3000)
    { // comparing as int, so that in case "NOW-time > CAN-time" NO client-reload happens
      return true;
    }
  }
  return false;
}


/** process received vt status message
  @return true -> message was processed; else the received CAN message will be served to other matching CanCustomer_c
*/
void
VtServerInstance_c::setLatestVtStatusData()
{
  ms_vtStateA.lastReceived =               mrc_isoTerminal.data().time();
  ms_vtStateA.saOfActiveWorkingSetMaster = mrc_isoTerminal.data().getUint8Data (1);
  ms_vtStateA.dataAlarmMask =              mrc_isoTerminal.data().getUint8Data (2) | (mrc_isoTerminal.data().getUint8Data (3) << 8);
  ms_vtStateA.softKeyMask =                mrc_isoTerminal.data().getUint8Data (4) | (mrc_isoTerminal.data().getUint8Data (5) << 8);
  ms_vtStateA.busyCodes =                  mrc_isoTerminal.data().getUint8Data (6);
  ms_vtStateA.functionBusy =               mrc_isoTerminal.data().getUint8Data (7);
}


/** process received language messages
  @return true -> message was processed; else the received CAN message will be served to other matching CanCustomer_c
 */
void
VtServerInstance_c::setLocalSettings()
{
  ms_localSettingsA.lastReceived =  mrc_isoTerminal.data().time();
  ms_localSettingsA.languageCode = (mrc_isoTerminal.data().getUint8Data (0) << 8) | mrc_isoTerminal.data().getUint8Data (1);
  ms_localSettingsA.nDecimalPoint = mrc_isoTerminal.data().getUint8Data (2) >> 6;
  ms_localSettingsA.nTimeFormat =  (mrc_isoTerminal.data().getUint8Data (2) >> 4) & 0x03;
  ms_localSettingsA.dFormat =       mrc_isoTerminal.data().getUint8Data (3);
  ms_localSettingsA.uDistance =     mrc_isoTerminal.data().getUint8Data (4) >> 6;
  ms_localSettingsA.uArea =        (mrc_isoTerminal.data().getUint8Data (4) >> 4) & 0x03;
  ms_localSettingsA.uVolume =      (mrc_isoTerminal.data().getUint8Data (4) >> 2) & 0x03;
  ms_localSettingsA.uMass =         mrc_isoTerminal.data().getUint8Data (4)       & 0x03;
  ms_localSettingsA.uTemperature =  mrc_isoTerminal.data().getUint8Data (5) >> 6;
  ms_localSettingsA.uPressure =    (mrc_isoTerminal.data().getUint8Data (5) >> 4) & 0x03;
  ms_localSettingsA.uForce =       (mrc_isoTerminal.data().getUint8Data (5) >> 2) & 0x03;
  ms_localSettingsA.uUnitsSystem =  mrc_isoTerminal.data().getUint8Data (5)       & 0x03;
  // The other fields are reserved. (yet ;-)
}


void
VtServerInstance_c::setVersion()
{
  ms_vtCapabilitiesA.lastReceivedVersion =  mrc_isoTerminal.data().time();
  ms_vtCapabilitiesA.iso11783version =      mrc_isoTerminal.data().getUint8Data (1);
}


void
VtServerInstance_c::setSoftKeyData()
{
  ms_vtCapabilitiesA.lastReceivedSoftkeys = mrc_isoTerminal.data().time();
  ms_vtCapabilitiesA.skWidth =              mrc_isoTerminal.data().getUint8Data (4);
  ms_vtCapabilitiesA.skHeight =             mrc_isoTerminal.data().getUint8Data (5);
  ms_vtCapabilitiesA.skVirtual =            mrc_isoTerminal.data().getUint8Data (6);
  ms_vtCapabilitiesA.skPhysical =           mrc_isoTerminal.data().getUint8Data (7);
}


void
VtServerInstance_c::setTextFontData()
{
  ms_vtCapabilitiesA.lastReceivedFont =     mrc_isoTerminal.data().time();
  ms_vtCapabilitiesA.fontSizes =           (mrc_isoTerminal.data().getUint8Data (5) << 1) | 0x01;
  ms_vtCapabilitiesA.fontSizes +=           mrc_isoTerminal.data().getUint8Data (6) << 8;
  ms_vtCapabilitiesA.fontTypes =            mrc_isoTerminal.data().getUint8Data (7);
}


void
VtServerInstance_c::setHardwareData()
{
  ms_vtCapabilitiesA.lastReceivedHardware = mrc_isoTerminal.data().time();
  ms_vtCapabilitiesA.hwGraphicType =        mrc_isoTerminal.data().getUint8Data (2);
  ms_vtCapabilitiesA.hwHardware =           mrc_isoTerminal.data().getUint8Data (3);
  ms_vtCapabilitiesA.hwWidth =              mrc_isoTerminal.data().getUint8Data (4) + (mrc_isoTerminal.data().getUint8Data (5) << 8);
  ms_vtCapabilitiesA.hwHeight =             mrc_isoTerminal.data().getUint8Data (6) + (mrc_isoTerminal.data().getUint8Data (7) << 8);
}


uint32_t
VtServerInstance_c::getVtHardwareDimension() const
{
  return (uint32_t) (ms_vtCapabilitiesA.hwWidth);
}

uint16_t
VtServerInstance_c::getVtFontSizes() const
{
  return ms_vtCapabilitiesA.fontSizes;
}


uint8_t
VtServerInstance_c::getVtIsoVersion() const
{
  return ms_vtCapabilitiesA.iso11783version;
}


IsoAgLib::iVtServerInstance_c*
VtServerInstance_c::toIvtServerInstancePtr_c()
{
  return static_cast<IsoAgLib::iVtServerInstance_c*>(this);
}


IsoAgLib::iVtServerInstance_c&
VtServerInstance_c::toIvtServerInstance_c()
{
  return static_cast<IsoAgLib::iVtServerInstance_c&>(*this);
}

} // end namespace __IsoAgLib
