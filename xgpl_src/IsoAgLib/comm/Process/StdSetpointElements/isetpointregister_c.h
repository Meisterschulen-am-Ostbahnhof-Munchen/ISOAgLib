/***************************************************************************
                          isetpointregister_c.h  - stores one setpoint with
                                                 management informations
                             -------------------
    begin                : Fri Apr 07 2000
    copyright            : (C) 2000 - 2004 by Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
    type                 : Header
    $LastChangedDate$
    $LastChangedRevision$
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 * This file is part of the "IsoAgLib", an object oriented program library *
 * to serve as a software layer between application specific program and   *
 * communication protocol details. By providing simple function calls for  *
 * jobs like starting a measuring program for a process data value on a    *
 * remote ECU, the main program has not to deal with single CAN telegram   *
 * formatting. This way communication problems between ECU's which use     *
 * this library should be prevented.                                       *
 * Everybody and every company is invited to use this library to make a    *
 * working plug and play standard out of the printed protocol standard.    *
 *                                                                         *
 * Copyright (C) 2000 - 2004 Dipl.-Inform. Achim Spangler                  *
 *                                                                         *
 * The IsoAgLib is free software; you can redistribute it and/or modify it *
 * under the terms of the GNU General Public License as published          *
 * by the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This library is distributed in the hope that it will be useful, but     *
 * WITHOUT ANY WARRANTY; without even the implied warranty of              *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       *
 * General Public License for more details.                                *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with IsoAgLib; if not, write to the Free Software Foundation,     *
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA           *
 *                                                                         *
 * As a special exception, if other files instantiate templates or use     *
 * macros or inline functions from this file, or you compile this file and *
 * link it with other works to produce a work based on this file, this file*
 * does not by itself cause the resulting work to be covered by the GNU    *
 * General Public License. However the source code for this file must still*
 * be made available in accordance with section (3) of the                 *
 * GNU General Public License.                                             *
 *                                                                         *
 * This exception does not invalidate any other reasons why a work based on*
 * this file might be covered by the GNU General Public License.           *
 *                                                                         *
 * Alternative licenses for IsoAgLib may be arranged by contacting         *
 * the main author Achim Spangler by a.spangler@osb-ag:de                  *
 ***************************************************************************/
#ifndef ISETPOINT_REGISTER_H
#define ISETPOINT_REGISTER_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include "impl/setpointregister_c.h"
#include <IsoAgLib/util/igetypos_c.h>
#include "../generalcommand_c.h"

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
    deliver gtp of commanding member
    @return GETY_POS of setpoint commander
  */
  iGetyPos_c gtp()const
  {return SetpointRegister_c::gtp();};
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
    deliver the setpoint according to the mod type
    @param rb_mod MOD code of wanted setpoint (exact 0, min 2, max 3, default)
    @return setpoint selected by MOD
  */
  int32_t valMod(GeneralCommand_c::ValueGroup_t en_valueGroup)const 
  {
    return SetpointRegister_c::valMod(en_valueGroup);
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
    checks if setpoint with type rb_mod exists
    @param rb_mod MOD code of tested setpoint type (exact 0, min 2, max 3, default)
    @return true -> a MOD type setpoint exist
  */
  bool existValMod(GeneralCommand_c::ValueGroup_t en_valueGroup) const {
    return SetpointRegister_c::existValMod(en_valueGroup);
  };


  /* ************************************ */
  /* ***writing member variable access*** */
  /* ************************************ */

  /**
    set gtp of commanding member
    @param rc_gtp GETY_POS of commanding member
  */
  void setGtp(uint8_t rb_val){SetpointRegister_c::setGtp(rb_val);};
  /**
    set the exact setpoint value
    @param ri32_val new exact setpoint value
  */
  void setExact(int32_t ri32_val){SetpointRegister_c::setExact(ri32_val);};
  /**
    set the minimum setpoint value
    @param ri32_val new minimum setpoint value
  */
  void setMin(int32_t ri32_val){SetpointRegister_c::setMin(ri32_val);};
  /**
    set the maximum setpoint value
    @param ri32_val new maximum setpoint value
  */
  void setMax(int32_t ri32_val){SetpointRegister_c::setMax(ri32_val);};
  /**
    set the default setpoint value
    @param ri32_val new default setpoint value
  */
  void setDefault(int32_t ri32_val){SetpointRegister_c::setDefault(ri32_val);};

  /**
    set the handled state; return if state was changed
    @param rb_state true -> mark this setpoint as handled
    @return true -> this call caused a state change for handled state
  */
  bool setHandled(bool rb_state = true){return SetpointRegister_c::setHandled(rb_state);};
  /**
    set the master state; return if state was changed
    @param rb_state true -> mark this setpoint as master
    @return true -> this call caused a state change for master state
  */
  bool setMaster(bool rb_state = true){return SetpointRegister_c::setMaster(rb_state);};
  /**
    set the valid state; return if state was changed
    @param rb_state true -> mark this setpoint as valid (accepted)
    @return true -> this call caused a state change for valid state
  */
  bool setValid(bool rb_state = true){return SetpointRegister_c::setValid(rb_state);};
  /**
    set a limit val for type given by rb_mod
    @param ri32_val new setpoint value
    @param rb_mod MOD code of setpoint type to set (exact 0, min 2, max 3, default)
  */
  void setValMod(int32_t ri32_val, GeneralCommand_c::ValueGroup_t en_valueGroup)
  {
    SetpointRegister_c::setValMod(ri32_val, en_valueGroup);
  };
};

}
#endif
