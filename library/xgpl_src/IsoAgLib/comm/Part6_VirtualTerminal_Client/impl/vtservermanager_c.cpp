/*
  vtservermanager_c.cpp: central VT Client management

  (C) Copyright 2009 - 2019 by OSB AG

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Usage under Commercial License:
  Licensees with a valid commercial license may use this file
  according to their commercial license agreement. (To obtain a
  commercial license contact OSB AG via <http://isoaglib.com/en/contact>)

  Usage under GNU General Public License with exceptions for ISOAgLib:
  Alternatively (if not holding a valid commercial license)
  use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#include "vtservermanager_c.h"

#include <IsoAgLib/comm/Part6_VirtualTerminal_Client/ivtclientobjectpool_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isoname_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isoitem_c.h>
#include "vtserverinstance_c.h"
#include "vtclientconnection_c.h"
#include "vtclient_c.h"

// @todo: needed?
#if defined(_MSC_VER)
#pragma warning( disable : 4355 )
#endif


namespace __IsoAgLib {


VtServerManager_c::VtServerManager_c()
  : ml_vtServerInst()
{
}

void
VtServerManager_c::close()
{
  while (!ml_vtServerInst.empty())
  {
    delete ml_vtServerInst.back();
    ml_vtServerInst.pop_back();
  }  
}


VtServerInstance_c* VtServerManager_c::getActiveVtServer( bool mustBePrimary, const VtServerInstance_c* ap_searchStart ) const
{
  auto lit_vtServerInst = ml_vtServerInst.begin();

  if(nullptr != ap_searchStart)
  {
    for ( ; lit_vtServerInst != ml_vtServerInst.end(); ++lit_vtServerInst)
    {
      if(ap_searchStart == *lit_vtServerInst)
      break;
    }
    
    if(lit_vtServerInst == ml_vtServerInst.end())
    {
       lit_vtServerInst = ml_vtServerInst.begin();
    }
    else
    {
       ++lit_vtServerInst;
    }
  }

  auto lit_searchStart = lit_vtServerInst;
  
  // search rest of list
  for ( ; lit_vtServerInst != ml_vtServerInst.end(); ++lit_vtServerInst)
    if ( (*lit_vtServerInst)->isVtActiveAndResetCapabilitiesIfInactive() && (!mustBePrimary || (*lit_vtServerInst)->isPrimaryVt()) )
      return (*lit_vtServerInst);

    
  // not found => wrap around and start at beginning until ap_searchStart reached again
  lit_vtServerInst = ml_vtServerInst.begin();
  for ( ; lit_vtServerInst != lit_searchStart; ++lit_vtServerInst)
    if ( (*lit_vtServerInst)->isVtActiveAndResetCapabilitiesIfInactive() && (!mustBePrimary || (*lit_vtServerInst)->isPrimaryVt()) )
      return (*lit_vtServerInst);

  return nullptr;
}


VtServerInstance_c* VtServerManager_c::getPreferredVtServer(const IsoName_c& aref_prefferedVTIsoName) const
{
  std::vector<VtServerInstance_c*>::const_iterator lit_vtServerInst;
  for (lit_vtServerInst = ml_vtServerInst.begin(); lit_vtServerInst != ml_vtServerInst.end(); ++lit_vtServerInst)
  {
    if ((*lit_vtServerInst)->isVtActiveAndResetCapabilitiesIfInactive() && ((*lit_vtServerInst)->getIsoName() == aref_prefferedVTIsoName))
      return (*lit_vtServerInst);
  }
  return nullptr;
}


VtServerInstance_c* VtServerManager_c::getSpecificVtServer(const IsoAgLib::iVtClientObjectPool_c& arc_pool) const
{
  std::vector<VtServerInstance_c*>::const_iterator lit_vtServerInst;
  for (lit_vtServerInst = ml_vtServerInst.begin(); lit_vtServerInst != ml_vtServerInst.end(); ++lit_vtServerInst)
  {
    if ((*lit_vtServerInst)->isVtActiveAndResetCapabilitiesIfInactive() && arc_pool.selectVtServer((*lit_vtServerInst)->getIsoName().toConstIisoName_c()))
      return (*lit_vtServerInst);
  }
  return nullptr;
}

uint16_t VtServerManager_c::getActiveVtCount() const
{
  uint16_t cnt = 0;
  
  std::vector<VtServerInstance_c*>::const_iterator lit_vtServerInst;
  for (lit_vtServerInst = ml_vtServerInst.begin(); lit_vtServerInst != ml_vtServerInst.end(); ++lit_vtServerInst)
  {
    if ((*lit_vtServerInst)->isVtActiveAndResetCapabilitiesIfInactive())
      cnt++;
  }
  return cnt;    
}



void
VtServerManager_c::reactOnIsoItemModification(
  ControlFunctionStateHandler_c::iIsoItemAction_e action,
  IsoItem_c const& isoItem,
  std::vector<VtClientConnection_c*>& aref_vtConnections,
  VtClient_c& aref_vtClient )
{
  // we only care for the VTs
  if( isoItem.isoName().getEcuType() != IsoName_c::ecuTypeVirtualTerminal )
    return;

  switch( action )
  {
    case ControlFunctionStateHandler_c::AddToMonitorList:
    { ///! Attention: This function is also called from "init()", not only from ISOMonitor!
      for(auto & iter : ml_vtServerInst)
      { // check if newly added VtServerInstance is already in our list
        if (&isoItem == &iter->getIsoItem())
          return;
      }

      ml_vtServerInst.push_back(
        new VtServerInstance_c( isoItem, aref_vtClient ) );
    } break;

    case ControlFunctionStateHandler_c::RemoveFromMonitorList:
      for( auto iter = ml_vtServerInst.begin();
           iter != ml_vtServerInst.end(); ++iter )
      { // check if lost VtServerInstance is in our list
        if (&isoItem == &(*iter)->getIsoItem())
        { // the VtServerInstance is already known and in our list,
          // delete it and notify all clients on early loss of that VtServerInstance
          for (auto & aref_vtConnection : aref_vtConnections)
          {
            if( aref_vtConnection )
              aref_vtConnection->notifyOnVtServerInstanceLoss( *(*iter) );
          }

          delete *iter;
          ( void )ml_vtServerInst.erase( iter );
          break;
        }
      }
      break;

    default:
      break;
  }
}


void
VtServerManager_c::processVtStatusMsg(const CanPkgExt_c& arc_data,
                                      std::vector<VtClientConnection_c*>& aref_vtConnections)
{
  std::vector<VtServerInstance_c*>::iterator lit_vtServerInst;

  for (lit_vtServerInst = ml_vtServerInst.begin();
      lit_vtServerInst != ml_vtServerInst.end();
      ++lit_vtServerInst)
  {
    if (&(*lit_vtServerInst)->getIsoItem() == arc_data.getMonitorItemForSA())
    {
      (*lit_vtServerInst)->setLatestVtStatusData( arc_data );

      // iterate through all registered VtClientServerCommunication and notify their pools with "eventVtStatusMsg"
      for (auto & aref_vtConnection : aref_vtConnections)
      {
        if (aref_vtConnection)
        {
          if (aref_vtConnection->getVtServerInstPtr() == (*lit_vtServerInst))
          { // this vtClientServerComm is connected to this VT, so notify the objectpool!!
            aref_vtConnection->notifyOnVtStatusMessage();
          }
        }
      }
      break;
    }
  }
}

    bool VtServerManager_c::isAnyVtAvailable() const 
	{ 
	return !ml_vtServerInst.empty(); 
	}

    std::vector<VtServerInstance_c *> &VtServerManager_c::getRefServerInstanceVec() 
	{ 
	return ml_vtServerInst; 
	}

    VtServerManager_c::~VtServerManager_c() = default;

// (currently not supported, due to multi VT enhancements)
#if 0
#ifdef USE_IOP_GENERATOR_FAKE_VT_PROPERTIES
void
VtServerManager_c::fakeVtProperties (uint16_t aui16_dimension, uint16_t aui16_skWidth, uint16_t aui16_skHeight,
                                     uint8_t aui16_colorDepth, uint16_t aui16_fontSizes,
                                     VtClient_c& aref_vtClient)
{
  const IsoItem_c c_dummyIsoItem;
  // casting NULL to a reference is okay here, as the reference isn't used for any FAKE_VT case (iop_generator, etc.)
  ml_vtServerInst.push_back ( new VtServerInstance_c (c_dummyIsoItem, aref_vtClient) );
  VtServerInstance_c* p_vtServerInst = ml_vtServerInst.back();
  p_vtServerInst->fakeVtProperties (aui16_dimension, aui16_skWidth, aui16_skHeight, aui16_colorDepth, aui16_fontSizes);
  // ... and notify all vtClientServerComm instances
}
#endif
#endif

} // __IsoAgLib
