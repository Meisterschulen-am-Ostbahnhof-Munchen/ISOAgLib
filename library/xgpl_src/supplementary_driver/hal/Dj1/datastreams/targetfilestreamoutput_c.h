/*
  targetfilestreamoutput_c.h

  (C) Copyright 2009 - 2011 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#ifndef TARGETFILESTREAMOUTPUT_C_H
#define TARGETFILESTREAMOUTPUT_C_H

#include <compilerswitches.h>
#include <IsoAgLib/hal/hal_typedef.h>
#include <supplementary_driver/hal/hal_datastreams.h>

#include <string>


//  +X2C Class 915 : FileTargetFileStreamOutput_c
class TargetFileStreamOutput_c
{

public:
  TargetFileStreamOutput_c();
  ~TargetFileStreamOutput_c();

  //! open a output stream
  bool open( CNAMESPACE::string& filename, FileMode_t at_mode );

  //! open a output stream
  bool open( const char* filename, FileMode_t at_mode );
  //! close a output stream
  //! @param pathname if pathname != NULL => sync file and path
  void close( const char* pathname = NULL);

  //  Operation: operator<<
  //! Parameter:
  //! @param ui8_data:
  virtual TargetFileStreamOutput_c& operator<<(uint8_t ui8_data);

  //  Operation: eof
  virtual bool eof() const { return false; };

  // Operation: fail
  virtual bool fail() const { return is_failed_; };

  // Operation: good
  virtual bool good() const { return !fail(); };

private:

  #if DEBUG_FILESTREAMOUTPUT
  uint32_t mui32_byteCount;
  uint8_t debugData[DEBUG_ARRAY_SIZE];
  #endif

  
  void *file_handle_;
  bool is_failed_;

}; // ~X2C

#endif // -X2C
