/***************************************************************************
                          igenericdata_c.h - Class to independently handle
                          any length of data-message (along with CAN-identifier)
                             -------------------
    begin                : Tue Oct 31 2006
    copyright            : (C) 1999 - 2007 by Christoph Staub
    email                : c.staub@osb-ag:de
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
 * Copyright (C) 1999 - 2007 Dipl.-Inform. Achim Spangler                  *
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

#ifndef IGENERICDATA_H
#define IGENERICDATA_H

#include <IsoAgLib/typedef.h>
#include <cstdlib>	// Include before vector or else CNAMESPACE stuff is screwed up for Tasking
#include <vector>
#include <IsoAgLib/comm/Part3_DataLink/icanpkgext_c.h>
#include <IsoAgLib/comm/Part3_DataLink/impl/canpkgext_c.h>
#include <IsoAgLib/util/impl/util_funcs.h>

/** Begin Namespace IsoAgLib */
namespace IsoAgLib
{
  /** storing the data in a vector of uint8_t allows to store any length of data
    for length > 8 the data is sent by Transport-Protocol
   */
  class iGenericData_c
  {
  public:
    /** Constructor */
    iGenericData_c();

    /** Destructor */
    ~iGenericData_c();

    /** Constructor that copies the content (ident and data) of iCanPkgExt_c
      @param arc_canPkg iCanPkgExt_c to be copied to the iGenericData_c
    */
    iGenericData_c (const iCanPkgExt_c& arc_canPkg);

    /** copy iGenericData_c content (ident and data) into an iCanPkgExt_c
      @param arc_canPkg iCanPkgExt_c to get the content to (destination)
    */
    void getDataToCanPkgExt (iCanPkgExt_c& arc_canPkg);

    /**
      set ident for the telegram
      @param aui32_ident ident for the telegram
    */
    void setIdent(uint32_t aui32_ident)
    {
      ui32_ident = aui32_ident;
    }

    /**
      @return uint32_t ident
    */
    uint32_t getIdent() const
    {
      return (ui32_ident);
    }

    /** storing data
      @param aui16_pos position to store data in vector
      @param aui8_data data byte
    */
    void setDataUi8( uint16_t aui16_pos, uint8_t aui8_data);

    /** storing data
      @param aui16_pos position to store data in vector
      @param ai8_data data byte
    */
    void setDataI8( uint16_t aui16_pos, int8_t ai8_data);

    /** storing data
      @param aui16_pos position to store data in vector
      @param aui16_data data bytes
    */
    void setDataUi16( uint16_t aui16_pos, uint16_t aui16_data);

    /** storing data
      @param aui16_pos position to store data in vector
      @param ai16_data data bytes
    */
    void setDataI16( uint16_t aui16_pos, int16_t ai16_data);

    /** storing data
      @param aui16_pos position to store data in vector
      @param aui32_data data bytes
    */
    void setDataUi32( uint16_t aui16_pos, uint32_t aui32_data);

    /** storing data
      @param aui16_pos position to store data in vector
      @param ai32_data data bytes
    */
    void setDataI32( uint16_t aui16_pos, int32_t ai32_data);

    /** deliver data from vector
      @param  aui16_pos position of byte in vector
      @return unsigned data byte at position aui16_pos
    */
    uint8_t getDataUi8( uint16_t aui16_pos) const;

    /** deliver data from vector
      @param  aui16_pos position of byte in vector
      @return signed data byte at position aui16_pos
    */
    int8_t getDataI8( uint16_t aui16_pos) const;

    /** deliver data from vector
      @param  aui16_pos position of byte in vector
      @return unsigned data bytes at position aui16_pos
    */
    uint16_t getDataUi16( uint16_t aui16_pos) const;

    /** deliver data from vector
      @param  aui16_pos position of byte in vector
      @return signed data bytes at position aui16_pos
    */
    int16_t getDataI16( uint16_t aui16_pos) const;

    /** deliver data from vector
      @param  aui16_pos position of byte in vector
      @return unsigned data bytes at position aui16_pos
    */
    uint32_t getDataUi32( uint16_t aui16_pos) const;

    /** deliver data from vector
      @param  aui16_pos position of byte in vector
      @return signed data bytes at position aui16_pos
    */
    int32_t getDataI32( uint16_t aui16_pos) const;

    /** return the length of data vector */
    uint16_t getLen() const;

    /** set data stream to vector vec_data
      @param aui16_pos position of byte in vector
      @param apui8_data pointer to data for read
      @param aui16_dataLength num of data bytes at position aui16_pos
    */
    void setDataStream(uint16_t aui16_pos, const uint8_t* apui8_data, uint16_t aui16_dataLength);

    /** set data stream to vector vec_data
      @param aui16_pos position of byte in vector
      @param arui8_data reference to data for read
      @param aui16_dataLength num of data bytes at position aui16_pos
    */
    void setDataStream(uint16_t aui16_pos, const uint8_t& arui8_data, uint16_t aui16_dataLength);

    /** deliver a pointer to the wanted data
      @param aui16_bytePos position of data
      @return uint8_t* pointer to data
    */
    const uint8_t* getDataStream(uint16_t aui16_bytePos=0) const;

    /** this method is clearing the vector
    */
    void clearVector()
    {
      /* clearing the vector */
      vec_data.clear();
    }

    /** this method is checking the size of the vector for writing data */
    void CheckSizeOfVectorForWrite(uint16_t aui16_pos, uint16_t aui16_size );

    /** this method is checking the size of the vector for reading data */
    bool CheckSizeOfVectorForRead(uint16_t aui16_pos, uint16_t aui16_size ) const;

    /** type definition of vector */
    typedef STL_NAMESPACE::vector<uint8_t> vec_data_t;

    /** type definition of an iterator to write the data */
    typedef STL_NAMESPACE::vector<uint8_t>::iterator vec_data_iterator_t;

    /** type definition of a constant iterator to read the data */
    typedef STL_NAMESPACE::vector<uint8_t>::const_iterator vec_data_const_iterator_t;

  private:
    /** vector to store the data */
    vec_data_t vec_data;

    /** ident of the message */
    uint32_t ui32_ident;
  };
};
#endif
