/***************************************************************************
                          streamchunk_c.h -
                             -------------------
    class                : ::StreamChunk_c
    project              : IsoAgLib
    begin                : Tue Jan 25 17:39:57 2005
    copyright            : (C) 2005 by Achim Spangler (a.spangler@osb-ag.de)
    email                : a.spangler@osb-ag:de
    $Id: streamchunk_c.h 1089 2005-05-04 17:39:28Z wodok $

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

#ifndef STREAMCHUNK_C_H
#define STREAMCHUNK_C_H


#include "stream_c.h"
#include "chunk_c.h"

// stl
#include <list>




// Begin Namespace __IsoAgLib
namespace __IsoAgLib {



//  +X2C Class 755 : StreamChunk_c
class StreamChunk_c : public Stream_c
{

public:

  StreamChunk_c (StreamType_t at_streamType, const IsoAgLib::ReceiveStreamIdentifier_c& ac_rsi, uint32_t aui32_msgSize SINGLETON_VEC_KEY_PARAMETER_DEF_WITH_COMMA , bool b_skipCtsAwait=false);

  StreamChunk_c( const StreamChunk_c& arc_src );

  //! Important!! Call this after Construction!
  void immediateInitAfterConstruction();

  //! Destructor: deletes the list
  virtual ~StreamChunk_c();

  const StreamChunk_c& operator=( const StreamChunk_c& arc_src );


  //  Operation: insert
  //! Parameter:
  //! @param pui8_data:
  void insert7Bytes(const uint8_t* pui8_data);

  #ifdef NMEA_2000_FAST_PACKET
  void insertFirst6Bytes(const uint8_t* pui8_data);
  #endif

  //  Operation: getNextNotParsed
  //! Also moves one byte forward!
  uint8_t getNextNotParsed();

  //  Operation: getNotParsedSize
  uint32_t getNotParsedSize();

  //  Operation: getNotParsed
  uint8_t getNotParsed(uint32_t ui32_notParsedRelativeOffset);

  bool eof() const;

private: // function
  inline void copyIterator (const STL_NAMESPACE::list<Chunk_c>& rc_listSource, const STL_NAMESPACE::list<Chunk_c>::iterator& rc_iterSource, STL_NAMESPACE::list<Chunk_c>& rc_listDestin, STL_NAMESPACE::list<Chunk_c>::iterator& rc_iterDestin);


private:
  //  Attribute: list_chunks
  STL_NAMESPACE::list<Chunk_c> list_chunks;

  //  Attribute: pc_iterWriteChunk
  STL_NAMESPACE::list<Chunk_c>::iterator pc_iterWriteChunk;

  //  Attribute: pc_iterParsedChunk
  STL_NAMESPACE::list<Chunk_c>::iterator pc_iterParsedChunk;

  //  Attribute: ui32_writeCnt
  uint32_t ui32_writeCnt;

  //  Attribute: ui32_parsedCnt
  uint32_t ui32_parsedCnt;

}; // ~X2C



inline void
StreamChunk_c::copyIterator (const STL_NAMESPACE::list<Chunk_c>& rc_listSource, const STL_NAMESPACE::list<Chunk_c>::iterator& rc_iterSource, STL_NAMESPACE::list<Chunk_c>& rc_listDestin, STL_NAMESPACE::list<Chunk_c>::iterator& rc_iterDestin)
{
  STL_NAMESPACE::list<Chunk_c>::const_iterator pc_iterSource = rc_listSource.begin();
                                   rc_iterDestin = rc_listDestin.begin();
  while (pc_iterSource != rc_listSource.end())
  {
    if (&(*pc_iterSource) == &(*rc_iterSource))
    { // destinIter found.
      return;
    }
    pc_iterSource++;
    rc_iterDestin++;
  }
}

} // end namespace __IsoAgLib
#endif // -X2C
