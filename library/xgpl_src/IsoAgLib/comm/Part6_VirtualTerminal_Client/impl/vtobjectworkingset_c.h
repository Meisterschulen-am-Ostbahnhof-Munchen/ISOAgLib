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



#include <memory> // PImpl
#include "vtobject_c.h"
#include "../ivtobjectmask_c.h"
#include "vtclient_c.h"


namespace __IsoAgLib {

class vtObjectWorkingSet_c : public vtObject_c
{
private:
	// Internal implementation class
	struct iVtObjectWorkingSet_s;

	// Pointer to the internal implementation
	std::unique_ptr<iVtObjectWorkingSet_s> vtObject_a;


public:
  uint16_t stream(uint8_t* destMemory,
                 uint16_t maxBytes,
                 objRange_t sourceOffset) override;
  IsoAgLib::ObjectID getID() const override;


  uint8_t               getNumLang()        const;
  const IsoAgLib::repeat_vtLanguage_s getLang(uint8_t lang)        const;

  void Append(iVtObject_c* vtObject, int16_t x, int16_t y);






  vtObjectWorkingSet_c(
		multiton ai_multitonInst,
		IsoAgLib::ObjectID ID,
		IsoAgLib::Colour backgroundColour,
  		uint8_t selectable,
		IsoAgLib::iVtObjectMask_c* activeMask // data or alarm mask
  		);

  vtObjectWorkingSet_c(iVtObjectWorkingSet_s* vtObjectWorkingSetSROM , multiton ai_multitonInst);

  vtObjectWorkingSet_c() = delete;

  ~vtObjectWorkingSet_c() override;

  uint32_t fitTerminal() const override;

    virtual void updateSelectable( uint8_t newSelectable );
    virtual void changeActiveMask(IsoAgLib::iVtObjectMask_c* apc_vtObjectMask, bool b_updateObject= false, bool b_enableReplaceOfCmd=false);
    virtual void setBackgroundColour(IsoAgLib::Colour newColour, bool b_updateObject= false, bool b_enableReplaceOfCmd=false);
  void setOriginSKM(bool b_SKM) override;

    virtual bool moveChildLocation(IsoAgLib::iVtObject_c* apc_childObject, int8_t dx, int8_t dy, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);

    virtual bool setChildPosition(IsoAgLib::iVtObject_c* apc_childObject, int16_t dx, int16_t dy, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
    virtual bool controlAudioDevice (uint8_t aui8_repetitions, uint16_t aui16_frequency, uint16_t aui16_onTime, uint16_t aui16_offTime);
    virtual bool setAudioVolume (uint8_t aui8_volume);
    virtual bool setColourMapOrPalette (uint16_t aui16_objectId);
#ifdef CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES
  /** these attributes are in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return 0; }
  IsoAgLib::Colour updateBackgroundColour(bool b_SendRequest=false);
  uint8_t updateSelectable(bool b_SendRequest=false);
  uint16_t updateActiveMask(bool b_SendRequest=false);
  */

  void saveReceivedAttribute (uint8_t attrID, uint8_t* pui8_attributeValue) override;
#endif
};

} // __IsoAgLib

#endif
