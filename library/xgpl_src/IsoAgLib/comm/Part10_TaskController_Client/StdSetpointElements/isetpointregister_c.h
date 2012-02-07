/*
  isetpointregister_c.h: stores one setpoint with management
    informations

  (C) Copyright 2009 - 2011 by OSB AG and developing partners

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
  int32_t exact()const{return SetpointRegister_c::exact();};
  /**
    deliver the minimum limit; if no min is given (~0) return i32_exact
    @return minimum setpoint value
  */
  int32_t min()const{return SetpointRegister_c::min();};
  /**
    deliver the maximum limit; if no max is given (~0) return i32_exact
    @return maximum setpoint value
  */
  int32_t max()const{return SetpointRegister_c::max();};
  /**
    deliver the default setpoint value; if no default is given (~0) return i32_exact
    @return default setpoint value
  */
  int32_t getDefault()const{return SetpointRegister_c::getDefault();};
  /**
    deliver the setpoint according to the value group
    @param en_valueGroup of wanted setpoint (exact 0, min 2, max 3, default)
    @return setpoint selected by value group
  */
  int32_t valForGroup(ProcessCmd_c::ValueGroup_t en_valueGroup)const
  {
    return SetpointRegister_c::valForGroup(en_valueGroup);
  };

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
  /**
    check if valid exact limit is set
    @return true -> this setpoint register instance has an exact setpoint value
  */
  bool existExact()const {return SetpointRegister_c::existExact();};
  /**
    check if valid minimum limit is set
    @return true -> this setpoint register instance has an minimum setpoint value
  */
  bool existMin()const {return SetpointRegister_c::existMin();};
  /**
    check if valid maximum limit is set
    @return true -> this setpoint register instance has an maximum setpoint value
  */
  bool existMax()const {return SetpointRegister_c::existMax();};
  /**
    check if valid default value is set
    @return true -> this setpoint register instance has an default setpoint value
  */
  bool existDefault()const {return SetpointRegister_c::existDefault();};
  /**
    checks if setpoint with value group en_valueGroup exists
    @param en_valueGroup value group of tested setpoint type (exact 0, min 2, max 3, default)
    @return true -> a setpoint for this valueGroup exists
  */
  bool valueGroupExists(ProcessCmd_c::ValueGroup_t en_valueGroup) const {
    return SetpointRegister_c::valueGroupExists(en_valueGroup);
  };


  /* ************************************ */
  /* ***writing member variable access*** */
  /* ************************************ */

  /**
    set the exact setpoint value
    @param ai32_val new exact setpoint value
  */
  void setExact(int32_t ai32_val){SetpointRegister_c::setExact(ai32_val);};
  /**
    set the minimum setpoint value
    @param ai32_val new minimum setpoint value
  */
  void setMin(int32_t ai32_val){SetpointRegister_c::setMin(ai32_val);};
  /**
    set the maximum setpoint value
    @param ai32_val new maximum setpoint value
  */
  void setMax(int32_t ai32_val){SetpointRegister_c::setMax(ai32_val);};
  /**
    set the default setpoint value
    @param ai32_val new default setpoint value
  */
  void setDefault(int32_t ai32_val){SetpointRegister_c::setDefault(ai32_val);};

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
