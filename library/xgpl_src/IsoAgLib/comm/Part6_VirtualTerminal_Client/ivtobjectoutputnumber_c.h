/*
  ivtobjectoutputnumber_c.h

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
#ifndef IVTOBJECTOUTPUTNUMBER_C_H
#define IVTOBJECTOUTPUTNUMBER_C_H

#include "impl/vtobjectoutputnumber_c.h"


#ifdef CONFIG_USE_VTOBJECT_outputnumber

#include "ivtobjectfontattributes_c.h"

namespace IsoAgLib {

class iVtObjectOutputNumber_c : public __IsoAgLib::vtObjectOutputNumber_c
{
public:


	iVtObjectOutputNumber_c() = delete;

	~iVtObjectOutputNumber_c() override;

	explicit iVtObjectOutputNumber_c(
		iVtClientObjectPool_c* pool,
		ObjectID ID = autoID,
		uint16_t width = 100,
		uint16_t height = 25,
		Colour backgroundColour = WHITE,
		iVtObjectFontAttributes_c *fontAttributes = nullptr,
		iVtObjectNumberOptions options = iVtObjectNumberOptions(ioallOptionsOff),
		iVtObjectNumberVariable_c *variableReference = nullptr,
		uint32_t value = 9,
		int32_t offset = 0,
		float scale = 1.0,
		uint8_t numberOfDecimals = 1,
		uint8_t format = 0,
		Justification justification = Justification(TopLeft));



  static ObjectType objectType();



  void setValue(uint32_t newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=true) override;
  void setWidth(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) override;
  void setHeight(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) override;
  void setBackgroundColour(Colour newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) override;
  void setFontAttributes(iVtObjectFontAttributes_c* newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) override;
  void setOptions(iVtObjectNumberOptions newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) override;
  void setVariableReference(iVtObjectNumberVariable_c* newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) override;
  void setOffset(int32_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) override;
  void setScale(float newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) override;
  void setNumberOfDecimals(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) override;
  void setFormat(bool newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) override;
  void setJustification(Justification newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) override;
  void setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) override;


#ifdef CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES  
  // ///////////////////////// getter for attributes
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return vtObjectOutputNumber_c::updateObjectType(); }
  */
  
  uint16_t updateWidth(bool b_SendRequest=false) override;
  uint16_t updateHeight(bool b_SendRequest=false) override;
  Colour updateBackgroundColour(bool b_SendRequest=false) override;
  iVtObjectFontAttributes_c* updateFontAttributes(bool b_SendRequest=false) override;
  iVtObjectNumberOptions updateOptions(bool b_SendRequest=false) override;
  iVtObjectNumberVariable_c * updateVariableReference(bool b_SendRequest=false) override;
  int32_t updateOffset(bool b_SendRequest=false) override;
  float updateScale(bool b_SendRequest=false) override;
  uint8_t updateNumberOfDecimals(bool b_SendRequest=false) override;
  uint8_t updateFormat(bool b_SendRequest=false) override;
  IsoAgLib::Justification updateJustification(bool b_SendRequest=false) override;

  /** that attribute is in parentheses in the spec, so commented out here
  uint32_t updateValue(bool b_SendRequest=false) {
    return vtObjectOutputNumber_c::getValue(b_SendRequest);
  }
  */
#endif
  ObjectType getObjectType() const override;
};

} // IsoAgLib

#endif //CONFIG_USE_VTOBJECT_outputnumber

#endif //IVTOBJECTOUTPUTNUMBER_C_H
