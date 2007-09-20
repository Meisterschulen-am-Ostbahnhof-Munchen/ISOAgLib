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


namespace __IsoAgLib {


/** default destructor which has nothing to do */
ISOFilterManager_c::~ISOFilterManager_c ()
{
//  close();
}


/** constructor for ISORequestPGN_c */
ISOFilterManager_c::ISOFilterManager_c ()
  : SingletonISOFilterManager_c ()
{
// functionality moved OUT of the constructor, as the constructor is NOT called in embedded systems for static class instances.
}


/** initialize directly after the singleton instance is created.
  * this is called from singleton.h and should NOT be called from the user again.
  * users please use init(...) instead. */
void
ISOFilterManager_c::singletonInit ()
{
  b_alreadyInitialized = false; // so init() will init!
}


void
ISOFilterManager_c::init()
{
  if (!b_alreadyInitialized)
  { // avoid double initialization. for now now close needed, only init once! ==> see Scheduler_c::startupSystem()
    b_alreadyInitialized = true;
    // register to get ISO monitor list changes
    __IsoAgLib::getIsoMonitorInstance4Comm().registerSaClaimHandler( this );
  }
}


//! Checks WITH CANCustomer!
bool
ISOFilterManager_c::existIsoFilter (const ISOFilter_s& rrefcs_isoFilter)
{
  for (ISOFilterBox_it it_isoFilterBox = vec_isoFilterBox.begin();
       it_isoFilterBox != vec_isoFilterBox.end();
       it_isoFilterBox++)
  { // Search for existing ISOFilterBox
    if (it_isoFilterBox->hasIsoFilterWithCustomer (rrefcs_isoFilter))
    { // This ISOFilterBox already has such a filter
      return true;
    }
  }
  return false;
}


void
ISOFilterManager_c::insertIsoFilter (const ISOFilter_s& rrefcs_isoFilter)
{
  // Check if ISOFilter does yet exist in some ISOFilterBox
  if (!existIsoFilter (rrefcs_isoFilter))
  { // insert an empty ISOFilterBox. initialized then in list right after
    vec_isoFilterBox.push_back (ISOFilterBox_c (SINGLETON_VEC_KEY));

    // now get the inserted ISOFilterBox
    ISOFilterBox_c& refc_isoFilterBox = vec_isoFilterBox.back();

    // add the filter(s)
    refc_isoFilterBox.addIsoFilter (rrefcs_isoFilter);

    // retrigger update of real hardware filters
    refc_isoFilterBox.syncFiltersToCan();
  }
}


/** @todo use vararg list somewhen! */
void
ISOFilterManager_c::insertIsoFilterConnected (const ISOFilter_s& rrefcs_isoFilter, const ISOFilter_s& rrefcs_isoFilter2)
{
  // Check if ISOFilter does yet exist in some ISOFilterBox
  if ( (!existIsoFilter (rrefcs_isoFilter)) && (!existIsoFilter (rrefcs_isoFilter2)) )
  { // insert an empty ISOFilterBox. initialized then in list right after
    vec_isoFilterBox.push_back (ISOFilterBox_c (SINGLETON_VEC_KEY));

    // now get the inserted ISOFilterBox
    ISOFilterBox_c& refc_isoFilterBox = vec_isoFilterBox.back();

    // add the filter(s)
    refc_isoFilterBox.addIsoFilter (rrefcs_isoFilter);
    refc_isoFilterBox.addIsoFilter (rrefcs_isoFilter2);

    // retrigger update of real hardware filters
    refc_isoFilterBox.syncFiltersToCan();
  }
}


bool
ISOFilterManager_c::addToIsoFilter (const ISOFilter_s& rrefcs_isoFilterExisting, const ISOFilter_s& rrefcs_isoFilterToAdd)
{
  for (ISOFilterBox_it it_isoFilterBox = vec_isoFilterBox.begin();
       it_isoFilterBox != vec_isoFilterBox.end();
       it_isoFilterBox++)
  { // Search for existing ISOFilterBox
    if (it_isoFilterBox->hasIsoFilterWithoutCustomer (rrefcs_isoFilterExisting))
    { // Add ISOFilter to existing ISOFilterBox

      // if filter not yet there
      if (!(it_isoFilterBox->hasIsoFilterWithCustomer (rrefcs_isoFilterToAdd)))
      { // add the filter
        it_isoFilterBox->addIsoFilter (rrefcs_isoFilterToAdd);
      }
      // maybe add some more... use variable argument list like printf?

      it_isoFilterBox->syncFiltersToCan();
      return true;
    }
  }

  // Existing ISOFilterBox not found, can't add ISOFilter
  return false;
}


bool
ISOFilterManager_c::removeIsoFilter (const ISOFilter_s& rrefcs_isoFilter)
{
  for (ISOFilterBox_it it_isoFilterBox = vec_isoFilterBox.begin();
       it_isoFilterBox != vec_isoFilterBox.end();
       it_isoFilterBox++)
  { // Search for existing ISOFilterBox
    switch (it_isoFilterBox->removeIsoFilter (rrefcs_isoFilter))
    {
      case ISOFilterBox_c::RemoveAnswerFailureNonExistent: break;
      case ISOFilterBox_c::RemoveAnswerSuccessBoxEmpty:
        it_isoFilterBox = vec_isoFilterBox.erase(it_isoFilterBox);
        // break; left out intentionally
      case ISOFilterBox_c::RemoveAnswerSuccessBoxNotEmpty:
        return true;
    }
  }

  // ISOFilterBox not existing, can't remove!
  return false;
}

/** this function is called by ISOMonitor_c when a new CLAIMED ISOItem_c is registered.
  * @param refc_isoName const reference to the item which ISOItem_c state is changed
  * @param rpc_newItem pointer to the currently corresponding ISOItem_c
  */
void
ISOFilterManager_c::reactOnMonitorListAdd (const ISOName_c& refc_isoName, const ISOItem_c* /*rpc_newItem*/)
{
  for (ISOFilterBox_it it_isoFilterBox = vec_isoFilterBox.begin();
       it_isoFilterBox != vec_isoFilterBox.end();
       it_isoFilterBox++)
  { // the ISOFilterBoxes will take care if they have to do anything at all or not...
    it_isoFilterBox->updateOnAdd (refc_isoName);
  }
}

/** this function is called by ISOMonitor_c when a device looses its ISOItem_c.
* @param refc_isoName const reference to the item which ISOItem_c state is changed
* @param rui8_oldSa previously used SA which is NOW LOST -> clients which were connected to this item can react explicitly
*/
void
ISOFilterManager_c::reactOnMonitorListRemove (const ISOName_c& refc_isoName, uint8_t /*rui8_oldSa*/)
{
  for (ISOFilterBox_it it_isoFilterBox = vec_isoFilterBox.begin();
       it_isoFilterBox != vec_isoFilterBox.end();
       it_isoFilterBox++)
  { // the ISOFilterBoxes will take care if they have to do anything at all or not...
    it_isoFilterBox->updateOnRemove (refc_isoName);
  }
}


#if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
/** C-style function, to get access to the unique ISOFilterManager_c singleton instance
 * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS */
ISOFilterManager_c& getIsoFilterManagerInstance (uint8_t rui8_instance)
{ // if > 1 singleton instance is used, no static reference can be used
  return ISOFilterManager_c::instance(rui8_instance);
};
#else
/** C-style function, to get access to the unique ISOFilterManager_c singleton instance */
ISOFilterManager_c& getIsoFilterManagerInstance (void)
{
  static ISOFilterManager_c& c_isoFilterManager = ISOFilterManager_c::instance ();
  return c_isoFilterManager;
};
#endif

} // end of namespace __IsoAgLib
