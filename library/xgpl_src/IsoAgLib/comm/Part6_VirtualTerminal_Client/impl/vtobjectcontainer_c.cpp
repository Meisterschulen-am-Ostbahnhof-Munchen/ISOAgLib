/*
  vtobjectcontainer_c.cpp

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

#include "vtobjectcontainer_c.h"
#include "../ivtobjectcontainer_c.h"

#ifdef CONFIG_USE_VTOBJECT_container
#include "../ivtobjectfontattributes_c.h"
#include "../ivtobjectbutton_c.h"
#include "../ivtobjectmacro_c.h"
#include "vtclient_c.h"




namespace __IsoAgLib {



struct vtObjectContainer_c::iVtObjectContainer_s : iVtObjectObject_s, iVtObjectwMacro_s {
  uint16_t width;
  uint16_t height;
  uint8_t hidden;
  iVtObjectContainer_s(
		  IsoAgLib::ObjectID ID,
  		  uint16_t width,
		  uint16_t height,
		  uint8_t hidden)
  : iVtObject_s(ID)
  , iVtObjectObject_s(ID)
  , iVtObjectwMacro_s(ID)
  , width(width)
  , height(height)
  , hidden(hidden)
  {}
};

int16_t
vtObjectContainer_c::stream(uint8_t* destMemory,
                            uint16_t maxBytes,
                            objRange_t sourceOffset)
{
    uint16_t curBytes=0; /* current bytes written */;
    MACRO_scaleLocalVars;
    MACRO_scaleSKLocalVars;

    uint32_t width  = (uint32_t)vtObject_a->width;
    uint32_t height = (uint32_t)vtObject_a->height;
    MACRO_scaleSizeI32(width, height);

    if (sourceOffset == 0) { // dump out constant sized stuff
      destMemory [0] = vtObject_a->ID & 0xFF;
      destMemory [1] = vtObject_a->ID >> 8;
      destMemory [2] = 3; // Object Type = Container
      destMemory [3] = width & 0xFF;
      destMemory [4] = width >> 8;
      destMemory [5] = height & 0xFF;
      destMemory [6] = height >> 8;
      destMemory [7] = vtObject_a->hidden;
      destMemory [8] = vtObject_a->numberOfObjectsToFollow;
      destMemory [9] = vtObject_a->numberOfMacrosToFollow;

      sourceOffset += 10;
      curBytes += 10;
    }

    MACRO_streamObjectXY(10);
    MACRO_streamEventMacro(10U+vtObject_a->numberOfObjectsToFollow*6U);
    return curBytes;
}


IsoAgLib::ObjectID vtObjectContainer_c::getID() const {
	isoaglib_assert(vtObject_a);
	return vtObject_a->ID;
}

uint32_t
vtObjectContainer_c::fitTerminal() const
{
  return 10+vtObject_a->numberOfObjectsToFollow*6+vtObject_a->numberOfMacrosToFollow*2;
}


void
vtObjectContainer_c::hideShow(uint8_t b_hideOrShow, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  if (b_updateObject)
	  vtObject_a->hidden = (!b_hideOrShow)&0x01;

   __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).commandHandler().sendCommandHideShow(
     this, b_hideOrShow, b_enableReplaceOfCmd );
}

void
vtObjectContainer_c::setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  if (b_updateObject) {
	  vtObject_a->width = newWidth;
	  vtObject_a->height = newHeight;
  }

  scaleSize( newWidth, newHeight );

  __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).commandHandler().sendCommandChangeSize(
    this, newWidth, newHeight, b_enableReplaceOfCmd );
}

bool
vtObjectContainer_c::moveChildLocation(IsoAgLib::iVtObject_c* apc_childObject, int16_t dx, int16_t dy, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  return genericChangeChildLocation (apc_childObject, dx, dy, b_updateObject, vtObject_a->numberOfObjectsToFollow, vtObject_a->objectsToFollow, b_enableReplaceOfCmd);
}

bool
vtObjectContainer_c::setChildPosition(IsoAgLib::iVtObject_c* apc_childObject, int16_t x, int16_t y, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  return genericChangeChildPosition (apc_childObject, x, y, b_updateObject, vtObject_a->numberOfObjectsToFollow, vtObject_a->objectsToFollow, b_enableReplaceOfCmd, NoOffset);
}

void
vtObjectContainer_c::setOriginSKM(bool b_SKM)
{
  if (b_SKM) {
    s_properties.flags |= FLAG_ORIGIN_SKM;
    for (int i=0; i<vtObject_a->numberOfObjectsToFollow; i++) {
      vtObject_a->objectsToFollow[i].vtObject->setOriginSKM (b_SKM);
    }
  }
}

void
vtObjectContainer_c::setOriginBTN(IsoAgLib::iVtObjectButton_c* p_btn)
{
  if (p_btn) p_parentButtonObject = p_btn;
  for (int i=0; i<vtObject_a->numberOfObjectsToFollow; i++) {
    vtObject_a->objectsToFollow[i].vtObject->setOriginBTN (p_btn);
  }
}

#ifdef CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES
/** these attributes are in parentheses in the spec, so commented out here
uint16_t
vtObjectContainer_c::updateWidth(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectContainer_a(), width), sizeof(iVtObjectContainer_s), 1);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectContainer_a(), width), sizeof(iVtObjectContainer_s));
}

uint16_t
vtObjectContainer_c::updateHeight(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectContainer_a(), height), sizeof(iVtObjectContainer_s), 2);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectContainer_a(), height), sizeof(iVtObjectContainer_s));
}

uint8_t
vtObjectContainer_c::updateHidden(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectContainer_a(), hidden), sizeof(iVtObjectContainer_s), 3);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectContainer_a(), hidden), sizeof(iVtObjectContainer_s));
}
*/

void
vtObjectContainer_c::saveReceivedAttribute(uint8_t attrID, uint8_t* /*pui8_attributeValue*/)
{
  switch (attrID)
  {
    /** these attributes are in parentheses in the spec, so commented out here
    case 1: saveValue16(MACRO_getStructOffset(get_vtObjectContainer_a(), width), sizeof(iVtObjectContainer_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    case 2: saveValue16(MACRO_getStructOffset(get_vtObjectContainer_a(), height), sizeof(iVtObjectContainer_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    case 3: saveValue8(MACRO_getStructOffset(get_vtObjectContainer_a(), hidden), sizeof(iVtObjectContainer_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    */
    default: break;
  }
}
#endif
    vtObjectContainer_c::vtObjectContainer_c(vtObjectContainer_c::iVtObjectContainer_s *vtObjectContainer_sROM,
                                             multiton ai_multitonInst)
            : vtObject_c(ai_multitonInst)
    		, vtObject_a(vtObjectContainer_sROM)
    {

    }


    vtObjectContainer_c::~vtObjectContainer_c() = default;

vtObjectContainer_c::vtObjectContainer_c(
		multiton ai_multitonInst,
		IsoAgLib::ObjectID ID,
		uint16_t width,
		uint16_t height,
		uint8_t hidden)
    : vtObjectContainer_c(
    		new iVtObjectContainer_s(
    				ID,
					width,
					height,
					hidden
    				),
					ai_multitonInst)
{
}



void vtObjectContainer_c::Append(iVtObject_c * const vtObject, int16_t x, int16_t y) {
	vtObject_a->Append(vtObject, x, y);
}


} // __IsoAgLib
#endif //CONFIG_USE_VTOBJECT_container
