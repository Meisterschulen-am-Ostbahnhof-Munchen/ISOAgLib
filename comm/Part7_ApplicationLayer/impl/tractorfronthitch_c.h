/*
  tractorfronthitch_c.h:

  (C) Copyright 2016 - 2017 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef TRACTORFRONTHITCH_C_H
#define TRACTORFRONTHITCH_C_H

#include <IsoAgLib/comm/Part7_ApplicationLayer/impl/tractorhitch_c.h>


namespace __IsoAgLib
{

  class TractorFrontHitch_c : public TractorHitch_c
  {
  private:
    TractorFrontHitch_c() : TractorHitch_c( FRONT_HITCH_STATE_PGN ) {}

    friend TractorFrontHitch_c &getTractorFrontHitchInstance( unsigned instance );
  };


  TractorFrontHitch_c &getTractorFrontHitchInstance( unsigned instance );

}
#endif
