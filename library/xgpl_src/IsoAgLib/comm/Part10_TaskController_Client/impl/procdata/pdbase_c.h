/*
  pdbase_c.h: Base class for handling Process Data

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
#ifndef PDBASE_C_H
#define PDBASE_C_H

#include <IsoAgLib/isoaglib_config.h>
#include <list>

namespace __IsoAgLib {

  class PdConnection_c;
  class ConnectedPd_c;


  class PdBase_c
  {
  public:
    PdBase_c();
    PdBase_c( uint16_t _ddi, uint16_t _element );
    virtual ~PdBase_c() {}

    uint16_t DDI()     const { return m_ddi; }
    uint16_t element() const { return m_element; }

    virtual ConnectedPd_c &createConnectedPd( PdConnection_c & ) = 0;

    void addConnected( ConnectedPd_c& );
    void removeConnected( ConnectedPd_c& );

    typedef STL_NAMESPACE::list<ConnectedPd_c*> ConnectedPds_t;
    ConnectedPds_t &connectedPds() { return m_connectedPds; }

  protected:
    void init( uint16_t ddi, uint16_t element );

  private:
    uint16_t m_ddi;
    uint16_t m_element;

    ConnectedPds_t m_connectedPds; // only cached, do not 'own' the instances

    /** not copyable */
    PdBase_c( const PdBase_c& );
    PdBase_c& operator=( const PdBase_c& );
  };


  inline
  PdBase_c::PdBase_c( uint16_t _ddi, uint16_t _element )
    : m_ddi( _ddi )
    , m_element( _element )
    , m_connectedPds()
  {
  }


  inline void
  PdBase_c::init( uint16_t _ddi, uint16_t _element )
  {
    m_ddi = _ddi;
    m_element = _element;
  }

}

#endif
