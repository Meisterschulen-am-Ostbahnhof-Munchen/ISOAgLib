/*
  pdpool_c.h: Base Storage container for storing PdBase_c instances

  (C) Copyright 2014 - 2014 by OSB AG and developing partners

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
    PdPool_c();

    void addPdBase( PdBase_c& pd );

    typedef STL_NAMESPACE::list<PdBase_c*> PdList_t;
    const PdList_t &getPdList() const { return m_procDatas; }
  
  protected:
    PdList_t m_procDatas;
  };
 

  inline
  PdPool_c::PdPool_c()
    : m_procDatas()
  {
  }


  inline void
  PdPool_c::addPdBase( PdBase_c &pd )
  {
    isoaglib_assert( STL_NAMESPACE::find( m_procDatas.begin(), m_procDatas.end(), &pd ) == m_procDatas.end() );
    m_procDatas.push_back( &pd );
  }


}

#endif
