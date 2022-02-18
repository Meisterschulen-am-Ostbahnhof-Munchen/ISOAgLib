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


#ifdef CONFIG_USE_VTOBJECT_button


namespace IsoAgLib {

class iVtObjectButton_c : public __IsoAgLib::vtObjectButton_c
{
public:
  static uint16_t objectType();


  iVtObjectButton_c() = delete;

  explicit iVtObjectButton_c(
		iVtClientObjectPool_c* pool,
		ObjectID ID = autoID,
  	    uint16_t width = 80,
  	    uint16_t height = 30,
  	    Colour backgroundColour = RED,
  	    Colour borderColour = GREY,
  	    uint8_t keyCode = 251,
  	    uint8_t options = 0
  		);

  ~iVtObjectButton_c() override;

  bool v4enable(bool b_updateObject= false, bool b_enableReplaceOfCmd=false);
  bool v4disable(bool b_updateObject= false, bool b_enableReplaceOfCmd=false);
  bool v4select(uint8_t selectOrActivate);


  void setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) override;

  void setWidth(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) override;

  void setHeight(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) override;

  void setBackgroundColour(Colour newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) override;

  void setBorderColour(Colour newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) override;

  void setKeyCode(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) override;

  void v4setOptions(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) override;

  bool moveChildLocation(IsoAgLib::iVtObject_c* apc_childObject, int8_t dx, int8_t dy, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) override;

  bool setChildPosition(IsoAgLib::iVtObject_c* apc_childObject, int16_t dx, int16_t dy, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) override;

#ifdef CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES
  // ///////////////////////// getter for attributes
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return vtObjectButton_c::updateObjectType(); }
  */

  uint16_t updateWidth(bool b_SendRequest=false) override;

  uint16_t updateHeight(bool b_SendRequest=false) override;

  IsoAgLib::Colour updateBackgroundColour(bool b_SendRequest=false) override;

  IsoAgLib::Colour updateBorderColour(bool b_SendRequest=false) override;

  uint8_t updateKeyCode(bool b_SendRequest=false) override;

  uint8_t updateOptions(bool b_SendRequest=false) override;
#endif

  uint16_t getObjectType() const override;
};

} // IsoAgLib

#endif //CONFIG_USE_VTOBJECT_button

#endif //IVTOBJECTBUTTON_C_H
