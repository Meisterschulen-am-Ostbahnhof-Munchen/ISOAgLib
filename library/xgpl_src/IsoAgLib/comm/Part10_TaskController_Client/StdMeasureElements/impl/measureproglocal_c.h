/*
  measureproglocal_c.h: object for managing local

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef MEASUREPROG_LOCAL_H
#define MEASUREPROG_LOCAL_H

#include "measuresubprog_c.h"
#include <IsoAgLib/hal/hal_typedef.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/processpkg_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/proc_c.h>

#include <list>

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {
class ProcDataLocal_c;


/**
  managing of local measure progs and values;
  sending values for running programs,
  answering value requests,
  starting and stopping measure programs by remote
  @see MeasureProgRemote
  @author Dipl.-Inform. Achim Spangler
*/
class MeasureProgLocal_c : public ClientBase  {
private:

  struct ThresholdInfo_s
  {
    Proc_c::measurementCommand_t en_type;
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
  /**
    constructor which can optionally set most element vars of MeasureProgLocal
    @param apc_processData optional pointer to containing ProcDataLocal_c instance (def NULL)
    @param ecutype remoteType_t of remote member
  */
  MeasureProgLocal_c(Proc_c::remoteType_t ecutype);

  /** default destructor which has nothing to do */
   virtual ~MeasureProgLocal_c() {}

  /**
    start local measuring programs -> create if necessary and send initial value
  */
  bool startMeasurement(ProcDataLocal_c& ac_processData, Proc_c::measurementCommand_t ren_type, int32_t ai32_increment);

  /**
    stop local measuring programs 
  */
  void stopMeasurement(ProcDataLocal_c& ac_processData);

  /**
    process a message: reset command or value requests
    @return true -> received msg processed by this instance
  */
  bool processMsg( ProcDataLocal_c& ac_processData, const ProcessPkg_c& arc_data );

  /**
    set the measure prog value and send values if triggered to do
    @param ai32_val new measure value
  */
  void setVal(ProcDataLocal_c& ac_processData, int32_t ai32_val);

  /**
    periodic events (e.g. send value for time proportional progs)
    @param pui16_nextTimePeriod calculated new time period, based on current measure progs (only for local proc data)
  */
  void timeEvent( ProcDataLocal_c& ac_processData, uint16_t& rui16_nextTimePeriod );

  /**
    deliver actual last received value
    @param ab_sendRequest choose wether a request for value update should be
        sent (default false == send no request)
    @return measure val for this prog (can differ from master measure val)
  */
  int32_t val() const { return mi32_val; }

  /**
    return the remoteType_t code for this measureprog
    @return remoteType_t of this measureprog
  */
  Proc_c::remoteType_t isoNameType() const { return m_ecuType; }

private: // Private methods
  /**
    add an aditional subprog or update if one with same kind exist already
    @param ren_type increment type: Proc_c::TimeProp, Proc_c::DistProp, ...
    @param ai32_increment increment value
  */
  MeasureSubprog_c& addSubprog(Proc_c::measurementCommand_t ren_type, int32_t ai32_increment);

  /**
    helper function to check val() against limits
    @return true if value sending allowed
  */
  bool minMaxLimitsPassed() const;

  /**
    process a message with an increment for a measuring program
    @param ren_doSend set process data subtype to send
  */
  void processMeasurementMsg( ProcDataLocal_c& ac_processData, const ProcessPkg_c& pkg, Proc_c::measurementCommand_t ren_type);

private: // Private attributes

  /** actual value
      (can differ from masterVal if f.e. value of this program
      was resetted by caller)
  */
  int32_t mi32_val;

  /** gathered information about currently running threshold sub progs */
  List_ThresholdInfo mlist_thresholdInfo;

  /** dynamic array for subprogs */
  Vec_MeasureSubprog mvec_measureSubprog;

  /** isoName type value of caller of program */
  Proc_c::remoteType_t m_ecuType;

private:
  /** not copyable : copy constructor is only declared, never defined */
  MeasureProgLocal_c(const MeasureProgLocal_c&);
  /** not copyable : copy operator is only declared, never defined */
  MeasureProgLocal_c& operator=(const MeasureProgLocal_c&); 
};

}
#endif
