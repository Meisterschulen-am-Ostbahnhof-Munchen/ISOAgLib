/*
 * ivtobjectauxiliarypointer_c.cpp
 *
 *  Created on: 05.02.2022
 *      Author: franz
 */



#include "ivtobjectauxiliarypointer_c.h"


namespace IsoAgLib {


    iVtObjectAuxiliaryControlDesignatorObjectPointer_c::~iVtObjectAuxiliaryControlDesignatorObjectPointer_c() = default;

    void iVtObjectAuxiliaryControlDesignatorObjectPointer_c::setAuxiliaryObjectId(iVtObject_c *apc_newObject, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectAuxiliaryControlDesignatorObjectPointer_c::setAuxiliaryObjectId (apc_newObject, b_updateObject, b_enableReplaceOfCmd);
    }

    ObjectType iVtObjectAuxiliaryControlDesignatorObjectPointer_c::getObjectType() const { return objectType(); }
} // IsoAgLib



