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
#include <vector>
#include <cctype>
#include <string.h>
#include <stdlib.h>
#include <sstream>
#include <set>
#include <iostream>
#include <algorithm>
// Includes (findfirst, findnext)
#include <stddef.h>
#include <sys/types.h>

#ifdef WIN32
 #include <fstream>
 #include <windows.h>
 #include <stdio.h>
//        extern "C"
 using namespace std;
#else
 #include <fstream.h>
 #include <dirent.h>
#endif

// Includes (proc2iso)
#include "proc2iso.hpp"
#include "proc2iso-defines.hpp"
#include "../../../xgpl_src/IsoAgLib/comm/SystemMgmt/ISO11783/impl/isoname_c.h"

// ### GLOBALS ###
FILE *partFileA;
FILE *partFileB;

char xmlFileGlobal [1024+1];

std::basic_string<char> c_project;

char objNameTable [(stringLength+1)*1000];
unsigned int objIDTable [1000];
unsigned int objType=0; //init for happy compiler
unsigned int objNextAutoID;
unsigned int kcNextAutoID;
unsigned int objNextUnnamedName;
unsigned int objCount;

std::vector<std::string> vecstr_attrString (maxAttributeNames);
std::stringstream buffer;
bool attrIsGiven [maxAttributeNames];
std::vector<std::string> vecstr_constructor (7);

// ---------------------------------------------------------------------------
//  void usage () --- Prints out usage text.
// ---------------------------------------------------------------------------

static void usage()
{
  std::cout << "\nproc2iso BUILD DATE: 27-Jun-2005\n\n"
    "Usage:\n"
    " proc2iso [options] <XML file>\n\n"
    "This program invokes the DOMBuilder, builds the DOM tree,\n"
    "and then converts the tree to ISO Process Data cpp-files.\n\n"
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
};


char proName[1024+1];

void clean_exit (int return_value, char* error_message=NULL)
{
  char partFileName [1024+1]; partFileName [1024+1-1] = 0x00;

  if (error_message != NULL)
    std::cout << error_message;

  if (partFileA) fclose (partFileA);

  if (partFileB) fclose (partFileB);

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

  exit (return_value);
};


void copyWithQuoteAndLength (char *dest, const char *src, unsigned int len)
{
  *dest++ = '"';
  unsigned int take = (strlen(src) <= len) ? strlen(src) : len;
  unsigned int i=0;
  for (; i<take; i++) *dest++ = *src++;
  for (; i<len; i++) *dest++ = ' '; // fill with spaces if necessary
  *dest++ = '"';
  *dest = 0x00;
};


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
//  unsigned int getID (char* objName, bool wishingID, unsigned int wishID=0)
// ---------------------------------------------------------------------------
unsigned int getID (const char* objName, bool wishingID, unsigned int wishID)
{
  bool isThere = false;
  unsigned int foundID = 0;
  static bool b_isFirstDevice = true;

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

  if (objType == otDevice)
  {
    if (b_isFirstDevice)
    {
      b_isFirstDevice = false;
      isThere = true;
      foundID = 0;
      objIDTable [objCount] = foundID;
      strncpy (&objNameTable [objCount*(stringLength+1)], objName, stringLength);
      objCount++;
    }
    else clean_exit (-1, "YOU CAN ONLY SPECIFY ONE <device> OBJECT! STOPPING PARSER! bye.\n\n");
  }

  if (!isThere) {
    // check what's the new ID to be
    if (wishingID) {
      foundID = wishID;
    } else {
      foundID = objNextAutoID;
      objNextAutoID--;
    }
    // insert new name-id pair now!
    objIDTable [objCount] = foundID;
    strncpy (&objNameTable [objCount*(stringLength+1)], objName, stringLength); // so we have 0-termination in every case, as our strings are 128+1 bytes!
    objCount++;
  }
  //std::cout << "foundID: " << foundID << " " << std::endl;
  //std::cout << "objName: " << objName << std::endl;
  return (foundID);
};


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
  kcNextAutoID = 254; // for safety, 255 should also be okay though...
  objNextUnnamedName = 1;

  strncpy (partFileName, xmlFile, 1024);
  strcat (partFileName, "-bytestream.inc");
  char FileName[200];
  char *tempFileName = strrchr(partFileName, '/');
  for (uint8_t i=0; i<strlen(tempFileName); i++) FileName[i]=tempFileName[i];
  partFileA = fopen (partFileName,"wt");

  fprintf (partFileA, "const HUGE_MEM uint8_t deviceDescription");

  strncpy (partFileName, xmlFile, 1024);
  strcat (partFileName, "-func.h");
  partFileB = fopen (partFileName,"wt");

  fprintf (partFileB, "#include \"%s\"\n\n", FileName+1);

  for (int j=0; j<maxAttributeNames; j++)
  {
    vecstr_attrString[j].clear();
  }
};


void defaultAttributes ()
{
  if (!attrIsGiven [attrSelf_conf]) {
    vecstr_attrString [attrSelf_conf] = "true";
    attrIsGiven [attrSelf_conf] = true;
  }
  if (!attrIsGiven [attrIndustry_group]) {
    vecstr_attrString [attrIndustry_group] = "2";
    attrIsGiven [attrIndustry_group] = true;
  }
  if (!attrIsGiven [attrFunction]) {
    vecstr_attrString [attrFunction] = "25";
    attrIsGiven [attrFunction] = true;
  }
  if (!attrIsGiven [attrWanted_SA]) {
    vecstr_attrString [attrWanted_SA] = "128";
    attrIsGiven [attrWanted_SA] = true;
  }
  if (!attrIsGiven [attrFunc_Inst]) {
    vecstr_attrString [attrFunc_Inst] = "0";
    attrIsGiven [attrFunc_Inst] = true;
  }
  if (!attrIsGiven [attrECU_Inst]) {
    vecstr_attrString [attrECU_Inst] = "0";
    attrIsGiven [attrECU_Inst] = true;
  }
  if (!attrIsGiven [attrStore_SA_at_EEPROM_address]) {
    vecstr_attrString [attrStore_SA_at_EEPROM_address] = "0xFFFF";
    attrIsGiven [attrStore_SA_at_EEPROM_address] = true;
  }
  if (!attrIsGiven [attrElement_number]) {
    vecstr_attrString [attrElement_number] = "0xFF";
    attrIsGiven [attrElement_number] = true;
  }
  if (!attrIsGiven [attrPriority]) {
    vecstr_attrString [attrPriority] = "2";
    attrIsGiven [attrPriority] = true;
  }
  if (!attrIsGiven [attrFeature_set]) {
    vecstr_attrString [attrFeature_set] = "";
    attrIsGiven [attrFeature_set] = true;
  }
  if (!attrIsGiven [attrProperties]) {
    vecstr_attrString [attrProperties] = "0";
    attrIsGiven [attrProperties] = true;
  }
  if (!attrIsGiven [attrTrigger_methods]) {
    vecstr_attrString [attrTrigger_methods] = "0";
    attrIsGiven [attrTrigger_methods] = true;
  }
  if (!attrIsGiven [attrDevice_value_presentation_name]) {
    vecstr_attrString [attrDevice_value_presentation_name] = "65535";
    attrIsGiven [attrDevice_value_presentation_name] = true;
  }
  if (!attrIsGiven [attrLis]) {
    vecstr_attrString [attrLis] = "0";
    attrIsGiven [attrLis] = true;
  }
  if (!attrIsGiven [attrWert_inst]) {
    vecstr_attrString [attrWert_inst] = "0xFF";
    attrIsGiven [attrWert_inst] = true;
  }
  if (!attrIsGiven [attrCumulative_value]) {
    vecstr_attrString [attrCumulative_value] = "false";
    attrIsGiven [attrCumulative_value] = true;
  }
};


unsigned int objectIsType (char* lookup_name)
{
  for (int i=0; i<maxObjectTypesToCompare; i++) {
    if (0 == strncmp (lookup_name, otCompTable [i], stringLength)) {
      return i;
    }
  }
  return 0xFFFF;
};


unsigned int idOrName_toi(const char* rpc_string)
{
  if (rpc_string [0] == 0x00) clean_exit (-1, "*** ERROR *** idOrName_toi: Empty 'object_id' attribute!\n\n");
  /** @todo check if all chars in the string are numbers, not only the first! */
  if ((rpc_string [0] >= '0') && (rpc_string [0] <= '9')) return atoi (rpc_string);
  // Starting with a letter, so look up id!
  return getID (rpc_string, false, 0);
};


unsigned int booltoi (const char *text_bool)
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
};


unsigned int elementtypetoi (const char *text_elemtype)
{
  int l;
  for (l=0; l<maxDeviceElementType; l++) {
    if (strncmp (text_elemtype, DeviceElementTypeTable [l], stringLength) == 0) return l+1;
  }
  std::cout << "INVALID DEVICE ELEMENT TYPE VALUE '" << text_elemtype << " ENCOUNTERED! STOPPING PARSER! bye.\n\n";
  clean_exit (-1);
  return 0; // to make compiler happy
};


unsigned int propertytoi (const char *text_property)
{
  int l, retval=0;
  for (l=0; l<maxProperty; l++) {
    if (strncmp (text_property, PropertyTable [l], stringLength) == 0) retval += (uint64_t(1)<<l);
  }
  return retval;
};


unsigned int triggermethodtoi (const char *text_trigger)
{
  int l, retval=0;
  for (l=0; l<maxTriggerMethods; l++) {
    if (strstr (text_trigger, TriggerMethodTable [l]) != 0) retval += (uint64_t(1)<<l);
  }
  return retval;
};

uint32_t stringtonumber(const char *text_number, uint8_t ui8_bitRange, uint8_t ui8_attrIndex)
{
  uint32_t ui32_temp;
  uint8_t ui8_check;
  char p_text_number[32];

  uint8_t i;
  for (i=0; i < strlen(text_number); i++)
  {
    p_text_number[i] = tolower (text_number[i]);
  }
  p_text_number[i] = 0x00;
  //check if hex or decimal number
  if (strncmp((p_text_number), "0x", 2) == 0) //yes, it is a hex number
    ui8_check = sscanf((p_text_number+2), "%x", &ui32_temp);
  else //it is a decimal number
    ui8_check = sscanf(p_text_number, "%d", &ui32_temp);

  if (ui8_check != 0)
  {
    //test correct range of value
    if ((ui32_temp >= 0) &&
    (ui32_temp < ((uint32_t)(1 << ui8_bitRange) - 1)))
      return ui32_temp;
    else
    {
      switch (ui8_attrIndex)
      {
        case attrSelf_conf:
          clean_exit (-1, "self_configurable_address has a value range of 1 bit! STOPPING PARSER! bye.\n\n");
          break;
        case attrIndustry_group:
          clean_exit (-1, "industry_group has a value range of 3 bits! STOPPING PARSER! bye.\n\n");
          break;
        case attrFunction:
          clean_exit (-1, "function has a value range of 8 bits! STOPPING PARSER! bye.\n\n");
          break;
        case attrFunc_Inst:
          clean_exit (-1, "function_instance has a value range of 5 bits! STOPPING PARSER! bye.\n\n");
          break;
        case attrECU_Inst:
          clean_exit (-1, "ecu_instance has a value range of 3 bits! STOPPING PARSER! bye.\n\n");
          break;
        case attrManufacturer_code:
          clean_exit (-1, "manufacturer_code has a value range of 11 bits! STOPPING PARSER! bye.\n\n");
          break;
        case attrDevice_class:
          clean_exit (-1, "device_class has a value range of 7 bits! STOPPING PARSER! bye.\n\n");
          break;
        case attrDevice_class_instance:
          clean_exit (-1, "device_class_instance has a value range of 4 bits! STOPPING PARSER! bye.\n\n");
          break;
        case attrWS_serial_number:
          clean_exit (-1, "ws_serial_number has a value range of 21 bits! STOPPING PARSER! bye.\n\n");
          break;
      }
    }
  }
  else clean_exit (-1, "Error while parsing workingset_mastername! STOPPING PARSER! bye.\n\n");
  return 0; //for happy compiler
};


static char attr_name [1024+1];
static char attr_value [1024+1];

char objName [stringLength+1];
unsigned int objID;
unsigned int tableID;
unsigned int buf_length=0;
char languageCmdCode[2];
bool is_objID;
bool deviceElementExists=false;
bool firstElement=true;


void utf16convert (char* source, char* destin, int count)
{
  int i=-1;
  do {
    i++;
    destin [i] = source [(i*2)];
  } while (destin [i] != 0x00);
}


void getAttributesFromNode(DOMNode *node) {
  DOMNamedNodeMap *pAttributes;
  if (node->hasAttributes()) { // parse through all attributes
    pAttributes = node->getAttributes();
    int nSize = pAttributes->getLength();

    // empty all possible attribute-values...
    for (int j=0; j<maxAttributeNames; j++) {
      for (int k=0; k<stringLength+1; k++) {
        vecstr_attrString [j].clear();
      }
      attrIsGiven [j] = false;
    }

    // now get all attributes listed in the <tag ...> element
    for (int i=0;i<nSize;++i) {
      DOMAttr *pAttributeNode = (DOMAttr*) pAttributes->item(i);
      utf16convert ((char *)pAttributeNode->getName(), attr_name, 1024);
      utf16convert ((char *)pAttributeNode->getValue(), attr_value, 1024);

      if (strncmp (attr_name, "designator", stringLength) == 0) {
          strncpy (objName, attr_value, stringLength);
          vecstr_attrString[attrDesignator] = attr_value;
          attrIsGiven[attrDesignator] = true;
          // DEBUG-OUT
          //std::cout << "FOUND ATTR: IND " << attrDesignator << ":= " << attrNameTable [attrDesignator] << " -> " << vecstr_attrString[attrDesignator] << ":"
          //<< attrIsGiven [attrDesignator] << "\n";
          continue;
      }


      int l;
      for (l=0; l<maxAttributeNames; l++) {
        if (strncmp (attr_name, attrNameTable [l], stringLength) == 0) {
          vecstr_attrString [l] = attr_value;
          attrIsGiven [l] = true;
          // DEBUG-OUT
          //std::cout << "FOUND ATTR: IND " << l << ":= " << attrNameTable [l] << " -> " << vecstr_attrString[l] << ":"
          //<< attrIsGiven [l] << "\n";
          break;
        }
      }

      // ERROR: We didn't match a possible attribute name
      if (l == maxAttributeNames)
      {
        std::cout << "\n\nUNKNOWN ATTRIBUTE " << attr_name <<"="<< attr_value <<" IN TAG <"<< XMLString::transcode(node->getNodeName()) <<"> ENCOUNTERED! STOPPING PARSER! bye.\n\n";
        clean_exit (-1);
      }
    }
  }

  if (!attrIsGiven[attrDesignator])
  {
    sprintf (objName, "Unnamed%d", objNextUnnamedName);
    ((DOMElement *)node)->setAttribute (X("designator"), X(objName));
    objNextUnnamedName++;
    attrIsGiven[attrDesignator] = true;
  }
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
static void processElement (DOMNode *node, uint64_t ombType, const char* rc_workDir)
#endif
{
  DOMNode *child;
  DOMNamedNodeMap *pAttributes;
  char *node_name = XMLString::transcode(node->getNodeName());
  std::vector<uint16_t> vecstr_childID;
  std::vector<uint16_t>::iterator it_childID;

  // all possible values of the objects
  unsigned int objChildObjects=0; //init for happy compiler

  unsigned int objChildID=0; bool is_objChildID=false; //init for happy compiler
  char objChildName [stringLength+1]; bool is_objChildName=false; //init for happy compiler
  char tempString [stringLength+1]; tempString [stringLength+1-1] = 0x00;
  char tempString2 [stringLength+1]; tempString2 [stringLength+1-1] = 0x00;

  bool b_lsb = false;

  buffer << std::hex << std::showbase;

  objName [stringLength+1-1] = 0x00;
  is_objID=false;

  if (!node) return;

  // get own ObjectType
  objType = objectIsType (node_name); // returns 0..6

  // ERROR: Wrong <TAG>
  if (objType == 0xFFFF) {
    std::cout << "\n\nUNKNOWN TAG <"<< node_name <<"> ENCOUNTERED! STOPPING PARSER! bye.\n\n";
    clean_exit (-1);
  }

  if (objType < maxObjectTypes)
  {
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
    getAttributesFromNode(node);

    // set all non-set attributes to default values
    defaultAttributes ();

    // get a new ID for this object
    objID = getID (objName, is_objID, objID);

    bool objNeedsName = false;
    switch (objType)
    {
      case otDevice:
      case otDeviceProcessData:
        objNeedsName=true;
    }

    bool objHasArrayObject = false;
    switch (objType)
    {
      case otDeviceElement:
        objHasArrayObject = true;
        deviceElementExists=true;
        break;
      case otDeviceProcessData:
      case otDeviceProperty:
        objHasArrayObject = true;
        break;
    }

    if (objNeedsName)
    {
      if (!attrIsGiven[attrDesignator])
      {
        std::cout << "\n\nYOU MUST SPECIFIY THE ATTRIBUTE designator= FOR <"<< node_name <<"> OBJECT! STOPPING PARSER! bye.\n\n";
        clean_exit (-1);
      }
    }

    if (objHasArrayObject)
    {
      // Process all Child-Elements
      vecstr_childID.clear();
      objChildObjects = 0;
      for (child = node->getFirstChild(); child != 0; child=child->getNextSibling())
      {   // if NOT Macro insert as normal object!
        if (child->getNodeType() == DOMNode::ELEMENT_NODE)
        {
          // get NAME attributes out of child
          if(child->hasAttributes()) {
            // parse through all attributes
            pAttributes = child->getAttributes();
            int nSize = pAttributes->getLength();

            objChildName [stringLength+1-1] = 0x00;
            is_objChildName = false;
            is_objChildID = false;

            for(int i=0;i<nSize;++i) {
              DOMAttr *pAttributeNode = (DOMAttr*) pAttributes->item(i);
              utf16convert ((char *)pAttributeNode->getName(), attr_name, 1024);
              utf16convert ((char *)pAttributeNode->getValue(), attr_value, 1024);

              // Get NAME directly
              if (strncmp (attr_name, "designator", stringLength) == 0) {
                strncpy (objChildName, attr_value, stringLength);
                is_objChildName = true;
              }
            }
          }
          if (is_objChildName == false)
          {
            // create auto-named NAME attribute
            sprintf (objChildName, "Unnamed%d", objNextUnnamedName);
            ((DOMElement *)child)->setAttribute (X("designator"), X(objChildName));
            objNextUnnamedName++;
            is_objChildName = true;
          }
          // give him an ID, although not necessary now...
          objChildID = getID (objChildName, is_objChildID, objChildID);
          //store that ID for later output in the bytestream-file
          vecstr_childID.push_back(objChildID);
          objChildObjects++;
        }
      }
    }

    // ###########################################
    // ### Print out inidivual object stuff... ###
    // ###########################################
    uint8_t i = 0;
    const uint8_t *mastername;
    __IsoAgLib::ISOName_c c_isoname (mastername);
    switch (objType)
    {
      case otDevice:
        if (!(attrIsGiven [attrSoftware_version] || attrIsGiven [attrStructure_label] || attrIsGiven [attrLocalization_label]))
        {
          clean_exit (-1, "YOU NEED TO SPECIFY THE software_version= AND structure_label= AND localization_label= ATTRIBUTES FOR THE <device> OBJECT! STOPPING PARSER! bye.\n\n");
        }
        if (vecstr_attrString[attrStructure_label].size() != 7 || vecstr_attrString[attrLocalization_label].size() != 7)
        {
          clean_exit (-1, "structure_label= AND localization_label= ATTRIBUTES FOR THE <device> OBJECT NEED A LENGTH OF 7 BYTES! STOPPING PARSER! bye.\n\n");
        }
        if (!(attrIsGiven [attrDevProgVarName]))
        {
          clean_exit (-1, "YOU NEED TO SPECIFY device_program_name= ATTRIBUTE FOR THE <device> OBJECT! STOPPING PARSER! bye.\n\n");
        }
        if (!(attrIsGiven [attrWorkingset_mastername] || (attrIsGiven [attrManufacturer_code] && attrIsGiven [attrWS_serial_number] && attrIsGiven [attrDevice_class] && attrIsGiven [attrDevice_class_instance])))
        {
          clean_exit (-1, "YOU NEED TO SPECIFY EITHER the workingset_mastername= ATTRIBUTE OR the manufacturer_code= AND ws_serial_number= AND device_class= AND device_class_instance= ATTRIBUTES FOR THE <device> OBJECT! STOPPING PARSER! bye.\n\n");
        }
        else
        {
          if (!attrIsGiven [attrWorkingset_mastername])
          {
            //alle Attribute zum Workingset_mastername zusammensetzen
            c_isoname.set(stringtonumber(vecstr_attrString [attrSelf_conf].c_str(), 1, attrSelf_conf),
                          stringtonumber(vecstr_attrString [attrIndustry_group].c_str(), 3, attrIndustry_group),
                          stringtonumber(vecstr_attrString [attrDevice_class].c_str(), 7, attrDevice_class),
                          stringtonumber(vecstr_attrString [attrDevice_class_instance].c_str(), 4, attrDevice_class_instance),
                          stringtonumber(vecstr_attrString [attrFunction].c_str(), 8, attrFunction),
                          stringtonumber(vecstr_attrString [attrManufacturer_code].c_str(), 11, attrManufacturer_code),
                          stringtonumber(vecstr_attrString [attrWS_serial_number].c_str(), 21, attrWS_serial_number),
                          stringtonumber(vecstr_attrString [attrFunc_Inst].c_str(), 5, attrFunc_Inst),
                          stringtonumber(vecstr_attrString [attrECU_Inst].c_str(), 3, attrECU_Inst));
          }
          else
          {
            for (i=0; i<vecstr_attrString[attrWorkingset_mastername].size(); i++)
            {
              vecstr_attrString[attrWorkingset_mastername][i] = tolower(vecstr_attrString[attrWorkingset_mastername][i]);
            }
            if (vecstr_attrString[attrWorkingset_mastername][0] != ('l' || 'm'))
            {
              clean_exit(-1, "workingset_mastername NEEDS TO START EITHER WITH \n \
                              L/l ... least signifant byte; see XML definition of device DVC (xs:hexBinary ISO 11783-10) OR \n \
                              M/m ... most signifant byte; normal definition (highest digit first) \n \
                              STOPPING PARSER! bye.\n\n");
            }
            //handle number least/most signifant byte
            b_lsb = (vecstr_attrString[attrWorkingset_mastername][0] == 'l');

            if (!(vecstr_attrString[attrWorkingset_mastername].size() == 17))
              clean_exit (-1, "workingset_mastername NEEDS TO HAVE AN IDENTIFIER \
                               FOR LEAST OR MOST SIGNIFICANT BIT (L/M) AND AN 8 BYTES LONG NAME! \n \
                               (EQUALS 16 HEXADECIMAL NUMBERS 0...F) STOPPING PARSER! bye.\n\n");

            for (i = 1; i < vecstr_attrString[attrWorkingset_mastername].size(); i++)
            {
              if (!((vecstr_attrString[attrWorkingset_mastername][i] >= '0' &&
                     vecstr_attrString[attrWorkingset_mastername][i] <= '9') ||
                    (vecstr_attrString[attrWorkingset_mastername][i] >= 'A' &&
                     vecstr_attrString[attrWorkingset_mastername][i] <= 'F')))
                clean_exit (-1, "PLEASE USE HEXADECIMAL FORMAT FOR workingset_mastername! STOPPING PARSER! bye.\n\n");
            }

            uint8_t ui8_tempName[8];
            for (i=0; i<8; i++)
            {
              uint8_t ui8_digitHi = vecstr_attrString[attrWorkingset_mastername][1+i*2]; // ASCII
              uint8_t ui8_digitLo = vecstr_attrString[attrWorkingset_mastername][1+i*2+1]; // ASCII

              uint8_t ui8_nibbleHi = (ui8_digitHi <= '9' /* 0..9 */) ? (ui8_digitHi-'0') : (ui8_digitHi-'A' + 10); // 0..9,10..15
              uint8_t ui8_nibbleLo = (ui8_digitLo <= '9' /* 0..9 */) ? (ui8_digitLo-'0') : (ui8_digitLo-'A' + 10); // 0..9,10..15

              const uint8_t pos = (b_lsb) ? (i) : (7-i);
              ui8_tempName[pos] = (ui8_nibbleHi << 4) | ui8_nibbleLo;
            }
            c_isoname.inputString(ui8_tempName);
          }

          languageCmdCode[0] = vecstr_attrString[attrLocalization_label][0];
          languageCmdCode[1] = vecstr_attrString[attrLocalization_label][1];

          //output: tableID & objID
          if (firstElement)
          {
            firstElement =false;
            buffer  << "_" << languageCmdCode[0] << languageCmdCode[1] << " [] = {0x61, \n'";
            buf_length++;
          } else
          {
            buffer  << ", '";

          }
          buffer  <<        TableIDTable [objType][0] << "', "
                  << "'" << TableIDTable [objType][1] << "', "
                  << "'" << TableIDTable [objType][2] << "', "
                  << uint16_t(objID & 0xFF)           << ", "
                  << uint16_t((objID >> 8) & 0xFF)    << ", ";
          buf_length += 5;
          //length of designator
          buffer << vecstr_attrString[attrDesignator].size() << ", ";
          buf_length++;
          //designator
          for (i = 0;i<vecstr_attrString[attrDesignator].size();i++)
          {
              buffer << "'" << vecstr_attrString[attrDesignator][i] << "', ";
          }
          buf_length += vecstr_attrString[attrDesignator].size();
          //length of software_version
          buffer << vecstr_attrString[attrSoftware_version].size() << ", ";
          buf_length++;
          //software_version
          for (i=0;i<vecstr_attrString[attrSoftware_version].size();i++)
          {
            buffer << "'" << vecstr_attrString[attrSoftware_version][i] << "', ";
          }
          buf_length += vecstr_attrString[attrSoftware_version].size();
          //workingset_mastername
          mastername = c_isoname.outputString();
          for (i=0; i<8; i++)
          {
            buffer << (uint16_t)mastername[i] << ", ";
          }
          buf_length += 8;
          //length serialnumber
          buffer << vecstr_attrString[attrSerial_number].size() << ", ";
          buf_length++;
          //serialnumber
          for (i=0;i<vecstr_attrString[attrSerial_number].size();i++)
          {
            buffer << "'" << vecstr_attrString[attrSerial_number][i] << "', ";
          }
          buf_length += vecstr_attrString[attrSerial_number].size();
          //structure_label
          for (i=0;i<vecstr_attrString[attrStructure_label].size();i++)
          {
            buffer << "'" << vecstr_attrString[attrStructure_label][i] << "', ";
          }
          buf_length += 7;
          //localization_label
          for (i=0;i<vecstr_attrString[attrLocalization_label].size();i++)
          {
              if (!((i+1)<(uint8_t)vecstr_attrString[attrLocalization_label].size()))
                buffer << "'" << vecstr_attrString[attrLocalization_label][i] << "'";
              else
                buffer << "'" << vecstr_attrString[attrLocalization_label][i] << "', ";
          }
          buf_length += 7;
          fprintf( partFileA, "%s", buffer.str().c_str() );
          buffer.str("");
          fprintf( partFileB, "IsoAgLib::iGetyPos_c %sGtp(0x%x, 0x%x);\n\n",
                   vecstr_attrString[attrDevProgVarName].c_str(), c_isoname.devClass(), c_isoname.devClassInst());
          fprintf( partFileB, "IsoAgLib::iIdentItem_c c_myIdent(&%sGtp, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x);\n\n",
                   vecstr_attrString[attrDevProgVarName].c_str(), c_isoname.selfConf(),
                   c_isoname.indGroup(), c_isoname.func(), c_isoname.manufCode(), c_isoname.serNo(),
                   atoi(vecstr_attrString[attrWanted_SA].c_str()), atoi(vecstr_attrString[attrStore_SA_at_EEPROM_address].c_str()),
                   c_isoname.funcInst(),c_isoname.ecuInst());
          vecstr_constructor[0] = vecstr_attrString[attrDevProgVarName].c_str();
          vecstr_constructor[1] = vecstr_attrString[attrPriority].c_str();
        }
        break;
      case otDeviceElement:
        if (!(attrIsGiven [attrElement_type] || attrIsGiven [attrParent_name]))
        {
          clean_exit (-1, "YOU NEED TO SPECIFY THE element_type= AND parent_name= ATTRIBUTES FOR THE <deviceelement> OBJECT! STOPPING PARSER! bye.\n\n");
        }
        //output: tableID & objID
        buffer  << ", \n'" << TableIDTable [objType][0] << "', "
                << "'" << TableIDTable [objType][1]   << "', "
                << "'" << TableIDTable [objType][2]   << "', "
                << uint16_t(objID & 0xFF)             << ", "
                << uint16_t((objID >> 8) & 0xFF)      << ", ";
        buf_length += 5;
        //elementtype
        buffer << elementtypetoi(vecstr_attrString[attrElement_type].c_str()) << ", ";
        buf_length++;
        //length of designator
        buffer << vecstr_attrString[attrDesignator].size() << ", ";
        buf_length++;
        //designator
        for (i = 0;i<vecstr_attrString[attrDesignator].size();i++)
        {
            buffer << "'" << vecstr_attrString[attrDesignator][i] << "', ";
        }
        buf_length += vecstr_attrString[attrDesignator].size();
        //element_number
        buffer  << (atoi(vecstr_attrString[attrElement_number].c_str()) & 0xFF) << ", "
                << ((atoi(vecstr_attrString[attrElement_number].c_str()) >> 8) & 0xFF) << ", ";
        buf_length += 2;
        //parent_name
        buffer  << (idOrName_toi(vecstr_attrString[attrParent_name].c_str()) & 0xFF) << ", "
                << ((idOrName_toi(vecstr_attrString[attrParent_name].c_str()) >> 8) & 0xFF) << ", ";
        buf_length += 2;
        //number_of_objects_to_follow
        buffer  << uint16_t(objChildObjects & 0xFF) << ", "
                << uint16_t((objChildObjects >> 8) & 0xFF);
        buf_length += 2;

        for (it_childID = vecstr_childID.begin(); it_childID != vecstr_childID.end(); it_childID++)
        {
            buffer << ", " << uint16_t(*it_childID & 0xFF) << ", "
                   << uint16_t((*it_childID >> 8) & 0xFF);
          buf_length += 2;
        }
        fprintf( partFileA, "%s", buffer.str().c_str() );
        buffer.str("");
        vecstr_constructor[2] = vecstr_attrString[attrElement_number].c_str();
        break;
      case otDeviceProcessData:
        if (!attrIsGiven [attrDdi])
        {
          clean_exit (-1, "YOU NEED TO SPECIFY THE ddi= ATTRIBUTE FOR THE <deviceprocessdata> OBJECT! STOPPING PARSER! bye.\n\n");
        }
        if (!(attrIsGiven [attrProcProgVarName]))
        {
          clean_exit (-1, "YOU NEED TO SPECIFY proc_program_name= ATTRIBUTE FOR THE <deviceprocessdata> OBJECT! STOPPING PARSER! bye.\n\n");
        }
        //output: tableID & objID
        buffer  << ", \n'" << TableIDTable [objType][0] << "', "
                << "'" << TableIDTable [objType][1]   << "', "
                << "'" << TableIDTable [objType][2]   << "', "
                << uint16_t(objID & 0xFF)             << ", "
                << uint16_t((objID >> 8) & 0xFF)      << ", ";
        buf_length += 5;
        //ddi
        buffer  << (atoi(vecstr_attrString[attrDdi].c_str()) & 0xFF) << ", "
                << ((atoi(vecstr_attrString[attrDdi].c_str()) >> 8) & 0xFF)  << ", ";
        buf_length += 2;
        //properties
        buffer  << propertytoi(vecstr_attrString[attrProperties].c_str()) << ", ";
        buf_length++;
        //triggermethods
        buffer  << triggermethodtoi(vecstr_attrString[attrTrigger_methods].c_str()) << ", ";
        buf_length++;
        //length of designator
        buffer << vecstr_attrString[attrDesignator].size() << ", ";
        buf_length++;
        //designator
        for (i = 0;i<vecstr_attrString[attrDesignator].size();i++)
        {
            buffer << "'" << vecstr_attrString[attrDesignator][i] << "', ";
        }
        buf_length += vecstr_attrString[attrDesignator].size();
        //device_value_presentation_object_id
        buffer  << (idOrName_toi(vecstr_attrString[attrDevice_value_presentation_name].c_str()) & 0xFF) << ", "
                << ((idOrName_toi(vecstr_attrString[attrDevice_value_presentation_name].c_str()) >> 8) & 0xFF);
        buf_length += 2;
        fprintf( partFileA, "%s", buffer.str().c_str() );
        buffer.str("");
        vecstr_constructor[3] = vecstr_attrString[attrFeature_set].c_str();
        vecstr_constructor[4] = vecstr_attrString[attrCumulative_value].c_str();
        vecstr_constructor[5] = vecstr_attrString[attrProcProgVarName].c_str();
        vecstr_constructor[6] = vecstr_attrString[attrDdi].c_str();
        break;
      case otDeviceProperty:
        if (!(attrIsGiven [attrDdi] || attrIsGiven [attrProperty_value]))
        {
          clean_exit (-1, "YOU NEED TO SPECIFY THE ddi= AND property_value= ATTRIBUTES FOR THE <deviceproperty> OBJECT! STOPPING PARSER! bye.\n\n");
        }
        //output: tableID & objID
        buffer  << ", \n'" << TableIDTable [objType][0] << "', "
                << "'" << TableIDTable [objType][1]   << "', "
                << "'" << TableIDTable [objType][2]   << "', "
                << uint16_t(objID & 0xFF)             << ", "
                << uint16_t((objID >> 8) & 0xFF)      << ", ";
        buf_length += 5;
        //ddi
        buffer  << (atoi(vecstr_attrString[attrDdi].c_str()) & 0xFF)  << ", "
                << ((atoi(vecstr_attrString[attrDdi].c_str()) >> 8) & 0xFF)  << ", ";
        buf_length += 2;
        //property_value
        buffer  << uint16_t(vecstr_attrString[attrProperty_value][0])  << ", "
                << uint16_t(vecstr_attrString[attrProperty_value][1])  << ", "
                << uint16_t(vecstr_attrString[attrProperty_value][2])  << ", "
                << uint16_t(vecstr_attrString[attrProperty_value][3])  << ", ";
        buf_length += 4;
        //length of designator
        buffer << vecstr_attrString[attrDesignator].size() << ", ";
        buf_length++;
        //designator
        for (i = 0;i<vecstr_attrString[attrDesignator].size();i++)
        {
            buffer << "'" << vecstr_attrString[attrDesignator][i] << "', ";
        }
        buf_length += vecstr_attrString[attrDesignator].size();
        //device_value_presentation_object_id
        buffer  << (idOrName_toi(vecstr_attrString[attrDevice_value_presentation_name].c_str()) & 0xFF) << ", "
                << ((idOrName_toi(vecstr_attrString[attrDevice_value_presentation_name].c_str()) >> 8) & 0xFF);
        buf_length += 2;
        fprintf( partFileA, "%s", buffer.str().c_str() );
        buffer.str("");
        break;
      case otDeviceDinProcessData:
        //this objecttype is only needed for creating the processdata-constructor
        if (!attrIsGiven [attrWert_inst])
        {
          clean_exit (-1, "YOU NEED TO SPECIFY THE wert_inst= ATTRIBUTE FOR THE <devicedinprocessdata> OBJECT! STOPPING PARSER! bye.\n\n");
        }
        fprintf( partFileB, "IsoAgLib::iProcDataLocal%s_c c_%s(", vecstr_constructor[3].c_str(), vecstr_constructor[5].c_str());
        fprintf( partFileB, "\n#ifdef USE_ISO_11783\n");
          fprintf( partFileB, "%d, 0x%x, ", atoi(vecstr_constructor[6].c_str()), atoi(vecstr_constructor[2].c_str()));
        fprintf( partFileB, "\n#endif\n");
        fprintf( partFileB, "#ifdef USE_DIN_9684\n");
          fprintf( partFileB, "0x%x, 0x%x, 0x%x, 0x%x, ", atoi(vecstr_attrString[attrLis].c_str()),
                   (atoi(vecstr_attrString[attrWert_inst].c_str()) & 0x0F), ((atoi(vecstr_attrString[attrWert_inst].c_str()) >> 4) & 0xF),
                   atoi(vecstr_constructor[2].c_str()));
        fprintf( partFileB, "\n#endif\n");
          fprintf( partFileB, "%sGtp, 0x%x, %sGtp, &%sGtp, %s",
                   vecstr_constructor[0].c_str(), atoi(vecstr_constructor[1].c_str()), vecstr_constructor[0].c_str(),
                   vecstr_constructor[0].c_str(), vecstr_constructor[4].c_str());
        fprintf( partFileB, "\n#ifdef USE_EEPROM_IO\n");
          fprintf( partFileB, ", 0x%x", atoi(vecstr_attrString [attrStore_SA_at_EEPROM_address].c_str()));
        fprintf( partFileB, "\n#endif\n");
        fprintf( partFileB, ");\n\n" );
        break;
      case otDeviceValuePresentation:
        if (!(attrIsGiven [attrOffset] || attrIsGiven [attrScale] || attrIsGiven [attrNumber_of_decimals]))
        {
          clean_exit (-1, "YOU NEED TO SPECIFY THE offset= AND scale= AND number_of_decimals= ATTRIBUTES FOR THE <devicevaluepresentation> OBJECT! STOPPING PARSER! bye.\n\n");
        }
        //output: tableID & objID
        buffer  << ", \n'" << TableIDTable [objType][0] << "', "
                << "'" << TableIDTable [objType][1]   << "', "
                << "'" << TableIDTable [objType][2]   << "', "
                << uint16_t(objID & 0xFF)             << ", "
                << uint16_t((objID >> 8) & 0xFF)      << ", ";
        buf_length += 5;
        //offset
        buffer  << uint16_t(vecstr_attrString[attrOffset][0]) << ", "
                << uint16_t(vecstr_attrString[attrOffset][1]) << ", "
                << uint16_t(vecstr_attrString[attrOffset][2]) << ", "
                << uint16_t(vecstr_attrString[attrOffset][3]) << ", ";
        buf_length += 4;
        //scale
        float f_temp = atof(vecstr_attrString[attrScale].c_str());
        uint8_t *pui8_temp = (uint8_t*)&f_temp;
        buffer  << uint16_t(pui8_temp[0]) << ", "
                << uint16_t(pui8_temp[1]) << ", "
                << uint16_t(pui8_temp[2]) << ", "
                << uint16_t(pui8_temp[3]) << ", ";
        buf_length += 4;
        //number_of_decimals
        buffer << atoi(vecstr_attrString[attrNumber_of_decimals].c_str()) << ", ";
        buf_length++;
        //length of unitdesignator
        buffer << vecstr_attrString[attrUnitdesignator].size() << ", ";
        buf_length++;
        //unit_designator
        for (i = 0;i<vecstr_attrString[attrUnitdesignator].size();i++)
        {
            if (!((i+1) < (uint8_t)vecstr_attrString[attrUnitdesignator].size()))
              buffer << "'" << vecstr_attrString[attrUnitdesignator][i] << "'";
            else
              buffer << "'" << vecstr_attrString[attrUnitdesignator][i] << "', ";
        }
        buf_length += vecstr_attrString[attrUnitdesignator].size();
        fprintf( partFileA, "%s", buffer.str().c_str() );

        buffer.str("");
        break;
      }
  }

  // Add all Child-Elements recursively
  uint64_t omcType = omcTypeTable [objType];

  for (child = node->getFirstChild(); child != 0; child=child->getNextSibling())
  {
    if (child->getNodeType() == DOMNode::ELEMENT_NODE)
    {
      processElement (child, omcType, rc_workDir);
    }
  }
}

// ---------------------------------------------------------------------------
//  int main (argC, argV) --- parse XML file and output to partA and partB files.
// ---------------------------------------------------------------------------
int main(int argC, char* argV[])
{
  // Check command line and extract arguments.
  if (argC < 2) { usage(); return 1; }

  const char* xmlFile = 0;
  AbstractDOMParser::ValSchemes valScheme = AbstractDOMParser::Val_Auto;
  bool        doNamespaces    = false;
  bool        doSchema     = false;
  bool        schemaFullChecking = false;
  bool        errorOccurred = false;
  char        localeStr[64];
  char        xmlFiles [256] [1024+1];
  char        xmlFileTemp [1024+1];
  int         indexXmlFile, amountXmlFiles=0;

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

  //  There should be only one parameter left, and that
  //  should be the file name.
  if (argInd != argC - 1) { usage(); return 1; }
  // get file list with matching files!
  std::basic_string<char> c_fileName( argV [argInd] );
  #ifdef WIN32
  int lastDirPos = c_fileName.find_last_of( "\\" );
  std::basic_string<char> c_directory = c_fileName.substr( 0, lastDirPos+1 );
  #else
  int lastDirPos = c_fileName.find_last_of( "/" );
  std::basic_string<char> c_directory = c_fileName.substr( 0, lastDirPos+1 );
  #endif
  /* globally defined */  c_project = c_fileName.substr( lastDirPos+1 );
  std::basic_string<char> c_unwantedType = ".inc";
  std::basic_string<char> c_unwantedType2 = ".h";
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
      {
        c_directoryCompareItem = FileData.cFileName;
        if ( c_directoryCompareItem[0] == '.' ) continue;
        if (c_directoryCompareItem [c_directoryCompareItem.length()-1] == '~') continue;
        if ( c_directoryCompareItem.substr( c_directoryCompareItem.length()-4 ) == c_unwantedType ) continue;
        if ( c_directoryCompareItem.substr( c_directoryCompareItem.length()-2 ) == c_unwantedType2 ) continue;
        if ( (c_directoryCompareItem.length() > 13) && (c_directoryCompareItem.substr( c_directoryCompareItem.length()-13 ) == c_unwantedType3) ) continue;

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

      if ( c_directoryCompareItem.find( c_project ) != std::string::npos ) {
        c_directoryCompareItem.insert(0, "/" );
        c_directoryCompareItem.insert(0, c_directory );
        strcpy (xmlFiles [amountXmlFiles], c_directoryCompareItem.c_str() );
        amountXmlFiles++;
      }
    }
    closedir(dp);
  } else
  {
    std::cerr <<  "Couldn't open the directory.";
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

    char xsdLocation[1024+1];
    strcpy (xsdLocation, argV[0]);
    // now trim exe filename
    for (int i=strlen(xsdLocation)-1; i >= 0; i--) {
      if (xsdLocation[i] == '/') { xsdLocation[i+1]=0x00; break; }
    }
    strcat (xsdLocation, "proc2iso.xsd");
    XMLCh* propertyValue = XMLString::transcode(xsdLocation);
    parser->setProperty(XMLUni::fgXercesSchemaExternalNoNameSpaceSchemaLocation, propertyValue);

    // And create our error handler and install it
    DOMCountErrorHandler errorHandler;
    parser->setErrorHandler(&errorHandler);

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

    //  Extract the DOM tree, get the list of all the elements and report the
    //  length as the count of elements.
    if (errorHandler.getSawErrors() || errorOccurred)
    {
      std::cout << "\nErrors occurred, no output available\n" << std::endl;
      errorOccurred = true;
    } else {
      if (doc) {
        // ### main routine starts right here!!! ###
        processElement ((DOMNode*)doc->getDocumentElement(), (uint64_t(1)<<otDevice), c_directory.c_str() ); // must all be included in an device tag !
        if (!deviceElementExists) clean_exit(-1, "YOU MUST SPECIFY AT LEAST ONE deviceelement OBJECT! STOPPING PARSER!\n\n");
        fprintf (partFileA, "};\n");
        fprintf (partFileA, "\nstatic const uint32_t ui32_arrayLength_%c%c = %d;\n", languageCmdCode[0], languageCmdCode[1], buf_length);
      }
    }

    //  Delete the parser itself. Must be done prior to calling Terminate, below.
    parser->release();

    // And call the termination method
    XMLPlatformUtils::Terminate();

  } // loop all files

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
