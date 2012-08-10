/***************************************************************************
                          setpointlocal_c.h - object for managing setpoints

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef SETPOINT_LOCAL_H
#define SETPOINT_LOCAL_H

#include <IsoAgLib/comm/Part10_TaskController_Client/impl/processelementbase_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/StdSetpointElements/impl/setpointregister_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/proc_c.h>

#include <list>

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {
class ProcDataLocal_c;
class SetpointRegister_c;

/**
  object for managing setpoints commanded from remote to local process data objects;
  handling of received setpoints;
  managing the master setpoint, which is used for controlling;
  help for sending correct notify;
  semi automatic and manual handling of received values
  @author Dipl.-Inform. Achim Spangler
*/
class SetpointLocal_c : public ProcessElementBase_c  {
private:
  typedef STL_NAMESPACE::list<SetpointRegister_c> Vec_SetpointRegister;
  typedef STL_NAMESPACE::list<SetpointRegister_c>::iterator Vec_SetpointRegisterIterator;
  typedef STL_NAMESPACE::list<SetpointRegister_c>::const_iterator Vec_SetpointRegisterConstIterator;

public:
  /**
    default constructor which can set needed pointers to containing objects
    @param apc_processData pointer to containing ProcessData instance
  */
  SetpointLocal_c( ProcDataLocal_c *const apc_processData = NULL ) : ProcessElementBase_c( apc_processData)
    {init( apc_processData );};

  /**
    initialise this SetpointLocal_c to a well defined starting condition
    @param apc_processData pointer to containing ProcessData instance
  */
  void init( ProcDataLocal_c *const apc_processData );
  /**
    assginment from another object
    @param acrc_src source SetpointLocal_c instance
    @return reference to source for cmd like "setp1 = setp2 = setp3;"
  */
  const SetpointLocal_c& operator=( const SetpointLocal_c& acrc_src );
  /**
    copy constructor for SetpointLocal
    @param acrc_src source SetpointLocal_c instance
  */
   SetpointLocal_c( const SetpointLocal_c& acrc_src );
  /** default destructor which has nothing to do */
  ~SetpointLocal_c();

  SetpointLocal_c( ProcDataLocal_c &arc_processData );


  /**
    retreive simple master setpoint
    @return actual received setpoint value (calculated with setpoint )
  */
  int32_t setpointVal() const {return mpc_master->value();};
  /**
    set the setpoint value
    @param ai32_val new setpoint value
  */
  void setSetpointVal( int32_t ai32_val )
    { if (!existMaster()) setStaticMaster();
      setMasterMeasurementVal( ai32_val );
    };

  /**
    set if master setpoint should be preserved even if caller
    isoName is no more active (default false )
    @param ab_static choose if master setpoint should be preserved
          if caller isn't acitve
  */
  void setStaticMaster( bool ab_static = true )
    {mb_staticMaster = ab_static;};
  /**
    check if actual a master setpoint item is defined
    @see master
    @return true -> an active master setpoint exist
  */
  bool existMaster() const {
    // Keep this argument order for operator !=, otherwise tasking 7.5
    // won't compile:
    return mvec_register.end() != mpc_master;
  }
  /**
    deliver the actual master entry
    @see existMaster
    @return actual valid master setpoint
  */
  SetpointRegister_c& master(){return *mpc_master;};
  /**
    deliver the actual master entry
    @see existMaster
    @return actual valid master setpoint
  */
  const SetpointRegister_c& masterConst() const {return *mpc_master;};
  /**
    set the master setpoint manually
    (in some cases remote systems request informations
     through process data setpoints )
    @param ai32_val wanted setpoint value
  */
  void setMasterMeasurementVal( int32_t ai32_val );

  /**
    check if the given measuremet value is correct for the actual
    master setpoint;
    @param ai32_val measured value
    @param ab_sendIfError true -> if actual value exceeds setpoint limits
           the actual value is sent as notification (default true )
    @return 0 -> correct; (n<0) -> measurement is delta n to low;
            (n>0) -> measurement is delta n to high
  */
  int32_t checkMeasurement( int32_t ai32_val, bool ab_sendIfError = true );
  /**
    perform periodic actions
    (here: check if measure val is in limits; delete old handled not master setpoints and
            delete master entry if it's isoName isn't registered active any more )
    @return true -> all planned activities performed in allowed time
  */
  virtual bool timeEvent( void );
  
  /**  process a setpoint message */
  void processMsg( const ProcessPkg_c& pkg );

  /**
     send a sub-setpoint (selected by value group) to a specified target (selected by GPT)
     @param ac_targetISOName ISOName of target
     @param en_valueGroup: min/max/exact/default
     @param en_command
     @return true -> successful sent
  */
  bool sendSetpointForGroup(const IsoName_c& ac_targetISOName,
                            ProcessCmd_c::ValueGroup_t en_valueGroup = ProcessCmd_c::noValue,
                            ProcessCmd_c::CommandType_t en_command = ProcessCmd_c::noCommand) const;

  /**
    send a exact-setpoint to a specified target (selected by GPT)
    @param ac_targetISOName ISOName of target
    @return true -> successful sent
  */
  bool sendMasterSetpointVal( const IsoName_c& ac_targetISOName) const
   { return sendSetpointForGroup(ac_targetISOName, ProcessCmd_c::exactValue, ProcessCmd_c::setValue );};

protected: // Protected methods
  /**
    send the values of an setpoint entry; if wanted 
    the values can be overridden with a special value
  
    possible errors:
        * dependant error in ProcDataLocal_c commander of this setpoint isn't found in Monitor List
        * dependant error in CanIo_c on CAN send problems
    @param acrc_src reference to SetpointRegister_c with registered setpoints
    @param ab_override true -> override registered setpoint with ai32_overrideVal
    @param ai32_overrideVal value which can override registered setpoint on ab_override == true
  */
  void sendSetpointVals( const SetpointRegister_c& acrc_src,
                          bool ab_override = false, int32_t ai32_overrideVal = 0)const;

private: // Private methods
  /** base function for assignment of element vars for copy constructor and operator= */
  void assignFromSource( const SetpointLocal_c& acrc_src );
  /**
    process a setpoint request for local process data
  */
  virtual void processRequest( const ProcessPkg_c& pkg ) const;
  /**
    process a setpoint set for local process data
  */
  virtual void processSet( const ProcessPkg_c& pkg );

  /**
    deliver a reference to ProcDataLocal_c
    (the base function only delivers ProcDataLocal_c )
    @return reference to containing ProcDataLocal_c instance
  */
  ProcDataLocal_c& processData()
  {
    return *((ProcDataLocal_c*)((void*)ProcessElementBase_c::pprocessData()));
  };
  /**
    deliver a reference to ProcDataLocal_c
    (the base function only delivers ProcDataLocal_c )
    @return reference to containing ProcDataLocal_c instance
  */
  const ProcDataLocal_c& processDataConst() const
  {
    return *((ProcDataLocal_c*)((void*)ProcessElementBase_c::pprocessData()));
  };
  /**
    deliver a pointer to ProcDataLocal_c
    (the base function only delivers ProcDataLocal_c )
    @return pointer to containing ProcDataLocal_c instance
  */
  ProcDataLocal_c* pprocessData() const
  {
    return ((ProcDataLocal_c*)((void*)ProcessElementBase_c::pprocessData()));
  };

private: // Private attributes
  /** container of registered setpoint values */
  Vec_SetpointRegister mvec_register;
  /** iterator cache to actual master setpoint */
  mutable Vec_SetpointRegisterIterator mpc_registerCache;
  /** SetpointRegister_c entry, which represents the actual used setpoint value */
  Vec_SetpointRegisterIterator mpc_master;

  /**
    set if master setpoint should be preserved even if caller
    isoName is no more active (default false )
  */
  bool mb_staticMaster;

};

}
#endif
