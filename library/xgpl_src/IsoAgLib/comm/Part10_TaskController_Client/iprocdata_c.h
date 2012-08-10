/*
  iprocdatalocal_c.h: managing of local

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef IPROCESS_DATA_H
#define IPROCESS_DATA_H

#include "impl/procdata/procdata_c.h"
#include <IsoAgLib/comm/Part10_TaskController_Client/iprocdata.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/iidentitem_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/iisoname_c.h>

namespace IsoAgLib {
  
class iProcData_c : private __IsoAgLib::ProcData_c  {
public:
  /**
    default constructor. init() mathod shall be called manually
  */
  iProcData_c() : ProcData_c() {}
  
  /**
    initialise this ProcData_c
    @param aui16_ddi process data ddi
    @param aui16_element process data parent element
    @param acrc_isoName process data ISOName
    @param ab_isSetpoint set this process data as set point
    @param aui8_triggerMethod set process data trigger methods
    @param apc_processDataChangeHandler optional pointer to handler class of application
    @param ai_multitonInst optional key for selection of IsoAgLib instance (default 0)
  */
  void init( iIdentItem_c& acrc_identItem,
             uint16_t aui16_ddi,
             uint16_t aui16_element,
             bool ab_isSetpoint,
             uint8_t aui8_triggerMethod,
             iProcDataHandler_c *apc_procDataHandler = NULL
            )
  {
    ProcData_c::init( acrc_identItem,
                      aui16_ddi, aui16_element,
                      ab_isSetpoint, aui8_triggerMethod,
                      apc_procDataHandler );
  }

  /** set the poitner to the handler class
    * @param apc_processDataChangeHandler pointer to handler class of application
    */
  void setProcDataHandler( iProcDataHandler_c *apc_procDataHandler )
   { ProcData_c::setProcDataHandler( apc_procDataHandler ); }

  /** deliver the poitner to the handler class
    * @return pointer to handler class of application (or NULL if not defined by application)
    */
  iProcDataHandler_c* getProcDataHandler() const
   { return ProcData_c::getProcDataHandler(); }

  /**
    deliver value ISOName (machine type specific table of process data types)
    use everytime the _device_class_ from the ident part, and take the _instance_ from the owner
    @return ISOName
  */
  const iIsoName_c& isoName() const { return ProcData_c::isoName().toConstIisoName_c(); }

  /**
    deliver value DDI (only possible if only one elementDDI in list)
    @return DDI
   */
  uint16_t DDI() const { return ProcData_c::DDI();}

  /**
  deliver value element (only possible if only one elementDDI in list)
  @return element
   */
  uint16_t element() const  { return ProcData_c::element();}

  /**
    send the base value (the one that is updated via setMasterMeasurementVal() )
    to a specified target (selected by GPT)
    @param ac_targetISOName ISOName of target
    @return true -> successful sent
  */
  void sendMeasurementVal( const iIsoName_c& ac_targetISOName ) const
    { ProcData_c::sendMeasurementVal( ac_targetISOName );}

  /**
    deliver the master value (central measure value of this process data;
    can differ from measure vals of measure progs, as these can be reseted
    independent)
    @return actual master value
  */
  int32_t measurementVal() const { return ProcData_c::measurementVal(); }

  /**
    set the masterMeasurementVal from main application independent from any measure progs
    @param ai32_val new measure value
  */
  void setMeasurementVal(int32_t ai32_val)
    { ProcData_c::setMeasurementVal(ai32_val); }

  /**
    set the masterMeasurementVal from main application independent from any measure progs
    @param ai16_val new measure value
  */
  void setMeasurementVal(int16_t ai16_val)
    { ProcData_c::setMeasurementVal((int32_t)ai16_val); }

  /**
    set the masterMeasurementVal from main application independent from any measure progs
    @param ab_val new measure value
  */
  void setMeasurementVal(uint8_t ab_val)
    { ProcData_c::setMeasurementVal((int32_t)ab_val); }

  /**
    increment the value -> update the local and the measuring programs values
    @param ai32_val size of increment of master value
  */
  void incrMeasurementVal(int32_t ai32_val)
    { ProcData_c::incrMeasurementVal(ai32_val); }

  /**
    retreive simple master setpoint
    @return actual received setpoint value
  */
  int32_t setpointVal() const
    { return ProcData_c::setpointVal(); }

  /**
    allow local client to actively start a measurement program
    (to react on a incoming "start" command for default data logging)
    @param ren_type measurement type: ProcData::TimeProp, ProcData::DistProp, ...
    @param ai32_increment
    @param apc_receiverDevice commanding ISOName
  */
  void startDataLogging(ProcData::measurementCommand_t ren_type /* ProcData::TimeProp, ProcData::DistProp, ... */,
                        int32_t ai32_increment, IsoAgLib::ProcData::remoteType_t a_ecuType )
  { ProcData_c::startDataLogging(ren_type, ai32_increment, a_ecuType ); }

};

}
#endif
