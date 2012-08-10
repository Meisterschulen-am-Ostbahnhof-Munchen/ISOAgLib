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

private:
  friend class iProcDataLocal_c;
};

}
#endif
