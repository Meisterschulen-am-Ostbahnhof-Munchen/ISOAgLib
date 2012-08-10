#include "tutorial_settings.h"
#include "component_tcclient.h"

// IsoAgLib
#include <IsoAgLib/comm/Part10_TaskController_Client/iprocess_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/iprocdatalocal_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/iidentitem_c.h>

// device description object pool
#include "component_tcclient_pool/tutorialDataSource-func.h"

// debug
#include <IsoAgLib/util/iassert.h>


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
IsoAgLibTutorialDataSource::TutorialDataSource_c::init( IsoAgLib::iIdentItem_c &arc_identItem )
{
  // Enable Default-Data-Logging
  c_defaultLogging.setProcessDataChangeHandler( this );

  const bool cb_registerSuccess =
    IsoAgLib::getIProcessInstance().getDevPropertyHandlerInstance().registerDevicePool(
      &arc_identItem, deviceDescription_en, ui32_arrayLength_en, true);

  (void)cb_registerSuccess;
  isoaglib_assert(cb_registerSuccess);

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
  c_workState.setMeasurementVal( ab_on ? 1 : 0);
}


void
IsoAgLibTutorialDataSource::TutorialDataSource_c::handleNewSensorAData( int32_t ai32_newData )
{
  c_setpointVolumeRate.setMeasurementVal( ai32_newData );
}


void
IsoAgLibTutorialDataSource::TutorialDataSource_c::handleNewSensorBData( int32_t ai32_newData )
{
  c_actualVolumeRate.setMeasurementVal( ai32_newData );
}


void
IsoAgLibTutorialDataSource::TutorialDataSource_c::processDefaultLoggingStart(
  const IsoAgLib::iIsoName_c& ac_callerIsoName)
{
  c_setpointVolumeRate.startDataLogging( IsoAgLib::Proc_c::MeasurementCommandTimeProp, 1000, ac_callerIsoName );
  c_actualVolumeRate.startDataLogging( IsoAgLib::Proc_c::MeasurementCommandTimeProp, 2000, ac_callerIsoName );
}
