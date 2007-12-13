/***************************************************************************
                         ivtobjectstringvariable_c.h
                             -------------------
    begin                : Mon Jun 30 14:41:54 2003
    copyright            : (C) 2003 by Martin Wodok
    email                : m.wodok@osb-ag.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 * This file is part of the "IsoAgLib", an object oriented program library *
 * to serve as a software layer between application specific program and   *
 * communication protocol details. By providing simple function calls for  *
 * jobs like starting a measuring program for a process data value on a    *
 * remote ECU, the main program has not to deal with single CAN telegram   *
 * formatting. This way communication problems between ECU's which use     *
 * this library should be prevented.                                       *
 * Everybody and every company is invited to use this library to make a    *
 * working plug and play standard out of the printed protocol standard.    *
 *                                                                         *
 * Copyright (C) 2000 - 2004 Dipl.-Inform. Achim Spangler                  *
 *                                                                         *
 * The IsoAgLib is free software; you can redistribute it and/or modify it *
 * under the terms of the GNU General Public License as published          *
 * by the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This library is distributed in the hope that it will be useful, but     *
 * WITHOUT ANY WARRANTY; without even the implied warranty of              *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       *
 * General Public License for more details.                                *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with IsoAgLib; if not, write to the Free Software Foundation,     *
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA           *
 *                                                                         *
 * As a special exception, if other files instantiate templates or use     *
 * macros or inline functions from this file, or you compile this file and *
 * link it with other works to produce a work based on this file, this file*
 * does not by itself cause the resulting work to be covered by the GNU    *
 * General Public License. However the source code for this file must still*
 * be made available in accordance with section (3) of the                 *
 * GNU General Public License.                                             *
 *                                                                         *
 * This exception does not invalidate any other reasons why a work based on*
 * this file might be covered by the GNU General Public License.           *
 *                                                                         *
 * Alternative licenses for IsoAgLib may be arranged by contacting         *
 * the main author Achim Spangler by a.spangler@osb-ag.de                  *
 ***************************************************************************/
#ifndef IVTOBJECTSTRINGVARIABLE_C_H
#define IVTOBJECTSTRINGVARIABLE_C_H

#include "impl/vtobjectstringvariable_c.h"

#if not defined PRJ_ISO_TERMINAL_OBJECT_SELECTION1 || defined USE_VTOBJECT_stringvariable || defined USE_VTOBJECT_outputstring
#include "ivtobjectoutputstring_c.h"

#ifdef USE_VT_UNICODE_SUPPORT
#include "ivtobjectfontattributes_c.h"
#endif
// Begin Namespace IsoAgLib
namespace IsoAgLib {

class iVtObjectStringVariable_c : public __IsoAgLib::vtObjectStringVariable_c
{
public:
  iVtObjectStringVariable_c();
  //  Operation: init
  //! @param vtObjectStringVariableSROM:
  //! @param b_initPointer:
  void init(const iVtObjectStringVariable_s* vtObjectStringVariableSROM SINGLETON_VEC_KEY_PARAMETER_DEF_WITH_COMMA) {
    vtObjectStringVariable_c::init (vtObjectStringVariableSROM SINGLETON_VEC_KEY_PARAMETER_USE_WITH_COMMA);
  }

  //  Operation: get_vtObjectStringVariable_a
  const iVtObjectStringVariable_s& get_vtObjectStringVariable_a() {
    return *vtObjectStringVariable_c::get_vtObjectStringVariable_a ();
  }

  //  Operation: setValueCopy
  //! @param newValue:
  //! @param b_updateObject:
  void setValueCopy(char* newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectStringVariable_c::setValueCopy (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

#ifdef USE_VT_UNICODE_SUPPORT
  //  Operation: setValueCopyUTF8
  //! @param newValue:
  //! @param b_updateObject:
  void setValueCopyUTF8(const char* newValue, uint8_t aui8_fontType, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectStringVariable_c::setValueCopyUTF8 (newValue, aui8_fontType, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setValueCopyUTF8
  //! @param newValue:
  //! @param b_updateObject:
  void setValueCopyUTF8(const char* newValue, iVtObjectOutputString_c& arc_ops, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    const uint8_t cui8_fontType = arc_ops.get_vtObjectOutputString_a().fontAttributes->get_vtObjectFontAttributes_a().fontType;
    vtObjectStringVariable_c::setValueCopyUTF8 (newValue, cui8_fontType, b_updateObject, b_enableReplaceOfCmd);
  }
#endif

  //  Operation: setValueRef
  //! @param newValue:
  //! @param b_updateObject:
  void setValueRef(const char* newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectStringVariable_c::setValueRef (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: getString
  const char* getString() { return vtObjectStringVariable_c::getString(); }

  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return vtObjectStringVariable_c::updateObjectType(); }
  */
};

} // end of namespace IsoAgLib

#endif
#endif
