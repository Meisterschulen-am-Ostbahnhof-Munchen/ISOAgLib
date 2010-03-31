/*
  filestreaminput_c.cpp

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include "filestreaminput_c.h"

#if DEBUG_FILESTREAMINPUT
  #ifdef SYSTEM_PC
    #include <iostream>
  #else
    #include <supplementary_driver/driver/rs232/impl/rs232io_c.h>
  #endif
  #include <IsoAgLib/util/impl/util_funcs.h>
#endif


//  Operation: open
bool
FileStreamInput_c::open ( STD_TSTRING& filename, FileMode_t at_mode )
{
  bool b_result = c_targetHandle.open( filename, at_mode );
  if (b_result) str_openedFile = filename;
  return b_result;
};



//  Operation: open
bool
FileStreamInput_c::open( const TCHAR* filename, FileMode_t at_mode )
{
  bool b_result = c_targetHandle.open( filename, at_mode);
  if (b_result) str_openedFile = filename;
  return b_result;
};



//  Operation: close
bool
FileStreamInput_c::close(bool b_deleteFile)
{
  c_targetHandle.close();
  if (b_deleteFile) {
    #if DEBUG_FILESTREAMINPUT
	  INTERNAL_DEBUG_DEVICE << TEXT("Removing file ") << str_openedFile << TEXT(".") << INTERNAL_DEBUG_DEVICE_ENDL;
    #endif
    #if defined(WINCE) //use this only for CE!
      return DeleteFile( str_openedFile.c_str() );
    #else
      return (remove( str_openedFile.c_str() ) == 0);
    #endif
  }
  return true; // success
};



//  Operation: operator>>
StreamInput_c&
FileStreamInput_c::operator>>(uint8_t& ui8_data)
{
  c_targetHandle.operator>>( ui8_data );
  return *this;
}
