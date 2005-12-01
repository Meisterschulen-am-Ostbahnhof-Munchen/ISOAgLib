/***************************************************************************
                          vt2iso.cpp
                             -------------------
    begin                : Mon Mar 31 2003
    copyright            : (C) 2003 - 2004 by Dipl.-Inf. Martin Wodok
    email                : m.wodok@osb-ag:de
    type                 : Source File
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
 ***************************************************************************/
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/parsers/AbstractDOMParser.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>
#include <xercesc/dom/DOMImplementationRegistry.hpp>
#include <xercesc/dom/DOMBuilder.hpp>
#include <xercesc/dom/DOMException.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMNodeList.hpp>
#include <xercesc/dom/DOMError.hpp>
#include <xercesc/dom/DOMLocator.hpp>
#include <xercesc/dom/DOMNamedNodeMap.hpp>
#include <xercesc/dom/DOMAttr.hpp>
#include <xercesc/dom/DOMElement.hpp>
#include <string>
#include <string.h>
#include <stdlib.h>

// Includes (findfirst, findnext)
#include <stddef.h>
#include <sys/types.h>

#ifdef WIN32
 #include <fstream>
 #include <windows.h>
// #include <winbase.h>
 #include <stdio.h>
 #include <ostream>
//        extern "C"
 using namespace std;
 RGBQUAD vtColorTable[256]=
 {
   {0x00,0x00,0x00,0x00},//0
   {0xFF,0xFF,0xff,0x00},//1
   {0x00,0x99,0x00,0x00},//2
   {0x99,0x99,0x00,0x00},//3
   {0x00,0x00,0x99,0x00},//4
   {0x99,0x00,0x99,0x00},//5
   {0x00,0x99,0x99,0x00},//6
   {0xCC,0xCC,0xCC,0x00},//7
   {0x99,0x99,0x99,0x00},//8
   {0xFF,0x00,0x00,0x00},//9
   {0x00,0xFF,0x00,0x00},//10
   {0xFF,0xFF,0x00,0x00},//11
   {0x00,0x00,0xFF,0x00},//12
   {0xFF,0x00,0xFF,0x00},//13
   {0x00,0xFF,0xFF,0x00},//14
   {0x99,0x00,0x00,0x00},//15
   {0x00,0x00,0x00,0x00},//16
   {0x33,0x00,0x00,0x00},//17
   {0x66,0x00,0x00,0x00},//18
   {0x99,0x00,0x00,0x00},//19
   {0xCC,0x00,0x00,0x00},//20
   {0xFF,0x00,0x00,0x00},//21
   {0x00,0x33,0x00,0x00},//22
   {0x33,0x33,0x00,0x00},//23
   {0x66,0x33,0x00,0x00},//24
   {0x99,0x33,0x00,0x00},//25
   {0xCC,0x33,0x00,0x00},//26
   {0xFF,0x33,0x00,0x00},//27
   {0x00,0x66,0x00,0x00},//28
   {0x33,0x66,0x00,0x00},//29
   {0x66,0x66,0x00,0x00},//30
   {0x99,0x66,0x00,0x00},//31
   {0xCC,0x66,0x00,0x00},//32
   {0xFF,0x66,0x00,0x00},//33
   {0x00,0x99,0x00,0x00},//34
   {0x33,0x99,0x00,0x00},//35
   {0x66,0x99,0x00,0x00},//36
   {0x99,0x99,0x00,0x00},//37
   {0xCC,0x99,0x00,0x00},//38
   {0xFF,0x99,0x00,0x00},//39
   {0x00,0xCC,0x00,0x00},//40
   {0x33,0xCC,0x00,0x00},//41
   {0x66,0xCC,0x00,0x00},//42
   {0x99,0xCC,0x00,0x00},//43
   {0xCC,0xCC,0x00,0x00},//44
   {0xFF,0xCC,0x00,0x00},//45
   {0x00,0xFF,0x00,0x00},//46
   {0x33,0xFF,0x00,0x00},//47
   {0x66,0xFF,0x00,0x00},//48
   {0x99,0xFF,0x00,0x00},//49
   {0xCC,0xFF,0x00,0x00},//50
   {0xFF,0xFF,0x00,0x00},//51
   {0x00,0x00,0x33,0x00},//52
   {0x33,0x00,0x33,0x00},//53
   {0x66,0x00,0x33,0x00},//54
   {0x99,0x00,0x33,0x00},//55
   {0xCC,0x00,0x33,0x00},//56
   {0xFF,0x00,0x33,0x00},//57
   {0x00,0x33,0x33,0x00},//58
   {0x33,0x33,0x33,0x00},//59
   {0x66,0x33,0x33,0x00},//60
   {0x99,0x33,0x33,0x00},//61
   {0xCC,0x33,0x33,0x00},//62
   {0xFF,0x33,0x33,0x00},//63
   {0x00,0x66,0x33,0x00},//64
   {0x33,0x66,0x33,0x00},//65
   {0x66,0x66,0x33,0x00},//66
   {0x99,0x66,0x33,0x00},//67
   {0xCC,0x66,0x33,0x00},//68
   {0xFF,0x66,0x33,0x00},//69
   {0x00,0x99,0x33,0x00},//70
   {0x33,0x99,0x33,0x00},//71
   {0x66,0x99,0x33,0x00},//72
   {0x99,0x99,0x33,0x00},//73
   {0xCC,0x99,0x33,0x00},//74
   {0xFF,0x99,0x33,0x00},//75
   {0x00,0xCC,0x33,0x00},//76
   {0x33,0xCC,0x33,0x00},//77
   {0x66,0xCC,0x33,0x00},//78
   {0x99,0xCC,0x33,0x00},//79
   {0xCC,0xCC,0x33,0x00},//80
   {0xFF,0xCC,0x33,0x00},//81
   {0x00,0xFF,0x33,0x00},//82
   {0x33,0xFF,0x33,0x00},//83
   {0x66,0xFF,0x33,0x00},//84
   {0x99,0xFF,0x33,0x00},//85
   {0xCC,0xFF,0x33,0x00},//86
   {0xFF,0xFF,0x33,0x00},//87
   {0x00,0x00,0x66,0x00},//88
   {0x33,0x00,0x66,0x00},//89
   {0x66,0x00,0x66,0x00},//90
   {0x99,0x00,0x66,0x00},//91
   {0xCC,0x00,0x66,0x00},//92
   {0xFF,0x00,0x66,0x00},//93
   {0x00,0x33,0x66,0x00},//94
   {0x33,0x33,0x66,0x00},//95
   {0x66,0x33,0x66,0x00},//96
   {0x99,0x33,0x66,0x00},//97
   {0xCC,0x33,0x66,0x00},//98
   {0xFF,0x33,0x66,0x00},//99
   {0x00,0x66,0x66,0x00},//100
   {0x33,0x66,0x66,0x00},//101
   {0x66,0x66,0x66,0x00},//102
   {0x99,0x66,0x66,0x00},//103
   {0xCC,0x66,0x66,0x00},//104
   {0xFF,0x66,0x66,0x00},//105
   {0x00,0x99,0x66,0x00},//106
   {0x33,0x99,0x66,0x00},//107
   {0x66,0x99,0x66,0x00},//108
   {0x99,0x99,0x66,0x00},//109
   {0xCC,0x99,0x66,0x00},//110
   {0xFF,0x99,0x66,0x00},//111
   {0x00,0xCC,0x66,0x00},//112
   {0x33,0xCC,0x66,0x00},//113
   {0x66,0xCC,0x66,0x00},//114
   {0x99,0xCC,0x66,0x00},//115
   {0xCC,0xCC,0x66,0x00},//116
   {0xFF,0xCC,0x66,0x00},//117
   {0x00,0xFF,0x66,0x00},//118
   {0x33,0xFF,0x66,0x00},//119
   {0x66,0xFF,0x66,0x00},//120
   {0x99,0xFF,0x66,0x00},//121
   {0xCC,0xFF,0x66,0x00},//122
   {0xFF,0xFF,0x66,0x00},//123
   {0x00,0x00,0x99,0x00},//124
   {0x33,0x00,0x99,0x00},//125
   {0x66,0x00,0x99,0x00},//126
   {0x99,0x00,0x99,0x00},//127
   {0xCC,0x00,0x99,0x00},//128
   {0xFF,0x00,0x99,0x00},//129
   {0x00,0x33,0x99,0x00},//130
   {0x33,0x33,0x99,0x00},//131
   {0x66,0x33,0x99,0x00},//132
   {0x99,0x33,0x99,0x00},//133
   {0xCC,0x33,0x99,0x00},//134
   {0xFF,0x33,0x99,0x00},//135
   {0x00,0x66,0x99,0x00},//136
   {0x33,0x66,0x99,0x00},//137
   {0x66,0x66,0x99,0x00},//138
   {0x99,0x66,0x99,0x00},//139
   {0xCC,0x66,0x99,0x00},//140
   {0xFF,0x66,0x99,0x00},//141
   {0x00,0x99,0x99,0x00},//142
   {0x33,0x99,0x99,0x00},//143
   {0x66,0x99,0x99,0x00},//144
   {0x99,0x99,0x99,0x00},//145
   {0xCC,0x99,0x99,0x00},//146
   {0xFF,0x99,0x99,0x00},//147
   {0x00,0xCC,0x99,0x00},//148
   {0x33,0xCC,0x99,0x00},//149
   {0x66,0xCC,0x99,0x00},//150
   {0x99,0xCC,0x99,0x00},//151
   {0xCC,0xCC,0x99,0x00},//152
   {0xFF,0xCC,0x99,0x00},//153
   {0x00,0xFF,0x99,0x00},//154
   {0x33,0xFF,0x99,0x00},//155
   {0x66,0xFF,0x99,0x00},//156
   {0x99,0xFF,0x99,0x00},//157
   {0xCC,0xFF,0x99,0x00},//158
   {0xFF,0xFF,0x99,0x00},//159
   {0x00,0x00,0xCC,0x00},//160
   {0x33,0x00,0xCC,0x00},//161
   {0x66,0x00,0xCC,0x00},//162
   {0x99,0x00,0xCC,0x00},//163
   {0xCC,0x00,0xCC,0x00},//164
   {0xFF,0x00,0xCC,0x00},//165
   {0x00,0x33,0xCC,0x00},//166
   {0x33,0x33,0xCC,0x00},//167
   {0x66,0x33,0xCC,0x00},//168
   {0x99,0x33,0xCC,0x00},//169
   {0xCC,0x33,0xCC,0x00},//170
   {0xFF,0x33,0xCC,0x00},//171
   {0x00,0x66,0xCC,0x00},//172
   {0x33,0x66,0xCC,0x00},//173
   {0x66,0x66,0xCC,0x00},//174
   {0x99,0x66,0xCC,0x00},//175
   {0xCC,0x66,0xCC,0x00},//176
   {0xFF,0x66,0xCC,0x00},//177
   {0x00,0x99,0xCC,0x00},//178
   {0x33,0x99,0xCC,0x00},//179
   {0x66,0x99,0xCC,0x00},//180
   {0x99,0x99,0xCC,0x00},//181
   {0xCC,0x99,0xCC,0x00},//182
   {0xFF,0x99,0xCC,0x00},//183
   {0x00,0xCC,0xCC,0x00},//184
   {0x33,0xCC,0xCC,0x00},//185
   {0x66,0xCC,0xCC,0x00},//186
   {0x99,0xCC,0xCC,0x00},//187
   {0xCC,0xCC,0xCC,0x00},//188
   {0xFF,0xCC,0xCC,0x00},//189
   {0x00,0xFF,0xCC,0x00},//190
   {0x33,0xFF,0xCC,0x00},//191
   {0x66,0xFF,0xCC,0x00},//192
   {0x99,0xFF,0xCC,0x00},//193
   {0xCC,0xFF,0xCC,0x00},//194
   {0xFF,0xFF,0xCC,0x00},//195
   {0x00,0x00,0xFF,0x00},//196
   {0x33,0x00,0xFF,0x00},//197
   {0x66,0x00,0xFF,0x00},//198
   {0x99,0x00,0xFF,0x00},//199
   {0xCC,0x00,0xFF,0x00},//200
   {0xFF,0x00,0xFF,0x00},//201
   {0x00,0x33,0xFF,0x00},//202
   {0x33,0x33,0xFF,0x00},//203
   {0x66,0x33,0xFF,0x00},//204
   {0x99,0x33,0xFF,0x00},//205
   {0xCC,0x33,0xFF,0x00},//206
   {0xFF,0x33,0xFF,0x00},//207
   {0x00,0x66,0xFF,0x00},//208
   {0x33,0x66,0xFF,0x00},//209
   {0x66,0x66,0xFF,0x00},//210
   {0x99,0x66,0xFF,0x00},//211
   {0xCC,0x66,0xFF,0x00},//212
   {0xFF,0x66,0xFF,0x00},//213
   {0x00,0x99,0xFF,0x00},//214
   {0x33,0x99,0xFF,0x00},//215
   {0x66,0x99,0xFF,0x00},//216
   {0x99,0x99,0xFF,0x00},//217
   {0xCC,0x99,0xFF,0x00},//218
   {0xFF,0x99,0xFF,0x00},//219
   {0x00,0xCC,0xFF,0x00},//220
   {0x33,0xCC,0xFF,0x00},//221
   {0x66,0xCC,0xFF,0x00},//222
   {0x99,0xCC,0xFF,0x00},//223
   {0xCC,0xCC,0xFF,0x00},//224
   {0xFF,0xCC,0xFF,0x00},//225
   {0x00,0xFF,0xFF,0x00},//226
   {0x33,0xFF,0xFF,0x00},//227
   {0x66,0xFF,0xFF,0x00},//228
   {0x99,0xFF,0xFF,0x00},//229
   {0xCC,0xFF,0xFF,0x00},//230
   {0xFF,0xFF,0xFF,0x00},//231
   //Proprietary
   {0x00,0x00,0x00,0x00},//232
   {0x00,0x00,0x00,0x00},//233
   {0x00,0x00,0x00,0x00},//234
   {0x00,0x00,0x00,0x00},//235
   {0x00,0x00,0x00,0x00},//236
   {0x00,0x00,0x00,0x00},//237
   {0x00,0x00,0x00,0x00},//238
   {0x00,0x00,0x00,0x00},//239
   {0x00,0x00,0x00,0x00},//240
   {0x00,0x00,0x00,0x00},//241
   {0x00,0x00,0x00,0x00},//242
   {0x00,0x00,0x00,0x00},//243
   {0x00,0x00,0x00,0x00},//244
   {0x00,0x00,0x00,0x00},//245
   {0x00,0x00,0x00,0x00},//246
   {0x00,0x00,0x00,0x00},//247
   {0x00,0x00,0x00,0x00},//248
   {0x00,0x00,0x00,0x00},//249
   {0x00,0x00,0x00,0x00},//250
   {0x00,0x00,0x00,0x00},//251
   {0x00,0x00,0x00,0x00},//252
   {0x00,0x00,0x00,0x00},//253
   {0x00,0x00,0x00,0x00},//254
   {0x00,0x00,0x00,0x00}//255
 };
#else
 #include <fstream.h>
 #include <dirent.h>
#endif

// Includes (vt2iso)
#include "vt2iso.hpp"
#include "vt2iso-defines.hpp"



/**** CHOOSE YOU IMAGE_PROCESSOR *****/
/** set it here or in the makefile! **/
// #define USE_FREE_IMAGE_LIB
// #define USE_PAINTLIB



#ifdef USE_FREE_IMAGE_LIB
 #include "vt2isoimagefreeimage_c.h"
#else
 #include "vt2isoimagepaintlib_c.h"
#endif


#include <string>
#include <set>
#include <iostream>
#include <vector>



#define MACRO_16bitToLE(value) (value & 0xFF), ((value >> 8) & 0xFF)
#define MACRO_32bitToLE(value) (value & 0xFF), ((value >> 8) & 0xFF), ((value >> 16) & 0xFF), ((value >> 24) & 0xFF)




// ### GLOBALS ###
#define iso639entries 136
char iso639table [iso639entries][2+1] = {{"aa"},{"ab"},{"af"},{"am"},{"ar"},{"as"},{"ay"},{"az"},{"ba"},{"be"},{"bg"},{"bh"},{"bi"},{"bn"},{"bo"},{"br"},{"ca"},{"co"},{"cs"},{"cy"},{"da"},{"de"},{"dz"},{"el"},{"en"},{"eo"},{"es"},{"et"},{"eu"},{"fa"},{"fi"},{"fj"},{"fo"},{"fr"},{"fy"},{"ga"},{"gd"},{"gl"},{"gn"},{"gu"},{"ha"},{"hi"},{"hr"},{"hu"},{"hy"},{"ia"},{"ie"},{"ik"},{"in"},{"is"},{"it"},{"iw"},{"ja"},{"ji"},{"jw"},{"ka"},{"kk"},{"kl"},{"km"},{"kn"},{"ko"},{"ks"},{"ku"},{"ky"},{"la"},{"ln"},{"lo"},{"lt"},{"lv"},{"mg"},{"mi"},{"mk"},{"ml"},{"mn"},{"mo"},{"mr"},{"ms"},{"mt"},{"my"},{"na"},{"ne"},{"nl"},{"no"},{"oc"},{"om"},{"or"},{"pa"},{"pl"},{"ps"},{"pt"},{"qu"},{"rm"},{"rn"},{"ro"},{"ru"},{"rw"},{"sa"},{"sd"},{"sg"},{"sh"},{"si"},{"sk"},{"sl"},{"sm"},{"sn"},{"so"},{"sq"},{"sr"},{"ss"},{"st"},{"su"},{"sv"},{"sw"},{"ta"},{"te"},{"tg"},{"th"},{"ti"},{"tk"},{"tl"},{"tn"},{"to"},{"tr"},{"ts"},{"tt"},{"tw"},{"uk"},{"ur"},{"uz"},{"vi"},{"vo"},{"wo"},{"xh"},{"yo"},{"zh"},{"zu"}};

FILE *partFileA;
FILE *partFileB;
FILE *partFileC;
FILE *partFileD;
FILE *partFileE; bool firstLineFileE = true;
FILE *partFileF;
FILE *partFileFderived;

char xmlFileGlobal [1024+1];

char std_bitmap_path [1024+1];
char fix_bitmap_path [1024+1];

std::basic_string<char> c_project;

char objNameTable [(stringLength+1)*4000];
unsigned int objIDTable [4000];
unsigned int objNextAutoID;
unsigned int objNextMacroAutoID;
unsigned int kcNextAutoID;
unsigned int objNextUnnamedName;
unsigned int objCount;
unsigned int opDimension; bool is_opDimension=false;
unsigned int skWidth;     bool is_skWidth=false;
unsigned int skHeight;    bool is_skHeight=false;

char attrString [maxAttributeNames] [stringLength+1];
bool attrIsGiven [maxAttributeNames];

// ---------------------------------------------------------------------------
//  GLOBAL Bitmap instance
// ---------------------------------------------------------------------------
//
#ifdef USE_FREE_IMAGE_LIB
  Vt2IsoImageFreeImage_c c_Bitmap;
#else
  Vt2IsoImagePaintlib_c c_Bitmap;
#endif

// ---------------------------------------------------------------------------
//  void usage () --- Prints out usage text.
// ---------------------------------------------------------------------------

static void usage()
{
  std::cout << "\nvt2iso BUILD DATE: 14-Jan-2005\n\n"
    "Usage:\n"
    " vt2iso [options] <XML file>\n\n"
    "This program invokes the DOMBuilder, builds the DOM tree,\n"
    "and then converts the tree to ISO Virtual Terminal cpp-files.\n\n"
    "Input files are all files starting with <XML file>,\nsorted by alphabetical order.\n"
    "This has been done to give the possibility to split \nlarge XML files into several ones.\n\n"
    "Options:\n"
    " -v=xxx   Validation scheme [always | never | auto]. Defaults to auto\n"
    " -n    Enable namespace processing. Defaults to off.\n"
    " -s    Enable schema processing. Defaults to off.\n"
    " -f    Enable full schema constraint checking. Defaults to off.\n"
    " -locale=ll_CC  specify the locale. Defaults to en_US.\n"
    " -?    Show this help.\n\n"
    << std::endl;
}


char proName[1024+1];

void clean_exit (int return_value, char* error_message=NULL)
{
  char partFileName [1024+1]; partFileName [1024+1-1] = 0x00;

  if (error_message != NULL)
    std::cout << error_message;

  if (partFileA) fclose (partFileA);

  if (partFileB) fclose (partFileB);

  if (partFileC) {
    fprintf (partFileC, "\n  for (int i=0;i<numObjects; i++) {");
    fprintf (partFileC, "\n    iVtObjects [i]->setOriginSKM (false);");
    fprintf (partFileC, "\n  }");
    fprintf (partFileC, "\n  b_initAllObjects = true;");
    fprintf (partFileC, "\n}\n");
    fclose (partFileC);
  }

  if (partFileD) {
    fprintf (partFileD, "\n#define vtKeyCodeACK 0\n");
    // OLD:  fprintf (partFileD, "\n#define vtObjectCount %d\n", objCount);
    fclose (partFileD);
  }

  if (partFileE) { // -list
    fputs ("\n};\n", partFileE);
    // write implementation of handler class constructor into list
    // as there the list must be known
    // -> the handler decleration can be included from everywhere
    fprintf (partFileE, "\n  iObjectPool_%s_c::iObjectPool_%s_c () : iIsoTerminalObjectPool_c (all_iVtObjects, %d, %d, %d, %d) {};\n",
    proName, proName, objCount, opDimension, skWidth, skHeight);

    fclose(partFileE);
  }
  if (partFileF) { // handler class direct
  // NEW:
    fprintf (partFileF, "\n #ifndef DECL_direct_iObjectPool_%s_c", proName );
    fprintf (partFileF, "\n #define DECL_direct_iObjectPool_%s_c", proName );
    fprintf (partFileF, "\nclass iObjectPool_%s_c : public IsoAgLib::iIsoTerminalObjectPool_c {", proName);
    fprintf (partFileF, "\npublic:");
    fprintf (partFileF, "\n  virtual void eventKeyCode (uint8_t keyActivationCode, uint16_t objId, uint16_t objIdMask, uint8_t keyCode, bool wasButton);");
    fprintf (partFileF, "\n  virtual void eventNumericValue (uint16_t objId, uint8_t ui8_value, uint32_t ui32_value);");
    fprintf (partFileF, "\n  virtual void eventStringValue (uint16_t rui16_objId, uint8_t rui8_length, StreamInput_c &refc_streaminput, uint8_t rui8_unparsedBytes, bool b_isFirst, bool b_isLast);");
    fprintf (partFileF, "\n  /* Uncomment the following function if you want to use input value string on-the-fly parsing/handling! */");
    fprintf (partFileF, "\n  //virtual void eventStringValueAbort();");
    fprintf (partFileF, "\n  virtual void eventObjectPoolUploadedSuccessfully ();");
    fprintf (partFileF, "\n  virtual void eventEnterSafeState ();");
    fprintf (partFileF, "\n  /* Uncomment the following function if you want to use command-response handling! */");
    fprintf (partFileF, "\n  //virtual void eventCommandResponse (uint8_t rui8_responseCommandError, const uint8_t rpui8_responseDataBytes[8]);");
    fprintf (partFileF, "\n  /* Uncomment the following function if you want to use a special colour-conversion! */");
    fprintf (partFileF, "\n  //virtual uint8_t convertColour (uint8_t colourValue, uint8_t colourDepth, IsoAgLib::iVtObject_c* obj, IsoAgLib::e_vtColour whichColour);");
    fprintf (partFileF, "\n  /* Uncomment the following function if you want to react on any incoming LANGUAGE_PGN */");
    fprintf (partFileF, "\n  //virtual void eventLanguagePgn (const localSettings_s& rrefs_localSettings);");
    fprintf (partFileF, "\n  void initAllObjectsOnce(SINGLETON_VEC_KEY_PARAMETER_DEF);");
    fprintf (partFileF, "\n  iObjectPool_%s_c ();", proName);
    fprintf (partFileF, "\n};\n");
    fprintf (partFileF, "\n #endif\n" );
    fclose (partFileF);
  }
  if (partFileFderived) { // handler class derived
  // NEW:
    fprintf (partFileFderived, "\n #ifndef DECL_derived_iObjectPool_%s_c", proName );
    fprintf (partFileFderived, "\n #define DECL_derived_iObjectPool_%s_c", proName );
    fprintf (partFileFderived, "\nclass iObjectPool_%s_c : public IsoAgLib::iIsoTerminalObjectPool_c {", proName);
    fprintf (partFileFderived, "\npublic:");
    fprintf (partFileFderived, "\n  void initAllObjectsOnce(SINGLETON_VEC_KEY_PARAMETER_DEF);");
    fprintf (partFileFderived, "\n  iObjectPool_%s_c ();", proName);
    fprintf (partFileFderived, "\n};\n");
    fprintf (partFileFderived, "\n #endif\n" );
    fclose (partFileFderived);
  }


// Write Direct Includes

  char* pc_lastDirectoryBackslash, *pc_lastDirectorySlash;
  char xmlFileWithoutPath[255];
  pc_lastDirectoryBackslash = strrchr( xmlFileGlobal, '\\' );
  pc_lastDirectorySlash = strrchr( xmlFileGlobal, '/' );

  if ( ( pc_lastDirectoryBackslash == NULL ) && ( pc_lastDirectorySlash == NULL ) )
  { // no path found
    strncpy( xmlFileWithoutPath, xmlFileGlobal, 254 );
  }
  else if ( ( pc_lastDirectoryBackslash == NULL ) && ( pc_lastDirectorySlash != NULL ) )
  { // only UNIX style slash found
    strncpy( xmlFileWithoutPath, (pc_lastDirectorySlash+1), 254 );
  }
  else if ( ( pc_lastDirectoryBackslash != NULL ) && ( pc_lastDirectorySlash == NULL ) )
  { // only WIN32 style Backslash found
    strncpy( xmlFileWithoutPath, (pc_lastDirectoryBackslash+1), 254 );
  }
  else if ( pc_lastDirectoryBackslash > pc_lastDirectorySlash )
  { // last backslash is behind last slash - cas of mixed directory seperators
    strncpy( xmlFileWithoutPath, (pc_lastDirectoryBackslash+1), 254 );
  }
  else
  { // last slash is behind last backslash - cas of mixed directory seperators
    strncpy( xmlFileWithoutPath, (pc_lastDirectorySlash+1), 254 );
  }


  strncpy (partFileName, xmlFileGlobal, 1024);
  strcat (partFileName, "_direct.h");
  partFileA = fopen (partFileName,"wt");

  fprintf (partFileA, "#include <IsoAgLib/comm/ISO_Terminal/ivtincludes.h>\n");
  fprintf (partFileA, "#include \"%s-handler-direct.inc\"\n", xmlFileWithoutPath);
  fprintf (partFileA, "#include \"%s-variables.inc\"\n", xmlFileWithoutPath);
  fprintf (partFileA, "#include \"%s-attributes.inc\"\n", xmlFileWithoutPath);
  fprintf (partFileA, "#include \"%s-list.inc\"\n", xmlFileWithoutPath);
  fprintf (partFileA, "#include \"%s-defines.inc\"\n", xmlFileWithoutPath);
  fprintf (partFileA, "#include \"%s-functions.inc\"\n", xmlFileWithoutPath);

  fclose (partFileA);


// Write Direct Includes
  strncpy (partFileName, xmlFileGlobal, 1024);
  strcat (partFileName, "_derived-cpp.h");
  partFileA = fopen (partFileName,"wt");


  fprintf (partFileA, "#include \"%s-variables.inc\"\n", xmlFileWithoutPath);
  fprintf (partFileA, "#include \"%s-attributes.inc\"\n", xmlFileWithoutPath);
  fprintf (partFileA, "#include \"%s-list.inc\"\n", xmlFileWithoutPath);
  fprintf (partFileA, "#include \"%s-defines.inc\"\n", xmlFileWithoutPath);
  fprintf (partFileA, "#include \"%s-functions.inc\"\n", xmlFileWithoutPath);

  fclose (partFileA);


// Write Direct Includes
  strncpy (partFileName, xmlFileGlobal, 1024);
  strcat (partFileName, "_derived-h.h");
  partFileA = fopen (partFileName,"wt");

  fprintf (partFileA, "#include <IsoAgLib/comm/ISO_Terminal/ivtincludes.h>\n");
  fprintf (partFileA, "#include \"%s-handler-derived.inc\"\n", xmlFileWithoutPath);

  fclose (partFileA);


  exit (return_value);
}



void copyWithQuoteAndLength (char *dest, const char *src, unsigned int len)
{
  *dest++ = '"';
  unsigned int take = (strlen(src) <= len) ? strlen(src) : len;
  unsigned int i=0;
  for (; i<take; i++) *dest++ = *src++;
  for (; i<len; i++) *dest++ = ' '; // fill with spaces if necessary
  *dest++ = '"';
  *dest = 0x00;
}



// ---------------------------------------------------------------------------
//  This is a simple class that lets us do easy (though not terribly efficient)
//  trancoding of char* data to XMLCh data.
// ---------------------------------------------------------------------------
class XStr
{
  public :
    // -----------------------------------------------------------------------
    //  Constructors and Destructor
    // -----------------------------------------------------------------------
    XStr(const char* const toTranscode)
    {
      // Call the private transcoding method
      fUnicodeForm = XMLString::transcode(toTranscode);
    }

    ~XStr()
    {
      XMLString::release(&fUnicodeForm);
    }


  // -----------------------------------------------------------------------
  //  Getter methods
  // -----------------------------------------------------------------------
  const XMLCh* unicodeForm() const
  {
    return fUnicodeForm;
  }

  private :
  // -----------------------------------------------------------------------
  //  Private data members
  //
  //  fUnicodeForm
  //   This is the Unicode XMLCh format of the string.
  // -----------------------------------------------------------------------
  XMLCh*   fUnicodeForm;
};

#define X(str) XStr(str).unicodeForm()



// ---------------------------------------------------------------------------
//  unsigned int getID (char* objName, bool isMacro, bool widhingID, unsigned int wishID=0)
// ---------------------------------------------------------------------------

unsigned int getID (char* objName, bool isMacro, bool wishingID, unsigned int wishID)
{
  bool isThere = false;
  unsigned int foundID = 0;

  // Added the following check. This is necessary so that objects like input lists which can contain lists of the NULL  object ID (65535)
  // do not assign object ids for this, and as a result count an additional object unnecessarily by incrementing objCount. -BAC 10-Jan-2004
  if (wishID == 65535)
  {
    return wishID;
  }
  // first check if ID is there already
  for (unsigned int i=0; i<objCount; i++)
  {
    // std::cout << "comparing " << objName << " with " << &objNameTable [i*(stringLength+1)] << "\n";
    if (strncmp (objName, &objNameTable [i*(stringLength+1)], stringLength) == 0)
    {
      foundID = objIDTable [i];
      isThere = true;
      break;
    }
  }

  if (!isThere) {
    // check what's the new ID to be
    if (wishingID) {
      foundID = wishID;
    } else {
      if (isMacro) {
        foundID = objNextMacroAutoID;
        objNextMacroAutoID--;
      } else {
        foundID = objNextAutoID;
        objNextAutoID--;
      }
    }
    // insert new name-id pair now!
    objIDTable [objCount] = foundID;
    strncpy (&objNameTable [objCount*(stringLength+1)], objName, stringLength); // so we have 0-termination in every case, as our strings are 128+1 bytes!
//     printf("objName: %s, objCount: %i\n", objName, objCount);
    objCount++;
  }
  // else { take ID found }
  return (foundID);
}


unsigned int idOrName_toi(char* rpc_string, bool rb_isMacro)
{
  if (rpc_string [0] == 0x00) clean_exit (-1, "*** ERROR *** idOrName_toi: Empty 'object_id' attribute!\n\n");
  /** @todo check if all chars in the string are numbers, not only the first! */
  if ((rpc_string [0] >= '0') && (rpc_string [0] <= '9')) return atoi (rpc_string);
  // Starting with a letter, so look up id!
  return getID (rpc_string, rb_isMacro, false, 0);
}


void getKeyCode ()
{
  attrIsGiven [attrKey_code] = true;
  sprintf (attrString [attrKey_code], "%d", kcNextAutoID);
  kcNextAutoID--;
}

void init (const char* xmlFile)
{
  strcpy (xmlFileGlobal, xmlFile);

  char partFileName [1024+1]; partFileName [1024+1-1] = 0x00;

  for (int i=0; i<((stringLength+1)*1000); i++)
  {
    objNameTable [i] = 0x00;
  }
  objCount = 0;
  objNextAutoID = 65534;
  objNextMacroAutoID = 255;
  kcNextAutoID = 254; // for safety, 255 should also be okay though...
  objNextUnnamedName = 1;

// partFileA = fopen ("picture.raw", "wb");
// fwrite (vtObjectdeXbitmap1_aRawBitmap, 16384, 1, partFileA);
// fclose (partFileA);

  strncpy (partFileName, xmlFile, 1024);
  strcat (partFileName, "-variables.inc");
  partFileA = fopen (partFileName,"wt");

  strncpy (partFileName, xmlFile, 1024);
  strcat (partFileName, "-attributes.inc");
  partFileB = fopen (partFileName,"wt");

  strncpy (partFileName, xmlFile, 1024);
  strcat (partFileName, "-functions.inc");
  partFileC = fopen (partFileName,"wt");
  fprintf (partFileC, "void iObjectPool_%s_c::initAllObjectsOnce (SINGLETON_VEC_KEY_PARAMETER_DEF)\n{\n", proName);
  fprintf (partFileC, "  if (b_initAllObjects) return;   // so the pointer to the ROM structures are only getting set once on initialization!\n");

  strncpy (partFileName, xmlFile, 1024);
  strcat (partFileName, "-defines.inc");
  partFileD = fopen (partFileName,"wt");

  strncpy (partFileName, xmlFile, 1024);
  strcat (partFileName, "-list.inc");
  partFileE = fopen (partFileName,"wt");
  fputs ("IsoAgLib::iVtObject_c* all_iVtObjects [] = {", partFileE);

  strncpy (partFileName, xmlFile, 1024);
  strcat (partFileName, "-handler-direct.inc");
  // check if "-hanlder-direct" is there, in this case generate "-handler-direct.inc-template" !
  partFileF = fopen (partFileName,"rb");
  if (partFileF) {
   // could open file, so it exists --> don't overwrite - create "-template" then
   fclose (partFileF);
   strcat (partFileName, "-template");
  } else {
    // file couldn't be opened, so create it, simply write to it...
  }
  partFileF = fopen (partFileName,"wt");

  strncpy (partFileName, xmlFile, 1024);
  strcat (partFileName, "-handler-derived.inc");
  partFileFderived = fopen (partFileName,"wt");

  for (int j=0; j<maxAttributeNames; j++)
    attrString [j] [stringLength+1-1] = 0x00;
}


void defaultAttributes ()
{
  if (!attrIsGiven [attrBackground_colour]) {
    sprintf (attrString [attrBackground_colour], "white");
    attrIsGiven [attrBackground_colour] = true;
  }
  if (!attrIsGiven [attrSelectable]) {
    sprintf (attrString [attrSelectable], "yes");
    attrIsGiven [attrSelectable] = true;
  }
  if (!attrIsGiven [attrSoft_key_mask]) {
    sprintf (attrString [attrSoft_key_mask], "NULL");
    attrIsGiven [attrSoft_key_mask] = true;
  }
  if (!attrIsGiven [attrHidden]) {
    sprintf (attrString [attrHidden], "no");
    attrIsGiven [attrHidden] = true;
  }
  if (!attrIsGiven [attrBorder_colour]) {
    sprintf (attrString [attrBorder_colour], "black");
    attrIsGiven [attrBorder_colour] = true;
  }
  if (!attrIsGiven [attrLatchable]) {
    sprintf (attrString [attrLatchable], "no");
    attrIsGiven [attrLatchable] = true;
  }
  if (!attrIsGiven [attrHorizontal_justification]) {
    sprintf (attrString [attrHorizontal_justification], "left");
    attrIsGiven [attrHorizontal_justification] = true;
  }
  if (!attrIsGiven [attrFont_style]) {
    sprintf (attrString [attrFont_style], "normal"); // anything that doesn't match anything from the fontstyleTable
    attrIsGiven [attrFont_style] = true;
  }
  // I think it's better to let the user specify, 8bit as default would blow up the pool, and if only b/w was wanted to be used.. too bad then.
  // if (!attrIsGiven [attrFormat]) {
  //  sprintf (attrString [attrFormat], "8bit");
  //  attrIsGiven [attrFormat] = true;
  // }
  if (!attrIsGiven [attrVariable_reference]) {
    sprintf (attrString [attrVariable_reference], "NULL");
    attrIsGiven [attrVariable_reference] = true;
  }
  if (!attrIsGiven [attrTarget_value_variable_reference]) {
    sprintf (attrString [attrTarget_value_variable_reference], "NULL");
    attrIsGiven [attrTarget_value_variable_reference] = true;
  }
  if (!attrIsGiven [attrEnabled]) {
    sprintf (attrString [attrEnabled], "yes");
    attrIsGiven [attrEnabled] = true;
  }
  /*
  if (!attrIsGiven [attr]) {
    sprintf (attrString [attr], "");
    attrIsGiven [attr] = true;
  }
  */
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
  if ((atoi(text_fonttype) == 0) || (atoi(text_fonttype) == 1) || (atoi(text_fonttype) == 255)) return atoi(text_fonttype);
  for (l=0; l<3; l++) {
    if (strncmp (text_fonttype, fonttypeTable [l], stringLength) == 0) {
      if (l == 2) return 0xFF;
      return l;
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
    if (strstr (text_fontstyle, fontstyleTable [l]) != NULL) {
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

/* ?????? ---OBSOLETE--- ??????
 *******************************

unsigned int componenttoindex6 (unsigned int component)
{
    if (component < (0x33 - 0x33/2)) return 0;
    if (component < (0x66 - 0x33/2)) return 1;
    if (component < (0x99 - 0x33/2)) return 2;
    if (component < (0xCC - 0x33/2)) return 3;
    if (component < (0xFF - 0x33/2)) return 4;
    return 0x5;
}


unsigned int componentto09CF (unsigned int comp)
{
    if (comp < (0x99/2)) return 0x00;
    if (comp < (0xCC-0x99)/2+0x99) return 0x99;
    if (comp < (0xFF-0xCC)/2+0xCC) return 0xCC;
    return 0xFF;
}

unsigned int componentto09F (unsigned int comp)
{
    if (comp < (0x99/2)) return 0x00;
    if (comp < (0xFF-0x99)/2+0x99) return 0xCC;
    return 0xFF;
}

unsigned int color16table [16] [3] = {
{0x00,0x00,0x00},
{0xFF,0xFF,0xFF},
{0x00,0x99,0x00},
{0x00,0x99,0x99},
{0x99,0x00,0x00},
{0x99,0x00,0x99},
{0x99,0x99,0x00},
{0xCC,0xCC,0xCC},
{0x99,0x99,0x99},
{0x00,0x00,0xFF},
{0x00,0xFF,0x00},
{0x00,0xFF,0xFF},
{0xFF,0x00,0x00},
{0xFF,0x00,0xFF},
{0xFF,0xFF,0x00},
{0x00,0x00,0x99}};

unsigned int indexfromtable (unsigned int red, unsigned int green, unsigned int blue)
{
    for (int i=0; i<16; i++) {
        if ((color16table [i] [0] == red) && (color16table [i] [1] == green) && (color16table [i] [2] == blue))
            return i;
    }
    return 0; // shouldn't happen!!
}

unsigned int rgbtopalette16 (unsigned int red, unsigned int green, unsigned int blue)
{
    red = componentto09CF (red);
    green = componentto09CF (green);
    blue = componentto09CF (blue);
    if ((red <= 0x99) && (green <= 0x99) && (blue <= 0x99)) return indexfromtable (red, green, blue); // 0x00s & 0x99s
    if (((red == 0xFF) || (red == 0x00)) && ((green == 0xFF) || (green == 0x00)) && ((blue == 0xFF) || (blue == 0x00))) return indexfromtable (red, green, blue); // 0x00s & 0xFFs
    if ((red == 0xCC) && (green == 0xCC) && (blue == 0xCC)) return 7; // special-case silver 0xCC 0xCC 0xCC
    red = componentto09F (red);
    green = componentto09F (green);
    blue = componentto09F (blue);
    if ((red == 0xFF) || (green == 0xFF) || (blue == 0xFF)) { // at least one component is 0xFF, so set the others to 0x00
        if (red != 0xFF) red = 0x00;
        if (green != 0xFF) green = 0x00;
        if (blue != 0xFF) blue = 0x00;
    }
    return indexfromtable (red, green, blue);
}
**************************************
*/



/* ??????? MAYBE IN BASE ?????????
 **********************************

// Commented this function out . . . needs work to be able to work with 8 bpp bitmaps . . . Don't need right now anyway . . .-BAC 9/9/04

// calculate optimal threshold for bw image generation
  // avoid complete white or complete black image
//int getOptimalBwThreshold( unsigned int width, unsigned int height, PLPixel32** pLineArray ) {
////////  PLPixel32* pLine;
////////  int threshold128AllWhite = ( ( width * height ) / 8 ),
////////      threshold128AllBlack = ( ( width * height ) / 8 ),
////////      thresholdOptimalAllWhite = ( ( width * height ) / 8 ),
////////      thresholdOptimalAllBlack = ( ( width * height ) / 8 ),
////////      thresholdLoopAllWhite = 0,
////////      thresholdLoopAllBlack = 0,
////////      optimalThreshold;
////////  for ( int threshold = 32; threshold <= 224; threshold += 16 ) {
////////    thresholdLoopAllWhite = 0;
////////    thresholdLoopAllBlack = 0;
////////    for (int y=0; y<height; y++) {
////////        pLine = pLineArray[y];
////////        for (int x=0; x<width; x+=8) {
////////            unsigned int pixel1PaletteIndex = (((pLine[x  ].GetR() + pLine[x  ].GetG() + pLine[x  ].GetB())/3) >= threshold )?1:0;
////////            unsigned int pixel2PaletteIndex = (((pLine[x+1].GetR() + pLine[x+1].GetG() + pLine[x+1].GetB())/3) >= threshold )?1:0;
////////            unsigned int pixel3PaletteIndex = (((pLine[x+2].GetR() + pLine[x+2].GetG() + pLine[x+2].GetB())/3) >= threshold )?1:0;
////////            unsigned int pixel4PaletteIndex = (((pLine[x+3].GetR() + pLine[x+3].GetG() + pLine[x+3].GetB())/3) >= threshold )?1:0;
////////            unsigned int pixel5PaletteIndex = (((pLine[x+4].GetR() + pLine[x+4].GetG() + pLine[x+4].GetB())/3) >= threshold )?1:0;
////////            unsigned int pixel6PaletteIndex = (((pLine[x+5].GetR() + pLine[x+5].GetG() + pLine[x+5].GetB())/3) >= threshold )?1:0;
////////            unsigned int pixel7PaletteIndex = (((pLine[x+6].GetR() + pLine[x+6].GetG() + pLine[x+6].GetB())/3) >= threshold )?1:0;
////////            unsigned int pixel8PaletteIndex = (((pLine[x+7].GetR() + pLine[x+7].GetG() + pLine[x+7].GetB())/3) >= threshold )?1:0;
////////            unsigned int byte = (pixel1PaletteIndex << 7) + (pixel2PaletteIndex << 6) + (pixel3PaletteIndex << 5) + (pixel4PaletteIndex << 4) + (pixel5PaletteIndex << 3) + (pixel6PaletteIndex << 2) + (pixel7PaletteIndex << 1) + pixel8PaletteIndex;
////////            if ( byte == 0 ) thresholdLoopAllWhite++;
////////            else if ( byte == 0xFF ) thresholdLoopAllBlack++;
////////        } // loop one columns of one line
////////    } // iterate loop for all lines
////////    if ( ( thresholdLoopAllWhite + thresholdLoopAllBlack ) < ( thresholdOptimalAllWhite + thresholdOptimalAllBlack ) ) {
////////      // new optimum found
////////      thresholdOptimalAllWhite = thresholdLoopAllWhite;
////////      thresholdOptimalAllBlack = thresholdLoopAllBlack;
////////      optimalThreshold = threshold;
////////    }
////////    if ( threshold == 128 ) {
////////      // currently 128 - default - analysed
////////      threshold128AllWhite = thresholdLoopAllWhite;
////////      threshold128AllBlack = thresholdLoopAllBlack;
////////    }
////////  }
////////  // now decide about optimal threshold -> use 128 if optimum is not BETTER
////////  if ( ( thresholdOptimalAllWhite + thresholdOptimalAllBlack ) < ( threshold128AllWhite + threshold128AllBlack ) ) {
////////    // optimal threshold is really better
////////   std::cerr << "Found Optimal Threshold: " << optimalThreshold << " which has only " << thresholdOptimalAllWhite << " all white and "
////////    << thresholdOptimalAllBlack << " all black entries" << std::endl;
////////    return optimalThreshold;
////////  }
////////  else {
////////    // no better threshold than standard 128 found
////////    std::cerr << "Default 128 Threshold is optimal as it has only " << threshold128AllWhite << " all white and "
////////         << threshold128AllBlack << " all black entries\n"
////////         << "Optimal Threshold has " << thresholdOptimalAllWhite << " allWhite and " << thresholdOptimalAllBlack
////////         << " all black"
////////   << std::endl;
////////    return 128;
////////  }
////////}

**************************************
*/

// Assuming an 8 bit per pixel bitmap.
static unsigned char picBuffer [480*480];

static char attr_name [1024+1];
static char attr_value [1024+1];
static char filename [1024+1];

char objName [stringLength+1];
bool is_objName;
unsigned int objID;
bool is_objID;

/* sets the passed attribute if name matches id */
void setAttributeValue(int attrID)
{
  if (strncmp (attr_name, attrNameTable[attrID], stringLength) == 0)
  {
    strncpy (attrString [attrID], attr_value, stringLength);
    attrIsGiven [attrID] = true;
  }
}

/* cleans the passed attribute value */
void cleanAttribute(int attrID)
{
  attrString [attrID] [stringLength+1-1] = 0x00;
  attrIsGiven [attrID] = false;
}

void utf16convert (char* source, char* destin, int count)
{
  int i=-1;
  do {
    i++;
    destin [i] = source [(i*2)];
  } while (destin [i] != 0x00);
}

void checkForFileOrFile148 (char *tag) {
  char errMsg[1024+1]; errMsg[0] = 0x00;
  if (!attrIsGiven [attrFile]) {
    switch (colordepthtoi (attrString [attrFormat])) {
      case 2: if (!(attrIsGiven [attrFile0] && attrIsGiven [attrFile2]))
                sprintf (errMsg, "YOU NEED TO SPECIFY THE file= OR MINIMUM fileX= (x=0,2) ATTRIBUTES FOR THE <%s> OBJECT! STOPPING PARSER! bye.\n\n", tag);
              break;
      case 1: if (!(attrIsGiven [attrFile0] && attrIsGiven [attrFile1]))
                sprintf (errMsg, "YOU NEED TO SPECIFY THE file= OR ALL fileX= (x=0,1) ATTRIBUTES FOR THE <%s> OBJECT! STOPPING PARSER! bye.\n\n", tag);
              break;
      case 0: if (!(attrIsGiven [attrFile0]))
                sprintf (errMsg, "YOU NEED TO SPECIFY THE file= OR THE file0= ATTRIBUTE FOR THE <%s> OBJECT! STOPPING PARSER! bye.\n\n", tag);
              break;
    }
    if (strlen(errMsg)) clean_exit (-1, errMsg);
  }
}


void getAttributesFromNode(DOMNode *n, bool treatSpecial) {
  DOMNamedNodeMap *pAttributes;
  if (n->hasAttributes()) { // parse through all attributes
    pAttributes = n->getAttributes();
    int nSize = pAttributes->getLength();

    // empty all possible attribute-values...
    for (int j=0; j<maxAttributeNames; j++) {
      for (int k=0; k<stringLength+1; k++) {
        attrString [j] [k] = 0x00;
      }
      attrIsGiven [j] = false;
    }

   // now get all attributes listed in the <tag ...> element
    for (int i=0;i<nSize;++i) {
      DOMAttr *pAttributeNode = (DOMAttr*) pAttributes->item(i);
      utf16convert ((char *)pAttributeNode->getName(), attr_name, 1024);
      utf16convert ((char *)pAttributeNode->getValue(), attr_value, 1024);

      if (treatSpecial) { // get 'name=', 'id=' and all other possible attributes
        if (strncmp (attr_name, "name", stringLength) == 0) {
          strncpy (objName, attr_value, stringLength);
          is_objName = true;
          continue;
        }
        if (strncmp (attr_name, "id", stringLength) == 0) {
          objID = atoi (attr_value);
          is_objID = true;
          continue;
        }
      }
      int l;
      for (l=0; l<maxAttributeNames; l++) {
        if (strncmp (attr_name, attrNameTable [l], stringLength) == 0) {
          strncpy (attrString [l], attr_value, stringLength);
          attrIsGiven [l] = true;
    // DEBUG-OUT
    //      std::cout << "FOUND ATTR: IND " << l << ":= " << attrNameTable [l] << " -> " << attrString[l] << ":"
    //                << attrIsGiven [l] << "\n";
          break;
        }
      }

      // ERROR: We didn't match a possible attribute name
      if (l == maxAttributeNames)
      {
        std::cout << "\n\nUNKNOWN ATTRIBUTE " << attr_name <<"="<< attr_value <<" IN TAG <"<< XMLString::transcode(n->getNodeName()) <<"> ENCOUNTERED! STOPPING PARSER! bye.\n\n";
        clean_exit (-1);
      }
    }
  }

  if (treatSpecial) {
    // If no 'name=' given, add 'name=unnamed%d' attribute
    if (is_objName == false)
    {
      sprintf (objName, "Unnamed%d", objNextUnnamedName);
      ((DOMElement *)n)->setAttribute (X("name"), X(objName));
      objNextUnnamedName++;
      is_objName = true;
    }
  }
}


void openDecodePrintOut (const char* workDir, char* _bitmap_path, unsigned int &options, int fixNr=-1)
{
  std::cout << "Bitmappath: " << _bitmap_path << std::endl;
  if (attrIsGiven [attrRle]) {
    // set rle stuff
    unsigned int rle = picturegraphicrletoi (attrString [attrRle]);
    // replace possible rle='auto' by 'rle1+rle4+rle8'
    if (rle & (uint64_t(1)<<3)) rle = (uint64_t(1)<<0) | (uint64_t(1)<<1) | (uint64_t(1)<<2);
    // and merge to options byte
    options |= rle<<2;
  }

  // generate all lower depth-bitmaps...
  for (unsigned int actDepth=0; actDepth <= colordepthtoi (attrString [attrFormat]); actDepth++) {
    if (fixNr == -1) { // noFix
      // It's allowed to leave out 16-color bitmaps as there's a fallback to 2-color bitmap!!
      if ((actDepth == 1) && ((!attrIsGiven[attrFile1] && !attrIsGiven[attrFile]) || (attrIsGiven [attrFile1] && (strlen (attrString [attrFile1]) == 0)))) continue;
    } else {
      if (attrIsGiven [attrFile0+actDepth] && (strlen (attrString [attrFile0+actDepth]) == 0)) continue;
    }

    if (fixNr == -1) fprintf (partFileB, "const HUGE_MEM uint8_t iVtObject%s_aRawBitmap%d [] = {", objName, actDepth);
    else /* -fix- */ fprintf (partFileB, "const HUGE_MEM uint8_t iVtObject%s_aRawBitmap%dFixed%d [] = {", objName, actDepth, fixNr);

    if (attrIsGiven [attrFile0+actDepth]) sprintf (filename, "%s%s/%s", workDir, _bitmap_path, attrString [attrFile0+actDepth]);
    else /* use std file for all depth */ sprintf (filename, "%s%s/%s", workDir, _bitmap_path, attrString [attrFile]);

    // Open Bitmap
    std::cout << std::endl; // Opening text is printed out by openBitmap
    if ( c_Bitmap.openBitmap( filename ) ) std::cout << "Loaded successfully!\n";
    else clean_exit (-1, "Loading failed!\n");

    // Decode bitmap to buffer!
    switch (actDepth) {
      case 0: // 1 bit colour (monochrome) = 2 colors (black/white)
        c_Bitmap.write1BitBitmap( picBuffer );
        break;
      case 1: // 4 bit colour = 16 colours
        c_Bitmap.write4BitBitmap( picBuffer );
        break;
      case 2: // 8 bit colour = 256 colours
        c_Bitmap.write8BitBitmap( picBuffer );
        break;
    } // switch

    if (c_Bitmap.objRawBitmapBytes [actDepth] == 0) clean_exit (-1, "===> Bitmap with size 0. Terminating!\n\n");

    // Is RLE wanted/sensible? ("rle[actDepth]" set?)
    if (options & (uint64_t(1)<<(2+actDepth))) {
      unsigned int offset=0;
      unsigned int rleBytes=0;
      while (offset < c_Bitmap.objRawBitmapBytes [actDepth]) {
        int cnt=1;
        while (((offset+cnt) < c_Bitmap.objRawBitmapBytes [actDepth]) && (picBuffer [offset] == picBuffer [offset+cnt]) && (cnt < 255)) cnt++;
        offset += cnt;
        rleBytes += 2;
      }
      // If RLE-size is larger than uncompressed size, clear the RLE1/4/8 flag!
      if (rleBytes >= c_Bitmap.objRawBitmapBytes [actDepth]) {
        options &= -1-(uint64_t(1)<<(2+actDepth));
        std::cout << "++INFORMATION++: <picturegraphic name='" << objName << "' actDepth012='"<< actDepth <<"'... /> has RLE-compressed bitmap-size of "<< rleBytes
                  << " bytes but uncompressed bitmap-size of "<< c_Bitmap.objRawBitmapBytes [actDepth] <<" bytes. Turning RLE-Encoding off for that object!\n";
      }
    }

    // Print out raw data or rle_data?
    if (options & (uint64_t(1)<<(2+actDepth))) { // rle1, rle4 or rle8 "STILL" set?
      //// +++ RLE-COMPRESSED OUTPUT +++
      unsigned int offset=0;
      int rleBytes=0;
      int PixelCount = 0;
      while (offset < c_Bitmap.objRawBitmapBytes [actDepth]) {
        unsigned int cnt=1;
        while (((offset+cnt) < c_Bitmap.objRawBitmapBytes [actDepth]) && (picBuffer [offset] == picBuffer [offset+cnt]) && (cnt < 255)) cnt++;
        if (offset == 0) fprintf (partFileB, " /* RLE-Encoded Raw-Bitmap Data */ %d, %d", cnt, picBuffer [offset]);
        else /* ----- */ fprintf (partFileB, ",%d, %d", cnt, picBuffer [offset]);
        offset += cnt;
        rleBytes += 2;
        PixelCount += cnt;
      }
      std::cout << "information: <picturegraphic name='" << objName << "' actDepth012='"<< actDepth <<"'... /> has RLE-compressed bitmap-size of "<< rleBytes
          << " bytes and uncompressed bitmap-size of "<< c_Bitmap.objRawBitmapBytes [actDepth] <<" bytes.\n";
      std::cout << "Bitmap PixelCount: " << PixelCount << ", Width: " << c_Bitmap.getWidth() << ", Height: " << c_Bitmap.getHeight() << "\n\n";
      c_Bitmap.objRawBitmapBytes [actDepth] = rleBytes;
    } else {
      //// +++ NORMAL UNCOMPRESSED OUTPUT +++
      if (actDepth == 1) { // nur der lesbarkeit halber!!
        fprintf (partFileB, " /* 16-Color Raw-Bitmap Data */ (%d << 4) | %d", picBuffer [0] >> 4, picBuffer [0] & 0xF);
        for (unsigned int i=1; i<c_Bitmap.objRawBitmapBytes [actDepth]; i++) {
          fprintf (partFileB, ", (%d << 4) | %d", picBuffer [i] >> 4, picBuffer [i] & 0xF);
        }
      } else {
        fprintf (partFileB, " /* Raw-Bitmap Data */ %d", picBuffer [0]);
        for (unsigned int i=1; i<c_Bitmap.objRawBitmapBytes [actDepth]; i++) {
          fprintf (partFileB, ", %d", picBuffer [i]);
        }
      }
    }
    fprintf (partFileB, "};\n");
  } // for actDepth...
}

// ---------------------------------------------------------------------------
//
//  static void processElement (DOMNode *n, unsigned int omb, const char* rc_workDir) --- Recursively process all ELEMENT XML-Tags...
// we're in an element here, not text or something else...
//
// ---------------------------------------------------------------------------
#ifdef WIN32
void processElement (DOMNode *n, uint64_t ombType, const char* rc_workDir)
#else
static void processElement (DOMNode *n, uint64_t ombType, const char* rc_workDir)
#endif
{
  DOMNode *child;
  DOMNamedNodeMap *pAttributes;
  char *node_name = XMLString::transcode(n->getNodeName());

  // all possible values of the objects
  unsigned int objChildID=0; bool is_objChildID=false; //init for happy compiler
  unsigned int objChildX=0; bool is_objChildX=false; //init for happy compiler
  unsigned int objChildY=0; bool is_objChildY=false; //init for happy compiler
  unsigned int objBlockRow=0; //init for happy compiler
  unsigned int objBlockCol=0; //init for happy compiler
  unsigned int objType=0; //init for happy compiler
  unsigned int commandType=0; //init for happy compiler
  unsigned int objChildObjects=0; //init for happy compiler

  unsigned int deXactualWidth=0; //init for happy compiler
  unsigned int deXactualHeight=0; //init for happy compiler
  unsigned int deXwidth=0; //init for happy compiler
  unsigned int deXcolorDepth=0; //init for happy compiler
  unsigned int deXtransCol=0; //init for happy compiler
  unsigned int stdRawBitmapBytes [3];

  #define maxFixedBitmaps 1000

  unsigned int fiXactualWidth [maxFixedBitmaps];
  unsigned int fiXactualHeight [maxFixedBitmaps];
  //unsigned int deXtransCol [maxFixedBitmaps];
  unsigned int fixRawBitmapBytes [maxFixedBitmaps] [3];
  unsigned int fixBitmapOptions [maxFixedBitmaps];
  unsigned int fixNr=0; //init for happy compiler

  unsigned int objChildMacros=0; //init for happy compiler
  unsigned int objChildCommands=0; //init for happy compiler
  unsigned int objChildPoints=0; //init for happy compiler
  unsigned int objChildLanguages=0; //init for happy compiler

  unsigned int objBitmapOptions;

  char commandMessage[stringLength+1];
  char objChildName [stringLength+1]; bool is_objChildName=false; //init for happy compiler
  char tempString [stringLength+1]; tempString [stringLength+1-1] = 0x00;
  char tempString2 [stringLength+1]; tempString2 [stringLength+1-1] = 0x00;
  char objBlockFont [stringLength+1];

  objName [stringLength+1-1] = 0x00;
  is_objName=false;
  is_objID=false;


  if (!n) return;

  // get own ObjectType
  objType = objectIsType (node_name); // returns 0..34


  // get own Command Type
  commandType = commandIsType (node_name);

  // ERROR: Wrong <TAG>
  if (objType == 0xFFFF && commandType == 0xFFFF) {
    std::cout << "\n\nUNKNOWN TAG <"<< node_name <<"> ENCOUNTERED! STOPPING PARSER! bye.\n\n";
    clean_exit (-1);
  }

  if (objType >= maxObjectTypes) {
    if (objType == otObjectpool) {
      // expect (datamask) dimension here
      if (n->hasAttributes()) { // parse through all attributes
        pAttributes = n->getAttributes();
        int nSize = pAttributes->getLength();

        // now get all attributes listed in the <tag ...> element
        for (int i=0;i<nSize;++i) {
          DOMAttr *pAttributeNode = (DOMAttr*) pAttributes->item(i);
          utf16convert ((char *)pAttributeNode->getName(), attr_name, 1024);
          utf16convert ((char *)pAttributeNode->getValue(), attr_value, 1024);

          // get 'name=', 'id=' and all other possible attributes
          if (strncmp (attr_name, "dimension", stringLength) == 0) {
            if (is_opDimension) {
              std::cout << "\n\nYOU MUSTN'T SPECIFY THE dimension= TAG IN <objectpool> MORE THAN ONCE! STOPPING PARSER! bye.\n\n";
              clean_exit (-1);
            }
            opDimension = atoi (attr_value);
            is_opDimension = true;
            continue;
          }
          if (strncmp (attr_name, "sk_width", stringLength) == 0) {
            if (is_skWidth) {
              std::cout << "\n\nYOU MUSTN'T SPECIFY THE sk_width= TAG IN <objectpool> MORE THAN ONCE! STOPPING PARSER! bye.\n\n";
              clean_exit (-1);
            }
            skWidth = atoi (attr_value);
            is_skWidth = true;
            continue;
          }
          if (strncmp (attr_name, "sk_height", stringLength) == 0) {
            if (is_skHeight) {
              std::cout << "\n\nYOU MUSTN'T SPECIFY THE sk_height= TAG IN <objectpool> MORE THAN ONCE! STOPPING PARSER! bye.\n\n";
              clean_exit (-1);
            }
            skHeight = atoi (attr_value);
            is_skHeight = true;
            continue;
          }
          if (strncmp (attr_name, "std_bitmap_path", stringLength) == 0) {
            strcpy (std_bitmap_path, attr_value);
            continue;
          }
          if (strncmp (attr_name, "fix_bitmap_path", stringLength) == 0) {
            strcpy (fix_bitmap_path, attr_value);
            continue;
          }
        }
      }
    }
  } else {
    // normal to insert element!
    if (( (uint64_t(1)<<objType) & ombType) == 0) {
      // ERROR: Unallowed <TAG> here?!
      std::cout << "\n\nENCOUNTERED WRONG TAG AT THIS POSITION!\nENCOUNTERED: <" << node_name << ">\nPOSSIBLE TAGS HERE WOULD BE: ";
      for (int j=0; j<maxObjectTypesToCompare; j++) {
        if ((uint64_t(1)<<j) & ombType) {
          std::cout << " <" << otCompTable [j] << ">  ";
        }
      }
      std::cout << "\n\n";
      clean_exit (-1);
    }
    getAttributesFromNode(n, true); // true: read name= and id=

    // set all non-set attributes to default values (as long as sensible, like bg_colour etc.)
    defaultAttributes ();

    // get a new ID for this object is not yet done
    objID = getID (objName, (objType == otMacro) ? true: false, is_objID, objID);


    // Completely parsed <tag ...> now, start writing out to the files!
    // ______________________________________________________________________________________________________________________________
    // we got objName, objID, objType here now! - print out standard definitions in A, B and C right after the array definition lines
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    bool objHasArrayEventMacro = false;
    switch (objType)
    {
      case otWorkingset:   case otDatamask:  case otAlarmmask:   case otContainer: case otSoftkeymask: case otKey: case otButton:
      case otInputboolean: case otInputstring:  case otInputnumber: case otInputlist:
      case otOutputstring: case otOutputnumber:
      case otLine:   case otRectangle:    case otEllipse:  case otPolygon:
      case otMeter:     case otLinearbargraph:  case otArchedbargraph:
      case otPicturegraphic:
      // variables have no events/macros
      case otFontattributes:  case otLineattributes:  case otFillattributes:  case otInputattributes:
      // object pointer has no events/macros
        objHasArrayEventMacro = true;
        break;
    }

    bool objHasArrayObject = false;
    switch (objType)
    {
      case otSoftkeymask:
      case otInputlist:
        objHasArrayObject = true;
        break;
    }

    bool objHasArrayObjectXY = false;
    switch (objType)
    {
      case otWorkingset:
      case otDatamask:
      case otAlarmmask:
      case otContainer:
      case otKey:
      case otButton:
      case otAuxiliaryfunction:
      case otAuxiliaryinput:
        objHasArrayObjectXY = true;
        break;
    }

    bool objHasArrayLanguagecode = false;
    switch (objType)
    {
      case otWorkingset:
        objHasArrayLanguagecode = true;
        break;
    }


    bool objHasArrayMacroCommand = false;
    switch (objType)
    {
      case otMacro:
        objHasArrayMacroCommand = true;
        break;
    }

    bool objHasArrayPoints = false;
    switch (objType)
    {
      case otPolygon:
        objHasArrayPoints = true;
        break;
    }

    // ##########################################
    // ### Print out Repeat Array definitions ###
    // ##########################################


    // ### Print out LANGUAGECODE array
    if (objHasArrayLanguagecode)
    {
      // Process all Child-Elements
      bool firstElement = true;
      objChildLanguages = 0;
      for (child = n->getFirstChild(); child != 0; child=child->getNextSibling())
      {
        if ( (child->getNodeType() == DOMNode::ELEMENT_NODE) && (0 == strcmp (XMLString::transcode(child->getNodeName()), otCompTable [otLanguage])) )
        {
          // get 'code=' out of child
          if(child->hasAttributes())
          {
            // parse through all attributes
            pAttributes = child->getAttributes();
            int nSize = pAttributes->getLength();

            attrString [attrCode] [stringLength+1-1] = 0x00; attrIsGiven [attrCode] = false;
            objChildName [stringLength+1-1] = 0x00; is_objChildName = false;

            for(int i=0;i<nSize;++i)
            {
              DOMAttr *pAttributeNode = (DOMAttr*) pAttributes->item(i);
              utf16convert ((char *)pAttributeNode->getName(), attr_name, 1024);
              utf16convert ((char *)pAttributeNode->getValue(), attr_value, 1024);

              if (strncmp (attr_name, "code", stringLength) == 0)
              {
                  strncpy (attrString [attrCode], attr_value, stringLength);
                  attrIsGiven [attrCode] = true;
              } else {
                  std::cout << "\n\nATTRIBUTE OTHER THAN 'code=' GIVEN IN <language ...> ! STOPPING PARSER! bye.\n\n";
                  clean_exit (-1);
              }
            }
          }

          if (firstElement) {
            fprintf (partFileB, "const IsoAgLib::repeat_vtLanguage_s ivtObject%s_aLanguage [] = {", objName);
          } else {
            fprintf (partFileB, ", ");
          }
          if (!(attrIsGiven [attrCode]))
          {
            std::cout << "\n\ncode ATTRIBUTE NEEDED IN <language ...> ! STOPPING PARSER! bye.\n\n";
            clean_exit (-1);
          }
          char languageCode[2];
          languageCode[0] = attrString[attrCode][0];
          languageCode[1] = attrString[attrCode][1];

          // Check for correct LanguageCode!
          int lc;
          for (lc=0; lc<iso639entries; lc++) {
            if ((iso639table[lc][0] == languageCode[0]) && (iso639table[lc][1] == languageCode[1])) break;
          }
          if (lc == iso639entries) {
            // language not found!
            std::cout << "\n\n<language code=\"" << languageCode[0] << languageCode[1] << "\" /> is NOT conform to ISO 639 (Maybe you didn't use lower-case letters?!)! STOPPING PARSER! bye.\n\n";
            clean_exit (-1);
          }
          sprintf (tempString, "'%c', '%c'", languageCode[0], languageCode[1]);
          fprintf (partFileB, "{%s}", tempString);
          objChildLanguages++;
          firstElement = false;
        }
      }
      if (firstElement == false)
        fprintf (partFileB, "};\n");
    }

    // ### Print out RAWBITMAP byte array
    if (objType == otPicturegraphic) {
      if (!(attrIsGiven [attrWidth] && attrIsGiven [attrFormat] && attrIsGiven [attrTransparency_colour]))
      {
        clean_exit (-1, "YOU NEED TO SPECIFY THE width= AND format= AND transparency_colour= ATTRIBUTES FOR THE <picturegraphic> OBJECT! STOPPING PARSER! bye.\n\n");
      }


      ///////////////////////////////////////////////////////
      /// ### +BEGIN+ -- FIRST -- process "standard" bitmap

      c_Bitmap.resetLengths();
      checkForFileOrFile148 ("picturegraphic");

      objBitmapOptions = picturegraphicoptionstoi (attrString [attrOptions]);
      openDecodePrintOut (rc_workDir, std_bitmap_path, objBitmapOptions);

      // copy values from c_Bitmap somewhere to have them when Print'ing out the array afterwards...
      deXwidth = atoi (attrString [attrWidth]);
      deXcolorDepth = colordepthtoi (attrString [attrFormat]);
      deXtransCol = colortoi (attrString [attrTransparency_colour]);
      deXactualWidth = c_Bitmap.getWidth();
      deXactualHeight = c_Bitmap.getHeight();
      for (int i=0; i<3; i++) stdRawBitmapBytes [i] = c_Bitmap.objRawBitmapBytes [i];

      /// ### +END+ -- FIRST -- process "standard" bitmap
      ///////////////////////////////////////////////////////


      //////////////////////////////////////////////////////
      /// ### +BEGIN+ -- SECOND -- process "fixed" bitmaps
      fixNr = 0;
      for (child = n->getFirstChild(); child != 0; child=child->getNextSibling()) {
        if ( (child->getNodeType() == DOMNode::ELEMENT_NODE) && (0 == strcmp (XMLString::transcode(child->getNodeName()), otCompTable [otFixedBitmap]) )) {
          getAttributesFromNode(child, false); // false: DON'T read name= and id=
          // no defaultAttributes() needed here...

          c_Bitmap.resetLengths();
          checkForFileOrFile148 ("fixedbitmap");

          fixBitmapOptions [fixNr] = objBitmapOptions & 0x3; // keep flashing/transparency information from <pictureobject>
          openDecodePrintOut (rc_workDir, fix_bitmap_path, fixBitmapOptions[fixNr], fixNr);

          // copy values from c_Bitmap somewhere in a temp array that will be printed afterwards............
          //fiXtransCol [fixNr] = colortoi (attrString [attrTransparency_colour]);
          fiXactualWidth [fixNr] = c_Bitmap.getWidth();
          fiXactualHeight [fixNr] = c_Bitmap.getHeight();
          for (int i=0; i<3; i++) fixRawBitmapBytes [fixNr] [i] = c_Bitmap.objRawBitmapBytes [i];

          fixNr++; //is count then!!
        }
      }
      // have there been any Fixed Bitmaps?
      if (fixNr > 0) {
        // 1st: Print out array here now...
        fprintf (partFileB, "const IsoAgLib::repeat_rawData_rawBytes_actWidth_actHeight_formatoptions_s iVtObject%s_aFixedBitmaps [] = {", objName);
        bool firstEntry=true;
        for (unsigned int i=0; i<fixNr; i++) {
          for (int actDepth=0; actDepth<2; actDepth++) {
            // was this depth generated for this special bitmap?
            if (fixRawBitmapBytes[i] [actDepth] > 0) {
              if (!firstEntry) fprintf (partFileB, ", ");
              unsigned int options = (fixBitmapOptions[i] & 0x3) | ( (fixBitmapOptions[i] & (uint64_t(1)<<(2+actDepth))) ? (uint64_t(1)<<2) : 0 );
              fprintf (partFileB, "{iVtObject%s_aRawBitmap%dFixed%d, %d, %d, %d, (%d << 6) | %d}", objName, actDepth, i, fixRawBitmapBytes[i] [actDepth], fiXactualWidth[i], fiXactualHeight[i], actDepth, options);
              firstEntry = false;
            }
          }
        }
        fprintf (partFileB, "};\n");

      }
      /// ### +END+ -- SECOND -- process "fixed" bitmaps
      //////////////////////////////////////////////////////

    }


    // ### Print out OBJECTID(_X_Y) array
    bool xyNeeded = false;
    if (objHasArrayObjectXY) xyNeeded = true;
    if (objHasArrayObjectXY || objHasArrayObject)
    {
      // Process all Child-Elements
      bool firstElement = true;
      objChildObjects = 0;
      for (child = n->getFirstChild(); child != 0; child=child->getNextSibling())
      {   // if NOT Macro insert as normal object!
        if (  (child->getNodeType() == DOMNode::ELEMENT_NODE) &&
            !((0 == strcmp (XMLString::transcode(child->getNodeName()), otCompTable [otMacro])) ||
              (0 == strcmp (XMLString::transcode(child->getNodeName()), otCompTable [otIncludemacro])) ||
              (0 == strcmp (XMLString::transcode(child->getNodeName()), otCompTable [otLanguage]))
             )
           )
        {
          // get NAME and POS_X and POS_Y attributes out of child
          if(child->hasAttributes()) {
            // parse through all attributes
            pAttributes = child->getAttributes();
            int nSize = pAttributes->getLength();

            objChildName [stringLength+1-1] = 0x00; is_objChildName = false;
            is_objChildID = false;
            is_objChildX = false;
            is_objChildY = false;

            strcpy (objBlockFont, "NULL");
            objBlockRow = 0;
            objBlockCol = 0;

            for(int i=0;i<nSize;++i) {
              DOMAttr *pAttributeNode = (DOMAttr*) pAttributes->item(i);
              utf16convert ((char *)pAttributeNode->getName(), attr_name, 1024);
              utf16convert ((char *)pAttributeNode->getValue(), attr_value, 1024);

              // Get NAME and POS_X and POS_Y directly
              if (strncmp (attr_name, "name", stringLength) == 0) {
                strncpy (objChildName, attr_value, stringLength);
                is_objChildName = true;
              }
              if (strncmp (attr_name, "id", stringLength) == 0) {
                objChildID = atoi (attr_value);
                is_objChildID = true;
              }
              if (strncmp (attr_name, "pos_x", stringLength) == 0) {
                objChildX = atoi (attr_value);
                is_objChildX = true;
              }
              if (strncmp (attr_name, "pos_y", stringLength) == 0) {
                objChildY = atoi (attr_value);
                is_objChildY = true;
              }
              if (strncmp (attr_name, "block_font", stringLength) == 0) {
                strcpy (objBlockFont, "&iVtObject");
                strncat (objBlockFont, attr_value, stringLength-9);
              }
              if (strncmp (attr_name, "block_row", stringLength) == 0) {
                objBlockRow = atoi (attr_value);
              }
              if (strncmp (attr_name, "block_col", stringLength) == 0) {
                objBlockCol = atoi (attr_value);
              }
            }
          }
          if (is_objChildName == false)
          {
            // create auto-named NAME attribute
            sprintf (objChildName, "Unnamed%d", objNextUnnamedName);
            ((DOMElement *)child)->setAttribute (X("name"), X(objChildName));
            objNextUnnamedName++;
            is_objChildName = true;
          }
          // give him an ID, although not necessary now...
          objChildID = getID (objChildName, false /* assumption: not a macro here */, is_objChildID, objChildID);
          if (firstElement) {
            if (xyNeeded) fprintf (partFileB, "const IsoAgLib::repeat_iVtObject_x_y_iVtObjectFontAttributes_row_col_s iVtObject%s_aObject_x_y_font_row_col [] = {", objName);
            else          fprintf (partFileB, "const IsoAgLib::repeat_iVtObject_s iVtObject%s_aObject [] = {", objName);
          } else {
            fprintf (partFileB, ", ");
          }
          if (xyNeeded) {
            if (!(is_objChildX && is_objChildY)) {
              std::cout << "\n\npos_x AND pos_y ATTRIBUTES NEEDED IN CHILD-OBJECT OF <"<< node_name <<"> ! STOPPING PARSER! bye.\n\n";
              clean_exit (-1);
            }
            fprintf (partFileB, "{&iVtObject%s, %d, %d, %s ,%d, %d}", objChildName, objChildX, objChildY, objBlockFont, objBlockRow, objBlockCol);
          } else {
            // Added this if statement to account for InputList objects who might have NULL Object IDs in their list of objects. (Which is legal per the standard!)
            // Instead of inserting a faulty object name, just insert NULL into the array. -BAC 07-Jan-2005
            if (objChildID == 65535)
            {
              fprintf (partFileB, "{NULL}");
            }
            else
            {
              fprintf (partFileB, "{&iVtObject%s}", objChildName);
            }
          }
          objChildObjects++;
          firstElement = false;
        }
      }
       // all child-elements processed, now:
      // special treatment for inputlist with NULL objects
      if (objType == otInputlist && objChildObjects < (uint16_t)atoi(attrString [attrNumber_of_items]))
      {
        //only some items are NULL objects which were not counted in objChildObjects
        if (objChildObjects>0)
        {
          for (uint16_t ui_leftChildObjects = objChildObjects; ui_leftChildObjects<(uint16_t)atoi(attrString [attrNumber_of_items]); ui_leftChildObjects++)
          {
            if (ui_leftChildObjects < atoi(attrString [attrNumber_of_items])) fprintf (partFileB, ", ");
            fprintf (partFileB, "{NULL}");
          }
          objChildObjects=(uint16_t)atoi(attrString [attrNumber_of_items]);
        }
        else {
          // no child-element at all in the inputlist (all items as NULL objects)
          // fill the reference-list with {NULL}-elements --> so they could be replaced during runtime with NOT NULL objects
          if (objChildObjects == 0 && atoi(attrString [attrNumber_of_items]) > 0)
          {
            // objChildObjects has to be set to number_of_items otherwise
            // it is set to 0 in the attributes of the inputlist
            objChildObjects = (uint16_t)atoi(attrString [attrNumber_of_items]);
            // create for all number_of_items a no-item placeholder
            fprintf (partFileB, "const IsoAgLib::repeat_iVtObject_s iVtObject%s_aObject [] = {", objName);
            for (int i_emptyChildObj=1; i_emptyChildObj <= atoi(attrString [attrNumber_of_items]); i_emptyChildObj++)
            {
              fprintf (partFileB, "{NULL}");
              if (i_emptyChildObj < atoi(attrString [attrNumber_of_items])) fprintf (partFileB, ", ");
            }
            fprintf (partFileB, "};\n");
          }
        }
      }
      if (firstElement == false)
        fprintf (partFileB, "};\n");
    }


    // ### Print out EVENT_MACRO array
    if (objHasArrayEventMacro)
    {
      // Process all Child-Elements
      bool firstElement = true;
      objChildMacros = 0;
      for (child = n->getFirstChild(); child != 0; child=child->getNextSibling())
      {
        if ( (child->getNodeType() == DOMNode::ELEMENT_NODE)
          && ( (0 == strcmp (XMLString::transcode(child->getNodeName()), otCompTable [otIncludemacro]))
        || (0 == strcmp (XMLString::transcode(child->getNodeName()), otCompTable [otMacro]))))
        {
          // get 'event=' and 'name=' out of child
          if(child->hasAttributes()) {
            // parse through all attributes
            pAttributes = child->getAttributes();
            int nSize = pAttributes->getLength();

            attrString [attrEvent] [stringLength+1-1] = 0x00; attrIsGiven [attrEvent] = false;
            objChildName [stringLength+1-1] = 0x00; is_objChildName = false;

            for(int i=0;i<nSize;++i) {
              DOMAttr *pAttributeNode = (DOMAttr*) pAttributes->item(i);
              utf16convert ((char *)pAttributeNode->getName(), attr_name, 1024);
              utf16convert ((char *)pAttributeNode->getValue(), attr_value, 1024);

              if (strncmp (attr_name, "event", stringLength) == 0) {
                strncpy (attrString [attrEvent], attr_value, stringLength);
                attrIsGiven [attrEvent] = true;
              }
              if (strncmp (attr_name, "name", stringLength) == 0) {
                strncpy (objChildName, attr_value, stringLength);
                is_objChildName = true;
              }
              if (strncmp (attr_name, "id", stringLength) == 0) {
                objChildID = atoi (attr_value);
                is_objChildID = true;
              }
            }
          }
          if (is_objChildName == false)
          {
            // create auto-named NAME attribute
            sprintf (objChildName, "Unnamed%d", objNextUnnamedName);
            ((DOMElement *)child)->setAttribute (X("name"), X(objChildName));
            objNextUnnamedName++;
            is_objChildName = true;
          }
          // give him an ID, although not necessary now...
          objChildID = getID (objChildName, true, is_objChildID, objChildID);
          if (firstElement) {
            // Changed the macro struct name in the following line to match what is in version 1.1.0 of IsoAgLib -bac 06-Jan-2005
            // fprintf (partFileB, "const IsoAgLib::repeat_Macro_iVtObject_s iVtObject%s_aMacro_Object [] = {", objName);
            fprintf (partFileB, "const IsoAgLib::repeat_event_iVtObjectMacro_s iVtObject%s_aMacro_Object [] = {", objName);
          } else {
            fprintf (partFileB, ", ");
          }
          if (!(attrIsGiven [attrEvent])) {
            std::cout << "\n\nevent ATTRIBUTE NEEDED IN <macro ...> ! STOPPING PARSER! bye.\n\n";
            clean_exit (-1);
          }
          //fprintf (partFileB, "{%d, &vtObject%s}", atoi (attrString [attrEvent]), objChildName);
          fprintf (partFileB, "{%d, &iVtObject%s}", eventToi(attrString [attrEvent]), objChildName);
          objChildMacros++;
          firstElement = false;
        }
      }
      if (firstElement == false)
        fprintf (partFileB, "};\n");
    }

//****************************************************************************************************************************************
// This section added by Brad Cox Sep 30, 2004 to parse out commands from macros and to parse out point arrays from polygon objects.
// New XML tags have been introduced included 20 for each macro-usable command and one called <point> for the points of a polygon object
  // ### Print out MACRO_COMMAND array
  if (objHasArrayMacroCommand)
  {
    // Process all Child-Elements
    bool firstElement = true;
    objChildCommands = 0;
    for (child = n->getFirstChild(); child != 0; child=child->getNextSibling())
    {
      if (child->getNodeType() == DOMNode::ELEMENT_NODE)  // May need to adjust this to be sure the child is really a command element and not something else to enforce integrity
      {
        char *command_name = XMLString::transcode(child->getNodeName());
        commandType = commandIsType (command_name);
        commandMessage [stringLength+1-1] = 0x00;
        switch (commandType)
        {
          case ctHideShowObject:
            if(child->hasAttributes())
            {
              // parse through all attributes
              pAttributes = child->getAttributes();
              int nSize = pAttributes->getLength();

              cleanAttribute(attrObjectID);
              cleanAttribute(attrHideShow);

              for(int i=0;i<nSize;++i)
              {
                DOMAttr *pAttributeNode = (DOMAttr*) pAttributes->item(i);
                utf16convert ((char *)pAttributeNode->getName(), attr_name, 1024);
                utf16convert ((char *)pAttributeNode->getValue(), attr_value, 1024);

                setAttributeValue(attrObjectID);
                setAttributeValue(attrHideShow);
              }
              // Need check for all attributes being present for this command -bac
              sprintf(commandMessage, "0xA0, %d, %d, %d, 0xFF, 0xFF, 0xFF, 0xFF", MACRO_16bitToLE(idOrName_toi(attrString [attrObjectID], /*macro?*/false)), booltoi(attrString[attrHideShow]));
              objChildCommands++;
            }
            break;
          case ctEnableDisableObject:
            if(child->hasAttributes())
            {
              // parse through all attributes
              pAttributes = child->getAttributes();
              int nSize = pAttributes->getLength();

              cleanAttribute(attrObjectID);
              cleanAttribute(attrDisable_enable);

              for(int i=0;i<nSize;++i)
              {
                DOMAttr *pAttributeNode = (DOMAttr*) pAttributes->item(i);
                utf16convert ((char *)pAttributeNode->getName(), attr_name, 1024);
                utf16convert ((char *)pAttributeNode->getValue(), attr_value, 1024);

                setAttributeValue(attrObjectID);
                setAttributeValue(attrDisable_enable);
              }
              // Need check for all attributes being present for this command -bac
              sprintf(commandMessage, "0xA1, %d, %d, %d, 0xFF, 0xFF, 0xFF, 0xFF", MACRO_16bitToLE(idOrName_toi(attrString [attrObjectID], /*macro?*/false)), booltoi(attrString[attrDisable_enable]));
              objChildCommands++;
            }
            break;
          case ctSelectInputObject:
            if(child->hasAttributes())
            {
              // parse through all attributes
              pAttributes = child->getAttributes();
              int nSize = pAttributes->getLength();

              cleanAttribute(attrObjectID);

              for(int i=0;i<nSize;++i)
              {
                DOMAttr *pAttributeNode = (DOMAttr*) pAttributes->item(i);
                utf16convert ((char *)pAttributeNode->getName(), attr_name, 1024);
                utf16convert ((char *)pAttributeNode->getValue(), attr_value, 1024);

                setAttributeValue(attrObjectID);
              }
              // Need check for all attributes being present for this command -bac
              sprintf(commandMessage, "0xA2, %d, %d, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF", MACRO_16bitToLE(idOrName_toi(attrString [attrObjectID], /*macro?*/false)));
              objChildCommands++;
            }
            break;
          case ctControlAudioDevice:
            if(child->hasAttributes())
            {
              // parse through all attributes
              pAttributes = child->getAttributes();
              int nSize = pAttributes->getLength();

              cleanAttribute(attrObjectID);
              cleanAttribute(attrNumber_of_repetitions);
              cleanAttribute(attrFrequency);
              cleanAttribute(attrOnTime_duration);
              cleanAttribute(attrOffTime_duration);

              for(int i=0;i<nSize;++i)
              {
                DOMAttr *pAttributeNode = (DOMAttr*) pAttributes->item(i);
                utf16convert ((char *)pAttributeNode->getName(), attr_name, 1024);
                utf16convert ((char *)pAttributeNode->getValue(), attr_value, 1024);

                setAttributeValue(attrNumber_of_repetitions);
                setAttributeValue(attrFrequency);
                setAttributeValue(attrOnTime_duration);
                setAttributeValue(attrOffTime_duration);
              }
              // Need check for all attributes being present for this command -bac
              sprintf(commandMessage, "0xA3, %d, %d,%d, %d,%d, %d,%d", atoi(attrString [attrNumber_of_repetitions]), MACRO_16bitToLE(atoi(attrString [attrFrequency])), MACRO_16bitToLE(atoi(attrString [attrOnTime_duration])), MACRO_16bitToLE(atoi(attrString [attrOffTime_duration])));
              objChildCommands++;
            }
            break;
          case ctSetAudioVolume:
            if(child->hasAttributes())
            {
              // parse through all attributes
              pAttributes = child->getAttributes();
              int nSize = pAttributes->getLength();

              cleanAttribute(attrPercentage);

              for(int i=0;i<nSize;++i)
              {
                DOMAttr *pAttributeNode = (DOMAttr*) pAttributes->item(i);
                utf16convert ((char *)pAttributeNode->getName(), attr_name, 1024);
                utf16convert ((char *)pAttributeNode->getValue(), attr_value, 1024);

                setAttributeValue(attrPercentage);
              }
              // Need check for all attributes being present for this command -bac
              sprintf(commandMessage, "0xA4, %d, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF", atoi(attrString [attrPercentage]));
              objChildCommands++;
            }
            break;
          case ctChangeChildLocation:
            if(child->hasAttributes())
            {
              // parse through all attributes
              pAttributes = child->getAttributes();
              int nSize = pAttributes->getLength();

              cleanAttribute(attrParent_objectID);
              cleanAttribute(attrObjectID);
              cleanAttribute(attrX_change);
              cleanAttribute(attrY_change);

              for(int i=0;i<nSize;++i)
              {
                DOMAttr *pAttributeNode = (DOMAttr*) pAttributes->item(i);
                utf16convert ((char *)pAttributeNode->getName(), attr_name, 1024);
                utf16convert ((char *)pAttributeNode->getValue(), attr_value, 1024);

                setAttributeValue(attrParent_objectID);
                setAttributeValue(attrObjectID);
                setAttributeValue(attrX_change);
                setAttributeValue(attrY_change);
              }
              // Need check for all attributes being present for this command -bac
              // add 127 to relative x,y
              sprintf(commandMessage, "0xA5, %d, %d, %d, %d, %d, %d, 0xFF", MACRO_16bitToLE(idOrName_toi(attrString [attrParent_objectID], /*macro?*/false)), MACRO_16bitToLE(idOrName_toi(attrString [attrObjectID], /*macro?*/false)), atoi(attrString [attrX_change]) + 127 ,atoi(attrString [attrY_change]) + 127 );
              objChildCommands++;
            }
            break;
          case ctChangeChildPosition:
            if(child->hasAttributes())
            {
              // parse through all attributes
              pAttributes = child->getAttributes();
              int nSize = pAttributes->getLength();

              cleanAttribute(attrParent_objectID);
              cleanAttribute(attrObjectID);
              cleanAttribute(attrX_pos);
              cleanAttribute(attrY_pos);

              for(int i=0;i<nSize;++i)
              {
                DOMAttr *pAttributeNode = (DOMAttr*) pAttributes->item(i);
                utf16convert ((char *)pAttributeNode->getName(), attr_name, 1024);
                utf16convert ((char *)pAttributeNode->getValue(), attr_value, 1024);

                setAttributeValue(attrParent_objectID);
                setAttributeValue(attrObjectID);
                setAttributeValue(attrX_pos);
                setAttributeValue(attrY_pos);
              }
              // Need check for all attributes being present for this command -bac
              sprintf(commandMessage, "0xB4, %d, %d, %d, %d, %d, %d, %d, %d", MACRO_16bitToLE(idOrName_toi(attrString [attrParent_objectID], /*macro?*/false)), MACRO_16bitToLE(idOrName_toi(attrString [attrObjectID], /*macro?*/false)), MACRO_16bitToLE(atoi(attrString [attrX_pos])), MACRO_16bitToLE(atoi(attrString [attrY_pos])));
              objChildCommands++;
            }
            break;
          case ctChangeSize:
            if(child->hasAttributes())
            {
              // parse through all attributes
              pAttributes = child->getAttributes();
              int nSize = pAttributes->getLength();

              cleanAttribute(attrObjectID);
              cleanAttribute(attrNew_width);
              cleanAttribute(attrNew_height);

              for(int i=0;i<nSize;++i)
              {
                DOMAttr *pAttributeNode = (DOMAttr*) pAttributes->item(i);
                utf16convert ((char *)pAttributeNode->getName(), attr_name, 1024);
                utf16convert ((char *)pAttributeNode->getValue(), attr_value, 1024);

                setAttributeValue(attrObjectID);
                setAttributeValue(attrNew_width);
                setAttributeValue(attrNew_height);
              }
              // Need check for all attributes being present for this command -bac
              sprintf(commandMessage, "0xA6, %d, %d, %d, %d, %d, %d, 0xFF", MACRO_16bitToLE(idOrName_toi(attrString [attrObjectID], /*macro?*/false)), MACRO_16bitToLE(atoi(attrString [attrNew_width])), MACRO_16bitToLE(atoi(attrString [attrNew_height])));
              objChildCommands++;
            }
            break;
          case ctChangeBackgroundColour:
            if(child->hasAttributes())
            {
              // parse through all attributes
              pAttributes = child->getAttributes();
              int nSize = pAttributes->getLength();

              cleanAttribute(attrObjectID);
              cleanAttribute(attrNew_background_colour);

              for(int i=0;i<nSize;++i)
              {
                DOMAttr *pAttributeNode = (DOMAttr*) pAttributes->item(i);
                utf16convert ((char *)pAttributeNode->getName(), attr_name, 1024);
                utf16convert ((char *)pAttributeNode->getValue(), attr_value, 1024);

                setAttributeValue(attrObjectID);
                setAttributeValue(attrNew_background_colour);
              }
              // Need check for all attributes being present for this command -bac
              sprintf(commandMessage, "0xA7, %d, %d, %d, 0xFF, 0xFF, 0xFF, 0xFF", MACRO_16bitToLE(idOrName_toi(attrString [attrObjectID], /*macro?*/false)), colortoi(attrString [attrNew_background_colour]));
              objChildCommands++;
            }
            break;
          case ctChangeNumericValue:
            if(child->hasAttributes())
            {
              // parse through all attributes
              pAttributes = child->getAttributes();
              int nSize = pAttributes->getLength();

              cleanAttribute(attrObjectID);
              cleanAttribute(attrNew_value);

              for(int i=0;i<nSize;++i)
              {
                DOMAttr *pAttributeNode = (DOMAttr*) pAttributes->item(i);
                utf16convert ((char *)pAttributeNode->getName(), attr_name, 1024);
                utf16convert ((char *)pAttributeNode->getValue(), attr_value, 1024);

                setAttributeValue(attrObjectID);
                setAttributeValue(attrNew_value);
              }
              // Need check for all attributes being present for this command -bac
              sprintf(commandMessage, "0xA8, %d, %d, 0x00, %d, %d, %d, %d", MACRO_16bitToLE(idOrName_toi(attrString [attrObjectID], /*macro?*/false)), MACRO_32bitToLE(idOrName_toi(attrString [attrNew_value], /*macro?*/false)));
              objChildCommands++;
            }
            break;
          case ctChangeStringValue:
            if(child->hasAttributes())
            {
              // parse through all attributes
              pAttributes = child->getAttributes();
              int nSize = pAttributes->getLength();

              cleanAttribute(attrObjectID);
              cleanAttribute(attrNumber_of_bytes);
              cleanAttribute(attrNew_value);

              for(int i=0;i<nSize;++i)
              {
                DOMAttr *pAttributeNode = (DOMAttr*) pAttributes->item(i);
                utf16convert ((char *)pAttributeNode->getName(), attr_name, 1024);
                utf16convert ((char *)pAttributeNode->getValue(), attr_value, 1024);

                setAttributeValue(attrObjectID);
                setAttributeValue(attrBytes_in_string);
                setAttributeValue(attrNew_value);
              }
              // Need check for all attributes being present for this command -bac
              int strLength;
              char *tempStrPtr;

              // reset
              tempString2[0]=0;

              tempStrPtr = attrString[attrNew_value];
              strLength = strlen(tempStrPtr);
              for(int i = 0; i < strLength; i++)
              {
                sprintf(tempString, ", %d", tempStrPtr[i]);
                strcat(tempString2, tempString);
              }
              //sprintf (attrString [attrValue], "%s", tempString2);
              sprintf(commandMessage, "0xB3, %d, %d, %d, %d%s", MACRO_16bitToLE(idOrName_toi(attrString [attrObjectID], /*macro?*/false)), MACRO_16bitToLE(idOrName_toi(attrString [attrBytes_in_string], /*macro?*/false)), tempString2);

              objChildCommands++;
            }
            break;
          case ctChangeEndPoint:
            if(child->hasAttributes())
            {
              // parse through all attributes
              pAttributes = child->getAttributes();
              int nSize = pAttributes->getLength();

              cleanAttribute(attrObjectID);
              cleanAttribute(attrNew_width);
              cleanAttribute(attrNew_height);
              cleanAttribute(attrLine_direction);

              for(int i=0;i<nSize;++i)
              {
                DOMAttr *pAttributeNode = (DOMAttr*) pAttributes->item(i);
                utf16convert ((char *)pAttributeNode->getName(), attr_name, 1024);
                utf16convert ((char *)pAttributeNode->getValue(), attr_value, 1024);

                setAttributeValue(attrObjectID);
                setAttributeValue(attrNew_width);
                setAttributeValue(attrNew_height);
                setAttributeValue(attrLine_direction);
              }
              // Need check for all attributes being present for this command -bac
              sprintf(commandMessage, "0xA9, %d,%d, %d,%d, %d,%d, %d", MACRO_16bitToLE(idOrName_toi(attrString [attrObjectID], /*macro?*/false)), MACRO_16bitToLE(atoi(attrString [attrNew_width])), MACRO_16bitToLE(atoi(attrString [attrNew_height])), atoi(attrString [attrLine_direction]));

              objChildCommands++;
            }
            break;
          case ctChangeFontAttributes:
            if(child->hasAttributes())
            {
              // parse through all attributes
              pAttributes = child->getAttributes();
              int nSize = pAttributes->getLength();

              cleanAttribute(attrObjectID);
              cleanAttribute(attrFont_colour);
              cleanAttribute(attrFont_size);
              cleanAttribute(attrFont_type);
              cleanAttribute(attrFont_style);

              for(int i=0;i<nSize;++i)
              {
                DOMAttr *pAttributeNode = (DOMAttr*) pAttributes->item(i);
                utf16convert ((char *)pAttributeNode->getName(), attr_name, 1024);
                utf16convert ((char *)pAttributeNode->getValue(), attr_value, 1024);

                setAttributeValue(attrObjectID);
                setAttributeValue(attrFont_colour);
                setAttributeValue(attrFont_size);
                setAttributeValue(attrFont_type);
                setAttributeValue(attrFont_style);
              }
              // Need check for all attributes being present for this command -bac
              sprintf(commandMessage, "0xAA, %d, %d, %d, %d, %d, %d, 0xFF", MACRO_16bitToLE(idOrName_toi(attrString [attrObjectID], /*macro?*/false)), colortoi(attrString [attrFont_colour]), fontsizetoi(attrString [attrFont_size]), atoi(attrString [attrFont_type]), fontstyletoi(attrString [attrFont_style]));

              objChildCommands++;
            }
            break;
          case ctChangeLineAttributes:
            if(child->hasAttributes())
            {
              // parse through all attributes
              pAttributes = child->getAttributes();
              int nSize = pAttributes->getLength();

              cleanAttribute(attrObjectID);
              cleanAttribute(attrLine_colour);
              cleanAttribute(attrLine_width);
              cleanAttribute(attrLine_art);

              for(int i=0;i<nSize;++i)
              {
                DOMAttr *pAttributeNode = (DOMAttr*) pAttributes->item(i);
                utf16convert ((char *)pAttributeNode->getName(), attr_name, 1024);
                utf16convert ((char *)pAttributeNode->getValue(), attr_value, 1024);

                setAttributeValue(attrObjectID);
                setAttributeValue(attrLine_colour);
                setAttributeValue(attrLine_width);
                setAttributeValue(attrLine_art);
              }
              // Need check for all attributes being present for this command -bac
              sprintf(commandMessage, "0xAB, %d, %d, %d, %d, %d, %d, 0xFF", MACRO_16bitToLE(idOrName_toi(attrString [attrObjectID], /*macro?*/false)), colortoi(attrString [attrLine_colour]), atoi(attrString [attrLine_width]), MACRO_16bitToLE(linearttoi(attrString [attrLine_art])));

              objChildCommands++;
            }
            break;
          case ctChangeFillAttributes:
            if(child->hasAttributes())
            {
              // parse through all attributes
              pAttributes = child->getAttributes();
              int nSize = pAttributes->getLength();

              cleanAttribute(attrObjectID);
              cleanAttribute(attrFill_colour);
              cleanAttribute(attrFill_type);
              cleanAttribute(attrFill_colour);
              cleanAttribute(attrFill_pattern);

              for(int i=0;i<nSize;++i)
              {
                DOMAttr *pAttributeNode = (DOMAttr*) pAttributes->item(i);
                utf16convert ((char *)pAttributeNode->getName(), attr_name, 1024);
                utf16convert ((char *)pAttributeNode->getValue(), attr_value, 1024);

                setAttributeValue(attrObjectID);
                setAttributeValue(attrFill_type);
                setAttributeValue(attrFill_colour);
                setAttributeValue(attrFill_pattern);
              }
              // Need check for all attributes being present for this command -bac
              sprintf(commandMessage, "0xAC, %d, %d, %d, %d, %d, %d, 0xFF", MACRO_16bitToLE(idOrName_toi(attrString [attrObjectID], /*macro?*/false)), filltypetoi(attrString [attrFill_type]), colortoi(attrString [attrFill_colour]), MACRO_16bitToLE(atoi(attrString [attrFill_pattern])));

              objChildCommands++;
            }
            break;
          case ctChangeActiveMask:
            if(child->hasAttributes())
            {
              // parse through all attributes
              pAttributes = child->getAttributes();
              int nSize = pAttributes->getLength();

              cleanAttribute(attrWorking_setID);
              cleanAttribute(attrNew_active_mask);

              for(int i=0;i<nSize;++i)
              {
                DOMAttr *pAttributeNode = (DOMAttr*) pAttributes->item(i);
                utf16convert ((char *)pAttributeNode->getName(), attr_name, 1024);
                utf16convert ((char *)pAttributeNode->getValue(), attr_value, 1024);

                setAttributeValue(attrWorking_setID);
                setAttributeValue(attrNew_active_mask);
              }
              // Need check for all attributes being present for this command -bac
              sprintf(commandMessage, "0xAD, %d, %d, %d, %d, 0xFF, 0xFF, 0xFF", MACRO_16bitToLE(idOrName_toi(attrString [attrWorking_setID], /*macro?*/false)), MACRO_16bitToLE(idOrName_toi(attrString [attrNew_active_mask], /*macro?*/false)));

              objChildCommands++;
            }
            break;
          case ctChangeSoftKeyMask:
            if(child->hasAttributes())
            {
              // parse through all attributes
              pAttributes = child->getAttributes();
              int nSize = pAttributes->getLength();

              cleanAttribute(attrMask_type);
              cleanAttribute(attrMaskID);
              cleanAttribute(attrNew_softkey_mask);

              for(int i=0;i<nSize;++i)
              {
                DOMAttr *pAttributeNode = (DOMAttr*) pAttributes->item(i);
                utf16convert ((char *)pAttributeNode->getName(), attr_name, 1024);
                utf16convert ((char *)pAttributeNode->getValue(), attr_value, 1024);

                setAttributeValue(attrMask_type);
                setAttributeValue(attrMaskID);
                setAttributeValue(attrNew_softkey_mask);
              }
              // Need check for all attributes being present for this command -bac
              sprintf(commandMessage, "0xAD, %d, %d, %d, %d, %d, 0xFF, 0xFF", atoi(attrString [attrMask_type]), MACRO_16bitToLE(idOrName_toi(attrString [attrMaskID], /*macro?*/false)), MACRO_16bitToLE(idOrName_toi(attrString [attrNew_softkey_mask], /*macro?*/false)));

              objChildCommands++;
            }
            break;
          case ctChangeAttribute:
            if(child->hasAttributes())
            {
              // parse through all attributes
              pAttributes = child->getAttributes();
              int nSize = pAttributes->getLength();

              cleanAttribute(attrObjectID);
              cleanAttribute(attrAttributeID);
              cleanAttribute(attrNew_value);

              for(int i=0;i<nSize;++i)
              {
                DOMAttr *pAttributeNode = (DOMAttr*) pAttributes->item(i);
                utf16convert ((char *)pAttributeNode->getName(), attr_name, 1024);
                utf16convert ((char *)pAttributeNode->getValue(), attr_value, 1024);

                setAttributeValue(attrObjectID);
                setAttributeValue(attrAttributeID);
                setAttributeValue(attrNew_value);
              }
              // Need check for all attributes being present for this command -bac
              sprintf(commandMessage, "0xAF, %d, %d, %d, %d, %d, %d, %d", MACRO_16bitToLE(idOrName_toi(attrString [attrObjectID], /*macro?*/false)), atoi(attrString [attrAttributeID]), MACRO_32bitToLE(idOrName_toi(attrString [attrNew_value], /*macro?*/false)));

              objChildCommands++;
            }
            break;
          case ctChangePriority:
            if(child->hasAttributes())
            {
              // parse through all attributes
              pAttributes = child->getAttributes();
              int nSize = pAttributes->getLength();

              cleanAttribute(attrObjectID);
              cleanAttribute(attrNew_priority);

              for(int i=0;i<nSize;++i)
              {
                DOMAttr *pAttributeNode = (DOMAttr*) pAttributes->item(i);
                utf16convert ((char *)pAttributeNode->getName(), attr_name, 1024);
                utf16convert ((char *)pAttributeNode->getValue(), attr_value, 1024);

                setAttributeValue(attrObjectID);
                setAttributeValue(attrNew_priority);
              }
              // Need check for all attributes being present for this command -bac
              sprintf(commandMessage, "0xB0, %d, %d, %d, 0xFF, 0xFF, 0xFF, 0xFF", MACRO_16bitToLE(idOrName_toi(attrString [attrObjectID], /*macro?*/false)), prioritytoi(attrString [attrNew_priority]));

              objChildCommands++;
            }
            break;
          case ctChangeListItem:
          if(child->hasAttributes())
          {
            // parse through all attributes
            pAttributes = child->getAttributes();
            int nSize = pAttributes->getLength();

            cleanAttribute(attrObjectID);
            cleanAttribute(attrList_index);
            cleanAttribute(attrNew_objectID);

            for(int i=0;i<nSize;++i)
            {
              DOMAttr *pAttributeNode = (DOMAttr*) pAttributes->item(i);
              utf16convert ((char *)pAttributeNode->getName(), attr_name, 1024);
              utf16convert ((char *)pAttributeNode->getValue(), attr_value, 1024);

              setAttributeValue(attrObjectID);
              setAttributeValue(attrList_index);
              setAttributeValue(attrNew_objectID);
            }
            // Need check for all attributes being present for this command -bac
            sprintf(commandMessage, "0xB1, %d, %d, %d, %d, %d, 0xFF, 0xFF", MACRO_16bitToLE(idOrName_toi(attrString [attrObjectID], /*macro?*/false)), atoi(attrString [attrList_index]), MACRO_16bitToLE(idOrName_toi(attrString [attrNew_objectID], /*macro?*/false)));

            objChildCommands++;
          }
          break;
        }

        if (firstElement)
        {
          fprintf (partFileB, "const uint8_t iVtObject%s_aMacro_Commands [] = {", objName);
        } else {
          fprintf (partFileB, ", ");
        }

        // We need something like this, but up above for each command type in order to deal with the varying attributes for each command. . . -bac
        /*if (!(attrIsGiven [attrEvent]))
        {
          std::cout << "\n\nevent ATTRIBUTE NEEDED IN <macro ...> ! STOPPING PARSER! bye.\n\n";
          clean_exit (-1);
        }*/
        //fprintf (partFileB, "{%d, &vtObject%s}", atoi (attrString [attrEvent]), objChildName);
        fprintf (partFileB, "%s", commandMessage);
        firstElement = false;
        }
      }

      if (firstElement == false)
        fprintf (partFileB, "};\n");
    }
// ****************** End of if (objHasArrayMacroCommand) *************************

  // ### Print out point array for a Polygon Object
    if (objHasArrayPoints)
    {
      // Process all Child-Elements
      bool firstElement = true;
      objChildPoints = 0;
      for (child = n->getFirstChild(); child != 0; child=child->getNextSibling())
      {
        if ( (child->getNodeType() == DOMNode::ELEMENT_NODE) && (0 == strcmp (XMLString::transcode(child->getNodeName()), otCompTable [otPoint]) ))
        {
          // get 'event=' and 'name=' out of child
          if(child->hasAttributes()) {
            // parse through all attributes
            pAttributes = child->getAttributes();
            int nSize = pAttributes->getLength();

            attrString [attrPos_x] [stringLength+1-1] = 0x00; attrIsGiven [attrPos_x] = false;
            attrString [attrPos_y] [stringLength+1-1] = 0x00; attrIsGiven [attrPos_y] = false;
            objChildName [stringLength+1-1] = 0x00; is_objChildName = false;

            for(int i=0;i<nSize;++i) {
              DOMAttr *pAttributeNode = (DOMAttr*) pAttributes->item(i);
              utf16convert ((char *)pAttributeNode->getName(), attr_name, 1024);
              utf16convert ((char *)pAttributeNode->getValue(), attr_value, 1024);

              if (strncmp (attr_name, "pos_x", stringLength) == 0)
              {
                strncpy (attrString [attrPos_x], attr_value, stringLength);
                attrIsGiven [attrPos_x] = true;
              }
              if (strncmp (attr_name, "pos_y", stringLength) == 0)
              {
                strncpy (attrString [attrPos_y], attr_value, stringLength);
                attrIsGiven [attrPos_y] = true;
              }
            }
          }

          if (firstElement) {
            fprintf (partFileB, "const IsoAgLib::repeat_x_y_s iVtObject%s_aPoints [] = {", objName);
          } else {
            fprintf (partFileB, ", ");
          }
          if (!(attrIsGiven [attrPos_x])) {
            std::cout << "\n\npos_x ATTRIBUTE NEEDED IN <point ...> ! STOPPING PARSER! bye.\n\n";
            clean_exit (-1);
          }
          if (!(attrIsGiven [attrPos_y])) {
            std::cout << "\n\npos_y ATTRIBUTE NEEDED IN <point ...> ! STOPPING PARSER! bye.\n\n";
            clean_exit (-1);
          }
          fprintf (partFileB, "{%d, %d}", atoi(attrString [attrPos_x]), atoi(attrString [attrPos_y]));
          objChildPoints++;
          firstElement = false;
        }
      }
      if (firstElement == false)
        fprintf (partFileB, "};\n");
    }
//*************************************************************************************************************************************************************
//******************** End of code added by Brad Cox **********************************************************************************************************


  // ###################################################
  // ### Print out definition, values and init-calls ###
  // ###################################################

  fprintf (partFileA, "IsoAgLib::iVtObject%s_c iVtObject%s;\n", otClassnameTable [objType], objName);
  fprintf (partFileB, "const IsoAgLib::iVtObject_c::iVtObject%s_s iVtObject%s_sROM = {%d", otClassnameTable [objType], objName, objID);
  fprintf (partFileC, "  iVtObject%s.init (&iVtObject%s_sROM SINGLETON_VEC_KEY_PARAMETER_VAR_WITH_COMMA);\n", objName, objName);
  fprintf (partFileD, "#define iVtObjectID%s %d\n", objName, objID);
  if (firstLineFileE) {
    firstLineFileE = false;
  } else {
    fprintf (partFileE, ",");
  }
  fprintf (partFileE, "\n  &iVtObject%s", objName);


  if (strcmp ("NULL", attrString [attrVariable_reference]) != 0)
  { // != 0 means an object reference is given, so add the "&iVtObject" prefix!!
    sprintf (tempString, "&iVtObject%s", attrString [attrVariable_reference]);
    sprintf (attrString [attrVariable_reference], "%s", tempString);
  }
  if (strcmp ("NULL", attrString [attrTarget_value_variable_reference]) != 0)
  { // != 0 means an object reference is given, so add the "&iVtObject" prefix!!
    sprintf (tempString, "&iVtObject%s", attrString [attrTarget_value_variable_reference]);
    sprintf (attrString [attrTarget_value_variable_reference], "%s", tempString);
  }


  // ###########################################
  // ### Print out inidivual object stuff... ###
  // ###########################################
  switch (objType)
  {
    case otWorkingset:
      if (!attrIsGiven [attrActive_mask]) clean_exit (-1, "YOU NEED TO SPECIFY THE active_mask= ATTRIBUTE FOR THE <workingset> OBJECT! STOPPING PARSER! bye.\n\n");
      fprintf (partFileB, ", %d, %d, &iVtObject%s", colortoi (attrString [attrBackground_colour]), booltoi (attrString [attrSelectable]), attrString [attrActive_mask]);
      break;

    case otDatamask:
      if ( (strcmp ("NULL", attrString [attrSoft_key_mask]) == 0)  || (strcmp("65535",  attrString [attrSoft_key_mask]) == 0))
        fprintf (partFileB, ", %d, NULL", colortoi (attrString [attrBackground_colour]));
      else
        fprintf (partFileB, ", %d, &iVtObject%s", colortoi (attrString [attrBackground_colour]), attrString [attrSoft_key_mask]);
      break;

    case otAlarmmask:
      if ( (strcmp ("NULL", attrString [attrSoft_key_mask]) == 0) || (strcmp("65535",  attrString [attrSoft_key_mask]) == 0))
        fprintf (partFileB, ", %d, NULL, %d, %d", colortoi (attrString [attrBackground_colour]), atoi (attrString [attrPriority]), atoi (attrString [attrAcoustic_signal]));
      else
        fprintf (partFileB, ", %d, &iVtObject%s, %d, %d", colortoi (attrString [attrBackground_colour]), attrString [attrSoft_key_mask], prioritytoi (attrString [attrPriority]), acousticsignaltoi (attrString [attrAcoustic_signal]));
      break;

    case otContainer:
      if (!(attrIsGiven [attrWidth] && attrIsGiven [attrHeight])) clean_exit (-1, "YOU NEED TO SPECIFY THE width= AND height= ATTRIBUTES FOR THE <container> OBJECT! STOPPING PARSER! bye.\n\n");
      fprintf (partFileB, ", %s, %s, %d", attrString [attrWidth], attrString [attrHeight], booltoi (attrString [attrHidden]));
      break;

    case otSoftkeymask:
      fprintf (partFileB, ", %d", colortoi (attrString [attrBackground_colour]));
      break;

    case otKey:
      if (!attrIsGiven [attrKey_code]) getKeyCode ();
      fprintf (partFileB, ", %d, %s", colortoi (attrString [attrBackground_colour]), attrString [attrKey_code]);
      fprintf (partFileD, "#define vtKeyCode%s %d\n", objName, atoi (attrString [attrKey_code])); // like in otButton
      break;

    case otButton:
      if (!attrIsGiven [attrKey_code]) getKeyCode ();
      if (!(attrIsGiven [attrWidth] && attrIsGiven [attrHeight])) clean_exit (-1, "YOU NEED TO SPECIFY THE width= AND height= ATTRIBUTES FOR THE <key> OBJECT! STOPPING PARSER! bye.\n\n");
      fprintf (partFileB, ", %s, %s, %d, %d, %s, %d", attrString [attrWidth], attrString [attrHeight], colortoi (attrString [attrBackground_colour]), colortoi (attrString [attrBorder_colour]), attrString [attrKey_code], booltoi (attrString [attrLatchable]));
      fprintf (partFileD, "#define vtKeyCode%s %d\n", objName, atoi (attrString [attrKey_code])); // like in otKey
      break;

    case otInputboolean:
      if (!(attrIsGiven [attrWidth] && attrIsGiven [attrForeground_colour])) clean_exit (-1, "YOU NEED TO SPECIFY THE width= AND foreground_colour= ATTRIBUTES FOR THE <inputboolean> OBJECT! STOPPING PARSER! bye.\n\n");
      if (!attrIsGiven [attrValue])
        sprintf (attrString [attrValue], "0");
      fprintf (partFileB, ", %d, %s, &iVtObject%s, %s, %s, %d", colortoi (attrString [attrBackground_colour]), attrString [attrWidth], attrString [attrForeground_colour], attrString [attrVariable_reference], attrString [attrValue], booltoi (attrString [attrEnabled]));
      break;

    case otInputstring:
      if (!attrIsGiven [attrValue])
      {
        if (!(attrIsGiven [attrWidth] && attrIsGiven [attrHeight] && attrIsGiven [attrFont_attributes] && attrIsGiven [attrLength] && attrIsGiven [attrEnabled]))
        {
          clean_exit (-1, "YOU NEED TO SPECIFY THE width= AND height= AND font_attributes= AND length= AND enabled= ATTRIBUTES FOR THE <inputstring> OBJECT IF NO VALUE IS GIVEN! STOPPING PARSER! bye.\n\n");
        }
        sprintf (attrString [attrValue], "NULL");
      }
      else
      {
        //auto-calculate string length
        if (!attrIsGiven [attrLength]) { sprintf (attrString [attrLength], "%d", strlen (attrString [attrValue])); attrIsGiven [attrLength] = true; }
        if (!(attrIsGiven [attrWidth] && attrIsGiven [attrHeight] && attrIsGiven [attrFont_attributes] && attrIsGiven [attrEnabled]))
        {
          clean_exit (-1, "YOU NEED TO SPECIFY THE width= AND height= AND font_attributes= AND enabled = ATTRIBUTES FOR THE <inputstring> OBJECT! STOPPING PARSER! bye.\n\n");
        }
        copyWithQuoteAndLength (tempString, attrString [attrValue], atoi (attrString [attrLength]));
  //      sprintf (tempString, "\"%s\"", attrString [attrValue]);
        sprintf (attrString [attrValue], "%s", tempString);
      }
      if ( (!attrIsGiven [attrInput_attributes]) || (strcmp( attrString[attrInput_attributes], "65535")==0) )
      {
        sprintf (attrString [attrInput_attributes], "NULL");
        fprintf (partFileB, ", %s, %s, %d, &iVtObject%s, %s, %d, %s, %d, %s, %s, %d", attrString [attrWidth], attrString [attrHeight], colortoi (attrString [attrBackground_colour]), attrString [attrFont_attributes], attrString [attrInput_attributes], optionstoi (attrString [attrOptions]), attrString [attrVariable_reference], horizontaljustificationtoi (attrString [attrHorizontal_justification]), attrString [attrLength], attrString [attrValue], booltoi(attrString [attrEnabled]) );
      }
      else
      {
        fprintf (partFileB, ", %s, %s, %d, &iVtObject%s, &iVtObject%s, %d, %s, %d, %s, %s, %d", attrString [attrWidth], attrString [attrHeight], colortoi (attrString [attrBackground_colour]), attrString [attrFont_attributes], attrString [attrInput_attributes], optionstoi (attrString [attrOptions]), attrString [attrVariable_reference], horizontaljustificationtoi (attrString [attrHorizontal_justification]), attrString [attrLength], attrString [attrValue], booltoi(attrString [attrEnabled]) );
      }
      break;

    case otInputnumber:
      if (!(attrIsGiven [attrWidth] && attrIsGiven [attrHeight] && attrIsGiven [attrFont_attributes] && attrIsGiven [attrMin_value] && attrIsGiven [attrMax_value]
        && attrIsGiven [attrOffset] && attrIsGiven [attrScale] && attrIsGiven [attrNumber_of_decimals] && attrIsGiven [attrFormat]))
      {
        clean_exit (-1, "YOU NEED TO SPECIFY THE width= AND height= AND font_attributes= AND min_value= AND max_value= AND offset= AND scale= AND number_of_decimals= AND format= ATTRIBUTES FOR THE <inputnumber> OBJECT! STOPPING PARSER! bye.\n\n");
      }
      if (!attrIsGiven [attrValue])
        sprintf (attrString [attrValue], "0");
      fprintf (partFileB, ", %s, %s, %d, &iVtObject%s, %d, %s, %sUL, %sUL, %sUL, %sL, %s, %s, %d, %d, %d", attrString [attrWidth], attrString [attrHeight],
        colortoi (attrString [attrBackground_colour]), attrString [attrFont_attributes], optionstoi (attrString [attrOptions]), attrString [attrVariable_reference],
        attrString [attrValue], attrString [attrMin_value], attrString [attrMax_value], attrString [attrOffset], attrString [attrScale],
        attrString [attrNumber_of_decimals], formattoi (attrString [attrFormat]), horizontaljustificationtoi (attrString [attrHorizontal_justification]),
        booltoi (attrString [attrEnabled]));
      break;

    case otInputlist:
      if (!(attrIsGiven [attrWidth] && attrIsGiven [attrHeight])) clean_exit (-1, "YOU NEED TO SPECIFY THE width= AND height= ATTRIBUTES FOR THE <inputlist> OBJECT! STOPPING PARSER! bye.\n\n");
      if (!attrIsGiven [attrValue])
        sprintf (attrString [attrValue], "0");
      fprintf (partFileB, ", %s, %s, %s, %s, %d", attrString [attrWidth], attrString [attrHeight], attrString [attrVariable_reference], attrString [attrValue],
          booltoi (attrString [attrEnabled]));
      break;

    case otOutputstring:
      if (!attrIsGiven [attrValue]) {
        // Variable Reference
        if (!(attrIsGiven [attrWidth] && attrIsGiven [attrHeight] && attrIsGiven [attrFont_attributes] && attrIsGiven [attrLength])) {
          clean_exit (-1, "YOU NEED TO SPECIFY THE width= AND height= AND font_attributes= AND length= ATTRIBUTES FOR THE <outputstring> OBJECT WHEN VALUE IS SET BY REFERENCE! STOPPING PARSER! bye.\n\n");
        }
        sprintf (attrString [attrValue], "NULL");
      } else {
        // Direct Value
        if (!attrIsGiven [attrLength]) {
          // Auto-calculate Length-field
          sprintf (attrString [attrLength], "%d", strlen (attrString [attrValue])); attrIsGiven [attrLength] = true;
        }
        if (!(attrIsGiven [attrWidth] && attrIsGiven [attrHeight] && attrIsGiven [attrFont_attributes] && attrIsGiven [attrLength])) {
            clean_exit (-1, "YOU NEED TO SPECIFY THE width= AND height= AND font_attributes= AND length= ATTRIBUTES FOR THE <outputstring> OBJECT! STOPPING PARSER! bye.\n\n");
        }
        copyWithQuoteAndLength (tempString, attrString [attrValue], atoi (attrString [attrLength]));
        sprintf (attrString [attrValue], "%s", tempString);
      }
      fprintf (partFileB, ", %s, %s, %d, &iVtObject%s, %d, %s, %d, %s, %s", attrString [attrWidth], attrString [attrHeight], colortoi (attrString [attrBackground_colour]),
                attrString [attrFont_attributes], optionstoi (attrString [attrOptions]), attrString [attrVariable_reference],
                horizontaljustificationtoi (attrString [attrHorizontal_justification]), attrString [attrLength], attrString [attrValue]);
  //    printf ("%s --- %d\n", attrString [attrOptions], optionstoi (attrString [attrOptions]));
      break;

    case otOutputnumber:
      if (!(attrIsGiven [attrWidth] && attrIsGiven [attrHeight] && attrIsGiven [attrFont_attributes] && attrIsGiven [attrOffset] && attrIsGiven [attrScale]
        && attrIsGiven [attrNumber_of_decimals] && attrIsGiven [attrFormat]))
      {
      clean_exit (-1, "YOU NEED TO SPECIFY THE width= AND height= AND font_attributes= AND offset= AND scale= AND number_of_decimals= AND format= ATTRIBUTES FOR THE <outputnumber> OBJECT! STOPPING PARSER! bye.\n\n");
      }
      if (!attrIsGiven [attrValue])
      sprintf (attrString [attrValue], "0");
      fprintf (partFileB, ", %s, %s, %d, &iVtObject%s, %d, %s, %sUL, %sL, %s, %s, %d, %d", attrString [attrWidth], attrString [attrHeight],
        colortoi (attrString [attrBackground_colour]), attrString [attrFont_attributes], outputnumberoptionstoi (attrString [attrOptions]),
        attrString [attrVariable_reference], attrString [attrValue], attrString [attrOffset], attrString [attrScale], attrString [attrNumber_of_decimals],
        formattoi (attrString [attrFormat]), horizontaljustificationtoi (attrString [attrHorizontal_justification]));
      break;

    case otLine:
      if (!(attrIsGiven [attrLine_attributes] && attrIsGiven [attrWidth] && attrIsGiven [attrHeight]))
      {
      clean_exit (-1, "YOU NEED TO SPECIFY THE Line_attributes= AND width= AND height= ATTRIBUTES FOR THE <line> OBJECT! STOPPING PARSER! bye.\n\n");
      }
      fprintf (partFileB, ", &iVtObject%s, %s, %s, %d", attrString [attrLine_attributes], attrString [attrWidth], attrString [attrHeight], linedirectiontoi (attrString [attrLine_direction]));
      break;

    case otRectangle:
      //clean_exit (-1, "<rectangle> OBJECT NOT YET IMPLEMENTED STOPPING PARSER! bye.\n\n");
      if (!(attrIsGiven [attrLine_attributes] && attrIsGiven [attrWidth] && attrIsGiven [attrHeight])) clean_exit (-1, "YOU NEED TO SPECIFY THE Line_attributes= AND width= AND height= ATTRIBUTES FOR THE <rectangle> OBJECT! STOPPING PARSER! bye.\n\n");
      if (!attrIsGiven [attrLine_suppression])
        sprintf (attrString [attrLine_suppression], "0");
      if ( (!attrIsGiven [attrFill_attributes]) || (strcmp( attrString[attrFill_attributes], "65535")==0) )
      {
        sprintf (attrString [attrFill_attributes], "NULL");
      }
      else
      {
        sprintf (tempString, "&iVtObject%s", attrString[attrFill_attributes]);
        sprintf (attrString [attrFill_attributes], "%s", tempString);
      }

      //else
      // sprintf (attrString [attrFill_attributes], "&vtObject%s", attrString[attrFill_attributes]);
      fprintf (partFileB, ", &iVtObject%s, %s, %s, %d, %s", attrString [attrLine_attributes], attrString [attrWidth], attrString [attrHeight], linesuppressiontoi (attrString [attrLine_suppression]), attrString[attrFill_attributes]);
      break;

    case otEllipse:
      //clean_exit (-1, "<ellipse> OBJECT NOT YET IMPLEMENTED STOPPING PARSER! bye.\n\n");
      if (!(attrIsGiven [attrLine_attributes] && attrIsGiven [attrWidth] && attrIsGiven [attrHeight] && attrIsGiven [attrStart_angle] && attrIsGiven [attrEnd_angle])) clean_exit (-1, "YOU NEED TO SPECIFY THE Line_attributes= AND width= AND height= AND start_angle= AND end_angle= ATTRIBUTES FOR THE <ellipse> OBJECT! STOPPING PARSER! bye.\n\n");
      if (atoi(attrString[attrStart_angle]) > 180 || atoi(attrString[attrEnd_angle]) > 180) clean_exit (-1, "start_angle= AND end_angle= FOR THE <ellipse> OBJECT NEED TO HAVE A VALUE LESS OR EQUAL TO 180! STOPPING PARSER! bye.\n\n");
      if (!attrIsGiven [attrEllipse_type])
        sprintf (attrString [attrEllipse_type], "0");
      if ( (!attrIsGiven [attrFill_attributes]) || (strcmp( attrString[attrFill_attributes], "65535")==0) )
      {
        sprintf (attrString [attrFill_attributes], "NULL");
      }
      else
      {
        sprintf (tempString, "&iVtObject%s", attrString[attrFill_attributes]);
        sprintf (attrString [attrFill_attributes], "%s", tempString);
      }
      fprintf (partFileB, ", &iVtObject%s, %s, %s, %d, %s, %s, %s", attrString [attrLine_attributes], attrString [attrWidth], attrString [attrHeight], ellipsetypetoi (attrString [attrEllipse_type]), attrString[attrStart_angle], attrString[attrEnd_angle], attrString[attrFill_attributes]);
      break;

    case otPolygon:
      //clean_exit (-1, "<polygon> OBJECT NOT YET IMPLEMENTED STOPPING PARSER! bye.\n\n");
      if (!(attrIsGiven [attrWidth] && attrIsGiven [attrHeight] && attrIsGiven [attrLine_attributes])) clean_exit (-1, "YOU NEED TO SPECIFY THE width= AND height= AND Line_attributes= ATTRIBUTES FOR THE <polygon> OBJECT! STOPPING PARSER! bye.\n\n");
      if (!attrIsGiven [attrPolygon_type])
        sprintf (attrString [attrPolygon_type], "0");
      if ( (!attrIsGiven [attrFill_attributes]) || (strcmp( attrString[attrFill_attributes], "65535")==0) )
      {
        sprintf (attrString [attrFill_attributes], "NULL");
      } else {
        sprintf (tempString, "&iVtObject%s", attrString[attrFill_attributes]);
        sprintf (attrString [attrFill_attributes], "%s", tempString);
      }
      fprintf (partFileB, ", %s, %s, &iVtObject%s, %s, %d", attrString [attrWidth], attrString [attrHeight], attrString [attrLine_attributes], attrString[attrFill_attributes], polygontypetoi (attrString [attrPolygon_type]));
      break;

    case otMeter:
      //clean_exit (-1, "<meter> OBJECT NOT YET IMPLEMENTED STOPPING PARSER! bye.\n\n");
      if (!(attrIsGiven [attrWidth] && attrIsGiven [attrNeedle_colour] && attrIsGiven [attrBorder_colour] && attrIsGiven [attrArc_and_tick_colour] && attrIsGiven [attrNumber_of_ticks] && attrIsGiven[attrStart_angle] && attrIsGiven[attrEnd_angle] && attrIsGiven [attrMin_value] && attrIsGiven [attrMax_value])) clean_exit (-1, "YOU NEED TO SPECIFY THE width= AND needle_colour= AND border_colour= AND target_line_colour= AND number_of_ticks= AND start_angle AND end_angle AND min_value= AND max_value= ATTRIBUTES FOR THE <meter> OBJECT! STOPPING PARSER! bye.\n\n");
      if (atoi(attrString[attrStart_angle]) > 180 || atoi(attrString[attrEnd_angle]) > 180) clean_exit (-1, "start_angle= AND end_angle= FOR THE <meter> OBJECT NEED TO HAVE A VALUE LESS OR EQUAL TO 180! STOPPING PARSER! bye.\n\n");
      if (!attrIsGiven [attrValue])
        sprintf (attrString [attrValue], "0");
      if (!attrIsGiven [attrOptions])
        sprintf(attrString[attrOptions], "0");

      fprintf (partFileB, ", %s, %d, %d, %d, %d, %s, %s, %s, %s, %s, %s, %s", attrString [attrWidth], colortoi(attrString[attrNeedle_colour]), colortoi (attrString [attrBorder_colour]), colortoi (attrString [attrArc_and_tick_colour]), meteroptionstoi (attrString [attrOptions]), attrString [attrNumber_of_ticks], attrString[attrStart_angle], attrString[attrEnd_angle], attrString [attrMin_value], attrString [attrMax_value], attrString [attrVariable_reference], attrString [attrValue]);
      break;

    case otLinearbargraph:
      if (!(attrIsGiven [attrWidth] && attrIsGiven [attrHeight] && attrIsGiven [attrColour] && attrIsGiven [attrTarget_line_colour]
        && attrIsGiven [attrNumber_of_ticks] && attrIsGiven [attrMin_value] && attrIsGiven [attrMax_value]))
      {
        clean_exit (-1, "YOU NEED TO SPECIFY THE width= AND height= AND colour= AND target_line_colour= AND number_of_ticks= AND min_value= AND max_value= ATTRIBUTES FOR THE <linearbargraph> OBJECT! STOPPING PARSER! bye.\n\n");
      }
      if (!attrIsGiven [attrValue])
        sprintf (attrString [attrValue], "0");
      if (!attrIsGiven [attrTarget_value])
        sprintf (attrString [attrTarget_value], "0");
      fprintf (partFileB, ", %s, %s, %d, %d, %d, %s, %s, %s, %s, %s, %s, %s", attrString [attrWidth], attrString [attrHeight], colortoi (attrString [attrColour]),
        colortoi (attrString [attrTarget_line_colour]), linearbargraphoptionstoi (attrString [attrOptions]), attrString [attrNumber_of_ticks],
        attrString [attrMin_value], attrString [attrMax_value], attrString [attrVariable_reference], attrString [attrValue],
        attrString [attrTarget_value_variable_reference], attrString [attrTarget_value]);
      break;

    case otArchedbargraph:
      //clean_exit (-1, "<archedbargraph> OBJECT NOT YET IMPLEMENTED STOPPING PARSER! bye.\n\n");
      if (!(attrIsGiven [attrWidth] && attrIsGiven [attrHeight] && attrIsGiven [attrColour] && attrIsGiven [attrTarget_line_colour] && attrIsGiven [attrStart_angle] && attrIsGiven [attrEnd_angle] && attrIsGiven[attrBar_graph_width] && attrIsGiven [attrMin_value] && attrIsGiven [attrMax_value])) clean_exit (-1, "YOU NEED TO SPECIFY THE width= AND height= AND colour= AND target_line_colour= AND start_angle= AND end_angle= AND bar_graph_width AND min_value= AND max_value= ATTRIBUTES FOR THE <archedbargraph> OBJECT! STOPPING PARSER! bye.\n\n");
      if (atoi(attrString[attrStart_angle]) > 180 || atoi(attrString[attrEnd_angle]) > 180) clean_exit (-1, "start_angle= AND end_angle= FOR THE <archedbargraph> OBJECT NEED TO HAVE A VALUE LESS OR EQUAL TO 180! STOPPING PARSER! bye.\n\n");
      if (!attrIsGiven [attrValue])
        sprintf (attrString [attrValue], "0");
      if (!attrIsGiven [attrTarget_value])
        sprintf (attrString [attrTarget_value], "0");
      if (!attrIsGiven [attrOptions])
        sprintf (attrString [attrOptions], "0");
      fprintf (partFileB, ", %s, %s, %d, %d, %d, %s, %s, %s, %s, %s, %s, %s, %s, %s", attrString [attrWidth], attrString [attrHeight], colortoi (attrString [attrColour]), colortoi (attrString [attrTarget_line_colour]), archedbargraphoptionstoi (attrString [attrOptions]), attrString[attrStart_angle], attrString[attrEnd_angle], attrString[attrBar_graph_width], attrString [attrMin_value], attrString [attrMax_value], attrString [attrVariable_reference], attrString [attrValue], attrString [attrTarget_value_variable_reference], attrString [attrTarget_value]);
      break;

    case otPicturegraphic:
      // check moved above!
      fprintf (partFileB, ", %d, %d,%d, %d, %d, %d", deXwidth, deXactualWidth, deXactualHeight, deXcolorDepth, objBitmapOptions, deXtransCol);
      break;

    case otNumbervariable:
      if (!attrIsGiven [attrValue])
        sprintf (attrString [attrValue], "0");
      fprintf (partFileB, ", %sUL", attrString [attrValue]);
      break;

    case otStringvariable:
      if (!attrIsGiven [attrValue]) {
        if (!(attrIsGiven [attrLength])) clean_exit (-1, "YOU NEED TO SPECIFY THE length= ATTRIBUTE FOR THE <stringvariable> OBJECT! STOPPING PARSER! bye.\n\n");
        sprintf (attrString [attrValue], "NULL");
      } else {
        //auto-calculate length
        if (!attrIsGiven [attrLength]) { sprintf (attrString [attrLength], "%d", strlen (attrString [attrValue])); attrIsGiven [attrLength] = true; }
        copyWithQuoteAndLength (tempString, attrString [attrValue], atoi (attrString [attrLength]));
    //     sprintf (tempString, "\"%s\"", attrString [attrValue]);
        sprintf (attrString [attrValue], "%s", tempString);
      }
      fprintf (partFileB, ", %s, %s", attrString [attrLength], attrString [attrValue]);
      break;

    case otFontattributes:
      if (!attrIsGiven [attrFont_type])
      {
        clean_exit (-1, "INFORMATION: WITH THAT VERSION OF VT2ISO YOU NEED TO SPECIFY THE font_type= ATTRIBUTE FOR THE <fontattributes> OBJECT! \n \
        VALID VALUES ARE latin1, latin9 or proprietary! STOPPING PARSER! bye.\n\n");
      }

      if (!(attrIsGiven [attrFont_colour] && attrIsGiven [attrFont_size]))
      {
        clean_exit (-1, "YOU NEED TO SPECIFY THE font_colour= AND font_size= AND font_type= ATTRIBUTE FOR THE <fontattributes> OBJECT! STOPPING PARSER! bye.\n\n");
      }
      fprintf (partFileB, ", %d, %d, %d, %d", colortoi (attrString [attrFont_colour]), fontsizetoi (attrString [attrFont_size]), fonttypetoi (attrString [attrFont_type]), fontstyletoi (attrString [attrFont_style]));
      break;

    case otLineattributes:
      if (!(attrIsGiven [attrLine_colour] && attrIsGiven [attrLine_width] && attrIsGiven [attrLine_art]))
      {
        clean_exit (-1, "YOU NEED TO SPECIFY THE line_colour= AND line_width= AND line_art= ATTRIBUTE FOR THE <lineattributes> OBJECT! STOPPING PARSER! bye.\n\n");
      }
      fprintf (partFileB, ", %d, %s, 0x%x", colortoi (attrString [attrLine_colour]), attrString [attrLine_width], linearttoi (attrString [attrLine_art]));
      break;

    case otFillattributes:
      //clean_exit (-1, "<fillattribute> OBJECT NOT YET IMPLEMENTED. STOPPING PARSER! bye.\n\n");
      if (!(attrIsGiven [attrFill_colour])) clean_exit (-1, "YOU NEED TO SPECIFY THE fill_colour= ATTRIBUTE FOR THE <fillattributes> OBJECT! STOPPING PARSER! bye.\n\n");
      if (!attrIsGiven [attrFill_type])
        sprintf (attrString [attrFill_type], "0");
      if(!attrIsGiven [attrFill_pattern])
      {
        sprintf (attrString [attrFill_pattern], "NULL");
        fprintf (partFileB, ",%d, %d, %s", filltypetoi (attrString[attrFill_type]), colortoi (attrString [attrFill_colour]), attrString [attrFill_pattern]);
      }
      else
      {
        fprintf (partFileB, ",%d, %d, &iVtObject%s", filltypetoi (attrString[attrFill_type]), colortoi (attrString [attrFill_colour]), attrString [attrFill_pattern]);
      }
      break;

    case otInputattributes:
      //clean_exit (-1, "<inputattribute> OBJECT NOT YET IMPLEMENTED. STOPPING PARSER! bye.\n\n");
      if (!attrIsGiven [attrValidation_type])
        sprintf (attrString [attrValidation_type], "0");
      if (!attrIsGiven [attrValidation_string]) {
        if (!(attrIsGiven [attrLength])) clean_exit (-1, "YOU NEED TO SPECIFY THE length= ATTRIBUTE FOR THE <inputattribute> OBJECT! STOPPING PARSER! bye.\n\n");
        sprintf (attrString [attrValidation_string], "NULL");
      } else {
        // auto-calculate string-length
        if (!attrIsGiven [attrLength]) { sprintf (attrString [attrLength], "%d", strlen (attrString [attrValidation_string])); attrIsGiven [attrLength] = true; }
        sprintf (tempString, "\"%s\"", attrString [attrValidation_string]);
        sprintf (attrString [attrValidation_string], "%s", tempString);
      }
      fprintf (partFileB, ", %d, %s, %s", validationtypetoi (attrString [attrValidation_type]), attrString [attrLength], attrString [attrValidation_string]);
      break;

    case otObjectpointer:
      if ( (!attrIsGiven [attrValue]) || (strcmp( attrString[attrValue], "65535")==0) )
      {
        sprintf (attrString [attrValue], "NULL");
        attrIsGiven [attrValue] = true;
      }
      if (strcmp ("NULL", attrString [attrValue]) != 0)
      { // != 0 means an object reference is given, so add the "&iVtObject" prefix!!
        sprintf (tempString, "&iVtObject%s", attrString [attrValue]);
        sprintf (attrString [attrValue], "%s", tempString);
      }
      fprintf (partFileB, ", %s", attrString [attrValue]);
      break;

    case otMacro:
      fprintf (partFileB, ", %s", attrString [attrNumber_of_bytes]);
      break;
    case otAuxiliaryfunction:
      if (!(attrIsGiven [attrBackground_colour] && attrIsGiven[attrFunction_type]) )
      {
        clean_exit (-1, "YOU NEED TO SPECIFY THE background_colour= and function_type= ATTRIBUTE FOR THE <auxiliaryfunction> OBJECT! STOPPING PARSER! bye.\n\n");
      }
      fprintf (partFileB, ", %d, %d", colortoi (attrString [attrBackground_colour]), auxfunctiontyptetoi(attrString [attrFunction_type]));
      break;
    case otAuxiliaryinput:
      if (!(attrIsGiven [attrBackground_colour] && attrIsGiven[attrFunction_type] && attrIsGiven[attrInput_id]) )
      {
        clean_exit (-1, "YOU NEED TO SPECIFY THE background_colour= and function_type= and input_id= ATTRIBUTE FOR THE <auxiliaryinput> OBJECT! STOPPING PARSER! bye.\n\n");
      }
      fprintf (partFileB, ", %d, %d, %s", colortoi (attrString [attrBackground_colour]), auxfunctiontyptetoi(attrString [attrFunction_type]), attrString[attrInput_id]);
      break;
    }


    // #########################################
    // ### Print out Repeat Array REFERENCES ###
    // #########################################

    if (objHasArrayObject) {
      if (objChildObjects == 0) {
        fprintf (partFileB, ", 0,NULL");
      } else {
        fprintf (partFileB, ", %d,iVtObject%s_aObject", objChildObjects, objName);
      }
    }

    if (objHasArrayObjectXY) {
      if (objChildObjects == 0) {
        fprintf (partFileB, ", 0,NULL");
      } else {
        fprintf (partFileB, ", %d,iVtObject%s_aObject_x_y_font_row_col", objChildObjects, objName);
      }
    }

    if (objType == otPicturegraphic) {
      for (unsigned int actDepth=0; actDepth <= 2; actDepth++) {
        if ( (actDepth > deXcolorDepth) || (stdRawBitmapBytes [actDepth] == 0)) {
          fprintf (partFileB, ", 0,NULL");
        } else {
          fprintf (partFileB, ", %d,iVtObject%s_aRawBitmap%d", stdRawBitmapBytes [actDepth], objName, actDepth);
        }
      }
      if (fixNr == 0) {
        fprintf (partFileB, ", 0,NULL");
      } else {
        fprintf (partFileB, ", %d,iVtObject%s_aFixedBitmaps", fixNr, objName);
      }
    }

    if (objHasArrayPoints) {
      if(objChildPoints == 0) {
        fprintf (partFileB, ", 0,NULL");
      } else {
        fprintf (partFileB, ", %d,iVtObject%s_aPoints", objChildPoints, objName);
      }
    }

    if (objHasArrayEventMacro) {
      if (objChildMacros == 0) {
        fprintf (partFileB, ", 0,NULL");
      } else {
        // Changed this line to give the correct name to the Macro object to match the naming conventions of IsoAgLib V 1.1.0.
        // This coincides with a change made above to the name of the Macro struct. -bac 06-Jan-2005
        //fprintf (partFileB, ", %d,iVtObject%s_aEvent_Macro", objChildMacros, objName);
        fprintf (partFileB, ", %d,iVtObject%s_aMacro_Object", objChildMacros, objName);
      }
    }

    if (objHasArrayLanguagecode) {
      if (objChildLanguages == 0) {
        fprintf (partFileB, ", 0,NULL");
      } else {
        fprintf (partFileB, ", %d,ivtObject%s_aLanguage", objChildLanguages, objName);
      }
    }

    if (objHasArrayMacroCommand) { /** @todo I think the "else" is not correct, the length has to be put out also!? -mjw */
      if (objChildCommands == 0) {
        fprintf (partFileB, ", 0,NULL");
      } else {
        fprintf (partFileB, ", iVtObject%s_aMacro_Commands", objName);
      }
    }
    fprintf (partFileB, "};\n"); //s_ROM bla blub terminator...
  }


  // Add all Child-Elements recursively
  uint64_t omcType = omcTypeTable [objType];
  if (objType == otContainer) omcType = ombType; // Object May Contain what the Object Is - Simple rule. more simple than the graphic in the spec. ;)

  for (child = n->getFirstChild(); child != 0; child=child->getNextSibling())
  {
    if (child->getNodeType() == DOMNode::ELEMENT_NODE)
    {
      processElement (child, omcType, rc_workDir);
    }
  }
}


// ---------------------------------------------------------------------------
//
//  int main (argC, argV) --- parse XML file and output to partA, partB and partC files.
//
// ---------------------------------------------------------------------------
int main(int argC, char* argV[])
{
  // Check command line and extract arguments.
  if (argC < 2) { usage(); return 1; }

  const char*    xmlFile = 0;
  AbstractDOMParser::ValSchemes valScheme = AbstractDOMParser::Val_Auto;
  bool        doNamespaces    = false;
  bool        doSchema     = false;
  bool        schemaFullChecking = false;
  bool        errorOccurred = false;
  char        localeStr[64];
  char        xmlFiles [256] [1024+1];
  char        xmlFileTemp [1024+1];
  int      indexXmlFile, amountXmlFiles=0;

  std_bitmap_path [0] = 0x00;
  fix_bitmap_path [0] = 0x00;
  attr_name [1024+1-1] = 0x00;
  attr_value [1024+1-1] = 0x00;

  memset(localeStr, 0, sizeof localeStr);

  int argInd;
  for (argInd = 1; argInd < argC; argInd++)
  {
    // Break out on first parm not starting with a dash
    if (argV[argInd][0] != '-')
    break;

    // Watch for special case help request
    if (!strcmp(argV[argInd], "-?"))
    {
      usage();
      return 2;
    }
    else if (!strncmp(argV[argInd], "-v=", 3)
      ||  !strncmp(argV[argInd], "-V=", 3))
    {
      const char* const parm = &argV[argInd][3];

      if (!strcmp(parm, "never"))
        valScheme = AbstractDOMParser::Val_Never;
      else if (!strcmp(parm, "auto"))
        valScheme = AbstractDOMParser::Val_Auto;
      else if (!strcmp(parm, "always"))
        valScheme = AbstractDOMParser::Val_Always;
      else
      {
        std::cerr << "Unknown -v= value: " << parm << std::endl;
        return 2;
      }
    }
    else if (!strcmp(argV[argInd], "-n")
     ||  !strcmp(argV[argInd], "-N"))
    {
      doNamespaces = true;
    }
    else if (!strcmp(argV[argInd], "-s")
      ||  !strcmp(argV[argInd], "-S"))
    {
      doSchema = true;
    }
    else if (!strcmp(argV[argInd], "-f")
      ||  !strcmp(argV[argInd], "-F"))
    {
      schemaFullChecking = true;
    }
    else if (!strncmp(argV[argInd], "-locale=", 8))
    {
      // Get out the end of line
      strcpy(localeStr, &(argV[argInd][8]));
    }
    else
    {
      std::cerr << "Unknown option '" << argV[argInd]
      << "', ignoring it\n" << std::endl;
    }
  }

  //
  //  There should be only one and only one parameter left, and that
  //  should be the file name.
  //
  if (argInd != argC - 1) { usage(); return 1; }


    // get file list with matching files!

    std::basic_string<char> c_fileName( argV [argInd] );
    #ifdef WIN32
    int lastDirPos = c_fileName.find_last_of( "\\" )
    std::basic_string<char> c_directory = c_fileName.substr( 0, lastDirPos+1 );
    if (c_directory == "") c_directory = ".\\";
    #else
    int lastDirPos = c_fileName.find_last_of( "/" );
    std::basic_string<char> c_directory = c_fileName.substr( 0, lastDirPos+1 );
    if (c_directory == "") c_directory = "./";
    #endif
    /* globally defined */  c_project = c_fileName.substr( lastDirPos+1 );
    std::basic_string<char> c_unwantedType = ".inc";
    std::basic_string<char> c_unwantedType2 = ".h";
    std::basic_string<char> c_unwantedType3 = ".inc-template";
    std::basic_string<char> c_unwantedType4 = ".iop";
    std::basic_string<char> c_directoryCompareItem;
    std::cerr << "--> Directory: " << c_directory << std::endl << "--> File:      " << c_project << std::endl;
    strncpy (proName, c_project.c_str(), 1024); proName [1024+1-1] = 0x00;
    for (unsigned int i=0; i<strlen(proName); i++) if (proName[i] == '.') { proName[i] = 0x00; break; }

    #ifdef WIN32
    HANDLE    hList;
    TCHAR     szDir[255];
    TCHAR     szSubDir[255];
    TCHAR    szCurDir[255];
    WIN32_FIND_DATA FileData;


  // save current directory
    GetCurrentDirectory(255, szCurDir);
    std::cerr << "CurDir: " << szCurDir << std::endl;

    // go to new working directory
    if (SetCurrentDirectory(c_directory.c_str()) == 0)
    {
      std::cerr <<  "Couldn't open the directory."<< std::endl;

      CHAR szBuf[80];
        DWORD dw = GetLastError();
      sprintf(szBuf, "Open %s failed: GetLastError returned %u\n", szCurDir, dw);
      printf(szBuf);
      HRESULT_FROM_WIN32(dw);
      return 0;
    }

    // Get the proper directory path
    wsprintf(szDir, TEXT("*") );
    hList = FindFirstFile(szDir, &FileData);
    if (hList != INVALID_HANDLE_VALUE)
    {   // Traverse through the directory structure
      do
      {   // Check the object is a directory or not
        if (!(FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
        {   //if ((strcmp(FileData.cFileName, ".") != 0) && (strcmp(FileData.cFileName, "..") != 0))
          c_directoryCompareItem = FileData.cFileName;
          if ( c_directoryCompareItem[0] == '.' ) continue;
          if (c_directoryCompareItem [c_directoryCompareItem.length()-1] == '~') continue;
          if ( c_directoryCompareItem.substr( c_directoryCompareItem.length()-4 ) == c_unwantedType ) continue;
          if ( c_directoryCompareItem.substr( c_directoryCompareItem.length()-2 ) == c_unwantedType2 ) continue;
          if ( (c_directoryCompareItem.length() > 13) && (c_directoryCompareItem.substr( c_directoryCompareItem.length()-13 ) == c_unwantedType3) ) continue;
          if ( c_directoryCompareItem.substr( c_directoryCompareItem.length()-4 ) == c_unwantedType4 ) continue;

          if ( c_directoryCompareItem.find( c_project ) != std::string::npos ) {
            c_directoryCompareItem.insert(0, "\\" );
            c_directoryCompareItem.insert(0, c_directory );
            strcpy (xmlFiles [amountXmlFiles], c_directoryCompareItem.c_str() );
            amountXmlFiles++;
            //std::cout << "found: " << c_directoryCompareItem << "\n";
          }
        }

      }while (FindNextFile(hList, &FileData));
      FindClose(hList);
    }
    else
    {
      std::cerr <<  "Couldn't open the directory.";
      return 0;
    }
    SetCurrentDirectory(szCurDir);
    #else
    // LINUX -> POSIX method for directory traversal
    DIR *dp;
    dp = opendir (c_directory.c_str());
    if (dp != NULL)
    {
      dirent *ep;
      while ((ep = readdir (dp))) {
        c_directoryCompareItem = ep->d_name;
        if ( c_directoryCompareItem[0] == '.' ) continue;
        if ( (c_directoryCompareItem.length() > 1  ) && ( c_directoryCompareItem [c_directoryCompareItem.length()-1] == '~')) continue;
        if ( (c_directoryCompareItem.length() > 4  ) && ( c_directoryCompareItem.substr( c_directoryCompareItem.length()-4  ) == c_unwantedType  ) ) continue;
        if ( (c_directoryCompareItem.length() > 2  ) && ( c_directoryCompareItem.substr( c_directoryCompareItem.length()-2  ) == c_unwantedType2 ) ) continue;
        if ( (c_directoryCompareItem.length() > 13 ) && ( c_directoryCompareItem.substr( c_directoryCompareItem.length()-13 ) == c_unwantedType3 ) ) continue;
        if ( (c_directoryCompareItem.length() > 4 ) && ( c_directoryCompareItem.substr( c_directoryCompareItem.length()-4 ) == c_unwantedType4 ) ) continue;

        if ( c_directoryCompareItem.find( c_project ) != std::string::npos ) {
          c_directoryCompareItem.insert(0, "/" );
          c_directoryCompareItem.insert(0, c_directory );
          strcpy (xmlFiles [amountXmlFiles], c_directoryCompareItem.c_str() );
          amountXmlFiles++;
      //    std::cout << "found: " << ep->d_name << "\n";
        }
      }
      closedir(dp);
    } else
    {
      std::cerr <<  "Couldn't open the directory '" << c_directory.c_str() << "'.";
      return 0;
    }
    #endif

    // now sort this list
    bool stillSorting;
    do {
      stillSorting=false;
      for (int a=1; a< amountXmlFiles; a++) {
        if (strcmp (xmlFiles [a-1], xmlFiles [a]) > 0) {
          strcpy (xmlFileTemp, xmlFiles [a]);
          strcpy (xmlFiles [a], xmlFiles [a-1]);
          strcpy (xmlFiles [a-1], xmlFileTemp);
          stillSorting=true;
        }
      }
    } while (stillSorting);

    std::cout << std::endl << "--> Sorted Filelist:" << std::endl;
    for (int dex=0; dex < amountXmlFiles; dex++) std::cout << xmlFiles [dex] << "\n";
    std::cout << "\n";

    // Do INITIALIZATION STUFF
    init (argV [argInd]);

    for (indexXmlFile = 0; indexXmlFile < amountXmlFiles; indexXmlFile++)
    { // loop all xmlFiles!
      // Initialize the XML4C system
      try
      {
        if (strlen(localeStr)) {
          XMLPlatformUtils::Initialize(localeStr);
        } else {
          XMLPlatformUtils::Initialize("de_DE");
        }
      }

      catch (const XMLException& toCatch)
      {
        std::cerr << "Error during initialization! :\n"
          << StrX(toCatch.getMessage()) << std::endl;
        return 1;
      }

      // Instantiate the DOM parser.
      static const XMLCh gLS[] = { chLatin_L, chLatin_S, chNull };
      DOMImplementation *impl = DOMImplementationRegistry::getDOMImplementation(gLS);
      DOMBuilder  *parser = ((DOMImplementationLS*)impl)->createDOMBuilder(DOMImplementationLS::MODE_SYNCHRONOUS, 0);

      parser->setFeature(XMLUni::fgDOMNamespaces, doNamespaces);
      parser->setFeature(XMLUni::fgXercesSchema, doSchema);
      parser->setFeature(XMLUni::fgXercesSchemaFullChecking, schemaFullChecking);

      if (valScheme == AbstractDOMParser::Val_Auto)        parser->setFeature(XMLUni::fgDOMValidateIfSchema, true);
      else if (valScheme == AbstractDOMParser::Val_Never)  parser->setFeature(XMLUni::fgDOMValidation, false);
      else if (valScheme == AbstractDOMParser::Val_Always) parser->setFeature(XMLUni::fgDOMValidation, true);

      // enable datatype normalization - default is off
      parser->setFeature(XMLUni::fgDOMDatatypeNormalization, true);

/** @todo Get path of vt2iso and add it to "vt2iso.xsd" */
      char xsdLocation[1024+1];
      strcpy (xsdLocation, argV[0]);
      // now trim exe filename
      for (int i=strlen(xsdLocation)-1; i >= 0; i--) {
        if (xsdLocation[i] == '/') { xsdLocation[i+1]=0x00; break; }
      }
      strcat (xsdLocation, "vt2iso.xsd");
      XMLCh* propertyValue = XMLString::transcode(xsdLocation);
      parser->setProperty(XMLUni::fgXercesSchemaExternalNoNameSpaceSchemaLocation, propertyValue);

      // And create our error handler and install it
      DOMCountErrorHandler errorHandler;
      parser->setErrorHandler(&errorHandler);

      //
      //  Get the starting time and kick off the parse of the indicated
      //  file. Catch any exceptions that might propogate out of it.
      //
      std::ifstream fin;

      char fURI[1000];
      //initialize the array to zeros
      memset(fURI,0,sizeof(fURI));
      xmlFile = xmlFiles [indexXmlFile];

      //reset error count first
      errorHandler.resetErrors();

      XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc = 0;

      try
      {   // reset document pool
        parser->resetDocumentPool();
        doc = parser->parseURI(xmlFile);
      }

      catch (const XMLException& toCatch)
      {
        std::cerr << "\nError during parsing: '" << xmlFile << "'\n"
          << "Exception message is:  \n"
          << StrX(toCatch.getMessage()) << "\n" << std::endl;
        errorOccurred = true;
      }
      catch (const DOMException& toCatch)
      {
        const unsigned int maxChars = 2047;
        XMLCh errText[maxChars + 1];

        std::cerr << "\nDOM Error during parsing: '" << xmlFile << "'\n"
          << "DOMException code is:  " << toCatch.code << std::endl;

        if (DOMImplementation::loadDOMExceptionMsg(toCatch.code, errText, maxChars))
          std::cerr << "Message is: " << StrX(errText) << std::endl;

        errorOccurred = true;
      }
      catch (...)
      {
        std::cerr << "\nUnexpected exception during parsing: '" << xmlFile << "'\n";
        errorOccurred = true;
      }

      //
      //  Extract the DOM tree, get the list of all the elements and report the
      //  length as the count of elements.
      //
      if (errorHandler.getSawErrors() || errorOccurred)
      {
        std::cout << "\nErrors occurred, no output available\n" << std::endl;
        errorOccurred = true;
      } else {
        if (doc) {
          // ### main routine starts right here!!! ###
          processElement ((DOMNode*)doc->getDocumentElement(), (uint64_t(1)<<otObjectpool), c_directory.c_str() ); // must all be included in an objectpool tag !
          if (!is_opDimension) {
          std::cout << "\n\nYOU NEED TO SPECIFY THE dimension= TAG IN <objectpool> ! STOPPING PARSER! bye.\n\n";
          clean_exit (-1);
          }
        }
      }


      //
      //  Delete the parser itself.  Must be done prior to calling Terminate, below.
      //
      parser->release();

      // And call the termination method
      XMLPlatformUtils::Terminate();

    } // loop all files


  if (!is_skWidth) skWidth = 60;
  if (!is_skHeight) skHeight = 32;
  if (!is_skWidth || !is_skHeight) {
    std::cout << "\n\nWARNING: You have NOT specified a SoftKey-Width/Height, so vt2iso assumes your softkeys are designed on a 60x32 pixel basis.\n"
              << "ATTENTION: SoftKeys are scaled and centered to fit the SK-Dimensions of the VT it is displayed on, so take care that you know what you're doing!\n\n";
  }
  fprintf (partFileD, "#define vtObjectPoolDimension %d\n", opDimension);
  fprintf (partFileD, "#define vtObjectPoolSoftKeyWidth %d\n", skWidth);
  fprintf (partFileD, "#define vtObjectPoolSoftKeyHeight %d\n", skHeight);


  if (errorOccurred) clean_exit (4, "XML-Parsing error occured. Terminating.\n");
  else clean_exit (0, "All conversion done successfully.\n");
}





DOMCountErrorHandler::DOMCountErrorHandler()
 : fSawErrors(false)
{
}

DOMCountErrorHandler::~DOMCountErrorHandler()
{
}


// ---------------------------------------------------------------------------
//  DOMCountHandlers: Overrides of the DOM ErrorHandler interface
// ---------------------------------------------------------------------------
bool DOMCountErrorHandler::handleError(const DOMError& domError)
{
  fSawErrors = true;
  if (domError.getSeverity() == DOMError::DOM_SEVERITY_WARNING)
    std::cerr << "\nWarning at file ";
  else if (domError.getSeverity() == DOMError::DOM_SEVERITY_ERROR)
    std::cerr << "\nError at file ";
  else
    std::cerr << "\nFatal Error at file ";

  std::cerr << StrX(domError.getLocation()->getURI())
    << ", line " << domError.getLocation()->getLineNumber()
    << ", char " << domError.getLocation()->getColumnNumber()
    << "\n  Message: " << StrX(domError.getMessage()) << std::endl;

  return true;
}

void DOMCountErrorHandler::resetErrors()
{
 fSawErrors = false;
}
