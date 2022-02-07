/*
  ivtobjectoutputlist_c.h

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
#ifndef IVTOBJECTOUTPUTLIST_C_H
#define IVTOBJECTOUTPUTLIST_C_H

#include "impl/vtobjectoutputlist_c.h"



namespace IsoAgLib {

class iVtObjectOutputList_c : public __IsoAgLib::vtObjectOutputList_c
{
public:
  static uint16_t objectType() { return VT_OBJECT_TYPE_OUTPUT_LIST; }


  iVtObject_c* getListItem(uint8_t xth) { return vtObjectOutputList_c::getListItem (xth); }

  uint8_t getNumberOfListItems() { return vtObjectOutputList_c::getNumberOfListItems(); }

  void setValue(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=true) {
    vtObjectOutputList_c::setValue (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  void setItem(uint8_t aui8_index, iVtObject_c* apc_object, bool b_enableReplaceOfCmd=false) {
    vtObjectOutputList_c::setItem (aui8_index, apc_object, b_enableReplaceOfCmd);
  }

  void setWidth(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectOutputList_c::setWidth (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  void setHeight(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectOutputList_c::setHeight (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  void setVariableReference(iVtObject_c* newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectOutputList_c::setVariableReference(newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  void setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectOutputList_c::setSize(newWidth, newHeight, b_updateObject, b_enableReplaceOfCmd);
  }


  // ///////////////////////// getter for attributes
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return vtObjectOutputList_c::updateObjectType(); }
  */
 
  uint16_t updateWidth(bool b_SendRequest=false) {
    return vtObjectOutputList_c::updateWidth(b_SendRequest);
  }
 
  uint16_t updateHeight(bool b_SendRequest=false) {
    return vtObjectOutputList_c::updateHeight(b_SendRequest);
  }

  uint16_t updateVariableReference(bool b_SendRequest=false) {
    return vtObjectOutputList_c::updateVariableReference(b_SendRequest);
  }

  /** these attributes are in parentheses in the spec, so commented out here
  uint8_t updateValue(bool b_SendRequest=false) {
    return vtObjectOutputList_c::getValue(b_SendRequest);
  }

  uint8_t updateOptions(bool b_SendRequest=false) {
    return vtObjectOutputList_c::updateOptions(b_SendRequest);
  }
  */

  virtual uint16_t getObjectType() const { return objectType(); }
};

} // IsoAgLib

#endif

