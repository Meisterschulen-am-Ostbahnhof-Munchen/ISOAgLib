/***************************************************************************
                          procdatalocalbase_c.h - managing of simple
                                                       local process data object
                             -------------------
    begin                : Fri Apr 07 2000
    copyright            : (C) 2000 - 2004 by Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
    type                 : Header
    $LastChangedDate$
    $LastChangedRevision$
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
 * - Call of a (member-) function                                          *
 * - Instantiation of a variable in a datatype from internal namespace     *
 * Allowed is:                                                             *
 * - Instatiation of a variable in a datatype from interface namespace,    *
 *   even if this is derived from a base class inside an internal namespace*
 * - Call of member functions which are defined in the interface class     *
 *   definition ( header )                                                 *
 * Pairing of internal and interface classes:                              *
 * - Internal implementation in an :impl: subdirectory                     *
 * - Interface in the parent directory of the corresponding internal class *
 * - Interface class name IsoAgLib::iFoo_c maps to the internal class      *
 *   __IsoAgLib::Foo_c                                                     *
 *                                                                         *
 * AS A RULE: Use only classes with names beginning with small letter :i:  *
 ***************************************************************************/
#ifndef PROCDATA_LOCAL_BASE_H
#define PROCDATA_LOCAL_BASE_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include "../../impl/procdatabase_c.h"
//#include "../../impl/processelementbase_c.h"
#include "../../impl/proc_c.h"

// Begin Namespace IsoAgLib
namespace __IsoAgLib {

/**
  common base class for all local process data independent from the
  individual feature set.
  the central accessible singletong instance of Process_c
  manages a list of pointers to ProcDataLocalBase_c
 */
class ProcDataLocalBase_c : public ProcDataBase_c
{
 public:
  /**
    constructor which can set all element vars

    possible errors:
        * Err_c::badAlloc not enough memory to insert first  MeasureProgLocal

    ISO parameter
    @param ps_elementDDI optional pointer to array of structure IsoAgLib::ElementDDI_s which contains DDI, element, isSetpoint and ValueGroup
                         (array is terminated by ElementDDI_s.ui16_element == 0xFFFF)

    DIN parameter
    @param rui8_lis optional LIS code of this instance
    @param rui8_wert optional WERT code of this instance
    @param rui8_inst optional INST code of this instance
    @param rui8_zaehlnum optional ZAEHLNUM code of this instance

    @param rc_devKey optional DEV_KEY code of Process-Data
    @param rui8_pri PRI code of messages with this process data instance (default 2)
    @param rc_ownerDevKey optional DEV_KEY of the owner
    @param rpc_devKey pointer to updated DEV_KEY variable of owner
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
  ProcDataLocalBase_c(
#ifdef USE_ISO_11783
                       const IsoAgLib::ElementDDI_s* ps_elementDDI = NULL,
                       uint16_t ui16_element = 0xFFFF,
#endif
#ifdef USE_DIN_9684
                       uint8_t rui8_lis = 0xFF,
                       uint8_t rui8_wert = 0,
                       uint8_t rui8_inst = 0,
                       uint8_t rui8_zaehlnum = 0xFF,
#endif
                       const DevKey_c& rc_devKey = DevKey_c::DevKeyInitialProcessData,
                       uint8_t rui8_pri = 2,
                       const DevKey_c& rc_ownerDevKey = DevKey_c::DevKeyUnspecified,
                       const DevKey_c *rpc_devKey = NULL,
                       bool rb_cumulativeValue = false
#ifdef USE_EEPROM_IO
                       , uint16_t rui16_eepromAdr = 0xFFFF
#endif
                       , IsoAgLib::ProcessDataChangeHandler_c *rpc_processDataChangeHandler = NULL
                       , int ri_singletonVecKey = 0
                       )
    : ProcDataBase_c(
#ifdef USE_ISO_11783
                     ps_elementDDI,
                     ui16_element,
#endif
#ifdef USE_DIN_9684
                     rui8_lis, rui8_wert, rui8_inst, rui8_zaehlnum,
#endif
                     rc_devKey, rui8_pri, rc_ownerDevKey, rpc_devKey, rpc_processDataChangeHandler, ri_singletonVecKey
                     )

    {init(
#ifdef USE_ISO_11783
          ps_elementDDI,
          ui16_element,
#endif
#ifdef USE_DIN_9684
          rui8_lis, rui8_wert, rui8_inst, rui8_zaehlnum,
#endif
          rc_devKey, rui8_pri, rc_ownerDevKey, rpc_devKey, rb_cumulativeValue
      #ifdef USE_EEPROM_IO
          , rui16_eepromAdr
      #endif // USE_EEPROM_IO
          , rpc_processDataChangeHandler
          , ri_singletonVecKey);};

  /**
    initialise this ProcDataLocalBase_c instance to a well defined initial state

    possible errors:
        * Err_c::badAlloc not enough memory to insert first  MeasureProgLocal
    ISO parameter
    @param ps_elementDDI optional pointer to array of structure IsoAgLib::ElementDDI_s which contains DDI, element, isSetpoint and ValueGroup
                         (array is terminated by ElementDDI_s.ui16_element == 0xFFFF)

    DIN parameter
    @param rui8_lis optional LIS code of this instance
    @param rui8_wert optional WERT code of this instance
    @param rui8_inst optional INST code of this instance
    @param rui8_zaehlnum optional ZAEHLNUM code of this instance

    @param rc_devKey optional DEV_KEY code of Process-Data
    @param rui8_pri PRI code of messages with this process data instance (default 2)
    @param rc_ownerDevKey optional DEV_KEY of the owner
    @param rpc_devKey pointer to updated DEV_KEY variable of owner
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
  void init(
#ifdef USE_ISO_11783
            const IsoAgLib::ElementDDI_s* ps_elementDDI,
            uint16_t rui16_element,
#endif
#ifdef USE_DIN_9684
            uint8_t rui8_lis = 0xFF,
            uint8_t rui8_wert = 0,
            uint8_t rui8_inst = 0,
            uint8_t rui8_zaehlnum = 0xFF,
#endif
            const DevKey_c& rc_devKey = DevKey_c::DevKeyInitialProcessData,
            uint8_t rui8_pri = 2,
            const DevKey_c& rc_ownerDevKey = DevKey_c::DevKeyUnspecified,
            const DevKey_c *rpc_devKey = NULL,
            bool rb_cumulativeValue = false
#ifdef USE_EEPROM_IO
            , uint16_t rui16_eepromAdr = 0xFFFF
#endif
            , IsoAgLib::ProcessDataChangeHandler_c *rpc_processDataChangeHandler = NULL
            , int ri_singletonVecKey = 0
            );
  /** copy constructor */
  ProcDataLocalBase_c( const ProcDataLocalBase_c& rrefc_src );
  /** assignment operator */
  const ProcDataLocalBase_c& operator=( const ProcDataLocalBase_c& rrefc_src );

  /** default destructor which has nothing to do */
  ~ProcDataLocalBase_c();


  #ifdef USE_EEPROM_IO
  /**
    deliver the eeprom adr for the value
    @return configured EEPROM adress
  */
  uint16_t eepromAdr()const{return ui16_eepromAdr;};
  /**
    set the eeprom adr for the value, read in value from EEPROM

    possible errors:
        * dependent error in EEPROMIO_c on problems during read
    @param rui16_eepromAdr new EEPROM adress
  */
  virtual void setEepromAdr(uint16_t rui16_eepromAdr);
  #endif
  /**
    deliver the master value (central measure value of this process data;
    can differ from measure vals of measure progs, as these can be reseted
    independent)
    @return actual master value
  */
  const int32_t& masterMeasurementVal()const{return i32_masterVal;};
  /**
    set the masterMeasurementVal from main application independent from any measure progs
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
    deliver the master value (central measure value of this process data;
    can differ from measure vals of measure progs, as these can be reseted
    independent) as float
    @return actual master value
  */
  const float& masterValFloat()const{return f_masterVal;};
  /**
    set the masterMeasurementVal from main application independent from any measure progs
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
    task for ProcDataLocal_c::timeEvent is to store the actual
    eeprom value in the defined time intervall
    @return true -> all planned executions performed
  */
  virtual bool timeEvent( void );
  /**
    send a min-information (selected by MOD) to a specified target (selected by DEVKEY)
    @param rc_targetDevKey DevKey of target
    @param ren_type optional PRI specifier of the message (default Proc_c::Target )
    @return true -> successful sent
  */
  bool sendMasterMeasurementVal( const DevKey_c& rc_targetDevKey, Proc_c::progType_t ren_progType = Proc_c::Target ) const;

 protected:
  /** processing of a setpoint message.
      this base class variant checks only, if a setpoint cmd was recieved
      which wants to reset a measurement value (this is wrongly used by some
      DIN 9684 implementations)
  */
  virtual void processSetpoint();
  /** process a measure prog message for local process data.
      this variant is only used for simple measurement progam management.
      derived classes with more flexible management (including measurement programs)
      use their own overloaded version.
  */
  virtual void processProg();
 private:
  friend class ManageMeasureProgLocal_c; /**< allow access to eepromVal() and resetEeprom() */
  friend class ProcDataLocal_c; /**< allow access to eepromVal() and resetEeprom() */
  friend class ProcDataLocalSimpleSetpoint_c; /**< allow access to eepromVal() and resetEeprom() */
  /** base function for assignment of element vars for copy constructor and operator= */
  void assignFromSource( const ProcDataLocalBase_c& rrefc_src );
#ifdef USE_EEPROM_IO
  /**
    deliver the eeprom value
    @return actual EEPROM value
  */
  const int32_t& eepromVal()const{return i32_eepromVal;};
  #ifdef USE_FLOAT_DATA_TYPE
  /**
    deliver the eeprom value
    @return actual EEPROM value
  */
  const float& eepromValFloat()const{return f_eepromVal;};
  #endif
  /**
    set the eeprom value
    @param ri32_val new EEPROM value
  */
  void setEepromVal(int32_t ri32_val){i32_eepromVal = ri32_val;};
  /**
    called from MeasureProg item -> if this item is first in list
    reset eeprom val

    possible errors:
        * dependent error in EEPROMIO_c on problems during read
    @param pc_progItem MeasureProgLocal_c instance which wants to reset EEPROM val
  */
  void resetEeprom( void );
#endif
  /**
    virtual function which check dependent on remote/local
    if send action with given var parameter and address claim state of owner is
    allowed and resolves the appropriate numbers for sender and receiver (empf)

    possible errors:
        * Err_c::elNonexistent one of resolved EMPF/SEND isn't registered with claimed address in Monitor
    @param rui8_pri PRI code of message
    @param rb_var variable number -> empf
    @param b_empf refernce to EMPF variable which is updated to rb_var
    @param b_send refernce to SEND variable which is only check for address claim state
    @param en_msgProto protocol type to use for the message
        IState_c::Din or IState_c::Iso (only compiled and used if USE_ISO_11783 is
        configured) (default: IState_c::Din)
    @return true -> owner of process data registered as active in Monitor-List
  */
  virtual bool var2empfSend(uint8_t rui8_pri, uint8_t rb_var, uint8_t &b_empf, uint8_t &b_send
  #ifdef USE_ISO_11783
    , IState_c::itemState_t &en_msgProto
  #endif
    ) const;
 private:
   /** allow explicit MeasureProgLocal_c the access to private elements */
  friend class MeasureProgLocal_c;
#ifdef USE_FLOAT_DATA_TYPE
  /** store the master value of the main programm
      in anonymous union for dircet access to float or long
      presentation
  */
  union {
    int32_t i32_masterVal;
    float f_masterVal;
  };
  #ifdef USE_EEPROM_IO
    /** the eeprom value can differ from main programm value
      (if value from eeprom has been restored, if value has been
      resetted); i32_masterVal starts with 0, and can be
      set disregarding any reset commands from remote;
      stored in anonymous union for dircet access to float or long
      presentation
      */
    union {
      int32_t i32_eepromVal;
      float f_eepromVal;
    };
  #endif
#else
  /** store the master value of the main programm */
  int32_t i32_masterVal;
  #ifdef USE_EEPROM_IO
    /** the eeprom value can differ from main programm value
      (if value from eeprom has been restored, if value has been
      resetted); i32_masterVal starts with 0, and can be
      set disregarding any reset commands from remote */
    int32_t i32_eepromVal;
  #endif
#endif
#ifdef USE_EEPROM_IO
  /** last time, where automatic value store was performed */
  int32_t i32_lastEepromStore;
  /** eeprom adress of the value, if this process data
    information should be stored permanent */
  uint16_t ui16_eepromAdr;
#endif
  /** register if this data is a cumulative type like distance, time, area */
  bool b_cumulativeValue;
};

}
#endif
