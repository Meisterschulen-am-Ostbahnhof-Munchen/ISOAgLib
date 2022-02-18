/*
  vtobjectfillattributes_c.h

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
#ifndef VTOBJECTFILLATTRIBUTES_C_H
#define VTOBJECTFILLATTRIBUTES_C_H

#include <IsoAgLib/isoaglib_config.h>


#ifdef CONFIG_USE_VTOBJECT_fillattributes

#include <memory> // PImpl
#include "vtobject_c.h"
#include "../ivtobjectpicturegraphic_c.h"


namespace __IsoAgLib {

class vtObjectFillAttributes_c : public vtObject_c
{
private:
	enum AttributeID:uint8_t;
	// Internal implementation class
	struct iVtObjectFillAttributes_s;

	// Pointer to the internal implementation
	std::unique_ptr<iVtObjectFillAttributes_s> vtObject_a;

	vtObjectFillAttributes_c() = delete;
	vtObjectFillAttributes_c(iVtObjectFillAttributes_s* vtObjectFillAttributesSROM , int ai_multitonInst);


public:
  int16_t stream(uint8_t* destMemory,
                 uint16_t maxBytes,
                 objRange_t sourceOffset);
  IsoAgLib::ObjectID getID() const;



  vtObjectFillAttributes_c(
		  int ai_multitonInst
		, IsoAgLib::ObjectID ID
  		, IsoAgLib::FillType fillType
		, IsoAgLib::Colour fillColour
#ifdef CONFIG_USE_VTOBJECT_picturegraphic
		, IsoAgLib::iVtObjectPictureGraphic_c *fillPatternObject
#endif
		);



  virtual ~vtObjectFillAttributes_c();

  uint32_t fitTerminal() const;

    virtual void setFillType(IsoAgLib::FillType newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);

    virtual void setFillColour(IsoAgLib::Colour newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
#ifdef CONFIG_USE_VTOBJECT_picturegraphic

        virtual void setFillPattern(IsoAgLib::iVtObjectPictureGraphic_c* newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);

        virtual void setFillAttributes(IsoAgLib::FillType newFillType, IsoAgLib::Colour newFillColour, IsoAgLib::iVtObjectPictureGraphic_c* newFillPattern, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
#endif

#ifdef CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES

        virtual /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return 25; }
  */

  IsoAgLib::FillType updateFillType(bool b_SendRequest=false);

        virtual IsoAgLib::Colour updateFillColour(bool b_SendRequest=false);

#ifdef CONFIG_USE_VTOBJECT_picturegraphic

        virtual IsoAgLib::iVtObjectPictureGraphic_c* updateFillPattern(bool b_SendRequest=false);
#endif

  void saveReceivedAttribute (uint8_t attrID, uint8_t* pui8_attributeValue);
#endif
};

} // __IsoAgLib

#endif //CONFIG_USE_VTOBJECT_fillattributes

#endif //VTOBJECTFILLATTRIBUTES_C_H
