/*
  vtobjectstringvariable_c.h

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
#ifndef VTOBJECTSTRINGVARIABLE_C_H
#define VTOBJECTSTRINGVARIABLE_C_H

#include <IsoAgLib/isoaglib_config.h>
#include "../ivtobjectstring_c.h"



#include "vtclientconnection_c.h"


namespace __IsoAgLib {

class vtObjectStringVariable_c : public IsoAgLib::iVtObjectString_c
{
private:
	// Internal implementation class
	struct iVtObjectStringVariable_s;

	// Pointer to the internal implementation
	iVtObjectStringVariable_s* vtObject_a;
	//TODO
	//std::unique_ptr<iVtObjectOutputString_s> vtObject_a;

public:
  int16_t stream(uint8_t* destMemory,
                 uint16_t maxBytes,
                 objRange_t sourceOffset);

  vtObjectStringVariable_c(
		  int ai_multitonInst,
		  IsoAgLib::ObjectID ID,
		  char *value);

  vtObjectStringVariable_c(iVtObjectStringVariable_s* vtObjectStringVariableSROM , int ai_multitonInst);
  iVtObjectStringVariable_s* get_vtObjectStringVariable_a();
  uint32_t fitTerminal() const;

    virtual void setValueCopy    (const char* newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
#ifdef USE_VT_UNICODE_SUPPORT

        virtual void setValueCopyUTF8(const char* newValue, uint8_t aui8_fontType, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);

        virtual void setValueCopyUTF16 (const char* newValue, uint16_t length, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
#endif

        virtual void setValueRef(const char* newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=false);

  const char* getString();
#ifdef USE_ISO_TERMINAL_GETATTRIBUTES
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return 22; }
  */

  void saveReceivedAttribute (uint8_t /*attrID*/, uint8_t* /*pui8_attributeValue*/);;
#endif
};

} // __IsoAgLib

#endif

