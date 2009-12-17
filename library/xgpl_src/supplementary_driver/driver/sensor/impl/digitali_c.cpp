/*
  digitali_c.cpp:
    implementation file for DigitalI_c, an object for digital sensor
    input

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#include "digitali_c.h"
#include "sensori_c.h"
#include <IsoAgLib/util/config.h>

#ifdef DEBUG
#include <supplementary_driver/driver/rs232/impl/rs232io_c.h>
#endif

namespace IsoAgLib {
/** function to handle a DigitalI_c event from HAL
  * @param aui8_channel channel of the input object, which received the IRQ
  *        from HAL
  */
void iSensorEventHandler::handleDigitalEvent( uint8_t /* aui8_channel is intentionally unused in this baseclass version */ ) {
}

};

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

#ifndef __TSW_CPP__
 #define IRQ_FUNC_PAR ...
#else
 #define IRQ_FUNC_PAR
#endif

void digitalInputIrqFunc_0(IRQ_FUNC_PAR){DigitalI_c::handleHalIrqEvent(0);}
void digitalInputIrqFunc_1(IRQ_FUNC_PAR){DigitalI_c::handleHalIrqEvent(1);}
void digitalInputIrqFunc_2(IRQ_FUNC_PAR){DigitalI_c::handleHalIrqEvent(2);}
void digitalInputIrqFunc_3(IRQ_FUNC_PAR){DigitalI_c::handleHalIrqEvent(3);}
void digitalInputIrqFunc_4(IRQ_FUNC_PAR){DigitalI_c::handleHalIrqEvent(4);}
void digitalInputIrqFunc_5(IRQ_FUNC_PAR){DigitalI_c::handleHalIrqEvent(5);}
void digitalInputIrqFunc_6(IRQ_FUNC_PAR){DigitalI_c::handleHalIrqEvent(6);}
void digitalInputIrqFunc_7(IRQ_FUNC_PAR){DigitalI_c::handleHalIrqEvent(7);}
void digitalInputIrqFunc_8(IRQ_FUNC_PAR){DigitalI_c::handleHalIrqEvent(8);}
void digitalInputIrqFunc_9(IRQ_FUNC_PAR){DigitalI_c::handleHalIrqEvent(9);}
void digitalInputIrqFunc_10(IRQ_FUNC_PAR){DigitalI_c::handleHalIrqEvent(10);}
void digitalInputIrqFunc_11(IRQ_FUNC_PAR){DigitalI_c::handleHalIrqEvent(11);}
void digitalInputIrqFunc_12(IRQ_FUNC_PAR){DigitalI_c::handleHalIrqEvent(12);}
void digitalInputIrqFunc_13(IRQ_FUNC_PAR){DigitalI_c::handleHalIrqEvent(13);}
void digitalInputIrqFunc_14(IRQ_FUNC_PAR){DigitalI_c::handleHalIrqEvent(14);}
void digitalInputIrqFunc_15(IRQ_FUNC_PAR){DigitalI_c::handleHalIrqEvent(15);}

typedef void (*irqFuncFunction)(IRQ_FUNC_PAR);

static irqFuncFunction digitalInputIrqFuncArr[16] =
{&digitalInputIrqFunc_0, &digitalInputIrqFunc_1, &digitalInputIrqFunc_2, &digitalInputIrqFunc_3, &digitalInputIrqFunc_4, &digitalInputIrqFunc_5,
 &digitalInputIrqFunc_6, &digitalInputIrqFunc_7, &digitalInputIrqFunc_8, &digitalInputIrqFunc_9, &digitalInputIrqFunc_10, &digitalInputIrqFunc_11,
 &digitalInputIrqFunc_12, &digitalInputIrqFunc_13, &digitalInputIrqFunc_14, &digitalInputIrqFunc_15};

/** array of pointers to handler for IRQ events */
#ifdef __TSW_CPP__
static IsoAgLib::iSensorEventHandler* ppc_handler[16];
#else
IsoAgLib::iSensorEventHandler* DigitalI_c::ppc_handler[16];
#endif

/** handler function which is called from HAL */
void DigitalI_c::handleHalIrqEvent( uint8_t aui8_channel ) {
  if ( ppc_handler[aui8_channel] != NULL )
  {
    ppc_handler[aui8_channel]->handleDigitalEvent( aui8_channel );
  }
}


/**
  internal called constructor for a new digital input channel which performs configuration of hardware
  (uses BIOS function)

  possible errors:
      * iLibErr_c::Range wrong input number
  @see SensorI_c::createDigital
  @see Sensor_c::t_onoff
  @param ab_channel default-argument for setting hardware channel for this input
  @param ren_onoff default-argument for setting whether 1 should be returned on High(Default: Sensor_c::OnHigh) or Low signal
  @param ab_static default-argument for setting if hardware input should be gathered static (default false with no static)
  @param apc_handler optional pointer to handler class, which can be called, if an HAL irq event occurs
*/
DigitalI_c::DigitalI_c(uint8_t ab_channel, Sensor_c::onoff_t ren_onoff, bool ab_static, IsoAgLib::iSensorEventHandler* apc_handler)
  : SensorBase_c(ab_channel, Sensor_c::digital), b_static(ab_static), en_onoff(ren_onoff) {
  if ( ab_channel != 0xFF )init( ab_channel, ren_onoff, ab_static, apc_handler );
}
/**
  internal called constructor for a new digital input channel which performs configuration of hardware
  (uses BIOS function)

  possible errors:
      * iLibErr_c::Range wrong input number
  @see SensorI_c::createDigital
  @see Sensor_c::t_onoff
  @param ab_channel default-argument for setting hardware channel for this input
  @param ren_onoff default-argument for setting whether 1 should be returned on High(Default: Sensor_c::OnHigh) or Low signal
  @param ab_static default-argument for setting if hardware input should be gathered static (default false with no static)
  @param apc_handler optional pointer to handler class, which can be called, if an HAL irq event occurs
*/
void DigitalI_c::init(uint8_t ab_channel, Sensor_c::onoff_t ren_onoff, bool ab_static, IsoAgLib::iSensorEventHandler* apc_handler)
{ // now init the digital input
  SensorBase_c::init(ab_channel, Sensor_c::digital);

  int16_t i16_initResult;
  if ( ( apc_handler != NULL ) && ( ab_channel < 16 ) )
    // register pointer to IRQ handler
    i16_initResult = HAL::init_digin(channelNr(), BOTH_EDGE, ren_onoff,digitalInputIrqFuncArr[ab_channel] );
  else
    i16_initResult = HAL::init_digin(channelNr(), DIGIN, ren_onoff, NULL);

  b_static = ab_static;

  if (i16_initResult)
  { // wrong input channel no
    getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::Sensor );
  }
  else
  { // correct input channel no - now register the valid new analog input into SensorI_c
    getSensorInstance().registerClient( this );
    // register optional pointer to handler
    if ( ab_channel < 16 ) ppc_handler[ab_channel] = apc_handler;
    #ifdef DEBUG
    if ( apc_handler != NULL )
     INTERNAL_DEBUG_DEVICE << "DigitalI_c::init() zu Channel: " << uint16_t(ab_channel) << " mit IRQ Handler" << INTERNAL_DEBUG_DEVICE_ENDL;
    #endif
  }
}
/** change detection mode of activity to OnHigh */
void DigitalI_c::setOnHigh( void )
{
  int16_t i16_initResult;
  if ( channelNr() < 16 )
    // register pointer to IRQ handler
    i16_initResult = HAL::init_digin(channelNr(), DIGIN, Sensor_c::OnHigh, digitalInputIrqFuncArr[channelNr()] );
  else
    i16_initResult = HAL::init_digin(channelNr(), DIGIN, Sensor_c::OnHigh, NULL);

  if (i16_initResult)
  { // wrong input channel no
    getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::Sensor );
  }
}
/** change detection mode of activity to OnLow */
void DigitalI_c::setOnLow( void )
{
  int16_t i16_initResult;
  if ( channelNr() < 16 )
    // register pointer to IRQ handler
    i16_initResult = HAL::init_digin(channelNr(), DIGIN, Sensor_c::OnLow, digitalInputIrqFuncArr[channelNr()] );
  else
    i16_initResult = HAL::init_digin(channelNr(), DIGIN, Sensor_c::OnLow, NULL);

  if (i16_initResult)
  { // wrong input channel no
    getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::Sensor );
  }
}

/**  destructor of the input object which can close explicit the hardware input */
DigitalI_c::~DigitalI_c(){
  // unregister element from SensorI_c
  getSensorInstance().unregisterClient( this );
}
/**
  check for the input value (uses BIOS function)

  possible errors:
      * iLibErr_c::Range wrong input number
  @return true for (High signal AND ren_onoff==OnHigh)(Default!) or (Low signal AND ren_onoff==OnLow); otherwise false
*/
bool DigitalI_c::active() const {
  int16_t i16_val = val();
  if (i16_val == HAL_RANGE_ERR)
  { // wrong input channel
    return false;
  }
  return (i16_val == ON);
}
/**
  check for the input value (uses BIOS function)
  possible errors:
      * iLibErr_c::Range wrong input number
  @return 1 for (High signal AND ren_onoff==OnHigh)(Default!) or (Low signal AND ren_onoff==OnLow); otherwise 0
*/
int16_t DigitalI_c::val()const{
  int16_t i16_val;
  if (b_static)
  { // static was forced
     i16_val = HAL::getDiginOnoffStatic(channelNr());
  }
  else
  { // normal
     i16_val = HAL::getDiginOnoff(channelNr());
  }
  if (i16_val == HAL_RANGE_ERR)
  { // wrong input channel
    getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::Sensor );
    return ERROR_VAL_16S;
  }
  else
  { // correct input channel
    return (i16_val == ON)?1:0;
  }
}

} // end of namespace __IsoAgLib
