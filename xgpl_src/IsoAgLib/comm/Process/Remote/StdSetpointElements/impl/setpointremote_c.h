/***************************************************************************
                          setpointremote_c.h - object for commanding remote
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
#ifndef SETPOINT_REMOTE_H
#define SETPOINT_REMOTE_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include "../../../StdSetpointElements/impl/setpointbase_c.h"
#include "../../../StdSetpointElements/impl/setpointregister_c.h"

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {
class ProcDataRemoteBase_c;

/**
  object for commanding remote process data objects;
  helps checking,
  - if commanded value is accepted,
  - if other setpoint is handled as master,
  - if this item is valid or invalid (detect by actual measure value)
  @author Dipl.-Inform. Achim Spangler
*/
class SetpointRemote_c : public SetpointBase_c  {
private:
public:
  /**
    default constructor which can set needed pointers to containing objects
    @param rpc_processData pointer to containing ProcessData instance
  */
  SetpointRemote_c(
    ProcDataBase_c *const rpc_processData = NULL ) : SetpointBase_c(rpc_processData)
    {init(rpc_processData);};
  /**
    initialise this SetpointRemote_c to a well defined starting condition
    @param rpc_processData pointer to containing ProcessData instance
  */
  void init( ProcDataBase_c *const rpc_processData );
  /**
    assginment from another object
    @param rrefc_src source SetpointRemote_c instance
    @return reference to source for cmd like "setp1 = setp2 = setp3;"
  */
  const SetpointRemote_c& operator=(const SetpointRemote_c& rrefc_src);
  /**
    copy constructor for SetpointRemote
    @param rrefc_src source SetpointRemote_c instance
  */
   SetpointRemote_c(const SetpointRemote_c& rrefc_src);
  /** default destructor which has nothing to do */
  ~SetpointRemote_c();
  /**
    deliver the actual percent setpoint
    @param rb_sendRequest true -> send request for actual value
    @return percent setpoint value
  */
  uint8_t setpointPercentVal(bool rb_sendRequest = false) const
    {if (rb_sendRequest) requestPercent();
     return master().percent();};
  /**
    send a setpoint cmd with given percent setpoint
    @param rb_val commanded setpoint percent value
  */
  void setSetpointPercentVal(uint8_t rb_val){ setPercent(rb_val); };
  /**
    deliver the actual master setpoint
    @param rb_sendRequest true -> send request for actual value
    @return setpoint value as long
  */
  int32_t setpointMasterVal(bool rb_sendRequest = false) const
    {if (rb_sendRequest) requestExact();
     return master().exact();};
  /**
    send a setpoint cmd with given exact setpoint
    @param ri32_val commanded setpoint value as long
  */
  void setSetpointMasterVal(int32_t ri32_val){setExact(ri32_val);};
  #ifdef USE_FLOAT_DATA_TYPE
  /**
    deliver the actual master setpoint
    @param rb_sendRequest true -> send request for actual value
    @return setpoint value as float
  */
  float setpointMasterValFloat(bool rb_sendRequest = false) const
    {if (rb_sendRequest) requestExact();
     return master().exactFloat();};
  /**
    send a setpoint cmd with given exact setpoint
    @param rf_val commanded setpoint value as float
  */
  void setSetpointMasterVal(float rf_val){ setExact(rf_val); };
  /**
    command a exact setpoint; store value as commanded and send command

    possible errors:
        * dependant error in ProcDataRemoteBase_c if comanded remote system not found in Monitor List
        * dependant error in CANIO_c on CAN send problems
    @return new exact setpoint to command
  */
  void setExact(float rf_val);
  #endif

  /**
    command a exact setpoint; store value as commanded and send command

    possible errors:
        * dependant error in ProcDataRemoteBase_c if comanded remote system not found in Monitor List
        * dependant error in CANIO_c on CAN send problems
    @return new exact setpoint to command
  */
  void setExact(int32_t ri32_val);
  /**
    command a percent setpoint; store value as commanded and send command

    possible errors:
        * dependant error in ProcDataRemoteBase_c if comanded remote system not found in Monitor List
        * dependant error in CANIO_c on CAN send problems
    @return new percantage setpoint to command
  */
  void setPercent(int32_t ri32_val);
  /**
    command a minimum setpoint; store value as commanded and send command

    possible errors:
        * dependant error in ProcDataRemoteBase_c if comanded remote system not found in Monitor List
        * dependant error in CANIO_c on CAN send problems
    @return new minimum setpoint to command
  */
  void setMin(int32_t ri32_val);
  /**
    command a maximum setpoint; store value as commanded and send command

    possible errors:
        * dependant error in ProcDataRemoteBase_c if comanded remote system not found in Monitor List
        * dependant error in CANIO_c on CAN send problems
    @return new maximum setpoint to command
  */
  void setMax(int32_t ri32_val);
  /**
    request remote master setpoint - exact
  */
  void requestExact() const;
  /**
    request remote master setpoint - percent
  */
  void requestPercent() const;
  /**
    request remote master setpoint - MIN
  */
  void requestMin() const;
  /**
    request remote master setpoint - MAX
  */
  void requestMax() const;

  /**
    check if the last commanded setpoint was accepted
    (am I am now the master)
    @return true -> the last commanded setpoint was accepted by remote system
  */
  bool accepted() const;
  /**
    check if the last setpoint command was answered
    @return true -> an answer to the last command was received
  */
  bool answered() const
    {return (i32_answeredTime > i32_commandedTime)?true:false;};
  /**
    check if an active master setpoint exist
    @return true -> a master setpoint commander exist
  */
  bool existMaster() const;
  /**
    deliver master setpoint entry
    @return reference to master setpoint
  */
  const SetpointRegister_c& master() const {return c_answeredMaster;};
  /**
    mark the master setpoint as released
    (send a suitable reset command)

    possible errors:
        * Err_c::precondition I'm not the master -> can't release master
        * dependant error in ProcDataRemoteBase_c if comanded remote system not found in Monitor List
        * dependant error in CANIO_c on CAN send problems
  */
  void releaseMaster();
 /**
    check if my commanded setpoint s valid:
    a) I am the master
    b) my setpoint is valid for the actual master setpoint
    @return true -> master OR my setpoint meets the actual master setpoint
  */
  bool valid() const;
  /**
    check if the master setpoint is actual valid
    (e.g. the controlling ECU can control as commanded by setpoint)
    @return true -> the remote system controls the process data value as commanded
  */
  bool masterValid() const {return c_answeredMaster.valid();};


  /**
    perform periodic actions
    (here: check if measure val is in limits)
    @return true -> all planned activities performed in allowed time
  */
  virtual bool timeEvent( void );
  /**
    reset entries (used if according remote system gtp isn't registered
    active any more
  */
  void reset();

private: // Private methods
  /** base function for assignment of element vars for copy constructor and operator= */
  void assignFromSource( const SetpointRemote_c& rrefc_src );
  /** process a setpoint request for remote process data */
  virtual void processRequest() const;
  /** process a setpoint set for remote process data */
  virtual void processSet();
  /**
    clear the master flag without checking
    if this action is allowed
  */
  void releaseMasterIntern();

  /**
    deliver a reference to ProcDataRemote_c
    (the base function only delivers ProcDataBase_c)
    @return reference to containing ProcDataRemoteBase_c instance
  */
  ProcDataRemoteBase_c& processData()
  {
    return *((ProcDataRemoteBase_c*)((void*)ProcessElementBase_c::pprocessData()));
  };
  /**
    deliver a reference to ProcDataRemote_c
    (the base function only delivers ProcDataBase_c)
    @return reference to containing ProcDataRemoteBase_c instance
  */
  const ProcDataRemoteBase_c& processDataConst() const
  {
    return *((ProcDataRemoteBase_c*)((void*)ProcessElementBase_c::pprocessData()));
  };
  /**
    deliver a pointer to ProcDataRemote_c
    (the base function only delivers ProcDataBase_c)
    @return pointer to containing ProcDataRemoteBase_c instance
  */
  ProcDataRemoteBase_c* pprocessData()
  {
    return ((ProcDataRemoteBase_c*)((void*)ProcessElementBase_c::pprocessData()));
  };

private: // Private attributes
  /** commanded setpoint for the remote process data */
  SetpointRegister_c c_commanded;
  /** directly adressed answered value */
  SetpointRegister_c c_answeredMe;
  /** answered value -> mostly answer of master setpoint*/
  SetpointRegister_c c_answeredMaster;
  /** time of last sending of setpoint command */
  int32_t i32_commandedTime;
  /** time of last direct answered setpoint information */
  int32_t i32_answeredTime;
};

}
#endif
