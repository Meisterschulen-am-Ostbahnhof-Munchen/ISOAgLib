#include "tutorial_settings.h"
#include "component_gps_sensor.h"
#include <IsoAgLib/driver/can/icanio_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/iidentitem_c.h>

IsoAgLibTutorialGpsSensor::TutorialGpsSensor_c* p_gpsSensor;
IsoAgLib::iIdentItem_c* p_ident;

/* ISO identification */
static const uint8_t scui8_indGroup = 2;
static const uint8_t scui8_devClass = 7;
static const uint8_t sci8_devClassInst = 0;
static const uint8_t sci8_func = 25;
static const uint16_t sci16_manfCode = 0x7FF;
static const uint32_t sci32_serNo = 27;
static const uint32_t sci32_prefSA = 254;
static const uint32_t sci32_eepromAddr = 0x100;
static const uint8_t sci8_funcInst = 0;
static const uint8_t sci8_ecuInst = 1;
static const uint8_t sci8_selfConf = true;
static const uint8_t sci8_master = 0;


bool ecuMain() {

  if ( ! IsoAgLib::getIcanInstance().init( IsoAgLibTutorial::scui_isoBusNumber ) ) {
    return false;
  }

  p_gpsSensor = new IsoAgLibTutorialGpsSensor::TutorialGpsSensor_c();
  if ( ! p_gpsSensor ) {
    return false;
  }

  p_ident = new IsoAgLib::iIdentItem_c();

  p_ident->init(
    scui8_indGroup,
    scui8_devClass,
    sci8_devClassInst,
    sci8_func,
    sci16_manfCode,
    sci32_serNo,
    sci32_prefSA,
    sci32_eepromAddr,
    sci8_funcInst,
    sci8_ecuInst,
    sci8_selfConf,
    sci8_master );

  p_gpsSensor->init( p_ident );
  return true;
}

bool ecuShutdown() {
  delete p_gpsSensor;
  delete p_ident;
  return true;
}
