/*
  vtobjectpolygon_c.h

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
#ifndef VTOBJECTPOLYGON_C_H
#define VTOBJECTPOLYGON_C_H

#include <IsoAgLib/isoaglib_config.h>


#ifdef CONFIG_USE_VTOBJECT_polygon

#include "vtobject_c.h"


namespace __IsoAgLib {

class vtObjectPolygon_c : public vtObject_c
{
private:
	// Internal implementation class
	struct iVtObjectPolygon_s;

	// Pointer to the internal implementation
	std::unique_ptr<iVtObjectPolygon_s> vtObject_a;

public:
  int16_t stream(uint8_t* destMemory,
                 uint16_t maxBytes,
                 objRange_t sourceOffset);
  IsoAgLib::ObjectID getID() const;

  vtObjectPolygon_c(iVtObjectPolygon_s* vtObjectPolygonSROM , int ai_multitonInst);
  iVtObjectPolygon_s* get_vtObjectPolygon_a();
  vtObjectPolygon_c();
  uint32_t fitTerminal() const;
  void setWidth(uint16_t newWidth, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
  void setHeight(uint16_t newHeight, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
  void setLineAttributes(IsoAgLib::iVtObjectLineAttributes_c* newLineAttributes, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
  void setFillAttributes(IsoAgLib::iVtObjectFillAttributes_c* newFillAttributes, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
  void setPolygonType(uint8_t newPolygonType, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
  void setOriginSKM(bool b_SKM);
  void setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
#ifdef CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES
  // ///////////////////////// getter for attributes
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return 16; }
  */

  uint16_t updateWidth(bool b_SendRequest=false);
  uint16_t updateHeight(bool b_SendRequest=false);
  uint16_t updateLineAttributes(bool b_SendRequest=false);
  uint16_t updateFillAttributes(bool b_SendRequest=false);
  uint8_t updatePolygonType(bool b_SendRequest=false);
  void saveReceivedAttribute (uint8_t attrID, uint8_t* pui8_attributeValue);
#endif
};

} // __IsoAgLibb

#endif // CONFIG_USE_VTOBJECT_polygon

#endif // VTOBJECTPOLYGON_C_H
