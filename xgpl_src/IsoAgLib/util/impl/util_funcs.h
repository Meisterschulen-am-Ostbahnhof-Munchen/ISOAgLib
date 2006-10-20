/***************************************************************************
                          util_funcs.h - utils functions
                                         for checking time distances
                                         smaller than 1min. [msec.]
                             -------------------
    begin                : Sun Aug 1 1999
    copyright            : (C) 1999 - 2004 Dipl.-Inform. Achim Spangler
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
#ifndef UTIL_FUNCS_H
#define UTIL_FUNCS_H

#include <IsoAgLib/typedef.h>
#ifdef USE_DATASTREAMS_IO
class StreamInput_c;
#endif
#include <vector>
#ifdef USE_VT_UNICODE_SUPPORT
#include <string>
#endif

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {
  uint8_t bcd2dec(uint8_t rb_bcd);
  uint8_t dec2bcd(uint8_t rb_dec);
/**
  calculate res = (mul_1 / div_1) * (mul_2 / div_2) with 4 uint8_t
  integers without overflow problems caused by (mul_1 * mul_2)
  and without rounding errors by (mul_1 / div_1) * (mul_2 / div_2)
  @param ri32_mul_1 first factor for multiplication
  @param ri32_div_1 first divisor for first factor
  @param ri32_mul_2 second factor for multiplication
  @param ri32_div_2 second divisor for first factor
  @return result as int32_t value
*/
int32_t mul1Div1Mul2Div2(int32_t ri32_mul_1, int32_t ri32_div_1, int32_t ri32_mul_2, int32_t ri32_div_2);

/**
  copy 4 uint8_t data string into pointer to float value;
  use e.g. to access uint8_t memory representation of
  int32_t value as float value;
  often needed for CANPkg_c formating
  @param pvFrom source data string
  @param pf_to target float value
*/
void littleEndianStream2FloatVar(const void *const pvFrom, float *const pf_to);
/**
  copy float value to 4 uint8_t data string into pointer;
  use e.g. to access uint8_t memory representation of
  float value as int32_t value;
  often needed for CANPkg_c formating
  @param pf_from source float value
  @param pvTo target data string
*/
void floatVar2LittleEndianStream(const float *const pf_from, void *const pvTo);

/** calculate the total allocated HEAP for:
  - slist<T> with given size of T
  - add the overhead per node for slist<T> ( pointer to next item )
  - add the overhead for malloc_alloc Allocator which calls malloc for each single node ( HEAP block pointer )
  - add the overhead for alignment based on sizeof(int)
  @param rui16_sizeT sizeof(T) -> size of the stored class
  @param rui16_cnt amount of items ( default: 1 )
  @return amount of corresponding byte in heap
*/
uint16_t sizeSlistTWithMalloc( uint16_t rui16_sizeT, uint16_t rui16_cnt = 1 );
/** calculate the total allocated HEAP for:
  - list<T> with given size of T
  - add the overhead per node for list<T> ( TWO pointer to next and prev item )
  - add the overhead for malloc_alloc Allocator which calls malloc for each single node ( HEAP block pointer )
  - add the overhead for alignment based on sizeof(int)
  @param rui16_sizeT sizeof(T) -> size of the stored class
  @param rui16_cnt amount of items ( default: 1 )
  @return amount of corresponding byte in heap
*/
uint16_t sizeListTWithMalloc( uint16_t rui16_sizeT, uint16_t rui16_cnt = 1 );
/** calculate the total allocated HEAP for:
  - vector<T> with given size of T
  - add the overhead for malloc_alloc Allocator which calls malloc for each vector instance ( HEAP block pointer )
  - add the overhead for alignment based on sizeof(int)
  @param rui16_sizeT sizeof(T) -> size of the stored class
  @param rui16_capacity reserved space for vector<T> ( >= amount of currently stored items )
  @return amount of corresponding byte in heap
*/
uint16_t sizeVectorTWithMalloc( uint16_t rui16_sizeT, uint16_t rui16_capacity );
/** calculate the total allocated HEAP for:
  - slist<T> with given size of T
  - add the overhead per node for slist<T> ( pointer to next item )
  - add the overhead caused by allocation large chunks of each 40 items
  - add overhead for linking the HEAP block by the lowloevel malloc
  - add the overhead for alignment based on sizeof(int)
  @param rui16_sizeT sizeof(T) -> size of the stored class
  @param rui16_cnt amount of items ( default: 1 )
  @return amount of corresponding byte in heap
*/
uint16_t sizeSlistTWithChunk( uint16_t rui16_sizeT, uint16_t rui16_cnt = 1 );
/** calculate the total allocated HEAP for:
  - list<T> with given size of T
  - add the overhead per node for slist<T> ( pointers to next+prev item )
  - add the overhead caused by allocation large chunks of each 40 items
  - add overhead for linking the HEAP block by the lowloevel malloc
  - add the overhead for alignment based on sizeof(int)
  @param rui16_sizeT sizeof(T) -> size of the stored class
  @param rui16_cnt amount of items ( default: 1 )
  @return amount of corresponding byte in heap
*/
uint16_t sizeListTWithChunk( uint16_t rui16_sizeT, uint16_t rui16_cnt = 1 );
/** calculate the total allocated HEAP for:
  - vector<T> with given size of T
  - add the overhead for malloc_alloc Allocator which calls malloc for each vector instance ( HEAP block pointer )
  - add the overhead for alignment based on sizeof(int)
  @param rui16_sizeT sizeof(T) -> size of the stored class
  @param rui16_capacity reserved space for vector<T> ( >= amount of currently stored items )
  @return amount of corresponding byte in heap
*/
uint16_t sizeVectorTWithChunk( uint16_t rui16_sizeT, uint16_t rui16_capacity );


/** convert little endian byte string into an unsigned variable */
template<class T> void convertLittleEndianString( const uint8_t* rpui8_src, T& ref_result )
{
  #ifdef OPTIMIZE_NUMBER_CONVERSIONS_FOR_LITTLE_ENDIAN
  std::memcpy( &ref_result, rpui8_src, sizeof(T) );
  #else
  for ( unsigned int ind = sizeof(T)-1; ind >= 0; ind-- )
  {
    ref_result |= (T(rpui8_src[ind]) << (8*ind));
  }

  #endif
}

/** convert receive multistream into an unsigned variable */
inline uint8_t convertLittleEndianStringUi8( const uint8_t* rpui8_src ) { return rpui8_src[0];};
/** convert receive multistream into an unsigned variable */
inline int8_t convertLittleEndianStringI8( const uint8_t* rpui8_src ){ return (int8_t)(rpui8_src[0]);};
/** convert receive multistream into an unsigned variable */
uint16_t convertLittleEndianStringUi16( const uint8_t* rpui8_src );
/** convert receive multistream into an unsigned variable */
int16_t convertLittleEndianStringI16( const uint8_t* rpui8_src );
/** convert receive multistream into an unsigned variable */
uint32_t convertLittleEndianStringUi32( const uint8_t* rpui8_src );
/** convert receive multistream into an unsigned variable */
int32_t convertLittleEndianStringI32( const uint8_t* rpui8_src );

/** convert number reference variable to little endian byte string */
template<class T> void numberRef2LittleEndianString( const T& refc_src, uint8_t* pui8_target )
{
  #ifdef OPTIMIZE_NUMBER_CONVERSIONS_FOR_LITTLE_ENDIAN
  std::memcpy( pui8_target, &refc_src, sizeof(T) );
  #else
  const unsigned int BitSize = sizeof(T) * 8;
  for ( unsigned int ind = 0; ( ind < BitSize ); ind += 8 )
  {
    *pui8_target = ((refc_src >> ind) & 0xFF);
    pui8_target++;
  }
  #endif
}
/** convert number call-by-val variable to little endian byte string */
template<class T> void number2LittleEndianString( const T rt_src, uint8_t* pui8_target )
{
  #ifdef OPTIMIZE_NUMBER_CONVERSIONS_FOR_LITTLE_ENDIAN
  std::memcpy( pui8_target, &rt_src, sizeof(T) );
  #else
  const unsigned int BitSize = sizeof(T) * 8;
  for ( unsigned int ind = 0; ( ind < BitSize ); ind += 8 )
  {
    *pui8_target = ((rt_src >> ind) & 0xFF);
    pui8_target++;
  }
  #endif
}


/** convert number reference variable to little endian byte string
 * @return iterator so that write can be continued directly after last written position
*/
template<class T> void numberRef2LittleEndianString( const T& refc_src, std::vector<uint8_t>& refc_target )
{
#ifdef OPTIMIZE_NUMBER_CONVERSIONS_FOR_LITTLE_ENDIAN
  const uint8_t* pui8_src = &refc_src;
  const unsigned int size = sizeof(T);
  for ( unsigned int ind = 0; ind < size; ind++ )
  {
    refc_target.push_back( pui8_src[ind] );
  }
#else
  const unsigned int BitSize = sizeof(T) * 8;
  for ( unsigned int ind = 0; ( ind < BitSize ); ind += 8 )
  {
    refc_target.push_back((refc_src >> ind) & 0xFF);
  }
#endif
}
/** convert number call-by-val variable to little endian byte string
 * @return iterator so that write can be continued directly after last written position
*/
template<class T> void number2LittleEndianString( const T rt_src, std::vector<uint8_t>& refc_target )
{
#ifdef OPTIMIZE_NUMBER_CONVERSIONS_FOR_LITTLE_ENDIAN
  const uint8_t* pui8_src = (const uint8_t*)(&rt_src);
  const unsigned int size = sizeof(T);
  for ( unsigned int ind = 0; ind < size; ind++ )
  {
    refc_target.push_back( pui8_src[ind] );
  }
#else
  const unsigned int BitSize = sizeof(T) * 8;
  for ( unsigned int ind = 0; ( ind < BitSize ); ind += 8 )
  {
    refc_target.push_back((rt_src >> ind) & 0xFF);
  }
#endif
}




/** convert big endian textual number representation into little endian uint8_t string of specified size */
void bigEndianHexNumberText2CanString( const char* rc_src, uint8_t* pui8_target, unsigned int size );

/** convert big endian textual unsigned int 8Bit number representation into little endian uint8_t string */
void bigEndianHexNumberText2CanStringUint8( const char* rc_src, uint8_t* pui8_target );
/** convert big endian textual unsigned int 16Bit number representation into little endian uint8_t string of specified size */
void bigEndianHexNumberText2CanStringUint16( const char* rc_src, uint8_t* pui8_target );
/** convert big endian textual unsigned int 32Bit number representation into little endian uint8_t string of specified size */
void bigEndianHexNumberText2CanStringUint32( const char* rc_src, uint8_t* pui8_target );
/** convert big endian textual unsigned int 64Bit number representation into little endian uint8_t string of specified size */
void bigEndianHexNumberText2CanStringUint64( const char* rc_src, uint8_t* pui8_target );

/** convert big endian textual unsigned int up to 16Bit number representation into little endian uint8_t string of specified size */
void bigEndianDecNumberText2CanStringUint( const char* rc_src, uint8_t* pui8_target );


class Flexible4ByteString_c;
class Flexible8ByteString_c;

/** helper class with some inline functions to enable flexible, efficient
    and safe access to 4-Byte strings for the 4-Byte part of a process data package as:
    + uint8_t  array of size 4 -> name uint8
    + uint16_t array of size 2 -> name uint16
    + uint32_t array of size 1 -> name uint32 (( only for CPU with 16Bit or more)
  IMPORTANT: The internal order of bytes is LITTLE ENDIAN, so that the bytes are directly
             used for sending of CAN messages.
   ==>> for big endian CPU all assignment and retrieval functions have to use the helper
        like numberRef2LittleEndianString for safe data conversion / adaptation
  */
class Flexible4ByteString_c {

 public:
  Flexible4ByteString_c( const Flexible4ByteString_c& rrefc_src )
  {uint32[0] = rrefc_src.uint32[0];};

  /**
    simply set a Flexible4ByteString_c at a specific value index with.
    Important: Flexible4ByteString_c and Flexible8ByteString_c have
               the SAME byte order as a CAN message byte string.
    ==>> for BIG ENDIAN systems, this value can NOT directly be used as intger value
    @param rui8_ind position of set Flexible4ByteString_c [0..1]
  */
  void setFlexible4DataValueInd(uint8_t rui8_ind, const Flexible8ByteString_c& rc_value );

  /** constructor for INIT directly from a flexible positioned Stream.
      IMPORTANT: this works also when the string starts at ODD position!
  */
  Flexible4ByteString_c( const uint8_t* rpui8_srcStream = NULL );
  /** set this object from a optionally odd addressed string */
  void setDataFromString( const uint8_t* rpui8_srcStream, uint8_t rui8_len = 4 )
  #ifndef SYSTEM_A2
  { if (rpui8_srcStream != NULL ) CNAMESPACE::memcpy(uint8, rpui8_srcStream, rui8_len );};
  #else
  { if (rpui8_srcStream != NULL ) memcpy(uint8, rpui8_srcStream, rui8_len );};
 #endif

  /** copy contents of this object to a optionally odd addressed string */
  void getDataToString( uint8_t* pui8_targetStream, uint8_t rui8_len = 4 ) const
  #ifndef SYSTEM_A2
    { if (pui8_targetStream != NULL ) CNAMESPACE::memcpy( pui8_targetStream, uint8, rui8_len );};
  #else
    { if (pui8_targetStream != NULL ) memcpy( pui8_targetStream, uint8, rui8_len );};
  #endif

  /** set this object from a optionally odd addressed string */
  void setDataFromString( uint8_t rui8_offset, const uint8_t* rpui8_srcStream, uint8_t rui8_len = 4 );
  /** copy contents of this object to a optionally odd addressed string */
  void getDataToString( uint8_t rui8_offset, uint8_t* pui8_targetStream, uint8_t rui8_len = 4 ) const;


  /** assignment */
  const Flexible4ByteString_c& operator=( const Flexible4ByteString_c& rrefc_src )
  { uint32[0] = rrefc_src.uint32[0]; return *this; };
  /** compare for EQUAL */
  bool operator==( const Flexible4ByteString_c& rrefc_cmp ) const
  { // use inline implementation for this case
    return ( uint32[0] == rrefc_cmp.uint32[0] )?true:false;
  };
  /** compare for DIFFERENT */
  bool operator!=( const Flexible4ByteString_c& rrefc_cmp ) const
  {
    return ( uint32[0] != rrefc_cmp.uint32[0] )?true:false;
  };

  #if defined(OPTIMIZE_NUMBER_CONVERSIONS_FOR_LITTLE_ENDIAN)
  /** compare for LOWER */
  bool operator<( const Flexible4ByteString_c& rrefc_cmp ) const
  { // use inline implementation for this case
    return ( uint32[0] < rrefc_cmp.uint32[0] )?true:false;
  };
  /** compare for LARGER */
  bool operator>( const Flexible4ByteString_c& rrefc_cmp ) const
  { // use inline implementation for this case
    return ( uint32[0] > rrefc_cmp.uint32[0] )?true:false;
  };
  /** compare with:
    @return 0 == equal;
           +1 == this item has higher value than par;
           -1 == this item has lower value than par */
  int compare( const Flexible4ByteString_c& rrefc_cmp ) const
  {
    if      (uint32[0] < rrefc_cmp.uint32[0]) return -1;
    else if (uint32[0] > rrefc_cmp.uint32[0]) return +1;
    return 0;
  };
  #else
  /** compare for LOWER */
  bool operator<( const Flexible4ByteString_c& rrefc_cmp ) const;
  /** compare for LARGER */
  bool operator>( const Flexible4ByteString_c& rrefc_cmp ) const;
  /** compare with:
    @return 0 == equal;
           +1 == this item has higher value than par;
           -1 == this item has lower value than par */
  int compare( const Flexible4ByteString_c& rrefc_cmp ) const;
  #endif


  /**
    set an uint8_t value at specified position in string.
    IMPORTANT: position 0 matches to the least significant byte,
               as the string is ordered in LittleEndian order,
               identic to the order which is used for CAN messages
    @param rui8_pos Byte position [0..7]
    @param rui8_val uint8_t value to set
  */
  void setUint8Data( uint8_t rui8_pos, uint8_t rui8_val) { uint8[rui8_pos] = rui8_val;};
  /**
    simply deliver a uint8_t from a specific position with operator[].
    IMPORTANT: position 0 matches to the least significant byte,
               as the string is ordered in LittleEndian order,
               identic to the order which is used for CAN messages
    @param rui8_pos position of delivered uint8_t [0..7]
    @return uint8_t balue in CAN data string at pos rui8_pos
  */
  uint8_t operator[](uint8_t rui8_pos) const {return uint8[rui8_pos];};
  /**
    simply deliver a uint8_t from a specific position with.
    IMPORTANT: position 0 matches to the least significant byte,
               as the string is ordered in LittleEndian order,
               identic to the order which is used for CAN messages
    @param rui8_pos position of delivered uint8_t [0..7]
    @return uint8_t balue in CAN data string at pos rui8_pos
  */
  uint8_t getUint8Data(uint8_t rui8_pos) const {return uint8[rui8_pos];};


  #ifdef OPTIMIZE_NUMBER_CONVERSIONS_FOR_LITTLE_ENDIAN
  /**
    set an uint16_t value at specified position in string.
    IMPORTANT: position 0 matches to the least significant byte,
               as the string is ordered in LittleEndian order,
               identic to the order which is used for CAN messages
    Possible Error: <LibErr_c::Range, LibErr_c::Can> when rui8_pos > 2
    @param rui8_pos Byte position [0..2]
    @param rui16_val uint16_t value to set
  */
  void setUint16Data( uint8_t rui8_pos, uint16_t rui16_val);
  /**
    set an int16_t value at specified position in string.
    IMPORTANT: position 0 matches to the least significant byte,
               as the string is ordered in LittleEndian order,
               identic to the order which is used for CAN messages
    Possible Error: <LibErr_c::Range, LibErr_c::Can> when rui8_pos > 2
    @param rui8_pos Byte position [0..2]
    @param ri16_val int16_t value to set
  */
  void setInt16Data( uint8_t rui8_pos, int16_t ri16_val);
  /**
    simply deliver a uint16_t from a specific starting position with.
    IMPORTANT: position 0 matches to the least significant byte,
               as the string is ordered in LittleEndian order,
               identic to the order which is used for CAN messages
    Possible Error: <LibErr_c::Range, LibErr_c::Can> when rui8_pos > 2
    @param rui8_pos position of delivered uint16_t [0..2]
    @return uint16_t balue in CAN data string at pos (rui8_pos, rui8_pos+1) read Low/High order
  */
  uint16_t getUint16Data(uint8_t rui8_pos) const;
  /**
    simply deliver a int16_t from a specific starting position with.
    IMPORTANT: position 0 matches to the least significant byte,
               as the string is ordered in LittleEndian order,
               identic to the order which is used for CAN messages
    Possible Error: <LibErr_c::Range, LibErr_c::Can> when rui8_pos > 2
    @param rui8_pos position of delivered int16_t [0..2]
    @return int16_t balue in CAN data string at pos (rui8_pos, rui8_pos+1) read Low/High order
  */
  int16_t getInt16Data(uint8_t rui8_pos) const;
  #else
  /**
    set an uint16_t value at specified position in string.
    IMPORTANT: position 0 matches to the least significant byte,
               as the string is ordered in LittleEndian order,
               identic to the order which is used for CAN messages
    @param rui8_pos Byte position [0..2]
    @param rui16_val uint16_t value to set
  */
  void setUint16Data( uint8_t rui8_pos, uint16_t rui16_val)
    {numberRef2LittleEndianString( rui16_val, (uint8+rui8_pos) );};
  /**
    set an int16_t value at specified position in string.
    IMPORTANT: position 0 matches to the least significant byte,
               as the string is ordered in LittleEndian order,
               identic to the order which is used for CAN messages
    @param rui8_pos Byte position [0..2]
    @param ri16_val int16_t value to set
  */
  void setInt16Data( uint8_t rui8_pos, int16_t ri16_val)
    {numberRef2LittleEndianString( ri16_val, (uint8+rui8_pos) );};

  /**
    simply deliver a uint16_t from a specific starting position with.
    IMPORTANT: position 0 matches to the least significant byte,
               as the string is ordered in LittleEndian order,
               identic to the order which is used for CAN messages
    @param rui8_pos position of delivered uint16_t [0..2]
    @return uint16_t balue in CAN data string at pos (rui8_pos, rui8_pos+1) read Low/High order
  */
  uint16_t getUint16Data(uint8_t rui8_pos) const { return convertLittleEndianStringUi16(uint8+rui8_pos);};
  /**
    simply deliver a int16_t from a specific starting position with.
    IMPORTANT: position 0 matches to the least significant byte,
               as the string is ordered in LittleEndian order,
               identic to the order which is used for CAN messages
    @param rui8_pos position of delivered int16_t [0..2]
    @return int16_t balue in CAN data string at pos (rui8_pos, rui8_pos+1) read Low/High order
  */
  int16_t getInt16Data(uint8_t rui8_pos) const { return convertLittleEndianStringI16(uint8+rui8_pos);};
  #endif


  /**
    set an uint32_t value
    @param rui32_val uint32_t value to set
  */
  void setUint32Data( uint32_t rui32_val)
    #ifdef OPTIMIZE_NUMBER_CONVERSIONS_FOR_LITTLE_ENDIAN
    { uint32[0] = rui32_val;};
    #else
    {numberRef2LittleEndianString( rui32_val, uint8 );};
    #endif
  /**
    set an int32_t value
    @param ri32_val int32_t value to set
  */
  void setInt32Data( int32_t ri32_val)
    #ifdef OPTIMIZE_NUMBER_CONVERSIONS_FOR_LITTLE_ENDIAN
    { int32[0] = ri32_val;};
    #else
    {numberRef2LittleEndianString( ri32_val, uint8 );};
    #endif

  /**
    simply deliver a uint32_t
    @return uint32_t balue in CAN data string at pos rui32_pos
  */
  uint32_t getUint32Data() const
    #ifdef OPTIMIZE_NUMBER_CONVERSIONS_FOR_LITTLE_ENDIAN
    {return uint32[0];};
    #else
    { return convertLittleEndianStringUi32(uint8);};
    #endif
  /**
    simply deliver a int32_t
    @return int32_t balue in CAN data string at pos rui32_pos
  */
  int32_t getInt32Data() const
    #ifdef OPTIMIZE_NUMBER_CONVERSIONS_FOR_LITTLE_ENDIAN
    {return int32[0];};
    #else
    { return convertLittleEndianStringI32(uint8);};
    #endif


  /** define the values as anonymous union */
  union {
    uint8_t  uint8 [4];
    int8_t    int8 [4];
    uint16_t uint16[2];
    int16_t   int16[2];
    uint32_t uint32[1];
    int32_t   int32[1];
  };
};


/** helper class with some inline functions to enable flexible, efficient
    and safe access to 8-Byte strings for ISONAME and CANPkg_c as:
    + uint8_t  array of size 8 -> name uint8
    + uint16_t array of size 4 -> name uint16
    + uint32_t array of size 2 -> name uint32 (( only for CPU with 16Bit or more)
  ( + uint64_t array of size 1 -> name uint64 ONLY FOR CPU_WORD_SIZE >= 4 )
  IMPORTANT: The internal order of bytes is LITTLE ENDIAN, so that the bytes are directly
             used for sending of CAN messages.
   ==>> for big endian CPU all assignment and retrieval functions have to use the helper
        like numberRef2LittleEndianString for safe data conversion / adaptation
  */
class Flexible8ByteString_c {

 public:
  Flexible8ByteString_c( const Flexible8ByteString_c& rrefc_src );
  /** constructor for INIT directly from a flexible positioned Stream.
      IMPORTANT: this works also when the string starts at ODD position!
  */
  Flexible8ByteString_c( const uint8_t* rpui8_srcStream = NULL );
  /** set this object from a optionally odd addressed string */
  void setDataFromString( const uint8_t* rpui8_srcStream, uint8_t rui8_len = 8 )
  #ifndef SYSTEM_A2
  { if (rpui8_srcStream != NULL ) CNAMESPACE::memcpy(uint8, rpui8_srcStream, rui8_len );};
  #else
  { if (rpui8_srcStream != NULL ) memcpy(uint8, rpui8_srcStream, rui8_len );};
  #endif

  /** copy contents of this object to a optionally odd addressed string */
  void getDataToString( uint8_t* pui8_srcStream, uint8_t rui8_len = 8 ) const
  #ifndef SYSTEM_A2
  { if (pui8_srcStream != NULL ) CNAMESPACE::memcpy( pui8_srcStream, uint8, rui8_len );};
  #else
  { if (pui8_srcStream != NULL ) memcpy( pui8_srcStream, uint8, rui8_len );};
  #endif
  /** set this object from a optionally odd addressed string */
  void setDataFromString( uint8_t rui8_offset, const uint8_t* rpui8_srcStream, uint8_t rui8_len = 8 );
  /** copy contents of this object to a optionally odd addressed string */
  void getDataToString( uint8_t rui8_offset, uint8_t* pui8_srcStream, uint8_t rui8_len = 8 ) const;

  /** retrieve a raw const pointer to uint8_t data string from given offset position onwards */
  const uint8_t* getUint8DataConstPointer( uint8_t rui8_positionOffset ) const
    { return uint8+rui8_positionOffset;};
  /** retrieve a raw const pointer to uint8_t data string */
  const uint8_t* getUint8DataConstPointer() const { return uint8;};
  /** retrieve a raw const pointer to uint8_t data string from given offset position onwards */
  uint8_t* getUint8DataPointer( uint8_t rui8_positionOffset )
    { return uint8+rui8_positionOffset;};
  /** retrieve a raw const pointer to uint8_t data string */
  uint8_t* getUint8DataPointer() { return uint8;};


  #if SIZEOF_INT >= 4
  /** assignment */
  const Flexible8ByteString_c& operator=( const Flexible8ByteString_c& rrefc_src )
  { uint64[0] = rrefc_src.uint64[0]; return *this; };
  /** compare for EQUAL */
  bool operator==( const Flexible8ByteString_c& rrefc_cmp ) const
  { // use inline implementation for this case
    return ( uint64[0] == rrefc_cmp.uint64[0] )?true:false;
  };
  /** compare for DIFFERENT */
  bool operator!=( const Flexible8ByteString_c& rrefc_cmp ) const
  {
    return ( uint64[0] != rrefc_cmp.uint64[0] )?true:false;
  };
  #else
  /** assignment */
  const Flexible8ByteString_c& operator=( const Flexible8ByteString_c& rrefc_src );
  /** compare for EQUAL */
  bool operator==( const Flexible8ByteString_c& rrefc_cmp ) const;
  /** compare for DIFFERENT */
  bool operator!=( const Flexible8ByteString_c& rrefc_cmp ) const;
  #endif


  #if defined(OPTIMIZE_NUMBER_CONVERSIONS_FOR_LITTLE_ENDIAN) && SIZEOF_INT >= 4
  /** compare for LOWER */
  bool operator<( const Flexible8ByteString_c& rrefc_cmp ) const
  { // use inline implementation for this case
    return ( uint64[0] < rrefc_cmp.uint64[0] )?true:false;
  };
  /** compare for LARGER */
  bool operator>( const Flexible8ByteString_c& rrefc_cmp ) const
  { // use inline implementation for this case
    return ( uint64[0] > rrefc_cmp.uint64[0] )?true:false;
  };
  /** compare with:
    @return 0 == equal;
           +1 == this item has higher value than par;
           -1 == this item has lower value than par */
  int compare( const Flexible8ByteString_c& rrefc_cmp ) const
  {
    if      (uint64[0] < rrefc_cmp.uint64[0]) return -1;
    else if (uint64[0] > rrefc_cmp.uint64[0]) return +1;
    return 0;
  };
  #else
  /** compare for LOWER */
  bool operator<( const Flexible8ByteString_c& rrefc_cmp ) const;
  /** compare for LARGER */
  bool operator>( const Flexible8ByteString_c& rrefc_cmp ) const;
  /** compare with:
    @return 0 == equal;
           +1 == this item has higher value than par;
           -1 == this item has lower value than par */
  int compare( const Flexible8ByteString_c& rrefc_cmp ) const;
  #endif

  /**
    set an uint8_t value at specified position in string.
    IMPORTANT: position 0 matches to the least significant byte,
               as the string is ordered in LittleEndian order,
               identic to the order which is used for CAN messages
    @param rui8_pos Byte position [0..7]
    @param rui8_val uint8_t value to set
  */
  void setUint8Data( uint8_t rui8_pos, uint8_t rui8_val) { uint8[rui8_pos] = rui8_val;};
  /**
    simply deliver a uint8_t from a specific position with operator[].
    IMPORTANT: position 0 matches to the least significant byte,
               as the string is ordered in LittleEndian order,
               identic to the order which is used for CAN messages
    @param rui8_pos position of delivered uint8_t [0..7]
    @return uint8_t balue in CAN data string at pos rui8_pos
  */
  uint8_t operator[](uint8_t rui8_pos) const {return uint8[rui8_pos];};
  /**
    simply deliver a uint8_t from a specific position with.
    IMPORTANT: position 0 matches to the least significant byte,
               as the string is ordered in LittleEndian order,
               identic to the order which is used for CAN messages
    @param rui8_pos position of delivered uint8_t [0..7]
    @return uint8_t balue in CAN data string at pos rui8_pos
  */
  uint8_t getUint8Data(uint8_t rui8_pos) const {return uint8[rui8_pos];};

  #ifdef OPTIMIZE_NUMBER_CONVERSIONS_FOR_LITTLE_ENDIAN
  /**
    set an uint16_t value at specified position in string.
    IMPORTANT: position 0 matches to the least significant byte,
               as the string is ordered in LittleEndian order,
               identic to the order which is used for CAN messages
    Possible Error: <LibErr_c::Range, LibErr_c::Can> when rui8_pos > 6
    @param rui8_pos Byte position [0..6]
    @param rui16_val uint16_t value to set
  */
  void setUint16Data( uint8_t rui8_pos, uint16_t rui16_val);
  /**
    set an int16_t value at specified position in string.
    IMPORTANT: position 0 matches to the least significant byte,
               as the string is ordered in LittleEndian order,
               identic to the order which is used for CAN messages
    Possible Error: <LibErr_c::Range, LibErr_c::Can> when rui8_pos > 6
    @param rui8_pos Byte position [0..6]
    @param ri16_val int16_t value to set
  */
  void setInt16Data( uint8_t rui8_pos, int16_t ri16_val);
  /**
    simply deliver a uint16_t from a specific starting position with.
    IMPORTANT: position 0 matches to the least significant byte,
               as the string is ordered in LittleEndian order,
               identic to the order which is used for CAN messages
    Possible Error: <LibErr_c::Range, LibErr_c::Can> when rui8_pos > 6
    @param rui8_pos position of delivered uint16_t [0..6]
    @return uint16_t balue in CAN data string at pos (rui8_pos, rui8_pos+1) read Low/High order
  */
  uint16_t getUint16Data(uint8_t rui8_pos) const;
  /**
    simply deliver a int16_t from a specific starting position with.
    IMPORTANT: position 0 matches to the least significant byte,
               as the string is ordered in LittleEndian order,
               identic to the order which is used for CAN messages
    Possible Error: <LibErr_c::Range, LibErr_c::Can> when rui8_pos > 6
    @param rui8_pos position of delivered int16_t [0..6]
    @return int16_t balue in CAN data string at pos (rui8_pos, rui8_pos+1) read Low/High order
  */
  int16_t getInt16Data(uint8_t rui8_pos) const;
  /**
    set an uint32_t value at specified position in string.
    IMPORTANT: position 0 matches to the least significant byte,
               as the string is ordered in LittleEndian order,
               identic to the order which is used for CAN messages
    Possible Error: <LibErr_c::Range, LibErr_c::Can> when rui8_pos > 6
    @param rui8_pos Byte position [0..4]
    @param rui32_val uint32_t value to set
  */
  void setUint32Data( uint8_t rui8_pos, uint32_t rui32_val);
  /**
    set an int32_t value at specified position in string.
    IMPORTANT: position 0 matches to the least significant byte,
               as the string is ordered in LittleEndian order,
               identic to the order which is used for CAN messages
    Possible Error: <LibErr_c::Range, LibErr_c::Can> when rui8_pos > 6
    @param rui8_pos Byte position [0..4]
    @param ri32_val int32_t value to set
  */
  void setInt32Data( uint8_t rui8_pos, int32_t ri32_val);
  /**
    simply deliver a uint32_t from a specific starting position with.
    IMPORTANT: position 0 matches to the least significant byte,
               as the string is ordered in LittleEndian order,
               identic to the order which is used for CAN messages
    Possible Error: <LibErr_c::Range, LibErr_c::Can> when rui8_pos > 6
    @param rui8_pos position of delivered uint32_t [0..4]
    @return uint32_t balue in CAN data string at pos (rui8_pos, rui8_pos+1) read Low/High order
  */
  uint32_t getUint32Data(uint8_t rui8_pos) const;
  /**
    simply deliver a int32_t from a specific starting position with.
    IMPORTANT: position 0 matches to the least significant byte,
               as the string is ordered in LittleEndian order,
               identic to the order which is used for CAN messages
    Possible Error: <LibErr_c::Range, LibErr_c::Can> when rui8_pos > 6
    @param rui8_pos position of delivered int32_t [0..4]
    @return int32_t balue in CAN data string at pos (rui8_pos, rui8_pos+1) read Low/High order
  */
  int32_t getInt32Data(uint8_t rui8_pos) const;
  #else
  /**
    set an uint16_t value at specified position in string.
    IMPORTANT: position 0 matches to the least significant byte,
               as the string is ordered in LittleEndian order,
               identic to the order which is used for CAN messages
    @param rui8_pos Byte position [0..6]
    @param rui16_val uint16_t value to set
  */
  void setUint16Data( uint8_t rui8_pos, uint16_t rui16_val)
    {numberRef2LittleEndianString( rui16_val, (uint8+rui8_pos) );};
  /**
    set an int16_t value at specified position in string.
    IMPORTANT: position 0 matches to the least significant byte,
               as the string is ordered in LittleEndian order,
               identic to the order which is used for CAN messages
    @param rui8_pos Byte position [0..6]
    @param ri16_val int16_t value to set
  */
  void setInt16Data( uint8_t rui8_pos, int16_t ri16_val)
    {numberRef2LittleEndianString( ri16_val, (uint8+rui8_pos) );};

  /**
    simply deliver a uint16_t from a specific starting position with.
    IMPORTANT: position 0 matches to the least significant byte,
               as the string is ordered in LittleEndian order,
               identic to the order which is used for CAN messages
    @param rui8_pos position of delivered uint16_t [0..6]
    @return uint16_t balue in CAN data string at pos (rui8_pos, rui8_pos+1) read Low/High order
  */
  uint16_t getUint16Data(uint8_t rui8_pos) const { return convertLittleEndianStringUi16(uint8+rui8_pos);};
  /**
    simply deliver a int16_t from a specific starting position with.
    IMPORTANT: position 0 matches to the least significant byte,
               as the string is ordered in LittleEndian order,
               identic to the order which is used for CAN messages
    @param rui8_pos position of delivered int16_t [0..6]
    @return int16_t balue in CAN data string at pos (rui8_pos, rui8_pos+1) read Low/High order
  */
  int16_t getInt16Data(uint8_t rui8_pos) const { return convertLittleEndianStringI16(uint8+rui8_pos);};
  /**
    set an uint32_t value at specified position in string.
    IMPORTANT: position 0 matches to the least significant byte,
               as the string is ordered in LittleEndian order,
               identic to the order which is used for CAN messages
    @param rui8_pos Byte position [0..4]
    @param rui32_val uint32_t value to set
  */
  void setUint32Data( uint8_t rui8_pos, uint32_t rui32_val)
    {numberRef2LittleEndianString( rui32_val, (uint8+rui8_pos) );};
  /**
    set an int32_t value at specified position in string.
    IMPORTANT: position 0 matches to the least significant byte,
               as the string is ordered in LittleEndian order,
               identic to the order which is used for CAN messages
    @param rui8_pos Byte position [0..4]
    @param ri32_val int32_t value to set
  */
  void setInt32Data( uint8_t rui8_pos, int32_t ri32_val)
    {numberRef2LittleEndianString( ri32_val, (uint8+rui8_pos) );};

  /**
    simply deliver a uint32_t from a specific starting position with.
    IMPORTANT: position 0 matches to the least significant byte,
               as the string is ordered in LittleEndian order,
               identic to the order which is used for CAN messages
    @param rui8_pos position of delivered uint32_t [0..4]
    @return uint32_t balue in CAN data string at pos (rui8_pos, rui8_pos+1) read Low/High order
  */
  uint32_t getUint32Data(uint8_t rui8_pos) const { return convertLittleEndianStringUi32(uint8+rui8_pos);};
  /**
    simply deliver a int32_t from a specific starting position with.
    IMPORTANT: position 0 matches to the least significant byte,
               as the string is ordered in LittleEndian order,
               identic to the order which is used for CAN messages
    @param rui8_pos position of delivered int32_t [0..4]
    @return int32_t balue in CAN data string at pos (rui8_pos, rui8_pos+1) read Low/High order
  */
  int32_t getInt32Data(uint8_t rui8_pos) const { return convertLittleEndianStringI32(uint8+rui8_pos);};
  #endif
  /**
    simply deliver a Flexible4ByteString_c from a specific value index with.
    Important: Flexible4ByteString_c has the SAME byte order as a CAN message byte string.
    ==>> for BIG ENDIAN systems, this value can NOT directly be used as intger value
    @param rui8_ind position of delivered uint16_t [0..1]
    @return Flexible4ByteString_c balue in CAN data string at pos rui16_pos
  */
  void setFlexible4DataValueInd(uint8_t rui8_ind, const Flexible4ByteString_c& rc_value )
    {uint32[rui8_ind] = rc_value.uint32[0];};



  /** define the values as anonymous union */
  union {
    uint8_t  uint8 [8];
    int8_t    int8 [8];
    uint16_t uint16[4];
    int16_t   int16[4];
    uint32_t uint32[2];
    int32_t   int32[2];
    #if SIZEOF_INT >= 4
    uint64_t uint64[1];
    int64_t   int64[1];
    #endif
  };
};


#ifdef USE_DATASTREAMS_IO
/** convert receive multistream into an unsigned variable */
uint16_t convertIstreamUi16( StreamInput_c& refc_stream );
/** convert receive multistream into an unsigned variable */
int16_t convertIstreamI16( StreamInput_c& refc_stream );
/** convert receive multistream into an unsigned variable */
uint32_t convertIstreamUi32( StreamInput_c& refc_stream );
/** convert receive multistream into an unsigned variable */
int32_t convertIstreamI32( StreamInput_c& refc_stream );
#endif




#ifdef USE_VT_UNICODE_SUPPORT
  /// Define UNICODE REPLACEMENT CHARACTER
  #define DEF_UNICODE_REPLACEMENT_CHAR 0xFFFD
  /// Define ISO-8859 REPLACEMENT CHARACTER (for any iso8859)
  #define DEF_ISO8859_REPLACEMENT_CHAR 0x20

  // forward declarations for inline functions!
  extern const uint16_t p96ui16_8859_2 [96];
  extern const uint16_t p96ui16_8859_4 [96];
  extern const uint16_t p96ui16_8859_5 [96];
  extern const uint16_t p96ui16_8859_7 [96];
  extern const uint16_t p96ui16_8859_15 [96];
  extern const uint16_t* pp96ui16_8859s [8];

  inline uint16_t convert8859ToUnicode (uint8_t rui8_char, uint8_t rui8_encoding)
  {
    if ((rui8_char < 0xA0) || (rui8_encoding == 0))
    { // direct mapping for [0x00..0x9F] and latin-1
      return uint16_t (rui8_char);
    }
    else // (rui8_char >= 0xA0) && (rui8_encoding != 0)
    { // lookup!
      return pp96ui16_8859s [rui8_encoding] [rui8_char - 0xA0]; // checking for valid encoding is omitted for performance reasons - there shouldn't be wrong entries in FontAttributes anyway!
    }
  }

  /// @todo OPTIMIZE: Reverse lookup up to now done a slow but non-space-consuming way. To be optimized later on using a generated reverse lookup table */
  /// keep inlined, as it will be optimized, so it's smaller then!
  inline uint8_t convertUnicodeTo8859 (uint16_t rui16_unicode, uint8_t rui8_encoding)
  {
    if (rui8_encoding == 0)
    {
      if (rui16_unicode >= 0x100)
        return DEF_ISO8859_REPLACEMENT_CHAR; // return iso8859-Replacement Character
      else
        return uint8_t (rui16_unicode);
    }
    const uint16_t* p96ui16_8859s = pp96ui16_8859s [rui8_encoding];
    if (rui16_unicode != DEF_UNICODE_REPLACEMENT_CHAR)
    {
      for (uint16_t ui16_cur=0xA0; ui16_cur < 0x100; ui16_cur++)
      {
        if (*p96ui16_8859s++ == rui16_unicode)
        {
          return uint8_t (ui16_cur);
        }
      }
    }
    return DEF_ISO8859_REPLACEMENT_CHAR; // default to iso8859-Replacement Character in case the char's not found;
  }

  void convertString8859ToUnicode (const std::string& rrefstr_iso8859, std::string& rrefstr_unicode, uint8_t rui8_encoding);
  void convertStringUnicodeTo8859 (const char* rpui8_stringUnicode, uint16_t rui16_stringUnicodeLength, char* rpui8_iso8859buffer, uint8_t rui8_encoding);
  void push_backUTF8 (std::string& rrefstr_string, uint16_t rui16_unicode);
#endif

} // end of namespace __IsoAgLib
#endif
