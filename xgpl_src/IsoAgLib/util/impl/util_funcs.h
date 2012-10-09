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
#include <IsoAgLib/comm/Multipacket/impl/stream_c.h>
#endif
#include <vector>

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {
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
void int2Float(const void *const pvFrom, float *const pf_to);
/**
  copy float value to 4 uint8_t data string into pointer;
  use e.g. to access uint8_t memory representation of
  float value as int32_t value;
  often needed for CANPkg_c formating
  @param pf_from source float value
  @param pvTo target data string
*/
void float2Int(const float *const pf_from, void *const pvTo);

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
/** convert little endian byte string into an unsigned variable */
template<class T> void convertLittleEndianString( const uint8_t* rpui8_src, T& ref_result )
{
  #ifdef OPTIMIZE_NUMBER_CONVERSIONS_FOR_LITTLE_ENDIAN
  std::memcpy( &ref_result, rpui8_src, sizeof(T) );
  #else
  ref_result = T(rpui8_src[0]);
  for ( unsigned int ind = 1; ( ind < sizeof(T) ); ind++ )
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
} // end of namespace __IsoAgLib
#endif
