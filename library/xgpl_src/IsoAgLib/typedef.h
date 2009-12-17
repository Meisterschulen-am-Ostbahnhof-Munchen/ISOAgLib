/*
  typedef.h: central typedef

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef __TYPEDEF_H__
#define __TYPEDEF_H__

// include interface aplication relevant configuration settings
#include <IsoAgLib/isoaglib_config.h>
#include <IsoAgLib/hal/config.h>
#include <IsoAgLib/util/compiler_adaptation.h>

/* ****************************************** */
/* * include of typedef header files */
/* ****************************************** */

/* include of system specific typedef headers */
#define _typedef_header_ <IsoAgLib/hal/HAL_SUBDIR/typedef.h>
#include _typedef_header_



#endif
