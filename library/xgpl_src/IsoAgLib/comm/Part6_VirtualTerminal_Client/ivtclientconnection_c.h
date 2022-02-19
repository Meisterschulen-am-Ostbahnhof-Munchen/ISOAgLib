/*
  ivtclientconnection.h: Class for managing a connection

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
  void sendCommandsToBus( bool commandsToBus );

  // specify in which command-queue all following commands will be placed.
  // 0 is the highest priority!
  // Range is [0..CONFIG_VT_CLIENT_NUM_SEND_PRIORITIES-1]
  void setSendPriority( unsigned priority );
  unsigned getSendPriority() const;

  bool sendCommandChangeActiveMask       (uint16_t aui16_objectUid, uint16_t aui16_maskId, bool b_enableReplaceOfCmd = true );
  bool sendCommandChangeNumericValue     (uint16_t aui16_objectUid, uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, bool b_enableReplaceOfCmd=true);
  bool sendCommandChangeAttribute        (uint16_t aui16_objectUid, uint8_t attrId, uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, bool b_enableReplaceOfCmd=true);
  bool sendCommandChangeSoftKeyMask      (uint16_t aui16_objectUid, uint8_t maskType, uint16_t newSoftKeyMaskID, bool b_enableReplaceOfCmd=true);
  bool sendCommandChangeStringValue      (uint16_t aui16_objectUid, const char* apc_newValue, uint16_t overrideSendLength, bool b_enableReplaceOfCmd=true);
  bool sendCommandChangeChildPosition    (uint16_t aui16_objectUid, uint16_t aui16_childObjectUid, int16_t x, int16_t y, bool b_enableReplaceOfCmd=true);
  bool sendCommandChangeChildLocation    (uint16_t aui16_objectUid, uint16_t aui16_childObjectUid, int16_t dx, int16_t dy, bool b_enableReplaceOfCmd=true);
  bool sendCommandChangeBackgroundColour (uint16_t aui16_objectUid, Colour newColour,  bool b_enableReplaceOfCmd=true);
  bool sendCommandChangeSize             (uint16_t aui16_objectUid, uint16_t newWidth, uint16_t newHeight, bool b_enableReplaceOfCmd=true);
#ifdef CONFIG_USE_VTOBJECT_fillattributes
#ifdef CONFIG_USE_VTOBJECT_picturegraphic
  bool sendCommandChangeFillAttributes   (uint16_t aui16_objectUid, uint8_t newFillType, Colour newFillColour, iVtObjectPictureGraphic_c* newFillPatternObject, bool b_enableReplaceOfCmd=true);
#endif
#endif
  bool sendCommandChangeFontAttributes   (uint16_t aui16_objectUid, Colour newFontColour, Font newFontSize, uint8_t newFontType, uint8_t newFontStyle, bool b_enableReplaceOfCmd=true);
  bool sendCommandChangeLineAttributes   (uint16_t aui16_objectUid, Colour newLineColour, uint8_t newLineWidth, uint16_t newLineArt, bool b_enableReplaceOfCmd=true);
  bool sendCommandHideShow               (uint16_t aui16_objectUid, uint8_t b_hideOrShow, bool b_enableReplaceOfCmd=true);
  bool sendCommandSelectInputObject      (uint16_t aui16_objectUid, bool b_activate, bool b_enableReplaceOfCmd=true);

  /**
   * After assignments have been initially loaded, the application can activate some "presets"
   * In case of error, the other functions that were found were still set!
   * Three actions are performed independently:
   * 1. if firstClearAllPAs is set, clear ALL current PA entries. This works even if the "assignments" list is empty.
   * 2. set all PAs from the "assignments" list. If the list is empty, nothing is set.
   * 3. The callback "storePreferredAux2Assignment" is called for each affected function.
   * @param firstClearAllPAs before setting the assignments, clear all assignments first
   * @param assignments A list of UserPreset-/Preferred-Assignments.
   * @return TRUE: All functions were found/valid, FALSE: One function wasn't valid!
   */
  bool setUserPreset( bool firstClearAllPAs, const iAux2Assignment_c &assignments );

  unsigned getCommandQueueSize() const { return commandHandler().getQueueSize(); }
  unsigned getCommandQueueSize( unsigned priority ) const { return commandHandler().getQueueSize( priority ); }

  //! @param versionLabel7chars == NULL: Use VersionLabel used for Uploading/Loading (must be given at init!)
  //!                                    This includes the language-code for multi-language pools!
  //!        versionLabel7chars != NULL: Use VersionLabel given. Must be 7 characters!
  bool sendNonVolatileDeleteVersion( const char* versionLabel7chars );
  bool sendCommandEsc (bool b_enableReplaceOfCmd=true);
  bool sendCommandUpdateObjectPool (iVtObject_c** rppc_vtObjects, uint16_t aui16_numObjects);

  uint8_t getClientId() const { return VtClientConnection_c::getClientId(); }
  bool isClientActive() const { return VtClientConnection_c::isClientActive(); }

  iIdentItem_c& getIdentItem() const { return static_cast<iIdentItem_c&>(VtClientConnection_c::getIdentItem()); }
  bool connectedToVtServer() const   { return VtClientConnection_c::connectedToVtServer(); }
  /** ATTENTION: Please assure "connectedToVtServer()" before getting this reference */
  iVtServerInstance_c* getVtServerInstPtr() const { return (VtClientConnection_c::getVtServerInstPtr() != NULL) ? VtClientConnection_c::getVtServerInst().toIvtServerInstancePtr_c() : NULL; }
  iVtServerInstance_c& getVtServerInst() const    { return VtClientConnection_c::getVtServerInst().toIvtServerInstance_c(); }

  /**
   * Start "move to next VT" procedure by sending the "delete object pool"
   *
   * return TRUE:  message sending was successfully
   * return FALSE: message sending failed
   *               reasons: a) sendCommandsToBus(false) is active
   *                        b) no connection to VT server
   **/
  bool moveToNextVt() { return VtClientConnection_c::moveToNextVt(); }

  bool disconnectFromVt() { return VtClientConnection_c::disconnectFromVt(); }

  // Note: This function will clear the stored timed out command after returning it!
  // @return 0x00: No command timed (or already checked and reset)
  //      != 0x00: The command that timed out and was the reason for a reconnect!
  uint8_t getAndResetLastTimedOutCommand() { return VtClientConnection_c::getAndResetLastTimedOutCommand(); }
  bool isDisconnectedForShutdown() const { return VtClientConnection_c::isDisconnectedForShutdown(); }

  uint8_t getVersion() const { return VtClientConnection_c::getVersion(); }

private:
  iVtClientConnection_c();

  friend class iVtClient_c;
  friend class __IsoAgLib::VtClientConnection_c;
};


} // IsoAgLib

#endif
