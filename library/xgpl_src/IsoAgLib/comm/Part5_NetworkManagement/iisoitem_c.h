/*
  iisoitem_c.h: object which represents an item in a iso monitor list

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef IISO_ITEM_H
#define IISO_ITEM_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include "impl/isoitem_c.h"
#include <IsoAgLib/comm/Part5_NetworkManagement/iisoname_c.h>

namespace __IsoAgLib
{ // forward declarations (for friends)
  class ProprietaryMessageHandler_c;
}

// Begin Namespace IsoAgLib
namespace IsoAgLib {

/** item class for ISO 11783 members monitor list to manage
  local (with address claim) and remote (statistic information)
  systems; utilizes IsoItem_c for basic MonitorList
  management and IsoName_c for management of the 64bit NAME field
  @short Item with services needed for ISO11783 monitor lists.
  @author Dipl.-Inform. Achim Spangler
  @see iMonitorItem
*/
class iIsoItem_c : private __IsoAgLib::IsoItem_c {
private:
  friend class iIdentItem_c;
  friend class iSystemMgmt_c;
  friend class iIsoMonitor_c;
  friend class iCanPkgExt_c;
  friend class IsoItem_c;
  friend class __IsoAgLib::ProprietaryMessageHandler_c;
  friend class iControlFunctionStateHandler_c;

public:
  /** deliver the ISOName code of this item */
  const iIsoName_c& isoName() const { return IsoItem_c::isoName().toConstIisoName_c(); }

  /** deliver the address of this item */
  uint8_t nr() const {return IsoItem_c::nr();}

  /** check if specific state is set
    @param ren_itemState state information to check
    @return true -> the given state is set
  */
  bool itemState(itemState_t ren_itemState) const {return IsoItem_c::itemState(ren_itemState);}

  /** deliver the state information
    @return state information of type itemState_t (with state informations coded by OR in enum)
  */
  itemState_t itemState() const {return IsoItem_c::itemState();}

  itemState_t setItemState(itemState_t ren_itemState) {return IsoItem_c::setItemState(ren_itemState, false);}

#ifdef USE_WORKING_SET
  /// For checking if the WS-Announce is completed use the
  bool isWsAnnounced (int32_t ai32_timeAnnounceStarted) { return IsoItem_c::isWsAnnounced (ai32_timeAnnounceStarted); }
  /** get master of this isoItem
      @return  this if master himself; get master if client; NULL if standalone
    */
  iIsoItem_c* getMaster() { return ( IsoItem_c::getMaster() == NULL)? NULL : &IsoItem_c::getMaster()->toIisoItem_c();}
#endif
};

}

#endif
