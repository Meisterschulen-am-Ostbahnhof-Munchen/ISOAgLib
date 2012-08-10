/***************************************************************************
                          measureproglocal_c.h - object for managing local

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef MEASUREPROG_LOCAL_H
#define MEASUREPROG_LOCAL_H

#include "measuresubprog_c.h"
#include <IsoAgLib/hal/hal_typedef.h>
#include <IsoAgLib/util/impl/singleton.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/processpkg_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/proc_c.h>

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {
class ProcDataLocal_c;


/**
  managing of local measure progs and values;
  sending values for running programs,
  answering value requests,
  starting and stopping measure programs by remote
  @see MeasureProgRemote
  @author Dipl.-Inform. Achim Spangler
*/
class MeasureProgLocal_c : public ClientBase  {
private:

  struct ThresholdInfo_s
  {
    Proc_c::type_t en_type;
    Proc_c::doSend_t en_doSend;
    int32_t i32_threshold;
    bool b_isMax;
  };
  #ifdef OPTIMIZE_HEAPSIZE_IN_FAVOR_OF_SPEED
  typedef STL_NAMESPACE::list<ThresholdInfo_s,MALLOC_TEMPLATE(ThresholdInfo_s) > List_ThresholdInfo;
  typedef STL_NAMESPACE::list<ThresholdInfo_s,MALLOC_TEMPLATE(ThresholdInfo_s) >::iterator List_ThresholdInfoIterator;
  typedef STL_NAMESPACE::list<ThresholdInfo_s,MALLOC_TEMPLATE(ThresholdInfo_s)>::const_iterator List_ThresholdInfoConstIterator;
  #else
  typedef STL_NAMESPACE::list<ThresholdInfo_s> List_ThresholdInfo;
  typedef STL_NAMESPACE::list<ThresholdInfo_s>::iterator List_ThresholdInfoIterator;
  typedef STL_NAMESPACE::list<ThresholdInfo_s>::const_iterator List_ThresholdInfoConstIterator;
  #endif

  #ifdef OPTIMIZE_HEAPSIZE_IN_FAVOR_OF_SPEED
  typedef STL_NAMESPACE::list<MeasureSubprog_c,MALLOC_TEMPLATE(MeasureSubprog_c) > Vec_MeasureSubprog;
  typedef STL_NAMESPACE::list<MeasureSubprog_c,MALLOC_TEMPLATE(MeasureSubprog_c)>::iterator Vec_MeasureSubprogIterator;
  #else
  typedef STL_NAMESPACE::list<MeasureSubprog_c> Vec_MeasureSubprog;
  typedef STL_NAMESPACE::list<MeasureSubprog_c>::iterator Vec_MeasureSubprogIterator;
  #endif

public:
  /**
    constructor which can optionally set most element vars of MeasureProgLocal
    @param apc_processData optional pointer to containing ProcDataLocal_c instance (def NULL)
    @param ai32_masterVal optional actual central local measured value used as masterVal (def 0)
    @param ai32_initialVal optional initial value (e.g which was stored in EEPROM) (default 0)
    @param ecuType optional ecuType_t of remote member, which caused creation of this instance (default 0xFF == no member)
  */
  MeasureProgLocal_c();
  /**
    initialise this MeasureProgLocal_c instance to a well defined initial state
    @param apc_processData optional pointer to containing ProcDataLocal_c instance (def NULL)
    @param ai32_masterVal optional actual central local measured value used as masterVal (def 0)
    @param ai32_initialVal optional initial value (e.g which was stored in EEPROM) (default 0)
    @param ecuType optional ecuType_t of remote member, which caused creation of this instance (default 0xFF == no member)
  */
  void init(
    int32_t ai32_masterVal = 0,
    int32_t ai32_initialVal = 0);

  /**
    assignment of MeasureProgLocal_c objects
    @param acrc_src source MeasureProgLocal_c instance
    @return reference to source instance for assignment like "prog1 = prog2 = prog3;"
  */
  const MeasureProgLocal_c& operator=(const MeasureProgLocal_c& acrc_src);
  /**
    copy constructor for MeasureProgLocal
    @param acrc_src source MeasureProgLocal_c instance
  */
   MeasureProgLocal_c(const MeasureProgLocal_c& acrc_src);
  /** default destructor which has nothing to do */
  virtual ~MeasureProgLocal_c();
  /**
    start a measuring programm with new master measurement value

    possible errors:
      * dependant error in ProcDataLocal_c if EMPF or SEND not valid
      * dependant error in CanIo_c on send problems

    @param ren_type used increment types: Proc_c::TimeProp, Proc_c::DistProp, ...
    @param ren_doSend value types to send on trigger of subprog: Proc_c::DoNone, Proc_c::DoVal...
    @param ai32_masterVal actual master value to start with
    @return true -> starting values sent with success
  */
  bool start(ProcDataLocal_c& ac_processData, Proc_c::type_t ren_type,
             Proc_c::doSend_t ren_doSend, int32_t ai32_masterVal);

  /**
    start a measuring program without new master measurement value

    possible errors:
      * dependant error in ProcDataLocal_c if EMPF or SEND not valid
      * dependant error in CanIo_c on send problems

    @param ren_type used increment types: Proc_c::TimeProp, Proc_c::DistProp, ...
    @param ren_doSend value types to send on trigger of subprog: Proc_c::DoNone, Proc_c::DoVal...
    @return true -> starting values sent with success
  */
  bool start(ProcDataLocal_c& ac_processData, Proc_c::type_t ren_type, Proc_c::doSend_t ren_doSend);
  /**
    stop local measuring programs -> send actual values

    possible errors:
      * dependant error in ProcDataLocal_c if EMPF or SEND not valid
      * dependant error in CanIo_c on send problems
    @param b_deleteSubProgs is only needed for remote ISO case (but is needed due to overloading here also)
    @param ren_type used increment types: Proc_c::TimeProp, Proc_c::DistProp, ...
    @param ren_doSend value types to send on trigger of subprog: Proc_c::DoNone, Proc_c::DoVal...
    @return true -> stop values sent with success
  */
  bool stop(ProcDataLocal_c& ac_processData, Proc_c::type_t ren_type = Proc_c::NullType,
                    Proc_c::doSend_t ren_doSend = Proc_c::DoVal);

  /**
    send a sub-information (selected by en_valueGroup) to a specified target (selected by GPT)
    @param en_valueGroup value group to send
    @param ac_targetISOName ISOName of target
    @return true -> successful sent
  */
  bool sendVal( ProcDataLocal_c& ac_processData, const IsoName_c& ac_targetISOName) const;

  /**
    process a message: reset command or value requests

    possible errors:
      * dependant error in ProcDataLocal_c if EMPF or SEND not valid
      * dependant error in CanIo_c on send problems
    @return true -> received msg processed by this instance
  */
  bool processMsg( ProcDataLocal_c& ac_processData, const ProcessPkg_c& arc_data );
  bool processMsgHelper( ProcDataLocal_c& ac_processData, const ProcessPkg_c& pkg );

  /**
    set the measure prog value and send values if triggered to do

    possible errors:
      * dependant error in ProcDataLocal_c if EMPF or SEND not valid
      * dependant error in CanIo_c on send problems
    @param ai32_val new measure value
  */
  void setVal(ProcDataLocal_c& ac_processData, int32_t ai32_val);

  /**
    send the values which are registered by a running mesuring program

    possible errors:
      * dependant error in ProcDataLocal_c if EMPF or SEND not valid
      * dependant error in CanIo_c on send problems
    @param ren_doSend value types to send on trigger of subprog: Proc_c::DoNone, Proc_c::DoVal...
    @return true -> value send triggered and performed with success
  */
  bool sendRegisteredVals( ProcDataLocal_c& ac_processData, Proc_c::doSend_t ren_doSend = Proc_c::DoVal);

  /**
    init the element vals
    @param ai32_val initial measure val
  */
  void initVal(int32_t ai32_val);
  /**
    reset the local value

    possible errors:
      * dependant error in ProcDataLocal_c if EMPF or SEND not valid
      * dependant error in CanIo_c on send problems
    @param ai32_val reset measure value to this value
    @return true -> reseted measure val sent with success
  */
  bool resetVal(ProcDataLocal_c& ac_processData, int32_t ai32_val = 0);

  /**
    periodic events
    (e.g. send value for time proportional progs)

    possible errors:
      * dependant error in ProcDataLocal_c if EMPF or SEND not valid
      * dependant error in CanIo_c on send problems
    @param pui16_nextTimePeriod calculated new time period, based on current measure progs (only for local proc data)
    @return true -> all planned activities performed in available time
  */
  bool timeEvent( ProcDataLocal_c& ac_processData, uint16_t *pui16_nextTimePeriod = NULL );

  /**
    add an aditional subprog or update if one with same kind exist already
    @param ren_type increment type: Proc_c::TimeProp, Proc_c::DistProp, ...
    @param ai32_increment increment value
    @param ren_doSend set process data subtype to send (Proc_c::DoNone, Proc_c::DoVal...)
    @return always true; only relevant for overoaded methods in derived classes
  */
  bool addSubprog(Proc_c::type_t ren_type, int32_t ai32_increment, Proc_c::doSend_t ren_doSend = Proc_c::DoVal);

  /**
    set active flag
    @param ab_active
  */
  void setActive(bool ab_active) { mb_active = ab_active; }

  /**
    check if this measure prog is running
    @return true -> program is running
  */
  bool started() {return (men_doSend != Proc_c::DoNone);};

  /**
    deliver actual last received value
    @param ab_sendRequest choose wether a request for value update should be
        sent (default false == send no request)
    @return measure val for this prog (can differ from master measure val)
  */
  int32_t val() const { return mi32_val; }

  /**
    return the ecuType_t code for this measureprog
    @return ecuType_t of this measureprog
  */
  IsoName_c::ecuType_t isoNameType () const { return m_ecuType; }

  /**
    return the program active flag
    @return true -> is active
  */
  bool getActive() const { return mb_active; };

  /**
    set the m_ecuType code for this measureprog
    @param ecuType ecuType_t for specification of partner system
  */
  void setISONameType( IsoName_c::ecuType_t ecuType ) {m_ecuType = ecuType;}

private: // Private methods
  /** base function for assignment of element vars for copy constructor and operator= */
  void assignFromSource( const MeasureProgLocal_c& acrc_src );

  /**
    helper function to check val() against limits
    @return true if value sending allowed
  */
  bool minMaxLimitsPassed(Proc_c::doSend_t ren_doSend) const;

  /**
    process a message with an increment for a measuring program
    @param ren_doSend set process data subtype to send (Proc_c::DoNone, Proc_c::DoVal...)
  */
  void processIncrementMsg( ProcDataLocal_c& ac_processData, const ProcessPkg_c& pkg, Proc_c::doSend_t ren_doSend = Proc_c::DoVal);

private: // Private attributes

  /**  last time were value was set */
  int32_t mi32_lastTime;

  /** actual value
      (can differ from masterVal if f.e. value of this program
      was resetted by caller)
  */
  int32_t mi32_val;

  /** last master (eg. main prog or sensor) val  */
  int32_t mi32_lastMasterVal;

  /** gathered information about currently running threshold sub progs */
  List_ThresholdInfo mlist_thresholdInfo;

  /** dynamic array for subprogs */
  Vec_MeasureSubprog mvec_measureSubprog;
  /** specifies which value types should be sent if one subprog triggers */
  Proc_c::doSend_t men_doSend;

  /** stores if programm is active */
  bool mb_active;

  /** isoName type value of caller of program */
  IsoName_c::ecuType_t m_ecuType;
};

}
#endif
