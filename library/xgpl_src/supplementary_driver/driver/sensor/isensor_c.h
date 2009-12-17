/*
  isensor.h
    header file for iSensor_c, a central object for enum types used in
    all sensor objects

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef ISENSOR_H
#define ISENSOR_H

#include "impl/sensor_c.h"

// Begin Namespace IsoAgLib
namespace IsoAgLib {

/**class for all enum types in relation to iSensor_I
  @see iSensor_I
  @see iAnalog_I
  @see iDigital_I
  @author Dipl.-Inform. Achim Spangler
  */
class iSensor_c : public __IsoAgLib::Sensor_c {
  public:
  /** constructor with no function, because this object is only needed for its enum types */
  iSensor_c() : Sensor_c(){};
  /** destructor with no function */
  ~iSensor_c() {};
};
}

#endif
