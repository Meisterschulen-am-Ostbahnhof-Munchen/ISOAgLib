/*
  procdatalocal_c.h: managing of local

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef PROCDATA_LOCAL_H
#define PROCDATA_LOCAL_H

#include <IsoAgLib/comm/Part10_TaskController_Client/impl/proc_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/StdMeasureElements/impl/managemeasureproglocal_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/StdSetpointElements/impl/setpointlocal_c.h>
#include <IsoAgLib/util/config.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isoname_c.h>
#include <IsoAgLib/util/impl/singleton.h>
#include "processpkg_c.h"

namespace IsoAgLib {
  class ProcessDataChangeHandler_c;
}

// Begin Namespace IsoAgLib
namespace __IsoAgLib {

/**
  @brief Internal implementation for managing of local process data object with standard
  ( %i.e. no restrictions ) feature set.
*/
class ProcDataLocal_c : public ClientBase  {

public:
  /**
    constructor which can set all element vars
    @param aui16_ddi process data ddi
    @param aui16_element process data parent element
    @param acrc_isoName process data ISOName
    @param ab_isSetpoint set this process data as set point
    @param aui8_triggerMethod set process data trigger methods
    @param apc_processDataChangeHandler optional pointer to handler class of application
    @param ai_multitonInst optional key for selection of IsoAgLib instance (default 0)
  */
  ProcDataLocal_c(uint16_t aui16_ddi,
                  uint16_t aui16_element,
                  const IsoName_c& acrc_isoName,
                  bool ab_isSetpoint,
                  uint8_t aui8_triggerMethod,
                  IsoAgLib::ProcessDataChangeHandler_c *apc_processDataChangeHandler = NULL,
                  int ai_multitonInst = 0
                  );

  /**
    initialise this ProcDataLocal_c
    @param aui16_ddi process data ddi
    @param aui16_element process data parent element
    @param acrc_isoName process data ISOName
    @param ab_isSetpoint set this process data as set point
    @param aui8_triggerMethod set process data trigger methods
    @param apc_processDataChangeHandler optional pointer to handler class of application
    @param ai_multitonInst optional key for selection of IsoAgLib instance (default 0)
  */
  void init(uint16_t aui16_ddi,
            uint16_t aui16_element,
            const IsoName_c& acrc_isoName,
            bool ab_isSetpoint,
            uint8_t aui8_triggerMethod,
            IsoAgLib::ProcessDataChangeHandler_c *apc_processDataChangeHandler = NULL,
            int ai_multitonInst = 0
            );

  /** default destructor which has nothing to do */
  ~ProcDataLocal_c();

  /** set the pointer to the handler class
    * @param apc_processDataChangeHandler pointer to handler class of application
    */
  void setProcessDataChangeHandler( IsoAgLib::ProcessDataChangeHandler_c *apc_processDataChangeHandler )
   { mpc_processDataChangeHandler = apc_processDataChangeHandler; }

  /** deliver the poitner to the handler class
    * @return pointer to handler class of application (or NULL if not defined by application)
    */
  IsoAgLib::ProcessDataChangeHandler_c* getProcessDataChangeHandler( void ) const { return mpc_processDataChangeHandler; }

  /** deliver the measured value
    can differ from measure vals of measure progs, as these can be reseted
    @return actual value
  */
  const int32_t& measurementVal() const {return mi32_value;}

  /**
    set the value independent from any measure progs
    @param ai32_val new measure value
  */
  void setMeasurementVal(int32_t ai32_val);

  /**
    increment the value -> update the measuring programs values
    @param ai32_val size of increment of master value
  */
  void incrMeasurementVal(int32_t ai32_val);

  /** process a message, which is adressed for this process data item */
  void processMsg( ProcessPkg_c& pkg );

  /**
    perform periodic actions
    delete all running measure programs of members which are >3sec inactive;
    deletion of outdated setpoints is managed by SetpointLocal_c::timeEvent
    @param pui16_nextTimePeriod calculated new time period, based on current measure progs (only for local proc data)
    @return true -> all planned executions performed
  */
  void timeEvent(  uint16_t& rui16_nextTimePeriod );

  /** send the value to a specified target (selected by ISOName)
    @param ac_targetISOName ISOName of target
    @return true -> successful sent
  */
  void sendMeasurementVal( const IsoName_c& ac_targetISOName ) const;

  /**
    Get setpoint value as received from remote system
    @return exact value of master setpoint
  */
  int32_t setpointValue() const { return mc_setpoint.setpointVal(); }

  /**
    allow local client to actively start a measurement program
    (to react on a incoming "start" command for default data logging)
    @param ren_type measurement type: Proc_c::TimeProp, Proc_c::DistProp, ...
    @param ai32_increment
    @param apc_receiverDevice commanding ISOName
    @return true -> measurement started
  */
  bool startDataLogging(Proc_c::measurementCommand_t ren_type /* Proc_c::TimeProp, Proc_c::DistProp, ... */,
                        int32_t ai32_increment, const IsoName_c& ac_receiverDevice );

  /**
    stop all measurement progs in all local process instances, started with given isoName
    @param rc_isoName
  */
  void stopRunningMeasurement(const IsoName_c& rc_isoName);

  /**
    deliver value DDI (only possible if only one elementDDI in list)
    @return DDI
  */
  uint16_t DDI() const{ return mui16_ddi; }

  /**
    deliver value element (only possible if only one elementDDI in list)
    @return element
  */
  uint16_t element() const{ return mui16_element; }

  /**
    return true if ProcessData is set as setpoint 
    @return setpoint information
  */
  bool isSetPoint() const { return procdataconfiguration.mb_isSetpoint; }

  /**
    deliver trigger method information
    @return setpoint information
  */
  uint8_t triggerMethod() const { return procdataconfiguration.mui8_triggerMethod; } 

  /**
    deliver the isoName (retrieved from pointed isoName value, if valid pointer)
    @return actual ISOName
  */
  const IsoName_c& isoName() const { return mc_isoName; }

  /**
    check if this item has the same identity as defined by the parameters,
    if aui8_devClassInst is 0xFF a lazy match disregarding pos is done
    (important for matching received process data msg);
    @param acrc_isoNameReceiver compared isoName value
    @param aui16_DDI compared DDI value
    @param aui16_element compared element value
    @return true -> this instance has same Process-Data identity
  */
  bool matchISO( const IsoName_c& acrc_isoNameReceiver,
                 uint16_t aui16_DDI,
                 uint16_t aui16_element
               ) const;

private: // Private methods
  /** send the given int32_t value with variable ISOName ac_varISOName;
      set the int32_t value with conversion (according to central data type) in message
      string and set data format flags corresponding to central data type of this process data
      set general command before sendValISOName !
      @param ac_varISOName variable ISOName
      @param ai32_val int32_t value to send
      @return true -> sendIntern set successful EMPF and SEND
  */
  void sendValISOName( ProcessPkg_c& pkg, const IsoName_c& ac_varISOName, int32_t ai32_val = 0) const;

private: // Private attributes
  /** IsoName_c used for this instance */
  IsoName_c mc_isoName;

  /** ProcessData ddi */
  uint16_t mui16_ddi;
  /** ProcessData parent device element */
  uint16_t mui16_element;

  /** ProcessData configuration : setpoint and trigger method information */
  struct {
    bool mb_isSetpoint : 1;
    uint8_t mui8_triggerMethod : 7;
  } procdataconfiguration;

  /** store the value of the main programm */
  int32_t mi32_value;

  /** pointer to applications handler class, with handler functions
      which shall be called on correltating change events.
      (e.g. new received setpoint for local process data
       or new received measurement value for remote process data)
    */
  IsoAgLib::ProcessDataChangeHandler_c* mpc_processDataChangeHandler;

  /** flaxible management of measure progs */
  ManageMeasureProgLocal_c mc_measureprog;

  /** flaxible management of setpoint */
  SetpointLocal_c mc_setpoint;

private:
  friend class MeasureProgLocal_c;

private:
  /** not copyable : copy constructor is only declared, never defined */
  ProcDataLocal_c(const ProcDataLocal_c&);
  /** not copyable : copy operator is only declared, never defined */
  ProcDataLocal_c& operator=(const ProcDataLocal_c&); 

};


}
#endif
