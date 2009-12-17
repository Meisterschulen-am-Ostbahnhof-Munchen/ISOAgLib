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

#include <bitset>

//! Example:
//! 11 bit
//! gets put into 2 bytes counted from 0 starting at the outmost left:
//! 0123 4567  89Ax xxxx
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
    std::bitset<T::number_of_bits> m_bitField;
};

#endif
