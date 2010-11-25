/** \example ecu_display.cpp
 * This is the Display ECU of the Tutorial ECU Network.
 *
 * It handles a local network node (iIdentItem_c instance) and
 * the sub-components Tractor-ECU, VT-Client and GPS.
 */

#include "tutorial_settings.h"
#include "tutorial_identdatastorage.h"

// components
#include "component_vtclient.h"
#include "component_tecu.h"
#include "component_timepos.h"

// IsoAgLib
#include <IsoAgLib/comm/iisobus_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/iidentitem_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/iisomonitor_c.h>


IsoAgLib::iIdentItem_c* p_ident = 0;
IsoAgLibTutorial::tutorialIdentDataStorage_c* p_identDataStorage = 0;
IsoAgLibTutorialDisplay::TutorialDisplay_c* p_display = 0;
IsoAgLibTutorialDisplay::TutorialDisplayTecu_c* p_tecu = 0;
IsoAgLibTutorialDisplay::TutorialDisplayTimePos_c* p_timepos = 0;

/* ISO identification */
static const uint16_t scui16_manfCode = 0x7FF;
static const uint32_t scui32_serNo = 27;
static const uint8_t scui8_devClass = 5;
static const uint8_t scui8_devClassInst = 0;
static const uint8_t scui8_ecuInst = 1;
static const uint8_t scui8_func = 27;
static const uint8_t scui8_funcInst = 0;
static const uint8_t scui8_indGroup = 2;
static const uint8_t scui8_selfConf = 1;

static const uint16_t scui16_eepromBaseAddress = 0x80;

bool ecuMain() {

  p_identDataStorage = new IsoAgLibTutorial::tutorialIdentDataStorage_c( scui16_eepromBaseAddress );
  p_tecu = new IsoAgLibTutorialDisplay::TutorialDisplayTecu_c();
  p_timepos = new IsoAgLibTutorialDisplay::TutorialDisplayTimePos_c();
  p_display = new IsoAgLibTutorialDisplay::TutorialDisplay_c();

  if( ! p_display || ! p_timepos || ! p_tecu || ! p_identDataStorage ) {
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

  p_ident->init( c_isoname, *p_identDataStorage, 0 );

  p_ident->setEcuIdentification( "PartNr D", "Serial 127", "Frontside", "Display", "OSB AG" ); // dummy values
  p_ident->setSwIdentification( "IsoAgLib Display ECU Tutorial*" );
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

  IsoAgLib::getIisoMonitorInstance().deregisterIdentItem( *p_ident );

  delete p_ident;
  delete p_identDataStorage;

  IsoAgLib::getIIsoBusInstance().close();

  return true;
}
