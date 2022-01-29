/*
  vtobjectworkingset_c.h

  (C) Copyright 2009 - 2019 by OSB AG

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Usage under Commercial License:
  Licensees with a valid commercial license may use this file
  according to their commercial license agreement. (To obtain a
  commercial license contact OSB AG via <http://isoaglib.com/en/contact>)

  Usage under GNU General Public License with exceptions for ISOAgLib:
  Alternatively (if not holding a valid commercial license)
  use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef VTOBJECTWORKINGSET_C_H
#define VTOBJECTWORKINGSET_C_H

#include "vtobject_c.h"
#include "../ivtobjectmask_c.h"
#include "vtclient_c.h"


namespace __IsoAgLib {

class vtObjectWorkingSet_c : public vtObject_c
{
public:
  int16_t stream(uint8_t* destMemory,
                 uint16_t maxBytes,
                 objRange_t sourceOffset);


  void Append(iVtObject_c* const vtObject, int16_t x, int16_t y);


  iVtObjectWorkingSet_s* get_vtObjectWorkingSet_a() { return dynamic_cast<iVtObjectWorkingSet_s *>(&(get_vtObject_a())); }

  vtObjectWorkingSet_c(const iVtObjectWorkingSet_s* vtObjectWorkingSetSROM , int ai_multitonInst);

  uint32_t fitTerminal() const;

  void updateSelectable( uint8_t newSelectable );

    virtual void changeActiveMask(IsoAgLib::iVtObjectMask_c* apc_vtObjectMask, bool b_updateObject= false, bool b_enableReplaceOfCmd=false);

    virtual void changeBackgroundColour(uint8_t newColour, bool b_updateObject= false, bool b_enableReplaceOfCmd=false);

  void setOriginSKM(bool b_SKM);

  bool moveChildLocation(IsoAgLib::iVtObject_c* apc_childObject, int8_t dx, int8_t dy, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
  bool setChildPosition(IsoAgLib::iVtObject_c* apc_childObject, int16_t dx, int16_t dy, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);

    virtual bool controlAudioDevice (uint8_t aui8_repetitions, uint16_t aui16_frequency, uint16_t aui16_onTime, uint16_t aui16_offTime);

    virtual bool setAudioVolume (uint8_t aui8_volume);

    virtual bool setColourMapOrPalette (uint16_t aui16_objectId);

#ifdef USE_ISO_TERMINAL_GETATTRIBUTES
  /** these attributes are in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return 0; }
  uint8_t updateBackgroundColour(bool b_SendRequest=false);
  uint8_t updateSelectable(bool b_SendRequest=false);
  uint16_t updateActiveMask(bool b_SendRequest=false);
  */

  void saveReceivedAttribute (uint8_t attrID, uint8_t* pui8_attributeValue);
#endif
};

} // __IsoAgLib

#endif
