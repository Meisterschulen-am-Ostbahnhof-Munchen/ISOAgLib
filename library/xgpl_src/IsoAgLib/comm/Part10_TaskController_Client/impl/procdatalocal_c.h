/***************************************************************************
                          procdatalocal_c.h - managing of local

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef PROCDATA_LOCAL_H
#define PROCDATA_LOCAL_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/proc_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/StdMeasureElements/impl/managemeasureproglocal_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/StdSetpointElements/impl/setpointlocal_c.h>
#include <IsoAgLib/util/config.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/istate_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isoname_c.h>
#include <IsoAgLib/util/impl/singleton.h>
#include "processpkg_c.h"
#include "processcmd_c.h"

namespace IsoAgLib {
  class ProcessDataChangeHandler_c;
  class EventSource_c;
}

// Begin Namespace IsoAgLib
namespace __IsoAgLib {
// ****************************************************************************************
// ********************************* ProcDataLocal_c *********************************
// ****************************************************************************************


/**
  @brief Internal implementation for managing of local process data object with standard
  ( %i.e. no restrictions ) feature set.

  Has __IsoAgLib::SetpointLocal_c / IsoAgLib::iSetpointLocal_c and __IsoAgLib::ManageMeasureProgLocal_c / IsoAgLib::iManageMeasureProgLocal_c
  as member for doing setpoint and measurement specific jobs.
  \n
  Projects which use this class, must include the files and succeding subdirectories
  of the following subdirectories of xgpl_src/IsoAgLib/Process:
  - \e Local/StdMeasureElements,
  - \e Local/StdSetpointElements,
  - \e StdMeasureElements,
  - \e StdSetpointElements and
  - \e impl
  \n
  Example:
  \code
  // define local device type
  IsoAgLib::iIsoName c_myIsoName( 1, 0 );
  // creation of process data instance
  iProcDataLocal_c c_workState;
  // init for LIS=0, local device type/subtype=1/0, complete work width=0xFF,
  // target process data/PRI=2, pointer to my local device type ( to resolve dynamic SA at time of cmd send ),
  // load/store measurememnt data to/from EEPROM
  c_workState.init( 0, 0, myIsoName, c_myIsoName, &c_myIsoName, false, 0x1234 );

  // update current measurement value ( real value, which can differ from commanded value )
  c_workState.setMasterMeasurementVal( 100 );
  // measurement programs and value requests are handled in the backend of the IsoAgLibrary

  // check if unhandled setpoints are received
  if ( c_workState.setpoint().unhandledCnt() > 0 ) {
    if ( ! c_workState.setpoint().existMaster() ) {
      for ( uint8_t ind = 0; ind < c_workState.setpoint().unhandledCnt(); ind++ )
      { // now decide on all received setpoints
        if ( ( c_workState.setpoint().unhandledCnt() == 1 )
          && ( ( c_workState.setpoint().unhandledFirst().exact() == 100 ) // undhandledFirst() is shortcut for unhandledInd( 0 )
            || ( c_workState.setpoint().unhandledInd( 0 ).exact() == 0   ) ) )
        { // as no master exist, and only one setpoint received -> simply accept if value is 0 or 100
          c_workState.setpoint().unhandledFirst().setMaster( true );
        }
      }
    }
    else if ( ( c_workState.setpoint().unhandledMaster().exact() == 100 )
           || ( c_workState.setpoint().unhandledMaster().exact() == 0 ) )
    { // simply accept new master if 0 or 100
      c_workState.setpoint().acceptNewMaster();
    }
    // send answer for all received setpoints
    c_workState.setpoint().respondAckNack();
  }
  \endcode

  @author Dipl.-Inform. Achim Spangler
*/
class ProcDataLocal_c : public ClientBase  {

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
  ProcDataLocal_c(uint16_t aui16_ddi,
                  uint16_t aui16_element,
                  const IsoName_c& acrc_isoName,
                  const IsoName_c *apc_externalOverridingIsoName = NULL,
                  bool ab_cumulativeValue = false,
                  IsoAgLib::ProcessDataChangeHandler_c *apc_processDataChangeHandler = NULL,
                  int ai_multitonInst = 0
                  );

  /**
    initialise this ProcDataLocal_c instance to a well defined initial state

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
  void init(uint16_t aui16_ddi,
            uint16_t aui16_element,
            const IsoName_c& acrc_isoName,
            const IsoName_c *apc_externalOverridingIsoName = NULL,
            bool ab_cumulativeValue = false,
            IsoAgLib::ProcessDataChangeHandler_c *apc_processDataChangeHandler = NULL,
            int ai_multitonInst = 0
            );

  /** default destructor which has nothing to do */
  ~ProcDataLocal_c();

  /** assignment operator for this base object
    @param acrc_src source instance
    @return reference to source instance for cmd like "prog1 = prog2 = prog3;"
  */
  const ProcDataLocal_c& operator=(const ProcDataLocal_c& acrc_src);

  /** copy constructor for ProcDataLocal_c
    @param acrc_src source instance
  */
  ProcDataLocal_c(const ProcDataLocal_c& acrc_src);

  /** deliver a reference to the setpoint management class */
  SetpointLocal_c& setpoint( void ) { return mc_setpoint; }

  const SetpointLocal_c& setpointConst( void ) const { return mc_setpoint; }

  /**
    check if specific measureprog exist

    @param acrc_isoName DEVCLASS code of searched measure program
    @return true -> found item
  */
  bool existProg(const IsoName_c& acrc_isoName)
      {return mc_measureprog.existProg(acrc_isoName);};

  /**
    search for suiting measureprog, if not found AND if ab_doCreate == true
    create copy from first element at end of vector

    possible errors:
        * Err_c::elNonexistent wanted measureprog doesn't exist and ab_doCreate == false

    @param acrc_isoName DEVCLASS code of searched measure program
    @param ab_doCreate true -> create suitable measure program if not found
  */
  MeasureProgLocal_c& prog(const IsoName_c& acrc_isoName, bool ab_doCreate)
    { return mc_measureprog.prog(acrc_isoName, ab_doCreate);};

  virtual const IsoName_c& commanderISOName() const { return IsoName_c::IsoNameUnspecified(); }

  /** set the pointer to the handler class
    * @param apc_processDataChangeHandler pointer to handler class of application
    */
  void setProcessDataChangeHandler( IsoAgLib::ProcessDataChangeHandler_c *apc_processDataChangeHandler )
   { mpc_processDataChangeHandler = apc_processDataChangeHandler; }

  /** deliver the poitner to the handler class
    * @return pointer to handler class of application (or NULL if not defined by application)
    */
  IsoAgLib::ProcessDataChangeHandler_c* getProcessDataChangeHandler( void ) const { return mpc_processDataChangeHandler; }

  /** deliver the master value (central measure value of this process data;
    can differ from measure vals of measure progs, as these can be reseted
    independent)
    @return actual master value
  */
  const int32_t& masterMeasurementVal() const {return mi32_masterVal;}

  /**
    set the masterMeasurementVal from main application independent from any measure progs
    @param ai32_val new measure value
  */
  virtual void setMasterMeasurementVal(int32_t ai32_val);

  /**
    increment the value -> update the local and the measuring programs values
    @param ai32_val size of increment of master value
  */
  virtual void incrMasterMeasurementVal(int32_t ai32_val);

  /** process a message, which is adressed for this process data item;
    ProcDataLocal_c::processMsg() is responsible to delegate the
    processing of setpoint and measurement messages to the appripriate
    functions processSetpoint and processProg;
    both functions are virtual, so that depending on loacl or remote
    process data the suitable reaction can be implemented
  */
  void processMsg( ProcessPkg_c& pkg );

  /**
    perform periodic actions
    delete all running measure programs of members which are >3sec inactive;
    deletion of outdated setpoints is managed by SetpointLocal_c::timeEvent
    @param pui16_nextTimePeriod calculated new time period, based on current measure progs (only for local proc data)
    @return true -> all planned executions performed
  */
  virtual bool timeEvent(  uint16_t *pui16_nextTimePeriod = NULL );

  /** send a min-information (selected by value group) to a specified target (selected by ISOName)
    @param ac_targetISOName ISOName of target
    @return true -> successful sent
  */
  bool sendMasterMeasurementVal( const IsoName_c& ac_targetISOName ) const;

  /**
    check if a setpoint master exists
    (used for accessing setpoint values from measure progs)
    @return true -> setpoint master exists
  */
  virtual bool setpointExistMaster() const { return setpointConst().existMaster();}

  /**
    (used for accessing setpoint values from measure progs)
    @return exact value of master setpoint
  */
  virtual int32_t setpointExactValue() const { return setpointConst().masterConst().exact();}

  /**
    (used for accessing setpoint values from measure progs)
    @return default value of master setpoint
  */
  virtual int32_t setpointDefaultValue() const { return setpointConst().masterConst().getDefault();}

  /**
    (used for accessing setpoint values from measure progs)
    @return min value of master setpoint
  */
  virtual int32_t setpointMinValue() const { return setpointConst().masterConst().min();}

  /**
    (used for accessing setpoint values from measure progs)
    @return max value of master setpoint
  */
  virtual int32_t setpointMaxValue() const { return setpointConst().masterConst().max();}

  /**
    allow local client to actively start a measurement program
    (to react on a incoming "start" command for default data logging)
    @param ren_type measurement type: Proc_c::TimeProp, Proc_c::DistProp, ...
    @param ai32_increment
    @param apc_receiverDevice commanding ISOName
    @return true -> measurement started
  */
  bool startDataLogging(Proc_c::type_t ren_type /* Proc_c::TimeProp, Proc_c::DistProp, ... */,
                        int32_t ai32_increment, const IsoName_c* apc_receiverDevice = NULL );

  /**
    stop all measurement progs in all local process instances, started with given isoName
    @param rc_isoName
  */
  virtual void stopRunningMeasurement(const IsoName_c& rc_isoName);

public: // from former base class

  /** check if this ProcIdent_c has the given DDI as element */
  bool hasDDI( uint16_t aui16_checkDDI ) const;
  bool hasType(bool ab_isSetpoint, ProcessCmd_c::ValueGroup_t t_ddiType) const;

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
    deliver the isoName (retrieved from pointed isoName value, if valid pointer)
    @return actual ISOName
  */
  const IsoName_c& isoName() const
    { return ((mpc_externalOverridingIsoName != 0)?(*mpc_externalOverridingIsoName):(mc_isoName));}

  /**
    set DDI, value group and setpoint/measure type of process msg
    @param aps_elementDDI
  */
  void setElementDDI(uint16_t aui16_ddi) { mui16_ddi = aui16_ddi; }

  /** set device element number
    * @param  aui16_element */
  void setElementNumber(uint16_t aui16_element) { mui16_element = aui16_element; }

  /**
    set value ISOName (machine type specific table of process data types)
    @param ac_val new ISOName val
  */
  void setISOName(const IsoName_c& ac_val){mc_isoName = ac_val;}

  /**
    set pointer to external isoName instances (used by isoName())
    @param apc_val pointer to ISOName
  */
  void setExternalOverridingIsoName(const IsoName_c* apc_val);

  /**
    check if this item has the same identity as defined by the parameters,
    if aui8_devClassInst is 0xFF a lazy match disregarding pos is done
    (important for matching received process data msg);
    if INSTANCE is defined (!= 0xFF) then one of the following conditions must be true:<ul>
    <li>parameter INSTANCE == ident INSTANCE (devClassInst())
    <li>parameter acrc_isoName == isoName()
    </ul>

    ISO parameter
    @param acrc_isoNameSender compare this parameter with owner isoName (only for remote, local calls: IsoNameUnspecified)
    @param acrc_isoNameReceiver compared isoName value
    @param aui16_DDI compared DDI value
    @param aui16_element compared element value

    @return true -> this instance has same Process-Data identity
  */
  bool matchISO( const IsoName_c& acrc_isoNameSender,
                 const IsoName_c& acrc_isoNameReceiver,
                 uint16_t aui16_DDI,
                 uint16_t aui16_element
               ) const;

protected: // Protected methods
  /** send the given int32_t value with variable ISOName ac_varISOName;
      set the int32_t value with conversion (according to central data type) in message
      string and set data format flags corresponding to central data type of this process data
      (local: receiver; remote: sender)
      (other paramter fixed by ident of process data)
      set general command before sendValISOName !

        possible errors:
      * Err_c::elNonexistent one of resolved EMPF/SEND isn't registered with claimed address in Monitor
      * dependant error in CanIo_c on CAN send problems

      @param ac_varISOName variable ISOName
      @param ai32_val int32_t value to send
      @return true -> sendIntern set successful EMPF and SEND
  */
  virtual bool sendValISOName( ProcessPkg_c& pkg, const IsoName_c& ac_varISOName, int32_t ai32_val = 0) const;

  void setBasicSendFlags( ProcessPkg_c& pkg ) const;

private: // Private methods
  /** base function for assignment of element vars for copy constructor and operator= */
  void assignFromSource( const ProcDataLocal_c& acrc_src );

  /** processing of a setpoint message */
  virtual void processSetpoint( const ProcessPkg_c& pkg );

  /** process a measure prog message for local process data */
  virtual void processProg( const ProcessPkg_c& pkg );

  /** deliver reference to ManageMeasureProgLocal_c */
  ManageMeasureProgLocal_c& getManageProg( void ) { return mc_measureprog;}

private: // Private attributes

  /** pointer to applications handler class, with handler functions
      which shall be called on correltating change events.
      (e.g. new received setpoint for local process data
       or new received measurement value for remote process data)
    */
  IsoAgLib::ProcessDataChangeHandler_c* mpc_processDataChangeHandler;

private:
  friend class ManageMeasureProgLocal_c; /** allow access to eepromVal() and resetEeprom() */
  friend class MeasureProgBase_c;
  friend class MeasureProgLocal_c;
  friend class SetpointBase_c;
  friend class SetpointLocal_c;

  /** store the master value of the main programm */
  int32_t mi32_masterVal;

  /** register if this data is a cumulative type like distance, time, area */
  bool mb_cumulativeValue;

private: // Private attributes

  /** DEVCLASS code of process data identity */
  const IsoName_c* mpc_externalOverridingIsoName; // only defined for own local data, otherwise NULL

  /** IsoName_c information for this instance */
  IsoName_c mc_isoName;

  uint16_t mui16_ddi;
  uint16_t mui16_element;

private:

  /** flaxible management of measure progs */
  ManageMeasureProgLocal_c mc_measureprog;

  /** flaxible management of setpoint */
  SetpointLocal_c mc_setpoint;
};


}
#endif
