/*
  vtobjectobjectpointer_c.h

  (C) Copyright 2009 - 2011 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef VTOBJECTOBJECTPOINTER_C_H
#define VTOBJECTOBJECTPOINTER_C_H

#include <IsoAgLib/hal/hal_typedef.h>

#ifdef USE_VTOBJECT_objectpointer
#include "vtobject_c.h"
#include "vttypes.h"
#include "vtclientservercommunication_c.h"

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

class vtObjectObjectPointer_c : public vtObject_c
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
  //! @param vtObjectObjectPointerSROM:
  //! @param b_initPointer:
  void init(const iVtObjectObjectPointer_s* vtObjectObjectPointerSROM MULTITON_INST_PARAMETER_DEF_WITH_COMMA)
  { vtObject_c::init ((iVtObject_s*) vtObjectObjectPointerSROM MULTITON_INST_PARAMETER_USE_WITH_COMMA);}

  //  Operation: get_vtObjectObjectPointer_a
  iVtObjectObjectPointer_s* get_vtObjectObjectPointer_a() { return (iVtObjectObjectPointer_s *)&(get_vtObject_a()); }

  //  Operation: vtObjectObjectPointer_c
  vtObjectObjectPointer_c();

  //  Operation: size
  uint32_t fitTerminal() const;

  //  Operation: setValue
  //! @param apc_newObject:
  //! @param b_updateObject:
  void setValue(IsoAgLib::iVtObject_c* apc_newObject, bool b_updateObject= false, bool b_enableReplaceOfCmd=false);

  //  Operation: setOriginSKM
  //! @param b_SKM:
  void setOriginSKM(bool b_SKM);

  //! @param p_btn:
  void setOriginBTN(IsoAgLib::iVtObjectButton_c* /*p_btn*/);
#ifdef USE_ISO_TERMINAL_GETATTRIBUTES
  /** these attributes are in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return 27; }

  uint16_t updateValue(bool b_SendRequest=false);
  */

  void saveReceivedAttribute (uint8_t attrID, uint8_t* pui8_attributeValue);
#endif
};

} // end of namespace __IsoAgLib

#endif
#endif
