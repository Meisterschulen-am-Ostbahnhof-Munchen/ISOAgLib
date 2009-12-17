/*
  iisofiltermanager_c.h

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
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
      @todo OPTIMIZATION make the intern function return a bool, too. */
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
