/*
  vtobjectauxiliaryinput_c.h

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef VTOBJECTAUXILIARYINPUT_C_H
#define VTOBJECTAUXILIARYINPUT_C_H

#include <IsoAgLib/hal/hal_typedef.h>

#ifdef USE_VTOBJECT_auxiliaryinput
#include "vtobject_c.h"
#include "vttypes.h"

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {
// Class : vtObjectAuxiliaryInput_c
class vtObjectAuxiliaryInput_c : public vtObject_c
{
public:
  //  Operation: stream
  //! @param destMemory:
  //! @param maxBytes: don't stream out more than that or you'll overrun the internal upload-buffer
  //! @param sourceOffset:
  int16_t stream(uint8_t* destMemory,
                 uint16_t maxBytes,
                 objRange_t sourceOffset);

  //  Operation: init
  //! @param vtObjectAuxiliaryInputSROM:
  //! @param b_initPointer:
  void init(const iVtObjectAuxiliaryInput_s* vtObjectAuxiliaryInputSROM MULTITON_INST_PARAMETER_DEF_WITH_COMMA)
  {vtObject_c::init((iVtObject_s *)vtObjectAuxiliaryInputSROM MULTITON_INST_PARAMETER_USE_WITH_COMMA);}

  //  Operation: get_vtObjectAuxiliaryInput_a
  iVtObjectAuxiliaryInput_s* get_vtObjectAuxiliaryInput_a() { return (iVtObjectAuxiliaryInput_s *)&(get_vtObject_a()); }

  //  Operation: vtObjectAuxiliaryInput_c
  vtObjectAuxiliaryInput_c();

  //  Operation: size
  uint32_t fitTerminal() const;

  //  Operation: setOriginSKM
  //! @param b_SKM:
  void setOriginSKM(bool b_SKM);

  bool moveChildLocation(IsoAgLib::iVtObject_c* apc_childObject, int8_t dx, int8_t dy, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);

  bool setChildPosition(IsoAgLib::iVtObject_c* apc_childObject, int16_t dx, int16_t dy, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);

#ifdef USE_ISO_TERMINAL_GETATTRIBUTES
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return 30; }
  */

  void saveReceivedAttribute (uint8_t /*attrID*/, uint8_t* /*pui8_attributeValue*/) {};
#endif
};

} // end of namespace __IsoAgLib

#endif
#endif
