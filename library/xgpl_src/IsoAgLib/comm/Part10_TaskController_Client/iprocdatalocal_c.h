/***************************************************************************
                          iprocdatalocal_c.h  - managing of local

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef IPROCESS_DATA_LOCAL_H
#define IPROCESS_DATA_LOCAL_H

#include "impl/procdatalocal_c.h"
#include <IsoAgLib/comm/Part10_TaskController_Client/proc_c.h>


namespace IsoAgLib {
  
class iProcDataLocal_c : private __IsoAgLib::ProcDataLocal_c  {
private:
  friend class EventSource_c;
public:
  /**
    constructor which can set all element vars
    @param ps_elementDDI optional pointer to array of structure IsoAgLib::ElementDdi_s which contains DDI, element, isSetpoint and ValueGroup
                         (array is terminated by ElementDdi_s.ui16_element == 0xFFFF)

    @param acrc_isoName optional ISOName code of Process-Data
    @param apc_externalOverridingIsoName pointer to updated ISOName variable
    @param ab_cumulativeValue
             -# for process data like distance, time, area
                 the value of the measure prog data sets is updated
                 on master value update dependent on the value increment
                 since the last master value update
                 -> if a remote member resets his data set copy, datas of
                    other members aren't changed
                 -> if this data is saved in EEPROM, the main application
                    needn't take into account the initial EEPROM value, as
                     setting of the master val is independent from EEPROM
             -#  for values like speed, state, rpm aren't updated by increment,
                  -> the given master value is propagated equally to all
                      measure prog data sets
                  -> if this data is saved in EEPROM, the stored value is loaded
                     as initial master value, and is initially propagated to all
                     measure prog data sets
    @param aui16_eepromAdr optional adress where value is stored in EEPROM
    @param apc_processDataChangeHandler optional pointer to handler class of application
    @param ai_multitonInst optional key for selection of IsoAgLib instance (default 0)
  */
    iProcDataLocal_c( ) : 
      ProcDataLocal_c (
        0xFFFF,
        0xFFFF,
        iIsoName_c::IsoNameUnspecified(),
        false,
        0) {}
        
  iProcDataLocal_c( uint16_t aui16_ddi,
                    uint16_t aui16_element,
                    const iIsoName_c& acrc_isoName,
                    bool ab_isSetpoint,
                    uint8_t aui8_triggerMethod,
                    ProcessDataChangeHandler_c *apc_processDataChangeHandler = NULL,
                    int ai_multitonInst = 0)
    : ProcDataLocal_c( aui16_ddi, aui16_element,
                       acrc_isoName, ab_isSetpoint,
                       aui8_triggerMethod,
                       apc_processDataChangeHandler,
                       ai_multitonInst
                      )
{}

  /**
    initialise this ProcDataLocal_c
    @param ps_elementDDI optional pointer to array of structure IsoAgLib::ElementDdi_s which contains DDI, element, isSetpoint and ValueGroup
                         (array is terminated by ElementDdi_s.ui16_element == 0xFFFF)

    @param acrc_isoName optional ISOName code of Process-Data
    @param ab_cumulativeValue
             -# for process data like distance, time, area
                 the value of the measure prog data sets is updated
                 on master value update dependent on the value increment
                 since the last master value update
                 -> if a remote member resets his data set copy, datas of
                    other members aren't changed
                 -> if this data is saved in EEPROM, the main application
                    needn't take into account the initial EEPROM value, as
                     setting of the master val is independent from EEPROM
             -#  for values like speed, state, rpm aren't updated by increment,
                  -> the given master value is propagated equally to all
                      measure prog data sets
                  -> if this data is saved in EEPROM, the stored value is loaded
                     as initial master value, and is initially propagated to all
                     measure prog data sets
    @param aui16_eepromAdr optional adress where value is stored in EEPROM
    @param apc_processDataChangeHandler optional pointer to handler class of application
    @param ai_multitonInst optional key for selection of IsoAgLib instance (default 0)
  */
  void init( uint16_t aui16_ddi,
             uint16_t aui16_element,
             const iIsoName_c& acrc_isoName,
             bool ab_isSetpoint,
             uint8_t aui8_triggerMethod,
             ProcessDataChangeHandler_c *apc_processDataChangeHandler = NULL,
             int ai_multitonInst = 0
            )
  {ProcDataLocal_c::init( aui16_ddi, aui16_element,
                         acrc_isoName, ab_isSetpoint,
                         aui8_triggerMethod,
                         apc_processDataChangeHandler,
                         ai_multitonInst);
  }

  /** set the poitner to the handler class
    * @param apc_processDataChangeHandler pointer to handler class of application
    */
  void setProcessDataChangeHandler( ProcessDataChangeHandler_c *apc_processDataChangeHandler )
   { ProcDataLocal_c::setProcessDataChangeHandler( apc_processDataChangeHandler ); }

  /** deliver the poitner to the handler class
    * @return pointer to handler class of application (or NULL if not defined by application)
    */
  IsoAgLib::ProcessDataChangeHandler_c* getProcessDataChangeHandler( void ) const
   { return ProcDataLocal_c::getProcessDataChangeHandler(); }

  /**
    deliver value ISOName (machine type specific table of process data types)
    use everytime the _device_class_ from the ident part, and take the _instance_ from the owner
    @return ISOName
  */
  const iIsoName_c& isoName() const {return ProcDataLocal_c::isoName().toConstIisoName_c();}

  /**
    deliver value DDI (only possible if only one elementDDI in list)
    @return DDI
   */
  uint16_t DDI() const { return ProcDataLocal_c::DDI();}

  /**
  deliver value element (only possible if only one elementDDI in list)
  @return element
   */
  uint16_t element() const  { return ProcDataLocal_c::element();}

  /**
    send the base value (the one that is updated via setMasterMeasurementVal() )
    to a specified target (selected by GPT)
    @param ac_targetISOName ISOName of target
    @return true -> successful sent
  */
  void sendMasterMeasurementVal( const iIsoName_c& ac_targetISOName ) const
    { ProcDataLocal_c::sendMasterMeasurementVal( ac_targetISOName );}

  /**
    deliver the master value (central measure value of this process data;
    can differ from measure vals of measure progs, as these can be reseted
    independent)
    @return actual master value
  */
  const int32_t& masterMeasurementVal()const{return ProcDataLocal_c::masterMeasurementVal();}

  /**
    set the masterMeasurementVal from main application independent from any measure progs
    @param ai32_val new measure value
  */
  void setMasterMeasurementVal(int32_t ai32_val)
    {ProcDataLocal_c::setMasterMeasurementVal(ai32_val);}

  /**
    set the masterMeasurementVal from main application independent from any measure progs
    @param ai16_val new measure value
  */
  void setMasterMeasurementVal(int16_t ai16_val)
    {ProcDataLocal_c::setMasterMeasurementVal((int32_t)ai16_val);}

  /**
    set the masterMeasurementVal from main application independent from any measure progs
    @param ab_val new measure value
  */
  void setMasterMeasurementVal(uint8_t ab_val)
    {ProcDataLocal_c::setMasterMeasurementVal((int32_t)ab_val);}

  /**
    increment the value -> update the local and the measuring programs values
    @param ai32_val size of increment of master value
  */
  void incrMasterMeasurementVal(int32_t ai32_val)
    {ProcDataLocal_c::incrMasterMeasurementVal(ai32_val);}

  /**
    retreive simple master setpoint
    @return actual received setpoint value
  */
  int32_t setpointVal() const
    { return ProcDataLocal_c::setpointValue();}

  /**
    allow local client to actively start a measurement program
    (to react on a incoming "start" command for default data logging)
    @param ren_type measurement type: Proc_c::TimeProp, Proc_c::DistProp, ...
    @param ai32_increment
    @param apc_receiverDevice commanding ISOName
    @return true -> measurement started
  */
  bool startDataLogging(Proc_c::type_t ren_type /* Proc_c::TimeProp, Proc_c::DistProp, ... */,
                        int32_t ai32_increment, const iIsoName_c* apc_receiverDevice = NULL )
    { return ProcDataLocal_c::startDataLogging(ren_type, ai32_increment, apc_receiverDevice); }


};

}
#endif
