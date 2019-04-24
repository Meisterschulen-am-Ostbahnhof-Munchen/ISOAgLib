/*
  tractorgroundbased_c.h:

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
#ifndef TRACTORGROUNDBASED_C_H
#define TRACTORGROUNDBASED_C_H

#include <IsoAgLib/comm/Part7_ApplicationLayer/impl/tractorcommonrx_c.h>


namespace __IsoAgLib
{

  class TractorGroundBased_c : public TractorCommonRx_c
  {
  public:
    /** is looking for a valid speed value
        @return if speed is valid
      */
    inline bool isSpeedUsable() const;

    /** only use this value if "isSpeedUsable()" returns true!
        @return speed value [mm/s]
      */
    uint16_t speed() const { return mui16_speed; }

    /** is looking for a valid distance value
        @return if distance is valid
      */
    inline bool isDistanceUsable() const;

    /** @return distance value [mm]
      */
    uint32_t distance() const { return mui32_distance; }

    /** @return direction of travel
      */
    IsoAgLib::IsoDirectionFlag_t direction() { return mt_direction; }

  private:
    TractorGroundBased_c() : TractorCommonRx_c( TractorCommonRx_c::TIMEOUT_SENDING_NODE_J1939, GROUND_BASED_SPEED_DIST_PGN ) {}

    virtual void setValues( const CanPkgExt_c& arc_data );
    virtual void resetValues();

  private:
    uint16_t mui16_speed;
    uint32_t mui32_distance;
    IsoAgLib::IsoDirectionFlag_t mt_direction;

    friend TractorGroundBased_c &getTractorGroundBasedInstance( unsigned instance );
  };

  TractorGroundBased_c &getTractorGroundBasedInstance( unsigned instance );

  inline bool
  TractorGroundBased_c::isSpeedUsable() const
  {
    return( mui16_speed <= MAX_VAL_16 );
  }


  inline bool
  TractorGroundBased_c::isDistanceUsable() const
  {
    return( mui32_distance <= MAX_VAL_32 );
  }

}
#endif
