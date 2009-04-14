#include "component_tecu_prop_can.h"
#include "tutorial_settings.h"
#include <IsoAgLib/driver/can/icanio_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/iidentitem_c.h>

IsoAgLibTutorialTecu::TutorialTecu_c* p_tractorBridge;
IsoAgLib::iIdentItem_c* p_ident;

/* ISO identification */
static const uint8_t scui8_indGroup = 3;
static const uint8_t scui8_devClass = 2;
static const uint8_t sci8_devClassInst = 0;
static const uint8_t sci8_func = 24;
static const uint16_t sci16_manfCode = 0x7FF;
static const uint32_t sci32_serNo = 27;
static const uint32_t sci32_prefSA = 254;
static const uint32_t sci32_eepromAddr = 0x100;
static const uint8_t sci8_funcInst = 0;
static const uint8_t sci8_ecuInst = 1;
static const uint8_t sci8_selfConf = true;
static const uint8_t sci8_master = 0;
static const uint8_t scui_isoCanInst = 0;
static const uint8_t scui_internalCanInst = 1;

bool ecuMain() {

  if ( ! IsoAgLib::getIcanInstance( scui_isoCanInst ).init( IsoAgLibTutorial::scui_isoBusNumber ) ) {
    return false;
  }

  if ( ! IsoAgLib::getIcanInstance( scui_internalCanInst ).init( IsoAgLibTutorial::scui_internalBusNumber, IsoAgLibTutorial::scui_internalCanBaud ) ) {
    return false;
  }

  p_tractorBridge = new IsoAgLibTutorialTecu::TutorialTecu_c();
  if ( ! p_tractorBridge ) {
    return false;
  }

  IsoAgLib::getIcanInstance( scui_internalCanInst ).insertFilter( *p_tractorBridge, static_cast<MASK_TYPE>( 0xFFF0 ), static_cast<MASK_TYPE>( 0x0000 ), true, IsoAgLib::iIdent_c::ExtendedIdent);

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

  p_tractorBridge->init( p_ident );

  return true;
}

bool ecuShutdown() {
  delete p_tractorBridge;
  delete p_ident;
  return true;
}
