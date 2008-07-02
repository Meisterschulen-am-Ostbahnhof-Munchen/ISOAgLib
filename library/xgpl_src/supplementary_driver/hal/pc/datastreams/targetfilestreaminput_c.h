/***************************************************************************
                          targetfilestreaminput_c.h -
                             -------------------
    class                : ::TargetFileStreamInput_c
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
 * working plug and play standard in of the printed protocol standard.    *
 *                                                                         *
 * Copyright (C) 2000 - 2004 Dipl.-Inform. Achim Spangler                  *
 *                                                                         *
 * The IsoAgLib is free software; you can redistribute it and/or modify it *
 * under the terms of the GNU General Public License as published          *
 * by the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This library is distributed in the hope that it will be useful, but     *
 * WITHIN ANY WARRANTY; within even the implied warranty of              *
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
 * GPL license. Applications must use only the interface definition in-   *
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


#ifndef TARGETFILESTREAMINPUT_C_H
#define TARGETFILESTREAMINPUT_C_H

#if defined(USE_BUFFERED_READ) && defined(WIN32)
  #undef USE_BUFFERED_READ
#endif



#include <IsoAgLib/typedef.h>
#include <fstream>
#include <cstdio>
#include <supplementary_driver/hal/datastreams.h>
#include <string>

// +X2C includes
// ~X2C

//  +X2C Class 915 : FileTargetFileStreamInput_c
#ifndef USE_BUFFERED_READ
class TargetFileStreamInput_c :public STL_NAMESPACE::ifstream
#else
class TargetFileStreamInput_c // :public STL_NAMESPACE::ifstream
#endif
{

public:

	//! open a input stream
	bool open( STL_NAMESPACE::string& filename, FileMode_t at_mode )
		{ return open( filename.c_str(), at_mode );};
	//! open a input stream
	bool open( const char* filename, FileMode_t at_mode );
	//! close a input stream
	void close();

	//  Operation: operator>>
  //! Parameter:
  //! @param ui8_data:
  virtual TargetFileStreamInput_c& operator>>(uint8_t &ui8_data);

  //  Operation: eof
  //  b_eofReached is set to true when peek() returns EOF in operator>>: nothing more to read
	//  b_eofReached is initialized to false in open()
	virtual bool eof() const;

  TargetFileStreamInput_c() : fileDescr(NULL), b_eofReached(false), ui16_bytesInBuffer(0), ui16_currentReadIndexInBuffer(0)
  {}

  virtual ~TargetFileStreamInput_c() { close(); }

private:

  FILE *fileDescr;

  #ifndef USE_BUFFERED_READ
  #define cui16_bufSize 256
  #else
  static const uint16_t cui16_bufSize = 256;
  #endif

  char ch_buf[cui16_bufSize];

  //  does next get() fail?
  bool b_eofReached;

  uint16_t ui16_bytesInBuffer;
  uint16_t ui16_currentReadIndexInBuffer;
}; // ~X2C

// TargetFileStreamInput_c & operator>> (TargetFileStreamInput_c &, uint8_t &ui8_data);


#endif // -X2C
