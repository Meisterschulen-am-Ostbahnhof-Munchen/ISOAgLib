/***************************************************************************
                          iCANPkg_c.h - header for iCANPkg_c, the
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
#ifndef iCAN_Pkg_H
#define iCAN_Pkg_H


#include "impl/canpkg_c.h"

// Begin Namespace IsoAgLib
namespace IsoAgLib {


/** @todo Interface class for FlexibleByteString stuff... */

/**
  Storing the informations of one CAN
  message package. Implements some methods
  for input and output of informations into
  other objects and data structures.
  @author Dipl.-Inform. Achim Spangler
*/
class iCANPkg_c : private __IsoAgLib::CANPkg_c
{
public:

  /**
    set ident for the telegram
    @param rt_ident ident for the telegram
    @param rt_type type of iIdent_c: 11bit iIdent_c::S or 29bit iIdent_c::E
      default defined in isoaglib_config.h
  */
  void setIdent (MASK_TYPE rt_ident, iIdent_c::identType_t rt_type = DEFAULT_IDENT_TYPE)
    { CANPkg_c::setIdent (rt_ident, rt_type); }

  /**
    set specific uint8_t of ident for the telegram
    (position 0 is least significant position -> nearest to DLC field of
    CAN frame)
    @param rb_val value for ident at wanted position for the telegram
    @param rb_pos position for wanted value for ident for the telegram
    @param rt_type type of Ident_c: 11bit Ident_c::S or 29bit Ident_c::E
      default defined in isoaglib_config.h
  */
  static void setIdent (uint8_t rb_val, uint8_t rb_pos, iIdent_c::identType_t rt_type = DEFAULT_IDENT_TYPE)
    { CANPkg_c::setIdent (rb_val, rb_pos, rt_type); }

  /**
    set type of ident
    @param rt_type type of Ident_c: 11bit Ident_c::S or 29bit Ident_c::E
  */
  static void setIdentType(iIdent_c::identType_t rt_type) { CANPkg_c::setIdentType (rt_type); }


  /**
    deliver type of Ident_c: 11bit standard or 29bit extended
    @return: Ident_c::S or Ident_c::E
  */
  static iIdent_c::identType_t identType() { return CANPkg_c::identType(); }

  /**
    deliver the ident
    @return ident setting as MASK_TYPE
  */
  static MASK_TYPE ident() { return CANPkg_c::ident(); }

  /**
    deliver the uint8_t value of ident at wanted position
    (position 0 is least significant position -> nearest to DLC field of
    CAN frame)
    @param rb_pos
    @return ident value
  */
  static MASK_TYPE ident(uint8_t rb_pos) { return CANPkg_c::ident(rb_pos); }

  /**
    deliver the uint8_t value of ident at wanted position
    (position 0 is least significant position -> nearest to DLC field of
    CAN frame)
    @param rb_pos
    @return ident value
  */
  static uint8_t& identRef(uint8_t rb_pos) { return CANPkg_c::identRef(rb_pos); }

  /** retrieve CAN data bytes represented by pointer to Union8ByteString_u */
  static __IsoAgLib::Flexible8ByteString_c* getDataUnion() { return CANPkg_c::getDataUnion(); }

  /** retrieve CAN data bytes represented by CONST pointer to Union8ByteString_u */
  static const __IsoAgLib::Flexible8ByteString_c* getDataUnionConst() { return CANPkg_c::getDataUnionConst(); }

  /** retrieve a raw const pointer to uint8_t data string from given offset position onwards */
  static const uint8_t* getUint8DataConstPointer( uint8_t rui8_positionOffset )
    { return CANPkg_c::getUint8DataConstPointer (rui8_positionOffset); }

  /** retrieve a raw const pointer to uint8_t data string */
  static const uint8_t* getUint8DataConstPointer() { return CANPkg_c::getUint8DataConstPointer();}

  /** retrieve a raw const pointer to uint8_t data string from given offset position onwards */
  static uint8_t* getUint8DataPointer (uint8_t rui8_positionOffset)
    { return CANPkg_c::getUint8DataPointer (rui8_positionOffset); }

  /** retrieve a raw const pointer to uint8_t data string */
  static uint8_t* getUint8DataPointer() { return CANPkg_c::getUint8DataPointer(); }

  /**
    set data with size bytes from source array;
    if rui8_len is greater than 8 (max data length of CAN) than max 8 bytes are transfered
    @param rb_data pointer to source data uint8_t array
    @param rui8_len amount/len of the source data string
  */
  static void setDataFromString(const uint8_t* rpb_data, uint8_t rui8_len)
    { CANPkg_c::setDataFromString(rpb_data, rui8_len); }

  /**
    set data with size bytes from source array;
    if rui8_len is greater than 8 (max data length of CAN) than max 8 bytes are transfered.
    this variant of the setDataFromString allows to set the CAN data from specified offset position onwards.
    @param rui8_targetPositionOffset
    @param rb_data pointer to source data uint8_t array
    @param rui8_len amount/len of the source data string
  */
  static void setDataFromString(uint8_t rui8_targetPositionOffset, const uint8_t* rpb_data, uint8_t rui8_len)
    { CANPkg_c::setDataFromString(rui8_targetPositionOffset, rpb_data, rui8_len); }

  /**
    set a complete 8-Byte data string from source type Union8ByteString_u.
    The length of this package is automatically set to length 8.
    @param rpc_data pointer to source data Flexible8ByteString_c
  */
  static void setDataUnion(const __IsoAgLib::Flexible8ByteString_c* rpc_data)
    { CANPkg_c::setDataUnion(rpc_data); }

  /**
    set a complete 8-Byte data string from source type Union8ByteString_u.
    The length of this package is automatically set to length 8.
    @param rpc_data pointer to source data Flexible8ByteString_c
  */
  static void setDataUnion(uint8_t rui8_ind, const __IsoAgLib::Flexible4ByteString_c* rpc_data)
    { CANPkg_c::setDataUnion(rui8_ind, rpc_data); }

  /** retrieve len of last received CAN message */
  static uint8_t getLen( void ) { return CANPkg_c::getLen(); }

  /**
    set the data len of CAN pkg (if rb_val is greater than 8, 8 is stored)
    @param rb_val length/amount of uint8_t in data string
  */
  static void setLen(uint8_t rb_val) { CANPkg_c::setLen (rb_val); }

  /**
    set receive time
    @param ri32_time time stamp of CAN telegram in [msec.] from system start
  */
  static void setTime(int32_t ri32_time) { CANPkg_c::setTime (ri32_time); }

  /**
    deliver time
    @return timestamp of the CAN telegram in [msec.] since system start
  */
  static int32_t time() { return CANPkg_c::time(); }

  /**
    set complete CAN msg with one function call
    (if rui8_len is greater than 8, than max 8 is used)
    @param rt_ident ident of the CAN telegram
    @param rpb_data pointer to the source data uint8_t string
    @param rui8_len amount of bytes in the data string
    @param ri32_time optional timestamp of CAN telegram in [msec.] since system start
  */
  static void set(MASK_TYPE rt_ident, const uint8_t* rpb_data, uint8_t rui8_len, int32_t ri32_time = 0,
    iIdent_c::identType_t rt_type = DEFAULT_IDENT_TYPE)
    { CANPkg_c::set (rt_ident, rpb_data, rui8_len, ri32_time, rt_type); }

  /**
    set complete CAN msg with one function call
    where exactly 8 data bytes are retrieved from pointer to Union8ByteString_u.
    As the union represents exactly 8-Data Byte, the message length is set to 8.
    @param rt_ident ident of the CAN telegram
    @param rpc_data pointer to the source data Flexible8ByteString_c string
    @param ri32_time optional timestamp of CAN telegram in [msec.] since system start
  */
  static void set(MASK_TYPE rt_ident, const __IsoAgLib::Flexible8ByteString_c* rpc_data, int32_t ri32_time = 0,
    iIdent_c::identType_t rt_type = DEFAULT_IDENT_TYPE)
    { CANPkg_c::set(rt_ident, rpc_data, ri32_time, rt_type); }


  /**
    put data into given reference to BIOS related data structure with data, len
    @param reft_ident     reference where the ident is placed for send
    @param refui8_identType reference to the ident type val: 0==std, 1==ext
    @param refb_dlcTarget reference to the DLC field of the target
    @param pb_dataTarget pointer to the data string of the target
  */
  virtual void getData(uint32_t& reft_ident, uint8_t& refui8_identType,
                       uint8_t& refb_dlcTarget, uint8_t* pb_dataTarget)
    { CANPkg_c::getData (reft_ident, refui8_identType, refb_dlcTarget, pb_dataTarget); }

  /** copy the data bytes from the CANPkg_c to the given uint8_t* pointer.
      the pointed array must be at least 8 byte in size.
      This function copies as many byte as are defined by ui8_len.
    */
  static void getDataToString( uint8_t* pui8_targetData )
    { CANPkg_c::getDataToString (pui8_targetData); }

  /** copy the data bytes from the CANPkg_c to the given uint8_t* pointer.
      the amount of copied data can be restricted by the last parameter.
      The first parameter defines the index of the first copied data byte.
    */
  static void getDataToString( uint8_t rui8_positionOffset, uint8_t* pui8_targetData, uint8_t rui8_maxSize )
    { CANPkg_c::getDataToString (rui8_positionOffset, pui8_targetData, rui8_maxSize); }

private:
  friend class iCANIO_c;
};

}
#endif
