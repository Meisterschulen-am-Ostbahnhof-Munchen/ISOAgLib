/***************************************************************************
                          targetExtensionCanTestserver.cc - source for the real time server
                                       with CAN communication through client-server call semantics
                             -------------------
    begin                : Tue Jul 15 2003
    copyright            : (C) 1999 - 2004 Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
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
 * Copyright (C) 1999 - 2004 Dipl.-Inform. Achim Spangler                 *
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
#define  STRICT

#define CVS_REV "$Revision$"
#define RTE_CLIENT

#include "can_target_extensions.h"
#include <stdlib.h>
#include <ctype.h>
#include <IsoAgLib/hal/system.h>
#include <IsoAgLib/hal/can.h>

// RTE Includes
#include <rte_client.h>		// be an RTE client
#include <iostream>
#include <iomanip.h>

#include <list>

namespace __HAL {
/////////////////////////////////////////////////////////////////////////
// Globals


struct can_data { // evtl. anpassen
  int32_t i32_time;
  int32_t i32_ident;
  uint8_t b_dlc;
  uint8_t b_xtd;
  uint8_t pb_data[8];
};

// IsoAgLib counting for BUS-NR and MsgObj starts both in C-Style with 0
// -> all needed offsets shall be added at the lowest possible layer
//    ( i.e. direct in the BIOS/OS call)
static const uint32_t cui32_maxCanBusCnt = ( HAL_CAN_MAX_BUS_NR + 1 );
static can_c* rteCan_c [cui32_maxCanBusCnt];
static rtd_can_bus_state_t rteCanBusState[cui32_maxCanBusCnt];

static can_data* rec_buf[cui32_maxCanBusCnt][15];
static int16_t rec_bufCnt[cui32_maxCanBusCnt][15];
static int16_t rec_bufSize[cui32_maxCanBusCnt][15];
static int16_t rec_bufOut[cui32_maxCanBusCnt][15];
static int16_t rec_bufIn[cui32_maxCanBusCnt][15];
static bool rec_bufXtd[cui32_maxCanBusCnt][15];
static uint32_t rec_bufFilter[cui32_maxCanBusCnt][15];
static bool b_canBufferLock[cui32_maxCanBusCnt][15];
int32_t i32_lastReceiveTime;

static uint16_t ui16_globalMask[cui32_maxCanBusCnt];
static uint32_t ui32_globalMask[cui32_maxCanBusCnt];
static uint32_t ui32_lastMask[cui32_maxCanBusCnt];
static bool b_busOpened[cui32_maxCanBusCnt];

class RteCanPkg {
  private:
    uint32_t id;
    uint8_t  b_xtd;
    uint8_t  size;
    uint8_t  data[8];
  public:
    RteCanPkg( uint32_t rui32_id = 0, uint8_t rui8_xtd = 0, uint8_t rui8_size = 0, const uint8_t* rpui8_data = NULL )
      { id = rui32_id; b_xtd = rui8_xtd; size = ( rui8_size <= 8 )?rui8_size:8; memcpy( data, rpui8_data, size ); };

    bool operator==( const RteCanPkg& rrefc_src ) const
      { return ( ( id == rrefc_src.id ) && ( b_xtd == rrefc_src.b_xtd ) && ( size == rrefc_src.size ) && ( memcmp( data, rrefc_src.data, size ) == 0 ) )?true:false; } ;

    bool operator!=( const RteCanPkg& rrefc_src ) const
      { return ( ( id != rrefc_src.id ) || ( b_xtd != rrefc_src.b_xtd ) || ( size != rrefc_src.size ) || ( memcmp( data, rrefc_src.data, size ) != 0 ) )?true:false; } ;

    const RteCanPkg& operator=( const RteCanPkg& rrefc_src )
      { id = rrefc_src.id; b_xtd = rrefc_src.b_xtd; size = rrefc_src.size; memcpy( data, rrefc_src.data, size ); return *this;};
};

static STL_NAMESPACE::list<RteCanPkg> arr_canQueue[cui32_maxCanBusCnt];



// globale Filehandle fuer simulierte CAN Input
static FILE* canlogDat[cui32_maxCanBusCnt];

// offset time
// static rte_time_t t_rteOffset = RTE_NEVER;

int16_t can_startDriver()
{
  // open the driver
	for ( uint32_t ind = 0; ind < cui32_maxCanBusCnt; ind++ )
	{
		rteCanBusState[ind] = rtd_can_bus_state_active;
		rteCan_c[ind] = NULL;
		canlogDat[ind] = NULL;
		b_busOpened[ind] = false;
    for (uint8_t ui8_nr = 0; ui8_nr < 15; ui8_nr++)
    {
      rec_bufSize[ind][ui8_nr] = 0;
      rec_bufCnt[ind][ui8_nr] = 0;
    }
	}

//  cout << "Trying to connect to rte1... ";
  if ( ! rte_is_init() ) {
    if (rte_connect( "rte1" ) < 0) {
      cerr << "Unable to connect RTE1 server." << endl;
      exit(1);
    } else {
      cout << "Connected to RTE1" << endl;
    }
  }

	for ( uint32_t ind = 0; ind < cui32_maxCanBusCnt; ind++ )
	{
		rteCan_c [ind] = new can_c ();
	}

  // clear list for CAN Pkg
//  arr_canQueue[0].list();
//  arr_canQueue[1].list();

  return HAL_NO_ERR;
}

int16_t can_stopDriver()
{
	for ( uint32_t ind = 0; ind < cui32_maxCanBusCnt; ind++ )
	{
		if (canlogDat[ind] != NULL){
			fclose(canlogDat[ind]);
			canlogDat[ind] = NULL;
		}
	  delete (rteCan_c [ind]);
  }

  return HAL_NO_ERR;
}

/** get last timestamp of CAN receive */
int32_t can_lastReceiveTime()
{
  checkMsg();
  return i32_lastReceiveTime;
}


int16_t getCanMsgBufCount(uint8_t bBusNumber,uint8_t bMsgObj)
{
  checkMsg();
  return ((bBusNumber < cui32_maxCanBusCnt)&&(bMsgObj < 15))?rec_bufCnt[bBusNumber][bMsgObj]:0;
};



int16_t closeCan ( uint8_t bBusNumber )
{
  if (canlogDat[bBusNumber] != NULL){
    fclose(canlogDat[bBusNumber]);
    canlogDat[bBusNumber] = NULL;
  }
  b_busOpened[bBusNumber] = false;

  for (int i=0; i < 15; i++) {
    closeCanObj (bBusNumber, i);
  }

  // clear list for CAN Pkg
  arr_canQueue[0].clear();
  arr_canQueue[1].clear();
  arr_canQueue[2].clear();
  arr_canQueue[3].clear();

  return HAL_NO_ERR;
};

int16_t chgCanObjPause ( uint8_t /* bBusNumber */, uint8_t /* bMsgObj */, uint16_t /* wPause */ )
{
/*
  printf("sende Pause auf BUS %d fuer CAN Objekt %d auf %d eingestellt\n",
  (int16_t)bBusNumber, (int16_t)bMsgObj, wPause);
  */
  return HAL_NO_ERR;
}

int16_t getCanBusStatus(uint8_t bBusNumber, tCanBusStatus* ptStatus )
{
	switch ( rteCanBusState[bBusNumber] )
	{
		case rtd_can_bus_state_offline:
		case rtd_can_bus_state_off:
			ptStatus->wCtrlStatusReg = CanStateOff;
			break;
		case rtd_can_bus_state_passive:
			ptStatus->wCtrlStatusReg = CanStateWarn;
			break;
		case rtd_can_bus_state_active:
			ptStatus->wCtrlStatusReg = CanStateNoErr;
			break;
	}
//  printf("getCanBusStatus fuer BUS %d\n", bBusNumber);
  return HAL_NO_ERR;
}

int16_t clearCanObjBuf(uint8_t bBusNumber, uint8_t bMsgObj)
{
  if (rec_bufCnt[bBusNumber][bMsgObj] == -1)
  { // it's a send object -> call native clear transmit
//    ncdFlushTransmitQueue(gPortHandle, gChannelMask);
  }
  else
  { // set receive buffer to 0
    rec_bufCnt[bBusNumber][bMsgObj] = 0;
    rec_bufOut[bBusNumber][bMsgObj] = 0;
    rec_bufIn[bBusNumber][bMsgObj] = 0;
  }
  return HAL_NO_ERR;
}

int16_t configCanObj ( uint8_t bBusNumber, uint8_t bMsgObj, tCanObjConfig * ptConfig )
{
	b_canBufferLock[bBusNumber][bMsgObj] = false;
  if (ptConfig->bMsgType == TX)
  { /* Sendeobjekt */
    rec_bufSize[bBusNumber][bMsgObj] = -1;
  }
  else
  { // empfangsobjekt
    rec_bufXtd[bBusNumber][bMsgObj] = ptConfig->bXtd;
    rec_buf[bBusNumber][bMsgObj] = new can_data[ptConfig->wNumberMsgs*20];
    rec_bufSize[bBusNumber][bMsgObj] = ptConfig->wNumberMsgs*20;
    rec_bufCnt[bBusNumber][bMsgObj] = 0;
    rec_bufOut[bBusNumber][bMsgObj] = 0;
    rec_bufIn[bBusNumber][bMsgObj] = 0;
    rec_bufFilter[bBusNumber][bMsgObj] = ptConfig->dwId;

    // clear list for CAN Pkg
    arr_canQueue[bBusNumber].clear();
    // set RTE time offset
    // t_rteOffset = rte_approx_server_now() - ( rte_time_t( getTime() ) * RTE_ONE_MILLISECOND );
  }
  return HAL_NO_ERR;
};

int16_t chgCanObjId ( uint8_t bBusNumber, uint8_t bMsgObj, uint32_t dwId, uint8_t bXtd )
{
	b_canBufferLock[bBusNumber][bMsgObj] = false;
  if (rec_bufSize[bBusNumber][bMsgObj] > -1)
  { // active receive object
    rec_bufFilter[bBusNumber][bMsgObj] = dwId;
    rec_bufXtd[bBusNumber][bMsgObj] = bXtd;
  }
  return HAL_NO_ERR;
}

/**
	lock a MsgObj to avoid further placement of messages into buffer.
  @param rui8_busNr number of the BUS to config
  @param rui8_msgobjNr number of the MsgObj to config
	@param rb_doLock true==lock(default); false==unlock
  @return HAL_NO_ERR == no error;
          HAL_CONFIG_ERR == BUS not initialised or ident can't be changed
          HAL_RANGE_ERR == wrong BUS or MsgObj number
	*/
int16_t lockCanObj( uint8_t rui8_busNr, uint8_t rui8_msgobjNr, bool rb_doLock )
{ // first get waiting messages
	rte_poll();
//	if (b_busOpened[rui8_busNr])
//  {
//    rteCan_c [rui8_busNr]->poll ();
//  }
	b_canBufferLock[rui8_busNr][rui8_msgobjNr] = rb_doLock;
  return HAL_NO_ERR;
}


int16_t closeCanObj ( uint8_t bBusNumber,uint8_t bMsgObj )
{
	b_canBufferLock[bBusNumber][bMsgObj] = false;
  if (rec_bufSize[bBusNumber][bMsgObj] == -1)
  { /* Sendeobjekt */
    rec_bufSize[bBusNumber][bMsgObj] = -1;
  }
  else
  { // empfangsobjekt
    delete [] (rec_buf[bBusNumber][bMsgObj]);
    rec_bufSize[bBusNumber][bMsgObj] = -1;
    rec_bufCnt[bBusNumber][bMsgObj] = 0;
    rec_bufOut[bBusNumber][bMsgObj] = 0;
    rec_bufIn[bBusNumber][bMsgObj] = 0;

    // clear list for CAN Pkg
    arr_canQueue[bBusNumber].clear();
  }
  return HAL_NO_ERR;
};

int16_t sendCanMsg ( uint8_t bBusNumber,uint8_t /* bMsgObj */, tSend * ptSend )
{
  int error;

  // set RTE time offset
  // t_rteOffset = rte_approx_server_now() - ( rte_time_t( getTime() ) * RTE_ONE_MILLISECOND );

  rteCan_c[bBusNumber]->set_delta (0); // NOW!
  rteCan_c[bBusNumber]->set_count (1); // send only once

  if (ptSend->bXtd) {
    rteCan_c[bBusNumber]->send (rtd_can_type_xtd_msg, ptSend->dwId, ptSend->bDlc, ptSend->abData);
  } else {
    rteCan_c[bBusNumber]->send (rtd_can_type_std_msg, ptSend->dwId, ptSend->bDlc, ptSend->abData);
  }

  error = 0;

  if (error)
  {
    return HAL_OVERFLOW_ERR;
  }
  else
  {
    #if 1
    // long l_time = ( ( rte_approx_server_now() - t_rteOffset ) / RTE_ONE_MILLISECOND);
    fprintf(canlogDat[bBusNumber], "Send    %d.%03d: %x  %hx %hx %hx %hx %hx %hx %hx %hx\n", (getTime()/1000), (getTime()%1000), ptSend->dwId,
    ptSend->abData[0], ptSend->abData[1], ptSend->abData[2],
    ptSend->abData[3], ptSend->abData[4], ptSend->abData[5],
    ptSend->abData[6], ptSend->abData[7]);
    fflush( canlogDat[bBusNumber] );
    // insert sent msg in array to detect later receive of self sent msg
    // arr_canQueue[bBusNumber].push_back( RteCanPkg( ptSend->dwId, ptSend->bXtd, ptSend->bDlc, ptSend->abData ) );
    #endif
    return HAL_NO_ERR;
  }
};

int16_t getCanMsg ( uint8_t bBusNumber,uint8_t bMsgObj, tReceive * ptReceive )
{
  if (rec_bufCnt[bBusNumber][bMsgObj] > 0)
  { // data received
    i32_lastReceiveTime = getTime();
    // get access to data
    int16_t i16_out = rec_bufOut[bBusNumber][bMsgObj];
    can_data* pc_data = &(rec_buf[bBusNumber][bMsgObj][i16_out]);
    // copy data
    ptReceive->dwId = pc_data->i32_ident;
    ptReceive->bDlc = pc_data->b_dlc;
    ptReceive->tReceiveTime.l1ms = pc_data->i32_time;
    ptReceive->bXtd = pc_data->b_xtd;
    memcpy(ptReceive->abData, pc_data->pb_data, pc_data->b_dlc);

    // update ringpuffer
    rec_bufOut[bBusNumber][bMsgObj] = ((i16_out + 1) % rec_bufSize[bBusNumber][bMsgObj]);
    rec_bufCnt[bBusNumber][bMsgObj] -= 1;
  }
  return HAL_NO_ERR;
};


int16_t checkMsg()
{
	rte_poll();
	/*
	for (uint8_t b_bus = 0; b_bus < cui32_maxCanBusCnt; b_bus++)
  {
    if (b_busOpened[b_bus])
    {
      rteCan_c [b_bus]->poll ();
    }
  }
	*/
  return 0;
}


//####################################################################

/*! Handler for identification reply */
static int ident_hdl(rtd_handler_para_t* para, char* ident) {
  if ((para == 0) || (ident == 0))
    return rte_set_error( null_pointer );

  return 0;
}

/*! Handler for acknowledge reply */
static int ackHdl(rtd_handler_para_t*, rte_return_t ) {
  return 0;
}

/*! Handler for incoming CAN messages. */
static int send(rtd_handler_para_t* para, rtd_can_type_t type, uint32_t id, uint8_t size, const void *data)
{
  static rte_time_t lastTime;
  uint8_t b_bus = para->rtd_msg->channel;
  uint8_t b_xtd = ( type == rtd_can_type_xtd_msg )?1:0;

  if (!b_busOpened[b_bus])
    return -1;

  // first check if the actual received msg is echo of previously sent msg
  #if 0
  if ( count( arr_canQueue[ b_bus ].begin(), arr_canQueue[ b_bus ].end(), RteCanPkg( id, b_xtd, size, (const uint8_t*)data ) ) > 0 ) {
    //handler is called for receive of echo of previously sent msg -> just erase msg from list and return
    std::cout << "ECHO Empfang" << std::endl;
    arr_canQueue[ b_bus ].erase( find( arr_canQueue[ b_bus ].begin(), arr_canQueue[ b_bus ].end(), RteCanPkg( id, b_xtd, size, (const uint8_t*)data ) ) );
    return 0;
  }
  #endif

  for (int16_t i16_obj = 0; i16_obj < 15; i16_obj++)
	{ // compare received msg with filter
		int16_t i16_in;
		can_data* pc_data;
		if ( b_canBufferLock[b_bus][i16_obj] )
		{ // don't even check this MsgObj as it shall not receive messages
			continue;
		}
		if
		(
			(
				( i16_obj < 14 )
			&& (
					( (rec_bufXtd[b_bus][i16_obj] == 1)
					&& (b_xtd == 1)
					&& (rec_bufSize[b_bus][i16_obj] > 0)
					&& ( (id & ui32_globalMask[b_bus]) ==  ((rec_bufFilter[b_bus][i16_obj]) & ui32_globalMask[b_bus]) )
					)
				|| ( (rec_bufXtd[b_bus][i16_obj] == 0)
					&& (b_xtd == 0)
					&& (rec_bufSize[b_bus][i16_obj] > 0)
					&& ( (id & ui16_globalMask[b_bus]) ==  (rec_bufFilter[b_bus][i16_obj] & ui16_globalMask[b_bus]) )
					)
				)
			)
		|| (
				( i16_obj == 14 )
			&& (
					( (rec_bufXtd[b_bus][i16_obj] == 1)
					&& (b_xtd == 1)
					&& (rec_bufSize[b_bus][i16_obj] > 0)
					&& ( (id & ui32_globalMask[b_bus] & ui32_lastMask[b_bus]) == ((rec_bufFilter[b_bus][i16_obj]) & ui32_globalMask[b_bus] & ui32_lastMask[b_bus]) )
					)
				|| ( (rec_bufXtd[b_bus][i16_obj] == 0)
					&& (b_xtd == 0)
					&& (rec_bufSize[b_bus][i16_obj] > 0)
					&& ( (id & ui16_globalMask[b_bus] & ui32_lastMask[b_bus]) == (rec_bufFilter[b_bus][i16_obj] & ui16_globalMask[b_bus] & ui32_lastMask[b_bus]) )
					)
				)
			)
		)
		{ // received msg fits actual filter
			i16_in = rec_bufIn[b_bus][i16_obj];
			rec_bufIn[b_bus][i16_obj] = ((i16_in + 1) % rec_bufSize[b_bus][i16_obj]);
			if (rec_bufCnt[b_bus][i16_obj] >= rec_bufSize[b_bus][i16_obj])
			{ // overflow -> insert new, and overwrite oldest msg in buffer
				rec_bufOut[b_bus][i16_obj] = rec_bufIn[b_bus][i16_obj];
			}
			else
			{
				rec_bufCnt[b_bus][i16_obj] += 1;
			}
			pc_data = &(rec_buf[b_bus][i16_obj][i16_in]);
			pc_data->i32_time = getTime();
			pc_data->i32_ident = id;
			pc_data->b_dlc = size;
			pc_data->b_xtd = (type == rtd_can_type_xtd_msg)?1:0;
			memcpy(pc_data->pb_data, data, pc_data->b_dlc);
			#if 0
			if (  ((id & 0x700) == 0x700)
					|| ((id & 0x700) == 0x500)
					|| ((id & 0x700) == 0x200)
					|| ((id & 0x700) == 0x000)
//        || ( pc_data->b_xtd )
					)
			{
				printf("Empfang %d.%03d: %x  %hx %hx %hx %hx %hx %hx %hx %hx\n", pc_data->i32_time/1000,pc_data->i32_time%1000, id,
				pc_data->pb_data[0], pc_data->pb_data[1], pc_data->pb_data[2],
				pc_data->pb_data[3], pc_data->pb_data[4], pc_data->pb_data[5],
				pc_data->pb_data[6], pc_data->pb_data[7]);

				fprintf(canlogDat[b_bus], "Empfang %d.%03d: %x  %hx %hx %hx %hx %hx %hx %hx %hx\n", pc_data->i32_time/1000,pc_data->i32_time%1000, id,
				pc_data->pb_data[0], pc_data->pb_data[1], pc_data->pb_data[2],
				pc_data->pb_data[3], pc_data->pb_data[4], pc_data->pb_data[5],
				pc_data->pb_data[6], pc_data->pb_data[7]);
			}
			#endif

		} // if fit
	} // for objNr
  can_c *c;
  if (rte_get_class<can_c>( para, c ) == 0)
    return rte_set_error( illegal_para );
    rte_time_t t = para->time;

  lastTime = t;
  return 0;
}

/*! Handler for CAN bus state indications. */
static int setBusState(rtd_handler_para_t* para, rtd_can_bus_state_t state)
{
  rteCanBusState[para->rtd_msg->channel] = state;
	switch( state ) {
  case rtd_can_bus_state_offline:
    cout << "CAN node offline" << endl; break;
  case rtd_can_bus_state_active:
    cout << "CAN node in error-active state" << endl; break;
  case rtd_can_bus_state_passive:
    cout << "CAN node in error-passive state" << endl; break;
  case rtd_can_bus_state_off:
    cout << "CAN node in bus-off state" << endl; break;
  }
  return 0;
}

int16_t init_can ( uint8_t bBusNumber,uint16_t wGlobMask,uint32_t dwGlobMask,uint32_t dwGlobMaskLastmsg ,uint16_t wBitrate )
{
  if ( bBusNumber >= cui32_maxCanBusCnt ) return HAL_RANGE_ERR;

  rte_poll ();
  char logName[50];
#ifdef WIN32
  sprintf( logName, "..\\..\\..\\simulated_io\\can_send_%1hd.txt", bBusNumber );
#else
  sprintf( logName, "../../../simulated_io/can_send_%1hd.txt", bBusNumber );
#endif
	static bool b_initFirstLog = true;
  static bool firstOpenLog[cui32_maxCanBusCnt];
	if ( b_initFirstLog )
	{
		for ( uint32_t ind = 0; ind < cui32_maxCanBusCnt; ind++ ) firstOpenLog[ind] = true;
		b_initFirstLog = false;
	}
  if ( firstOpenLog[bBusNumber] ) {
    firstOpenLog[bBusNumber] = false;
    canlogDat[bBusNumber] = fopen( logName, "w");
    // BEGIN: Added by M.Wodok 6.12.04
    if (canlogDat [bBusNumber] == NULL) {
      // failed opening? Try in current directory then...
      sprintf( logName, "can_send_%1hd.txt", bBusNumber );
      canlogDat[bBusNumber] = fopen( logName, "w");
    }
    // END: Added by M.Wodok 6.12.04
  }
  else {
    // use appending open on succeding can inits during one program run
    canlogDat[bBusNumber] = fopen( logName, "a+");
    // BEGIN: Added by M.Wodok 6.12.04
    if (canlogDat [bBusNumber] == NULL) {
      // failed opening? Try in current directory then...
      sprintf( logName, "can_send_%1hd.txt", bBusNumber );
      canlogDat[bBusNumber] = fopen( logName, "a+");
    }
    // END: Added by M.Wodok 6.12.04
  }


  // Open a CAN messaging interface
  rteCan_c [bBusNumber]->set_channel( bBusNumber );
  rteCan_c [bBusNumber]->set_default_timeout( 60*RTE_ONE_SECOND );
  rteCan_c [bBusNumber]->set_echo( false );
  rteCan_c [bBusNumber]->init();

  // build parameter string with sprintf to handle different baudrates
  char parameterString[50];
  sprintf( parameterString, "C%d,R,B%d,E0,L0", bBusNumber, wBitrate );
  rteCan_c [bBusNumber]->set_parameters( parameterString );
  #if 0
  if (bBusNumber == 0)
    rteCan_c [0]->set_parameters( "C0,R,B250,E0,L0" );
  else
    rteCan_c [1]->set_parameters( "C1,R,B250,E0,L0" );
  #endif

  // Set handler functions.
  rteCan_c [bBusNumber]->set_acknowledge_handler( ackHdl, rteCan_c [bBusNumber]);
  rteCan_c [bBusNumber]->set_identify_handler( ident_hdl, rteCan_c [bBusNumber]);
  rteCan_c [bBusNumber]->set_send_handler( send, rteCan_c [bBusNumber] );
  rteCan_c [bBusNumber]->set_set_bus_state_handler( setBusState, rteCan_c [bBusNumber] );

  // Set Global Masks
  ui16_globalMask[bBusNumber] = wGlobMask;
  ui32_globalMask[bBusNumber] = dwGlobMask;
	ui32_lastMask[bBusNumber] = dwGlobMaskLastmsg;

  // test if init was okay? (later)
  b_busOpened[bBusNumber] = true;

  return HAL_NO_ERR;
};



} // end namespace __HAL
