/*
  targetfilestreamoutput_c.cpp

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include <generalhelpers.h>

#include "targetfilestreamoutput_c.h"
#include <djbiostypedefs.h>




namespace __HAL
{
  extern "C"
  {
    /** include the BIOS specific header into __HAL */
    #include <commercial_BIOS/bios_Dj1/DjBios1.h>
  }
}


TargetFileStreamOutput_c::TargetFileStreamOutput_c() :
  file_handle_(NULL), is_failed_(false)
{
}

TargetFileStreamOutput_c::~TargetFileStreamOutput_c()
{
  close();
}


bool TargetFileStreamOutput_c::open( CNAMESPACE::string& filename, FileMode_t at_mode )
{
  return open( filename.c_str() ,  at_mode );
}

//! open a output stream
bool TargetFileStreamOutput_c::open( const char* filename, FileMode_t at_mode )
{
  if (NULL != file_handle_)
  {
    return false;
  }

  CNAMESPACE::string mode_string;

  if (StreamOut & at_mode) {
  mode_string.push_back('w');
  }

  if (mode_string.empty()) {
  return false;
  }

  file_handle_ = __HAL::DjBios_IOP_Open(filename, mode_string.c_str());

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

    file_handle_ = NULL;
  }
  is_failed_ = false;
}
