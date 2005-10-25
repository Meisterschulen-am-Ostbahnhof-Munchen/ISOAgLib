/***************************************************************************
                          util_funcs.cpp - utils functions
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
#include "util_funcs.h"
#include <cstring>
#include <IsoAgLib/util/config.h>
#include <stdio.h>

using namespace std;

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
int32_t mul1Div1Mul2Div2(int32_t ri32_mul_1, int32_t ri32_div_1, int32_t ri32_mul_2, int32_t ri32_div_2)
{
  if ((ri32_mul_1 == 0) || (ri32_mul_2 == 0) || (ri32_div_1 == 0) || (ri32_div_2 == 0)) return 0;
  int32_t i32_result = (ri32_mul_1 / ri32_div_1) * (ri32_mul_2 / ri32_div_2);
  i32_result += ((ri32_mul_1 % ri32_div_1) * (ri32_mul_2 % ri32_div_2)) / (ri32_div_1 * ri32_div_2);
  i32_result += ((ri32_mul_1 / ri32_div_1) * (ri32_mul_2 % ri32_div_2)) / ri32_div_2;
  i32_result += ((ri32_mul_1 % ri32_div_1) * (ri32_mul_2 / ri32_mul_2)) / ri32_div_1;
  return i32_result;
}

/**
  copy 4 uint8_t data string into pointer to float value;
  use e.g. to access uint8_t memory representation of
  int32_t value as float value;
  often needed for CANPkg_c formating
  @param pvFrom source data string
  @param pf_to target float value
*/
void int2Float(const void *const pvFrom, float *const pf_to)
{
  uint8_t *pb_to = (uint8_t*)pf_to;
  const uint8_t *const pb_from   = (const uint8_t *)pvFrom;

  #if FLOAT_WORD_ORDER == WORD_HI_LO
    // change first and last two bytes for float access
    memcpy(pb_to, pb_from+2, 2);
    memcpy(pb_to+2, pb_from, 2);
  #else
    memcpy(pb_to, pb_from, 4);
  #endif
}
/**
  copy float value to 4 uint8_t data string into pointer;
  use e.g. to access uint8_t memory representation of
  float value as int32_t value;
  often needed for CANPkg_c formating
  @param pf_from source float value
  @param pvTo target data string
*/
void float2Int(const float *const pf_from, void *const pvTo)
{
  uint8_t *pb_to = (uint8_t*)pvTo;
  const uint8_t *const pb_from = (const uint8_t *)pf_from;

  #if FLOAT_WORD_ORDER == WORD_HI_LO
    // change first and last two bytes for float access
    memcpy(pb_to, pb_from+2, 2);
    memcpy(pb_to+2, pb_from, 2);
  #else
    memcpy(pb_to, pb_from, 4);
  #endif
}

/** calculate the total allocated HEAP for:
  - slist<T> with given size of T
  - add the overhead per node for slist<T> ( pointer to next item )
  - add the overhead for malloc_alloc Allocator which calls malloc for each single node ( HEAP block pointer )
  - add the overhead for alignment based on SIZEOF_INT
  @param rui16_sizeT sizeof(T) -> size of the stored class
  @param rui16_cnt amount of items ( default: 1 )
  @return amount of corresponding byte in heap
*/
uint16_t sizeSlistTWithMalloc( uint16_t rui16_sizeT, uint16_t rui16_cnt )
{ // size calculated from: stored class size + pointer to next item + block pointers in MALLOC HEAP structure
  const uint16_t unalignedItemSize = ( rui16_sizeT + 3 * sizeof(void*) );
  // regard alignment
  const uint16_t alignmentBase = ( ( 2 * SIZEOF_INT ) - 1 );
  const uint16_t alignedItemSize= (unalignedItemSize + alignmentBase) & (unsigned int)~alignmentBase;

  return ( alignedItemSize * rui16_cnt );
}

/** calculate the total allocated HEAP for:
  - list<T> with given size of T
  - add the overhead per node for list<T> ( TWO pointer to next and prev item )
  - add the overhead for malloc_alloc Allocator which calls malloc for each single node ( HEAP block pointer )
  - add the overhead for alignment based on SIZEOF_INT
  @param rui16_sizeT sizeof(T) -> size of the stored class
  @param rui16_cnt amount of items ( default: 1 )
  @return amount of corresponding byte in heap
*/
uint16_t sizeListTWithMalloc( uint16_t rui16_sizeT, uint16_t rui16_cnt )
{ // size calculated from: stored class size + pointer to next+prev item + block pointers in MALLOC HEAP structure
  const uint16_t unalignedItemSize = ( rui16_sizeT + 4 * sizeof(void*) );
  // regard alignment
  const uint16_t alignmentBase = ( ( 2 * SIZEOF_INT ) - 1 );
  const uint16_t alignedItemSize= (unalignedItemSize + alignmentBase) & (unsigned int)~alignmentBase;

  return ( alignedItemSize * rui16_cnt );
}
/** calculate the total allocated HEAP for:
  - vector<T> with given size of T
  - add the overhead for malloc_alloc Allocator which calls malloc for each vector instance ( HEAP block pointer )
  - add the overhead for alignment based on SIZEOF_INT
  @param rui16_sizeT sizeof(T) -> size of the stored class
  @param rui16_capacity reserved space for vector<T> ( >= amount of currently stored items )
  @return amount of corresponding byte in heap
*/
uint16_t sizeVectorTWithMalloc( uint16_t rui16_sizeT, uint16_t rui16_capacity )
{ // size calculated from: stored class size + block pointers in MALLOC HEAP structure
  const uint16_t sizeWithoutAlignment = (rui16_capacity * rui16_sizeT) + ( 2 * sizeof(void*) );
  // regard alignment
  const uint16_t alignmentBase = ( ( 2 * SIZEOF_INT ) - 1 );
  return (sizeWithoutAlignment + alignmentBase) & (unsigned int)~alignmentBase;
}

static const uint16_t targetChunkSize = 40;

/** get chunked size */
uint16_t getChunkedSize( uint16_t rui16_cnt )
{
  const uint16_t chunkCnt =
     ( ( rui16_cnt % targetChunkSize ) == 0 )
    ? rui16_cnt
    : ( ( ( rui16_cnt / targetChunkSize ) + 1 ) * targetChunkSize );
  return chunkCnt;
}


/** calculate the total allocated HEAP for:
  - slist<T> with given size of T
  - add the overhead per node for slist<T> ( pointer to next item )
  - add the overhead caused by allocation large chunks of each 40 items
  - add overhead for linking the HEAP block by the lowloevel malloc
  - add the overhead for alignment based on SIZEOF_INT
  @param rui16_sizeT sizeof(T) -> size of the stored class
  @param rui16_cnt amount of items ( default: 1 )
  @return amount of corresponding byte in heap
*/
uint16_t sizeSlistTWithChunk( uint16_t rui16_sizeT, uint16_t rui16_cnt )
{ // amount of allocated items - multiple of targetChunkSize
  const unsigned int chunkCnt = getChunkedSize(rui16_cnt);
  // size calculated from: chunkCnt of nodes with element T and the next-pointer
  const uint16_t sizeWithoutAlignment
    = ( ( ( rui16_sizeT + sizeof(void*) ) * chunkCnt ) + ( 2 * sizeof(void*) ) );
  // regard alignment
  const uint16_t alignmentBase = ( ( 2 * SIZEOF_INT ) - 1 );
  return (sizeWithoutAlignment + alignmentBase) & (unsigned int)~alignmentBase;
}

/** calculate the total allocated HEAP for:
  - list<T> with given size of T
  - add the overhead per node for slist<T> ( pointers to next+prev item )
  - add the overhead caused by allocation large chunks of each 40 items
  - add overhead for linking the HEAP block by the lowloevel malloc
  - add the overhead for alignment based on SIZEOF_INT
  @param rui16_sizeT sizeof(T) -> size of the stored class
  @param rui16_cnt amount of items ( default: 1 )
  @return amount of corresponding byte in heap
*/
uint16_t sizeListTWithChunk( uint16_t rui16_sizeT, uint16_t rui16_cnt )
{ // amount of allocated items - multiple of targetChunkSize
  const unsigned int chunkCnt = getChunkedSize(rui16_cnt);
  // size calculated from: chunkCnt of nodes with element T and the next/prev-pointers
  const uint16_t sizeWithoutAlignment
    = ( ( ( rui16_sizeT + ( 2 * sizeof(void*) ) ) * chunkCnt ) + ( 2 * sizeof(void*) ) );
  // regard alignment
  const uint16_t alignmentBase = ( ( 2 * SIZEOF_INT ) - 1 );
  return (sizeWithoutAlignment + alignmentBase) & (unsigned int)~alignmentBase;
}
/** calculate the total allocated HEAP for:
  - vector<T> with given size of T
  - add the overhead for malloc_alloc Allocator which calls malloc for each vector instance ( HEAP block pointer )
  - add the overhead for alignment based on SIZEOF_INT
  @param rui16_sizeT sizeof(T) -> size of the stored class
  @param rui16_capacity reserved space for vector<T> ( >= amount of currently stored items )
  @return amount of corresponding byte in heap
*/
uint16_t sizeVectorTWithChunk( uint16_t rui16_sizeT, uint16_t rui16_capacity )
{ // amount of allocated items - multiple of targetChunkSize
  const unsigned int chunkCnt = getChunkedSize(rui16_capacity);
  // chunked amount of items stored in one compact vector
  const uint16_t sizeWithoutAlignment = (chunkCnt * rui16_sizeT) + ( 2 * sizeof(void*) );
  // regard alignment
  const uint16_t alignmentBase = ( ( 2 * SIZEOF_INT ) - 1 );
  return (sizeWithoutAlignment + alignmentBase) & (unsigned int)~alignmentBase;
}

#ifdef USE_DATASTREAMS_IO
/** convert receive multistream into an unsigned variable */
uint16_t convertIstreamUi16( StreamInput_c& refc_stream )
{
  uint16_t ui16_temp;
  #ifdef OPTIMIZE_NUMBER_CONVERSIONS_FOR_LITTLE_ENDIAN
  uint8_t* pui8_writePointer = (uint8_t*)(&ui16_temp);
  refc_stream >> pui8_writePointer[0];
  refc_stream >> pui8_writePointer[1];
  #else
  uint8_t ui8_temp;
  refc_stream >> ui8_temp;
  ui16_temp = ui8_temp;
  refc_stream >> ui8_temp;
  ui16_temp |= uint16_t(ui8_temp) << 8;
  #endif
  return ui16_temp;
};
/** convert receive multistream into an unsigned variable */
int16_t convertIstreamI16( StreamInput_c& refc_stream )
{
  int16_t i16_temp;
#ifdef OPTIMIZE_NUMBER_CONVERSIONS_FOR_LITTLE_ENDIAN
  uint8_t* pui8_writePointer = (uint8_t*)(&i16_temp);
  refc_stream >> pui8_writePointer[0];
  refc_stream >> pui8_writePointer[1];
#else
  uint8_t ui8_temp;
  refc_stream >> ui8_temp;
  i16_temp = ui8_temp;
  refc_stream >> ui8_temp;
  i16_temp |= int16_t(ui8_temp) << 8;
#endif
  return i16_temp;
};
/** convert receive multistream into an unsigned variable */
uint32_t convertIstreamUi32( StreamInput_c& refc_stream )
{
  uint32_t ui32_temp;
  #ifdef OPTIMIZE_NUMBER_CONVERSIONS_FOR_LITTLE_ENDIAN
  uint8_t* pui8_writePointer = (uint8_t*)(&ui32_temp);
  for ( unsigned int ind = 0; ( ( ind < 4 ) && ( !refc_stream.eof() ) ); ind++ )
  {
    refc_stream >> *pui8_writePointer;
    pui8_writePointer++;
  }
  #else
  uint8_t ui8_temp;
  refc_stream >> ui8_temp;
  ui32_temp = uint32_t(ui8_temp);
  for ( unsigned int ind = 1; ( ( ind < 4 ) && ( !refc_stream.eof() ) ); ind++ )
  {
    refc_stream >> ui8_temp;
    ui32_temp |= (uint32_t(ui8_temp) << (8*ind));
  }
  #endif
  return ui32_temp;
};
/** convert receive multistream into an unsigned variable */
int32_t convertIstreamI32( StreamInput_c& refc_stream )
{
  int32_t i32_temp;
#ifdef OPTIMIZE_NUMBER_CONVERSIONS_FOR_LITTLE_ENDIAN
  uint8_t* pui8_writePointer = (uint8_t*)(&i32_temp);
  for ( unsigned int ind = 0; ( ( ind < 4 ) && ( !refc_stream.eof() ) ); ind++ )
  {
    refc_stream >> *pui8_writePointer;
    pui8_writePointer++;
  }
#else
  uint8_t ui8_temp;
  refc_stream >> ui8_temp;
  i32_temp = int32_t(ui8_temp);
  for ( unsigned int ind = 1; ( ( ind < 4 ) && ( !refc_stream.eof() ) ); ind++ )
  {
    refc_stream >> ui8_temp;
    i32_temp |= (int32_t(ui8_temp) << (8*ind));
  }
#endif
  return i32_temp;
};
#endif


/** convert receive multistream into an unsigned variable */
uint16_t convertLittleEndianStringUi16( const uint8_t* rpui8_src )
{
  uint16_t ui16_temp;
#ifdef OPTIMIZE_NUMBER_CONVERSIONS_FOR_LITTLE_ENDIAN
  memcpy( &ui16_temp, rpui8_src, sizeof(uint16_t) );
#else
  ui16_temp = uint16_t(rpui8_src[0]) | (uint16_t(rpui8_src[1]) << 8);
#endif
  return ui16_temp;
}

/** convert receive multistream into an unsigned variable */
int16_t convertLittleEndianStringI16( const uint8_t* rpui8_src )
{
  int16_t i16_temp;
#ifdef OPTIMIZE_NUMBER_CONVERSIONS_FOR_LITTLE_ENDIAN
  memcpy( &i16_temp, rpui8_src, sizeof(int16_t) );
#else
  i16_temp = int16_t(rpui8_src[0]) | (int16_t(rpui8_src[1]) << 8);
#endif
  return i16_temp;
}
/** convert receive multistream into an unsigned variable */
uint32_t convertLittleEndianStringUi32( const uint8_t* rpui8_src )
{
  uint32_t ui32_temp;
#ifdef OPTIMIZE_NUMBER_CONVERSIONS_FOR_LITTLE_ENDIAN
  memcpy( &ui32_temp, rpui8_src, sizeof(uint32_t) );
#else
  ui32_temp = uint32_t(rpui8_src[0]);
  for ( unsigned int ind = 1; ( ind < sizeof(uint32_t) ); ind++ )
  {
    ui32_temp |= (uint32_t(rpui8_src[ind]) << (8*ind));
  }
#endif
  return ui32_temp;
}
/** convert receive multistream into an unsigned variable */
int32_t convertLittleEndianStringI32( const uint8_t* rpui8_src )
{
  int32_t i32_temp;
#ifdef OPTIMIZE_NUMBER_CONVERSIONS_FOR_LITTLE_ENDIAN
  memcpy( &i32_temp, rpui8_src, sizeof(int32_t) );
#else
  i32_temp = int32_t(rpui8_src[0]);
  for ( unsigned int ind = 1; ( ind < sizeof(int32_t) ); ind++ )
  {
    i32_temp |= (int32_t(rpui8_src[ind]) << (8*ind));
  }
#endif
  return i32_temp;
}


void int2littleEndianString( unsigned int input, uint8_t* pui8_target, unsigned int size )
{
  #ifdef OPTIMIZE_NUMBER_CONVERSIONS_FOR_LITTLE_ENDIAN
  memcpy( pui8_target,   &input, size );
  #else
  for ( unsigned int ind = 0; ind < size; ind++ )
  {
    pui8_target[ind] = ((input >> ( 8 * ind )) & 0xFF );
  }
  #endif
}

#if SIZEOF_INT == 1
  #define SCANF_INT_STRING "%2x"
#elif SIZEOF_INT == 2
  #define SCANF_INT_STRING "%4x"
#elif SIZEOF_INT == 4
  #define SCANF_INT_STRING "%8x"
#elif SIZEOF_INT == 8
  #define SCANF_INT_STRING "%8x"
#endif

/** convert big endian textual number representation into little endian uint8_t string of specified size */
void bigEndianHexNumberText2CanString( const char* rc_src, uint8_t* pui8_target, unsigned int size )
{
  if ( NULL == rc_src ){memset( pui8_target, 0, size ); return;}
  unsigned int temp;

  const unsigned int inputLen = strlen(rc_src);
  uint8_t* pui8_write = pui8_target;

  int ind = inputLen - ( 2 * SIZEOF_INT );
  for ( ; ind >= 0; ind -= ( 2 * SIZEOF_INT ) )
  {
    sscanf( (rc_src+ind), SCANF_INT_STRING, &temp );
    int2littleEndianString( temp, pui8_write, SIZEOF_INT );
    pui8_write += SIZEOF_INT;
    if ( ( pui8_write - pui8_target ) > size ) break;
  }
  if ( ( ind < 0 ) && ( ind  > ( -1 * ( 2 * SIZEOF_INT ) ) ) )
  {
    unsigned ci_overhandByte = ind + ( 2 * SIZEOF_INT );
    switch ( ci_overhandByte )
    {
      case 1:  case 2:  sscanf( rc_src, "%2x",  &temp );   break;
      case 3:  case 4:  sscanf( rc_src, "%4x",  &temp );   break;
      case 5:  case 6:  sscanf( rc_src, "%6x",  &temp );   break;
      case 7:  case 8:  sscanf( rc_src, "%8x",  &temp );   break;
      case 9:  case 10: sscanf( rc_src, "%10x", &temp );   break;
      case 11: case 12: sscanf( rc_src, "%12x", &temp );   break;
    }
    unsigned int availableSize = size - ( pui8_write - pui8_target );
    if ( ci_overhandByte < availableSize ) availableSize = ci_overhandByte;
    int2littleEndianString( temp, pui8_write, availableSize );
  }
}

#if SIZEOF_INT <= 2
  #define SCANF_HEX_INT16_STRING "%4x"
#else
  #define SCANF_HEX_INT16_STRING "%4hx"
#endif

/** convert big endian textual unsigned int 8Bit number representation into little endian uint8_t string */
void bigEndianHexNumberText2CanStringUint8( const char* rc_src, uint8_t* pui8_target )
{
  if ( NULL == rc_src ){memset( pui8_target, 0, 1 ); return;}
  unsigned int temp;
  sscanf( rc_src, "%2x",  &temp );
  pui8_target[0] = ( temp & 0xFF );
}
/** convert big endian textual unsigned int 16Bit number representation into little endian uint8_t string of specified size */
void bigEndianHexNumberText2CanStringUint16( const char* rc_src, uint8_t* pui8_target )
{
  if ( NULL == rc_src ){memset( pui8_target, 0, 2 ); return;}
#if SIZEOF_INT <= 4
  uint16_t temp;
  sscanf( rc_src, SCANF_HEX_INT16_STRING, &temp );
  #else
  unsigned int temp;
  sscanf( rc_src, "%4x",  &temp );
  #endif
  #ifdef OPTIMIZE_NUMBER_CONVERSIONS_FOR_LITTLE_ENDIAN
  memcpy( pui8_target,   &temp, 2 );
  #else
  pui8_target[0] =   ( temp        & 0xFF );
  pui8_target[1] = ( ( temp >> 8 ) & 0xFF );
  #endif
}
/** convert big endian textual unsigned int 32Bit number representation into little endian uint8_t string of specified size */
void bigEndianHexNumberText2CanStringUint32( const char* rc_src, uint8_t* pui8_target )
{
  if ( NULL == rc_src ){memset( pui8_target, 0, 4 ); return;}
#if SIZEOF_INT <= 2
  uint32_t temp;
  sscanf( rc_src, "%8lx", &temp );
  #elif SIZEOF_INT == 4
  uint32_t temp;
  sscanf( rc_src, "%8x", &temp );
  #else
  unsigned int temp;
  sscanf( rc_src, "%8x", &temp );
  #endif
  #ifdef OPTIMIZE_NUMBER_CONVERSIONS_FOR_LITTLE_ENDIAN
  memcpy( pui8_target,   &temp, 4 );
  #else
  pui8_target[0] =   ( temp         & 0xFF );
  pui8_target[1] = ( ( temp >> 8  ) & 0xFF );
  pui8_target[1] = ( ( temp >> 16 ) & 0xFF );
  pui8_target[3] = ( ( temp >> 24 ) & 0xFF );
  #endif
}
/** convert big endian textual unsigned int 64Bit number representation into little endian uint8_t string of specified size */
void bigEndianHexNumberText2CanStringUint64( const char* rc_src, uint8_t* pui8_target )
{
  if ( NULL == rc_src ){memset( pui8_target, 0, 8 ); return;}
#if SIZEOF_INT <= 2
  uint32_t temp[2] = {0UL, 0UL};
  const unsigned int len = strlen( rc_src );
  const int lowerPartValStart = len - 8;
  if ( lowerPartValStart >= 0 )
  {
    sscanf( rc_src+lowerPartValStart, "%8lx", &(temp[0]) );
    switch ( lowerPartValStart )
    {
      case 0: break;
      case 1: sscanf( rc_src, "%1lx", &(temp[1]) ); break;
      case 2: sscanf( rc_src, "%2lx", &(temp[1]) ); break;
      case 3: sscanf( rc_src, "%3lx", &(temp[1]) ); break;
      case 4: sscanf( rc_src, "%4lx", &(temp[1]) ); break;
      case 5: sscanf( rc_src, "%5lx", &(temp[1]) ); break;
      case 6: sscanf( rc_src, "%6lx", &(temp[1]) ); break;
      case 7: sscanf( rc_src, "%7lx", &(temp[1]) ); break;
      case 8: sscanf( rc_src, "%8lx", &(temp[1]) ); break;
    }
  }
  else
  { // source string contains only digits for lower 4-byte value
    sscanf( rc_src, "%8lx", &(temp[0]) );
  }
  #ifdef OPTIMIZE_NUMBER_CONVERSIONS_FOR_LITTLE_ENDIAN
  memcpy( pui8_target,   &(temp[0]), 4 );
  memcpy( pui8_target+4, &(temp[1]), 4 );
  #else
  for ( unsigned int ind = 0; ind < 2; ind++ )
  {
    pui8_target[(ind*4)]   =   ( temp[ind]         & 0xFF );
    pui8_target[(ind*4)+1] = ( ( temp[ind] >> 8  ) & 0xFF );
    pui8_target[(ind*4)+2] = ( ( temp[ind] >> 16 ) & 0xFF );
    pui8_target[(ind*4)+3] = ( ( temp[ind] >> 24 ) & 0xFF );
  }
  #endif
#elif SIZEOF_INT == 4
  uint64_t temp;
#ifdef SYSTEM_PC_VC
  sscanf( rc_src, "%16I64x", &temp );
#else
  sscanf( rc_src, "%16llx", &temp );
#endif
  #ifdef OPTIMIZE_NUMBER_CONVERSIONS_FOR_LITTLE_ENDIAN
  memcpy( pui8_target, &temp, 8 );
  #else
  for ( unsigned int ind = 0; ind < 8; ind++ ) pui8_target[ind] = ( ( temp >> (ind*8) ) & 0xFF );
  #endif
#else
  unsigned int temp;
  sscanf( rc_src, "%16x", &temp );
  #ifdef OPTIMIZE_NUMBER_CONVERSIONS_FOR_LITTLE_ENDIAN
  memcpy( pui8_target, &temp, 8 );
  #else
  for ( unsigned int ind = 0; ind < 8; ind++ ) pui8_target[ind] = ( ( temp >> (ind*8) ) & 0xFF );
  #endif
#endif
}

#if SIZEOF_INT <= 2
#define SCANF_DEC_INT16_STRING "%4d"
#else
#define SCANF_DEC_INT16_STRING "%4hd"
#endif

/** convert big endian textual unsigned int up to 16Bit number representation into little endian uint8_t string of specified size */
void bigEndianDecNumberText2CanStringUint( const char* rc_src, uint8_t* pui8_target )
{
  if ( NULL == rc_src ){memset( pui8_target, 0, 2 ); return;}
#if SIZEOF_INT <= 4
  uint16_t temp;
  sscanf( rc_src, SCANF_DEC_INT16_STRING, &temp );
#else
  unsigned int temp;
  sscanf( rc_src, "%4d",  &temp );
#endif
  #ifdef OPTIMIZE_NUMBER_CONVERSIONS_FOR_LITTLE_ENDIAN
  memcpy( pui8_target, &temp, 2 );
  #else
  pui8_target[0] =   ( temp        & 0xFF );
  pui8_target[1] = ( ( temp >> 8 ) & 0xFF );
  #endif
}

} // end of namespace __IsoAgLib
