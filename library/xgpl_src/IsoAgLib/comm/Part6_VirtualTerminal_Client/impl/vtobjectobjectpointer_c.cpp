/*
  vtobjectobjectpointer_c.cpp

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

#include "vtobjectobjectpointer_c.h"

#ifdef USE_VTOBJECT_objectpointer
#include "vtclient_c.h"


namespace __IsoAgLib {


struct vtObjectObjectPointer_c::iVtObjectObjectPointer_s: iVtObject_s {
	IsoAgLib::iVtObject_c *value;
	iVtObjectObjectPointer_s(
			IsoAgLib::ObjectID ID,
			iVtObject_c *value)
	: iVtObject_s(ID)
	, value(value)
	{
	}
};

int16_t
vtObjectObjectPointer_c::stream(uint8_t* destMemory,
                                uint16_t /*maxBytes*/,
                                objRange_t sourceOffset)
{

    if (sourceOffset == 0) { // dump out constant sized stuff
      destMemory [0] = vtObject_a->ID & 0xFF;
      destMemory [1] = vtObject_a->ID >> 8;
      destMemory [2] = 27; // Object Type = Object Pointer
      if (vtObject_a->value != NULL) {
        destMemory [3] = vtObject_a->value->getID() & 0xFF;
        destMemory [4] = vtObject_a->value->getID() >> 8;
      } else {
        destMemory [3] = 0xFF;
        destMemory [4] = 0xFF;
      }
      return 5;
    }
    return 0;
}

IsoAgLib::ObjectID vtObjectObjectPointer_c::getID() const {
	isoaglib_assert(vtObject_a);
	return vtObject_a->ID;
}


uint32_t
vtObjectObjectPointer_c::fitTerminal() const
{
  return 5;
}


void
vtObjectObjectPointer_c::setValue(IsoAgLib::iVtObject_c* apc_newObject, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  if (b_updateObject)
	  vtObject_a->value = apc_newObject;

  if (apc_newObject != NULL) __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).commandHandler().sendCommandChangeNumericValue (this, apc_newObject->getID() & 0xFF, (apc_newObject->getID() >> 8) & 0xFF, 0x00, 0x00, b_enableReplaceOfCmd);
  else                       __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).commandHandler().sendCommandChangeNumericValue (this, 0xFF, 0xFF, 0x00, 0x00, b_enableReplaceOfCmd);
}

void
vtObjectObjectPointer_c::setOriginSKM(bool b_SKM)
{
  if (vtObject_a->value != NULL) {
    vtObject_a->value->setOriginSKM (b_SKM);
  }
}

void
vtObjectObjectPointer_c::setOriginBTN(IsoAgLib::iVtObjectButton_c* p_btn)
{
  if (p_btn) p_parentButtonObject = p_btn;
  if (vtObject_a->value != NULL) {
    vtObject_a->value->setOriginBTN (p_btn);
  }
}

#ifdef CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES
/** that attribute is in parentheses in the spec, so commented out here
uint16_t
vtObjectObjectPointer_c::updateValue(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectObjectPointer_a(), value), sizeof(iVtObjectObjectPointer_s), 1);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectObjectPointer_a(), value), sizeof(iVtObjectObjectPointer_s));
}
*/

void
vtObjectObjectPointer_c::saveReceivedAttribute(uint8_t /*attrID*/, uint8_t* /*pui8_attributeValue*/)
{
  /** that attribute is in parentheses in the spec, so commented out here
  case 1: saveValue16(MACRO_getStructOffset(get_vtObjectObjectPointer_a(), value), sizeof(iVtObjectObjectPointer_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
  default: break;
  */
}
#endif
    vtObjectObjectPointer_c::vtObjectObjectPointer_c(
    		vtObjectObjectPointer_c::iVtObjectObjectPointer_s *vtObjectObjectPointerSROM, int ai_multitonInst)
            :vtObject_c((iVtObject_s*) vtObjectObjectPointerSROM , ai_multitonInst)
    		,vtObject_a(vtObjectObjectPointerSROM)
    {}

    vtObjectObjectPointer_c::iVtObjectObjectPointer_s *vtObjectObjectPointer_c::get_vtObjectObjectPointer_a()
    {
    	return vtObject_a;
    }


} // __IsoAgLib

#endif //USE_VTOBJECT_objectpointer
