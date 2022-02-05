/*
  ivtobjectstringvariable_c.h

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
#ifndef IVTOBJECTSTRINGVARIABLE_C_H
#define IVTOBJECTSTRINGVARIABLE_C_H

#include "impl/vtobjectstringvariable_c.h"

#include "ivtobjectoutputstring_c.h"

#ifdef USE_VT_UNICODE_SUPPORT
#include "ivtobjectfontattributes_c.h"
#endif


namespace IsoAgLib {

class iVtObjectStringVariable_c : public __IsoAgLib::vtObjectStringVariable_c
{
public:
  static uint16_t objectType();

  void setValueCopy(const char* newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
#ifdef USE_VT_UNICODE_SUPPORT
  void setValueCopyUTF8(const char* newValue, uint8_t aui8_fontType, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
  void setValueCopyUTF8(const char* newValue, iVtObjectOutputString_c& arc_ops, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
  void setValueCopyUTF16(const char* newValue, uint16_t length, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
#endif

  void setValueRef(const char* newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
  const char* getString();
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return vtObjectStringVariable_c::updateObjectType(); }
  */
  virtual uint16_t getObjectType() const;
};

} // IsoAgLib

#endif


