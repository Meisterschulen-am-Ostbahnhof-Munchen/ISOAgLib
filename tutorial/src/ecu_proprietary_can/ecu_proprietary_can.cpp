#include "tutorial_settings.h"
#include "component_proprietary_can.h"
#include "component_sensors.h"

#include <IsoAgLib/driver/can/icanio_c.h>

IsoAgLibTutorialPropCan::TutorialPropCan_c* p_propCan;
IsoAgLibTutorialPropCan::SpeedSensor_c* p_speedSensor;
IsoAgLibTutorialPropCan::DistanceSensor_c* p_distanceSensor;
IsoAgLibTutorialPropCan::PtoSensor_c* p_ptoFrontSensor;
IsoAgLibTutorialPropCan::PtoSensor_c* p_ptoRearSensor;
IsoAgLibTutorialPropCan::PowerSensor_c* p_powerSensor;

bool ecuMain() {

  p_propCan = new IsoAgLibTutorialPropCan::TutorialPropCan_c();

  if ( ! IsoAgLib::getIcanInstance().init( IsoAgLibTutorial::scui_internalBusNumber, IsoAgLibTutorial::scui_internalCanBaud ) ) {
    return false;
  }

  p_speedSensor = new IsoAgLibTutorialPropCan::SpeedSensor_c();
  p_distanceSensor = new IsoAgLibTutorialPropCan::DistanceSensor_c();
  p_ptoFrontSensor = new IsoAgLibTutorialPropCan::PtoSensor_c();
  p_ptoFrontSensor->setMode( IsoAgLibTutorialPropCan::PtoSensor_c::modeFront );
  p_ptoRearSensor = new IsoAgLibTutorialPropCan::PtoSensor_c();
  p_ptoRearSensor->setMode( IsoAgLibTutorialPropCan::PtoSensor_c::modeRear );
  p_powerSensor = new IsoAgLibTutorialPropCan::PowerSensor_c();

  p_propCan->registerSensor( p_speedSensor );
  p_propCan->registerSensor( p_distanceSensor );
  p_propCan->registerSensor( p_ptoFrontSensor );
  p_propCan->registerSensor( p_ptoRearSensor );
  p_propCan->registerSensor( p_powerSensor );

  return true;
}

bool ecuShutdown() {
  p_propCan->close();
  delete p_propCan;

  delete p_speedSensor;
  delete p_distanceSensor;
  delete p_ptoFrontSensor;
  delete p_ptoRearSensor;
  delete p_powerSensor;

  return true;
}
