/*
  setpointregister_c.h: stores one setpoint with management
    informations

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef SETPOINT_REGISTER_H
#define SETPOINT_REGISTER_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include <IsoAgLib/typedef.h>
#include <IsoAgLib/util/config.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isoname_c.h>

#include <IsoAgLib/comm/Part7_ProcessData/impl/processcmd_c.h>

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
    @param acrc_isoName device key of commander of this setpoint register set
    @param ai32_exact exact setpoint value
    @param ai32_min minimum setpoint value
    @param ai32_max maximum setpoint value
    @param ai32_default default setpoint value
    @param ab_handled true -> this setpoint register nistance was handled by main application
    @param ab_master true -> this setpoint register instance represents the actual master setpoint
    @param ab_valid true -> this setpoint register instance is accepted as valid
  */
  SetpointRegister_c(const IsoName_c& acrc_isoName = IsoName_c::IsoNameUnspecified(), int32_t ai32_exact = NO_VAL_32S,
      int32_t ai32_min = NO_VAL_32S, int32_t ai32_max = NO_VAL_32S, int32_t ai32_default = NO_VAL_32S,
      bool ab_handled = false, bool ab_master = false, bool ab_valid = true)
      {  init(acrc_isoName, ai32_exact, ai32_min, ai32_max, ai32_default, ab_handled, ab_master, ab_valid);}
  /**
    initialise this SetpointRegister_c to a well defined starting condition
    @param acrc_isoName device key of commander of this setpoint register set
    @param ai32_exact exact setpoint value
    @param ai32_min minimum setpoint value
    @param ai32_max maximum setpoint value
    @param ai32_default default setpoint value
    @param ab_handled true -> this setpoint register nistance was handled by main application
    @param ab_master true -> this setpoint register instance represents the actual master setpoint
    @param ab_valid true -> this setpoint register instance is accepted as valid
  */
  void init(const IsoName_c& acrc_isoName = IsoName_c::IsoNameUnspecified(), int32_t ai32_exact = NO_VAL_32S,
      int32_t ai32_min = NO_VAL_32S, int32_t ai32_max = NO_VAL_32S, int32_t ai32_default = NO_VAL_32S,
      bool ab_handled = false, bool ab_master = false, bool ab_valid = true);

  /**
    operator= for SetpointRegister_c class
    @param acrc_src source SetpointRegister_c instance
    @return reference to source instance for cmd like "setp1 = setp2 = setp3;"
  */
  const SetpointRegister_c& operator=(const SetpointRegister_c& acrc_src);
  /**
    copy constructor for SetpointRegister_c class
    @param acrc_src source SetpointRegister_c instance
  */
  SetpointRegister_c(const SetpointRegister_c& acrc_src);

  /** default destructor which has nothing to do */
  ~SetpointRegister_c();

  /**
    compare two Setpoint Register items by value
    @param acrc_src compared instance
    @return true -> both setpoint sets are equal
  */
  bool operator==(const SetpointRegister_c& acrc_src)const;

  /* ************************************ */
  /* ***reading member variable access*** */
  /* ************************************ */

  /**
    deliver isoName of commanding member
    @return ISOName of setpoint commander
  */
  const IsoName_c& isoName()const{return mc_requestISOName;}
  /**
    deliver the exact setpoint
    @return exact setpoint value
  */
  int32_t exact()const{return mi32_exactOrMin;}

  /**
    deliver the minimum limit; if no min is given (~0) return mi32_exactOrMin
    @return minimum setpoint value
  */
  int32_t min()const{return mi32_exactOrMin;}
  /**
    deliver the maximum limit ; if no max is given (~0) return mi32_exactOrMin
    @return maximum setpoint value
  */
  int32_t max()const{return (existMax())?(mi32_max):(mi32_exactOrMin);}
  /**
    deliver the default limit ; if no default is given (~0) return mi32_exactOrMin
    name: getDefault() because default() doesn't compile
    @return default setpoint value
  */
  int32_t getDefault()const{return (existDefault())?(mi32_default):(mi32_exactOrMin);}
  /**
    deliver the setpoint according to the value group
    @param en_valueGroup code of wanted setpoint (exact 0, min 2, max 3, default)
    @return setpoint selected by value group
  */
  int32_t valForGroup(ProcessCmd_c::ValueGroup_t en_valueGroup)const;

  /**
    check if setpoint value was already handled
    @return true -> this setpoint was handled by the application
  */
  bool handled()const{return (data.b_handled == 1);}
  /**
    deliver the timestamp of the last setHandled event
    @return last setHandled timestamp
  */
  int32_t lastHandledTime()const{return mi32_lastHandledTime;}
  /**
    check if setpoint is used as master control
    @return true -> the application set this setpoint as master before
  */
  bool master()const{return (data.b_master == 1);}
  /**
    check if setpoint is conformant with actual (!!) master setpoint
    @return true -> the application set this setpoint as valid (accepted)
  */
  bool valid()const{return (data.b_valid == 1);}
  /**
    check if valid exact limit is set
    @return true -> this setpoint register instance has an exact setpoint value
  */
  bool existExact()const{return ((data.en_definedSetpoints & exactType) != 0);}
  /**
    check if valid minimum limit is set
    @return true -> this setpoint register instance has an minimum setpoint value
  */
  bool existMin()const{return ((data.en_definedSetpoints & minType) != 0);}
  /**
    check if valid maximum limit is set
    @return true -> this setpoint register instance has an maximum setpoint value
  */
  bool existMax()const{return ((data.en_definedSetpoints & maxType) != 0);}
  /**
    check if valid default value is set
    @return true -> this setpoint register instance has a default setpoint value
  */
  bool existDefault()const{return ((data.en_definedSetpoints & defaultType) != 0);}
  /**
    checks if setpoint with value group en_valueGroup exists
    @param en_valueGroup value group of tested setpoint type (exact 0, min 2, max 3, default)
    @return true -> a setpoint for this valueGroup exists
  */
  bool valueGroupExists(ProcessCmd_c::ValueGroup_t en_valueGroup)const;


  /* ************************************ */
  /* ***writing member variable access*** */
  /* ************************************ */

  /**
    set isoName of cammanding member
    @param ac_val ISOName of commanding member
  */
  void setISOName(const IsoName_c& ac_val){mc_requestISOName = ac_val;}
  /**
    set isoName of cammanding member
    @param aui8_devClass device class of commanding member
    @param aui8_devClassInst device class instance of commanding member
  */
  void setISOName(uint8_t aui8_devClass, uint8_t aui8_devClassInst){mc_requestISOName.set( aui8_devClass, aui8_devClassInst );}
  /**
    set the exact setpoint value
    @param ai32_val new exact setpoint value
  */
  void setExact(int32_t ai32_val);
  /**
    set the minimum setpoint value
    @param ai32_val new minimum setpoint value
  */
  void setMin(int32_t ai32_val);
  /**
    set the maximum setpoint value
    @param ai32_val new maximum setpoint value
  */
  void setMax(int32_t ai32_val);
  /**
    set the default setpoint value
    @param ai32_val new default setpoint value
  */
  void setDefault(int32_t ai32_val);

  /**
    set the handled state; return if state was changed
    @param ab_state true -> mark this setpoint as handled
    @param ai32_handledTime timestamp for detecting the last setHandle event
    @return true -> this call caused a state change for handled state
  */
  bool setHandled(bool ab_state = true, int32_t ai32_handledTime = -1);
  /**
    set the master state; return if state was changed
    @param ab_state true -> mark this setpoint as master
    @return true -> this call caused a state change for master state
  */
  bool setMaster(bool ab_state = true);
  /**
    set the valid state; return if state was changed
    @param ab_state true -> mark this setpoint as valid (accepted)
    @return true -> this call caused a state change for valid state
  */
  bool setValid(bool ab_state = true);
  /**
    set a limit val for type given by value group
    @param ai32_val new setpoint value
    @param en_valueGroup of setpoint type to set (exact 0, min 2, max 3, default)
  */
  void setValForGroup(int32_t ai32_val, ProcessCmd_c::ValueGroup_t en_valueGroup);

private: // Private methods
  /** base function for assignment of element vars for copy constructor and operator= */
  void assignFromSource( const SetpointRegister_c& acrc_src );

private: // Private attributes
  /** minimum limit setpoint */
  int32_t mi32_exactOrMin;
  /** maximum limit setpoint */
  int32_t mi32_max;
  /** default setpoint */
  int32_t mi32_default;
  /** tiemstamp of last setXx operation */
  int32_t mi32_lastHandledTime;

  /** isoName code of requester */
  IsoName_c mc_requestISOName;

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
