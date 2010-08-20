/*
  typedef.h: system dependent basic typedef's

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef TYPEDEF_DJ1_H
#define TYPEDEF_DJ1_H



#if defined(__TSW_CPP__)
#include <cstdlib>
#include <cstddef>
#include <cstring>
typedef unsigned int size_t;

#endif

/*
namespace std
{
 typedef unsigned int size_t;
//  class cout : stream {}


  } // end namespace
*/
#define ON                1
#define OFF               0

#define FALSE false
#define TRUE true


typedef bool boolean;
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

#endif //end HANDLE_FAST_DATATYPE_AS_STRICT


// NOTE: The Tasking can NOT compile this because it does NOT understand what a long long is.
//       At least this was the case with Tasking version 7.5 r 6.
//typedef unsigned long long int uint64_t;
//typedef long long int int64_t;

#endif
