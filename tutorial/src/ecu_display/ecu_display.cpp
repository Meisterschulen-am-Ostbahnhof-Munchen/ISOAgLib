#include "tutorial_settings.h"

// components
#include "component_vtclient.h"
#include "component_tecu.h"
#include "component_timepos.h"

// IsoAgLib
#include <IsoAgLib/driver/can/icanio_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/iidentitem_c.h>


IsoAgLib::iIdentItem_c* p_ident;
IsoAgLibTutorialDisplay::TutorialDisplay_c* p_display;
IsoAgLibTutorialDisplay::TutorialDisplayTecu_c* p_tecu;
IsoAgLibTutorialDisplay::TutorialDisplayTimePos_c* p_timepos;

/* ISO identification */
static const uint8_t scui8_indGroup = 2;
static const uint8_t scui8_devClass = 5;
static const uint8_t sci8_devClassInst = 0;
static const uint8_t sci8_func = 27;
static const uint16_t sci16_manfCode = 0x7FF;
static const uint32_t sci32_serNo = 27;
static const uint32_t sci32_prefSA = 254;
static const uint32_t sci32_eepromAddr = 0x100;
static const uint8_t sci8_funcInst = 0;
static const uint8_t sci8_ecuInst = 1;
static const uint8_t sci8_selfConf = true;
static const uint8_t sci8_master = 0;


bool ecuMain() {

  p_tecu = new IsoAgLibTutorialDisplay::TutorialDisplayTecu_c();
  p_timepos = new IsoAgLibTutorialDisplay::TutorialDisplayTimePos_c();
  p_display = new IsoAgLibTutorialDisplay::TutorialDisplay_c();

  if( ! p_display || ! p_timepos || ! p_tecu ) {
    return false;
  }

  if ( ! IsoAgLib::getIcanInstance().init( IsoAgLibTutorial::scui_isoBusNumber ) ) {
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

  p_ident->setEcuIdentification( "PartNr D", "Serial 127", "Manufacturer" ); // dummy values
  p_ident->setSwIdentification( "IsoAgLib Display ECU Tutorial" );
  p_ident->setCertificationData(
    2009, // certification year
    IsoAgLib::CertificationRevisionNotAvailable,
    IsoAgLib::CertificationLabTypeNotAvailable,
    42, // dummy laboratory id
    IsoAgLib::CertificationBitMask_t()
      .setBit (IsoAgLib::CertificationVtWsMaster)
      .setBit (IsoAgLib::CertificationMinEcu),
    8); // dummy reference number

  p_display->init( *p_ident );
  p_tecu->init( p_ident );
  p_timepos->init( p_ident );

  p_tecu->registerDisplay( p_display->getDisplayHandler() );
  p_timepos->registerDisplay( p_display->getDisplayHandler() );

  return true;
}


bool ecuShutdown() {

  p_tecu->unRegisterDisplay( p_display->getDisplayHandler() );
  p_timepos->unRegisterDisplay( p_display->getDisplayHandler() );

  delete p_display;
  delete p_tecu;
  delete p_timepos;
  delete p_ident;

  return true;
}
