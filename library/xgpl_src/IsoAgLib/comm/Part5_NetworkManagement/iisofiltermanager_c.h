/***************************************************************************
                              iisofiltermanager_c.h
                             -------------------
    begin                : Tue Oct 10 2006
    copyright            : (C) 2006 by Dipl.-Inf. Martin Wodok
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
 * Copyright (C) 2000 - 2009 Dipl.-Inform. Achim Spangler                  *
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
#ifndef I_ISO_FILTER_MANAGER_C_H
#define I_ISO_FILTER_MANAGER_C_H

#include "impl/isofiltermanager_c.h"
#include "iisofilter_s.h"

#include <cstdlib>	// Include before vector or else CNAMESPACE stuff is screwed up for Tasking
#include <vector>


// Begin Namespace IsoAgLib
namespace IsoAgLib {

/** this object manages iISO-Filters - those may contain references
    to iISOName's and are initelligent self-adapting can-filters
    @short Manager for handling of inserting/adapting FilterBox_c-instances
    @see __IsoAgLib::IsoFilterManager_c
    @author Dipl.-Inf. Martin Wodok */
class iIsoFilterManager_c : private __IsoAgLib::IsoFilterManager_c
{
public:

  /** @return true on success
      @todo SOON make the intern function return a bool, too. */
  bool insertIsoFilter (iIsoFilter_s const& arcs_isoFilter, bool ab_immReconfigure=true)
  { IsoFilterManager_c::insertIsoFilter (arcs_isoFilter, ab_immReconfigure); return true; }

  void removeIsoFilter (iIsoFilter_s const& arcs_isoFilter)
  { IsoFilterManager_c::removeIsoFilter (arcs_isoFilter); }

  bool existIsoFilter (iIsoFilter_s const& arcs_isoFilter)
  { return IsoFilterManager_c::existIsoFilter (arcs_isoFilter); }


private: // Private methods/attributes
  /** HIDDEN constructor for an iIsoFilterManager_c object instance
    NEVER instantiate a variable of type iIsoMonitor_c within application
    only access iIsoFilterManager_c via getIisoFilterManagerInstance() or
    getIisoFilterManagerInstance (int riLbsBusNr) in case more than one ISO11783 BUS is used for IsoAgLib
    */
  iIsoFilterManager_c() : IsoFilterManager_c() {}

  #if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
  friend iIsoFilterManager_c& getIisoFilterManagerInstance( uint8_t aui8_instance );
  #else
  friend iIsoFilterManager_c& getIisoFilterManagerInstance( void );
  #endif
};

#if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
  /** C-style function, to get access to the unique IsoFilterManager_c singleton instance
      if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS */
  inline iIsoFilterManager_c& getIisoFilterManagerInstance (uint8_t aui8_instance = 0)
  { return static_cast<iIsoFilterManager_c&>(__IsoAgLib::getIsoFilterManagerInstance (aui8_instance)); }
#else
  /** C-style function, to get access to the unique IsoFilterManager_c singleton instance */
  inline iIsoFilterManager_c& getIisoFilterManagerInstance (void)
  { return static_cast<iIsoFilterManager_c&>(__IsoAgLib::getIsoFilterManagerInstance()); }
#endif

}
#endif
