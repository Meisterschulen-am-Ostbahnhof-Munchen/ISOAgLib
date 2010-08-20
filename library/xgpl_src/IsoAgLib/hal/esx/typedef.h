/*
  typedef.h:  system dependent basic typedef's

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef TYPEDEF_ESX_H
#define TYPEDEF_ESX_H

// for Tasking 7.x EDE these includes have to be included FIRST
// because they are included by the broken stl-headers into global namespace
#if defined(__TSW_CPP__)
#include <cstdlib>
#include <cstddef>
#include <cstring>
typedef unsigned int size_t;
#endif

typedef unsigned char uint8_t;
typedef unsigned char byte;
typedef char int8_t;
typedef unsigned int uint16_t;
typedef int int16_t;
typedef unsigned long uint32_t;
typedef long int32_t;


#ifdef HANDLE_FAST_DATATYPE_AS_STRICT

/**  SIZEOF_INT = 2 */
/* Signed.  */
typedef signed char     int_fast8_t;
typedef int             int_fast16_t;
typedef long            int_fast32_t;
//typedef long long int   int_fast64_t;

/* Unsigned.  */
typedef unsigned char           uint_fast8_t;
typedef unsigned int            uint_fast16_t;
typedef unsigned long           uint_fast32_t;
//typedef unsigned long long int  uint_fast64_t;
#endif // end  HANDLE_FAST_DATATYPE_AS_STRICT


// NOTE: The C2C can NOT compile this because it does NOT understand what a long long is.
//       At least this was the case with Tasking version 7.5 r 6.
//typedef unsigned long long int uint64_t;
//typedef long long int int64_t;

#endif
