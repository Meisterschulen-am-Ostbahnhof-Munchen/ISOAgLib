/***************************************************************************
                          procdataremotesimplemeasure_c.h  -
                              managing of remote process data object
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
#ifndef PROCDATA_REMOTE_SIMPLEMEASURE_H
#define PROCDATA_REMOTE_SIMPLEMEASURE_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include "../../impl/procdataremotebase_c.h"
#include "../../StdSetpointElements/impl/setpointremote_c.h"
#include "../../SimpleMeasureElements/impl/simplemanagemeasureprogremote_c.h"

// Begin Namespace IsoAgLib
namespace __IsoAgLib {
// ***************************************************************************************
// ***************** ProcDataRemoteSimpleMeasure_c ******************
// ***************************************************************************************

/**
  @brief Internal Implementation of managing of remote process data object with restricted
  measurement data feature set ( %i.e. without measurement programs
  -> only one shot value requests are supported ).

  Has __IsoAgLib::SetpointRemote_c and __IsoAgLib::SimpleManageMeasureProgRemote_c
  as member for doing setpoint and measurement specific jobs.
  \n
  Projects which use this class, must include the files and succeding subdirectories
  of the following subdirectories of xgpl_src/IsoAgLib/Process:
  - \e Remote/SimpleMeasureElements,
  - \e Remote/StdSetpointElements,
  - \e StdSetpointElements and
  - \e impl
  \n
  Example:
  \code
  // define local device type
  IsoAgLib::ISOName c_myISOName( 1, 0 );
  // creation of process data instance
  iProcDataRemoteSimpleMeasure_c c_workState;
  // init for LIS=0, remote device type/subtype=5/0, complete work width=0xFF,
  // target process data/PRI=2, pointer to my local device type ( to resolve dynamic SA at time of cmd send )
  c_workState.init( 0, 0, IsoAgLib::iIsoName_c( 0x5, 0 ), IsoAgLib::iIsoName_c( 0x5, 0 ),  &c_myISOName );
  // request current measurement value ( real value, which can differ from commanded value ); triger update request
  int lastReceivedMeasureState = c_workState.masterMeasurementVal( true );
  // as this class doesn't support measurement programs, this is all that can be done on measurement data

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
class ProcDataRemoteSimpleMeasure_c : public ProcDataRemoteBase_c  {
private:
public:
  /**
    constructor which can set all element vars
    ISO parameter
    @param ps_elementDDI optional pointer to array of structure IsoAgLib::ElementDdi_s which contains DDI, element, isSetpoint and ValueGroup
                         (array is terminated by ElementDdi_s.ui16_element == 0xFFFF)

    @param ac_isoName optional ISOName code of this instance
    @param ac_ownerISOName optional ISOName of the owner
    @param apc_commanderISOName pointer to updated ISOName variable of commander
    @param apc_processDataChangeHandler optional pointer to handler class of application
    @param ai_singletonVecKey optional key for selection of IsoAgLib instance (default 0)
  */
  ProcDataRemoteSimpleMeasure_c( const IsoAgLib::ElementDdi_s* ps_elementDDI = NULL,
                                 uint16_t aui16_element = 0xFFFF,
                                 const IsoName_c& ac_isoName = IsoName_c::IsoNameInitialProcessData(),
                                 const IsoName_c& ac_ownerISOName = IsoName_c::IsoNameUnspecified(),
                                 const IsoName_c* apc_commanderISOName = NULL,
                                 IsoAgLib::ProcessDataChangeHandler_c *apc_processDataChangeHandler = NULL,
                                 int ai_singletonVecKey = 0);

  /**
    initialise this ProcDataRemoteSimpleMeasure_c instance to a well defined initial state
    ISO parameter
    @param ps_elementDDI optional pointer to array of structure IsoAgLib::ElementDdi_s which contains DDI, element, isSetpoint and ValueGroup
                         (array is terminated by ElementDdi_s.ui16_element == 0xFFFF)

    @param ac_isoName optional ISOName code of this instance
    @param ac_ownerISOName optional ISOName of the owner
    @param apc_commanderISOName pointer to updated ISOName variable of commander
    @param apc_processDataChangeHandler optional pointer to handler class of application
    @param ai_singletonVecKey optional key for selection of IsoAgLib instance (default 0)
  */
  void init(const IsoAgLib::ElementDdi_s* ps_elementDDI, uint16_t aui16_element,
            const IsoName_c& ac_isoName = IsoName_c::IsoNameInitialProcessData(),
            const IsoName_c& ac_ownerISOName = IsoName_c::IsoNameUnspecified(),
            const IsoName_c* apc_commanderISOName = NULL,
            IsoAgLib::ProcessDataChangeHandler_c *apc_processDataChangeHandler = NULL,
            int ai_singletonVecKey = 0);

   /**
    assignment operator for this object
    @param arc_src source instance
    @return reference to source instance for cmd like "prog1 = prog2 = prog3;"
  */
  const ProcDataRemoteSimpleMeasure_c& operator=(const ProcDataRemoteSimpleMeasure_c& arc_src);

  /**
    copy constructor for IsoAgLibProcDataRemote
    @param arc_src source instance
  */
   ProcDataRemoteSimpleMeasure_c(const ProcDataRemoteSimpleMeasure_c& arc_src);

  /** default destructor which has nothing to do */
  ~ProcDataRemoteSimpleMeasure_c();

  /**
    deliver a reference to the setpoint object
    @return reference to member object with setpoint informations
  */
  SetpointRemote_c& setpoint() { return mc_setpoint;};

  /**
    deliver a reference to the measure prog object
    @return reference to member object with measure prog informations
  */
  SimpleManageMeasureProgRemote_c& prog() { return mc_measure;};


  /**
    deliver the actual master setpoint
    @param ab_sendRequest true -> send request for actual value
    @return setpoint value as long
  */
  int32_t setpointMasterVal(bool ab_sendRequest = false)
    { return mc_setpoint.setpointMasterVal( ab_sendRequest ); };
  /**
    send a setpoint cmd with given exact setpoint
    @param ai32_val commanded setpoint value as long
  */
  void setSetpointMasterVal(int32_t ai32_val){ mc_setpoint.setSetpointMasterVal(ai32_val);};
  #ifdef USE_FLOAT_DATA_TYPE
  /**
    deliver the actual master setpoint
    @param ab_sendRequest true -> send request for actual value
    @return setpoint value as float
  */
  float setpointMasterValFloat(bool ab_sendRequest = false)
    { return mc_setpoint.setpointMasterValFloat( ab_sendRequest ); };
  /**
    send a setpoint cmd with given exact setpoint
    @param af_val commanded setpoint value as float
  */
  void setSetpointMasterVal(float af_val){ mc_setpoint.setSetpointMasterVal(af_val);};
  #endif
  /**
    deliver actual measurement value as long
    @param ab_sendRequest true -> request for new value is sent (optional, default false)
  */
  int32_t masterMeasurementVal(bool ab_sendRequest = false)
    { return mc_measure.masterMeasurementVal( ab_sendRequest );};
  /**
    send reset cmd for the measurement value
  */
  void resetMasterVal() { mc_measure.resetMasterVal();};
  #ifdef USE_FLOAT_DATA_TYPE
  /**
    deliver actual measurement value as float
    @param ab_sendRequest true -> request for new value is sent (optional, default false)
  */
  float masterValFloat(bool ab_sendRequest = false)
    { return mc_measure.masterValFloat( ab_sendRequest );};
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
  SetpointRemote_c mc_setpoint;

  /** object for managing jobs of remote measure programs */
  SimpleManageMeasureProgRemote_c mc_measure;
};

}
#endif
