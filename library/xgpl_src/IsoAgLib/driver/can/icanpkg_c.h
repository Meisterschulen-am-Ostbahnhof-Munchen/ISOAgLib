/*
  icanpkg_c.h

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef iCAN_Pkg_H
#define iCAN_Pkg_H


#include "impl/canpkg_c.h"

// Begin Namespace IsoAgLib
namespace IsoAgLib {


/** @todo SOON-129: Add Interface class for FlexibleByteString stuff in iutil_funcs.h, so that this iCanPkg_c gets consistent ... */

/**
  Storing the informations of one CAN
  message package. Implements some methods
  for input and output of informations into
  other objects and data structures.
  @author Dipl.-Inform. Achim Spangler
*/
class iCanPkg_c : private __IsoAgLib::CanPkg_c
{
public:

  /**
    set ident for the telegram
    @param at_ident ident for the telegram
    @param at_type type of iIdent_c: 11bit iIdent_c::S or 29bit iIdent_c::E
  */
  void setIdent (MASK_TYPE at_ident, iIdent_c::identType_t at_type)
    { CanPkg_c::setIdent (at_ident, at_type); }

  /**
    set specific uint8_t of ident for the telegram
    (position 0 is least significant position -> nearest to DLC field of
    CAN frame)
    @param ab_val value for ident at wanted position for the telegram
    @param ab_pos position for wanted value for ident for the telegram
  */
  static void setIdentByte (uint8_t ab_val, uint8_t ab_pos )
    { CanPkg_c::setIdentByte (ab_val, ab_pos); }

  /**
    set type of ident
    @param at_type type of Ident_c: 11bit Ident_c::S or 29bit Ident_c::E
  */
  static void setIdentType(iIdent_c::identType_t at_type) { CanPkg_c::setIdentType (at_type); }


  /**
    deliver type of Ident_c: 11bit standard or 29bit extended
    @return: Ident_c::S or Ident_c::E
  */
  static iIdent_c::identType_t identType() { return CanPkg_c::identType(); }

  /**
    deliver the ident
    @return ident setting as MASK_TYPE
  */
  static MASK_TYPE ident() { return CanPkg_c::ident(); }

  /**
    deliver the uint8_t value of ident at wanted position
    (position 0 is least significant position -> nearest to DLC field of
    CAN frame)
    @param ab_pos
    @return ident value
  */
  static MASK_TYPE ident(uint8_t ab_pos) { return CanPkg_c::ident(ab_pos); }

  /** retrieve CAN data bytes represented by pointer to Union8ByteString_u */
  static __IsoAgLib::Flexible8ByteString_c* getDataUnion() { return CanPkg_c::getDataUnion(); }

  /** retrieve CAN data bytes represented by CONST pointer to Union8ByteString_u */
  static const __IsoAgLib::Flexible8ByteString_c* getDataUnionConst() { return CanPkg_c::getDataUnionConst(); }

  /** retrieve a raw const pointer to uint8_t data string from given offset position onwards */
  static const uint8_t* getUint8DataConstPointer( uint8_t aui8_positionOffset )
    { return CanPkg_c::getUint8DataConstPointer (aui8_positionOffset); }

  /** retrieve a raw const pointer to uint8_t data string */
  static const uint8_t* getUint8DataConstPointer() { return CanPkg_c::getUint8DataConstPointer();}

  /** retrieve a raw const pointer to uint8_t data string from given offset position onwards */
  static uint8_t* getUint8DataPointer (uint8_t aui8_positionOffset)
    { return CanPkg_c::getUint8DataPointer (aui8_positionOffset); }

  /** retrieve a raw const pointer to uint8_t data string */
  static uint8_t* getUint8DataPointer() { return CanPkg_c::getUint8DataPointer(); }

  /**
    set data with size bytes from source array;
    if aui8_len is greater than 8 (max data length of CAN) than max 8 bytes are transfered
    @param apb_data pointer to source data uint8_t array
    @param aui8_len amount/len of the source data string
  */
  static void setDataFromString(const uint8_t* apb_data, uint8_t aui8_len)
    { CanPkg_c::setDataFromString(apb_data, aui8_len); }

  /**
    set data with size bytes from source array;
    if aui8_len is greater than 8 (max data length of CAN) than max 8 bytes are transfered.
    this variant of the setDataFromString allows to set the CAN data from specified offset position onwards.
    @param aui8_targetPositionOffset
    @param apb_data pointer to source data uint8_t array
    @param aui8_len amount/len of the source data string
  */
  static void setDataFromString(uint8_t aui8_targetPositionOffset, const uint8_t* apb_data, uint8_t aui8_len)
    { CanPkg_c::setDataFromString(aui8_targetPositionOffset, apb_data, aui8_len); }

  /**
    set a complete 8-Byte data string from source type Union8ByteString_u.
    The length of this package is automatically set to length 8.
    @param apc_data pointer to source data Flexible8ByteString_c
  */
  static void setDataUnion(const __IsoAgLib::Flexible8ByteString_c* apc_data)
    { CanPkg_c::setDataUnion(apc_data); }

  /**
    set a complete 8-Byte data string from source type Union8ByteString_u.
    The length of this package is automatically set to length 8.
    @param apc_data pointer to source data Flexible8ByteString_c
  */
  static void setDataUnion(uint8_t aui8_ind, const __IsoAgLib::Flexible4ByteString_c* apc_data)
    { CanPkg_c::setDataUnion(aui8_ind, apc_data); }

  /** retrieve len of last received CAN message */
  static uint8_t getLen( void ) { return CanPkg_c::getLen(); }

  /**
    set the data len of CAN pkg (if ab_val is greater than 8, 8 is stored)
    @param ab_val length/amount of uint8_t in data string
  */
  static void setLen(uint8_t ab_val) { CanPkg_c::setLen (ab_val); }

  /**
    set receive time
    @param ai32_time time stamp of CAN telegram in [msec.] from system start
  */
  static void setTime(int32_t ai32_time) { CanPkg_c::setTime (ai32_time); }

  /**
    deliver time
    @return timestamp of the CAN telegram in [msec.] since system start
  */
  static int32_t time() { return CanPkg_c::time(); }

  /**
    set complete CAN msg with one function call
    (if aui8_len is greater than 8, than max 8 is used)
    @param at_ident ident of the CAN telegram
    @param apb_data pointer to the source data uint8_t string
    @param aui8_len amount of bytes in the data string
    @param ai32_time optional timestamp of CAN telegram in [msec.] since system start
  */
  static void set(MASK_TYPE at_ident, const uint8_t* apb_data, uint8_t aui8_len, int32_t ai32_time,
    iIdent_c::identType_t at_type)
    { CanPkg_c::set (at_ident, apb_data, aui8_len, ai32_time, at_type); }

  /**
    set complete CAN msg with one function call
    where exactly 8 data bytes are retrieved from pointer to Union8ByteString_u.
    As the union represents exactly 8-Data Byte, the message length is set to 8.
    @param at_ident ident of the CAN telegram
    @param apc_data pointer to the source data Flexible8ByteString_c string
    @param ai32_time optional timestamp of CAN telegram in [msec.] since system start
  */
  static void set(MASK_TYPE at_ident, const __IsoAgLib::Flexible8ByteString_c* apc_data, int32_t ai32_time,
    iIdent_c::identType_t at_type)
    { CanPkg_c::set(at_ident, apc_data, ai32_time, at_type); }


  /**
    put data into given reference to BIOS related data structure with data, len
    @param rt_ident     reference where the ident is placed for send
    @param rui8_identType reference to the ident type val: 0==std, 1==ext
    @param rb_dlcTarget reference to the DLC field of the target
    @param pb_dataTarget pointer to the data string of the target
  */
  virtual void getData(uint32_t& rt_ident, uint8_t& rui8_identType,
                       uint8_t& rb_dlcTarget, uint8_t* pb_dataTarget)
    { CanPkg_c::getData (rt_ident, rui8_identType, rb_dlcTarget, pb_dataTarget); }

  /** copy the data bytes from the CanPkg_c to the given uint8_t* pointer.
      the pointed array must be at least 8 byte in size.
      This function copies as many byte as are defined by msui8_len.
    */
  static void getDataToString( uint8_t* pui8_targetData )
    { CanPkg_c::getDataToString (pui8_targetData); }

  /** copy the data bytes from the CanPkg_c to the given uint8_t* pointer.
      the amount of copied data can be restricted by the last parameter.
      The first parameter defines the index of the first copied data byte.
    */
  static void getDataToString( uint8_t aui8_positionOffset, uint8_t* pui8_targetData, uint8_t aui8_maxSize )
    { CanPkg_c::getDataToString (aui8_positionOffset, pui8_targetData, aui8_maxSize); }

private:
  friend class iIsoBus_c;
  friend class iProprietaryBus_c;
};

/** this typedef is only for some time to provide backward compatibility at API level */
typedef iCanPkg_c iCANPkg_c;

}
#endif
