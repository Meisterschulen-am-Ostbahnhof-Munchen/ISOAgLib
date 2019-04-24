/*
  tractorrearhitch_c.h:

  (C) Copyright 2016 - 2019 by OSB AG

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
