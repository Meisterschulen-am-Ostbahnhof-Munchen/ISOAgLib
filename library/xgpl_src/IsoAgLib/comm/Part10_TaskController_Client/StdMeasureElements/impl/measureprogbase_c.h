/*
  measureprogbase_c.h: base class for measure values and programs

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef MEASUREPROG_BASE_H
#define MEASUREPROG_BASE_H

#include "measuresubprog_c.h"
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/processelementbase_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/proc_c.h>

#include <list>

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
  typedef STL_NAMESPACE::list<MeasureSubprog_c,MALLOC_TEMPLATE(MeasureSubprog_c) > Vec_MeasureSubprog;
  typedef STL_NAMESPACE::list<MeasureSubprog_c,MALLOC_TEMPLATE(MeasureSubprog_c)>::iterator Vec_MeasureSubprogIterator;
  #else
  typedef STL_NAMESPACE::list<MeasureSubprog_c> Vec_MeasureSubprog;
  typedef STL_NAMESPACE::list<MeasureSubprog_c>::iterator Vec_MeasureSubprogIterator;
  #endif

public:
  /** allow explicit MeasureProgLocal_c the access to private elements */
  friend class MeasureProgLocal_c;

  /**
    constructor which can optional set most element vars
    @param apc_processData optional pointer to containing ProcDataLocal_c instance (default NULL)
    @param ai32_val optional individual measure val for this program instance (can differ from master measure value)
    @param acrc_isoName optional ISOName of partner member for this measure program
  */
  MeasureProgBase_c(
    ProcDataLocal_c *const apc_processData = NULL,
    int32_t ai32_val = 0,
    const IsoName_c& acrc_isoName = IsoName_c::IsoNameUnspecified() ) : ProcessElementBase_c(apc_processData),
      mvec_measureSubprog() {init(apc_processData, ai32_val, acrc_isoName);}

  /**
    constructor which can optional set most element vars
    @param acrc_processData optional reference to containing ProcDataLocal_c instance (default NULL)
    @param ai32_val optional individual measure val for this program instance (can differ from master measure value)
    @param acrc_isoName optional ISOName of partner member for this measure program
  */
  MeasureProgBase_c(
    ProcDataLocal_c &acrc_processData,
    int32_t ai32_val = 0,
    const IsoName_c& acrc_isoName = IsoName_c::IsoNameUnspecified()) : ProcessElementBase_c(acrc_processData),
      mvec_measureSubprog() {init(&acrc_processData, ai32_val, acrc_isoName);}

  /**
    initialise the measure prog instance, to set this instance to a well defined starting condition
    @param apc_processData optional reference to containing ProcDataLocal_c instance (default NULL)
    @param ai32_val optional individual measure val for this program instance (can differ from master measure value)
    @param acrc_isoName optional ISOName of partner member for this measure program
  */
  void init(
    ProcDataLocal_c *const apc_processData,
    int32_t ai32_val = 0,
    const IsoName_c& acrc_isoName = IsoName_c::IsoNameUnspecified());

  /**
    assignment of MeasureProgBase_c objects
    @param acrc_src source MeasureProgBase_c instance
    @return reference to source instance for cmd like "prog1 = prog2 = prog3;"
  */
  const MeasureProgBase_c& operator=(const MeasureProgBase_c& acrc_src);

  /**
    copy constructor
    @param acrc_src source MeasureProgBase_c instance
  */
   MeasureProgBase_c(const MeasureProgBase_c& acrc_src);

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
    {return (men_doSend != Proc_c::DoNone);};

  /**
    deliver actual last received value
    @param ab_sendRequest choose wether a request for value update should be
        sent (default false == send no request)
    @return measure val for this prog (can differ from master measure val)
  */
  int32_t val(bool ab_sendRequest = false) const;

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

  /**
    return the mc_isoName code for this measureprog
    @return ISOName of this measureprog
  */
  const IsoName_c& isoName() const{return mc_isoName;}

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
    {return ((men_doSend & ren_doSend) > 0);}

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

  /**
    set a new value (pure virtual function)
    @param ai32_val new measure value
  */
  virtual void setVal(int32_t ai32_val) = 0;

  /**
    set the mc_isoName code for this measureprog
    @param acrc_isoName ISOName for exact specification of partner system
  */
  // This has something to do with the init failing for the iProcDataRemote_c object. -bac
  void setISOName(const IsoName_c& acrc_isoName){mc_isoName = acrc_isoName;}

  /**
    process a message;
    MeasureProgBase_c::processMsg is responsible for measure prog
    controlling commands
    @return true -> message was already edited complete
  */
  virtual bool processMsg( const ProcessPkg_c& arc_data );

  /**
    compare two items for PRI and ISOName
    @param acrc_right compared object
    @return true -> both instances are equal (ISOName and active flag)
  */
  bool operator==(const MeasureProgBase_c& acrc_right) const
    {return (calcCompVal() == acrc_right.calcCompVal());}

  /**
    compare two MeasureProg with <
    @param acrc_right compared object
    @return true -> this instance is < than the other (ISOName and active flag)
  */
  bool operator<(const MeasureProgBase_c& acrc_right) const
    {return (calcCompVal() < acrc_right.calcCompVal());}

  /**
    compare two MeasureProg with <=
    @param acrc_right compared object
    @return true -> this instance is <= than the other (ISOName and active flag)
  */
  bool operator<=(const MeasureProgBase_c& acrc_right) const
    {return (calcCompVal() <= acrc_right.calcCompVal());}

  /**
    compare two MeasureProg with >
    @param acrc_right compared object
    @return true -> this instance is > than the other (ISOName and active flag)
  */
  bool operator>(const MeasureProgBase_c& acrc_right) const
    {return (calcCompVal() > acrc_right.calcCompVal());}

  /**
    compare two MeasureProg with >=
    @param acrc_right compared object
    @return true -> this instance is >= than the other (ISOName and active flag)
  */
  bool operator>=(const MeasureProgBase_c& acrc_right) const
    {return (calcCompVal() >= acrc_right.calcCompVal());}

protected: // Protected methods
  /**
    reset according to the value group the appropriate value
    @param en_valueGroup of wanted subtype
    @param ai32_val reset measure value to this value (ISO remote only)
  */
  void resetValForGroup(int32_t ai32_val = 0);
  /**
    internal increment the value
    @param ai32_val increment for internal measure val
  */
  void incrVal(int32_t ai32_val){mi32_val += ai32_val;}

protected: // Protected attributes
  /**  last time were value was set */
  int32_t mi32_lastTime;

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

private: // Private methods
  /** base function for assignment of element vars for copy constructor and operator= */
  void assignFromSource( const MeasureProgBase_c& acrc_src );

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
  int32_t valForGroup() const;
  /**
    process a message with an increment for a measuring program

    possible errors:o
        * Err_c::badAlloc not enough memory to add new subprog
    @param ren_doSend set process data subtype to send (Proc_c::DoNone, Proc_c::DoVal, Proc_c::DoValForExactSetpoint...)
  */
  void processIncrementMsg( const ProcessPkg_c& pkg, Proc_c::doSend_t ren_doSend = Proc_c::DoVal);

private: // Private attributes

  /** dynamic array for subprogs */
  Vec_MeasureSubprog mvec_measureSubprog;
  /** specifies which value types should be sent if one subprog triggers */
  Proc_c::doSend_t men_doSend;

  /** stores if programm is active */
  bool mb_active;

  /** used proportional type for calculating accumulated values (min, max, med, insteg) */
  Proc_c::accumProp_t men_accumProp;
  /** isoName value of caller of program */
  IsoName_c mc_isoName;
};

}
#endif
