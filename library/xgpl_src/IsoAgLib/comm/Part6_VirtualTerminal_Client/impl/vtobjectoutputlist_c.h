/*
  vtobjectoutputlist_c.h

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef VTOBJECTOUTPUTLIST_C_H
#define VTOBJECTOUTPUTLIST_C_H

#include <IsoAgLib/typedef.h>

#ifdef USE_VTOBJECT_outputlist
#include "vtobject_c.h"
#include "vtclientservercommunication_c.h"

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

class vtObjectOutputList_c : public vtObject_c
{
public:
  //  Operation: stream
  //! @param destMemory:
  //! @param maxBytes: don't stream out more than that or you'll overrun the internal upload-buffer
  //! @param sourceOffset:
  int16_t stream(uint8_t* destMemory,
                 uint16_t maxBytes,
                 objRange_t sourceOffset);

  //  Operation: init
  //! @param vtObjectOutputListSROM:
  //! @param b_initPointer:
  void init(const iVtObjectOutputList_s* vtObjectOutputListSROM SINGLETON_VEC_KEY_PARAMETER_DEF_WITH_COMMA)
  { vtObject_c::init ((iVtObject_s*) vtObjectOutputListSROM SINGLETON_VEC_KEY_PARAMETER_USE_WITH_COMMA);}

  //  Operation: get_vtObjectOutputList_a
  iVtObjectOutputList_s* get_vtObjectOutputList_a() { return (iVtObjectOutputList_s *)&(get_vtObject_a()); }

  //  Operation: vtObjectOutputList_c
  vtObjectOutputList_c();

  //  Operation: getListItem
  //! @param xth:
  IsoAgLib::iVtObject_c* getListItem(uint8_t xth);

  //  Operation: getNumberOfListItems
  uint8_t getNumberOfListItems();

  //  Operation: size
  uint32_t fitTerminal() const;

  //  Operation: setValue
  //! @param newValue:
  //! @param b_updateObject:
  void setValue(uint8_t newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=true);

  //  Operation: setItem
  //! @param aui8_index:
  //! @param apc_object:
  void setItem(uint8_t aui8_index, IsoAgLib::iVtObject_c* apc_object, bool b_enableReplaceOfCmd=false);

  // //////////////////////////////////
  // All special Attribute-Set methods
  void setWidth(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValue16SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectOutputList_a(), width) : 0, sizeof(iVtObjectOutputList_s), 1, newValue, b_enableReplaceOfCmd);
  }

  void setHeight(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValue16SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectOutputList_a(), height) : 0, sizeof(iVtObjectOutputList_s), 2, newValue, b_enableReplaceOfCmd);
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

} // end of namespace __IsoAgLib

#endif
#endif
