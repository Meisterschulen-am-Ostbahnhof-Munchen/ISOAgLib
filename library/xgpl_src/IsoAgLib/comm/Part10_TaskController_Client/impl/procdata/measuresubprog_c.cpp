/*
  measuresubprog_c.cpp

  (C) Copyright 2009 - 2013 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#include "measuresubprog_c.h"
#include <IsoAgLib/scheduler/impl/schedulertask_c.h>
#include <cstdlib>
#include <IsoAgLib/util/impl/util_funcs.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/procdata/procdata_c.h>

#if defined(USE_BASE) || defined(USE_TRACTOR_MOVE)
  #include <IsoAgLib/comm/Part7_ApplicationLayer/impl/tracmove_c.h>
#endif


namespace __IsoAgLib {

MeasureSubprog_c::MeasureSubprog_c( IsoAgLib::ProcData::MeasurementCommand_t ren_type, int32_t ai32_increment )
  : mi32_lastVal( 0 )
  , mi32_increment( ai32_increment )
  , men_type( ren_type )
{
}


MeasureSubprog_c::MeasureSubprog_c( const MeasureSubprog_c& acrc_src )
  : mi32_lastVal( acrc_src.mi32_lastVal )
  , mi32_increment( acrc_src.mi32_increment )
  , men_type( acrc_src.men_type )
{
}


void
MeasureSubprog_c::start( int32_t ai32_lastVal, int32_t ai32_increment )
{
  // if wanted store given values (in both cases 0 is interpreted as not wanted)
  if (ai32_increment != 0)
    mi32_increment = ai32_increment;

  if (ai32_lastVal != 0)
    mi32_lastVal = ai32_lastVal;
}


bool
MeasureSubprog_c::updateTrigger( int32_t ai32_val )
{
  if ( ( type() == IsoAgLib::ProcData::MeasurementCommandOnChange ) && ( mi32_increment == 0 ) )
  { // special case: OnChange with value 0 means: SEND NO value; 1 meanse: send any change; ...
    return false;
  }
  else if (__IsoAgLib::abs(ai32_val - mi32_lastVal) >= mi32_increment)
  {
    mi32_lastVal = ai32_val;
    return true;
  }
  else
  {
    return false;
  }
}


int32_t
MeasureSubprog_c::nextTriggerTime( ProcData_c& ac_processData, int32_t ai32_val )
{
  switch( type() )
  {
    case IsoAgLib::ProcData::MeasurementCommandTimeProp:
      return (mi32_lastVal + mi32_increment - ai32_val);
	  
    case IsoAgLib::ProcData::MeasurementCommandDistProp:
    {
#if defined(USE_BASE) || defined(USE_TRACTOR_MOVE)
      const int32_t ci32_restDistance = mi32_lastVal + mi32_increment - ai32_val;
      const int32_t ci32_speed = __IsoAgLib::abs(getTracMoveInstance(ac_processData.getTcClientConnection().getIdentItem().getMultitonInst()).selectedSpeed());  // speed can be negative

      if (0 == ci32_speed)
        // speed == 0
        return 500;

      if ( ! getTracMoveInstance(ac_processData.getTcClientConnection().getIdentItem().getMultitonInst()).isSelectedSpeedUsable() )
      { // invalid speed, no tractor available
        return 200;
      }

      if (ci32_restDistance < 0)
        // should not happen if distance does only grow
        return 100;

      int32_t i32_nextTriggerTime = (ci32_restDistance * 1000 ) / ci32_speed; // distance in mm, div speed in mm/sec, result in msec

      if (i32_nextTriggerTime > 500)
      {
        i32_nextTriggerTime = 500;
      }

      return i32_nextTriggerTime;  // distance (in mm) div speed (in mm/sec) => time in msec
#else
      (void)ac_processData;
      return 200; // 200 msec
#endif
    }

    default:
      return 0;
  }
}

}
