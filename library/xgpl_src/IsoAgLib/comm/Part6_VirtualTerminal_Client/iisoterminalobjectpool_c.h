/***************************************************************************
                             iisoterminalobjectpool.h - class for managing
                             an object pool with softkey/button activation event handling
                             -------------------
    begin                : Tue Jul 22 2003
    copyright            : (C) 2003 by Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
    type                 : Header
    $LastChangedDate$
    $LastChangedRevision$
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 * This file is part of the "IsoAgLib", an object oriented program library *
 * to serve as a software layer between application specific program and   *
 * communication protocol details. By providing simple function calls for  *
 * jobs like starting a measuring program for a process data value on a    *
 * remote ECU, the main program has not to deal with single CAN telegram   *
 * formatting. This way communication problems between ECU's which use     *
 * this library should be prevented.                                       *
 * Everybody and every company is invited to use this library to make a    *
 * working plug and play standard out of the printed protocol standard.    *
 *                                                                         *
 * Copyright (C) 2000 - 2004 Dipl.-Inform. Achim Spangler                  *
 *                                                                         *
 * The IsoAgLib is free software; you can redistribute it and/or modify it *
 * under the terms of the GNU General Public License as published          *
 * by the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This library is distributed in the hope that it will be useful, but     *
 * WITHOUT ANY WARRANTY; without even the implied warranty of              *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       *
 * General Public License for more details.                                *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with IsoAgLib; if not, write to the Free Software Foundation,     *
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA           *
 *                                                                         *
 * As a special exception, if other files instantiate templates or use     *
 * macros or inline functions from this file, or you compile this file and *
 * link it with other works to produce a work based on this file, this file*
 * does not by itself cause the resulting work to be covered by the GNU    *
 * General Public License. However the source code for this file must still*
 * be made available in accordance with section (3) of the                 *
 * GNU General Public License.                                             *
 *                                                                         *
 * This exception does not invalidate any other reasons why a work based on*
 * this file might be covered by the GNU General Public License.           *
 *                                                                         *
 * Alternative licenses for IsoAgLib may be arranged by contacting         *
 * the main author Achim Spangler by a.spangler@osb-ag:de                  *
 ***************************************************************************/
#ifndef IISOTERMINALOBJECTPOOL_H
#define IISOTERMINALOBJECTPOOL_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */

#include "ivttypes.h"
#include <supplementary_driver/driver/datastreams/streaminput_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/iisoname_c.h>
//#include <IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/isoterminal_c.h>

/* *************************************** */
/* ********** command defines ************ */
/* *************************************** */

// Notice: There's no notification when a button gets released!
//#define BUTTON_HAS_BEEN_UNLATCHED 0
//#define BUTTON_HAS_BEEN_PRESSED 1
//#define BUTTON_HAS_BEEN_LATCHED 1
//#define BUTTON_IS_STILL_HELD 2

//#define KEY_HAS_BEEN_RELEASED 0
//#define KEY_HAS_BEEN_PRESSED 1
//#define KEY_IS_STILL_HELD 2

// forward declaration
struct localSettings_s;

// Begin Namespace IsoAgLib
namespace IsoAgLib {
  class iStream_c;
/**
  @brief This class is needed to handle Terminal KeyCodes (SoftKey or Button) and Numeric Value Changes and also
  gives you the possibility to perform some action right after successfull objectpool uploading.

  For how to specify an ISO VT Object Pool please refer to \ref XMLspec .

  If you're just interested in _PRESSED events, you can simply leave out the "if (wasButton)".
  \n
  Example framework for implementing eventKeyCode:
  \code
  if (wasButton) {
    switch (keyCode) {
      case vtKeyCodeLatchableTestButton:
        switch (keyActivationCode) {
          case BUTTON_HAS_BEEN_LATCHED:
            // handle latching of vtObjectLatchableTestButton
            break;
          case BUTTON_HAS_BEEN_UNLATCHED:
            // handle unlatching of vtObjectLatchableTestButton
            break;
        }
        break;
      case vtKeyCodeSimpleTestButton:
        switch (keyActivationCode) {
          case BUTTON_HAS_BEEN_PRESSED:
            // handle pressing of vtObjectSimpleTestButton
            break;
          case BUTTON_IS_STILL_HELD:
            // handle holding of vtObjectSimpleTestButton
            break;
          // case BUTTON_HAS_BEEN_RELEASED:
          //   this one is SPECIFIED TO NOT BE POSSIBLE in ISO 11783!
        }
        break;
      // [...]
    }
  } else { // wasKey
    switch (keyCode) {
      case vtKeyCodeSimpleTestKey:
        switch (keyActivationCode) {
          case KEY_HAS_BEEN_PRESSED:
            // handle pressing of vtObjectSimpleTestKey
            break;
          case KEY_HAS_BEEN_RELEASED:
            // handle release of vtObjectSimpleTestKey
            break;
          case KEY_IS_STILL_HELD:
            // handle holding of vtObjectSimpleTestKey
            break;
        }
        break;
      // [...]
    }
  }
  \endcode

  Example framework for implementing eventNumericValue:
  \code
  switch (objId) {
    case vtObjectIDMyByteInput1:
      doMyInputNumber1action (ui8_value);
      break;
    case vtObjectIDMyIntInput2:
      doMyInputNumber2action (ui32_value);
      break;
    // [...]
  }
  \endcode

  Example for implementing eventObjectPoolUploadedSuccessfully:
  \code
  // Now send some initial variables and then show main mask.
  vtObjectMySpeed.setValue (12);
  vtObjectMyDistance.setValue (2700);

  vtObjectMyWorkingSet.showMask (&vtObjectMyMainDataMask);
  \endcode

  @author Dipl.-Inform. Achim Spangler
*/

class iIsoTerminalObjectPool_c {

public:

  /**
    hook function that gets called after the ISO_Terminal_c instance
    receives a "Soft Key Activation" / "Button Activation" Message
    @param aui8_keyActivationCode 0, 1 or 2. In case of a Latchable Button use [BUTTON_HAS_BEEN_UNLATCHED, BUTTON_HAS_BEEN_LATCHED], for Unlatchable Buttons use [BUTTON_HAS_BEEN_PRESSED, BUTTON_IS_STILL_HELD] (notice there's NO BUTTON_HAS_BEEN_RELEASED !]. For keys use one of [KEY_HAS_BEEN_RELEASED, KEY_HAS_BEEN_PRESSED, KEY_IS_STILL_HELD]
    @param aui16_objId ObjectID of the vtObjectButton / vtObjectSoftKey object
    @param aui16_objIdMask ObjectID of the Mask that contains the vtObjectButton / vtObjectSoftKey object
    @param aui8_keyCode KeyCode as defined in the vtObjectButton / vtObjectSoftKey object
    @param ab_wasButton true if it was a button object, false if it was a soft key
  */
  virtual void eventKeyCode (uint8_t aui8_keyActivationCode, uint16_t aui16_objId, uint16_t aui16_objIdMask, uint8_t aui8_keyCode, bool ab_wasButton)=0;

  /**
    hook function that gets called after the ISO_Terminal_c instance
    receives a "Pointing Event" Message
    <!--@param aui16_xPosition of where the user has clicked (besides buttons and input objects) or touched
    @param aui16_yPosition of where the user has clicked (besides buttons and input objects) or touched-->
  */
  virtual void eventPointingEvent (uint16_t /*aui16_xPosition*/, uint16_t /*aui16_yPosition*/) {}

  /**
    hook function that gets called after the ISO_Terminal_c instance
    receives a "VT Change Numeric Value" Message
    @param aui16_objId of the object where the user changed the value
    @param aui8_value the value in 8bit
    @param aui32_value the (same) value in 32bit
  */
  virtual void eventNumericValue (uint16_t aui16_objId, uint8_t aui8_value, uint32_t aui32_value)=0;

  /**
    hook function that gets called after the ISO_Terminal_c instance
    receives a "Input String Value" Message
    @param aui16_objId of the object where the user changed the value
    @param aui8_length the length in 8bit
    @param rc_streaminput reference to streaminput
    @param aui8_unparsedBytes
    @param b_isFirst
    @param b_isLast
  */
  virtual void eventStringValue (uint16_t aui16_objId, uint8_t aui8_length, StreamInput_c &rc_streaminput, uint8_t aui8_unparsedBytes, bool b_isFirst, bool b_isLast)=0;

  /**
    the abort function is implemented empty by default because
    it is only needed for on-the-fly parsing of the string value
  */
  virtual void eventStringValueAbort() {}

  /**
    *  the client received message vt select input object
    *  @param uint16_t aui16_objectId The object ID of the selected object
    *  @param uint8_t aui8_hasFocus byte 4 of the message, 0 means the object lost focus, 1 means the object received focus
    *  @param uint8_t aui8_selected byte 5 of the message, 0 means the object is not open/selected for input, 1  means the object is open/selected for input
    */
  virtual void eventVtSelectInputObject(uint16_t /*aui16_objectId*/, uint8_t /*aui8_hasFocus*/, uint8_t /*aui8_selected*/) { }

  /**
    hook function that gets called immediately after the
    "End Of Object Pool Response" Message was received.
    @param ab_wasLanguageUpdate TRUE if the object pool was updated while it was already active/being displayed.
                                FALSE if the object pool was initially uploaded
    @param ai8_languageIndex -1 if a non-supported language was selected (and hence the default language (index 0) has been uploaded/updated)
                             0..(n-1) for the index to the supported languages. 0 is the first (=default) language. 1 the second, etc.
    @param aui16_languageCode the language code of the uploaded language (one of your supported languages!)
  */
  virtual void eventObjectPoolUploadedSuccessfully (bool ab_wasLanguageUpdate, int8_t ai8_languageIndex, uint16_t aui16_languageCode)=0;


  /**
    This function is called right before a language update is being sent to the VT,
    so the application has a chance to e.g. switch to a "Wait while updating language..." datamask
    before the object pool is being updated...
    <!--@param ai8_languageIndex -1 if a non-supported language was selected (and hence the default language (index 0) will be uploaded/updated)
                             0..(n-1) for the index to the supported languages. 0 is the first (=default) language. 1 the second, etc.
    @param aui16_languageCode the language code of the afterwards being uploaded language (one of your supported languages!)-->
  */
  virtual void eventPrepareForLanguageChange (int8_t /*ai8_languageIndex*/, uint16_t /*aui16_languageCode*/) {}

  /**
    hook function that gets called immediately after recognizing
    the loss of the VT (VT Status Msg not coming for >= 3secs.)
  */
  virtual void eventEnterSafeState()=0;

  /**
    hook function that gets called immediately after recognizing the success/error
    of a command-response message. please keep the implementation short as
    this is directly called from IsoTerminal_c's processMsg();
  */
  virtual void eventCommandResponse(uint8_t /*aui8_responseCommandError*/, const uint8_t /*apui8_responseDataBytes*/[8]) {}

  /**
    hook function that gets called immediately after recognizing an incoming LANGUAGE_PGN.
    react on any change by adjusting your pool to the new UNITS.
    please keep the implementation short as this is directly called from IsoTerminal_c's processMsg();
    for changing the LANGUAGE please refer to "eventObjectPoolUploadedSuccessfully".
    VERY IMPORTANT: THIS FUNCTION CALL MAY OCCUR PRIOR TO AN SUCCESSFULLY UPLOADED POOL !!!!!!!
  */
  virtual void eventLanguagePgn(const localSettings_s& /*ars_localSettings*/) {}

  /**
    hook function that gets called immediately after recognizing an incoming
    VT status message.
  */
  virtual void eventVtStatusMsg() {}

  /**
    hook function that gets called immediately after recognizing an incoming
    VT ESC.
   */
  virtual void eventVtESC() {}

  /**
    hook function that gets called immediately after recognizing an incoming
    Auxiliary Input Status message - but only if you have a function assigned to it!
   */
  virtual void eventAuxFunctionValue (uint16_t /*mui16_functionUid*/,
                                      uint16_t /*cui16_inputValueAnalog*/,
                                      uint16_t /*cui16_inputValueTransitions*/,
                                      uint8_t  /*cui8_inputValueDigital*/) {}

  enum display_status {
    ds_wsm_hidden = 0,
    ds_wsm_inactive = 1,
  };

  /**
    hook function that gets called immediately after recognizing an incoming
    Display Activation Msg or Vt Status Msg with display state change.
   */
  void eventDisplayActivation (
    enum display_status display_status,
    uint16_t visible_mask_oid,
    uint16_t visible_skm_oid
  ) {
    fprintf (stderr, "iisoterminalobjectpool: display_status %d, visible_mask_oid %d, visible_skm_oid %d\n", display_status, visible_mask_oid, visible_skm_oid);
  }

  /**
     Gets called after recognizing an incoming VT proprietary message.
   */
  uint8_t eventProprietaryCommand(iIsoName_c const &acr_isoname) { return doEventProprietaryCommand(acr_isoname); };

  /**
  Gets called after recognizing an incoming VT proprietary message.
   */
  uint8_t eventProprietaryCommand(iIsoName_c const &acr_isoname, uint8_t aui8_commandByte, IsoAgLib::iStream_c& arc_stream)
  { return doEventProprietaryCommand(acr_isoname, aui8_commandByte, arc_stream); };

  /**
    this init function has to be idempotent! use "b_initAllObjects" for this reason, it's initialized to false at construction time.
  */
  virtual void initAllObjectsOnce(SINGLETON_VEC_KEY_PARAMETER_DEF)=0;

  iIsoTerminalObjectPool_c(iVtObject_c*HUGE_MEM** a_iVtObjects, uint16_t a_numObjects, uint16_t a_numObjectsLang, uint16_t a_dimension, uint16_t a_skWidth=60, uint16_t a_skHeight=32)
    : iVtObjects (a_iVtObjects)
    , numObjects (a_numObjects)
    , numObjectsLang (a_numObjectsLang)
    , dimension (a_dimension)
    , skWidth (a_skWidth)
    , skHeight (a_skHeight)
    , b_initAllObjects (false)
  {
    numLang=0;
    iVtObject_c*HUGE_MEM** iter = a_iVtObjects+1; // first entry should be the general object pool part!
    while (*iter++ != NULL) numLang++;
  };

   virtual ~iIsoTerminalObjectPool_c() {}

  /**
    hook function that gets called every time a color-value
    has to be adapted to VT's color-depth (as it violates the color-range!).
    --> Standard implementation will simply clip all color-values to
    BLACK (Color 0) besides the background/transparency colors to WHITE (Color 1)
    Please overload this function if other behaviour is wanted
    <!--@param colorValue The color-value that was originally defined in the object
    @param colorDepth 0 for 1bit-color depth (2-colored VT, black/white)
                      1 for 4bit-color depth (16-colored VT)
                      2 for 8bit-color depth (256-colored VT)
    @param obj Reference to the object that's color's to be converted, use it for distinguishing a little more...-->
    @param whichColour Type of colour: BackgroundColour, LineColour, NeedleColour, etc. (See IsoAgLib::e_vtColour)
  */
  virtual uint8_t convertColour(uint8_t /* colorValue */, uint8_t /* colorDepth */, IsoAgLib::iVtObject_c* /* obj */, IsoAgLib::e_vtColour whichColour)
  {
    if ((whichColour == BackgroundColour) || (whichColour == TransparencyColour))
      return 1; /* white - std. background/transparency colour */
    else
      return 0; /* black - std. drawing colour */
  };
  /**
    hook function that gets called immediately after recognizing the success of a command-response message
    for Get Attribute Value command
  */
  virtual void eventAttributeValue (IsoAgLib::iVtObject_c* /*obj*/, uint8_t /*ui8_attributeValue*/, uint8_t* /*pui8_value*/) {}
private:
  /**
     hook function that gets called after recognizing an incoming
     VT proprietary message.
   */
  virtual uint8_t doEventProprietaryCommand(iIsoName_c const &/*acr_isoname*/) { return 0; }
  /**
    hook function that gets called after recognizing an incoming VT proprietary message.
   */
  virtual uint8_t doEventProprietaryCommand(iIsoName_c const &/*acr_isoname*/, uint8_t /*aui8_commandByte*/, IsoAgLib::iStream_c& /*arc_stream*/)  { return 0; };

protected:
  iVtObject_c*HUGE_MEM** iVtObjects;
  uint16_t numObjects;
  uint16_t numObjectsLang;
  uint16_t dimension;
  uint16_t skWidth;
  uint16_t skHeight;
  bool b_initAllObjects;
  uint8_t numLang;

public:
  iVtObject_c*HUGE_MEM**
            getIVtObjects()     const { return iVtObjects; }
  uint16_t  getNumObjects()     const { return numObjects; }
  uint16_t  getNumObjectsLang() const { return numObjectsLang; }
  uint16_t  getDimension()      const { return dimension; }
  uint16_t  getSkWidth()        const { return skWidth; }
  uint16_t  getSkHeight()       const { return skHeight; }
  uint8_t   getNumLang()        const { return numLang; }

  iVtObjectWorkingSet_c&
  getWorkingSetObject() const { return *(iVtObjectWorkingSet_c*)(**iVtObjects); }
};

} // end namespace IsoAgLib
#endif
