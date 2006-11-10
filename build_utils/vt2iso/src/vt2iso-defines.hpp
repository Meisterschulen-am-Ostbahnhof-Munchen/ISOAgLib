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
#define otGraphicsContext   (36)
#define maxObjectTypes      (37) //! +++ MAX +++ object will not be inserted if (objType >= maxObjectTypes)

#define otObjectpool        (55) // for objTypeCompareTable (won't be inserted)
#define otIncludeobject     (56) // for objTypeCompareTable (won't be inserted)
#define otIncludemacro      (57) // for objTypeCompareTable (won't be inserted)
#define otPoint             (58) // for Polygon Object
#define otLanguage          (59) // for Working Set Object
#define otFixedBitmap       (60) // for Picture Graphic Object "hack"


// Command Types (for Macros)
#define ctHideShowObject              (0)
#define ctEnableDisableObject         (1)
#define ctSelectInputObject           (2)
#define ctControlAudioDevice          (3)
#define ctSetAudioVolume              (4)
#define ctChangeChildLocation         (5)
#define ctChangeChildPosition         (6)
#define ctChangeSize                  (7)
#define ctChangeBackgroundColour      (8)
#define ctChangeNumericValue          (9)
#define ctChangeStringValue           (10)
#define ctChangeEndPoint              (11)
#define ctChangeFontAttributes        (12)
#define ctChangeLineAttributes        (13)
#define ctChangeFillAttributes        (14)
#define ctChangeActiveMask            (15)
#define ctChangeSoftKeyMask           (16)
#define ctChangeAttribute             (17)
#define ctChangePriority              (18)
#define ctChangeListItem              (19)
// Graphics Context Sub-Commands:
#define ctSetGraphicsCursor           (20)
#define ctSetForegroundColour         (21)
#define ctSetBackgroundColour         (22)
#define ctSetLineAttributes           (23)
#define ctSetFillAttributes           (24)
#define ctSetFontAttributes           (25)
#define ctEraseRectangle              (26)
#define ctDrawPoint                   (27)
#define ctDrawLine                    (28)
#define ctDrawRectangle               (29)
#define ctDrawClosedEllipse           (30)
#define ctDrawPolygon                 (31)
#define ctDrawText                    (32)
#define ctPanViewport                 (33)
#define ctZoomViewport                (34)
#define ctPanAndZoomViewport          (35)
#define ctChangeViewportSize          (36)
#define ctDrawVtObject                (37)
#define ctCopyCanvas2PictureGraphic   (38)
#define ctCopyViewport2PictureGraphic (39)
//! Maximum command type for macros.
#define maxCommands                   (40)

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

#define maxAttributeNames (114)

//! Number of regular object plus gap plus number of internal objects.
#define maxObjectTypesToCompare (maxObjectTypes+18+6)
char otCompTable [maxObjectTypesToCompare] [stringLength+1] = {
  "workingset",
  "datamask",
  "alarmmask",
  "container",
  "softkeymask",
  "key",
  "button",
  "inputboolean",
  "inputstring",
  "inputnumber",
  "inputlist",
  "outputstring",
  "outputnumber",
  "line",
  "rectangle",
  "ellipse",
  "polygon",
  "meter",
  "linearbargraph",
  "archedbargraph",
  "picturegraphic",
  "numbervariable",
  "stringvariable",
  "fontattributes",
  "lineattributes",
  "fillattributes",
  "inputattributes",
  "objectpointer",
  "macro",
  "auxiliaryfunction",
  "auxiliaryinput",     // Object ID 30
  "",
  "",
  "",
  "",
  "",
  "graphicscontext",    // Object ID 36
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "objectpool",          // Object ID 55
  "include_object",
  "include_macro",
  "point",
  "language",
  "fixedbitmap"
};

uint64_t omcTypeTable [maxObjectTypesToCompare] = {
    /* "workingset", */     ombMacro | ombOutputfield | ombOutputshape | ombPicturegraphic | ombGraphicsContext,
    /* "datamask", */       ombMacro | ombOutputfield | ombInputfield | ombOutputgraphic | ombOutputshape | ombPicturegraphic | ombButton | ombContainer | ombObjectpointer | ombGraphicsContext,
    /* "alarmmask", */      ombMacro | ombOutputfield | ombOutputgraphic | ombOutputshape | ombPicturegraphic | ombContainer | ombObjectpointer | ombGraphicsContext,
    /* "container", */      0, // same as the object that included the container
    /* "softkeymask", */    ombMacro | ombKey | ombObjectpointer,
    /* "key", */            ombMacro | ombOutputfield | ombOutputshape | ombPicturegraphic | ombContainer | ombObjectpointer | ombGraphicsContext,
    /* "button", */         ombMacro | ombOutputfield | ombOutputshape | ombPicturegraphic | ombContainer | ombObjectpointer | ombGraphicsContext,
    /* "inputboolean", */   ombMacro | 0,
    /* "inputstring", */    ombMacro | 0,
    /* "inputnumber", */    ombMacro | 0,
    /* "inputlist", */      ombMacro | ombOutputfield | ombPicturegraphic | ombGraphicsContext,
    /* "outputstring", */   ombMacro | 0,
    /* "outputnumber", */   ombMacro | 0,
    /* "line", */           ombMacro | 0,
    /* "rectangle", */      ombMacro | 0,
    /* "ellipse", */        ombMacro | 0,
    /* "polygon", */        ombMacro | ombPoint,
    /* "meter", */          ombMacro | 0,
    /* "linearbargraph", */ ombMacro | 0,
    /* "archedbargraph", */ ombMacro | 0,
    /* "picturegraphic", */ ombMacro | ombFixedBitmap,
    /* "numbervariable", */ 0, // really NONE
    /* "stringvariable", */ 0, // really NONE
    /* "fontattributes", */ ombMacro | 0,
    /* "lineattributes", */ ombMacro | 0,
    /* "fillattributes", */ ombMacro | 0,
    /* "inputattributes",*/ ombMacro | 0,
    /* "objectpointer", */  0, // really NONE
    /* "macro", */          0, // really NONE
    /* "auxfunction", */    ombOutputfield | ombOutputshape | ombPicturegraphic | ombGraphicsContext, // really NONE
    /* "auxinput", */       ombOutputfield | ombOutputshape | ombPicturegraphic | ombGraphicsContext, // really NONE
                            0, //31
                            0,
                            0,
                            0,
                            0,
    /* "graphicscontext" */ 0,
                            0,
                            0,
                            0,
                            0,
                            0,
                            0,
                            0,
                            0,
                            0,
                            0,
                            0,
                            0,
                            0,
                            0,
                            0,
                            0,
                            0,
                            0,
    /* "objectpool" */     ~0, // all
    /* "include_object" */  0, // really NONE
    /* "include_macro" */   0, // really NONE
    /* "point" */           0, // really NONE
    /* "language" */        0, // really NONE
    /* "fixedbitmap" */     0
};

char otClassnameTable [maxObjectTypes] [stringLength+1] = {
  "WorkingSet",
  "DataMask",
  "AlarmMask",
  "Container",
  "SoftKeyMask",
  "Key",
  "Button",
  "InputBoolean",
  "InputString",
  "InputNumber",
  "InputList",
  "OutputString",
  "OutputNumber",
  "Line",
  "Rectangle",
  "Ellipse",
  "Polygon",
  "Meter",
  "LinearBarGraph",
  "ArchedBarGraph",
  "PictureGraphic",
  "NumberVariable",
  "StringVariable",
  "FontAttributes",
  "LineAttributes",
  "FillAttributes",
  "InputAttributes",
  "ObjectPointer",
  "Macro",
  "AuxiliaryFunction",
  "AuxiliaryInput",
  "",
  "",
  "",
  "",
  "",
  "GraphicsContext",
};

char attrNameTable [maxAttributeNames] [stringLength+1] = {
  "background_colour",
  "selectable",
  "active_mask",
  "soft_key_mask",
  "priority",
  "acoustic_signal",
  "width",
  "height",
  "hidden",
  "key_code",
  "border_colour",
  "foreground_colour",
  "variable_reference",
  "value",
  "font_attributes",
  "input_attributes",
  "options",
  "horizontal_justification",
  "length",
  "min_value",
  "max_value",
  "offset",
  "scale",
  "number_of_decimals",
  "format",
  "line_attributes",
  "line_suppression",
  "fill_attributes",
  "ellipse_type",
  "start_angle",
  "end_angle",
  "polygon_type",
  "needle_colour",
  "arc_and_tick_colour",
  "number_of_ticks",
  "colour",
  "target_line_colour",
  "target_value_variable_reference",
  "target_value",
  "bar_graph_width",
  "actual_width",
  "actual_height",
  "transparency_colour",
  "font_colour",
  "font_size",
  "font_type",
  "font_style",
  "line_colour",
  "line_width",
  "line_art",
  "fill_type",
  "fill_colour",
  "fill_pattern",
  "validation_type",
  "validation_string",
  "pos_x",
  "pos_y",
  "event",
  "file",
  "line_direction",
  "enabled",
  "file1",
  "file4",
  "file8",
  "block_font",
  "block_row",
  "block_col",
// new attributes from the new objects (and macros?!)
  "number_of_items",
  "number_of_points",
  "rle",
  "number_of_bytes",
  "function_type",
  "input_id",
  "object_id",
  "hide_show",
  "enable_disable",
  "number_of_repetitions",
  "frequency",
  "on_time_duration",
  "off_time_duration",
  "percentage",
  "parent_object_id",
  "x_pos_change",
  "y_pos_change",
  "x_pos",
  "y_pos",
  "new_width",
  "new_height",
  "new_background_colour",
  "new_value",
  "working_set_object_id",
  "new_active_mask_object_id",
  "mask_type",
  "mask_object_id",
  "new_softkey_mask_object_id",
  "attribute_id",
  "new_priority",
  "list_index",
  "new_object_id",
  "bytes_in_string",
  "code",
  "language",
  "in_key",
  "in_button",
  "viewport_width",
  "viewport_height",
  "viewport_x",
  "viewport_y",
  "canvas_width",
  "canvas_height",
  "viewport_zoom",
  "cursor_x",
  "cursor_y",
  "inputobject_options"
};

// Table of possible Macro Commands
#define maxCommandsToCompare (maxCommands)
char ctCommandTable [maxCommandsToCompare] [stringLength+1] = {
  "command_hide_show_object",
  "command_enable_disable_object",
  "command_select_input_object",
  "command_control_audio_device",
  "command_set_audio_volume",
  "command_change_child_location",
  "command_change_child_position",
  "command_change_size",
  "command_change_background_color",
  "command_change_numeric_value",
  "command_change_string_value",
  "command_change_end_point",
  "command_change_font_attributes",
  "command_change_line_attributes",
  "command_change_fill_attributes",
  "command_change_active_mask",
  "command_change_soft_key_mask",
  "command_change_attribute",
  "command_change_priority",
  "command_change_list_item"
};

char colorTable [16] [stringLength+1] = {
  "black",
  "white",
  "green",
  "teal",
  "maroon",
  "purple",
  "olive",
  "silver",
  "grey",
  "blue",
  "lime",
  "cyan",
  "red",
  "magenta",
  "yellow",
  "navy"
};

char masktypeTable [3] [stringLength+1] = {
  "undefined",
  "data",
  "alarm"
};

char colorDepthTable [3] = {'1', '4', '8'};

#define maxFontsizeTable 15
char fontsizeTable [maxFontsizeTable] [stringLength+1] = {
  "6x8",
  "8x8",
  "8x12",
  "12x16",
  "16x16",
  "16x24",
  "24x32",
  "32x32",
  "32x48",
  "48x64",
  "64x64",
  "64x96",
  "96x128",
  "128x128",
  "128x192"
};

#define maxFontstyleTable 7
char fontstyleTable [maxFontstyleTable] [stringLength+1] = {
  "bold",
  "crossed",
  "underlined",
  "italic",
  "inverted",
  "flashinginverted",
  "flashinghidden"
};

#define maxFonttypeTable 9
char fonttypeTable [maxFonttypeTable] [stringLength+1] = {
  "latin1",
  "latin9",
  "latin2",
  "reserved",
  "latin4",
  "latin5",
  "reserved",
  "latin7",
  "proprietary"
};

#define maxTruthTable 5
char truthTable [maxTruthTable] [stringLength+1] = {
  "yes",
  "true",
  "on",
  "show",
  "enable"
};

#define maxFalseTable 5
char falseTable [maxFalseTable] [stringLength+1] = {
  "no",
  "false",
  "off",
  "hide",
  "disable"
};

#define maxFormatTable 2
char formatTable [maxFormatTable] [stringLength+1] = {
  "fixed",
  "exponential"
};

#define maxHorizontalJustificationTable 3
char horizontalJustificationTable [maxHorizontalJustificationTable] [stringLength+1] = {
  "left",
  "middle",
  "right"
};

#define maxOptionsTable 2
char optionsTable [maxOptionsTable] [stringLength+1] = {
  "transparent",
  "autowrap"
};

#define maxOutputNumberOptionsTable 3
char outputNumberOptionsTable [maxOutputNumberOptionsTable] [stringLength+1] = {
  "transparent",
  "leadingzeros",
  "blankzero"
};

#define maxLineSuppressionTable 4
char lineSuppressionTable [maxLineSuppressionTable] [stringLength+1] = {
  "top",
  "right",
  "bottom",
  "left"
};

#define maxEllipseTypeTable 4
char ellipseTypeTable [maxEllipseTypeTable] [stringLength+1] = {
  "closed",
  "open",
  "closedsegment",
  "closedsection"
};

#define maxPolygonTypeTable 4
char polygonTypeTable [maxPolygonTypeTable] [stringLength+1] = {
  "convex",
  "nonconvex",
  "complex",
  "open"
};


#define maxValidationTypeTable 2
char validationTypeTable [maxValidationTypeTable] [stringLength+1] = {
  "valid_characters",
  "invalid_characters"
};


#define maxFillTypeTable 4
char fillTypeTable [maxFillTypeTable] [stringLength+1] = {
  "no_fill",
  "linecolour",
  "fillcolour",
  "pattern"
};

#define maxPictureGraphicOptionsTable 2
char pictureGraphicOptionsTable [maxPictureGraphicOptionsTable] [stringLength+1] = {
  "transparent",
  "flashing",
};

#define maxPictureGraphicRleTable 4
char pictureGraphicRleTable [maxPictureGraphicRleTable] [stringLength+1] = {
  "1",
  "4",
  "8",
  "auto"
};

#define maxLinearBarGraphOptionsTable 6
char linearBarGraphOptionsTable [maxLinearBarGraphOptionsTable] [stringLength+1] = {
  "border",
  "targetline",
  "ticks",
  "nofill",
  "horizontal",
  "growpositive"
};

#define maxMeterOptionsTable 4
char meterOptionsTable[maxMeterOptionsTable] [stringLength+1] ={
  "arc",
  "border",
  "ticks",
  "clockwise"
};

#define maxArchedBarGraphOptionsTable 5
char archedBarGraphOptionsTable [maxArchedBarGraphOptionsTable] [stringLength+1] = {
  "border",
  "targetline",
  "not_used",
  "nofill",
  "clockwise",
};

#define maxPriorityAcousticSignalTable 4
char priorityAcousticSignalTable [maxPriorityAcousticSignalTable] [stringLength+1] = {
  "high",
  "medium",
  "low",
  "none"
};

//event Table for Macros:
#define maxEventTable 26
char eventTable [maxEventTable] [stringLength+1] = {
  "on_activate",
  "on_deactivate",
  "on_show",
  "on_hide",
  "on_enable",
  "on_disable",
  "on_change_active_mask",
  "on_change_soft_key_mask",
  "on_change_attribute",
  "on_change_background_colour",
  "on_change_font_attributes",
  "on_change_line_attributes",
  "on_change_fill_attributes",
  "on_change_child_location",
  "on_change_size",
  "on_change_value",
  "on_change_priority",
  "on_change_end_point",
  "on_input_field_selection",
  "on_input_field_deselection",
  "on_ESC",
  "on_entry_of_value",
  "on_entry_of_new_value",
  "on_key_press",
  "on_key_release",
  "on_change_child_position"
};

#define maxAuxFunctionTypes 3
char auxFunctionTypeTable [maxAuxFunctionTypes] [stringLength+1] = {
  "latchingboolean",
  "analog",
  "nonlatchingboolean"
};

#define maxGCOptions 2
char GCOptionsTable [maxGCOptions] [stringLength+1] = {
  "transparent",
  "uselinefontfillcolourfordraw"
};

#define maxButtonOptions 6
char buttonOptionsTable [maxButtonOptions] [stringLength+1] = {
  "latchable",
  "latched",
  "suppress_border",
  "transparent_background",
  "disabled",
  "no_border"
};

#define maxInputObjectOptionsTable 2
char inputobjectOptionsTable [maxInputObjectOptionsTable] [stringLength+1] = {
  "enable",
  "live_editing"
};

