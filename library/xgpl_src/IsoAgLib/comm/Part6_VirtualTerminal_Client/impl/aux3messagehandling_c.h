/*
  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef AUX3_STATE_ENGINE_H
#define AUX3_STATE_ENGINE_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include <IsoAgLib/comm/Part5_NetworkManagement/iidentitem_c.h>
#include "../ivttypes.h"


// Begin Namespace __IsoAgLib
namespace __IsoAgLib {
class Aux3Mgmt_c;

/** utility class for de/encoding of AUX3 messages */
class Aux3MessageHandling_c {
 public:
  Aux3MessageHandling_c(Aux3Mgmt_c& arc_aux3Mgmt) : mrc_aux3Mgmt(arc_aux3Mgmt){};

  /** decode and call suitable functions like Aux3Mgmt_c::processAuxAssignment()
   */
  bool processMsg();

 private:
  /** back reference to Aux3Mgmt_c */
  Aux3Mgmt_c& mrc_aux3Mgmt;

 private:
  /** not copyable : copy constructor is only declared, never defined */
  Aux3MessageHandling_c(const Aux3MessageHandling_c&);
  /** not copyable : copy operator is only declared, never defined */
  Aux3MessageHandling_c& operator=(const Aux3MessageHandling_c&); 
};

}
#endif
