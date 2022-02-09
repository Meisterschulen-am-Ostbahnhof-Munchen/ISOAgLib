/*
 * ivtclientconnection_c.cpp
 *
 *  Created on: 01.02.2022
 *      Author: franz
 */

#include "ivtclientconnection_c.h"

namespace IsoAgLib {

bool
iVtClientConnection_c::setUserPreset( bool firstClearAllPAs, const IsoAgLib::iAux2Assignment_c &assignments )
{
  return VtClientConnection_c::setUserPreset( firstClearAllPAs, assignments );
}


bool
iVtClientConnection_c::sendNonVolatileDeleteVersion( const char* versionLabel7chars )
{
  if( versionLabel7chars == NULL )
  {
    isoaglib_header_assert( uploadPoolState().versionLabel() );
    return commandHandler().sendNonVolatileDeleteVersion( uploadPoolState().versionLabel() );
  }
  else
    return commandHandler().sendNonVolatileDeleteVersion( versionLabel7chars );
}

void
iVtClientConnection_c::sendCommandsToBus( bool commandsToBus )
{
  commandHandler().sendCommandsToBus( commandsToBus );
}

void
iVtClientConnection_c::setSendPriority( unsigned priority )
{
  commandHandler().setSendPriority( priority );
}

unsigned
iVtClientConnection_c::getSendPriority() const
{
  return commandHandler().getSendPriority();
}

bool
iVtClientConnection_c::sendCommandUpdateObjectPool (IsoAgLib::iVtObject_c** rppc_vtObjects, uint16_t aui16_numObjects)
{
  return commandHandler().sendCommandUpdateObjectPool (rppc_vtObjects, aui16_numObjects);
}

bool
iVtClientConnection_c::sendCommandChangeActiveMask (uint16_t aui16_objectUid, uint16_t aui16_maskId, bool b_enableReplaceOfCmd)
{
  return commandHandler().sendCommandChangeActiveMask( aui16_objectUid, aui16_maskId, b_enableReplaceOfCmd );
}

bool
iVtClientConnection_c::sendCommandChangeNumericValue (uint16_t aui16_objectUid, uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, bool b_enableReplaceOfCmd)
{
  return commandHandler().sendCommandChangeNumericValue(aui16_objectUid, byte1, byte2, byte3, byte4, b_enableReplaceOfCmd);
}

bool
iVtClientConnection_c::sendCommandChangeAttribute (uint16_t aui16_objectUid, uint8_t attrId, uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, bool b_enableReplaceOfCmd)
{
  return commandHandler().sendCommandChangeAttribute(aui16_objectUid, attrId, byte1, byte2, byte3, byte4, b_enableReplaceOfCmd);
}

bool
iVtClientConnection_c::sendCommandChangeSoftKeyMask  (uint16_t aui16_objectUid, uint8_t maskType, uint16_t newSoftKeyMaskID, bool b_enableReplaceOfCmd)
{
  return commandHandler().sendCommandChangeSoftKeyMask(aui16_objectUid, maskType, newSoftKeyMaskID, b_enableReplaceOfCmd);
}

bool
iVtClientConnection_c::sendCommandChangeStringValue  (uint16_t aui16_objectUid, const char* apc_newValue, uint16_t overrideSendLength, bool b_enableReplaceOfCmd)
{
  return commandHandler().sendCommandChangeStringValue(aui16_objectUid, apc_newValue, overrideSendLength, b_enableReplaceOfCmd);
}

bool
iVtClientConnection_c::sendCommandChangeChildPosition (uint16_t aui16_objectUid, uint16_t aui16_childObjectUid, int16_t x, int16_t y, bool b_enableReplaceOfCmd)
{
  return commandHandler().sendCommandChangeChildPosition(aui16_objectUid, aui16_childObjectUid, x, y, b_enableReplaceOfCmd);
}

bool
iVtClientConnection_c::sendCommandChangeChildLocation (uint16_t aui16_objectUid, uint16_t aui16_childObjectUid, int16_t dx, int16_t dy, bool b_enableReplaceOfCmd)
{
  return commandHandler().sendCommandChangeChildLocation(aui16_objectUid, aui16_childObjectUid, dx, dy, b_enableReplaceOfCmd);
}

bool
iVtClientConnection_c::sendCommandChangeBackgroundColour (uint16_t aui16_objectUid, Colour newColour,  bool b_enableReplaceOfCmd)
{
  return commandHandler().sendCommandChangeBackgroundColour(aui16_objectUid, newColour,  b_enableReplaceOfCmd);
}

bool
iVtClientConnection_c::sendCommandChangeSize             (uint16_t aui16_objectUid, uint16_t newWidth, uint16_t newHeight, bool b_enableReplaceOfCmd)
{
  return commandHandler().sendCommandChangeSize(aui16_objectUid, newWidth, newHeight, b_enableReplaceOfCmd);
}

#ifdef CONFIG_USE_VTOBJECT_fillattributes
bool
iVtClientConnection_c::sendCommandChangeFillAttributes (uint16_t aui16_objectUid, uint8_t newFillType, Colour newFillColour, IsoAgLib::iVtObjectPictureGraphic_c* newFillPatternObject, bool b_enableReplaceOfCmd)
{
  return commandHandler().sendCommandChangeFillAttributes(aui16_objectUid, newFillType, newFillColour, newFillPatternObject, b_enableReplaceOfCmd);
}
#endif

bool
iVtClientConnection_c::sendCommandChangeFontAttributes (uint16_t aui16_objectUid, Colour newFontColour, Font newFontSize, uint8_t newFontType, uint8_t newFontStyle, bool b_enableReplaceOfCmd)
{
  return commandHandler().sendCommandChangeFontAttributes(aui16_objectUid, newFontColour, newFontSize, newFontType, newFontStyle, b_enableReplaceOfCmd);
}

bool
iVtClientConnection_c::sendCommandChangeLineAttributes (uint16_t aui16_objectUid, Colour newLineColour, uint8_t newLineWidth, uint16_t newLineArt, bool b_enableReplaceOfCmd)
{
  return commandHandler().sendCommandChangeLineAttributes(aui16_objectUid, newLineColour, newLineWidth, newLineArt, b_enableReplaceOfCmd);
}

bool
iVtClientConnection_c::sendCommandHideShow (uint16_t aui16_objectUid, uint8_t b_hideOrShow, bool b_enableReplaceOfCmd)
{
  return commandHandler().sendCommandHideShow(aui16_objectUid, b_hideOrShow, b_enableReplaceOfCmd);
}

bool
iVtClientConnection_c::sendCommandEsc (bool b_enableReplaceOfCmd)
{
  return commandHandler().sendCommandEsc(b_enableReplaceOfCmd);
}

bool
iVtClientConnection_c::sendCommandSelectInputObject (uint16_t aui16_objectUid, bool b_activate, bool b_enableReplaceOfCmd)
{
  return commandHandler().sendCommandSelectInputObject(aui16_objectUid, b_activate, b_enableReplaceOfCmd);
}



}
