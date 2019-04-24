/*
  pdbase_c.cpp: Class for handling Process Data

  (C) Copyright 2014 - 2019 by OSB AG

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
#include "pdbase_c.h"
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/procdata/connectedpd_c.h>

#if defined(_MSC_VER)
#pragma warning( disable : 4355 )
#endif


namespace __IsoAgLib {

 
  PdBase_c::PdBase_c()
    : m_ddi( 0xDEAD )
    , m_element( 0xDEAD )
    , m_connectedPds()
  {
  }


  void
  PdBase_c::addConnected( ConnectedPd_c& cPd )
  {
    m_connectedPds.push_front( &cPd );
  }


  void
  PdBase_c::removeConnected( ConnectedPd_c& cPd )
  {
    for( ConnectedPds_t::iterator i = m_connectedPds.begin(); i != m_connectedPds.end(); ++i )
    {
      if ( (*i) == &cPd )
      {
        m_connectedPds.erase(i);
        break;
      }
    } 
  }

}
