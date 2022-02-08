/*
  vtobjectauxiliarypointer_c.cpp - actually
  "vtobjectauxiliarycontroldesignatorobjectpointer_c.cpp" but shortened
  because of problems with the filesystem's name/path-lengths.

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
#include "vtobjectauxiliarypointer_c.h"
#include "vtclient_c.h"




namespace __IsoAgLib {


struct vtObjectAuxiliaryControlDesignatorObjectPointer_c::iVtObjectAuxiliaryControlDesignatorObjectPointer_s: iVtObject_s {
	uint8_t pointerType;
	iVtObject_c *value;
	iVtObjectAuxiliaryControlDesignatorObjectPointer_s(
			IsoAgLib::ObjectID ID,
			uint8_t pointerType,
			iVtObject_c *value)
	: iVtObject_s(ID)
	, pointerType(pointerType)
	, value(value)
	{
	}


};

int16_t
vtObjectAuxiliaryControlDesignatorObjectPointer_c::stream(uint8_t* destMemory,
                                                          uint16_t /*maxBytes*/,
                                                          objRange_t sourceOffset)
{
  uint16_t curBytes=0; /* current bytes written */;
  if (sourceOffset == 0) { // dump out constant sized stuff
    destMemory [0] = vtObject_a->ID & 0xFF;
    destMemory [1] = vtObject_a->ID >> 8;
    destMemory [2] = 33; // Object Type = Auxiliary Control Designator Object Pointer
    destMemory [3] = vtObject_a->pointerType;
    if (vtObject_a->value)
    {
      destMemory [4] = vtObject_a->value->getID() & 0xFF;
      destMemory [5] = vtObject_a->value->getID() >> 8;
    }
    else
    {
      destMemory [4] = 0xFF;
      destMemory [5] = 0xFF;
    }
    sourceOffset += 6;
    curBytes += 6;
  }
  
  return curBytes;
}

IsoAgLib::ObjectID vtObjectAuxiliaryControlDesignatorObjectPointer_c::getID() const {
	isoaglib_assert(vtObject_a);
	return vtObject_a->ID;
}

uint32_t
vtObjectAuxiliaryControlDesignatorObjectPointer_c::fitTerminal() const
{
  return 6;
}


#ifdef CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES
/** that attribute is in parentheses in the spec, so commented out here
uint16_t
vtObjectAuxiliaryControlDesignatorObjectPointer_c::updateValue(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectAuxiliaryControlDesignatorObjectPointer_a(), value), sizeof(iVtObjectAuxiliaryControlDesignatorObjectPointer_s), 1);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectAuxiliaryControlDesignatorObjectPointer_a(), value), sizeof(iVtObjectAuxiliaryControlDesignatorObjectPointer_s));
}
*/

void
vtObjectAuxiliaryControlDesignatorObjectPointer_c::saveReceivedAttribute(uint8_t /*attrID*/, uint8_t* /*pui8_attributeValue*/)
{
  /** that attribute is in parentheses in the spec, so commented out here
  case 1: saveValue16(MACRO_getStructOffset(get_vtObjectAuxiliaryControlDesignatorObjectPointer_a(), value), sizeof(iVtObjectAuxiliaryControlDesignatorObjectPointer_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
  default: break;
  */
}
#endif


    vtObjectAuxiliaryControlDesignatorObjectPointer_c::vtObjectAuxiliaryControlDesignatorObjectPointer_c(
            vtObjectAuxiliaryControlDesignatorObjectPointer_c::iVtObjectAuxiliaryControlDesignatorObjectPointer_s *vtObjectAuxiliaryControlDesignatorObjectPointerSROM,
            int ai_multitonInst)
            : vtObject_c((iVtObject_s *)vtObjectAuxiliaryControlDesignatorObjectPointerSROM , ai_multitonInst)
    		, vtObject_a(vtObjectAuxiliaryControlDesignatorObjectPointerSROM)
    {}

    vtObjectAuxiliaryControlDesignatorObjectPointer_c::iVtObjectAuxiliaryControlDesignatorObjectPointer_s *
    vtObjectAuxiliaryControlDesignatorObjectPointer_c::get_vtObjectAuxiliaryControlDesignatorObjectPointer_a()
    {
    	return vtObject_a;
    }

    void vtObjectAuxiliaryControlDesignatorObjectPointer_c::setAuxiliaryObjectId(IsoAgLib::iVtObject_c *apc_newObject,
                                                                                 bool b_updateObject,
                                                                                 bool b_enableReplaceOfCmd) {
        saveValuePSetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectAuxiliaryControlDesignatorObjectPointer_a(), value) : 0, sizeof(iVtObjectAuxiliaryControlDesignatorObjectPointer_s), 2 /* "auxiliary object ID Attribute" */, apc_newObject, b_enableReplaceOfCmd);
    }



} // __IsoAgLib


