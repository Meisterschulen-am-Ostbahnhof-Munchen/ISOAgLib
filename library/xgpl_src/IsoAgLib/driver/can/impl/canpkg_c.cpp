/*
  canpkg_c.cpp: source for CanPkg_c, the encapsulation of CAN
     telegrams

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include <IsoAgLib/driver/can/impl/canpkg_c.h>

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

/**
  default constructor, which does nothing for the base class,
  but can do something in derived classes
*/
CanPkg_c::CanPkg_c() 
  : mc_data(),
    mi32_time( 0 ),
    mc_ident( 0, Ident_c::ExtendedIdent ),
    mui8_len( 0 )
 {
}

/** virtual destructor, which can be overloaded in derived classes */
CanPkg_c::~CanPkg_c(){}


CanPkg_c::CanPkg_c( const CanPkg_c& arc_src )
  : mc_data( arc_src.mc_data ),
    mi32_time( arc_src.mi32_time ),
    mc_ident( arc_src.mc_ident ),
    mui8_len( arc_src.mui8_len ) {}

#if 0
/**
  assign operator to insert informations from one CanPkg_c into another
  @see __IsoAgLib::FilterBox_c::operator>>
  @see CanPkgExt_c::operator=
  @see CanPkgExt_c::getData
  @param acrc_right reference to the source CanPkg_c on the right
  @return reference to the source CanPkg_c to enable assign chains like "pkg1 = pkg2 = pkg3 = pkg4;"
*/
const CanPkg_c& CanPkg_c::operator=(const CanPkg_c& acrc_right)
{
  mc_ident = acrc_right.mc_ident;
  mui8_len = acrc_right.mui8_len;
  mc_data = acrc_right.mc_data;
  mi32_time = acrc_right.mi32_time;

  return *this;
}
#endif

/**
  put data into given reference to BIOS related data structure with data, len
  @param rt_ident     reference where the ident is placed for send
  @param rui8_identType reference to the ident type val: 0==std, 1==ext
  @param rb_dlcTarget reference to the DLC field of the target
  @param pb_dataTarget pointer to the data string of the target
*/
void CanPkg_c::getData(uint32_t& rt_ident, uint8_t& rui8_identType,
                     uint8_t& rb_dlcTarget, uint8_t* pb_dataTarget)
{
  rt_ident = ident();
  rui8_identType = identType();
  rb_dlcTarget = mui8_len;
  mc_data.getDataToString( pb_dataTarget, mui8_len );
}

/**
  set complete CAN msg with one function call
  (if aui8_len is greater than 8, than max 8 is used)
  @param at_ident ident of the CAN telegram
  @param apb_data pointer to the source data uint8_t string
  @param aui8_len amount of bytes in the data string
  @param ai32_time optional timestamp of CAN telegram in [msec.] since system start
*/
void CanPkg_c::set(MASK_TYPE at_ident, const uint8_t* apb_data, uint8_t aui8_len, int32_t ai32_time, Ident_c::identType_t at_type)
{
  mc_ident.set(at_ident, at_type);
  mui8_len = (aui8_len<9)?aui8_len:8;
  mc_data.setDataFromString( apb_data, mui8_len );
  mi32_time = ai32_time;
}

/**
  set data with size bytes from source array;
  if aui8_len is greater than 8 (max data length of CAN) than max 8 bytes are transfered
  @param apb_data pointer to source data uint8_t array
  @param aui8_len amount/len of the source data string
*/
void CanPkg_c::setDataFromString(const uint8_t* apb_data, uint8_t aui8_len)
{
  if ( aui8_len > 0 )
  { // there is something to set - this function might be called from some generic algorithms which rely
    // on the underlying functions to handle zero-lenght properly
    mui8_len = (aui8_len<9)?aui8_len:8;
    mc_data.setDataFromString( apb_data, mui8_len);
  }
}

/**
  set data with size bytes from source array;
  if aui8_len is greater than 8 (max data length of CAN) than max 8 bytes are transfered.
  this variant of the setDataFromString allows to set the CAN data from specified offset position onwards.
  @param aui8_targetPositionOffset
  @param apb_data pointer to source data uint8_t array
  @param aui8_len amount/len of the source data string
*/
void CanPkg_c::setDataFromString(uint8_t aui8_targetPositionOffset, const uint8_t* apb_data, uint8_t aui8_len)
{
  if ( aui8_len > 0 )
  { // there is something to set - this function might be called from some generic algorithms which rely
    // on the underlying functions to handle zero-lenght properly
    const unsigned int cui_copyByteCnt = (aui8_len+aui8_targetPositionOffset <= 8)?aui8_len:(8-aui8_targetPositionOffset);
    mui8_len = aui8_targetPositionOffset + cui_copyByteCnt;
    mc_data.setDataFromString( aui8_targetPositionOffset, apb_data, cui_copyByteCnt);
  }
}

/**
  compare for equality with other CANPkg
  @param acrc_cmp reference to the to be compared CANPkg
  @return true -> both CanPkg_c have the same data
*/
bool CanPkg_c::operator==(const CanPkg_c& acrc_cmp) const
{
  return (mc_data == acrc_cmp.mc_data)
    && (mc_ident == acrc_cmp.mc_ident)
    && (mui8_len == acrc_cmp.mui8_len)
    && (mi32_time == acrc_cmp.mi32_time);
}

/**
  compare for difference to other CANPkg
  @param acrc_cmp reference to the to be compared CANPkg
  @return true -> both CanPkg_c have different data
*/
bool CanPkg_c::operator!=(const CanPkg_c& acrc_cmp) const
{
  return (mc_data != acrc_cmp.mc_data)
    && (mc_ident != acrc_cmp.mc_ident)
    && (mui8_len != acrc_cmp.mui8_len)
    && (mi32_time != acrc_cmp.mi32_time);
}

} // end of namespace __IsoAgLib
