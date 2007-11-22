/***************************************************************************
          vtserverinstance_c.h - class for wrapping one vtserver instance
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
#ifndef VT_SERVER_INSTANCE_H
#define VT_SERVER_INSTANCE_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include <IsoAgLib/comm/Part5_NetworkManagement/iidentitem_c.h>
#include "../ivttypes.h"

/** struct that stores the Language and Units information
  received from the LANGUAGE_PGN
*/
typedef struct localSettings_s{
  uint32_t lastReceived; /* Timestamp of last reception */
  uint32_t lastRequested;

  /* the following data is extracted from one "VT Status Message" */
  uint16_t languageCode; /* 2 ASCII chars packed into 16 bit! "en", "de", etc. */
  // Number Formats
  uint8_t  nDecimalPoint;
  uint8_t  nTimeFormat;
  // Date Formats
  uint8_t  dFormat;
  // Unit Formats
  uint8_t  uDistance;
  uint8_t  uArea;
  uint8_t  uVolume;
  uint8_t  uMass;
  uint8_t  uTemperature;
  uint8_t  uPressure;
  uint8_t  uForce;
  uint8_t  uUnitsSystem;
} localSettings_s;


// Begin Namespace __IsoAgLib
namespace __IsoAgLib {
//forward declarations
class IsoTerminal_c;
class VtClientServerCommunication_c;

/** class for wrapping one vtserver instance */
class VtServerInstance_c : public ClientBase
{
public:
  /** struct that stores the "Get Number Of Soft Keys Response",
    "Get Text Font Data Response" and "Get Hardware Response"
  */
  typedef struct vtCapabilities_s {
    uint32_t lastReceivedHardware;
    uint32_t lastRequestedHardware;
    uint8_t  hwGraphicType; // 0, 1 or 2 (2, 16 or 256 colors)
    uint8_t  hwHardware; // bit 0: touchscreen, bit 1: pointing device, bit 2: multiple freuency audio, bit 3: adjustable audio volume
    uint16_t hwWidth;   // hwWidth and
    uint16_t hwHeight;  // hwHeight have to be the same (only square mask dimension!)

    uint32_t lastReceivedFont;
    uint32_t lastRequestedFont;
    uint16_t fontSizes;
    uint8_t  fontTypes;

    uint32_t lastReceivedVersion;
    uint8_t  iso11783version;  // here the version number of the terminal gets stored as soon as the "VT Get Memory Response" was received

    uint32_t lastReceivedSoftkeys;
    uint32_t lastRequestedSoftkeys;
    uint8_t  skPhysical;
    uint8_t  skVirtual;
    uint8_t  skWidth;
    uint8_t  skHeight;
  } vtCapabilities_s;


  /** default destructor, which initiate sending address release for all own identities
  @see VtServerInstance_c::VtServerInstance_c
  */
  virtual ~VtServerInstance_c();

  /** check if there's already been at least one vt_statusMessage in the last 3 seconds
    @return true if at least one vt_statusMessage - false if there's not yet been one or the last one is more than 3 seconds old
  */
  bool isVtActive();

  /** setter */
  void setLatestVtStatusData();
  void setLocalSettings();
  void setVersion();
  void setSoftKeyData();
  void setTextFontData();
  void setHardwareData();
  void resetVtAlive();

  /** getter */
  const IsoName_c&           getIsoName()            { return mc_isoName; }
  uint8_t                    getVtSourceAddress()     { return (mcpc_isoItem != NULL)? mcpc_isoItem->nr() : 0xfe; }
  uint32_t                   getVtHardwareDimension();
  uint16_t                   getVtFontSizes();
  vtCapabilities_s*          getVtCapabilities()      { return &ms_vtCapabilitiesA; }
  const IsoAgLib::vtState_s* getVtState() const       { return &ms_vtStateA; }
  localSettings_s*           getLocalSettings()       { return &ms_localSettingsA; }
  const IsoItem_c*           getIsoItem()             { return mcpc_isoItem; }

// the following define should be globally defined in the project settings...
#ifdef FAKE_VT_PROPERTIES
  void fakeVtProperties (uint16_t aui16_dimension, uint16_t aui16_skWidth, uint16_t aui16_skHeight, uint8_t aui16_colorDepth, uint16_t aui16_fontSizes)
  {
    ms_vtCapabilitiesA.hwWidth = aui16_dimension;
    ms_vtCapabilitiesA.skWidth = aui16_skWidth;
    ms_vtCapabilitiesA.skHeight = aui16_skHeight;
    ms_vtCapabilitiesA.hwGraphicType = aui16_colorDepth;
    ms_vtCapabilitiesA.fontSizes = aui16_fontSizes;
  }
#endif

private:
  friend class IsoTerminal_c;
  /** private constructor which prevents direct instantiation in user application
    * NEVER define instance of IsoTerminal_c within application
    */
  VtServerInstance_c(const IsoItem_c& r_newItem, IsoName_c c_newISOName, IsoTerminal_c& r_isoTerminal SINGLETON_VEC_KEY_PARAMETER_DEF_WITH_COMMA);

private: // attributes
  const IsoItem_c* mcpc_isoItem;

  IsoName_c mc_isoName;

  IsoTerminal_c& mrc_isoTerminal; // back ref.

  /** stores the last "VT Status Message" */
  IsoAgLib::vtState_s ms_vtStateA;

  /** gets set as soon as the responses for the requests arrive */
  vtCapabilities_s ms_vtCapabilitiesA;

  /** stores the Local Settings like Language, Units, Date Format */
  localSettings_s ms_localSettingsA;
};

}
#endif
