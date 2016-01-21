/*
  ignss_c.h: Receiving GNSS Position, Direction and Date/Time.

  (C) Copyright 2015 - 2015 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef IGNSS_C_H
#define IGNSS_C_H

#include <IsoAgLib/comm/Part5_NetworkManagement/iidentitem_c.h>
#include "ibasetypes.h"
#include "impl/gnss_c.h"

// Begin Namespace IsoAgLib
namespace IsoAgLib {


class iGnss_c : private __IsoAgLib::Gnss_c {
public:
  /** Retrieve the time of last position update (-1 if no position available (anymore, i.e. after timeout!)) */
  ecutime_t lastPositionUpdate() const { return Gnss_c::lastPositionUpdate(); }

  /** Retrieve the time of last date/time update (-1 if no date/time available (anymore, i.e. after timeout!)) */
  ecutime_t lastDateTimeUpdate() const { return Gnss_c::lastDateTimeUpdate(); }

  /** Retrieve the time of last direction update (-1 if no direction available (anymore, i.e. after timeout!)) */
  ecutime_t lastDirectionUpdate() const { return Gnss_c::lastDirectionUpdate(); }

  const IsoAgLib::iDateTime_s &getDateTime() const { return Gnss_c::getDateTime(); }

  /** deliver raw GPS Latitude [degree] with scaling 10.0e-7 */
  int32_t getGpsLatitudeDegree10Minus7(void) const { return Gnss_c::getGpsLatitudeDegree10Minus7();}

  /** deliver raw GPS Longitude [degree] with scaling 10.0e-7 */
  int32_t getGpsLongitudeDegree10Minus7(void) const { return Gnss_c::getGpsLongitudeDegree10Minus7();}

  /** deliver Minute GPS Latitude */
  float getGpsLatitudeMinute(void) const { return Gnss_c::getGpsLatitudeMinute();}

  /** deliver Minute GPS Longitude */
  float getGpsLongitudeMinute(void) const { return Gnss_c::getGpsLongitudeMinute();}

  /** deliver Degree GPS Latitude */
  float getGpsLatitudeDegree(void) const { return Gnss_c::getGpsLatitudeDegree();}

  /** deliver Degree GPS Longitude */
  float getGpsLongitudeDegree(void) const { return Gnss_c::getGpsLongitudeDegree();}

  /** deliver GPS Speed Over Ground as [cm/s] */
  uint16_t getGpsSpeedCmSec(void) const { return Gnss_c::getGpsSpeedCmSec(); }
  /** deliver GPS Course Over Ground [1x10E-4rad] */
  uint16_t getGpsCourseRad10Minus4(void) const { return Gnss_c::getGpsCourseRad10Minus4(); }

  /** deliver GPS receive qualitiy */
  IsoAgLib::IsoGnssMethod_t getGnssMode(void) const { return Gnss_c::getGnssMode();}

  /** deliver GPS altitude - [cm] */
  int32_t getGpsAltitudeCm(void) const { return Gnss_c::getGpsAltitudeCm();}

  /** deliver GPS Geodial Separation - [cm] */
  int32_t getGpsGeoidalSeparationCm ( void ) const { return Gnss_c::getGpsGeoidalSeparationCm(); }

  /** simply check for some sort of Differential signal */
  bool hasDifferentialPosition() const{ return Gnss_c::hasDifferentialPosition();}

  /** deliver GNSS type ( e.g. GPS, GLONASS or SBAS ) */
  IsoAgLib::IsoGnssType_t getGnssType(void) const { return Gnss_c::getGnssType();}

  /** deliver number of received satellites */
  uint8_t satelliteCnt() const { return Gnss_c::satelliteCnt();}

  /** deliver HDOP with scaling [1x10E-2] */
  int16_t hdop10Minus2() const { return Gnss_c::hdop10Minus2();}

  /** PDOP with scaling [1x10E-2] */
  int16_t pdop10Minus2() const { return Gnss_c::pdop10Minus2();}

  /** register an event handler that gets called for any incoming PGN.
      Please look into the implementation to see for which PGNs it is
      actually called.
      Note: Double registration will be allowed, whereas deregistration
            will remove all occurances. */
  void registerMsgEventHandler (iMsgEventHandler_c &handler) { Gnss_c::registerMsgEventHandler(handler); }

  /** deregister all event handlers matching the parameter
      @param arc_msgEventHandler Reference to an implementation of the
                                 handler class of type iMsgEventHandler_c */
  void deregisterMsgEventHandler (iMsgEventHandler_c &handler) { Gnss_c::deregisterMsgEventHandler(handler); }

private:
#if ( PRT_INSTANCE_CNT == 1 )
  friend iGnss_c& getIGnssInstance();
#endif
  friend iGnss_c& getIGnssInstance( unsigned instance );
};

#if ( PRT_INSTANCE_CNT == 1 )
  inline iGnss_c& getIGnssInstance()
  { return static_cast<iGnss_c&>(__IsoAgLib::getGnssInstance( 0 )); }
#endif
  inline iGnss_c& getIGnssInstance( unsigned instance )
  { return static_cast<iGnss_c&>(__IsoAgLib::getGnssInstance( instance )); }

}
#endif
