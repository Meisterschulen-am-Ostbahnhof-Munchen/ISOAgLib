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
#include <xercesc/dom/DOMBuilder.hpp>
#include <xercesc/util/XMLString.hpp>
#include <iostream>
#include <set>
#include <vector>
#include <map>
#include <list>
#include <bitset>

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

inline std::ostream& operator<<(std::ostream& target, const StrX& toDump)
{
    target << toDump.localForm();
    return target;
}

inline bool DOMCountErrorHandler::getSawErrors() const
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

class OneAttribute_c
{
public:
  void clear();
  void set (const std::string& newString);
  const std::string& get();
  void setIfNotGiven (const std::string& newString);
  bool isGiven();
  int getLength();
  int getIntValue();
  char getCharacterAtPos(int index);
  void setCharacterAtPos(int index, char character);
  void pushBack(char ch);
  bool isNull();
  std::string getObjectReference();
  std::string getObjectReferencePrefixed (const std::string& arstr_prefix);

private:
  std::string attrString;
  bool attrIsGiven;
};


class vt2iso_c : public DOMCountErrorHandler
{
public:
  vt2iso_c (const std::string &poolIdent);

  ~vt2iso_c();

  struct Path_s
  {
    Path_s() : b_relativePath(false) {}
    std::string str_pathName;
    bool b_relativePath;
  };

  enum action_en
  {
    actionMarkIds,
    actionProcessElement
  };

  bool setCommandElement(unsigned int& commandType, DOMNode  *child,  /*DOMNamedNodeMap *pAttributes,*/ DOMNode *n, unsigned int& objChildCommands, std::string &commandMessage);

  void markIds (DOMNode *n);

  bool processElement (DOMNode *n, uint64_t ombType/* const char* rpcc_inKey, const char* rpcc_inButton, */);

  bool processPointElements(unsigned int& r_objChildPoints, DOMNode *r_n, bool ab_outputEnabled = true);

  bool processMacroElements(unsigned int& r_objMacros, DOMNode *r_n, bool ab_outputEnabled = true);

  bool processChildElements(unsigned int& r_objChildren, DOMNode *r_n, bool xyNeeded, bool ab_outputEnabled = true);

  void clean_exit(const char* error_message = NULL);

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

  const std::string& getOPAdditionallyRequiredObjects() const { return opAdditionallyRequiredObjects; }

  void setOPDimension (unsigned int OPDimension) { opDimension = OPDimension; }
  void setSKWidth (unsigned int SKWidth) { skWidth = SKWidth; }
  void setSKHeight (unsigned int SKHeight) { skHeight = SKHeight; }

  void skRelatedFileOutput();

  void generateIncludeDefines();

  int getAmountXmlFiles() const {return vec_xmlFiles.size();}

  // don't keep this reference too long, xmlFiles may get changed.
  const std::string& getXmlFile (int index) { return vec_xmlFiles[index].str_pathName; }
  bool getXmlFileRelativePath (int index) { return vec_xmlFiles[index].b_relativePath; }

  const char* getAttributeValue (DOMNode* pc_node, const char* attributeName);

  void getKeyCode();

  void init (const std::string& xmlFile, std::string* dictionary, bool ab_externalize, bool ab_disableContainmentRules, DOMBuilder* ap_parser, bool ab_verbose);

  void parse();

  bool prepareFileNameAndDirectory (std::string& astr_fileName);

  void convertIdReferenceToNameReference (int ai_attrType);

  signed long int idOrName_toi (const char* apc_string, bool ab_isMacro);

  void defaultAttributes (unsigned int a_objType);

  void setParseModeWorkingSet (bool ab_parseOnlyWorkingSet) { mb_parseOnlyWorkingSet = ab_parseOnlyWorkingSet; }
  bool isParseModeWorkingSet() { return mb_parseOnlyWorkingSet;}

private:
  signed int strlenUnescaped (const std::string& pcc_string);

  bool copyWithQuoteAndLength (std::string &dest, const std::string& src, unsigned int len);

  signed long int getID (const char* objName, bool b_isMacro, bool b_wishingID, unsigned int wishID);

  signed long int setID (const char* objName, unsigned int wishID);

  void convertIdReferencesToNameReferences();

  int languageCodeToIndex (char* lc);

  void setAttributeValue (int attrID);
  void cleanAttribute (int attrID);

  bool checkForFileOrFile148 (const char *tag);

  DOMNamedNodeMap* patched_getAttributes (DOMNode *n);

  bool getAttributesFromNode (DOMNode *n, bool treatSpecial);

  bool openDecodePrintOut (const std::list<Path_s>& rcl_bitmapPath, unsigned int &options, int fixNr=-1);

  bool checkForAllowedExecution() const;

  std::string getObjectReferencePrefixed (int ai_attributeIndex) { return arrc_attributes [ai_attributeIndex].getObjectReferencePrefixed (mstr_poolIdent); }

  void autoDetectLanguage (DOMNode *n);

  void splitFunction (bool ab_onlyClose);

  bool processProjectFile(const std::string& pch_fileName);

  unsigned int getFreeId (unsigned int& aui_objNextId);

  bool doAllFiles (action_en aen_action);

public:
  static bool isVerbose() { return smb_verbose; }
  static bool smb_verbose;

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

//  char xmlFileGlobal [1024+1];
  std::string xmlFileGlobal;
  std::string spc_autoLanguage;
  std::string proName;
  std::string c_project;

  bool mb_projectFile;
  std::string c_directory;
  const char* ac_workDir;
  std::vector<Path_s> vec_xmlFiles;
//string array
  std::string mstr_poolIdent;

  std::bitset<65536> mbitset_objIdUsed;

  std::map<std::string, uint16_t> map_objNameIdTable;

  unsigned int objNextAutoID;
  unsigned int objNextMacroAutoID;
  unsigned int kcNextAutoID;
  unsigned int objNextUnnamedName;
  unsigned int opDimension;
  unsigned int skWidth;
  unsigned int skHeight;
  std::string opAdditionallyRequiredObjects;
  bool is_opDimension;
  bool is_opAdditionallyRequiredObjects;
  bool is_skWidth;
  bool is_skHeight;

  bool b_externalize;
  bool mb_parseOnlyWorkingSet;

  OneAttribute_c arrc_attributes [maxAttributeNames];
  void clearAndSetElements (DOMNode *child, const std::vector <int> &avec);

  typedef std::map<uint16_t, std::string> ObjListEntry;

  std::map<int32_t, ObjListEntry> map_objNameAndID;
  static bool sb_WSFound;
  uint16_t ui16_WSObjID;

  language_s arrs_language [DEF_iso639entries];

  // Assuming an 8 bit per pixel bitmap.
  unsigned char picBuffer [480*480];

  std::string attr_name;
  std::string attr_value;
  std::string attr_value2;
  std::string filename;

  std::string objName;
  bool is_objName;
  unsigned int objID;
  bool is_objID;
  unsigned int objType;

  bool arrb_objTypes [ DEF_iso639entries ];

  bool b_hasUnknownAttributes;
  bool b_hasMoreThan6SoftKeys;
  bool b_disableContainmentRules;
  bool errorOccurred;

  // used in processElement(...)
  char* m_nodeName;

  DOMBuilder* parser;
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

  std::list<Path_s> l_stdBitmapPath;
  std::list<Path_s> l_fixedBitmapPath;
};

#endif // VT2ISO_H
