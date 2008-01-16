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
#include <IsoAgLib/comm/Part7_ProcessData/impl/processelementbase_c.h>
#include "../../impl/proc_c.h"

#ifdef DO_USE_SLIST
  #if defined(SYSTEM_PC) && !defined(SYSTEM_PC_VC) && !defined(SYSTEM_A1) && __GNUC__ >= 3
    #include <ext/slist>
    namespace std { using __gnu_cxx::slist;}
  #else
    #include <slist>
  #endif
#else
  #include <list>
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
  typedef STL_NAMESPACE::USABLE_SLIST<MeasureSubprog_c,MALLOC_TEMPLATE(MeasureSubprog_c) > Vec_MeasureSubprog;
  typedef STL_NAMESPACE::USABLE_SLIST<MeasureSubprog_c,MALLOC_TEMPLATE(MeasureSubprog_c)>::iterator Vec_MeasureSubprogIterator;
  #else
  typedef STL_NAMESPACE::USABLE_SLIST<MeasureSubprog_c> Vec_MeasureSubprog;
  typedef STL_NAMESPACE::USABLE_SLIST<MeasureSubprog_c>::iterator Vec_MeasureSubprogIterator;
  #endif

public:
  /** allow explicit MeasureProgLocal_c the access to private elements */
  friend class MeasureProgLocal_c;
  /** allow explicit MeasureProgRemote_c the access to private elements */
  friend class MeasureProgRemote_c;

  /**
    constructor which can optional set most element vars
    @param apc_processData optional pointer to containing ProcDataBase_c instance (default NULL)
    @param ai32_val optional individual measure val for this program instance (can differ from master measure value)
    @param ac_isoName optional ISOName of partner member for this measure program
  */
  MeasureProgBase_c(
    ProcDataBase_c *const apc_processData = NULL,
    int32_t ai32_val = 0,
    const IsoName_c& ac_isoName = IsoName_c::IsoNameUnspecified() ) : ProcessElementBase_c(apc_processData),
      mvec_measureSubprog() {init(apc_processData, ai32_val, ac_isoName);}

  /**
    constructor which can optional set most element vars
    @param arc_processData optional reference to containing ProcDataBase_c instance (default NULL)
    @param ai32_val optional individual measure val for this program instance (can differ from master measure value)
    @param ac_isoName optional ISOName of partner member for this measure program
  */
  MeasureProgBase_c(
    ProcDataBase_c &arc_processData,
    int32_t ai32_val = 0,
    const IsoName_c& ac_isoName = IsoName_c::IsoNameUnspecified()) : ProcessElementBase_c(arc_processData),
      mvec_measureSubprog() {init(&arc_processData, ai32_val, ac_isoName);}

  /**
    initialise the measure prog instance, to set this instance to a well defined starting condition
    @param apc_processData optional reference to containing ProcDataBase_c instance (default NULL)
    @param ai32_val optional individual measure val for this program instance (can differ from master measure value)
    @param ac_isoName optional ISOName of partner member for this measure program
  */
  void init(
    ProcDataBase_c *const apc_processData,
    int32_t ai32_val = 0,
    const IsoName_c& ac_isoName = IsoName_c::IsoNameUnspecified());

  /**
    assignment of MeasureProgBase_c objects
    @param arc_src source MeasureProgBase_c instance
    @return reference to source instance for cmd like "prog1 = prog2 = prog3;"
  */
  const MeasureProgBase_c& operator=(const MeasureProgBase_c& arc_src);

  /**
    copy constructor
    @param arc_src source MeasureProgBase_c instance
  */
   MeasureProgBase_c(const MeasureProgBase_c& arc_src);

  /** default destructor which has nothing to do */
  virtual ~MeasureProgBase_c();

  /**
    add an aditional subprog or update if one with same kind exist already

    possible errors:
        * Err_c::badAlloc not enough memory to add new subprog
    @param ren_type increment type: Proc_c::TimeProp, Proc_c::DistProp, ...
    @param ai32_increment increment value
    @param ren_doSend set process data subtype to send (Proc_c::DoNone, Proc_c::DoVal, Proc_c::DoValForExactSetpoint...)
    @return always true; only relevant for overoaded methods in derived classes
  */
  bool addSubprog(Proc_c::type_t ren_type, int32_t ai32_increment, Proc_c::doSend_t ren_doSend = Proc_c::DoVal);

  /**
    set active flag
    @param ab_active
  */
  void setActive(bool ab_active) { mb_active = ab_active; }

  /**
    start a measuring programm
    @param ren_type wanted increment type (Proc_c::TimeProp, Proc_c::DistProp, ...)
    @param ren_doSend set process data subtype to send (Proc_c::DoNone, Proc_c::DoVal, Proc_c::DoValForExactSetpoint...)
    @return always true; only relevant for overoaded methods in derived classes
  */
  virtual bool start(Proc_c::type_t ren_type, Proc_c::doSend_t ren_doSend);

  /**
    stop all running subprog
    @param b_deleteSubProgs is only needed for remote ISO case (but is needed due to overloading here also)
    @param ren_type wanted increment type (Proc_c::TimeProp, Proc_c::DistProp, ...)
    @param ren_doSend set process data subtype to stop (Proc_c::DoNone, Proc_c::DoVal, Proc_c::DoValForExactSetpoint...)
    @return always true; only relevant for overoaded methods in derived classes
  */
  virtual bool stop(bool b_deleteSubProgs = true, Proc_c::type_t ren_type = Proc_c::NullType,
                    Proc_c::doSend_t ren_doSend = Proc_c::DoNone);

  /**
    check if this measure prog is running
    @return true -> program is running
  */
  bool started()
    {return (men_doSend == Proc_c::DoNone)?false:true;};

  /**
    deliver actual last received value
    @param ab_sendRequest choose wether a request for value update should be
        sent (default false == send no request)
    @return measure val for this prog (can differ from master measure val)
  */
  int32_t val(bool ab_sendRequest = false) const;

  /**
    deliver min val
    @param ab_sendRequest choose wether a request for value update should be
        sent (default false == send no request)
    @return MIN val for this measure prog
  */
  int32_t min(bool ab_sendRequest = false) const;

  /**
    deliver max val
    @param ab_sendRequest choose wether a request for value update should be
        sent (default false == send no request)
    @return MAX val for this measure prog
  */
  int32_t max(bool ab_sendRequest = false) const;

  /**
    deliver the delta
    @return:delta between the last two measure vals [1/sec]
  */
  int32_t delta() const {return mi32_delta;}

  /**
    deliver the acceleration
    @return acceleration calculated from the last delta values [1/(sec*sec)]
  */
  int32_t accel() const {return mi32_accel;}

#ifdef USE_FLOAT_DATA_TYPE
  /**
    initialise the measure prog instance, to set this instance to a well defined starting condition
    @param arc_processData optional reference to containing ProcDataBase_c instance (default NULL)
    @param af_val optional individual measure val for this program instance (can differ from master measure value)
    @param ac_isoName optional ISOName of partner member for this measure program
  */
  void init(
    ProcDataBase_c *const apc_processData,
    float af_val,
    const IsoName_c& ac_isoName = IsoName_c::IsoNameUnspecified());

  /**
    deliver actual last received value
    @param ab_sendRequest choose wether a request for value update should be
        sent (default false == send no request)
    @return measure val for this prog (can differ from master measure val)
  */
  float valFloat(bool ab_sendRequest = false) const;

  /**
    deliver integ val
    @param ab_sendRequest choose wether a request for value update should be
        sent (default false == send no request)
    @return integral val for this measure prog
  */
  float integFloat(bool ab_sendRequest = false) const;

  /**
    deliver min val
    @param ab_sendRequest choose wether a request for value update should be
        sent (default false == send no request)
    @return MIN val for this measure prog
  */
  float minFloat(bool ab_sendRequest = false) const;

  /**
    deliver max val
    @param ab_sendRequest choose wether a request for value update should be
        sent (default false == send no request)
    @return MAX val for this measure prog
  */
  float maxFloat(bool ab_sendRequest = false) const;

  /**
    deliver the delta
    @return:delta between the last two measure vals [1/sec]
  */
  float deltaFloat() const {return f_delta;}

  /**
    deliver the acceleration
    @return acceleration calculated from the last delta values [1/(sec*sec)]
  */
  float accelFloat() const {return f_accel;}
#endif

  /**
    return the mc_isoName code for this measureprog
    @return ISOName of this measureprog
  */
  const IsoName_c& isoName() const{return mc_isoName;}

  /**
    deliver the type of the active increment types
    @return actual Bit-OR combined increment types
  */
  Proc_c::type_t type() const {return men_type;}

  /**
    check if the given increment type is active
    @param ren_type tested increment type
    @return true -> given increment type is set
  */
  bool checkType(Proc_c::type_t ren_type) const
    {return ((men_type & ren_type) > 0)?true:false;}

  /**
    return the program active flag
    @return true -> is active
  */
  bool getActive() const { return mb_active; };

  /**
    check if given send type is activated
    @param ren_doSend tested sended value
    @return true -> given value is sent on trigger
  */
  bool checkDoSend(Proc_c::doSend_t ren_doSend) const
    {return ((men_doSend & ren_doSend) > 0)?true:false;}

  /**
    init the element vars
    @param ai32_val initial measure val
  */
  virtual void initVal(int32_t ai32_val);

  /**
    reset the value (pure virtual function)
    @param ai32_val reset measure value to this value (ISO remote only)
  */
  virtual bool resetVal(int32_t ai32_val = 0) = 0;

#ifdef USE_FLOAT_DATA_TYPE
  /**
    init the element vars with float value
    @param af_val initial measure val
  */
  virtual void initVal(float af_val);
#endif

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
    @param ai32_val new measure value
  */
  virtual void setVal(int32_t ai32_val) = 0;

#ifdef USE_FLOAT_DATA_TYPE
  /**
    set a new value (pure virtual function)
    @param af_val new measure value
  */
  virtual void setVal(float af_val) = 0;
#endif

  /**
    set the type of the active increment types
    @param ren_type Bit-OR combined increment type(s)
  */
  void setType(Proc_c::type_t ren_type){men_type = ren_type;}

  /**
    set the mc_isoName code for this measureprog
    @param ac_isoName ISOName for exact specification of partner system
  */
  // This has something to do with the init failing for the iProcDataRemote_c object. -bac
  void setISOName(const IsoName_c& ac_isoName){mc_isoName = ac_isoName;}

  /**
    process a message;
    MeasureProgBase_c::processMsg is responsible for measure prog
    controlling commands
    @return true -> message was already edited complete
  */
  virtual bool processMsg();

  /**
    perform periodic actions
    @param pui16_nextTimePeriod calculated new time period, based on current measure progs (only for local proc data)
    @return true -> all planned activities performed in available time
  */
  virtual bool timeEvent( uint16_t *pui16_nextTimePeriod = NULL );

  /**
    compare two items for PRI and ISOName
    @param right compared object
    @return true -> both instances are equal (ISOName and active flag)
  */
  bool operator==(const MeasureProgBase_c& right) const
    {return (calcCompVal() == right.calcCompVal());}

  /**
    compare two MeasureProg with <
    @param right compared object
    @return true -> this instance is < than the other (ISOName and active flag)
  */
  bool operator<(const MeasureProgBase_c& right) const
    {return (calcCompVal() < right.calcCompVal());}

  /**
    compare two MeasureProg with <=
    @param right compared object
    @return true -> this instance is <= than the other (ISOName and active flag)
  */
  bool operator<=(const MeasureProgBase_c& right) const
    {return (calcCompVal() <= right.calcCompVal());}

  /**
    compare two MeasureProg with >
    @param arc_right compared object
    @return true -> this instance is > than the other (ISOName and active flag)
  */
  bool operator>(const MeasureProgBase_c& right) const
    {return (calcCompVal() > right.calcCompVal());}

  /**
    compare two MeasureProg with >=
    @param right compared object
    @return true -> this instance is >= than the other (ISOName and active flag)
  */
  bool operator>=(const MeasureProgBase_c& right) const
    {return (calcCompVal() >= right.calcCompVal());}

protected: // Protected methods
  /**
    reset according to the value group the appropriate value
    @param en_valueGroup of wanted subtype
    @param ai32_val reset measure value to this value (ISO remote only)
  */
  void resetValForGroup(ProcessCmd_c::ValueGroup_t en_valueGroup, int32_t ai32_val = 0);

  /**
    internal increment the value
    @param ai32_val increment for internal measure val
  */
  void incrVal(int32_t ai32_val){mi32_val += ai32_val;}

  /**
    set min val
    @param ai32_val new MIN value
  */
  void setMin(int32_t ai32_val){mi32_min = ai32_val;}

  /**
    set max val
    @param ai32_val new MAN value
  */
  void setMax(int32_t ai32_val){mi32_max = ai32_val;}

#ifdef USE_FLOAT_DATA_TYPE
  /**
    internal increment the value
    @param af_val increment for internal measure val
  */
  void incrVal(float af_val){f_val += af_val;}

  /**
    set min val
    @param af_val new MIN value
  */
  void setMin(float af_val){f_min = af_val;}

  /**
    set max val
    @param af_val new MAN value
  */
  void setMax(float af_val){f_max = af_val;}

#endif

protected: // Protected attributes
  /**  last time were value was set */
  int32_t mi32_lastTime;
#ifdef USE_FLOAT_DATA_TYPE
  union {
    /** actual value
        (can differ from masterVal if f.e. value of this program
        was resetted by caller)
    */
    int32_t mi32_val;
    /** actual value as float representation
        (can differ from masterVal if f.e. value of this program
        was resetted by caller)
    */
    float f_val;
  };

  union {
    /** calculated delta value between actual and last val */
    int32_t mi32_delta;
    /** calculated delta value between actual and last val */
    float f_delta;
  };

  union {
    /** acceleration between values
        (= delta between last i32_deltaVal and actual i32_deltaVal)
    */
    int32_t mi32_accel;
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
  int32_t mi32_val;

  /** calculated delta value between actual and last val */
  int32_t mi32_delta;

  /** acceleration between values
      (= delta between last i32_deltaVal and actual i32_deltaVal)
  */
  int32_t mi32_accel;
#endif

private: // Private methods
  /** base function for assignment of element vars for copy constructor and operator= */
  void assignFromSource( const MeasureProgBase_c& arc_src );

  /**
    calculates a single value from identifying values
    (for easy <,>,...)
    @return single comparison value (depends on ISOName and active flag)
  */
  int32_t calcCompVal()const {return ( ( (mc_isoName.devClass() << 4) | (mc_isoName.devClassInst()) ) * (mb_active + 1));}

  /**
    deliver value according measure value group
    @param en_valueGroup of wanted subtype
    @return value of specified subtype
  */
  int32_t valForGroup(ProcessCmd_c::ValueGroup_t en_valueGroup) const;

#ifdef USE_FLOAT_DATA_TYPE
  /**
    deliver value according measure value group
    @param en_valueGroup of wanted subtype
    @return value of specified subtype
  */
  float valForGroupFloat(ProcessCmd_c::ValueGroup_t en_valueGroup) const;
#endif

  /**
    process a message with an increment for a measuring program

    possible errors:o
        * Err_c::badAlloc not enough memory to add new subprog
    @param ren_doSend set process data subtype to send (Proc_c::DoNone, Proc_c::DoVal, Proc_c::DoValForExactSetpoint...)
  */
  void processIncrementMsg(Proc_c::doSend_t ren_doSend = Proc_c::DoVal);

private: // Private attributes
#ifdef USE_FLOAT_DATA_TYPE
  union {
    /**
      minimum value (only defined if one proportional prog is active)
    */
    int32_t mi32_min;
    /**
      minimum value (only defined if one proportional prog is active)
    */
    float f_min;
  };

  union {
    /**
      maximum value (only defined if one proportional prog is active)
    */
    int32_t mi32_max;
    /**
      maximum value (only defined if one proportional prog is active)
    */
    float f_max;
  };

#else

  /**
    minimum value (only defined if one proportional prog is active)
  */
  int32_t mi32_min;

  /**
    maximum value (only defined if one proportional prog is active)
  */
  int32_t mi32_max;

#endif

  /** dynamic array for subprogs */
  Vec_MeasureSubprog mvec_measureSubprog;
  /** specifies which value types should be sent if one subprog triggers */
  Proc_c::doSend_t men_doSend;

  /**
    stores if programm is active
  */
  bool mb_active;

  /**
    active increment types: some of {TimeProp, DistProp, ...}
  */
  Proc_c::type_t men_type;

  /**
    used proportional type for calculating accumulated values (min, max, med, insteg)
  */
  Proc_c::accumProp_t men_accumProp;
  /** isoName value of caller of program */
  IsoName_c mc_isoName;
};

}
#endif
