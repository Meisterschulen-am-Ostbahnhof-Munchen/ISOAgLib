/*
  bitfieldwrapper_c.h

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#ifndef BITFIELDWRAPPER_C_H
#define BITFIELDWRAPPER_C_H

// ISOAgLib
#include <IsoAgLib/util/iassert.h>

// STL
#include <vector>


template <unsigned N> class IsoaglibBitset {
  private:
    STL_NAMESPACE::vector<uint8_t> _v;
  public:
    IsoaglibBitset( void ): _v((N+7)>>3, uint8_t(0) ) {}
    IsoaglibBitset( const IsoaglibBitset& ar_src ) : _v(ar_src._v){};

    IsoaglibBitset<N>& set( void ) {
      STL_NAMESPACE::vector<uint8_t>::iterator i;
      for (i=_v.begin(); i!=_v.end(); ++i) *i = 0xFFu;
      return *this;
    }

    IsoaglibBitset<N>& reset( void ) {
      STL_NAMESPACE::vector<uint8_t>::iterator i;
      for (i=_v.begin(); i!=_v.end(); ++i) *i = 0x00u;
      return *this;
    }

    bool isBitSet(unsigned n) const {
      return ((_v[n>>3] & (uint8_t(1)<<(7-(n&0x07))))>0);
    }

    IsoaglibBitset<N>& setBit( unsigned n, int val = 1 ) {
      if (0 == val)
        _v[n>>3] &= ~(uint8_t(1)<<(7-(n&0x07)));
      else
        _v[n>>3] |=  (uint8_t(1)<<(7-(n&0x07)));
      return *this;
    }

    IsoaglibBitset<N>& reset( unsigned n ) {
      return setBit(n, 0);
    }

    unsigned count( void ) const {
      unsigned n,c=0;
      for (n=0; n<N; ++n) if (isBitSet(n)) ++c;
      return c;
    }

    uint8_t getByte(unsigned n) const {
      isoaglib_assert (n < ((N+7)>>3));
      return _v[n];
    }
};



#if 0
//! This "better" version is commented out, because the IAR-compiler does not know #include <bitset>
//!
//! Example:
//! 11 bit
//! gets put into 2 bytes counted from 0 starting at the outmost left:
//! 0123 4567  89Ax xxxx
//!
//! Example-usage:
//! struct Certification_s
//! {
//!   typedef Certification_t enum_type;
//!   enum { number_of_bits = 24 };
//! };
//! typedef BitFieldWrapper_c<Certification_s> CertificationBitMask_t;

#include <bitset>


template <class T>
class BitFieldWrapper_c
{
  private:
    enum {sizeInBits = T::number_of_bits};
    enum {sizeInBytes = ( T::number_of_bits+7 ) >> 3 };

  public:
    /** Constructor */
      BitFieldWrapper_c ():
        m_bitField()
    {
    }

    /** Destructor */
    ~BitFieldWrapper_c()
    {
    }


    /** Set the given bit to 1. Bits are counted from the left
        @param  a_bitsFromTheLeft  bit position from the left, counting from 0
        @return  reference to BitFieldWrapper_c
      */
    BitFieldWrapper_c& setBit( typename T::enum_type a_bitsFromTheLeft )
    {
      if ( static_cast<unsigned int>(a_bitsFromTheLeft) < static_cast<unsigned int>(sizeInBits) )
      {
         m_bitField.set (a_bitsFromTheLeft);
      }
      // else: Out of Range - nothing done.
      return *this;
    }

    /** set all bits - be sure to check if this is appropriate,
        or if there are any reserved bits which should not be set */
    BitFieldWrapper_c& setAllBits()
    {
      m_bitField.set();
      return *this;
    }

    /** Checks wheether the given bit is set to 1. Bits are counted from the left
    @param  a_bitsFromTheLeft  bit position from the left, counting from 0
                               if it is out-of-range, false (0 bit) is returned
    @return  true if the bit is set
    */
    bool isBitSet( typename T::enum_type a_bitsFromTheLeft )
    {
      if ( static_cast<unsigned int>(a_bitsFromTheLeft) < static_cast<unsigned int>(sizeInBits) )
      {
        return m_bitField.test (a_bitsFromTheLeft);
      }
      // else: Out of Range handling: return false.
      return false;
    }

    /** do bitwise AND assignment */
    void operator &= ( const BitFieldWrapper_c& c_refBitField )
    {
      m_bitField &= c_refBitField;
    }

    /** do bitwise OR assignment */
    void operator |= ( const BitFieldWrapper_c& c_refBitField )
    {
      m_bitField |= c_refBitField;
    }

    /** do bitwise XOR assignment */
    void operator ^= ( const BitFieldWrapper_c& c_refBitField )
    {
      m_bitField ^= c_refBitField;
    }

    /** do bitwise left shift */
    void operator <<= ( size_t num )
    { // for bitset, bit 0 is the LSB => we need to invert <<= to >>=
      m_bitField >>= num;
    }

    /** do bitwise right shift */
    void operator >>= ( size_t num )
    { // for bitset, bit 0 is the LSB => we need to invert >>= to <<=
      m_bitField <<= num;
    }

    /** A bytewise getter */
    uint8_t getByte ( uint8_t aui8_byteOffset ) const
    {
      if ( aui8_byteOffset < sizeInBytes )
      {
          uint8_t ui8_res = 0;
          for (int i=0; i<8; ++i)
          {
            if (m_bitField[aui8_byteOffset*8+i])
              ui8_res |= (1 << (7-i));
          }
          return ui8_res;
      }
      // out-of-range handling
      return 0;
    }

  private:
    STL_NAMESPACE::bitset<T::number_of_bits> m_bitField;
};
#endif

#endif
