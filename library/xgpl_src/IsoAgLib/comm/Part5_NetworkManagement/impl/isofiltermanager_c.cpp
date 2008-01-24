/***************************************************************************
                          isofiltermanager_c.cpp - object for managing IsoFilters
                             -------------------
    begin                : Fri Oct 10 2006
    copyright            : (C) 2006 - 2006 by Dipl.-Inf. Martin Wodok
    email                : m.wodok@osb-ag:de
    type                 : Source
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
#include "isofiltermanager_c.h"

#include <IsoAgLib/driver/can/impl/canio_c.h>


namespace __IsoAgLib {


/** default destructor which has nothing to do */
IsoFilterManager_c::~IsoFilterManager_c ()
{
//  close();
}


/** constructor for IsoRequestPgn_c */
IsoFilterManager_c::IsoFilterManager_c ()
: SingletonIsoFilterManager_c ()
{
// functionality moved OUT of the constructor, as the constructor is NOT called in embedded systems for static class instances.
}


/** initialize directly after the singleton instance is created.
  * this is called from singleton.h and should NOT be called from the user again.
  * users please use init(...) instead. */
void
IsoFilterManager_c::singletonInit ()
{
  mb_alreadyInitialized = false; // so init() will init!
  // set very long execution period as this singleton has no periodic jobs
  setTimePeriod( 10000 );
}


/** just a dummy implementation of virtual abstract functions in Scheduler_Task_c */
bool IsoFilterManager_c::timeEvent( void )
{
  return true;
}


/** just a dummy implementation of virtual abstract functions in Scheduler_Task_c */
void IsoFilterManager_c::close( void )
{
}


/** just a dummy implementation of virtual abstract functions in Scheduler_Task_c */
const char* IsoFilterManager_c::getTaskName() const
{
  return "IsoFilterManager_c";
}


void
IsoFilterManager_c::init()
{
  if (!mb_alreadyInitialized)
  { // avoid double initialization. for now now close needed, only init once! ==> see Scheduler_c::startupSystem()
    mb_alreadyInitialized = true;
    // register to get IsoMonitor list changes
    __IsoAgLib::getIsoMonitorInstance4Comm().registerSaClaimHandler( this );
  }
}


bool
IsoFilterManager_c::existIsoFilter (const IsoFilter_s& arcs_isoFilter)
{
  for (IsoFilterBox_it it_isoFilterBox = mvec_isoFilterBox.begin();
       it_isoFilterBox != mvec_isoFilterBox.end();
       it_isoFilterBox++)
  { // Search for existing IsoFilterBox
    if (it_isoFilterBox->hasIsoFilter (arcs_isoFilter))
    { // This IsoFilterBox already has such a filter
      return true;
    }
  }
  return false;
}


void
IsoFilterManager_c::insertIsoFilter (const IsoFilter_s& arcs_isoFilter, bool ab_immReconfigure)
{
  // Check if IsoFilter does yet exist in some IsoFilterBox
  if (!existIsoFilter (arcs_isoFilter))
  { // insert an empty IsoFilterBox. initialized then in list right after
    mvec_isoFilterBox.push_back (IsoFilterBox_c (arcs_isoFilter SINGLETON_VEC_KEY_WITH_COMMA));

    // now get the inserted IsoFilterBox and retrigger update of real hardware filters
    if (mvec_isoFilterBox.back().updateOnAdd())
    {
      if (ab_immReconfigure)
        getCanInstance4Comm().reconfigureMsgObj();
    }
  }
}


bool
IsoFilterManager_c::removeIsoFilter (const IsoFilter_s& arcs_isoFilter)
{
  for (IsoFilterBox_it it_isoFilterBox = mvec_isoFilterBox.begin();
       it_isoFilterBox != mvec_isoFilterBox.end();
       it_isoFilterBox++)
  { // Search for existing IsoFilterBox
    if (it_isoFilterBox->hasIsoFilter (arcs_isoFilter))
    {
      it_isoFilterBox->updateOnRemove (NULL);
      mvec_isoFilterBox.erase (it_isoFilterBox);
      return true;
    }
  }

  // IsoFilterBox not existing, can't remove!
  return false;
}



/** this function is called by IsoMonitor_c on addition, state-change and removal of an IsoItem.
 * @param at_action enumeration indicating what happened to this IsoItem. @see IsoItemModification_en / IsoItemModification_t
 * @param acrc_isoItem reference to the (const) IsoItem which is changed (by existance or state)
   */
void
IsoFilterManager_c::reactOnIsoItemModification (IsoItemModification_t at_action, IsoItem_c const& acrc_isoItem)
{
#ifdef DEBUG_NETWORK_MANAGEMENT
  INTERNAL_DEBUG_DEVICE << "[DNN] IsoFilterManager_c: React on IsoItem (" << 
    #ifdef SYSTEM_PC
        "0x" << STL_NAMESPACE::hex
    #endif //SYSTEM_PC
  << (int)acrc_isoItem.nr() << ") modification ";

       if (at_action == AddToMonitorList)      { INTERNAL_DEBUG_DEVICE << "AddToMonitorList" << INTERNAL_DEBUG_DEVICE_ENDL; }
  else if (at_action == ChangedAddress)        { INTERNAL_DEBUG_DEVICE << "ChangedAddress" << INTERNAL_DEBUG_DEVICE_ENDL; }
  else if (at_action == LostAddress)           { INTERNAL_DEBUG_DEVICE << "LostAddress" << INTERNAL_DEBUG_DEVICE_ENDL; }
  else if (at_action == ReclaimedAddress)      { INTERNAL_DEBUG_DEVICE << "ReclaimedAddress" << INTERNAL_DEBUG_DEVICE_ENDL; }
  else if (at_action == RemoveFromMonitorList) { INTERNAL_DEBUG_DEVICE << "RemoveFromMonitorList" << INTERNAL_DEBUG_DEVICE_ENDL; }
#endif //DEBUG_NETWORK_MANAGEMENT

  if ((at_action == AddToMonitorList) || (at_action == ReclaimedAddress))
  {
    bool b_reconfig = false;
    for (IsoFilterBox_it it_isoFilterBox = mvec_isoFilterBox.begin();
        it_isoFilterBox != mvec_isoFilterBox.end();
        it_isoFilterBox++)
    { // the ISOFilterBoxes will take care if they have to do anything at all or not...
      b_reconfig |= it_isoFilterBox->updateOnAdd();
    }

    if (b_reconfig)
      getCanInstance4Comm().reconfigureMsgObj();
  }
  else if (at_action == ChangedAddress)
  {
    bool b_reconfig = false;
    for (IsoFilterBox_it it_isoFilterBox = mvec_isoFilterBox.begin();
        it_isoFilterBox != mvec_isoFilterBox.end();
        it_isoFilterBox++)
    { // the ISOFilterBoxes will take care if they have to do anything at all or not...
      it_isoFilterBox->updateOnRemove(&acrc_isoItem.isoName());
      b_reconfig |= it_isoFilterBox->updateOnAdd();
    }

    if (b_reconfig)
      getCanInstance4Comm().reconfigureMsgObj();
  }
  else
  { // ((at_action == RemoveFromMonitorList) || (at_action == LostAddress))
    for (IsoFilterBox_it it_isoFilterBox = mvec_isoFilterBox.begin();
         it_isoFilterBox != mvec_isoFilterBox.end();
         it_isoFilterBox++)
    { // the ISOFilterBoxes will take care if they have to do anything at all or not...
      it_isoFilterBox->updateOnRemove (&acrc_isoItem.isoName());
    }
  }
}



#if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
/** C-style function, to get access to the unique IsoFilterManager_c singleton instance
 * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS */
IsoFilterManager_c& getIsoFilterManagerInstance (uint8_t aui8_instance)
{ // if > 1 singleton instance is used, no static reference can be used
  return IsoFilterManager_c::instance(aui8_instance);
};
#else
/** C-style function, to get access to the unique IsoFilterManager_c singleton instance */
IsoFilterManager_c& getIsoFilterManagerInstance (void)
{
  static IsoFilterManager_c& c_isoFilterManager = IsoFilterManager_c::instance ();
  return c_isoFilterManager;
};
#endif

} // end of namespace __IsoAgLib
