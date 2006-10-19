/***************************************************************************
                          procdataremote_c.h  - managing of remote
                                                       process data object
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
#ifndef PROCDATA_REMOTE_H
#define PROCDATA_REMOTE_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include "../../impl/procdataremotebase_c.h"
#include "../../StdSetpointElements/impl/setpointremote_c.h"
#include "../../StdMeasureElements/impl/measureprogremote_c.h"

// forward declarations for friend declarations below...
// needed for iProcess_c::addDDI2ExistingProcData()
namespace IsoAgLib
{
  class iProcDataRemote_c;
  class iProcDataRemoteSimpleMeasure_c;
  class iProcDataRemoteSimpleSetpoint_c;
  class iProcDataRemoteSimpleMeasureSetpoint_c;
  class iProcDataRemoteSimpleMeasureSetpointCombined_c;
}

// Begin Namespace IsoAgLib
namespace __IsoAgLib {
/**
  @brief Internal implementation for managing of remote process data object with standard
  ( %i.e. no restrictions ) feature set.

  Has __IsoAgLib::SetpointRemote_c / IsoAgLib::iSetpointRemote_c and __IsoAgLib::MeasureProgRemote_c / IsoAgLib::iMeasureProgRemote_c
  as member for doing setpoint and measurement specific jobs.
  \n
  Projects which use this class, must include the files and succeding subdirectories
  of the following subdirectories of xgpl_src/IsoAgLib/Process:
  - \e Remote/StdMeasureElements,
  - \e Remote/StdSetpointElements,
  - \e StdMeasureElements,
  - \e StdSetpointElements and
  - \e impl
  \n
  Example:
  \code
  // define local device type
  IsoAgLib::ISOName c_myISOName( 1, 0 );
  // creation of process data instance
  iProcDataRemote_c c_workState;
  // init for LIS=0, remote device type/subtype=5/0, complete work width=0xFF,
  // target process data/PRI=2, pointer to my local device type ( to resolve dynamic SA at time of cmd send )
  c_workState.init( 0, IsoAgLib::ISOName_c( 0x5, 0 ), 0x5, 0x0, 0xFF, 2, IsoAgLib::ISOName_c( 0x5, 0 ), &c_myISOName );

  // request current measurement value ( real value, which can differ from commanded value ); triger update request
  int lastReceivedMeasureState = c_workState.masterMeasurementVal( true );
  // start a measuring program
  // a) trigger value update every 1000 msec.
  c_workState.prog().addSubprog(Proc_c::TimeProp, 1000);
  // b) start measure program: trigger send of current measure value ( and not MIN/MAX/AVG/ etc. )
  c_workState.prog().start(Proc_c::Target, Proc_c::TimeProp, Proc_c::DoVal);

  // request current setpoint value ( parameter true -> send request to remote ECU, false -> just deliver last received value )
  int lastReceivedState = c_workState.setpointMasterVal( false );
  // set a setpoint for remote data ( SA / nr of sending and receiving node is derived at time of send
  // -> process data can be initialized even if both ECU are not yet claimed address on BUS )
  c_workState.setSetpointMasterVal( 100 );
  // check if last setpoint set is already answered
  if ( c_workState.setpoint().answered() )
  { // check if it was accepted or rejected
    if ( c_workState.setpoint().accepted() )
    { // happy!
    }
    // otherwise check which other ECU is currently the master, and retrieve the master setpoint
    else if ( c_workState.setpoint().existMaster() )
    { // now retrieve current master setpoint
      int masterSetpoint = c_workState.setpoint().master().exact();
    }
  }
  // check if my current setpoint wish is valid in regard to the current real measurement value
  if ( c_workState.setpoint().valid() )
  { // happy! - even if we're not master, our wished value is delivered at the moment
  }
  // independent of the current master setpoint ( local or other remote ECU )
  // - check if current master setpoint is exact mapped by real measurement data
  if ( c_workState.setpoint().masterValid() )
  { // the master setpoint value is currently mapped
  }
  // if I'm the master setpoint commander, I should release the setpoint explicitly if the
  // controlo is not needed any more
  if ( c_workState.setpoint().master().isoName() == c_myISOName )
  { // I'm the master -> release control
    c_workState.setpoint().releaseMaster();
  }
  \endcode

  @author Dipl.-Inform. Achim Spangler
*/
class ProcDataRemote_c : public ProcDataRemoteBase_c  {

private:
public:
  /**
      constructor which can set all element vars
      ISO parameter
      @param ps_elementDDI optional pointer to array of structure IsoAgLib::ElementDDI_s which contains DDI, isSetpoint and ValueGroup
                          (array is terminated by ElementDDI_s.ddi == 0xFFFF)
      @param rui16_element device element number

      common parameter
      @param rc_isoName optional ISOName code of this instance
      @param rc_ownerISOName optional ISOName of the owner
      @param rpc_commanderISOName pointer to updated ISOName variable of commander
      @param rpc_processDataChangeHandler optional pointer to handler class of application
      @param ri_singletonVecKey optional key for selection of IsoAgLib instance (default 0)
    */
  ProcDataRemote_c( const IsoAgLib::ElementDDI_s* ps_elementDDI = NULL, uint16_t rui16_element = 0xFFFF,
                    const ISOName_c& rc_isoName = ISOName_c::ISONameInitialProcessData,
                    const ISOName_c& rc_ownerISOName = ISOName_c::ISONameUnspecified,
                    const ISOName_c* rpc_commanderISOName = NULL,
                    IsoAgLib::ProcessDataChangeHandler_c *rpc_processDataChangeHandler = NULL,
                    int ri_singletonVecKey = 0);

  /**
    initialise this ProcDataRemote_c instance to a well defined initial state
    ISO parameter
    @param ps_elementDDI optional pointer to array of structure IsoAgLib::ElementDDI_s which contains DDI, element, isSetpoint and ValueGroup
                         (array is terminated by ElementDDI_s.ui16_element == 0xFFFF)
    @param rui16_element device element number

    common parameter
    @param rc_isoName optional ISOName code of this instance
    @param rc_ownerISOName optional ISOName of the owner
    @param rpc_commanderISOName pointer to updated ISOName variable of commander
    @param rpc_processDataChangeHandler optional pointer to handler class of application
    @param ri_singletonVecKey optional key for selection of IsoAgLib instance (default 0)
  */
  void init( const IsoAgLib::ElementDDI_s* ps_elementDDI, uint16_t rui16_element,
             const ISOName_c& rc_isoName = ISOName_c::ISONameInitialProcessData,
             const ISOName_c& rc_ownerISOName = ISOName_c::ISONameUnspecified,
             const ISOName_c* rpc_commanderISOName = NULL,
             IsoAgLib::ProcessDataChangeHandler_c *rpc_processDataChangeHandler = NULL,
             int ri_singletonVecKey = 0);

  /**
      assignment operator for this object
      @param rrefc_src source instance
      @return reference to source instance for cmd like "prog1 = prog2 = prog3;"
    */
  const ProcDataRemote_c& operator=(const ProcDataRemote_c& rrefc_src);

  /**
      copy constructor for IsoAgLibProcDataRemote
      @param rrefc_src source instance
    */
   ProcDataRemote_c(const ProcDataRemote_c& rrefc_src);

  /** default destructor which has nothing to do */
  ~ProcDataRemote_c();

  /**
    deliver a reference to the setpoint object
    @return reference to member object with setpoint informations
  */
  SetpointRemote_c& setpoint() { return c_setpoint;};

  /**
    deliver a reference to the measure prog object
    @return reference to member object with measure prog informations
  */
  MeasureProgRemote_c& prog() { return c_measure;};

  /**
    deliver the actual master setpoint
    @param rb_sendRequest true -> send request for actual value
    @return setpoint value as long
  */
  int32_t setpointMasterVal(bool rb_sendRequest = false) const
    { return c_setpoint.setpointMasterVal( rb_sendRequest ); };
  /**
    send a setpoint cmd with given exact setpoint
    @param ri32_val commanded setpoint value as long
  */
  void setSetpointMasterVal(int32_t ri32_val){ c_setpoint.setSetpointMasterVal(ri32_val);};
  #ifdef USE_FLOAT_DATA_TYPE
  /**
    deliver the actual master setpoint
    @param rb_sendRequest true -> send request for actual value
    @return setpoint value as float
  */
  float setpointMasterValFloat(bool rb_sendRequest = false)
    { return c_setpoint.setpointMasterValFloat( rb_sendRequest ); };
  /**
    send a setpoint cmd with given exact setpoint
    @param rf_val commanded setpoint value as float
  */
  void setSetpointMasterVal(float rf_val){ c_setpoint.setSetpointMasterVal(rf_val);};
  #endif
  /**
    deliver actual measurement value as long
    - if no exactValue present => take max or min as fallback
    @param rb_sendRequest true -> request for new value is sent (optional, default false)
  */
  int32_t masterMeasurementVal(bool rb_sendRequest = false);
  /**
    send reset cmd for the measurement value
  */
  void resetMasterVal() { c_measure.resetMasterVal();};
  #ifdef USE_FLOAT_DATA_TYPE
  /**
    deliver actual measurement value as float
    @param rb_sendRequest true -> request for new value is sent (optional, default false)
  */
  float masterValFloat(bool rb_sendRequest = false)
    { return c_measure.masterValFloat( rb_sendRequest );};
  #endif


  /**
    perform periodic actions
    @param pui16_nextTimePeriod calculated new time period, based on current measure progs (only for local proc data)
    @return true -> all planned executions performed
  */
  virtual bool timeEvent( uint16_t *pui16_nextTimePeriod = NULL );

private: // Private methods
  /** process a measure prog message for remote process data */
  virtual void processProg();
  /** processing of a setpoint message */
  virtual void processSetpoint();

private: // Private attributes
  /** object for managing jobs of remote setpoints */
  SetpointRemote_c c_setpoint;

  /** object for managing jobs of remote measure programs */
  MeasureProgRemote_c c_measure;

  // iProcess_c::addDDI2ExistingProcData() does a static_cast __IsoAgLib::ProcDataRemote_c => iProcDataRemote_c
  // => access is needed
  friend class IsoAgLib::iProcDataRemote_c;
  friend class IsoAgLib::iProcDataRemoteSimpleMeasure_c;
  friend class IsoAgLib::iProcDataRemoteSimpleSetpoint_c;
  friend class IsoAgLib::iProcDataRemoteSimpleMeasureSetpoint_c;
  friend class IsoAgLib::iProcDataRemoteSimpleMeasureSetpointCombined_c;

};

}
#endif
