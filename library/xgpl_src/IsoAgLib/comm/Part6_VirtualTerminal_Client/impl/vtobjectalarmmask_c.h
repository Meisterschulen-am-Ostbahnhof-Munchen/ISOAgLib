/*
  vtobjectalarmmask_c.h

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
#ifndef VTOBJECTALARMMASK_C_H
#define VTOBJECTALARMMASK_C_H

#include <IsoAgLib/isoaglib_config.h>

#ifdef CONFIG_USE_VTOBJECT_alarmmask

#include <memory> // PImpl
#include "../ivtobjectmask_c.h"
#include "../ivtobjectsoftkeymask_c.h"
#include "vtclient_c.h"


namespace __IsoAgLib {

class vtObjectAlarmMask_c : public IsoAgLib::iVtObjectMask_c
{
private:
	enum AttributeID:uint8_t;
	// Internal implementation class
	struct iVtObjectAlarmMask_s;

	// Pointer to the internal implementation
	std::unique_ptr<iVtObjectAlarmMask_s> vtObject_a;


public:
  int16_t stream(uint8_t* destMemory,
                 uint16_t maxBytes,
                 objRange_t sourceOffset);
  IsoAgLib::ObjectID getID() const;

  vtObjectAlarmMask_c(
		    int ai_multitonInst
		  , IsoAgLib::ObjectID ID
		  , IsoAgLib::Colour backgroundColour
#ifdef CONFIG_USE_VTOBJECT_softkeymask
		  , IsoAgLib::iVtObjectSoftKeyMask_c *softKeyMask
#endif
		  , uint8_t priority
		  , uint8_t acousticSignal);

  vtObjectAlarmMask_c(iVtObjectAlarmMask_s* vtObjectAlarmMaskSROM , int ai_multitonInst);

  ~vtObjectAlarmMask_c() override;

  uint32_t fitTerminal() const;

    virtual void setBackgroundColour(IsoAgLib::Colour newValue,  bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
#ifdef CONFIG_USE_VTOBJECT_softkeymask
  void setSoftKeyMask(IsoAgLib::iVtObjectSoftKeyMask_c* newSoftKeyMask, bool b_updateObject= false, bool b_enableReplaceOfCmd=false);
#endif

        virtual void setPriority(uint8_t newValue,  bool b_updateObject=false, bool b_enableReplaceOfCmd=false);

        virtual void setAcousticSignal(uint8_t newValue,  bool b_updateObject=false, bool b_enableReplaceOfCmd=false);

        virtual bool moveChildLocation(IsoAgLib::iVtObject_c* apc_childObject, int8_t dx, int8_t dy, bool b_updateObject=false);

        virtual bool setChildPosition(IsoAgLib::iVtObject_c* apc_childObject, int16_t dx, int16_t dy, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);

#ifdef CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES

        virtual /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return 2; }
  */

  IsoAgLib::Colour updateBackgroundColour(bool b_SendRequest=false);
#ifdef CONFIG_USE_VTOBJECT_softkeymask

        virtual IsoAgLib::iVtObjectSoftKeyMask_c * updateSoftKeyMask(bool b_SendRequest=false);
#endif

        virtual uint8_t updatePriority(bool b_SendRequest=false);

        virtual uint8_t updateAcousticSignal(bool b_SendRequest=false);

  void saveReceivedAttribute (uint8_t attrID, uint8_t* pui8_attributeValue);
#endif
};

} // __IsoAgLib

#endif

#endif
