/*
  vtobjectnumbervariable_c.cpp

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

#include "vtobjectnumbervariable_c.h"

#ifdef CONFIG_USE_VTOBJECT_numbervariable 
#include "vtclient_c.h"


namespace __IsoAgLib {




struct vtObjectNumberVariable_c::iVtObjectNumberVariable_s: iVtObject_s {
	uint32_t value;
	iVtObjectNumberVariable_s(
			IsoAgLib::ObjectID ID,
			uint32_t value)
	: iVtObject_s(ID)
	, value(value)
	{
	}
};



int16_t
vtObjectNumberVariable_c::stream(uint8_t* destMemory,
                                 uint16_t /*maxBytes*/,
                                 objRange_t sourceOffset)
{

    if (sourceOffset == 0) { // dump out constant sized stuff
      destMemory [0] = vtObject_a->ID & 0xFF;
      destMemory [1] = vtObject_a->ID >> 8;
      destMemory [2] = 21; // Object Type = Number Variable

      destMemory [3] = (vtObject_a->value) & 0xFF;
      destMemory [4] = (vtObject_a->value >> 8) & 0xFF;
      destMemory [5] = (vtObject_a->value >> 16) & 0xFF;
      destMemory [6] = (vtObject_a->value >> 24) & 0xFF;
      return 7;
    }
    return 0;
}

IsoAgLib::ObjectID vtObjectNumberVariable_c::getID() const {
	isoaglib_assert(vtObject_a);
	return vtObject_a->ID;
}


uint32_t
vtObjectNumberVariable_c::fitTerminal() const
{
  // 3 byte header and 4 byte attributes.
  return (3+4);
}


void
vtObjectNumberVariable_c::setValue(uint32_t newValue,
                                   bool b_updateObject, bool b_enableReplaceOfCmd)
{
  if (b_updateObject)
	  vtObject_a->value = newValue;

  __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).commandHandler().sendCommandChangeNumericValue (this, newValue & 0xFF, (newValue >> 8) & 0xFF, (newValue >> 16) & 0xFF, newValue >> 24, b_enableReplaceOfCmd);
}

#ifdef CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES
/** that attribute is in parentheses in the spec, so commented out here
uint32_t
vtObjectNumberVariable_c::updateValue(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue32GetAttribute(MACRO_getStructOffset(get_vtObjectNumberVariable_a(), value), sizeof(iVtObjectNumberVariable_s), 1);
  else
    return getValue32(MACRO_getStructOffset(get_vtObjectNumberVariable_a(), value), sizeof(iVtObjectNumberVariable_s));
}
*/

void
vtObjectNumberVariable_c::saveReceivedAttribute(uint8_t /*attrID*/, uint8_t* /*pui8_attributeValue*/)
{
  /** that attribute is in parentheses in the spec, so commented out here
  if (attrID == 1)
    saveValue32(MACRO_getStructOffset(get_vtObjectNumberVariable_a(), value), sizeof(iVtObjectNumberVariable_s), convertLittleEndianStringUi32(pui8_attributeValue));
  */
}
#endif


    vtObjectNumberVariable_c::vtObjectNumberVariable_c(
    		vtObjectNumberVariable_c::iVtObjectNumberVariable_s *vtObjectNumberVariableSROM, multiton ai_multitonInst)
            :vtObject_c(ai_multitonInst)
    		,vtObject_a(vtObjectNumberVariableSROM)
    {}




} // __IsoAgLib

#endif //CONFIG_USE_VTOBJECT_numbervariable
