/***************************************************************************
                          ivtobjectobjectpointer_c.h
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

#ifndef IVTOBJECTOBJECTPOINTER_C_H
#define IVTOBJECTOBJECTPOINTER_C_H


// +X2C includes
#include "impl/vtobjectobjectpointer_c.h"
// ~X2C

// Begin Namespace IsoAgLib
namespace IsoAgLib {


//  +X2C Class 179 : iVtObjectObjectPointer_c
//!  Stereotype: Klasse
class iVtObjectObjectPointer_c : public __IsoAgLib::vtObjectObjectPointer_c
{

public:

  //  Operation: init
  //! Parameter:
  //! @param vtObjectObjectPointerSROM:
  //! @param b_initPointer:
  void init(const iVtObjectObjectPointer_s* vtObjectObjectPointerSROM SINGLETON_VEC_KEY_PARAMETER_DEF_WITH_COMMA) {
    vtObjectObjectPointer_c::init (vtObjectObjectPointerSROM SINGLETON_VEC_KEY_PARAMETER_VAR_WITH_COMMA);
  };

  //  Operation: get_vtObjectObjectPointer_a
  iVtObjectObjectPointer_s* get_vtObjectObjectPointer_a() { return vtObjectObjectPointer_c::get_vtObjectObjectPointer_a(); };

  //  Operation: setValue
  //! Parameter:
  //! @param rpc_newObject:
  //! @param b_updateObject:
  void setValue(iVtObject_c* rpc_newObject, bool b_updateObject= false, bool b_enableReplaceOfCmd=false) {
    vtObjectObjectPointer_c::setValue (rpc_newObject, b_updateObject, b_enableReplaceOfCmd);
  };

}; // ~X2C

} // end of namespace IsoAgLib

#endif // -X2C
