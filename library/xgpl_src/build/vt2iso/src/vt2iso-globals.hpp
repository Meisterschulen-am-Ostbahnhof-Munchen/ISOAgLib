/***************************************************************************
                          vt2iso-globals.h
                             -------------------
    begin                : Mon Mar 31 2003
    copyright            : (C) 2003 - 2004 by Dipl.-Inf.(FH)
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
#ifndef VT2ISGLOBALS_H
#define VT2ISGLOBALS_H

#include "vt2iso-defines.hpp"
#include <iostream>
#include <string>



extern bool itocolor(unsigned int ui_index, std::string& c_outputText);
extern bool itofontsize(unsigned int ui_index, std::string& c_outputText);
extern bool itofonttype(unsigned int ui_index, std::string& c_outputText);
extern bool itoformat(unsigned int ui_index, std::string& c_outputText);
extern bool itohorizontaljustification(unsigned int ui_index, std::string& c_outputText);
extern bool itopolygontype(unsigned int ui_index, std::string& c_outputText);
extern bool itoellipsetype(unsigned int ui_index, std::string& c_outputText);
extern bool itoacousticsignal(unsigned int ui_index, std::string& c_outputText);
extern bool itopriority(unsigned int ui_index, std::string& c_outputText);
extern bool itoauxfunctiontype(unsigned int ui_index, std::string& c_outputText);
extern bool itofilltype(unsigned int ui_index, std::string& c_outputText);
extern bool itovalidationtype(unsigned int ui_index, std::string& c_outputText);
extern bool itolineardirection(unsigned int ui_index, std::string& c_outputText);
extern bool itometeroptions(uint8_t ui8_options, std::string& c_outputText);
extern bool itoarchedbargraphoptions(uint8_t ui8_options, std::string& c_outputText);
extern bool itostringoptions(uint8_t ui8_options, std::string& c_outputText);
extern bool itoinputnumberoptions(uint8_t ui8_options, std::string& c_outputText);
extern bool itonumberoptions(uint8_t ui8_options, std::string& c_outputText);
extern bool itopicturegraphicoptions(uint8_t ui8_options, std::string& c_outputText);
extern bool itopicturegraphicrle(uint8_t ui8_options, std::string& c_outputText);
extern bool itolinearbargraphoptions(uint8_t ui8_options, std::string& c_outputText);
extern bool itogcoptions(uint8_t ui8_options, string& c_outputText);
extern bool itoinputobjectoptions(uint8_t ui8_options, string& c_outputText);
extern bool itobuttonoptions(uint8_t ui8_options, string& c_outputText);
extern bool itolinesuppression(uint8_t ui8_options, std::string& c_outputText);
extern bool itofontstyle(uint8_t ui8_options, std::string& c_outputText);
extern bool itolineart(int i_lineart, std::string& c_outputText);
extern bool itocolordepth(uint8_t ui8_options, std::string& c_outputText);


extern unsigned int colortoi (char* text_color);
extern unsigned int masktypetoi (char* masktype);
extern unsigned int colordepthtoi (char* text_colordepth);
extern signed int fonttypetoi (char* text_fonttype);
extern signed int booltoi (char *text_bool);
extern signed int fontsizetoi (char *text_fontsize);
extern signed int formattoi (char *text_format);
extern signed int horizontaljustificationtoi (char *text_horiz);
extern unsigned int stringoptionstoi (char *text_options);
extern unsigned int inputnumberoptionstoi (char *text_options);
extern unsigned int numberoptionstoi (char *text_options);
extern unsigned int picturegraphicoptionstoi (char *text_options);
extern unsigned int picturegraphicrletoi (char *text_options);
extern unsigned int meteroptionstoi (char *text_options);
extern unsigned int linearbargraphoptionstoi (char *text_options);
extern unsigned int archedbargraphoptionstoi (char *text_options);
extern signed int prioritytoi (char *text_priority);
extern signed int acousticsignaltoi (char *text_acousticsignal);
extern unsigned int fontstyletoi (char *text_fontstyle);
extern unsigned int linedirectiontoi (char *text_linedirection);
extern unsigned int linearttoi (char *text_lineart);
extern unsigned int linesuppressiontoi (char *text_linesuppression);
extern unsigned int ellipsetypetoi (char *text_ellipsetype);
extern unsigned int polygontypetoi (char *text_polygontype);
extern unsigned int validationtypetoi (char *text_validationtype);
extern unsigned int filltypetoi (char *text_filltype);
extern unsigned int eventtoi (char *text_eventName);
extern unsigned int auxfunctiontypetoi(char *text_auxFunctionType);
extern unsigned int gcoptionstoi (char *text_options);
extern unsigned int inputobjectoptiontoi (char *text_inputobjectoptions);
extern unsigned int buttonoptiontoi (char *text_buttonoptions);

#endif // #endif VT2ISGLOBALS_H
