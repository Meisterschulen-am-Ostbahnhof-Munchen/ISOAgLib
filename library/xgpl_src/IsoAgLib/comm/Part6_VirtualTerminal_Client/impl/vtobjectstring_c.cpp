/***************************************************************************
                          isoterminalstringmss_c.cpp
                             -------------------
    begin                : Don Sep 4 2003
    copyright            : (C) 2003 by Martin Wodok
    email                : m.wodok@osb-ag:de
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
#include "vtobjectstring_c.h"
#include "../ivtobjectstring_c.h"

#include <IsoAgLib/comm/Multipacket/impl/multisendpkg_c.h>

namespace IsoAgLib {
  // implement here a normal constructor/destructor, as the compiler dislikes inlining of that simple
  // constructor/destructor direct in scope of iVtObjectString_c
  iVtObjectString_c::iVtObjectString_c() : vtObjectString_c() {}
  iVtObjectString_c::~iVtObjectString_c(){}
}

namespace __IsoAgLib {

// Operation : setDataNextStreamPart
//! @param mspData:
//! @param bytes:
void
vtObjectStringStreamer_c::setDataNextStreamPart(__IsoAgLib::MultiSendPkg_c* mspData, uint8_t bytes)
{
  uint8_t bytesDone=0;
  if (mui32_streamPosition == 0) {
    set5ByteCommandHeader(marr_uploadBuffer);
    bytesDone = 5;

    mui32_streamPosition += 5;
  }

  while (bytesDone < bytes) {
    marr_uploadBuffer [bytesDone] = mpc_stringToStream [mui32_streamPosition-5];
    bytesDone++;
    mui32_streamPosition++;
  }

  mspData->setDataPart (marr_uploadBuffer, 0, bytes);
}


void
vtObjectStringStreamer_c::set5ByteCommandHeader(uint8_t* destinBuffer)
{
  destinBuffer [0] = getFirstByte(); // 179 == Command: "Command" --- Parameter: "Change String Value"
  destinBuffer [1] = getID () & 0xFF;
  destinBuffer [2] = getID () >> 8;
  destinBuffer [3] = mui16_strLenToSend & 0xFF;
  destinBuffer [4] = mui16_strLenToSend >> 8;
}

// Operation : resetDataNextStreamPart
void
vtObjectStringStreamer_c::resetDataNextStreamPart()
{
  mui32_streamPosition = 0;
}


// Operation : saveDataNextStreamPart
void
vtObjectStringStreamer_c::saveDataNextStreamPart()
{
  mui32_streamPositionStored = mui32_streamPosition;
}


// Operation : restoreDataNextStreamPart
void
vtObjectStringStreamer_c::restoreDataNextStreamPart()
{
  mui32_streamPosition = mui32_streamPositionStored;
}

// Operation : getStreamSize
uint32_t
vtObjectStringStreamer_c::getStreamSize()
{
  return 5+mui16_strLenToSend;
}


vtObjectString_c::vtObjectString_c()
{}

vtObjectString_c::~vtObjectString_c()
{}

void vtObjectString_c::setStringToStream( const char* apc_stringToStream )
{
  mc_streamer.setStringToStream( apc_stringToStream );
  // additionally set the ID
  mc_streamer.setID( getID() );
}

void vtObjectString_c::setStrLenToSend( uint16_t aui16_strLenToSend )
{
  mc_streamer.setStrLenToSend( aui16_strLenToSend );
}

} // end namespace __IsoAgLib
