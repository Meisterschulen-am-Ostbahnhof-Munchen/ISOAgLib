/***************************************************************************
                          procdataremote_c.cpp - managing of remote
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

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include "procdataremote_c.h"
#include "../../../impl/process_c.h"

// #include <string>
namespace __IsoAgLib {

  /**
      constructor which can set all element vars
      ISO parameter
      @param ps_elementDDI optional pointer to array of structure IsoAgLib::ElementDdi_s which contains DDI, isSetpoint and ValueGroup
                          (array is terminated by ElementDdi_s.ddi == 0xFFFF)
      @param aui16_element device element number

      common parameter
      @param ac_isoName optional ISOName code of this instance
      @param apc_commanderISOName pointer to updated ISOName variable of commander
      @param apc_processDataChangeHandler optional pointer to handler class of application
      @param ai_singletonVecKey optional key for selection of IsoAgLib instance (default 0)
    */
ProcDataRemote_c::ProcDataRemote_c( const IsoAgLib::ElementDdi_s* ps_elementDDI,
                                    uint16_t aui16_element, const IsoName_c& ac_isoName,
                                    const IsoName_c* apc_commanderISOName,
                                    IsoAgLib::ProcessDataChangeHandler_c *apc_processDataChangeHandler,
                                    int ai_singletonVecKey)

  : ProcDataRemoteBase_c( ps_elementDDI, aui16_element,
                          ac_isoName, apc_commanderISOName,
                          apc_processDataChangeHandler, ai_singletonVecKey)
  , mc_setpoint(this)
  , mc_measure(this)
{}

  /**
      initialise this ProcDataRemote_c instance to a well defined initial state
      ISO parameter
      @param ps_elementDDI optional pointer to array of structure IsoAgLib::ElementDdi_s which contains DDI, isSetpoint and ValueGroup
                          (array is terminated by ElementDdi_s.ddi == 0xFFFF)
      @param aui16_element device element number

      common parameter
      @param ac_isoName optional ISOName code of this instance
      @param apc_commanderISOName pointer to updated ISOName variable of commander
      @param apc_processDataChangeHandler optional pointer to handler class of application
      @param ai_singletonVecKey optional key for selection of IsoAgLib instance (default 0)
    */
void ProcDataRemote_c::init(const IsoAgLib::ElementDdi_s* ps_elementDDI, uint16_t aui16_element,
                            const IsoName_c& ac_isoName, const IsoName_c* apc_commanderISOName,
                            IsoAgLib::ProcessDataChangeHandler_c *apc_processDataChangeHandler,
                            int ai_singletonVecKey)
{
  ProcDataRemoteBase_c::init( ps_elementDDI, aui16_element,
                              ac_isoName, apc_commanderISOName,
                              apc_processDataChangeHandler, ai_singletonVecKey);
  mc_setpoint.init( this );
  mc_measure.init( this );
}
/**
  assignment operator for this object
  @param arcc_src source instance
  @return reference to source instance for cmd like "prog1 = prog2 = prog3;"
*/
const ProcDataRemote_c& ProcDataRemote_c::operator=(const ProcDataRemote_c& arcc_src){
  // call the assignment operator for the base class
  ProcDataRemoteBase_c::operator=(arcc_src);

  // now copy the element var
  mc_setpoint = arcc_src.mc_setpoint;
  mc_measure = arcc_src.mc_measure;
  // return reference to source
  return arcc_src;
}

/**
  copy constructor for IsoAgLibProcDataRemote
  @param arcc_src source instance
*/
ProcDataRemote_c::ProcDataRemote_c(const ProcDataRemote_c& arcc_src)
  : ProcDataRemoteBase_c(arcc_src),
    mc_setpoint(arcc_src.mc_setpoint),
    mc_measure(arcc_src.mc_measure)
{
}

/** default destructor which has nothing to do */
ProcDataRemote_c::~ProcDataRemote_c(){
  // now unregister the pointer to this instance in Process_c
  getProcessInstance4Comm().unregisterRemoteProcessData( this );
}
/**
  perform periodic actions
  @param pui16_nextTimePeriod calculated new time period, based on current measure progs (only for local proc data)
  @return true -> all planned executions performed
*/
bool ProcDataRemote_c::timeEvent( uint16_t* /* pui16_nextTimePeriod */)
{
  // check if remote isoName isn't active any more (>3sec) and stop all
  // measureing progs
  // -> checks and reaction performed by SetpointRemote_c::timeEvent and
  // MeasureProgRemote_c::timeEvent
  // set isoName for MeasureProgRemote_c if changed
  prog().setISOName(commanderISOName());
  if ( ! prog().timeEvent() ) return false;

  if ( ! setpoint().timeEvent() ) return false;
  return true;
}

/** process a setpoint message */
void ProcDataRemote_c::processSetpoint(){
  setpoint().processMsg();
}

/** process a measure prog message for remote process data */
void ProcDataRemote_c::processProg(){
  // simply call the process function for the one member object
  prog().processMsg();
}

/**
  deliver actual measurement value as long
  - if no exactValue present => take max or min as fallback
  @param ab_sendRequest true -> request for new value is sent (optional, default false)
*/
int32_t ProcDataRemote_c::masterMeasurementVal(bool ab_sendRequest)
{
  if (hasType(FALSE /* b_isSetpoint */, ProcessCmd_c::exactValue))
    return mc_measure.masterMeasurementVal( ab_sendRequest );
  if (hasType(FALSE /* b_isSetpoint */, ProcessCmd_c::maxValue))
    return mc_measure.max( ab_sendRequest );
  if (hasType(FALSE /* b_isSetpoint */, ProcessCmd_c::minValue))
    return mc_measure.max( ab_sendRequest );

  return 0;

};

} // end of namespace __IsoAgLib
