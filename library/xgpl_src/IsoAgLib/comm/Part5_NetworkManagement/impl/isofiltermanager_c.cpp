/*
  isofiltermanager_c.cpp: object for managing IsoFilters

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include "isofiltermanager_c.h"

#include <IsoAgLib/comm/impl/isobus_c.h>
#include <IsoAgLib/util/iassert.h>

#if defined(_MSC_VER)
#pragma warning( disable : 4355 )
#endif

namespace __IsoAgLib {


/** constructor for IsoRequestPgn_c */
IsoFilterManager_c::IsoFilterManager_c () :
  mvec_isoFilterBox(),
  mt_handler(*this)
{
}


/** just a dummy implementation of virtual abstract functions in Scheduler_Task_c */
bool IsoFilterManager_c::timeEvent( void )
{
  return true;
}


#if DEBUG_SCHEDULER
const char* IsoFilterManager_c::getTaskName() const
{ return "IsoFilterManager_c"; }
#endif


void
IsoFilterManager_c::init()
{
  isoaglib_assert (!initialized());

  __IsoAgLib::getIsoMonitorInstance4Comm().registerControlFunctionStateHandler( mt_handler );

  // set very long execution period as this singleton has no periodic jobs
  setTimePeriod( 10000 );

  setInitialized();
}


void
IsoFilterManager_c::close()
{
  isoaglib_assert (initialized());

  // for now, clear all the registered filters.
  for (IsoFilterBox_it it_isoFilterBox = mvec_isoFilterBox.begin();
       it_isoFilterBox != mvec_isoFilterBox.end();
        ++it_isoFilterBox)
  { // Search for existing IsoFilterBox
    it_isoFilterBox->updateOnRemove (NULL);
  }
  mvec_isoFilterBox.clear();
  // for later, all modules should remove their filters!

  getIsoMonitorInstance4Comm().deregisterControlFunctionStateHandler( mt_handler );

  setClosed();
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
    mvec_isoFilterBox.push_back (IsoFilterBox_c (arcs_isoFilter MULTITON_INST_WITH_COMMA));

    // now get the inserted IsoFilterBox and retrigger update of real hardware filters
    if (mvec_isoFilterBox.back().updateOnAdd())
    {
      if (ab_immReconfigure)
        getIsoBusInstance4Comm().reconfigureMsgObj();
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
IsoFilterManager_c::reactOnIsoItemModification (ControlFunctionStateHandler_c::IsoItemModification_t at_action, IsoItem_c const& acrc_isoItem)
{
#if DEBUG_NETWORK_MANAGEMENT
  INTERNAL_DEBUG_DEVICE
    << "[DNN] IsoFilterManager_c: React on IsoItem ("
#  ifdef SYSTEM_PC
    << "0x" << STL_NAMESPACE::hex
#  endif //SYSTEM_PC
    << int(acrc_isoItem.nr()) << ") modification "
#  ifdef SYSTEM_PC
    << STL_NAMESPACE::dec
#  endif //SYSTEM_PC
    ;
  {
    const char *pcch_action;
    switch (at_action) {
    case ControlFunctionStateHandler_c::AddToMonitorList:
      pcch_action = "AddToMonitorList"; break;
    case ControlFunctionStateHandler_c::ChangedAddress:
      pcch_action = "ChangedAddress"; break;
    case ControlFunctionStateHandler_c::LostAddress:
      pcch_action = "LostAddress"; break;
    case ControlFunctionStateHandler_c::ReclaimedAddress:
      pcch_action = "ReclaimedAddress"; break;
    case ControlFunctionStateHandler_c::RemoveFromMonitorList:
      pcch_action = "RemoveFromMonitorList"; break;
    default:
      pcch_action = "?";
    }
    INTERNAL_DEBUG_DEVICE << pcch_action << INTERNAL_DEBUG_DEVICE_ENDL;
  }
#endif //DEBUG_NETWORK_MANAGEMENT

  if ((at_action == ControlFunctionStateHandler_c::AddToMonitorList) || (at_action == ControlFunctionStateHandler_c::ReclaimedAddress))
  {
    bool b_reconfig = false;
    for (IsoFilterBox_it it_isoFilterBox = mvec_isoFilterBox.begin();
        it_isoFilterBox != mvec_isoFilterBox.end();
        it_isoFilterBox++)
    { // the ISOFilterBoxes will take care if they have to do anything at all or not...
      bool const cb_set = it_isoFilterBox->updateOnAdd();
      if (cb_set)
        b_reconfig = true;
    }

    if (b_reconfig)
      getIsoBusInstance4Comm().reconfigureMsgObj();
  }
  else if (at_action == ControlFunctionStateHandler_c::ChangedAddress)
  {
    bool b_reconfig = false;
    for (IsoFilterBox_it it_isoFilterBox = mvec_isoFilterBox.begin();
        it_isoFilterBox != mvec_isoFilterBox.end();
        it_isoFilterBox++)
    { // the ISOFilterBoxes will take care if they have to do anything at all or not...
      it_isoFilterBox->updateOnRemove(&acrc_isoItem.isoName());
      bool const cb_set = it_isoFilterBox->updateOnAdd();
      if (cb_set)
        b_reconfig = true;
    }

    if (b_reconfig)
      getIsoBusInstance4Comm().reconfigureMsgObj();
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



/** C-style function, to get access to the unique IsoFilterManager_c singleton instance
 * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS */
IsoFilterManager_c &getIsoFilterManagerInstance (uint8_t aui8_instance)
{ // if > 1 singleton instance is used, no static reference can be used
  MACRO_MULTITON_GET_INSTANCE_BODY(IsoFilterManager_c, PRT_INSTANCE_CNT, aui8_instance);
}

} // end of namespace __IsoAgLib
