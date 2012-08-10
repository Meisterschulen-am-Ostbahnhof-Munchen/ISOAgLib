/*
  setpointregister_c.h: stores one setpoint with management
    informations

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef SETPOINT_REGISTER_H
#define SETPOINT_REGISTER_H

#include <IsoAgLib/isoaglib_config.h>
#include <IsoAgLib/util/config.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isoname_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/processcmd_c.h>

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

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
    @param ai32_value setpoint value
    @param ab_valid true -> this setpoint register instance is accepted as valid
  */
  SetpointRegister_c(const IsoName_c& acrc_isoName = IsoName_c::IsoNameUnspecified(), int32_t ai32_value = NO_VAL_32S,
     bool ab_valid = true)
      {  init(acrc_isoName, ai32_value, ab_valid);}
  /**
    initialise this SetpointRegister_c to a well defined starting condition
    @param acrc_isoName device key of commander of this setpoint register set
    @param ai32_value setpoint value
    @param ab_valid true -> this setpoint register instance is accepted as valid
  */
  void init(const IsoName_c& acrc_isoName = IsoName_c::IsoNameUnspecified(), int32_t ai32_value = NO_VAL_32S,
      bool ab_valid = true);

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
  int32_t value() const {return mi32_exactOrMin;}
 
  /**
    check if setpoint is conformant with actual (!!) master setpoint
    @return true -> the application set this setpoint as valid (accepted)
  */
  bool valid() const {return b_valid;}

  /* ************************************ */
  /* ***writing member variable access*** */
  /* ************************************ */

  /**
    set isoName of cammanding member
    @param ac_val ISOName of commanding member
  */
  void setISOName(const IsoName_c& ac_val){mc_requestISOName = ac_val;}
  /**
    set the exact setpoint value
    @param ai32_val new exact setpoint value
  */
  void setValue(int32_t ai32_val);
  /**
    set the valid state; return if state was changed
    @param ab_state true -> mark this setpoint as valid (accepted)
    @return true -> this call caused a state change for valid state
  */
  bool setValid(bool ab_state = true);

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

  /** isoName code of requester */
  IsoName_c mc_requestISOName;
  
  /** valid state == the setpoint value is actually valid - it is conformant with the master setpoint */
  bool b_valid;
};

}
#endif
