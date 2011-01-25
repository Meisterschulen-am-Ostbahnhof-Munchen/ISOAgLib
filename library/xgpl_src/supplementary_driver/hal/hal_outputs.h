/*
  hal_outputs.h: include dependent on used target (defined in
    IsoAgLib/isoaglib_config.h) the suitable HAL specific header for
    output control

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#ifndef _HAL_INDEPENDENT_OUTPUTS_H_
#define _HAL_INDEPENDENT_OUTPUTS_H_

#include <IsoAgLib/isoaglib_config.h>


#ifdef HAL_PATH_SUPPLEMENTARY_OUTPUTS
#  define _hal_outputs_header_ <HAL_PATH_SUPPLEMENTARY_OUTPUTS/outputs.h>
#  include _hal_outputs_header_
#endif


#endif
