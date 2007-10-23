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



/** Handler class which can be used to react on SA claims and esp. conflicts */
class SaClaimHandler_c : public IsoRequestPgnHandler_c
{
 public:
   SaClaimHandler_c() {}
   virtual ~SaClaimHandler_c() {}

   /** this function is called by IsoMonitor_c when a new CLAIMED IsoItem_c is registered.
     * @param rc_isoName const reference to the item which IsoItem_c state is changed
     * @param apc_newItem pointer to the currently corresponding IsoItem_c
     */
   virtual void reactOnMonitorListAdd( const IsoName_c& /*rc_isoName*/, const IsoItem_c* /*apc_newItem*/ ) {};

   /** this function is called by IsoMonitor_c when a device looses its IsoItem_c.
    * @param rc_isoName const reference to the item which IsoItem_c state is changed
    * @param aui8_oldSa previously used SA which is NOW LOST -> clients which were connected to this item can react explicitly
    */
   virtual void reactOnMonitorListRemove( const IsoName_c& /*rc_isoName*/, uint8_t /*aui8_oldSa*/ ) {};
};

}
#endif
