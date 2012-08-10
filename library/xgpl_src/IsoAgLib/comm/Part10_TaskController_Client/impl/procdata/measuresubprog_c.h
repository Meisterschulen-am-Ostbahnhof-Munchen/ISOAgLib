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
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/proc_c.h>
#include <IsoAgLib/util/impl/singleton.h>

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {


/**
  Every increment type of a measure prog is
  managed by a MeasureSubprog_c instance.
  @author Dipl.-Inform. Achim Spangler
*/
class MeasureSubprog_c : public ClientBase {
public:
  /**
    default constructor which can optionally set increment type, increment and running state
    @param ren_type optional increment type (default time proportional)
    @param ren_doSend set process data subtype to send
    @param ai32_increment optional increment value of ren_type (default 0)
    @param ab_started optional running state (default off)
    @param ai32_lastVal optional value of last trigger event (default 0)
  */
  MeasureSubprog_c(Proc_c::measurementCommand_t ren_type, int32_t ai32_increment  MULTITON_INST_PARAMETER_DEFAULT_NULL_DEF_WITH_COMMA);

  /**
    copy constructor for Subprogs
    @param acrc_src source instance
  */
   MeasureSubprog_c(const MeasureSubprog_c& acrc_src);

  /** default destructor which has nothing to do */
  ~MeasureSubprog_c();

  /**
    deliver subprog type
    @return type of this measure subprogram increment
  */
  Proc_c::measurementCommand_t type() const { return men_type; }

  /**
    deliver the increment value of this subprog
    @return increment value
  */
  int32_t increment() const { return mi32_increment; }
  /**
    set increment value
    @param ai32_val wanted increment value
  */
  void setIncrement(int32_t ai32_val) { mi32_increment = ai32_val; }

  /**
    start a measuring subprogramm, potentially with increment and lastVal definition
    @param ai32_increment optional increment value (default old increment value used)
    @param ai32_lastVal last trigger value (default 0)
  */
  void start(int32_t ai32_increment = 0, int32_t ai32_lastVal = 0);
  /**
    delivers if given value forces trigger of send of registered informations
    @param ai32_val actual increment relevant value (time for TimeProp, distance for DistProp, ...)
    @return true -> this subprog triggers (e.g. send actual value)
  */
  bool updateTrigger(int32_t ai32_val);

  int32_t nextTriggerTime(int32_t ai32_val);

  /** stop a measuring subprogram  */
  void stop() {}

private: // Private attributes
  /** last value for the increment test of this instance */
  int32_t mi32_lastVal;
  /** increment of this subprog item */
  int32_t mi32_increment;

  /** type of this subprog instance */
  const Proc_c::measurementCommand_t men_type;

private:
  /** not copyable : copy operator is only declared, never defined */
  MeasureSubprog_c& operator=(const MeasureSubprog_c&); 
};

}
#endif
