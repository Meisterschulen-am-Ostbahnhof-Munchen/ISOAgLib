/***************************************************************************
                          specialparsingbase_c.h
                             -------------------
    begin                : Mon Mar 31 2003
    copyright            : (C) 2003 - 2004 by Dipl.-Inf.(FH) Martina Winkler
    email                : m.winkler@osb-ag:de
    type                 : Header
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
#ifndef SPECIALPARSINGBASE_C_H
#define SPECIALPARSINGBASE_C_H

#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMDocumentType.hpp>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>
#include <xercesc/dom/DOMNodeIterator.hpp>
#include <xercesc/dom/DOMNodeList.hpp>
#include <xercesc/dom/DOMText.hpp>

#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/XMLUni.hpp>

#ifndef SYSTEM_PC_VC
#include <stdint.h>
#else
  typedef unsigned char uint8_t;
  typedef signed char int8_t;
  typedef unsigned short uint16_t;
  typedef short int16_t;
  typedef unsigned int uint32_t;
  typedef int int32_t;
  typedef unsigned long long int uint64_t;
  typedef long int long int64_t;
#endif

#include <string>

XERCES_CPP_NAMESPACE_USE

class SpecialParsingBase_c {
public:
  /** that function tries to parse an already known tag from vt2iso which contains
    * unknown attribute tags
    * @return returns 0 if successful else anything unequal to 0
    * @param pNode pointer to element in xml file
    * @param objectType objectType of known xml tag
    * @param objName name of object
    * @param pb_isObjID pointer to flag if object is wished
    * @param pui16_objID pointer to variable of object ID
    * @param pcch_SKM pointer to softkeymask if object is a datamask or alarmmask
    * @param pcch_varRef1st pointer to variable reference if given
    * @param pcch_varRef2nd pointer to 2nd variable reference (only used for linear and arched bar graph)
  */
  virtual bool parseKnownTag (DOMNode* pNode, const uint8_t objectType,
                              const char* objName, unsigned int* pui_objID,
                              bool* pb_isObjID, const char* pcch_SKM=NULL,
                              const char* pcch_varRef1st=NULL, const char* pcch_varRef2nd=NULL) = 0;

  /** check if found tag is invalid at this position */
  virtual bool checkTag(DOMNode* pNode, unsigned int objectType, uint64_t ombType) = 0;

  /** that function tries to parse an unknown tag from vt2iso
    * @return returns 0 if successful else anything unequal to 0
    * @param pNode pointer to element in xml file
    * @param objectType object type of pNode
    * @param pb_isObjID pointer to boolean
  */
  virtual bool parseUnknownTag (DOMNode* pNode, unsigned int objectType, bool* pb_isObjID, char* pc_objName, unsigned int rui_objID) = 0;

  /// returns the object type from a given DOMNode if type is known and sets it
  virtual uint16_t getObjType (const char* node_name) { return 0xFFFF; }

  /// returns the total sum of basic and special object types
  virtual uint8_t getMaxOfObjTypes() = 0;

  /// if a tag has unknown attributes, set that flag
  virtual void setUnknownAttributes (bool b_hasUnknownAttr) {}

  /** that functions creates all necessary files and prints out all collected data */
  virtual void outputCollectedData2Files() {}

  /** that functions writes to already existing files and prints out a record during runtime of vt2iso */
  virtual void outputData2FilesPiecewise() {}

  /** that functions writes all necessary file includes into a given file */
  virtual void addFileIncludes(FILE* p_includeFile, const char* fileName) {}

  /** check if the found wish ID is already used as resource ID
    * @return if true, the found ID is NOT used as resource ID
  */
  virtual bool checkUseOfResourceID (unsigned int objID) { return true; }

  /// returns if the current parsing module contains proprietary object types which can be handled like basic object types
  virtual bool checkForProprietaryOrBasicObjTypes() = 0;

  virtual ~SpecialParsingBase_c() {}
};

#endif
