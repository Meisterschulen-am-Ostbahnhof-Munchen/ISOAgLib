/*
 * file:    tutorialPropCanSensors.cpp
 * created: 02/25/09 09:58:40 AM CET
 *
 * comment header added here
 *
 */

#include "component_sensors.h"

/* speed */
IsoAgLibTutorialPropCan::SpeedSensor_c::SpeedSensor_c() : mui_speed( mscui_minSpeed ), mb_updown( true ) {}
IsoAgLibTutorialPropCan::SpeedSensor_c::~SpeedSensor_c() {}
bool IsoAgLibTutorialPropCan::SpeedSensor_c::getData( IsoAgLibTutorial::TutorialInternalPkg_c& ar_pkg ) {
  ar_pkg.m_ident = IsoAgLibTutorial::InternalSpeed;

  if (( mui_speed < mscui_minSpeed ) || ( mui_speed >= mscui_maxSpeed ) )
    mb_updown = !mb_updown;

  if ( mb_updown ) {
    mui_speed += mscui_step;
  }
  else {
    mui_speed -= mscui_step;
  }

  ar_pkg.mu_tracData.Speed.real = mui_speed;
  ar_pkg.mu_tracData.Speed.theor = mui_speed; /* FIXME */

  return true;
}

/* distance */
IsoAgLibTutorialPropCan::DistanceSensor_c::DistanceSensor_c() : mui_distance( 0 ) {}

IsoAgLibTutorialPropCan::DistanceSensor_c::~DistanceSensor_c() {}

bool IsoAgLibTutorialPropCan::DistanceSensor_c::getData( IsoAgLibTutorial::TutorialInternalPkg_c& ar_pkg ) {
  ar_pkg.m_ident = IsoAgLibTutorial::InternalDistance;
  ar_pkg.mu_tracData.Distance.real = mui_distance;
  ar_pkg.mu_tracData.Distance.theor = mui_distance;
  mui_distance += 1000;
  return true;
}

/* pto */
IsoAgLibTutorialPropCan::PtoSensor_c::PtoSensor_c() : m_mode( modeFront ), mui_rpm( mscui_minRpm ), mb_updown( true ) {}

IsoAgLibTutorialPropCan::PtoSensor_c::~PtoSensor_c() {}

void IsoAgLibTutorialPropCan::PtoSensor_c::setMode( PtoMode_e a_mode ) {
  m_mode = a_mode;
  switch ( m_mode ) {
    case modeFront:
      mui_rpm = mscui_maxRpm;
      break;
    case modeRear:
      mui_rpm = mscui_minRpm;
      break;
  }
}

bool IsoAgLibTutorialPropCan::PtoSensor_c::getData( IsoAgLibTutorial::TutorialInternalPkg_c& ar_pkg ) {

  if (( mui_rpm < mscui_minRpm ) || ( mui_rpm >= mscui_maxRpm ) ) {
    mb_updown = !mb_updown;
  }

  if ( mb_updown ) {
    mui_rpm += mscui_step;
  }
  else {
    mui_rpm -= mscui_step;
  }

  switch ( m_mode ) {
    case modeFront:
      ar_pkg.m_ident = IsoAgLibTutorial::InternalPtoFront;
      ar_pkg.mu_tracData.PtoFront.front = mui_rpm;
      ar_pkg.mu_tracData.PtoFront.engaged = 1;
      break;
    case modeRear:
      ar_pkg.m_ident = IsoAgLibTutorial::InternalPtoRear;
      ar_pkg.mu_tracData.PtoRear.rear = mui_rpm;
      ar_pkg.mu_tracData.PtoRear.engaged = 1;
      break;
    default:
      return false;
  }

  return true;
}

/* power */
IsoAgLibTutorialPropCan::PowerSensor_c::PowerSensor_c() : mui_keySwitch( 1 ) { }

IsoAgLibTutorialPropCan::PowerSensor_c::~PowerSensor_c() { }

bool IsoAgLibTutorialPropCan::PowerSensor_c::getData( IsoAgLibTutorial::TutorialInternalPkg_c& ar_pkg ) {
  ar_pkg.m_ident = IsoAgLibTutorial::InternalPower;
  ar_pkg.mu_tracData.Power.keySwitch = mui_keySwitch;
  ar_pkg.mu_tracData.Power.powerMaxTime = ( HAL::getTime() % 200 ) + 50;
  return true;
}

/* eof tutorialPropCanSensors.cpp */
