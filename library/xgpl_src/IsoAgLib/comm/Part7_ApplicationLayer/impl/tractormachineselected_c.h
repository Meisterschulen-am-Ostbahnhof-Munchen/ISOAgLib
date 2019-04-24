/*
  tractormachineselected_c.h:

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
#ifndef TRACTORMACHINESELECTED_C_H
#define TRACTORMACHINESELECTED_C_H

#include <IsoAgLib/comm/Part7_ApplicationLayer/impl/tractorcommonrx_c.h>


namespace __IsoAgLib
{

  class TractorMachineSelected_c : public TractorCommonRx_c
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

    IsoAgLib::IsoLimitFlag_t speedLimitStatus() const { return mt_speedLimitStatus; }
    IsoAgLib::IsoSpeedSourceFlag_t speedSource() const { return mt_speedSource; }

  private:
    TractorMachineSelected_c() : TractorCommonRx_c( TractorCommonRx_c::TIMEOUT_SENDING_NODE_J1939, MACHINE_SELECTED_SPEED_PGN ) {}

    virtual void setValues( const CanPkgExt_c& arc_data );
    virtual void resetValues();

  private:
    uint16_t mui16_speed;
    uint32_t mui32_distance;

    IsoAgLib::IsoLimitFlag_t mt_speedLimitStatus;
    IsoAgLib::IsoSpeedSourceFlag_t mt_speedSource;
    IsoAgLib::IsoDirectionFlag_t mt_direction;

    friend TractorMachineSelected_c &getTractorMachineSelectedInstance( unsigned instance );
  };


  TractorMachineSelected_c &getTractorMachineSelectedInstance( unsigned instance );


  inline bool
  TractorMachineSelected_c::isSpeedUsable() const
  {
    return( mui16_speed <= MAX_VAL_16 );
  }


  inline bool
  TractorMachineSelected_c::isDistanceUsable() const
  {
    return( mui32_distance <= MAX_VAL_32 );
  }

}
#endif
