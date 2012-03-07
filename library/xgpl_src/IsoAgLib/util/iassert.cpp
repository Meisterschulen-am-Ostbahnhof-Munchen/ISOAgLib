/*
  iassert.cpp: assertions

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#if defined(__GNUC__) || defined(WIN32)
// nothing to do: provided by standard libraries
#elif !defined(NEBUG)

#include "iassert.h"
#include <stdlib.h>

#if defined(__TSW_CPP__)
void assert_fail()
{
  abort();
}
#else
void assert_fail(char const *pcs_file, int i_line, char const *pcs_expression)
{
  abort();
}
#endif

#endif
