/*
  streamoutput_c.h

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

#ifndef STREAMOUTPUT_C_H
#define STREAMOUTPUT_C_H


#include <IsoAgLib/isoaglib_config.h>

// +X2C includes
// ~X2C

//  +X2C Class 915 : StreamOutput_c
class StreamOutput_c
{

public:

  //  Operation: operator<<
  //! Parameter:
  //! @param ui8_data:
  virtual StreamOutput_c& operator<<(uint8_t ui8_data)=0;

  void put(uint8_t aui8_data) { operator<<(aui8_data); };

  //  Operation: eof
  virtual bool eof() const=0;

  // Operation: fail
  virtual bool fail() const=0;

  // Operation: good
  virtual bool good() const=0;

	virtual ~StreamOutput_c() {};
protected:

private:

}; // ~X2C


#endif // -X2C
