/*
  vtobjectdatamask_c.cpp

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

#include "vtobjectdatamask_c.h"

#include "../ivtobject_c.h"
#include "../ivtobjectfontattributes_c.h"
#include "../ivtobjectsoftkeymask_c.h"
#include "../ivtobjectmacro_c.h"
#include "vtclient_c.h"


namespace __IsoAgLib {


enum vtObjectDataMask_c::AttributeID:uint8_t
{

	BackgroundColour = 1,
	SoftKeyMask      = 2,
};


struct vtObjectDataMask_c::iVtObjectDataMask_s: iVtObjectMask_s {
	IsoAgLib::Colour backgroundColour;
#ifdef CONFIG_USE_VTOBJECT_softkeymask
	IsoAgLib::iVtObjectSoftKeyMask_c *softKeyMask;
#endif
	explicit iVtObjectDataMask_s(
			  IsoAgLib::ObjectID ID
			, IsoAgLib::Colour backgroundColour
#ifdef CONFIG_USE_VTOBJECT_softkeymask
			, IsoAgLib::iVtObjectSoftKeyMask_c *softKeyMask
#endif
			)
    : iVtObject_s(ID)
	, iVtObjectMask_s(ID)
	, backgroundColour(backgroundColour)
#ifdef CONFIG_USE_VTOBJECT_softkeymask
	, softKeyMask(softKeyMask)
#endif
	{
	}
};



int16_t
vtObjectDataMask_c::stream(uint8_t* destMemory,
                           uint16_t maxBytes,
                           objRange_t sourceOffset)
{
    uint16_t curBytes=0; /* current bytes written */;
    MACRO_scaleLocalVars;

    if (sourceOffset == 0) { // dump out constant sized stuff
      destMemory [0] = vtObject_a->ID & 0xFF;
      destMemory [1] = vtObject_a->ID >> 8;
      destMemory [2] = 1; // Object Type = Data Mask
      destMemory [3] = __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).getUserConvertedColor (vtObject_a->backgroundColour, this, IsoAgLib::BackgroundColour);
#ifdef CONFIG_USE_VTOBJECT_softkeymask
      if (vtObject_a->softKeyMask != NULL) {
          destMemory [4] = vtObject_a->softKeyMask->getID() & 0xFF;
          destMemory [5] = vtObject_a->softKeyMask->getID() >> 8;
      } else
#endif
      {
          destMemory [4] = 0xFF;
          destMemory [5] = 0xFF;
      }
      destMemory [6] = vtObject_a->numberOfObjectsToFollow;
      destMemory [7] = vtObject_a->numberOfMacrosToFollow;
      sourceOffset += 8;
      curBytes += 8;
    }

    MACRO_DAMstreamObjectXY(8);
    MACRO_streamEventMacro(8U+vtObject_a->numberOfObjectsToFollow*6U);

    return curBytes;
}


IsoAgLib::ObjectID vtObjectDataMask_c::getID() const {
	isoaglib_assert(vtObject_a);
	return vtObject_a->ID;
}


vtObjectDataMask_c::vtObjectDataMask_c(iVtObjectDataMask_s *vtObjectDataMaskSROM, int ai_multitonInst)
	: iVtObjectMask_c((iVtObjectMask_s*) vtObjectDataMaskSROM, ai_multitonInst)
	, vtObject_a(vtObjectDataMaskSROM)
{
}


vtObjectDataMask_c::~vtObjectDataMask_c() = default;

vtObjectDataMask_c::vtObjectDataMask_c(
		  int ai_multitonInst
		, IsoAgLib::ObjectID ID
		, IsoAgLib::Colour backgroundColour
#ifdef CONFIG_USE_VTOBJECT_softkeymask
		, IsoAgLib::iVtObjectSoftKeyMask_c *softKeyMask
#endif
		)
:vtObjectDataMask_c(
		new iVtObjectDataMask_s(
				  ID
				, backgroundColour
#ifdef CONFIG_USE_VTOBJECT_softkeymask
				, softKeyMask
#endif
				),
		ai_multitonInst)
{
}

uint32_t
vtObjectDataMask_c::fitTerminal() const
{
  return 8+vtObject_a->numberOfObjectsToFollow*6+vtObject_a->numberOfMacrosToFollow*2;
}

#ifdef CONFIG_USE_VTOBJECT_softkeymask
void
vtObjectDataMask_c::setSoftKeyMask(IsoAgLib::iVtObjectSoftKeyMask_c* newSoftKeyMask, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  if (b_updateObject)
	  vtObject_a->softKeyMask = newSoftKeyMask;

  __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).commandHandler().sendCommandChangeSoftKeyMask(
    this, 1 /* "Type: Data Mask" */, (newSoftKeyMask == NULL) ? 0xFFFF : newSoftKeyMask->getID(), b_enableReplaceOfCmd );
}
#endif

bool
vtObjectDataMask_c::moveChildLocation(IsoAgLib::iVtObject_c* apc_childObject, int8_t dx, int8_t dy, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  return genericChangeChildLocation (apc_childObject, dx, dy, b_updateObject, vtObject_a->numberOfObjectsToFollow, vtObject_a->objectsToFollow, b_enableReplaceOfCmd);
}

bool
vtObjectDataMask_c::setChildPosition(IsoAgLib::iVtObject_c* apc_childObject, int16_t x, int16_t y, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  return genericChangeChildPosition (apc_childObject, x, y, b_updateObject, vtObject_a->numberOfObjectsToFollow, vtObject_a->objectsToFollow, b_enableReplaceOfCmd, DataAlarmMaskOffset);
}


bool
vtObjectDataMask_c::lockUnlockMask( bool b_lockMask, uint16_t ui16_lockTimeOut )
{
  bool result = true;
  if( __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).getVersion() >= 4 )
  {
    result = __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).commandHandler().sendCommandLockUnlockMask( this, b_lockMask, ui16_lockTimeOut );
  }
  return result;
}


void vtObjectDataMask_c::Append(iVtObject_c* const vtObject, int16_t x, int16_t y)
{
	vtObject_a->Append(vtObject, x, y);
}




#ifdef CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES
IsoAgLib::Colour
vtObjectDataMask_c::updateBackgroundColour (bool b_SendRequest)
{
  if (b_SendRequest)
    getAttribute(BackgroundColour);
  return vtObject_a->backgroundColour;
}

#ifdef CONFIG_USE_VTOBJECT_softkeymask
IsoAgLib::iVtObjectSoftKeyMask_c*
vtObjectDataMask_c::updateSoftKeyMask(bool b_SendRequest)
{
  if (b_SendRequest)
    getAttribute(SoftKeyMask);
  return vtObject_a->softKeyMask;
}
#endif

void
vtObjectDataMask_c::saveReceivedAttribute (uint8_t attrID, uint8_t* pui8_attributeValue)
{
  switch (attrID)
  {
    case BackgroundColour: vtObject_a->backgroundColour = convertLittleEndianStringColour(pui8_attributeValue); break;
    //case SoftKeyMask: vtObject_a->softKeyMask      = convertLittleEndianStringUi16(  pui8_attributeValue); break; //TODO
    default: break;
  }
}

    void vtObjectDataMask_c::setBackgroundColour(IsoAgLib::Colour newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
    	if (b_updateObject)
    		vtObject_a->backgroundColour = newValue;
        setAttribute (BackgroundColour /* "Background Colour" */, __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).getUserConvertedColor (newValue, this, IsoAgLib::BackgroundColour), b_enableReplaceOfCmd);
    }

#endif

} // __IsoAgLib
