#include "component_proprietary_can.h"

#include <tutorial_settings.h>

#include <IsoAgLib/driver/can/icanio_c.h>

IsoAgLibTutorialPropCan::TutorialPropCan_c::TutorialPropCan_c() {
  setTimePeriod( IsoAgLibTutorial::scui_propCanTimePeriod );
}

IsoAgLibTutorialPropCan::TutorialPropCan_c::~TutorialPropCan_c() { }

bool IsoAgLibTutorialPropCan::TutorialPropCan_c::timeEvent( void ) {
  if ( ! mvec_sensors.empty() ) {
    for ( std::vector<iPropCanSensor_c*>::iterator it = mvec_sensors.begin(); it != mvec_sensors.end(); ++it ) {
      if (( *it )->getData( mc_internalData ) ) {
        IsoAgLib::getIcanInstance() << mc_internalData;
      }
    }
  }

  return true;
}

void IsoAgLibTutorialPropCan::TutorialPropCan_c::registerSensor( iPropCanSensor_c* ap_sensor ) {
  if ( 0 != ap_sensor )
    mvec_sensors.push_back( ap_sensor );
}

void IsoAgLibTutorialPropCan::TutorialPropCan_c::close( void ) {
  mvec_sensors.clear();
}

void IsoAgLibTutorialPropCan::TutorialPropCan_c::updateEarlierAndLatestInterval() {
  setEarlierInterval( getTimePeriod() * IsoAgLibTutorial::scf_propCanEarlierTimeFactor );
  setLatestInterval( getTimePeriod() * IsoAgLibTutorial::scf_propCanLatestTimeFactor );
}


const char* IsoAgLibTutorialPropCan::TutorialPropCan_c::getTaskName() const {
  return const_cast<const char*>( "EcuPropCan" );
}
/* eof tutorialPropCan_c.cpp */
