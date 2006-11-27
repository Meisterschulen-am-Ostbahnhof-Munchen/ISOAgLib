
/***************************************************************************
                          vt2iso-defines.hpp
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
#ifndef VT2ISO_DEFINES_DEFINED
#define VT2ISO_DEFINES_DEFINED


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


#define stringLength 1024


// Object Types
#define otWorkingset        (0)
#define otDatamask          (1)
#define otAlarmmask         (2)
#define otContainer         (3)
#define otSoftkeymask       (4)
#define otKey               (5)
#define otButton            (6)
#define otInputboolean      (7)
#define otInputstring       (8)
#define otInputnumber       (9)
#define otInputlist         (10)
#define otOutputstring      (11)
#define otOutputnumber      (12)
#define otLine              (13)
#define otRectangle         (14)
#define otEllipse           (15)
#define otPolygon           (16)
#define otMeter             (17)
#define otLinearbargraph    (18)
#define otArchedbargraph    (19)
#define otPicturegraphic    (20)
#define otNumbervariable    (21)
#define otStringvariable    (22)
#define otFontattributes    (23)
#define otLineattributes    (24)
#define otFillattributes    (25)
#define otInputattributes   (26)
#define otObjectpointer     (27)
#define otMacro             (28)
#define otAuxiliaryfunction (29)
#define otAuxiliaryinput    (30)
#define maxObjectTypes      (31) // +++ MAX +++ //object will not be inserted if (objType >= maxObjectTypes)

#define otObjectpool        (31) // for objTypeCompareTable (won't be inserted)
#define otIncludeobject     (32) // for objTypeCompareTable (won't be inserted)
#define otIncludemacro      (33) // for objTypeCompareTable (won't be inserted)
#define otPoint             (34) // for Polygon Object
#define otLanguage          (35) // for Working Set Object
#define otFixedBitmap       (36) // for Picture Graphic Object "hack"


// Command Types (for Macros)
#define ctHideShowObject         (0)
#define ctEnableDisableObject    (1)
#define ctSelectInputObject      (2)
#define ctControlAudioDevice     (3)
#define ctSetAudioVolume         (4)
#define ctChangeChildLocation    (5)
#define ctChangeChildPosition    (6)
#define ctChangeSize             (7)
#define ctChangeBackgroundColour (8)
#define ctChangeNumericValue     (9)
#define ctChangeStringValue      (10)
#define ctChangeEndPoint         (11)
#define ctChangeFontAttributes   (12)
#define ctChangeLineAttributes   (13)
#define ctChangeFillAttributes   (14)
#define ctChangeActiveMask       (15)
#define ctChangeSoftKeyMask      (16)
#define ctChangeAttribute        (17)
#define ctChangePriority         (18)
#define ctChangeListItem         (19)
#define maxCommands              (20) // +++ MAX +++

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
#define ombOutputfield       (ombOutputstring | ombOutputnumber)
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

#define ombObjectpool        (uint64_t(1)<<31) // for objTypeCompareTable (won't be inserted)
#define ombIncludeobject     (uint64_t(1)<<32) // for objTypeCompareTable (won't be inserted)
#define ombIncludemacro      (uint64_t(1)<<33) // for objTypeCompareTable (won't be inserted)
#define ombPoint             (uint64_t(1)<<34) // for Polygon Object
#define ombLanguage          (uint64_t(1)<<35) // for Working Set Object
#define ombFixedBitmap       (uint64_t(1)<<36) // for Picture Graphic Object "hack"

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
#define attrLatchable (11)
#define attrForeground_colour (12)
#define attrVariable_reference (13)
#define attrValue (14)
#define attrFont_attributes (15)
#define attrInput_attributes (16)
#define attrOptions (17)
#define attrHorizontal_justification (18)
#define attrLength (19)
#define attrMin_value (20)
#define attrMax_value (21)
#define attrOffset (22)
#define attrScale (23)
#define attrNumber_of_decimals (24)
#define attrFormat (25)
#define attrLine_attributes (26)
#define attrLine_suppression (27)
#define attrFill_attributes (28)
#define attrEllipse_type (29)
#define attrStart_angle (30)
#define attrEnd_angle (31)
#define attrPolygon_type (32)
#define attrNeedle_colour (33)
#define attrArc_and_tick_colour (34)
#define attrNumber_of_ticks (35)
#define attrColour (36)
#define attrTarget_line_colour (37)
#define attrTarget_value_variable_reference (38)
#define attrTarget_value (39)
#define attrBar_graph_width (40)
#define attrActual_width (41)
#define attrActual_height (42)
#define attrTransparency_colour (43)
#define attrFont_colour (44)
#define attrFont_size (45)
#define attrFont_type (46)
#define attrFont_style (47)
#define attrLine_colour (48)
#define attrLine_width (49)
#define attrLine_art (50)
#define attrFill_type (51)
#define attrFill_colour (52)
#define attrFill_pattern (53)
#define attrValidation_type (54)
#define attrValidation_string (55)
#define attrPos_x (56)
#define attrPos_y (57)
#define attrEvent (58)
#define attrFile (59)
#define attrLine_direction (60)
#define attrEnabled (61)
#define attrFile0 (62)
#define attrFile1 (63)
#define attrFile2 (64)
#define attrBlock_font (65)
#define attrBlock_row (66)
#define attrBlock_col (67)

#define attrNumber_of_items (68)
#define attrNumber_of_points (69)
#define attrRle (70)
#define attrNumber_of_bytes (71)
//addtional Aux Function/Input Attributes
#define attrFunction_type (72)
#define attrInput_id (73)

// Command Attributes:
#define attrObjectID  (74)
#define attrHideShow  (75)
#define attrDisable_enable (76)
#define attrNumber_of_repetitions (77)
#define attrFrequency (78)
#define attrOnTime_duration (79)
#define attrOffTime_duration (80)
#define attrPercentage (81)
#define attrParent_objectID (82)
#define attrX_change (83)
#define attrY_change (84)
#define attrX_pos (85)
#define attrY_pos (86)
#define attrNew_width (87)
#define attrNew_height (88)
#define attrNew_background_colour (89)
#define attrNew_value (90)
#define attrWorking_setID (91)
#define attrNew_active_mask (92)
#define attrMask_type (93)
#define attrMaskID (94)
#define attrNew_softkey_mask (95)
#define attrAttributeID (96)
#define attrNew_priority (97)
#define attrList_index (98)
#define attrNew_objectID (99)
#define attrBytes_in_string (100)
#define attrCode (101)
#define attrLanguage (102)
#define attrInKey (103)
#define attrInButton (104)

#define maxAttributeNames (105)


#define maxObjectTypesToCompare (maxObjectTypes+6)
extern char otCompTable [maxObjectTypesToCompare] [stringLength+1];

extern uint64_t omcTypeTable [maxObjectTypesToCompare];

extern char otClassnameTable [maxObjectTypes] [stringLength+1];

extern char attrNameTable [maxAttributeNames] [stringLength+1];

// Table of possible Macro Commands
#define maxCommandsToCompare (maxCommands)
extern char ctCommandTable [maxCommandsToCompare] [stringLength+1];

extern char colorTable [16] [stringLength+1];

extern char masktypeTable [3] [stringLength+1];

extern char colorDepthTable [3];

#define maxFontsizeTable 15
extern char fontsizeTable [maxFontsizeTable] [stringLength+1];

#define maxFontstyleTable 7
extern char fontstyleTable [maxFontstyleTable] [stringLength+1];

#define maxFonttypeTable 9
extern char fonttypeTable [maxFonttypeTable] [stringLength+1];

#define maxTruthTable 5
extern char truthTable [maxTruthTable] [stringLength+1];

#define maxFalseTable 5
extern char falseTable [maxFalseTable] [stringLength+1];

#define maxFormatTable 2
extern char formatTable [maxFormatTable] [stringLength+1];

#define maxHorizontalJustificationTable 3
extern char horizontalJustificationTable [maxHorizontalJustificationTable] [stringLength+1];

#define maxOptionsTable 2
extern char optionsTable [maxOptionsTable] [stringLength+1];

#define maxOutputNumberOptionsTable 3
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



unsigned int objectIsType (char*);

unsigned int commandIsType (char*);

void utf16convert (char* source, char* destin, int count);

unsigned int colortoi (char* text_color);

unsigned int colordepthtoi (char* text_colordepth);

unsigned int fonttypetoi (char* text_fonttype);

unsigned int booltoi (char *text_bool);

unsigned int fontsizetoi (char *text_fontsize);

unsigned int formattoi (char *text_format);

unsigned int horizontaljustificationtoi (char *text_horiz);

unsigned int optionstoi (char *text_options);

unsigned int outputnumberoptionstoi (char *text_options);

unsigned int picturegraphicoptionstoi (char *text_options);

unsigned int picturegraphicrletoi (char *text_options);

unsigned int meteroptionstoi (char *text_options);

unsigned int linearbargraphoptionstoi (char *text_options);

unsigned int archedbargraphoptionstoi (char *text_options);

unsigned int prioritytoi (char *text_priority);

unsigned int acousticsignaltoi (char *text_acousticsignal);

unsigned int fontstyletoi (char *text_fontstyle);

unsigned int linedirectiontoi (char *text_linedirection);

unsigned int linearttoi (char *text_lineart);

unsigned int linesuppressiontoi (char *text_linesuppression);

unsigned int ellipsetypetoi (char *text_ellipsetype);

unsigned int polygontypetoi (char *text_polygontype);

unsigned int validationtypetoi (char *text_validationtype);

unsigned int filltypetoi (char *text_filltype);

unsigned int eventToi (char *text_eventName);

unsigned int auxfunctiontyptetoi(char *text_auxFunctionType);

//###__XML Extension for VtGuiBuilder BEGIN-### ##########################################################

// Da es eine unterschiedliche Angabe von Eigenschaften vorkommen kann
/// < rectangle color='green' color='12'  // muss man unterscheiden zw String-Zahl und String-Wort

unsigned int getvalidtypefromstring (char *text_validtype);

unsigned int getcolorfromstring (char *text_backgrndcol);

unsigned int gethorizjustifyfromstring (char *text_horizstr);

unsigned int getboolfromstring (char *text_boolstr);

unsigned int getformatfromstring (char *text_formatstr);

unsigned int getlinesuppresfromstring (char *text_linesupprstr);

unsigned int  getellipsetypefromstring (char *text_ellipsetype);

unsigned int getpolygontypefromstring (char *text_polygontype);

unsigned int getoptionsfromstring (char *text_options);

unsigned int getoutnumoptionsfromstring (char *text_options);

unsigned int getmeteroptionsfromstring (char *text_options);

unsigned int getlinebargraphoptionfromstring (char *text_options);

unsigned int getcolordepthfromstring (char* text_colordepth);

unsigned int getpicgraphoptionfromstring (char *text_options);

unsigned int getfontsizefromstring (char *text_fontsize);

unsigned int getfonttypefromstring (char* text_fonttype);

unsigned int getfontstylefromstring (char *text_fontstyle);

unsigned int getfilltypefromstring (char *text_filltype);

unsigned int geteventfromstring (char *text_eventName);

//###__XML Extension for VtGuiBuilder ###-END ##########################################################


#endif
