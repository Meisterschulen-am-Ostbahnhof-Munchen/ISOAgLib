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

	~iVtObjectOutputNumber_c();

	iVtObjectOutputNumber_c(
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



  void setValue(uint32_t newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=true);
  void setWidth(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
  void setHeight(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
  void setBackgroundColour(Colour newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
  void setFontAttributes(iVtObjectFontAttributes_c* newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
  void setOptions(iVtObjectNumberOptions newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
  void setVariableReference(iVtObjectNumberVariable_c* newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
  void setOffset(int32_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
  void setScale(float newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
  void setNumberOfDecimals(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
  void setFormat(bool newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
  void setJustification(Justification newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
  void setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);


#ifdef CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES  
  // ///////////////////////// getter for attributes
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return vtObjectOutputNumber_c::updateObjectType(); }
  */
  
  uint16_t updateWidth(bool b_SendRequest=false);
  uint16_t updateHeight(bool b_SendRequest=false);
  Colour updateBackgroundColour(bool b_SendRequest=false);
  iVtObjectFontAttributes_c* updateFontAttributes(bool b_SendRequest=false);

  iVtObjectNumberOptions updateOptions(bool b_SendRequest=false) {
    return vtObjectOutputNumber_c::updateOptions(b_SendRequest);
  }

  iVtObjectNumberVariable_c * updateVariableReference(bool b_SendRequest=false) {
    return vtObjectOutputNumber_c::updateVariableReference(b_SendRequest);
  }

  int32_t updateOffset(bool b_SendRequest=false) {
    return vtObjectOutputNumber_c::updateOffset(b_SendRequest);
  }

  float updateScale(bool b_SendRequest=false) {
    return vtObjectOutputNumber_c::updateScale(b_SendRequest);
  }

  uint8_t updateNumberOfDecimals(bool b_SendRequest=false) {
    return vtObjectOutputNumber_c::updateNumberOfDecimals(b_SendRequest);
  }

  uint8_t updateFormat(bool b_SendRequest=false) {
    return vtObjectOutputNumber_c::updateFormat(b_SendRequest);
  }

  IsoAgLib::Justification updateJustification(bool b_SendRequest=false) {
    return vtObjectOutputNumber_c::updateJustification(b_SendRequest);
  }

  /** that attribute is in parentheses in the spec, so commented out here
  uint32_t updateValue(bool b_SendRequest=false) {
    return vtObjectOutputNumber_c::getValue(b_SendRequest);
  }
  */
#endif

  virtual ObjectType getObjectType() const;
};

} // IsoAgLib

#endif //CONFIG_USE_VTOBJECT_outputnumber

#endif //IVTOBJECTOUTPUTNUMBER_C_H
