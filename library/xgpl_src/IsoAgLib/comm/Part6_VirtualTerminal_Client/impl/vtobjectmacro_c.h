/*
  vtobjectmacro_c.h

  (C) Copyright 2009 - 2011 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef VTOBJECTMACRO_C_H
#define VTOBJECTMACRO_C_H

#include "vtobject_c.h"

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

class vtObjectMacro_c : public vtObject_c
{
public:
  /// Operation: stream
  /// @param destMemory
  /// @param maxBytes don't stream out more than that or you'll overrun the internal upload-buffer
  /// @param sourceOffset
  int16_t stream(uint8_t* destMemory,
                 uint16_t maxBytes,
                 objRange_t sourceOffset);

  /// Operation: init
  /// @param vtObjectMacroSROM
  /// @param b_initPointer
  void init(const iVtObjectMacro_s* vtObjectMacroSROM MULTITON_INST_PARAMETER_DEF_WITH_COMMA)
  { vtObject_c::init ((iVtObject_s*) vtObjectMacroSROM MULTITON_INST_PARAMETER_USE_WITH_COMMA); }

  //  Operation: get_vtObjectMacro_a
  iVtObjectMacro_s* get_vtObjectMacro_a() { return (iVtObjectMacro_s *)&(get_vtObject_a()); }

  //  Operation: vtObjectMacro_c
  vtObjectMacro_c();

  //  Operation: fitTerminal
  uint32_t fitTerminal() const;

#ifdef USE_ISO_TERMINAL_GETATTRIBUTES
  /** these attributes are in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return 28; }
   */

  void saveReceivedAttribute (uint8_t /*attrID*/, uint8_t* /*pui8_attributeValue*/) {};
#endif
};

} // end namespace __IsoAgLib

#endif
