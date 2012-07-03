/*
  ivtclientservercommunication.h: Interface class for vt-client-server
    communication

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef IVT_CLIENT_SERVER_COMMUNICATION_H
#define IVT_CLIENT_SERVER_COMMUNICATION_H

//include the declaration of the internal object
#include "impl/vtclientservercommunication_c.h"
#include "iisoterminal_c.h"
#include "ivtserverinstance_c.h"

// Begin Namespace IsoAgLib
namespace IsoAgLib {

/** Instance Object for one VT-Client-Server communication channel
  @author Dipl.-Inf. Martin Wodok
  */
class iVtClientServerCommunication_c : private __IsoAgLib::VtClientServerCommunication_c
{
public:

  bool isVtActive() { return VtClientServerCommunication_c::isVtActive(); }

  /* enable/disable sending of commands to the VT over the ISOBUS.
     defaults to true, can be temporarily switched OFF (false) to locally
     modify the objects, partial pool update them and then switched
     ON (true) again. */     
  void sendCommandsToBus( bool commandsToBus ) { return VtClientServerCommunication_c::sendCommandsToBus( commandsToBus ); }

  bool sendCommandDeleteObjectPool()
  {return VtClientServerCommunication_c::sendCommandDeleteObjectPool();}

  bool sendCommandUpdateObjectPool (IsoAgLib::iVtObject_c** rppc_vtObjects, uint16_t aui16_numObjects)
    { return VtClientServerCommunication_c::sendCommandUpdateObjectPool (rppc_vtObjects, aui16_numObjects); }

  bool sendCommandChangeActiveMask (uint16_t aui16_objectUid, uint16_t aui16_maskId, bool b_enableReplaceOfCmd = true )
    {return VtClientServerCommunication_c::sendCommandChangeActiveMask( aui16_objectUid, aui16_maskId, b_enableReplaceOfCmd ); }
  bool sendCommandChangeNumericValue (uint16_t aui16_objectUid, uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, bool b_enableReplaceOfCmd=true)
    {return VtClientServerCommunication_c::sendCommandChangeNumericValue(aui16_objectUid, byte1, byte2, byte3, byte4, b_enableReplaceOfCmd);}
  bool sendCommandChangeAttribute    (uint16_t aui16_objectUid, uint8_t attrId, uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, bool b_enableReplaceOfCmd=true)
    {return VtClientServerCommunication_c::sendCommandChangeAttribute(aui16_objectUid, attrId, byte1, byte2, byte3, byte4, b_enableReplaceOfCmd);}
  bool sendCommandChangeSoftKeyMask  (uint16_t aui16_objectUid, uint8_t maskType, uint16_t newSoftKeyMaskID, bool b_enableReplaceOfCmd=true)
    {return VtClientServerCommunication_c::sendCommandChangeSoftKeyMask(aui16_objectUid, maskType, newSoftKeyMaskID, b_enableReplaceOfCmd);}
  bool sendCommandChangeStringValue  (uint16_t aui16_objectUid, const char* apc_newValue, uint16_t overrideSendLength, bool b_enableReplaceOfCmd=true)
    {return VtClientServerCommunication_c::sendCommandChangeStringValue(aui16_objectUid, apc_newValue, overrideSendLength, b_enableReplaceOfCmd);}
  bool sendCommandChangeChildPosition (uint16_t aui16_objectUid, uint16_t aui16_childObjectUid, int16_t x, int16_t y, bool b_enableReplaceOfCmd=true)
    {return VtClientServerCommunication_c::sendCommandChangeChildPosition(aui16_objectUid, aui16_childObjectUid, x, y, b_enableReplaceOfCmd);}
  bool sendCommandChangeChildLocation (uint16_t aui16_objectUid, uint16_t aui16_childObjectUid, int16_t dx, int16_t dy, bool b_enableReplaceOfCmd=true)
    {return VtClientServerCommunication_c::sendCommandChangeChildLocation(aui16_objectUid, aui16_childObjectUid, dx, dy, b_enableReplaceOfCmd);}
  bool sendCommandChangeBackgroundColour (uint16_t aui16_objectUid, uint8_t newColour,  bool b_enableReplaceOfCmd=true)
    {return VtClientServerCommunication_c::sendCommandChangeBackgroundColour(aui16_objectUid, newColour,  b_enableReplaceOfCmd);}
  bool sendCommandChangeSize             (uint16_t aui16_objectUid, uint16_t newWidth, uint16_t newHeight, bool b_enableReplaceOfCmd=true)
    {return VtClientServerCommunication_c::sendCommandChangeSize(aui16_objectUid, newWidth, newHeight, b_enableReplaceOfCmd);}
  bool sendCommandChangeFillAttributes (uint16_t aui16_objectUid, uint8_t newFillType, uint8_t newFillColour, IsoAgLib::iVtObjectPictureGraphic_c* newFillPatternObject, bool b_enableReplaceOfCmd=true)
    {return VtClientServerCommunication_c::sendCommandChangeFillAttributes(aui16_objectUid, newFillType, newFillColour, newFillPatternObject, b_enableReplaceOfCmd);}
  bool sendCommandChangeFontAttributes (uint16_t aui16_objectUid, uint8_t newFontColour, uint8_t newFontSize, uint8_t newFontType, uint8_t newFontStyle, bool b_enableReplaceOfCmd=true)
    {return VtClientServerCommunication_c::sendCommandChangeFontAttributes(aui16_objectUid, newFontColour, newFontSize, newFontType, newFontStyle, b_enableReplaceOfCmd);}
  bool sendCommandChangeLineAttributes (uint16_t aui16_objectUid, uint8_t newLineColour, uint8_t newLineWidth, uint16_t newLineArt, bool b_enableReplaceOfCmd=true)
    {return VtClientServerCommunication_c::sendCommandChangeLineAttributes(aui16_objectUid, newLineColour, newLineWidth, newLineArt, b_enableReplaceOfCmd);}
  bool sendCommandHideShow (uint16_t aui16_objectUid, uint8_t b_hideOrShow, bool b_enableReplaceOfCmd=true)
    {return VtClientServerCommunication_c::sendCommandHideShow(aui16_objectUid, b_hideOrShow, b_enableReplaceOfCmd);}
  bool sendCommandEsc (bool b_enableReplaceOfCmd=true)
    {return VtClientServerCommunication_c::sendCommandEsc(b_enableReplaceOfCmd);}

  //! @param versionLabel7chars == NULL: Use VersionLabel used for Uploading/Loading (must be given at init!)
  //!                                    This includes the language-code for multi-language pools!
  //!        versionLabel7chars != NULL: Use VersionLabel given. Must be 7 characters!
  bool sendNonVolatileDeleteVersion( const char* versionLabel7chars )
    { return VtClientServerCommunication_c::sendNonVolatileDeleteVersion( versionLabel7chars ); }

  uint8_t getClientId() const
    { return VtClientServerCommunication_c::getClientId(); }

  bool isClientActive()
    { return VtClientServerCommunication_c::isClientActive(); }

  void setAux2ModelIdentificationCodeForInputDevice(uint16_t a_model)
    { VtClientServerCommunication_c::setAux2ModelIdentificationCodeForInputDevice(a_model); }

  void setAux2WaitTimeForSendingPreferredAssignment(uint32_t a_waitTime)
    { VtClientServerCommunication_c::setAux2WaitTimeForSendingPreferredAssignment(a_waitTime); }

  iIdentItem_c& getIdentItem() const { return static_cast<IsoAgLib::iIdentItem_c&>(VtClientServerCommunication_c::getIdentItem()); }
  bool connectedToVtServer() const   { return VtClientServerCommunication_c::connectedToVtServer(); }
  /** ATTENTION: Please assure "connectedToVtServer()" before getting this reference */
  iVtServerInstance_c* getVtServerInstPtr() const { return (VtClientServerCommunication_c::getVtServerInstPtr() != NULL) ? VtClientServerCommunication_c::getVtServerInst().toIvtServerInstancePtr_c() : NULL; }
  iVtServerInstance_c& getVtServerInst() const    { return VtClientServerCommunication_c::getVtServerInst().toIvtServerInstance_c(); }

private:
  /** PRIVATE constructor to forbid instantiation of this interface class.
    * it can only be static_cast'ed to this class, not constructed!
    */
  iVtClientServerCommunication_c ();

  friend class iIsoTerminal_c;
  friend class __IsoAgLib::VtClientServerCommunication_c;
};

}
#endif
