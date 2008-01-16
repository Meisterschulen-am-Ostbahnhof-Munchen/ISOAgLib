/***************************************************************************
                          multisendpkg_c.cpp - data object for Multipacket messages
                             -------------------
    begin                : Fri Sep 28 2001
    copyright            : (C) 2000 - 2004 by Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
    type                 : Source
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

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include "multisendpkg_c.h"
#include <IsoAgLib/driver/can/impl/ident_c.h>

namespace __IsoAgLib {


static const uint8_t paddingDataArr[] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

/**
  set the 7 uint8_t data part of transfer message
  @param apb_source source data pointer
  @param ai32_pos uint8_t position in data string to start
  @param ab_partSize optional amount of bytes of data stream for actual pkg (default 7) {Anyway MAX 7 bytes!!}
*/
void MultiSendPkg_c::setDataPart(const HUGE_MEM uint8_t* apb_source, int32_t ai32_pos, uint8_t ab_partSize)
{
  #ifdef USE_HUGE_MEM
  const HUGE_MEM uint8_t* pb_source = apb_source + ai32_pos;
  // fist assign is performed with index:=1, so that last assign should be performed with
  // ind == ab_partSize (e.g. ab_partSize:=7 ==> last written index:=7 == Byt8 of CanPkg_c)
  for ( uint8_t ind = 1; ind <= ab_partSize; ind++ )
  { // the ind matches already to the appropriate CanPkg_c data string position,
    // while the pb_source pointer is also set to the start of the source data
    setUint8Data( ind, *pb_source );
    // now increment the source pointer for next access
    pb_source++;
  }
  #else
  setDataFromString (1, apb_source + ai32_pos, ab_partSize);
  #endif
  if ( ab_partSize < 7 )
  { // only pad when less than 7 data byte
    setDataFromString( 1+ab_partSize, paddingDataArr, (7-ab_partSize) );
  }
}



/**
  set the 7 uint8_t data part of transfer message
  @param rc_vecSource source data pointer
  @param ai32_pos uint8_t position in data string to start
  @param ab_partSize optional amount of bytes of data stream for actual pkg (default 7)
  */
void MultiSendPkg_c::setDataPart(const STL_NAMESPACE::vector<uint8_t>& rc_vecSource, int32_t ai32_pos, uint8_t ab_partSize)
{
  STL_NAMESPACE::vector<uint8_t>::const_iterator iter = rc_vecSource.begin()+ai32_pos;
  // fist assign is performed with index:=1, so that last assign should be performed with
  // ind == ab_partSize (e.g. ab_partSize:=7 ==> last written index:=7 == Byt8 of CanPkg_c)
  for ( uint8_t ind = 1; ind <= ab_partSize; ind++ )
  { // the ind matches already to the appropriate CanPkg_c data string position,
    // while the iter iterator is also set to the start of the source data
    setUint8Data( ind, *iter );
    // now increment the iterator for next access
    iter++;
  }
  if ( ab_partSize < 7 )
  { // only pad when less than 7 data byte
    setDataFromString( 1+ab_partSize, paddingDataArr, (7-ab_partSize) );
  }
}



#if defined(ENABLE_MULTIPACKET_VARIANT_FAST_PACKET)
/**
  set the 7 uint8_t data part of transfer message
  @param apb_source source data pointer
  @param ai32_pos uint8_t position in data string to start
  @param ab_partSize optional amount of bytes of data stream for actual pkg (default 7) {Anyway MAX 7 bytes!!}
 */
void MultiSendPkg_c::setFastPacketDataPart(const HUGE_MEM uint8_t* apb_source, int32_t ai32_pos, uint8_t ab_partSize, uint8_t aui8_offset)
{
#ifdef USE_HUGE_MEM
  const HUGE_MEM uint8_t* pb_source = apb_source + ai32_pos;
  // examples:
  // ab_partSize:=7, aui8_offset:=1 ==> cui8_endCondition:=8
  // ==> first assign should match target byte index aui8_offset:=1
  //     and last index should be <8 -> i.e. index:=7
  const uint8_t cui8_endCondition = ab_partSize + aui8_offset;
  for ( uint8_t ind = aui8_offset; ind < cui8_endCondition; ind++ )
  { // the ind matches already to the appropriate CanPkg_c data string position,
    // while the pb_source pointer is also set to the start of the source data
    setUint8Data( ind, *pb_source );
    // now increment the source pointer for next access
    pb_source++;
  }
#else
  setDataFromString (aui8_offset, apb_source + ai32_pos, ab_partSize);
#endif
  setDataFromString (aui8_offset+ab_partSize, paddingDataArr, (8-aui8_offset-ab_partSize));
}



/**
  set the 7 uint8_t data part of transfer message
  @param apb_source source data pointer
  @param ai32_pos uint8_t position in data string to start
  @param ab_partSize optional amount of bytes of data stream for actual pkg (default 7)
  */
void MultiSendPkg_c::setFastPacketDataPart(const STL_NAMESPACE::vector<uint8_t>& rc_vecSource, int32_t ai32_pos, uint8_t ab_partSize, uint8_t aui8_offset )
{
  STL_NAMESPACE::vector<uint8_t>::const_iterator iter = rc_vecSource.begin()+ai32_pos;
  // examples:
  // ab_partSize:=7, aui8_offset:=1 ==> cui8_endCondition:=8
  // ==> first assign should match target byte index aui8_offset:=1
  //     and last index should be <8 -> i.e. index:=7
  const uint8_t cui8_endCondition = ab_partSize + aui8_offset;
  for ( uint8_t ind = aui8_offset; ind < cui8_endCondition; ind++ )
  { // the ind matches already to the appropriate CanPkg_c data string position,
    // while the iter iterator is also set to the start of the source data
    setUint8Data( ind, *iter );
    // now increment the iterator for next access
    iter++;
  }
  setDataFromString (aui8_offset+ab_partSize, paddingDataArr, (8-aui8_offset-ab_partSize));
}
#endif

} // end of namespace __IsoAgLib
