/*
  outputs.h: definition of Hardware Abstraction Layer of output
    functions for Dj Hardware

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#ifndef _HAL_DJ1_OUTPUTS_H_
#define _HAL_DJ1_OUTPUTS_H_

#include <IsoAgLib/hal/Dj1/config.h>
#include <IsoAgLib/hal/Dj1/typedef.h>
#include <IsoAgLib/hal/Dj1/errcodes.h>

/* ******************************************************** */
/**
 * \name Basic BIOS limit constants
 */
#define DIGITAL_OUTPUT_MIN 0
// only uncomment following line if  DIGITAL_OUTPUT_MIN > 0
//#define DIGITAL_OUTPUT_MIN_GREATER_ZERO
#define DIGITAL_OUTPUT_MAX 1


namespace __HAL
{
  extern "C"
  {
    /** include the BIOS specific header into __HAL */
    #include <commercial_BIOS/bios_Dj1/DjBios1.h>
  }

}

/**
   namespace with layer of inline (cost NO overhead -> compiler replaces
   inline function with call to orig BIOS function)
   functions between all IsoAgLib calls for BIOS and the corresponding BIOS functions
   --> simply replace the call to the corresponding BIOS function in this header
       for adaption to new platform
 */
namespace HAL
{
  /* **************************** */
  /** \name Output BIOS functions */

  inline void setMainRelais( bool bitState )
  {
    /* ON during system Initialization and off during close */
    __HAL::DjBios_PowerHold ( bitState ? __HAL::BIOS_TRUE: __HAL::BIOS_FALSE );
  };
   

  /**
    define the frequency of the pwm signal
      @param bOutput PWM output channel [0..1]
      @param dwFrequency PWM frequency in mHz [5x10^3..1x10^8]
      @return error state (HAL_NO_ERR == o.k.; HAL_RANGE_ERR == wrong channel OR frequency)
  */
  inline int16_t setPwmFreq ( uint8_t bOutput, uint32_t dwFrequency )
  {
//jtm    return __HAL::set_pwm_freq(bOutput, dwFrequency);
    return ( (__HAL::DjBios_OutSetFreq ( bOutput, dwFrequency ) == __HAL::BIOS_OUT_NO_ERR) ? HAL_NO_ERR : HAL_RANGE_ERR );
//    return ( HAL_NO_ERR );
  }


  /**
    retrieve maximal PWM frequency -> setting to this value results in maximal output
      @param aui8_channel channel number of output [0..1]
      @return max possible PWM value
  */
  inline uint16_t getMaxPwmDigout ( uint8_t aui8_channel )
  {
//jtm      __HAL::tOutput tOutputstatus;
//jtm      __HAL::get_digout_status (aui8_channel, &tOutputstatus );
//jtm      return tOutputstatus.wMaxOutput;
    return ( __HAL::DjBios_OutGetMaxTimerCount ( aui8_channel ) );
//    return ( 0xffff );
  }


  /**
    set pwm value 0 ... 100 %
      @param aui8_channel channel number of output [0..1]
      @param wPWMValue Value to set; depends on configured PWM freq; [0..0xFFFF]
      @return error state (HAL_NO_ERR == o.k.; HAL_RANGE_ERR == wrong channel)
  */
  inline int16_t setDigout(uint8_t aui8_channel, uint16_t wPWMValue)
  {
//jtm      return __HAL::set_digout(aui8_channel, wPWMValue);
    return ( __HAL::DjBios_OutSetPWMCount ( aui8_channel, wPWMValue ) == __HAL::BIOS_OUT_NO_ERR ? HAL_NO_ERR : HAL_RANGE_ERR );
//    return ( HAL_NO_ERR );
  };


  /**
    deliver the actual current of the digital output
      @param aui8_channel channel to check
      @return current in [mA] ( if specified channel doesn't support current measurement, -1 is returned )
  */
  inline int16_t getDigoutCurrent( uint8_t aui8_channel )
  {
    return -1;
  };


  /**
    deliver the state of a digital output
      This function evaluates the current where possible, otherwise it evaluates
      the measured voltage at the output. The latter interpretation can go wrong
      if the PWM setting is >0 but has a very low value, so that even under normal
      conditions the voltage with connected consuming device is lower than to open
      connector state at low level.
      @param aui8_channel channel to check
      @param aui16_minCurrent minimal allowed current in [mA]
      @param aui16_maxCurrent maximum allowed current in [mA]
      @return HAL_NO_ERR, HAL_DIGOUT_OPEN, HAL_DIGOUT_SHORTCUT, HAL_DIGOUT_OVERTEMP,
              HAL_DIGOUT_UNDERVOLT, HAL_DIGOUT_OVERVOLT
  */
//jtm    int16_t getDigoutDiagnose(uint8_t aui8_channel, uint16_t aui16_minCurrent, uint16_t aui16_maxCurrent);
  inline int16_t getDigoutDiagnose ( uint8_t aui8_channel,
                         uint16_t aui16_minCurrent, uint16_t aui16_maxCurrent)
  {
    switch ( __HAL::DjBios_OutGetStatus(aui8_channel) )
    {
      default:
      case __HAL::BIOS_OUT_NO_ERR:
      {
        return ( HAL_NO_ERR );
      }

      case __HAL::BIOS_OUT_SHORT:
      {
        return ( HAL_DIGOUT_SHORTCUT );
      }

      case __HAL::BIOS_OUT_OPEN:
      {
        return ( HAL_DIGOUT_OPEN );
      }

      case __HAL::BIOS_OUT_SHUTDOWN:
      {
        return ( HAL_DIGOUT_OVERTEMP );
      }

    } /* end switch() */
//    return ( HAL_NO_ERR );
  };


/** deliver the measure voltage at the PWM output.
    Use this for application specific state evaluation for cases, where the standard
    getDigoutDiagnose function can go wrong.
    @return voltage at PWM channel [mV]
*/
//jtm    int16_t getDigoutAdc( uint8_t aui8_channel );
  inline int16_t getDigoutAdc( uint8_t aui8_channel )
  {
    return ( 0 );
  };

} /* namespace HAL */

#endif /* _HAL_DJ1_OUTPUTS_H_ */
