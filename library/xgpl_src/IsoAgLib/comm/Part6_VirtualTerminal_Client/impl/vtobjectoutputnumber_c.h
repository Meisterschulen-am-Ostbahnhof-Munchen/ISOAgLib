/*
  vtobjectoutputnumber_c.h

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
#ifndef VTOBJECTOUTPUTNUMBER_C_H
#define VTOBJECTOUTPUTNUMBER_C_H

#include <IsoAgLib/isoaglib_config.h>


#include "../ivtobjectfontattributes_c.h"
#include "../ivtobject_c.h"
#include "vtobject_c.h"
#include "vtclient_c.h"
#include "vtclientconnection_c.h"


namespace __IsoAgLib {

class vtObjectOutputNumber_c : public vtObject_c
{
public:
  int16_t stream(uint8_t* destMemory,
                 uint16_t maxBytes,
                 objRange_t sourceOffset);

  vtObjectOutputNumber_c(const iVtObjectOutputNumber_s* vtObjectOutputNumberSROMs , int ai_multitonInst)
  :vtObject_c((iVtObject_s*) vtObjectOutputNumberSROMs , ai_multitonInst)
  {}

  iVtObjectOutputNumber_s* get_vtObjectOutputNumber_a() { return dynamic_cast<iVtObjectOutputNumber_s *>(&(get_vtObject_a())); }

  vtObjectOutputNumber_c();

  uint32_t fitTerminal() const;

    virtual void setValue(uint32_t newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=true);

  void setOriginSKM(bool b_SKM);

  void setOriginBTN(IsoAgLib::iVtObjectButton_c* p_btn);

    // //////////////////////////////////
  // All special Attribute-Set methods
   virtual void setWidth(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);

    virtual void setHeight(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValue16SetAttributeScaled ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectOutputNumber_a(), height) : 0, sizeof(iVtObjectOutputNumber_s), 2, newValue, b_enableReplaceOfCmd);
  }

    virtual void setBackgroundColour(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectOutputNumber_a(), backgroundColour) : 0, sizeof(iVtObjectOutputNumber_s), 3, newValue, __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).getUserConvertedColor (newValue, this, IsoAgLib::BackgroundColour), b_enableReplaceOfCmd);
  }

    virtual void setFontAttributes(IsoAgLib::iVtObjectFontAttributes_c* newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValuePSetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectOutputNumber_a(), fontAttributes) : 0, sizeof(iVtObjectOutputNumber_s), 4, newValue, b_enableReplaceOfCmd);
  }

    virtual void setOptions(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectOutputNumber_a(), options) : 0, sizeof(iVtObjectOutputNumber_s), 5, newValue, newValue, b_enableReplaceOfCmd);
  }

    virtual void setVariableReference(IsoAgLib::iVtObjectNumberVariable_c* newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValuePSetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectOutputNumber_a(), variableReference) : 0, sizeof(iVtObjectOutputNumber_s), 6, (iVtObject_c*)newValue, b_enableReplaceOfCmd);
  }

    virtual void setOffset(int32_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValue32SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectOutputNumber_a(), offset) : 0, sizeof(iVtObjectOutputNumber_s), 7, *((uint32_t*)&newValue), b_enableReplaceOfCmd);
  }

    virtual void setScale(float newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValueFloatSetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectOutputNumber_a(), scale) : 0, sizeof(iVtObjectOutputNumber_s), 8, newValue, b_enableReplaceOfCmd);
  }

    virtual void setNumberOfDecimals(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectOutputNumber_a(), numberOfDecimals) : 0, sizeof(iVtObjectOutputNumber_s), 9, newValue, newValue, b_enableReplaceOfCmd);
  }

    virtual void setFormat(bool newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectOutputNumber_a(), format) : 0, sizeof(iVtObjectOutputNumber_s), 10, (newValue) ? 1 : 0, (newValue) ? 1 : 0, b_enableReplaceOfCmd);
  }

    virtual void setHorizontalJustification(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectOutputNumber_a(), horizontalJustification) : 0, sizeof(iVtObjectOutputNumber_s), 11, newValue, newValue, b_enableReplaceOfCmd);
  }

    virtual void setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);

#ifdef USE_ISO_TERMINAL_GETATTRIBUTES
    // ///////////////////////// getter for attributes
    virtual /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return 12; }
   */

  uint16_t updateWidth(bool b_SendRequest=false);

        virtual uint16_t updateHeight(bool b_SendRequest=false);

        virtual uint8_t updateBackgroundColour(bool b_SendRequest=false);

        virtual uint16_t updateFontAttributes(bool b_SendRequest=false);

        virtual uint8_t updateOptions(bool b_SendRequest=false);

        virtual uint16_t updateVariableReference(bool b_SendRequest=false);

        virtual int32_t updateOffset(bool b_SendRequest=false);

        virtual float updateScale(bool b_SendRequest=false);

        virtual uint8_t updateNumberOfDecimals(bool b_SendRequest=false);

        virtual uint8_t updateFormat(bool b_SendRequest=false);

        virtual uint8_t updateJustification(bool b_SendRequest=false);

  /** that attribute is in parentheses in the spec, so commented out here
  uint32_t updateValue(bool b_SendRequest=false);
  */

  void saveReceivedAttribute (uint8_t attrID, uint8_t* pui8_attributeValue);
#endif
};

} // __IsoAgLib

#endif


