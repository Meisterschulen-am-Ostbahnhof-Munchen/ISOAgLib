/***************************************************************************
                          procdatalocalbase_c.h - managing of simple
                                                       local process data object
                             -------------------
    begin                : Fri Apr 07 2000
    copyright            : (C) 2000 - 2009 by Dipl.-Inform. Achim Spangler
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
 * Copyright (C) 2000 - 2009 Dipl.-Inform. Achim Spangler                  *
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
    @param ai_singletonVecKey optional key for selection of IsoAgLib instance (default 0)
  */
  ProcDataLocalBase_c( const IsoAgLib::ElementDdi_s* aps_elementDDI = NULL, uint16_t aui16_element = 0xFFFF,
                       const IsoName_c& acrc_isoName = IsoName_c::IsoNameInitialProcessData(),
                       const IsoName_c *apc_externalOverridingIsoName = NULL,
                       bool ab_cumulativeValue = false
#ifdef USE_EEPROM_IO
                       , uint16_t aui16_eepromAdr = 0xFFFF
#endif
                       , IsoAgLib::ProcessDataChangeHandler_c *apc_processDataChangeHandler = NULL
                       , int ai_singletonVecKey = 0
                       )
    : ProcDataBase_c( aps_elementDDI, aui16_element,
                      acrc_isoName, apc_externalOverridingIsoName, apc_processDataChangeHandler, ai_singletonVecKey
                     )

    {
      init( aps_elementDDI, aui16_element, acrc_isoName, apc_externalOverridingIsoName, ab_cumulativeValue
      #ifdef USE_EEPROM_IO
          , aui16_eepromAdr
      #endif // USE_EEPROM_IO
          , apc_processDataChangeHandler
          , ai_singletonVecKey);
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
    @param ai_singletonVecKey optional key for selection of IsoAgLib instance (default 0)
  */
  void init(const IsoAgLib::ElementDdi_s* ps_elementDDI, uint16_t aui16_element,
            const IsoName_c& acrc_isoName = IsoName_c::IsoNameInitialProcessData(),
            const IsoName_c *apc_externalOverridingIsoName = NULL,
            bool ab_cumulativeValue = false
#ifdef USE_EEPROM_IO
            , uint16_t aui16_eepromAdr = 0xFFFF
#endif
            , IsoAgLib::ProcessDataChangeHandler_c *apc_processDataChangeHandler = NULL
            , int ai_singletonVecKey = 0
            );

  /** copy constructor */
  ProcDataLocalBase_c( const ProcDataLocalBase_c& acrc_src );

  /** assignment operator */
  const ProcDataLocalBase_c& operator=( const ProcDataLocalBase_c& acrc_src );

  /** default destructor which has nothing to do */
  ~ProcDataLocalBase_c();

  #ifdef USE_EEPROM_IO
  /** deliver the eeprom adr for the value
    @return configured EEPROM adress
  */
  uint16_t eepromAdr() const {return mui16_eepromAdr;}

  /** set the eeprom adr for the value, read in value from EEPROM
    possible errors:
        * dependent error in EepromIo_c on problems during read
    @param aui16_eepromAdr new EEPROM adress
  */
  virtual void setEepromAdr(uint16_t aui16_eepromAdr);
  #endif

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
  bool sendValISOName( const IsoName_c& ac_varISOName, int32_t ai32_val = 0) const;

 protected:
  /** processing of a setpoint message.
      this base class variant checks only, if a setpoint cmd was recieved
      which wants to reset a measurement value
  */
  virtual void processSetpoint();

  /** process a measure prog message for local process data.
      this variant is only used for simple measurement progam management.
      derived classes with more flexible management (including measurement programs)
      use their own overloaded version.
  */
  virtual void processProg();

private:
  friend class ManageMeasureProgLocal_c; /** allow access to eepromVal() and resetEeprom() */
  friend class ProcDataLocal_c; /** allow access to eepromVal() and resetEeprom() */
  friend class ProcDataLocalSimpleSetpoint_c; /** allow access to eepromVal() and resetEeprom() */

  /** base function for assignment of element vars for copy constructor and operator= */
  void assignFromSource( const ProcDataLocalBase_c& acrc_src );

#ifdef USE_EEPROM_IO
  /** deliver the eeprom value
    @return actual EEPROM value
  */
  const int32_t& eepromVal() const {return mi32_eepromVal;}

  /** set the eeprom value
    @param ai32_val new EEPROM value
  */
  void setEepromVal (int32_t ai32_val) {mi32_eepromVal = ai32_val;}

  /** called from MeasureProg item -> if this item is first in list
    reset eeprom val
    possible errors:
        * dependent error in EepromIo_c on problems during read
    @param pc_progItem MeasureProgLocal_c instance which wants to reset EEPROM val
  */
  void resetEeprom( void );
#endif

  void setLocalSendFlags (const IsoName_c& ac_varISOName) const;

 private:
   /** allow explicit MeasureProgLocal_c the access to private elements */
  friend class MeasureProgLocal_c;

  /** store the master value of the main programm */
  int32_t mi32_masterVal;

  #ifdef USE_EEPROM_IO
    /** the eeprom value can differ from main programm value
      (if value from eeprom has been restored, if value has been
      resetted); mi32_masterVal starts with 0, and can be
      set disregarding any reset commands from remote */
    int32_t mi32_eepromVal;
  #endif

#ifdef USE_EEPROM_IO
  /** last time, where automatic value store was performed */
  int32_t mi32_lastEepromStore;
  /** eeprom adress of the value, if this process data
    information should be stored permanent */
  uint16_t mui16_eepromAdr;
#endif

  /** register if this data is a cumulative type like distance, time, area */
  bool mb_cumulativeValue;
};

}
#endif
