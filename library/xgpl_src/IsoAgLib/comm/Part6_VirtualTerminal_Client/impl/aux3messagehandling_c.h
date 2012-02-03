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
};

}
#endif
