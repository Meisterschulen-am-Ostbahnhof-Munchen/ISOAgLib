/*
  vtobjectfillattributes_c.h

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
#ifndef VTOBJECTFILLATTRIBUTES_C_H
#define VTOBJECTFILLATTRIBUTES_C_H

#include "vtobject_c.h"
#include "../ivtobjectpicturegraphic_c.h"


namespace __IsoAgLib {

class vtObjectFillAttributes_c : public vtObject_c
{
private:
	// Internal implementation class
	struct iVtObjectFillAttributes_s;

	// Pointer to the internal implementation
	iVtObjectFillAttributes_s* vtObject_a;
	//TODO
	//std::unique_ptr<iVtObjectFillAttributes_s> vtObject_a;


public:
  int16_t stream(uint8_t* destMemory,
                 uint16_t maxBytes,
                 objRange_t sourceOffset);
  IsoAgLib::ObjectID getID() const;

  vtObjectFillAttributes_c(iVtObjectFillAttributes_s* vtObjectFillAttributesSROM , int ai_multitonInst);
  iVtObjectFillAttributes_s* get_vtObjectFillAttributes_a();
  uint32_t fitTerminal() const;
  void setFillType(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
  void setFillColour(IsoAgLib::Colour newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
  void setFillPattern(IsoAgLib::iVtObjectPictureGraphic_c* newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
  void setFillAttributes(uint8_t newFillType, IsoAgLib::Colour newFillColour, IsoAgLib::iVtObjectPictureGraphic_c* newFillPattern, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);

#ifdef CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return 25; }
  */

  uint8_t updateFillType(bool b_SendRequest=false);

  uint8_t updateFillColour(bool b_SendRequest=false);

  uint16_t updateFillPattern(bool b_SendRequest=false);

  void saveReceivedAttribute (uint8_t attrID, uint8_t* pui8_attributeValue);
#endif
};

} // __IsoAgLib

#endif
