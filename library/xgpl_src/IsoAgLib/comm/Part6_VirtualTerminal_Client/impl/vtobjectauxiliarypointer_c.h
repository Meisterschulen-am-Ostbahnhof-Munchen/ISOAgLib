/*
  vtobjectauxiliarypointer_c.h - actually
  "vtobjectauxiliarycontroldesignatorobjectpointer_c.h" but shortened
  because of problems with the filesystem's name/path-lengths.

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef VTOBJECTAUXILIARYCONTROLDESIGNATOROBJECTPOINTER_C_H
#define VTOBJECTAUXILIARYCONTROLDESIGNATOROBJECTPOINTER_C_H

#include <IsoAgLib/isoaglib_config.h>

#ifdef USE_VTOBJECT_auxiliarycontroldesignatorobjectpointer
#include "vtobject_c.h"
#include "vttypes.h"
#include "vtclientservercommunication_c.h"

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

class vtObjectAuxiliaryControlDesignatorObjectPointer_c : public vtObject_c
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
  //! @param vtObjectAuxiliaryControlDesignatorObjectPointerSROM:
  //! @param b_initPointer:
  void init(const iVtObjectAuxiliaryControlDesignatorObjectPointer_s* vtObjectAuxiliaryControlDesignatorObjectPointerSROM MULTITON_INST_PARAMETER_DEF_WITH_COMMA)
  {vtObject_c::init((iVtObject_s *)vtObjectAuxiliaryControlDesignatorObjectPointerSROM MULTITON_INST_PARAMETER_USE_WITH_COMMA);}

  //  Operation: get_vtObjectAuxiliaryControlDesignatorObjectPointer_a
  iVtObjectAuxiliaryControlDesignatorObjectPointer_s* get_vtObjectAuxiliaryControlDesignatorObjectPointer_a() { return (iVtObjectAuxiliaryControlDesignatorObjectPointer_s *)&(get_vtObject_a()); }

  //  Operation: vtObjectAuxiliaryControlDesignatorObjectPointer_c
  vtObjectAuxiliaryControlDesignatorObjectPointer_c();
  
  ~vtObjectAuxiliaryControlDesignatorObjectPointer_c() {}


  //  Operation: size
  uint32_t fitTerminal() const;

#ifdef USE_ISO_TERMINAL_GETATTRIBUTES
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return 33; }
  */
  
  uint16_t updateValue(bool b_SendRequest=false);

  void saveReceivedAttribute (uint8_t attrID, uint8_t* pui8_attributeValue);
#endif
};

} // end of namespace __IsoAgLib

#endif
#endif
