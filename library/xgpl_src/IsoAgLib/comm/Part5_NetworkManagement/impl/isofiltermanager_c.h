/*
  isofiltermanager_c.h

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef ISO_FILTER_MANAGER_C_H
#define ISO_FILTER_MANAGER_C_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include <IsoAgLib/isoaglib_config.h>
#include <IsoAgLib/util/config.h>

#include <IsoAgLib/util/impl/singleton.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isomonitor_c.h>

#include "isofilterbox_c.h"

#include <cstdlib>	// Include before vector or else CNAMESPACE stuff is screwed up for Tasking
#include <vector>

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

/** this object manages ISO-Filters - those may contain references
    to ISOName's and are initelligent self-adapting can-filters
    @short Manager for handling of inserting/adapting FilterBox_c-instances
    @see __IsoAgLib::ControlFunctionStateHandler_c
    @author Dipl.-Inf. Martin Wodok */
class IsoFilterManager_c : public Subsystem_c
{
  MACRO_MULTITON_CONTRIBUTION();
public:
  typedef STL_NAMESPACE::vector<IsoFilterBox_c> IsoFilterBox_vec;
  typedef STL_NAMESPACE::vector<IsoFilterBox_c>::iterator IsoFilterBox_it;

  void init();
  void close();

  ~IsoFilterManager_c () {}

  bool existIsoFilter (const IsoFilter_s& rrefcs_isoFilter);

  void insertIsoFilter (const IsoFilter_s& rrefcs_isoFilter, bool ab_immReconfigure=true);

  bool removeIsoFilter (const IsoFilter_s& rrefcs_isoFilter);

private:
  class ControlFunctionStateHandlerProxy_c : public ControlFunctionStateHandler_c {
  public:
    typedef IsoFilterManager_c Owner_t;

    ControlFunctionStateHandlerProxy_c(Owner_t &art_owner) : mrt_owner(art_owner) {}

    virtual ~ControlFunctionStateHandlerProxy_c() {}

  private:
    virtual void reactOnIsoItemModification(
        IsoItemModification_t at_action,
        IsoItem_c const &acrc_isoItem)
    {
      mrt_owner.reactOnIsoItemModification(at_action, acrc_isoItem);
    }

    // ControlFunctionStateHandlerProxy_c shall not be copyable. Otherwise the
    // reference to the containing object would become invalid.
    ControlFunctionStateHandlerProxy_c(ControlFunctionStateHandlerProxy_c const &);

    ControlFunctionStateHandlerProxy_c &operator=(ControlFunctionStateHandlerProxy_c const &);

    Owner_t &mrt_owner;
  };
  typedef ControlFunctionStateHandlerProxy_c Handler_t;

  // Private methods

  /** this function is called by IsoMonitor_c on addition, state-change and removal of an IsoItem.
   * @param at_action enumeration indicating what happened to this IsoItem. @see IsoItemModification_en / IsoItemModification_t
   * @param acrc_isoItem reference to the (const) IsoItem which is changed (by existance or state)
   */
  void reactOnIsoItemModification (ControlFunctionStateHandler_c::IsoItemModification_t /*at_action*/, IsoItem_c const& /*acrc_isoItem*/);

private:
  /** constructor for IsoFilterManager_c */
  IsoFilterManager_c();

  // Private attributes
  IsoFilterBox_vec mvec_isoFilterBox;

  Handler_t mt_handler;

  friend IsoFilterManager_c &getIsoFilterManagerInstance (uint8_t aui8_instance);
};

/** C-style function, to get access to the unique IsoFilterManager_c singleton instance
    if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS */
IsoFilterManager_c &getIsoFilterManagerInstance (uint8_t aui8_instance = 0);

}
#endif
