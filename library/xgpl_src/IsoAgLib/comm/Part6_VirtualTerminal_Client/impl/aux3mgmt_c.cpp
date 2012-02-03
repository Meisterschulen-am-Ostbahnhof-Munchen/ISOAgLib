#include "aux3mgmt_c.h"

#include <IsoAgLib/hal/hal_system.h>
#include <IsoAgLib/driver/can/impl/canio_c.h>
#include "isoterminal_c.h"

#if defined(DEBUG) || defined(DEBUG_HEAP_USEAGE)
  #include <supplementary_driver/driver/rs232/impl/rs232io_c.h>
  #include <IsoAgLib/util/impl/util_funcs.h>
  #ifdef SYSTEM_PC
    #include <iostream>
  #else
    #include <supplementary_driver/driver/rs232/impl/rs232io_c.h>
  #endif
#endif

namespace __IsoAgLib {


} // end namespace __IsoAgLib
