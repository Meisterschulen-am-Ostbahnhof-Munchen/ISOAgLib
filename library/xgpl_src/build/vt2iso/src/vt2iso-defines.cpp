/***************************************************************************
                          vt2iso-defines.cpp
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
#include "vt2iso-defines.hpp"
#include <cstring>

//! Number of regular object plus gap plus number of internal objects.
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
  "auxiliaryfunction2",
  "auxiliaryinput2",
  "auxiliaryassignment",
  "auxiliarycontroldesignatorobjectpointer",
  "colourmap",
  "graphicscontext",    // Object ID 36
  "outputlist",
  "extendedinputattributes",
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
  /* "workingset", */     ombMacro | ombOutputfield | ombOutputshape | ombPicturegraphic | ombGraphicsContext | ombContainer | ombObjectpointer,
  /* "datamask", */       ombMacro | ombOutputfield | ombInputfield | ombOutputgraphic | ombOutputshape | ombPicturegraphic | ombButton | ombContainer | ombObjectpointer | ombGraphicsContext,
  /* "alarmmask", */      ombMacro | ombOutputfield | ombOutputgraphic | ombOutputshape | ombPicturegraphic | ombContainer | ombObjectpointer | ombGraphicsContext,
  /* "container", */      0, // same as the object that included the container
  /* "softkeymask", */    ombMacro | ombKey | ombObjectpointer,
  /* "key", */            ombMacro | ombOutputfield | ombOutputshape | ombPicturegraphic | ombContainer | ombObjectpointer | ombGraphicsContext,
  /* "button", */         ombMacro | ombOutputfield | ombOutputshape | ombPicturegraphic | ombContainer | ombObjectpointer | ombGraphicsContext,
  /* "inputboolean", */   ombMacro | 0,
  /* "inputstring", */    ombMacro | 0,
  /* "inputnumber", */    ombMacro | 0,
  /* "inputlist", */      ombMacro | ombContainer | ombOutputfield | ombOutputshape | ombOutputgraphic | ombPicturegraphic | ombGraphicsContext,
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
  /* "outputlist" */      ombMacro | ombOutputfield  | ombOutputshape | ombOutputgraphic | ombInputfield | ombPicturegraphic | ombGraphicsContext,
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
  "AuxiliaryFunction2",
  "AuxiliaryInput2",
  "AuxiliaryAssignment",
  "AuxiliaryControlDesignatorObjectPointer",
  "ColourMap",
  "GraphicsContext",
  "OutputList",
  "ExtendedInputAttributes"
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

char fontstyleTable [maxFontstyleTable] [stringLength+1] = {
  "bold",
  "crossed",
  "underlined",
  "italic",
  "inverted",
  "flashinginverted",
  "flashinghidden"
};

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

char truthTable [maxTruthTable] [stringLength+1] = {
  "yes",
  "true",
  "on",
  "show",
  "enable",
  "1"
};

char falseTable [maxFalseTable] [stringLength+1] = {
  "no",
  "false",
  "off",
  "hide",
  "disable",
  "0"
};

char formatTable [maxFormatTable] [stringLength+1] = {
  "fixed",
  "exponential"
};

char horizontalJustificationTable [maxHorizontalJustificationTable] [stringLength+1] = {
  "left",
  "middle",
  "right"
};

char stringOptionsTable [maxStringOptionsTable] [stringLength+1] = {
  "transparent",
  "autowrap",
  "wrap_on_hyphen"
};

char inputNumberOptionsTable [maxInputNumberOptionsTable] [stringLength+1] = {
  "transparent",
  "leadingzeros",
  "blankzero",
  "truncate"
};

char outputNumberOptionsTable [maxOutputNumberOptionsTable] [stringLength+1] = {
  "transparent",
  "leadingzeros",
  "blankzero",
  "truncate"
};

char lineSuppressionTable [maxLineSuppressionTable] [stringLength+1] = {
  "top",
  "right",
  "bottom",
  "left"
};

char ellipseTypeTable [maxEllipseTypeTable] [stringLength+1] = {
  "closed",
  "open",
  "closedsegment",
  "closedsection"
};

char polygonTypeTable [maxPolygonTypeTable] [stringLength+1] = {
  "convex",
  "nonconvex",
  "complex",
  "open"
};


char validationTypeTable [maxValidationTypeTable] [stringLength+1] = {
  "valid_characters",
  "invalid_characters"
};


char fillTypeTable [maxFillTypeTable] [stringLength+1] = {
  "no_fill",
  "linecolour",
  "fillcolour",
  "pattern"
};

char pictureGraphicOptionsTable [maxPictureGraphicOptionsTable] [stringLength+1] = {
  "transparent",
  "flashing",
};

char pictureGraphicRleTable [maxPictureGraphicRleTable] [stringLength+1] = {
  "1",
  "4",
  "8",
  "auto"
};

char linearBarGraphOptionsTable [maxLinearBarGraphOptionsTable] [stringLength+1] = {
  "border",
  "targetline",
  "ticks",
  "nofill",
  "horizontal",
  "growpositive"
};

char meterOptionsTable[maxMeterOptionsTable] [stringLength+1] ={
  "arc",
  "border",
  "ticks",
  "clockwise"
};

char archedBarGraphOptionsTable [maxArchedBarGraphOptionsTable] [stringLength+1] = {
  "border",
  "targetline",
  "not_used",
  "nofill",
  "clockwise",
};

char priorityAcousticSignalTable [maxPriorityAcousticSignalTable] [stringLength+1] = {
  "high",
  "medium",
  "low",
  "none"
};

//event Table for Macros
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

char auxFunctionTypeTable [maxAuxFunctionTypes] [stringLength+1] = {
  "latchingboolean",
  "analog",
  "nonlatchingboolean"
};

char GCOptionsTable [maxGCOptions] [stringLength+1] = {
  "transparent",
  "uselinefontfillcolourfordraw"
};

char buttonOptionsTable [maxButtonOptions] [stringLength+1] = {
  "latchable",
  "latched",
  "suppress_border",
  "transparent_background",
  "disabled",
  "no_border"
};

char inputobjectOptionsTable [maxInputObjectOptionsTable] [stringLength+1] = {
  "enable",
  "live_editing"
};

void utf16convert (char* source, char* destin, int count)
{
  int index=-1;
  do
  {
    index++;
    destin [index] = source [(index*2)];
  } while (destin [index] != 0x00);
}

unsigned int objectIsType (char* lookup_name)
{
  for (int i=0; i<maxObjectTypesToCompare; i++) {
    if (0 == strncmp (lookup_name, otCompTable [i], stringLength)) {
      return i;
    }
  }
  return 0xFFFF;
}

unsigned int commandIsType (char* lookup_name)
{
  for (int i=0; i<maxCommandsToCompare; i++) {
    if (0 == strncmp (lookup_name, ctCommandTable [i], stringLength)) {
      return i;
    }
  }
  return 0xFFFF;
}
