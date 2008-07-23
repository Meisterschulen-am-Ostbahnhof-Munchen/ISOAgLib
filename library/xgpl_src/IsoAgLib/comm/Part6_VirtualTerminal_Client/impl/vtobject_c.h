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
#include  <IsoAgLib/comm/Part5_NetworkManagement/impl/isoname_c.h>
// ~X2C

#ifdef DEBUG
  #include <supplementary_driver/driver/rs232/impl/rs232io_c.h>
#endif


// Begin Namespace __IsoAgLib
namespace __IsoAgLib {
//  +X2C Class 2 : vtObject_c
//!  Stereotype: Klasse
class vtObject_c : public IsoAgLib::iVtObject_c
{
public:
  vtObject_c();
  virtual ~vtObject_c();
  //  Operation: stream
  //! Parameter:
  //! @param destMemory:
  //! @param maxBytes: don't stream out more than that or you'll overrun the internal upload-buffer
  //! @param sourceOffset:
  virtual int16_t stream(uint8_t* destMemory,
                         uint16_t maxBytes,
                         objRange_t sourceOffset) = 0;

  //! Give size of VT object including header and attributes.
  virtual uint32_t fitTerminal() const = 0;

  /** latest version (v3) of VT spec offers to also get the type of an vtObject_c; to realize it anytime this function was introduced
      here and in all other derived vtObjectXYZ_c classes
  virtual uint8_t updateObjectType() const = 0;
  */

#ifdef USE_ISO_TERMINAL_GETATTRIBUTES
  virtual void saveReceivedAttribute (uint8_t attrID, uint8_t* pui8_attributeValue) = 0;
#endif

protected:
  /** @todo ON REQUEST: check for double initialization via flags & STRUCT_IN_RAM etc.
           - double init call should normally not happen
    */
  void init (iVtObject_s* aps_vtObject_a SINGLETON_VEC_KEY_PARAMETER_DEF_WITH_COMMA) { vtObject_a = aps_vtObject_a; SINGLETON_VEC_KEY_INIT_CALL }

  //  Operation: get_vtObject_a
  iVtObject_s& get_vtObject_a()
  {
    #ifdef DEBUG
    if (vtObject_a == NULL)
    {
      INTERNAL_DEBUG_DEVICE << "vtObject(s) not initialized properly. Do not used vtObjects before having called initAndRegisterIsoObjectPool(...)." << INTERNAL_DEBUG_DEVICE_ENDL;
      #ifdef SYSTEM_PC
      MACRO_ISOAGLIB_ABORT();
      #endif
    }
    #endif
    return *vtObject_a;
  }

  // //////////////////////////////// +X2C Operation 783 : setAttribute
  //! Parameter:
  //! @param attrID: Attribute ID of the object's attribute
  //! @param newValue: New Value of the attribute
  void setAttribute      (uint8_t attrID, uint32_t newValue, bool b_enableReplaceOfCmd=true);
  void setAttributeFloat (uint8_t attrID, float newValue, bool b_enableReplaceOfCmd=true);

  #ifdef USE_ISO_TERMINAL_GETATTRIBUTES
  void getAttribute      (uint8_t attrID, bool b_enableReplaceOfCmd=true);
  #endif


  void createRamStructIfNotYet (uint16_t ui16_structLen);

  void saveValueBool  (const uint16_t ui16_structOffset, const uint16_t ui16_structLen, bool b_newValue);
  void saveValue8     (uint16_t ui16_structOffset, uint16_t ui16_structLen, uint8_t ui8_newValue);
  void saveValue16    (uint16_t ui16_structOffset, uint16_t ui16_structLen, uint16_t ui16_newValue);
  void saveValue32    (uint16_t ui16_structOffset, uint16_t ui16_structLen, uint32_t ui32_newValue);
  void saveSignedValue8  (uint16_t ui16_structOffset, uint16_t ui16_structLen, int8_t i8_newValue);
  void saveSignedValue16 (uint16_t ui16_structOffset, uint16_t ui16_structLen, int16_t i16_newValue);
  void saveSignedValue32 (uint16_t ui16_structOffset, uint16_t ui16_structLen, int32_t i32_newValue);
  void saveValueFloat (uint16_t ui16_structOffset, uint16_t ui16_structLen, float f_newValue);
  void saveValueP     (uint16_t ui16_structOffset, uint16_t ui16_structLen, const IsoAgLib::iVtObject_c* const p_newValue);
  void saveValueISOName (uint16_t ui16_structOffset, uint16_t ui16_structLen, const IsoAgLib::iIsoName_c& ar_newIsoName);
  void saveValueBoolSetAttribute (uint16_t ui16_structOffset, uint16_t ui16_structLen, uint8_t ui8_ind, uint8_t ui8_newValue, bool b_enableReplaceOfCmd);
  void saveValue8SetAttribute     (uint16_t ui16_structOffset, uint16_t ui16_structLen, uint8_t ui8_ind, uint8_t ui8_newValue, uint8_t newValueSend, bool b_enableReplaceOfCmd);
  void saveValue16SetAttribute    (uint16_t ui16_structOffset, uint16_t ui16_structLen, uint8_t ui8_ind, uint16_t ui16_newValue, bool b_enableReplaceOfCmd);
  void saveValue32SetAttribute    (uint16_t ui16_structOffset, uint16_t ui16_structLen, uint8_t ui8_ind, uint32_t ui32_newValue, bool b_enableReplaceOfCmd);
  void saveValueFloatSetAttribute (uint16_t ui16_structOffset, uint16_t ui16_structLen, uint8_t ui8_ind, float f_newValue, bool b_enableReplaceOfCmd);
  void saveValuePSetAttribute     (uint16_t ui16_structOffset, uint16_t ui16_structLen, uint8_t ui8_ind, IsoAgLib::iVtObject_c* p_newValue, bool b_enableReplaceOfCmd);

  uint8_t getValue8   (uint16_t ui16_structOffset, uint16_t ui16_structLen, bool b_createRamStructIfNeeded=false);
  uint16_t getValue16 (uint16_t ui16_structOffset, uint16_t ui16_structLen, bool b_createRamStructIfNeeded=false);
  uint32_t getValue32 (uint16_t ui16_structOffset, uint16_t ui16_structLen, bool b_createRamStructIfNeeded=false);
  int8_t getSignedValue8   (uint16_t ui16_structOffset, uint16_t ui16_structLen, bool b_createRamStructIfNeeded=false);
  int16_t getSignedValue16 (uint16_t ui16_structOffset, uint16_t ui16_structLen, bool b_createRamStructIfNeeded=false);
  int32_t getSignedValue32 (uint16_t ui16_structOffset, uint16_t ui16_structLen, bool b_createRamStructIfNeeded=false);
  float getValueFloat (uint16_t ui16_structOffset, uint16_t ui16_structLen, bool b_createRamStructIfNeeded=false);
  #ifdef USE_ISO_TERMINAL_GETATTRIBUTES
  uint8_t getValue8GetAttribute  (uint16_t ui16_structOffset, uint16_t ui16_structLen, uint8_t ui8_ind, bool b_createRamStructIfNeeded=false);
  uint16_t getValue16GetAttribute (uint16_t ui16_structOffset, uint16_t ui16_structLen, uint8_t ui8_ind, bool b_createRamStructIfNeeded=false);
  uint32_t getValue32GetAttribute (uint16_t ui16_structOffset, uint16_t ui16_structLen, uint8_t ui8_ind, bool b_createRamStructIfNeeded=false);
  int8_t getSignedValue8GetAttribute  (uint16_t ui16_structOffset, uint16_t ui16_structLen, uint8_t ui8_ind, bool b_createRamStructIfNeeded=false);
  int16_t getSignedValue16GetAttribute (uint16_t ui16_structOffset, uint16_t ui16_structLen, uint8_t ui8_ind, bool b_createRamStructIfNeeded=false);
  int32_t getSignedValue32GetAttribute (uint16_t ui16_structOffset, uint16_t ui16_structLen, uint8_t ui8_ind, bool b_createRamStructIfNeeded=false);
  float getValueFloatGetAttribute (uint16_t ui16_structOffset, uint16_t ui16_structLen, uint8_t ui8_ind, bool b_enableReplaceOfCmd=false);
  #endif


  bool genericChangeChildLocation (IsoAgLib::iVtObject_c* childObject, int16_t dx, int16_t dy, bool b_updateObject, uint8_t numObjectsToFollow, IsoAgLib::repeat_iVtObject_x_y_iVtObjectFontAttributes_row_col_s* objectsToFollow, uint16_t ui16_structOffset, uint16_t ui16_structLen,bool b_enableReplaceOfCmd);
  bool genericChangeChildPosition (IsoAgLib::iVtObject_c* childObject, int16_t dx, int16_t dy, bool b_updateObject, uint8_t numObjectsToFollow, IsoAgLib::repeat_iVtObject_x_y_iVtObjectFontAttributes_row_col_s* objectsToFollow, uint16_t ui16_structOffset, uint16_t ui16_structLen, bool b_enableReplaceOfCmd);

  //  Operation: able
  //! ATTENTION: Should only be used by Input-Objects of course!
  //! Parameter:
  //! @param b_updateObject:
  bool able (uint8_t enOrDis, bool b_updateObject= false, bool b_enableReplaceOfCmd=true);

  //  Operation: select
  bool select(uint8_t selectOrActivate);

private:
  friend class SINGLETON( iVtObject_c );
  friend class SINGLETON( vtObject_c );
  friend class vtLayoutManager_c;
  bool genericChangeChildLocationPosition (bool ab_isLocation, IsoAgLib::iVtObject_c* childObject, int16_t dx, int16_t dy, bool b_updateObject, uint8_t numObjectsToFollow, IsoAgLib::repeat_iVtObject_x_y_iVtObjectFontAttributes_row_col_s* objectsToFollow, uint16_t ui16_structOffset, uint16_t ui16_structLen);

  //  Operation: updateEnable
  virtual void updateEnable(uint8_t /*aui8_enOrDis*/) {}

}; // ~X2C

} // end of namespace __IsoAgLib

#endif // -X2C
