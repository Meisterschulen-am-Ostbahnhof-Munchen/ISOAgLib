/*
  measuresubprog_c.cpp

  (C) Copyright 2009 - 2019 by OSB AG

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Usage under Commercial License:
  Licensees with a valid commercial license may use this file
  according to their commercial license agreement. (To obtain a
  commercial license contact OSB AG via <http://isoaglib.com/en/contact>)

  Usage under GNU General Public License with exceptions for ISOAgLib:
  Alternatively (if not holding a valid commercial license)
  use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#include "measuresubprog_c.h"
#include <IsoAgLib/scheduler/impl/scheduler_c.h>
#include <IsoAgLib/util/impl/util_funcs.h>
#include <IsoAgLib/util/iassert.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/procdata/procdata_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/procdata/measureprog_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/pdconnection_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/tcclient_c.h>


namespace __IsoAgLib {



/// ------------- MeasureDistProp_c -------------



MeasureDistProp_c::MeasureDistProp_c( MeasureProg_c& measureProg )
  : SchedulerTask_c( 50, false )
  , m_measureProg( measureProg )
  , mui32_lastDistance( 0 )
  , mi32_increment( 0 )
{
  // If Distance-measurement is used there needs to be Provider registered!
  isoaglib_assert( getTcClientInstance( m_measureProg.connection().getMultitonInst() ).getProvider() );

  getSchedulerInstance().registerTask( *this, 0 );
}


MeasureDistProp_c::~MeasureDistProp_c()
{
  getSchedulerInstance().deregisterTask( *this );
}


void
MeasureDistProp_c::start( uint32_t currentDistance, int32_t ai32_increment )
{
  mi32_increment = ai32_increment;
  mui32_lastDistance = currentDistance;

  if( m_measureProg.minMaxLimitsPassed() )
    m_measureProg.sendValue();
}


bool
MeasureDistProp_c::updateTrigger( uint32_t currentDistance )
{
  // Distance should always be a monotone increasing function.
  // If not, well, we'll just send out the value. Doesn't matter too much.
  if( int32_t( currentDistance - mui32_lastDistance ) >= mi32_increment )
  {
    mui32_lastDistance = currentDistance;
    return true;
  }
  else
  {
    return false;
  }
}


int32_t
MeasureDistProp_c::nextTriggerTime( uint32_t distance )
{
  const int32_t multitonInst = m_measureProg.connection().getMultitonInst();
  const int32_t ci32_restDistance = mui32_lastDistance - distance + mi32_increment;
  const uint16_t cui16_speed = getTcClientInstance( multitonInst ).getProvider()->provideSpeed();

  // zero or no speed
  if( ( 0 == cui16_speed ) || ( cui16_speed > 0xFAFFu ) )
    return 500;

  if (ci32_restDistance < 0)
    return 100;

  int32_t i32_nextTriggerTime = (ci32_restDistance * 1000 ) / cui16_speed; // distance in mm, div speed in mm/sec, result in msec

  if (i32_nextTriggerTime > 500)
  {
    i32_nextTriggerTime = 500;
  }

  return i32_nextTriggerTime;  // distance (in mm) div speed (in mm/sec) => time in msec
}


void MeasureDistProp_c::timeEvent()
{
  const int32_t multitonInst = m_measureProg.connection().getMultitonInst();
  const uint32_t distance = getTcClientInstance( multitonInst ).getProvider()->provideDistance();
  
  const bool sendProcMsg = updateTrigger( distance );
  const int32_t nextTimePeriod = nextTriggerTime( distance );

  if( nextTimePeriod > 0 )
    setPeriod( nextTimePeriod, false );
  else
    setPeriod( 10, false ); // fallback, TODO later...

  if( sendProcMsg && m_measureProg.minMaxLimitsPassed() )
    m_measureProg.sendValue();
}



/// ------------- MeasureTimeProp_c -------------



MeasureTimeProp_c::MeasureTimeProp_c( MeasureProg_c& measureProg )
  : SchedulerTask_c( 50, false )
  , m_measureProg( measureProg )
  , mt_lastTime( 0 )
  , mi32_increment( 0 )
{
  getSchedulerInstance().registerTask( *this, 0 );
}


MeasureTimeProp_c::~MeasureTimeProp_c()
{
  getSchedulerInstance().deregisterTask( *this );
}


void
MeasureTimeProp_c::start( ecutime_t currentTime, int32_t ai32_increment )
{
  mi32_increment = ai32_increment;
  mt_lastTime = currentTime;

  setPeriod( ai32_increment, true );

  if( m_measureProg.minMaxLimitsPassed() )
    m_measureProg.sendValue();
}


bool
MeasureTimeProp_c::updateTrigger( ecutime_t currentTime )
{
  if( int32_t( currentTime - mt_lastTime ) >= mi32_increment )
  {
    mt_lastTime = currentTime;
    return true;
  }
  else
  {
    return false;
  }
}


int32_t
MeasureTimeProp_c::nextTriggerTime( ecutime_t currentTime )
{
  return int32_t(mt_lastTime - currentTime + mi32_increment );
}


void MeasureTimeProp_c::timeEvent()
{
  const ecutime_t now = System_c::getTime();
  const bool sendProcMsg = updateTrigger( now );
  const int32_t nextTimePeriod = nextTriggerTime( now );

  if( nextTimePeriod > 0 )
    setPeriod( nextTimePeriod, false );
  else
    setPeriod( 10, false ); // fallback, TODO later...

  if( sendProcMsg && m_measureProg.minMaxLimitsPassed() )
    m_measureProg.sendValue();
}



/// ------------- MeasureOnChange_c -------------



MeasureOnChange_c::MeasureOnChange_c( MeasureProg_c& measureProg )
  : m_measureProg( measureProg )
  , mi32_lastVal( 0 )
  , mi32_increment( 0 )
{
}


void
MeasureOnChange_c::start( int32_t ai32_lastVal, int32_t ai32_increment )
{
  mi32_increment = ai32_increment;
  mi32_lastVal = ai32_lastVal;

  if( m_measureProg.minMaxLimitsPassed() )
    m_measureProg.sendValue();
}


bool
MeasureOnChange_c::updateTrigger( int32_t ai32_val )
{
  if (__IsoAgLib::abs(ai32_val - mi32_lastVal) >= mi32_increment)
  {
    mi32_lastVal = ai32_val;
    return true;
  }
  else
  {
    return false;
  }
}


void
MeasureOnChange_c::setValue( int32_t value )
{
  const bool sendProcMsg = updateTrigger( value );

  if( sendProcMsg && m_measureProg.minMaxLimitsPassed() )
    m_measureProg.sendValue();
}



}
