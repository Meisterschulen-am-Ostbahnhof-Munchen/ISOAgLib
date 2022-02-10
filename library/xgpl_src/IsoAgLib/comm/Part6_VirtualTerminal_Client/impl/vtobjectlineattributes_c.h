/*
  vtobjectlineattributes_c.h

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
#ifndef VTOBJECTLINEATTRIBUTES_C_H
#define VTOBJECTLINEATTRIBUTES_C_H

#include <IsoAgLib/isoaglib_config.h>

#ifdef CONFIG_USE_VTOBJECT_lineattributes

#include <memory> // PImpl

#include "vtobject_c.h"
#include "vtclient_c.h"
#include "vtclientconnection_c.h"


namespace __IsoAgLib {

class vtObjectLineAttributes_c : public vtObject_c
{
private:
	enum AttributeID:uint8_t;
	// Internal implementation class
	struct iVtObjectLineAttributes_s;

	// Pointer to the internal implementation
	std::unique_ptr<iVtObjectLineAttributes_s> vtObject_a;



public:
  int16_t stream(uint8_t* destMemory,
                 uint16_t maxBytes,
                 objRange_t sourceOffset);
  IsoAgLib::ObjectID getID() const;


  vtObjectLineAttributes_c(
		  int ai_multitonInst,
		  IsoAgLib::ObjectID ID,
		  IsoAgLib::Colour lineColour,
  		  uint8_t lineWidth,
  		  uint16_t lineArt);

  vtObjectLineAttributes_c(iVtObjectLineAttributes_s* vtObjectLineAttributesSROM , int ai_multitonInst);
  iVtObjectLineAttributes_s* get_vtObjectLineAttributes_a();



  uint32_t fitTerminal() const;

  // //////////////////////////////////
  // All special Attribute-Set methods
    virtual void setLineColour(IsoAgLib::Colour newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
    virtual void setLineWidth(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
    virtual void setLineArt(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
    virtual void setLineAttributes(IsoAgLib::Colour newLineColour, uint8_t newLineWidth, uint16_t newLineArt, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);

#ifdef CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return 24; }
  */

  virtual uint8_t updateLineColour(bool b_SendRequest=false);
  virtual uint8_t updateLineWidth(bool b_SendRequest=false);
  virtual uint16_t updateLineArt(bool b_SendRequest=false);

  void saveReceivedAttribute (uint8_t attrID, uint8_t* pui8_attributeValue);
#endif
};

} // __IsoAgLib

#endif // CONFIG_USE_VTOBJECT_lineattributes

#endif //VTOBJECTLINEATTRIBUTES_C_H
