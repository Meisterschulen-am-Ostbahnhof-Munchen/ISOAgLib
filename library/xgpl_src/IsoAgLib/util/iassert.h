
/*
  iassert.h: assertions

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#ifndef IASSERT_H
#define IASSERT_H

#include "compiler_adaptation.h"

#if defined(__GNUC__) || defined(WIN32)
# include <assert.h>
# define isoaglib_assert assert
#elif defined(NDEBUG)
#  define isoaglib_assert(e)
#elif defined(__TSW_CPP__)
/* strings are too expensive => simple assert_fail*/
void assert_fail();
# define isoaglib_assert(e) \
  ((e) ? static_cast< void >(0) : assert_fail())
#else /* support file, line and expression */
void assert_fail(char const *pcs_file, int i_line, char const *pcs_expression);
# define isoaglib_assert(e) \
  ((e) ? static_cast< void >(0) : assert_fail(__FILE__, __LINE__, #e))
#endif

#endif
