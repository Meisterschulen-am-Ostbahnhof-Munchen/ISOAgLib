// Tutorial
#include "tutorial_settings.h"
#include "tutorial_identdatastorage.h"

// Components
#include "component_tcclient.h"
#include "component_simulatedsource.h"


// IsoAgLib
#include <IsoAgLib/comm/iisobus_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/iisomonitor_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/iidentitem_c.h>


IsoAgLib::iIdentItem_c* p_ident;
IsoAgLibTutorial::tutorialDataStorage_c* p_dataStorage;
IsoAgLibTutorialDataSource::TutorialDataSource_c* p_dataSource;
IsoAgLibTutorialDataSource::TutorialDataSourceSimulation_c* p_simulatedSource;

/* ISO identification */
static const uint16_t scui16_manfCode = 0x7FF;
static const uint32_t scui32_serNo = 0x1b;
static const uint8_t scui8_devClass = 0x5;
static const uint8_t scui8_devClassInst = 0;
static const uint8_t scui8_ecuInst = 0;
static const uint8_t scui8_func = 25;
static const uint8_t scui8_funcInst = 2;
static const uint8_t scui8_indGroup = 0x2;
static const uint8_t scui8_selfConf = 0x1;

static const uint16_t scui16_eepromBaseAddress = 0x100;

bool ecuMain()
{
  p_dataStorage = new IsoAgLibTutorial::tutorialDataStorage_c( scui16_eepromBaseAddress );
  p_dataSource = new IsoAgLibTutorialDataSource::TutorialDataSource_c();
  p_simulatedSource = new IsoAgLibTutorialDataSource::TutorialDataSourceSimulation_c();

  if ( !p_dataSource || !p_simulatedSource || !p_dataStorage )
    return false;

  if ( ! IsoAgLib::getIIsoBusInstance().init( IsoAgLibTutorial::scui_isoBusNumber ) ) {
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

  p_ident->init( c_isoname, *p_dataStorage, 0, NULL, true );

  p_ident->setEcuIdentification( "PartNr T", "Serial 127", "Frontside", "PD Supplier", "OSB AG" ); // dummy values
  p_ident->setSwIdentification( "IsoAgLib Data Source ECU Tutorial*" );
  p_ident->setCertificationData(
    2009, // certification year
    IsoAgLib::CertificationRevisionNotAvailable,
    IsoAgLib::CertificationLabTypeNotAvailable,
    42, // dummy laboratory id
    IsoAgLib::CertificationBitMask_t()
      .setBit (IsoAgLib::CertificationTcWsMaster)
      .setBit (IsoAgLib::CertificationMinEcu),
    8); // dummy reference number

  if ( ! IsoAgLib::getIisoMonitorInstance().registerIdentItem( *p_ident ) ) {
    return false;
  }

  p_dataSource->init( *p_ident );
  p_simulatedSource->init();

  p_simulatedSource->registerDataSource( *p_dataSource );

  return true;
}


bool ecuShutdown()
{
  p_simulatedSource->unRegisterDataSource( *p_dataSource );

  delete p_simulatedSource;
  delete p_dataSource;

  IsoAgLib::getIisoMonitorInstance().deregisterIdentItem( *p_ident );

  delete p_ident;
  delete p_dataStorage;

  IsoAgLib::getIIsoBusInstance().close();

  return true;
}
