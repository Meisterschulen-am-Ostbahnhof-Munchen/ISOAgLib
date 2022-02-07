/*
  vtobjectauxiliaryfunction_c.h

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
#ifndef VTOBJECTAUXILIARYFUNCTION_C_H
#define VTOBJECTAUXILIARYFUNCTION_C_H

#include <IsoAgLib/isoaglib_config.h>




#include "vtobject_c.h"
#include "vttypes.h"

namespace __IsoAgLib {

class vtObjectAuxiliaryFunction_c : public vtObject_c
{
private:
	// Internal implementation class
	struct iVtObjectAuxiliaryFunction_s;

	// Pointer to the internal implementation
	iVtObjectAuxiliaryFunction_s* vtObject_a;
	//TODO
	//std::unique_ptr<iVtObjectAuxiliaryFunction_s> vtObject_a;

public:
  int16_t stream(uint8_t* destMemory,
                 uint16_t maxBytes,
                 objRange_t sourceOffset);
  IsoAgLib::ObjectID getID() const;

  vtObjectAuxiliaryFunction_c(iVtObjectAuxiliaryFunction_s* vtObjectAuxiliaryFunctionSROM , int ai_multitonInst);

  iVtObjectAuxiliaryFunction_s* get_vtObjectAuxiliaryFunction_a();

  ~vtObjectAuxiliaryFunction_c() = default;

  uint32_t fitTerminal() const;

  void setOriginSKM(bool b_SKM);

  bool moveChildLocation(IsoAgLib::iVtObject_c* apc_childObject, int8_t dx, int8_t dy, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
  bool setChildPosition(IsoAgLib::iVtObject_c* apc_childObject, int16_t dx, int16_t dy, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);

#ifdef USE_ISO_TERMINAL_GETATTRIBUTES
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return 29; }
  */

  void saveReceivedAttribute (uint8_t /*attrID*/, uint8_t* /*pui8_attributeValue*/);;
#endif
};

} // __IsoAgLib

#endif


