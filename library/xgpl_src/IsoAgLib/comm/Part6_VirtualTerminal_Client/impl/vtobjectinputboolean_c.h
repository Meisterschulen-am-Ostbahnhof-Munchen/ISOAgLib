/*
  vtobjectinputboolean_c.h

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
#ifndef VTOBJECTINPUTBOOLEAN_C_H
#define VTOBJECTINPUTBOOLEAN_C_H

#include <IsoAgLib/isoaglib_config.h>


#ifdef CONFIG_USE_VTOBJECT_inputboolean

#include "../ivtobjectfontattributes_c.h"
#include "../ivtobjectnumbervariable_c.h"
#include "vtobject_c.h"
#include "vtclient_c.h"


namespace __IsoAgLib {

class vtObjectInputBoolean_c : public vtObject_c
{
private:
	enum AttributeID:uint8_t;
	// Internal implementation class
	struct iVtObjectInputBoolean_s;

	// Pointer to the internal implementation
	std::unique_ptr<iVtObjectInputBoolean_s> vtObject_a;


	vtObjectInputBoolean_c() = delete;
	vtObjectInputBoolean_c(iVtObjectInputBoolean_s* vtObjectInputBooleanSROM , multiton ai_multitonInst);

public:
  int16_t stream(uint8_t* destMemory,
                 uint16_t maxBytes,
                 objRange_t sourceOffset);
  IsoAgLib::ObjectID getID() const;

  vtObjectInputBoolean_c(
		multiton ai_multitonInst,
		IsoAgLib::ObjectID ID,
		IsoAgLib::Colour backgroundColour,
		uint16_t width,
		IsoAgLib::iVtObjectFontAttributes_c *foregroundColour,
		IsoAgLib::iVtObjectNumberVariable_c *variableReference,
		uint8_t value,
		uint8_t enabled);


  ~vtObjectInputBoolean_c();


  uint32_t fitTerminal() const;

  void setValue(bool newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=true);
  // //////////////////////////////////
  // All special Attribute-Set methods
  void setBackgroundColour(IsoAgLib::Colour newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
  void setWidth(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
  void setForegroundColour(IsoAgLib::iVtObjectFontAttributes_c* newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
  void setVariableReference(IsoAgLib::iVtObjectNumberVariable_c* newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);

#ifdef CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES
  // ///////////////////////// getter for attributes
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return 7; }
  */

  IsoAgLib::Colour updateBackgroundColour(bool b_SendRequest=false);
  uint16_t updateWidth(bool b_SendRequest=false);
  IsoAgLib::iVtObjectFontAttributes_c* updateForegroundColour(bool b_SendRequest=false);
  IsoAgLib::iVtObjectNumberVariable_c* updateVariableReference(bool b_SendRequest=false);

  /** these attributes are in parentheses in the spec, so commented out here
  uint8_t updateValue(bool b_SendRequest=false);

  uint8_t updateEnabled(bool b_SendRequest=false);
  */

  void saveReceivedAttribute (uint8_t attrID, uint8_t* pui8_attributeValue);
#endif
private:
  void updateEnable(uint8_t aui8_enOrDis);
};

} // __IsoAgLib

#endif //CONFIG_USE_VTOBJECT_inputboolean
#endif //VTOBJECTINPUTBOOLEAN_C_H
