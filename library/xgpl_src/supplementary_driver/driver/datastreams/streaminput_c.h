/***************************************************************************
                          streaminput_c.h -
                             -------------------
    class                : ::StreamInput_c
    project              : IsoAgLib
    begin                : Tue Jan 25 17:41:42 2005
    copyright            : (C) 2005 by Achim Spangler (a.spangler@osb-ag.de)
    email                : a.spangler@osb-ag:de
    $Id$

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
 * Copyright (C) 2000 - 2009 Dipl.-Inform. Achim Spangler                  *
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


#ifndef STREAMINPUT_C_H
#define STREAMINPUT_C_H

#ifdef PREPARE_VTGB
  #include <IsoAgLib/comm/Part6_VirtualTerminal_Client/ivttypes.h>
#endif
#include <IsoAgLib/util/convert.h>

/** Abstract base class for a streaming input source.
  * Realized by FileStreamInput_c for file readout,
  * and VolatileMemory_c for memory buffer readout.
  * iStream_c is defining a superset of StreamInput_c.
  */
class StreamInput_c
{
public:
  // Abstract interface definition

  //! Stream input operation.
  //! @param ui8_data Referenze to store read byte to.
  virtual StreamInput_c& operator>>(uint8_t& ui8_data)=0;

  //! End of file
  virtual bool eof() const=0;

  //! Virtual destructor to prevent from warning.
  virtual ~StreamInput_c() {};

  // For convenience some transparent interface extensions.

  //! Read one byte
  uint8_t get( void ) { uint8_t byte; *this >> byte; return byte; };

  //! Stream input of int8_t
  StreamInput_c& operator>>(int8_t& i8_data) {
    i8_data = convert_n::castI( get() ); return *this;
  }

  //! Stream input of uint16_t
  StreamInput_c& operator>>(uint16_t& ui16_data) {
    uint8_t l,h; *this >> l >> h;
    ui16_data = (uint16_t(h) << 8) | uint16_t(l);
    return *this;
  }

  //! Stream input of int16_t
  StreamInput_c& operator>>(int16_t& i16_data) {
    uint16_t ui16; *this >> ui16;
    i16_data = convert_n::castI( ui16 ); return *this;
  }

#if defined(_BASIC_STRING_H) || defined(__BASTRING__)
  //! Read string up to given size or up to eof() if less.
  void get( STL_NAMESPACE::string& c_str, int i_size ) {
    for (;(i_size > 0) && (!eof());--i_size) c_str.push_back( get() );
  }

  //! Read string up to eof
  void get( STL_NAMESPACE::string& c_str ) {
    while (!eof()) c_str.push_back( get() );
  }
#endif
};

#endif // STREAMINPUT_C_H

