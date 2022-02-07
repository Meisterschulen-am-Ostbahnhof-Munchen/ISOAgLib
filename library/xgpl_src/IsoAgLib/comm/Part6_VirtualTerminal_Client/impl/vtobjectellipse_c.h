/*
  vtobjectellipse_c.h

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
#ifndef VTOBJECTELLIPSE_C_H
#define VTOBJECTELLIPSE_C_H

#include <IsoAgLib/isoaglib_config.h>




#include "vtobject_c.h"
#include "../ivtobjectlineattributes_c.h"
#include "../ivtobjectfillattributes_c.h"

namespace __IsoAgLib {

class vtObjectEllipse_c : public vtObject_c
{
private:
	// Internal implementation class
	struct iVtObjectEllipse_s;

	// Pointer to the internal implementation
	iVtObjectEllipse_s* vtObject_a;
	//TODO
	//std::unique_ptr<iVtObjectEllipse_s> vtObject_a;


public:
  int16_t stream(uint8_t* destMemory,
                 uint16_t maxBytes,
                 objRange_t sourceOffset);
  IsoAgLib::ObjectID getID() const;


  vtObjectEllipse_c(
		    int ai_multitonInst,
			IsoAgLib::ObjectID ID,
			IsoAgLib::iVtObjectLineAttributes_c *lineAttributes,
			uint16_t width,
			uint16_t height,
			uint8_t ellipseType,
			uint8_t startAngle,
			uint8_t endAngle,
			IsoAgLib::iVtObjectFillAttributes_c *fillAttributes);


  vtObjectEllipse_c(iVtObjectEllipse_s* vtObjectellipseSROM , int ai_multitonInst);

    iVtObjectEllipse_s* get_vtObjectEllipse_a();


  uint32_t fitTerminal() const;

    virtual void setLineAttributes(IsoAgLib::iVtObjectLineAttributes_c* newLineAttributes, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
    virtual void setWidth(uint16_t newWidth, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
    virtual void setHeight(uint16_t newHeight, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
    virtual void setEllipseType(uint8_t newEllipseType, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
    virtual void setStartAngle(uint8_t newStartAngle, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
    virtual void setEndAngle(uint8_t newEndAngle, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
    virtual void setFillAttributes(IsoAgLib::iVtObjectFillAttributes_c* newFillAttributes, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
    virtual void setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
  void setOriginSKM(bool b_SKM);

#ifdef USE_ISO_TERMINAL_GETATTRIBUTES
  // ///////////////////////// getter for attributes
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return 15; }
  */

  uint16_t updateLineAttributes(bool b_SendRequest=false);
  uint16_t updateWidth(bool b_SendRequest=false);
  uint16_t updateHeight(bool b_SendRequest=false);
  virtual uint8_t updateEllipseType(bool b_SendRequest=false);
  uint8_t updateStartAngle(bool b_SendRequest=false);
  uint8_t updateEndAngle(bool b_SendRequest=false);
  uint16_t updateFillAttributes(bool b_SendRequest=false);
  void saveReceivedAttribute (uint8_t attrID, uint8_t* pui8_attributeValue);
#endif
};

} // __IsoAgLib

#endif


