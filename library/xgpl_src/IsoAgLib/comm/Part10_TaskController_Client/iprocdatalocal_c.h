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
#include <IsoAgLib/comm/Part10_TaskController_Client/StdMeasureElements/imeasureproglocal_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/StdSetpointElements/isetpointlocal_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/proc_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/iprocesscmd_c.h>


namespace IsoAgLib {
  
class iProcDataLocal_c : private __IsoAgLib::ProcDataLocal_c  {
private:
  friend class EventSource_c;
public:
  /**
    constructor which can set all element vars

    possible errors:
        * Err_c::badAlloc not enough memory to insert first  MeasureProgLocal
    ISO parameter
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
  iProcDataLocal_c( uint16_t aui16_ddi,
                    uint16_t aui16_element,
                    const iIsoName_c& acrc_isoName,
                    const iIsoName_c *apc_externalOverridingIsoName = NULL,
                    bool ab_cumulativeValue = false,
                  ProcessDataChangeHandler_c *apc_processDataChangeHandler = NULL,
                  int ai_multitonInst = 0)
    : ProcDataLocal_c( aui16_ddi, aui16_element,
                       acrc_isoName, apc_externalOverridingIsoName, ab_cumulativeValue,
                       apc_processDataChangeHandler,
                       ai_multitonInst
                      )
{}

  /**
    initialise this ProcDataLocal_c
    instance to a well defined initial state

    possible errors:
        * Err_c::badAlloc not enough memory to insert first  MeasureProgLocal
    ISO parameter
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
  void init( uint16_t aui16_ddi,
             uint16_t aui16_element,
             const iIsoName_c& acrc_isoName,
             const iIsoName_c *apc_externalOverridingIsoName = NULL, bool ab_cumulativeValue = false,
            ProcessDataChangeHandler_c *apc_processDataChangeHandler = NULL,
            int ai_multitonInst = 0
            )
  {ProcDataLocal_c::init( aui16_ddi, aui16_element,
                         acrc_isoName, apc_externalOverridingIsoName, ab_cumulativeValue,
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
  uint16_t DDI() const { return ProcIdent_c::DDI();}

  /**
  deliver value element (only possible if only one elementDDI in list)
  @return element
   */
  uint16_t element() const  { return ProcIdent_c::element();}

  /**
    send the base value (the one that is updated via setMasterMeasurementVal() )
    to a specified target (selected by GPT)
    @param ac_targetISOName ISOName of target
    @return true -> successful sent
  */
  bool sendMasterMeasurementVal( const iIsoName_c& ac_targetISOName ) const
    { return ProcDataLocal_c::sendMasterMeasurementVal( ac_targetISOName );}

  /**
    send a exact-setpoint to a specified target (selected by GPT)
    @param ac_targetISOName ISOName of target
    @return true -> successful sent
  */
  bool sendMasterSetpointVal( const iIsoName_c& ac_targetISOName ) const
   { return setpointConst().sendMasterSetpointVal( ac_targetISOName );}

  /**
    send a sub-setpoint (selected by value group) to a specified target (selected by GPT)
    @param en_valueGroup select sub-type of setpoint
    @param ac_targetISOName ISOName of target
    @return true -> successful sent
  */
  bool sendSetpointForGroup( ProcessCmd_c::ValueGroup_t en_valueGroup, const iIsoName_c& ac_targetISOName ) const
  {
    return setpointConst().sendSetpointForGroup( ac_targetISOName,
                                            en_valueGroup, __IsoAgLib::ProcessCmd_c::setValue );
  }

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
  int32_t setpointMasterVal() const
    { return ProcDataLocal_c::setpointConst().setpointMasterVal();}

  /**
    set the setpoint value
    @param ai32_val new setpoint value
  */
  void setSetpointMasterVal(int32_t ai32_val)
    { ProcDataLocal_c::setpoint().setSetpointMasterVal(ai32_val);}

   /**
    check if specific measureprog exist
    @param acrc_isoName DEVCLASS code of searched measure program
    @return true -> found item
  */
  bool existProg(const iIsoName_c& acrc_isoName)
      {return ProcDataLocal_c::existProg(acrc_isoName);}

  /**
    search for suiting measureprog, if not found AND if ab_doCreate == true
    create copy from first element at end of vector

    possible errors:
        * Err_c::elNonexistent wanted measureprog doesn't exist and ab_doCreate == false

    @param acrc_isoName DEVCLASS code of searched measure program
    @param ab_doCreate true -> create suitable measure program if not found
  */
  iMeasureProgLocal_c& prog(const iIsoName_c& acrc_isoName, bool ab_doCreate)
    { return static_cast<iMeasureProgLocal_c&>(ProcDataLocal_c::prog(acrc_isoName, ab_doCreate));}

  /** deliver reference to setpoint */
  iSetpointLocal_c& setpoint( void )
    { return static_cast<iSetpointLocal_c&>(ProcDataLocal_c::setpoint()); }

  bool startDataLogging(Proc_c::type_t ren_type /* Proc_c::TimeProp, Proc_c::DistProp, ... */,
                        int32_t ai32_increment, const iIsoName_c* apc_receiverDevice = NULL )
    { return ProcDataLocal_c::startDataLogging(ren_type, ai32_increment, apc_receiverDevice); }


};

}
#endif
