/*
  vtobjectinputnumber_c.h

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
#ifndef VTOBJECTINPUTNUMBER_C_H
#define VTOBJECTINPUTNUMBER_C_H

#include <IsoAgLib/isoaglib_config.h>


#ifdef CONFIG_USE_VTOBJECT_inputnumber

#include "ivtobjectfontattributes_c.h"
#include "vtobject_c.h"
#include "vtclientconnection_c.h"
#include "vtclient_c.h"


namespace __IsoAgLib {

class vtObjectInputNumber_c : public vtObject_c
{
private:
	enum AttributeID:uint8_t;
	// Internal implementation class
	struct iVtObjectInputNumber_s;

	// Pointer to the internal implementation
	std::unique_ptr<iVtObjectInputNumber_s> vtObject_a;

	vtObjectInputNumber_c() = delete;
	vtObjectInputNumber_c(iVtObjectInputNumber_s* vtObjectInputNumberSROM , multiton ai_multitonInst);

public:
  int16_t stream(uint8_t* destMemory,
                 uint16_t maxBytes,
                 objRange_t sourceOffset);
  IsoAgLib::ObjectID getID() const;


  vtObjectInputNumber_c(
		    multiton ai_multitonInst,
			IsoAgLib::ObjectID ID,
			uint16_t width,
			uint16_t height,
			IsoAgLib::Colour backgroundColour,
			IsoAgLib::iVtObjectFontAttributes_c *fontAttributes,
			IsoAgLib::iVtObjectInputNumberOptions options,
			IsoAgLib::iVtObjectNumberVariable_c *variableReference,
			uint32_t value,
			uint32_t minValue,
			uint32_t maxValue,
			int32_t offset,
			float scale,
			uint8_t numberOfDecimals,
			uint8_t format,
			uint8_t horizontalJustification,
			IsoAgLib::iVtObjectInputNumberOptions2 secondOptionsByte);


  ~vtObjectInputNumber_c() override;

  uint32_t fitTerminal() const;
  void setValue(uint32_t newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=true);

  // //////////////////////////////////
  // All special Attribute-Set methods
  void setWidth(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
  void setHeight(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
  void setBackgroundColour(IsoAgLib::Colour newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
  void setFontAttributes(IsoAgLib::iVtObjectFontAttributes_c* newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
  void setOptions(IsoAgLib::iVtObjectInputNumberOptions newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
  void setVariableReference(IsoAgLib::iVtObjectNumberVariable_c* newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
  void setMinValue(uint32_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
  void setMaxValue(uint32_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
  void setOffset(int32_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
  void setScale(float newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
  void setNumberOfDecimals(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
  void setFormat(bool newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
  void setHorizontalJustification(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
  void setSecondOptionsByte(IsoAgLib::iVtObjectInputNumberOptions2 newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
  void setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);

#ifdef CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES
  // ///////////////////////// getter for attributes
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return 9; }
   */

  uint16_t updateWidth(bool b_SendRequest=false);
  uint16_t updateHeight(bool b_SendRequest=false);
  IsoAgLib::Colour updateBackgroundColour(bool b_SendRequest=false);
  IsoAgLib::iVtObjectFontAttributes_c* updateFontAttributes(bool b_SendRequest=false);
  IsoAgLib::iVtObjectInputNumberOptions updateOptions(bool b_SendRequest=false);
  IsoAgLib::iVtObjectNumberVariable_c* updateVariableReference(bool b_SendRequest=false);
  uint32_t updateMinValue(bool b_SendRequest=false);
  uint32_t updateMaxValue(bool b_SendRequest=false);
  int32_t updateOffset(bool b_SendRequest=false);
  float updateScale(bool b_SendRequest=false);
  uint8_t updateNumberOfDecimals(bool b_SendRequest=false);
  uint8_t updateFormat(bool b_SendRequest=false);
  uint8_t updateJustification(bool b_SendRequest=false);

  /** these attributes are in parentheses in the spec, so commented out here
  uint32_t updateValue(bool b_SendRequest=false);

  uint8_t update2ndOptionsByte(bool b_SendRequest=false);
  */

  void saveReceivedAttribute (uint8_t attrID, uint8_t* pui8_attributeValue);
#endif
private:
  void updateEnable(IsoAgLib::Enabled aui8_enOrDis);
};

} // __IsoAgLib

#endif // CONFIG_USE_VTOBJECT_inputnumber

#endif // VTOBJECTINPUTNUMBER_C_H
