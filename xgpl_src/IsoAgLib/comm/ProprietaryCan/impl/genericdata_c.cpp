/***************************************************************************
                          genericdata_c.cpp -
                             -------------------
    begin                : Tue Dec 14 2006
    copyright            : (C) 1999 - 2006 by Dipl.-Inform. Achim Spangler
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
 * - Instatiation of a variable with a datatype from interface nsetIdentamespace,  *
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

#include "genericdata_c.h"
#include <IsoAgLib/util/iliberr_c.h>


// Begin Namespace IsoAgLib
namespace IsoAgLib
{
  /** Constructor which has nothing to do */
  GenericData_c::GenericData_c()
  {
  }

  /** Destructor is clearing the vector */
  GenericData_c::~GenericData_c()
  {
    ClearVector();
  }

  /**
    set ident for the telegram
    @param rui32_ident ident for the telegram
  */
  void GenericData_c::setIdent(uint32_t rui32_ident)
  {
    ui32_ident = __IsoAgLib::CANPkgExt_c::ident();
  }

  /**
    @return uint32_t ident
  */
  uint32_t GenericData_c::getIdent() const
  {
    return (ui32_ident);
  }

  /** storing data
    @param rui16_pos position to store data in vector
    @param rui8_data data byte
  */
  void GenericData_c::setDataUi8( uint16_t rui16_pos, uint8_t rui8_data)
  {
    /** first check size of vector */
    CheckSizeOfVectorForWrite( rui16_pos, sizeof(rui8_data) );
    /** overloaded generic method to set the right data */
    __IsoAgLib::number2LittleEndianString( rui8_data, vec_data, rui16_pos);
  }

  /** storing data
    @param rui16_pos position to store data in vector
    @param ri8_data data byte
  */
  void GenericData_c::setDataI8( uint16_t rui16_pos, int8_t ri8_data)
  {
      /** first check size of vector */
    CheckSizeOfVectorForWrite( rui16_pos, sizeof(ri8_data) );
    /** overloaded generic method to set the right data */
    __IsoAgLib::number2LittleEndianString( ri8_data, vec_data, rui16_pos);
  }

  /** storing data
    @param rui16_pos position to store data in vector
    @param rui16_data data bytes
  */
  void GenericData_c::setDataUi16( uint16_t rui16_pos, uint16_t rui16_data)
  {
    /** first check size of vector */
    CheckSizeOfVectorForWrite( rui16_pos, sizeof(rui16_data) );
    /** overloaded generic method to set the right data */
    __IsoAgLib::number2LittleEndianString( rui16_data, vec_data, rui16_pos);
  }

  /** storing data
    @param rui16_pos position to store data in vector
    @param ri16_data data bytes
  */
  void GenericData_c::setDataI16( uint16_t rui16_pos, int16_t ri16_data)
  {
    /** first check size of vector */
    CheckSizeOfVectorForWrite( rui16_pos, sizeof(ri16_data) );
    /** overloaded generic method to set the right data */
    __IsoAgLib::number2LittleEndianString( ri16_data, vec_data, rui16_pos);
  }

  /** storing data
    @param rui16_pos position to store data in vector
    @param rui32_data data bytes
  */
  void GenericData_c::setDataUi32( uint16_t rui16_pos, uint32_t rui32_data)
  {
    /** first check size of vector */
    CheckSizeOfVectorForWrite( rui16_pos, sizeof(rui32_data) );
    /** overloaded generic method to set the right data */
    __IsoAgLib::number2LittleEndianString( rui32_data, vec_data, rui16_pos);
  }

  /** storing data
    @param rui16_pos position to store data in vector
    @param ri32_data data bytes
  */
  void GenericData_c::setDataI32( uint16_t rui16_pos, int32_t ri32_data)
  {
    /** first check size of vector */
    CheckSizeOfVectorForWrite( rui16_pos, sizeof(ri32_data) );
    /** overloaded generic method to set the right data */
    __IsoAgLib::number2LittleEndianString( ri32_data, vec_data, rui16_pos);
  }

  /** deliver data from vector
    @param  rui16_pos position of byte in vector
    @return unsigned data byte at position rui16_pos
  */
  uint8_t GenericData_c::getDataUi8( uint16_t rui16_pos) const
  {
    /** element is existing */
    if ( CheckSizeOfVectorForRead( rui16_pos, sizeof(uint8_t) ) )
    {
      /** data ok */
      return(vec_data.at(rui16_pos));
    }
    else
    {
      /** in case of an error */
      IsoAgLib::getILibErrInstance().registerError(IsoAgLib::iLibErr_c::Range, IsoAgLib::iLibErr_c::Can);
      return(0);
    }
  }

  /** deliver data from vector
    @param  rui16_pos position of byte in vector
    @return signed data byte at position rui16_pos
  */
  int8_t GenericData_c::getDataI8( uint16_t rui16_pos) const
  {
    /** element is existing */
    if ( CheckSizeOfVectorForRead( rui16_pos, sizeof(int8_t) ) )
    {
      /** data ok */
      return( (int8_t)(vec_data.at(rui16_pos)) );
    }
    else
    {
      /** in case of an error */
      IsoAgLib::getILibErrInstance().registerError(IsoAgLib::iLibErr_c::Range, IsoAgLib::iLibErr_c::Can);
      return(0);
    }
  }

  /** deliver data from vector
    @param  rui16_pos position of byte in vector
    @return unsigned data bytes at position rui16_pos
  */
  uint16_t GenericData_c::getDataUi16( uint16_t rui16_pos) const
  {
    /** initialize temporary variable */
    uint16_t ui16_retData = 0x0000;
    /** element is existing */
    if ( CheckSizeOfVectorForRead( rui16_pos, sizeof(uint16_t) ) )
    {
      ui16_retData = vec_data.at(rui16_pos) << 8;
      ui16_retData = ui16_retData | vec_data.at(rui16_pos + 1) ;
      /** data ok */
      return(ui16_retData);
    }
    else
    {
      /** in case of an error */
      IsoAgLib::getILibErrInstance().registerError(IsoAgLib::iLibErr_c::Range, IsoAgLib::iLibErr_c::Can);
      return(0);
    }
  }

  /** deliver data from vector
    @param  rui16_pos position of byte in vector
    @return signed data bytes at position rui16_pos
  */
  int16_t GenericData_c::getDataI16( uint16_t rui16_pos) const
  {
    /** initialize temporary variable */
    uint16_t ui16_retData = 0x0000;
    /** element is existing */
    if (CheckSizeOfVectorForRead( rui16_pos, sizeof(int16_t) ) )
    {
      ui16_retData = vec_data.at(rui16_pos) << 8;
      ui16_retData = ui16_retData | vec_data.at(rui16_pos + 1) ;
      /** data ok */
      return( (int16_t)(ui16_retData) );
    }
    else
    {
      /** in case of an error */
      IsoAgLib::getILibErrInstance().registerError(IsoAgLib::iLibErr_c::Range, IsoAgLib::iLibErr_c::Can);
      return(0);
    }
  }

  /** deliver data from vector
    @param  rui16_pos position of byte in vector
    @return unsigned data bytes at position rui16_pos
  */
  uint32_t GenericData_c::getDataUi32( uint16_t rui16_pos) const
  {
    /** initialize temporary variable */
    uint32_t ui32_retData = 0x00000000;
    /** element is existing */
    if ( CheckSizeOfVectorForRead( rui16_pos, sizeof(uint32_t) ) )
    {
      ui32_retData = vec_data.at(rui16_pos) << 24;
      ui32_retData = ui32_retData | vec_data.at(rui16_pos + 1) << 16;
      ui32_retData = ui32_retData | vec_data.at(rui16_pos + 2) << 8;
      ui32_retData = ui32_retData | vec_data.at(rui16_pos + 3) ;
      return(ui32_retData);
    }
    else
    {
      /** in case of an error */
      IsoAgLib::getILibErrInstance().registerError(IsoAgLib::iLibErr_c::Range, IsoAgLib::iLibErr_c::Can);
      return(0);
    }
  }

  /** deliver data from vector
    @param  rui16_pos position of byte in vector
    @return signed data bytes at position rui16_pos
  */
  int32_t GenericData_c::getDataI32( uint16_t rui16_pos) const
  {
    /** initialize temporary variable */
    uint32_t ui32_retData = 0x00000000;
    /** element is existing */
    if ( CheckSizeOfVectorForRead( rui16_pos, sizeof(int32_t) ) )
    {
      ui32_retData = vec_data.at(rui16_pos) << 24;
      ui32_retData = ui32_retData | vec_data.at(rui16_pos + 1) << 16;
      ui32_retData = ui32_retData | vec_data.at(rui16_pos + 2) << 8;
      ui32_retData = ui32_retData | vec_data.at(rui16_pos + 3) ;
      return( (int32_t)(ui32_retData) );
    }
    else
    {
      /** in case of an error */
      IsoAgLib::getILibErrInstance().registerError(IsoAgLib::iLibErr_c::Range, IsoAgLib::iLibErr_c::Can);
      return(0);
    }
  }

  /**
  */
  void GenericData_c::setDataStream(uint16_t rui16_bytePos, const uint8_t* rpui8_data, uint16_t rui16_dataLength)
  {
    if (rpui8_data != NULL )
    {
      const unsigned int cui_useLen = ( (rui16_dataLength + rui16_bytePos) < 8 ) ? (rui16_dataLength + rui16_bytePos) : 8;

    }
  }

  /**
  */
  const uint8_t* GenericData_c::getDataStream(uint16_t rui16_bytePos) const
  {
    return(s_datastream);
  }

  /** this method is clearing the vector
  */
  void GenericData_c::ClearVector()
  {
    /* if the vector is not empty */
    if (vec_data.size() > 0)
    {
      /* clearing the vector */
      vec_data.clear();
    }
  }

  /** check whether the write position is out of range
      @param rui16_pos position in vector to write
      @param rui8_size size of data
  */
  void GenericData_c::CheckSizeOfVectorForWrite( uint16_t rui16_pos, uint8_t rui8_size )
  {
    if ( (rui16_pos + rui8_size) > (vec_data.size() - 1 ) )
    {
      /** iterate from size to rui16_pos + number of bytes */
      for (vec_data_iterator_t vec_data_iterator = vec_data.begin() + vec_data.size() - 1;
           vec_data_iterator >= vec_data.begin() + rui16_pos + rui8_size;
           vec_data_iterator++ )
      {
        /** initialize the element of the vector*/
        *vec_data_iterator = 0x00;
      }
    }
  }

  /** check whether the read position is out of range
      @param rui16_pos position in vector to read
      @param rui8_size size of data
      @return false if data does'nt exist otherwise true
   */
  bool GenericData_c::CheckSizeOfVectorForRead( uint16_t rui16_pos, uint8_t rui8_size ) const
  {
    /** have a look to the size of the vector */
    if ( (rui16_pos + rui8_size) > (vec_data.size() - 1 ) )
    {
      /** data at position rui16_pos not existant */
      return(false);
    }
    else
    {
      /** data ok */
      return(true);
    }
  }
};

