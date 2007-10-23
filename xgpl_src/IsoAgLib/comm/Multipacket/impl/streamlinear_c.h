/***************************************************************************
                          streamlinear_c.h -
                             -------------------
    class                : ::StreamLinear_c
    project              : IsoAgLib
    begin                : Tue Jan 25 17:39:57 2005
    copyright            : (C) 2005 by Achim Spangler (a.spangler@osb-ag.de)
    email                : a.spangler@osb-ag:de
    $Id: streamlinear_c.h 1089 2005-05-04 17:39:28Z wodok $

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

#ifndef STREAMLINEAR_C_H
#define STREAMLINEAR_C_H


#include "stream_c.h"

// stl
#include <cstdlib>	// Include before vector or else CNAMESPACE stuff is screwed up for Tasking
#include <vector>



// Begin Namespace __IsoAgLib
namespace __IsoAgLib {



// forward declarations
class Chunk_c;


//  +X2C Class 755 : StreamLinear_c
class StreamLinear_c : public Stream_c
{

public:

  StreamLinear_c (StreamType_t at_streamType, const IsoAgLib::ReceiveStreamIdentifier_c& ac_rsi, uint32_t aui32_msgSize SINGLETON_VEC_KEY_PARAMETER_DEF_WITH_COMMA);

  virtual ~StreamLinear_c ();

  //  Operation: insert
  //! Parameter:
  //! @param pui8_data:
  void insert7Bytes(const uint8_t* pui8_data);

  #ifdef NMEA_2000_FAST_PACKET
  void insertFirst6Bytes(const uint8_t* pui8_data);
  #endif

  //  Operation: getNextNotParsed
  //! Also moves one byte forward!
  inline uint8_t getNextNotParsed();

  //  Operation: getNotParsedSize
  inline uint32_t getNotParsedSize();

  //  Operation: getNotParsed
  inline uint8_t getNotParsed(uint32_t ui32_notParsedRelativeOffset);

  //  Operation: getLinearBuffer
  inline uint8_t* getLinearBuffer();

  inline bool eof() const;
	void immediateInitAfterConstruction() const {};

protected:

private:

  STL_NAMESPACE::vector<uint8_t> vui8_buffer; // linearBuffer

  //  Attribute: ui32_parsedCnt
  uint32_t ui32_parsedCnt;

}; // ~X2C



// //////////////////////////////// +X2C Operation 783 : getNextNotParsed
//! ATTENTION: Only use this function if it is assured using getNotParsedSize that there's something in the buffer!
inline uint8_t
StreamLinear_c::getNextNotParsed()
{ // ~X2C
  return vui8_buffer[ui32_parsedCnt++];
} // -X2C



inline uint8_t*
StreamLinear_c::getLinearBuffer()
{
  return &vui8_buffer.front();
}


// //////////////////////////////// +X2C Operation 838 : getNotParsedSize
inline uint32_t
StreamLinear_c::getNotParsedSize()
{ // ~X2C
  uint32_t ui32_bufSize = vui8_buffer.size();
  // if the buffer is larger than our totalMsgSize, clip at totalMsgSize.
  // this occurs as we always insert 7 bytes, even if the last 7byte packet should NOT been taken completely
  if (ui32_byteTotalSize < ui32_bufSize)
    return (ui32_byteTotalSize - ui32_parsedCnt);
  else
    return (ui32_bufSize - ui32_parsedCnt);
} // -X2C



// //////////////////////////////// +X2C Operation : getNotParsed
//! be sure to NOT read from an offset which overflows the unparsed buffer!
inline uint8_t
StreamLinear_c::getNotParsed (uint32_t ui32_notParsedRelativeOffset)
{
  return vui8_buffer[ui32_parsedCnt + ui32_notParsedRelativeOffset];
}



// //////////////////////////////// +X2C Operation 2878 : eof
inline bool
StreamLinear_c::eof() const
{ // ~X2C
  if (ui32_parsedCnt >= ui32_byteTotalSize)
    return true;
  else
    return false;
} // -X2C



} // end namespace __IsoAgLib
#endif // -X2C
