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
    deliver isoName of commanding member
    @return ISOName of setpoint commander
  */
  const iIsoName_c& isoName()const
  {return SetpointRegister_c::isoName().toConstIisoName_c();};
  /**
    deliver the exact setpoint
    @return exact setpoint value
  */
  int32_t value()const{return SetpointRegister_c::value();};

  /**
    check if setpoint value was already handled
    @return true -> this setpoint was handled by the application
  */
  bool handled()const {return SetpointRegister_c::handled();};
  /**
    check if setpoint is used as master control
    @return true -> the application set this setpoint as master before
  */
  bool master()const {return SetpointRegister_c::master();};
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
    set the handled state; return if state was changed
    @param ab_state true -> mark this setpoint as handled
    @return true -> this call caused a state change for handled state
  */
  bool setHandled(bool ab_state = true){return SetpointRegister_c::setHandled(ab_state);};
  /**
    set the master state; return if state was changed
    @param ab_state true -> mark this setpoint as master
    @return true -> this call caused a state change for master state
  */
  bool setMaster(bool ab_state = true){return SetpointRegister_c::setMaster(ab_state);};
  /**
    set the valid state; return if state was changed
    @param ab_state true -> mark this setpoint as valid (accepted)
    @return true -> this call caused a state change for valid state
  */
  bool setValid(bool ab_state = true){return SetpointRegister_c::setValid(ab_state);};
  /**
    set a limit val for type given by value group
    @param ai32_val new setpoint value
    @param en_valueGroup of setpoint type to set (exact 0, min 2, max 3, default)
  */
  void setValForGroup(int32_t ai32_val, ProcessCmd_c::ValueGroup_t en_valueGroup)
  {
    SetpointRegister_c::setValForGroup(ai32_val, en_valueGroup);
  };
};

}
#endif
