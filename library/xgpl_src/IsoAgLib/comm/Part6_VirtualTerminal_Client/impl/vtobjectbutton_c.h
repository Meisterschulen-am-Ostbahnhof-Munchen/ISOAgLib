/*
  vtobjectbutton_c.h

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef VTOBJECTBUTTON_C_H
#define VTOBJECTBUTTON_C_H

#include "vtobject_c.h"
#include "isoterminal_c.h"
#include "vtclientservercommunication_c.h"

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

class vtObjectButton_c : public vtObject_c
{
public:
  //  Operation: stream
  //! @param destMemory:
  //! @param maxBytes: don't stream out more than that or you'll overrun the internal upload-buffer
  //! @param sourceOffset:
  int16_t stream(uint8_t* destMemory,
                 uint16_t maxBytes,
                 objRange_t sourceOffset);

  /// Operation: init
  /// @param vtObjectButtonSROM
  /// @param b_initPointer
  void init(const iVtObjectButton_s* vtObjectButtonSROM MULTITON_INST_PARAMETER_DEF_WITH_COMMA)
  { vtObject_c::init ((iVtObject_s*) vtObjectButtonSROM MULTITON_INST_PARAMETER_USE_WITH_COMMA); }

  //  Operation: get_vtObjectButton_a
  iVtObjectButton_s* get_vtObjectButton_a() { return (iVtObjectButton_s *)&(get_vtObject_a()); }

  //  Operation: vtObjectButton_c
  vtObjectButton_c();

  //  Operation: size
  uint32_t fitTerminal() const;

  void setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);

  // //////////////////////////////////
  // All special Attribute-Set methods
  void setWidth(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValue16SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectButton_a(), width) : 0, sizeof(iVtObjectButton_s), 1, newValue, b_enableReplaceOfCmd);
  }

  void setHeight(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValue16SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectButton_a(), height) : 0, sizeof(iVtObjectButton_s), 2, newValue, b_enableReplaceOfCmd);
  }

  void setBackgroundColour(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectButton_a(), backgroundColour) : 0, sizeof(iVtObjectButton_s), 3, newValue, __IsoAgLib::getIsoTerminalInstance4Comm().getClientByID (s_properties.clientId).getUserClippedColor (newValue, this, IsoAgLib::BackgroundColour), b_enableReplaceOfCmd);
  }

  void setBorderColour(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectButton_a(), borderColour) : 0, sizeof(iVtObjectButton_s), 4, newValue, __IsoAgLib::getIsoTerminalInstance4Comm().getClientByID (s_properties.clientId).getUserClippedColor (newValue, this, IsoAgLib::BorderColour), b_enableReplaceOfCmd);
  }

  void setKeyCode(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectButton_a(), keyCode) : 0, sizeof(iVtObjectButton_s), 5, newValue, newValue, b_enableReplaceOfCmd);
  }

  bool moveChildLocation(IsoAgLib::iVtObject_c* apc_childObject, int8_t dx, int8_t dy, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);

  bool setChildPosition(IsoAgLib::iVtObject_c* apc_childObject, int16_t dx, int16_t dy, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);

  void setOriginBTN(IsoAgLib::iVtObjectButton_c* p_btn);

#ifdef PROPRIETARY_VERSION4_CLIENT_HACK
  void setOptions (uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectButton_a(), options) : 0, sizeof(iVtObjectButton_s), 6, newValue, newValue, b_enableReplaceOfCmd);
  }
#endif

#ifdef USE_ISO_TERMINAL_GETATTRIBUTES
    // ///////////////////////// getter for attributes
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return 6; }
  */

  uint16_t updateWidth(bool b_SendRequest=false);

  uint16_t updateHeight(bool b_SendRequest=false);

  uint8_t updateBackgroundColour(bool b_SendRequest=false);

  uint8_t updateBorderColour(bool b_SendRequest=false);

  uint8_t updateKeyCode(bool b_SendRequest=false);

  uint8_t updateOptions(bool b_SendRequest=false);

  void saveReceivedAttribute (uint8_t attrID, uint8_t* pui8_attributeValue);
#endif
};

} // end of namespace __IsoAgLib

#endif
