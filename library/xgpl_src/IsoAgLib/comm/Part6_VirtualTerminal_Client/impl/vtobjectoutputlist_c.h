/*
  vtobjectoutputlist_c.h

  (C) Copyright 2009 - 2019 by OSB AG

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Usage under Commercial License:
  Licensees with a valid commercial license may use this file
  according to their commercial license agreement. (To obtain a
  commercial license contact OSB AG via <http://isoaglib.com/en/contact>)

  Usage under GNU General Public License with exceptions for ISOAgLib:
  Alternatively (if not holding a valid commercial license)
  use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef VTOBJECTOUTPUTLIST_C_H
#define VTOBJECTOUTPUTLIST_C_H

#include <IsoAgLib/isoaglib_config.h>




#include "vtobject_c.h"
#include "vtclientconnection_c.h"


namespace __IsoAgLib {

class vtObjectOutputList_c : public vtObject_c
{
public:
  int16_t stream(uint8_t* destMemory,
                 uint16_t maxBytes,
                 objRange_t sourceOffset);

  void init(const iVtObjectOutputList_s* vtObjectOutputListSROM , int ai_multitonInst)
  { vtObject_c::init ((iVtObject_s*) vtObjectOutputListSROM , ai_multitonInst);}

  iVtObjectOutputList_s* get_vtObjectOutputList_a() { return dynamic_cast<iVtObjectOutputList_s *>(&(get_vtObject_a())); }

  vtObjectOutputList_c();

  IsoAgLib::iVtObject_c* getListItem(uint8_t xth);

  uint8_t getNumberOfListItems();

  uint32_t fitTerminal() const;

  void setValue(uint8_t newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=true);

  void setItem(uint8_t aui8_index, IsoAgLib::iVtObject_c* apc_object, bool b_enableReplaceOfCmd=false);

  // //////////////////////////////////
  // All special Attribute-Set methods
  void setWidth(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValue16SetAttributeScaled ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectOutputList_a(), width) : 0, sizeof(iVtObjectOutputList_s), 1, newValue, b_enableReplaceOfCmd);
  }

  void setHeight(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValue16SetAttributeScaled ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectOutputList_a(), height) : 0, sizeof(iVtObjectOutputList_s), 2, newValue, b_enableReplaceOfCmd);
  }

  void setVariableReference(IsoAgLib::iVtObject_c* newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValuePSetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectOutputList_a(), variableReference) : 0, sizeof(iVtObjectOutputList_s), 3, newValue, b_enableReplaceOfCmd);
  }

  void setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);

  // ///////////////////////// getter for attributes
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return 10; }
  */

  uint16_t updateWidth(bool b_SendRequest=false);
  uint16_t updateHeight(bool b_SendRequest=false);
  uint16_t updateVariableReference(bool b_SendRequest=false);

  /** these attributes are in parentheses in the spec, so commented out here
  uint8_t updateValue(bool b_SendRequest=false);
  uint8_t updateOptions(bool b_SendRequest=false);
  */

  void saveReceivedAttribute (uint8_t attrID, uint8_t* pui8_attributeValue);

private:
};

} // __IsoAgLib

#endif


