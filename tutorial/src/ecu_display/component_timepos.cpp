#include <IsoAgLib/comm/Part7_ApplicationLayer/itimeposgps_c.h>

#include "component_timepos.h"

/* reception of time pos data */
IsoAgLibTutorialDisplay::TutorialDisplayTimePos_c::TutorialDisplayTimePos_c()
{
  setTimePeriod (250);
}


IsoAgLibTutorialDisplay::TutorialDisplayTimePos_c::~TutorialDisplayTimePos_c() {}

/* iSchedulerTask_c */
void IsoAgLibTutorialDisplay::TutorialDisplayTimePos_c::init( IsoAgLib::iIdentItem_c* ap_ident ) {
  IsoAgLib::getITimePosGpsInstance().config( &( ap_ident->isoName() ), IsoAgLib::IdentModeImplement );
  IsoAgLib::getITimePosGpsInstance().configGps( &( ap_ident->isoName() ), IsoAgLib::IdentModeImplement );
}

bool IsoAgLibTutorialDisplay::TutorialDisplayTimePos_c::timeEvent( void ) {
  checkTimeUpdate();
  checkPosUpdate();
  return true;
}

void IsoAgLibTutorialDisplay::TutorialDisplayTimePos_c::registerDisplay( iDisplay_c* ap_display ) {
  mvec_display.push_back( ap_display );
}

void IsoAgLibTutorialDisplay::TutorialDisplayTimePos_c::unRegisterDisplay( iDisplay_c* ap_display ) {
  for ( std::vector<iDisplay_c*>::iterator it = mvec_display.begin(); it != mvec_display.end(); ) {
    if (( *it ) == ap_display ) {
      it = mvec_display.erase( it );
    }
    else {
      ++it;
    }
  }
}

void IsoAgLibTutorialDisplay::TutorialDisplayTimePos_c::close( void ) { }

const char* IsoAgLibTutorialDisplay::TutorialDisplayTimePos_c::getTaskName() const {
  return const_cast<const char*>( "EcuDisplayTimePos" );
}

void IsoAgLibTutorialDisplay::TutorialDisplayTimePos_c::checkPosUpdate( bool ab_force ) {
  PosCtn_s c;
  c.lat = IsoAgLib::getITimePosGpsInstance().getGpsLatitudeDegree10Minus7();
  c.lon = IsoAgLib::getITimePosGpsInstance().getGpsLongitudeDegree10Minus7();

  if (( c != ms_lastPos ) || ab_force ) {
    ms_lastPos = c;
    sendVtPosValues( c );
  }
}

void IsoAgLibTutorialDisplay::TutorialDisplayTimePos_c::sendVtPosValues( const PosCtn_s& c ) {
  if ( ! mvec_display.empty() ) {
    for ( std::vector<iDisplay_c*>::iterator it = mvec_display.begin(); it != mvec_display.end(); ++it ) {
      ( *it )->handleNewPosValues( c.lat, c.lon );
    }
  }
}

void IsoAgLibTutorialDisplay::TutorialDisplayTimePos_c::checkTimeUpdate( bool ab_force ) {
  TimeCtn_s c;
  c.h = IsoAgLib::getITimePosGpsInstance().hourLocal();
  c.m = IsoAgLib::getITimePosGpsInstance().minuteLocal();
  c.s = IsoAgLib::getITimePosGpsInstance().second();

  if (( c != ms_lastTime ) || ab_force ) {
    ms_lastTime = c;
    sendVtTimeValues( c );
  }
}

void IsoAgLibTutorialDisplay::TutorialDisplayTimePos_c::sendVtTimeValues( const TimeCtn_s& c ) {
  if ( ! mvec_display.empty() ) {
    for ( std::vector<iDisplay_c*>::iterator it = mvec_display.begin(); it != mvec_display.end(); ++it ) {
      ( *it )->handleNewTimeValues( c.h, c.m, c.s );
    }
  }
}
