/***************************************************************************
                          vt2iso.hpp
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
#ifndef VT2ISO_H
#define VT2ISO_H

#define DEF_MAX_OBJECTS (10000)

#include <xercesc/dom/DOMErrorHandler.hpp>
#include <xercesc/util/XMLString.hpp>
#include <iostream>
#include <set>
#include <vector>
#include <map>

#include "vt2iso-defines.hpp"
#include "vt2iso-globals.hpp"

XERCES_CPP_NAMESPACE_USE

//  Simple error handler deriviative to install on parser
class DOMCountErrorHandler : public DOMErrorHandler
{
public:
    //  Constructors and Destructor
    DOMCountErrorHandler();
    ~DOMCountErrorHandler();

    // -----------------------------------------------------------------------
    //  Getter methods
    // -----------------------------------------------------------------------
    bool getSawErrors() const;

    // -----------------------------------------------------------------------
    //  Implementation of the DOM ErrorHandler interface
    // -----------------------------------------------------------------------
    bool handleError(const DOMError& domError);
    void resetErrors();

private :
    // -----------------------------------------------------------------------
    //  Unimplemented constructors and operators
    // -----------------------------------------------------------------------
    DOMCountErrorHandler(const DOMCountErrorHandler&);
    void operator=(const DOMCountErrorHandler&);

    // -----------------------------------------------------------------------
    //  Private data members
    //
    //  fSawErrors
    //      This is set if we get any errors, and is queryable via a getter
    //      method. Its used by the main code to suppress output if there are
    //      errors.
    // -----------------------------------------------------------------------
    bool    fSawErrors;
};


// ---------------------------------------------------------------------------
//  This is a simple class that lets us do easy (though not terribly efficient)
//  trancoding of XMLCh data to local code page for display.
// ---------------------------------------------------------------------------
class StrX
{
public :
    // -----------------------------------------------------------------------
    //  Constructors and Destructor
    // -----------------------------------------------------------------------
    StrX(const XMLCh* const toTranscode)
    {
        // Call the private transcoding method
        fLocalForm = XMLString::transcode(toTranscode);
    }

    ~StrX()
    {
        XMLString::release(&fLocalForm);
    }

    // -----------------------------------------------------------------------
    //  Getter methods
    // -----------------------------------------------------------------------
    const char* localForm() const
    {
        return fLocalForm;
    }

private :
    // -----------------------------------------------------------------------
    //  Private data members
    //
    //  fLocalForm
    //      This is the local code page form of the string.
    // -----------------------------------------------------------------------
    char*   fLocalForm;
};

#ifdef WIN32
std::ostream& operator<<(std::ostream& target, const StrX& toDump)
#else
inline std::ostream& operator<<(std::ostream& target, const StrX& toDump)
#endif
{
    target << toDump.localForm();
    return target;
}

#ifdef WIN32
bool DOMCountErrorHandler::getSawErrors() const
#else
inline bool DOMCountErrorHandler::getSawErrors() const
#endif
{
    return fSawErrors;
}


// forward declarations
#ifdef USE_SPECIAL_PARSING_PROP
  class SpecialParsingUsePropTag_c;
#else
  class SpecialParsingBasePropTag_c;
#endif

#ifdef USE_SPECIAL_PARSING
  class SpecialParsingUse_c;
#else
  class SpecialParsingBase_c;
#endif

class vt2iso_c
{
public:
  vt2iso_c (char* poolIdent);

  ~vt2iso_c();

  bool processElement (DOMNode *n, uint64_t ombType/* const char* rpcc_inKey, const char* rpcc_inButton, */);

  bool processPointElements(unsigned int& r_objChildPoints, DOMNode *r_n);

  bool processMacroElements(unsigned int& r_objMacros, DOMNode *r_n);

  bool processChildElements(unsigned int& r_objChildren, DOMNode *r_n, bool xyNeeded);

  void clean_exit(char* error_message = NULL);

  bool getIsOPDimension() const { return is_opDimension; }
  bool getIsOPAdditionallyRequiredObjects() const { return is_opAdditionallyRequiredObjects; }
  bool getIsSKWidth() const { return is_skWidth; }
  bool getIsSKHeight() const { return is_skHeight; }

  void setIsOPDimension (bool b_isOPDimension) { is_opDimension = b_isOPDimension; }
  void setIsOPAdditionallyRequiredObjects (bool b_isOPAdditionallyRequiredObjects) { is_opAdditionallyRequiredObjects = b_isOPAdditionallyRequiredObjects; }
  void setIsSKWidth (bool b_isSKWidth) { is_skWidth = b_isSKWidth; }
  void setIsSKHeight (bool b_isSKHeight) { is_skHeight = b_isSKHeight; }

  unsigned int getOPDimension() const { return opDimension; }
  unsigned int getSKWidth() const { return skWidth; }
  unsigned int getSKHeight() const { return skHeight; }

  const char* getOPAdditionallyRequiredObjects() const { return opAdditionallyRequiredObjects; }

  void setOPDimension (unsigned int OPDimension) { opDimension = OPDimension; }
  void setSKWidth (unsigned int SKWidth) { skWidth = SKWidth; }
  void setSKHeight (unsigned int SKHeight) { skHeight = SKHeight; }

  void skRelatedFileOutput();

  void generateIncludeDefines();

  int getAmountXmlFiles() const {return amountXmlFiles;}

  const char* getXmlFile (int index) { return xmlFiles[index]; }

  const char* getAttributeValue (DOMNode* pc_node, const char* attributeName);

  void getKeyCode();

  void init (const char* xmlFile, std::basic_string<char>* dictionary, bool ab_externalize, bool ab_disableContainmentRules);

  bool prepareFileNameAndDirectory (std::basic_string<char>* pch_fileName);

  void convertIdReferenceToNameReference (int ai_attrType);

  signed long int idOrName_toi (char* apc_string, bool ab_isMacro);

  void defaultAttributes (unsigned int a_objType);


private:
  signed int strlenUnescaped (const char* pcc_string);

  bool copyWithQuoteAndLength (char *dest, const char *src, unsigned int len);

  signed long int getID (const char* objName, bool b_isMacro, bool b_wishingID, unsigned int wishID);

  signed long int setID (const char* objName, unsigned int wishID);

  void convertIdReferencesToNameReferences();

  int languageCodeToIndex (char* lc);

  void setAttributeValue (int attrID);
  void cleanAttribute (int attrID);

  bool checkForFileOrFile148 (char *tag);

  DOMNamedNodeMap* patched_getAttributes (DOMNode *n);

  bool getAttributesFromNode (DOMNode *n, bool treatSpecial);

  bool openDecodePrintOut (const char* workDir, char* _bitmap_path, unsigned int &options, int fixNr=-1);

  bool checkForAllowedExecution() const;

  std::string getObjNameWithPoolIdent (char* pcch_objName);

  void autoDetectLanguage (DOMNode *n);

  void splitFunction (bool ab_onlyClose);

private:
  bool firstLineFileE;

  FILE *partFile_variables;
  FILE *partFile_variables_extern;
  FILE *partFile_attributes;
  FILE *partFile_attributes_extern;
  FILE *partFile_functions;
  FILE *partFile_functions_origin;
  FILE *partFile_defines;
  FILE *partFile_list;
//  FILE *partFile_list_extern;
  FILE *partFile_handler_direct;
  FILE *partFile_handler_derived;
  FILE *partFile_split_function;
  FILE *partFile_obj_selection;

  unsigned int ui_languages;

  char xmlFileGlobal [1024+1];
  char std_bitmap_path [1024+1];
  char fix_bitmap_path [1024+1];
  char spc_autoLanguage[1024+1];

  std::string proName;
  std::basic_string<char> c_project;

  int amountXmlFiles;
  std::basic_string<char> c_directory;
  const char* ac_workDir;
  char xmlFiles [256] [1024+1];
  const char* pcch_poolIdent;

  char objNameTable [(stringLength+1)*DEF_MAX_OBJECTS];
  unsigned int objIDTable [DEF_MAX_OBJECTS];
  unsigned int objNextAutoID;
  unsigned int objNextMacroAutoID;
  unsigned int kcNextAutoID;
  unsigned int objNextUnnamedName;
  unsigned int objCount;
  unsigned int opDimension;
  unsigned int skWidth;
  unsigned int skHeight;
  char opAdditionallyRequiredObjects[1024+1];
  bool is_opDimension;
  bool is_opAdditionallyRequiredObjects;
  bool is_skWidth;
  bool is_skHeight;

  bool b_externalize;

  char attrString [maxAttributeNames] [stringLength+1];
  bool attrIsGiven [maxAttributeNames];

  typedef std::map<uint16_t, std::string> ObjListEntry;

  std::map<int32_t, ObjListEntry> map_objNameAndID;
  static bool sb_WSFound;
  uint16_t ui16_WSObjID;

  language_s arrs_language [DEF_iso639entries];

  // Assuming an 8 bit per pixel bitmap.
  unsigned char picBuffer [480*480];

  char attr_name [1024+1];
  char attr_value [1024+1];
  char attr_value2 [1024+1];
  char filename [1024+1];

  char objName [stringLength+1];
  bool is_objName;
  unsigned int objID;
  bool is_objID;
  unsigned int objType;

  bool arrb_objTypes [ DEF_iso639entries ];

  bool b_hasUnknownAttributes;
  bool b_hasMoreThan6SoftKeys;
  bool b_disableContainmentRules;


#ifdef USE_SPECIAL_PARSING_PROP
  SpecialParsingUsePropTag_c* pc_specialParsingPropTag;
#else
  SpecialParsingBasePropTag_c* pc_specialParsingPropTag;
#endif

#ifdef USE_SPECIAL_PARSING
  SpecialParsingUse_c* pc_specialParsing;
#else
  SpecialParsingBase_c* pc_specialParsing;
#endif
};

#endif // VT2ISO_H
