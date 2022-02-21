/*
  iinputs_c.h - header for the inputs management object

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
#ifndef IINPUTS_C_H
#define IINPUTS_C_H

#include "impl/inputs_c.h"


namespace IsoAgLib {

/**
  Management of the Inputs in general.
  Currently "empty", but do init/close nevertheless!
  @see iAnalogI_c
  @see iDigitalI_c
  @see iCounterI_c
  */
class iInputs_c : private __IsoAgLib::Inputs_c
{
public:
  void init() { Inputs_c::init(); }
  void close() { Inputs_c::close(); }

private:
  // unimplemented, shouldn't be called by app.
  virtual ~iInputs_c();

  /** allow getIinputsInstance() access to shielded base class */
  friend iInputs_c& getIinputsInstance();
};

/** C-style function, to get access to the unique iInputs_c singleton instance */
inline iInputs_c& getIinputsInstance() { return static_cast<iInputs_c&>(__IsoAgLib::getInputsInstance()); }

} // IsoAgLib

#endif
