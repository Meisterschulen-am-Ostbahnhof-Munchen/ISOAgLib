#include <stddef.h>
#include <sys/types.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <iostream>

#include "vt2isostr2bin.h"
//#include "vt2iso-defines.hpp"

using namespace std;

char otCompTable [maxObjectTypesToCompare] [stringLength+1] = {
    "workingset",       // 0
    "datamask",         // 1
    "alarmmask",        // 2
    "container",        // 3
    "softkeymask",      // 4
    "key",              // 5
    "button",           // 6
    "inputboolean",     // 7
    "inputstring",      // 8
    "inputnumber",      // 9
    "inputlist",        // 10
    "outputstring",     // 11
    "outputnumber",     // 12
    "line",             // 13
    "rectangle",        // 14
    "ellipse",          // 15
    "polygon",          // 16
    "meter",            // 17
    "linearbargraph",   // 18
    "archedbargraph",   // 19
    "picturegraphic",   // 20
    "numbervariable",   // 21
    "stringvariable",   // 22
    "fontattributes",   // 23
    "lineattributes",   // 24
    "fillattributes",   // 25
    "inputattributes",  // 26
    "objectpointer",    // 27
    "macro",            // 28
    "auxiliaryfunction", // 29
    "auxiliaryinput",     // 30 Maximal Mögliche Object Type
    "objectpool",         // 31
    "include_object",     // 32 = 0x20
    "include_macro",      // 33 = 0x21
    "point",              // 34 = 0x22
    "language",           // 35 = 0x23
    "fixedbitmap"         // 36 = 0x24
};

uint64_t omcTypeTable [maxObjectTypesToCompare] = {
    /* "workingset", */     ombMacro | ombOutputfield | ombOutputshape | ombPicturegraphic,
    /* "datamask", */       ombMacro | ombOutputfield | ombInputfield | ombOutputgraphic | ombOutputshape | ombPicturegraphic | ombButton | ombContainer | ombObjectpointer,
    /* "alarmmask", */      ombMacro | ombOutputfield | ombOutputgraphic | ombOutputshape | ombPicturegraphic | ombContainer | ombObjectpointer,
    /* "container", */      0, // same as the object that included the container
    /* "softkeymask", */    ombMacro | ombKey | ombObjectpointer,
    /* "key", */            ombMacro | ombOutputfield | ombOutputshape | ombPicturegraphic | ombContainer | ombObjectpointer,
    /* "button", */         ombMacro | ombOutputfield | ombOutputshape | ombPicturegraphic | ombContainer | ombObjectpointer,
    /* "inputboolean", */   ombMacro | 0,
    /* "inputstring", */    ombMacro | 0,
    /* "inputnumber", */    ombMacro | 0,
    /* "inputlist", */      ombMacro | ombOutputfield | ombPicturegraphic,
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
    /* "auxfunction", */    ombOutputfield | ombOutputshape | ombPicturegraphic, // really NONE
    /* "auxinput", */       ombOutputfield | ombOutputshape | ombPicturegraphic, // really NONE

    /* "objectpool", */     ~0, // all
    /* "include_object", */ 0, // really NONE
    /* "include_macro", */  0, // really NONE
    /* "point" */           0, // really NONE
    /* "language" */        0  // really NONE

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
    "AuxiliaryInput"
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
    "latchable",
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
    "in_button"
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
    "enable"
};


char falseTable [maxFalseTable] [stringLength+1] = {
    "no",
    "false",
    "off",
    "hide",
    "disable"
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

char optionsTable [maxOptionsTable] [stringLength+1] = {
    "transparent",
    "autowrap"
};

char outputNumberOptionsTable [maxOutputNumberOptionsTable] [stringLength+1] = {
    "transparent",
    "leadingzeros",
    "blankzero"
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

//event Table for Macros:
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


//   objCount = 0;
//   objNextAutoID = 65534;
//   objNextMacroAutoID = 255;
//   kcNextAutoID = 254; // for safety, 255 should also be okay though...
//   objNextUnnamedName = 1;

// extern unsigned int objCount;
// extern unsigned int objNextMacroAutoID;
// extern unsigned int objNextAutoID;
// extern char objNameTable [(stringLength+1)*4000];
 extern void clean_exit (int return_value, char* error_message=NULL);
// extern unsigned int objIDTable [4000];
// extern unsigned int objNextMacroAutoID;
// extern char attrString [maxAttributeNames] [stringLength+1];
// extern bool attrIsGiven [maxAttributeNames];
// extern unsigned int kcNextAutoID;
//
// extern char attr_name [1024+1];
// extern char attr_value [1024+1];

// extern struct language_s;
// extern unsigned int ui_languages;
// extern language_s arrs_language [DEF_iso639entries];


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

unsigned int colortoi (char* text_color)
{
  int l;
  for (l=0; l<16; l++) {
    if (strncmp (text_color, colorTable [l], stringLength) == 0) {
      return l;
    }
  }
  return atoi (text_color);
}

unsigned int colordepthtoi (char* text_colordepth)
{
  int l;
  for (l=0; l<2; l++) {
    if (text_colordepth [0] == colorDepthTable [l]) {
      return l;
    }
  }
  return 2;
}

unsigned int fonttypetoi (char* text_fonttype)
{
  int l;
  if (text_fonttype && isdigit(*text_fonttype))
  {
    if ((atoi(text_fonttype) == 0) || (atoi(text_fonttype) == 1) || (atoi(text_fonttype) == 2) || (atoi(text_fonttype) == 4) || (atoi(text_fonttype) == 5) || (atoi(text_fonttype) == 7) || (atoi(text_fonttype) == 255)) return atoi(text_fonttype);
  }
  else
  {
    for (l=0; l<maxFonttypeTable; l++) {
      if (strncmp (text_fonttype, fonttypeTable [l], stringLength) == 0) {
        if (l == maxFonttypeTable-1) return 0xFF;
        return l;
      }
    }
  }
  std::cout << "INVALID FONT TYPE '" << text_fonttype << "' ENCOUNTERED! STOPPING PARSER! bye.\n\n";
  clean_exit (-1);
  return 0; // to make compiler happy
}


unsigned int booltoi (char *text_bool)
{
  int l;
  for (l=0; l<maxTruthTable; l++) {
    if (strncmp (text_bool, truthTable [l], stringLength) == 0) {
      return true;
    }
  }
  for (l=0; l<maxFalseTable; l++) {
    if (strncmp (text_bool, falseTable [l], stringLength) == 0) {
      return false;
    }
  }
  std::cout << "INVALID TRUTH VALUE '" << text_bool << " ENCOUNTERED! STOPPING PARSER! bye.\n\n";
  clean_exit (-1);
  return 0; // to make compiler happy
}

unsigned int fontsizetoi (char *text_fontsize)
{
  int l;
  for (l=0; l<maxFontsizeTable; l++) {
    if (strncmp (text_fontsize, fontsizeTable [l], stringLength) == 0) {
      return l;
    }
  }
  std::cout << "INVALID FONT SIZE '" << text_fontsize << "' ENCOUNTERED! STOPPING PARSER! bye.\n\n";
  clean_exit (-1);
  return 0; // to make compiler happy
}

unsigned int formattoi (char *text_format)
{
  int l;
  for (l=0; l<maxFormatTable; l++) {
    if (strncmp (text_format, formatTable [l], stringLength) == 0) {
      return l;
    }
  }
  std::cout << "INVALID FORMAT '" << text_format << "' ENCOUNTERED! STOPPING PARSER! bye.\n\n";
  clean_exit (-1);
  return 0; // to make compiler happy
}

unsigned int horizontaljustificationtoi (char *text_horiz)
{
  int l;
  for (l=0; l<maxHorizontalJustificationTable; l++) {
    if (strncmp (text_horiz, horizontalJustificationTable [l], stringLength) == 0) {
      return l;
    }
  }
  std::cout << "INVALID HORIZONTALJUSTIFICATION '" << text_horiz << "' ENCOUNTERED! STOPPING PARSER! bye.\n\n";
  clean_exit (-1);
  return 0; // to make compiler happy
}


unsigned int optionstoi (char *text_options)
{
  int l, retval=0;
  for (l=0; l<maxOptionsTable; l++) {
    if (strstr (text_options, optionsTable [l]) != 0) {
      retval += (uint64_t(1)<<l);
    }
  }
  return retval;
}

unsigned int outputnumberoptionstoi (char *text_options)
{
  int l, retval=0;
  for (l=0; l<maxOutputNumberOptionsTable; l++) {
    if (strstr (text_options, outputNumberOptionsTable [l]) != 0) {
      retval += (uint64_t(1)<<l);
    }
  }
  return retval;
}

unsigned int picturegraphicoptionstoi (char *text_options)
{
  int l, retval=0;
  for (l=0; l<maxPictureGraphicOptionsTable; l++) {
    if (strstr (text_options, pictureGraphicOptionsTable [l]) != 0) {
      retval += (uint64_t(1)<<l);
    }
  }
  return retval;
}

unsigned int picturegraphicrletoi (char *text_options)
{
  int l, retval=0;
  for (l=0; l<maxPictureGraphicRleTable; l++) {
    if (strstr (text_options, pictureGraphicRleTable [l]) != 0) {
      retval += (uint64_t(1)<<l);
    }
  }
  return retval;
}

unsigned int meteroptionstoi (char *text_options)
{
  int l, retval=0;
  for (l=0; l<maxMeterOptionsTable; l++) {
    if (strstr (text_options, meterOptionsTable [l]) != 0) {
      retval += (uint64_t(1)<<l);
    }
  }
  return retval;
}

unsigned int linearbargraphoptionstoi (char *text_options)
{
  int l, retval=0;
  for (l=0; l<maxLinearBarGraphOptionsTable; l++) {
    if (strstr (text_options, linearBarGraphOptionsTable [l]) != 0) {
      retval += (uint64_t(1)<<l);
    }
  }
  return retval;
}

unsigned int archedbargraphoptionstoi (char *text_options)
{
  int l, retval=0;
  for (l=0; l<maxArchedBarGraphOptionsTable; l++) {
    if (strstr (text_options, archedBarGraphOptionsTable [l]) != 0) {
      retval += (uint64_t(1)<<l);
    }
  }
  return retval;
}

unsigned int prioritytoi (char *text_priority)
{
  int l;
  for (l=0; l<maxPriorityAcousticSignalTable-1; l++) {
    if (strncmp (text_priority, priorityAcousticSignalTable [l], stringLength) == 0) {
      return l;
    }
  }
  std::cout << "INVALID PRIORITY '" << text_priority << "' ENCOUNTERED! STOPPING PARSER! bye.\n\n";
  clean_exit (-1);
  return 0; // to make compiler happy
}

unsigned int acousticsignaltoi (char *text_acousticsignal)
{
  int l;
  for (l=0; l<maxPriorityAcousticSignalTable; l++) {
    if (strncmp (text_acousticsignal, priorityAcousticSignalTable [l], stringLength) == 0) {
      return l;
    }
  }
  std::cout << "INVALID ACOUSTIC SIGNAL '" << text_acousticsignal << "' ENCOUNTERED! STOPPING PARSER! bye.\n\n";
  clean_exit (-1);
  return 0; // to make compiler happy
}


unsigned int fontstyletoi (char *text_fontstyle)
{
  int l, retval=0;
  for (l=0; l<maxFontstyleTable; l++) {
    char *pos=strstr (text_fontstyle, fontstyleTable [l]);
    if (pos != NULL)
    {
      bool b_flashingInverted = false;
      if (l==4)
      { // "inverted" - only take if it's NOT flashing inverted!
        if (pos >= text_fontstyle+strlen (fontstyleTable [5])-strlen (fontstyleTable [4]))
        { // now check if the part left of it is "flashing" - in this case, do NOT consider this one as inverted!
          if (strncmp (pos-(strlen (fontstyleTable [5])-strlen (fontstyleTable [4])), fontstyleTable [5], strlen (fontstyleTable [5])) == 0)
            b_flashingInverted = true;
        }
      }
      if (!b_flashingInverted)
        retval += (uint64_t(1)<<l);
    }
  }
  return retval;
}

unsigned int linedirectiontoi (char *text_linedirection)
{
  int retval=0;
  if (strstr (text_linedirection, "bottomlefttotopright") != NULL) {
    retval = 1;
  }
  return retval;
}

unsigned int linearttoi (char *text_lineart)
{
  int retval=0;
  char thischar;
  while ((thischar = *text_lineart) != 0x00) {
    retval <<= 1;
    if (thischar == '1') {
      retval |= 0x0001;
    }
    text_lineart++;
  }
  return retval;
}


unsigned int linesuppressiontoi (char *text_linesuppression)
{
  int l, retval=0;
  for (l=0; l<maxLineSuppressionTable; l++) {
    if (strstr (text_linesuppression, lineSuppressionTable [l]) != 0) {
      retval += (uint64_t(1)<<l);
    }
  }
  return retval;
}


unsigned int ellipsetypetoi (char *text_ellipsetype)
{
  int l, retval=0;
  for (l=0; l<maxEllipseTypeTable; l++) {
    if (strcmp(text_ellipsetype, ellipseTypeTable [l]) == 0) {
      retval = l;
      break;
    }
  }
  return retval;
}

unsigned int polygontypetoi (char *text_polygontype)
{
  int l, retval=0;
  for (l=0; l<maxPolygonTypeTable; l++) {
    if (strcmp (text_polygontype, polygonTypeTable [l]) == 0) {
      retval = l;
      break;
    }
  }
  return retval;
}

unsigned int validationtypetoi (char *text_validationtype)
{
  int retval=0;
  if (strstr (text_validationtype, "invalid") != 0) {
    retval = 1;
  }
  return retval;
}


unsigned int filltypetoi (char *text_filltype)
{
  int l, retval=0;
  for (l=0; l<maxFillTypeTable; l++) {
    if (strstr (text_filltype, fillTypeTable [l]) != 0) {
      retval = l;
      break;
    }
  }
  return retval;
}

unsigned int eventToi (char *text_eventName)
{
  int l, retval=0;
  for (l=0; l<maxEventTable; l++) {
    if (strstr (text_eventName, eventTable [l]) != 0) {
      retval = l + 1;
      break;
    }
  }
  return retval;
}

unsigned int auxfunctiontyptetoi(char *text_auxFunctionType)
{
  int l, retval=0;
  for (l=0; l<maxAuxFunctionTypes; l++) {
    if (strstr (text_auxFunctionType, auxFunctionTypeTable [l]) != 0) {
      retval = l;
      break;
    }
  }
  return retval;
}

//###__XML Extension for VtGuiBuilder BEGIN-### ##########################################################


unsigned int getcolorfromstring (char *text_backgrndcol)
{
  unsigned int l = 0;
        if (isalpha(text_backgrndcol[0]))
          l = colortoi(text_backgrndcol);
        else if (isdigit(text_backgrndcol[0]))
          l = atoi(text_backgrndcol);
        else l = 1;
  return l;
}


unsigned int getboolfromstring (char *text_boolstr)
{
  unsigned int l = 0;
        if (isalpha(text_boolstr[0]))
          l = (unsigned int)booltoi(text_boolstr);
        else if (isdigit(text_boolstr[0]))
          l = atoi(text_boolstr);
        else l = 1;
  return l;
}


unsigned int gethorizjustifyfromstring (char *text_horizstr)
{
  int l;
    if (isalpha(text_horizstr[0]))
        l = (unsigned int)horizontaljustificationtoi(text_horizstr);
    else if (isdigit(text_horizstr[0]))
        l = atoi(text_horizstr);
    else l = 1;
  return l;
}


unsigned int getformatfromstring (char *text_formatstr)
{
  int l;
    if (isalpha(text_formatstr[0]))
        l = (unsigned int)formattoi(text_formatstr);
    else if (isdigit(text_formatstr[0]))
        l = atoi(text_formatstr);
    else l = 1;
  return l;
}

unsigned int getlinesuppresfromstring (char *text_linesupprstr)
{
  int l=0;
    if (isalpha(text_linesupprstr[0]))
        l = (unsigned int)linesuppressiontoi(text_linesupprstr);
    else if (isdigit(text_linesupprstr[0]))
        l = atoi(text_linesupprstr);
    else l = 0;
  return l;
}


unsigned int  getellipsetypefromstring (char *text_ellipsetype)
{
  int l=0;
    if (isalpha(text_ellipsetype[0]))
        l = (unsigned int)ellipsetypetoi(text_ellipsetype);
    else if (isdigit(text_ellipsetype[0]))
        l = atoi(text_ellipsetype);
    else l = 0;
  return l;
}

unsigned int getpolygontypefromstring (char *text_polygontype)
{
  int l=0;
    if (isalpha(text_polygontype[0]))
        l = (unsigned int)polygontypetoi(text_polygontype);
    else if (isdigit(text_polygontype[0]))
        l = atoi(text_polygontype);
    else l = 0;
  return l;
}

unsigned int getoptionsfromstring (char *text_options)
{
  int l=0;
    if (isalpha(text_options[0]))
        l = (unsigned int) optionstoi(text_options);
    else if ( isdigit(text_options[0]) )
        l = atoi(text_options);
    else l = 0;
  return l;
}

unsigned int getoutnumoptionsfromstring (char *text_options)
{
  int l=0;
    if (isalpha(text_options[0]))
        l = (unsigned int) outputnumberoptionstoi(text_options);
    else if ( isdigit(text_options[0]) )
        l = atoi(text_options);
    else l = 0;
  return l;
}

unsigned int getmeteroptionsfromstring (char *text_options)
{
  int l=0;
    if (isalpha(text_options[0]))
        l = (unsigned int) meteroptionstoi(text_options);
    else if ( isdigit(text_options[0]) )
        l = atoi(text_options);
    else l = 0;
  return l;
}

unsigned int getlinebargraphoptionfromstring (char *text_options)
{
  int l=0;
    if (isalpha(text_options[0]))
        l = (unsigned int) linearbargraphoptionstoi(text_options);
    else if ( isdigit(text_options[0]) )
        l = atoi(text_options);
    else l = 0;
  return l;
}

unsigned int getcolordepthfromstring (char* text_colordepth)
{
  int l=0;
    if (isalpha(text_colordepth[0]))
        l = (unsigned int) colordepthtoi(text_colordepth);
    else if ( isdigit(text_colordepth[0]) )
        l = atoi(text_colordepth);
    else l = 0;
  return l;
}

unsigned int getpicgraphoptionfromstring (char *text_options)
{
  int l=0;
    if (isalpha(text_options[0]))
        l = (unsigned int) picturegraphicoptionstoi(text_options);
    else if ( isdigit(text_options[0]) )
        l = atoi(text_options);
    else l = 0;
  return l;
}

unsigned int getfontsizefromstring (char *text_fontsize)
{
  int l=0;
    if (isalpha(text_fontsize[0]))
        l = (unsigned int) fontsizetoi(text_fontsize);
    else if ( isdigit(text_fontsize[0]) )
        l = atoi(text_fontsize);
    else l = 0;
  return l;
}

unsigned int getfonttypefromstring (char* text_fonttype)
{
  int l=0;
    if (isalpha(text_fonttype[0]))
        l = (unsigned int) fonttypetoi(text_fonttype);
    else if ( isdigit(text_fonttype[0]) )
        l = atoi(text_fonttype);
    else l = 0;
  return l;
}

unsigned int getfontstylefromstring (char *text_fontstyle)
{
  int l=0;
    if (isalpha(text_fontstyle[0]))
        l = (unsigned int) fontstyletoi(text_fontstyle);
    else if ( isdigit(text_fontstyle[0]) )
        l = atoi(text_fontstyle);
    else l = 0;
  return l;
}

unsigned int getfilltypefromstring (char *text_filltype)
{
 int l=0;
    if (isalpha(text_filltype[0]))
        l = (unsigned int) filltypetoi (text_filltype);
    else if ( isdigit(text_filltype[0]) )
        l = atoi(text_filltype);
    else l = 0;
  return l;
}

unsigned int getvalidtypefromstring (char *text_validtype)
{
 int l=0;
    if (isalpha(text_validtype[0]))
        l = (unsigned int) validationtypetoi (text_validtype);
    else if ( isdigit(text_validtype[0]) )
        l = atoi(text_validtype);
    else l = 0;
  return l;
}



unsigned int geteventfromstring (char *text_eventName)
{
 int l=0;
    if (isalpha(text_eventName[0]))
        l = (unsigned int) eventToi (text_eventName);
    else if ( isdigit(text_eventName[0]) )
        l = atoi(text_eventName);
    else l = 0;
  return l;
}
//###__XML Extension for VtGuiBuilder ###-END ##########################################################


