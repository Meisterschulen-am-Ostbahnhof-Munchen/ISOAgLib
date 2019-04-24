/*
  pc_eeditor_rs232.cpp: source for the PSEUDO BIOS RS232 EEPROM Editor
    module for development and test on a PC

  (C) Copyright 2009 - 2019 by OSB AG

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Usage under Commercial License:
  Licensees with a valid commercial license may use this file
  according to their commercial license agreement. (To obtain a
  commercial license contact OSB AG via <http://isoaglib.com/en/contact>)

  Usage under GNU General Public License with exceptions for ISOAgLib:
  Alternatively (if not holding a valid commercial license)
  use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#include "eeprom.h"
#include <cstdio>

namespace __HAL {

int16_t iCallRs232EEMonitor(void)
{
  printf("iCallRs232EEMonitor())\n");
  return HAL_NO_ERR;
}
} // end namespace __HAL
