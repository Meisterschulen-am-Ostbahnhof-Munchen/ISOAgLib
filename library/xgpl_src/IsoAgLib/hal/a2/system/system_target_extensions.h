/*
  system_target_extensions.h: header for A2 specific extensions for
    the HAL for central system

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

/** \file library/xgpl_src/IsoAgLib/hal/a2/system/system_target_extensions.h
 * A module targetExtensions should be used
 * for all methods, which can't be simply
 * mapped from ECU standard BIOS to the needs of
 * IsoAgLib by mostly renaming and reordering of functions, parameters
 * and types in <i>\<target\>/\<device\>/\<device\>.h</i> .
 * ********************************************************** */

#ifndef _HAL_A2_SYSTEM_TARGET_EXTENSIONS_H_
#define _HAL_A2_SYSTEM_TARGET_EXTENSIONS_H_

#include "../typedef.h"
#include "../errcodes.h"

#if defined( DEBUG )
#include <supplementary_driver/driver/rs232/irs232io_c.h>	    //BW, 4/12/06
#include <supplementary_driver/driver/rs232/impl/rs232io_c.h>  //BW, 4/12/06
#endif


namespace __HAL {
   #define  GET_U_C               35        /* UC (Boardspannung)   */
   #define  GET_U_EXT_8_5_V       15        /* U 8,5 V EXT */


/* ******************************************** */
/** \name General BIOS Extenders                */
/*@{*/
/**
  open the system with system specific function call
  @return error state (C_NO_ERR == o.k.)
*/
int16_t open_system( void );
/**
  close the system with system specific function call
  @return error state (C_NO_ERR == o.k.)
*/
int16_t closeSystem( void );
/** check if open_System() has already been called */
bool isSystemOpened( void );

/** trigger the watchdog */
void wdTriggern(void);

//BW
int32_t get_time(void); //return time in milliseconds 
void delay_us(word wTimer); // delay micro secounds
void wd_triggern(void); //empty operation
int  config_wd(tWDConfig *tConfigArray);
int  close_esx(void);                    /* stop esx background system*/
int  open_esx(tSystem *tSystem);         /* start esx background system*/
int  get_on_off_switch(void);            /* the evaluation of the on/off-switch (D+)*/
void set_relais(boolean bitState);       /* switch relais on or off*/
void power_down(void);                   /* to deactivate the power selfholding*/
void staying_alive(void);                /* to activate the power selfholding*/
int  wd_reset(void);                     /* to force a reset*/
int get_snr(byte *snr_dat);               /* serial number of esx */
int get_adc(byte bKanalnummer);     /* evaluation of analog channels */
void start_task_timer (word wT5_reload);



/**
  configure the watchdog of the system with the
  settings of configEsx
  @return error state (C_NO_ERR == o.k.)
    or DATA_CHANGED on new values -> need call of wdReset to use new settings
  @see wdReset
*/
int16_t configWatchdog( void );
/*@}*/
}

#endif
