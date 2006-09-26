/***************************************************************************
                          procdatalocalsimplesetpoint_c.h  -
                             managing of local process data object
                             -------------------
    begin                : Fri Apr 07 2000
    copyright            : (C) 2000 - 2004 by Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
    type                 : Header
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 * This file is part of the "IsoAgLib", an object oriented program library *
 * to serve as a software layer between application specific program and   *
 * communication protocol details. By providing simple function calls for  *
 * jobs like starting a measuring program for a process data value on a    *
 * remote ECU, the main program has not to deal with single CAN telegram   *
 * formatting. This way communication problems between ECU's which use     *
 * this library should be prevented.                                       *
 * Everybody and every company is invited to use this library to make a    *
 * working plug and play standard out of the printed protocol standard.    *
 *                                                                         *
 * Copyright (C) 2000 - 2004 Dipl.-Inform. Achim Spangler                  *
 *                                                                         *
 * The IsoAgLib is free software; you can redistribute it and/or modify it *
 * under the terms of the GNU General Public License as published          *
 * by the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This library is distributed in the hope that it will be useful, but     *
 * WITHOUT ANY WARRANTY; without even the implied warranty of              *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       *
 * General Public License for more details.                                *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with IsoAgLib; if not, write to the Free Software Foundation,     *
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA           *
 *                                                                         *
 * As a special exception, if other files instantiate templates or use     *
 * macros or inline functions from this file, or you compile this file and *
 * link it with other works to produce a work based on this file, this file*
 * does not by itself cause the resulting work to be covered by the GNU    *
 * General Public License. However the source code for this file must still*
 * be made available in accordance with section (3) of the                 *
 * GNU General Public License.                                             *
 *                                                                         *
 * This exception does not invalidate any other reasons why a work based on*
 * this file might be covered by the GNU General Public License.           *
 *                                                                         *
 * Alternative licenses for IsoAgLib may be arranged by contacting         *
 * the main author Achim Spangler by a.spangler@osb-ag:de                  *
 ***************************************************************************/

 /**************************************************************************
 *                                                                         *
 *     ###    !!!    ---    ===    IMPORTANT    ===    ---    !!!    ###   *
 * Each software module, which accesses directly elements of this file,    *
 * is considered to be an extension of IsoAgLib and is thus covered by the *
 * GPL license. Applications must use only the interface definition out-   *
 * side :impl: subdirectories. Never access direct elements of __IsoAgLib  *
 * and __HAL namespaces from applications which shouldnt be affected by    *
 * the license. Only access their interface counterparts in the IsoAgLib   *
 * and HAL namespaces. Contact a.spangler@osb-ag:de in case your applicat- *
 * ion really needs access to a part of an internal namespace, so that the *
 * interface might be extended if your request is accepted.                *
 *                                                                         *
 * Definition of direct access:                                            *
 * - Instantiation of a variable with a datatype from internal namespace   *
 * - Call of a (member-) function                                          *
 * Allowed is:                                                             *
 * - Instatiation of a variable with a datatype from interface namespace,  *
 *   even if this is derived from a base class inside an internal namespace*
 * - Call of member functions which are defined in the interface class     *
 *   definition ( header )                                                 *
 *                                                                         *
 * Pairing of internal and interface classes:                              *
 * - Internal implementation in an :impl: subdirectory                     *
 * - Interface in the parent directory of the corresponding internal class *
 * - Interface class name IsoAgLib::iFoo_c maps to the internal class      *
 *   __IsoAgLib::Foo_c                                                     *
 *                                                                         *
 * AS A RULE: Use only classes with names beginning with small letter :i:  *
 ***************************************************************************/
#ifndef PROCDATA_LOCAL_SIMPLESETPOINT_H
#define PROCDATA_LOCAL_SIMPLESETPOINT_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include "../../SimpleSetpointElements/impl/simplemanagesetpointlocal_c.h"
#include "../../impl/procdatalocalbase_c.h"
#include "../../StdMeasureElements/impl/managemeasureproglocal_c.h"

// Begin Namespace IsoAgLib
namespace __IsoAgLib {


// *****************************************************************************************
// ************************** ProcDataLocalSimpleSetpoint_c **************************
// *****************************************************************************************

/**
  @brief Internal implementation for managing of local process data object with restricted setpoint
  management ( %i.e. set setpoint, without handling of acceptance, several concurring
  setpoint commanding ECUs ).

  Has __IsoAgLib::SimpleManageSetpointLocal_c / IsoAgLib::iSimpleManageSetpointLocal_c and
  __IsoAgLib::ManageMeasureProgLocal_c / IsoAgLib::iManageMeasureProgLocal_c
  as member for doing setpoint and measurement specific jobs.
  \n
  Projects which use this class, must include the files and succeding subdirectories
  of the following subdirectories of xgpl_src/IsoAgLib/Process:
  - \e Local/StdMeasureElements,
  - \e Local/SimpleSetpointElements,
  - \e StdMeasureElements and
  - \e impl
  \n
  Example:
  \code
  // define local device type
  IsoAgLib::ISOName c_myISOName( 1, 0 );
  // creation of process data instance
  iProcDataLocalSimpleSetpoint_c c_workState;
  // init for LIS=0, local device type/subtype=1/0, complete work width=0xFF,
  // target process data/PRI=2, pointer to my local device type ( to resolve dynamic SA at time of cmd send ),
  // load/store measurememnt data to/from EEPROM
  c_workState.init( 0, myISOName, 0x1, 0x0, 0xFF, 2, c_myISOName, &c_myISOName, false, 0x1234 );

  // update current measurement value ( real value, which can differ from commanded value )
  c_workState.setMasterMeasurementVal( 100 );
  // measurement programs and value requests are handled in the backend of the IsoAgLibrary

  // simply access received setpoints ( without facility to reject wrong values )
  int newVal = c_workState.setpointMasterVal();
  // set a different setpoint as the current "valid" setpoint
  // -> if other ECU requests current setpoint, it will recieve the new value as answer
  c_workState.setSetpointMasterVal( 100 );
  \endcode

  @author Dipl.-Inform. Achim Spangler
*/
class ProcDataLocalSimpleSetpoint_c : public ProcDataLocalBase_c  {

public:
  /**
    constructor which can set all element vars

    possible errors:
        * Err_c::badAlloc not enough memory to insert first  MeasureProgLocal
    ISO parameter
    @param ps_elementDDI optional pointer to array of structure IsoAgLib::ElementDDI_s which contains DDI, element, isSetpoint and ValueGroup
                         (array is terminated by ElementDDI_s.ui16_element == 0xFFFF)

    @param rc_isoName optional ISOName code of Process-Data
    @param rui8_pri PRI code of messages with this process data instance (default 2)
    @param rc_ownerISOName optional ISOName of the owner
    @param rpc_isoName pointer to updated ISOName variable of owner
    @param rb_cumulativeValue
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
    @param rui16_eepromAdr optional adress where value is stored in EEPROM
    @param rpc_processDataChangeHandler optional pointer to handler class of application
    @param ri_singletonVecKey optional key for selection of IsoAgLib instance (default 0)
  */
  ProcDataLocalSimpleSetpoint_c(const IsoAgLib::ElementDDI_s* ps_elementDDI = NULL, uint16_t rui16_element = 0xFFFF,
                                const ISOName_c& rc_isoName = ISOName_c::ISONameInitialProcessData, uint8_t rui8_pri = 2,
                                const ISOName_c& rc_ownerISOName = ISOName_c::ISONameUnspecified,
                                const ISOName_c *rpc_isoName = NULL,
                                bool rb_cumulativeValue = false,
#ifdef USE_EEPROM_IO
                                uint16_t rui16_eepromAdr = 0xFFFF,
#endif
                                IsoAgLib::ProcessDataChangeHandler_c *rpc_processDataChangeHandler = NULL,
                                int ri_singletonVecKey = 0
      );

  /**
    initialise this ProcDataLocalSimpleSetpoint_c instance to a well defined initial state

    possible errors:
        * Err_c::badAlloc not enough memory to insert first  MeasureProgLocal
    ISO parameter
    @param ps_elementDDI optional pointer to array of structure IsoAgLib::ElementDDI_s which contains DDI, element, isSetpoint and ValueGroup
                         (array is terminated by ElementDDI_s.ui16_element == 0xFFFF)

    @param rc_isoName optional ISOName code of Process-Data
    @param rui8_pri PRI code of messages with this process data instance (default 2)
    @param rc_ownerISOName optional ISOName of the owner
    @param rpc_isoName pointer to updated ISOName variable of owner
    @param rb_cumulativeValue
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
    @param rui16_eepromAdr optional adress where value is stored in EEPROM
    @param rpc_processDataChangeHandler optional pointer to handler class of application
    @param ri_singletonVecKey optional key for selection of IsoAgLib instance (default 0)
  */
  void init(const IsoAgLib::ElementDDI_s* ps_elementDDI,
            uint16_t rui16_element,
            const ISOName_c& rc_isoName = ISOName_c::ISONameInitialProcessData,
            uint8_t rui8_pri = 2,
            const ISOName_c& rc_ownerISOName = ISOName_c::ISONameUnspecified,
            const ISOName_c *rpc_isoName = NULL,
            bool rb_cumulativeValue = false,
#ifdef USE_EEPROM_IO
            uint16_t rui16_eepromAdr = 0xFFFF,
#endif
            IsoAgLib::ProcessDataChangeHandler_c *rpc_processDataChangeHandler = NULL,
            int ri_singletonVecKey = 0
      );

  /** copy constructor */
  ProcDataLocalSimpleSetpoint_c( const ProcDataLocalSimpleSetpoint_c& rrefc_src );
  /** assignment operator */
  const ProcDataLocalSimpleSetpoint_c& operator=( const ProcDataLocalSimpleSetpoint_c& rrefc_src );
  /** default destructor which has nothing to do */
  ~ProcDataLocalSimpleSetpoint_c();
  /** deliver a reference to the setpoint management class */
  SimpleManageSetpointLocal_c& setpoint( void ) { return c_setpoint; };
  /** deliver a reference to the setpoint management class */
  const SimpleManageSetpointLocal_c& setpointConst( void ) const { return c_setpoint; };
  /**
    check if specific measureprog exist
    @param rui8_pri PRI code of searched measure program
    @param rc_isoName DEVCLASS code of searched measure program
    @return true -> found item
  */
  bool existProg(uint8_t rui8_pri, const ISOName_c& rc_isoName)
      {return c_measureprog.existProg(rui8_pri, rc_isoName);};

  /**
    search for suiting measureprog, if not found AND if rb_doCreate == true
    create copy from first element at end of vector

    possible errors:
        * Err_c::elNonexistent wanted measureprog doesn't exist and rb_doCreate == false
    @param rui8_pri PRI code of searched measure program
    @param rc_isoName DEVCLASS code of searched measure program
    @param rb_doCreated true -> create suitable measure program if not found
  */
  MeasureProgLocal_c& prog(uint8_t rui8_pri, const ISOName_c& rc_isoName, bool rb_doCreate)
    { return c_measureprog.prog(rui8_pri, rc_isoName, rb_doCreate);};


  #ifdef USE_EEPROM_IO
  /**
    set the eeprom adr for the value, read in value from EEPROM

    possible errors:
        * dependent error in EEPROMIO_c on problems during read
    @param rui16_eepromAdr new EEPROM adress
  */
  virtual void setEepromAdr(uint16_t rui16_eepromAdr);
  #endif
  /**
    set the masterVal from main application independent from any measure progs
    @param ri32_val new measure value
  */
  virtual void setMasterMeasurementVal(int32_t ri32_val);
  /**
    increment the value -> update the local and the measuring programs values
    @param ri32_val size of increment of master value
  */
  virtual void incrMasterMeasurementVal(int32_t ri32_val);
#ifdef USE_FLOAT_DATA_TYPE
  /**
    set the masterVal from main application independent from any measure progs
    @param rf_val new measure value
  */
  virtual void setMasterMeasurementVal(float rf_val);
  /**
    increment the value -> update the local and the measuring programs values
    @param rf_val size of increment of master value

  */
  virtual void incrMasterMeasurementVal(float rf_val);
#endif
  /**
    perform periodic actions
    delete all running measure programs of members which are >3sec inactive;
    deletion of outdated setpoints is managed by SetpointLocal_c::timeEvent
    @return true -> all planned executions performed
  */
  virtual bool timeEvent( void );

  /**
    check if a setpoint master exists
    (used for accessing setpoint values from measure progs)
    @return true -> setpoint master exists
  */
  virtual bool setpointExistMaster() const { return true;};
  /**
    (used for accessing setpoint values from measure progs)
    @return exact value of master setpoint
  */
  virtual int32_t setpointExactValue() const { return setpointConst().setpointMasterVal();};
  /**
    (used for accessing setpoint values from measure progs)
    @return default value of master setpoint
  */
  virtual int32_t setpointDefaultValue() const { return setpointConst().setpointMasterVal();};
  /**
    (used for accessing setpoint values from measure progs)
    @return min value of master setpoint
  */
  virtual int32_t setpointMinValue() const { return setpointConst().setpointMasterVal();};
  /**
    (used for accessing setpoint values from measure progs)
    @return max value of master setpoint
  */
  virtual int32_t setpointMaxValue() const { return setpointConst().setpointMasterVal();};

private: // Private methods
#ifdef USE_EEPROM_IO
  /**
    called from MeasureProg item -> if this item is first in list
    reset eeprom val

    possible errors:
        * dependent error in EEPROMIO_c on problems during read
    @param pc_progItem MeasureProgLocal_c instance which wants to reset EEPROM val
  */
  void resetEeprom( MeasureProgLocal_c* pc_progItem );
#endif
  /** process a measure prog message for local process data */
  virtual void processProg();
  /** processing of a setpoint message */
  virtual void processSetpoint();
  /** deliver reference to ManageMeasureProgLocal_c */
  ManageMeasureProgLocal_c& getManageProg( void ) { return c_measureprog;};
 private:
  /** flaxible management of measure progs */
  ManageMeasureProgLocal_c c_measureprog;
  /** simple management of setpoints */
  SimpleManageSetpointLocal_c c_setpoint;
};

}
#endif
