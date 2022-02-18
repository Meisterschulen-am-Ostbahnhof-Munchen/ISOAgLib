/*
  ivtobjectalarmmask_c.h

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
#ifndef IVTOBJECTALARMMASK_C_H
#define IVTOBJECTALARMMASK_C_H

#include "impl/vtobjectalarmmask_c.h"

#ifdef CONFIG_USE_VTOBJECT_alarmmask

#include "ivtobjectsoftkeymask_c.h"


namespace IsoAgLib {

class iVtObjectAlarmMask_c : public __IsoAgLib::vtObjectAlarmMask_c
{
public:
  static uint16_t objectType() { return VT_OBJECT_TYPE_ALARM_MASK; }


  iVtObjectAlarmMask_c(
			        iVtClientObjectPool_c* pool
				  , ObjectID ID = autoID
				  , Colour backgroundColour = BLACK
#ifdef CONFIG_USE_VTOBJECT_softkeymask
				  , iVtObjectSoftKeyMask_c *softKeyMask = nullptr
#endif
				  , uint8_t priority = 0
		          , uint8_t acousticSignal = 0)
	:vtObjectAlarmMask_c(
					   pool->getAiMultitonInst()
					 , ID
					 , backgroundColour
#ifdef CONFIG_USE_VTOBJECT_softkeymask
					 , softKeyMask
#endif
					 , priority
			         , acousticSignal)
	{
		pool->Append(this);
  }
  ~iVtObjectAlarmMask_c() override;


  void setBackgroundColour(Colour newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) override;

#ifdef CONFIG_USE_VTOBJECT_softkeymask
  void setSoftKeyMask(iVtObjectSoftKeyMask_c* newSoftKeyMask, bool b_updateObject= false, bool b_enableReplaceOfCmd=false) override;
#endif

  void setPriority(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) override;

  void setAcousticSignal(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) override;

  bool moveChildLocation(iVtObject_c* apc_childObject, int8_t dx, int8_t dy, bool b_updateObject=false) override;

  bool setChildPosition(IsoAgLib::iVtObject_c* apc_childObject, int16_t dx, int16_t dy, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) override;
#ifdef CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES
  // ///////////////////////// getter for attributes
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const {
    return vtObjectAlarmMask_c::updateObjectType();
  }
   */

  IsoAgLib::Colour updateBackgroundColour(bool b_SendRequest=false) override;


#ifdef CONFIG_USE_VTOBJECT_softkeymask
  IsoAgLib::iVtObjectSoftKeyMask_c * updateSoftKeyMask(bool b_SendRequest=false) override;
#endif


  uint8_t updatePriority(bool b_SendRequest=false) override;

  uint8_t updateAcousticSignal(bool b_SendRequest=false) override;
#endif // CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES

  uint16_t getObjectType() const override;
};

} // IsoAgLib

#endif // CONFIG_USE_VTOBJECT_alarmmask

#endif // IVTOBJECTALARMMASK_C_H
