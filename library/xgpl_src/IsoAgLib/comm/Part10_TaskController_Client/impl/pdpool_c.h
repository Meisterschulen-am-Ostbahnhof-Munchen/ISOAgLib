/*
  pdpool_c.h: Base Storage container for storing PdBase_c instances

  (C) Copyright 2014 - 2019 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef _PDPOOL_C_H_
#define _PDPOOL_C_H_

#include <IsoAgLib/comm/Part10_TaskController_Client/impl/procdata/pdbase_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/pdconnection_c.h>

#ifndef NDEBUG
#include <algorithm>
#endif

namespace __IsoAgLib {


  class PdPool_c
  {
  public:
    PdPool_c( unsigned int reserveSize );

    void addPdBase( PdBase_c& pd );

    typedef STL_NAMESPACE::vector<PdBase_c*> PdBases_t;
    const PdBases_t &getPdList() const { return m_procDatas; }
  
  protected:
    PdBases_t m_procDatas;
  };
 

  inline
  PdPool_c::PdPool_c( unsigned int reserveSize )
    : m_procDatas()
  {
    m_procDatas.reserve( reserveSize );
  }


  inline void
  PdPool_c::addPdBase( PdBase_c &pd )
  {
    isoaglib_assert( STL_NAMESPACE::find( m_procDatas.begin(), m_procDatas.end(), &pd ) == m_procDatas.end() );
    isoaglib_assert( ( m_procDatas.size() < m_procDatas.capacity() ) && "Capacity to small, causes realloc in Release mode!" );

    m_procDatas.push_back( &pd );
  }


}

#endif
