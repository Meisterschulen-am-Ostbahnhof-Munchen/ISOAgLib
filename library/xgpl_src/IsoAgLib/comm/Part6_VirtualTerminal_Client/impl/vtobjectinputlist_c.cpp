/*
  vtobjectinputlist_c.cpp

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
#include "vtobjectinputlist_c.h"


#include "../ivtobjectbutton_c.h"
#include "../ivtobjectmacro_c.h"
#include "vtclient_c.h"


namespace __IsoAgLib {


struct vtObjectInputList_c::iVtObjectInputList_s : iVtObjectObject_s, iVtObjectwMacro_s {
  uint16_t width;
  uint16_t height;
  IsoAgLib::iVtObject_c* variableReference;
  uint8_t value;
  uint8_t options;
  iVtObjectInputList_s(
		    IsoAgLib::ObjectID ID,
  		    uint16_t width,
			uint16_t height,
			IsoAgLib::iVtObject_c *variableReference,
			uint8_t value,
			uint8_t options)
  : iVtObject_s(ID)
  , iVtObjectObject_s(ID)
  , iVtObjectwMacro_s(ID)
  , width(width)
  , height(height)
  , variableReference(variableReference)
  , value(value)
  , options(options)
  {}
};

int16_t
vtObjectInputList_c::stream(uint8_t* destMemory,
                            uint16_t maxBytes,
                            objRange_t sourceOffset)
{
#define MACRO_vtObjectTypeA vtObjectInputList_a
#define MACRO_vtObjectTypeS iVtObjectInputList_s
    MACRO_streamLocalVars;
    MACRO_scaleLocalVars;
    MACRO_scaleSKLocalVars;

    uint32_t width  = (uint32_t)vtObject_a->width;
    uint32_t height = (uint32_t)vtObject_a->height;
    MACRO_scaleSizeI32(width, height);

    if (sourceOffset == 0) { // dump out constant sized stuff
      destMemory [0] = vtObject_a->ID & 0xFF;
      destMemory [1] = vtObject_a->ID >> 8;
      destMemory [2] = 10; // Object Type = Input List
      destMemory [3] = width & 0xFF;
      destMemory [4] = width >> 8;
      destMemory [5] = height & 0xFF;
      destMemory [6] = height >> 8;
      if (vtObject_a->variableReference != NULL) {
        destMemory [7] = vtObject_a->variableReference->getID() & 0xFF;
        destMemory [8] = vtObject_a->variableReference->getID() >> 8;
      } else {
        destMemory [7] = 0xFF;
        destMemory [8] = 0xFF;
      }
      destMemory [9] = vtObject_a->value;

      destMemory [10] = vtObject_a->numberOfObjectsToFollow;
      destMemory [11] = vtObject_a->options;
      destMemory [12] = vtObject_a->numberOfMacrosToFollow;
      sourceOffset += 13;
      curBytes += 13;
    }

    MACRO_streamObject(13);
    MACRO_streamEventMacro(13U+vtObject_a->numberOfObjectsToFollow*2U);
    return curBytes;
}



IsoAgLib::ObjectID vtObjectInputList_c::getID() const {
	isoaglib_assert(vtObject_a);
	return vtObject_a->ID;
}

IsoAgLib::iVtObject_c*
vtObjectInputList_c::getListItem(uint8_t xth)
{
  return (dynamic_cast<iVtObjectInputList_s *>(vtObject_a))->objectsToFollow[xth].vtObject;
}


uint8_t
vtObjectInputList_c::getNumberOfListItems()
{
    return (dynamic_cast<iVtObjectInputList_s *>(vtObject_a))->numberOfObjectsToFollow;
}


uint32_t
vtObjectInputList_c::fitTerminal() const
{
  return 13+vtObject_a->numberOfObjectsToFollow*2+vtObject_a->numberOfMacrosToFollow*2;
}


void
vtObjectInputList_c::updateEnable(uint8_t aui8_enOrDis)
{
	vtObject_a->options = aui8_enOrDis;
}


void
vtObjectInputList_c::setValue(uint8_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  if (get_vtObjectInputList_a()->variableReference == NULL) {
    if (b_updateObject)
    	vtObject_a->value = newValue;

    __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).commandHandler().sendCommandChangeNumericValue (this, newValue, 0x00, 0x00, 0x00, b_enableReplaceOfCmd);
  }
}


void
vtObjectInputList_c::setItem(uint8_t aui8_index, IsoAgLib::iVtObject_c* apc_object, bool b_enableReplaceOfCmd)
{
  uint8_t hi, lo;
  if (apc_object == NULL) {
    lo = 0xFF;
    hi = 0xFF;
  } else {
    lo = apc_object->getID() & 0xFF;
    hi = apc_object->getID() >> 8;
  }
  __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).commandHandler().sendCommand(
    177 /* Command: Command --- Parameter: Change List Item */,
    this->getID() & 0xFF, this->getID() >> 8, aui8_index, lo, hi, 0xFF, 0xFF, b_enableReplaceOfCmd );
}

void
vtObjectInputList_c::setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  if (b_updateObject) {
	  vtObject_a->width = newWidth;
	  vtObject_a->height = newHeight;
  }

  scaleSize( newWidth, newHeight );

  __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).commandHandler().sendCommandChangeSize (this, newWidth, newHeight, b_enableReplaceOfCmd);
}

#ifdef USE_ISO_TERMINAL_GETATTRIBUTES
uint16_t
vtObjectInputList_c::updateWidth(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectInputList_a(), width), sizeof(iVtObjectInputList_s), 1);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectInputList_a(), width), sizeof(iVtObjectInputList_s));
}

uint16_t
vtObjectInputList_c::updateHeight(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectInputList_a(), height), sizeof(iVtObjectInputList_s), 2);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectInputList_a(), height), sizeof(iVtObjectInputList_s));
}

uint16_t
vtObjectInputList_c::updateVariableReference(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectInputList_a(), variableReference), sizeof(iVtObjectInputList_s), 3);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectInputList_a(), variableReference), sizeof(iVtObjectInputList_s));
}

/** these attributes are in parentheses in the spec, so commented out here
uint8_t
vtObjectInputList_c::updateValue(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectInputList_a(), value), sizeof(iVtObjectInputList_s), 4);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectInputList_a(), value), sizeof(iVtObjectInputList_s));
}

uint8_t
vtObjectInputList_c::updateOptions(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectInputList_a(), enabled), sizeof(iVtObjectInputList_s), 5);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectInputList_a(), enabled), sizeof(iVtObjectInputList_s));
}
*/

void
vtObjectInputList_c::saveReceivedAttribute(uint8_t attrID, uint8_t* pui8_attributeValue)
{
  switch (attrID)
  {
    case 1: saveValue16(MACRO_getStructOffset(get_vtObjectInputList_a(), width), sizeof(iVtObjectInputList_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    case 2: saveValue16(MACRO_getStructOffset(get_vtObjectInputList_a(), height), sizeof(iVtObjectInputList_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    case 3: saveValue16(MACRO_getStructOffset(get_vtObjectInputList_a(), variableReference), sizeof(iVtObjectInputList_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    /** these attributes are in parentheses in the spec, so commented out here
    case 4: saveValue8(MACRO_getStructOffset(get_vtObjectInputList_a(), value), sizeof(iVtObjectInputList_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    case 5: saveValue8(MACRO_getStructOffset(get_vtObjectInputList_a(), enabled), sizeof(iVtObjectInputList_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    */
    default: break;
  }
}
#endif
    vtObjectInputList_c::vtObjectInputList_c(vtObjectInputList_c::iVtObjectInputList_s *vtObjectInputListSROM,
                                             int ai_multitonInst)
            :vtObject_c((iVtObject_s*) vtObjectInputListSROM , ai_multitonInst)
    		,vtObject_a(vtObjectInputListSROM)
    {}

    vtObjectInputList_c::iVtObjectInputList_s *vtObjectInputList_c::get_vtObjectInputList_a()
    {
    	return vtObject_a;
    }

    void vtObjectInputList_c::setWidth(uint16_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        saveValue16SetAttributeScaled ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectInputList_a(), width) : 0, sizeof(iVtObjectInputList_s), 1, newValue, b_enableReplaceOfCmd);
    }

    void vtObjectInputList_c::setHeight(uint16_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        saveValue16SetAttributeScaled ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectInputList_a(), height) : 0, sizeof(iVtObjectInputList_s), 2, newValue, b_enableReplaceOfCmd);
    }

    void vtObjectInputList_c::setVariableReference(IsoAgLib::iVtObject_c *newValue, bool b_updateObject,
                                                   bool b_enableReplaceOfCmd) {
        saveValuePSetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectInputList_a(), variableReference) : 0, sizeof(iVtObjectInputList_s), 3, newValue, b_enableReplaceOfCmd);
    }

    void vtObjectInputList_c::setOptions(uint8_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectInputList_a(), options) : 0, sizeof(iVtObjectInputList_s), 5, newValue, newValue, b_enableReplaceOfCmd);
    }


} // __IsoAgLib


