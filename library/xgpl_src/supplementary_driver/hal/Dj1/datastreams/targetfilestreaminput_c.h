/*
  targetfilestreaminput_c.h

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#ifndef TARGETFILESTREAMINPUT_C_H
#define TARGETFILESTREAMINPUT_C_H


#include <IsoAgLib/hal/hal_typedef.h>

#include <compilerswitches.h>


#include <string>
#include <supplementary_driver/hal/hal_datastreams.h>


// +X2C includes
// ~X2C

//  +X2C Class 915 : FileTargetFileStreamInput_c
class TargetFileStreamInput_c //: public STL_NAMESPACE::ifstream
{

public:
  TargetFileStreamInput_c();
  ~TargetFileStreamInput_c();

	//! open a input stream
	bool open( STL_NAMESPACE::string& filename, FileMode_t at_mode )
		{ return open( filename.c_str(), at_mode ); }
	//! open a input stream
	bool open( const char* filename, FileMode_t at_mode );
	//! close a input stream
	void close();
  
  //  Operation: eof
  virtual bool eof() const;

  // Operation: fail
  virtual bool fail() const { return is_failed_; };

  // Operation: good
  virtual bool good() const { return !eof() && !fail(); };

	//  Operation: operator>>
  //! Parameter:
  //! @param ui8_data:
  virtual TargetFileStreamInput_c& operator>>(uint8_t &ui8_data);
	
private:
  #if DEBUG_FILESTREAMINPUT
  uint32_t mui32_byteCount;
  uint8_t debugData[DEBUG_ARRAY_SIZE];
  #endif

  void *file_handle_;
  bool is_failed_;
}; // ~X2C

// TargetFileStreamInput_c & operator>> (TargetFileStreamInput_c &, uint8_t &ui8_data);


#endif // -X2C

