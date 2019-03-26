/*
  tractorcommonrx_c.cpp: base class for receiving typical tractor information

  (C) Copyright 2016 - 2019 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include "tractorcommonrx_c.h"
#include <IsoAgLib/driver/can/impl/canpkg_c.h>
#include <IsoAgLib/comm/impl/isobus_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isomonitor_c.h>
#include <IsoAgLib/util/iassert.h>


namespace __IsoAgLib {


void
TractorCommonRx_c::init()
{
  isoaglib_assert( !initialized() );

  mi32_lastMsgReceived = -1;
  mc_sender.setUnspecified();

  resetValues();

  getSchedulerInstance().registerTask( mt_task, 0 );
  getIsoBusInstance4Comm().insertFilter( *this, IsoAgLib::iMaskFilter_c( 0x3FFFF00UL, ( mui32_pgn << 8 ) ), 8 );

  setInitialized();
}


void
TractorCommonRx_c::close()
{
  isoaglib_assert( initialized() );
  isoaglib_assert( mvec_msgEventHandlers.empty() );

  setClosed();

  getIsoBusInstance4Comm().deleteFilter( *this, IsoAgLib::iMaskFilter_c( 0x3FFFF00UL, ( mui32_pgn << 8 ) ) );
  getSchedulerInstance().deregisterTask(mt_task);
};


bool
TractorCommonRx_c::checkParseReceived( const IsoName_c& sender ) const
{
  if( mc_sender == sender )
    return true; // actual sender equivalent to last, always fine. (should be most typical case)

  if( mc_sender.isUnspecified() )
    return true; // no sender yet locked to, so take ANY sender.

  if( sender.getEcuType() != IsoName_c::ecuTypeTractorECU )
    return false; // if we have any sender, we only change to TECUs

  return( ( mc_sender.getEcuType() != IsoName_c::ecuTypeTractorECU ) // fine if we didn't have a TECU yet.
       || ( mc_sender.funcInst() > sender.funcInst() ) ); // new sender has lower TECU function instance (i.e. higher priority) than the current TECU.
}


void
TractorCommonRx_c::deregisterMsgEventHandler (IsoAgLib::iMsgEventHandler_c &arc_msgEventHandler)
{
  for (STL_NAMESPACE::vector<IsoAgLib::iMsgEventHandler_c*>::iterator iter = mvec_msgEventHandlers.begin(); iter != mvec_msgEventHandlers.end();)
  {
    if ((*iter) == &arc_msgEventHandler)
      iter = mvec_msgEventHandlers.erase (iter);
    else
      ++iter;
  }
}


void
TractorCommonRx_c::notifyOnEvent()
{
  STL_NAMESPACE::vector<IsoAgLib::iMsgEventHandler_c*>::iterator iter_end = mvec_msgEventHandlers.end();
  for (STL_NAMESPACE::vector<IsoAgLib::iMsgEventHandler_c*>::iterator iter = mvec_msgEventHandlers.begin(); iter != iter_end; ++iter)
  {
    (*iter)->handleMsgEvent( mui32_pgn );
  }
}


void
TractorCommonRx_c::timeEvent()
{
  if ( mi32_lastMsgReceived < 0 )
    return;
  
  if( ( mui16_timeOut != TIMEOUT_SENDING_NODE_NONE ) &&
      ( lastedTimeSinceUpdate() >= mui16_timeOut ) )
  {
    mi32_lastMsgReceived = -1;
    mc_sender.setUnspecified();

    resetValues();

    notifyOnEvent();
  }
}


void
TractorCommonRx_c::processMsg( const CanPkg_c& data )
{
  CanPkgExt_c pkg( data, getMultitonInst() );
  if( !pkg.isValid() || (pkg.getMonitorItemForSA() == NULL) )
    return;

  isoaglib_assert( pkg.isoPgn() == mui32_pgn );

  IsoName_c const& sender = pkg.getISONameForSA();

  if( checkParseReceived( sender ) )
  {
    updateReceived( pkg.time(), sender );

    setValues( pkg );

    notifyOnEvent();
  }
}


} // __IsoAgLib
