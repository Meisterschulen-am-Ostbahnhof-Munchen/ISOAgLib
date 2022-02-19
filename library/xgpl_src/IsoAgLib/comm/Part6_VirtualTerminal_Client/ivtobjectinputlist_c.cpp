/*
 * ivtobjectinputlist_c.cpp
 *
 *  Created on: 05.02.2022
 *      Author: franz
 */

#include "ivtobjectinputlist_c.h"


namespace IsoAgLib {


    iVtObjectInputList_c::iVtObjectInputList_c(
    		iVtClientObjectPool_c *pool,
			ObjectID ID,
			uint16_t width,
			uint16_t height,
			iVtObjectNumberVariable_c *variableReference,
			uint8_t value,
			uint8_t options)
    	    :vtObjectInputList_c(
    				pool->getAiMultitonInst(),
    				ID,
					width,
					height,
					variableReference,
					value,
					options)
    		{
    			pool->Append(this);
    		}

    uint16_t iVtObjectInputList_c::objectType() { return VT_OBJECT_TYPE_INPUT_LIST; }

    iVtObject_c *iVtObjectInputList_c::getListItem(uint8_t xth) { return vtObjectInputList_c::getListItem (xth); }

    uint8_t iVtObjectInputList_c::getNumberOfListItems() { return vtObjectInputList_c::getNumberOfListItems(); }

    void iVtObjectInputList_c::setValue(uint8_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectInputList_c::setValue (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectInputList_c::setItem(uint8_t aui8_index, iVtObject_c *apc_object, bool b_enableReplaceOfCmd) {
        vtObjectInputList_c::setItem (aui8_index, apc_object, b_enableReplaceOfCmd);
    }

    void iVtObjectInputList_c::setWidth(uint16_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectInputList_c::setWidth (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectInputList_c::setHeight(uint16_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectInputList_c::setHeight (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectInputList_c::setVariableReference(iVtObjectNumberVariable_c *newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectInputList_c::setVariableReference(newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectInputList_c::setOptions(uint8_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectInputList_c::setOptions (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectInputList_c::setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectInputList_c::setSize(newWidth, newHeight, b_updateObject, b_enableReplaceOfCmd);
    }

    bool iVtObjectInputList_c::enable(bool b_updateObject, bool b_enableReplaceOfCmd) {
        return vtObjectInputList_c::enable (b_updateObject, b_enableReplaceOfCmd);
    }
    bool iVtObjectInputList_c::disable(bool b_updateObject, bool b_enableReplaceOfCmd) {
        return vtObjectInputList_c::disable (b_updateObject, b_enableReplaceOfCmd);
    }
    bool iVtObjectInputList_c::getEnabled() {
        return vtObjectInputList_c::getEnabled ();
    }

    bool iVtObjectInputList_c::select(uint8_t selectOrActivate) { return vtObject_c::select(selectOrActivate); }

    uint16_t iVtObjectInputList_c::updateWidth(bool b_SendRequest) {
        return vtObjectInputList_c::updateWidth(b_SendRequest);
    }

    uint16_t iVtObjectInputList_c::updateHeight(bool b_SendRequest) {
        return vtObjectInputList_c::updateHeight(b_SendRequest);
    }

    iVtObjectNumberVariable_c *iVtObjectInputList_c::updateVariableReference(bool b_SendRequest) {
        return vtObjectInputList_c::updateVariableReference(b_SendRequest);
    }

    uint16_t iVtObjectInputList_c::getObjectType() const { return objectType(); }

    iVtObjectInputList_c::~iVtObjectInputList_c() = default;
} // IsoAgLib



