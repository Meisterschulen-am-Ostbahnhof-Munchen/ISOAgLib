/***************************************************************************
                          procdatalocalbase_c.h - managing of simple

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef PROCDATA_LOCAL_BASE_H
#define PROCDATA_LOCAL_BASE_H

#include <IsoAgLib/comm/Part10_TaskController_Client/impl/procdatabase_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/proc_c.h>

// Begin Namespace IsoAgLib
namespace __IsoAgLib {


/** common base class for all local process data independent from the
  individual feature set.
  the central accessible singletong instance of Process_c
  manages a list of pointers to ProcDataLocalBase_c
 */
class ProcDataLocalBase_c : public ProcDataBase_c
{
 public:
  /** constructor which can set all element vars
    possible errors:
        * Err_c::badAlloc not enough memory to insert first  MeasureProgLocal
    ISO parameter
    @param aps_elementDDI optional pointer to array of structure IsoAgLib::ElementDdi_s which contains DDI, element, isSetpoint and ValueGroup
                         (array is terminated by ElementDdi_s.ui16_element == 0xFFFF)
    @param aui16_element (default:0xFFFF)
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
  ProcDataLocalBase_c( const IsoAgLib::ElementDdi_s* aps_elementDDI, uint16_t aui16_element,
                       const IsoName_c& acrc_isoName,
                       const IsoName_c *apc_externalOverridingIsoName = NULL,
                       bool ab_cumulativeValue = false,
                       IsoAgLib::ProcessDataChangeHandler_c *apc_processDataChangeHandler = NULL,
                       int ai_multitonInst = 0
                       )
    : ProcDataBase_c( aps_elementDDI, aui16_element,
                      acrc_isoName, apc_externalOverridingIsoName, apc_processDataChangeHandler, ai_multitonInst
                     )

    {
      init( aps_elementDDI, aui16_element, acrc_isoName, apc_externalOverridingIsoName, ab_cumulativeValue
          , apc_processDataChangeHandler
          , ai_multitonInst);
    }

  /** initialise this ProcDataLocalBase_c instance to a well defined initial state
    possible errors:
        * Err_c::badAlloc not enough memory to insert first  MeasureProgLocal
    ISO parameter
    @param ps_elementDDI optional pointer to array of structure IsoAgLib::ElementDdi_s which contains DDI, element, isSetpoint and ValueGroup
                         (array is terminated by ElementDdi_s.ui16_element == 0xFFFF)
    common parameter
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
  void init(const IsoAgLib::ElementDdi_s* ps_elementDDI, uint16_t aui16_element,
            const IsoName_c& acrc_isoName,
            const IsoName_c *apc_externalOverridingIsoName = NULL,
            bool ab_cumulativeValue = false,
			IsoAgLib::ProcessDataChangeHandler_c *apc_processDataChangeHandler = NULL,
            int ai_multitonInst = 0
            );

  /** copy constructor */
  ProcDataLocalBase_c( const ProcDataLocalBase_c& acrc_src );

  /** assignment operator */
  const ProcDataLocalBase_c& operator=( const ProcDataLocalBase_c& acrc_src );

  /** default destructor which has nothing to do */
  ~ProcDataLocalBase_c();

  /** deliver the master value (central measure value of this process data;
    can differ from measure vals of measure progs, as these can be reseted
    independent)
    @return actual master value
  */
  const int32_t& masterMeasurementVal() const {return mi32_masterVal;}

  /** set the masterMeasurementVal from main application independent from any measure progs
    @param ai32_val new measure value
  */
  virtual void setMasterMeasurementVal (int32_t ai32_val);

  /** increment the value -> update the local and the measuring programs values
    @param ai32_val size of increment of master value
  */
  virtual void incrMasterMeasurementVal (int32_t ai32_val);

  /** perform periodic actions
    task for ProcDataLocal_c::timeEvent is to store the actual
    eeprom value in the defined time intervall
    @param pui16_nextTimePeriod calculated new time period, based on current measure progs (only for local proc data)
    @return true -> all planned executions performed
  */
  virtual bool timeEvent( uint16_t *pui16_nextTimePeriod = NULL );

  /** send a min-information (selected by value group) to a specified target (selected by ISOName)
    @param ac_targetISOName ISOName of target
    @return true -> successful sent
  */
  bool sendMasterMeasurementVal( const IsoName_c& ac_targetISOName ) const;

  /** check if a setpoint master exists
    (used for accessing setpoint values from measure progs)
    @return true -> setpoint master exists
  */
  virtual bool setpointExistMaster() const { return false;}

  /** (used for accessing setpoint values from measure progs)
    @return exact value of master setpoint
  */
  virtual int32_t setpointExactValue() const { return 0;}

  /** (used for accessing setpoint values from measure progs)
    @return default value of master setpoint
  */
  virtual int32_t setpointDefaultValue() const { return 0;}

  /** (used for accessing setpoint values from measure progs)
    @return min value of master setpoint
  */
  virtual int32_t setpointMinValue() const { return 0;}

  /** (used for accessing setpoint values from measure progs)
    @return max value of master setpoint
  */
  virtual int32_t setpointMaxValue() const { return 0;}

  /** stop all measurement progs in all local process instances, started with given isoName
    (not used for simple measurement)
    <!--@param rc_isoName-->
  */
  virtual void stopRunningMeasurement(const IsoName_c& /* rc_isoName */) {}

  /** send the given int32_t value with variable ISOName ac_varISOName;
      set the int32_t value with conversion (according to central data type) in message
      string and set data format flags corresponding to central data type of this process data
      (other parameter fixed by ident of process data)
        possible errors:
      * Err_c::elNonexistent one of resolved EMPF/SEND isn't registered with claimed address in Monitor
      * dependant error in CanIo_c on CAN send problems

      @param ac_varISOName variable ISOName
      @param ai32_val int32_t value to send
      @return true -> sendIntern set successful EMPF and SEND
  */
  bool sendValISOName( ProcessPkg_c& arc_pkg, const IsoName_c& ac_varISOName, int32_t ai32_val = 0) const;

 protected:
  /** processing of a setpoint message.
      this base class variant checks only, if a setpoint cmd was recieved
      which wants to reset a measurement value
  */
  virtual void processSetpoint( const ProcessPkg_c& pkg );

  /** process a measure prog message for local process data.
      this variant is only used for simple measurement progam management.
      derived classes with more flexible management (including measurement programs)
      use their own overloaded version.
  */
  virtual void processProg( const ProcessPkg_c& pkg );

private:
  friend class ManageMeasureProgLocal_c; /** allow access to eepromVal() and resetEeprom() */
  friend class ProcDataLocal_c; /** allow access to eepromVal() and resetEeprom() */
  friend class ProcDataLocalSimpleSetpoint_c; /** allow access to eepromVal() and resetEeprom() */

  /** base function for assignment of element vars for copy constructor and operator= */
  void assignFromSource( const ProcDataLocalBase_c& acrc_src );

 private:
   /** allow explicit MeasureProgLocal_c the access to private elements */
  friend class MeasureProgLocal_c;

  /** store the master value of the main programm */
  int32_t mi32_masterVal;

  /** register if this data is a cumulative type like distance, time, area */
  bool mb_cumulativeValue;
};

}
#endif
