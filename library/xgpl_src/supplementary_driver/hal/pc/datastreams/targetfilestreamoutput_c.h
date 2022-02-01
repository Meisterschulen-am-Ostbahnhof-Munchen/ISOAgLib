/*
  targetfilestreamoutput_c.h

  (C) Copyright 2009 - 2019 by OSB AG

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Usage under Commercial License:
  Licensees with a valid commercial license may use this file
  according to their commercial license agreement. (To obtain a
  commercial license contact OSB AG via <http://isoaglib.com/en/contact>)

  Usage under GNU General Public License with exceptions for ISOAgLib:
  Alternatively (if not holding a valid commercial license)
  use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#ifndef TARGETFILESTREAMOUTPUT_C_H
#define TARGETFILESTREAMOUTPUT_C_H

#include <IsoAgLib/isoaglib_config.h>
#include <fstream>
#include <supplementary_driver/hal/hal_datastreams.h>
#include <string>

#ifndef WIN32
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#endif
// +X2C includes
// ~X2C

//  +X2C Class 915 : FileTargetFileStreamOutput_c
class TargetFileStreamOutput_c : public std::ofstream
{

public:
	//! open a output stream
	bool open( STD_TSTRING& filename, FileMode_t at_mode ){ return open( filename.c_str(), at_mode );};
	//! open a output stream
	bool open( const TCHAR* filename, FileMode_t at_mode );
	//! close a output stream
  //! @param pathname if pathname != NULL => sync file and path
	void close( const TCHAR* pathname = NULL);

  //  Operation: operator<<
  //! Parameter:
  //! @param ui8_data:
  virtual TargetFileStreamOutput_c& operator<<(uint8_t ui8_data);

  //  Operation: eof
  virtual bool eof() const { return static_cast<const std::ofstream*>(this)->eof();};

  // Operation: fail
  virtual bool fail() const { return static_cast<const std::ofstream*>(this)->fail();};

  // Operation: good
  virtual bool good() const { return static_cast<const std::ofstream*>(this)->good();};

}; // ~X2C

#endif // -X2C
