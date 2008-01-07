/***************************************************************************
                              isofilterbox_c.h
                             -------------------
    begin                : Mon Oct 09 2006
    copyright            : (C) 2000 - 2007 by Dipl.-Inf. Martin Wodok
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
#ifndef ISO_FILTER_BOX_C_H
#define ISO_FILTER_BOX_C_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include <IsoAgLib/typedef.h>
#include <IsoAgLib/driver/can/impl/ident_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isoname_c.h>


// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

// forward declarations
class FilterBox_c;
class CanCustomer_c;

struct IsoFilter_s
{
  // dlcForce == -1: don't check dlc. value of 0..8: force to be exactly this dlc!
  IsoFilter_s (CanCustomer_c& arc_canCustomer, uint32_t aui32_mask, uint32_t aui32_filter, const IsoName_c* apc_isoNameDa = NULL, const IsoName_c* apc_isoNameSa = NULL, int8_t ai8_dlcForce=-1, Ident_c::identType_t at_identType=Ident_c::ExtendedIdent);
  ~IsoFilter_s();

  uint32_t         getMask()      const { return mc_identMask.ident(); }
  uint32_t         getFilter()    const { return mc_identFilter.ident(); }
  const IsoName_c& getIsoNameDa() const { return mc_isoNameDa; }
  const IsoName_c& getIsoNameSa() const { return mc_isoNameSa; }

  /** operator== does INTENTIONALLY NOT compare the dlcForce field as you can't have the same filter for
      the same customer just with another dlcForce! do NEVER do this! */
  bool operator == (const IsoFilter_s& arc_isoFilter) const
  { return equalMaskAndFilter (arc_isoFilter) && (mpc_canCustomer == arc_isoFilter.mpc_canCustomer) /*&& (mi8_dlcForce == arc_isoFilter.mi8_dlcForce) */;}
  bool operator != (const IsoFilter_s& arc_isoFilter) const
  { return !operator == (arc_isoFilter); }

private:
  bool equalMaskAndFilter (const IsoFilter_s& arc_isoFilter) const;

private:
  Ident_c mc_identMask;
  Ident_c mc_identFilter;
  IsoName_c mc_isoNameDa;
  IsoName_c mc_isoNameSa;

  /** Pointer to a CanCustomer_c instance. Assume this like a reference to be always valid! */
  CanCustomer_c* mpc_canCustomer;

  int8_t mi8_dlcForce; // 0..8 to force the DLC, -1 to X (don't care)

  friend class IsoFilterBox_c;
};


class IsoFilterBox_c : public ClientBase
{
public:
  IsoFilterBox_c (const IsoFilter_s& rrefcs_isoFilter SINGLETON_VEC_KEY_PARAMETER_DEF_WITH_COMMA);

  bool hasIsoFilter (const IsoFilter_s& arcs_isoFilter) { return (ms_isoFilter == arcs_isoFilter); }

  bool updateOnAdd    ();
  void updateOnRemove (const IsoName_c* arc_isoName);

  //! Here could come another constructor that takes a variable list of filters and
  //! keeps them all connected. Yet to be done, but not important right now...

private:
  IsoFilter_s ms_isoFilter;
  FilterBox_c* mpc_filterBox;
  Ident_c mc_adaptedIdentMask;
  Ident_c mc_adaptedIdentFilter;
};

/** this typedef is only for some time to provide backward compatibility at API level */
typedef IsoFilter_s ISOFilter_s;

} // End Namespace __IsoAgLib
#endif
