#include "component_simulatedsource.h"

#include <IsoAgLib/driver/system/isystem_c.h>

// c'tor
IsoAgLibTutorialDataSource::TutorialDataSourceSimulation_c::TutorialDataSourceSimulation_c()
  : mi32_simulatedSensorA(0)
  , mi32_simulatedSensorB(0)
  , mb_simulatedWorkState(false)
{
  setTimePeriod (1000); // 1s
}

// d'tor
IsoAgLibTutorialDataSource::TutorialDataSourceSimulation_c::~TutorialDataSourceSimulation_c()
{ // nop
}


void IsoAgLibTutorialDataSource::TutorialDataSourceSimulation_c::init()
{ // nop
}


/* iSchedulerTask_c */
bool IsoAgLibTutorialDataSource::TutorialDataSourceSimulation_c::timeEvent( void )
{
  /// First, get the new sensor values
  mi32_simulatedSensorA += 3;
  mi32_simulatedSensorB = HAL::getTime();
  mb_simulatedWorkState = (((HAL::getTime() / 30000) % 2) == 1); // change workstate all 30 seconds.

  /// Second, update all registered DataSources
  for ( std::vector<iDataSource_c*>::iterator it = mvec_dataSource.begin(); it != mvec_dataSource.end(); ++it ) {
    ( *it )->handleNewWorkStateData( mb_simulatedWorkState );
    ( *it )->handleNewSensorAData( mi32_simulatedSensorA );
    ( *it )->handleNewSensorBData( mi32_simulatedSensorB );
  }

  return true;
}


void IsoAgLibTutorialDataSource::TutorialDataSourceSimulation_c::registerDataSource( iDataSource_c& ar_dataSource ) {
  mvec_dataSource.push_back( &ar_dataSource );
}


void IsoAgLibTutorialDataSource::TutorialDataSourceSimulation_c::unRegisterDataSource( iDataSource_c& ar_dataSource ) {
  for ( std::vector<iDataSource_c*>::iterator it = mvec_dataSource.begin(); it != mvec_dataSource.end(); ++it ) {
    if (( *it ) == &ar_dataSource ) {
      it = mvec_dataSource.erase( it );
    }
    else {
      ++it;
    }
  }
}


void IsoAgLibTutorialDataSource::TutorialDataSourceSimulation_c::close( void )
{ // nop
}


const char* IsoAgLibTutorialDataSource::TutorialDataSourceSimulation_c::getTaskName() const {
  return const_cast<const char*>( "EcuDataSourceSimulatedSource" );
}

