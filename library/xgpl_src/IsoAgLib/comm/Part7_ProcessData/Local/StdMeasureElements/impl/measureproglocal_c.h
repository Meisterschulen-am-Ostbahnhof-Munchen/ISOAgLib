/***************************************************************************
                          measureproglocal_c.h - object for managing local
                                                 measure progs and values
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
#ifndef MEASUREPROG_LOCAL_H
#define MEASUREPROG_LOCAL_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include <IsoAgLib/comm/Part7_ProcessData/StdMeasureElements/impl/measureprogbase_c.h>

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {
class ProcDataLocalBase_c;

/**
  managing of local measure progs and values;
  sending values for running programs,
  answering value requests,
  starting and stopping measure programs by remote
  @see MeasureProgRemote
  @author Dipl.-Inform. Achim Spangler
*/
class MeasureProgLocal_c : public MeasureProgBase_c  {
private:

  struct ThresholdInfo_s
  {
    Proc_c::type_t en_type;
    Proc_c::doSend_t en_doSend;
    int32_t i32_threshold;
    bool b_isMax;
  };
  #ifdef OPTIMIZE_HEAPSIZE_IN_FAVOR_OF_SPEED
  typedef STL_NAMESPACE::USABLE_SLIST<ThresholdInfo_s,MALLOC_TEMPLATE(ThresholdInfo_s) > List_ThresholdInfo;
  typedef STL_NAMESPACE::USABLE_SLIST<ThresholdInfo_s,MALLOC_TEMPLATE(ThresholdInfo_s) >::iterator List_ThresholdInfoIterator;
  typedef STL_NAMESPACE::USABLE_SLIST<ThresholdInfo_s,MALLOC_TEMPLATE(ThresholdInfo_s)>::const_iterator List_ThresholdInfoConstIterator;
  #else
  typedef STL_NAMESPACE::USABLE_SLIST<ThresholdInfo_s> List_ThresholdInfo;
  typedef STL_NAMESPACE::USABLE_SLIST<ThresholdInfo_s>::iterator List_ThresholdInfoIterator;
  typedef STL_NAMESPACE::USABLE_SLIST<ThresholdInfo_s>::const_iterator List_ThresholdInfoConstIterator;
  #endif

public:
  /**
    constructor which can optionally set most element vars of MeasureProgLocal
    @param apc_processData optional pointer to containing ProcDataLocal_c instance (def NULL)
    @param ai32_masterVal optional actual central local measured value used as masterVal (def 0)
    @param ai32_initialVal optional initial value (e.g which was stored in EEPROM) (default 0)
    @param ac_callerISOName optional ISOName of remote member, which caused creation of this instance (default 0xFF == no member)
  */
  MeasureProgLocal_c(
    ProcDataBase_c *const apc_processData = NULL,
    int32_t ai32_masterVal = 0,
    int32_t ai32_initialVal = 0,
    const IsoName_c& ac_callerISOName = IsoName_c::IsoNameUnspecified() );
  /**
    initialise this MeasureProgLocal_c instance to a well defined initial state
    @param apc_processData optional pointer to containing ProcDataLocal_c instance (def NULL)
    @param ai32_masterVal optional actual central local measured value used as masterVal (def 0)
    @param ai32_initialVal optional initial value (e.g which was stored in EEPROM) (default 0)
    @param ac_callerISOName optional ISOName of remote member, which caused creation of this instance (default 0xFF == no member)
  */
  void init(
    ProcDataBase_c *const apc_processData,
    int32_t ai32_masterVal = 0,
    int32_t ai32_initialVal = 0,
    const IsoName_c& ac_callerISOName = IsoName_c::IsoNameUnspecified() );
#ifdef USE_FLOAT_DATA_TYPE
  /**
    constructor which can optionally set most element vars of MeasureProgLocal
    @param apc_processData optional pointer to containing ProcDataLocal_c instance (def NULL)
    @param af_masterVal actual central local measured value used as float masterVal
    @param af_eepromVal optional value stored in EEPROM (default 0.0)
    @param ac_callerISOName optional ISOName of remote member, which caused creation of this instance (default 0xFF == no member)
  */
  MeasureProgLocal_c(
    ProcDataBase_c *const apc_processData,
    float af_masterVal,
    float af_eepromVal = 0.0F,
    const IsoName_c& ac_callerISOName = IsoName_c::IsoNameUnspecified() )
    : MeasureProgBase_c(apc_processData, 0, ac_callerISOName )
    {init(apc_processData, af_masterVal, af_eepromVal, ac_callerISOName  );};
  /**
    initialise this MeasureProgLocal_c instance to a well defined initial state
    @param apc_processData optional pointer to containing ProcDataLocal_c instance (def NULL)
    @param af_masterVal actual central local measured value used as float masterVal
    @param af_eepromVal optional value stored in EEPROM (default 0.0)
    @param ac_callerISOName optional ISOName of remote member, which caused creation of this instance (default 0xFF == no member)
  */
  void init(
    ProcDataBase_c *const apc_processData,
    float af_masterVal,
    float af_eepromVal = 0.0F,
    const IsoName_c& ac_callerISOName = IsoName_c::IsoNameUnspecified() );
#endif
  /**
    assignment of MeasureProgLocal_c objects
    @param arc_src source MeasureProgLocal_c instance
    @return reference to source instance for assignment like "prog1 = prog2 = prog3;"
  */
  const MeasureProgLocal_c& operator=(const MeasureProgLocal_c& arc_src);
  /**
    copy constructor for MeasureProgLocal
    @param arc_src source MeasureProgLocal_c instance
  */
   MeasureProgLocal_c(const MeasureProgLocal_c& arc_src);
  /** default destructor which has nothing to do */
  virtual ~MeasureProgLocal_c();
  /**
    start a measuring programm with new master measurement value

    possible errors:
      * dependant error in ProcDataLocal_c if EMPF or SEND not valid
      * dependant error in CanIo_c on send problems

    @param ren_type used increment types: Proc_c::TimeProp, Proc_c::DistProp, Proc_c::ValIncr
    @param ren_doSend value types to send on trigger of subprog: Proc_c::DoNone, Proc_c::DoVal, Proc_c::DoValForExactSetpoint...
    @param ai32_masterVal actual master value to start with
    @return true -> starting values sent with success
  */
  bool start(Proc_c::type_t ren_type,
             Proc_c::doSend_t ren_doSend, int32_t ai32_masterVal);
#ifdef USE_FLOAT_DATA_TYPE
  /**
    start a measuring programm with new master measurement value

    possible errors:
      * dependant error in ProcDataLocal_c if EMPF or SEND not valid
      * dependant error in CanIo_c on send problems

    @param ren_type used increment types: Proc_c::TimeProp, Proc_c::DistProp, Proc_c::ValIncr
    @param ren_doSend value types to send on trigger of subprog: Proc_c::DoNone, Proc_c::DoVal, Proc_c::DoValForExactSetpoint...
    @param af_masterVal actual float master value to start with
    @return true -> starting values sent with success
  */
  bool start(Proc_c::type_t ren_type,
             Proc_c::doSend_t ren_doSend, float af_masterVal);
#endif
  /**
    start a measuring program without new master measurement value

    possible errors:
      * dependant error in ProcDataLocal_c if EMPF or SEND not valid
      * dependant error in CanIo_c on send problems

    @param ren_type used increment types: Proc_c::TimeProp, Proc_c::DistProp, Proc_c::ValIncr
    @param ren_doSend value types to send on trigger of subprog: Proc_c::DoNone, Proc_c::DoVal, Proc_c::DoValForExactSetpoint...
    @return true -> starting values sent with success
  */
  virtual bool start(Proc_c::type_t ren_type, Proc_c::doSend_t ren_doSend);
  /**
    stop local measuring programs -> send actual values

    possible errors:
      * dependant error in ProcDataLocal_c if EMPF or SEND not valid
      * dependant error in CanIo_c on send problems
    @param b_deleteSubProgs is only needed for remote ISO case (but is needed due to overloading here also)
    @param ren_type used increment types: Proc_c::TimeProp, Proc_c::DistProp, Proc_c::ValIncr
    @param ren_doSend value types to send on trigger of subprog: Proc_c::DoNone, Proc_c::DoVal, Proc_c::DoValForExactSetpoint...
    @return true -> stop values sent with success
  */
  virtual bool stop(bool b_deleteSubProgs = true, Proc_c::type_t ren_type = Proc_c::NullType,
                    Proc_c::doSend_t ren_doSend = Proc_c::DoVal);
  /**
    deliver med val
    @return actual medium value
  */
  int32_t med(bool ab_sendRequest = false) const {return ( (mi32_medCnt > 0) && (ab_sendRequest || !ab_sendRequest))?(mi32_medSum/mi32_medCnt):(0);};
#ifdef USE_FLOAT_DATA_TYPE
  /**
    deliver med val as float
    @return actual medium value
  */
  float medFloat(bool ab_sendRequest = false) const {return ((mi32_medCnt > 0) && (ab_sendRequest || !ab_sendRequest))?(f_medSum/(float)mi32_medCnt):(0.0F);};
#endif
  /**
    send a sub-information (selected by en_valueGroup) to a specified target (selected by GPT)
    @param en_valueGroup value group to send
    @param ac_targetISOName ISOName of target
    @return true -> successful sent
  */
  bool sendValMod( GeneralCommand_c::ValueGroup_t en_valueGroup, const IsoName_c& ac_targetISOName) const;

  /**
    send a sub-information from the corresponding setpoint master to a specified target (selected by GPT)
    @param en_valueGroup value group to send
    @param ac_targetISOName ISOName of target
    @return true -> successful sent
  */
  bool sendSetpointValMod( GeneralCommand_c::ValueGroup_t en_valueGroup, const IsoName_c& ac_targetISOName) const;
  /**
    deliver to en_valueGroup according setpoint from a master setpoint
    @param en_valueGroup of wanted subtype
    @return value of specified subtype
  */
  int32_t setpointValMod(GeneralCommand_c::ValueGroup_t en_valueGroup) const;

  /**
    process a message: reset command or value requests

    possible errors:
      * dependant error in ProcDataLocal_c if EMPF or SEND not valid
      * dependant error in CanIo_c on send problems
    @return true -> received msg processed by this instance
  */
  virtual bool processMsg();
  /**
    set the measure prog value and send values if triggered to do

    possible errors:
      * dependant error in ProcDataLocal_c if EMPF or SEND not valid
      * dependant error in CanIo_c on send problems
    @param ai32_val new measure value
  */
  virtual void setVal(int32_t ai32_val);
#ifdef USE_FLOAT_DATA_TYPE
  /**
    set the measure prog value as float and send values if triggered to do

    possible errors:
      * dependant error in ProcDataLocal_c if EMPF or SEND not valid
      * dependant error in CanIo_c on send problems
    @param af_val new measure value
  */
  virtual void setVal(float af_val);
#endif
  /**
    send the values which are registered by a running mesuring program

    possible errors:
      * dependant error in ProcDataLocal_c if EMPF or SEND not valid
      * dependant error in CanIo_c on send problems
    @param ren_doSend value types to send on trigger of subprog: Proc_c::DoNone, Proc_c::DoVal, Proc_c::DoValForExactSetpoint...
    @return true -> value send triggered and performed with success
  */
  bool sendRegisteredVals(Proc_c::doSend_t ren_doSend = Proc_c::DoVal);

  /**
    init the element vals
    @param ai32_val initial measure val
  */
  virtual void initVal(int32_t ai32_val);
  /**
    reset the local value

    possible errors:
      * dependant error in ProcDataLocal_c if EMPF or SEND not valid
      * dependant error in CanIo_c on send problems
    @param ai32_val reset measure value to this value (ISO only)
    @return true -> reseted measure val sent with success
  */
  virtual bool resetVal(int32_t ai32_val = 0);
#ifdef USE_FLOAT_DATA_TYPE
  /**
    init the element vals
    @param af_val initial measure val
  */
  virtual void initVal(float af_val);
#endif
  /**
    reset the local intgral value

    possible errors:
      * dependant error in ProcDataLocal_c if EMPF or SEND not valid
      * dependant error in CanIo_c on send problems
    @return true -> reseted integ val sent with success
  */
  virtual bool resetInteg();
  /**
    reset the local medium value

    possible errors:
      * dependant error in ProcDataLocal_c if EMPF or SEND not valid
      * dependant error in CanIo_c on send problems
    @return true -> reseted medium val sent with success
  */
  virtual bool resetMed();
  /**
    reset the local min value

    possible errors:
      * dependant error in ProcDataLocal_c if EMPF or SEND not valid
      * dependant error in CanIo_c on send problems
    @return true -> reseted minimum val sent with success
  */
  virtual bool resetMin();
  /**
    reset the local max value

    possible errors:
      * dependant error in ProcDataLocal_c if EMPF or SEND not valid
      * dependant error in CanIo_c on send problems
    @return true -> reseted maximum val sent with success
  */
  virtual bool resetMax();
  /**
    periodic events
    (e.g. send value for time proportional progs)

    possible errors:
      * dependant error in ProcDataLocal_c if EMPF or SEND not valid
      * dependant error in CanIo_c on send problems
    @param pui16_nextTimePeriod calculated new time period, based on current measure progs (only for local proc data)
    @return true -> all planned activities performed in available time
  */
  virtual bool timeEvent( uint16_t *pui16_nextTimePeriod = NULL );

private: // Private methods
  /** base function for assignment of element vars for copy constructor and operator= */
  void assignFromSource( const MeasureProgLocal_c& arc_src );
  /**  update proportional dependent values */
  void updatePropDepVals();

  /**
    set medium val
    @param ai32_val initial medium value
  */
  void setMed(int32_t ai32_val){mi32_medSum = ai32_val; mi32_medCnt = 1;};
  /**
    increment medCnt
  */
  void incrMedCnt(){mi32_medCnt++;};
  /**
    increment the mediumSum
    @param ai32_val new measure val to use for calculating the medium value
  */
  void incrMedSum(int32_t ai32_val){mi32_medSum += ai32_val;};
#ifdef USE_FLOAT_DATA_TYPE
  /**
    set medium val
    @param af_val initial medium value
  */
  void setMed(float af_val){f_medSum = af_val; mi32_medCnt = 1;};
  /**
    increment the mediumSum
    @param af_val new measure val to use for calculating the medium value
  */
  void incrMedSum(float af_val){f_medSum += af_val;};
#endif
  /**
    deliver a reference to ProcDataLocal_c
    (the base function only delivers ProcDataBase_c)
    @return reference to containing ProcDataLocal_c
  */
  ProcDataLocalBase_c& processData()
  {
    return *((ProcDataLocalBase_c*)((void*)ProcessElementBase_c::pprocessData()));
  };
  /**
    deliver a pointer to ProcDataLocal_c
      (the base function only delivers ProcDataBase_c)
    @return pointer to containing ProcDataLocal_c
  */
  ProcDataLocalBase_c *const pprocessData()const
  {
    return ((ProcDataLocalBase_c*)((void*)ProcessElementBase_c::pprocessData()));
  };

  /**
    helper function to check val() against limits
    @return TRUE if value sending allowed
  */
  bool minMaxLimitsPassed(Proc_c::doSend_t ren_doSend) const;

private: // Private attributes
#ifdef USE_FLOAT_DATA_TYPE
  union {
    /** last master (eg. main prog or sensor) val  */
    int32_t mi32_lastMasterVal;
    /** last master (eg. main prog or sensor) val  */
    float f_lastMasterVal;
  };
  union {
    /**
      sum of values which are used to calculate the medium
      (only defined if one proportional prog is active)
    */
    int32_t mi32_medSum;
    /**
      sum of values which are used to calculate the medium
      (only defined if one proportional prog is active)
    */
    float f_medSum;
  };
#else
  /** last master (eg. main prog or sensor) val  */
  int32_t mi32_lastMasterVal;
  /**
    sum of values which are used to calculate the medium
    (only defined if one proportional prog is active)
  */
  int32_t mi32_medSum;
#endif
  /** count of used songle values to calculate medium val  */
  int32_t mi32_medCnt;

  /**
    stores if one subprog triggered,
    if sent wasn't possible within one call, this could be retried
    in later calls; a true val is only reseted to false, if sending was possible
  */
  bool mb_triggeredIncrement;

  /** gathered information about currently running threshold sub progs */
  List_ThresholdInfo mlist_thresholdInfo;
};

}
#endif
