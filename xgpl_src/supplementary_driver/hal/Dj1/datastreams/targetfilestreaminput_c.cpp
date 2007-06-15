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
#include "djbioseprominterface_c.h"

#ifdef WIN32
#include <stdio.h>
#endif


//	#include "../djbiosdrawinterface.h"
#include <IsoAgLib/typedef.h>

#ifndef _huge
#define _huge
#endif

// namespace __HAL
// {
// extern "C" 
// {
//   /** include the BIOS specific header into __HAL */
// #ifdef SYSTEM_PC
// #include <../../IsoAgLib/commercial_BIOS/bios_Dj1/DjBiosMVT.h>
// #else
// #include <commercial_BIOS/bios_Dj1/DjBiosMVT.h>
// #endif
// }
// 
// // static const char dummy_file_data[] = "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef";
// // static const char *dummy_file_pointer = dummy_file_data;
// 
// void *DjBios_IOP_Open_dummy ( char const *Filename, char const *Mode )
// {
//   dummy_file_pointer = dummy_file_data;
//   return (void*)&dummy_file_pointer;
// }
// 
// enum_Bool DjBios_IOP_EoF_dummy ( void const *handle )
// {
//   if (NULL == handle || &dummy_file_pointer != handle || 0 == *dummy_file_pointer)
//     return BIOS_TRUE;
//   else
//     return BIOS_FALSE;
// }
// 
// uword DjBios_IOP_Read_dummy ( void const *handle, uword Size, uword NumObj, void *Data )
// {
//   if (&dummy_file_pointer != handle || 0 == *dummy_file_pointer) {
//     return 0;
//   }
//   *((char *)Data) = *dummy_file_pointer;
//   ++dummy_file_pointer;
//   return 1;
// }
// 
// enum_Bool DjBios_IOP_Close_dummy ( void const *Handle )
// {
//   return BIOS_TRUE;
// }
// 
// } // Namespace HAL
// 
// using namespace std;



TargetFileStreamInput_c::TargetFileStreamInput_c() :
  file_handle_(0), n_data_read_(0)
{
}

TargetFileStreamInput_c::~TargetFileStreamInput_c()
{
  close();
}

//! open an input stream
bool TargetFileStreamInput_c::open( const char* filename, FileMode_t rt_mode )
{
  CNAMESPACE::string mode_string;

  if (StreamIn & rt_mode) {
    mode_string.push_back('r');
  }
  
  if (mode_string.empty()) {
    return false;
  }

  file_handle_ = DjBiosEpromInterface_c::OpenIop(filename, mode_string.c_str());

  bool result = (NULL != file_handle_);
  return result;
};

bool TargetFileStreamInput_c::eof() const
{
  __HAL::enum_Bool bios_result = DjBiosEpromInterface_c::EofIop(file_handle_);
  bool result = (__HAL::BIOS_FALSE != bios_result);
  return result;
}

//  Operation: operator>>
//! Parameter:
//! @param ui8_data:
TargetFileStreamInput_c& TargetFileStreamInput_c::operator>>(uint8_t &ui8_data)
{
  if (eof()) {
    ui8_data = 0;
  } else {
    ui8_data = DjBiosEpromInterface_c::ReadIop(file_handle_, 1, 1, &ui8_data);
    ++n_data_read_;
  }
	return *this;
}

void TargetFileStreamInput_c::close()
{
  (void)DjBiosEpromInterface_c::CloseIop(file_handle_);
}
