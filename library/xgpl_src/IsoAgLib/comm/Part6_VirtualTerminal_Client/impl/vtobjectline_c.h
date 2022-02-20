/*
  vtobjectline_c.h

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
#ifndef VTOBJECTLINE_C_H
#define VTOBJECTLINE_C_H

#include <IsoAgLib/isoaglib_config.h>


#ifdef CONFIG_USE_VTOBJECT_line

#include <memory> // PImpl
#include "vtobject_c.h"
#include "vtclient_c.h"

namespace __IsoAgLib {

class vtObjectLine_c : public vtObject_c
{
private:
	enum AttributeID:uint8_t;
	// Internal implementation class
	struct iVtObjectLine_s;

	// Pointer to the internal implementation
	std::unique_ptr<iVtObjectLine_s> vtObject_a;


public:
  uint16_t stream(uint8_t* destMemory,
                 uint16_t maxBytes,
                 objRange_t sourceOffset);
  IsoAgLib::ObjectID getID() const;


  vtObjectLine_c() = delete;

  vtObjectLine_c(
		  multiton ai_multitonInst,
		  IsoAgLib::ObjectID ID,
		  IsoAgLib::iVtObjectLineAttributes_c *lineAttributes,
		  uint16_t width,
		  uint16_t height,
		  IsoAgLib::LineDirection lineDirection
		  );


  vtObjectLine_c(iVtObjectLine_s* vtObjectLineSROM , multiton ai_multitonInst);

  virtual ~vtObjectLine_c();

  uint32_t fitTerminal() const;
  void setOriginSKM(bool b_SKM);

    // //////////////////////////////////
    // All special Attribute-Set methods
    virtual void setLineAttributes(IsoAgLib::iVtObjectLineAttributes_c* newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
    virtual void setWidth (uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
    virtual void setHeight (uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
    virtual void setLineDirection (IsoAgLib::LineDirection newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
    virtual void setEndPoint (uint16_t newWidth, uint16_t newHeight, IsoAgLib::LineDirection newLineDirection, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
    virtual void setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);

#ifdef CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES
  // ///////////////////////// getter for attributes
  virtual /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return 13; }
  */
  IsoAgLib::iVtObjectLineAttributes_c*  updateLineAttributes(bool b_SendRequest=false);
  virtual uint16_t updateWidth(bool b_SendRequest=false);
  virtual uint16_t updateHeight(bool b_SendRequest=false);
  virtual uint8_t updateLineDirection(bool b_SendRequest=false);
  void saveReceivedAttribute (uint8_t attrID, uint8_t* pui8_attributeValue);
#endif
};

} // __IsoAgLib

#endif //CONFIG_USE_VTOBJECT_line

#endif //VTOBJECTLINE_C_H
