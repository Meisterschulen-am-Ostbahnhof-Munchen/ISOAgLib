/***************************************************************************
                          iutil_funcs.h  -  utils functions
                             -------------------
    begin                : Tue Aug 3 1999
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
#ifndef IUTIL_FUNCS_H
#define IUTIL_FUNCS_H

#include "impl/util_funcs.h"
#ifdef USE_DATASTREAMS_IO
#include <IsoAgLib/comm/Part3_DataLink/impl/stream_c.h>
#endif

// Begin Namespace IsoAgLib
namespace IsoAgLib {

inline int32_t mul1Div1Mul2Div2(int32_t ai32_mul_1, int32_t ai32_div_1, int32_t ai32_mul_2, int32_t ai32_div_2)
{ return __IsoAgLib::mul1Div1Mul2Div2(ai32_mul_1, ai32_div_1, ai32_mul_2, ai32_div_2);}

/**
  copy 4 uint8_t data string into pointer to float value;
  use e.g. to access uint8_t memory representation of
  int32_t value as float value;
  often needed for CanPkg_c formating
  @param pvFrom source data string
  @param pf_to target float value
*/
inline void littleEndianStream2FloatVar(const void *const pvFrom, float *const pf_to) { __IsoAgLib::littleEndianStream2FloatVar( pvFrom, pf_to ); }

/**
  copy float value to 4 uint8_t data string into pointer;
  use e.g. to access uint8_t memory representation of
  float value as int32_t value;
  often needed for CanPkg_c formating
  @param pf_from source float value
  @param pvTo target data string
*/
inline void floatVar2LittleEndianStream(const float *const pf_from, void *const pvTo) { __IsoAgLib::floatVar2LittleEndianStream( pf_from, pvTo );}


/** calculate the total allocated HEAP for:
  - slist<T> with given size of T
  - add the overhead per node for slist<T> ( pointer to next item )
  - add the overhead for malloc_alloc Allocator which calls malloc for each single node ( HEAP block pointer )
  - add the overhead for alignment based on sizeof(int)
  @param aui16_sizeT sizeof(T) -> size of the stored class
  @param aui16_cnt amount of items ( default: 1 )
  @return amount of corresponding byte in heap
*/
inline uint16_t sizeSlistTWithMalloc( uint16_t aui16_sizeT, uint16_t aui16_cnt = 1 )
  { return __IsoAgLib::sizeSlistTWithMalloc( aui16_sizeT, aui16_cnt );}

/** calculate the total allocated HEAP for:
  - list<T> with given size of T
  - add the overhead per node for list<T> ( TWO pointer to next and prev item )
  - add the overhead for malloc_alloc Allocator which calls malloc for each single node ( HEAP block pointer )
  - add the overhead for alignment based on sizeof(int)
  @param aui16_sizeT sizeof(T) -> size of the stored class
  @param aui16_cnt amount of items ( default: 1 )
  @return amount of corresponding byte in heap
*/
inline uint16_t sizeListTWithMalloc( uint16_t aui16_sizeT, uint16_t aui16_cnt = 1 )
  { return __IsoAgLib::sizeListTWithMalloc( aui16_sizeT, aui16_cnt );}

/** calculate the total allocated HEAP for:
  - vector<T> with given size of T
  - add the overhead for malloc_alloc Allocator which calls malloc for each vector instance ( HEAP block pointer )
  - add the overhead for alignment based on sizeof(int)
  @param aui16_sizeT sizeof(T) -> size of the stored class
  @param aui16_capacity reserved space for vector<T> ( >= amount of currently stored items )
  @return amount of corresponding byte in heap
*/
inline uint16_t sizeVectorTWithMalloc( uint16_t aui16_sizeT, uint16_t aui16_capacity )
  { return __IsoAgLib::sizeVectorTWithMalloc( aui16_sizeT, aui16_capacity );}

/** calculate the total allocated HEAP for:
  - slist<T> with given size of T
  - add the overhead per node for slist<T> ( pointer to next item )
  - add the overhead caused by allocation large chunks of each 40 items
  - add overhead for linking the HEAP block by the lowloevel malloc
  - add the overhead for alignment based on sizeof(int)
  @param aui16_sizeT sizeof(T) -> size of the stored class
  @param aui16_cnt amount of items ( default: 1 )
  @return amount of corresponding byte in heap
*/
inline uint16_t sizeSlistTWithChunk( uint16_t aui16_sizeT, uint16_t aui16_cnt = 1 )
  { return __IsoAgLib::sizeSlistTWithChunk( aui16_sizeT, aui16_cnt );}

/** calculate the total allocated HEAP for:
  - list<T> with given size of T
  - add the overhead per node for slist<T> ( pointers to next+prev item )
  - add the overhead caused by allocation large chunks of each 40 items
  - add overhead for linking the HEAP block by the lowloevel malloc
  - add the overhead for alignment based on sizeof(int)
  @param aui16_sizeT sizeof(T) -> size of the stored class
  @param aui16_cnt amount of items ( default: 1 )
  @return amount of corresponding byte in heap
*/
inline uint16_t sizeListTWithChunk( uint16_t aui16_sizeT, uint16_t aui16_cnt = 1 )
  { return __IsoAgLib::sizeListTWithChunk( aui16_sizeT, aui16_cnt );}

/** calculate the total allocated HEAP for:
  - vector<T> with given size of T
  - add the overhead for malloc_alloc Allocator which calls malloc for each vector instance ( HEAP block pointer )
  - add the overhead for alignment based on sizeof(int)
  @param aui16_sizeT sizeof(T) -> size of the stored class
  @param aui16_capacity reserved space for vector<T> ( >= amount of currently stored items )
  @return amount of corresponding byte in heap
*/
inline uint16_t sizeVectorTWithChunk( uint16_t aui16_sizeT, uint16_t aui16_capacity )
  { return __IsoAgLib::sizeVectorTWithChunk( aui16_sizeT, aui16_capacity );}

#ifdef USE_DATASTREAMS_IO
/** convert receive multistream into an unsigned variable */
template<class T> bool convertIstream( StreamInput_c& rc_stream, T& r_result )
{
  const unsigned int size = sizeof(T);
#ifdef OPTIMIZE_NUMBER_CONVERSIONS_FOR_LITTLE_ENDIAN
  uint8_t* pui8_writePointer = (uint8_t*)(&r_result);
  unsigned int ind = 0;
  for ( ; ( ( ind < size ) && ( !rc_stream.eof() ) ); ind++ )
  {
    rc_stream >> pui8_writePointer[ind];
  }
#else
  uint8_t ui8_temp;
  rc_stream >> ui8_temp;
  r_result = T(ui8_temp);
  unsigned int ind = 1;
  for ( ; ( ( ind < sizeof(T) ) && ( !rc_stream.eof() ) ); ind++ )
  {
    rc_stream >> ui8_temp;
    r_result |= (T(ui8_temp) << (8*ind));
  }
#endif
  if ( ind == size ) return true;  // all result bytes were read
  else return false;  // less bytes than inside the result type transferred
}

/** convert receive multistream into an unsigned variable */
inline uint8_t convertIstreamUi8( StreamInput_c& rc_stream )
{ return rc_stream.get(); }

/** convert receive multistream into an unsigned variable */
inline int8_t convertIstreamI8( StreamInput_c& rc_stream )
{ return (int8_t)rc_stream.get(); }

/** convert receive multistream into an unsigned variable */
inline uint16_t convertIstreamUi16( StreamInput_c& rc_stream )
{ return __IsoAgLib::convertIstreamUi16( rc_stream ); }

/** convert receive multistream into an unsigned variable */
inline int16_t convertIstreamI16( StreamInput_c& rc_stream )
{ return __IsoAgLib::convertIstreamI16( rc_stream ); }

/** convert receive multistream into an unsigned variable */
inline uint32_t convertIstreamUi32( StreamInput_c& rc_stream )
{ return __IsoAgLib::convertIstreamUi32( rc_stream ); }

/** convert receive multistream into an unsigned variable */
inline int32_t convertIstreamI32( StreamInput_c& rc_stream )
{ return __IsoAgLib::convertIstreamI32( rc_stream ); }

#endif

/** convert big endian textual number representation into little endian uint8_t string of specified size */
inline void bigEndianHexNumberText2CanString( const char* ac_src, uint8_t* pui8_target, unsigned int size )
{ __IsoAgLib::bigEndianHexNumberText2CanString( ac_src, pui8_target, size );}

/** convert big endian textual unsigned int 8Bit number representation into little endian uint8_t string */
inline void bigEndianHexNumberText2CanStringUint8( const char* ac_src, uint8_t* pui8_target )
{ __IsoAgLib::bigEndianHexNumberText2CanStringUint8( ac_src, pui8_target );}

/** convert big endian textual unsigned int 16Bit number representation into little endian uint8_t string of specified size */
inline void bigEndianHexNumberText2CanStringUint16( const char* ac_src, uint8_t* pui8_target )
{ __IsoAgLib::bigEndianHexNumberText2CanStringUint16( ac_src, pui8_target );}

/** convert big endian textual unsigned int 32Bit number representation into little endian uint8_t string of specified size */
inline void bigEndianHexNumberText2CanStringUint32( const char* ac_src, uint8_t* pui8_target )
{ __IsoAgLib::bigEndianHexNumberText2CanStringUint32( ac_src, pui8_target );}

/** convert big endian textual unsigned int 64Bit number representation into little endian uint8_t string of specified size */
inline void bigEndianHexNumberText2CanStringUint64( const char* ac_src, uint8_t* pui8_target )
{ __IsoAgLib::bigEndianHexNumberText2CanStringUint64( ac_src, pui8_target );}

/** convert big endian textual unsigned int up to 16Bit number representation into little endian uint8_t string of specified size */
inline void bigEndianDecNumberText2CanStringUint( const char* ac_src, uint8_t* pui8_target )
{ __IsoAgLib::bigEndianDecNumberText2CanStringUint( ac_src, pui8_target );}


#ifdef USE_VT_UNICODE_SUPPORT

inline uint16_t convert8859ToUnicode (uint8_t aui8_char, uint8_t aui8_encoding)
{ return __IsoAgLib::convert8859ToUnicode (aui8_char, aui8_encoding); }

inline uint8_t convertUnicodeTo8859 (uint16_t aui16_unicode, uint8_t aui8_encoding)
{ return __IsoAgLib::convertUnicodeTo8859 (aui16_unicode, aui8_encoding); }

inline void convertString8859ToUnicode (const STL_NAMESPACE::string& rrefstr_iso8859, STL_NAMESPACE::string& rrefstr_unicode, uint8_t aui8_encoding)
{ __IsoAgLib::convertString8859ToUnicode (rrefstr_iso8859, rrefstr_unicode, aui8_encoding); }

inline void convertStringUnicodeTo8859 (const char* apui8_stringUnicode, uint16_t aui16_stringUnicodeLength, char* apui8_iso8859buffer, uint8_t aui8_encoding)
{ __IsoAgLib::convertStringUnicodeTo8859 (apui8_stringUnicode, aui16_stringUnicodeLength, apui8_iso8859buffer, aui8_encoding); }

inline void push_backUTF8 (STL_NAMESPACE::string& rrefstr_string, uint16_t aui16_unicode)
{ push_backUTF8 (rrefstr_string, aui16_unicode); }

#endif

} // end of namespace
#endif
