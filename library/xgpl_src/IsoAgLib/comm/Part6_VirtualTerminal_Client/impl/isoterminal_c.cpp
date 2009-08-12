/***************************************************************************
                          isoterminal_c.cpp - central ISO terminal management
                             -------------------
    begin                : Wed Jul 09 2003
    copyright            : (C) 2003 by Dipl.-Inf. Martin Wodok
    email                : m.wodok@osb-ag:de
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

#include "isoterminal_c.h"

#include <IsoAgLib/scheduler/impl/scheduler_c.h>
#include <IsoAgLib/driver/can/impl/canio_c.h>
#include <IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/vtclientservercommunication_c.h>
// #include "vttypes.h"

#ifdef DEBUG
  #ifdef SYSTEM_PC
    #include <iostream>
  #else
    #include <supplementary_driver/driver/rs232/impl/rs232io_c.h>
  #endif
#endif

namespace __IsoAgLib {
#if defined(PRT_INSTANCE_CNT) && (PRT_INSTANCE_CNT > 1)
  /** C-style function, to get access to the unique IsoTerminal_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  IsoTerminal_c& getIsoTerminalInstance(uint8_t aui8_instance)
  { // if > 1 singleton instance is used, no static reference can be used
    return IsoTerminal_c::instance(aui8_instance);
  };
#else
  /** C-style function, to get access to the unique IsoTerminal_c singleton instance */
  IsoTerminal_c& getIsoTerminalInstance(void)
  {
    static IsoTerminal_c& c_isoTerminal = IsoTerminal_c::instance();
    return c_isoTerminal;
  };
#endif



/** deliver reference to data pkg as reference to CanPkgExt_c
  to implement the base virtual function correct
 */
CanPkgExt_c& IsoTerminal_c::dataBase()
{
  return mc_data;
}


/** default constructor
 */
IsoTerminal_c::IsoTerminal_c()
{
  /// all variable initialization moved to singletonInit!
}


/** default destructor
*/
IsoTerminal_c::~IsoTerminal_c()
{
  close();
}


void
IsoTerminal_c::singletonInit()
{
  // clear state of b_alreadyClosed, so that close() is called one time
  setAlreadyClosed();

  // call real init() code...
  init();
}


/** initialise element which can't be done during construct */
void
IsoTerminal_c::init()
{
  if (checkAlreadyClosed())
  { // avoid another call
    clearAlreadyClosed();

    // register in Scheduler_c to get time-events
    getSchedulerInstance4Comm().registerClient(this);
    // register to get ISO monitor list changes
    getIsoMonitorInstance4Comm().registerSaClaimHandler(this);

    // register Filter in CanIo_c
    bool b_atLeastOneFilterAdded=false;
    b_atLeastOneFilterAdded |= (getCanInstance4Comm().insertStandardIsoFilter(*this,(VT_TO_GLOBAL_PGN),false)!= NULL);
    b_atLeastOneFilterAdded |= (getCanInstance4Comm().insertStandardIsoFilter(*this,(LANGUAGE_PGN),false)!= NULL);

    if (b_atLeastOneFilterAdded) getCanInstance4Comm().reconfigureMsgObj();
  }
}


/** every subsystem of IsoAgLib has explicit function for controlled shutdown */
void
IsoTerminal_c::close()
{
  if (!checkAlreadyClosed())
  { // avoid another call
    setAlreadyClosed();

    // deregister in Scheduler_c
    getSchedulerInstance4Comm().unregisterClient(this);
    // deregister in IsoMonitor_c
    getIsoMonitorInstance4Comm().deregisterSaClaimHandler(this);

    getCanInstance4Comm().deleteFilter(*this, (0x3FFFF00UL), (static_cast<MASK_TYPE>(VT_TO_GLOBAL_PGN) << 8),    Ident_c::ExtendedIdent);
    getCanInstance4Comm().deleteFilter(*this, (0x3FFFF00UL), (static_cast<MASK_TYPE>(LANGUAGE_PGN) << 8),        Ident_c::ExtendedIdent);

    for (uint8_t ui8_index = 0; ui8_index < mvec_vtClientServerComm.size(); ui8_index++)
    {
      if (mvec_vtClientServerComm[ui8_index])
      {
        deregisterIsoObjectPoolInd (ui8_index);
      }
    }
  }
}


/** Register the given object pool
  It will automatically be uploaded as soon as ISO_Terminal_c is connected to the VT
  and all initialization stuff has been done (Get VT Capabilities, Memory, etc.)
  @return NULL if new vtClientServerCommunication_c instance could be created.
          This could be the case because you have passed a versionLabel
          - longer than 7 characters for a non-multilanguage object-pool
          - longer than 5 characters for a multilanguage object-pool
          or if you already registered an object-pool for this IdentItem
 */
VtClientServerCommunication_c*
IsoTerminal_c::initAndRegisterIsoObjectPool (IdentItem_c& rc_identItem, IsoAgLib::iIsoTerminalObjectPool_c& arc_pool, char* apc_versionLabel)
{
  if (!rc_identItem.isMaster())
  {
    /// IdentItem must be a Master
    #if defined (DEBUG)
    INTERNAL_DEBUG_DEVICE << "IdentItem is not Master!" << INTERNAL_DEBUG_DEVICE_ENDL;
    #if defined (SYSTEM_PC)
    MACRO_ISOAGLIB_ABORT();
    #endif
    #endif
    return NULL;
  }
  uint8_t ui8_index = 0;
  // add new instance of VtClientServerCommunication
  for (; ui8_index < mvec_vtClientServerComm.size(); ui8_index++)
  {
    if (mvec_vtClientServerComm[ui8_index] == NULL)
    { // found one emtpy entry
      break;
    }
    else
    {
      if (mvec_vtClientServerComm[ui8_index]->getIdentItem() == rc_identItem)
      { // this IdentItem has already one pool registered - use multiple
        // IdentItems if you want to use multiple pools!
        return NULL;
      }
    }
  }
  // create new instance
  VtClientServerCommunication_c* pc_vtCSC = new VtClientServerCommunication_c (rc_identItem, *this, arc_pool, apc_versionLabel, ui8_index SINGLETON_VEC_KEY_WITH_COMMA);
  if (pc_vtCSC->men_objectPoolState == VtClientServerCommunication_c::OPCannotBeUploaded) // meaning here is: OPCannotBeInitialized (due to versionLabel problems)
  { // most likely due to wrong version label
    /// Error already registered in the VtClientServerCommunication_c(..) constructor!
    #if defined (DEBUG)
    INTERNAL_DEBUG_DEVICE << "Wrong version label (too long!)" << INTERNAL_DEBUG_DEVICE_ENDL;
    #if defined (SYSTEM_PC)
    MACRO_ISOAGLIB_ABORT();
    #endif
    #endif
    delete pc_vtCSC;
    return NULL;
  }

  // add new instance to vector
  if (ui8_index < mvec_vtClientServerComm.size())
    mvec_vtClientServerComm[ui8_index] = pc_vtCSC;
  else
    mvec_vtClientServerComm.push_back(pc_vtCSC);

  // if at least one VtServerInstance is online, notify the new VtClientServerCommunication
  if (!ml_vtServerInst.empty()) pc_vtCSC->notifyOnNewVtServerInstance (ml_vtServerInst.front());
  return pc_vtCSC;
}


/** De-Register the registered object pool and versionLabel string (if one was copied) */
bool
IsoTerminal_c::deregisterIsoObjectPool (IdentItem_c& r_identItem)
{
  /* what states the IdentItem could have we have to interrupt???
  * - IState_c::ClaimedAddress -> that item is Active and Member on ISOBUS
  * - !UploadType::UploadIdle -> interrupt any upload
  */
  uint8_t ui8_index = 0;
  for (; ui8_index < mvec_vtClientServerComm.size(); ui8_index++)
  {
    if (mvec_vtClientServerComm[ui8_index])
    {
      if (&r_identItem == &mvec_vtClientServerComm[ui8_index]->getIdentItem())
      {
        deregisterIsoObjectPoolInd (ui8_index);
        break;
      }
    }
  }

  if (ui8_index == mvec_vtClientServerComm.size())
    return false; // appropriate IdentItem could not be found, so nothing was deleted
  else
    return true; // IdentItem was found and deleted
}


void
IsoTerminal_c::deregisterIsoObjectPoolInd (uint8_t aui8_index)
{
  delete mvec_vtClientServerComm[aui8_index];
  mvec_vtClientServerComm[aui8_index] = NULL;
}




/** periodically event
  @return true -> all planned activities from all vtClientServerCommuniactions were performed in allowed time
 */
bool
IsoTerminal_c::timeEvent(void)
{
  bool b_allActivitiesPerformed = true;

  for (uint8_t ui8_index = 0; ui8_index < mvec_vtClientServerComm.size(); ui8_index++)
  {
    if (mvec_vtClientServerComm[ui8_index])
      b_allActivitiesPerformed &= mvec_vtClientServerComm[ui8_index]->timeEvent();
  }
  /** @todo SOON-241: maybe store the one that was out of time if not all could perform their actions? - by member variable of IsoTerminal_c
                      Update: They all need to be scheduled on their own, so then it's okay with this point. */

  // set back the scheduler period to 100msec, as any waiting command has been set
  if (getTimePeriod() != 100) setTimePeriod( 100 );

  return b_allActivitiesPerformed;
}


/** process received can messages
  @return true -> message was processed; else the received CAN message will be served to other matching CanCustomer_c
 */
bool
IsoTerminal_c::processMsg()
{
  // VT_TO_GLOBAL is the only PGN we accept without VT being active, because it marks the VT active!!
  STL_NAMESPACE::list<VtServerInstance_c>::iterator lit_vtServerInst;
  uint8_t ui8_index;

  /// -->VT_TO_GLOBAL_PGN<-- ///
  if ((data().isoPgn() & 0x3FFFF) == VT_TO_GLOBAL_PGN)
  { // iterate through all registered VtServerInstances and process msg if vtSourceAddress == isoSa
    uint8_t const cui8_cmdByte = data().getUint8Data (1-1);
    if (cui8_cmdByte == 0xFE) // Command: "Status", Parameter: "VT Status Message"
    {
      for (lit_vtServerInst = ml_vtServerInst.begin(); lit_vtServerInst != ml_vtServerInst.end(); lit_vtServerInst++)
      {
        if (lit_vtServerInst->getVtSourceAddress() == data().isoSa()) // getVtSourceAddress gets the SA from the IsoItem, so it's the current one...
        {
          lit_vtServerInst->setLatestVtStatusData();

          // iterate through all registered VtClientServerCommunication and notify their pools with "eventVtStatusMsg"
          for (ui8_index = 0; ui8_index < mvec_vtClientServerComm.size(); ui8_index++)
          {
            if (mvec_vtClientServerComm[ui8_index])
            {
              if (mvec_vtClientServerComm[ui8_index]->getVtServerInstPtr() == &(*lit_vtServerInst))
              { // this vtClientServerComm is connected to this VT, so notify the objectpool!!
                mvec_vtClientServerComm[ui8_index]->notifyOnVtStatusMessage();
              }
            }
          }
          return true; // VT Status Message is NOT of interest for anyone else!
        }
      }
    }
    else if (cui8_cmdByte == 0x21) // Command: "Auxiliary Control", Parameter: "Auxiliary Input Status"
    { // iterate through all registered VtClientServerCommunication and notify them, maybe they have functions that need that input status!
      for (ui8_index = 0; ui8_index < mvec_vtClientServerComm.size(); ui8_index++)
      {
        if (mvec_vtClientServerComm[ui8_index])
        {
          mvec_vtClientServerComm[ui8_index]->notifyOnAuxInputStatus();
        }
      }
    }
    return true; // CHANGED! - VT_TO_GLOBAL is NOT of interest for anyone else anymore as we're handlign Aux Assignment here now!
  }


  /// -->LANGUAGE_PGN<-- ///
  if ((data().isoPgn() & 0x3FFFF) == LANGUAGE_PGN)
  {
      // first process LANGUAGE_PGN for all VtServerInstances BEFORE processing for the VtClientServerCommunications
    for (lit_vtServerInst = ml_vtServerInst.begin(); lit_vtServerInst != ml_vtServerInst.end(); lit_vtServerInst++)
    {
      if (lit_vtServerInst->getVtSourceAddress() == data().isoSa())
        lit_vtServerInst->setLocalSettings();
    }

    for (ui8_index = 0; ui8_index < mvec_vtClientServerComm.size(); ui8_index++)
    {
      if (mvec_vtClientServerComm[ui8_index])
        mvec_vtClientServerComm[ui8_index]->notifyOnVtsLanguagePgn(); /// @todo OPTIMIZATION: Only notify those vtCSCs which are connected to this VT!
    }
    return false;
  }

  return false; /** shouldn't reach here as all filters are handled and returned above */
}


bool
IsoTerminal_c::sendCommandForDEBUG(IsoAgLib::iIdentItem_c& mrc_wsMasterIdentItem, uint8_t* apui8_buffer, uint32_t ui32_size)
{
  for (uint8_t ui8_index = 0; ui8_index < mvec_vtClientServerComm.size(); ui8_index++)
  {
    if (static_cast<__IsoAgLib::IdentItem_c&>(mrc_wsMasterIdentItem) == mvec_vtClientServerComm[ui8_index]->getIdentItem())
      return mvec_vtClientServerComm[ui8_index]->sendCommand(apui8_buffer, ui32_size);
  }
  return false;
}



/** this function is called by IsoMonitor_c on addition, state-change and removal of an IsoItem.
 * @param at_action enumeration indicating what happened to this IsoItem. @see IsoItemModification_en / IsoItemModification_t
 * @param acrc_isoItem reference to the (const) IsoItem which is changed (by existance or state)
 */
void
IsoTerminal_c::reactOnIsoItemModification (IsoItemModification_t at_action, IsoItem_c const& acrc_isoItem)
{
  // we only care for the VTs
  if (acrc_isoItem.isoName().getEcuType() != IsoName_c::ecuTypeVirtualTerminal) return;

  STL_NAMESPACE::list<VtServerInstance_c>::iterator lit_vtServerInst;

  switch (at_action)
  {
    case AddToMonitorList:
      { ///! Attention: This function is also called from "init()", not only from ISOMonitor!
        for (lit_vtServerInst = ml_vtServerInst.begin(); lit_vtServerInst != ml_vtServerInst.end(); lit_vtServerInst++)
        { // check if newly added VtServerInstance is already in our list
          if (lit_vtServerInst->getIsoItem())
          {
            if (acrc_isoItem.isoName() == lit_vtServerInst->getIsoItem()->isoName())
            { // the VtServerInstance is already known and in our list, so update the source address in case it has changed now
              return;
            }
          }
        }

        // VtServerInstance not yet in list, so add it ...
        /// @todo SOON-79: It should be enough if we store the IsoItem*, we don't need both the IsoItem AND IsoName...
        ml_vtServerInst.push_back (VtServerInstance_c (acrc_isoItem, acrc_isoItem.isoName(), *this SINGLETON_VEC_KEY_WITH_COMMA));
        VtServerInstance_c& r_vtServerInst = ml_vtServerInst.back();

        // ... and notify all vtClientServerComm instances
        for (uint8_t ui8_index = 0; ui8_index < mvec_vtClientServerComm.size(); ui8_index++)
        {
          if (mvec_vtClientServerComm[ui8_index])
            mvec_vtClientServerComm[ui8_index]->notifyOnNewVtServerInstance (r_vtServerInst);
        }
      } break;

    case RemoveFromMonitorList:
      for (lit_vtServerInst = ml_vtServerInst.begin(); lit_vtServerInst != ml_vtServerInst.end(); lit_vtServerInst++)
      { // check if lost VtServerInstance is in our list
        if (lit_vtServerInst->getIsoItem())
        {
          if (acrc_isoItem.isoName() == lit_vtServerInst->getIsoItem()->isoName())
          { // the VtServerInstance is already known and in our list, so it could be deleted
            // notify all clients on early loss of that VtServerInstance
            for (uint8_t ui8_index = 0; ui8_index < mvec_vtClientServerComm.size(); ui8_index++)
            {
              if (mvec_vtClientServerComm[ui8_index])
              {
                mvec_vtClientServerComm[ui8_index]->notifyOnVtServerInstanceLoss(*lit_vtServerInst);
              }
            }

            ml_vtServerInst.erase (lit_vtServerInst);
            break;
          }
        }
      }
      break;

    default:
      // for right now, don't care if VT changes its SourceAddress.
      break;
  }
}




/// INTERFACE FUNTIONS ///
// the following define should be globally defined in the project settings...
/// FAKE_VT_PROPERTIES IS ONLY NEEDED FOR ***IOP_GENERATOR**
#ifdef FAKE_VT_PROPERTIES
void
IsoTerminal_c::fakeVtProperties (uint16_t aui16_dimension, uint16_t aui16_skWidth, uint16_t aui16_skHeight, uint8_t aui16_colorDepth, uint16_t aui16_fontSizes)
{
  const IsoItem_c c_dummyIsoItem;
  // casting NULL to a reference is okay here, as the reference isn't used for any FAKE_VT case (iop_generator, etc.)
  ml_vtServerInst.push_back (VtServerInstance_c (c_dummyIsoItem, IsoName_c::IsoNameUnspecified(), (*this) SINGLETON_VEC_KEY_WITH_COMMA));
  VtServerInstance_c& r_vtServerInst = ml_vtServerInst.back();
  r_vtServerInst.fakeVtProperties (aui16_dimension, aui16_skWidth, aui16_skHeight, aui16_colorDepth, aui16_fontSizes);

  // ... and notify all vtClientServerComm instances
  for (uint8_t ui8_index = 0; ui8_index < mvec_vtClientServerComm.size(); ui8_index++)
  {
    if (mvec_vtClientServerComm[ui8_index])
      mvec_vtClientServerComm[ui8_index]->notifyOnNewVtServerInstance(r_vtServerInst);
  }
}
#endif
///  Used for Debugging Tasks in Scheduler_c
const char*
IsoTerminal_c::getTaskName() const
{ return "IsoTerminal_c()"; }


} // end namespace __IsoAgLib
