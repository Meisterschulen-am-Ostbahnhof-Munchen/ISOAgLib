/***************************************************************************
                          canpkg_c.cpp - source for CANPkg_c, the
                                        encapsulation of CAN telegrams
                             -------------------
    begin                : Sun Aug 1 1999
    copyright            : (C) 1999 - 2004 by Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
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
 * Copyright (C) 1999 - 2004 Dipl.-Inform. Achim Spangler                  *
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
#include "canpkg_c.h"

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

/** max 8 data bytes defined as union */
Flexible8ByteString_c CANPkg_c::c_data;

/** receive time of CAN message */
int32_t CANPkg_c::i32_time = 0;

/** identifier of CAN msg */
__IsoAgLib::Ident_c CANPkg_c::c_ident;

/** state of the received can-message */
MessageState_t CANPkg_c::t_msgState = MessageValid; // no default value is needed, but anyway..

/** size of data */
uint8_t CANPkg_c::ui8_len = 0;

/**
  default constructor, which does nothing for the base class,
  but can do something in derived classes
*/
CANPkg_c::CANPkg_c( int ri_singletonVecKey )
  : ClientBase( ri_singletonVecKey )
 {
}

/** virtual destructor, which can be overloaded in derived classes */
CANPkg_c::~CANPkg_c(){
}

/**
  ==> OBSOLETE, because now all pkg-data is STATIC!
  ==> REACTIVATE if some NON-STATIC member vars will be added!
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  assign operator to insert informations from one CANPkg_c into another
  @see __IsoAgLib::FilterBox_c::operator>>
  @see CANPkgExt_c::operator=
  @see CANPkgExt_c::getData
  @param rrefc_right reference to the source CANPkg_c on the right
  @return reference to the source CANPkg_c to enable assign chains like "pkg1 = pkg2 = pkg3 = pkg4;"
const CANPkg_c& CANPkg_c::operator=(const CANPkg_c& rrefc_right)
{
  c_ident = rrefc_right.c_ident;
  ui8_len = rrefc_right.ui8_len;
  c_data = rrefc_right.c_data;
  i32_time = rrefc_right.i32_time;

  return *this;
}
*/

/**
  put data into given reference to BIOS related data structure with data, len
  @param reft_ident     reference where the ident is placed for send
  @param refui8_identType reference to the ident type val: 0==std, 1==ext
  @param refb_dlcTarget reference to the DLC field of the target
  @param pb_dataTarget pointer to the data string of the target
*/
void CANPkg_c::getData(uint32_t& reft_ident, uint8_t& refui8_identType,
                     uint8_t& refb_dlcTarget, uint8_t* pb_dataTarget)
{
  reft_ident = ident();
  refui8_identType = identType();
  refb_dlcTarget = ui8_len;
  c_data.getDataToString( pb_dataTarget, ui8_len );
}

/**
  set complete CAN msg with one function call
  (if rui8_len is greater than 8, than max 8 is used)
  @param rt_ident ident of the CAN telegram
  @param rpb_data pointer to the source data uint8_t string
  @param rui8_len amount of bytes in the data string
  @param ri32_time optional timestamp of CAN telegram in [msec.] since system start
*/
void CANPkg_c::set(MASK_TYPE rt_ident, const uint8_t* rpb_data, uint8_t rui8_len, int32_t ri32_time, Ident_c::identType_t rt_type)
{
  c_ident.set(rt_ident, rt_type);
  ui8_len = (rui8_len<9)?rui8_len:8;
  c_data.setDataFromString( rpb_data, ui8_len );
  i32_time = ri32_time;
}

/**
  set data with size bytes from source array;
  if rui8_len is greater than 8 (max data length of CAN) than max 8 bytes are transfered
  @param rb_data pointer to source data uint8_t array
  @param rui8_len amount/len of the source data string
*/
void CANPkg_c::setDataFromString(const uint8_t* rpb_data, uint8_t rui8_len)
{
  if ( rui8_len > 0 )
  { // there is something to set - this function might be called from some generic algorithms which rely
    // on the underlying functions to handle zero-lenght properly
    ui8_len = (rui8_len<9)?rui8_len:8;
    c_data.setDataFromString( rpb_data, ui8_len);
  }
}

/**
  set data with size bytes from source array;
  if rui8_len is greater than 8 (max data length of CAN) than max 8 bytes are transfered.
  this variant of the setDataFromString allows to set the CAN data from specified offset position onwards.
  @param rui8_targetPositionOffset
  @param rb_data pointer to source data uint8_t array
  @param rui8_len amount/len of the source data string
*/
void CANPkg_c::setDataFromString(uint8_t rui8_targetPositionOffset, const uint8_t* rpb_data, uint8_t rui8_len)
{
  if ( rui8_len > 0 )
  { // there is something to set - this function might be called from some generic algorithms which rely
    // on the underlying functions to handle zero-lenght properly
    const unsigned int cui_copyByteCnt = (rui8_len+rui8_targetPositionOffset <= 8)?rui8_len:(8-rui8_targetPositionOffset);
    ui8_len = rui8_targetPositionOffset + cui_copyByteCnt;
    c_data.setDataFromString( rui8_targetPositionOffset, rpb_data, cui_copyByteCnt);
  }
}

/**
  ==> OBSOLETE, because now all pkg-data is STATIC!
  ==> REACTIVATE if some NON-STATIC member vars will be added!

  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  compare for equality with other CANPkg
  @param rrefc_cmp reference to the to be compared CANPkg
  @return true -> both CANPkg_c have the same data
bool CANPkg_c::operator==(const CANPkg_c& rrefc_cmp) const
{
  bool b_result = true;
  if (c_data != rrefc_cmp.c_data) b_result = false;
  if (c_ident != rrefc_cmp.c_ident) b_result = false;
  if (ui8_len != rrefc_cmp.ui8_len) b_result = false;
  if (i32_time != rrefc_cmp.i32_time) b_result = false;
  return b_result;
}
*/

/**
  ==> OBSOLETE, because now all pkg-data is STATIC!
  ==> REACTIVATE if some NON-STATIC member vars will be added!
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  compare for difference to other CANPkg
  @param rrefc_cmp reference to the to be compared CANPkg
  @return true -> both CANPkg_c have different data
bool CANPkg_c::operator!=(const CANPkg_c& rrefc_cmp) const
{
  ANYWAY I THINK THIS LOGIC IS WRONG!!!!!!!!!!!!!!!!!!!!!!!!

  bool b_result = true;

  if (c_data == rrefc_cmp.c_data) b_result = false;
  if (c_ident == rrefc_cmp.c_ident) b_result = false;
  if (ui8_len == rrefc_cmp.ui8_len) b_result = false;
  if (i32_time == rrefc_cmp.i32_time) b_result = false;

  return b_result;
  return false;
}
*/

} // end of namespace __IsoAgLib
