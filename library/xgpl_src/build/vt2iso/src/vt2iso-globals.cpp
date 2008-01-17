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


bool itogeneraloption(uint8_t ui8_options, std::string& c_outputText, uint8_t ui8_numEntries, char* pchar_table )
{
  uint16_t pos,idx;
  bool b_addPlus = false;
  c_outputText.clear();

  for (pos=0 ; pos < ui8_numEntries; pos++)
  {
    if (ui8_options & (1<<pos))
    {
      if (b_addPlus) c_outputText.append( "+" );
      b_addPlus = true;

      // multidimensional array access!
      c_outputText.append( &pchar_table[pos*(stringLength+1)] );
    }
  }
  if (!b_addPlus) c_outputText.append( "none" );

  return true;
}


unsigned int
colortoi (char* text_color)
{
  int l;
  for (l=0; l<maxColorTable; l++) {
    if (strncmp (text_color, colorTable [l], stringLength) == 0) {
      return l;
    }
  }
  return atoi (text_color);
}

bool itocolor(unsigned int ui_index, string& c_outputText)
{
  if(ui_index < maxColorTable )
  {
   c_outputText = colorTable[ui_index];

  }
  else
 {
    char c_tmp_buf[stringLength];
    sprintf(c_tmp_buf,"%d",ui_index);
    c_outputText = c_tmp_buf;
 }
  return true;
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

bool itocolordepth(uint8_t ui8_options, std::string& c_outputText)
{
  c_outputText.clear();
  switch (ui8_options) {
    case 0: c_outputText="1bit"; break;
    case 1: c_outputText="4bit"; break;
    case 2: c_outputText="8bit"; break;
    default: return false;
  }
  return true;
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


bool itofonttype(unsigned int ui_index, string& c_outputText)
{

  if(ui_index < 8 )
  {
    char c_tmpBuf[stringLength];
    sprintf(c_tmpBuf,"%d",ui_index);
    c_outputText = c_tmpBuf;
  }
  else if( ui_index == 255)
  {
    c_outputText = "proprietary";
  }
  else
  {
    if(ui_index < maxFonttypeTable )
    {
      c_outputText = fonttypeTable[ui_index];
    }
    else return false;
  }
  return true;
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

bool itofontsize(unsigned int ui_index, string& c_outputText)
{
  if(ui_index < maxFontsizeTable )
  {
    c_outputText = fontsizeTable[ui_index];
    return true;
  }
  return false;
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

bool itoformat(unsigned int ui_index, string& c_outputText)
{
  if(ui_index < maxFormatTable )
  {
    c_outputText = formatTable[ui_index];
    return true;
  }
  return false;
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

bool itohorizontaljustification(unsigned int ui_index, string& c_outputText)
{
  if(ui_index < maxHorizontalJustificationTable )
  {
    c_outputText = horizontalJustificationTable[ui_index];
    return true;
  }
  return false;
}


unsigned int
stringoptionstoi (char *text_options)
{
  int l, retval=0;
  for (l=0; l<maxStringOptionsTable; l++) {
    if (strstr (text_options, stringOptionsTable [l]) != 0) {
      retval += (uint64_t(1)<<l);
    }
  }
  return retval;
}

bool itostringoptions(uint8_t ui8_options, std::string& c_outputText)
{
  return itogeneraloption(ui8_options, c_outputText, maxStringOptionsTable, &stringOptionsTable[0][0]);
}

unsigned int
inputnumberoptionstoi (char *text_options)
{
  int l, retval=0;
  for (l=0; l<maxInputNumberOptionsTable; l++) {
    if (strstr (text_options, inputNumberOptionsTable [l]) != 0) {
      retval += (uint64_t(1)<<l);
    }
  }
  return retval;
}

bool itoinputnumberoptions(uint8_t ui8_options, std::string& c_outputText)
{
  return itogeneraloption(ui8_options, c_outputText, maxInputNumberOptionsTable, &inputNumberOptionsTable[0][0]);
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

bool itonumberoptions(uint8_t ui8_options, std::string& c_outputText)
{
  return itogeneraloption(ui8_options, c_outputText, maxOutputNumberOptionsTable, &outputNumberOptionsTable[0][0]);
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

bool itopicturegraphicoptions(uint8_t ui8_options, std::string& c_outputText)
{
  return itogeneraloption(ui8_options, c_outputText, maxPictureGraphicOptionsTable, &pictureGraphicOptionsTable[0][0]);
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

bool itopicturegraphicrle(uint8_t ui8_options, std::string& c_outputText)
{
  return itogeneraloption(ui8_options, c_outputText, maxPictureGraphicRleTable, &pictureGraphicRleTable[0][0]);
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

bool itometeroptions(uint8_t ui8_options, string& c_outputText)
{
  return itogeneraloption(ui8_options, c_outputText, maxMeterOptionsTable, &meterOptionsTable[0][0]);
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

bool itolinearbargraphoptions(uint8_t ui8_options, std::string& c_outputText)
{
  return itogeneraloption(ui8_options, c_outputText, maxLinearBarGraphOptionsTable, &linearBarGraphOptionsTable[0][0]);
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

bool itoarchedbargraphoptions(uint8_t ui8_options, std::string& c_outputText)
{
  return itogeneraloption(ui8_options, c_outputText, maxArchedBarGraphOptionsTable, &archedBarGraphOptionsTable[0][0]);
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

bool itopriority(unsigned int ui_index, string& c_outputText)
{
  if(ui_index < maxPriorityAcousticSignalTable )
  {
    c_outputText = priorityAcousticSignalTable[ui_index];
    return true;
  }
  return false;
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

bool itoacousticsignal(unsigned int ui_index,string& c_outputText)
{
  if(ui_index < maxPriorityAcousticSignalTable )
  {
    c_outputText = priorityAcousticSignalTable[ui_index];
    return true;
  }
  return false;
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

bool itofontstyle(uint8_t ui8_options, std::string& c_outputText)
{ // the additional check in the above function is not necessary
  return itogeneraloption(ui8_options, c_outputText, maxFontstyleTable, &fontstyleTable[0][0]);
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

bool itolineardirection(unsigned int ui_index, string& c_outputText)
{
  char c_tmpBuf[stringLength];
  if(ui_index == 0)
  {
    sprintf(c_tmpBuf,"%s","toplefttobottomright");
    c_outputText = c_tmpBuf;
  }
  else if(ui_index == 1)
  {
    sprintf(c_tmpBuf,"%s","bottomlefttotopright");
    c_outputText = c_tmpBuf;
  }
  else return false;

  return true;
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

bool itolineart(int i_lineart, std::string& c_outputText)
{
  c_outputText.clear();
  for (int8_t i8_cnt = 15; i8_cnt >= 0; i8_cnt--)
  {
    if (i_lineart & (1<<i8_cnt))
      c_outputText.append("1");
    else
      c_outputText.append("0");
  }
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

bool itolinesuppression(uint8_t ui8_options, std::string& c_outputText)
{
  return itogeneraloption(ui8_options, c_outputText, maxLineSuppressionTable, &lineSuppressionTable[0][0]);
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

bool itoellipsetype(unsigned int ui_index, string& c_outputText)
{
  if(ui_index < maxEllipseTypeTable )
  {
    c_outputText = ellipseTypeTable[ui_index];
    return true;
  }
  return false;
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

bool itopolygontype(unsigned int ui_index, string& c_outputText)
{
  if(ui_index < maxPolygonTypeTable )
  {
    c_outputText = polygonTypeTable[ui_index];
    return true;
  }
  return false;
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

bool itovalidationtype(unsigned int ui_index, string& c_outputText)
{
  char c_tmpBuf[stringLength];

  if(ui_index == 0)
  {
    sprintf(c_tmpBuf,"%s","valid_characters");
    c_outputText = c_tmpBuf;
  }
  else if(ui_index == 1)
  {
    sprintf(c_tmpBuf,"%s","invalid_characters");
    c_outputText = c_tmpBuf;
  }
  else return false;

 return true;
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

bool itofilltype(unsigned int ui_index, string& c_outputText)
{
  if(ui_index < maxFillTypeTable )
  {
    c_outputText = fillTypeTable[ui_index];
    return true;
  }
  return false;
}


unsigned int
eventtoi (char *text_eventName)
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
auxfunctiontypetoi(char *text_auxFunctionType)
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

bool itoauxfunctiontype(unsigned int ui_index, string& c_outputText)
{
  if(ui_index < maxAuxFunctionTypes )
  {
    c_outputText = auxFunctionTypeTable[ui_index];
    return true;
  }
  return false;
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

bool itogcoptions(uint8_t ui8_options, string& c_outputText)
{
  return itogeneraloption(ui8_options, c_outputText, maxGCOptions, &GCOptionsTable[0][0]);
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

bool itoinputobjectoptions(uint8_t ui8_options, string& c_outputText)
{
  return itogeneraloption(ui8_options, c_outputText, maxInputObjectOptionsTable, &inputobjectOptionsTable[0][0]);
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

bool itobuttonoptions(uint8_t ui8_options, string& c_outputText)
{
  return itogeneraloption(ui8_options, c_outputText, maxButtonOptions, &buttonOptionsTable[0][0]);
}
