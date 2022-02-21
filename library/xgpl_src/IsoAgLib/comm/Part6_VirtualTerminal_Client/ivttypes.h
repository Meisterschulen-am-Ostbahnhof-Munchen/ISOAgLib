/*
  ivttypes.h

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
#ifndef IVTTYPES_H
#define IVTTYPES_H

#include <IsoAgLib/isoaglib_config.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/iisoname_c.h>
#include <cstdlib>       // using abs()
#include <algorithm>     // using min() max()
#include <list>
#include "impl/vttypes.h"

enum KeyActivationCode : uint8_t {
	BUTTON_HAS_BEEN_RELEASED = 0,
	BUTTON_HAS_BEEN_PRESSED  = 1,
	BUTTON_IS_STILL_HELD     = 2,
	BUTTON_PRESS_ABORTED     = 3,
	// BUTTON_PRESS_ABORTED is only valid from VT Version 4, but maybe lower versioned VTs do send it also...
};
#define BUTTON_HAS_BEEN_UNLATCHED 0
#define BUTTON_HAS_BEEN_LATCHED 1

#define KEY_HAS_BEEN_RELEASED 0
#define KEY_HAS_BEEN_PRESSED 1
#define KEY_IS_STILL_HELD 2
#define KEY_IS_STILL_PRESSED 2
#define KEY_PRESS_ABORTED 3
// KEY_IS_STILL_HELD is the obsolete/wrong naming. (Although it means the same :-)
// KEY_PRESS_ABORTED is only valid from VT Version 4, but maybe lower versioned VTs do send it also...

#define TOUCH_VER3_HAS_BEEN_PRESSED 0xFF
#define TOUCH_VER4_RELEASED 0
#define TOUCH_VER4_HAS_BEEN_PRESSED 1
#define TOUCH_VER4_HELD 2
#define TOUCH_HAS_BEEN_PRESSED(touchState)  ( (touchState == TOUCH_VER3_HAS_BEEN_PRESSED) || (touchState == TOUCH_VER4_HAS_BEEN_PRESSED) )
// Please note that a HELD/RELEASE will only be available at v4+, not for v2/v3.


enum ObjectType : uint8_t {
	VT_OBJECT_TYPE_WORKING_SET               = 0,
	VT_OBJECT_TYPE_DATA_MASK                 = 1,
	VT_OBJECT_TYPE_ALARM_MASK                = 2,
	VT_OBJECT_TYPE_CONTAINER                 = 3,
	VT_OBJECT_TYPE_SOFT_KEY_MASK             = 4,
	VT_OBJECT_TYPE_KEY                       = 5,
	VT_OBJECT_TYPE_BUTTON                    = 6,
	VT_OBJECT_TYPE_INPUT_BOOLEAN             = 7,
	VT_OBJECT_TYPE_INPUT_STRING              = 8,
	VT_OBJECT_TYPE_INPUT_NUMBER              = 9,
	VT_OBJECT_TYPE_INPUT_LIST                = 10,
	VT_OBJECT_TYPE_OUTPUT_STRING             = 11,
	VT_OBJECT_TYPE_OUTPUT_NUMBER             = 12,
	VT_OBJECT_TYPE_LINE                      = 13,
	VT_OBJECT_TYPE_RECTANGLE                 = 14,
	VT_OBJECT_TYPE_ELLIPSE                   = 15,
	VT_OBJECT_TYPE_POLYGON                   = 16,
	VT_OBJECT_TYPE_METER                     = 17,
	VT_OBJECT_TYPE_LINEAR_BAR_GRAPH          = 18,
	VT_OBJECT_TYPE_ARCHED_BAR_GRAPH          = 19,
	VT_OBJECT_TYPE_PICTURE_GRAPHIC           = 20,
	VT_OBJECT_TYPE_NUMBER_VARIABLE           = 21,
	VT_OBJECT_TYPE_STRING_VARIABLE           = 22,
	VT_OBJECT_TYPE_FONT_ATTRIBUTES           = 23,
	VT_OBJECT_TYPE_LINE_ATTRIBUTES           = 24,
	VT_OBJECT_TYPE_FILL_ATTRIBUTES           = 25,
	VT_OBJECT_TYPE_INPUT_ATTRIBUTES          = 26,
	VT_OBJECT_TYPE_OBJECT_POINTER            = 27,
	VT_OBJECT_TYPE_MACRO                     = 28,
	VT_OBJECT_TYPE_AUXILIARY_FUNCTION_1      = 29,
	VT_OBJECT_TYPE_AUXILIARY_INPUT_1         = 30,
	VT_OBJECT_TYPE_AUXILIARY_FUNCTION_2      = 31,
	VT_OBJECT_TYPE_AUXILIARY_INPUT_2         = 32,
	VT_OBJECT_TYPE_AUXILIARY_POINTER         = 33,
	VT_OBJECT_TYPE_WINDOW_MASK               = 34,
	VT_OBJECT_TYPE_KEY_GROUP                 = 35,
	VT_OBJECT_TYPE_GRAPHICS_CONTEXT          = 36,
	VT_OBJECT_TYPE_OUTPUT_LIST               = 37,
	VT_OBJECT_TYPE_EXTENDED_INPUT_ATTRIBUTES = 38,
	VT_OBJECT_TYPE_COLOUR_MAP                = 39,
};



namespace IsoAgLib {

class iVtObject_c;
class iVtObjectWorkingSet_c;
class iVtObjectDataMask_c;
class iVtObjectAlarmMask_c;
class iVtObjectContainer_c;
#ifdef CONFIG_USE_VTOBJECT_softkeymask
class iVtObjectSoftKeyMask_c;
#endif
class iVtObjectKey_c;
#ifdef CONFIG_USE_VTOBJECT_button
	class iVtObjectButton_c;
#endif //CONFIG_USE_VTOBJECT_button
class iVtObjectInputBoolean_c;
class iVtObjectInputString_c;
class iVtObjectInputNumber_c;
class iVtObjectInputList_c;
class iVtObjectOutputString_c;
class iVtObjectOutputNumber_c;
class iVtObjectOutputList_c;
class iVtObjectLine_c;
class iVtObjectMask_c;
class iVtObjectRectangle_c;
class iVtObjectEllipse_c;
class iVtObjectPolygon_c;
class iVtObjectMeter_c;
class iVtObjectLinearBarGraph_c;
class iVtObjectArchedBarGraph_c;
#ifdef CONFIG_USE_VTOBJECT_picturegraphic
	class iVtObjectPictureGraphic_c;
#endif
class iVtObjectColourMap_c;
class iVtObjectNumberVariable_c;
class iVtObjectStringVariable_c;
class iVtObjectFontAttributes_c;
class iVtObjectLineAttributes_c;
class iVtObjectFillAttributes_c;
class iVtObjectInputAttributes_c;
class iVtObjectObjectPointer_c;
class iVtObjectMacro_c;

class iVtObjectAuxiliaryFunction_c;
class iVtObjectAuxiliaryInput_c;
class iVtObjectAuxiliaryFunction2_c;
class iVtObjectAuxiliaryInput2_c;
class iVtObjectAuxiliaryControlDesignatorObjectPointer_c;

// +++ (attribute list) struct declarations
struct repeat_iVtObject_s {
  iVtObject_c* vtObject;
};

// +++ (attribute list) struct declarations
struct repeat_iVtObject_x_y_iVtObjectFontAttributes_row_col_s {
  iVtObject_c* vtObject;
  int16_t x;
  int16_t y;
  iVtObjectFontAttributes_c* vtObjectBlockFont;
  int16_t row;
  int16_t col;
  repeat_iVtObject_x_y_iVtObjectFontAttributes_row_col_s(
		  iVtObject_c *vtObject = nullptr,
		  int16_t x = 0,
		  int16_t y = 0,
		  iVtObjectFontAttributes_c *vtObjectBlockFont = nullptr,
		  int16_t row = 0,
          int16_t col = 0);
};

struct repeat_event_iVtObjectMacro_s {
  uint8_t event;
  iVtObjectMacro_c* vtObjectMacro;
};



struct repeat_vtLanguage_s {
  uint8_t language[2];
};



enum e_vtColour {
  BackgroundColour=0,
  BorderColour=1,
  NeedleColour=2,
  ArcAndTickColour=3,
  BarGraphColour=4,
  AColour=4 /*intentionally!*/,
  TargetLineColour=5,
  TransparencyColour=6,
  FontColour=7,
  LineColour=8,
  FillColour=9
#ifdef CONFIG_VT_CLIENT_PICTURE_GRAPHIC_COLOUR_CONVERSION
 ,PictureGraphicColour=10
#endif
};

/** struct of the data contained in the "VT Status Message" */
typedef struct vtState_s {
  ecutime_t lastReceived; /* Timestamp of last reception */

  /* the following data is extracted from one "VT Status Message" */
  uint8_t  saOfActiveWorkingSetMaster;
  uint16_t dataAlarmMask;
  uint16_t softKeyMask;
  uint8_t  busyCodes;
  uint8_t  functionBusy;
} vtState_s;

struct iAux2InputData
{
  iIsoName_c name;
  uint16_t    modelIdentificationCode;
  uint16_t    uid;
  bool        preferredAssignmentMatched;
  bool        preserve;

  iAux2InputData()
    : name(),
      modelIdentificationCode(0xFFFF),
      uid(0xFFFF),
      preferredAssignmentMatched(false), // a matching input object is found in an active joy stick ECU
      preserve(false)                    // if CONFIG_MAX_AUX2_PREFERRED_ASSIGNMENT_PER_FUNCTION is reached
                                         // => old preferred assignments candidates with preserve == false will be removed
  {}

  iAux2InputData(const iIsoName_c& a_name,
                 uint16_t a_modelIdentificationCode,
                 uint16_t a_uid,
                 bool a_preserve)
    : name(a_name),
      modelIdentificationCode(a_modelIdentificationCode),
      uid(a_uid),
      preferredAssignmentMatched(false),
      preserve(a_preserve)
  {}

  bool operator==(const iAux2InputData& arc_ref) const
  {
    // - check for preferredAssignmentMatched and preserve is not necessary
    if ( (name != arc_ref.name) ||
         (modelIdentificationCode != arc_ref.modelIdentificationCode) ||
         (uid != arc_ref.uid) )
      return false;
    else
      return true;
  }

  // Some old Tasking Compiler needs this operator,
  // so don't remove it!
  bool operator!=(const iAux2InputData& arc_ref) const
  {
    return !(operator==(arc_ref));
  }
};


/**
 * AUX2 assigment storage
 */
struct iAux2FunctionInputAssignment_t{
  uint16_t functionUid;
  iAux2InputData input;
};

typedef std::list<iAux2FunctionInputAssignment_t> iAux2Assignment_c;
typedef std::list<iAux2FunctionInputAssignment_t>::iterator iAux2AssignmentIterator_c;
typedef std::list<iAux2FunctionInputAssignment_t>::const_iterator iAux2AssignmentConstIterator_c;

} // IsoAgLib

#endif

