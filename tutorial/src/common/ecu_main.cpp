/** \example ecu_main.cpp
 * This file implements the common main function
 * which is used in all tutorials.
 *
 * The main-loop will keep ISOAgLib running.
 *
 * The single tutorial-ECUs differ in the
 * implementation of ecuMain() and ecuShutdown() only.
 */

#include <IsoAgLib/driver/can/icanio_c.h>
#include <IsoAgLib/scheduler/ischeduler_c.h>

extern bool ecuMain();
extern bool ecuShutdown();

int main( int /* argc */, char** /*argv*/ )
{
  ecuMain();

  while ( IsoAgLib::iSystem_c::canEn() )
  {
    int32_t i32_idleTimeSpread = 
      IsoAgLib::getISchedulerInstance().timeEvent();

    if ( i32_idleTimeSpread > 0 )
      IsoAgLib::iCanIo_c::waitUntilCanReceiveOrTimeout( i32_idleTimeSpread );
  }

  ecuShutdown();

  return 1;
}

/* eof ecu_main.cpp */
