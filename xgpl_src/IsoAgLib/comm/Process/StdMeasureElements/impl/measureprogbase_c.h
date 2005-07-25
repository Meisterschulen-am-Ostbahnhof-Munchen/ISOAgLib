/***************************************************************************
                          measureprogbase_c.h - base class for measure
                                                values and programs
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
#ifndef MEASUREPROG_BASE_H
#define MEASUREPROG_BASE_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include "measuresubprog_c.h"
// #include "../../impl/processelementbase_c.h"
#include <IsoAgLib/comm/Process/impl/processelementbase_c.h>
#include "../../impl/proc_c.h"

#if defined(SYSTEM_PC) && !defined(SYSTEM_PC_VC) && !defined(SYSTEM_A1)
  #include <ext/slist>
  namespace std { using __gnu_cxx::slist;};
#else
  #include <slist>
#endif

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

/**
  base class for measure values and programs,
  storing, reading, changing of values and subprogs without special reaction,
  which are implemented by MeasureProgLocal_c or MeasureProgRemote
  @author Dipl.-Inform. Achim Spangler
*/
class MeasureProgBase_c : public ProcessElementBase_c {
private:
  #ifdef OPTIMIZE_HEAPSIZE_IN_FAVOR_OF_SPEED
  typedef STL_NAMESPACE::slist<MeasureSubprog_c,STL_NAMESPACE::__malloc_alloc_template<0> > Vec_MeasureSubprog;
  typedef STL_NAMESPACE::slist<MeasureSubprog_c,STL_NAMESPACE::__malloc_alloc_template<0> >::iterator Vec_MeasureSubprogIterator;
  #else
  typedef STL_NAMESPACE::slist<MeasureSubprog_c> Vec_MeasureSubprog;
  typedef STL_NAMESPACE::slist<MeasureSubprog_c>::iterator Vec_MeasureSubprogIterator;
  #endif

public:
  /** allow explicit MeasureProgLocal_c the access to private elements */
  friend class MeasureProgLocal_c;
  /** allow explicit MeasureProgRemote_c the access to private elements */
  friend class MeasureProgRemote_c;

  /**
    constructor which can optional set most element vars
    @param rpc_processData optional pointer to containing ProcDataBase_c instance (default NULL)
    @param ren_progType optional program type: Proc_c::Base, Proc_c::Target (default Proc_c::UndefinedProg)
    @param ri32_val optional individual measure val for this program instance (can differ from master measure value)
    @param rc_gtp optional GETY_POS of partner member for this measure program
  */
  MeasureProgBase_c(
    ProcDataBase_c *const rpc_processData = NULL,
    Proc_c::progType_t ren_progType = Proc_c::UndefinedProg,
    int32_t ri32_val = 0,
    const GetyPos_c& rc_gtp = GetyPos_c::GetyPosUnspecified ) : ProcessElementBase_c(rpc_processData),
      vec_measureSubprog() {init(rpc_processData, ren_progType, ri32_val, rc_gtp);};
  /**
    constructor which can optional set most element vars
    @param rrefc_processData optional reference to containing ProcDataBase_c instance (default NULL)
    @param ren_progType optional program type: Proc_c::Base, Proc_c::Target (default Proc_c::UndefinedProg)
    @param ri32_val optional individual measure val for this program instance (can differ from master measure value)
    @param rc_gtp optional GETY_POS of partner member for this measure program
  */
  MeasureProgBase_c(
    ProcDataBase_c &rrefc_processData,
    Proc_c::progType_t ren_progType = Proc_c::UndefinedProg,
    int32_t ri32_val = 0,
    const GetyPos_c& rc_gtp = GetyPos_c::GetyPosUnspecified) : ProcessElementBase_c(rrefc_processData),
      vec_measureSubprog() {init(&rrefc_processData, ren_progType, ri32_val, rc_gtp);};
  /**
    initialise the measure prog instance, to set this instance to a well defined starting condition
    @param rrefc_processData optional reference to containing ProcDataBase_c instance (default NULL)
    @param ren_progType optional program type: Proc_c::Base, Proc_c::Target (default Proc_c::UndefinedProg)
    @param ri32_val optional individual measure val for this program instance (can differ from master measure value)
    @param rc_gtp optional GETY_POS of partner member for this measure program
  */
  void init(
    ProcDataBase_c *const rpc_processData,
    Proc_c::progType_t ren_progType = Proc_c::UndefinedProg,
    int32_t ri32_val = 0,
    const GetyPos_c& rc_gtp = GetyPos_c::GetyPosUnspecified);
  /**
    assignment of MeasureProgBase_c objects
    @param rrefc_src source MeasureProgBase_c instance
    @return reference to source instance for cmd like "prog1 = prog2 = prog3;"
  */
  const MeasureProgBase_c& operator=(const MeasureProgBase_c& rrefc_src);
  /**
    copy constructor
    @param rrefc_src source MeasureProgBase_c instance
  */
   MeasureProgBase_c(const MeasureProgBase_c& rrefc_src);
  /** default destructor which has nothing to do */
  virtual ~MeasureProgBase_c();

  /**
    add an aditional subprog or update if one with same kind exist already

    possible errors:
        * Err_c::badAlloc not enough memory to add new subprog
    @param ren_type increment type: Proc_c::TimeProp, Proc_c::DistProp, Proc_c::ValIncr
    @param ri32_increment increment value
    @return always true; only relevant for overoaded methods in derived classes
  */
  bool addSubprog(Proc_c::type_t ren_type, int32_t ri32_increment);
  /**
    LBS+ uses positive values even for time proportional measure prog
    -> only the start cmd choose increment type
    -> search for forced increment type and set first to according type if needed
  */
  void forceSubprogType(Proc_c::type_t ren_type);
  /**
    start a measuring programm
    @param ren_progType wanted msg type for measure prog (Proc_c::Base, Proc_c::Target)
    @param ren_type wanted increment type (Proc_c::TimeProp, Proc_c::DistProp, Proc_c::ValIncr)
    @param ren_doSend set process data subtype to send (Proc_c::DoNone, Proc_c::DoVal, Proc_c::DoMed, Proc_c::DoInteg)
    @return always true; only relevant for overoaded methods in derived classes
  */
  virtual bool start(Proc_c::progType_t ren_progType, Proc_c::type_t ren_type,
                        Proc_c::doSend_t ren_doSend);
  /**
    stop all running subprog
    @return always true; only relevant for overoaded methods in derived classes
  */
  virtual bool stop();
  /**
    check if this measure prog is running
    @return true -> program is running
  */
  bool started()
    {return (en_doSend == Proc_c::DoNone)?false:true;};

  /**
    deliver actual last received value
    @param rb_sendRequest choose wether a request for value update should be
        sent (default false == send no request)
    @return measure val for this prog (can differ from master measure val)
  */
  int32_t val(bool rb_sendRequest = false) const;
  /**
    deliver the medium value of a measure program (pure virtual function)
    @param rb_sendRequest choose wether a request for value update should be
        sent (default false == send no request)
  */
  virtual int32_t med(bool rb_sendRequest = false) const = 0;
  /**
    deliver integ val
    @param rb_sendRequest choose wether a request for value update should be
        sent (default false == send no request)
    @return integral val for this measure prog
  */
  int32_t integ(bool rb_sendRequest = false) const;
  /**
    deliver min val
    @param rb_sendRequest choose wether a request for value update should be
        sent (default false == send no request)
    @return MIN val for this measure prog
  */
  int32_t min(bool rb_sendRequest = false) const;
  /**
    deliver max val
    @param rb_sendRequest choose wether a request for value update should be
        sent (default false == send no request)
    @return MAX val for this measure prog
  */
  int32_t max(bool rb_sendRequest = false) const;
  /**
    deliver the delta
    @return:delta between the last two measure vals [1/sec]
  */
  int32_t delta() const {return i32_delta;};
  /**
    deliver the acceleration
    @return acceleration calculated from the last delta values [1/(sec*sec)]
  */
  int32_t accel() const {return i32_accel;};
#ifdef USE_FLOAT_DATA_TYPE
  /**
    initialise the measure prog instance, to set this instance to a well defined starting condition
    @param rrefc_processData optional reference to containing ProcDataBase_c instance (default NULL)
    @param ren_progType optional program type: Proc_c::Base, Proc_c::Target (default Proc_c::UndefinedProg)
    @param rf_val optional individual measure val for this program instance (can differ from master measure value)
    @param rc_gtp optional GETY_POS of partner member for this measure program
  */
  void init(
    ProcDataBase_c *const rpc_processData,
    Proc_c::progType_t ren_progType,
    float rf_val,
    const GetyPos_c& rc_gtp = GetyPos_c::GetyPosUnspecified);
  /**
    deliver actual last received value
    @param rb_sendRequest choose wether a request for value update should be
        sent (default false == send no request)
    @return measure val for this prog (can differ from master measure val)
  */
  float valFloat(bool rb_sendRequest = false) const;
  /**
    deliver the medium value of a measure program (pure virtual function)
    @param rb_sendRequest choose wether a request for value update should be
        sent (default false == send no request)
  */
  virtual float medFloat(bool rb_sendRequest = false) const = 0;
  /**
    deliver integ val
    @param rb_sendRequest choose wether a request for value update should be
        sent (default false == send no request)
    @return integral val for this measure prog
  */
  float integFloat(bool rb_sendRequest = false) const;
  /**
    deliver min val
    @param rb_sendRequest choose wether a request for value update should be
        sent (default false == send no request)
    @return MIN val for this measure prog
  */
  float minFloat(bool rb_sendRequest = false) const;
  /**
    deliver max val
    @param rb_sendRequest choose wether a request for value update should be
        sent (default false == send no request)
    @return MAX val for this measure prog
  */
  float maxFloat(bool rb_sendRequest = false) const;
  /**
    deliver the delta
    @return:delta between the last two measure vals [1/sec]
  */
  float deltaFloat() const {return f_delta;};
  /**
    deliver the acceleration
    @return acceleration calculated from the last delta values [1/(sec*sec)]
  */
  float accelFloat() const {return f_accel;};
#endif
  /**
    return the c_gtp code for this measureprog
    @return GETY_POS of this measureprog
  */
  const GetyPos_c& gtp() const{return c_gtp;};
  /**
    deliver the type of the active increment types
    @return actual Bit-OR combined increment types
  */
  Proc_c::type_t type() const {return en_type;};
  /**
    check if the given increment type is active
    @param ren_type tested increment type
    @return true -> given increment type is set
  */
  bool checkType(Proc_c::type_t ren_type) const
    {return ((en_type & ren_type) > 0)?true:false;};
  /**
    return the program type of the item
    @return ProgType: Proc_c::UndefinedProg, Proc_c::Base, Proc_c::Target
  */
  uint8_t progType() const{return static_cast<uint8_t>(en_progType);};
  /**
    check if given progType (base, target) is active
    @param ren_progType tested Prog-Type
    @return true -> given Prog-Type is set
  */
  bool checkProgType(Proc_c::progType_t ren_progType) const
    {return ((en_progType & ren_progType) > 0)?true:false;};
  /**
    check if given send type is activated
    @param ren_doSend tested sended value
    @return true -> given value is sent on trigger
  */
  bool checkDoSend(Proc_c::doSend_t ren_doSend) const
    {return ((en_doSend & ren_doSend) > 0)?true:false;};

  /**
    init the element vars
    @param ri32_val initial measure val
  */
  virtual void initVal(int32_t ri32_val);
  /**
    reset the value (pure virtual function)
    @param ri32_val reset measure value to this value (ISO remote only)
  */
  virtual bool resetVal(int32_t ri32_val = 0) = 0;
#ifdef USE_FLOAT_DATA_TYPE
  /**
    init the element vars with float value
    @param rf_val initial measure val
  */
  virtual void initVal(float rf_val);
#endif
  /**
    reset the medium (pure virtual function)
  */
  virtual bool resetMed() = 0;
  /**
    reset the integral (pure virtual function)
  */
  virtual bool resetInteg() = 0;
  /**
    reset MIN (pure virtual function)
  */
  virtual bool resetMin() = 0;
  /**
    reset MAX (pure virtual function)
  */
  virtual bool resetMax() = 0;

  /**
    set a new value (pure virtual function)
    @param ri32_val new measure value
  */
  virtual void setVal(int32_t ri32_val) = 0;
#ifdef USE_FLOAT_DATA_TYPE
  /**
    set a new value (pure virtual function)
    @param rf_val new measure value
  */
  virtual void setVal(float rf_val) = 0;
#endif
  /**
    set the programm type of the item
    @param rb_type wanted ProgType: Proc_c::UndefinedProg, Proc_c::Base, Proc_c::Target
  */
  void setProgType(uint8_t rb_type)
    {en_progType = Proc_c::progType_t(rb_type);};
  /**
    set the type of the active increment types
    @param ren_type Bit-OR combined increment type(s)
  */
  void setType(Proc_c::type_t ren_type){en_type = ren_type;};
  /**
    set the c_gtp code for this measureprog
    @param rc_gtp GETY_POS for exact specification of partner system
  */
  // This has something to do with the init failing for the iProcDataRemote_c object. -bac
  void setGtp(const GetyPos_c& rc_gtp){c_gtp = rc_gtp;};

  /**
    process a message;
    MeasureProgBase_c::processMsg is responsible for measure prog
    controlling commands
    @return true -> message was already edited complete
  */
  virtual bool processMsg();

  /**
    perform periodic actions
    @return true -> all planned activities performed in available time
  */
  virtual bool timeEvent( void );

  /**
    compare two items for PRI and GTP
    @param rrefc_right compared object
    @return true -> both instances are equal (GETY_POS and ProgType)
  */
  bool operator==(const MeasureProgBase_c& right) const
    {return (calcCompVal() == right.calcCompVal());};
  /**
    compare two MeasureProg with <
    @param rrefc_right compared object
    @return true -> this instance is < than the other (GETY_POS and ProgType)
  */
  bool operator<(const MeasureProgBase_c& right) const
    {return (calcCompVal() < right.calcCompVal());};
  /**
    compare two MeasureProg with <=
    @param rrefc_right compared object
    @return true -> this instance is <= than the other (GETY_POS and ProgType)
  */
  bool operator<=(const MeasureProgBase_c& right) const
    {return (calcCompVal() <= right.calcCompVal());};
  /**
    compare two MeasureProg with >
    @param rrefc_right compared object
    @return true -> this instance is > than the other (GETY_POS and ProgType)
  */
  bool operator>(const MeasureProgBase_c& right) const
    {return (calcCompVal() > right.calcCompVal());};
  /**
    compare two MeasureProg with >=
    @param rrefc_right compared object
    @return true -> this instance is >= than the other (GETY_POS and ProgType)
  */
  bool operator>=(const MeasureProgBase_c& right) const
    {return (calcCompVal() >= right.calcCompVal());};
protected: // Protected methods
  /**
    reset according to the MOD the appropriate value type

    possible errors:
        * Err_c:range MOD is not in allowed range [0..4]
    @param en_valueGroup MOD of wanted subtype
    @param ri32_val reset measure value to this value (ISO remote only)
  */
  void resetValMod(GeneralCommand_c::ValueGroup_t en_valueGroup, int32_t ri32_val = 0);

  /**
    internal increment the value
    @param ri32_val increment for internal measure val
  */
  void incrVal(int32_t ri32_val){i32_val += ri32_val;};
  /**
    increment the integer value
    @param ri32_val increment for integral
  */
  void incrInteg(int32_t ri32_val){i32_integ += ri32_val;};
  /**
    set min val
    @param ri32_val new MIN value
  */
  void setMin(int32_t ri32_val){i32_min = ri32_val;};
  /**
    set max val
    @param ri32_val new MAN value
  */
  void setMax(int32_t ri32_val){i32_max = ri32_val;};
  /**
    set integ val
    @param ri32_val new integral value
  */
  void setInteg(int32_t ri32_val){i32_integ = ri32_val;};
#ifdef USE_FLOAT_DATA_TYPE
  /**
    internal increment the value
    @param rf_val increment for internal measure val
  */
  void incrVal(float rf_val){f_val += rf_val;};
  /**
    increment the integer value
    @param rf_val increment for integral
  */
  void incrInteg(float rf_val){f_integ += rf_val;};
  /**
    set min val
    @param rf_val new MIN value
  */
  void setMin(float rf_val){f_min = rf_val;};
  /**
    set max val
    @param rf_val new MAN value
  */
  void setMax(float rf_val){f_max = rf_val;};
  /**
    set integ val
    @param rf_val new integral value
  */
  void setInteg(float rf_val){f_integ = rf_val;};
#endif
protected: // Protected attributes
  /**  last time were value was set */
  int32_t i32_lastTime;
#ifdef USE_FLOAT_DATA_TYPE
  union {
    /** actual value
        (can differ from masterVal if f.e. value of this program
        was resetted by caller)
    */
    int32_t i32_val;
    /** actual value as float representation
        (can differ from masterVal if f.e. value of this program
        was resetted by caller)
    */
    float f_val;
  };
  union {
    /** calculated delta value between actual and last val */
    int32_t i32_delta;
    /** calculated delta value between actual and last val */
    float f_delta;
  };
  union {
    /** acceleration between values
        (= delta between last i32_deltaVal and actual i32_deltaVal)
    */
    int32_t i32_accel;
    /** acceleration between values
        (= delta between last f_deltaVal and actual f_deltaVal)
    */
    float f_accel;
  };
#else
  /** actual value
      (can differ from masterVal if f.e. value of this program
      was resetted by caller)
  */
  int32_t i32_val;
  /** calculated delta value between actual and last val */
  int32_t i32_delta;
  /** acceleration between values
      (= delta between last i32_deltaVal and actual i32_deltaVal)
  */
  int32_t i32_accel;
#endif
private: // Private methods
  /** base function for assignment of element vars for copy constructor and operator= */
  void assignFromSource( const MeasureProgBase_c& rrefc_src );
  /**
    calculates a single value from identifying values
    (for easy <,>,...)
    @return single comparison value (depends on GETY_POS and Prog-Type)
  */
  int32_t calcCompVal()const {return ( ( (c_gtp.getGety() << 4) | (c_gtp.getPos()) ) * en_progType);};
  /**
    deliver to rb_mod according measure val type

    possible errors:
        * Err_c:range MOD is not in allowed range [0..6]
    @param en_valueGroup MOD of wanted subtype
    @return value of specified subtype
  */
  int32_t valMod(GeneralCommand_c::ValueGroup_t en_valueGroup) const;
#ifdef USE_FLOAT_DATA_TYPE
  /**
    deliver to rb_mod according measure val type
    as float value

    possible errors:
        * Err_c:range MOD is not in allowed range [0..6]
    @param en_valueGroup of wanted subtype
    @return value of specified subtype
  */
  float valModFloat(GeneralCommand_c::ValueGroup_t en_valueGroup) const;
#endif
  /**
    reset according to the process msg command the appropriate value type
    @param rb_comm command from Scheduler_c reset message
  */
  void reset(uint8_t rb_comm);
  /**
    process a message with an increment for a measuring program

    possible errors:o
        * Err_c::badAlloc not enough memory to add new subprog
  */
  void processIncrementMsg();

private: // Private attributes
#ifdef USE_FLOAT_DATA_TYPE
  union {
    /**
      minimum value (only defined if one proportional prog is active)
    */
    int32_t i32_min;
    /**
      minimum value (only defined if one proportional prog is active)
    */
    float f_min;
  };
  union {
    /**
      maximum value (only defined if one proportional prog is active)
    */
    int32_t i32_max;
    /**
      maximum value (only defined if one proportional prog is active)
    */
    float f_max;
  };
  union {
    /**
      integral value (only defined if one proportional prog is active)
    */
    int32_t i32_integ;
    /**
      integral value (only defined if one proportional prog is active)
    */
    float f_integ;
  };
#else
  /**
    minimum value (only defined if one proportional prog is active)
  */
  int32_t i32_min;
  /**
    maximum value (only defined if one proportional prog is active)
  */
  int32_t i32_max;
  /**
    integral value (only defined if one proportional prog is active)
  */
  int32_t i32_integ;
#endif
  /** dynamic array for subprogs */
  Vec_MeasureSubprog vec_measureSubprog;
  /** specifies which value types should be sent if one subprog triggers */
  Proc_c::doSend_t en_doSend;
  /**
    stores if programm is started with baseProcess msg
    or with targetProcess msg -> this type is used for sending of values
  */
  Proc_c::progType_t en_progType;
  /**
    active increment types: some of {TimeProp, DistProp, ValIncr, DeltaIncr,
                  AccelIncr, MedIncr, MinIncr, MaxIncr, IntegIncr}
  */
  Proc_c::type_t en_type;
  /**
    used proportional type for calculating accumulated values (min, max, med, insteg)
  */
  Proc_c::accumProp_t en_accumProp;
  /** gtp value of caller of program */
  GetyPos_c c_gtp;
};

}
#endif
