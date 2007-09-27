/* *************************************************************************
                          test_hal.cpp  - main application to test HAL
                             -------------------
    begin                : Mon Okt 16 08:00:00 CEST 2000

    copyright            : (C) 2000 - 2004 Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
 ***************************************************************************/

/* *************************************************************************
 * This example main application is published NOT as GPL´ed Open Source,  *
 * so that you can include this source file in propietary closed projects. *
 * Everybody is encouraged to use the examples in the lbsLibExample      *
 * directory for a quick and easy start of development for IsoAgLib applications*
 *                                                                         *
 * So nobody is bound to publish changes of the example source files.      *
 * But every changed example, which could help learning the use of the    *
 * GPLed "IsoAgLib", is invited to be sent to the original author, so  *
 * that it can be published with the other examples.                       *
 ***************************************************************************/

/* *************************************************************************
 *                                                                         *
 * This is an example of "IsoAgLib", an object oriented program library *
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
 * The "IsoAgLib" is free software; you can redistribute it and/or      *
 * modify it under the terms of the GNU Lesser General Public License as   *
 * published by the Free Software Foundation; either version 2 of the      *
 * License, or (at your option) any later version.                         *
 *                                                                         *
 * The "IsoAgLib" is distributed in the hope that it will be useful, but*
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

/* *********************************************************************** */
/** \example test_hal.cpp
 * This example was written to check most of the HAL functions. It was used
 * for the adaptation of the mitron167 hal. You may use this also for test and
 * creation of new HAL sets. Please contribute your extensions to the
 * IsoAgLib mailing list, so that this example can get a real testsuite for
 * HAL adaptation.
 *                                                                         */
/* *************************************************************************/

// include the central interface header for the hardware adaptation layer part
// of the "IsoAgLib"
#include <IsoAgLib/util/config.h>
// #include <IsoAgLib/driver/system/isystem.h>


// #include <IsoAgLib/driver/can/ican_io.h>

#include <IsoAgLib/hal/system.h>
#include <IsoAgLib/hal/can.h>
#include <IsoAgLib/driver/can/impl/ident_c.h>
#include <IsoAgLib/util/impl/canpkg_c.h>
#include <supplementary_driver/hal/rs232.h>
#include <cstdio>
#include <ctime>

extern "C" {
  /** include the BIOS specific header with the part for CAN into __HAL */
  #include <C166.h>
  #include <reg167cs.h>
}

// the interface objects of the IsoAgLib are placed in the IsoAgLibAll namespace
// -> include all elements of this area for easy access
// with this command the text part "IsoAgLib::" can be avoided, which
// is needed for the documentation generator
// using namespace IsoAgLib;
extern "C" STL_NAMESPACE::time_t _time( STL_NAMESPACE::time_t *pt );

int main()
{ // simply call startImi
  HAL::open_system();
  // init RS232
  HAL::init_rs232( 19200, 5, 1, false );

  __IsoAgLib::Ident_c::identType_t t_initType[2] = { __IsoAgLib::Ident_c::StandardIdent,
                                 __IsoAgLib::Ident_c::ExtendedIdent };

  for ( uint8_t ind = 0; ind < 2 ; ind++ ) {
	  // init CAN1
	  HAL::can_configGlobalInit( ind, ( 125 * ( ind + 1 ) ), 0, 0, 0 );
	  // send MsgObj
	  __IsoAgLib::Ident_c c_tempIdent( ind, t_initType[ind] );
	  HAL::can_configMsgobjInit( ind, 0, c_tempIdent, 1 );
	  // receive Obj
	  HAL::can_configMsgobjInit( ind, 1, c_tempIdent, 0 );
	  // close CAN
	  HAL::can_configGlobalClose( ind );

	  // init CAN1
	  HAL::can_configGlobalInit( ind, ( 125 * ( ind + 1 ) ), 0, 0, 0 );
	  // send MsgObj
	  HAL::can_configMsgobjInit( ind, 0, c_tempIdent, 1 );
	  // receive Obj
	  HAL::can_configMsgobjInit( ind, 1, c_tempIdent, 0 );
	  // close CAN
	  HAL::can_configGlobalClose( ind );

	  // init again - to test masks
	  HAL::can_configGlobalInit( ind, ( 125 * ( ind + 1 ) ), 0x7, 0x7F00000, 0 );
	  HAL::can_configMsgobjInit( ind, 0, c_tempIdent, 1 );
	  // 1st receive Obj
	  c_tempIdent.set( 0x500, t_initType[ind] );
	  HAL::can_configMsgobjInit( ind, 1, c_tempIdent, 0 );
    HAL::can_configMsgobjClose( ind, 1 );
	  c_tempIdent.set( 0x200, t_initType[ind] );
	  HAL::can_configMsgobjInit( ind, 1, c_tempIdent, 0 );
	  c_tempIdent.set( 0x0, t_initType[ind] );
	  HAL::can_configMsgobjChgid( ind, 1, c_tempIdent );
	  // 2nd receive Obj
	  c_tempIdent.set( 0x100, t_initType[ind] );
	  HAL::can_configMsgobjInit( ind, 2, c_tempIdent, 0 );
	  // 3rd receive Obj
	  c_tempIdent.set( 0x500, t_initType[ind] );
	  HAL::can_configMsgobjInit( ind, 3, c_tempIdent, 0 );
	  c_tempIdent.set( 0x200, t_initType[ind] );
	  HAL::can_configMsgobjChgid( ind, 3, c_tempIdent );
	  // 4th receive Obj
	  c_tempIdent.set( 0x400, t_initType[ind] );
	  HAL::can_configMsgobjInit( ind, 4, c_tempIdent, 0 );
	  // 5th receive Obj
	  c_tempIdent.set( 0x800, t_initType[ind] );
	  HAL::can_configMsgobjInit( ind, 5, c_tempIdent, 0 );
	  // 6th receive Obj
	  c_tempIdent.set( 0x010, t_initType[ind] );
	  HAL::can_configMsgobjInit( ind, 6, c_tempIdent, 0 );
	  // 7th receive Obj
	  c_tempIdent.set( 0x020, t_initType[ind] );
	  HAL::can_configMsgobjInit( ind, 7, c_tempIdent, 0 );
	  // 8th receive Obj
	  c_tempIdent.set( 0x010, t_initType[ind] );
	  HAL::can_configMsgobjInit( ind, 8, c_tempIdent, 0 );
	  // 9th receive Obj
	  c_tempIdent.set( 0x020, t_initType[ind] );
	  HAL::can_configMsgobjInit( ind, 9, c_tempIdent, 0 );
	  // 10th receive Obj
	  c_tempIdent.set( 0x040, t_initType[ind] );
	  HAL::can_configMsgobjInit( ind, 10, c_tempIdent, 0 );

    // wait for 3 sec
    int32_t i32_delayEnd = HAL::getTime() + 3000;
    int32_t indTest = 0;
    while ( i32_delayEnd > HAL::getTime() ) indTest++;
  }


  char testString[500];
  int32_t i32_nextPrint = HAL::getTime() + 1000;
  int32_t i32_checkIdent;
  // set CAN data
  uint8_t ui8_data[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
  __IsoAgLib::CANPkg_c c_pkg[2];

  c_pkg[0].setIdent( 0x123, t_initType[0] );
  c_pkg[0].setDataString( ui8_data, 8 );

  c_pkg[1].setIdent( 0x123, t_initType[1] );
  c_pkg[1].setDataString( ui8_data, 8 );

  uint32_t ui32_t5 = T5;
  uint32_t ui32_test = ( ( ( ui32_t5 << 16 ) | ( T6 ) ) / 39 );

  while ( 1 ) {
    // check for received input
	if ( HAL::getRs232RxBufCount() > 0 ) {
	  // send echo with timestamp
	  sprintf( testString, "ECHO at time %ld:", HAL::getTime() );
      HAL::put_rs232String( (const uint8_t*)testString );
	  HAL::getRs232String( (uint8_t*)testString, '\0' );
      HAL::put_rs232String( (const uint8_t*)testString );
      HAL::put_rs232String( "\n\r" );
    }
	#if 1
  for ( uint8_t bus_nr = 0; bus_nr < 2 ; bus_nr++ ) {
		for ( uint16_t ind = 1; ind < 15; ind++ ) {
			if ( HAL::can_stateMsgobjBuffercnt( bus_nr, ind ) > 0 ) {
			  HAL::can_useMsgobjReceivedIdent( bus_nr, ind, i32_checkIdent );
			  sprintf( testString, "CheckIdent: %ld\n\r", i32_checkIdent );
		      HAL::put_rs232String( (const uint8_t *) testString );
			  // CAN mes received
			  HAL::can_useMsgobjGet( bus_nr, ind, &(c_pkg[bus_nr]) );
			  // send notify fo CAN
			  sprintf( testString, "CAN receive with timestamp: %ld\n\r", c_pkg[bus_nr].time() );
	      HAL::put_rs232String( (const uint8_t*)testString );
	      sprintf( testString, "BUS: %hd\tMsgObj: %hd\tTime: %lu:\tIdent:%luDLC: %hu\tData: %hx %hx %hx %hx %hx %hx %hx %hx\n\r",
          bus_nr,
				  ind,
	        c_pkg[bus_nr].time(),
				  c_pkg[bus_nr].ident(),
	        c_pkg[bus_nr].ui8_len,
				  c_pkg[bus_nr].pb_data[0],
				  c_pkg[bus_nr].pb_data[1],
				  c_pkg[bus_nr].pb_data[2],
				  c_pkg[bus_nr].pb_data[3],
				  c_pkg[bus_nr].pb_data[4],
				  c_pkg[bus_nr].pb_data[5],
				  c_pkg[bus_nr].pb_data[6],
				  c_pkg[bus_nr].pb_data[7] );
		    HAL::put_rs232String( (const uint8_t *) testString );
		  }
    }
	}
	#endif
    if ( HAL::getTime() >= i32_nextPrint ) {
      i32_nextPrint = HAL::getTime() + 1000;
      ui32_t5 = T5;
      ui32_test = ( ( ( ui32_t5 << 16 ) | ( T6 ) ) / 39 );
  	  sprintf( testString, "Test time %ld\tT5: %u\tT6: %u\t TestZeit:%ld\tLow Level Time: %ld\n\r",
  	    HAL::getTime(), T5, T6, ui32_test, _time( 0 ) );
      HAL::put_rs232String( (const uint8_t*)testString );
      for ( uint8_t ind = 0; ind < 6; ind++ ) {
  	    ui8_data[0] = ( T5 >> 8 );
	      ui8_data[1] = ( T5 & 0xFF );
	      ui8_data[2] = ( T6 >> 8 );
	      ui8_data[3] = ( T6 & 0xFF );
        c_pkg[0].setDataString( ui8_data, 8 );
        c_pkg[1].setDataString( ui8_data, 8 );
	      HAL::can_useMsgobjSend( 0, 0, &(c_pkg[0]));
	      HAL::can_useMsgobjSend( 1, 0, &(c_pkg[1]));
	    }
	  }
  }
  return 1;
}

