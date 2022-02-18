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

#include <cstddef>
#include <cstdint>

#include "commandhandler_c.h"
#include "iassert.h"
#include "ivtobject_c.h"
#include "ivttypes.h"
#include "multiton.h"
#include "util_funcs.h"
#include "vtclientconnection_c.h"


#ifdef CONFIG_USE_VTOBJECT_fillattributes

#include "../ivtobjectpicturegraphic_c.h"
#include "../ivtobjectmacro_c.h"
#include "vtclient_c.h"


namespace __IsoAgLib {

enum vtObjectFillAttributes_c::AttributeID:uint8_t
{
	FillType           = 1,
	FillColour         = 2,
	FillPatternObject  = 3,
};

struct vtObjectFillAttributes_c::iVtObjectFillAttributes_s : iVtObjectwMacro_s {
  IsoAgLib::FillType fillType;
  IsoAgLib::Colour fillColour;
#ifdef CONFIG_USE_VTOBJECT_picturegraphic
  IsoAgLib::iVtObjectPictureGraphic_c* fillPatternObject;
#endif
  iVtObjectFillAttributes_s(
		  IsoAgLib::ObjectID ID
  		, IsoAgLib::FillType fillType
		, IsoAgLib::Colour fillColour
#ifdef CONFIG_USE_VTOBJECT_picturegraphic
		, IsoAgLib::iVtObjectPictureGraphic_c *fillPatternObject
#endif
		)
  : iVtObject_s(ID)
  , iVtObjectwMacro_s(ID)
  , fillType(fillType)
  , fillColour(fillColour)
#ifdef CONFIG_USE_VTOBJECT_picturegraphic
  , fillPatternObject(fillPatternObject)
#endif
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
#ifdef CONFIG_USE_VTOBJECT_picturegraphic
      if (vtObject_a->fillPatternObject != NULL)
      {
        destMemory [5] = vtObject_a->fillPatternObject->getID() & 0xFF;
        destMemory [6] = vtObject_a->fillPatternObject->getID() >> 8;
      }
      else
#endif
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


#ifdef CONFIG_USE_VTOBJECT_picturegraphic
void
vtObjectFillAttributes_c::setFillAttributes(IsoAgLib::FillType newFillType, IsoAgLib::Colour newFillColour, IsoAgLib::iVtObjectPictureGraphic_c* newFillPattern, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  if (b_updateObject) {
	vtObject_a->fillType   = newFillType;
    vtObject_a->fillColour = __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).getUserConvertedColor (newFillColour, this, IsoAgLib::FillColour);
    vtObject_a->fillPatternObject = newFillPattern;
  }
  __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).commandHandler().sendCommandChangeFillAttributes (this, newFillType, __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).getUserConvertedColor(
    newFillColour, this, IsoAgLib::FillColour), newFillPattern, b_enableReplaceOfCmd );
}
#endif

#ifdef CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES
IsoAgLib::FillType
vtObjectFillAttributes_c::updateFillType(bool b_SendRequest)
{
  if (b_SendRequest)
    getAttribute(FillType);
  return vtObject_a->fillType;
}

IsoAgLib::Colour
vtObjectFillAttributes_c::updateFillColour(bool b_SendRequest)
{
  if (b_SendRequest)
    getAttribute(FillColour);
  return vtObject_a->fillColour;
}

#ifdef CONFIG_USE_VTOBJECT_picturegraphic
IsoAgLib::iVtObjectPictureGraphic_c*
vtObjectFillAttributes_c::updateFillPattern(bool b_SendRequest)
{
  if (b_SendRequest)
    getAttribute(FillPatternObject);
  return vtObject_a->fillPatternObject;
}
#endif


void
vtObjectFillAttributes_c::saveReceivedAttribute(uint8_t attrID, uint8_t* pui8_attributeValue)
{
  switch (attrID)
  {
    case FillType:          vtObject_a->fillType          = convertLittleEndianStringFillType(pui8_attributeValue); break;
    case FillColour:        vtObject_a->fillColour        = convertLittleEndianStringColour(  pui8_attributeValue); break;
    //case FillPatternObject: vtObject_a->fillPatternObject = convertLittleEndianStringUi16(pui8_attributeValue); break; //TODO
    default: break;
  }
}
#endif

    vtObjectFillAttributes_c::vtObjectFillAttributes_c(
            vtObjectFillAttributes_c::iVtObjectFillAttributes_s *vtObjectFillAttributesSROM, int ai_multitonInst)
            : vtObject_c(ai_multitonInst)
    		, vtObject_a(vtObjectFillAttributesSROM)
    {}


    vtObjectFillAttributes_c::~vtObjectFillAttributes_c() = default;


    void vtObjectFillAttributes_c::setFillType(IsoAgLib::FillType newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
    	if (b_updateObject)
    		vtObject_a->fillType = newValue;
    	setAttribute(FillType /* "Fill Type" */, newValue, b_enableReplaceOfCmd);
    }

    void vtObjectFillAttributes_c::setFillColour(IsoAgLib::Colour newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
    	if (b_updateObject)
    		vtObject_a->fillColour = newValue;
    	setAttribute(FillColour /* "Fill Colour" */, __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).getUserConvertedColor (newValue, this, IsoAgLib::FillColour), b_enableReplaceOfCmd);
    }

#ifdef CONFIG_USE_VTOBJECT_picturegraphic
    void vtObjectFillAttributes_c::setFillPattern(IsoAgLib::iVtObjectPictureGraphic_c *newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
    	if (b_updateObject)
    		vtObject_a->fillPatternObject = newValue;
    	setAttribute(FillPatternObject /* "Fill Pattern" */, newValue->getID(), b_enableReplaceOfCmd);
    }
#endif

vtObjectFillAttributes_c::vtObjectFillAttributes_c(
		int ai_multitonInst,
		IsoAgLib::ObjectID ID,
		IsoAgLib::FillType fillType,
		IsoAgLib::Colour fillColour,
		IsoAgLib::iVtObjectPictureGraphic_c *fillPatternObject)
	:vtObjectFillAttributes_c(
			new iVtObjectFillAttributes_s(
			ID,
			fillType,
			fillColour,
			fillPatternObject),
	ai_multitonInst)
{
}

} // __IsoAgLib

#endif //CONFIG_USE_VTOBJECT_fillattributes
