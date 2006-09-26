/***************************************************************************
                          setpointregister_c.h  - stores one setpoint with
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

 /**************************************************************************
 *                                                                         *
 *     ###    !!!    ---    ===    IMPORTANT    ===    ---    !!!    ###   *
 * Each software module, which accesses directly elements of this file,    *
 * is considered to be an extension of IsoAgLib and is thus covered by the *
 * GPL license. Applications must use only the interface definition out-   *
 * side :impl: subdirectories. Never access direct elements of __IsoAgLib  *
 * and __HAL namespaces from applications which shouldnt be affected by    *
 * the license. Only access their interface counterparts in the IsoAgLib   *
 * and HAL namespaces. Contact a.spangler@osb-ag:de in case your applicat- *
 * ion really needs access to a part of an internal namespace, so that the *
 * interface might be extended if your request is accepted.                *
 *                                                                         *
 * Definition of direct access:                                            *
 * - Instantiation of a variable with a datatype from internal namespace   *
 * - Call of a (member-) function                                          *
 * Allowed is:                                                             *
 * - Instatiation of a variable with a datatype from interface namespace,  *
 *   even if this is derived from a base class inside an internal namespace*
 * - Call of member functions which are defined in the interface class     *
 *   definition ( header )                                                 *
 *                                                                         *
 * Pairing of internal and interface classes:                              *
 * - Internal implementation in an :impl: subdirectory                     *
 * - Interface in the parent directory of the corresponding internal class *
 * - Interface class name IsoAgLib::iFoo_c maps to the internal class      *
 *   __IsoAgLib::Foo_c                                                     *
 *                                                                         *
 * AS A RULE: Use only classes with names beginning with small letter :i:  *
 ***************************************************************************/
#ifndef SETPOINT_REGISTER_H
#define SETPOINT_REGISTER_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include <IsoAgLib/typedef.h>
#include <IsoAgLib/util/config.h>
#include <IsoAgLib/comm/SystemMgmt/ISO11783/impl/isoname_c.h>

#include <IsoAgLib/comm/Process/impl/generalcommand_c.h>

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

/** enum for specification of defined setpoint type */
enum definedSetpoints_t { exactType = 1, minType = 4, maxType = 8,
                          minMaxType = 0xC, exactMinType = 5, exactMaxType = 9,
                          defaultType = 0x10, minMaxDefaultType = 0x1C, exactMaxDefaultType = 0x19,
                          exactMinDefaultType = 0x15, exactMinMaxType = 0xD,
                          maxDefaultType = 0x18, minDefaultType = 0x14};

/**
  stores one setpoint with management informations
  to caller and controlling state
  @author Dipl.-Inform. Achim Spangler
*/
class SetpointRegister_c {
public:
  /**
    constructor which can set all element variables (all parameters are optional)
    @param rc_isoName device key of commander of this setpoint register set
    @param ri32_exact exact setpoint value
    @param ri32_min minimum setpoint value
    @param ri32_max maximum setpoint value
    @param ri32_default default setpoint value
    @param rb_handled true -> this setpoint register nistance was handled by main application
    @param rb_master true -> this setpoint register instance represents the actual master setpoint
    @param rb_valid true -> this setpoint register instance is accepted as valid
  */
  SetpointRegister_c(const ISOName_c& rc_isoName = ISOName_c::ISONameUnspecified, int32_t ri32_exact = NO_VAL_32S,
      int32_t ri32_min = NO_VAL_32S, int32_t ri32_max = NO_VAL_32S, int32_t ri32_default = NO_VAL_32S,
      bool rb_handled = false, bool rb_master = false, bool rb_valid = true)
      {  init(rc_isoName, ri32_exact, ri32_min, ri32_max, ri32_default, rb_handled, rb_master, rb_valid);}
  /**
    initialise this SetpointRegister_c to a well defined starting condition
    @param rc_isoName device key of commander of this setpoint register set
    @param ri32_exact exact setpoint value
    @param ri32_min minimum setpoint value
    @param ri32_max maximum setpoint value
    @param ri32_default default setpoint value
    @param rb_handled true -> this setpoint register nistance was handled by main application
    @param rb_master true -> this setpoint register instance represents the actual master setpoint
    @param rb_valid true -> this setpoint register instance is accepted as valid
  */
  void init(const ISOName_c& rc_isoName = ISOName_c::ISONameUnspecified, int32_t ri32_exact = NO_VAL_32S,
      int32_t ri32_min = NO_VAL_32S, int32_t ri32_max = NO_VAL_32S, int32_t ri32_default = NO_VAL_32S,
      bool rb_handled = false, bool rb_master = false, bool rb_valid = true);

  /**
    operator= for SetpointRegister_c class
    @param rrefc_src source SetpointRegister_c instance
    @return reference to source instance for cmd like "setp1 = setp2 = setp3;"
  */
  const SetpointRegister_c& operator=(const SetpointRegister_c& rrefc_src);
  /**
    copy constructor for SetpointRegister_c class
    @param rrefc_src source SetpointRegister_c instance
  */
  SetpointRegister_c(const SetpointRegister_c& rrefc_src);

  /** default destructor which has nothing to do */
  ~SetpointRegister_c();

  /**
    compare two Setpoint Register items by value
    @param rrefc_src compared instance
    @return true -> both setpoint sets are equal
  */
  bool operator==(const SetpointRegister_c& rrefc_src)const;

  /* ************************************ */
  /* ***reading member variable access*** */
  /* ************************************ */

  /**
    deliver isoName of commanding member
    @return ISOName of setpoint commander
  */
  const ISOName_c& isoName()const{return c_requestISOName;}
  /**
    deliver the exact setpoint
    @return exact setpoint value
  */
  int32_t exact()const{return i32_exactOrMin;}
#ifdef USE_FLOAT_DATA_TYPE
  /**
    deliver the exact setpoint
    @return exact setpoint value
  */
  float exactFloat()const{return f_exactOrMin;}
#endif
  /**
    deliver the minimum limit; if no min is given (~0) return i32_exactOrMin
    @return minimum setpoint value
  */
  int32_t min()const{return i32_exactOrMin;}
  /**
    deliver the maximum limit ; if no max is given (~0) return i32_exactOrMin
    @return maximum setpoint value
  */
  int32_t max()const{return (existMax())?(i32_max):(i32_exactOrMin);}
  /**
    deliver the default limit ; if no default is given (~0) return i32_exactOrMin
    name: getDefault() because default() doesn't compile
    @return default setpoint value
  */
  int32_t getDefault()const{return (existDefault())?(i32_default):(i32_exactOrMin);}
  /**
    deliver the setpoint according to the mod type
    @param en_valueGroup code of wanted setpoint (exact 0, min 2, max 3, default)
    @return setpoint selected by MOD
  */
  int32_t valMod(GeneralCommand_c::ValueGroup_t en_valueGroup)const;
#ifdef USE_FLOAT_DATA_TYPE
  /**
    deliver the minimum limit; if no min is given (~0) return f_exactOrMin
    @return minimum setpoint value
  */
  float minFloat()const{return f_exactOrMin;}
  /**
    deliver the maximum limit; if no max is given (~0) return f_exactOrMin
    @return maximum setpoint value
  */
  float maxFloat()const{return (existMax())?(f_max):(f_exactOrMin);}
  /**
    deliver the default value; if no default is given (~0) return f_exactOrMin
    @return default setpoint value
  */
  float defaultFloat()const{return (existDefault())?(f_default):(f_exactOrMin);}
  /**
    deliver the setpoint according to the mod type
    @param en_valueGroup code of wanted setpoint (exact 0, min 2, max 3, default)
    @return setpoint selected by MOD
  */
  float valModFloat(GeneralCommand_c::ValueGroup_t en_valueGroup)const;
#endif
  /**
    check if setpoint value was already handled
    @return true -> this setpoint was handled by the application
  */
  bool handled()const{return (data.b_handled == 1)?true:false;}
  /**
    deliver the timestamp of the last setHandled event
    @return last setHandled timestamp
  */
  int32_t lastHandledTime()const{return i32_lastHandledTime;}
  /**
    check if setpoint is used as master control
    @return true -> the application set this setpoint as master before
  */
  bool master()const{return (data.b_master == 1)?true:false;}
  /**
    check if setpoint is conformant with actual (!!) master setpoint
    @return true -> the application set this setpoint as valid (accepted)
  */
  bool valid()const{return (data.b_valid == 1)?true:false;}
  /**
    check if valid exact limit is set
    @return true -> this setpoint register instance has an exact setpoint value
  */
  bool existExact()const{return ((data.en_definedSetpoints & exactType) != 0)?true:false;}
  /**
    check if valid minimum limit is set
    @return true -> this setpoint register instance has an minimum setpoint value
  */
  bool existMin()const{return ((data.en_definedSetpoints & minType) != 0)?true:false;}
  /**
    check if valid maximum limit is set
    @return true -> this setpoint register instance has an maximum setpoint value
  */
  bool existMax()const{return ((data.en_definedSetpoints & maxType) != 0)?true:false;}
  /**
    check if valid default value is set
    @return true -> this setpoint register instance has a default setpoint value
  */
  bool existDefault()const{return ((data.en_definedSetpoints & defaultType) != 0)?true:false;}
  /**
    checks if setpoint with type rb_mod exists
    @param en_valueGroup value group of tested setpoint type (exact 0, min 2, max 3, default)
    @return true -> a MOD type setpoint exist
  */
  bool existValMod(GeneralCommand_c::ValueGroup_t en_valueGroup)const;


  /* ************************************ */
  /* ***writing member variable access*** */
  /* ************************************ */

  /**
    set isoName of cammanding member
    @param rc_isoName ISOName of commanding member
  */
  void setISOName(const ISOName_c& rc_val){c_requestISOName = rc_val;}
  /**
    set isoName of cammanding member
    @param rc_isoName ISOName of commanding member
  */
  void setISOName(uint8_t rui8_devClass, uint8_t rui8_devClassInst){c_requestISOName.set( rui8_devClass, rui8_devClassInst );}
  /**
    set the exact setpoint value
    @param ri32_val new exact setpoint value
  */
  void setExact(int32_t ri32_val);
  /**
    set the minimum setpoint value
    @param ri32_val new minimum setpoint value
  */
  void setMin(int32_t ri32_val);
  /**
    set the maximum setpoint value
    @param ri32_val new maximum setpoint value
  */
  void setMax(int32_t ri32_val);
  /**
    set the default setpoint value
    @param ri32_val new default setpoint value
  */
  void setDefault(int32_t ri32_val);
#ifdef USE_FLOAT_DATA_TYPE
  /**
    set the exact setpoint value
    @param rf_val new exact setpoint value
  */
  void setExact(float rf_val);
  /**
    set the minimum setpoint value
    @param rf_val new minimum setpoint value
  */
  void setMin(float rf_val);
  /**
    set the maximum setpoint value
    @param rf_val new maximum setpoint value
  */
  void setMax(float rf_val);
  /**
    set the default setpoint value
    @param rf_val new default setpoint value
  */
  void setDefault(float rf_val);
  /**
    set a limit val for type given by rb_mod
    @param rf_val new setpoint value
    @param en_valueGroup code of setpoint type to set (exact 0, min 2, max 3, default)
  */
  void setValMod(float rf_val, GeneralCommand_c::ValueGroup_t en_valueGroup);
#endif
  /**
    set the handled state; return if state was changed
    @param rb_state true -> mark this setpoint as handled
    @param ri32_handledTime timestamp for detecting the last setHandle event
    @return true -> this call caused a state change for handled state
  */
  bool setHandled(bool rb_state = true, int32_t ri32_handledTime = -1);
  /**
    set the master state; return if state was changed
    @param rb_state true -> mark this setpoint as master
    @return true -> this call caused a state change for master state
  */
  bool setMaster(bool rb_state = true);
  /**
    set the valid state; return if state was changed
    @param rb_state true -> mark this setpoint as valid (accepted)
    @return true -> this call caused a state change for valid state
  */
  bool setValid(bool rb_state = true);
  /**
    set a limit val for type given by rb_mod
    @param ri32_val new setpoint value
    @param en_valueGroup code of setpoint type to set (exact 0, min 2, max 3, default)
  */
  void setValMod(int32_t ri32_val, GeneralCommand_c::ValueGroup_t en_valueGroup);

private: // Private methods
  /** base function for assignment of element vars for copy constructor and operator= */
  void assignFromSource( const SetpointRegister_c& rrefc_src );
private: // Private attributes
#ifdef USE_FLOAT_DATA_TYPE
  union {
    /** minimum limit setpoint */
    int32_t i32_exactOrMin;
    float f_exactOrMin;
  };
  union {
    /** maximum limit setpoint */
    int32_t i32_max;
    float f_max;
  };
  union {
    /** default setpoint */
    int32_t i32_default;
    float f_default;
  };
#else
  /** minimum limit setpoint */
  int32_t i32_exactOrMin;
  /** maximum limit setpoint */
  int32_t i32_max;
  /** default setpoint */
  int32_t i32_default;
#endif
  /** tiemstamp of last setXx operation */
  int32_t i32_lastHandledTime;
  /** isoName code of requester */
  ISOName_c c_requestISOName;
  struct {
    /** master state == the setpoint requester can change the value if needed */
    bool b_master : 1;
    /** valid state == the setpoint value is actually valid - it is conformant with the master setpoint */
    bool b_valid : 1;
    /** handled false state == this setpoint wasn't checked by the main application */
    bool b_handled : 1;
    /** defined setpoint types */
    definedSetpoints_t en_definedSetpoints : 5;
  } data;
};

}
#endif
