/***************************************************************************
														bitfieldwrapper_c.h -
                             -------------------
		class                : ::BitFieldWrapper_c
    begin                : Mai 2009
    copyright            : (C) 2009 by Nizar Souissi (n.souissi@osb-ag.de)
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

//! Example:
//! 11 bit
//! gets put into 2 bytes counted from 0 starting at the outmost left:
//! 0123 4567  89Ax xxxx
template <class T>
class BitFieldWrapper_c
{
  public:
    /** Constructor */
    BitFieldWrapper_c ( uint16_t aui16_size ) :
        mui16_sizeInBits ( aui16_size ),
        mui8_sizeInBytes ( ( aui16_size+7 ) >> 3 )
    {
      mpui8_bitField = new uint8_t[mui8_sizeInBytes];
      for ( int i = 0; i < mui8_sizeInBytes; i++ )
        mpui8_bitField [i] = 0;
    }

    /** Destructor */
    ~BitFieldWrapper_c()
    {
      delete[] mpui8_bitField;
    }


    /** Set the given bit to 1. Bits are counted from the left
    		@param  a_bitsFromTheLeft  bit position from the left, counting from 0
        @return  reference to BitFieldWrapper_c
      */
    BitFieldWrapper_c& setBit ( T a_bitsFromTheLeft )
    {
      if ( a_bitsFromTheLeft < mui16_sizeInBits )
      {
        mpui8_bitField[ a_bitsFromTheLeft >> 3 ] |= ( 0x80 >> ( a_bitsFromTheLeft & 0x07 ) );
      }
      return *this;
    }

#if 0
		// This function is unused rigth at the moment. One still needs to decide whether or not to set the unused bits
    /** set all object types */
    BitFieldWrapper_c& setAllBits()
    {
      for ( int i = 0; i < mui8_sizeInBytes; i++ )
        mpui8_bitField [i] = 0xFF;
      return *this;
    }
#endif

    /** Checks wheether the given bit is set to 1. Bits are counted from the left
    @param  a_bitsFromTheLeft  bit position from the left, counting from 0
    @return  true if the bit is set
    */
    bool isBitSet ( T a_bitsFromTheLeft )
    {
      if ( a_bitsFromTheLeft < mui16_sizeInBits )
      {
        if ( mpui8_bitField[ a_bitsFromTheLeft >> 3 ] & ( 0x80 >> ( a_bitsFromTheLeft & 0x07 ) ) )
          return true;
      }
      return false;
    }

    /** do bitwise AND assignment for each uint8_t of array */
    void operator&= ( const BitFieldWrapper_c& c_refBitField )
    {
      uint8_t minSize = ( mui8_sizeInBytes < c_refBitField.mui8_sizeInBytes ) ? mui8_sizeInBytes : c_refBitField.mui8_sizeInBytes  ;

      for ( int i = 0; i < minSize; i++ )
        mpui8_bitField[i] &= c_refBitField.mpui8_bitField[i];
    }

		/** A bytewise getter
		*/
    uint8_t getByte ( uint8_t aui8_byteOrder ) const
    {
      if ( aui8_byteOrder <= mui8_sizeInBytes )
      {
        return mpui8_bitField[aui8_byteOrder];
      }
      return 0;
    }

  private:
    uint8_t* mpui8_bitField;
    uint16_t mui16_sizeInBits;
    uint8_t  mui8_sizeInBytes;

}; // ~X2C

#endif // -X2C
