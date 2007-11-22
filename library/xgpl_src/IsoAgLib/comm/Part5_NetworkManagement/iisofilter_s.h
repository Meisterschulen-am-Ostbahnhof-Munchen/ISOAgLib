/***************************************************************************
                          iiso_item.h - object which represents an item
                                           in a iso monitor list
                             -------------------
    begin                : Tue Jan 02 2001
    copyright            : (C) 2001 - 2004 by Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
    type                 : Header
    $LastChangedDate: 2006-10-05 08:36:45 +0200 (Do, 05 Okt 2006) $
    $LastChangedRevision: 2046 $
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
#ifndef IISO_FILTER_H
#define IISO_FILTER_H

#include <IsoAgLib/util/icancustomer_c.h>
#include <IsoAgLib/comm/SystemMgmt/ISO11783/impl/isofiltermanager_c.h>
#include <IsoAgLib/comm/SystemMgmt/ISO11783/iisoname_c.h>

namespace __IsoAgLib
{ // forward declarations
  class ProprietaryMessageHandler_c;
}


namespace IsoAgLib
{ // forward declarations
  class iIdent_c;


struct iIsoFilter_s : private __IsoAgLib::IsoFilter_s
{
  iIsoFilter_s (iCanCustomer_c& arc_canCustomer, uint32_t aui32_mask, uint32_t aui32_filter, const iIsoName_c* apc_isoNameDa = NULL, const iIsoName_c* apc_isoNameSa = NULL, int8_t ai8_dlcForce=-1, iIdent_c::identType_t at_identType=iIdent_c::ExtendedIdent)
    : IsoFilter_s (static_cast<__IsoAgLib::CanCustomer_c&>(arc_canCustomer),
      aui32_mask, aui32_filter,
      apc_isoNameDa, apc_isoNameSa,
      ai8_dlcForce, at_identType) {}

  uint32_t          getMask()      const { return IsoFilter_s::getMask(); }
  uint32_t          getFilter()    const { return IsoFilter_s::getFilter(); }
  const iIsoName_c& getIsoNameDa() const { return static_cast<const iIsoName_c&>(IsoFilter_s::getIsoNameDa()); }
  const iIsoName_c& getIsoNameSa() const { return static_cast<const iIsoName_c&>(IsoFilter_s::getIsoNameSa()); }

  bool operator == (const iIsoFilter_s rrefcs_isoFilter) const { return __IsoAgLib::IsoFilter_s::operator == (rrefcs_isoFilter); }
  bool operator != (const iIsoFilter_s rrefcs_isoFilter) const { return __IsoAgLib::IsoFilter_s::operator != (rrefcs_isoFilter); }

private:
  friend class iIsoFilterManager_c;
  friend class __IsoAgLib::ProprietaryMessageHandler_c;
};

} // End Namespace IsoAgLib
#endif
