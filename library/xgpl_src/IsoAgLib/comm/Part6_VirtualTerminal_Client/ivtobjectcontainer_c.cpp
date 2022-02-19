/*
 * ivtobjectcontainer_c.cpp
 *
 *  Created on: 05.02.2022
 *      Author: franz
 */

#include "ivtobjectcontainer_c.h"


#ifdef CONFIG_USE_VTOBJECT_container


namespace IsoAgLib {


  // implement here a normal constructor/destructor, as the compiler dislikes inlining of that simple
  // constructor/destructor direct in scope of iVtObjectContainer_c
  iVtObjectContainer_c::~iVtObjectContainer_c() = default;

    uint16_t iVtObjectContainer_c::objectType() { return VT_OBJECT_TYPE_CONTAINER; }

    void iVtObjectContainer_c::hide(bool b_updateObject, bool b_enableReplaceOfCmd) {
		vtObjectContainer_c::hideShow (0, b_updateObject, b_enableReplaceOfCmd);
	}

    void iVtObjectContainer_c::show(bool b_updateObject, bool b_enableReplaceOfCmd) {
		vtObjectContainer_c::hideShow (1, b_updateObject, b_enableReplaceOfCmd);
	}

    void iVtObjectContainer_c::setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectContainer_c::setSize(newWidth, newHeight, b_updateObject, b_enableReplaceOfCmd);
    }

    bool iVtObjectContainer_c::moveChildLocation(iVtObject_c *apc_childObject, int16_t dx, int16_t dy, bool b_updateObject, bool b_enableReplaceOfCmd) {
        return vtObjectContainer_c::moveChildLocation(apc_childObject, dx, dy, b_updateObject, b_enableReplaceOfCmd);
    }

    bool iVtObjectContainer_c::setChildPosition(iVtObject_c *apc_childObject, int16_t dx, int16_t dy, bool b_updateObject, bool b_enableReplaceOfCmd) {
        return vtObjectContainer_c::setChildPosition(apc_childObject, dx, dy, b_updateObject, b_enableReplaceOfCmd);
    }

    uint16_t iVtObjectContainer_c::getObjectType() const { return objectType(); }

iVtObjectContainer_c::iVtObjectContainer_c(
		iVtClientObjectPool_c *pool,
		ObjectID ID,
		uint16_t width,
		uint16_t height,
		uint8_t hidden)
:vtObjectContainer_c(
		pool->getAiMultitonInst(),
		ID,
		width,
		height,
		hidden)
{
	pool->Append(this);
}

} // IsoAgLib

#endif //CONFIG_USE_VTOBJECT_container
