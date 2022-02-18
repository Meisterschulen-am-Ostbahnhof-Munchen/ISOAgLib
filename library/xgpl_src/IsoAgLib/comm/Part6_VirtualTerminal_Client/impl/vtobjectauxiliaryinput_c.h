/*
  vtobjectauxiliaryinput_c.h

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
#ifndef VTOBJECTAUXILIARYINPUT_C_H
#define VTOBJECTAUXILIARYINPUT_C_H

#include <IsoAgLib/isoaglib_config.h>


#ifdef CONFIG_USE_VTOBJECT_auxiliaryinput

#include <memory> // PImpl
#include "vtobject_c.h"
#include "vttypes.h"

namespace __IsoAgLib {

class vtObjectAuxiliaryInput_c : public vtObject_c
{
private:
	// Internal implementation class
	struct iVtObjectAuxiliaryInput_s;

	// Pointer to the internal implementation
	std::unique_ptr<iVtObjectAuxiliaryInput_s> vtObject_a;

public:
  int16_t stream(uint8_t* destMemory,
                 uint16_t maxBytes,
                 objRange_t sourceOffset);
  IsoAgLib::ObjectID getID() const;

  vtObjectAuxiliaryInput_c(iVtObjectAuxiliaryInput_s* vtObjectAuxiliaryInputSROM , int ai_multitonInst);



  uint32_t fitTerminal() const;

  void setOriginSKM(bool b_SKM);

  bool moveChildLocation(IsoAgLib::iVtObject_c* apc_childObject, int8_t dx,  int8_t dy,  bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
  bool setChildPosition( IsoAgLib::iVtObject_c* apc_childObject, int16_t dx, int16_t dy, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);

#ifdef CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return 30; }
  */

  void saveReceivedAttribute (uint8_t /*attrID*/, uint8_t* /*pui8_attributeValue*/);
#endif
};

} // __IsoAgLib

#endif //CONFIG_USE_VTOBJECT_auxiliaryinput

#endif //VTOBJECTAUXILIARYINPUT_C_H
