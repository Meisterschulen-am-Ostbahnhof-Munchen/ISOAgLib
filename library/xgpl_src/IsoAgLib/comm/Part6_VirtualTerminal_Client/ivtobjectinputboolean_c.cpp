/*
 * ivtobjectinputboolean_c.cpp
 *
 *  Created on: 05.02.2022
 *      Author: franz
 */


#include "ivtobjectinputboolean_c.h"


namespace IsoAgLib {


    iVtObjectInputBoolean_c::iVtObjectInputBoolean_c(
    		iVtClientObjectPool_c* pool,
			ObjectID ID,
			Colour backgroundColour,
			uint16_t width,
			iVtObjectFontAttributes_c *foregroundColour,
			iVtObjectNumberVariable_c *variableReference,
			uint8_t value,
			uint8_t enabled)
    :vtObjectInputBoolean_c(
			pool->getAiMultitonInst(),
			ID,
			backgroundColour,
			width,
			foregroundColour,
			variableReference,
			value,
			enabled)
	{
		pool->Append(this);
	}

    uint16_t iVtObjectInputBoolean_c::objectType() { return VT_OBJECT_TYPE_INPUT_BOOLEAN; }

    void iVtObjectInputBoolean_c::setValue(bool newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectInputBoolean_c::setValue (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectInputBoolean_c::setBackgroundColour(Colour newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectInputBoolean_c::setBackgroundColour (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectInputBoolean_c::setWidth(uint16_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectInputBoolean_c::setWidth (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectInputBoolean_c::setForegroundColour(iVtObjectFontAttributes_c *newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectInputBoolean_c::setForegroundColour (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectInputBoolean_c::setVariableReference(iVtObjectNumberVariable_c *newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectInputBoolean_c::setVariableReference (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    bool iVtObjectInputBoolean_c::enable(bool b_updateObject, bool b_enableReplaceOfCmd) {
        return vtObjectInputBoolean_c::enable (b_updateObject, b_enableReplaceOfCmd);
    }
    bool iVtObjectInputBoolean_c::disable(bool b_updateObject, bool b_enableReplaceOfCmd) {
        return vtObjectInputBoolean_c::disable (b_updateObject, b_enableReplaceOfCmd);
    }
    bool iVtObjectInputBoolean_c::getEnabled() {
        return vtObjectInputBoolean_c::getEnabled ();
    }

    bool iVtObjectInputBoolean_c::select(uint8_t selectOrActivate) { return vtObject_c::select(selectOrActivate); }

    iVtObjectInputBoolean_c::~iVtObjectInputBoolean_c() = default;


} // IsoAgLib



