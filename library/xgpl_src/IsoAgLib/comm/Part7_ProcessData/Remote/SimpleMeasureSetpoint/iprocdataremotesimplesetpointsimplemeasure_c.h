/***************************************************************************
                          iprocdataremotesimplesetpointsimplemeasure_c.h  -
                             managing of remote process data object
                             -------------------
    begin                : Fri Apr 07 2000
    copyright            : (C) 2000 - 2009 by Dipl.-Inform. Achim Spangler
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
 * Copyright (C) 2000 - 2009 Dipl.-Inform. Achim Spangler                  *
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
#ifndef IPROC_DATA_REMOTE_SIMPLESETPOINT_SIMPLEMEASURE_H
#define IPROC_DATA_REMOTE_SIMPLESETPOINT_SIMPLEMEASURE_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include "impl/procdataremotesimplesetpointsimplemeasure_c.h"
#include <IsoAgLib/comm/Part7_ProcessData/processdatachangehandler_c.h>

// Begin Namespace IsoAgLib
namespace IsoAgLib {

/**
  @brief Interface for managing of remote process data object with restricted measurement
  and setpoint management ( %i.e. only one-shot measurement requests;
  set setpoint, without handling of acceptance, several concurring setpoint commanding ECUs ).

  Has __IsoAgLib::SimpleManageMeasureProgRemote_c / IsoAgLib::iSimpleManageMeasureProgRemote_c and
  __IsoAgLib::SimpleManageSetpointRemote_c / IsoAgLib::iSimpleManageSetpointRemote_c
  as member for doing setpoint and measurement specific jobs.
  \n
  Projects which use this class, must include the files and succeding subdirectories
  of the following subdirectories of xgpl_src/IsoAgLib/Process:
  - \e Remote/StdMeasureElements,
  - \e Remote/SimpleSetpointElements and
  - \e impl
  \n
  Example:
  \code
  // define local device type
  IsoAgLib::ISOName c_myISOName( 1, 0 );
  // creation of process data instance
  iProcDataRemoteSimpleSetpoint_c c_workState;
  // init for LIS=0, remote device type/subtype=5/0, complete work width=0xFF,
  // target process data/PRI=2, pointer to my local device type ( to resolve dynamic SA at time of cmd send )
  c_workState.init( 0, 0, IsoAgLib::iIsoName_c( 0x5, 0 ), IsoAgLib::iIsoName_c( 0x5, 0 ), &c_myISOName );

  // request current measurement value ( real value, which can differ from commanded value ); triger update request
  int lastReceivedMeasureState = c_workState.masterMeasurementVal( true );
  // the restricted measurement feature set doesn't provide measurement programs

  // request current setpoint value ( parameter true -> send request to remote ECU, false -> just deliver last received value )
  int lastReceivedState = c_workState.setpointMasterVal( false );
  // set a setpoint for remote data ( SA / nr of sending and receiving node is derived at time of send
  // -> process data can be initialized even if both ECU are not yet claimed address on BUS )
  c_workState.setSetpointMasterVal( 100 );
  // in spite to the standard setpoint management, this class doesn't provide
  // check for master setpoint, acceptance, etc.
  \endcode

  @author Dipl.-Inform. Achim Spangler
*/
class iProcDataRemoteSimpleSetpointSimpleMeasure_c : private __IsoAgLib::ProcDataRemoteSimpleSetpointSimpleMeasure_c {
private:
  friend class EventSource_c;

  // iProcess_c::addDDI2ExistingProcData() does a static_cast __IsoAgLib::ProcDataRemote_c => iProcDataRemote_c
  // => access is needed
  friend class iProcess_c;

public:

  /**
    constructor which can set all element vars
    ISO parameter
    @param ps_elementDDI optional pointer to array of structure IsoAgLib::ElementDdi_s which contains DDI, element, isSetpoint and ValueGroup
                         (array is terminated by ElementDdi_s.ui16_element == 0xFFFF)
    @param aui16_element device element number

    @param acrc_isoName optional ISOName code of this instance
    @param apc_commanderISOName pointer to updated ISOName variable of commander
    @param apc_processDataChangeHandler optional pointer to handler class of application
    @param ai_multitonInst optional key for selection of IsoAgLib instance (default 0)
  */
  iProcDataRemoteSimpleSetpointSimpleMeasure_c( const IsoAgLib::ElementDdi_s* ps_elementDDI = NULL,
                                                uint16_t aui16_element = 0xFFFF,
                                                const iIsoName_c& acrc_isoName = iIsoName_c::iIsoNameInitialProcessData(),
                                                const iIsoName_c* apc_commanderISOName = NULL,
                                                ProcessDataChangeHandler_c *apc_processDataChangeHandler = NULL,
                                                int ai_multitonInst = 0)
  : ProcDataRemoteSimpleSetpointSimpleMeasure_c( ps_elementDDI, aui16_element,
                                                 acrc_isoName, apc_commanderISOName,
                                                 apc_processDataChangeHandler, ai_multitonInst){}

  /**
    initialise this ProcDataRemoteSimpleSetpointSimpleMeasure_c instance to a well defined initial state
    ISO parameter
    @param ps_elementDDI optional pointer to array of structure IsoAgLib::ElementDdi_s which contains DDI, element, isSetpoint and ValueGroup
                         (array is terminated by ElementDdi_s.ui16_element == 0xFFFF)
    @param aui16_element device element number

    @param acrc_isoName optional ISOName code of this instance
    @param apc_commanderISOName pointer to updated ISOName variable of commander
    @param apc_processDataChangeHandler optional pointer to handler class of application
    @param ai_multitonInst optional key for selection of IsoAgLib instance (default 0)
  */
  void init(const IsoAgLib::ElementDdi_s* ps_elementDDI,
            uint16_t aui16_element,
            const iIsoName_c& acrc_isoName = iIsoName_c::iIsoNameInitialProcessData(),
            const iIsoName_c* apc_commanderISOName = NULL,
            ProcessDataChangeHandler_c *apc_processDataChangeHandler = NULL,
            int ai_multitonInst = 0)
  {ProcDataRemoteSimpleSetpointSimpleMeasure_c::init( ps_elementDDI, aui16_element,
                                                      acrc_isoName, apc_commanderISOName,
                                                      apc_processDataChangeHandler, ai_multitonInst);
  }

  /** set the poitner to the handler class
    * @param apc_processDataChangeHandler pointer to handler class of application
    */
  void setProcessDataChangeHandler( ProcessDataChangeHandler_c *apc_processDataChangeHandler )
   { ProcDataRemoteSimpleSetpointSimpleMeasure_c::setProcessDataChangeHandler( apc_processDataChangeHandler ); }

  /** deliver the poitner to the handler class
    * @return pointer to handler class of application (or NULL if not defined by application)
    */
  IsoAgLib::ProcessDataChangeHandler_c* getProcessDataChangeHandler( void ) const
   { return ProcDataRemoteSimpleSetpointSimpleMeasure_c::getProcessDataChangeHandler(); }

  /**
    deliver value DEVCLASS (machine type specific table of process data types)
    @return DEVCLASS
  */
  uint8_t devClass() const{return ProcDataRemoteSimpleSetpointSimpleMeasure_c::devClass();}

  /**
    deliver value _instance_ (important if more than one machine with equal _device_class_ are active)
    @return POS
  */
  uint8_t devClassInst() const{return ProcDataRemoteSimpleSetpointSimpleMeasure_c::devClassInst();}

  /** check if this ProcIdent_c has the given DDI as element */
  bool hasDDI( uint16_t aui16_checkDDI ) const { return ProcIdent_c::hasDDI( aui16_checkDDI );}

  /**
    deliver value DDI (only possible if only one elementDDI in list)
    @return DDI
   */
  uint16_t DDI() const { return ProcIdent_c::DDI();}

  /**
  deliver value element (only possible if only one elementDDI in list)
  @return element
   */
  uint16_t element() const  { return ProcIdent_c::element();}

  /**
    deliver DDI from last received can pkg
    @return DDI
  */
  uint16_t getDDIfromCANPkg() const { return ProcDataBase_c::getDDIfromCANPkg();}


  /* ******************************************** *
   * some convenience functions for master        *
   * setpoint access with NOT simple setpoint      *
   * ******************************************** */
  /**
    deliver the actual master setpoint
    @param ab_sendRequest true -> send request for actual value
    @return setpoint value as long
  */
  int32_t setpointMasterVal(bool ab_sendRequest = false)
    {return ProcDataRemoteSimpleSetpointSimpleMeasure_c::setpointMasterVal(ab_sendRequest);}

  /**
    send a setpoint cmd with given exact setpoint
    @param ai32_val commanded setpoint value as long
  */
  void setSetpointMasterVal(int32_t ai32_val)
    {ProcDataRemoteSimpleSetpointSimpleMeasure_c::setSetpointMasterVal(ai32_val);}

  /**
    deliver actual measurement value as long
    @param ab_sendRequest true -> request for new value is sent (optional, default false)
  */
  int32_t masterMeasurementVal(bool ab_sendRequest = false)
    {return ProcDataRemoteSimpleSetpointSimpleMeasure_c::masterMeasurementVal(ab_sendRequest);}

  /**
    send reset cmd for the measurement value
  */
  void resetMasterVal(){ProcDataRemoteSimpleSetpointSimpleMeasure_c::resetMasterVal();}

  /**
    deliver the commanderISOName (ISOName of local member)
    @return ISOName used for sending commands to remote owner member
  */
  const iIsoName_c& commanderISOName()const{return ProcDataRemoteSimpleSetpointSimpleMeasure_c::commanderISOName().toConstIisoName_c();}

  /**
    set the pointer to the commander ident isoName
    @param apc_externalOverridingIsoName pointer to ISOName var of local member used for
                sending commands to remote owner member
  */
  void setCommanderISOName(const __IsoAgLib::IsoName_c* apc_externalOverridingIsoName){ProcDataRemoteSimpleSetpointSimpleMeasure_c::setCommanderISOName(apc_externalOverridingIsoName);}
};

}
#endif
