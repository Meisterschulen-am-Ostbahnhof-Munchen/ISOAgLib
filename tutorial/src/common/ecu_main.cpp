/*
 * file:    main.cpp
 * created: 02/20/09 04:57:55 PM CET
 *
 * This file implements the common main,
 * which is used in all tutorials
 *
 * The single tutorial-ECUs differ in the
 * implementation of ecuMain() and ecuShutdown() only.
 *
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

/* eof main.cpp */
