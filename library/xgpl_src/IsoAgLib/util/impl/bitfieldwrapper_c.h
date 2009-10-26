/***************************************************************************
                             bitfieldwrapper_c.h
                             -------------------
    class                : ::BitFieldWrapper_c
    begin                : Mai 2009
    authors              : Nizar Souissi (n.souissi@osb-ag.de)
                           Christophe Henry (c.henry@osb-ag.de)
    $Log$
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
* Copyright (C) 1999 - 2009 Dipl.-Inform. Achim Spangler                  *
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
