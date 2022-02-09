/*
  vtobjectbutton_c.h

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
#ifndef VTOBJECTBUTTON_C_H
#define VTOBJECTBUTTON_C_H

#include <IsoAgLib/isoaglib_config.h>


#ifdef CONFIG_USE_VTOBJECT_button

#include <memory> // PImpl
#include "vtobject_c.h"
#include "vtclient_c.h"
#include "vtclientconnection_c.h"


namespace __IsoAgLib {

class vtObjectButton_c : public vtObject_c
{
private:
	enum AttributeID:uint8_t;
	// Internal implementation class
	struct iVtObjectButton_s;

	// Pointer to the internal implementation
	std::unique_ptr<iVtObjectButton_s> vtObject_a;


public:
  int16_t stream(uint8_t* destMemory,
                 uint16_t maxBytes,
                 objRange_t sourceOffset) override;
  IsoAgLib::ObjectID getID() const override;

  vtObjectButton_c() = delete;

  vtObjectButton_c(
    int ai_multitonInst,
	IsoAgLib::ObjectID ID,
	uint16_t width,
	uint16_t height,
	IsoAgLib::Colour backgroundColour,
	IsoAgLib::Colour borderColour,
	uint8_t keyCode,
	uint8_t options);

  vtObjectButton_c(iVtObjectButton_s* vtObjectButtonSROM , int ai_multitonInst);

  ~vtObjectButton_c() override;


  void Append(iVtObject_c* const vtObject, int16_t x, int16_t y);

  uint32_t fitTerminal() const override;

    virtual void setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);

    // //////////////////////////////////
    // All special Attribute-Set methods
    virtual void setWidth(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);

    virtual void setHeight(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);

    virtual uint16_t getHeight() const;

    virtual uint16_t getWidth() const;

    virtual void setBackgroundColour(IsoAgLib::Colour newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);

    virtual void setBorderColour(IsoAgLib::Colour newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);

    virtual void setKeyCode(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);

    virtual bool moveChildLocation(IsoAgLib::iVtObject_c* apc_childObject, int8_t dx, int8_t dy, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);

    virtual bool setChildPosition(IsoAgLib::iVtObject_c* apc_childObject, int16_t dx, int16_t dy, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);

#ifdef CONFIG_USE_VTOBJECT_button
  void setOriginBTN(IsoAgLib::iVtObjectButton_c* p_btn);
#endif //CONFIG_USE_VTOBJECT_button

    virtual void v4setOptions (uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);

#ifdef CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES
    // ///////////////////////// getter for attributes
    virtual /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return 6; }
  */

  uint16_t updateWidth(bool b_SendRequest=false);

        virtual uint16_t updateHeight(bool b_SendRequest=false);

        virtual uint8_t updateBackgroundColour(bool b_SendRequest=false);

        virtual uint8_t updateBorderColour(bool b_SendRequest=false);

        virtual uint8_t updateKeyCode(bool b_SendRequest=false);

        virtual uint8_t updateOptions(bool b_SendRequest=false);

  void saveReceivedAttribute (uint8_t attrID, uint8_t* pui8_attributeValue);
#endif
};

} // __IsoAgLib

#endif //CONFIG_USE_VTOBJECT_button

#endif //VTOBJECTBUTTON_C_H
