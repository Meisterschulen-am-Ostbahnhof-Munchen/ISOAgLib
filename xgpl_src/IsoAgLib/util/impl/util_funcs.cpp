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




#ifdef USE_VT_UNICODE_SUPPORT

#define DEF_uR DEF_UNICODE_REPLACEMENT_CHAR
// latin-1 (west)
// gets directly mapped to unicode, no lookup-table needed

const uint16_t p96ui16_8859_2 [96] =
{ // latin-2 (east)
/*  0x0080,0x0081,0x0082,0x0083,0x0084,0x0085,0x0086,0x0087,0x0088,0x0089,0x008a,0x008b,0x008c,0x008d,0x008e,0x008f,
  0x0090,0x0091,0x0092,0x0093,0x0094,0x0095,0x0096,0x0097,0x0098,0x0099,0x009a,0x009b,0x009c,0x009d,0x009e,0x009f,*/
  0x00a0,0x0104,0x02d8,0x0141,0x00a4,0x013d,0x015a,0x00a7,0x00a8,0x0160,0x015e,0x0164,0x0179,0x00ad,0x017d,0x017b,
  0x00b0,0x0105,0x02db,0x0142,0x00b4,0x013e,0x015b,0x02c7,0x00b8,0x0161,0x015f,0x0165,0x017a,0x02dd,0x017e,0x017c,
  0x0154,0x00c1,0x00c2,0x0102,0x00c4,0x0139,0x0106,0x00c7,0x010c,0x00c9,0x0118,0x00cb,0x011a,0x00cd,0x00ce,0x010e,
  0x0110,0x0143,0x0147,0x00d3,0x00d4,0x0150,0x00d6,0x00d7,0x0158,0x016e,0x00da,0x0170,0x00dc,0x00dd,0x0162,0x00df,
  0x0155,0x00e1,0x00e2,0x0103,0x00e4,0x013a,0x0107,0x00e7,0x010d,0x00e9,0x0119,0x00eb,0x011b,0x00ed,0x00ee,0x010f,
  0x0111,0x0144,0x0148,0x00f3,0x00f4,0x0151,0x00f6,0x00f7,0x0159,0x016f,0x00fa,0x0171,0x00fc,0x00fd,0x0163,0x02d9
};

const uint16_t p96ui16_8859_4 [96] =
{ // latin-4 (north)
//0x0080,0x0081,0x0082,0x0083,0x0084,0x0085,0x0086,0x0087,0x0088,0x0089,0x008a,0x008b,0x008c,0x008d,0x008e,0x008f,
//0x0090,0x0091,0x0092,0x0093,0x0094,0x0095,0x0096,0x0097,0x0098,0x0099,0x009a,0x009b,0x009c,0x009d,0x009e,0x009f,
  0x00a0,0x0104,0x0138,0x0156,0x00a4,0x0128,0x013b,0x00a7,0x00a8,0x0160,0x0112,0x0122,0x0166,0x00ad,0x017d,0x00af,
  0x00b0,0x0105,0x02db,0x0157,0x00b4,0x0129,0x013c,0x02c7,0x00b8,0x0161,0x0113,0x0123,0x0167,0x014a,0x017e,0x014b,
  0x0100,0x00c1,0x00c2,0x00c3,0x00c4,0x00c5,0x00c6,0x012e,0x010c,0x00c9,0x0118,0x00cb,0x0116,0x00cd,0x00ce,0x012a,
  0x0110,0x0145,0x014c,0x0136,0x00d4,0x00d5,0x00d6,0x00d7,0x00d8,0x0172,0x00da,0x00db,0x00dc,0x0168,0x016a,0x00df,
  0x0101,0x00e1,0x00e2,0x00e3,0x00e4,0x00e5,0x00e6,0x012f,0x010d,0x00e9,0x0119,0x00eb,0x0117,0x00ed,0x00ee,0x012b,
  0x0111,0x0146,0x014d,0x0137,0x00f4,0x00f5,0x00f6,0x00f7,0x00f8,0x0173,0x00fa,0x00fb,0x00fc,0x0169,0x016b,0x02d9
};

const uint16_t p96ui16_8859_5 [96] =
{ // cyrillic
//0x0080,0x0081,0x0082,0x0083,0x0084,0x0085,0x0086,0x0087,0x0088,0x0089,0x008a,0x008b,0x008c,0x008d,0x008e,0x008f,
//0x0090,0x0091,0x0092,0x0093,0x0094,0x0095,0x0096,0x0097,0x0098,0x0099,0x009a,0x009b,0x009c,0x009d,0x009e,0x009f,
  0x00a0,0x0401,0x0402,0x0403,0x0404,0x0405,0x0406,0x0407,0x0408,0x0409,0x040a,0x040b,0x040c,0x00ad,0x040e,0x040f,
  0x0410,0x0411,0x0412,0x0413,0x0414,0x0415,0x0416,0x0417,0x0418,0x0419,0x041a,0x041b,0x041c,0x041d,0x041e,0x041f,
  0x0420,0x0421,0x0422,0x0423,0x0424,0x0425,0x0426,0x0427,0x0428,0x0429,0x042a,0x042b,0x042c,0x042d,0x042e,0x042f,
  0x0430,0x0431,0x0432,0x0433,0x0434,0x0435,0x0436,0x0437,0x0438,0x0439,0x043a,0x043b,0x043c,0x043d,0x043e,0x043f,
  0x0440,0x0441,0x0442,0x0443,0x0444,0x0445,0x0446,0x0447,0x0448,0x0449,0x044a,0x044b,0x044c,0x044d,0x044e,0x044f,
  0x2116,0x0451,0x0452,0x0453,0x0454,0x0455,0x0456,0x0457,0x0458,0x0459,0x045a,0x045b,0x045c,0x00a7,0x045e,0x045f
};

const uint16_t p96ui16_8859_7 [96] =
{ // greek
//0x0080,0x0081,0x0082,0x0083,0x0084,0x0085,0x0086,0x0087,0x0088,0x0089,0x008a,0x008b,0x008c,0x008d,0x008e,0x008f,
//0x0090,0x0091,0x0092,0x0093,0x0094,0x0095,0x0096,0x0097,0x0098,0x0099,0x009a,0x009b,0x009c,0x009d,0x009e,0x009f,
  0x00a0,0x2018,0x2019,0x00a3,DEF_uR,DEF_uR,0x00a6,0x00a7,0x00a8,0x00a9,DEF_uR,0x00ab,0x00ac,0x00ad,DEF_uR,0x2015,
  0x00b0,0x00b1,0x00b2,0x00b3,0x0384,0x0385,0x0386,0x00b7,0x0388,0x0389,0x038a,0x00bb,0x038c,0x00bd,0x038e,0x038f,
  0x0390,0x0391,0x0392,0x0393,0x0394,0x0395,0x0396,0x0397,0x0398,0x0399,0x039a,0x039b,0x039c,0x039d,0x039e,0x039f,
  0x03a0,0x03a1,DEF_uR,0x03a3,0x03a4,0x03a5,0x03a6,0x03a7,0x03a8,0x03a9,0x03aa,0x03ab,0x03ac,0x03ad,0x03ae,0x03af,
  0x03b0,0x03b1,0x03b2,0x03b3,0x03b4,0x03b5,0x03b6,0x03b7,0x03b8,0x03b9,0x03ba,0x03bb,0x03bc,0x03bd,0x03be,0x03bf,
  0x03c0,0x03c1,0x03c2,0x03c3,0x03c4,0x03c5,0x03c6,0x03c7,0x03c8,0x03c9,0x03ca,0x03cb,0x03cc,0x03cd,0x03ce,DEF_uR
};

const uint16_t p96ui16_8859_15 [96] =
{ // latin-9 (euro)
//0x0080,0x0081,0x0082,0x0083,0x0084,0x0085,0x0086,0x0087,0x0088,0x0089,0x008a,0x008b,0x008c,0x008d,0x008e,0x008f,
//0x0090,0x0091,0x0092,0x0093,0x0094,0x0095,0x0096,0x0097,0x0098,0x0099,0x009a,0x009b,0x009c,0x009d,0x009e,0x009f,
  0x00a0,0x00a1,0x00a2,0x00a3,0x20ac,0x00a5,0x0160,0x00a7,0x0161,0x00a9,0x00aa,0x00ab,0x00ac,0x00ad,0x00ae,0x00af,
  0x00b0,0x00b1,0x00b2,0x00b3,0x017d,0x00b5,0x00b6,0x00b7,0x017e,0x00b9,0x00ba,0x00bb,0x0152,0x0153,0x0178,0x00bf,
  0x00c0,0x00c1,0x00c2,0x00c3,0x00c4,0x00c5,0x00c6,0x00c7,0x00c8,0x00c9,0x00ca,0x00cb,0x00cc,0x00cd,0x00ce,0x00cf,
  0x00d0,0x00d1,0x00d2,0x00d3,0x00d4,0x00d5,0x00d6,0x00d7,0x00d8,0x00d9,0x00da,0x00db,0x00dc,0x00dd,0x00de,0x00df,
  0x00e0,0x00e1,0x00e2,0x00e3,0x00e4,0x00e5,0x00e6,0x00e7,0x00e8,0x00e9,0x00ea,0x00eb,0x00ec,0x00ed,0x00ee,0x00ef,
  0x00f0,0x00f1,0x00f2,0x00f3,0x00f4,0x00f5,0x00f6,0x00f7,0x00f8,0x00f9,0x00fa,0x00fb,0x00fc,0x00fd,0x00fe,0x00ff
};

const uint16_t* pp96ui16_8859s [8] =
{
  /* 0 */ NULL, // would be "p96ui16_8859_1", which is directly mapped to unicode - so no table lookup needed here! see special handling in code! */
  /* 1 */ p96ui16_8859_15,
  /* 2 */ p96ui16_8859_2,
  /* 3 */ NULL, // p96ui16_8859_3,
  /* 4 */ p96ui16_8859_4,
  /* 5 */ p96ui16_8859_5,
  /* 6 */ NULL, // p96ui16_8859_6,
  /* 7 */ p96ui16_8859_7,
};


void convertString8859ToUnicode (const std::string& rrefstr_iso8859, std::string& rrefstr_unicode, uint8_t rui8_encoding)
{
  std::string::const_iterator cit_str8859;
  rrefstr_unicode.clear ();
  for (cit_str8859=rrefstr_iso8859.begin(); cit_str8859 != rrefstr_iso8859.end(); cit_str8859++)
  {
    uint8_t byte = *cit_str8859;
    const uint16_t cui16_unicode = __IsoAgLib::convert8859ToUnicode (byte, rui8_encoding);
    __IsoAgLib::push_backUTF8 (rrefstr_unicode, cui16_unicode);
  }
}



void convertStringUnicodeTo8859 (const char* rpui8_stringUnicode, uint16_t rui16_stringUnicodeLength, char* rpui8_iso8859buffer, uint8_t rui8_encoding)
{
  int i_more=0;
  uint32_t ui32_unicode;
  for (int i=0; i<rui16_stringUnicodeLength; i++)
  {
    const uint8_t cui8_byte = *rpui8_stringUnicode++;
    if ((cui8_byte & 0xC0) == 0x80) // check for highest 2 bits to be "10xx xxxx"
    { // continuation byte
      if (i_more > 0)
      { // yes, we did have something to continue - okay.
        ui32_unicode <<= 6;
        ui32_unicode |= cui8_byte & 0x3F;
        i_more--;
        if (i_more == 0)
        { // yes, that was the last continuation - reverse lookup and store!
          if (ui32_unicode < 0x10000)
          {
            *rpui8_iso8859buffer++ = convertUnicodeTo8859 (ui32_unicode, rui8_encoding);
          }
          else // we don't support such characters, so in case they occur, we put an ISO8859-Replacement-Char there!
          {
            *rpui8_iso8859buffer++ = DEF_ISO8859_REPLACEMENT_CHAR;
          }
        }
      }
      else break; // error - continuation when NOT expected!
    }
    else
    { // no continuation, is it ASCII or MULTI-BYTE BEGIN?
      if (i_more > 0) break; // error - expected CONTINUATION, but got other!

      if (cui8_byte < 0x80)
      { // ASCII, take unconverted
        *rpui8_iso8859buffer++ = cui8_byte;
        continue;
        // i_more is still 0, so the value will be taken below.
      }
      else if ((cui8_byte & 0xE0) == 0xC0)
      { // expect one continuation byte
        ui32_unicode = cui8_byte & 0x1F;
        i_more = 1;
      }
      else if ((cui8_byte & 0xF0) == 0xE0)
      { // expect two continuation bytes
        ui32_unicode = cui8_byte & 0x0F;
        i_more = 2;
      }
      else if ((cui8_byte & 0xF8) == 0xF0)
      { // expect three continuation bytes
        ui32_unicode = cui8_byte & 0x07;
        i_more = 3;
      }
      else if ((cui8_byte & 0xFC) == 0xF8)
      { // expect four continuation bytes
        ui32_unicode = cui8_byte & 0x03;
        i_more = 4;
      }
      else if ((cui8_byte & 0xFE) == 0xFC)
      { // expect five continuation bytes
        ui32_unicode = cui8_byte & 0x01;
        i_more = 5;
      }
    }
  }
  *rpui8_iso8859buffer = 0x00;
}


// only supports 2-byte UTF-8 encodings, but this should be enough for us...
void push_backUTF8 (std::string& rrefstr_string, uint16_t rui16_unicode)
{
  if (rui16_unicode < 0x80)
  {
    rrefstr_string.push_back (uint8_t (rui16_unicode));
  }
  else if (rui16_unicode < 0x800)
  {
    rrefstr_string.push_back (uint8_t (0xC0 | ((rui16_unicode >> 6)       ))); // 110kjihg - no need to AND, because it's < 0x800 !
    rrefstr_string.push_back (uint8_t (0x80 | ((rui16_unicode     ) & 0x3F))); // 10fedcba
  }
  else
  {
    rrefstr_string.push_back (uint8_t (0xE0 | ((rui16_unicode >>12)       ))); // 110kjihg - no need to AND, because it's < 0x800 !
    rrefstr_string.push_back (uint8_t (0x80 | ((rui16_unicode >> 6) & 0x3F))); // 10fedcba
    rrefstr_string.push_back (uint8_t (0x80 | ((rui16_unicode     ) & 0x3F))); // 10fedcba
  }
}


#endif



} // end of namespace __IsoAgLib
