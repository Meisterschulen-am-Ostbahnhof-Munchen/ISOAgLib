/*
  measuresubprog_c.h - Every increment type of a measure prog is
    managed by a MeasureSubprog_c instance

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef MEASURE_SUBPROG_H
#define MEASURE_SUBPROG_H

#include <IsoAgLib/hal/hal_typedef.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/iprocdata.h>

namespace __IsoAgLib {

class MeasureSubprog_c {
public:
  MeasureSubprog_c( IsoAgLib::ProcData::measurementCommand_t ren_type, int32_t ai32_increment);
  MeasureSubprog_c( const MeasureSubprog_c& acrc_src );
  ~MeasureSubprog_c();

  IsoAgLib::ProcData::measurementCommand_t type() const { return men_type; }

  int32_t increment() const { return mi32_increment; }
  void setIncrement(int32_t ai32_val) { mi32_increment = ai32_val; }

  void start(int32_t ai32_increment = 0, int32_t ai32_lastVal = 0);

  bool updateTrigger(int32_t ai32_val);
  int32_t nextTriggerTime(int32_t ai32_val);

private:
  int32_t mi32_lastVal;
  int32_t mi32_increment;

  const IsoAgLib::ProcData::measurementCommand_t men_type;

private:
  /** not copyable : copy operator is only declared, never defined */
  MeasureSubprog_c& operator=(const MeasureSubprog_c&); 
};

}
#endif
