/***************************************************************************
                          vtobject_c.h
                             -------------------
    begin                : Don Sep 4 2003
    copyright            : (C) 2003 by Martin Wodok
    email                : m.wodok@osb-ag:de
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
 * the main author Achim Spangler by a.spangler@osb-ag:de                  *
 ***************************************************************************/

 /**************************************************************************
 *                                                                         *
 *     ###    !!!    ---    ===    IMPORTANT    ===    ---    !!!    ###   *
 * Each software module, which accesses directly elements of this file,    *
 * is considered to be an extension of IsoAgLib and is thus covered by the *
 * GPL license. Applications must use only the interface definition out-   *
 * side :impl: subdirectories. Never access direct elements of __IsoAgLib  *
 * and __HAL namespaces from applications which shouldnt be affected by    *
 * the license. Only access their interface counterparts in the IsoAgLib   *
 * and HAL namespaces. Contact a.spangler@osb-ag:de in case your applicat- *
 * ion really needs access to a part of an internal namespace, so that the *
 * interface might be extended if your request is accepted.                *
 *                                                                         *
 * Definition of direct access:                                            *
 * - Instantiation of a variable with a datatype from internal namespace   *
 * - Call of a (member-) function                                          *
 * Allowed is:                                                             *
 * - Instatiation of a variable with a datatype from interface namespace,  *
 *   even if this is derived from a base class inside an internal namespace*
 * - Call of member functions which are defined in the interface class     *
 *   definition ( header )                                                 *
 *                                                                         *
 * Pairing of internal and interface classes:                              *
 * - Internal implementation in an :impl: subdirectory                     *
 * - Interface in the parent directory of the corresponding internal class *
 * - Interface class name IsoAgLib::iFoo_c maps to the internal class      *
 *   __IsoAgLib::Foo_c                                                     *
 *                                                                         *
 * AS A RULE: Use only classes with names beginning with small letter :i:  *
 ***************************************************************************/

#ifndef VTOBJECT_C_H
#define VTOBJECT_C_H


// +X2C includes
#include "../ivtobject_c.h"
#include "vttypes.h"
// ~X2C

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {


//  +X2C Class 2 : vtObject_c
//!  Stereotype: Klasse
class vtObject_c : public IsoAgLib::iVtObject_c
{

public:
  virtual ~vtObject_c(){};
  //  Operation: stream
  //! Parameter:
  //! @param destMemory:
  //! @param maxBytes: don't stream out more than that or you'll overrun the internal upload-buffer
  //! @param sourceOffset:
  virtual int16_t stream(uint8_t* destMemory,
                         uint16_t maxBytes,
                         uint16_t sourceOffset) = 0;

  //  Operation: fitTerminal
  virtual uint32_t fitTerminal() = 0;

  //  Operation: select
  bool select();

protected:

  /** @todo check for double initialization via flags & STRUCT_IN_RAM etc. */
  void init (iVtObject_s* rps_vtObject_a) { vtObject_a = rps_vtObject_a; }

  // //////////////////////////////// +X2C Operation 783 : setAttribute
  //! Parameter:
  //! @param attrID: Attribute ID of the object's attribute
  //! @param newValue: New Value of the attribute
  void setAttribute      (uint8_t attrID, uint32_t newValue, bool b_enableReplaceOfCmd=true);
  void setAttributeFloat (uint8_t attrID, float newValue, bool b_enableReplaceOfCmd=true);

  void createRamStructIfNotYet (uint16_t ui16_structLen);

  void saveValue8     (uint16_t ui16_structOffset, uint16_t ui16_structLen, uint8_t ui8_newValue);
  void saveValue16    (uint16_t ui16_structOffset, uint16_t ui16_structLen, uint16_t ui16_newValue);
  void saveValue32    (uint16_t ui16_structOffset, uint16_t ui16_structLen, uint32_t ui32_newValue);
  void saveValueFloat (uint16_t ui16_structOffset, uint16_t ui16_structLen, float f_newValue);
  void saveValueP     (uint16_t ui16_structOffset, uint16_t ui16_structLen, IsoAgLib::iVtObject_c* p_newValue);
  void saveValue8SetAttribute     (uint16_t ui16_structOffset, uint16_t ui16_structLen, uint8_t ui8_ind, uint8_t ui8_newValue, uint8_t newValueSend, bool b_enableReplaceOfCmd=true);
  void saveValue16SetAttribute    (uint16_t ui16_structOffset, uint16_t ui16_structLen, uint8_t ui8_ind, uint16_t ui16_newValue, bool b_enableReplaceOfCmd=true);
  void saveValue32SetAttribute    (uint16_t ui16_structOffset, uint16_t ui16_structLen, uint8_t ui8_ind, uint32_t ui32_newValue, bool b_enableReplaceOfCmd=true);
  void saveValueFloatSetAttribute (uint16_t ui16_structOffset, uint16_t ui16_structLen, uint8_t ui8_ind, float f_newValue, bool b_enableReplaceOfCmd=true);
  void saveValuePSetAttribute     (uint16_t ui16_structOffset, uint16_t ui16_structLen, uint8_t ui8_ind, IsoAgLib::iVtObject_c* p_newValue, bool b_enableReplaceOfCmd=true);

  bool genericChangeChildLocation (IsoAgLib::iVtObject_c* childObject, int16_t dx, int16_t dy, bool b_updateObject, uint8_t numObjectsToFollow, IsoAgLib::repeat_iVtObject_x_y_iVtObjectFontAttributes_row_col_s* objectsToFollow, uint16_t ui16_structOffset, uint16_t ui16_structLen,bool b_enableReplaceOfCmd=true);
  bool genericChangeChildPosition (IsoAgLib::iVtObject_c* childObject, int16_t dx, int16_t dy, bool b_updateObject, uint8_t numObjectsToFollow, IsoAgLib::repeat_iVtObject_x_y_iVtObjectFontAttributes_row_col_s* objectsToFollow, uint16_t ui16_structOffset, uint16_t ui16_structLen, bool b_enableReplaceOfCmd=true);

  //  Operation: able
  //! Parameter:
  //! @param b_updateObject:
  bool able (uint8_t enOrDis, bool b_updateObject= false, bool b_enableReplaceOfCmd=true);

private:
  bool genericChangeChildLocationPosition (bool rb_isLocation, IsoAgLib::iVtObject_c* childObject, int16_t dx, int16_t dy, bool b_updateObject, uint8_t numObjectsToFollow, IsoAgLib::repeat_iVtObject_x_y_iVtObjectFontAttributes_row_col_s* objectsToFollow, uint16_t ui16_structOffset, uint16_t ui16_structLen);

    //  Operation: updateEnable
  //! Parameter:
  //! @param b_enableOrDisable:
  virtual void updateEnable(bool /*b_enableOrDisable*/) {};

}; // ~X2C

} // end of namespace __IsoAgLib

#endif // -X2C
