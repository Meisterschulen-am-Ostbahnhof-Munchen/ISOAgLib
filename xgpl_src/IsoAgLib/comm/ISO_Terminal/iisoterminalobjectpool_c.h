/***************************************************************************
                             iisoterminalobjectpool.h - class for managing an object pool with softkey/button activation event handling
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

// Begin Namespace IsoAgLib
namespace IsoAgLib {
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
    @param ui8_keyActivationCode 0, 1 or 2. In case of a Latchable Button use [BUTTON_HAS_BEEN_UNLATCHED, BUTTON_HAS_BEEN_LATCHED], for Unlatchable Buttons use [BUTTON_HAS_BEEN_PRESSED, BUTTON_IS_STILL_HELD] (notice there's NO BUTTON_HAS_BEEN_RELEASED !]. For keys use one of [KEY_HAS_BEEN_RELEASED, KEY_HAS_BEEN_PRESSED, KEY_IS_STILL_HELD]
    @param rui16_objId ObjectID of the vtObjectButton / vtObjectSoftKey object
    @param rui16_objIdMask ObjectID of the Mask that contains the vtObjectButton / vtObjectSoftKey object
    @param rui8_keyCode KeyCode as defined in the vtObjectButton / vtObjectSoftKey object
    @param rb_wasButton true if it was a button object, false if it was a soft key
  */

  virtual void eventKeyCode (uint8_t rui8_keyActivationCode, uint16_t rui16_objId, uint16_t rui16_objIdMask, uint8_t rui8_keyCode, bool rb_wasButton)=0;
  /**
    hook function that gets called after the ISO_Terminal_c instance
    receives a "VT Change Numeric Value" Message
    @param rui16_objId of the object where the user chaged the value
    @param rui8_value the value in 8bit
    @param rui32_value the (same) value in 32bit
  */
  virtual void eventNumericValue (uint16_t rui16_objId, uint8_t rui8_value, uint32_t rui32_value)=0;

  /**
    hook function that gets called immediately after the
    "End Of Object Pool Response" Message was received.
  */
  virtual void eventObjectPoolUploadedSuccessfully()=0;

  /**
    this init function has to be idempotent!
  */
  virtual void initAllObjectsOnce()=0;
  
  iIsoTerminalObjectPool_c(iVtObject_c** r_iVtObjects, uint16_t r_numObjects, uint16_t r_dimension) :
    iVtObjects (r_iVtObjects), numObjects (r_numObjects), dimension (r_dimension), b_initAllObjects (false) {};
    
protected:
  iVtObject_c** iVtObjects;
  uint16_t numObjects;
  uint16_t dimension;
  bool b_initAllObjects;

public:
  iVtObject_c** getIVtObjects() { return iVtObjects; };
  uint16_t getNumObjects() { return numObjects; };
  uint16_t getDimension() { return dimension; };
    
};

} // end namespace IsoAgLib
#endif
