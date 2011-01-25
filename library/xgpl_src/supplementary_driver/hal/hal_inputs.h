/*
  hal_inputs.h: include dependent on used target (defined in
    IsoAgLib/isoaglib_config.h) the suitable HAL specific header for
    data input

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

/* ************************************************************ */
/** \file
 *
 * include dependent on used target (defined in
   IsoAgLib/isoaglib_config.h) the suitable HAL
   specific header for data input.
 */
/* ************************************************************ */

/** \file
 *
 * The header <i>\<target\>/\<device\>/\<device\>.h</i> performs a name
   mapping between platform specific BIOS / OS function names
   and the function names, the IsoAgLib uses for hardware access.
   In this header only the groups for error codes, global system
   functions and CAN must be adapted to a new platform.
   If a simple name mapping is not possible for a specific
   function, the module targetExtensions can be used to implement
   the needed activity. The implementation of the ESX version
   can be used to derive the meaning of the specific function.
  */

#ifndef _HAL_INDEPENDENT_INPUTS_H_
#define _HAL_INDEPENDENT_INPUTS_H_

#include <IsoAgLib/isoaglib_config.h>


#ifdef HAL_PATH_SUPPLEMENTARY_INPUTS
#  define _hal_inputs_header_ <HAL_PATH_SUPPLEMENTARY_INPUTS/inputs.h>
#  include _hal_inputs_header_
#endif


#endif
