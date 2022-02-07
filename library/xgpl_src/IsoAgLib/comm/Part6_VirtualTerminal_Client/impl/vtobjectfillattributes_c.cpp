/*
  vtobjectfillattributes_c.cpp

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
#include "vtobjectfillattributes_c.h"

#include "../ivtobjectpicturegraphic_c.h"
#include "../ivtobjectmacro_c.h"
#include "vtclient_c.h"


namespace __IsoAgLib {



struct vtObjectFillAttributes_c::iVtObjectFillAttributes_s : iVtObjectwMacro_s {
  uint8_t fillType;
  IsoAgLib::Colour fillColour;
  IsoAgLib::iVtObjectPictureGraphic_c* fillPatternObject;
  iVtObjectFillAttributes_s(
		IsoAgLib::ObjectID ID,
  		uint8_t fillType,
		IsoAgLib::Colour fillColour,
		IsoAgLib::iVtObjectPictureGraphic_c *fillPatternObject)
  : iVtObject_s(ID)
  , iVtObjectwMacro_s(ID)
  , fillType(fillType)
  , fillColour(fillColour)
  , fillPatternObject(fillPatternObject)
  {}
};




int16_t
vtObjectFillAttributes_c::stream(uint8_t* destMemory,
                                 uint16_t maxBytes,
                                 objRange_t sourceOffset)
{
    uint16_t curBytes=0; /* current bytes written */;

    if (sourceOffset == 0) { // dump out constant sized stuff
      destMemory [0] = vtObject_a->ID & 0xFF;
      destMemory [1] = vtObject_a->ID >> 8;
      destMemory [2] = 25; // Object Type = Fill Attributes
      destMemory [3] = vtObject_a->fillType;
      destMemory [4] = __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).getUserConvertedColor (vtObject_a->fillColour, this, IsoAgLib::FillColour);
      if (vtObject_a->fillPatternObject != NULL)
      {
        destMemory [5] = vtObject_a->fillPatternObject->getID() & 0xFF;
        destMemory [6] = vtObject_a->fillPatternObject->getID() >> 8;
      }
      else
      {
        destMemory [5] = 0xFF;
        destMemory [6] = 0xFF;
      }
      destMemory [7] = vtObject_a->numberOfMacrosToFollow;
      sourceOffset += 8;
      curBytes += 8;
    }

    MACRO_streamEventMacro(8);
    return curBytes;
}

IsoAgLib::ObjectID vtObjectFillAttributes_c::getID() const {
	isoaglib_assert(vtObject_a);
	return vtObject_a->ID;
}


uint32_t
vtObjectFillAttributes_c::fitTerminal() const
{
  return 8+vtObject_a->numberOfMacrosToFollow*2;
}

void
vtObjectFillAttributes_c::setFillAttributes(uint8_t newFillType, IsoAgLib::Colour newFillColour, IsoAgLib::iVtObjectPictureGraphic_c* newFillPattern, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  if (b_updateObject) {
	vtObject_a->fillType   = newFillType;
    vtObject_a->fillColour = __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).getUserConvertedColor (newFillColour, this, IsoAgLib::FillColour);
    vtObject_a->fillPatternObject = newFillPattern;
  }
  __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).commandHandler().sendCommandChangeFillAttributes (this, newFillType, __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).getUserConvertedColor(
    newFillColour, this, IsoAgLib::FillColour), newFillPattern, b_enableReplaceOfCmd );
}

#ifdef USE_ISO_TERMINAL_GETATTRIBUTES
uint8_t
vtObjectFillAttributes_c::updateFillType(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectFillAttributes_a(), fillType), sizeof(iVtObjectFillAttributes_s), 1);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectFillAttributes_a(), fillType), sizeof(iVtObjectFillAttributes_s));
}

uint8_t
vtObjectFillAttributes_c::updateFillColour(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectFillAttributes_a(), fillColour), sizeof(iVtObjectFillAttributes_s), 2);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectFillAttributes_a(), fillColour), sizeof(iVtObjectFillAttributes_s));
}

uint16_t
vtObjectFillAttributes_c::updateFillPattern(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectFillAttributes_a(), fillPatternObject), sizeof(iVtObjectFillAttributes_s), 3);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectFillAttributes_a(), fillPatternObject), sizeof(iVtObjectFillAttributes_s));
}

void
vtObjectFillAttributes_c::saveReceivedAttribute(uint8_t attrID, uint8_t* pui8_attributeValue)
{
  switch (attrID)
  {
    case 1: saveValue8(MACRO_getStructOffset(get_vtObjectFillAttributes_a(), fillType), sizeof(iVtObjectFillAttributes_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    case 2: saveValue8(MACRO_getStructOffset(get_vtObjectFillAttributes_a(), fillColour), sizeof(iVtObjectFillAttributes_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    case 3: saveValue16(MACRO_getStructOffset(get_vtObjectFillAttributes_a(), fillPatternObject), sizeof(iVtObjectFillAttributes_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    default: break;
  }
}
#endif

    vtObjectFillAttributes_c::vtObjectFillAttributes_c(
            vtObjectFillAttributes_c::iVtObjectFillAttributes_s *vtObjectFillAttributesSROM, int ai_multitonInst)
            : vtObject_c((iVtObject_s*) vtObjectFillAttributesSROM , ai_multitonInst)
    		, vtObject_a(vtObjectFillAttributesSROM)
    {}

    vtObjectFillAttributes_c::iVtObjectFillAttributes_s *vtObjectFillAttributes_c::get_vtObjectFillAttributes_a()
    {
    	return vtObject_a;
    }

    void vtObjectFillAttributes_c::setFillType(uint8_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectFillAttributes_a(), fillType) : 0, sizeof(iVtObjectFillAttributes_s), 1 /* "Fill Type" */, newValue, newValue, b_enableReplaceOfCmd);
    }

    void vtObjectFillAttributes_c::setFillColour(IsoAgLib::Colour newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectFillAttributes_a(), fillColour) : 0, sizeof(iVtObjectFillAttributes_s), 2 /* "Fill Colour" */, newValue, __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).getUserConvertedColor (newValue, this, IsoAgLib::FillColour), b_enableReplaceOfCmd);
    }

    void vtObjectFillAttributes_c::setFillPattern(IsoAgLib::iVtObjectPictureGraphic_c *newValue, bool b_updateObject,
                                                  bool b_enableReplaceOfCmd) {
        saveValuePSetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectFillAttributes_a(), fillPatternObject) : 0, sizeof(iVtObjectFillAttributes_s), 3 /* "Fill Pattern" */, newValue, b_enableReplaceOfCmd);
    }


} // __IsoAgLib
