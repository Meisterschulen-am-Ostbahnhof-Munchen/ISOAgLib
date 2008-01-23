/***************************************************************************
                          chunk_c.cpp -
                             -------------------
    class                : ::Chunk_c
    project              : IsoAgLib
    begin                : Tue Jan 25 17:40:50 2005
    copyright            : (C) 2005 by Achim Spangler (a.spangler@osb-ag.de)
    email                : a.spangler@osb-ag:de
    $Id: chunk_c.cpp 1134 2005-05-16 14:06:15Z torborg $

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
 * Copyright (C) 2000 - 2004 Dipl.-Inform. Achim Spangler                  *
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

#include "chunk_c.h"
#include "stream_c.h"


// Begin Namespace __IsoAgLib
namespace __IsoAgLib {




// Copy constructor
Chunk_c::Chunk_c( const Chunk_c& arcc_src )
{
  marr_data.reserve (arcc_src.marr_data.capacity());
  STL_NAMESPACE::copy(arcc_src.marr_data.begin(), arcc_src.marr_data.end(), marr_data.begin());
}



// //////////////////////////////// +X2C Operation : init
//! Reserves Memory for the Vector
//! basing on the Packet-burst-size times the Packet-size (7)
bool
Chunk_c::init()
{ // ~X2C
  marr_data.reserve( mscui16_chunkSize );
  return marr_data.capacity() == mscui16_chunkSize;
} // -X2C



// //////////////////////////////// +X2C Operation : insert
//! inserts (push-back) a byte into the vector
//! Parameter:
//! @param aui8_data: Byte to be inserted
//! @return Number of inserted bytes
uint8_t
Chunk_c::insert(uint8_t aui8_data)
{ // ~X2C
  if (full()) return 0;
  marr_data.push_back( aui8_data );
  return 1;
} // -X2C



// //////////////////////////////// +X2C Operation : insert7Bytes
//! inserts (push-back) 7 bytes into the vector if possible
//! Parameter:
//! @param pui8_data: Bytes to be inserted
//! @return Number of inserted bytes
uint8_t
Chunk_c::insert7Bytes(const uint8_t* pui8_data)
{ // ~X2C
  uint8_t  nbr=0;

  while (!full() && (nbr < 7))
    marr_data.push_back(pui8_data[nbr++]);
  return nbr;
} // -X2C



// //////////////////////////////// +X2C Operation : getData
//! Returns the spezified byte-value
//! Parameter:
//! @param aui32_pos: position (0..x)
//! @return Byte-value or 0xffff (out of range)
uint16_t
Chunk_c::getData(uint32_t aui32_pos)
{ // ~X2C
  if (aui32_pos >= marr_data.size()) return 0xffff;
  return marr_data[aui32_pos];
} // -X2C



// //////////////////////////////// +X2C Operation : setFree
//! Delets all objects of the vector
void
Chunk_c::setFree()
{ // ~X2C
  marr_data.clear();
} // -X2C



// //////////////////////////////// +X2C Operation : getFreeCnt
//! Returns the Number of objects
//! @return Number of objects
uint8_t
Chunk_c::getFreeCnt() const
{ // ~X2C
  return marr_data.capacity() - marr_data.size();
} // -X2C



// //////////////////////////////// +X2C Operation : free
//! Returns True if the Chunk is empty
//! @return True if the Chunk is empty
bool
Chunk_c::free()
{ // ~X2C
  return marr_data.empty();
} // -X2C



// //////////////////////////////// +X2C Operation : full
//! Returns True if the Chunk is full
//! @return True if the Chunk is full
bool
Chunk_c::full()
{ // ~X2C
  return marr_data.size() == marr_data.capacity();
} // -X2C

} // end namespace __IsoAgLib
