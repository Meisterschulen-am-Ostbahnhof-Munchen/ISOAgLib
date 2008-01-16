/***************************************************************************
                            saclaimhandler_c.h
                             -------------------
    begin                : Thu May 16 2006
    copyright            : (C) 2000 - 2004 by Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
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

#ifndef SA_CLAIM_HANDLER_C_H
#define SA_CLAIM_HANDLER_C_H

#include <IsoAgLib/typedef.h>
#include "isorequestpgnhandler_c.h"

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */

namespace __IsoAgLib {



/** Handler class which can be used to react on all actions depending on address-claims.
 *  Actions that can take place are listed in the class-embdedded enum.
 */
class SaClaimHandler_c : public IsoRequestPgnHandler_c
{
public:

  typedef enum IsoItemAction_en
  { /// ATTENTION: If a node is added and "AddToMonitorList" is broadcast,
    /// ~~~~~~~~~  the node doesn't need to have a valid Address at this moment!!
    AddToMonitorList,        /// Initial Address-Claim --> Node added to IsoMonitor
    ChangedAddress,          /// Changed Address
    LostAddress,             /// Address stolen due to Address-Claim of other node with Same SA (New Address is 254)
    ReclaimedAddress,        /// Reclaimed an Address after Address was stolen (Old Address was 254)
    RemoveFromMonitorList    /// No more Address-Claims (after requests) --> Node removed from IsoMonitor
  } IsoItemModification_t;

   SaClaimHandler_c() {}
   virtual ~SaClaimHandler_c() {}

   /** this function is called by IsoMonitor_c on addition, state-change and removal of an IsoItem.
     <!-- @param at_action enumeration indicating what happened to this IsoItem. @see IsoItemModification_en / IsoItemModification_t
      @param arcc_isoItem reference to the (const) IsoItem which is changed (by existance or state)-->
     */
   virtual void reactOnIsoItemModification (IsoItemModification_t /*at_action*/, IsoItem_c const& /*arcc_isoItem*/) {}
};

}
#endif
