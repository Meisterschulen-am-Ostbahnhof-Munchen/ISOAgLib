/*
  measureproglocal_c.h

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef MEASUREPROG_H
#define MEASUREPROG_H

#include "measuresubprog_c.h"
#include <IsoAgLib/isoaglib_config.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/processpkg_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/iprocdata.h>

#include <list>

namespace __IsoAgLib {

class ProcData_c;

class MeasureProg_c
{
private:
  struct ThresholdInfo_s
  {
    IsoAgLib::ProcData::measurementCommand_t en_type;
    int32_t i32_threshold;
  };
  #ifdef OPTIMIZE_HEAPSIZE_IN_FAVOR_OF_SPEED
  typedef STL_NAMESPACE::list<ThresholdInfo_s,MALLOC_TEMPLATE(ThresholdInfo_s) > List_ThresholdInfo;
  typedef STL_NAMESPACE::list<ThresholdInfo_s,MALLOC_TEMPLATE(ThresholdInfo_s) >::iterator List_ThresholdInfoIterator;
  typedef STL_NAMESPACE::list<ThresholdInfo_s,MALLOC_TEMPLATE(ThresholdInfo_s)>::const_iterator List_ThresholdInfoConstIterator;
  #else
  typedef STL_NAMESPACE::list<ThresholdInfo_s> List_ThresholdInfo;
  typedef STL_NAMESPACE::list<ThresholdInfo_s>::iterator List_ThresholdInfoIterator;
  typedef STL_NAMESPACE::list<ThresholdInfo_s>::const_iterator List_ThresholdInfoConstIterator;
  #endif

  #ifdef OPTIMIZE_HEAPSIZE_IN_FAVOR_OF_SPEED
  typedef STL_NAMESPACE::list<MeasureSubprog_c,MALLOC_TEMPLATE(MeasureSubprog_c) > Vec_MeasureSubprog;
  typedef STL_NAMESPACE::list<MeasureSubprog_c,MALLOC_TEMPLATE(MeasureSubprog_c)>::iterator Vec_MeasureSubprogIterator;
  #else
  typedef STL_NAMESPACE::list<MeasureSubprog_c> Vec_MeasureSubprog;
  typedef STL_NAMESPACE::list<MeasureSubprog_c>::iterator Vec_MeasureSubprogIterator;
  #endif

public:
  MeasureProg_c(IsoAgLib::ProcData::remoteType_t ecutype);

  virtual ~MeasureProg_c() {}

  bool startMeasurement(ProcData_c& ac_processData, IsoAgLib::ProcData::measurementCommand_t ren_type, int32_t ai32_increment, int32_t value);
  void stopMeasurement(ProcData_c& ac_processData);

  bool processMsg( ProcData_c& ac_processData, const ProcessPkg_c& arc_data, int32_t value );
  void timeEvent( ProcData_c& ac_processData, uint16_t& rui16_nextTimePeriod, int32_t value );

  void setVal(ProcData_c& ac_processData, int32_t ai32_val);

  IsoAgLib::ProcData::remoteType_t isoNameType() const { return m_ecuType; }

private:
  MeasureSubprog_c& addSubprog(IsoAgLib::ProcData::measurementCommand_t ren_type, int32_t ai32_increment);
  bool minMaxLimitsPassed(int32_t value) const;

private:
  List_ThresholdInfo mlist_thresholdInfo;
  Vec_MeasureSubprog mvec_measureSubprog;
  IsoAgLib::ProcData::remoteType_t m_ecuType;
  
private:
  /** not copyable : copy constructor is only declared, never defined */
  MeasureProg_c(const MeasureProg_c&);
  /** not copyable : copy operator is only declared, never defined */
  MeasureProg_c& operator=(const MeasureProg_c&); 
};

}
#endif
