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
  SetpointRegister_c()
  { init(IsoName_c::ecuTypeANYOTHER, NO_VAL_32S); }

  /**
    initialise this SetpointRegister_c to a well defined starting condition
    @param acrc_isoName device key of commander of this setpoint register set
    @param ai32_value setpoint value
    @param ab_valid true -> this setpoint register instance is accepted as valid
  */
  void init(IsoName_c::ecuType_t ecuType, int32_t ai32_value);

  /** default destructor which has nothing to do */
  ~SetpointRegister_c() {}

  /* ************************************ */
  /* ***reading member variable access*** */
  /* ************************************ */

  /**
    deliver isoName of commanding member
    @return ISOName of setpoint commander
  */
  IsoName_c::ecuType_t isoNameType () const { return m_requestEcuType; }

  /**
    deliver the exact setpoint
    @return exact setpoint value
  */
  int32_t value() const {return mi32_value;}
 
  /* ************************************ */
  /* ***writing member variable access*** */
  /* ************************************ */

  /**
    set isoName of cammanding member
    @param ac_val ISOName of commanding member
  */
  void setISONameType( IsoName_c::ecuType_t ecuType ) {m_requestEcuType = ecuType;}
  /**
    set the exact setpoint value
    @param ai32_val new exact setpoint value
  */
  void setValue(int32_t ai32_val);

private: // Private attributes
  /** setpoint value */
  int32_t mi32_value;

  /** isoName code of requester */
  IsoName_c::ecuType_t m_requestEcuType;

private:
  /** not copyable : copy constructor is only declared, never defined */
  SetpointRegister_c(const SetpointRegister_c&);
  /** not copyable : copy operator is only declared, never defined */
  SetpointRegister_c& operator=(const SetpointRegister_c&); 
};

}
#endif
