/*
  ivtobjectstring_c.h

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
#ifndef IVTOBJECTSTRING_H
#define IVTOBJECTSTRING_H

#include "impl/vtobject_c.h"


namespace IsoAgLib {

class iVtObjectString_c : public __IsoAgLib::vtObject_c
{
public:
  iVtObjectString_c();
  ~iVtObjectString_c();

  virtual const char* getString() = 0;
};

} // IsoAgLib

#endif
