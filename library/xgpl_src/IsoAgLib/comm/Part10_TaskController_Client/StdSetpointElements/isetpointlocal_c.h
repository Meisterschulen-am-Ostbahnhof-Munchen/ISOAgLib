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
    check if unhandled new setpoint from actual master was received
    -> check the actual master first before answering the other unhandled
    @see unhandledMaster
    @see acceptNewMaster
    @return true -> master setpoint comanding member sent new setpoint
  */
  bool existUnhandledMaster() {return SetpointLocal_c::existUnhandledMaster();};
  /**
    deliver the new sent setpoint values from the actual master;
    if no unhandled setpoint of the master exist, return the actual accepted value

    possible errors:
        * Err_c::elNonexistent no master setpoint found
    @see existUnhandledMaster
    @see acceptNewMaster
    @return reference to master setpoint register
  */
  iSetpointRegister_c& unhandledMaster()
      {return ((iSetpointRegister_c&)(SetpointLocal_c::unhandledMaster()));};
  /**
    if there exist a new unhandled setpoint from the master setpoint sender;
    accept the new value with this command
    otherwise the old value is still used and answered

    possible errors:
        * Err_c::elNonexistent no master setpoint found
    @see existUnhandledMaster
    @see unhandledMaster
    @param ab_accept true ( default ) -> new setpoint of master is accepted -> send positive notify
  */
  void acceptNewMaster(bool ab_accept = true ) {SetpointLocal_c::acceptNewMaster(ab_accept);};
  /**
    set the master setpoint manually
    (in some cases remote systems request informations
     through process data setpoints)
    @param ai32_val wanted setpoint value
  */
  void setMasterMeasurementVal(int32_t ai32_val)
    {SetpointLocal_c::setMasterMeasurementVal(ai32_val);};

  /**
    deliver the count of unhandled setpoints
    @return count of unhandled received setpoints
  */
  uint8_t unhandledCnt() {return SetpointLocal_c::unhandledCnt();};
  /**
    deliver the aui8_ind of the unhandled setpoints
    @param aui8_ind position of the wanted setpoint entry in list of unhandled setpoints
      (first entry: aui8_ind == 0!!)

    possible errors:
        * Err_c::range there are less than aui8_ind unhandled setpoints found
    @see unhandledCnt
    @return wanted unhandled setpoint
  */
  iSetpointRegister_c& unhandledInd(uint8_t aui8_ind)
    {return ((iSetpointRegister_c&)(SetpointLocal_c::unhandledInd(aui8_ind)));};
  /**
    deliver the first unhandled entry

    possible errors:
        * Err_c::elNonexistent no unhandled stepoint exist
    @see unhandledCnt
    @return reference to first unhandled new received setpoint entry
  */
  iSetpointRegister_c& unhandledFirst()
    {return ((iSetpointRegister_c&)(SetpointLocal_c::unhandledFirst()));};
  /**
    answer all unhandled setpoint enties;
    if new master setpoint was set, send the master
    the new accepted setpoint value;
    send all others NO_VAL_32S

    possible errors:
        * dependant error in ProcessDataLocal_c commander of some setpoint isn't found in Monitor List
        * dependant error in CanIo_c on CAN send problems
  */
  void respondAckNack() {SetpointLocal_c::respondAckNack();};
  /**
    set the allowed percentual deviation between actual measurement and
    existing master setpoint
    @param ab_percent allowed percantual deviation
  */
  void setAllowedDeltaPercent(uint8_t ab_percent)
    {SetpointLocal_c::setAllowedDeltaPercent(ab_percent);};
  /**
    get the allowed percentual deviation between actual measurement and
    existing master setpoint
    @return allowed percantual deviation
  */
  uint8_t allowedDeltaPercent()const
    {return SetpointLocal_c::allowedDeltaPercent();};
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
