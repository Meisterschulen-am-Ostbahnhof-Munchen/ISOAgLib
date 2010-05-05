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

#include <IsoAgLib/driver/can/impl/canio_c.h>


namespace __IsoAgLib {


/** default destructor which has nothing to do */
IsoFilterManager_c::~IsoFilterManager_c ()
{
  close();
}


/** constructor for IsoRequestPgn_c */
IsoFilterManager_c::IsoFilterManager_c () :
  SingletonIsoFilterManager_c (),
  mt_handler(*this)
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
  if ( ! checkAlreadyClosed() ) {
    // avoid another call
    setAlreadyClosed();

      // unregister ISO monitor list changes
    __IsoAgLib::getIsoMonitorInstance4Comm().deregisterControlFunctionStateHandler( mt_handler );
  }
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
    // clear state of b_alreadyClosed, so that close() is called one time
    clearAlreadyClosed();
    // register to get IsoMonitor list changes
    __IsoAgLib::getIsoMonitorInstance4Comm().registerControlFunctionStateHandler( mt_handler );
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
      getCanInstance4Comm().reconfigureMsgObj();
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
