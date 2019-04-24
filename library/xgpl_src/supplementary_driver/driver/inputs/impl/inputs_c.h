/*
  inputs_c.h - header for the inputs management object

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
#ifndef INPUTS_C_H
#define INPUTS_C_H


namespace __IsoAgLib {

/**
  Class for management of Inputs.
  Currently unsused, but should be initialized/closed nevertheless!
  */
class Inputs_c {
public:
  void init() {}
  void close() {}

private:
  // only for singleton
  Inputs_c() {}
  ~Inputs_c() {}

private:
  friend Inputs_c &getInputsInstance();
};


Inputs_c &
getInputsInstance();


} // __IsoAgLib

#endif
