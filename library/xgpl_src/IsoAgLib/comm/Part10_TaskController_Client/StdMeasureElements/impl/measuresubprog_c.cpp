/*
  measuresubprog_c.cpp: Every increment type of a measure prog is
    managed by a MeasureSubprog_c instance

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

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

#if defined(USE_BASE) || defined(USE_TRACTOR_MOVE)
  #include <IsoAgLib/comm/Part7_ApplicationLayer/impl/tracmove_c.h>
#endif

namespace __IsoAgLib {

MeasureSubprog_c::MeasureSubprog_c(Proc_c::type_t ren_type, Proc_c::doSend_t ren_doSend, int32_t ai32_increment MULTITON_INST_PARAMETER_DEF_WITH_COMMA)
: ClientBase( MULTITON_INST_PARAMETER_USE ),
  mi32_lastVal( 0 ),
  mi32_increment( ai32_increment ),
  mb_started( false ),
  men_type( ren_type ),
  men_doSend( ren_doSend )
{
}

const MeasureSubprog_c& MeasureSubprog_c::operator=(const MeasureSubprog_c& acrc_src){
  mb_started = acrc_src.mb_started;
  men_type = acrc_src.men_type;
  men_doSend = acrc_src.men_doSend;
  mi32_increment = acrc_src.mi32_increment;
  mi32_lastVal = acrc_src.mi32_lastVal;
  return acrc_src;
}

MeasureSubprog_c::MeasureSubprog_c(const MeasureSubprog_c& acrc_src)
: ClientBase(acrc_src)
{
  mb_started = acrc_src.mb_started;
  men_type = acrc_src.men_type;
  men_doSend = acrc_src.men_doSend;
  mi32_increment = acrc_src.mi32_increment;
  mi32_lastVal = acrc_src.mi32_lastVal;
}

MeasureSubprog_c::~MeasureSubprog_c(){
}

void MeasureSubprog_c::start(int32_t ai32_lastVal, int32_t ai32_increment){
  // if wanted store given values (in both cases 0 is interpreted as not wanted)
  if (ai32_increment != 0) mi32_increment = ai32_increment;
  if (ai32_lastVal != 0) mi32_lastVal = ai32_lastVal;

  // register as started
  mb_started = true;
}

bool MeasureSubprog_c::updateTrigger(int32_t ai32_val){
  if ( ( type() == Proc_c::OnChange ) && ( mi32_increment == 0 ) )
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

int32_t MeasureSubprog_c::nextTriggerTime(int32_t ai32_val)
{
  switch (type())
  {
    case Proc_c::TimeProp:
      return (mi32_lastVal + mi32_increment - ai32_val);
    case Proc_c::DistProp:
    {
#if defined(USE_BASE) || defined(USE_TRACTOR_MOVE)
      const int32_t ci32_restDistance = mi32_lastVal + mi32_increment - ai32_val;
      const int32_t ci32_speed = __IsoAgLib::abs(getTracMoveInstance4Comm().selectedSpeed());  // speed can be negative

      if (0 == ci32_speed)
        // speed == 0
        return 500;

      if ( ! getTracMoveInstance4Comm().isSelectedSpeedUsable() )
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
      return 200; // 200 msec
#endif
    }
    default:
      return 0;
  }
}

int32_t MeasureSubprog_c::calcCompVal()const{
  // for Subprog two items are considered equiv if type is identical
  return (men_type);
}

bool MeasureSubprog_c::operator==(const MeasureSubprog_c& acrc_right)const{
  return (calcCompVal() == acrc_right.calcCompVal());
}

bool MeasureSubprog_c::operator!=(const MeasureSubprog_c& acrc_right)const{
  return (calcCompVal() != acrc_right.calcCompVal());
}

bool MeasureSubprog_c::operator<(const MeasureSubprog_c& acrc_right)const{
  return (calcCompVal() < acrc_right.calcCompVal());
}

bool MeasureSubprog_c::operator<=(const MeasureSubprog_c& acrc_right)const{
  return (calcCompVal() <= acrc_right.calcCompVal());
}

bool MeasureSubprog_c::operator>(const MeasureSubprog_c& acrc_right)const{
  return (calcCompVal() > acrc_right.calcCompVal());
}

bool MeasureSubprog_c::operator>=(const MeasureSubprog_c& acrc_right)const{
  return (calcCompVal() >= acrc_right.calcCompVal());
}

bool MeasureSubprog_c::operator==(Proc_c::type_t ren_type)const{
  return (men_type == ren_type);
}

bool MeasureSubprog_c::operator!=(Proc_c::type_t ren_type)const{
  return (men_type != ren_type);
}

bool MeasureSubprog_c::operator<(Proc_c::type_t ren_type)const{
  return (men_type < ren_type);
}

bool MeasureSubprog_c::operator<=(Proc_c::type_t ren_type)const{
  return (men_type <= ren_type);
}

bool MeasureSubprog_c::operator>(Proc_c::type_t ren_type)const{
  return (men_type > ren_type);
}

bool MeasureSubprog_c::operator>=(Proc_c::type_t ren_type)const{
  return (men_type >= ren_type);
}

} // end of namespace __IsoAgLib
