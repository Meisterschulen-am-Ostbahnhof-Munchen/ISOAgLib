/*
  ivtobjectbutton_c.h

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
#ifndef IVTOBJECTBUTTON_C_H
#define IVTOBJECTBUTTON_C_H

#include "impl/vtobjectbutton_c.h"


namespace IsoAgLib {

class iVtObjectButton_c : public __IsoAgLib::vtObjectButton_c
{
public:
  static uint16_t objectType() { return VT_OBJECT_TYPE_BUTTON; }

  iVtObjectButton_c(){}
  iVtObjectButton_c(const iVtObjectButton_s* vtObjectButtonSROM , int ai_multitonInst)
  :vtObjectButton_c(vtObjectButtonSROM , ai_multitonInst)
  {}


  iVtObjectButton_c(
		int ai_multitonInst,
  		uint16_t ID = 0,
  	    uint16_t width = 80,
  	    uint16_t height = 30,
  	    uint8_t backgroundColour = 12,
  	    uint8_t borderColour = 8,
  	    uint8_t keyCode = 251,
  	    uint8_t options = 0,
  	    uint8_t numberOfObjectsToFollow = 0,
  	    const repeat_iVtObject_x_y_iVtObjectFontAttributes_row_col_s* objectsToFollow = nullptr,
  	    uint8_t numberOfMacrosToFollow = 0,
  	    const repeat_event_iVtObjectMacro_s* macrosToFollow = nullptr
  		)
  : vtObjectButton_c(
		  new iVtObjectButton_s(
				    ID,
			  	    width,
			  	    height,
			  	    backgroundColour,
			  	    borderColour,
			  	    keyCode,
			  	    options,
			  	    numberOfObjectsToFollow,
			  	    objectsToFollow,
			  	    numberOfMacrosToFollow,
			  	    macrosToFollow)
                    , ai_multitonInst)
  {}


  void init(const iVtObjectButton_s* vtObjectButtonSROM , int ai_multitonInst) {
    vtObjectButton_c::init (vtObjectButtonSROM , ai_multitonInst);
  }

  const iVtObjectButton_s& get_vtObjectButton_a();

  bool v4enable(bool b_updateObject= false, bool b_enableReplaceOfCmd=false) { return vtObject_c::able (1, b_updateObject, b_enableReplaceOfCmd); }
  bool v4disable(bool b_updateObject= false, bool b_enableReplaceOfCmd=false) { return vtObject_c::able (0, b_updateObject, b_enableReplaceOfCmd); }

  bool v4select(uint8_t selectOrActivate) { return vtObject_c::select(selectOrActivate); }


  void setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectButton_c::setSize (newWidth, newHeight, b_updateObject, b_enableReplaceOfCmd);
  }

  void setWidth(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectButton_c::setWidth (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  void setHeight(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectButton_c::setHeight (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  void setBackgroundColour(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectButton_c::setBackgroundColour (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  void setBorderColour(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectButton_c::setBorderColour (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  void setKeyCode(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectButton_c::setKeyCode (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  void v4setOptions(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectButton_c::v4setOptions (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  bool moveChildLocation(IsoAgLib::iVtObject_c* apc_childObject, int8_t dx, int8_t dy, bool b_updateObject=false) {
    return vtObjectButton_c::moveChildLocation(apc_childObject, dx, dy, b_updateObject);
  }

  bool setChildPosition(IsoAgLib::iVtObject_c* apc_childObject, int16_t dx, int16_t dy, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    return vtObjectButton_c::setChildPosition(apc_childObject, dx, dy, b_updateObject, b_enableReplaceOfCmd);
  }

#ifdef USE_ISO_TERMINAL_GETATTRIBUTES
  // ///////////////////////// getter for attributes
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return vtObjectButton_c::updateObjectType(); }
  */

  uint16_t updateWidth(bool b_SendRequest=false) {
    return vtObjectButton_c::updateWidth(b_SendRequest);
  }

  uint16_t updateHeight(bool b_SendRequest=false) {
    return vtObjectButton_c::updateHeight(b_SendRequest);
  }

  uint8_t updateBackgroundColour(bool b_SendRequest=false) {
    return vtObjectButton_c::updateBackgroundColour(b_SendRequest);
  }

  uint8_t updateBorderColour(bool b_SendRequest=false) {
    return vtObjectButton_c::updateBorderColour(b_SendRequest);
  }

  uint8_t updateKeyCode(bool b_SendRequest=false) {
    return vtObjectButton_c::updateKeyCode(b_SendRequest);
  }

  uint8_t updateOptions(bool b_SendRequest=false) {
    return vtObjectButton_c::updateOptions(b_SendRequest);
  }
#endif

  virtual uint16_t getObjectType() const { return objectType(); }
};

} // IsoAgLib

#endif
