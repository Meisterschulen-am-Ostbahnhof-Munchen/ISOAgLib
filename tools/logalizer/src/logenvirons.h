/*
  logenvirons.h

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#ifndef LOGENVIRONS_H
#define LOGENVIRONS_H


#include <stdlib.h>
#include <cstdint>
#include <iostream>


char getAscii(uint8_t val);

void exit_with_error(const char* error_message);

/** Inline definitions. */

inline char getAscii(uint8_t val)
{
  return ((val >= 0x20) && (val < 0x7F)) ? (char(val)) : '.';
}

inline void exit_with_error(const char* error_message)
{
  std::cerr << error_message << std::endl;
  exit(-1);
}

#endif//ndef LOGENVIRONS_H
