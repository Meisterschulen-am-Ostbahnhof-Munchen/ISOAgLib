/*
  iiso_item.h: object which represents an item in a iso monitor list

  (C) Copyright 2009 - 2011 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef IISO_FILTER_H
#define IISO_FILTER_H

#include <IsoAgLib/driver/can/icancustomer_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isofiltermanager_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/iisoname_c.h>
#include <IsoAgLib/scheduler/ischedulertask_c.h>

namespace __IsoAgLib
{ // forward declarations
  class ProprietaryMessageHandler_c;
}


namespace IsoAgLib
{ // forward declarations
  class iIdent_c;


struct iIsoFilter_s : private __IsoAgLib::IsoFilter_s
{
  iIsoFilter_s (iCanCustomer_c& arc_canCustomer, const IsoAgLib::iMaskFilter_c& arc_maskFilter, const iIsoName_c* apc_isoNameDa = NULL, const iIsoName_c* apc_isoNameSa = NULL, int8_t ai8_dlcForce=-1 )
    : IsoFilter_s (static_cast<__IsoAgLib::CanCustomer_c&>(arc_canCustomer),
      arc_maskFilter, 
      apc_isoNameDa, apc_isoNameSa,
      ai8_dlcForce ) {}


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
