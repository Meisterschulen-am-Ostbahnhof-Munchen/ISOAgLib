/*
  vtobjectnumbervariable_c.h

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
#ifndef VTOBJECTNUMBERVARIABLE_C_H
#define VTOBJECTNUMBERVARIABLE_C_H

#include <IsoAgLib/isoaglib_config.h>


#ifdef USE_VTOBJECT_numbervariable

#include "vtobject_c.h"
#include "vtclientconnection_c.h"


namespace __IsoAgLib {

class vtObjectNumberVariable_c : public vtObject_c
{
private:
	// Internal implementation class
	struct iVtObjectNumberVariable_s;

	// Pointer to the internal implementation
	iVtObjectNumberVariable_s* vtObject_a;
	//TODO
	//std::unique_ptr<iVtObjectNumberVariable_s> vtObject_a;

public:
  int16_t stream(uint8_t* destMemory,
                 uint16_t maxBytes,
                 objRange_t sourceOffset);
  IsoAgLib::ObjectID getID() const;

  vtObjectNumberVariable_c(iVtObjectNumberVariable_s* vtObjectNumberVariableSROM , int ai_multitonInst);
  iVtObjectNumberVariable_s* get_vtObjectNumberVariable_a();
  vtObjectNumberVariable_c();
  uint32_t fitTerminal() const;
  void setValue(uint32_t newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=true);

#ifdef CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES
  // ///////////////////////// getter for attributes
  /** these attributes are in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return 21; }

  uint32_t updateValue(bool b_SendRequest=false);
  */

  void saveReceivedAttribute (uint8_t attrID, uint8_t* pui8_attributeValue);
#endif
};

} // __IsoAgLib

#endif //USE_VTOBJECT_numbervariable

#endif //VTOBJECTNUMBERVARIABLE_C_H
