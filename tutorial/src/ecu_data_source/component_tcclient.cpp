#include "tutorial_settings.h"
#include "component_tcclient.h"

// IsoAgLib
#include <IsoAgLib/comm/Part5_NetworkManagement/iidentitem_c.h>
#include <IsoAgLib/comm/Part7_ProcessData/iprocess_c.h>
#include <IsoAgLib/comm/Part7_ProcessData/Local/Std/iprocdatalocal_c.h>

// device description object pool
#include "component_tcclient_pool/tutorialDataSource-func.h"

// debug
#include <assert.h>

using namespace IsoAgLib;


/*
 * Tutorial DataSource implementation
 * -------------------------------
 *
 * Here the main work is done of this component
 *
 * */
IsoAgLibTutorialDataSource::TutorialDataSource_c::TutorialDataSource_c()
{}


IsoAgLibTutorialDataSource::TutorialDataSource_c::~TutorialDataSource_c()
{}


void
IsoAgLibTutorialDataSource::TutorialDataSource_c::init()
{
  const bool cb_registerSuccess =
    getIProcessInstance().getDevPropertyHandlerInstance().registerDevicePool(
      &c_myIdent, deviceDescription_de, ui32_arrayLength_de, true);

  assert (cb_registerSuccess);

  handleNewWorkStateData (false);
  handleNewSensorAData (0);
  handleNewSensorBData (0);
}


/*
 * Tutorial DataSource iDataSource implementation
 * ----------------------------------------------
 *
 * Here the incoming measurement updates are handled.
 *
 * */

void
IsoAgLibTutorialDataSource::TutorialDataSource_c::handleNewWorkStateData( bool ab_on )
{
  c_workState.setMasterMeasurementVal( ab_on ? 1 : 0);
}


void
IsoAgLibTutorialDataSource::TutorialDataSource_c::handleNewSensorAData( int32_t ai32_newData )
{
  c_applicationRateSensorA.setMasterMeasurementVal( ai32_newData );
}


void
IsoAgLibTutorialDataSource::TutorialDataSource_c::handleNewSensorBData( int32_t ai32_newData )
{
  c_applicationRateSensorA.setMasterMeasurementVal( ai32_newData );
}

/* eof tutorialDisplay_c */
