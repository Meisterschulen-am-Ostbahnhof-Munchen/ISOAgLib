/*
  targetfilestreamoutput_c.cpp

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
#include "targetfilestreamoutput_c.h"

using namespace std;

//! open a output stream
bool TargetFileStreamOutput_c::open( const TCHAR* filename, FileMode_t at_mode )
{
#if __GNUC__ < 3
	if ( ( at_mode & StreamIn ) != 0 ) return false;

	ios::openmode mode = ios::out;

	if ( ( at_mode & StreamAte    ) != 0 ) mode = ios::openmode( mode | ios::ate    );
	if ( ( at_mode & StreamApp    ) != 0 ) mode = ios::openmode( mode | ios::app    );
	if ( ( at_mode & StreamTrunc  ) != 0 ) mode = ios::openmode( mode | ios::trunc  );
	if ( ( at_mode & StreamBinary ) != 0 ) mode = ios::openmode( mode | ios::binary );

	static_cast<ofstream*>(this)->open( filename, mode );

	if ( ( static_cast<ofstream*>(this)->good() )
		&& ( static_cast<ofstream*>(this)->is_open() ) ) return true;
	else
		return false;
#else
	if ( ( at_mode & StreamIn ) != 0 ) return false;

	std::ios_base::openmode mode = std::ios_base::out;

	if ( ( at_mode & StreamAte    ) != 0 ) mode = std::ios_base::openmode( mode | std::ios_base::ate    );
	if ( ( at_mode & StreamApp    ) != 0 ) mode = std::ios_base::openmode( mode | std::ios_base::app    );
	if ( ( at_mode & StreamTrunc  ) != 0 ) mode = std::ios_base::openmode( mode | std::ios_base::trunc  );
	if ( ( at_mode & StreamBinary ) != 0 ) mode = std::ios_base::openmode( mode | std::ios_base::binary );

	static_cast<std::ofstream*>(this)->open( filename, mode );

	if ( ( static_cast<std::ofstream*>(this)->good() )
		&& ( static_cast<std::ofstream*>(this)->is_open() ) ) return true;
	else
		return false;
#endif
}

//  Operation: operator>>
//! Parameter:
//! @param ui8_data:
TargetFileStreamOutput_c& TargetFileStreamOutput_c::operator<<(uint8_t ui8_data)
{
//	(static_cast<std::ofstream*>(this))->put(ui8_data);
	(static_cast<ofstream*>(this))->put(ui8_data);

	return *this;
}

//! close a output stream
//! Parameter:
//! @param pathname if pathname != NULL => sync file and path
void TargetFileStreamOutput_c::close(const TCHAR* pathname)
{
  if (pathname)
  {
    (static_cast<std::ofstream*>(this))->flush();
#if defined(__GNUC__) && __GNUC__ < 3
    fsync((static_cast<std::ofstream*>(this))->rdbuf()->fd());
    // sync also directory entry (not done by fsync!)
    int fd = ::open(pathname, O_RDONLY);
    if (-1 != fd)
    {
      fsync(fd);
      ::close(fd);
    }
#else
    (static_cast<std::ofstream*>(this))->rdbuf()->pubsync();
#endif
  }
  static_cast<std::ofstream*>(this)->close();
}
