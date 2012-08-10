/***************************************************************************
                          isetpointlocal_c.h - object for managing setpoints

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef ISETPOINT_LOCAL_H
#define ISETPOINT_LOCAL_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include "impl/setpointlocal_c.h"
#include <IsoAgLib/comm/Part10_TaskController_Client/StdSetpointElements/isetpointregister_c.h>

// Begin Namespace IsoAgLib
namespace IsoAgLib {

/**
  object for managing setpoints commanded from remote to local process data objects;
  handling of received setpoints;
  managing the master setpoint, which is used for controlling;
  help for sending correct notify;
  semi automatic and manual handling of received values
  @author Dipl.-Inform. Achim Spangler
*/
class iSetpointLocal_c : private __IsoAgLib::SetpointLocal_c {
public:

  /**
    set if master setpoint should be preserved even if caller
    isoName is no more active (default false)
    @param ab_static choose if master setpoint should be preserved
          if caller isn't acitve
  */
  void setStaticMaster(bool ab_static = true)
    {SetpointLocal_c::setStaticMaster(ab_static);};
  /**
    check if actual a master setpoint item is defined
    @see master
    @return true -> an active master setpoint exist
  */
  bool existMaster(){return SetpointLocal_c::existMaster();};
  /**
    deliver the actual master entry
    @see existMaster
    @return actual valid master setpoint
  */
  iSetpointRegister_c& master()
    {return ((iSetpointRegister_c&)(SetpointLocal_c::master()));};

  /**
    set the master setpoint manually
    (in some cases remote systems request informations
     through process data setpoints)
    @param ai32_val wanted setpoint value
  */
  void setMasterMeasurementVal(int32_t ai32_val)
    {SetpointLocal_c::setMasterMeasurementVal(ai32_val);};

  /**
    check if the given measuremet value is correct for the actual
    master setpoint;
    @param ai32_val measured value
    @param ab_sendIfError true -> if actual value exceeds setpoint limits
           the actual value is sent as notification (default true)
    @return 0 -> correct; (n<0) -> measurement is delta n to low;
            (n>0) -> measurement is delta n to high
  */
  int32_t checkMeasurement(int32_t ai32_val, bool ab_sendIfError = true)
    {return SetpointLocal_c::checkMeasurement(ai32_val, ab_sendIfError);};
 private:
  friend class iProcDataLocal_c;
};

}
#endif
