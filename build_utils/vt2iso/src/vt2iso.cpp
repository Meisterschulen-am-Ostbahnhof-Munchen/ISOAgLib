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
// #include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/parsers/AbstractDOMParser.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>
#include <xercesc/dom/DOMImplementationRegistry.hpp>
#include <xercesc/dom/DOMBuilder.hpp>
// #include <xercesc/dom/DOMException.hpp>
// #include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMNodeList.hpp>
// #include <xercesc/dom/DOMError.hpp>
#include <xercesc/dom/DOMLocator.hpp>
#include <xercesc/dom/DOMNamedNodeMap.hpp>
#include <xercesc/dom/DOMAttr.hpp>
#include <xercesc/dom/DOMElement.hpp>

// Includes (findfirst, findnext)
#include <fstream>

#ifdef WIN32
 #include <windows.h>
 #include <stdio.h>
 #include <ostream>
#else
 #include <dirent.h>
#endif

#ifdef USE_FREE_IMAGE_LIB
#include "vt2isoimagefreeimage_c.h"
#else
#include "vt2isoimagepaintlib_c.h"
#endif

 // Includes (vt2iso)
#include "vt2iso.hpp"

#ifdef USE_SPECIAL_PARSING_PROP
 #include <specialparsinguseproptag_c.h>
#else
  #include <specialparsingbaseproptag_c.h>
#endif

#ifdef USE_SPECIAL_PARSING
 #include <specialparsinguse_c.h>
#else
  #include <specialparsingbase_c.h>
#endif

using namespace std;

BGR_s vtColorTable[256]=
{ /// ATTENTION: This is stored "Blue-Green-Red" !!!
  {0x00,0x00,0x00},//0
  {0xFF,0xFF,0xff},//1
  {0x00,0x99,0x00},//2
  {0x99,0x99,0x00},//3
  {0x00,0x00,0x99},//4
  {0x99,0x00,0x99},//5
  {0x00,0x99,0x99},//6
  {0xCC,0xCC,0xCC},//7
  {0x99,0x99,0x99},//8
  {0xFF,0x00,0x00},//9
  {0x00,0xFF,0x00},//10
  {0xFF,0xFF,0x00},//11
  {0x00,0x00,0xFF},//12
  {0xFF,0x00,0xFF},//13
  {0x00,0xFF,0xFF},//14
  {0x99,0x00,0x00},//15
  {0x00,0x00,0x00},//16
  {0x33,0x00,0x00},//17
  {0x66,0x00,0x00},//18
  {0x99,0x00,0x00},//19
  {0xCC,0x00,0x00},//20
  {0xFF,0x00,0x00},//21
  {0x00,0x33,0x00},//22
  {0x33,0x33,0x00},//23
  {0x66,0x33,0x00},//24
  {0x99,0x33,0x00},//25
  {0xCC,0x33,0x00},//26
  {0xFF,0x33,0x00},//27
  {0x00,0x66,0x00},//28
  {0x33,0x66,0x00},//29
  {0x66,0x66,0x00},//30
  {0x99,0x66,0x00},//31
  {0xCC,0x66,0x00},//32
  {0xFF,0x66,0x00},//33
  {0x00,0x99,0x00},//34
  {0x33,0x99,0x00},//35
  {0x66,0x99,0x00},//36
  {0x99,0x99,0x00},//37
  {0xCC,0x99,0x00},//38
  {0xFF,0x99,0x00},//39
  {0x00,0xCC,0x00},//40
  {0x33,0xCC,0x00},//41
  {0x66,0xCC,0x00},//42
  {0x99,0xCC,0x00},//43
  {0xCC,0xCC,0x00},//44
  {0xFF,0xCC,0x00},//45
  {0x00,0xFF,0x00},//46
  {0x33,0xFF,0x00},//47
  {0x66,0xFF,0x00},//48
  {0x99,0xFF,0x00},//49
  {0xCC,0xFF,0x00},//50
  {0xFF,0xFF,0x00},//51
  {0x00,0x00,0x33},//52
  {0x33,0x00,0x33},//53
  {0x66,0x00,0x33},//54
  {0x99,0x00,0x33},//55
  {0xCC,0x00,0x33},//56
  {0xFF,0x00,0x33},//57
  {0x00,0x33,0x33},//58
  {0x33,0x33,0x33},//59
  {0x66,0x33,0x33},//60
  {0x99,0x33,0x33},//61
  {0xCC,0x33,0x33},//62
  {0xFF,0x33,0x33},//63
  {0x00,0x66,0x33},//64
  {0x33,0x66,0x33},//65
  {0x66,0x66,0x33},//66
  {0x99,0x66,0x33},//67
  {0xCC,0x66,0x33},//68
  {0xFF,0x66,0x33},//69
  {0x00,0x99,0x33},//70
  {0x33,0x99,0x33},//71
  {0x66,0x99,0x33},//72
  {0x99,0x99,0x33},//73
  {0xCC,0x99,0x33},//74
  {0xFF,0x99,0x33},//75
  {0x00,0xCC,0x33},//76
  {0x33,0xCC,0x33},//77
  {0x66,0xCC,0x33},//78
  {0x99,0xCC,0x33},//79
  {0xCC,0xCC,0x33},//80
  {0xFF,0xCC,0x33},//81
  {0x00,0xFF,0x33},//82
  {0x33,0xFF,0x33},//83
  {0x66,0xFF,0x33},//84
  {0x99,0xFF,0x33},//85
  {0xCC,0xFF,0x33},//86
  {0xFF,0xFF,0x33},//87
  {0x00,0x00,0x66},//88
  {0x33,0x00,0x66},//89
  {0x66,0x00,0x66},//90
  {0x99,0x00,0x66},//91
  {0xCC,0x00,0x66},//92
  {0xFF,0x00,0x66},//93
  {0x00,0x33,0x66},//94
  {0x33,0x33,0x66},//95
  {0x66,0x33,0x66},//96
  {0x99,0x33,0x66},//97
  {0xCC,0x33,0x66},//98
  {0xFF,0x33,0x66},//99
  {0x00,0x66,0x66},//100
  {0x33,0x66,0x66},//101
  {0x66,0x66,0x66},//102
  {0x99,0x66,0x66},//103
  {0xCC,0x66,0x66},//104
  {0xFF,0x66,0x66},//105
  {0x00,0x99,0x66},//106
  {0x33,0x99,0x66},//107
  {0x66,0x99,0x66},//108
  {0x99,0x99,0x66},//109
  {0xCC,0x99,0x66},//110
  {0xFF,0x99,0x66},//111
  {0x00,0xCC,0x66},//112
  {0x33,0xCC,0x66},//113
  {0x66,0xCC,0x66},//114
  {0x99,0xCC,0x66},//115
  {0xCC,0xCC,0x66},//116
  {0xFF,0xCC,0x66},//117
  {0x00,0xFF,0x66},//118
  {0x33,0xFF,0x66},//119
  {0x66,0xFF,0x66},//120
  {0x99,0xFF,0x66},//121
  {0xCC,0xFF,0x66},//122
  {0xFF,0xFF,0x66},//123
  {0x00,0x00,0x99},//124
  {0x33,0x00,0x99},//125
  {0x66,0x00,0x99},//126
  {0x99,0x00,0x99},//127
  {0xCC,0x00,0x99},//128
  {0xFF,0x00,0x99},//129
  {0x00,0x33,0x99},//130
  {0x33,0x33,0x99},//131
  {0x66,0x33,0x99},//132
  {0x99,0x33,0x99},//133
  {0xCC,0x33,0x99},//134
  {0xFF,0x33,0x99},//135
  {0x00,0x66,0x99},//136
  {0x33,0x66,0x99},//137
  {0x66,0x66,0x99},//138
  {0x99,0x66,0x99},//139
  {0xCC,0x66,0x99},//140
  {0xFF,0x66,0x99},//141
  {0x00,0x99,0x99},//142
  {0x33,0x99,0x99},//143
  {0x66,0x99,0x99},//144
  {0x99,0x99,0x99},//145
  {0xCC,0x99,0x99},//146
  {0xFF,0x99,0x99},//147
  {0x00,0xCC,0x99},//148
  {0x33,0xCC,0x99},//149
  {0x66,0xCC,0x99},//150
  {0x99,0xCC,0x99},//151
  {0xCC,0xCC,0x99},//152
  {0xFF,0xCC,0x99},//153
  {0x00,0xFF,0x99},//154
  {0x33,0xFF,0x99},//155
  {0x66,0xFF,0x99},//156
  {0x99,0xFF,0x99},//157
  {0xCC,0xFF,0x99},//158
  {0xFF,0xFF,0x99},//159
  {0x00,0x00,0xCC},//160
  {0x33,0x00,0xCC},//161
  {0x66,0x00,0xCC},//162
  {0x99,0x00,0xCC},//163
  {0xCC,0x00,0xCC},//164
  {0xFF,0x00,0xCC},//165
  {0x00,0x33,0xCC},//166
  {0x33,0x33,0xCC},//167
  {0x66,0x33,0xCC},//168
  {0x99,0x33,0xCC},//169
  {0xCC,0x33,0xCC},//170
  {0xFF,0x33,0xCC},//171
  {0x00,0x66,0xCC},//172
  {0x33,0x66,0xCC},//173
  {0x66,0x66,0xCC},//174
  {0x99,0x66,0xCC},//175
  {0xCC,0x66,0xCC},//176
  {0xFF,0x66,0xCC},//177
  {0x00,0x99,0xCC},//178
  {0x33,0x99,0xCC},//179
  {0x66,0x99,0xCC},//180
  {0x99,0x99,0xCC},//181
  {0xCC,0x99,0xCC},//182
  {0xFF,0x99,0xCC},//183
  {0x00,0xCC,0xCC},//184
  {0x33,0xCC,0xCC},//185
  {0x66,0xCC,0xCC},//186
  {0x99,0xCC,0xCC},//187
  {0xCC,0xCC,0xCC},//188
  {0xFF,0xCC,0xCC},//189
  {0x00,0xFF,0xCC},//190
  {0x33,0xFF,0xCC},//191
  {0x66,0xFF,0xCC},//192
  {0x99,0xFF,0xCC},//193
  {0xCC,0xFF,0xCC},//194
  {0xFF,0xFF,0xCC},//195
  {0x00,0x00,0xFF},//196
  {0x33,0x00,0xFF},//197
  {0x66,0x00,0xFF},//198
  {0x99,0x00,0xFF},//199
  {0xCC,0x00,0xFF},//200
  {0xFF,0x00,0xFF},//201
  {0x00,0x33,0xFF},//202
  {0x33,0x33,0xFF},//203
  {0x66,0x33,0xFF},//204
  {0x99,0x33,0xFF},//205
  {0xCC,0x33,0xFF},//206
  {0xFF,0x33,0xFF},//207
  {0x00,0x66,0xFF},//208
  {0x33,0x66,0xFF},//209
  {0x66,0x66,0xFF},//210
  {0x99,0x66,0xFF},//211
  {0xCC,0x66,0xFF},//212
  {0xFF,0x66,0xFF},//213
  {0x00,0x99,0xFF},//214
  {0x33,0x99,0xFF},//215
  {0x66,0x99,0xFF},//216
  {0x99,0x99,0xFF},//217
  {0xCC,0x99,0xFF},//218
  {0xFF,0x99,0xFF},//219
  {0x00,0xCC,0xFF},//220
  {0x33,0xCC,0xFF},//221
  {0x66,0xCC,0xFF},//222
  {0x99,0xCC,0xFF},//223
  {0xCC,0xCC,0xFF},//224
  {0xFF,0xCC,0xFF},//225
  {0x00,0xFF,0xFF},//226
  {0x33,0xFF,0xFF},//227
  {0x66,0xFF,0xFF},//228
  {0x99,0xFF,0xFF},//229
  {0xCC,0xFF,0xFF},//230
  {0xFF,0xFF,0xFF},//231
  //Proprietary
  {0x00,0x00,0x00},//232
  {0x00,0x00,0x00},//233
  {0x00,0x00,0x00},//234
  {0x00,0x00,0x00},//235
  {0x00,0x00,0x00},//236
  {0x00,0x00,0x00},//237
  {0x00,0x00,0x00},//238
  {0x00,0x00,0x00},//239
  {0x00,0x00,0x00},//240
  {0x00,0x00,0x00},//241
  {0x00,0x00,0x00},//242
  {0x00,0x00,0x00},//243
  {0x00,0x00,0x00},//244
  {0x00,0x00,0x00},//245
  {0x00,0x00,0x00},//246
  {0x00,0x00,0x00},//247
  {0x00,0x00,0x00},//248
  {0x00,0x00,0x00},//249
  {0x00,0x00,0x00},//250
  {0x00,0x00,0x00},//251
  {0x00,0x00,0x00},//252
  {0x00,0x00,0x00},//253
  {0x00,0x00,0x00},//254
  {0x00,0x00,0x00} //255
};

/**** CHOOSE YOU IMAGE_PROCESSOR *****/
/** set it here or in the makefile! **/
// #define USE_FREE_IMAGE_LIB
// #define USE_PAINTLIB

#define MACRO_16bitToLE(value) (value & 0xFF), ((value >> 8) & 0xFF)
#define MACRO_32bitToLE(value) (value & 0xFF), ((value >> 8) & 0xFF), ((value >> 16) & 0xFF), ((value >> 24) & 0xFF)

// ### GLOBALS ###
char iso639table [DEF_iso639entries][2+1] = {{"aa"},{"ab"},{"af"},{"am"},{"ar"},{"as"},{"ay"},{"az"},{"ba"},{"be"},{"bg"},{"bh"},{"bi"},{"bn"},{"bo"},{"br"},{"ca"},{"co"},{"cs"},{"cy"},{"da"},{"de"},{"dz"},{"el"},{"en"},{"eo"},{"es"},{"et"},{"eu"},{"fa"},{"fi"},{"fj"},{"fo"},{"fr"},{"fy"},{"ga"},{"gd"},{"gl"},{"gn"},{"gu"},{"ha"},{"hi"},{"hr"},{"hu"},{"hy"},{"ia"},{"ie"},{"ik"},{"in"},{"is"},{"it"},{"iw"},{"ja"},{"ji"},{"jw"},{"ka"},{"kk"},{"kl"},{"km"},{"kn"},{"ko"},{"ks"},{"ku"},{"ky"},{"la"},{"ln"},{"lo"},{"lt"},{"lv"},{"mg"},{"mi"},{"mk"},{"ml"},{"mn"},{"mo"},{"mr"},{"ms"},{"mt"},{"my"},{"na"},{"ne"},{"nl"},{"no"},{"oc"},{"om"},{"or"},{"pa"},{"pl"},{"ps"},{"pt"},{"qu"},{"rm"},{"rn"},{"ro"},{"ru"},{"rw"},{"sa"},{"sd"},{"sg"},{"sh"},{"si"},{"sk"},{"sl"},{"sm"},{"sn"},{"so"},{"sq"},{"sr"},{"ss"},{"st"},{"su"},{"sv"},{"sw"},{"ta"},{"te"},{"tg"},{"th"},{"ti"},{"tk"},{"tl"},{"tn"},{"to"},{"tr"},{"ts"},{"tt"},{"tw"},{"uk"},{"ur"},{"uz"},{"vi"},{"vo"},{"wo"},{"xh"},{"yo"},{"zh"},{"zu"}};

#define X(str) XStr(str).unicodeForm()

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
    //  fUnicodeForm
    //   This is the Unicode XMLCh format of the string.
    // -----------------------------------------------------------------------
    XMLCh*   fUnicodeForm;
};

DOMCountErrorHandler::DOMCountErrorHandler()
  : fSawErrors(false) {}

DOMCountErrorHandler::~DOMCountErrorHandler() {}

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


// ---------------------------------------------------------------------------
//  GLOBAL Bitmap instance
// ---------------------------------------------------------------------------
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
  std::cout << "\nvt2iso BUILD DATE: 03-Aug-2007\n\n"
    "Usage:\n"
    " vt2iso [options] <XML file>\n\n"
    "This program invokes the DOMBuilder, builds the DOM tree,\n"
    "and then converts the tree to ISO Virtual Terminal cpp-files.\n\n"
    "Input files are all files starting with <XML file>,\nsorted by alphabetical order.\n"
    "This has been done to give the possibility to split \nlarge XML files into several ones.\n\n"
    "Features:\n"
    " - auto_language=\"..\" attribute\n"
    " - support for auto-detecting language=\"..\" attribute if the value is given in the .values.xx.txt files.\n\n"
    "Options:\n"
    " -v=xxx Validation scheme [always | never | auto]. Defaults to auto\n"
    " -n     Enable namespace processing. Defaults to off.\n"
    " -s     Enable schema processing. Defaults to off.\n"
    " -f     Enable full schema constraint checking. Defaults to off.\n"
    " -locale=ll_CC  specify the locale. Defaults to en_US.\n"
    " -p     Output ISO11783-VT Palette to act-file.\n"
    " -e     Externalize. If you need to use the split-up version of the generated files, use this option.\n";

#ifdef USE_SPECIAL_PARSING
    std::cout <<
    " -i=xxx Specify a unique identfication which will be used as prefix for every object in the pool. (max. 8 char\n"
    " -dict=xxx Specify the relative path to the resource dictionary\n";
#endif
  std::cout << " -?    Show this help.\n\n" << std::endl;
}

bool vt2iso_c::sb_WSFound = false;

void
vt2iso_c::clean_exit (char* error_message)
{
  FILE* partFile_direct = NULL;
  char partFileName [1024+1]; partFileName [1024+1-1] = 0x00;

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

  // close all streams to files at the end of this function because someone may want
  // to write into these files in special parsing

  if (partFile_functions) {
    fprintf (partFile_functions, "\n  #include \"%s-functions-origin.inc\"\n", xmlFileWithoutPath);
    fprintf (partFile_functions, "\n  b_initAllObjects = true;");
    fprintf (partFile_functions, "\n}\n");
  }

  if (partFile_functions_origin) {
    fprintf (partFile_functions_origin, "\n  for (int i=0;i<numObjects; i++) {");
    fprintf (partFile_functions_origin, "\n    iVtObjects [0][i]->setOriginSKM (false);");
    fprintf (partFile_functions_origin, "\n    iVtObjects [0][i]->setOriginBTN (NULL);");
    fprintf (partFile_functions_origin, "\n  }\n");
  }

  if (partFile_defines) {
    fprintf (partFile_defines, "\n#define vtKeyCodeACK 0\n");
    // OLD:  fprintf (partFile_defines, "\n#define vtObjectCount %d\n", objCount);
  }

  std::map<int32_t, ObjListEntry>::iterator mit_lang;
  std::map<uint16_t, std::string>::iterator mit_obj;

  FILE* fileList;
  bool *pb_firstLine;

  for (mit_lang = map_objNameAndID.begin(); mit_lang != map_objNameAndID.end(); mit_lang++)
  {
    if (mit_lang->first < 0x0) // negative index was defined for default objects
    {
      fileList = partFile_list;
      pb_firstLine = &firstLineFileE;

      // get workingset object and set as first item in list -> needed for pool upload/initialization
      if (sb_WSFound) // should always be the case!
      {
        mit_obj = mit_lang->second.find (ui16_WSObjID);
        *pb_firstLine = false;

        // first, print out the workingset object
        fprintf (fileList, "\n %s", mit_obj->second.c_str());
        // delete from the map
        mit_lang->second.erase (mit_obj);
      }
    }
    else
    {
      fileList = arrs_language[mit_lang->first].partFile;
      pb_firstLine = &arrs_language[mit_lang->first].firstLine;
    }

    for (mit_obj = mit_lang->second.begin(); mit_obj != mit_lang->second.end(); mit_obj++)
    {
      if (*pb_firstLine) {
        *pb_firstLine = false;
      } else {
        fprintf (fileList, ",");
      }

      fprintf (fileList, "\n %s", mit_obj->second.c_str());
    }
  }

  if (partFile_list) { // -list
    if (firstLineFileE)
    {
      firstLineFileE = false;
    }
    fputs ("\n};\n", partFile_list);
    // write implementation of handler class constructor into list
    // as there the list must be known
    // -> the handler decleration can be included from everywhere
    fprintf (partFile_list, "\nIsoAgLib::iVtObject_c* HUGE_MEM * all_iVtObjectLists%s [] = {", pcch_poolIdent);
    fprintf (partFile_list, "\n  all_iVtObjects%s,", pcch_poolIdent);
    for (unsigned int i=0; i<ui_languages; i++)
      fprintf (partFile_list, "\n  all_iVtObjects%s%d,", pcch_poolIdent, i);
    fprintf (partFile_list, "\n  NULL // indicate end of list");
    fprintf (partFile_list, "\n};\n");
    int extraLanguageLists = (ui_languages>0)?arrs_language[0].count : 0;
    fprintf (partFile_list, "\niObjectPool_%s_c::iObjectPool_%s_c () : iIsoTerminalObjectPool_c (all_iVtObjectLists%s, %d, %d, %d, %d, %d) {};\n",
             proName.c_str(), proName.c_str(), pcch_poolIdent, objCount - extraLanguageLists, extraLanguageLists, opDimension, skWidth, skHeight);

    fclose(partFile_list);
  }
//   if (partFile_list_extern);
//     fclose(partFile_list_extern);


  for (unsigned int i=0; i<ui_languages; i++)
  {
    if (arrs_language [i].firstLine)
    { // put in at least one entry so that compiler doesn't error out (gcc doesn't care, but others do!)
      fputs ("\n  NULL", arrs_language [i].partFile);
    }
    fputs ("\n};\n", arrs_language [i].partFile);
    // write implementation of handler class constructor into list
    // as there the list must be known
    // -> the handler decleration can be included from everywhere

    fclose (arrs_language [i].partFile);
  }

  if (partFile_handler_direct) { // handler class direct
  // NEW:
    fprintf (partFile_handler_direct, "\n #ifndef DECL_direct_iObjectPool_%s_c", proName.c_str() );
    fprintf (partFile_handler_direct, "\n #define DECL_direct_iObjectPool_%s_c", proName.c_str() );
    fprintf (partFile_handler_direct, "\nclass iObjectPool_%s_c : public IsoAgLib::iIsoTerminalObjectPool_c {", proName.c_str());
    fprintf (partFile_handler_direct, "\npublic:");
    fprintf (partFile_handler_direct, "\n  virtual void eventKeyCode (uint8_t keyActivationCode, uint16_t objId, uint16_t objIdMask, uint8_t keyCode, bool wasButton);");
    fprintf (partFile_handler_direct, "\n  /* Uncomment the following function if you want to use command-response handling! */");
    fprintf (partFile_handler_direct, "\n  //virtual void eventPointingEvent (uint16_t rui16_xPosition, uint16_t rui16_yPosition);");
    fprintf (partFile_handler_direct, "\n  virtual void eventNumericValue (uint16_t objId, uint8_t ui8_value, uint32_t ui32_value);");
    fprintf (partFile_handler_direct, "\n  virtual void eventStringValue (uint16_t rui16_objId, uint8_t rui8_length, StreamInput_c &refc_streaminput, uint8_t rui8_unparsedBytes, bool b_isFirst, bool b_isLast);");
    fprintf (partFile_handler_direct, "\n  /* Uncomment the following function if you want to use input value string on-the-fly parsing/handling! */");
    fprintf (partFile_handler_direct, "\n  //virtual void eventStringValueAbort();");
    fprintf (partFile_handler_direct, "\n  virtual void eventObjectPoolUploadedSuccessfully (bool rb_wasLanguageUpdate, int8_t ri8_languageIndex, uint16_t rui16_languageCode);");
    fprintf (partFile_handler_direct, "\n  //virtual void eventPrepareForLanguageChange (int8_t ri8_languageIndex, uint16_t rui16_languageCode);");
    fprintf (partFile_handler_direct, "\n  virtual void eventEnterSafeState ();");
    fprintf (partFile_handler_direct, "\n  /* Uncomment the following function if you want to use command-response handling! */");
    fprintf (partFile_handler_direct, "\n  //virtual void eventCommandResponse (uint8_t rui8_responseCommandError, const uint8_t rpui8_responseDataBytes[8]);");
    fprintf (partFile_handler_direct, "\n  /* Uncomment the following function if you want to use a special colour-conversion! */");
    fprintf (partFile_handler_direct, "\n  //virtual uint8_t convertColour (uint8_t colourValue, uint8_t colourDepth, IsoAgLib::iVtObject_c* obj, IsoAgLib::e_vtColour whichColour);");
    fprintf (partFile_handler_direct, "\n  /* Uncomment the following function if you want to react on any incoming LANGUAGE_PGN */");
    fprintf (partFile_handler_direct, "\n  //virtual void eventLanguagePgn (const localSettings_s& rrefs_localSettings);");
    fprintf (partFile_handler_direct, "\n  /* Uncomment the following function if you want to react on any incoming VT Status messages */");
    fprintf (partFile_handler_direct, "\n  //virtual void eventVtStatusMsg();");
    fprintf (partFile_handler_direct, "\n  /* Uncomment the following function if you want to react on any incoming VT ESC */");
    fprintf (partFile_handler_direct, "\n  //virtual void eventVtESC();");
    fprintf (partFile_handler_direct, "\n  /* Uncomment the following function if you want to react on any incoming VT Get Attribute Value messages */");
    fprintf (partFile_handler_direct, "\n  //virtual void eventAttributeValue(IsoAgLib::iVtObject_c* obj, uint8_t ui8_attributeValue, uint8_t* pui8_value);");
    fprintf (partFile_handler_direct, "\n  void initAllObjectsOnce(SINGLETON_VEC_KEY_PARAMETER_DEF);");
    fprintf (partFile_handler_direct, "\n  iObjectPool_%s_c ();", proName.c_str());
    fprintf (partFile_handler_direct, "\n};\n");
    fprintf (partFile_handler_direct, "\n #endif\n" );
    fclose (partFile_handler_direct);
  }
  if (partFile_handler_derived) { // handler class derived
  // NEW:
    fprintf (partFile_handler_derived, "\n #ifndef DECL_derived_iObjectPool_%s_c", proName.c_str() );
    fprintf (partFile_handler_derived, "\n #define DECL_derived_iObjectPool_%s_c", proName.c_str() );
    fprintf (partFile_handler_derived, "\nclass iObjectPool_%s_c : public IsoAgLib::iIsoTerminalObjectPool_c {", proName.c_str());
    fprintf (partFile_handler_derived, "\npublic:");
    fprintf (partFile_handler_derived, "\n  void initAllObjectsOnce(SINGLETON_VEC_KEY_PARAMETER_DEF);");
    fprintf (partFile_handler_derived, "\n  iObjectPool_%s_c ();", proName.c_str());
    fprintf (partFile_handler_derived, "\n};\n");
    fprintf (partFile_handler_derived, "\n #endif\n" );
    fclose (partFile_handler_derived);
  }

  char extension [32+1];
  if (b_externalize)
    strcpy (extension, "-extern");
  else
    strcpy (extension, "");

// Write Direct Includes
  strncpy (partFileName, xmlFileGlobal, 1024);
  strcat (partFileName, "_direct.h");
  partFile_direct = fopen (partFileName,"wt");

  fprintf (partFile_direct, "#include <IsoAgLib/comm/ISO_Terminal/ivtincludes.h>\n");
  fprintf (partFile_direct, "#include \"%s-handler-direct.inc\"\n", xmlFileWithoutPath);
  fprintf (partFile_direct, "#include \"%s-variables%s.inc\"\n", xmlFileWithoutPath, extension);
  fprintf (partFile_direct, "#include \"%s-attributes%s.inc\"\n", xmlFileWithoutPath, extension);
  for (unsigned int i=0; i<ui_languages; i++)
  {
    fprintf (partFile_direct, "#include \"%s-list%02d.inc\"\n", xmlFileWithoutPath, i);
  }
  fprintf (partFile_direct, "#include \"%s-list.inc\"\n", xmlFileWithoutPath);
  fprintf (partFile_direct, "#include \"%s-defines.inc\"\n", xmlFileWithoutPath);
  fprintf (partFile_direct, "#include \"%s-functions.inc\"\n", xmlFileWithoutPath);

  if (pc_specialParsing)
  {
    pc_specialParsing->addFileIncludes(partFile_direct, xmlFileWithoutPath);
  }

  // Write Direct Includes
  strncpy (partFileName, xmlFileGlobal, 1024);
  strcat (partFileName, "_derived-cpp.h");
  partFile_direct = fopen (partFileName,"wt");

  fprintf (partFile_direct, "#include \"%s-variables%s.inc\"\n", xmlFileWithoutPath, extension);
  fprintf (partFile_direct, "#include \"%s-attributes%s.inc\"\n", xmlFileWithoutPath, extension);
  for (unsigned int i=0; i<ui_languages; i++)
  {
    fprintf (partFile_direct, "#include \"%s-list%02d.inc\"\n", xmlFileWithoutPath, i);
  }
  fprintf (partFile_direct, "#include \"%s-list.inc\"\n", xmlFileWithoutPath);
  fprintf (partFile_direct, "#include \"%s-defines.inc\"\n", xmlFileWithoutPath);
  fprintf (partFile_direct, "#include \"%s-functions%s.inc\"\n", xmlFileWithoutPath, extension);

  if (pc_specialParsing)
  {
    pc_specialParsing->addFileIncludes(partFile_direct, xmlFileWithoutPath);
  }

  fclose (partFile_direct);

// Write Direct Includes
  strncpy (partFileName, xmlFileGlobal, 1024);
  strcat (partFileName, "_derived-h.h");
  partFile_direct = fopen (partFileName,"wt");

  fprintf (partFile_direct, "#include <IsoAgLib/comm/ISO_Terminal/ivtincludes.h>\n");
  fprintf (partFile_direct, "#include \"%s-handler-derived.inc\"\n", xmlFileWithoutPath);

  /// if USE_SPECIAL_PARSING is defined additional output is done
  if (pc_specialParsing)
    pc_specialParsing->outputCollectedData2Files();

  if (partFile_direct)           fclose (partFile_direct);
  if (partFile_variables_extern) fclose (partFile_variables_extern);
  if (partFile_attributes)       fclose (partFile_attributes);
  if (partFile_attributes_extern) fclose (partFile_attributes_extern);
  if (partFile_defines)          fclose (partFile_defines);
  if (partFile_functions)        fclose (partFile_functions);
  if (partFile_functions_origin) fclose (partFile_functions_origin);

  splitFunction (true);

  if (error_message != NULL)
    std::cout << error_message;
}

signed int
vt2iso_c::strlenUnescaped (const char* pcc_string)
{
  int i_unescapedLength=0;
  char c_current;
  bool b_lastWasEscapeChar=false;
  while ((c_current=*pcc_string++) != 0x00)
  {
    if ((c_current == '\\') && (!b_lastWasEscapeChar))
    { // this is the escape character
      b_lastWasEscapeChar=true;
    }
    else
    { // count as normal character
      i_unescapedLength++;
      b_lastWasEscapeChar=false;
    }
  }

  if (b_lastWasEscapeChar)
    return -1;
  else
    return i_unescapedLength;
}

bool
vt2iso_c::copyWithQuoteAndLength (char *dest, const char *src, unsigned int len)
{
  *dest++ = '"';

  signed int ret = strlenUnescaped(src);
  if (ret == -1)
  {
    printf ("Error in copyWithQuoteAndLength()! STOP PARSER! bye.\n\n");
    return false;
  }

  unsigned int take = ( (unsigned int)ret <= len) ? ret : len;

  unsigned int i=0;
  bool b_lastWasEscapeChar=false;
  for (; i<take;)
  { // copy probably escaped string
    if ((*src == '\\') && (!b_lastWasEscapeChar))
    {
      b_lastWasEscapeChar=true;
      // do NOT increment i!
    }
    else
    {
      b_lastWasEscapeChar=false;
      i++;
    }
    *dest++ = *src++;
  }
  for (; i<len; i++) *dest++ = ' '; // fill with spaces if necessary
  *dest++ = '"';
  *dest = 0x00;

  return true;
}

// ---------------------------------------------------------------------------
//  signed long int getID (const char* objName, bool isMacro, bool widhingID, unsigned int wishID=0)
// ---------------------------------------------------------------------------
signed long int
vt2iso_c::getID (const char* objName, bool b_isMacro, bool b_wishingID, unsigned int wishID)
{
  bool isThere = false;
  unsigned int foundID = 0;

  // Added the following check. This is necessary so that objects like input lists which can contain lists of the NULL object ID (65535)
  // do not assign object ids for this, and as a result count an additional object unnecessarily by incrementing objCount.
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
    if (b_wishingID) {
      // first check if ID is there already
      for (unsigned int i=0; i<objCount; i++)
      {
        // std::cout << "comparing " << wishID << " with " << objIDTable [i] << "\n";
        if (objIDTable [i] == wishID)
        {
          if (pc_specialParsing)
            std::cout << "DOUBLE USE OF RESOURCE NAME '" << pc_specialParsing->getResourceName (wishID) << "' WITH ID '"<< (uint16_t)wishID << "' ENCOUNTERED! STOPPING PARSER! bye.\n\n";
          else
            std::cout << "DOUBLE USE OF OBJECT ID '" << wishID << "' ENCOUNTERED! STOPPING PARSER! bye.\n\n";
          return -1;
        }
      }
      // if we reach here that wishID is not assigned yet, so we can use it!
      foundID = wishID;
    } else if ( checkForAllowedExecution() ) {
      /// only auto-decrement if the current object has a basic or proprietary object type
      if (b_isMacro) {
        foundID = objNextMacroAutoID;
        objNextMacroAutoID--;
      } else {
        foundID = objNextAutoID;
        objNextAutoID--;
      }
    }

    /// only insert object if it is basic or proprietary
    if ( checkForAllowedExecution() )
    {
      // insert new name-id pair now!
      objIDTable [objCount] = foundID;
      strncpy (&objNameTable [objCount*(stringLength+1)], objName, stringLength); // so we have 0-termination in every case, as our strings are 128+1 bytes!
      //printf("newly added: objName: %s, objCount: %i\n", objName, objCount);
      objCount++;
    }
  }

  return (foundID);
}

signed long int
vt2iso_c::setID (const char* objName, unsigned int wishID)
{
  b_hasUnknownAttributes = false;

  // Added the following check. This is necessary so that objects like input lists which can contain lists of the NULL object ID (65535)
  // do not assign object ids for this, and as a result count an additional object unnecessarily by incrementing objCount.
  if (wishID == 65535)
  {
    return wishID;
  }

  // first check if object is there already
  for (unsigned int i=0; i<objCount; i++)
  {
    // std::cout << "comparing " << objName << " with " << &objNameTable [i*(stringLength+1)] << "\n";
    if (strncmp (objName, &objNameTable [i*(stringLength+1)], stringLength) == 0)
    {
      if (objIDTable [i] != wishID) // only set ID if different from wishID
        objIDTable [i] = wishID;

      return (wishID);
    }
  }

  return getID (objName, false, true, wishID);
}

signed long int
vt2iso_c::idOrName_toi(char* rpc_string, bool rb_isMacro)
{
  if (rpc_string [0] == 0x00)
  {
    std::cout << "*** ERROR *** idOrName_toi: Empty 'object_id' attribute!\n\n";
    return -1;
  }
  /** @todo check if all chars in the string are numbers, not only the first! */
  if ((rpc_string [0] >= '0') && (rpc_string [0] <= '9')) return atoi (rpc_string);

  if (strstr (rpc_string, pcch_poolIdent) != rpc_string)
  {
    // add the pool_ident to the name
    std::string fullName = std::string(pcch_poolIdent) + std::string(rpc_string);
    // Starting with a letter, so look up id!
    return getID (fullName.c_str(), rb_isMacro, false, 0);
  }
  else
    return getID (rpc_string, rb_isMacro, false, 0);
}

void
vt2iso_c::getKeyCode()
{
  attrIsGiven [attrKey_code] = true;
  sprintf (attrString [attrKey_code], "%d", kcNextAutoID);
  kcNextAutoID--;
}

void
vt2iso_c::splitFunction (bool rb_onlyClose=false)
{
  static int splitFunctionPart = 0;
  char partFileName [1024+1]; partFileName [1024+1-1] = 0x00;

  if (partFile_split_function)
  {
    fprintf (partFile_split_function, "}\n");
    fclose (partFile_split_function);
    partFile_split_function = NULL;
    splitFunctionPart++;
  }
  if (!rb_onlyClose)
  {
    sprintf (partFileName, "%s-function%d.cpp", xmlFileGlobal, splitFunctionPart);
    partFile_split_function = fopen (partFileName, "wt");
    fprintf (partFile_split_function, "#include <IsoAgLib/comm/ISO_Terminal/ivtincludes.h>\n");
    fprintf (partFile_split_function, "#include \"%s-variables-extern.inc\"\n", xmlFileGlobal);
    fprintf (partFile_split_function, "#include \"%s-variables-attributes.inc\"\n", xmlFileGlobal);
    fprintf (partFile_split_function, "void initAllObjectsOnce%d(SINGLETON_VEC_KEY_PARAMETER_DEF)\n", splitFunctionPart);
    fprintf (partFile_split_function, "{\n");
    fprintf (partFile_functions,      "  initAllObjectsOnce%d(SINGLETON_VEC_KEY_PARAMETER_VAR_WITH_COMMA);\n", splitFunctionPart);
  }
}

void
vt2iso_c::init (const char* xmlFile, std::basic_string<char>* dictionary, bool rb_externalize)
{
  b_externalize = rb_externalize;

  firstLineFileE = true;
  ui_languages=0;

  is_opDimension=false;
  is_skWidth=false;
  is_skHeight=false;

  partFile_variables = NULL;
  partFile_variables_extern = NULL;
  partFile_attributes = NULL;
  partFile_attributes_extern = NULL;
  partFile_functions = NULL;
  partFile_functions_origin = NULL;
  partFile_defines = NULL;
  partFile_list = NULL;
  partFile_handler_direct = NULL;
  partFile_handler_derived = NULL;

  strcpy (xmlFileGlobal, xmlFile);

  std_bitmap_path [0] = 0x00;
  fix_bitmap_path [0] = 0x00;
  attr_name [1024+1-1] = 0x00;
  attr_value [1024+1-1] = 0x00;
  spc_autoLanguage[0] = 0x00; // default to no autoLanguage

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

// partFile_variables = fopen ("picture.raw", "wb");
// fwrite (vtObjectdeXbitmap1_aRawBitmap, 16384, 1, partFile_variables);
// fclose (partFile_variables);

  strncpy (partFileName, xmlFile, 1024);
  strcat (partFileName, "-variables.inc");
  partFile_variables = fopen (partFileName,"wt");
  if (partFile_variables == NULL) clean_exit ("Couldn't create -variables.inc.");

  strncpy (partFileName, xmlFile, 1024);
  strcat (partFileName, "-variables-extern.inc");
  partFile_variables_extern = fopen (partFileName,"wt");
  if (partFile_variables_extern == NULL) clean_exit ("Couldn't create -variables-extern.inc.");

  strncpy (partFileName, xmlFile, 1024);
  strcat (partFileName, "-attributes.inc");
  partFile_attributes = fopen (partFileName,"wt");
  if (partFile_attributes == NULL) clean_exit ("Couldn't create -attributes.inc.");

  strncpy (partFileName, xmlFile, 1024);
  strcat (partFileName, "-attributes-extern.inc");
  partFile_attributes_extern = fopen (partFileName,"wt");
  if (partFile_attributes_extern == NULL) clean_exit ("Couldn't create -attributes_extern.inc.");

  strncpy (partFileName, xmlFile, 1024);
  strcat (partFileName, "-functions.inc");
  partFile_functions = fopen (partFileName,"wt");
  if (partFile_functions == NULL) clean_exit ("Couldn't create -functions.inc.");

  if (b_externalize)
  {
    for (int i=0; i<20; i++)
    {
      fprintf (partFile_functions, "extern void initAllObjectsOnce%d (SINGLETON_VEC_KEY_PARAMETER_DEF);\n", i);
    }
  }
  fprintf (partFile_functions, "void iObjectPool_%s_c::initAllObjectsOnce (SINGLETON_VEC_KEY_PARAMETER_DEF)\n{\n", proName.c_str());
  fprintf (partFile_functions, "  if (b_initAllObjects) return;   // so the pointer to the ROM structures are only getting set once on initialization!\n");

  strncpy (partFileName, xmlFile, 1024);
  strcat (partFileName, "-functions-origin.inc");
  partFile_functions_origin = fopen (partFileName,"wt");
  if (partFile_functions_origin == NULL) clean_exit ("Couldn't create -functions-origin.inc.");

  strncpy (partFileName, xmlFile, 1024);
  strcat (partFileName, "-defines.inc");
  partFile_defines = fopen (partFileName,"wt");
  if (partFile_defines == NULL) clean_exit ("Couldn't create -defines.inc.");

  strncpy (partFileName, xmlFile, 1024);
  strcat (partFileName, "-list.inc");
  partFile_list = fopen (partFileName,"wt");
  fprintf (partFile_list, "IsoAgLib::iVtObject_c* HUGE_MEM all_iVtObjects%s [] = {", pcch_poolIdent);
  if (partFile_list == NULL) clean_exit ("Couldn't create -list.inc.");

  strncpy (partFileName, xmlFile, 1024);
  strcat (partFileName, "-handler-direct.inc");
  // check if "-hanlder-direct" is there, in this case generate "-handler-direct.inc-template" !
  partFile_handler_direct = fopen (partFileName,"rb");
  if (partFile_handler_direct) {
   // could open file, so it exists --> don't overwrite - create "-template" then
    fclose (partFile_handler_direct);
    strcat (partFileName, "-template");
  } else {
    // file couldn't be opened, so create it, simply write to it...
  }
  partFile_handler_direct = fopen (partFileName,"wt");
  if (partFile_handler_direct == NULL) clean_exit ("Couldn't create -handler_direct.inc.");

  strncpy (partFileName, xmlFile, 1024);
  strcat (partFileName, "-handler-derived.inc");
  partFile_handler_derived = fopen (partFileName,"wt");
  if (partFile_handler_derived == NULL) clean_exit ("Couldn't create -handler_derived.inc.");

  for (int j=0; j<maxAttributeNames; j++)
    attrString [j] [stringLength+1-1] = 0x00;

  FILE* partFileTmp;

  if (b_externalize)
  {
    strncpy (partFileName, xmlFile, 1024);
    strcat (partFileName, "-variables.cpp");
    partFileTmp = fopen (partFileName, "wt");
    fprintf (partFileTmp, "#include <IsoAgLib/comm/ISO_Terminal/ivtincludes.h>\n");
    fprintf (partFileTmp, "#include \"%s-variables.inc\"\n", xmlFile);
    fclose (partFileTmp);

    strncpy (partFileName, xmlFile, 1024);
    strcat (partFileName, "-attributes.cpp");
    partFileTmp = fopen (partFileName, "wt");
    fprintf (partFileTmp, "#include <IsoAgLib/comm/ISO_Terminal/ivtincludes.h>\n");
    fprintf (partFileTmp, "#include \"%s-variables-extern.inc\"\n", xmlFile);
    fprintf (partFileTmp, "#include \"%s-attributes-extern.inc\"\n", xmlFile);
    fprintf (partFileTmp, "#include \"%s-attributes.inc\"\n", xmlFile);
    fclose (partFileTmp);

    strncpy (partFileName, xmlFile, 1024);
    strcat (partFileName, "-list.cpp");
    partFileTmp = fopen (partFileName, "wt");
    fprintf (partFileTmp, "#include <IsoAgLib/comm/ISO_Terminal/ivtincludes.h>\n");
    fprintf (partFileTmp, "#include \"%s-list.inc\"\n", xmlFile);
    fclose (partFileTmp);
  }

#ifdef USE_SPECIAL_PARSING_PROP
  pc_specialParsingPropTag = new SpecialParsingUsePropTag_c (xmlFile,
      partFile_variables,
      partFile_variables_extern,
      partFile_attributes,
      partFile_functions,
      partFile_defines
                                                            );
#else
  pc_specialParsingPropTag = NULL;
#endif

#ifdef USE_SPECIAL_PARSING
  pc_specialParsing = new SpecialParsingUse_c (xmlFile, c_directory, dictionary, pcch_poolIdent);
#else
  pc_specialParsing = NULL;
#endif
}

void
vt2iso_c::defaultAttributes (unsigned int r_objType)
{
  if (r_objType != otGraphicsContext)
  {
    if (!attrIsGiven [attrBackground_colour]) {
      sprintf (attrString [attrBackground_colour], "white");
      attrIsGiven [attrBackground_colour] = true;
    }
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
  if ((r_objType == otButton) && !attrIsGiven [attrOptions]) {
    sprintf (attrString [attrOptions], "0");
    attrIsGiven [attrOptions] = true;
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

  // may the variable reference is given as 65535 (0xFFFF)
  if (attrIsGiven [attrVariable_reference] && (atoi (attrString [attrVariable_reference]) == 0xFFFF)) {
    sprintf (attrString [attrVariable_reference], "NULL");
  }
  if (attrIsGiven [attrTarget_value_variable_reference] && (atoi (attrString [attrTarget_value_variable_reference]) == 0xFFFF)) {
    sprintf (attrString [attrTarget_value_variable_reference], "NULL");
  }
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
  if ((r_objType == otStringvariable) || (r_objType == otOutputstring))
  {
    if (!attrIsGiven [attrLanguage] && (strlen (spc_autoLanguage) >0)) {
      sprintf (attrString [attrLanguage], spc_autoLanguage);
      attrIsGiven [attrLanguage] = true;
    }
  }
  if (r_objType == otGraphicsContext)
  {
    if (!attrIsGiven [attrCursorX]) {
      sprintf (attrString [attrCursorX], "0");
      attrIsGiven [attrCursorX] = true;
    }
    if (!attrIsGiven [attrCursorY]) {
      sprintf (attrString [attrCursorY], "0");
      attrIsGiven [attrCursorY] = true;
    }
  }
  if ((r_objType == otInputnumber) && !attrIsGiven [attrInputObjectOptions]) {
    sprintf (attrString [attrInputObjectOptions], "enabled");
    attrIsGiven [attrInputObjectOptions] = true;
  }
  if (!attrIsGiven [attrScale]) {
    sprintf (attrString [attrScale], "1");
    attrIsGiven [attrScale] = true;
  }
}

void
vt2iso_c::convertIdReferenceToNameReference(int ri_attrType)
{
  if (attrIsGiven [ri_attrType])
  {
    int len = strlen (attrString [ri_attrType]);
    if (len == 0) return; // do nothing if string empty

    char firstChar = attrString [ri_attrType][0];
    if (firstChar >= '0' && firstChar <= '9')
    { // ID is given, so lookup the name for it!
      int uid = atoi (attrString [ri_attrType]);
      // first check if ID has an associated name
      for (unsigned int i=0; i<objCount; i++)
      {
        if (int(objIDTable[i]) == uid)
          strcpy (attrString [ri_attrType], &objNameTable [i*(stringLength+1)]);
      }
    }
  }
}

void
vt2iso_c::convertIdReferencesToNameReferences()
{
  convertIdReferenceToNameReference (attrActive_mask);
  convertIdReferenceToNameReference (attrSoft_key_mask);
  convertIdReferenceToNameReference (attrVariable_reference);
  convertIdReferenceToNameReference (attrFont_attributes);
  convertIdReferenceToNameReference (attrInput_attributes);
  convertIdReferenceToNameReference (attrLine_attributes);
  convertIdReferenceToNameReference (attrFill_attributes);
  convertIdReferenceToNameReference (attrTarget_value_variable_reference);
}

int
vt2iso_c::languageCodeToIndex (char* lc)
{
  for (unsigned int index = 0; index<ui_languages; index++)
  {
    if ( (arrs_language[index].code [0] == lc[0])
          && (arrs_language[index].code [1] == lc[1])
       )
    {
      return index;
    }
  }
  return -1; // negative number to indicate language code not found
}

/* sets the passed attribute if name matches id */
void
vt2iso_c::setAttributeValue(int attrID)
{
  if (strncmp (attr_name, attrNameTable[attrID], stringLength) == 0)
  {
    strncpy (attrString [attrID], attr_value, stringLength);
    attrIsGiven [attrID] = true;
  }
}

/* cleans the passed attribute value */
void
vt2iso_c::cleanAttribute(int attrID)
{
  attrString [attrID] [stringLength+1-1] = 0x00;
  attrIsGiven [attrID] = false;
}

bool
vt2iso_c::checkForFileOrFile148 (char *tag)
{
  char errMsg[1024+1]; errMsg[0] = 0x00;
  if (!attrIsGiven [attrFile])
  {
    switch (colordepthtoi (attrString [attrFormat]))
    {
      case 0: if (!(attrIsGiven [attrFile0]))
        sprintf (errMsg, "YOU NEED TO SPECIFY THE file= OR THE file0= ATTRIBUTE FOR THE <%s> OBJECT! STOPPING PARSER! bye.\n\n", tag);
        break;
      case 1: if (!(attrIsGiven [attrFile0] && attrIsGiven [attrFile1]))
        sprintf (errMsg, "YOU NEED TO SPECIFY THE file= OR ALL fileX= (x=0,1) ATTRIBUTES FOR THE <%s> OBJECT! STOPPING PARSER! bye.\n\n", tag);
        break;
      case 2: if (!(attrIsGiven [attrFile0] && attrIsGiven [attrFile2]))
        sprintf (errMsg, "YOU NEED TO SPECIFY THE file= OR MINIMUM fileX= (x=0,2) ATTRIBUTES FOR THE <%s> OBJECT! STOPPING PARSER! bye.\n\n", tag);
        break;
    }
    if (strlen(errMsg))
    {
      std::cout <<  errMsg;
      return false;
    }
  }
  return true;
}

DOMNamedNodeMap*
vt2iso_c::patched_getAttributes(DOMNode *n)
{
  char local_attrName [1024+1];
  char local_attrValue[1024+1];
  DOMNamedNodeMap *pAttributes = n->getAttributes();

  if (n->hasAttributes()) { // parse through all attributes
    int nSize = pAttributes->getLength();

    for (int i=0;i<nSize;++i) {
      DOMAttr *pAttributeNode = (DOMAttr*) pAttributes->item(i);
      utf16convert ((char *)pAttributeNode->getName(), local_attrName, 1024);
      utf16convert ((char *)pAttributeNode->getValue(), local_attrValue, 1024);

      if (strncmp (local_attrName, "latchable", stringLength) == 0)
      {
        // remove in any case, if latchable="no" removing and no addition is fine...
        ((DOMElement *)n)->removeAttributeNode (pAttributeNode);
        if ( (strncmp (local_attrValue, "yes", stringLength) == 0)
          || (strncmp (local_attrValue, "true", stringLength) == 0)
          || (strncmp (local_attrValue, "on", stringLength) == 0)
          || (strncmp (local_attrValue, "show", stringLength) == 0)
          || (strncmp (local_attrValue, "enable", stringLength) == 0)
           )
        {
          std::cout << "*** WARNING: Replaced latchable='"<<local_attrValue<<"' by the new options='latchable'. Please adapt your XML files... ***"<<std::endl;
          sprintf (local_attrValue, "latchable");
          ((DOMElement *)n)->setAttribute (X("options"), X(local_attrValue));
        }
        break;
      }
    }
  }
  return pAttributes;
}


bool
vt2iso_c::getAttributesFromNode(DOMNode *n, bool treatSpecial)
{
  DOMNamedNodeMap *pAttributes;
  if (n->hasAttributes()) { // parse through all attributes
    pAttributes = patched_getAttributes(n);
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
          sprintf (objName, "%s%s", pcch_poolIdent, attr_value);
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
        if (strncmp (attr_name, "soft_key_mask", stringLength) == 0) {
          sprintf (attrString [attrSoft_key_mask], "%s%s", pcch_poolIdent, attr_value);
          attrIsGiven [attrSoft_key_mask] = true;
          break;
        }
        else if (strncmp (attr_name, "active_mask", stringLength) == 0) {
          sprintf (attrString [attrActive_mask], "%s%s", pcch_poolIdent, attr_value);
          attrIsGiven [attrActive_mask] = true;
          break;
        }

        if (strncmp (attr_name, attrNameTable [l], stringLength) == 0) {
          strncpy (attrString [l], attr_value, stringLength);
          attrIsGiven [l] = true;
    // DEBUG-OUT
//          std::cout << "FOUND ATTR: IND " << l << ":= " << attrNameTable [l] << " -> " << attrString[l] << ":"
//                    << attrIsGiven [l] << "\n";
          break;
        }
      }

      /// if USE_SPECIAL_PARSING is NOT defined, output an error warning and stop here, if attribute is unknown
      // ERROR: We didn't match a possible attribute name
      if ((pc_specialParsingPropTag || pc_specialParsing) && (l == maxAttributeNames))
      {
        /// set a flag to sign if any attribute is unknown
        if (pc_specialParsing)
          pc_specialParsing->setUnknownAttributes (true);

        if (pc_specialParsingPropTag)
          pc_specialParsingPropTag->setUnknownAttributes (true);

        b_hasUnknownAttributes = true;
      }
      else if (l == maxAttributeNames)
      {
        std::cout << "\n\nUNKNOWN ATTRIBUTE " << attr_name << "=" << attr_value << " IN TAG <" << XMLString::transcode(n->getNodeName())
                  << "> ENCOUNTERED! STOPPING PARSER! bye.\n\n";
        return false;
      }
    }
  }

  if (treatSpecial) {
    // If no 'name=' given, add 'name=Unnamed%d' attribute
    if (is_objName == false)
    {
      sprintf (objName, "%sUnnamed%d", pcch_poolIdent, objNextUnnamedName);
      ((DOMElement *)n)->setAttribute (X("name"), X(objName));
      objNextUnnamedName++;
      is_objName = true;
    }
  }
  return true;
}

bool
vt2iso_c::openDecodePrintOut (const char* workDir, char* _bitmap_path, unsigned int &options, int fixNr)
{
  //std::cout << "Bitmappath: " << _bitmap_path << std::endl;
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

    if (fixNr == -1) fprintf (partFile_attributes, "const HUGE_MEM uint8_t iVtObject%s_aRawBitmap%d [] = {", objName, actDepth);
    else /* -fix- */ fprintf (partFile_attributes, "const HUGE_MEM uint8_t iVtObject%s_aRawBitmap%dFixed%d [] = {", objName, actDepth, fixNr);

    if (attrIsGiven [attrFile0+actDepth]) sprintf (filename, "%s%s/%s", workDir, _bitmap_path, attrString [attrFile0+actDepth]);
    else /* use std file for all depth */ sprintf (filename, "%s%s/%s", workDir, _bitmap_path, attrString [attrFile]);

    // Open Bitmap
    std::cout << std::endl; // Opening text is printed out by openBitmap
    if ( c_Bitmap.openBitmap( filename ) ) std::cout << "Loaded successfully! ";
    else
    {
      std::cout << "Loading failed!\n";
      return false;
    }

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

    if (c_Bitmap.objRawBitmapBytes [actDepth] == 0)
    {
      std::cout << "===> Bitmap with size 0. Terminating!\n\n";
      return false;
    }

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
        std::cout << objName << "' actDepth012='" << actDepth
            << "' has RLE size of " << rleBytes << " but uncompressed size of "
            << c_Bitmap.objRawBitmapBytes [actDepth] << ". Turning RLE off!"<<std::endl;
      }
    }

    static bool b_largeObject = false;
    bool b_oldLarge = b_largeObject;
    // Print out raw data or rle_data?
    if (options & (uint64_t(1)<<(2+actDepth))) { // rle1, rle4 or rle8 "STILL" set?
      //// +++ RLE-COMPRESSED OUTPUT +++
      unsigned int offset=0;
      int rleBytes=0;
      int PixelCount = 0;
      while (offset < c_Bitmap.objRawBitmapBytes [actDepth]) {
        unsigned int cnt=1;
        while (((offset+cnt) < c_Bitmap.objRawBitmapBytes [actDepth]) && (picBuffer [offset] == picBuffer [offset+cnt]) && (cnt < 255)) cnt++;
        if (offset == 0) fprintf (partFile_attributes, " /* RLE-Encoded Raw-Bitmap Data */ %d, %d", cnt, picBuffer [offset]);
        else /* ----- */ fprintf (partFile_attributes, ",%d, %d", cnt, picBuffer [offset]);
        offset += cnt;
        rleBytes += 2;
        PixelCount += cnt;
      }
      std::cout << objName << "' actDepth012='" << actDepth
          << "' has RLE size of " << rleBytes << " but uncompressed size of "
          << c_Bitmap.objRawBitmapBytes [actDepth]
          << "#Pixels: " << PixelCount << ", Width: " << c_Bitmap.getWidth() << ", Height: "
          << c_Bitmap.getHeight() << std::endl;
      c_Bitmap.objRawBitmapBytes [actDepth] = rleBytes;
      if (rleBytes > 65000) b_largeObject = true; // normally 65535-restAttributesSize would be enough, but don't care, go save!
    } else {
      //// +++ NORMAL UNCOMPRESSED OUTPUT +++
      if (actDepth == 1) { // nur der lesbarkeit halber!!
        fprintf (partFile_attributes, " /* 16-Color Raw-Bitmap Data */ (%d << 4) | %d", picBuffer [0] >> 4, picBuffer [0] & 0xF);
        for (unsigned int i=1; i<c_Bitmap.objRawBitmapBytes [actDepth]; i++) {
          fprintf (partFile_attributes, ", (%d << 4) | %d", picBuffer [i] >> 4, picBuffer [i] & 0xF);
        }
      } else {
        fprintf (partFile_attributes, " /* Raw-Bitmap Data */ %d", picBuffer [0]);
        for (unsigned int i=1; i<c_Bitmap.objRawBitmapBytes [actDepth]; i++) {
          fprintf (partFile_attributes, ", %d", picBuffer [i]);
        }
      }
      if (c_Bitmap.objRawBitmapBytes [actDepth] > 65000) b_largeObject = true; // normally 65535-restAttributesSize would be enough, but don't care, go save!
    }
    fprintf (partFile_attributes, "};\n");
    if (!b_oldLarge && b_largeObject)
    {
      fprintf (partFile_attributes, "#ifndef USE_OBJECTS_LARGER_THAN_64K\n");
      fprintf (partFile_attributes, "  #error \"Your objectpool has large object(s) so you need to compile your project with the   USE_OBJECTS_LARGER_THAN_64K   define set.\"\n");
      fprintf (partFile_attributes, "#endif\n");
    }
  } // for actDepth...

  return true;
}

bool
vt2iso_c::checkForAllowedExecution() const
{
  // no module is wished
  if (!pc_specialParsing && !pc_specialParsingPropTag)
    return true;

  // if parsing of proprietary tags/attributes is wished, check here for correct proprietary type
  if (pc_specialParsingPropTag && pc_specialParsingPropTag->checkForProprietaryOrBasicObjTypes())
    return true;

  // if parsing for lookup tables is wished, check here for correct additional types
  if (pc_specialParsing && pc_specialParsing->checkForProprietaryOrBasicObjTypes (objType))
    return true;

  return false;
}

void
vt2iso_c::autoDetectLanguage (DOMNode *n)
{
  static char newLanguageValue[136*2+1];
  newLanguageValue[0] = 0x00;

  static char searchName[1024+1];
  strcpy (searchName, getAttributeValue (n, "name"));

  if (searchName[0] == 0x00)
  { // no name, so we can't search for the
    strcpy (searchName, getAttributeValue (n, "id"));
  }

  if (searchName[0] == 0x00)
  { // no id, so we can't search for the value...
    return;
  }

  if (getAttributeValue (n, "language")[0] != 0x00)
  { // language found, no need to auto-detection
#ifdef DEBUG_LANGUAGE_AUTO_DETECT
    std::cout << "AUTO-DETECT-LANGUAGE: language=\""<< getAttributeValue (n, "language") << "\" already specified for ["<< searchName <<"].\n";
#endif
    return;
  }

// #ifdef DEBUG_LANGUAGE_AUTO_DETECT
//   std::cout << "AUTO-DETECT-LANGUAGE: searching language file for ["<<searchName<<"]... ";
// #endif

  // language not explicitly given, check if it should be given implicitly
  for (unsigned int curLang=0; curLang<ui_languages; curLang++)
  {
      /// Check if object is in language-value-file
    if (arrs_language[curLang].valueBuffer != NULL)
    {
      char* bufferCur = arrs_language[curLang].valueBuffer;
      char* bufferEnd = bufferCur + arrs_language[curLang].valueBufferLen;
      while (bufferCur < bufferEnd)
      { // check this line (\n and \r has been previously converted to 0x00
        char pc_id [1024+1];
        char* firstChar=bufferCur;
        while (*firstChar == ' ') firstChar++;
        if ( (*firstChar == 0x00)
              || (strstr (bufferCur, "//") && (strstr (bufferCur, "//") == firstChar))
          )
        { // ignore line
        }
        else
        {
          char* comma = strchr (bufferCur, ',');
          if (comma)
          {
            *comma = 0x00;
            strcpy (pc_id, bufferCur);
            *comma = ','; // revert comma-separator
            if (strcmp (pc_id, searchName) == 0)
            { /// add this language to the language=".." attribute!
              strcat (newLanguageValue, arrs_language [curLang].code);
              break;
            }
          }
          else
          { // no comma found, although it was not a commentary line :(
            std::cout << "No COMMA in a non-comment line in the language file!\n";
            return;
          }
        }
          // advance to next line
        int lineLen = strlen (bufferCur)+1; // include terminating 0x00
        bufferCur += lineLen;
      }
    }
  }
  ((DOMElement *)n)->setAttribute (X("language"), X(newLanguageValue));
  if (newLanguageValue[0] != 0x00)
    std::cout << "AUTO-DETECT-LANGUAGE: FOUND: ["<<newLanguageValue <<"] for "<<searchName<<"."<<std::endl;

}

// ---------------------------------------------------------------------------
//
//  static void processElement (DOMNode *n, unsigned int omb) --- Recursively process all ELEMENT XML-Tags...
// we're in an element here, not text or something else...
//
// ---------------------------------------------------------------------------
/// @todo const char* rpcc_inKey, const char* rpcc_inButton
///       NOT USED atm for recursive uses, as the recursion is done in the setOrigin code!!!!
bool
vt2iso_c::processElement (DOMNode *n, uint64_t ombType /*, const char* rpcc_inKey, const char* rpcc_inButton*/)
{
  DOMNode *child;
  DOMNamedNodeMap *pAttributes;
  char *node_name = XMLString::transcode(n->getNodeName());

  // all possible values of the objects
  signed long int objChildID=0;
  unsigned int objChildX=0;
  unsigned int objChildY=0;
  bool is_objChildID=false; //init for happy compiler
  bool is_objChildX=false; //init for happy compiler
  bool is_objChildY=false; //init for happy compiler
  unsigned int objBlockRow=0; //init for happy compiler
  unsigned int objBlockCol=0; //init for happy compiler
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
  char recursePass [stringLength+1];
  const char* rpcc_inKey=NULL;
  const char* rpcc_inButton=NULL;
  objName [stringLength+1-1] = 0x00;
  is_objName = false;
  is_objID=false;

  char pc_postfix [16+1]; // almost arbitrary number ;)
  pc_postfix [0] = 0x00;
  unsigned int curLang;

  if (!n)
  {
    printf ("processElement(): DOMNode is invalid! STOP PARSER! bye.\n\n");
    return false;
  }

  // get own ObjectType
  objType = objectIsType (node_name); // returns 0..34

  // get own Command Type
  commandType = commandIsType (node_name);

  /// if USE_SPECIAL_PARSING is defined the objType will also be tested if it is a valid additional object type
  if ( ( pc_specialParsing || pc_specialParsingPropTag) && (objType == 0xFFFF) && (commandType == 0xFFFF))
  {
    if ( pc_specialParsing )
      objType = pc_specialParsing->getObjType(node_name);

    if ( pc_specialParsingPropTag && objType == 0xFFFF )
      objType = pc_specialParsingPropTag->getObjType (node_name);
  }

  // ERROR: Wrong <TAG>
  if (objType == 0xFFFF && commandType == 0xFFFF) {
    std::cout << "\n\nUNKNOWN TAG <"<< node_name <<"> ENCOUNTERED! STOPPING PARSER! bye.\n\n";
    return false;
  }

  if (objType >= otObjectpool)
  {
    if (objType == otObjectpool)
    { // expect (datamask) dimension here
      if (n->hasAttributes()) { // parse through all attributes
        pAttributes = patched_getAttributes(n);
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
              return false;
            }
            opDimension = atoi (attr_value);
            is_opDimension = true;
            continue;
          }
          if (strncmp (attr_name, "sk_width", stringLength) == 0) {
            if (is_skWidth) {
              std::cout << "\n\nYOU MUSTN'T SPECIFY THE sk_width= TAG IN <objectpool> MORE THAN ONCE! STOPPING PARSER! bye.\n\n";
              return false;
            }
            skWidth = atoi (attr_value);
            is_skWidth = true;
            continue;
          }
          if (strncmp (attr_name, "sk_height", stringLength) == 0) {
            if (is_skHeight) {
              std::cout << "\n\nYOU MUSTN'T SPECIFY THE sk_height= TAG IN <objectpool> MORE THAN ONCE! STOPPING PARSER! bye.\n\n";
              return false;
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
          if (strncmp (attr_name, "auto_language", stringLength) == 0) {
            strcpy (spc_autoLanguage, attr_value);
            continue;
          }
        }
      }
    }
    if (pc_specialParsing && (objType == otIncludeobject))
    {
      sprintf (objName, "%s%s", pcch_poolIdent, getAttributeValue (n, "name"));
      if (!pc_specialParsing->parseKnownTag(n, objType, objName, &objID, &is_objID))
        return false;
    }
  }
  else
  {
    if ( objType < maxObjectTypes && ( ( (uint64_t(1)<<objType) & ombType) == 0 ) )
    {
      // ERROR: Unallowed <TAG> here?!
      std::cout << "\n\nENCOUNTERED WRONG TAG AT THIS POSITION!\nENCOUNTERED: <" << node_name << "> '" << getAttributeValue (n, "name") << "'objType: "
                << objType << " ombType: " << ombType << "\nPOSSIBLE TAGS HERE WOULD BE: ";
      for (int j=0; j<maxObjectTypesToCompare; j++) {
        if ((uint64_t(1)<<j) & ombType) {
          std::cout << " <" << otCompTable [j] << ">  ";
        }
      }
      std::cout << "\n\n";
      return false;
    }

    /// if USE_SPECIAL_PARSING is defined, check here if found tag is valid at this position
    bool b_unknownTag = false;
    if (pc_specialParsing && ((objType >= maxObjectTypes) && !pc_specialParsing->checkTag(n, objType, ombType)))
    {
      b_unknownTag = true;
    }

    if (!b_unknownTag && pc_specialParsingPropTag && ((objType >= maxObjectTypes) && !pc_specialParsingPropTag->checkTag (n, objType, ombType)))
    {
      std::cout << "Unknown tag for enhanced parsing modules! STOP PARSER! bye.\n\n";
      return false;
    }

    static bool sb_firstObject=true;
    if (sb_firstObject)
    { // first object has to be WorkingSet
      if (objType != otWorkingset)
      {
        std::cout << "\n\nFIRST ELEMENT HAS TO BE <workingset>! Stopping parser. Please put your workingset-object at the top in your <objectpool>!\n\n ";
        return false;
      }
      sb_firstObject = false; // check is only valid for first element!
    }

    autoDetectLanguage (n);
    if (!getAttributesFromNode (n, true))
      return false; // true: read name= and id=

    // set all non-set attributes to default values (as long as sensible, like bg_colour etc.)
    defaultAttributes (objType);

    // get a new ID for this object if not yet done
    signed long int checkObjID = getID (objName, (objType == otMacro) ? true: false, is_objID, objID);

    if (checkObjID == -1)
    {
      std::cout << "Error in getID() from object <" << node_name << "> '" << objName << "'! STOP PARSER! bye.\n\n";
      return false;
    }
    else
      objID = (unsigned int)checkObjID;

    if (pc_specialParsingPropTag && (objType >= maxObjectTypes))
    {
      if (!pc_specialParsingPropTag->parseUnknownTag (n, objType, objName))
        return false;
    }

    /** if USE_SPECIAL_PARSING is defined then a special parsing follows here
      * either to parse the additional attributes in the known tags
      * or to fully parse an unknown tag
    */
    if (pc_specialParsing)
    {
      /// first check if the wished id (attribute id="" is given) is a valid resource id
      if (is_objID && !pc_specialParsing->checkUseOfResourceID (objID))
      {
        const char* pcch_tmpName = (is_objName)? objName:"unnamed object";
        std::cout << "\n\nMISUSE OF RESOURCE ID '" << objID << "' AS WISH OBJECT ID IN OBJECT <" << node_name << "> with name '"
            << pcch_tmpName << "' STOPPING PARSER! bye.\n\n ";
        return false;
      }

      if (objType >= maxObjectTypes) /// that tag is unknown for basic vt2iso
      {
        if (!pc_specialParsing->parseUnknownTag(n, objType, &is_objID, objID))
          return false;
        b_hasUnknownAttributes = false;
      }
      else
      {
        switch (objType)
        {
          case otDatamask:
          case otAlarmmask:
            /// attribute for reference to softkeymask
            if (!pc_specialParsing->parseKnownTag(n, objType, objName, &objID, &is_objID, attrString[attrSoft_key_mask]))
              return false;
            break;
          default:
            /// attribute for variable reference
            if (!pc_specialParsing->parseKnownTag(n, objType, objName, &objID, &is_objID, "NULL", attrString[attrVariable_reference]))
              return false;
            break;
        }
      }
      if (b_hasUnknownAttributes)
      // set object ID here to ensure that the resource ID is used as object ID
        setID (objName, objID);
    }

    // Completely parsed <tag ...> now, start writing out to the files!
    // ______________________________________________________________________________________________________________________________
    // we got objName, objID, objType here now! - print out standard definitions in A, B and C right after the array definition lines
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    bool objHasArrayEventMacro = false;
    switch (objType)
    {
      case otWorkingset:     case otDatamask:       case otAlarmmask:       case otContainer:  case otSoftkeymask:  case otKey:  case otButton:
      case otInputboolean:   case otInputstring:    case otInputnumber:     case otInputlist:
      case otOutputstring:   case otOutputnumber:   case otOutputlist:
      case otLine:           case otRectangle:      case otEllipse:         case otPolygon:
      case otMeter:          case otLinearbargraph: case otArchedbargraph:
      case otPicturegraphic:
      case otFontattributes: case otLineattributes:  case otFillattributes:  case otInputattributes:
        objHasArrayEventMacro = true; // only variables and object pointer have no events/macros
        break;
    }

    bool objHasArrayObject = false;
    switch (objType)
    {
      case otSoftkeymask:
      case otInputlist:
      case otOutputlist:
        objHasArrayObject = true;
        break;
    }

    bool objHasArrayObjectXY = false;
    switch (objType)
    {
      case otKey:
        strcpy (recursePass, objName);
        rpcc_inKey = recursePass;
        objHasArrayObjectXY = true;
        break;
      case otButton:
        strcpy (recursePass, objName);
        rpcc_inButton = recursePass;
        objHasArrayObjectXY = true;
        break;
      case otWorkingset:
      case otDatamask:
      case otAlarmmask:
      case otContainer:
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

    static char duplicateForLanguages[(136*2)+1]; // max for all languages

    /// Backup as those values are overriden by the multilanguage processing when getting values from the .xx.txt
    char backupAttrStringValue [stringLength+1];
    strcpy (backupAttrStringValue, attrString [attrValue]);
    bool backupAttrIsGivenValue = attrIsGiven [attrValue];

    char backupAttrStringLength [stringLength+1];
    strcpy (backupAttrStringLength, attrString [attrLength]);
    bool backupAttrIsGivenLength = attrIsGiven [attrLength];

    autoDetectLanguage (n);

    // ************* NEW: Language duplicates!! ************
    duplicateForLanguages [0] = 0x00;
    char* dupLangNext=duplicateForLanguages; // if dup, then start with first one of course...
    bool b_dupMode=false;
    if ((attrIsGiven [attrLanguage]) && ((strlen (attrString [attrLanguage]) > 0) && (strlen (attrString [attrLanguage]) != 2)))
    { // "*" or multiple languages, so we need to loop!
      if (strcmp (attrString [attrLanguage], "*") == 0)
      { // build all languages we have defined in the working set
        for (unsigned int ui=0; ui<ui_languages; ui++)
          strcat (duplicateForLanguages, arrs_language [ui].code);
      }
      else
        strcpy (duplicateForLanguages, attrString [attrLanguage]);

      b_dupMode=true;
    }

    do // language duplication loop!
    {
      if (b_dupMode)
      { // "fake-copy" next language to "attrString [attrLanguage]"
        if (*dupLangNext == '+') dupLangNext++; // skip "+"s!
        attrString [attrLanguage][0] = *dupLangNext++;
        attrString [attrLanguage][1] = *dupLangNext++;
        attrString [attrLanguage][2] = 0x00;
        if (*dupLangNext == '+') dupLangNext++; // skip "+"s!

        /// Restore "value=" attribute, as it may have been changed by the multilanguage get from .xx.txt file!
        strcpy (attrString [attrValue], backupAttrStringValue);
        attrIsGiven [attrValue] = backupAttrIsGivenValue;
        strcpy (attrString [attrLength], backupAttrStringLength);
        attrIsGiven [attrLength] = backupAttrIsGivenLength;
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
            /// Here we're in one <language code="xx"> element!!!
            // get 'code=' out of child
            if(child->hasAttributes())
            {
              // parse through all attributes
              pAttributes = patched_getAttributes(child);
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
                  return false;
                }
              }
            }

            if (firstElement) {
              fprintf (partFile_attributes, "const IsoAgLib::repeat_vtLanguage_s ivtObject%s_aLanguage [] = {", objName);
            } else {
              fprintf (partFile_attributes, ", ");
            }
            if (!(attrIsGiven [attrCode]))
            {
              std::cout << "\n\ncode ATTRIBUTE NEEDED IN <language ...> ! STOPPING PARSER! bye.\n\n";
              return false;
            }
            char languageCode[2];
            languageCode[0] = attrString[attrCode][0];
            languageCode[1] = attrString[attrCode][1];

            // Check for correct LanguageCode!
            int lc;
            for (lc=0; lc<DEF_iso639entries; lc++) {
              if ((iso639table[lc][0] == languageCode[0]) && (iso639table[lc][1] == languageCode[1])) break;
            }
            if (lc == DEF_iso639entries) {
              // language not found!
              std::cout << "\n\n<language code=\"" << languageCode[0] << languageCode[1] << "\" /> is NOT conform to ISO 639 (Maybe you didn't use lower-case letters?!)! STOPPING PARSER! bye.\n\n";
              return false;
            }
            sprintf (tempString, "'%c', '%c'", languageCode[0], languageCode[1]);
            fprintf (partFile_attributes, "{%s}", tempString);
            objChildLanguages++;
            firstElement = false;

            /// Also add this language to the intern language-table!
            char langFileName [1024+1];
            sprintf (langFileName, "%s-list%02d.inc", xmlFileGlobal, ui_languages);
            arrs_language [ui_languages].partFile = fopen (langFileName, "wt");
            sprintf (langFileName, "IsoAgLib::iVtObject_c* HUGE_MEM all_iVtObjects%s%d [] = {", pcch_poolIdent, ui_languages);
            fputs (langFileName, arrs_language [ui_languages].partFile);
            arrs_language [ui_languages].code[0] = languageCode[0];
            arrs_language [ui_languages].code[1] = languageCode[1];
            arrs_language [ui_languages].code[2] = 0x00;
            arrs_language [ui_languages].count = 0;
            arrs_language [ui_languages].firstLine = true;
            // Open and read complete languages-files
            sprintf (langFileName, "%s.values.%s.txt", xmlFileGlobal, arrs_language [ui_languages].code);
            FILE* tmpHandle = fopen (langFileName, "rb");
            if (tmpHandle != NULL)
            {
              fseek (tmpHandle, 0, SEEK_END);
              long length = ftell (tmpHandle);
              fseek (tmpHandle, 0, SEEK_SET);
              arrs_language [ui_languages].valueBufferLen = length + 1; // we'll internally add an extra NULL-byte!
              arrs_language [ui_languages].valueBuffer = new (char [arrs_language [ui_languages].valueBufferLen]);
              fread (arrs_language [ui_languages].valueBuffer, 1, length, tmpHandle);
              arrs_language [ui_languages].valueBuffer[length+1-1] = 0x00;
              char *iterate=arrs_language [ui_languages].valueBuffer;
              char *iterateEnd=iterate+length;
              // terminate all the lines with an 0x00 char.
              while (iterate < iterateEnd)
              {
                if ((*iterate == '\n') || (*iterate == '\r')) *iterate = 0x00;
                iterate++;
              }
              fclose (tmpHandle);
            }
            else
            {
              arrs_language [ui_languages].valueBufferLen = 0; // we'll internally add an extra NULL-byte!
              arrs_language [ui_languages].valueBuffer = NULL;
            }
            ui_languages++;
          }
        }
        if (firstElement == false)
          fprintf (partFile_attributes, "};\n");
      }

      // ### Print out RAWBITMAP byte array
      if (objType == otPicturegraphic)
      {
        if (!(attrIsGiven [attrWidth] && attrIsGiven [attrFormat] && attrIsGiven [attrTransparency_colour]))
        {
          std::cout << "YOU NEED TO SPECIFY THE width= AND format= AND transparency_colour= ATTRIBUTES FOR THE <picturegraphic> OBJECT " << objName << "! STOPPING PARSER! bye.\n\n";
          return false;
        }

        ///////////////////////////////////////////////////////
        /// ### +BEGIN+ -- FIRST -- process "standard" bitmap
        c_Bitmap.resetLengths();
        checkForFileOrFile148 ("picturegraphic");

        objBitmapOptions = picturegraphicoptionstoi (attrString [attrOptions]);
        if (!openDecodePrintOut (rc_workDir, std_bitmap_path, objBitmapOptions)) return false;

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
            if (!getAttributesFromNode(child, false)) return false; // false: DON'T read name= and id=
            // no defaultAttributes() needed here...

            c_Bitmap.resetLengths();
            checkForFileOrFile148 ("fixedbitmap");

            fixBitmapOptions [fixNr] = objBitmapOptions & 0x3; // keep flashing/transparency information from <pictureobject>
            if (!openDecodePrintOut (rc_workDir, fix_bitmap_path, fixBitmapOptions[fixNr], fixNr)) return false;

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
          fprintf (partFile_attributes, "const IsoAgLib::repeat_rawData_rawBytes_actWidth_actHeight_formatoptions_s iVtObject%s_aFixedBitmaps [] = {", objName);
          bool firstEntry=true;
          for (unsigned int i=0; i<fixNr; i++) {
            for (int actDepth=0; actDepth<2; actDepth++) {
              // was this depth generated for this special bitmap?
              if (fixRawBitmapBytes[i] [actDepth] > 0) {
                if (!firstEntry) fprintf (partFile_attributes, ", ");
                unsigned int options = (fixBitmapOptions[i] & 0x3) | ( (fixBitmapOptions[i] & (uint64_t(1)<<(2+actDepth))) ? (uint64_t(1)<<2) : 0 );
                fprintf (partFile_attributes, "{iVtObject%s_aRawBitmap%dFixed%d, %d, %d, %d, (%d << 6) | %d}", objName, actDepth, i, fixRawBitmapBytes[i] [actDepth], fiXactualWidth[i], fiXactualHeight[i], actDepth, options);
                firstEntry = false;
              }
            }
          }
          fprintf (partFile_attributes, "};\n");
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
            autoDetectLanguage (child);

            bool b_foundLanguageAttribute=false; // default: none found in this element!

            if ( (strlen (spc_autoLanguage) > 0) &&
                  (  (0 == strcmp (XMLString::transcode(child->getNodeName()), otCompTable [otOutputstring])) ||
                  (0 == strcmp (XMLString::transcode(child->getNodeName()), otCompTable [otStringvariable]))  )
               )
            { // use default language
              b_foundLanguageAttribute = true;
              strcpy (attr_value, spc_autoLanguage);
            }

            if (child->hasAttributes())
            { // see where there may be a LANGUAGE= attribute
              pAttributes = patched_getAttributes(child);
              int nSize = pAttributes->getLength();
              for (int i=0; i<nSize; ++i) {
                DOMAttr *pAttributeNode = (DOMAttr*) pAttributes->item(i);
                utf16convert ((char *)pAttributeNode->getName(), attr_name, 1024);
                utf16convert ((char *)pAttributeNode->getValue(), attr_value2, 1024);

                if (strncmp (attr_name, "language", stringLength) == 0)
                {
                  strcpy (attr_value, attr_value2);
                  b_foundLanguageAttribute = true;
                  break;
                }
              }
            }

            static char duplicateForLanguagesChild[(136*2)+1]; // max for all languages
            char* dupLangNextChild=NULL;
            bool b_dupModeChild=false;

            /// Duplicate Loop here also!!!!!
            if (b_foundLanguageAttribute && (strlen (attr_value) > 0) && (strlen (attr_value) != 2))
            { // "*" or multiple languages, so we need to loop!
              if (strcmp (attr_value, "*") == 0)
              { // build all languages we have defined in the working set
                duplicateForLanguagesChild [0] = 0x00;
                for (unsigned int ui=0; ui<ui_languages; ui++)
                {
                  strcat (duplicateForLanguagesChild, arrs_language [ui].code);
                }
              }
              else
              {
                strcpy (duplicateForLanguagesChild, attr_value);
              }
              b_dupModeChild=true;
              dupLangNextChild=duplicateForLanguagesChild;
            }

            do // language duplication loop!
            {
              if (b_dupModeChild)
              { // "fake-copy" next language to "attrString [attrLanguage]"
                if (*dupLangNextChild == '+') dupLangNextChild++; // skip "+"s!
                attr_value[0] = *dupLangNextChild++;
                attr_value[1] = *dupLangNextChild++;
                attr_value[2] = 0x00;
                if (*dupLangNextChild == '+') dupLangNextChild++; // skip "+"s!
              }

              // Check if it's an ALTERNATIVE language or NO language at all, then DO NOT add this as child!
              unsigned int childLang=0;
              bool b_addAsChild=true;

              if (b_foundLanguageAttribute && (strlen (attr_value) > 0))
              {
                if (ui_languages > 0)
                {
                  for (unsigned int curChildLang=0; curChildLang<ui_languages; curChildLang++)
                  {
                    if (strncmp (attr_value, arrs_language[curChildLang].code, stringLength) == 0)
                    {
                      childLang = curChildLang;
                    }
                  }
                  if (strncmp (attr_value, arrs_language[0].code, stringLength) != 0)
                  {
                    b_addAsChild = false;
                  }
                }
              }

              if (b_addAsChild)
              {
                char objNameWithoutPoolIdent[stringLength+1];
                // get NAME and POS_X and POS_Y attributes out of child
                if(child->hasAttributes()) {
                  // parse through all attributes
                  pAttributes = patched_getAttributes(child);
                  int nSize = pAttributes->getLength();

                  is_objChildName = false; objChildName [stringLength+1-1] = 0x00;
                  is_objChildID = false;
                  is_objChildX = false;
                  is_objChildY = false;

                  strcpy (objBlockFont, "NULL");
                  objBlockRow = 0;
                  objBlockCol = 0;

                  for (int i=0; i<nSize; ++i)
                  {
                    DOMAttr *pAttributeNode = (DOMAttr*) pAttributes->item(i);
                    utf16convert ((char *)pAttributeNode->getName(), attr_name, 1024);
                    utf16convert ((char *)pAttributeNode->getValue(), attr_value, 1024);

                    // Get NAME and POS_X and POS_Y directly
                    if (strncmp (attr_name, "name", stringLength) == 0) {
                      strncpy (objNameWithoutPoolIdent, attr_value, stringLength);
                      sprintf (objChildName, "%s%s", pcch_poolIdent, attr_value);
                      is_objChildName = true;
                      continue;
                    }
                    if (strncmp (attr_name, "id", stringLength) == 0) {
                      objChildID = atoi (attr_value);
                      is_objChildID = true;
                      continue;
                    }
                    if (strncmp (attr_name, "pos_x", stringLength) == 0) {
                      objChildX = atoi (attr_value);
                      is_objChildX = true;
                      continue;
                    }
                    if (strncmp (attr_name, "pos_y", stringLength) == 0) {
                      objChildY = atoi (attr_value);
                      is_objChildY = true;
                      continue;
                    }
                    if (strncmp (attr_name, "block_font", stringLength) == 0) {
                      sprintf (objBlockFont, "&iVtObject%s", pcch_poolIdent);
                      strncat (objBlockFont, attr_value, stringLength-9);
                      continue;
                    }
                    if (strncmp (attr_name, "block_row", stringLength) == 0) {
                      objBlockRow = atoi (attr_value);
                      continue;
                    }
                    if (strncmp (attr_name, "block_col", stringLength) == 0) {
                      objBlockCol = atoi (attr_value);
                      continue;
                    }
                  }
                }

                if (is_objChildName == false)
                {
                  // create auto-named NAME attribute
                  sprintf (objChildName, "Unnamed%d", objNextUnnamedName);
                  ((DOMElement *)child)->setAttribute (X("name"), X(objChildName));
                  // add pool_ident to the name for getting an ID -> if child gets processed, its name gets the pool_ident added
                  sprintf (objChildName,  "%sUnnamed%d", pcch_poolIdent, objNextUnnamedName);
                  objNextUnnamedName++;
                  is_objChildName = true;
                }

                // give him an ID, although not necessary now...
                objChildID = getID (objChildName, false /* assumption: not a macro here */, is_objChildID, objChildID);

                if (objChildID == -1)
                {
                  std::cout << "Error in getID() from object <" << node_name << "> '" << objName << "'! STOP PARSER! bye.\n\n";
                  return false;
                }

                if (firstElement) {
                  if (xyNeeded) fprintf (partFile_attributes, "const IsoAgLib::repeat_iVtObject_x_y_iVtObjectFontAttributes_row_col_s iVtObject%s_aObject_x_y_font_row_col [] = {", objName);
                  else          fprintf (partFile_attributes, "const IsoAgLib::repeat_iVtObject_s iVtObject%s_aObject [] = {", objName);
                } else {
                  fprintf (partFile_attributes, ", ");
                }
                if (xyNeeded) {
                  if (!(is_objChildX && is_objChildY)) {
                    std::cout << "\n\npos_x AND pos_y ATTRIBUTES NEEDED IN CHILD-OBJECT OF <"<< node_name <<"> " << objChildName << "! STOPPING PARSER! bye.\n\n";
                    return false;
                  }
                  fprintf (partFile_attributes, "{&iVtObject%s, %d, %d, %s ,%d, %d}", objChildName, objChildX, objChildY, objBlockFont, objBlockRow, objBlockCol);
                } else {
                  // Added this if statement to account for InputList/OutputList objects who might have NULL Object IDs in their list of objects. (Which is legal per the standard!)
                  // Instead of inserting a faulty object name, just insert NULL into the array. -BAC 07-Jan-2005
                  if (is_objChildID && (objChildID == 65535))
                  {
                    fprintf (partFile_attributes, "{NULL}");
                  }
                  else
                  {
                    fprintf (partFile_attributes, "{&iVtObject%s}", objChildName);
                  }
                }
                objChildObjects++;
                firstElement = false;
              }
              else
              { // !b_addAsChild ==> so manually write the "setOriginXXX" call !!
                /// Add implicit Button/Key includement
                if (rpcc_inButton)
                {
                  fprintf (partFile_functions_origin, "  iVtObject%s_%d.setOriginBTN (&iVtObject%s);\n", objChildName, childLang, rpcc_inButton);
                }
                if (rpcc_inKey)
                {
                  fprintf (partFile_functions_origin, "  iVtObject%s_%d.setOriginSKM (%s);\n", objChildName, childLang, rpcc_inKey ? "true":"false"); // is now always "true"...
                }
              }
            } while (b_dupModeChild && (*dupLangNextChild != 0x00));
            /// END Language Dup Loop
          }
        }
        // all child-elements processed, now:
        // special treatment for inputlist/outputlist with NULL objects
        if (((objType == otInputlist) || (objType == otOutputlist)) && objChildObjects < (uint16_t)atoi(attrString [attrNumber_of_items]))
        {
          //only some items are NULL objects which were not counted in objChildObjects
          if (objChildObjects>0)
          {
            for (uint16_t ui_leftChildObjects = objChildObjects; ui_leftChildObjects<(uint16_t)atoi(attrString [attrNumber_of_items]); ui_leftChildObjects++)
            {
              if (ui_leftChildObjects < atoi(attrString [attrNumber_of_items])) fprintf (partFile_attributes, ", ");
              fprintf (partFile_attributes, "{NULL}");
            }
            objChildObjects=(uint16_t)atoi(attrString [attrNumber_of_items]);
          }
          else {
            // no child-element at all in the inputlist/outputlist (all items as NULL objects)
            // fill the reference-list with {NULL}-elements --> so they could be replaced during runtime with NOT NULL objects
            if (objChildObjects == 0 && atoi(attrString [attrNumber_of_items]) > 0)
            {
              // objChildObjects has to be set to number_of_items otherwise
              // it is set to 0 in the attributes of the inputlist/output
              objChildObjects = (uint16_t)atoi(attrString [attrNumber_of_items]);
              // create for all number_of_items a no-item placeholder
              fprintf (partFile_attributes, "const IsoAgLib::repeat_iVtObject_s iVtObject%s_aObject [] = {", objName);
              for (int i_emptyChildObj=1; i_emptyChildObj <= atoi(attrString [attrNumber_of_items]); i_emptyChildObj++)
              {
                fprintf (partFile_attributes, "{NULL}");
                if (i_emptyChildObj < atoi(attrString [attrNumber_of_items])) fprintf (partFile_attributes, ", ");
              }
              fprintf (partFile_attributes, "};\n");
            }
          }
        }
        if (firstElement == false)
          fprintf (partFile_attributes, "};\n");
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
              pAttributes = patched_getAttributes(child);
              int nSize = pAttributes->getLength();

              attrString [attrEvent] [stringLength+1-1] = 0x00; attrIsGiven [attrEvent] = false;
              objChildName [stringLength+1-1] = 0x00; is_objChildName = false;
              is_objChildID = false;

              for(int i=0;i<nSize;++i) {
                DOMAttr *pAttributeNode = (DOMAttr*) pAttributes->item(i);
                utf16convert ((char *)pAttributeNode->getName(), attr_name, 1024);
                utf16convert ((char *)pAttributeNode->getValue(), attr_value, 1024);

                if (strncmp (attr_name, "event", stringLength) == 0) {
                  strncpy (attrString [attrEvent], attr_value, stringLength);
                  attrIsGiven [attrEvent] = true;
                }
                if (strncmp (attr_name, "name", stringLength) == 0) {
                  sprintf (objChildName, "%s%s", pcch_poolIdent, attr_value);
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
              // add pool_ident to the name for getting an ID -> if child gets processed, its name gets the pool_ident added
              sprintf (objChildName, "%sUnnamed%d", pcch_poolIdent, objNextUnnamedName);
              objNextUnnamedName++;
              is_objChildName = true;
            }
            // give him an ID, although not necessary now...
            objChildID = getID (objChildName, true, is_objChildID, objChildID);

            if (objChildID == -1)
            {
              std::cout << "Error in getID()  from object <" << node_name << "> '" << objName << "'! STOP PARSER! bye.\n\n";
              return false;
            }

            if (firstElement) {
              // Changed the macro struct name in the following line to match what is in version 1.1.0 of IsoAgLib -bac 06-Jan-2005
              // fprintf (partFile_attributes, "const IsoAgLib::repeat_Macro_iVtObject_s iVtObject%s_aMacro_Object [] = {", objName);
              fprintf (partFile_attributes, "const IsoAgLib::repeat_event_iVtObjectMacro_s iVtObject%s_aMacro_Object [] = {", objName);
            } else {
              fprintf (partFile_attributes, ", ");
            }
            if (!(attrIsGiven [attrEvent])) {
              std::cout << "\n\nevent ATTRIBUTE NEEDED IN <macro ...> ! STOPPING PARSER! bye.\n\n";
              return false;
            }
            //fprintf (partFile_attributes, "{%d, &vtObject%s}", atoi (attrString [attrEvent]), objChildName);
            fprintf (partFile_attributes, "{%d, &iVtObject%s}", eventtoi(attrString [attrEvent]), objChildName);
            objChildMacros++;
            firstElement = false;
          }
        }
        if (firstElement == false)
          fprintf (partFile_attributes, "};\n");
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
                  pAttributes = patched_getAttributes(child);
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
                  signed long int ret = idOrName_toi(attrString [attrObjectID], /*macro?*/false);
                  signed int retHideShow = booltoi(attrString[attrHideShow]);
                  if ( ret == -1 )
                  {
                    std::cout << "Error in idOrName_toi() from object <" << node_name << "> '" << objName << "'! STOP PARSER! bye.\n\n";
                    return false;
                  }
                  if ( retHideShow == -1 )
                  {
                    std::cout << "Error in booltoi() from object <" << node_name << "> '" << objName << "'! STOP PARSER! bye.\n\n";
                    return false;
                  }

                  // Need check for all attributes being present for this command -bac
                  sprintf(commandMessage, "0xA0, %d, %d, %d, 0xFF, 0xFF, 0xFF, 0xFF", MACRO_16bitToLE((unsigned int)ret), (unsigned int)retHideShow);
                  objChildCommands++;
                }
                break;

              case ctEnableDisableObject:
                if(child->hasAttributes())
                {
                  // parse through all attributes
                  pAttributes = patched_getAttributes(child);
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
                  signed long int ret = idOrName_toi(attrString [attrObjectID], /*macro?*/false);
                  signed int retDisEnable = booltoi(attrString[attrDisable_enable]);
                  if ( ret == -1 )
                  {
                    std::cout << "Error in idOrName_toi() from object <" << node_name << "> '" << objName << "'! STOP PARSER! bye.\n\n";
                    return false;
                  }
                  if ( retDisEnable == -1 )
                  {
                    std::cout << "Error in booltoi() from object <" << node_name << "> '" << objName << "'! STOP PARSER! bye.\n\n";
                    return false;
                  }
                  // Need check for all attributes being present for this command -bac
                  sprintf(commandMessage, "0xA1, %d, %d, %d, 0xFF, 0xFF, 0xFF, 0xFF", MACRO_16bitToLE((unsigned int)ret), (unsigned int)retDisEnable);
                  objChildCommands++;
                }
                break;

              case ctSelectInputObject:
                if(child->hasAttributes())
                {
                  // parse through all attributes
                  pAttributes = patched_getAttributes(child);
                  int nSize = pAttributes->getLength();

                  cleanAttribute(attrObjectID);

                  for(int i=0;i<nSize;++i)
                  {
                    DOMAttr *pAttributeNode = (DOMAttr*) pAttributes->item(i);
                    utf16convert ((char *)pAttributeNode->getName(), attr_name, 1024);
                    utf16convert ((char *)pAttributeNode->getValue(), attr_value, 1024);

                    setAttributeValue(attrObjectID);
                  }
                  signed long int ret = idOrName_toi(attrString [attrObjectID], /*macro?*/false);
                  if (ret == -1)
                  {
                    std::cout << "Error in idOrName_toi() from object <" << node_name << "> '" << objName << "'! STOP PARSER! bye.\n\n";
                    return false;
                  }
                  // Need check for all attributes being present for this command -bac
                  sprintf(commandMessage, "0xA2, %d, %d, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF", MACRO_16bitToLE((unsigned int)ret));
                  objChildCommands++;
                }
                break;

              case ctControlAudioDevice:
                if(child->hasAttributes())
                {
                  // parse through all attributes
                  pAttributes = patched_getAttributes(child);
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
                  pAttributes = patched_getAttributes(child);
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
                  pAttributes = patched_getAttributes(child);
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
                  signed long int retParent = idOrName_toi(attrString [attrParent_objectID], /*macro?*/false);
                  signed long int ret = idOrName_toi(attrString [attrObjectID], /*macro?*/false);
                  if ((ret == -1) || (retParent == -1))
                  {
                    std::cout << "Error in idOrName_toi() from object <" << node_name << "> '" << objName << "'! STOP PARSER! bye.\n\n";
                    return false;
                  }
                  sprintf(commandMessage, "0xA5, %d, %d, %d, %d, %d, %d, 0xFF", MACRO_16bitToLE((unsigned int)retParent), MACRO_16bitToLE((unsigned int)ret), atoi(attrString [attrX_change]) + 127 ,atoi(attrString [attrY_change]) + 127 );
                  objChildCommands++;
                }
                break;

              case ctChangeChildPosition:
                if(child->hasAttributes())
                {
                  // parse through all attributes
                  pAttributes = patched_getAttributes(child);
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
                  signed long int retParent = idOrName_toi(attrString [attrParent_objectID], /*macro?*/false);
                  signed long int ret = idOrName_toi(attrString [attrObjectID], /*macro?*/false);
                  if ((ret == -1) || (retParent == -1))
                  {
                    std::cout << "Error in idOrName_toi() from object <" << node_name << "> '" << objName << "'! STOP PARSER! bye.\n\n";
                    return false;
                  }
                  // Need check for all attributes being present for this command -bac
                  sprintf(commandMessage, "0xB4, %d, %d, %d, %d, %d, %d, %d, %d", MACRO_16bitToLE((unsigned int)retParent), MACRO_16bitToLE((unsigned int)ret), MACRO_16bitToLE(atoi(attrString [attrX_pos])), MACRO_16bitToLE(atoi(attrString [attrY_pos])));
                  objChildCommands++;
                }
                break;

              case ctChangeSize:
                if(child->hasAttributes())
                {
                  // parse through all attributes
                  pAttributes = patched_getAttributes(child);
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
                  signed long int ret = idOrName_toi(attrString [attrObjectID], /*macro?*/false);
                  if (ret == -1)
                  {
                    std::cout << "Error in idOrName_toi() from object <" << node_name << "> '" << objName << "'! STOP PARSER! bye.\n\n";
                    return false;
                  }
                  // Need check for all attributes being present for this command -bac
                  sprintf(commandMessage, "0xA6, %d, %d, %d, %d, %d, %d, 0xFF", MACRO_16bitToLE((unsigned int)ret), MACRO_16bitToLE(atoi(attrString [attrNew_width])), MACRO_16bitToLE(atoi(attrString [attrNew_height])));
                  objChildCommands++;
                }
                break;

              case ctChangeBackgroundColour:
                if(child->hasAttributes())
                {
                  // parse through all attributes
                  pAttributes = patched_getAttributes(child);
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
                  signed long int ret = idOrName_toi(attrString [attrObjectID], /*macro?*/false);
                  if (ret == -1)
                  {
                    std::cout << "Error in idOrName_toi() from object <" << node_name << "> '" << objName << "'! STOP PARSER! bye.\n\n";
                    return false;
                  }
                  // Need check for all attributes being present for this command -bac
                  sprintf(commandMessage, "0xA7, %d, %d, %d, 0xFF, 0xFF, 0xFF, 0xFF", MACRO_16bitToLE((unsigned int)ret), colortoi(attrString [attrNew_background_colour]));
                  objChildCommands++;
                }
                break;

              case ctChangeNumericValue:
                if(child->hasAttributes())
                {
                  // parse through all attributes
                  pAttributes = patched_getAttributes(child);
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
                  signed long int ret = idOrName_toi(attrString [attrObjectID], /*macro?*/false);
                  signed long int retNewValue = idOrName_toi(attrString [attrNew_value], /*macro?*/false); // "idOrName_toi" is okay here, because we could change an objectpointer to point to some other object!
                  if ((retNewValue == -1) || (ret == -1))
                  {
                    std::cout << "Error in idOrName_toi() from object <" << node_name << "> '" << objName << "'! STOP PARSER! bye.\n\n";
                    return false;
                  }
                  // Need check for all attributes being present for this command -bac
                  sprintf(commandMessage, "0xA8, %d, %d, 0x00, %li, %li, %li, %li", MACRO_16bitToLE((unsigned int)ret), MACRO_32bitToLE(retNewValue));
                  objChildCommands++;
                }
                break;

              case ctChangeStringValue:
                if(child->hasAttributes())
                {
                  // parse through all attributes
                  pAttributes = patched_getAttributes(child);
                  int nSize = pAttributes->getLength();

                  cleanAttribute(attrObjectID);
                  cleanAttribute(attrBytes_in_string);
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
                  // Also need to replace the atoi below to some integertoi stuff so characters will register an error!
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
                  signed long int ret = idOrName_toi(attrString [attrObjectID], /*macro?*/false);
                  signed long int retBytesInString = atoi(attrString [attrBytes_in_string]);
                  if ((ret == -1) || (retBytesInString == -1))
                  {
                    std::cout << "Error in idOrName_toi() from object <" << node_name << "> '" << objName << "'! STOP PARSER! bye.\n\n";
                    return false;
                  }
                  //sprintf (attrString [attrValue], "%s", tempString2);
                  sprintf(commandMessage, "0xB3, %d, %d, %d, %d%s", MACRO_16bitToLE((unsigned int)ret), MACRO_16bitToLE((unsigned int)retBytesInString), tempString2);

                  objChildCommands++;
                }
                break;

              case ctChangeEndPoint:
                if(child->hasAttributes())
                {
                  // parse through all attributes
                  pAttributes = patched_getAttributes(child);
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
                  signed long int ret = idOrName_toi(attrString [attrObjectID], /*macro?*/false);
                  if (ret == -1)
                  {
                    std::cout << "Error in idOrName_toi() from object <" << node_name << "> '" << objName << "'! STOP PARSER! bye.\n\n";
                    return false;
                  }
                  // Need check for all attributes being present for this command -bac
                  sprintf(commandMessage, "0xA9, %d,%d, %d,%d, %d,%d, %d", MACRO_16bitToLE((unsigned int)ret), MACRO_16bitToLE(atoi(attrString [attrNew_width])), MACRO_16bitToLE(atoi(attrString [attrNew_height])), atoi(attrString [attrLine_direction]));

                  objChildCommands++;
                }
                break;

              case ctChangeFontAttributes:
                if(child->hasAttributes())
                {
                  // parse through all attributes
                  pAttributes = patched_getAttributes(child);
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
                  signed long int ret = idOrName_toi(attrString [attrObjectID], /*macro?*/false);
                  signed long int retFontSize = fontsizetoi(attrString [attrFont_size]);
                  if ( ret == -1 )
                  {
                    std::cout << "Error in idOrName_toi() from object <" << node_name << "> '" << objName << "'! STOP PARSER! bye.\n\n";
                    return false;
                  }
                  if ( retFontSize == -1 )
                  {
                    std::cout << "Error in fontsizetoi() from object <" << node_name << "> '" << objName << "'! STOP PARSER! bye.\n\n";
                    return false;
                  }

                  // Need check for all attributes being present for this command -bac
                  sprintf(commandMessage, "0xAA, %d, %d, %d, %d, %d, %d, 0xFF", MACRO_16bitToLE((unsigned int)ret), colortoi(attrString [attrFont_colour]), (unsigned int)retFontSize, atoi(attrString [attrFont_type]), fontstyletoi(attrString [attrFont_style]));

                  objChildCommands++;
                }
                break;

              case ctChangeLineAttributes:
                if(child->hasAttributes())
                {
                  // parse through all attributes
                  pAttributes = patched_getAttributes(child);
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
                  signed long int ret = idOrName_toi(attrString [attrObjectID], /*macro?*/false);
                  if (ret == -1)
                  {
                    std::cout << "Error in idOrName_toi() from object <" << node_name << "> '" << objName << "'! STOP PARSER! bye.\n\n";
                    return false;
                  }
                  // Need check for all attributes being present for this command -bac
                  sprintf(commandMessage, "0xAB, %d, %d, %d, %d, %d, %d, 0xFF", MACRO_16bitToLE((unsigned int)ret), colortoi(attrString [attrLine_colour]), atoi(attrString [attrLine_width]), MACRO_16bitToLE(linearttoi(attrString [attrLine_art])));

                  objChildCommands++;
                }
                break;

              case ctChangeFillAttributes:
                if(child->hasAttributes())
                {
                  // parse through all attributes
                  pAttributes = patched_getAttributes(child);
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
                  signed long int ret = idOrName_toi(attrString [attrObjectID], /*macro?*/false);
                  if (ret == -1)
                  {
                    std::cout << "Error in idOrName_toi() from object <" << node_name << "> '" << objName << "'! STOP PARSER! bye.\n\n";
                    return false;
                  }
                  // Need check for all attributes being present for this command -bac
                  sprintf(commandMessage, "0xAC, %d, %d, %d, %d, %d, %d, 0xFF", MACRO_16bitToLE((unsigned int)ret), filltypetoi(attrString [attrFill_type]), colortoi(attrString [attrFill_colour]), MACRO_16bitToLE(atoi(attrString [attrFill_pattern])));

                  objChildCommands++;
                }
                break;

              case ctChangeActiveMask:
                if(child->hasAttributes())
                {
                  // parse through all attributes
                  pAttributes = patched_getAttributes(child);
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
                  signed long int retWS_ID = idOrName_toi(attrString [attrWorking_setID], /*macro?*/false);
                  signed long int retNewMask = idOrName_toi(attrString [attrNew_active_mask], /*macro?*/false);
                  if ((retWS_ID == -1) || (retNewMask == -1))
                  {
                    std::cout << "Error in idOrName_toi() from object <" << node_name << "> '" << objName << "'! STOP PARSER! bye.\n\n";
                    return false;
                  }
                  // Need check for all attributes being present for this command -bac
                  sprintf(commandMessage, "0xAD, %d, %d, %d, %d, 0xFF, 0xFF, 0xFF", MACRO_16bitToLE((unsigned int)retWS_ID), MACRO_16bitToLE((unsigned int)retNewMask));

                  objChildCommands++;
                }
                break;

              case ctChangeSoftKeyMask:
                if(child->hasAttributes())
                {
                  // parse through all attributes
                  pAttributes = patched_getAttributes(child);
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
                  signed long int retMaskID = idOrName_toi(attrString [attrMaskID], /*macro?*/false);
                  signed long int retNewSKM = idOrName_toi(attrString [attrNew_softkey_mask], /*macro?*/false);
                  if ((retMaskID == -1) || (retNewSKM == -1))
                  {
                    std::cout << "Error in idOrName_toi() from object <" << node_name << "> '" << objName << "'! STOP PARSER! bye.\n\n";
                    return false;
                  }
                  // Need check for all attributes being present for this command -bac
                  sprintf(commandMessage, "0xAE, %d, %d, %d, %d, %d, 0xFF, 0xFF", masktypetoi(attrString [attrMask_type]), MACRO_16bitToLE((unsigned int)retMaskID), MACRO_16bitToLE((unsigned int)retNewSKM));

                  objChildCommands++;
                }
                break;

              case ctChangeAttribute:
                if(child->hasAttributes())
                {
                  // parse through all attributes
                  pAttributes = patched_getAttributes(child);
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
                  signed long int ret = idOrName_toi(attrString [attrObjectID], /*macro?*/false);
                  signed long int retNewValue = idOrName_toi(attrString [attrNew_value], /*macro?*/false);
                  if ((ret == -1) || (retNewValue == -1))
                  {
                    std::cout << "Error in idOrName_toi() from object <" << node_name << "> '" << objName << "'! STOP PARSER! bye.\n\n";
                    return false;
                  }
                  // Need check for all attributes being present for this command -bac
                  sprintf(commandMessage, "0xAF, %d, %d, %d, %li, %li, %li, %li", MACRO_16bitToLE((unsigned int)ret), atoi(attrString [attrAttributeID]), MACRO_32bitToLE(retNewValue));

                  objChildCommands++;
                }
                break;

              case ctChangePriority:
                if(child->hasAttributes())
                {
                  // parse through all attributes
                  pAttributes = patched_getAttributes(child);
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
                  signed long int ret = idOrName_toi(attrString [attrObjectID], /*macro?*/false);
                  signed long int retPrio = prioritytoi(attrString [attrNew_priority]);
                  if (ret == -1)
                  {
                    std::cout << "Error in idOrName_toi() from object <" << node_name << "> '" << objName << "'! STOP PARSER! bye.\n\n";
                    return false;
                  }
                  if (retPrio == -1)
                  {
                    std::cout << "Error in prioritytoi() from object <" << node_name << "> '" << objName << "'! STOP PARSER! bye.\n\n";
                    return false;
                  }
                  // Need check for all attributes being present for this command -bac
                  sprintf(commandMessage, "0xB0, %d, %d, %d, 0xFF, 0xFF, 0xFF, 0xFF", MACRO_16bitToLE((unsigned int)ret), (unsigned int)retPrio);

                  objChildCommands++;
                }
                break;

              case ctChangeListItem:
                if(child->hasAttributes())
                {
                // parse through all attributes
                  pAttributes = patched_getAttributes(child);
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
                  signed long int ret = idOrName_toi(attrString [attrObjectID], /*macro?*/false);
                  signed long int retNewID = idOrName_toi(attrString [attrNew_objectID], /*macro?*/false);
                  if ((ret == -1) || (retNewID == -1))
                  {
                    std::cout << "Error in idOrName_toi() from object <" << node_name << "> '" << objName << "'! STOP PARSER! bye.\n\n";
                    return false;
                  }
                // Need check for all attributes being present for this command -bac
                  sprintf(commandMessage, "0xB1, %d, %d, %d, %d, %d, 0xFF, 0xFF", MACRO_16bitToLE((unsigned int)ret), atoi(attrString [attrList_index]), MACRO_16bitToLE((unsigned int)retNewID));

                  objChildCommands++;
                }
                break;
            }

            if (firstElement)
            {
              fprintf (partFile_attributes, "const uint8_t iVtObject%s_aMacro_Commands [] = {", objName);
            } else {
              fprintf (partFile_attributes, ", ");
            }

            // We need something like this, but up above for each command type in order to deal with the varying attributes for each command. . . -bac
            /*if (!(attrIsGiven [attrEvent]))
            {
            std::cout << "\n\nevent ATTRIBUTE NEEDED IN <macro ...> ! STOPPING PARSER! bye.\n\n";
            return false;
          }*/
            //fprintf (partFile_attributes, "{%d, &vtObject%s}", atoi (attrString [attrEvent]), objChildName);
            fprintf (partFile_attributes, "%s", commandMessage);
            firstElement = false;
          }
        }

        if (firstElement == false)
          fprintf (partFile_attributes, "};\n");
      } // End of if (objHasArrayMacroCommand)

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
              pAttributes = patched_getAttributes(child);
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
              fprintf (partFile_attributes, "const IsoAgLib::repeat_x_y_s iVtObject%s_aPoints [] = {", objName);
            } else {
              fprintf (partFile_attributes, ", ");
            }
            if (!(attrIsGiven [attrPos_x])) {
              std::cout << "\n\npos_x ATTRIBUTE NEEDED IN <point ...>  from object <" << node_name << "> '" << objName << "'! STOPPING PARSER! bye.\n\n";
              return false;
            }
            if (!(attrIsGiven [attrPos_y])) {
              std::cout << "\n\npos_y ATTRIBUTE NEEDED IN <point ...>  from object <" << node_name << "> '" << objName << "'! STOPPING PARSER! bye.\n\n";
              return false;
            }
            fprintf (partFile_attributes, "{%d, %d}", atoi(attrString [attrPos_x]), atoi(attrString [attrPos_y]));
            objChildPoints++;
            firstElement = false;
          }
        }
        if (objChildPoints < 3)
        {
          std::cout << "\n\nYOU NEED TO SPECIFY AT LEAST 3 <point ...> elements for object <" << node_name << "> '" << objName << "'! STOPPING PARSER! bye.\n\n";
          return false;
        }
        else
        {
          sprintf (attrString [attrNumber_of_points], "%i", objChildPoints);
          if (!attrIsGiven [attrNumber_of_points])
            attrIsGiven [attrNumber_of_points] = true;
        }
        if (firstElement == false)
          fprintf (partFile_attributes, "};\n");
      }
      //*************************************************************************************************************************************************************
      //******************** End of code added by Brad Cox **********************************************************************************************************

      // ###################################################
      // ### Print out definition, values and init-calls ###
      // ###################################################
      FILE* fileList;
      bool *pb_firstLine;
      /// MultiLanguage-Support: See which -list.inc file to write object to!
      if (attrIsGiven [attrLanguage] && (strlen (attrString [attrLanguage]) > 0))
      { // write to special language-list file if language is correct!
        // search language in list-file-array
        for (curLang=0; curLang<ui_languages; curLang++)
        {
          if (strcmp (arrs_language[curLang].code, attrString[attrLanguage]) == 0)
            break; // found the language! => index is "curLang"
        }
        if ((curLang==ui_languages) && (ui_languages>0))
        { // language not found!
          std::cout << "\n\nYOU NEED TO SPECIFY A VALID LANGUAGE which you have also defined in the <workingset> object ("<<attrString[attrLanguage]<<" is not!)! STOPPING PARSER! bye.\n\n";
          return false;
        }
        fileList = arrs_language[curLang].partFile;
        pb_firstLine = &arrs_language[curLang].firstLine;

        arrs_language[curLang].count++; // and do NOT count the normal list up!
        if (curLang > 0)
        {
          sprintf (pc_postfix, "_%d", curLang);
        }

        if ( checkForAllowedExecution() )
        {
          std::string str_objClassName = std::string("&iVtObject") + std::string(objName) + std::string(pc_postfix);
          // first check if found language was inserted before into the map
          std::map<int32_t, ObjListEntry>::iterator mit_entry = map_objNameAndID.find (curLang);

          if (mit_entry != map_objNameAndID.end()) // success! language entry found
          { // only insert an further entry in the found language map
            //store the objectId - objectName pair in the appropriate language map
            mit_entry->second.insert (std::pair<uint16_t, std::string>(objID, str_objClassName));
          }
          else
          { // add a new map entry for a further language which includes the first entry in that language specific map
            ObjListEntry listEntry;
            listEntry.insert (std::pair<uint16_t, std::string>(objID, str_objClassName));
            map_objNameAndID.insert (std::pair<int32_t, ObjListEntry>(curLang, listEntry));
          }
        }

        /// Try to retrieve   value='...'   from language-value-file
        if (arrs_language[curLang].valueBuffer != NULL)
        {
          //std::cout << "searching language file for ["<<objName<<"].\n";
          bool b_foundValue=false;
          char* bufferCur = arrs_language[curLang].valueBuffer;
          char* bufferEnd = bufferCur + arrs_language[curLang].valueBufferLen;
          char pc_foundValue [4096+1];
          while (bufferCur < bufferEnd)
          {
            // check this line (\n and \r has been previously converted to 0x00
            char pc_id [1024+1];
            char* firstChar=bufferCur;
            while (*firstChar == ' ') firstChar++;
            if ( (*firstChar == 0x00)
                   || (strstr (bufferCur, "//") && (strstr (bufferCur, "//") == firstChar))
               )
            { // ignore line
            }
            else
            {
              char* comma = strchr (bufferCur, ',');
              if (comma)
              {
                *comma = 0x00;
                strcpy (pc_id, bufferCur);
                *comma = ','; // revert comma-separator
                comma++; // goto next character following the comma
                char* firstQuote = strchr (comma, '"');
                if (firstQuote)
                {
                  char* nextQuote = strrchr (firstQuote+1, '"');
                  if (nextQuote)
                  { // extract text from inbetween
                    char* destIt = pc_foundValue;
                    for (char* it=firstQuote+1; it < nextQuote; it++)
                    { // extract byte by byte
                      *destIt++ = *it;
                    }
                    *destIt = 0x00;
                  }
                  else
                  { // no closing quote
                    std::cout << "No CLOSING quote in the language file!\n";
                    return false;
                  }
                }
                else
                { // no opening quote
                  std::cout << "No OPENING quote in the language file!\n";
                  return false;
                }
                /// BREAK HERE TO WATCH GOTTEN IN THE DIFFERENT CASES!
                if (strcmp (pc_id, objName) == 0)
                { // set value and break
                  //std::cout << "found language value for [" << objName << "].\n";
                  b_foundValue = true;
                  break;
                }
              }
              else
              { // no comma found, although it was not a commentary line :(
                std::cout << "No COMMA in a non-comment line in the language file!\n";
                return false;
              }
            }
            // advance to next line
            int lineLen = strlen (bufferCur)+1; // include terminating 0x00
            bufferCur += lineLen;
          }

          if (b_foundValue)
          {
            /// Do we have conflicting 'value='s now? Just put out a warning!
            if (attrIsGiven [attrValue]) std::cout <<"\n\nOverriding value & length from ["<< objName <<"]!!" << std::endl;

            // anyway, override attrValue and clear length (so it gets auto-calculated below!)
              attrIsGiven [attrValue] = true;
              strcpy (attrString [attrValue], pc_foundValue);
            attrIsGiven [attrLength] = false;
            }
          }
        }
      else
      {
        if ( checkForAllowedExecution() )
        {
          // was the standard map already created?
          // standard map get the index -1 to differ between "real" language map and normal object map
          std::map<int32_t, ObjListEntry>::iterator mit_entry = map_objNameAndID.find (0xFFFFFFFF);

          std::string str_objClassName = std::string("&iVtObject") + std::string(objName) + std::string(pc_postfix);

          if (mit_entry != map_objNameAndID.end())
          {
            mit_entry->second.insert (std::pair<uint16_t, std::string>(objID, str_objClassName));
          }
          else
          {
            ObjListEntry listEntry;
            listEntry.insert (std::pair<uint16_t, std::string>(objID, str_objClassName));
            map_objNameAndID.insert (std::pair<int32_t, ObjListEntry>(0xFFFFFFFF, listEntry));
          }
        }
      }

      // if special parsing is active and the object type is greater than maxObjectType
      // the output to the files must be done separately
      if ( pc_specialParsingPropTag && (objType >= maxObjectTypes) )
      { // need to know the object id in special parsing when data is written to files
        pc_specialParsingPropTag->setObjID (objID);
        bool b_outputOK = pc_specialParsingPropTag->outputData2FilesPiecewise(attrString, attrIsGiven, this);
        if ( !b_outputOK )
        {
          std::cout << "Error in outputData2FilesPiecewise() from object <" << node_name << "> '" << objName << "'! STOPPING PARSER! bye.\n\n";
          return false;
        }
      }
      else if (objType < maxObjectTypes)
      {
        fprintf (partFile_variables,                "IsoAgLib::iVtObject%s_c iVtObject%s%s;\n", otClassnameTable [objType], objName, pc_postfix);
        fprintf (partFile_variables_extern,  "extern IsoAgLib::iVtObject%s_c iVtObject%s%s;\n", otClassnameTable [objType], objName, pc_postfix);
        fprintf (partFile_attributes,               "const IsoAgLib::iVtObject_c::iVtObject%s_s iVtObject%s%s_sROM = {%d", otClassnameTable [objType], objName, pc_postfix, objID);
        fprintf (partFile_attributes_extern, "extern const IsoAgLib::iVtObject_c::iVtObject%s_s iVtObject%s%s_sROM;\n"   , otClassnameTable [objType], objName, pc_postfix);
        if (b_externalize)
        {
          static int splitCount=0;
          if ((splitCount & 0x1FF) == 0) splitFunction();
          splitCount++;
          fprintf (partFile_split_function,    "  iVtObject%s%s.init (&iVtObject%s%s_sROM SINGLETON_VEC_KEY_PARAMETER_VAR_WITH_COMMA);\n", objName, pc_postfix, objName, pc_postfix);
        }
        else
        {
          fprintf (partFile_functions,         "  iVtObject%s%s.init (&iVtObject%s%s_sROM SINGLETON_VEC_KEY_PARAMETER_VAR_WITH_COMMA);\n", objName, pc_postfix, objName, pc_postfix);
        }
        fprintf (partFile_defines, "#define iVtObjectID%s%s %d\n", objName, pc_postfix, objID);
      }

      /// Add explicit Button/Key includement
      if (attrIsGiven [attrInButton])
      {
        fprintf (partFile_functions_origin, "  iVtObject%s%s.setOriginBTN (&iVtObject%s);\n", objName, pc_postfix, attrString [attrInButton]);
      }
      if (attrIsGiven [attrInKey])
      {
        signed int resultat = booltoi (attrString [attrInKey]);
        if (resultat == -1)
        {
          std::cout << "Error in booltoi() from object <" << node_name << "> '" << objName << "'! STOPPING PARSER! bye.\n\n";
          return false;
        }
        fprintf (partFile_functions_origin, "  iVtObject%s%s.setOriginSKM (%s);\n", objName, pc_postfix, resultat ? "true":"false");
      }


      if ((strcmp ("NULL", attrString [attrVariable_reference]) != 0) && (strncmp (attrString [attrVariable_reference], "&iVtObject", strlen ("&iVtObject")) != 0))
      { // != 0 means an object reference is given, so add the "&iVtObject" prefix!!
        sprintf (tempString, "&iVtObject%s", getObjNameWithPoolIdent (attrString [attrVariable_reference]).c_str());
        sprintf (attrString [attrVariable_reference], "%s", tempString);
      }
      if ((strcmp ("NULL", attrString [attrTarget_value_variable_reference]) != 0) && (strncmp (attrString [attrTarget_value_variable_reference], "&iVtObject", strlen ("&iVtObject")) != 0))
      { // != 0 means an object reference is given, so add the "&iVtObject" prefix!!
        sprintf (tempString, "&iVtObject%s", getObjNameWithPoolIdent (attrString [attrTarget_value_variable_reference]).c_str());
        sprintf (attrString [attrTarget_value_variable_reference], "%s", tempString);
      }

      //! @todo To be enabled when handling cases where only IDs are used in XMLs. Not 100% supported now, even if the
      //! following "convertIdReferencesToNameReferences();" call wouldn't be commented out!
      //convertIdReferencesToNameReferences();
      // ###########################################
      // ### Print out inidivual object stuff... ###
      // ###########################################
      switch (objType)
      {
        case otWorkingset:
        {
          signed int retSelectable = booltoi (attrString [attrSelectable]);
          if (retSelectable == -1)
          {
            std::cout << "Error in booltoi() from object <" << node_name << "> '" << objName << "'! STOPPING PARSER! bye.\n\n";
            return false;
          }
          if (!attrIsGiven [attrActive_mask] && (retSelectable == 1))
          {
            std::cout << "YOU NEED TO SPECIFY THE active_mask= ATTRIBUTE FOR THE <workingset> OBJECT (if selectable='yes'!)! STOPPING PARSER! bye.\n\n";
            return false;
          }
          if (retSelectable == 0)
            fprintf (partFile_attributes, ", %d, %d, NULL",         colortoi (attrString [attrBackground_colour]), retSelectable);
          else
            fprintf (partFile_attributes, ", %d, %d, &iVtObject%s", colortoi (attrString [attrBackground_colour]), retSelectable, attrString [attrActive_mask]);

          if (!vt2iso_c::sb_WSFound)
          {
            vt2iso_c::sb_WSFound = true;
            ui16_WSObjID = objID;
          }
          break;
        }

        case otDatamask:
          if ( (strcmp ("NULL", attrString [attrSoft_key_mask]) == 0)  || (strcmp("65535",  attrString [attrSoft_key_mask]) == 0))
            fprintf (partFile_attributes, ", %d, NULL", colortoi (attrString [attrBackground_colour]));
          else
            fprintf (partFile_attributes, ", %d, &iVtObject%s", colortoi (attrString [attrBackground_colour]), attrString [attrSoft_key_mask]);
          break;

        case otAlarmmask:
        {
          signed long int retPrio = prioritytoi(attrString [attrPriority]);
          signed long int retSignal = acousticsignaltoi (attrString [attrAcoustic_signal]);
          if ((retPrio == -1) || (retSignal == -1))
          {
            if (retPrio == -1)
              std::cout << "Error in prioritytoi()  from object <" << node_name << "> '" << objName << "'! STOPPING PARSER! bye.\n\n";
            else
              std::cout << "Error in acousticsignaltoi()  from object <" << node_name << "> '" << objName << "'! STOPPING PARSER! bye.\n\n";
            return false;
          }
          if ( (strcmp ("NULL", attrString [attrSoft_key_mask]) == 0) || (strcmp("65535",  attrString [attrSoft_key_mask]) == 0))
            fprintf (partFile_attributes, ", %d, NULL, %d, %d", colortoi (attrString [attrBackground_colour]), atoi (attrString [attrPriority]), atoi (attrString [attrAcoustic_signal]));
          else
            fprintf (partFile_attributes, ", %d, &iVtObject%s, %d, %d", colortoi (attrString [attrBackground_colour]), attrString [attrSoft_key_mask], (unsigned int)retPrio, (unsigned int)retSignal);
          break;
        }

        case otContainer:
        {
          if (!(attrIsGiven [attrWidth] && attrIsGiven [attrHeight]))
          {
            std::cout << "YOU NEED TO SPECIFY THE width= AND height= ATTRIBUTES FOR THE <container> OBJECT '" << objName << "'! STOPPING PARSER! bye.\n\n";
            return false;
          }
          signed int retHidden = booltoi (attrString [attrHidden]);
          if (retHidden == -1)
          {
            std::cout << "Error in booltoi()  from object <" << node_name << "> '" << objName << "'! STOPPING PARSER! bye.\n\n";
            return false;
          }
          fprintf (partFile_attributes, ", %s, %s, %d", attrString [attrWidth], attrString [attrHeight], (unsigned int)retHidden);
          break;
        }

        case otSoftkeymask:
          fprintf (partFile_attributes, ", %d", colortoi (attrString [attrBackground_colour]));
          break;

        case otKey:
          if (!attrIsGiven [attrKey_code]) getKeyCode ();
          fprintf (partFile_attributes, ", %d, %s", colortoi (attrString [attrBackground_colour]), attrString [attrKey_code]);
          fprintf (partFile_defines, "#define vtKeyCode%s %d\n", objName, atoi (attrString [attrKey_code])); // like in otButton
          break;

        case otButton:
          if (!attrIsGiven [attrKey_code]) getKeyCode ();
          if (!(attrIsGiven [attrWidth] && attrIsGiven [attrHeight]))
          {
            std::cout << "YOU NEED TO SPECIFY THE width= AND height= ATTRIBUTES FOR THE <key> OBJECT '" << objName << "'! STOPPING PARSER! bye.\n\n";
            return false;
          }
          fprintf (partFile_attributes, ", %s, %s, %d, %d, %s, %d", attrString [attrWidth], attrString [attrHeight], colortoi (attrString [attrBackground_colour]), colortoi (attrString [attrBorder_colour]), attrString [attrKey_code], buttonoptiontoi (attrString [attrOptions]));
          fprintf (partFile_defines, "#define vtKeyCode%s %d\n", objName, atoi (attrString [attrKey_code])); // like in otKey
          break;

        case otInputboolean:
        {
          if (!(attrIsGiven [attrWidth] && attrIsGiven [attrForeground_colour]))
          {
            std::cout << "YOU NEED TO SPECIFY THE width= AND foreground_colour= ATTRIBUTES FOR THE <inputboolean> OBJECT '" << objName << "'! STOPPING PARSER! bye.\n\n";
            return false;
          }
          if (!attrIsGiven [attrValue])
            sprintf (attrString [attrValue], "0");

          signed int retEnabled = booltoi (attrString [attrEnabled]);
          if (retEnabled == -1)
          {
            std::cout << "Error in booltoi() from object <" << node_name << "> '" << objName << "'! STOPPING PARSER! bye.\n\n";
            return false;
          }
          fprintf (partFile_attributes, ", %d, %s, &iVtObject%s, %s, %s, %d", colortoi (attrString [attrBackground_colour]), attrString [attrWidth], getObjNameWithPoolIdent (attrString [attrForeground_colour]).c_str(), attrString [attrVariable_reference], attrString [attrValue], (unsigned int)retEnabled);
          break;
        }

        case otInputstring:
        {
          if (!attrIsGiven [attrValue])
          { // no value given
            if (!(attrIsGiven [attrWidth] && attrIsGiven [attrHeight] && attrIsGiven [attrFont_attributes] && attrIsGiven [attrLength] && attrIsGiven [attrEnabled]))
            {
              std::cout << "YOU NEED TO SPECIFY THE width= AND height= AND font_attributes= AND length= AND enabled= ATTRIBUTES FOR THE <inputstring> OBJECT '" << objName << "' IF NO VALUE IS GIVEN! STOPPING PARSER! bye.\n\n";
              return false;
            }
          /// @todo MAYBE WARN/FAIL HERE WHEN NO LANGUAGE IS GIVEN BUT NO ENTRY IS DEFINED????????
          /*
            if (arrs_language[i].valueBuffer == NULL)
            { // open failed.
            std::cout << "\n\nLanguage file for code=\"" << arrs_language[i].code[0] << arrs_language[i].code[1] << "\" in object ["<< objName <<"] not found! STOPPING PARSER! bye.\n\n";
            return false;
          }*/
            sprintf (attrString [attrValue], "NULL");
          }
          else
          { // value given
            signed int ret = strlenUnescaped (attrString [attrValue]);
            if (ret == -1)
            {
              std::cout << "Error in strlenUnescaped() from object <" << node_name << "> '" << objName << "'! STOPPING PARSER! bye.\n\n";
              return false;
            }
            //auto-calculate string length
            if (!attrIsGiven [attrLength]) { sprintf (attrString [attrLength], "%d", ret); attrIsGiven [attrLength] = true; }
            if (!(attrIsGiven [attrWidth] && attrIsGiven [attrHeight] && attrIsGiven [attrFont_attributes] && attrIsGiven [attrEnabled]))
            {
              std::cout << "YOU NEED TO SPECIFY THE width= AND height= AND font_attributes= AND enabled = ATTRIBUTES FOR THE <inputstring> OBJECT '" << objName << "'! STOPPING PARSER! bye.\n\n";
              return false;
            }
            if (!copyWithQuoteAndLength (tempString, attrString [attrValue], atoi (attrString [attrLength])))
              return false;

            sprintf (attrString [attrValue], "%s", tempString);
          }
          signed int retEnabled = booltoi(attrString [attrEnabled]);
          signed int retJustification = horizontaljustificationtoi (attrString [attrHorizontal_justification]);
          if (retEnabled == -1 || retJustification == -1)
          {
            if (retEnabled == -1)
              std::cout << "Error in booltoi() from object <" << node_name << "> '" << objName << "'! STOPPING PARSER! bye.\n\n";
            else
              std::cout << "Error in horizontaljustificationtoi() from object <" << node_name << "> '" << objName << "'! STOPPING PARSER! bye.\n\n";
            return false;
          }
          if ( (!attrIsGiven [attrInput_attributes]) || (strcmp( attrString[attrInput_attributes], "65535")==0) )
          {
            sprintf (attrString [attrInput_attributes], "NULL");

            fprintf (partFile_attributes, ", %s, %s, %d, &iVtObject%s, %s, %d, %s, %d, %s, %s, %d", attrString [attrWidth], attrString [attrHeight], colortoi (attrString [attrBackground_colour]), getObjNameWithPoolIdent (attrString [attrFont_attributes]).c_str(), getObjNameWithPoolIdent (attrString [attrInput_attributes]).c_str(), stringoptionstoi (attrString [attrOptions]), attrString [attrVariable_reference], (unsigned int)retJustification, attrString [attrLength], attrString [attrValue], (unsigned int)retEnabled );
          }
          else
          {
            fprintf (partFile_attributes, ", %s, %s, %d, &iVtObject%s, &iVtObject%s, %d, %s, %d, %s, %s, %d", attrString [attrWidth], attrString [attrHeight], colortoi (attrString [attrBackground_colour]), getObjNameWithPoolIdent (attrString [attrFont_attributes]).c_str(), getObjNameWithPoolIdent (attrString [attrInput_attributes]).c_str(), stringoptionstoi (attrString [attrOptions]), attrString [attrVariable_reference], (unsigned int)retJustification, attrString [attrLength], attrString [attrValue], (unsigned int)retEnabled );
          }

          break;
        }

        case otInputnumber:
        {
          if (!(attrIsGiven [attrWidth] && attrIsGiven [attrHeight] && attrIsGiven [attrFont_attributes] && attrIsGiven [attrMin_value] && attrIsGiven [attrMax_value]
                && attrIsGiven [attrOffset] && attrIsGiven [attrScale] && attrIsGiven [attrNumber_of_decimals] && attrIsGiven [attrFormat]))
          {
            std::cout << "YOU NEED TO SPECIFY THE width= AND height= AND font_attributes= AND min_value= AND max_value= "
                        << "AND offset= AND scale= AND number_of_decimals= AND format= ATTRIBUTES FOR THE <inputnumber> OBJECT "
                        << objName << "! STOPPING PARSER! bye.\n\n";
            return false;
          }
          if (!attrIsGiven [attrValue])
            sprintf (attrString [attrValue], "0");
          fprintf (partFile_attributes, ", %s, %s, %d, &iVtObject%s, %d, %s, %sUL, %sUL, %sUL", attrString [attrWidth], attrString [attrHeight],
                   colortoi (attrString [attrBackground_colour]), getObjNameWithPoolIdent (attrString [attrFont_attributes]).c_str(),
                   inputobjectoptiontoi (attrString [attrOptions]), attrString [attrVariable_reference], attrString [attrValue],
                   attrString [attrMin_value], attrString [attrMax_value]);
          if ( strchr( attrString [attrOffset], 'L' ) != NULL )
          { // contains already a number type specifier
            fprintf (partFile_attributes, ", %s", attrString [attrOffset] );
          }
          else
          { // place "L" for type specifier
            fprintf (partFile_attributes, ", %sL", attrString [attrOffset] );
          }

          signed int retInputOption = inputobjectoptiontoi (attrString [attrInputObjectOptions]);
          signed int retFormat = formattoi (attrString [attrFormat]);
          signed int retJust = horizontaljustificationtoi (attrString [attrHorizontal_justification]);
          if ((retInputOption == -1) || (retFormat == -1) || (retJust == -1))
          {
            if (retInputOption == -1)
              std::cout << "Error in inputobjectoptiontoi() from object <" << node_name << "> '" << objName << "'! STOPPING PARSER! bye.\n\n";

            if (retFormat == -1)
              std::cout << "Error in formattoi() from object <" << node_name << "> '" << objName << "'! STOPPING PARSER! bye.\n\n";

            if (retJust == -1)
              std::cout << "Error in horizontaljustificationtoi() from object <" << node_name << "> '" << objName << "'! STOPPING PARSER! bye.\n\n";
            return false;
          }

          fprintf (partFile_attributes, ", %s, %s, %d, %d, %d", attrString [attrScale], attrString [attrNumber_of_decimals],
                   (unsigned int)retFormat, (unsigned int)retJust, (unsigned int)retInputOption);
          break;
        }

        case otInputlist:
        {
          if (!(attrIsGiven [attrWidth] && attrIsGiven [attrHeight]))
          {
            std::cout << "YOU NEED TO SPECIFY THE width= AND height= ATTRIBUTES FOR THE <inputlist> OBJECT '" << objName << "'! STOPPING PARSER! bye.\n\n";
            return false;
          }
          if (!attrIsGiven [attrValue])
            sprintf (attrString [attrValue], "0");

          signed int retEnabled = booltoi (attrString [attrEnabled]);
          if (retEnabled == -1)
          {
            std::cout << "Error in booltoi() from object <" << node_name << "> '" << objName << "'! STOPPING PARSER! bye.\n\n";
            return false;
          }
          fprintf (partFile_attributes, ", %s, %s, %s, %s, %d", attrString [attrWidth], attrString [attrHeight], attrString [attrVariable_reference],
                   attrString [attrValue], (unsigned int)retEnabled);
          break;
        }

        case otOutputlist:
        {
          if (!(attrIsGiven [attrWidth] && attrIsGiven [attrHeight]))
          {
            std::cout << "YOU NEED TO SPECIFY THE width= AND height= ATTRIBUTES FOR THE <outputlist> OBJECT '" << objName << "'! STOPPING PARSER! bye.\n\n";
            return false;
          }
          if (!attrIsGiven [attrValue])
            sprintf (attrString [attrValue], "0");

          fprintf (partFile_attributes, ", %s, %s, %s, %s", attrString [attrWidth], attrString [attrHeight], attrString [attrVariable_reference],
                   attrString [attrValue]);
          break;
        }

        case otOutputstring:
        {
          if (!attrIsGiven [attrValue])
          {
            // Variable Reference
            if (!(attrIsGiven [attrWidth] && attrIsGiven [attrHeight] && attrIsGiven [attrFont_attributes] && attrIsGiven [attrLength]))
            {
              std::cout << "YOU NEED TO SPECIFY THE width= AND height= AND font_attributes= AND length= ATTRIBUTES FOR THE <outputstring> OBJECT  from object '" << objName << "' WHEN VALUE IS SET BY REFERENCE! STOPPING PARSER! bye.\n\n";
              return false;
            }
            sprintf (attrString [attrValue], "NULL");
          }
          else
          {
            // Direct Value
            if (!attrIsGiven [attrLength])
            {
              signed int ret = strlenUnescaped (attrString [attrValue]);
              if (ret == -1)
              {
                std::cout << "Error in strlenUnescaped() from object <" << node_name << "> '" << objName << "'! STOPPING PARSER! bye.\n\n";
                return false;
              }
              // Auto-calculate Length-field
              sprintf (attrString [attrLength], "%d", ret); attrIsGiven [attrLength] = true;
            }
            if (!(attrIsGiven [attrWidth] && attrIsGiven [attrHeight] && attrIsGiven [attrFont_attributes] && attrIsGiven [attrLength]))
            {
              std::cout << "YOU NEED TO SPECIFY THE width= AND height= AND font_attributes= AND length= ATTRIBUTES FOR THE <outputstring> OBJECT '" << objName << "'! STOPPING PARSER! bye.\n\n";
              return false;
            }
            if (!copyWithQuoteAndLength (tempString, attrString [attrValue], atoi (attrString [attrLength])))
              return false;

            sprintf (attrString [attrValue], "%s", tempString);
          }
          signed int retJust = horizontaljustificationtoi (attrString [attrHorizontal_justification]);
          if (retJust == -1)
          {
            std::cout << "Error in horizontaljustificationtoi() from object <" << node_name << "> '" << objName << "'! STOPPING PARSER! bye.\n\n";
            return false;
          }
          fprintf (partFile_attributes, ", %s, %s, %d, &iVtObject%s, %d, %s, %d, %s, %s", attrString [attrWidth], attrString [attrHeight],
                   colortoi (attrString [attrBackground_colour]), getObjNameWithPoolIdent (attrString [attrFont_attributes]).c_str(),
                   stringoptionstoi (attrString [attrOptions]), attrString [attrVariable_reference],
                   (unsigned int)retJust, attrString [attrLength], attrString [attrValue]);
      //    printf ("%s --- %d\n", attrString [attrOptions], optionstoi (attrString [attrOptions]));
          break;
        }

        case otOutputnumber:
        {
          if (!(attrIsGiven [attrWidth] && attrIsGiven [attrHeight] && attrIsGiven [attrFont_attributes] && attrIsGiven [attrOffset] && attrIsGiven [attrScale]
                && attrIsGiven [attrNumber_of_decimals] && attrIsGiven [attrFormat]))
          {
            std::cout << "YOU NEED TO SPECIFY THE width= AND height= AND font_attributes= AND offset= AND scale= AND number_of_decimals= AND format= ATTRIBUTES FOR THE <outputnumber> OBJECT '" << objName << "''! STOPPING PARSER! bye.\n\n";
            return false;
          }
          if (!attrIsGiven [attrValue])
            sprintf (attrString [attrValue], "0");
          fprintf (partFile_attributes, ", %s, %s, %d, &iVtObject%s, %d, %s, %sUL", attrString [attrWidth], attrString [attrHeight],
                   colortoi (attrString [attrBackground_colour]), getObjNameWithPoolIdent (attrString [attrFont_attributes]).c_str(),
                   numberoptionstoi (attrString [attrOptions]), attrString [attrVariable_reference],
                   attrString [attrValue]);
          if ( strchr( attrString [attrOffset], 'L' ) != NULL )
          { // offset has already type indication -> don't add additional "L"
            fprintf (partFile_attributes, ", %s", attrString [attrOffset]);
          }
          else
          {
            fprintf (partFile_attributes, ", %sL", attrString [attrOffset]);
          }
          signed int retFormat = formattoi (attrString [attrFormat]);
          signed int retJust = horizontaljustificationtoi (attrString [attrHorizontal_justification]);
          if ((retFormat == -1) || (retJust == -1))
          {
            if (retFormat == -1)
              std::cout << "Error in formattoi() from object <" << node_name << "> '" << objName << "'! STOPPING PARSER! bye.\n\n";

            if (retJust == -1)
              std::cout << "Error in horizontaljustificationtoi() from object <" << node_name << "> '" << objName << "'! STOPPING PARSER! bye.\n\n";
            return false;
          }
          fprintf (partFile_attributes, ", %s, %s, %d, %d", attrString [attrScale], attrString [attrNumber_of_decimals],
                   (unsigned int)retFormat, (unsigned int)retJust);
          break;
        }

        case otLine:
          if (!(attrIsGiven [attrLine_attributes] && attrIsGiven [attrWidth] && attrIsGiven [attrHeight]))
          {
            std::cout << "YOU NEED TO SPECIFY THE line_attributes= AND width= AND height= ATTRIBUTES FOR THE <line> OBJECT '" << objName << "'! STOPPING PARSER! bye.\n\n";
            return false;
          }
          fprintf (partFile_attributes, ", &iVtObject%s, %s, %s, %d", getObjNameWithPoolIdent (attrString [attrLine_attributes]).c_str(),
                   attrString [attrWidth], attrString [attrHeight], linedirectiontoi (attrString [attrLine_direction]));
          break;

        case otRectangle:
          if (!(attrIsGiven [attrLine_attributes] && attrIsGiven [attrWidth] && attrIsGiven [attrHeight]))
          {
            std::cout << "YOU NEED TO SPECIFY THE line_attributes= AND width= AND height= ATTRIBUTES FOR THE <rectangle> OBJECT '" << objName << "'! STOPPING PARSER! bye.\n\n";
            return false;
          }
          if (!attrIsGiven [attrLine_suppression])
            sprintf (attrString [attrLine_suppression], "0");

          if ( (!attrIsGiven [attrFill_attributes]) || (strcmp( attrString[attrFill_attributes], "65535")==0) )
            sprintf (attrString [attrFill_attributes], "NULL");
          else
          {
            sprintf (tempString, "&iVtObject%s", getObjNameWithPoolIdent (attrString[attrFill_attributes]).c_str());
            sprintf (attrString [attrFill_attributes], "%s", tempString);
          }

          //else
          // sprintf (attrString [attrFill_attributes], "&vtObject%s", attrString[attrFill_attributes]);
          fprintf (partFile_attributes, ", &iVtObject%s, %s, %s, %d, %s", getObjNameWithPoolIdent (attrString [attrLine_attributes]).c_str(),
                   attrString [attrWidth], attrString [attrHeight], linesuppressiontoi (attrString [attrLine_suppression]),
                   attrString[attrFill_attributes]);
          break;

        case otEllipse:
          if (!(attrIsGiven [attrLine_attributes] && attrIsGiven [attrWidth] && attrIsGiven [attrHeight] && attrIsGiven [attrStart_angle] && attrIsGiven [attrEnd_angle]))
          {
            std::cout << "YOU NEED TO SPECIFY THE line_attributes= AND width= AND height= AND start_angle= AND end_angle= ATTRIBUTES FOR THE <ellipse> OBJECT from object '" << objName << "'! STOPPING PARSER! bye.\n\n";
            return false;
          }
          if (atoi(attrString[attrStart_angle]) > 180 || atoi(attrString[attrEnd_angle]) > 180)
          {
            std::cout << "start_angle= AND end_angle= FOR THE <ellipse> OBJECT '" << objName << "' NEED TO HAVE A VALUE LESS OR EQUAL TO 180! STOPPING PARSER! bye.\n\n";
            return false;
          }
          if (!attrIsGiven [attrEllipse_type])
            sprintf (attrString [attrEllipse_type], "0");

          if ( (!attrIsGiven [attrFill_attributes]) || (strcmp( attrString[attrFill_attributes], "65535")==0) )
            sprintf (attrString [attrFill_attributes], "NULL");
          else
          {
            sprintf (tempString, "&iVtObject%s", getObjNameWithPoolIdent (attrString[attrFill_attributes]).c_str());
            sprintf (attrString [attrFill_attributes], "%s", tempString);
          }
          fprintf (partFile_attributes, ", &iVtObject%s, %s, %s, %d, %s, %s, %s", getObjNameWithPoolIdent (attrString [attrLine_attributes]).c_str(),
                   attrString [attrWidth], attrString [attrHeight], ellipsetypetoi (attrString [attrEllipse_type]),
                   attrString[attrStart_angle], attrString[attrEnd_angle], attrString[attrFill_attributes]);
          break;

        case otPolygon:
          if (!(attrIsGiven [attrWidth] && attrIsGiven [attrHeight] && attrIsGiven [attrLine_attributes]))
          {
            std::cout << "YOU NEED TO SPECIFY THE width= AND height= AND line_attributes= ATTRIBUTES FOR THE <polygon> OBJECT '" << objName << "'! STOPPING PARSER! bye.\n\n";
            return false;
          }
          if (!attrIsGiven [attrPolygon_type])
            sprintf (attrString [attrPolygon_type], "0");

          if ( (!attrIsGiven [attrFill_attributes]) || (strcmp( attrString[attrFill_attributes], "65535")==0) )
            sprintf (attrString [attrFill_attributes], "NULL");
          else
          {
            sprintf (tempString, "&iVtObject%s", getObjNameWithPoolIdent (attrString[attrFill_attributes]).c_str());
            sprintf (attrString [attrFill_attributes], "%s", tempString);
          }
          fprintf (partFile_attributes, ", %s, %s, &iVtObject%s, %s, %d", attrString [attrWidth], attrString [attrHeight],
                   getObjNameWithPoolIdent (attrString [attrLine_attributes]).c_str(), attrString[attrFill_attributes],
                   polygontypetoi (attrString [attrPolygon_type]));
          break;

        case otMeter:
          if (!(attrIsGiven [attrWidth] && attrIsGiven [attrNeedle_colour] && attrIsGiven [attrBorder_colour] && attrIsGiven [attrArc_and_tick_colour] && attrIsGiven [attrNumber_of_ticks] && attrIsGiven[attrStart_angle] && attrIsGiven[attrEnd_angle] && attrIsGiven [attrMin_value] && attrIsGiven [attrMax_value]))
          {
            std::cout << "YOU NEED TO SPECIFY THE width= AND needle_colour= AND border_colour= AND arc_and_tick_colour= AND number_of_ticks= AND start_angle= AND end_angle= AND min_value= AND max_value= ATTRIBUTES FOR THE <meter> OBJECT '" << objName << "'! STOPPING PARSER! bye.\n\n";
            return false;
          }
          if (atoi(attrString[attrStart_angle]) > 180 || atoi(attrString[attrEnd_angle]) > 180)
          {
            std::cout << "start_angle= AND end_angle= FOR THE <meter> OBJECT '" << objName << "' NEED TO HAVE A VALUE LESS OR EQUAL TO 180! STOPPING PARSER! bye.\n\n";
            return false;
          }
          if (!attrIsGiven [attrValue])
            sprintf (attrString [attrValue], "0");

          if (!attrIsGiven [attrOptions])
            sprintf(attrString[attrOptions], "0");

          fprintf (partFile_attributes, ", %s, %d, %d, %d, %d, %s, %s, %s, %s, %s, %s, %s", attrString [attrWidth],
                   colortoi(attrString[attrNeedle_colour]), colortoi (attrString [attrBorder_colour]),
                   colortoi (attrString [attrArc_and_tick_colour]), meteroptionstoi (attrString [attrOptions]),
                   attrString [attrNumber_of_ticks], attrString[attrStart_angle], attrString[attrEnd_angle], attrString [attrMin_value],
                   attrString [attrMax_value], attrString [attrVariable_reference], attrString [attrValue]);
          break;

        case otLinearbargraph:
          if (!(attrIsGiven [attrWidth] && attrIsGiven [attrHeight] && attrIsGiven [attrColour] && attrIsGiven [attrTarget_line_colour]
                && attrIsGiven [attrNumber_of_ticks] && attrIsGiven [attrMin_value] && attrIsGiven [attrMax_value]))
          {
            std::cout << "YOU NEED TO SPECIFY THE width= AND height= AND colour= AND target_line_colour= AND number_of_ticks= AND min_value= AND max_value= ATTRIBUTES FOR THE <linearbargraph> OBJECT '" << objName << "'! STOPPING PARSER! bye.\n\n";
            return false;
          }
          if (!attrIsGiven [attrValue])
            sprintf (attrString [attrValue], "0");

          if (!attrIsGiven [attrTarget_value])
            sprintf (attrString [attrTarget_value], "0");

          fprintf (partFile_attributes, ", %s, %s, %d, %d, %d, %s, %s, %s, %s, %s, %s, %s", attrString [attrWidth], attrString [attrHeight],
                   colortoi (attrString [attrColour]), colortoi (attrString [attrTarget_line_colour]),
                   linearbargraphoptionstoi (attrString [attrOptions]), attrString [attrNumber_of_ticks], attrString [attrMin_value],
                   attrString [attrMax_value], attrString [attrVariable_reference], attrString [attrValue],
                   attrString [attrTarget_value_variable_reference], attrString [attrTarget_value]);
          break;

        case otArchedbargraph:
          if (!(attrIsGiven [attrWidth] && attrIsGiven [attrHeight] && attrIsGiven [attrColour] && attrIsGiven [attrTarget_line_colour] && attrIsGiven [attrStart_angle] && attrIsGiven [attrEnd_angle] && attrIsGiven[attrBar_graph_width] && attrIsGiven [attrMin_value] && attrIsGiven [attrMax_value]))
          {
            std::cout << "YOU NEED TO SPECIFY THE width= AND height= AND colour= AND target_line_colour= AND start_angle= AND end_angle= AND bar_graph_width AND min_value= AND max_value= ATTRIBUTES FOR THE <archedbargraph> OBJECT '" << objName << "'! STOPPING PARSER! bye.\n\n";
            return false;
          }
          if (atoi(attrString[attrStart_angle]) > 180 || atoi(attrString[attrEnd_angle]) > 180)
          {
            std::cout << "start_angle= AND end_angle= FOR THE <archedbargraph> OBJECT '" << objName << "' NEED TO HAVE A VALUE LESS OR EQUAL TO 180! STOPPING PARSER! bye.\n\n";
            return false;
          }
          if (!attrIsGiven [attrValue])
            sprintf (attrString [attrValue], "0");

          if (!attrIsGiven [attrTarget_value])
            sprintf (attrString [attrTarget_value], "0");

          if (!attrIsGiven [attrOptions])
            sprintf (attrString [attrOptions], "0");

          fprintf (partFile_attributes, ", %s, %s, %d, %d, %d, %s, %s, %s, %s, %s, %s, %s, %s, %s", attrString [attrWidth],
                   attrString [attrHeight], colortoi (attrString [attrColour]), colortoi (attrString [attrTarget_line_colour]),
                   archedbargraphoptionstoi (attrString [attrOptions]), attrString[attrStart_angle], attrString[attrEnd_angle],
                   attrString[attrBar_graph_width], attrString [attrMin_value], attrString [attrMax_value],
                   attrString [attrVariable_reference], attrString [attrValue],
                   attrString [attrTarget_value_variable_reference], attrString [attrTarget_value]);
          break;

        case otPicturegraphic:
          // check moved above!
          fprintf (partFile_attributes, ", %d, %d,%d, %d, %d, %d", deXwidth, deXactualWidth, deXactualHeight, deXcolorDepth,
                   objBitmapOptions, deXtransCol);
          break;

        case otNumbervariable:
          if (!attrIsGiven [attrValue])
            sprintf (attrString [attrValue], "0");

          fprintf (partFile_attributes, ", %sUL", attrString [attrValue]);
          break;

        case otStringvariable:
          if (!attrIsGiven [attrValue])
          {
            if (!(attrIsGiven [attrLength]))
            {
              std::cout << "YOU NEED TO SPECIFY THE length= ATTRIBUTE FOR THE <stringvariable> OBJECT '" << objName << "' (as no value= is given)! STOPPING PARSER! bye.\n\n";
              return false;
            }
            sprintf (attrString [attrValue], "NULL");
          }
          else
          {
            signed int ret = strlenUnescaped (attrString [attrValue]);
            if (ret == -1)
            {
              std::cout << "Error in strlenUnescaped() from object <" << node_name << "> '" << objName << "'! STOPPING PARSER! bye.\n\n";
              return false;
            }
            //auto-calculate length
            if (!attrIsGiven [attrLength])
            {
              sprintf (attrString [attrLength], "%d", ret);
              attrIsGiven [attrLength] = true;
            }

            if (!copyWithQuoteAndLength (tempString, attrString [attrValue], atoi (attrString [attrLength])))
              return false;

            sprintf (attrString [attrValue], "%s", tempString);
          }
          fprintf (partFile_attributes, ", %s, %s", attrString [attrLength], attrString [attrValue]);
          break;

        case otFontattributes:
        {
          if (!attrIsGiven [attrFont_type])
          {
            std::cout << "INFORMATION: WITH THAT VERSION OF VT2ISO YOU NEED TO SPECIFY THE font_type= ATTRIBUTE FOR THE <fontattributes> OBJECT '" << objName << "'! \n \
                VALID VALUES ARE latin1, latin2, latin4, latin5, latin7, latin9 or proprietary! STOPPING PARSER! bye.\n\n";
            return false;
          }

          if (!(attrIsGiven [attrFont_colour] && attrIsGiven [attrFont_size]))
          {
            std::cout << "YOU NEED TO SPECIFY THE font_colour= AND font_size= AND font_type= ATTRIBUTE FOR THE <fontattributes> OBJECT '" << objName << "'! STOPPING PARSER! bye.\n\n";
            return false;
          }
          signed int ret = fonttypetoi (attrString [attrFont_type]);
          signed int retFontSize = fontsizetoi (attrString [attrFont_size]);
          if ((ret == -1) || (retFontSize == -1))
          {
            if (ret == -1)
              std::cout << "Error in fonttypetoi() from object <" << node_name << "> '" << objName << "'! STOPPING PARSER! bye.\n\n";

            if (retFontSize == -1)
              std::cout << "Error in fontsizetoi() from object <" << node_name << "> '" << objName << "'! STOPPING PARSER! bye.\n\n";
            return false;
          }

          fprintf (partFile_attributes, ", %d, %d, %d, %d", colortoi (attrString [attrFont_colour]), (unsigned int)retFontSize,
                   (unsigned int)ret, fontstyletoi (attrString [attrFont_style]));
          break;
        }

        case otLineattributes:
          if (!(attrIsGiven [attrLine_colour] && attrIsGiven [attrLine_width] && attrIsGiven [attrLine_art]))
          {
            std::cout << "YOU NEED TO SPECIFY THE line_colour= AND line_width= AND line_art= ATTRIBUTE FOR THE <lineattributes> OBJECT '" << objName << "'! STOPPING PARSER! bye.\n\n";
            return false;
          }
          fprintf (partFile_attributes, ", %d, %s, 0x%x", colortoi (attrString [attrLine_colour]), attrString [attrLine_width],
                   linearttoi (attrString [attrLine_art]));
          break;

        case otFillattributes:
          if (!(attrIsGiven [attrFill_colour]))
          {
            std::cout << "YOU NEED TO SPECIFY THE fill_colour= ATTRIBUTE FOR THE <fillattributes> OBJECT '" << objName << "'! STOPPING PARSER! bye.\n\n";
            return false;
          }
          if (!attrIsGiven [attrFill_type])
            sprintf (attrString [attrFill_type], "0");

          if(!attrIsGiven [attrFill_pattern] || (strcmp (attrString[attrFill_pattern], "65535") == 0))
          {
            sprintf (attrString[attrFill_pattern], "NULL");
            fprintf (partFile_attributes, ",%d, %d, %s", filltypetoi (attrString[attrFill_type]), colortoi (attrString [attrFill_colour]),
                     getObjNameWithPoolIdent (attrString [attrFill_pattern]).c_str());
          }
          else
            fprintf (partFile_attributes, ",%d, %d, &iVtObject%s", filltypetoi (attrString[attrFill_type]),
                     colortoi (attrString[attrFill_colour]), getObjNameWithPoolIdent (attrString [attrFill_pattern]).c_str());
          break;

        case otInputattributes:
          if (!attrIsGiven [attrValidation_type])
            sprintf (attrString [attrValidation_type], "0");

          if (!attrIsGiven [attrValidation_string])
          {
            if (!(attrIsGiven [attrLength]))
            {
              std::cout << "YOU NEED TO SPECIFY THE length= ATTRIBUTE FOR THE <inputattribute> OBJECT '" << objName << "'! STOPPING PARSER! bye.\n\n";
              return false;
            }
            sprintf (attrString [attrValidation_string], "NULL");
          }
          else
          {
            signed int ret = strlenUnescaped (attrString [attrValidation_string]);
            if (ret == -1)
            {
              std::cout << "Error in strlenUnescaped() from object <" << node_name << "> '" << objName << "'! STOPPING PARSER! bye.\n\n";
              return false;
            }
            // auto-calculate string-length
            if (!attrIsGiven [attrLength]) { sprintf (attrString [attrLength], "%d", ret); attrIsGiven [attrLength] = true; }
            sprintf (tempString, "\"%s\"", attrString [attrValidation_string]);
            sprintf (attrString [attrValidation_string], "%s", tempString);
          }
          fprintf (partFile_attributes, ", %d, %s, %s", validationtypetoi (attrString [attrValidation_type]), attrString [attrLength],
                   attrString [attrValidation_string]);
          break;

        case otObjectpointer:
          if ( (!attrIsGiven [attrValue]) || (strcmp( attrString[attrValue], "65535")==0) )
          {
            sprintf (attrString [attrValue], "NULL");
            attrIsGiven [attrValue] = true;
          }
          if (strcmp ("NULL", attrString [attrValue]) != 0)
          { // != 0 means an object reference is given, so add the "&iVtObject" prefix!!
            sprintf (tempString, "&iVtObject%s", getObjNameWithPoolIdent (attrString [attrValue]).c_str());
            sprintf (attrString [attrValue], "%s", tempString);
          }
          fprintf (partFile_attributes, ", %s", attrString [attrValue]);
          break;

        case otMacro:
          fprintf (partFile_attributes, ", %s", attrString [attrNumber_of_bytes]);
          break;

        case otAuxiliaryfunction:
          if (!(attrIsGiven [attrBackground_colour] && attrIsGiven[attrFunction_type]))
          {
            std::cout << "YOU NEED TO SPECIFY THE background_colour= and function_type= ATTRIBUTE FOR THE <auxiliaryfunction> OBJECT '" << objName << "'! STOPPING PARSER! bye.\n\n";
            return false;
          }
          fprintf (partFile_attributes, ", %d, %d", colortoi (attrString [attrBackground_colour]),
                   auxfunctiontyptetoi(attrString [attrFunction_type]));
          break;

        case otAuxiliaryinput:
          if (!(attrIsGiven [attrBackground_colour] && attrIsGiven[attrFunction_type] && attrIsGiven[attrInput_id]) )
          {
            std::cout << "YOU NEED TO SPECIFY THE background_colour= and function_type= and input_id= ATTRIBUTE FOR THE <auxiliaryinput> OBJECT '" << objName << "'! STOPPING PARSER! bye.\n\n";
            return false;
          }
          fprintf (partFile_attributes, ", %d, %d, %s", colortoi (attrString [attrBackground_colour]),
                   auxfunctiontyptetoi(attrString [attrFunction_type]), attrString[attrInput_id]);
          break;

        case otGraphicsContext:
          if (!attrIsGiven [attrViewportWidth] || !attrIsGiven [attrViewportHeight])
          {
            std::cout << "YOU NEED TO SPECIFY THE viewport_width= and viewport_height= ATTRIBUTE FOR THE <graphicscontext> OBJECT '" << objName << "'! STOPPING PARSER! bye.\n\n";
            return false;
          }
          if (!attrIsGiven [attrViewportX] || !attrIsGiven [attrViewportY])
          {
            std::cout << "YOU NEED TO SPECIFY THE viewport_x= and viewport_y= ATTRIBUTE FOR THE <graphicscontext> OBJECT '" << objName << "'! STOPPING PARSER! bye.\n\n";
            return false;
          }
          if (!attrIsGiven [attrCanvasWidth] || !attrIsGiven [attrCanvasHeight])
          {
            std::cout << "YOU NEED TO SPECIFY THE canvas_width= and canvas_height= ATTRIBUTE FOR THE <graphicscontext> OBJECT '" << objName << "'! STOPPING PARSER! bye.\n\n";
            return false;
          }
          if (!attrIsGiven [attrViewportZoom])
          {
            std::cout << "YOU NEED TO SPECIFY THE viewport_zoom= ATTRIBUTE FOR THE <graphicscontext> OBJECT '" << objName << "'! STOPPING PARSER! bye.\n\n";
            return false;
          }
          if (!attrIsGiven [attrForeground_colour] || !attrIsGiven [attrBackground_colour])
          {
            std::cout << "YOU NEED TO SPECIFY THE foreground_colour= and background_colour= ATTRIBUTE FOR THE <graphicscontext> OBJECT '" << objName << "'! STOPPING PARSER! bye.\n\n";
            return false;
          }
          if (!attrIsGiven [attrFormat] || !attrIsGiven [attrOptions])
          {
            std::cout << "YOU NEED TO SPECIFY THE format= and options= ATTRIBUTE FOR THE <graphicscontext> OBJECT '" << objName << "'! STOPPING PARSER! bye.\n\n";
            return false;
          }

          fprintf (partFile_attributes, ", %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d",
                   atoi (attrString [attrViewportWidth]), atoi (attrString [attrViewportHeight]),
                   atoi (attrString [attrViewportX]),     atoi (attrString [attrViewportY]),
                   atoi (attrString [attrCanvasWidth]),   atoi (attrString [attrCanvasHeight]),
                   atoi (attrString [attrViewportZoom]),
                   atoi (attrString [attrCursorX]),       atoi (attrString [attrCursorY]),
                   colortoi (attrString [attrForeground_colour]),
                   colortoi (attrString [attrBackground_colour]));

          if (!attrIsGiven [attrFont_attributes])
            fprintf (partFile_attributes, ", NULL");
          else
            fprintf (partFile_attributes, ", &iVtObject%s", getObjNameWithPoolIdent (attrString [attrFont_attributes]).c_str());

          if (!attrIsGiven [attrLine_attributes])
            fprintf (partFile_attributes, ", NULL");
          else
            fprintf (partFile_attributes, ", &iVtObject%s", getObjNameWithPoolIdent (attrString [attrLine_attributes]).c_str());

          if (!attrIsGiven [attrFill_attributes])
            fprintf (partFile_attributes, ", NULL");
          else
            fprintf (partFile_attributes, ", &iVtObject%s", getObjNameWithPoolIdent (attrString [attrFill_attributes]).c_str());

          fprintf (partFile_attributes, ", %d, %d", colordepthtoi (attrString [attrFormat]), gcoptionstoi (attrString [attrOptions]));

          if (!attrIsGiven [attrTransparency_colour])
            fprintf (partFile_attributes, ", 0");
          else
            fprintf (partFile_attributes, ", %d", colortoi (attrString [attrTransparency_colour]));

          break;
      }

      // #########################################
      // ### Print out Repeat Array REFERENCES ###
      // #########################################
      if (objHasArrayObject)
      {
        if (objChildObjects == 0)
          fprintf (partFile_attributes, ", 0,NULL");
        else
          fprintf (partFile_attributes, ", %d,iVtObject%s_aObject", objChildObjects, objName);
      }

      if (objHasArrayObjectXY)
      {
        if (objChildObjects == 0)
          fprintf (partFile_attributes, ", 0,NULL");
        else
          fprintf (partFile_attributes, ", %d,iVtObject%s_aObject_x_y_font_row_col", objChildObjects, objName);
      }

      if (objType == otPicturegraphic)
      {
        for (unsigned int actDepth=0; actDepth <= 2; actDepth++)
        {
          if ( (actDepth > deXcolorDepth) || (stdRawBitmapBytes [actDepth] == 0))
            fprintf (partFile_attributes, ", 0,NULL");
          else
            fprintf (partFile_attributes, ", %d,iVtObject%s_aRawBitmap%d", stdRawBitmapBytes [actDepth], objName, actDepth);
        }
        if (fixNr == 0)
          fprintf (partFile_attributes, ", 0,NULL");
        else
          fprintf (partFile_attributes, ", %d,iVtObject%s_aFixedBitmaps", fixNr, objName);
      }

      if (objHasArrayPoints)
      {
        if(objChildPoints == 0)
          fprintf (partFile_attributes, ", 0,NULL");
        else
          fprintf (partFile_attributes, ", %d,iVtObject%s_aPoints", objChildPoints, objName);
      }

      if (objHasArrayEventMacro)
      {
        if (objChildMacros == 0)
          fprintf (partFile_attributes, ", 0,NULL");
        else
          // Changed this line to give the correct name to the Macro object to match the naming conventions of IsoAgLib V 1.1.0.
          // This coincides with a change made above to the name of the Macro struct. -bac 06-Jan-2005
          //fprintf (partFile_attributes, ", %d,iVtObject%s_aEvent_Macro", objChildMacros, objName);
          fprintf (partFile_attributes, ", %d,iVtObject%s_aMacro_Object", objChildMacros, objName);
      }

      if (objHasArrayLanguagecode)
      {
        if (objChildLanguages == 0)
          fprintf (partFile_attributes, ", 0,NULL");
        else
          fprintf (partFile_attributes, ", %d,ivtObject%s_aLanguage", objChildLanguages, objName);
      }

      if (objHasArrayMacroCommand)
      { /** @todo I think the "else" is not correct, the length has to be put out also!? -mjw */
        if (objChildCommands == 0)
          fprintf (partFile_attributes, ", 0,NULL");
        else
          fprintf (partFile_attributes, ", iVtObject%s_aMacro_Commands", objName);
      }

      if ( checkForAllowedExecution() )
        fprintf (partFile_attributes, "};\n"); //s_ROM bla blub terminator...

    } while (b_dupMode && (*dupLangNext != 0x00));
  } // end of "normal" element processing

  // Add all Child-Elements recursively
  uint64_t omcType = omcTypeTable [objType];
  if (objType == otContainer) omcType = ombType; // Object May Contain what the Object Is - Simple rule. more simple than the graphic in the spec. ;)

  for (child = n->getFirstChild(); child != 0; child=child->getNextSibling())
  {
    if (child->getNodeType() == DOMNode::ELEMENT_NODE)
    {
      if (!processElement (child, omcType/*, rpcc_inKey, rpcc_inButton*/))
        return false;
    }
  }

  return true;
}

vt2iso_c::vt2iso_c(char* pch_poolIdent)
  : amountXmlFiles(0)
  , pcch_poolIdent (pch_poolIdent)
  , b_hasUnknownAttributes (false)
  , partFile_split_function( NULL )
{}

vt2iso_c::~vt2iso_c()
{
  delete pc_specialParsing;
  delete pc_specialParsingPropTag;
}

void
vt2iso_c::skRelatedFileOutput()
{
  if (!getIsSKWidth()) setSKWidth (60);
  if (!getIsSKHeight()) setSKHeight (32);
  if (!getIsSKWidth() || !getIsSKHeight()) {
    std::cout << "\n\nWARNING: You have NOT specified a SoftKey-Width/Height, so vt2iso assumes your softkeys are designed on a 60x32 pixel basis.\n"
        << "ATTENTION: SoftKeys are scaled and centered to fit the SK-Dimensions of the VT it is displayed on, so take care that you know what you're doing!\n\n";
  }
  fprintf (partFile_defines, "#define vtObjectPoolDimension %d\n", getOPDimension());
  fprintf (partFile_defines, "#define vtObjectPoolSoftKeyWidth %d\n", getSKWidth());
  fprintf (partFile_defines, "#define vtObjectPoolSoftKeyHeight %d\n", getSKHeight());
}

const char*
vt2iso_c::getAttributeValue (DOMNode* pc_node, const char* attributeName)
{
  /// get attributes of node and compare all attributes with given params or until match
  DOMNamedNodeMap *pAttributes = pc_node->getAttributes();
  int nodeSize = pAttributes->getLength();
  for (int index=0; index<nodeSize; ++index)
  {
    DOMAttr *pAttributeNode = (DOMAttr*) pAttributes->item(index);
    utf16convert ((char*)pAttributeNode->getName(), attr_name, 1024);

    if (strncmp (attr_name, attributeName, stringLength) == 0)
    { /// correct attribute found, now compare the given vale with attribute's value
      utf16convert ((char*)pAttributeNode->getValue(), attr_value, 1024);
      return attr_value;
    }
  }

  return "";
}

bool
vt2iso_c::prepareFileNameAndDirectory (std::basic_string<char>* pch_fileName)
{
  char xmlFileTemp [1024+1];
  int lastDirPos;

  std::basic_string<char> c_expectedType = ".xml";
  std::basic_string<char> c_expectedType2 = ".XML";

  std::basic_string<char> c_unwantedType = ".inc";
  std::basic_string<char> c_unwantedType2 = ".h";
  std::basic_string<char> c_unwantedType3 = ".inc-template";
  std::basic_string<char> c_unwantedType4 = ".iop";
  std::basic_string<char> c_unwantedType5 = ".txt";
  std::basic_string<char> c_unwantedType6 = ".csv";
  std::basic_string<char> c_unwantedType7 = ".cpp";
  std::basic_string<char> c_unwantedType8 = ".bak";

  // strip the ".xml" away!
  if (pch_fileName->length() > 4)
  { // see if the user gave ".xml" !
    if ( (pch_fileName->substr( pch_fileName->length()-4 ) == c_expectedType)
          ||(pch_fileName->substr( pch_fileName->length()-4 ) == c_expectedType) )
    { // strip off ".xml"
      pch_fileName->erase (pch_fileName->length()-4, 4);
    }
  }
#ifdef WIN32
  lastDirPos = pch_fileName->find_last_of( "\\" );
  c_directory = pch_fileName->substr( 0, lastDirPos+1 );
  if (c_directory == "") c_directory = ".\\";
#else
  lastDirPos = pch_fileName->find_last_of( "/" );
  c_directory = pch_fileName->substr( 0, lastDirPos+1 );
  if (c_directory == "") c_directory = "./";
#endif

  c_project = pch_fileName->substr( lastDirPos+1 );

  std::basic_string<char> c_directoryCompareItem;
  std::cerr << "--> Directory: " << c_directory << std::endl << "--> File:      " << c_project << std::endl;

  proName = c_project;

#ifdef WIN32
  HANDLE hList;
  TCHAR  szDir[255];
  TCHAR  szSubDir[255];
  TCHAR  szCurDir[255];
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
    return false;
  }

  // Get the proper directory path
  wsprintf(szDir, TEXT("*") );
  hList = FindFirstFile(szDir, &FileData);
  if (hList != INVALID_HANDLE_VALUE)
  { // Traverse through the directory structure
    do
    { // Check the object is a directory or not
      if (!(FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
      { //if ((strcmp(FileData.cFileName, ".") != 0) && (strcmp(FileData.cFileName, "..") != 0))
        c_directoryCompareItem = FileData.cFileName;
        if ( c_directoryCompareItem[0] == '.' ) continue;
        if (c_directoryCompareItem [c_directoryCompareItem.length()-1] == '~') continue;
        if ( c_directoryCompareItem.substr( c_directoryCompareItem.length()-4 ) == c_unwantedType ) continue;
        if ( c_directoryCompareItem.substr( c_directoryCompareItem.length()-2 ) == c_unwantedType2 ) continue;
        if ( (c_directoryCompareItem.length() > 13) && (c_directoryCompareItem.substr( c_directoryCompareItem.length()-13 ) == c_unwantedType3) ) continue;
        if ( c_directoryCompareItem.substr( c_directoryCompareItem.length()-4 ) == c_unwantedType4 ) continue;
        if ( c_directoryCompareItem.substr( c_directoryCompareItem.length()-4 ) == c_unwantedType5 ) continue;
        if ( c_directoryCompareItem.substr( c_directoryCompareItem.length()-4 ) == c_unwantedType6 ) continue;
        if ( c_directoryCompareItem.substr( c_directoryCompareItem.length()-4 ) == c_unwantedType7 ) continue;
        if ( c_directoryCompareItem.substr( c_directoryCompareItem.length()-4 ) == c_unwantedType8 ) continue;

        if ( c_directoryCompareItem.find( c_project ) != std::string::npos ) {
          c_directoryCompareItem.insert(0, "\\" );
          c_directoryCompareItem.insert(0, c_directory );
          strcpy (xmlFiles [amountXmlFiles], c_directoryCompareItem.c_str() );
          amountXmlFiles++;
          //std::cout << "found: " << c_directoryCompareItem << "\n";
        }
      }
    } while (FindNextFile(hList, &FileData));
    FindClose(hList);
  }
  else
  {
    std::cerr <<  "Couldn't open the directory.";
    return false;
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
      if ( (c_directoryCompareItem.length() > 4  ) && ( c_directoryCompareItem.substr( c_directoryCompareItem.length()-4  ) == c_unwantedType4 ) ) continue;
      if ( (c_directoryCompareItem.length() > 4  ) && ( c_directoryCompareItem.substr( c_directoryCompareItem.length()-4  ) == c_unwantedType5 ) ) continue;
      if ( (c_directoryCompareItem.length() > 4  ) && ( c_directoryCompareItem.substr( c_directoryCompareItem.length()-4  ) == c_unwantedType6 ) ) continue;
      if ( (c_directoryCompareItem.length() > 4  ) && ( c_directoryCompareItem.substr( c_directoryCompareItem.length()-4  ) == c_unwantedType7 ) ) continue;
      if ( (c_directoryCompareItem.length() > 4  ) && ( c_directoryCompareItem.substr( c_directoryCompareItem.length()-4  ) == c_unwantedType8 ) ) continue;

      if ( c_directoryCompareItem.find( c_project ) != std::string::npos ) {
        c_directoryCompareItem.insert(0, "/" );
        c_directoryCompareItem.insert(0, c_directory );
        strcpy (xmlFiles [amountXmlFiles], c_directoryCompareItem.c_str() );
        amountXmlFiles++;
    //    std::cout << "found: " << ep->d_name << "\n";
      }
    }
    closedir(dp);
  }
  else
  {
    std::cerr <<  "Couldn't open the directory '" << c_directory.c_str() << "'." << std::endl;
    return false;
  }
#endif
  // finished preparations on directory -> needed in processElement(...)
  rc_workDir = c_directory.c_str();

  // now sort this list
  bool stillSorting;
  do {
    stillSorting=false;
    for (int a=1; a < amountXmlFiles; a++) {
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

  return true;
}

std::string
vt2iso_c::getObjNameWithPoolIdent (char* pcch_objName)
{
  if (!pcch_objName || (strcmp (pcch_objName, "") == 0))
    pcch_objName = "NULL";

  std::string objName = std::string (pcch_objName);

  if (strcmp (objName.c_str(), "NULL") == 0)
    return objName;

  if (pcch_poolIdent && strstr (objName.c_str(), pcch_poolIdent) != objName.c_str())
    objName = std::string (pcch_poolIdent) + objName;

  return objName;
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

  const char* xmlFile = 0;
  AbstractDOMParser::ValSchemes valScheme = AbstractDOMParser::Val_Auto;
  bool doNamespaces       = false;
  bool doSchema           = false;
  bool schemaFullChecking = false;
  bool errorOccurred      = false;
  char localeStr[64];
  char* poolIdentStr = "";
  std::basic_string<char> dictionary = "";
  int  indexXmlFile;

  bool generatePalette = false;
  bool externalize = false;

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
    else if (!strcmp(argV[argInd], "-p")
              ||  !strcmp(argV[argInd], "-P"))
    {
      generatePalette = true;
    }
    else if (!strcmp(argV[argInd], "-e")
              ||  !strcmp(argV[argInd], "-E"))
    {
      externalize = true;
    }
    else if (!strncmp(argV[argInd], "-i=", 3)
              ||  !strncmp(argV[argInd], "-I=", 3))
    {
      poolIdentStr = &argV[argInd][3];
    }
    else if (!strncmp(argV[argInd], "-dict=", 6))
    {
      dictionary = &argV[argInd][6];
    }
    else
    {
      std::cerr << "Unknown option '" << argV[argInd] << "', ignoring it\n" << std::endl;
    }
  }

  if (generatePalette)
  {
    std::cout << "Generating ISO11783-6(VT)-Palette to 'iso11783vt-palette.act'..." << std::endl;
    FILE* paletteFile = fopen ("iso11783vt-palette.act", "wt");
    RGB_s vtColorTableRGB [256];
    for (int i=0; i<256; i++) {
      vtColorTableRGB[i].rgbRed  = vtColorTable[i].bgrRed;
      vtColorTableRGB[i].rgbGreen= vtColorTable[i].bgrGreen;
      vtColorTableRGB[i].rgbBlue = vtColorTable[i].bgrBlue;
    }
    fwrite (vtColorTableRGB, sizeof(RGB_s), 256, paletteFile);
    fclose (paletteFile);
  }

  //
  //  There should be only one parameter left, and that
  //  should be the file name.
  //
  if (argInd != argC - 1) { usage(); return 1; }

  // get file list with matching files!
  std::basic_string<char> c_fileName( argV [argInd] );

  // Do INITIALIZATION STUFF
#ifdef USE_SPECIAL_PARSING
  vt2iso_c* pc_vt2iso = new vt2iso_c(poolIdentStr);
#else
  vt2iso_c* pc_vt2iso = new vt2iso_c(poolIdentStr);
#endif

  if (!pc_vt2iso->prepareFileNameAndDirectory (&c_fileName))
  {
    pc_vt2iso->clean_exit("Error occurred. Terminating.\n");
    delete pc_vt2iso;
    exit (-1);
  }

  pc_vt2iso->init (c_fileName.c_str(), &dictionary, externalize);

  for (indexXmlFile = 0; indexXmlFile < pc_vt2iso->getAmountXmlFiles(); indexXmlFile++)
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
    xmlFile = pc_vt2iso->getXmlFile(indexXmlFile);

    //reset error count first
    errorHandler.resetErrors();

    XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* doc;

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

    //  Extract the DOM tree, get the list of all the elements and report the
    //  length as the count of elements.
    if (errorHandler.getSawErrors() || errorOccurred)
    {
      std::cout << "\nErrors occurred, no output available\n" << std::endl;
      errorOccurred = true;
    } else {
      if (doc) {
        // ### main routine starts right here!!! ###
        bool b_returnRootElement = pc_vt2iso->processElement ((DOMNode*)doc->getDocumentElement(), (uint64_t(1)<<otObjectpool) /*, NULL, NULL*/); // must all be included in an objectpool tag !

        if (!pc_vt2iso->getIsOPDimension()) {
          std::cout << "\n\nYOU NEED TO SPECIFY THE dimension= TAG IN <objectpool> ! STOPPING PARSER! bye.\n\n";
          pc_vt2iso->clean_exit();
          delete pc_vt2iso;
          exit (-1);
        }
        if (!b_returnRootElement)
        { /// ANY ERROR OCCURED DURING THE PARSING PROCESS
          pc_vt2iso->clean_exit();
          delete pc_vt2iso;
          exit (-1);
        }
      }
    }

    //  Delete the parser itself.  Must be done prior to calling Terminate, below.
    parser->release();

    // And call the termination method
    XMLPlatformUtils::Terminate();

  } // loop all files

  pc_vt2iso->skRelatedFileOutput();

  if (errorOccurred)
    pc_vt2iso->clean_exit ("XML-Parsing error occurred. Terminating.\n\n");
  else
    pc_vt2iso->clean_exit ("All conversion done successfully.\n\n");

  delete pc_vt2iso;
  return 0; /// everything well done
}
