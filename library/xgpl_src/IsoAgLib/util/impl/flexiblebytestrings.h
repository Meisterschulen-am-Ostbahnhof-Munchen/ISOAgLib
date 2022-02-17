/*
  flexiblebytestrings.h

  (C) Copyright 2012 - 2019 by OSB AG

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Usage under Commercial License:
  Licensees with a valid commercial license may use this file
  according to their commercial license agreement. (To obtain a
  commercial license contact OSB AG via <http://isoaglib.com/en/contact>)

  Usage under GNU General Public License with exceptions for ISOAgLib:
  Alternatively (if not holding a valid commercial license)
  use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#ifndef FLEXIBLEBYTESTRINGS_H
#define FLEXIBLEBYTESTRINGS_H

#include <IsoAgLib/isoaglib_config.h>
#include <IsoAgLib/util/iassert.h>
#include <IsoAgLib/util/impl/util_funcs.h>
#include <string.h>

namespace __IsoAgLib {

  class Flexible8ByteString_c {
    public:
      Flexible8ByteString_c( const Flexible8ByteString_c& src );
      Flexible8ByteString_c( const uint8_t* src = NULL );

      void setDataFromString( const uint8_t* src, uint8_t aui8_len = 8 ) {
        if( aui8_len == 0 )
          return;

        isoaglib_header_assert( src );
        std::memcpy( uint8, src, aui8_len );
      }

      void getDataToString( uint8_t* dest, uint8_t aui8_len = 8 ) const {
        isoaglib_header_assert( dest );
        std::memcpy( dest, uint8, aui8_len );
      }

      void setDataFromString( uint8_t aui8_offset, const uint8_t* src, uint8_t aui8_len = 8 );
      void getDataToString( uint8_t aui8_offset, uint8_t* dest, uint8_t aui8_len = 8 ) const;

      const uint8_t* getUint8DataConstPointer( uint8_t aui8_positionOffset ) const { return uint8+aui8_positionOffset; };
      const uint8_t* getUint8DataConstPointer() const { return uint8; };
      uint8_t* getUint8DataPointer( uint8_t aui8_positionOffset ) { return uint8+aui8_positionOffset; };
      uint8_t* getUint8DataPointer() { return uint8; };


      /* operator */
      const Flexible8ByteString_c& operator=( const Flexible8ByteString_c& src );
      bool operator==( const Flexible8ByteString_c& cmp ) const;
      bool operator!=( const Flexible8ByteString_c& cmp ) const;
      bool operator<( const Flexible8ByteString_c& cmp ) const;
      bool operator>( const Flexible8ByteString_c& cmp ) const;
      int compare( const Flexible8ByteString_c& cmp ) const;

      /* 8 */
      void setUint8Data( uint8_t aui8_pos, uint8_t aui8_val ) { uint8[aui8_pos] = aui8_val; };
      uint8_t getUint8Data( uint8_t aui8_pos ) const { return uint8[aui8_pos]; };
      uint8_t operator[]( uint8_t aui8_pos ) const { return uint8[aui8_pos]; };

      /* float */
      float getFloatData( uint8_t aui8_pos ) const;
      void setFloatData( uint8_t aui8_pos, const float af_val );

#if defined(OPTIMIZE_NUMBER_CONVERSIONS_FOR_LITTLE_ENDIAN) && !defined(NO_8BIT_CHAR_TYPE)
      /* 16 */
      void setUint16Data( uint8_t aui8_pos, uint16_t aui16_val );
      void setInt16Data( uint8_t aui8_pos, int16_t ai16_val );
      uint16_t getUint16Data( uint8_t aui8_pos ) const;
      int16_t getInt16Data( uint8_t aui8_pos ) const;

      /* 32 */
      void setUint32Data( uint8_t aui8_pos, uint32_t aui32_val );
      void setInt32Data( uint8_t aui8_pos, int32_t ai32_val );
      uint32_t getUint32Data( uint8_t aui8_pos ) const;
      int32_t getInt32Data( uint8_t aui8_pos ) const;

#ifdef HAS_64BIT_INT_TYPE
      /* 64 */
      void setUint64Data( uint64_t aui64_val ) {
        uint64[0] = aui64_val;
      };
      void setInt64Data( int64_t ai64_val ) {
        int64[0] = ai64_val;
      };
      uint64_t getUint64Data() const {
        return uint64[0];
      };
      int64_t getInt64Data() const {
        return int64[0];
      };
#endif

#else
      /* 16 */
      void setUint16Data( uint8_t aui8_pos, uint16_t aui16_val ) {
        numberRef2LittleEndianString( aui16_val, ( uint8+aui8_pos ) );
      };
      void setInt16Data( uint8_t aui8_pos, int16_t ai16_val ) {
        numberRef2LittleEndianString( ai16_val, ( uint8+aui8_pos ) );
      };
      uint16_t getUint16Data( uint8_t aui8_pos ) const {
        return convertLittleEndianStringUi16( uint8+aui8_pos );
      };
      int16_t getInt16Data( uint8_t aui8_pos ) const {
        return convertLittleEndianStringI16( uint8+aui8_pos );
      };

      /* 32 */
      void setUint32Data( uint8_t aui8_pos, uint32_t aui32_val ) {
        numberRef2LittleEndianString( aui32_val, ( uint8+aui8_pos ) );
      };
      void setInt32Data( uint8_t aui8_pos, int32_t ai32_val ) {
        numberRef2LittleEndianString( ai32_val, ( uint8+aui8_pos ) );
      };
      uint32_t getUint32Data( uint8_t aui8_pos ) const {
        return convertLittleEndianStringUi32( uint8+aui8_pos );
      };
      int32_t getInt32Data( uint8_t aui8_pos ) const {
        return convertLittleEndianStringI32( uint8+aui8_pos );
      };

#ifdef HAS_64BIT_INT_TYPE
      /* 64 */
      void setUint64Data( uint64_t aui64_val ) {
        numberRef2LittleEndianString( aui64_val, uint8 );
      };
      void setInt64Data( int64_t ai64_val ) {
        numberRef2LittleEndianString( ai64_val, uint8 );
      };
      uint64_t getUint64Data() const {
        return convertLittleEndianStringUi64( uint8 );
      };
      int64_t getInt64Data() const {
        return convertLittleEndianStringI64( uint8 );
      };
#endif
      
#endif

      union {
        uint8_t uint8 [8];
        int8_t int8 [8];
#ifndef NO_8BIT_CHAR_TYPE
        uint16_t uint16[4];
        int16_t int16[4];
        uint32_t uint32[2];
        int32_t int32[2];
#ifdef HAS_64BIT_INT_TYPE
        uint64_t uint64[1];
        int64_t int64[1];
#endif
#endif
      };
  };
}

#endif
