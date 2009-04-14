#include <IsoAgLib/comm/Part7_ApplicationLayer/itimeposgps_c.h>

#include "component_gps_sensor.h"
#include <tutorial_settings.h>

/* FIXME add some real data here */
static const IsoAgLibTutorialGpsSensor::LatLon_s sc_positionData[] = {
  { 481375113, 115543127 },
  { 481374540, 115533685 },
  { 481363943, 115532398 },
  { 481361938, 115526390 },
  { 481359934, 115518665 },
  { 481357356, 115511798 },
  { 481348191, 115510940 },
  { 481345613, 115510940 },
  { 481339599, 115510511 },
  { 481331293, 115508365 },
  { 481318404, 115504932 },
  { 481304082, 115501928 },
  { 481304655, 115486049 },
  { 481323273, 115490770 },
  { 481324991, 115475749 },
  { 481306087, 115473175 },
  { 481295489, 115469741 },
  { 481286323, 115468454 },
  { 481283458, 115508794 },
  { 481294916, 115511798 },
  { 481302936, 115513086 },
  { 481322986, 115516090 },
  { 481336734, 115519094 },
  { 481342176, 115490341 },
  { 481349050, 115480041 },
  { 481362798, 115499782 },
  { 481373681, 115500211 },
  { 481374826, 115544414 }
};

IsoAgLibTutorialGpsSensor::TutorialGpsSensor_c::TutorialGpsSensor_c(): mui_pos( 0 ) {
  setTimePeriod( IsoAgLibTutorial::scui_gpsSensorTimePeriod );
}

IsoAgLibTutorialGpsSensor::TutorialGpsSensor_c::~TutorialGpsSensor_c() { }

void IsoAgLibTutorialGpsSensor::TutorialGpsSensor_c::init( IsoAgLib::iIdentItem_c* ap_ident ) {
  IsoAgLib::getITimePosGpsInstance().config( &( ap_ident->isoName() ), IsoAgLib::IdentModeTractor, 1 );
  IsoAgLib::getITimePosGpsInstance().configGps( &( ap_ident->isoName() ), IsoAgLib::IdentModeTractor );

}

/* iSchedulerTask */
bool IsoAgLibTutorialGpsSensor::TutorialGpsSensor_c::timeEvent() {

  updateGpsPos( );
  updateGpsTime( );

  return true;
}

void IsoAgLibTutorialGpsSensor::TutorialGpsSensor_c::close( void ) {
}

void IsoAgLibTutorialGpsSensor::TutorialGpsSensor_c::updateEarlierAndLatestInterval() {
  setEarlierInterval( getTimePeriod() * IsoAgLibTutorial::scf_gpsSensorEarlierTimeFactor );
  setLatestInterval( getTimePeriod() * IsoAgLibTutorial::scf_gpsSensorLatestTimeFactor );
}

const char* IsoAgLibTutorialGpsSensor::TutorialGpsSensor_c::getTaskName() const {
  return const_cast<const char*>( "EcuGpsSensor" );
}
/* eof iSchedulerTask */

bool IsoAgLibTutorialGpsSensor::TutorialGpsSensor_c::updateGpsPos() {

  if ( mui_pos > sizeof( sc_positionData ) / sizeof( struct LatLon_s ) )
    mui_pos = 0;

  IsoAgLib::getITimePosGpsInstance().setGpsLatitudeDegree10Minus7( sc_positionData[mui_pos].lat );
  IsoAgLib::getITimePosGpsInstance().setGpsLongitudeDegree10Minus7( sc_positionData[mui_pos].lon );

  mui_pos++;

  return true;
}

bool IsoAgLibTutorialGpsSensor::TutorialGpsSensor_c::updateGpsTime() {

  uint32_t ui32_time = HAL::getTime() / DEF_MSEC_PER_SEC; // sec

  uint8_t ui8_sec = ui32_time % DEF_SEC_PER_MIN;
  ui32_time = ( ui32_time - ui8_sec ) / DEF_SEC_PER_MIN;

  uint8_t ui8_min = ui32_time % DEF_MIN_PER_H;
  ui32_time = ( ui32_time - ui8_min ) / DEF_MIN_PER_H;

  uint32_t ui8_h = ui32_time % DEF_H_PER_D;

  IsoAgLib::getITimePosGpsInstance().setTimeUtc( ui8_h, ui8_min, ui8_sec );

  return true;
}

/* eof tutorialGpsSensor_c.cpp */
