/*
  targetfilestreaminput_c.cpp

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include <generalhelpers.h>

#include "targetfilestreaminput_c.h"



#if defined(DEBUG)
  #ifdef SYSTEM_PC
    #include <iostream>
  #else
    #include <supplementary_driver/driver/rs232/impl/rs232io_c.h>
  #endif
  #include <IsoAgLib/util/impl/util_funcs.h>
#endif



namespace __HAL
{
  extern "C"
  {
      /** include the BIOS specific header into __HAL */
    #include <commercial_BIOS/bios_Dj1/DjBios1.h>
  }
}

#ifdef WIN32
#include <stdio.h>
#endif




//



//  #include "../djbiosdrawinterface.h"
//#include <IsoAgLib/hal/hal_typedef.h>



//#include <djbiostypedefs.h>



#ifndef _huge
#define _huge
#endif

TargetFileStreamInput_c::TargetFileStreamInput_c() :
  file_handle_(NULL), is_failed_(false)
{
}

TargetFileStreamInput_c::~TargetFileStreamInput_c()
{
  close();
}

//! open an input stream
bool TargetFileStreamInput_c::open( const char* filename, FileMode_t at_mode )
{

  #if DEBUG_FILESTREAMINPUT
  INTERNAL_DEBUG_DEVICE
  << "Filestreaminput: trying to open the file " <<  filename
  << " in mode 0x"
  //<< CNAMESPACE::hex
  << at_mode
  //<< CNAMESPACE::dec
  << INTERNAL_DEBUG_DEVICE_ENDL;
  #endif

  if (NULL != file_handle_) {
  #if DEBUG_FILESTREAMINPUT
    INTERNAL_DEBUG_DEVICE
    << "Filestreaminput: Error: file_handle is already not NULL!" << INTERNAL_DEBUG_DEVICE_ENDL;
  #endif
      return false;
  }

  CNAMESPACE::string mode_string;

  if (StreamIn & at_mode) {
    mode_string.push_back('r');
  }

  if (mode_string.empty()) {
    return false;
  }

  file_handle_ = __HAL::DjBios_IOP_Open(filename, mode_string.c_str());
  #if DEBUG_FILESTREAMINPUT
  mui32_byteCount = 0;
  if (NULL == file_handle_)
    INTERNAL_DEBUG_DEVICE
    << "Filestreaminput: failed fiel open" << INTERNAL_DEBUG_DEVICE_ENDL;
  #endif

  bool result = (NULL != file_handle_);
  return result;
};

bool TargetFileStreamInput_c::eof() const
{
  __HAL::enum_Bool bios_result = __HAL::DjBios_IOP_EoF(file_handle_);
  bool result = (__HAL::BIOS_FALSE != bios_result);
  return result;
}

//  Operation: operator>>
//! Parameter:
//! @param ui8_data:
TargetFileStreamInput_c& TargetFileStreamInput_c::operator>>(uint8_t &ui8_data)
{
  /** @TODO ON REQUEST HAL DJ: check if call to eof() can be omitted. (If DjBios_IOP_Read
      behaves like standard fread, then this should be the case.) */
  if (eof()) {
    is_failed_ = true;
  #if DEBUG_FILESTREAMINPUT
  INTERNAL_DEBUG_DEVICE << "Error reading EOF - bytes read : " << mui32_byteCount <<  INTERNAL_DEBUG_DEVICE_ENDL;
  #endif
    return *this;
  }

  bool is_wrong_amount = (1 != __HAL::DjBios_IOP_Read(
                              file_handle_, 1, 1, &ui8_data));
  if (is_wrong_amount) {
    is_failed_ = true;
  #if DEBUG_FILESTREAMINPUT
  INTERNAL_DEBUG_DEVICE << "Error reading EOF - bytes read : " << mui32_byteCount <<  INTERNAL_DEBUG_DEVICE_ENDL;
  #endif
  }

  #if DEBUG_FILESTREAMINPUT
//    INTERNAL_DEBUG_DEVICE << " >> 0x" << getHex(ui8_data);
//    debugData[mui32_byteCount % DEBUG_ARRAY_SIZE] = ui8_data;
    mui32_byteCount++;
    if (mui32_byteCount % DEBUG_ARRAY_SIZE == 0)
    {
      INTERNAL_DEBUG_DEVICE << mui32_byteCount << " bytes read " ;
      //for (int i=0; i< DEBUG_ARRAY_SIZE; i++)
      //{
      //  INTERNAL_DEBUG_DEVICE << getHex(debugData[i]);
      //}
      INTERNAL_DEBUG_DEVICE << INTERNAL_DEBUG_DEVICE_ENDL;
    }
  #endif

  return *this;
}

void TargetFileStreamInput_c::close()
{
  if (NULL != file_handle_) {
    (void)__HAL::DjBios_IOP_Close(file_handle_);
  #if DEBUG_FILESTREAMINPUT
//    INTERNAL_DEBUG_DEVICE << mui32_byteCount << " bytes read " ;
//      for (int i=0; i< mui32_byteCount % DEBUG_ARRAY_SIZE; i++)
//      {
//        INTERNAL_DEBUG_DEVICE << getHex(debugData[i]);
//      }
//    INTERNAL_DEBUG_DEVICE << INTERNAL_DEBUG_DEVICE_ENDL;

    INTERNAL_DEBUG_DEVICE << "Filestreaminput: closing file => bytes read : "  << mui32_byteCount << INTERNAL_DEBUG_DEVICE_ENDL;
  #endif

    file_handle_ = NULL;
  }
  is_failed_ = false;
}
