/*
 * ivtobjectstringvariable_c.cpp
 *
 *  Created on: 04.02.2022
 *      Author: franz
 */

#include "ivtobjectstringvariable_c.h"


namespace IsoAgLib {


    uint16_t iVtObjectStringVariable_c::objectType() { return VT_OBJECT_TYPE_STRING_VARIABLE; }



    void iVtObjectStringVariable_c::setValueCopy(const char *newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectStringVariable_c::setValueCopy (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectStringVariable_c::setValueRef(const char *newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectStringVariable_c::setValueRef (newValue, b_updateObject, b_enableReplaceOfCmd);
    }
#ifdef USE_VT_UNICODE_SUPPORT
    void iVtObjectStringVariable_c::setValueCopyUTF8(const char *newValue, uint8_t aui8_fontType, bool b_updateObject,
                                                     bool b_enableReplaceOfCmd) {
        vtObjectStringVariable_c::setValueCopyUTF8 (newValue, aui8_fontType, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectStringVariable_c::setValueCopyUTF8(const char *newValue, iVtObjectOutputString_c &arc_ops,
                                                     bool b_updateObject, bool b_enableReplaceOfCmd) {
        //TODO!!
        //const uint8_t cui8_fontType = arc_ops.get_vtObjectOutputString_a().fontAttributes->get_vtObjectFontAttributes_a().fontType;
        //vtObjectStringVariable_c::setValueCopyUTF8 (newValue, cui8_fontType, b_updateObject, b_enableReplaceOfCmd);
    }

    void iVtObjectStringVariable_c::setValueCopyUTF16(const char *newValue, uint16_t length, bool b_updateObject,
                                                      bool b_enableReplaceOfCmd) {
        vtObjectStringVariable_c::setValueCopyUTF16 (newValue, length, b_updateObject, b_enableReplaceOfCmd);
    }
#endif
    const char *iVtObjectStringVariable_c::getString() { return vtObjectStringVariable_c::getString(); }

    uint16_t iVtObjectStringVariable_c::getObjectType() const { return objectType(); }

iVtObjectStringVariable_c::iVtObjectStringVariable_c(
		iVtClientObjectPool_c *pool,
		ObjectID ID,
		char *value)
	: vtObjectStringVariable_c(pool->getAiMultitonInst()
	, ID
	, value /* size length+1 (0-termination intern!) */
	)
{
	pool->Append(this);
}

} // IsoAgLib
