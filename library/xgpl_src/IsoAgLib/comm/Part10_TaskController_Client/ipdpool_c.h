/*
  ipdpool_c.h: interface for managing of local process data

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
#ifndef IPDPOOL_C_H
#define IPDPOOL_C_H

#include "impl/pdpool_c.h"
#include "ipdlocal_c.h"
#include "ipdremote_c.h"


namespace IsoAgLib {

  class iPdPool_c : private __IsoAgLib::PdPool_c
  {
  public:
    iPdPool_c( unsigned int reserveSize );

    void add( iPdLocal_c & );
    void add( iPdRemote_c & );

    friend class iTcClient_c;
  };


  inline
  iPdPool_c::iPdPool_c( unsigned int reserveSize )
    : PdPool_c( reserveSize )
  {
  }

  
  inline void
  iPdPool_c::add( iPdLocal_c &pdLocal )
  {
    PdPool_c::addPdBase( pdLocal  );
  }

  
  inline void
  iPdPool_c::add( iPdRemote_c &pdRemote )
  {
    PdPool_c::addPdBase( pdRemote );
  }

}

#endif
