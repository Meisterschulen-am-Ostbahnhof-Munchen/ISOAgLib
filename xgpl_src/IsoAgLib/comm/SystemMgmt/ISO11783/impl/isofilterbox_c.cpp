/***************************************************************************
                              isofilterbox_c.h
                             -------------------
    begin                : Mon Oct 09 2006
    copyright            : (C) 2000 - 2006 by Dipl.-Inf, Martin Wodok
    email                : m.wodok@osb-ag:de
    type                 : Header
    $LastChangedDate:  $
    $LastChangedRevision:  $
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

#include "isofilterbox_c.h"
#include <IsoAgLib/comm/SystemMgmt/ISO11783/impl/isomonitor_c.h>
#include <IsoAgLib/driver/can/impl/canio_c.h>

namespace __IsoAgLib {


/** empty constructor - everything has to be "constructed" with the "init"-function! */
ISOFilterBox_c::ISOFilterBox_c()
: ui8_filtersSetUp (0)
{}

ISOFilterBox_c::ISOFilterBox_c(const ISOFilterBox_c& rrefcc_refFB)
: slist_managedISOFilter (rrefcc_refFB.slist_managedISOFilter)
, ui8_filtersSetUp (rrefcc_refFB.ui8_filtersSetUp)
{}


// void
// ISOFilterBox_c::init (CANCustomer_c& rrefc_canCustomer)
// {
// }


// dlcForce == -1: don't check dlc. value of 0..8: force to be exactly this dlc!
ISOFilter_s::ISOFilter_s (CANCustomer_c& rrefc_canCustomer, uint32_t rui32_mask, uint32_t rui32_filter, const ISOName_c* rpc_isoNameDa, const ISOName_c* rpc_isoNameSa, int8_t ri8_dlcForce, Ident_c::identType_t rt_identType)
  : c_identMask (rui32_mask, rt_identType)
  , c_identFilter (rui32_filter, rt_identType)
  , pc_canCustomer (&rrefc_canCustomer)
  , i8_dlcForce (ri8_dlcForce)
{
  if (rpc_isoNameDa) c_isoNameDa = *rpc_isoNameDa;                  // operator =
  else /* no name */ c_isoNameDa.setUnspecified();
  if (rpc_isoNameSa) c_isoNameSa = *rpc_isoNameSa;                  // operator =
  else /* no name */ c_isoNameSa.setUnspecified();
}

ISOFilter_s::~ISOFilter_s() {}


bool
ISOFilter_s::equalMaskAndFilter (const ISOFilter_s& rrefc_isoFilter) const
{
  return ((c_identMask   == rrefc_isoFilter.c_identMask)
       && (c_identFilter == rrefc_isoFilter.c_identFilter)
       && (c_isoNameSa   == rrefc_isoFilter.c_isoNameSa)
       && (c_isoNameDa   == rrefc_isoFilter.c_isoNameDa));
}



bool
ISOFilterBox_c::addIsoFilter (const ISOFilter_s& rrefcs_isoFilter)
{
  slist_managedISOFilter.push_front (rrefcs_isoFilter); // operator = (not overwritten, but mem-copy is okay!)

  return true; /** @todo Check if filter could be pushed to front.. well, will almost always do.. */
}


ISOFilterBox_c::RemoveAnswer_en
ISOFilterBox_c::removeIsoFilter (const ISOFilter_s& rrefcs_isoFilter)
{
  for (ManagedISOFilter_it it_managedIsoFilter = slist_managedISOFilter.begin();
       it_managedIsoFilter != slist_managedISOFilter.end();
       it_managedIsoFilter++)
  {
    if (it_managedIsoFilter->s_isoFilter == rrefcs_isoFilter) // yes, compare all - including canCustomer (but NOT dlcForce!!!)
    {
      if (it_managedIsoFilter->pc_filterBox)
      { // filter was created in CANIO_c - remove filter
        /** @todo Optimize with a (to be implemented) "deleteFilter (FilterBox_c*);" function */
        getCanInstance4Comm().deleteFilter (*it_managedIsoFilter->s_isoFilter.pc_canCustomer,
                                            it_managedIsoFilter->s_isoFilter.c_identMask.ident(),
                                            it_managedIsoFilter->s_isoFilter.c_identFilter.ident(),
                                            it_managedIsoFilter->s_isoFilter.c_identMask.identType());
        ui8_filtersSetUp--;
      }
      it_managedIsoFilter = slist_managedISOFilter.erase (it_managedIsoFilter);
      if (ui8_filtersSetUp == 0) return RemoveAnswerSuccessBoxEmpty;
      else /* ----------- > 0 */ return RemoveAnswerSuccessBoxNotEmpty;
    }
  }

  return RemoveAnswerFailureNonExistent;
}


bool
ISOFilterBox_c::hasIsoFilterWithCustomer (const ISOFilter_s& rrefcs_isoFilter)
{
  for (ManagedISOFilter_it it_managedIsoFilter = slist_managedISOFilter.begin();
       it_managedIsoFilter != slist_managedISOFilter.end();
       it_managedIsoFilter++)
  {
    if (it_managedIsoFilter->s_isoFilter == rrefcs_isoFilter) // overloaded operator==.
      return true;
  }
  return false;
}


bool
ISOFilterBox_c::hasIsoFilterWithoutCustomer (const ISOFilter_s& rrefcs_isoFilter)
{
  for (ManagedISOFilter_it it_managedIsoFilter = slist_managedISOFilter.begin();
       it_managedIsoFilter != slist_managedISOFilter.end();
       it_managedIsoFilter++)
  {
    if (it_managedIsoFilter->s_isoFilter.equalMaskAndFilter (rrefcs_isoFilter))
      return true;
  }
  return false;
}


#if 0
/** @param rpc_isoName NULL: Check all "ISOItem_c"s because this filter is initially created
                    != NULL: Only exactly this item was claimed right now, check if we can now
                             create the FilterBox_c in CANIO_c (if not already created!)
 */
void
ISOFilterBox_c::updateOnAdd (const ISOName_c* rpc_isoName)
{
  if (ui8_filtersSetUp == slist_managedISOFilter.size())
  { // all filters set up, nothing more to do until "updateOnRemove" comes to play..
    return;
  }

  bool b_reconfig = false;
  FilterBox_c* pc_firstFilter = NULL;
  for (ManagedISOFilter_it it_managedIsoFilter = slist_managedISOFilter.begin();
       it_managedIsoFilter != slist_managedISOFilter.end();
       it_managedIsoFilter++)
  {
    if (it_managedIsoFilter->pc_filterBox == NULL)
    { // There's no filter for this IsoFilter yet, let's try if we can create it!
      Ident_c c_mask   = it_managedIsoFilter->s_isoFilter.c_identMask;
      Ident_c c_filter = it_managedIsoFilter->s_isoFilter.c_identFilter;
      bool b_canCreateFilter = true;

      if (it_managedIsoFilter->s_isoFilter.c_isoNameSa.isSpecified())
      { // see if it's in the monitorlist!
        if ( (rpc_isoName && (*rpc_isoName == it_managedIsoFilter->s_isoFilter.c_isoNameSa))
           || getIsoMonitorInstance4Comm().existIsoMemberISOName (it_managedIsoFilter->s_isoFilter.c_isoNameSa, true)) //true: Member needs to have claimed an address
        { // retrieve current Address
          const uint8_t cui8_adr = getIsoMonitorInstance4Comm().isoMemberISOName (it_managedIsoFilter->s_isoFilter.c_isoNameSa).nr(); //true: Member needs to have claimed an address
          c_mask.set   (cui8_adr, 0, Ident_c::ExtendedIdent);
          c_filter.set (0xFF,     0, Ident_c::ExtendedIdent); // open filter for Address-Byte
        }
        else
        { // can't create the filter - ISOName not claimed on the bus!
          b_canCreateFilter = false;
        }
      }
      /** @todo Yeah, copy & paste does suck... */
      if (it_managedIsoFilter->s_isoFilter.c_isoNameDa.isSpecified())
      { // see if it's in the monitorlist!
        if ( (rpc_isoName && (*rpc_isoName == it_managedIsoFilter->s_isoFilter.c_isoNameDa))
           || getIsoMonitorInstance4Comm().existIsoMemberISOName (it_managedIsoFilter->s_isoFilter.c_isoNameDa, true)) //true: Member needs to have claimed an address
        { // retrieve current Address
          const uint8_t cui8_adr = getIsoMonitorInstance4Comm().isoMemberISOName (it_managedIsoFilter->s_isoFilter.c_isoNameDa).nr(); //true: Member needs to have claimed an address
          c_mask.set   (cui8_adr, 1, Ident_c::ExtendedIdent);
          c_filter.set (0xFF,     1, Ident_c::ExtendedIdent); // open filter for Address-Byte
        }
        else
        { // can't create the filter - ISOName not claimed on the bus!
          b_canCreateFilter = false;
        }
      }

      if (b_canCreateFilter)
      {
        // If we don't have a filterbox to connect to yet, search the list to the end to see if there's one!
        if (pc_firstFilter == NULL)
        { // see if we can connect to one...
          for (ManagedISOFilter_it it_managedIsoFilterSearch = it_managedIsoFilter;
              it_managedIsoFilterSearch != slist_managedISOFilter.end();
              it_managedIsoFilterSearch++)
          {
            if (it_managedIsoFilterSearch->pc_filterBox)
            { // yihaa, found one already created filter!
              pc_firstFilter = it_managedIsoFilterSearch->pc_filterBox;
              break; // from for-loop
            }
          }
        }
        FilterBox_c* const cpc_insertedFilter =
          getCanInstance4Comm().insertFilter (*it_managedIsoFilter->s_isoFilter.pc_canCustomer,
                                              c_mask.ident(), c_filter.ident(),
                                              false, c_filter.identType(), pc_firstFilter);
        ui8_filtersSetUp++;
        it_managedIsoFilter->pc_filterBox = cpc_insertedFilter;
        b_reconfig = true;
      }
    }

    if ((pc_firstFilter == NULL) && (it_managedIsoFilter->pc_filterBox))
    { // we had no first existing filter, so take this one in case the later boxes need one to attach to
      pc_firstFilter = it_managedIsoFilter->pc_filterBox;
    }
  }

  if (b_reconfig)
  { // we created at least one FilterBox
    getCanInstance4Comm().reconfigureMsgObj();
  }
}


/** @param rpc_isoName NULL: shutdown, remove all -
                    != NULL: only exactly this item was removed!
 */
void
ISOFilterBox_c::updateOnRemove (const ISOName_c* rpc_isoName)
{
  if (ui8_filtersSetUp == 0)
  { // no filters set up, nothing more to do until "updateOnAdd" comes to play..
    return;
  }

  bool b_reconfig = false;
  for (ManagedISOFilter_it it_managedIsoFilter = slist_managedISOFilter.begin();
       it_managedIsoFilter != slist_managedISOFilter.end();
       it_managedIsoFilter++)
  {
    if (it_managedIsoFilter->pc_filterBox)
    { // let's see what we have merged into this filter
      if ( (rpc_isoName == NULL) // if NULL is given as parameter, remove ALL the filters
        || (it_managedIsoFilter->s_isoFilter.c_isoNameSa == *rpc_isoName)
        || (it_managedIsoFilter->s_isoFilter.c_isoNameDa == *rpc_isoName)
         )
      {
        /** @todo Optimize with a (to be implemented) "deleteFilter (FilterBox_c*);" function */
        getCanInstance4Comm().deleteFilter (*it_managedIsoFilter->s_isoFilter.pc_canCustomer,
                                            it_managedIsoFilter->s_isoFilter.c_identMask.ident(),
                                            it_managedIsoFilter->s_isoFilter.c_identFilter.ident(),
                                            it_managedIsoFilter->s_isoFilter.c_identMask.identType());
        ui8_filtersSetUp--;
        it_managedIsoFilter->pc_filterBox = NULL;
      }
    }
  }

  if (b_reconfig)
  { // we created at least one FilterBox
    getCanInstance4Comm().reconfigureMsgObj();
  }
}
#endif


/** This method gets sure that always have a clean setup of the filters that can be created at the moment
    (some isonames may not be claimed right now..).
    @todo Maybe optimize to not touch the FilterBoxes that are INdependent of any ISONames? */
void
ISOFilterBox_c::syncFiltersToCan()
{
  bool b_reconfigFilter=false;

  /// 1st) Remove all that were created!
  for (ManagedISOFilter_it it_managedIsoFilter = slist_managedISOFilter.begin();
       it_managedIsoFilter != slist_managedISOFilter.end();
       it_managedIsoFilter++)
  {
    if (it_managedIsoFilter->pc_filterBox)
    {
      getCanInstance4Comm().deleteFilter (*it_managedIsoFilter->s_isoFilter.pc_canCustomer,
                                          it_managedIsoFilter->s_isoFilter.c_identMask.ident(),
                                          it_managedIsoFilter->s_isoFilter.c_identFilter.ident(),
                                          it_managedIsoFilter->s_isoFilter.c_identMask.identType());
      it_managedIsoFilter->pc_filterBox = NULL;
      b_reconfigFilter = true;
    }
  }
  ui8_filtersSetUp=0;

  /// 2nd) Insert all filters possible and use the first one as connected filterbox!
  FilterBox_c* pc_firstFilter = NULL;
  for (ManagedISOFilter_it it_managedIsoFilter = slist_managedISOFilter.begin();
       it_managedIsoFilter != slist_managedISOFilter.end();
       it_managedIsoFilter++)
  {
    // should always be, we erased them all above!
    // if (it_managedIsoFilter->pc_filterBox == NULL)
    {
      Ident_c c_mask   = it_managedIsoFilter->s_isoFilter.c_identMask;
      Ident_c c_filter = it_managedIsoFilter->s_isoFilter.c_identFilter;
      bool b_canCreateFilter = true;

      if (it_managedIsoFilter->s_isoFilter.c_isoNameSa.isSpecified())
      { // see if it's in the monitorlist!
        if (getIsoMonitorInstance4Comm().existIsoMemberISOName (it_managedIsoFilter->s_isoFilter.c_isoNameSa, true))
        { // retrieve current address
          const uint8_t cui8_adr = getIsoMonitorInstance4Comm().isoMemberISOName (it_managedIsoFilter->s_isoFilter.c_isoNameSa).nr();
          c_filter.set (cui8_adr, 0, Ident_c::ExtendedIdent);
          c_mask.set   (0xFF,     0, Ident_c::ExtendedIdent); // open filter for address-byte
        }
        else
        { // can't create the filter - ISOName not claimed on the bus!
          b_canCreateFilter = false;
        }
      }
      if (it_managedIsoFilter->s_isoFilter.c_isoNameDa.isSpecified())
      { // see if it's in the monitorlist!
        if (getIsoMonitorInstance4Comm().existIsoMemberISOName (it_managedIsoFilter->s_isoFilter.c_isoNameDa, true))
        { // retrieve current address
          const uint8_t cui8_adr = getIsoMonitorInstance4Comm().isoMemberISOName (it_managedIsoFilter->s_isoFilter.c_isoNameDa).nr();
          c_filter.set (cui8_adr, 1, Ident_c::ExtendedIdent);
          c_mask.set   (0xFF,     1, Ident_c::ExtendedIdent); // open filter for address-byte
        }
        else
        { // can't create the filter - ISOName not claimed on the bus!
          b_canCreateFilter = false;
        }
      }

      if (b_canCreateFilter)
      {
        FilterBox_c* const cpc_insertedFilter =
          getCanInstance4Comm().insertFilter (*it_managedIsoFilter->s_isoFilter.pc_canCustomer,
                                              c_mask.ident(), c_filter.ident(),
                                              false, c_filter.identType(),
                                              it_managedIsoFilter->s_isoFilter.i8_dlcForce, pc_firstFilter);
        ui8_filtersSetUp++;
        b_reconfigFilter = true;
        it_managedIsoFilter->pc_filterBox = cpc_insertedFilter;
        if (pc_firstFilter == NULL) pc_firstFilter = cpc_insertedFilter;
      }
    }
  }

  if (b_reconfigFilter)
  { // at least one filter deleted/inserted
    getCanInstance4Comm().reconfigureMsgObj();
  }
}


void
ISOFilterBox_c::updateOnAdd (const ISOName_c& rrefc_isoName)
{
  if (ui8_filtersSetUp == slist_managedISOFilter.size())
  { // all filters set up, nothing more to do until "updateOnRemove" comes to play..
    return;
  }

  for (ManagedISOFilter_it it_managedIsoFilter = slist_managedISOFilter.begin();
       it_managedIsoFilter != slist_managedISOFilter.end();
       it_managedIsoFilter++)
  {
    if (it_managedIsoFilter->pc_filterBox == NULL)
    { // There's no filter for this IsoFilter yet, let's see if we could create it!
      bool b_canCreateFilter = true;

      if (it_managedIsoFilter->s_isoFilter.c_isoNameSa.isSpecified())
      { // see if it's in the monitorlist!
        if ( !(
               (it_managedIsoFilter->s_isoFilter.c_isoNameSa == rrefc_isoName)
                ||
               (getIsoMonitorInstance4Comm().existIsoMemberISOName (it_managedIsoFilter->s_isoFilter.c_isoNameSa, true))
              ))
        { // can't create the filter - ISOName not claimed on the bus!
          b_canCreateFilter = false;
        }
      }
      if (it_managedIsoFilter->s_isoFilter.c_isoNameDa.isSpecified())
      { // see if it's in the monitorlist!
        if ( !(
               (it_managedIsoFilter->s_isoFilter.c_isoNameDa == rrefc_isoName)
                ||
               (getIsoMonitorInstance4Comm().existIsoMemberISOName (it_managedIsoFilter->s_isoFilter.c_isoNameDa, true))
              ))
        { // can't create the filter - ISOName not claimed on the bus!
          b_canCreateFilter = false;
        }
      }

      if (b_canCreateFilter)
      { // if we can at least create one filter, then GO!
        syncFiltersToCan();
        return;
      }
    }
  }
}


void
ISOFilterBox_c::updateOnRemove (const ISOName_c& rrefc_isoName)
{
  if (ui8_filtersSetUp == 0)
  { // no filters set up, nothing more to do until "updateOnAdd" comes to play..
    return;
  }

  for (ManagedISOFilter_it it_managedIsoFilter = slist_managedISOFilter.begin();
       it_managedIsoFilter != slist_managedISOFilter.end();
       it_managedIsoFilter++)
  {
    if (it_managedIsoFilter->pc_filterBox)
    { // let's see what we have merged into this filter
      if ( (it_managedIsoFilter->s_isoFilter.c_isoNameSa == rrefc_isoName)
            ||
           (it_managedIsoFilter->s_isoFilter.c_isoNameDa == rrefc_isoName) )
      {
        syncFiltersToCan();
      }
    }
  }
}


} // end of namespace __IsoAgLib
