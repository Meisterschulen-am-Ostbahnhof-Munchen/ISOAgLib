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

/** max 8 data bytes defined as union */
Flexible8ByteString_c CanPkg_c::msc_data;

/** receive time of CAN message */
int32_t CanPkg_c::msi32_time = 0;

/** identifier of CAN msg */
__IsoAgLib::Ident_c CanPkg_c::msc_ident;

/** state of the received can-message */
MessageState_t CanPkg_c::mst_msgState = MessageValid; // no default value is needed, but anyway..

/** size of data */
uint8_t CanPkg_c::msui8_len = 0;

/**
  default constructor, which does nothing for the base class,
  but can do something in derived classes
*/
CanPkg_c::CanPkg_c( int ai_singletonVecKey )
  : ClientBase( ai_singletonVecKey )
 {
}

/** virtual destructor, which can be overloaded in derived classes */
CanPkg_c::~CanPkg_c(){
}

/**
  ==> OBSOLETE, because now all pkg-data is STATIC!
  ==> REACTIVATE if some NON-STATIC member vars will be added!
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  assign operator to insert informations from one CanPkg_c into another
  @see __IsoAgLib::FilterBox_c::operator>>
  @see CanPkgExt_c::operator=
  @see CanPkgExt_c::getData
  @param acrc_right reference to the source CanPkg_c on the right
  @return reference to the source CanPkg_c to enable assign chains like "pkg1 = pkg2 = pkg3 = pkg4;"
const CanPkg_c& CanPkg_c::operator=(const CanPkg_c& acrc_right)
{
  msc_ident = acrc_right.msc_ident;
  msui8_len = acrc_right.msui8_len;
  msc_data = acrc_right.msc_data;
  msi32_time = acrc_right.msi32_time;

  return *this;
}
*/

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
  rb_dlcTarget = msui8_len;
  msc_data.getDataToString( pb_dataTarget, msui8_len );
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
  msc_ident.set(at_ident, at_type);
  msui8_len = (aui8_len<9)?aui8_len:8;
  msc_data.setDataFromString( apb_data, msui8_len );
  msi32_time = ai32_time;
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
    msui8_len = (aui8_len<9)?aui8_len:8;
    msc_data.setDataFromString( apb_data, msui8_len);
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
    msui8_len = aui8_targetPositionOffset + cui_copyByteCnt;
    msc_data.setDataFromString( aui8_targetPositionOffset, apb_data, cui_copyByteCnt);
  }
}

/**
  ==> OBSOLETE, because now all pkg-data is STATIC!
  ==> REACTIVATE if some NON-STATIC member vars will be added!

  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  compare for equality with other CANPkg
  @param acrc_cmp reference to the to be compared CANPkg
  @return true -> both CanPkg_c have the same data
bool CanPkg_c::operator==(const CanPkg_c& acrc_cmp) const
{
  bool b_result = true;
  if (msc_data != acrc_cmp.msc_data) b_result = false;
  if (msc_ident != acrc_cmp.msc_ident) b_result = false;
  if (msui8_len != acrc_cmp.msui8_len) b_result = false;
  if (msi32_time != acrc_cmp.msi32_time) b_result = false;
  return b_result;
}
*/

/**
  ==> OBSOLETE, because now all pkg-data is STATIC!
  ==> REACTIVATE if some NON-STATIC member vars will be added!
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  compare for difference to other CANPkg
  @param acrc_cmp reference to the to be compared CANPkg
  @return true -> both CanPkg_c have different data
bool CanPkg_c::operator!=(const CanPkg_c& acrc_cmp) const
{
  ANYWAY I THINK THIS LOGIC IS WRONG!!!!!!!!!!!!!!!!!!!!!!!!

  bool b_result = true;

  if (msc_data == acrc_cmp.msc_data) b_result = false;
  if (msc_ident == acrc_cmp.msc_ident) b_result = false;
  if (msui8_len == acrc_cmp.msui8_len) b_result = false;
  if (msi32_time == acrc_cmp.msi32_time) b_result = false;

  return b_result;
  return false;
}
*/

} // end of namespace __IsoAgLib
