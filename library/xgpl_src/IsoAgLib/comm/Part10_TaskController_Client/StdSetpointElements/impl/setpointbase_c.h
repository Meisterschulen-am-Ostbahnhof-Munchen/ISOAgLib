/*
  setpointbase_c.h: base class for management of setpoints

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef SETPOINT_BASE_H
#define SETPOINT_BASE_H

#include <IsoAgLib/comm/Part10_TaskController_Client/impl/processelementbase_c.h>

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {
class SetpointRegister_c;

/**
  base class for management of setpoints for storing, changing, reading the setpoint values;
  special actions are implemented by SetpointLocal_c or SetpointRemote
  @author Dipl.-Inform. Achim Spangler
*/
class SetpointBase_c : public ProcessElementBase_c {
public: 
  /**
    default constructor which can set all member vars
    @param apc_processData optional pointer to containing ProcessData instance
  */
  SetpointBase_c( ProcDataLocal_c *const apc_processData = NULL );
  /**
    default constructor which can set all member vars
    @param arc_processData optional reference to containing ProcessData instance
  */
  SetpointBase_c( ProcDataLocal_c &arc_processData );
  /**
    assignement from another object
    @param acrc_src source SetpointBase_c instance
    @return reference to source for cmd like "setp1 = setp2 = setp3;"
  */
  const SetpointBase_c& operator=( const SetpointBase_c& acrc_src );
  /**
    copy constructor for SetpointBase
    @param acrc_src source SetpointBase_c instance
  */
   SetpointBase_c( const SetpointBase_c& acrc_src);
  /** default destructor which has nothing to do */
  virtual ~SetpointBase_c();
  /**
    init this item after the containing object item 
    was inserted in a list completely
    @param apc_data pointer to containing ProcessData instance
  */
  void init( ProcDataLocal_c* apc_data = 0 );
  /**
    perform periodic actions
    @return true -> all planned executions performed
  */
  virtual bool timeEvent( void );
  /**  process a setpoint message */
  void processMsg( const ProcessPkg_c& pkg );
  /**  process a setpoint set command (pure virtual method)  */
  virtual void processSet( const ProcessPkg_c& pkg ) = 0;
  /** process a setpoint request command (pure virtual method) */
  virtual void processRequest( const ProcessPkg_c& pkg ) const = 0;

protected: // Protected methods
  /**
    send the values of an setpoint entry; if wanted 
    the values can be overridden with a special value
  
    possible errors:
        * dependant error in ProcDataBase_c commander of this setpoint isn't found in Monitor List
        * dependant error in CanIo_c on CAN send problems
    @param acrc_src reference to SetpointRegister_c with registered setpoints
    @param ab_override true -> override registered setpoint with ai32_overrideVal
    @param ai32_overrideVal value which can override registered setpoint on ab_override == true
  */
  void sendSetpointVals( const SetpointRegister_c& acrc_src,
                          bool ab_override = false, int32_t ai32_overrideVal = 0)const;
};

}
#endif
