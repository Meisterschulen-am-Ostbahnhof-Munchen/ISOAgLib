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
#ifdef USE_OUTPUTS
#  include <supplementary_driver/driver/outputs/ioutputs_c.h>
#endif
#ifdef USE_INPUTS
#  include <supplementary_driver/driver/inputs/iinputs_c.h>
#endif
#ifdef USE_RS232
#  include <supplementary_driver/driver/rs232/irs232io_c.h>
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
  /// Init Core-ISOAgLib (along with System)
  IsoAgLib::getISchedulerInstance().init();

  /// Init Supplementary-ISOAgLib
  #ifdef USE_EEPROM_IO
  IsoAgLib::getIeepromInstance().init();
  #endif
  #ifdef USE_OUTPUTS
  IsoAgLib::getIoutputsInstance().init();
  #endif
  #ifdef USE_INPUTS
  IsoAgLib::getIinputsInstance().init();
  #endif
  #ifdef USE_RS232
  IsoAgLib::getIrs232Instance().init(
      19200,
      IsoAgLib::iRS232IO_c::_8_N_1
  ); // Instance 0
  #endif

  /// Init Application
  ecuMain();

  /// Keep it running...
  while ( IsoAgLib::iSystem_c::canEn() )
    ecuMainLoop();

  /// Shutdown Application
  ecuShutdown();

  /// Shutdown Supplementary-ISOAgLib
  #ifdef USE_RS232
  IsoAgLib::getIrs232Instance().close();
  #endif
  #ifdef USE_INPUTS
  IsoAgLib::getIinputsInstance().close();
  #endif
  #ifdef USE_OUTPUTS
  IsoAgLib::getIoutputsInstance().close();
  #endif
  #ifdef USE_EEPROM_IO
  IsoAgLib::getIeepromInstance().close();
  #endif

  /// Shutdown Core-ISOAgLib
  IsoAgLib::getISchedulerInstance().close();

  return 0;
}

/* eof ecu_main.cpp */
