/***************************************************************************
                          setpointlocal_c.h - object for managing setpoints
                                             commanded from remote to local
                                             process data objects
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
#ifndef SETPOINT_LOCAL_H
#define SETPOINT_LOCAL_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include "../../../StdSetpointElements/impl/setpointbase_c.h"
#include "../../../StdSetpointElements/impl/setpointregister_c.h"
#include "../../../impl/proc_c.h"

#if defined(SYSTEM_PC) && !defined(SYSTEM_PC_VC) && !defined(SYSTEM_A1)
  #include <ext/slist>
  namespace std { using __gnu_cxx::slist;};
#else
  #include <slist>
#endif

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {
class ProcDataLocalBase_c;

/**
  object for managing setpoints commanded from remote to local process data objects;
  handling of received setpoints;
  managing the master setpoint, which is used for controlling;
  help for sending correct notify;
  semi automatic and manual handling of received values
  @author Dipl.-Inform. Achim Spangler
*/
class SetpointLocal_c : public SetpointBase_c  {
private:
  typedef STL_NAMESPACE::slist<SetpointRegister_c> Vec_SetpointRegister;
  typedef STL_NAMESPACE::slist<SetpointRegister_c>::iterator Vec_SetpointRegisterIterator;
  typedef STL_NAMESPACE::slist<SetpointRegister_c>::const_iterator Vec_SetpointRegisterConstIterator;

public:
  /**
    default constructor which can set needed pointers to containing objects
    @param rpc_processData pointer to containing ProcessData instance
  */
  SetpointLocal_c(
    ProcDataBase_c *const rpc_processData = NULL ) : SetpointBase_c( rpc_processData )
    {init( rpc_processData );};

  /**
    initialise this SetpointLocal_c to a well defined starting condition
    @param rpc_processData pointer to containing ProcessData instance
  */
  void init( ProcDataBase_c *const rpc_processData );
  /**
    assginment from another object
    @param rrefc_src source SetpointLocal_c instance
    @return reference to source for cmd like "setp1 = setp2 = setp3;"
  */
  const SetpointLocal_c& operator=( const SetpointLocal_c& rrefc_src );
  /**
    copy constructor for SetpointLocal
    @param rrefc_src source SetpointLocal_c instance
  */
   SetpointLocal_c( const SetpointLocal_c& rrefc_src );
  /** default destructor which has nothing to do */
  ~SetpointLocal_c();

  /**
    retreive simple master setpoint
    @return actual received setpoint value (calculated with setpoint )
  */
  int32_t setpointMasterVal() const {return pc_master->exact();};
  /**
    set the setpoint value
    @param ri32_val new setpoint value
  */
  void setSetpointMasterVal( int32_t ri32_val )
    { if (!existMaster()) setStaticMaster();
      setMasterMeasurementVal( ri32_val );
    };
  #ifdef USE_FLOAT_DATA_TYPE
  /**
    retreive simple master setpoint
    @return actual received setpoint value (calculated with setpoint )
  */
  float setpointMasterValFloat() const {return pc_master->exactFloat();};
  /**
    set the setpoint value as float value
    @param rf_val new setpoint value
  */
  void setSetpointMasterVal( float rf_val )
    { if (!existMaster()) setStaticMaster();
      setMasterMeasurementVal( rf_val );
    };
  #endif

  /**
    set if master setpoint should be preserved even if caller
    isoName is no more active (default false )
    @param rb_static choose if master setpoint should be preserved
          if caller isn't acitve
  */
  void setStaticMaster( bool rb_static = true )
    {b_staticMaster = rb_static;};
  /**
    check if actual a master setpoint item is defined
    @see master
    @return true -> an active master setpoint exist
  */
  bool existMaster() const {return pc_master != vec_register.end();};
  /**
    deliver the actual master entry
    @see existMaster
    @return actual valid master setpoint
  */
  SetpointRegister_c& master(){return *pc_master;};
  /**
    deliver the actual master entry
    @see existMaster
    @return actual valid master setpoint
  */
  const SetpointRegister_c& masterConst() const {return *pc_master;};
  /**
    check if unhandled new setpoint from actual master was received
    -> check the actual master first before answering the other unhandled
    @see unhandledMaster
    @see acceptNewMaster
    @return true -> master setpoint comanding member sent new setpoint
  */
  bool existUnhandledMaster();
  /**
    deliver the new sent setpoint values from the actual master;
    if no unhandled setpoint of the master exist, return the actual accepted value

    possible errors:
        * Err_c::elNonexistent no master setpoint found
    @see existUnhandledMaster
    @see acceptNewMaster
    @return reference to master setpoint register
  */
  SetpointRegister_c& unhandledMaster();
  /**
    if there exist a new unhandled setpoint from the master setpoint sender;
    accept the new value with this command
    otherwise the old value is still used and answered

    possible errors:
        * Err_c::elNonexistent no master setpoint found
    @see existUnhandledMaster
    @see unhandledMaster
    @param rb_accept true ( default ) -> new setpoint of master is accepted -> send positive notify
  */
  void acceptNewMaster( bool rb_accept = true );
  /**
    set the master setpoint manually
    (in some cases remote systems request informations
     through process data setpoints )
    @param ri32_val wanted setpoint value
  */
  void setMasterMeasurementVal( int32_t ri32_val );
#ifdef USE_FLOAT_DATA_TYPE
  /**
    set the master setpoint manually
    (in some cases remote systems request informations
     through process data setpoints )
    @param rf_val wanted setpoint value
  */
  void setMasterMeasurementVal( float rf_val );
#endif
  /**
    deliver the count of unhandled setpoints
    @return count of unhandled received setpoints
  */
  uint8_t unhandledCnt()const;
  /**
    deliver the rui8_ind of the unhandled setpoints
    @param rui8_ind position of the wanted setpoint entry in list of unhandled setpoints
      (first entry: rui8_ind == 0!!)

    possible errors:
        * Err_c::range there are less than rui8_ind unhandled setpoints found
    @see unhandledCnt
    @return wanted unhandled setpoint
  */
  SetpointRegister_c& unhandledInd( uint8_t rui8_ind );
  /**
    deliver the first unhandled entry

    possible errors:
        * Err_c::elNonexistent no unhandled stepoint exist
    @see unhandledCnt
    @return reference to first unhandled new received setpoint entry
  */
  SetpointRegister_c& unhandledFirst();
  /**
    answer all unhandled setpoint enties;
    if new master setpoint was set, send the master
    the new accepted setpoint value;
    send all others NO_VAL_32S

    possible errors:
        * dependant error in ProcDataLocalBase_c commander of some setpoint isn't found in Monitor List
        * dependant error in CANIO_c on CAN send problems
  */
  void respondAckNack();
  /**
    set the allowed percentual deviation between actual measurement and
    existing master setpoint
    @param rb_percent allowed percantual deviation
  */
  void setAllowedDeltaPercent( uint8_t rb_percent){b_allowedDeltaPercent = rb_percent;};
  /**
    get the allowed percentual deviation between actual measurement and
    existing master setpoint
    @return allowed percantual deviation
  */
  uint8_t allowedDeltaPercent()const{return b_allowedDeltaPercent;};
  /**
    check if the given measuremet value is correct for the actual
    master setpoint;
    @param ri32_val measured value
    @param rb_sendIfError true -> if actual value exceeds setpoint limits
           the actual value is sent as notification (default true )
    @return 0 -> correct; (n<0) -> measurement is delta n to low;
            (n>0) -> measurement is delta n to high
  */
  int32_t checkMeasurement( int32_t ri32_val, bool rb_sendIfError = true );
  /**
    perform periodic actions
    (here: check if measure val is in limits; delete old handled not master setpoints and
            delete master entry if it's isoName isn't registered active any more )
    @return true -> all planned activities performed in allowed time
  */
  virtual bool timeEvent( void );


  /**
     send a sub-setpoint (selected by MOD) to a specified target (selected by GPT)
     @param GeneralCommand_c::ValueGroup_t min/max/exact/default code of the value type to send
     @param rc_targetISOName ISOName of target
     @param en_valueGroup: min/max/exact/default
     @param en_command
     @return true -> successful sent
  */
  bool sendSetpointMod(const ISOName_c& rc_targetISOName,
                       GeneralCommand_c::ValueGroup_t en_valueGroup = GeneralCommand_c::noValue,
                       GeneralCommand_c::CommandType_t en_command = GeneralCommand_c::noCommand) const;

  /**
    send a exact-setpoint to a specified target (selected by GPT)
    @param rc_targetISOName ISOName of target
    @return true -> successful sent
  */
  bool sendMasterSetpointVal( const ISOName_c& rc_targetISOName) const
   { return sendSetpointMod(rc_targetISOName, GeneralCommand_c::exactValue, GeneralCommand_c::setValue );};

private: // Private methods
  /** base function for assignment of element vars for copy constructor and operator= */
  void assignFromSource( const SetpointLocal_c& rrefc_src );
  /**
    process a setpoint request for local process data
  */
  virtual void processRequest() const;
  /**
    process a setpoint set for local process data
  */
  virtual void processSet();

  /**
    deliver a reference to ProcDataLocal_c
    (the base function only delivers ProcDataBase_c )
    @return reference to containing ProcDataLocalBase_c instance
  */
  ProcDataLocalBase_c& processData()
  {
    return *((ProcDataLocalBase_c*)((void*)ProcessElementBase_c::pprocessData()));
  };
  /**
    deliver a reference to ProcDataLocal_c
    (the base function only delivers ProcDataBase_c )
    @return reference to containing ProcDataLocalBase_c instance
  */
  const ProcDataLocalBase_c& processDataConst() const
  {
    return *((ProcDataLocalBase_c*)((void*)ProcessElementBase_c::pprocessData()));
  };
  /**
    deliver a pointer to ProcDataLocal_c
    (the base function only delivers ProcDataBase_c )
    @return pointer to containing ProcDataLocalBase_c instance
  */
  ProcDataLocalBase_c* pprocessData()
  {
    return ((ProcDataLocalBase_c*)((void*)ProcessElementBase_c::pprocessData()));
  };

private: // Private attributes
  /** container of registered setpoint values */
  Vec_SetpointRegister vec_register;
  /** iterator cache to actual master setpoint */
  mutable Vec_SetpointRegisterIterator pc_registerCache;
  /** SetpointRegister_c entry, which represents the actual used setpoint value */
  Vec_SetpointRegisterIterator pc_master;
  /**
    maximal allowed setpoint (updated according to system state by main application )
    --> incoming setpoints can be handled automatic
  */
  int32_t i32_setpointMaxAllowed;
  /**
    minimum allowed setpoint (updated according to system state by main application )
    --> incoming setpoints can be handled automatic
  */
  int32_t i32_setpointMinAllowed;
  /* allowed variation between setpoint and measurement */
  uint8_t b_allowedDeltaPercent;
  /**
    set if master setpoint should be preserved even if caller
    isoName is no more active (default false )
  */
  bool b_staticMaster;

};

}
#endif
