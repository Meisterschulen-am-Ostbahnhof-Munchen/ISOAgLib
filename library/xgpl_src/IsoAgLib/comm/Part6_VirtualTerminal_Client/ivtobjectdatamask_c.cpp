//
// Created by franz on 28.01.22.
//


#include "ivtobjectdatamask_c.h"

namespace IsoAgLib {






    iVtObjectDataMask_c::iVtObjectDataMask_c(
    		iVtClientObjectPool_c* pool,
			ObjectID ID,
			Colour backgroundColour,
			iVtObjectSoftKeyMask_c *softKeyMask)
    : vtObjectDataMask_c(
    				pool->getAiMultitonInst(),
    				ID,
    				backgroundColour,
    				softKeyMask)
    {
    	pool->Append(this);
    }

#ifdef CONFIG_USE_VTOBJECT_softkeymask
    void iVtObjectDataMask_c::setSoftKeyMask(iVtObjectSoftKeyMask_c *newSoftKeyMask, bool b_updateObject,
                                             bool b_enableReplaceOfCmd) {
        vtObjectDataMask_c::setSoftKeyMask (newSoftKeyMask, b_updateObject, b_enableReplaceOfCmd);
    }
#endif

    void iVtObjectDataMask_c::setBackgroundColour(Colour newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        vtObjectDataMask_c::setBackgroundColour (newValue, b_updateObject, b_enableReplaceOfCmd);
    }

    uint16_t iVtObjectDataMask_c::getObjectType() const { return objectType(); }

    bool iVtObjectDataMask_c::setChildPosition(IsoAgLib::iVtObject_c *apc_childObject, int16_t dx, int16_t dy,
                                               bool b_updateObject, bool b_enableReplaceOfCmd) {
        return vtObjectDataMask_c::setChildPosition(apc_childObject, dx, dy, b_updateObject, b_enableReplaceOfCmd);
    }

    bool iVtObjectDataMask_c::moveChildLocation(iVtObject_c *apc_childObject, int8_t dx, int8_t dy, bool b_updateObject) {
        return vtObjectDataMask_c::moveChildLocation (apc_childObject, dx, dy, b_updateObject);
    }


    uint16_t iVtObjectDataMask_c::objectType() { return VT_OBJECT_TYPE_DATA_MASK; }


#ifdef CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES

    uint8_t iVtObjectDataMask_c::updateBackgroundColour(bool b_SendRequest) {
        return vtObjectDataMask_c::updateBackgroundColour(b_SendRequest);
    }

#ifdef CONFIG_USE_VTOBJECT_softkeymask
    uint16_t iVtObjectDataMask_c::updateSoftKeyMask(bool b_SendRequest) {
        return vtObjectDataMask_c::updateSoftKeyMask(b_SendRequest);
    }
#endif

#endif
}
