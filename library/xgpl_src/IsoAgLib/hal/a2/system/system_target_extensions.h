/***************************************************************************
                          system_target_extensions.h - header for A2 specific
                                                extensions for the HAL
                                                for central system
                             -------------------
    begin                : Wed Mar 15 2000
    copyright            : (C) 2000 - 2009 Dipl.-Inform. Achim Spangler
             : This file was based on the corresponding file in
             : the ESX HAL and modified for A2 HAL.
             : These changes (C) 2006 - 2010 Brian Wei
    email                : a.spangler@osb-ag:de
             : brian.wei@agcocorp:com
    type                 : Header
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
 * Copyright (C) 1999 - 2009 Dipl.-Inform. Achim Spangler                 *
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

/* ********************************************************** */
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

#if defined( DEBUG ) && defined( SYSTEM_A2 )
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