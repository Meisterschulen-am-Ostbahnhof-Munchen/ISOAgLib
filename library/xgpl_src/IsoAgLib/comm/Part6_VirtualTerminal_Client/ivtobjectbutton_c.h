/*
  ivtobjectbutton_c.h

  (C) Copyright 2009 - 2011 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef IVTOBJECTBUTTON_C_H
#define IVTOBJECTBUTTON_C_H


// +X2C includes
#include "impl/vtobjectbutton_c.h"
// ~X2C

// Begin Namespace IsoAgLib
namespace IsoAgLib {


//  +X2C Class 98 : iVtObjectButton_c
//!  Stereotype: Klasse
class iVtObjectButton_c : public __IsoAgLib::vtObjectButton_c
{

public:

  /// Operation: init
  /// 
  /// @param vtObjectButtonSROM
  /// @param b_initPointer
  void init(const iVtObjectButton_s* vtObjectButtonSROM MULTITON_INST_PARAMETER_DEF_WITH_COMMA) {
    vtObjectButton_c::init (vtObjectButtonSROM MULTITON_INST_PARAMETER_USE_WITH_COMMA);
  }

  //  Operation: get_vtObjectButton_a
  const iVtObjectButton_s& get_vtObjectButton_a();

  //  Operation: select
  //  ATTENTION: THIS IS ONLY SUPPORTED BY REVISION 4 VTs
  //  WHICH ARE CURRENTLY NOT YET ON THE MARKET.
  //  SO USE THIS COMMAND ONYL FOR SPECIAL PURPOSES AND IF YOU KNOW YOUR VT-SERVER
  //  SUPPORTS THIS COMMAND - OTHER THAN THAT YOU WILL GET 4,5 SECONDS OF DELAY IN YOUR COMMUNICATION!
  //  @todo SOON-59 Put this command into some #ifdef REVISION3_CLIENT.
  //  @todo SOON-59 Change #ifdef REVISION3_CLIENT to #ifdef REVISION4_CLIENT
  bool select(uint8_t selectOrActivate) { return vtObject_c::select(selectOrActivate); }


  /// Operation: setSize
  /// 
  /// @param newWidth
  /// @param newHeight
  /// @param b_updateObject
  void setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectButton_c::setSize (newWidth, newHeight, b_updateObject, b_enableReplaceOfCmd);
  }

  /// Operation: setWidth
  /// 
  /// @param newValue
  /// @param b_updateObject
  void setWidth(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectButton_c::setWidth (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  /// Operation: setHeight
  ///
  /// @param newValue
  /// @param b_updateObject
  void setHeight(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectButton_c::setHeight (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  /// Operation: setBackgroundColour
  ///
  /// @param newValue
  /// @param b_updateObject
  void setBackgroundColour(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectButton_c::setBackgroundColour (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setBorderColour
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setBorderColour(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectButton_c::setBorderColour (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setKeyCode
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setKeyCode(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectButton_c::setKeyCode (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

#ifdef REVISION3_CLIENT
  //  Operation: setOptions
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setOptions(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectButton_c::setOptions (newValue, b_updateObject, b_enableReplaceOfCmd);
  }
#endif

  //  Operation: moveChildLocation
  //! Parameter:
  //! @param apc_childObject:
  //! @param dx:
  //! @param dy:
  bool moveChildLocation(IsoAgLib::iVtObject_c* apc_childObject, int8_t dx, int8_t dy, bool b_updateObject=false) {
    return vtObjectButton_c::moveChildLocation(apc_childObject, dx, dy, b_updateObject);
  }

  //  Operation: setChildPosition
  //! @param apc_childObject:
  //! @param dx:
  //! @param dy:
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
};

} // end of namespace IsoAgLib

#endif
