/*
  digitali_c.cpp:
    implementation file for DigitalI_c, an object for digital input

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#include "digitali_c.h"
#include "inputs_c.h"

#include <IsoAgLib/util/config.h>
#include <IsoAgLib/util/iliberr_c.h>

#if DEBUG_INPUTS
#  include <supplementary_driver/driver/rs232/impl/rs232io_c.h>
#endif


namespace IsoAgLib {
/** function to handle a DigitalI_c event from HAL
  * @param aui8_channel channel of the input object, which received the IRQ
  *        from HAL
  */
void iInputEventHandler::handleDigitalEvent( uint8_t )
{
  // nop in base-class
}

};

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

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


#ifdef __TSW_CPP__
static IsoAgLib::iInputEventHandler* ppc_handler[16];
#else
IsoAgLib::iInputEventHandler* DigitalI_c::ppc_handler[16];
#endif


void
DigitalI_c::handleHalIrqEvent( uint8_t aui8_channel ) {
  if ( ppc_handler[aui8_channel] != NULL )
  {
    ppc_handler[aui8_channel]->handleDigitalEvent( aui8_channel );
  }
}


DigitalI_c::DigitalI_c(uint8_t ab_channel, onoff_t ren_onoff, bool ab_static, IsoAgLib::iInputEventHandler* apc_handler)
  : InputBase_c(ab_channel, digital), b_static(ab_static), en_onoff(ren_onoff) {
  if ( ab_channel != 0xFF )init( ab_channel, ren_onoff, ab_static, apc_handler );
}


void
DigitalI_c::init(uint8_t ab_channel, onoff_t ren_onoff, bool ab_static, IsoAgLib::iInputEventHandler* apc_handler)
{ // now init the digital input
  InputBase_c::init(ab_channel, digital);

  ;
  if ( ( apc_handler != NULL ) && ( ab_channel < 16 ) ) {
    // register pointer to IRQ handler
    const int16_t i16_initResult = HAL::init_digin(channelNr(), BOTH_EDGE, ren_onoff,digitalInputIrqFuncArr[ab_channel] );
	isoaglib_assert( ! i16_initResult ); (void)i16_initResult;
  }
  else {
    const int16_t i16_initResult = HAL::init_digin(channelNr(), DIGIN, ren_onoff, NULL);
	isoaglib_assert( ! i16_initResult ); (void)i16_initResult;
  }

  b_static = ab_static;

  
  getInputsInstance().registerClient( this );
  // register optional pointer to handler
  if ( ab_channel < 16 ) ppc_handler[ab_channel] = apc_handler;
  #if DEBUG_INPUTS
  if ( apc_handler != NULL )
   INTERNAL_DEBUG_DEVICE << "DigitalI_c::init() zu Channel: " << uint16_t(ab_channel) << " mit IRQ Handler" << INTERNAL_DEBUG_DEVICE_ENDL;
  #endif
}


void DigitalI_c::setOnHigh( void )
{
  if ( channelNr() < 16 ) {
    // register pointer to IRQ handler
    const int16_t i16_initResult = HAL::init_digin(channelNr(), DIGIN, OnHigh, digitalInputIrqFuncArr[channelNr()] );
	isoaglib_assert( ! i16_initResult ); (void)i16_initResult;
  }
  else
  {
    const int16_t i16_initResult = HAL::init_digin(channelNr(), DIGIN, OnHigh, NULL);
	isoaglib_assert( ! i16_initResult ); (void)i16_initResult;
  }
  
}


void DigitalI_c::setOnLow( void )
{
  
  if ( channelNr() < 16 ) {
    // register pointer to IRQ handler
    const int16_t i16_initResult = HAL::init_digin(channelNr(), DIGIN, OnLow, digitalInputIrqFuncArr[channelNr()] );
	isoaglib_assert( ! i16_initResult ); (void)i16_initResult;
  }
  else
  {
    const int16_t i16_initResult = HAL::init_digin(channelNr(), DIGIN, OnLow, NULL);
	isoaglib_assert( ! i16_initResult ); (void)i16_initResult;
   }
}


DigitalI_c::~DigitalI_c()
{
  // unregister element from Inputs_c
  getInputsInstance().unregisterClient( this );
}


bool
DigitalI_c::active() const
{
  int16_t i16_val = val();
  if (i16_val == HAL_RANGE_ERR)
  { // wrong input channel
    return false;
  }
  return (i16_val == ON);
}


uint16_t
DigitalI_c::val()const
{
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
    return uint16_t( ERROR_VAL_16S ); // needs to be fixed!
  }
  else
  { // correct input channel
    return (i16_val == ON)?1:0;
  }
}

} // end of namespace __IsoAgLib
