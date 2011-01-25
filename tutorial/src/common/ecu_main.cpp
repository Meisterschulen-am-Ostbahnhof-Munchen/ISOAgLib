/** \example ecu_main.cpp
 * This file implements the common main function
 * which is used in all tutorials.
 *
 * The main-loop will keep ISOAgLib running.
 *
 * The single tutorial-ECUs differ in the
 * implementation of ecuMain() and ecuShutdown() only.
 */

#include <IsoAgLib/scheduler/ischeduler_c.h>
#ifdef USE_EEPROM_IO
#  include <supplementary_driver/driver/eeprom/ieepromio_c.h>
#endif

extern bool ecuMain();
extern bool ecuShutdown();


void ecuMainLoop()
{
  int32_t i32_idleTimeSpread =
    IsoAgLib::getISchedulerInstance().timeEvent();

  if ( i32_idleTimeSpread > 0 )
  {
    IsoAgLib::getISchedulerInstance().waitUntilCanReceiveOrTimeout( i32_idleTimeSpread );
  }
}


int main( int /* argc */, char** /*argv*/ )
{
  /// Init Core-ISOAgLib (along with System, Actor, Sensor, RS232)
  IsoAgLib::getISchedulerInstance().init();

  /// Init Supplementary-ISOAgLib (EEPROM for IdentItem)
  #ifdef USE_EEPROM_IO
  IsoAgLib::getIeepromInstance().init();
  #endif

  /// Init Application
  ecuMain();

  /// Keep it running...
  while ( IsoAgLib::iSystem_c::canEn() )
    ecuMainLoop();

  /// Shutdown Application
  ecuShutdown();

  /// Shutdown Supplementary-ISOAgLib (EEPROM for IdentItem)
  #ifdef USE_EEPROM_IO
  IsoAgLib::getIeepromInstance().close();
  #endif

  /// Shutdown Core-ISOAgLib
  IsoAgLib::getISchedulerInstance().close();

  return 0;
}

/* eof ecu_main.cpp */
