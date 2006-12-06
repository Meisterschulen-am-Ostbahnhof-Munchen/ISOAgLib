/***************************************************************************
                          vt2iso-globals.cpp
                             -------------------
    begin                : Mon Mar 31 2003
    copyright            : (C) 2003 - 2004 by Dipl.-Inf.(FH) Eva Erdmann
    email                : eva.erdmannk@osb-ag:de
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
#include "vt2iso-globals.hpp"

using namespace std;

unsigned int
colortoi (char* text_color)
{
  int l;
  for (l=0; l<16; l++) {
    if (strncmp (text_color, colorTable [l], stringLength) == 0) {
      return l;
    }
  }
  return atoi (text_color);
}

unsigned int
masktypetoi (char* masktype)
{
  int l;
  for (l=0; l<3; l++) {
    if (strncmp (masktype, masktypeTable [l], stringLength) == 0) {
      return l;
    }
  }
  return atoi (masktype);
}

unsigned int
colordepthtoi (char* text_colordepth)
{
  int l;
  for (l=0; l<2; l++) {
    if (text_colordepth [0] == colorDepthTable [l]) {
      return l;
    }
  }
  return 2;
}

signed int
fonttypetoi (char* text_fonttype)
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
  return -1;
}

signed int
booltoi (char *text_bool)
{
  int l;
  for (l=0; l<maxTruthTable; l++) {
    if (strncmp (text_bool, truthTable [l], stringLength) == 0) {
      return 1;
    }
  }
  for (l=0; l<maxFalseTable; l++) {
    if (strncmp (text_bool, falseTable [l], stringLength) == 0) {
      return 0;
    }
  }
  std::cout << "INVALID TRUTH VALUE '" << text_bool << " ENCOUNTERED! STOPPING PARSER! bye.\n\n";
  return -1;
}

signed int
fontsizetoi (char *text_fontsize)
{
  int l;
  for (l=0; l<maxFontsizeTable; l++) {
    if (strncmp (text_fontsize, fontsizeTable [l], stringLength) == 0) {
      return l;
    }
  }
  std::cout << "INVALID FONT SIZE '" << text_fontsize << "' ENCOUNTERED! STOPPING PARSER! bye.\n\n";
  return -1;
}

signed int
formattoi (char *text_format)
{
  int l;
  for (l=0; l<maxFormatTable; l++) {
    if (strncmp (text_format, formatTable [l], stringLength) == 0) {
      return l;
    }
  }
  std::cout << "INVALID FORMAT '" << text_format << "' ENCOUNTERED! STOPPING PARSER! bye.\n\n";
  return -1;
}

signed int
horizontaljustificationtoi (char *text_horiz)
{
  int l;
  for (l=0; l<maxHorizontalJustificationTable; l++) {
    if (strncmp (text_horiz, horizontalJustificationTable [l], stringLength) == 0) {
      return l;
    }
  }
  std::cout << "INVALID HORIZONTALJUSTIFICATION '" << text_horiz << "' ENCOUNTERED! STOPPING PARSER! bye.\n\n";
  return -1;
}

unsigned int
optionstoi (char *text_options)
{
  int l, retval=0;
  for (l=0; l<maxOptionsTable; l++) {
    if (strstr (text_options, optionsTable [l]) != 0) {
      retval += (uint64_t(1)<<l);
    }
  }
  return retval;
}

unsigned int
numberoptionstoi (char *text_options)
{
  int l, retval=0;
  for (l=0; l<maxOutputNumberOptionsTable; l++) {
    if (strstr (text_options, outputNumberOptionsTable [l]) != 0) {
      retval += (uint64_t(1)<<l);
    }
  }
  return retval;
}

unsigned int
picturegraphicoptionstoi (char *text_options)
{
  int l, retval=0;
  for (l=0; l<maxPictureGraphicOptionsTable; l++) {
    if (strstr (text_options, pictureGraphicOptionsTable [l]) != 0) {
      retval += (uint64_t(1)<<l);
    }
  }
  return retval;
}

unsigned int
picturegraphicrletoi (char *text_options)
{
  int l, retval=0;
  for (l=0; l<maxPictureGraphicRleTable; l++) {
    if (strstr (text_options, pictureGraphicRleTable [l]) != 0) {
      retval += (uint64_t(1)<<l);
    }
  }
  return retval;
}

unsigned int
meteroptionstoi (char *text_options)
{
  int l, retval=0;
  for (l=0; l<maxMeterOptionsTable; l++) {
    if (strstr (text_options, meterOptionsTable [l]) != 0) {
      retval += (uint64_t(1)<<l);
    }
  }
  return retval;
}

unsigned int
linearbargraphoptionstoi (char *text_options)
{
  int l, retval=0;
  for (l=0; l<maxLinearBarGraphOptionsTable; l++) {
    if (strstr (text_options, linearBarGraphOptionsTable [l]) != 0) {
      retval += (uint64_t(1)<<l);
    }
  }
  return retval;
}

unsigned int
archedbargraphoptionstoi (char *text_options)
{
  int l, retval=0;
  for (l=0; l<maxArchedBarGraphOptionsTable; l++) {
    if (strstr (text_options, archedBarGraphOptionsTable [l]) != 0) {
      retval += (uint64_t(1)<<l);
    }
  }
  return retval;
}

signed int
prioritytoi (char *text_priority)
{
  int l;
  for (l=0; l<maxPriorityAcousticSignalTable-1; l++) {
    if (strncmp (text_priority, priorityAcousticSignalTable [l], stringLength) == 0) {
      return l;
    }
  }
  std::cout << "INVALID PRIORITY '" << text_priority << "' ENCOUNTERED! STOPPING PARSER! bye.\n\n";
  return -1;
}

signed int
acousticsignaltoi (char *text_acousticsignal)
{
  int l;
  for (l=0; l<maxPriorityAcousticSignalTable; l++) {
    if (strncmp (text_acousticsignal, priorityAcousticSignalTable [l], stringLength) == 0) {
      return l;
    }
  }
  std::cout << "INVALID ACOUSTIC SIGNAL '" << text_acousticsignal << "' ENCOUNTERED! STOPPING PARSER! bye.\n\n";
  return -1;
}

unsigned int
fontstyletoi (char *text_fontstyle)
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

unsigned int
linedirectiontoi (char *text_linedirection)
{
  int retval=0;
  if (strstr (text_linedirection, "bottomlefttotopright") != NULL) {
    retval = 1;
  }
  return retval;
}

unsigned int
linearttoi (char *text_lineart)
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

unsigned int
linesuppressiontoi (char *text_linesuppression)
{
  int l, retval=0;
  for (l=0; l<maxLineSuppressionTable; l++) {
    if (strstr (text_linesuppression, lineSuppressionTable [l]) != 0) {
      retval += (uint64_t(1)<<l);
    }
  }
  return retval;
}

unsigned int
ellipsetypetoi (char *text_ellipsetype)
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

unsigned int
polygontypetoi (char *text_polygontype)
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

unsigned int
validationtypetoi (char *text_validationtype)
{
  int retval=0;
  if (strstr (text_validationtype, "invalid") != 0) {
    retval = 1;
  }
  return retval;
}

unsigned int
filltypetoi (char *text_filltype)
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

unsigned int
eventToi (char *text_eventName)
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

unsigned int
auxfunctiontyptetoi(char *text_auxFunctionType)
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

unsigned int
gcoptionstoi (char *text_options)
{
  int l, retval=0;
  for (l=0; l<maxGCOptions; l++) {
    if (strstr (text_options, GCOptionsTable [l]) != 0) {
      retval += (uint64_t(1)<<l);
    }
  }
  return retval;
}

unsigned int
inputobjectoptiontoi (char *text_inputobjectoptions)
{
  int l, retval=0;
  for (l=0; l<maxInputObjectOptionsTable; l++) {
    if (strstr (text_inputobjectoptions, inputobjectOptionsTable [l]) != 0) {
      retval += (uint64_t(1)<<l);
    }
  }
  return retval;
}

unsigned int
buttonoptiontoi (char *text_buttonoptions)
{
  int l, retval=0;
  for (l=0; l<maxButtonOptions; l++) {
    if (strstr (text_buttonoptions, buttonOptionsTable [l]) != 0) {
      retval += (uint64_t(1)<<l);
    }
  }
  return retval;
}
