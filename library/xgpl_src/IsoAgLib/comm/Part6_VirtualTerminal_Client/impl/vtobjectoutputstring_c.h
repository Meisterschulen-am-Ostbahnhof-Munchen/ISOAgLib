/*
  vtobjectoutputstring_c.h

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
#ifndef VTOBJECTOUTPUTSTRING_C_H
#define VTOBJECTOUTPUTSTRING_C_H

#include <IsoAgLib/isoaglib_config.h>


#ifdef CONFIG_USE_VTOBJECT_outputstring


#include <memory> // PImpl

#include "../ivtobjectstring_c.h"
#include "vtclientconnection_c.h"
#include "vtclient_c.h"


namespace __IsoAgLib {

class vtObjectOutputString_c : public IsoAgLib::iVtObjectString_c
{
private:
	enum AttributeID:uint8_t;
	// Internal implementation class
	struct iVtObjectOutputString_s;

	// Pointer to the internal implementation
	std::unique_ptr<iVtObjectOutputString_s> vtObject_a;

public:
  int16_t stream(uint8_t* destMemory,
                 uint16_t maxBytes,
                 objRange_t sourceOffset);
  IsoAgLib::ObjectID getID() const;


  vtObjectOutputString_c(
		  	int ai_multitonInst,
		  	IsoAgLib::ObjectID ID,
			uint16_t width,
			uint16_t height,
			IsoAgLib::Colour backgroundColour,
			IsoAgLib::iVtObjectFontAttributes_c *fontAttributes,
			uint8_t options,
			IsoAgLib::iVtObjectStringVariable_c *variableReference,
			uint8_t horizontalJustification,
			char *value);




  vtObjectOutputString_c(iVtObjectOutputString_s* vtObjectOutputStringSROM , int ai_multitonInst);
  ~vtObjectOutputString_c();
  uint32_t fitTerminal() const;
  void setOriginSKM(bool b_SKM);
#ifdef CONFIG_USE_VTOBJECT_button
  void setOriginBTN(IsoAgLib::iVtObjectButton_c* p_btn);
#endif //CONFIG_USE_VTOBJECT_button
  const char* getString();
  virtual void setValueCopy    (char* newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
#ifdef USE_VT_UNICODE_SUPPORT

        virtual void setValueCopyUTF8(const char* newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
        virtual void setValueCopyUTF16(const char* newValue, uint16_t length, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
#endif

        virtual void setValueRef(char* newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
  // //////////////////////////////////
  // All special Attribute-Set methods
        virtual void setWidth(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
        virtual void setHeight(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
        virtual void setBackgroundColour(IsoAgLib::Colour newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
        virtual void setFontAttributes(IsoAgLib::iVtObjectFontAttributes_c* newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
        virtual void setOptions(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
        virtual void setVariableReference(IsoAgLib::iVtObjectStringVariable_c* newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
        virtual void setHorizontalJustification(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
        virtual void setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);

#ifdef CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES
  // ///////////////////////// getter for attributes
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return 11; }
  */

    virtual uint16_t updateWidth(bool b_SendRequest=false);
  virtual uint16_t updateHeight(bool b_SendRequest=false);
  virtual IsoAgLib::Colour updateBackgroundColour(bool b_SendRequest=false);
  virtual uint16_t updateFontAttributes(bool b_SendRequest=false);
  virtual uint8_t updateOptions(bool b_SendRequest=false);
  virtual IsoAgLib::iVtObjectStringVariable_c*  updateVariableReference(bool b_SendRequest=false);
  virtual uint8_t updateJustification(bool b_SendRequest=false);

  void saveReceivedAttribute (uint8_t attrID, uint8_t* pui8_attributeValue);
#endif
};

} // __IsoAgLib

#endif //CONFIG_USE_VTOBJECT_outputstring

#endif //VTOBJECTOUTPUTSTRING_C_H
