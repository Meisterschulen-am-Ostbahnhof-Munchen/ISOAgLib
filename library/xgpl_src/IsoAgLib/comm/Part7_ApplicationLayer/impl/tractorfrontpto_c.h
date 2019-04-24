/*
  tractorfrontpto_c.h:

  (C) Copyright 2017 - 2019 by OSB AG

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
#ifndef TRACTORFRONTPTO_C_H
#define TRACTORFRONTPTO_C_H

#include <IsoAgLib/comm/Part7_ApplicationLayer/impl/tractorpto_c.h>


namespace __IsoAgLib
{

  class TractorFrontPto_c : public TractorPto_c
  {
  private:
    TractorFrontPto_c() : TractorPto_c( FRONT_PTO_STATE_PGN ) {}

    friend TractorFrontPto_c &getTractorFrontPtoInstance( unsigned instance );
  };


  TractorFrontPto_c &getTractorFrontPtoInstance( unsigned instance );

}
#endif
