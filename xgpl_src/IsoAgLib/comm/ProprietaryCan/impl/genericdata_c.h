/***************************************************************************
                          genericdata_c.h -
                             -------------------
    begin                : Tue Oct 31 2006
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

#ifndef GENERICDATA_H
#define GENERICDATA_H

#include <IsoAgLib/typedef.h>
#include <vector>
#include <IsoAgLib/util/icanpkgext_c.h>
#include <IsoAgLib/util/impl/canpkgext_c.h>


//#include "IsoAgLib/util/impl/canpkg_c.h"

/** storing the data in a vector of uint8_t allows to store any length of data
    for length > 8 the data is sent by Transport-Protocol
 */
class GenericData_c
{
public:
  /** Constructor */
  GenericData_c();

  /** Destructor */
  ~GenericData_c();

  /**
    set ident for the telegram
    @param rui32_ident ident for the telegram
  */
  void setIdent(uint32_t rui32_ident);

  /**
  */
  void setDataUi8( uint16_t rui16_pos, uint8_t rui8_data);

  /**
  */

  void setDataI8( uint16_t rui16_pos, int8_t ri8_data);

  /**
  */
  void setDataUi16( uint16_t rui16_pos, uint16_t rui16_data);

  /**
  */
  void setDataI16( uint16_t rui16_pos, int16_t ri16_data);

  /**
  */
  void setDataUi32( uint16_t rui16_pos, uint32_t rui32_data);

  /**
  */
  void setDataI32( uint16_t rui16_pos, int32_t ri32_data);

  /**
  */
  void setDataStream(uint16_t rui16_bytePos, const uint8_t* rpui8_data, uint16_t rui16_dataLength);

  /**
  */
  uint32_t GenericData_c::getIdent() const;

  /**
  */
  uint8_t getDataUi8( uint16_t rui16_pos) const;

  /**
  */
  int8_t getDataI8( uint16_t rui16_pos) const;

  /**
  */
  uint16_t getDataUi16( uint16_t rui16_pos) const;

  /**
  */
  int16_t getDataI16( uint16_t rui16_pos) const;

  /**
  */
  uint32_t getDataUi32( uint16_t rui16_pos) const;

  /**
  */
  int32_t getDataI32( uint16_t rui16_pos) const;

  /**
  */
  const uint8_t* getDataStream(uint16_t rui16_bytePos) const;

  /** this method is clearing the vector
  */
  void ClearVector();

  /** this method is checking the size of the vector for writing data
  */
  void CheckSizeOfVectorForWrite(uint16_t rui16_pos, uint8_t rui8_size );

  /** this method is checking the size of the vector for reading data
  */
  void CheckSizeOfVectorForRead(uint16_t rui16_pos, uint8_t rui8_size ) const;

  typedef std::vector<uint8_t> vec_data_t;
  typedef std::vector<uint8_t>::iterator vec_data_iterator_t;
  typedef std::vector<uint8_t>::const_iterator vec_data_const_iterator_t;

private:
  /**
  */
  vec_data_t vec_data;

  uint16_t ui16_pos;

  uint32_t ui32_ident;

};
#endif
