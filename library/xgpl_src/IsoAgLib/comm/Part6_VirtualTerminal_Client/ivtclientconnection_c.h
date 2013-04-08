/*
  ivtclientconnection.h: Class for managing a connection

  (C) Copyright 2009 - 2013 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef IVTCLIENTCONNECTION_H
#define IVTCLIENTCONNECTION_H

#include "impl/vtclientconnection_c.h"
#include "ivtclient_c.h"
#include "ivtserverinstance_c.h"

namespace IsoAgLib {

class iVtClientConnection_c : private __IsoAgLib::VtClientConnection_c
{
public:

  bool isVtActive() { return VtClientConnection_c::isVtActive(); }

  /* enable/disable sending of commands to the VT over the ISOBUS.
     defaults to true, can be temporarily switched OFF (false) to locally
     modify the objects, partial pool update them and then switched
     ON (true) again. */     
  void sendCommandsToBus( bool commandsToBus ) { return VtClientConnection_c::sendCommandsToBus( commandsToBus ); }

  bool sendCommandDeleteObjectPool()
  {return VtClientConnection_c::sendCommandDeleteObjectPool();}

  bool sendCommandUpdateObjectPool (IsoAgLib::iVtObject_c** rppc_vtObjects, uint16_t aui16_numObjects)
    { return VtClientConnection_c::sendCommandUpdateObjectPool (rppc_vtObjects, aui16_numObjects); }

  bool sendCommandChangeActiveMask (uint16_t aui16_objectUid, uint16_t aui16_maskId, bool b_enableReplaceOfCmd = true )
    {return VtClientConnection_c::sendCommandChangeActiveMask( aui16_objectUid, aui16_maskId, b_enableReplaceOfCmd ); }
  bool sendCommandChangeNumericValue (uint16_t aui16_objectUid, uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, bool b_enableReplaceOfCmd=true)
    {return VtClientConnection_c::sendCommandChangeNumericValue(aui16_objectUid, byte1, byte2, byte3, byte4, b_enableReplaceOfCmd);}
  bool sendCommandChangeAttribute    (uint16_t aui16_objectUid, uint8_t attrId, uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, bool b_enableReplaceOfCmd=true)
    {return VtClientConnection_c::sendCommandChangeAttribute(aui16_objectUid, attrId, byte1, byte2, byte3, byte4, b_enableReplaceOfCmd);}
  bool sendCommandChangeSoftKeyMask  (uint16_t aui16_objectUid, uint8_t maskType, uint16_t newSoftKeyMaskID, bool b_enableReplaceOfCmd=true)
    {return VtClientConnection_c::sendCommandChangeSoftKeyMask(aui16_objectUid, maskType, newSoftKeyMaskID, b_enableReplaceOfCmd);}
  bool sendCommandChangeStringValue  (uint16_t aui16_objectUid, const char* apc_newValue, uint16_t overrideSendLength, bool b_enableReplaceOfCmd=true)
    {return VtClientConnection_c::sendCommandChangeStringValue(aui16_objectUid, apc_newValue, overrideSendLength, b_enableReplaceOfCmd);}
  bool sendCommandChangeChildPosition (uint16_t aui16_objectUid, uint16_t aui16_childObjectUid, int16_t x, int16_t y, bool b_enableReplaceOfCmd=true)
    {return VtClientConnection_c::sendCommandChangeChildPosition(aui16_objectUid, aui16_childObjectUid, x, y, b_enableReplaceOfCmd);}
  bool sendCommandChangeChildLocation (uint16_t aui16_objectUid, uint16_t aui16_childObjectUid, int16_t dx, int16_t dy, bool b_enableReplaceOfCmd=true)
    {return VtClientConnection_c::sendCommandChangeChildLocation(aui16_objectUid, aui16_childObjectUid, dx, dy, b_enableReplaceOfCmd);}
  bool sendCommandChangeBackgroundColour (uint16_t aui16_objectUid, uint8_t newColour,  bool b_enableReplaceOfCmd=true)
    {return VtClientConnection_c::sendCommandChangeBackgroundColour(aui16_objectUid, newColour,  b_enableReplaceOfCmd);}
  bool sendCommandChangeSize             (uint16_t aui16_objectUid, uint16_t newWidth, uint16_t newHeight, bool b_enableReplaceOfCmd=true)
    {return VtClientConnection_c::sendCommandChangeSize(aui16_objectUid, newWidth, newHeight, b_enableReplaceOfCmd);}
  bool sendCommandChangeFillAttributes (uint16_t aui16_objectUid, uint8_t newFillType, uint8_t newFillColour, IsoAgLib::iVtObjectPictureGraphic_c* newFillPatternObject, bool b_enableReplaceOfCmd=true)
    {return VtClientConnection_c::sendCommandChangeFillAttributes(aui16_objectUid, newFillType, newFillColour, newFillPatternObject, b_enableReplaceOfCmd);}
  bool sendCommandChangeFontAttributes (uint16_t aui16_objectUid, uint8_t newFontColour, uint8_t newFontSize, uint8_t newFontType, uint8_t newFontStyle, bool b_enableReplaceOfCmd=true)
    {return VtClientConnection_c::sendCommandChangeFontAttributes(aui16_objectUid, newFontColour, newFontSize, newFontType, newFontStyle, b_enableReplaceOfCmd);}
  bool sendCommandChangeLineAttributes (uint16_t aui16_objectUid, uint8_t newLineColour, uint8_t newLineWidth, uint16_t newLineArt, bool b_enableReplaceOfCmd=true)
    {return VtClientConnection_c::sendCommandChangeLineAttributes(aui16_objectUid, newLineColour, newLineWidth, newLineArt, b_enableReplaceOfCmd);}
  bool sendCommandHideShow (uint16_t aui16_objectUid, uint8_t b_hideOrShow, bool b_enableReplaceOfCmd=true)
    {return VtClientConnection_c::sendCommandHideShow(aui16_objectUid, b_hideOrShow, b_enableReplaceOfCmd);}
  bool sendCommandEsc (bool b_enableReplaceOfCmd=true)
    {return VtClientConnection_c::sendCommandEsc(b_enableReplaceOfCmd);}

  unsigned getCommandQueueSize() const
    { return VtClientConnection_c::getUploadBufferSize(); }

  //! @param versionLabel7chars == NULL: Use VersionLabel used for Uploading/Loading (must be given at init!)
  //!                                    This includes the language-code for multi-language pools!
  //!        versionLabel7chars != NULL: Use VersionLabel given. Must be 7 characters!
  bool sendNonVolatileDeleteVersion( const char* versionLabel7chars )
    { return VtClientConnection_c::sendNonVolatileDeleteVersion( versionLabel7chars ); }

  uint8_t getClientId() const
    { return VtClientConnection_c::getClientId(); }

  bool isClientActive()
    { return VtClientConnection_c::isClientActive(); }

  iIdentItem_c& getIdentItem() const { return static_cast<IsoAgLib::iIdentItem_c&>(VtClientConnection_c::getIdentItem()); }
  bool connectedToVtServer() const   { return VtClientConnection_c::connectedToVtServer(); }
  /** ATTENTION: Please assure "connectedToVtServer()" before getting this reference */
  iVtServerInstance_c* getVtServerInstPtr() const { return (VtClientConnection_c::getVtServerInstPtr() != NULL) ? VtClientConnection_c::getVtServerInst().toIvtServerInstancePtr_c() : NULL; }
  iVtServerInstance_c& getVtServerInst() const    { return VtClientConnection_c::getVtServerInst().toIvtServerInstance_c(); }

private:
  /** PRIVATE constructor to forbid instantiation of this interface class.
    * it can only be static_cast'ed to this class, not constructed!
    */
  iVtClientConnection_c ();

  friend class iVtClient_c;
  friend class __IsoAgLib::VtClientConnection_c;
};

}
#endif
