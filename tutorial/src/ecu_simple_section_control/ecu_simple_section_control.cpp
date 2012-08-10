/** \example ecu_simple_section_control.cpp
 * This is the Section Control ECU of the Tutorial ECU Network.
 *
 * It handles a local network node (iIdentItem_c instance) and
 * the sub-components VT-Client and TC-Client.
 */

#include "tutorial_settings.h"
#include "tutorial_identdatastorage.h"

// components
#include "component_tcclient.h"

// IsoAgLib
#include <IsoAgLib/comm/iisobus_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/iidentitem_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/iisomonitor_c.h>

IsoAgLib::iIdentItem_c* p_ident = 0;
IsoAgLibTutorial::tutorialDataStorage_c* p_dataStorage = 0;
#ifndef USE_DYNAMIC_TCCLIENT
IsoAgLibTutorialSectionControl::TutorialSectionControlTc_c* p_task = 0;
#endif

/* ISO identification */
static const uint16_t scui16_manfCode = 0x165;
static const uint32_t scui32_serNo = 27;
static const uint8_t scui8_devClass = 6;
static const uint8_t scui8_devClassInst = 0;
static const uint8_t scui8_ecuInst = 0;
static const uint8_t scui8_func = 0x80;
static const uint8_t scui8_funcInst = 0;
static const uint8_t scui8_indGroup = 2;
static const uint8_t scui8_selfConf = 1;

static const uint16_t scui16_eepromBaseAddress = 0x80;

bool ecuMain() {

  p_dataStorage = new IsoAgLibTutorial::tutorialDataStorage_c( scui16_eepromBaseAddress );
#ifndef USE_DYNAMIC_TCCLIENT
  p_task = new IsoAgLibTutorialSectionControl::TutorialSectionControlTc_c();
#endif

  if( ! p_dataStorage ) {
    return false;
  }

  if ( ! IsoAgLib::getIIsoBusInstance().init (IsoAgLibTutorial::scui_isoBusNumber) ) {
    return false;
  }

  p_ident = new IsoAgLib::iIdentItem_c();

  const IsoAgLib::iIsoName_c c_isoname (
      scui8_selfConf,
      scui8_indGroup,
      scui8_devClass,
      scui8_devClassInst,
      scui8_func,
      scui16_manfCode,
      scui32_serNo,
      scui8_funcInst,
      scui8_ecuInst );

  p_ident->init( c_isoname, *p_dataStorage, 0 , NULL, true );

  p_ident->setEcuIdentification( "PartNr D", "Serial 127", "Frontside", "Display", "OSB AG" ); // dummy values
  p_ident->setSwIdentification( "IsoAgLib Simple Section Control ECU Tutorial*" );
  p_ident->setCertificationData(
    2009, // certification year
    IsoAgLib::CertificationRevisionNotAvailable,
    IsoAgLib::CertificationLabTypeNotAvailable,
    42, // dummy laboratory id
    IsoAgLib::CertificationBitMask_t()
      .setBit (IsoAgLib::CertificationVtWsMaster)
      .setBit (IsoAgLib::CertificationMinEcu),
    8); // dummy reference number
  
  if ( ! IsoAgLib::getIisoMonitorInstance().registerIdentItem( *p_ident ) ) {
    return false;
  }
#ifndef USE_DYNAMIC_TCCLIENT
  p_task->init( *p_ident );
#else
  getTcClientDynamic().InitDpdAndRegisterDDOP(p_ident);
#endif
  return true;
}


bool ecuShutdown() {
#ifndef USE_DYNAMIC_TCCLIENT
  delete p_task;
#endif

  IsoAgLib::getIisoMonitorInstance().deregisterIdentItem( *p_ident );

  delete p_ident;
  delete p_dataStorage;

  if (!IsoAgLib::getIIsoBusInstance().close())
    return false;

  return true;
}
