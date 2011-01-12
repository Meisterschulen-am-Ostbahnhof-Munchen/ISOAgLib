/*
  canpkg_c.h: header for CanPkg_c, the encapsulation of CAN telegrams

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef CAN_PKG_H
#define CAN_PKG_H


#include <IsoAgLib/isoaglib_config.h>
#include <IsoAgLib/driver/can/impl/ident_c.h>
#include <IsoAgLib/util/impl/util_funcs.h>

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

/** values which indicate the state of the received can-message */
typedef enum MessageState_en { MessageValid        = 0,

               AdrResolveMask      = (0x3<<0), // AdrResolve wraps SA and DA into one!
               AdrValid            = (0x0<<0),
               AdrOnlyNetworkMgmt  = (0x1<<0), // e.g. someone sending to a remote address (can be interesting to snoop!)
               AdrInvalid          = (0x3<<0), // e.g. someone sending with SA 0xFF. definitely malformed can-packet!

               DlcValidationMask   = (0x3<<2),
               DlcValid            = (0x0<<2),
               DlcInvalidTooLong   = (0x1<<2),
               DlcInvalidTooShort  = (0x2<<2),

               // from here on it's just additional information -
               // decisions can be purely taken on the flags above!
               SaValidationMask    = (0x3<<4),
               SaValid             = (0x0<<4),
               SaInvalidUnknown    = (0x1<<4),
               SaInvalidGlobal     = (0x2<<4),
               SaInvalidLocal      = (0x3<<4),

               DaValidationMask    = (0x3<<6),
               DaValid             = (0x0<<6),
               DaInvalidUnknown    = (0x1<<6),
               DaInvalidAnonymous  = (0x2<<6),
               DaInvalidRemote     = (0x3<<6),
             } MessageState_t;


/**
  Storing the informations of one CAN
  message package. Implements some methods
  for input and output of informations into
  other objects and data structures.
  @author Dipl.-Inform. Achim Spangler
*/
class CanPkg_c : public ClientBase {
public:
  /**
    default constructor, which does nothing for the base class,
    but can do something in derived classes
  */
  CanPkg_c( int ai_multitonInst = 0 );

  /** virtual destructor, which can be overloaded in derived classes */
  virtual ~CanPkg_c();

  /**
    set ident for the telegram
    @param at_ident ident for the telegram
    @param at_type type of Ident_c: 11bit Ident_c::S or 29bit Ident_c::E
  */
  static void setIdent(MASK_TYPE at_ident, __IsoAgLib::Ident_c::identType_t at_type)
    {msc_ident.set(at_ident, at_type);}

  /**
    set specific uint8_t of ident for the telegram
    (position 0 is least significant position -> nearest to DLC field of
    CAN frame)
    @param ab_val value for ident at wanted position for the telegram
    @param ab_pos position for wanted value for ident for the telegram
  */
  static void setIdentByte(uint8_t ab_val, uint8_t ab_pos) {
    msc_ident.setByte(ab_val, ab_pos);
  }

  /**
    set specific uint16_t of ident for the telegram
    (position 0 is least significant position -> nearest to DLC field of
    CAN frame)
    @param aui16_val value for ident at wanted position for the telegram
    @param aui8_pos position [0..1] for wanted value for ident for the telegram (pos0==byte0, pos1==byte2)
  */
  static void setIdentWord(uint16_t aui16_val, uint8_t aui8_pos) {
    msc_ident.setWord(aui16_val, aui8_pos);
  }

  /**
    set type of ident
    @param at_type type of Ident_c: 11bit Ident_c::S or 29bit Ident_c::E
  */
  static void setIdentType(__IsoAgLib::Ident_c::identType_t at_type){msc_ident.setIdentType(at_type);}


  /**
    deliver type of Ident_c: 11bit standard or 29bit extended
    @return: Ident_c::S or Ident_c::E
  */
  static __IsoAgLib::Ident_c::identType_t identType() {return msc_ident.identType();}

  /**
    deliver the ident
    @return ident setting as MASK_TYPE
  */
  static MASK_TYPE ident() {return msc_ident.ident();}

  /**
    deliver the uint8_t value of ident at wanted position
    (position 0 is least significant position -> nearest to DLC field of
    CAN frame)
    @param ab_pos
    @return ident value
  */
  static uint8_t ident(uint8_t ab_pos) {return msc_ident.ident(ab_pos);}

  /** retrieve CAN data bytes represented by pointer to Union8ByteString_u */
  static Flexible8ByteString_c* getDataUnion() { return &msc_data;}

  /** retrieve CAN data bytes represented by CONST pointer to Union8ByteString_u */
  static const Flexible8ByteString_c* getDataUnionConst() { return &msc_data;}

  /** retrieve a raw const pointer to uint8_t data string from given offset position onwards */
  static const uint8_t* getUint8DataConstPointer( uint8_t aui8_positionOffset )
    { return msc_data.getUint8DataConstPointer( aui8_positionOffset );}

  /** retrieve a raw const pointer to uint8_t data string */
  static const uint8_t* getUint8DataConstPointer() { return msc_data.getUint8DataConstPointer();}

  /** retrieve a raw const pointer to uint8_t data string from given offset position onwards */
  static uint8_t* getUint8DataPointer( uint8_t aui8_positionOffset )
    { return msc_data.getUint8DataPointer( aui8_positionOffset );}

  /** retrieve a raw const pointer to uint8_t data string */
  static uint8_t* getUint8DataPointer() { return msc_data.getUint8DataPointer();}

  /**
    set data with size bytes from source array;
    if aui8_len is greater than 8 (max data length of CAN) than max 8 bytes are transfered
    @param apb_data pointer to source data uint8_t array
    @param aui8_len amount/len of the source data string
  */
  static void setDataFromString(const uint8_t* apb_data, uint8_t aui8_len);

  /**
    set data with size bytes from source array;
    if aui8_len is greater than 8 (max data length of CAN) than max 8 bytes are transfered.
    this variant of the setDataFromString allows to set the CAN data from specified offset position onwards.
    @param aui8_targetPositionOffset
    @param apb_data pointer to source data uint8_t array
    @param aui8_len amount/len of the source data string
  */
  static void setDataFromString(uint8_t aui8_targetPositionOffset, const uint8_t* apb_data, uint8_t aui8_len);

  /**
    set a complete 8-Byte data string from source type Union8ByteString_u.
    The length of this package is automatically set to length 8.
    @param apc_data pointer to source data Flexible8ByteString_c
  */
  static void setDataUnion(const Flexible8ByteString_c* apc_data)
    { msui8_len = 8; msc_data = *apc_data; }

  /**
    set a complete 8-Byte data string from source type Union8ByteString_u.
    The length of this package is automatically set to length 8.
    @param apc_data pointer to source data Flexible8ByteString_c
  */
  static void setDataUnion(uint8_t aui8_ind, const Flexible4ByteString_c* apc_data)
    { msc_data.setFlexible4DataValueInd( aui8_ind, *apc_data ); }

  /** retrieve len of last received CAN message */
  static uint8_t getLen( void ) { return msui8_len;}

  /**
    set the data len of CAN pkg (if ab_val is greater than 8, 8 is stored)
    @param ab_val length/amount of uint8_t in data string
  */
  static void setLen(uint8_t ab_val){msui8_len = (ab_val<9)?ab_val:uint8_t(8);}

  /**
    set receive time
    @param ai32_time time stamp of CAN telegram in [msec.] from system start
  */
  static void setTime(int32_t ai32_time){msi32_time = ai32_time;}

  /**
    deliver time
    @return timestamp of the CAN telegram in [msec.] since system start
  */
  static int32_t time(){return msi32_time;}

  /**
    set complete CAN msg with one function call
    (if aui8_len is greater than 8, than max 8 is used)
    @param at_ident ident of the CAN telegram
    @param apb_data pointer to the source data uint8_t string
    @param aui8_len amount of bytes in the data string
    @param ai32_time optional timestamp of CAN telegram in [msec.] since system start
  */
  static void set(MASK_TYPE at_ident, const uint8_t* apb_data, uint8_t aui8_len, int32_t ai32_time,
    __IsoAgLib::Ident_c::identType_t at_type );

  /**
    set complete CAN msg with one function call
    where exactly 8 data bytes are retrieved from pointer to Union8ByteString_u.
    As the union represents exactly 8-Data Byte, the message length is set to 8.
    @param at_ident ident of the CAN telegram
    @param apc_data pointer to the source data Flexible8ByteString_c string
    @param ai32_time optional timestamp of CAN telegram in [msec.] since system start
  */
  static void set(MASK_TYPE at_ident, const Flexible8ByteString_c* apc_data, int32_t ai32_time,
    __IsoAgLib::Ident_c::identType_t at_type);

  /**
    ==> OBSOLETE, because now all pkg-data is STATIC!
    ==> REACTIVATE if some NON-STATIC member vars will be added!
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    assign operator to insert informations from one CanPkg_c into another
    @see __IsoAgLib::FilterBox_c::operator>>
    @see CanPkgExt_c::operator=
    @see CanPkgExt_c::getData
    @param acrc_right reference to the source CanPkg_c on the right
    @return reference to the source CanPkg_c to enable assign chains like
        "pkg1 = pkg2 = pkg3 = pkg4;"
  virtual const CanPkg_c& operator=(const CanPkg_c& acrc_right);
  */

  /**
    put data into given reference to BIOS related data structure with data, len
    @param rt_ident     reference where the ident is placed for send
    @param rui8_identType reference to the ident type val: 0==std, 1==ext
    @param rb_dlcTarget reference to the DLC field of the target
    @param pb_dataTarget pointer to the data string of the target
  */
  virtual void getData(uint32_t& rt_ident, uint8_t& rui8_identType,
                       uint8_t& rb_dlcTarget, uint8_t* pb_dataTarget);

  /** copy the data bytes from the CanPkg_c to the given uint8_t* pointer.
      the pointed array must be at least 8 byte in size.
      This function copies as many byte as are defined by msui8_len.
    */
  static void getDataToString( uint8_t* pui8_targetData )
    { msc_data.getDataToString( pui8_targetData, msui8_len  ); }

  /** copy the data bytes from the CanPkg_c to the given uint8_t* pointer.
      the amount of copied data can be restricted by the last parameter.
      The first parameter defines the index of the first copied data byte.
    */
  static void getDataToString( uint8_t aui8_positionOffset, uint8_t* pui8_targetData, uint8_t aui8_maxSize )
    {msc_data.getDataToString( aui8_positionOffset, pui8_targetData, aui8_maxSize );}
  /**
    ==> OBSOLETE, because now all pkg-data is STATIC!
    ==> REACTIVATE if some NON-STATIC member vars will be added!
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    compare for equality with other CANPkg
    @param acrc_cmp reference to the to be compared CANPkg
    @return true -> both CanPkg_c have the same data
  bool operator==(const CanPkg_c& acrc_cmp) const;
  */

  /**
    ==> OBSOLETE, because now all pkg-data is STATIC!
    ==> REACTIVATE if some NON-STATIC member vars will be added!
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    compare for difference to other CANPkg
    @param acrc_cmp reference to the to be compared CANPkg
    @return true -> both CanPkg_c have different data
  bool operator!=(const CanPkg_c& acrc_cmp) const;
  */

//protected: // Protected attributes
public:
  /** max 8 data bytes defined as union */
  static Flexible8ByteString_c msc_data;

  /** receive time of CAN message */
  static int32_t msi32_time;

  /** identifier of CAN msg */
  static __IsoAgLib::Ident_c msc_ident;

  static MessageState_t mst_msgState;

  /** size of data */
  static uint8_t msui8_len;
};

/** this typedef is only for some time to provide backward compatibility at API level */
typedef CanPkg_c CANPkg_c;

}
#endif
