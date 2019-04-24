/*
  outputs_c.h - header for the outputs management object

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
#ifndef OUTPUTS_C_H
#define OUTPUTS_C_H

#include <IsoAgLib/util/impl/singleton.h>
#include "digitalo_c.h"


namespace __IsoAgLib {

/**
  Hardware dependent object for hardware independent getting of output data;
  */
class Outputs_c
{
public:
  void init() {}
  void close() {}

  /** control the relay which is responsible for activation of the PWM output */
  void setMainRelais( bool ab_active ); 

private:
  // for singleton only
  Outputs_c() {}
  ~Outputs_c() {}

private:
  friend Outputs_c &getOutputsInstance();
};


Outputs_c &
getOutputsInstance();

}

#endif
