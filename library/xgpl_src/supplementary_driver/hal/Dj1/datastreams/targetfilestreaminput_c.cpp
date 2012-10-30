/*
  targetfilestreaminput_c.cpp

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include <generalhelpers.h>

#include "targetfilestreaminput_c.h"



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
  if (NULL != file_handle_) {
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
    return *this;
  }

  bool is_wrong_amount = (1 != __HAL::DjBios_IOP_Read(
                              file_handle_, 1, 1, &ui8_data));
  if (is_wrong_amount) {
    is_failed_ = true;
  }

  return *this;
}

void TargetFileStreamInput_c::close()
{
  if (NULL != file_handle_) {
    (void)__HAL::DjBios_IOP_Close(file_handle_);

    file_handle_ = NULL;
  }
  is_failed_ = false;
}
