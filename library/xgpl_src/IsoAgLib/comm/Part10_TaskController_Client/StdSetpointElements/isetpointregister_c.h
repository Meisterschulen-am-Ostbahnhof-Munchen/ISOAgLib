/*
  isetpointregister_c.h: stores one setpoint with management
    informations

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef ISETPOINT_REGISTER_H
#define ISETPOINT_REGISTER_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include "impl/setpointregister_c.h"
#include <IsoAgLib/comm/Part5_NetworkManagement/iisoname_c.h>

#include "../iprocesscmd_c.h"

// Begin Namespace IsoAgLib
namespace IsoAgLib {

/**
  stores one setpoint with management informations
  to caller and controlling state
  @author Dipl.-Inform. Achim Spangler
*/
class iSetpointRegister_c : private __IsoAgLib::SetpointRegister_c {
public:
  /* ************************************ */
  /* ***reading member variable access*** */
  /* ************************************ */
  /**
    deliver the exact setpoint
    @return exact setpoint value
  */
  int32_t value()const{return SetpointRegister_c::value();};
  /**
    check if setpoint is conformant with actual (!!) master setpoint
    @return true -> the application set this setpoint as valid (accepted)
  */
  bool valid()const {return SetpointRegister_c::valid();};

  /* ************************************ */
  /* ***writing member variable access*** */
  /* ************************************ */

  /**
    set the exact setpoint value
    @param ai32_val new exact setpoint value
  */
  void setValue(int32_t ai32_val){SetpointRegister_c::setValue(ai32_val);};

  /**
    set the valid state; return if state was changed
    @param ab_state true -> mark this setpoint as valid (accepted)
    @return true -> this call caused a state change for valid state
  */
  bool setValid(bool ab_state = true){return SetpointRegister_c::setValid(ab_state);};
};

}
#endif
