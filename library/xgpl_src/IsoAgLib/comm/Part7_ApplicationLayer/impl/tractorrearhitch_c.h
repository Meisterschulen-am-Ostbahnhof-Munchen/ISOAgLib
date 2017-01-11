/*
  tractorrearhitch_c.h:

  (C) Copyright 2016 - 2017 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef TRACTORREARHITCH_C_H
#define TRACTORREARHITCH_C_H

#include <IsoAgLib/comm/Part7_ApplicationLayer/impl/tractorhitch_c.h>


namespace __IsoAgLib
{

  class TractorRearHitch_c : public TractorHitch_c
  {
  private:
    TractorRearHitch_c() : TractorHitch_c( REAR_HITCH_STATE_PGN ) {}

    friend TractorRearHitch_c &getTractorRearHitchInstance( unsigned instance );
  };


  TractorRearHitch_c &getTractorRearHitchInstance( unsigned instance );

}
#endif
