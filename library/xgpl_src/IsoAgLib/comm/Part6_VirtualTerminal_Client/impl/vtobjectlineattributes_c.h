/*
  vtobjectlineattributes_c.h

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
#ifndef VTOBJECTLINEATTRIBUTES_C_H
#define VTOBJECTLINEATTRIBUTES_C_H

#include "vtobject_c.h"
#include "vtclient_c.h"
#include "vtclientconnection_c.h"


namespace __IsoAgLib {

class vtObjectLineAttributes_c : public vtObject_c
{
public:
  int16_t stream(uint8_t* destMemory,
                 uint16_t maxBytes,
                 objRange_t sourceOffset);

  vtObjectLineAttributes_c(const iVtObjectLineAttributes_s* vtObjectLineAttributesSROM , int ai_multitonInst)
  :vtObject_c((iVtObject_s*) vtObjectLineAttributesSROM , ai_multitonInst)
  {}

  iVtObjectLineAttributes_s* get_vtObjectLineAttributes_a() { return dynamic_cast<iVtObjectLineAttributes_s *>(&(get_vtObject_a())); }



  uint32_t fitTerminal() const;

    virtual // //////////////////////////////////
  // All special Attribute-Set methods
  void setLineColour(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectLineAttributes_a(), lineColour) : 0, sizeof(iVtObjectLineAttributes_s), 1, newValue, __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).getUserConvertedColor (newValue, this, IsoAgLib::LineColour), b_enableReplaceOfCmd);
  }

    virtual void setLineWidth(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectLineAttributes_a(), lineWidth) : 0, sizeof(iVtObjectLineAttributes_s), 2, newValue, newValue, b_enableReplaceOfCmd);
  }

    virtual void setLineArt(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValue16SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectLineAttributes_a(), lineArt) : 0, sizeof(iVtObjectLineAttributes_s), 3, newValue, b_enableReplaceOfCmd);
  }

    virtual void setLineAttributes(uint8_t newLineColour, uint8_t newLineWidth, uint16_t newLineArt, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);

#ifdef USE_ISO_TERMINAL_GETATTRIBUTES

        virtual /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return 24; }
  */

  uint8_t updateLineColour(bool b_SendRequest=false);

        virtual uint8_t updateLineWidth(bool b_SendRequest=false);

        virtual uint16_t updateLineArt(bool b_SendRequest=false);

  void saveReceivedAttribute (uint8_t attrID, uint8_t* pui8_attributeValue);
#endif
};

} // __IsoAgLib

#endif
