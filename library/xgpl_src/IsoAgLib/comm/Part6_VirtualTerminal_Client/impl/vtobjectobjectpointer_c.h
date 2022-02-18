/*
  vtobjectobjectpointer_c.h

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
#ifndef VTOBJECTOBJECTPOINTER_C_H
#define VTOBJECTOBJECTPOINTER_C_H

#include <IsoAgLib/isoaglib_config.h>

#ifdef CONFIG_USE_VTOBJECT_objectpointer
#include <memory> // PImpl
#include "vtobject_c.h"
#include "vttypes.h"
#include "vtclientconnection_c.h"


namespace __IsoAgLib {

class vtObjectObjectPointer_c : public vtObject_c
{
private:
	// Internal implementation class
	struct iVtObjectObjectPointer_s;

	// Pointer to the internal implementation
	std::unique_ptr<iVtObjectObjectPointer_s> vtObject_a;

public:
  int16_t stream(uint8_t* destMemory,
                 uint16_t maxBytes,
                 objRange_t sourceOffset);
  IsoAgLib::ObjectID getID() const;


  vtObjectObjectPointer_c(iVtObjectObjectPointer_s* vtObjectObjectPointerSROM , int ai_multitonInst);

  vtObjectObjectPointer_c();
  uint32_t fitTerminal() const;
  void setValue(IsoAgLib::iVtObject_c* apc_newObject, bool b_updateObject= false, bool b_enableReplaceOfCmd=false);
  void setOriginSKM(bool b_SKM);
  void setOriginBTN(IsoAgLib::iVtObjectButton_c* /*p_btn*/);
#ifdef CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES
  /** these attributes are in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return 27; }

  uint16_t updateValue(bool b_SendRequest=false);
  */

  void saveReceivedAttribute (uint8_t attrID, uint8_t* pui8_attributeValue);
#endif
};

} // __IsoAgLib

#endif //CONFIG_USE_VTOBJECT_objectpointer

#endif //VTOBJECTOBJECTPOINTER_C_H
