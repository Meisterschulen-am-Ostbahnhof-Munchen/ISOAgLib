/***************************************************************************
                          targetfilestreaminput_c.cpp -
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
 * WITHOUT ANY WARRANTY; within even the implied warranty of              *
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
#include "targetfilestreaminput_c.h"
#ifndef USE_BUFFERED_READ
  #include <cstdio>
  #include <fstream>
#else
  #include <fcntl.h>
#endif

#if __GNUC__ < 3
  #define MACRO_IOS ios
#else
  #define MACRO_IOS std::ios_base
#endif

using namespace std;

//! open a input stream
bool TargetFileStreamInput_c::open( const char* filename, FileMode_t rt_mode )
{
#ifndef USE_BUFFERED_READ

  b_eofReached = false;

  if ( ( rt_mode & StreamOut ) != 0 ) return false;

  MACRO_IOS::openmode mode = MACRO_IOS::in;

  if ( ( rt_mode & StreamAte   )  != 0 ) mode = MACRO_IOS::openmode( mode | MACRO_IOS::ate    );
  if ( ( rt_mode & StreamApp   )  != 0 ) mode = MACRO_IOS::openmode( mode | MACRO_IOS::app    );
  if ( ( rt_mode & StreamTrunc )  != 0 ) mode = MACRO_IOS::openmode( mode | MACRO_IOS::trunc  );
  if ( ( rt_mode & StreamBinary ) != 0 ) mode = MACRO_IOS::openmode( mode | MACRO_IOS::binary );


  static_cast<ifstream*>(this)->open( filename, mode );

  if ( ( static_cast<ifstream*>(this)->good()    )
    && ( static_cast<ifstream*>(this)->is_open() ) ) return true;
  else
    return false;

#else

  if ( ( rt_mode & StreamOut ) != 0 ) return false;

  if ( ( rt_mode & StreamTrunc ) != 0 )
  {
    fileDescr = fopen(filename, "w"); // Truncate  file  to  zero length
    if (NULL == fileDescr)
      return false;

    fclose(fileDescr);
    fileDescr = NULL;
  }

  if ( ( rt_mode & StreamApp ) != 0 )
    fileDescr = fopen(filename, "a+"); // open for reading and appending, file position for reading at the begin
  else
    fileDescr = fopen(filename, "r"); // open for reading

  if (NULL == fileDescr)
    return false;

  if ( ( rt_mode & StreamAte ) != 0 )
    fseek(fileDescr, 0, SEEK_END); // file position for reading at end

  b_eofReached = feof(fileDescr);

  return true;
#endif
};


//  Operation: operator>>
//! Parameter:
//! @param ui8_data:
TargetFileStreamInput_c& TargetFileStreamInput_c::operator>>(uint8_t &ui8_data)
{
#ifndef USE_BUFFERED_READ

  ifstream* isp_tmp = static_cast<ifstream*>(this);

  ui8_data = isp_tmp->get();

  // check if next get returns EOF: nothing more to read
  if ((isp_tmp->peek() == EOF) || (isp_tmp->eof()))
    b_eofReached = true;

#else

  if (ui16_currentReadIndexInBuffer >= ui16_bytesInBuffer)
  {
    ui16_bytesInBuffer = fread(ch_buf, 1, cui16_bufSize, fileDescr);
    ui16_currentReadIndexInBuffer = 0;
    b_eofReached = feof(fileDescr);
    if (!b_eofReached)
    {
      ungetc(fgetc(fileDescr), fileDescr); // do peek
      // stream state is still EOF after fgetc/ungetc in case fgetc yields EOF
      b_eofReached = feof(fileDescr);
    }
  }

  if (ui16_currentReadIndexInBuffer < ui16_bytesInBuffer)
  {
    ui8_data = ch_buf[ui16_currentReadIndexInBuffer];
    ui16_currentReadIndexInBuffer++;
  }
#endif

  return *this;
}

bool
TargetFileStreamInput_c::eof() const
{
#ifndef USE_BUFFERED_READ
  return b_eofReached | static_cast<const ifstream*>(this)->eof();
#else
  if (!fileDescr)
    return true;

  return (b_eofReached && (ui16_currentReadIndexInBuffer >= ui16_bytesInBuffer));
#endif
}

void
TargetFileStreamInput_c::close()
{
#ifndef USE_BUFFERED_READ
  static_cast<ifstream*>(this)->close();
#else
  if (fileDescr)
    fclose(fileDescr);

  fileDescr = NULL;
#endif
}

