/***************************************************************************
                          targetfilestreamoutput_c.cpp -
                             -------------------
    class                : ::TargetFileStreamOutput_c
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
 * Copyright (C) 2000 - 2004 Dipl.-Outform. Achim Spangler                  *
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
 * Outc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA           *
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
 * - Outstantiation of a variable with a datatype from internal namespace   *
 * - Call of a (member-) function                                          *
 * Allowed is:                                                             *
 * - Outstatiation of a variable with a datatype from interface namespace,  *
 *   even if this is derived from a base class inside an internal namespace*
 * - Call of member functions which are defined in the interface class     *
 *   definition ( header )                                                 *
 *                                                                         *
 * Pairing of internal and interface classes:                              *
 * - Outternal implementation in an :impl: subdirectory                     *
 * - Outterface in the parent directory of the corresponding internal class *
 * - Outterface class name IsoAgLib::iFoo_c maps to the internal class      *
 *   __IsoAgLib::Foo_c                                                     *
 *                                                                         *
 * AS A RULE: Use only classes with names beginning with small letter :i:  *
 ***************************************************************************/


#include "targetfilestreamoutput_c.h"
#include <djbiostypedefs.h>


#include <generalhelpers.h>
 

namespace __HAL
{
  extern "C"
  {
    /** include the BIOS specific header into __HAL */
    #include <commercial_BIOS/bios_Dj1/DjBiosMVT.h>
  }
}

#if defined(DEBUG)
  #ifdef SYSTEM_PC
    #include <iostream>
  #else
    #include <supplementary_driver/driver/rs232/impl/rs232io_c.h>
  #endif
  #include <IsoAgLib/util/impl/util_funcs.h>
#endif

//using namespace std;

TargetFileStreamOutput_c::TargetFileStreamOutput_c() :
  file_handle_(NULL), is_failed_(false)
{
}

TargetFileStreamOutput_c::~TargetFileStreamOutput_c()
{
  close();
}


bool TargetFileStreamOutput_c::open( CNAMESPACE::string& filename, FileMode_t rt_mode )
{
	return open( filename.c_str() ,  rt_mode );
}

//! open a output stream
bool TargetFileStreamOutput_c::open( const char* filename, FileMode_t rt_mode )
{

	#if defined(DEBUG) && DEBUG_FILESTREAMOUTPUT
		INTERNAL_DEBUG_DEVICE
		<< "Filestreamoutput: trying to open the file " <<  filename  
		<< " in mode " 
		//<< CNAMESPACE::hex 
		<< rt_mode 
		//<< CNAMESPACE::dec 
		<< INTERNAL_DEBUG_DEVICE_ENDL;
	#endif

	if (NULL != file_handle_) 
	{
		#if defined(DEBUG) && DEBUG_FILESTREAMOUTPUT
			INTERNAL_DEBUG_DEVICE
			<< "Filestreamoutput: Error : file_handle is already not NULL !!!!!!" << INTERNAL_DEBUG_DEVICE_ENDL;
		#endif
		return false;
	}

	CNAMESPACE::string mode_string;

	if (StreamOut & rt_mode) {
	mode_string.push_back('w');
	}

	if (mode_string.empty()) {
	return false;
	}

	file_handle_ = __HAL::DjBios_IOP_Open(filename, mode_string.c_str());
	#if defined(DEBUG) && DEBUG_FILESTREAMOUTPUT
		mui32_byteCount = 0;
		if (NULL == file_handle_)
		INTERNAL_DEBUG_DEVICE
		<< "Filestreamoutput: failed file open " << INTERNAL_DEBUG_DEVICE_ENDL;
	#endif
	


  bool result = (NULL != file_handle_);
  return result;
}

//  Operation: operator>>
//! Parameter:
//! @param ui8_data:
TargetFileStreamOutput_c& TargetFileStreamOutput_c::operator<<(uint8_t ui8_data)
{
  bool is_wrong_amount = (1 != __HAL::DjBios_IOP_Write(
                              file_handle_, 1, 1, &ui8_data));
  if (is_wrong_amount) {
    is_failed_ = true;
  }

	#if defined(DEBUG) && DEBUG_FILESTREAMINPUT
//		debugData[mui32_byteCount % DEBUG_ARRAY_SIZE] = ui8_data;
		mui32_byteCount++;
		if (mui32_byteCount % DEBUG_ARRAY_SIZE == 0)
		{
			INTERNAL_DEBUG_DEVICE << mui32_byteCount << " bytes written " ;
//			for (int i=0; i< DEBUG_ARRAY_SIZE; i++)
//			{
//				INTERNAL_DEBUG_DEVICE << getHex(debugData[i]);
//			}
			INTERNAL_DEBUG_DEVICE << INTERNAL_DEBUG_DEVICE_ENDL;
		}
	#endif

  return *this;
}

//! close a output stream
//! Parameter:
//! @param pathname if pathname != NULL => sync file and path
void TargetFileStreamOutput_c::close(const char* pathname)
{
  if (NULL != file_handle_) 
  {
    (void)__HAL::DjBios_IOP_Close(file_handle_);
	#if defined(DEBUG) && DEBUG_FILESTREAMOUTPUT
//		INTERNAL_DEBUG_DEVICE << mui32_byteCount << " bytes written " ;
//			for (int i=0; i< mui32_byteCount % DEBUG_ARRAY_SIZE; i++)
//			{
//				INTERNAL_DEBUG_DEVICE << getHex(debugData[i]);
//			}
//		INTERNAL_DEBUG_DEVICE << INTERNAL_DEBUG_DEVICE_ENDL;

		INTERNAL_DEBUG_DEVICE << "Filestreamoutput: closing file => bytes written : "  << mui32_byteCount << INTERNAL_DEBUG_DEVICE_ENDL;
	#endif

    file_handle_ = NULL;
  }
  is_failed_ = false;
}
