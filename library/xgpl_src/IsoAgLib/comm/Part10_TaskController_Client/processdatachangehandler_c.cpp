/*
  processdatachangehandler_c.cpp: base class for application handler
    classes, with handler methods which are called after each change
    access on process data from external systems

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include "processdatachangehandler_c.h"
#include "iprocdatalocal_c.h"
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/procdatalocal_c.h>

namespace IsoAgLib {

iProcDataLocal_c* EventSource_c::makeIProcDataLocal( void ) {
  return static_cast<iProcDataLocal_c*>(pc_src);
}

} // end of namespace IsoAgLib
