/***************************************************************************
                          vt2iso-defines.h
                             -------------------
    begin                : Mon Mar 31 2003
    copyright            : (C) 2003 - 2004 by Dipl.-Inf. Martin Wodok
    email                : m.wodok@osb-ag:de
    type                 : Header
    $LastChangedDate: 2004-10-01 13:51:42 +0200 (Fri, 01 Oct 2004) $
    $LastChangedRevision: 426 $
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
 ***************************************************************************/
#ifndef VT2ISODEFINES_H
#define VT2ISODEFINES_H

#ifndef SYSTEM_PC_VC
#include <stdint.h>
#else
  typedef unsigned char uint8_t;
  typedef signed char int8_t;
  typedef unsigned short uint16_t;
  typedef short int16_t;
  typedef unsigned int uint32_t;
  typedef int int32_t;
  typedef unsigned long long int uint64_t;
  typedef long int long int64_t;
#endif

#include <cstdio>

typedef struct tagBGR {
  uint8_t bgrBlue;
  uint8_t bgrGreen;
  uint8_t bgrRed;
} BGR_s;

typedef struct tagRGB {
  uint8_t rgbRed;
  uint8_t rgbGreen;
  uint8_t rgbBlue;
} RGB_s;

typedef struct {
  FILE* partFile;
  char code [2+1]; // NULL terminated, so it gets comparable by "strcmp", etc.
  unsigned int count;
  bool firstLine;
  char* valueBuffer;
  unsigned int valueBufferLen;
} language_s;

#define DEF_iso639entries 136

#define stringLength 1024

// Object Types
#define otWorkingset              (0)
#define otDatamask                (1)
#define otAlarmmask               (2)
#define otContainer               (3)
#define otSoftkeymask             (4)
#define otKey                     (5)
#define otButton                  (6)
#define otInputboolean            (7)
#define otInputstring             (8)
#define otInputnumber             (9)
#define otInputlist               (10)
#define otOutputstring            (11)
#define otOutputnumber            (12)
#define otLine                    (13)
#define otRectangle               (14)
#define otEllipse                 (15)
#define otPolygon                 (16)
#define otMeter                   (17)
#define otLinearbargraph          (18)
#define otArchedbargraph          (19)
#define otPicturegraphic          (20)
#define otNumbervariable          (21)
#define otStringvariable          (22)
#define otFontattributes          (23)
#define otLineattributes          (24)
#define otFillattributes          (25)
#define otInputattributes         (26)
#define otObjectpointer           (27)
#define otMacro                   (28)
#define otAuxiliaryfunction       (29)
#define otAuxiliaryinput          (30)
#define otAuxiliaryFunctionType2  (31)
#define otAuxiliaryInputType2     (32)
#define otAuxiliaryAssignment     (33)
#define otAuxiliaryControlDesignatorObjectPointer (34)
#define otColourMap               (35)
#define otGraphicsContext         (36)
#define otOutputlist              (37)
#define otExtendedInputAttributes (38)
#define maxObjectTypes            (39) //! +++ MAX +++ object will not be inserted if (objType >= maxObjectTypes)

#define otObjectpool              (55) // for objTypeCompareTable (won't be inserted)
#define otIncludeobject           (56) // for objTypeCompareTable (won't be inserted)
#define otIncludemacro            (57) // for objTypeCompareTable (won't be inserted)
#define otPoint                   (58) // for Polygon Object
#define otLanguage                (59) // for Working Set Object
#define otFixedBitmap             (60) // for Picture Graphic Object "hack"


// Command Types (for Macros)
#define ctHideShowObject         (0)
#define ctEnableDisableObject    (1)
#define ctSelectInputObject      (2)
#define ctControlAudioDevice     (3)
#define ctSetAudioVolume         (4)
#define ctChangeChildLocation    (5)
#define ctChangeSize             (6)
#define ctChangeBackgroundColour (7)
#define ctChangeNumericValue     (8)
#define ctChangeEndPoint         (9)
#define ctChangeFontAttributes   (10)
#define ctChangeLineAttributes   (11)
#define ctChangeFillAttributes   (12)
#define ctChangeActiveMask       (13)
#define ctChangeSoftKeyMask      (14)
#define ctChangeAttribute        (15)
#define ctChangePriority         (16)
#define ctChangeListItem         (17)
#define notvalidMacroCommand     (18)
#define ctChangeStringValue      (19)
#define ctChangeChildPosition    (20)
#define ctSetObjectLabel         (21)
#define ctChangePolygonPoint     (22)
#define ctChangePolygonScale     (23)
#define ctGraphicsContext        (24)
#define notvalidMacroCommand2    (25)
#define notvalidMacroCommand3    (26)
#define notvalidMacroCommand4    (27)
#define notvalidMacroCommand5    (28)
#define ctLockUnlockMask         (29)
#define ctExecuteMacro           (30)
// Graphics Context Sub-Commands:
#define ctSetGraphicsCursor           (31)
#define ctSetForegroundColour         (32)
#define ctSetBackgroundColour         (33)
#define ctSetLineAttributes           (34)
#define ctSetFillAttributes           (35)
#define ctSetFontAttributes           (36)
#define ctEraseRectangle              (37)
#define ctDrawPoint                   (38)
#define ctDrawLine                    (39)
#define ctDrawRectangle               (40)
#define ctDrawClosedEllipse           (41)
#define ctDrawPolygon                 (42)
#define ctDrawText                    (43)
#define ctPanViewport                 (44)
#define ctZoomViewport                (45)
#define ctPanAndZoomViewport          (46)
#define ctChangeViewportSize          (47)
#define ctDrawVtObject                (48)
#define ctCopyCanvas2PictureGraphic   (49)
#define ctCopyViewport2PictureGraphic (50)
#define maxCommands                   (51)  // +++ MAX +++


// Object May Be
#define ombWorkingset        (uint64_t(1)<<0)
#define ombDatamask          (uint64_t(1)<<1)
#define ombAlarmmask         (uint64_t(1)<<2)
#define ombContainer         (uint64_t(1)<<3)
#define ombSoftkeymask       (uint64_t(1)<<4)
#define ombKey               (uint64_t(1)<<5)
#define ombButton            (uint64_t(1)<<6)
#define ombInputboolean      (uint64_t(1)<<7)
#define ombInputstring       (uint64_t(1)<<8)
#define ombInputnumber       (uint64_t(1)<<9)
#define ombInputlist         (uint64_t(1)<<10)
#define ombInputfield        (ombInputboolean | ombInputstring | ombInputnumber | ombInputlist)
#define ombOutputstring      (uint64_t(1)<<11)
#define ombOutputnumber      (uint64_t(1)<<12)
#define ombOutputlist        (uint64_t(1)<<37)
#define ombOutputfield       (ombOutputstring | ombOutputnumber | ombOutputlist)
#define ombLine              (uint64_t(1)<<13)
#define ombRectangle         (uint64_t(1)<<14)
#define ombEllipse           (uint64_t(1)<<15)
#define ombPolygon           (uint64_t(1)<<16)
#define ombOutputshape       (ombLine | ombRectangle | ombEllipse | ombPolygon)
#define ombMeter             (uint64_t(1)<<17)
#define ombLinearbargraph    (uint64_t(1)<<18)
#define ombArchedbargraph    (uint64_t(1)<<19)
#define ombOutputgraphic     (ombMeter | ombLinearbargraph | ombArchedbargraph)
#define ombPicturegraphic    (uint64_t(1)<<20)
#define ombNumbervariable    (uint64_t(1)<<21)
#define ombStringvariable    (uint64_t(1)<<22)
#define ombVariables         (ombNumberveriable | ombStringvariable)
#define ombFontattributes    (uint64_t(1)<<23)
#define ombLineattributes    (uint64_t(1)<<24)
#define ombFillattributes    (uint64_t(1)<<25)
#define ombInputattributes   (uint64_t(1)<<26)
#define ombAttributes        (ombFontattributes | ombLineattributes | ombFillattributes | ombInputattributes)
#define ombObjectpointer     (uint64_t(1)<<27)
#define ombMacro             (uint64_t(1)<<28)
#define ombAuxiliaryfunction (uint64_t(1)<<29)
#define ombAuxiliaryinput    (uint64_t(1)<<30)
#define ombGraphicsContext   (uint64_t(1)<<36)

#define ombObjectpool        (uint64_t(1)<<55) // for objTypeCompareTable (won't be inserted)
#define ombIncludeobject     (uint64_t(1)<<56) // for objTypeCompareTable (won't be inserted)
#define ombIncludemacro      (uint64_t(1)<<57) // for objTypeCompareTable (won't be inserted)
#define ombPoint             (uint64_t(1)<<58) // for Polygon Object
#define ombLanguage          (uint64_t(1)<<59) // for Working Set Object
#define ombFixedBitmap       (uint64_t(1)<<60) // for Picture Graphic Object "hack"

// Attributes
#define attrBackground_colour (0)
#define attrSelectable (1)
#define attrActive_mask (2)
#define attrSoft_key_mask (3)
#define attrPriority (4)
#define attrAcoustic_signal (5)
#define attrWidth (6)
#define attrHeight (7)
#define attrHidden (8)
#define attrKey_code (9)
#define attrBorder_colour (10)
#define attrForeground_colour (11)
#define attrVariable_reference (12)
#define attrValue (13)
#define attrFont_attributes (14)
#define attrInput_attributes (15)
#define attrOptions (16)
#define attrHorizontal_justification (17)
#define attrLength (18)
#define attrMin_value (19)
#define attrMax_value (20)
#define attrOffset (21)
#define attrScale (22)
#define attrNumber_of_decimals (23)
#define attrFormat (24)
#define attrLine_attributes (25)
#define attrLine_suppression (26)
#define attrFill_attributes (27)
#define attrEllipse_type (28)
#define attrStart_angle (29)
#define attrEnd_angle (30)
#define attrPolygon_type (31)
#define attrNeedle_colour (32)
#define attrArc_and_tick_colour (33)
#define attrNumber_of_ticks (34)
#define attrColour (35)
#define attrTarget_line_colour (36)
#define attrTarget_value_variable_reference (37)
#define attrTarget_value (38)
#define attrBar_graph_width (39)
#define attrActual_width (40)
#define attrActual_height (41)
#define attrTransparency_colour (42)
#define attrFont_colour (43)
#define attrFont_size (44)
#define attrFont_type (45)
#define attrFont_style (46)
#define attrLine_colour (47)
#define attrLine_width (48)
#define attrLine_art (49)
#define attrFill_type (50)
#define attrFill_colour (51)
#define attrFill_pattern (52)
#define attrValidation_type (53)
#define attrValidation_string (54)
#define attrPos_x (55)
#define attrPos_y (56)
#define attrEvent (57)
#define attrFile (58)
#define attrLine_direction (59)
#define attrEnabled (60)
#define attrFile0 (61)
#define attrFile1 (62)
#define attrFile2 (63)
#define attrBlock_font (64)
#define attrBlock_row (65)
#define attrBlock_col (66)

#define attrNumber_of_items (67)
#define attrNumber_of_points (68)
#define attrRle (69)
#define attrNumber_of_bytes (70)
//addtional Aux Function/Input Attributes
#define attrFunction_type (71)
#define attrInput_id (72)

// Command Attributes:
#define attrObjectID  (73)
#define attrHideShow  (74)
#define attrDisable_enable (75)
#define attrNumber_of_repetitions (76)
#define attrFrequency (77)
#define attrOnTime_duration (78)
#define attrOffTime_duration (79)
#define attrPercentage (80)
#define attrParent_objectID (81)
#define attrX_change (82)
#define attrY_change (83)
#define attrX_pos (84)
#define attrY_pos (85)
#define attrNew_width (86)
#define attrNew_height (87)
#define attrNew_background_colour (88)
#define attrNew_value (89)
#define attrWorking_setID (90)
#define attrNew_active_mask (91)
#define attrMask_type (92)
#define attrMaskID (93)
#define attrNew_softkey_mask (94)
#define attrAttributeID (95)
#define attrNew_priority (96)
#define attrList_index (97)
#define attrNew_objectID (98)
#define attrBytes_in_string (99)
#define attrCode (100)
#define attrLanguage (101)
#define attrInKey (102)
#define attrInButton (103)

// Attributes for graphics context
#define attrViewportWidth (104)
#define attrViewportHeight (105)
#define attrViewportX (106)
#define attrViewportY (107)
#define attrCanvasWidth (108)
#define attrCanvasHeight (109)
#define attrViewportZoom (110)
#define attrCursorX (111)
#define attrCursorY (112)
// Attributes allready defined and used by graphics context:
//#define attrForeground_colour (12)
//#define attrBackground_colour (0)
//#define attrFont_attributes (15)
//#define attrLine_attributes (26)
//#define attrFill_attributes (28)
//#define attrFormat (25)
//#define attrOptions (17)
//#define attrTransparency_colour (43)

// attribute for inputnumber/list only!
#define attrInputObjectOptions (113)

#define attrVertical_justification (114)

#define maxAttributeNames (115)

//! Number of regular object plus gap plus number of internal objects.
#define maxObjectTypesToCompare (maxObjectTypes+18+6)
extern char otCompTable [maxObjectTypesToCompare] [stringLength+1];

extern uint64_t omcTypeTable [maxObjectTypesToCompare];

extern char otClassnameTable [maxObjectTypes] [stringLength+1];

extern char attrNameTable [maxAttributeNames] [stringLength+1];

// Table of possible Macro Commands
#define maxCommandsToCompare (maxCommands)
extern char ctCommandTable [maxCommandsToCompare] [stringLength+1];

#define maxColourTable 16
extern char colourTable [maxColourTable] [stringLength+1];

extern char masktypeTable [3] [stringLength+1];

extern char colourDepthTable [3];

#define maxFontsizeTable 15
extern char fontsizeTable [maxFontsizeTable] [stringLength+1];

#define maxFontstyleTable 7
extern char fontstyleTable [maxFontstyleTable] [stringLength+1];

#define maxFonttypeTable 9
extern char fonttypeTable [maxFonttypeTable] [stringLength+1];

#define maxTruthTable 6
extern char truthTable [maxTruthTable] [stringLength+1];

#define maxFalseTable 6
extern char falseTable [maxFalseTable] [stringLength+1];

#define maxFormatTable 2
extern char formatTable [maxFormatTable] [stringLength+1];

#define maxHorizontalJustificationTable 3
extern char horizontalJustificationTable [maxHorizontalJustificationTable] [stringLength+1];

#define maxVerticalJustificationTable 3
extern char verticalJustificationTable [maxVerticalJustificationTable] [stringLength+1];

#define maxStringOptionsTable 3
extern char stringOptionsTable [maxStringOptionsTable] [stringLength+1];

#define maxInputNumberOptionsTable 4
extern char inputNumberOptionsTable [maxInputNumberOptionsTable] [stringLength+1];

#define maxOutputNumberOptionsTable 4
extern char outputNumberOptionsTable [maxOutputNumberOptionsTable] [stringLength+1];

#define maxLineSuppressionTable 4
extern char lineSuppressionTable [maxLineSuppressionTable] [stringLength+1];

#define maxEllipseTypeTable 4
extern char ellipseTypeTable [maxEllipseTypeTable] [stringLength+1];

#define maxPolygonTypeTable 4
extern char polygonTypeTable [maxPolygonTypeTable] [stringLength+1];

#define maxValidationTypeTable 2
extern char validationTypeTable [maxValidationTypeTable] [stringLength+1];

#define maxFillTypeTable 4
extern char fillTypeTable [maxFillTypeTable] [stringLength+1];

#define maxPictureGraphicOptionsTable 2
extern char pictureGraphicOptionsTable [maxPictureGraphicOptionsTable] [stringLength+1];

#define maxPictureGraphicRleTable 4
extern char pictureGraphicRleTable [maxPictureGraphicRleTable] [stringLength+1];

#define maxLinearBarGraphOptionsTable 6
extern char linearBarGraphOptionsTable [maxLinearBarGraphOptionsTable] [stringLength+1];

#define maxMeterOptionsTable 4
extern char meterOptionsTable[maxMeterOptionsTable] [stringLength+1];

#define maxArchedBarGraphOptionsTable 5
extern char archedBarGraphOptionsTable [maxArchedBarGraphOptionsTable] [stringLength+1];

#define maxPriorityAcousticSignalTable 4
extern char priorityAcousticSignalTable [maxPriorityAcousticSignalTable] [stringLength+1];

//event Table for Macros
#define maxEventTable 26
extern char eventTable [maxEventTable] [stringLength+1];

#define maxAuxFunctionTypes 3
extern char auxFunctionTypeTable [maxAuxFunctionTypes] [stringLength+1];

#define maxGCOptions 2
extern char GCOptionsTable [maxGCOptions] [stringLength+1];

#define maxButtonOptions 6
extern char buttonOptionsTable [maxButtonOptions] [stringLength+1];

#define maxInputObjectOptionsTable 2
extern char inputobjectOptionsTable [maxInputObjectOptionsTable] [stringLength+1];


void utf16convert (char* source, char* destin, int count);

unsigned int objectIsType (char* lookup_name);

unsigned int commandIsType (char*);

#endif // #endif VT2ISODEFINES_H
