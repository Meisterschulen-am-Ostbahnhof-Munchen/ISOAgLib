/***************************************************************************
                          digital_i.cc  -  implementation file for DigitalI_c,
                                           an object for digital sensor input
                             -------------------
    begin                : Mon Oct 25 1999
    copyright            : (C) 1999 - 2004 by Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
    type                 : Source
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 * This file is part of the "IsoAgLib", an object oriented program library *
 * to serve as a software layer between application specific program and   *
 * communication protocol details. By providing simple function calls for  *
 * jobs like starting a measuring program for a process data value on a    *
 * remote ECU, the main program has not to deal with single CAN telegram   *
 * formatting. This way communication problems between ECU's which use     *
 * this library should be prevented.                                       *
 * Everybody and every company is invited to use this library to make a    *
 * working plug and play standard out of the printed protocol standard.    *
 *                                                                         *
 * Copyright (C) 1999 - 2004 Dipl.-Inform. Achim Spangler                  *
 *                                                                         *
 * The IsoAgLib is free software; you can redistribute it and/or modify it *
 * under the terms of the GNU General Public License as published          *
 * by the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This library is distributed in the hope that it will be useful, but     *
 * WITHOUT ANY WARRANTY; without even the implied warranty of              *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       *
 * General Public License for more details.                                *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with IsoAgLib; if not, write to the Free Software Foundation,     *
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA           *
 *                                                                         *
 * As a special exception, if other files instantiate templates or use     *
 * macros or inline functions from this file, or you compile this file and *
 * link it with other works to produce a work based on this file, this file*
 * does not by itself cause the resulting work to be covered by the GNU    *
 * General Public License. However the source code for this file must still*
 * be made available in accordance with section (3) of the                 *
 * GNU General Public License.                                             *
 *                                                                         *
 * This exception does not invalidate any other reasons why a work based on*
 * this file might be covered by the GNU General Public License.           *
 *                                                                         *
 * Alternative licenses for IsoAgLib may be arranged by contacting         *
 * the main author Achim Spangler by a.spangler@osb-ag:de                  * 
 ***************************************************************************/ 

 /**************************************************************************
 *                                                                         * 
 *     ###    !!!    ---    ===    IMPORTANT    ===    ---    !!!    ###   * 
 * Each software module, which accesses directly elements of this file,    * 
 * is considered to be an extension of IsoAgLib and is thus covered by the * 
 * GPL license. Applications must use only the interface definition out-   * 
 * side :impl: subdirectories. Never access direct elements of __IsoAgLib  * 
 * and __HAL namespaces from applications which shouldnt be affected by    * 
 * the license. Only access their interface counterparts in the IsoAgLib   * 
 * and HAL namespaces. Contact a.spangler@osb-ag:de in case your applicat- * 
 * ion really needs access to a part of an internal namespace, so that the * 
 * interface might be extended if your request is accepted.                * 
 *                                                                         * 
 * Definition of direct access:                                            * 
 * - Instantiation of a variable with a datatype from internal namespace   * 
 * - Call of a (member-) function                                          * 
 * Allowed is:                                                             * 
 * - Instatiation of a variable with a datatype from interface namespace,  * 
 *   even if this is derived from a base class inside an internal namespace* 
 * - Call of member functions which are defined in the interface class     * 
 *   definition ( header )                                                 * 
 *                                                                         * 
 * Pairing of internal and interface classes:                              * 
 * - Internal implementation in an :impl: subdirectory                     * 
 * - Interface in the parent directory of the corresponding internal class * 
 * - Interface class name IsoAgLib::iFoo_c maps to the internal class      * 
 *   __IsoAgLib::Foo_c                                                     * 
 *                                                                         * 
 * AS A RULE: Use only classes with names beginning with small letter :i:  *
 ***************************************************************************/
#include "digitali_c.h"
#include "sensori_c.h"
#include <IsoAgLib/util/config.h>

#ifdef DEBUG
#include <supplementary_driver/driver/rs232/impl/rs232io_c.h>
#endif

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {
/** function to handle a DigitalI_c event from HAL
  * @param rui8_channel channel of the input object, which received the IRQ
  *        from HAL
  */
void SensorEventHandler::handleDigitalEvent( uint8_t /* rui8_channel is intentionally unused in this baseclass version */ ) {
}

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
static SensorEventHandler* ppc_handler[16];
#else
SensorEventHandler* DigitalI_c::ppc_handler[16];
#endif

/** handler function which is called from HAL */
void DigitalI_c::handleHalIrqEvent( uint8_t rui8_channel ) {
  if ( ppc_handler[rui8_channel] != NULL )
  {
    ppc_handler[rui8_channel]->handleDigitalEvent( rui8_channel );
  }
}


/**
  internal called constructor for a new digital input channel which performs configuration of hardware
  (uses BIOS function)

  possible errors:
      * LibErr_c::Range wrong input number
  @see SensorI_c::createDigital
  @see Sensor_c::t_onoff
  @param rb_channel default-argument for setting hardware channel for this input
  @param ren_onoff default-argument for setting whether 1 should be returned on High(Default: Sensor_c::OnHigh) or Low signal
  @param rb_static default-argument for setting if hardware input should be gathered static (default false with no static)
  @param rpc_handler optional pointer to handler class, which can be called, if an HAL irq event occurs
*/
DigitalI_c::DigitalI_c(uint8_t rb_channel, Sensor_c::onoff_t ren_onoff, bool rb_static, SensorEventHandler* rpc_handler)
  : SensorBase_c(rb_channel, Sensor_c::digital), b_static(rb_static), en_onoff(ren_onoff) {
  if ( rb_channel != 0xFF )init( rb_channel, ren_onoff, rb_static, rpc_handler );
}
/**
  internal called constructor for a new digital input channel which performs configuration of hardware
  (uses BIOS function)

  possible errors:
      * LibErr_c::Range wrong input number
  @see SensorI_c::createDigital
  @see Sensor_c::t_onoff
  @param rb_channel default-argument for setting hardware channel for this input
  @param ren_onoff default-argument for setting whether 1 should be returned on High(Default: Sensor_c::OnHigh) or Low signal
  @param rb_static default-argument for setting if hardware input should be gathered static (default false with no static)
  @param rpc_handler optional pointer to handler class, which can be called, if an HAL irq event occurs
*/
void DigitalI_c::init(uint8_t rb_channel, Sensor_c::onoff_t ren_onoff, bool rb_static, SensorEventHandler* rpc_handler)
{ // now init the digital input
  SensorBase_c::init(rb_channel, Sensor_c::digital);

  int16_t i16_initResult;
  if ( ( rpc_handler != NULL ) && ( rb_channel < 16 ) )
    // register pointer to IRQ handler
    i16_initResult = HAL::init_digin(channelNr(), BOTH_EDGE, ren_onoff,digitalInputIrqFuncArr[rb_channel] );
  else
    i16_initResult = HAL::init_digin(channelNr(), DIGIN, ren_onoff, NULL);

  b_static = rb_static;

  if (i16_initResult)
  { // wrong input channel no
    getLbsErrInstance().registerError( LibErr_c::Range, LibErr_c::Sensor );
  }
  else
  { // correct input channel no - now register the valid new analog input into SensorI_c
    getSensorInstance().registerClient( this );
    // register optional pointer to handler
    if ( rb_channel < 16 ) ppc_handler[rb_channel] = rpc_handler;
    #ifdef DEBUG
    if ( rpc_handler != NULL )
     getRs232Instance() << "DigitalI_c::init() zu Channel: " << uint16_t(rb_channel) << " mit IRQ Handler\n";
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
    getLbsErrInstance().registerError( LibErr_c::Range, LibErr_c::Sensor );
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
    getLbsErrInstance().registerError( LibErr_c::Range, LibErr_c::Sensor );
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
      * LibErr_c::Range wrong input number
  @return true for (High signal AND ren_onoff==OnHigh)(Default!) or (Low signal AND ren_onoff==OnLow); otherwise false
*/
bool DigitalI_c::active() const {
  int16_t i16_val = val();
  if (i16_val == HAL_RANGE_ERR)
  { // wrong input channel
    return false;
  }
  return (i16_val == ON)?true:false;
}
/**
  check for the input value (uses BIOS function)
  possible errors:
      * LibErr_c::Range wrong input number
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
    getLbsErrInstance().registerError( LibErr_c::Range, LibErr_c::Sensor );
    return ERROR_VAL_16S;
  }
  else
  { // correct input channel
    return (i16_val == ON)?1:0;
  }
}

} // end of namespace __IsoAgLib
