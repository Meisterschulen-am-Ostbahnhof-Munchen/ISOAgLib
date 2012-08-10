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
#include <IsoAgLib/util/impl/singleton.h>
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
    @param ecuType ecuType_t of remote member
  */
  MeasureProgLocal_c(IsoName_c::ecuType_t ecutype);

  /** default destructor which has nothing to do */
   virtual ~MeasureProgLocal_c() {}

  /**
    start a measuring programm with new master measurement value
    @param ren_type used increment types: Proc_c::TimeProp, Proc_c::DistProp, ...
  */
  void start(ProcDataLocal_c& ac_processData, Proc_c::measurementCommand_t ren_type);

  /**
    stop local measuring programs -> send actual values
  */
  void stop(ProcDataLocal_c& ac_processData);

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
    add an aditional subprog or update if one with same kind exist already
    @param ren_type increment type: Proc_c::TimeProp, Proc_c::DistProp, ...
    @param ai32_increment increment value
  */
  void addSubprog(Proc_c::measurementCommand_t ren_type, int32_t ai32_increment);

  /**
    check if this measure prog is running
    @return true -> program is running
  */
  bool started() const {return (men_doSend != Proc_c::DoNone);};

  /**
    deliver actual last received value
    @param ab_sendRequest choose wether a request for value update should be
        sent (default false == send no request)
    @return measure val for this prog (can differ from master measure val)
  */
  int32_t val() const { return mi32_val; }

  /**
    return the ecuType_t code for this measureprog
    @return ecuType_t of this measureprog
  */
  IsoName_c::ecuType_t isoNameType() const { return m_ecuType; }

private: // Private methods
  /**
    send the values which are registered by a running mesuring program
    @param ren_doSend value types to send on trigger of subprog
  */
  void sendRegisteredVals( ProcDataLocal_c& ac_processData);

  /**
    send a sub-information (selected by en_valueGroup) to a specified target (selected by GPT)
    @param en_valueGroup value group to send
    @param ac_targetISOName ISOName of target
  */
  void sendVal( ProcDataLocal_c& ac_processData, const IsoName_c& ac_targetISOName) const;

  /**
    helper function to check val() against limits
    @return true if value sending allowed
  */
  bool minMaxLimitsPassed() const;

  /**
    process a message with an increment for a measuring program
    @param ren_doSend set process data subtype to send
  */
  void processIncrementMsg( ProcDataLocal_c& ac_processData, const ProcessPkg_c& pkg);

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

  /** specifies which value types should be sent if one subprog triggers */
  Proc_c::doSend_t men_doSend;

  /** isoName type value of caller of program */
  IsoName_c::ecuType_t m_ecuType;

private:
  /** not copyable : copy constructor is only declared, never defined */
  MeasureProgLocal_c(const MeasureProgLocal_c&);
  /** not copyable : copy operator is only declared, never defined */
  MeasureProgLocal_c& operator=(const MeasureProgLocal_c&); 
};

}
#endif
