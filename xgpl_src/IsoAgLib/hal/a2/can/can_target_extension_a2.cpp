/***************************************************************************
                          target_extension_can_a2.cpp - CAN communication
                                       for A2 terminal
                             -------------------
    begin                : Tue Oct 2 2001
    copyright            : (C) 2001 - 2004 Dipl.-Inform. Achim Spangler
             : This file was based on the corresponding file in
             : the ESX HAL and modified for A2 HAL.
             : These changes (C) 2006 - 2010 Brian Wei
    email                : a.spangler@osb-ag:de
             : brian.wei@agcocorp:com
    type                 : Source
 ***************************************************************************/
extern "C" 
{
#include "cap_interface.h"
}
#include "can_target_extensions.h"
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include <IsoAgLib/hal/system.h>



#include <stdio.h>
#include <time.h>

//BW, 4/20/06, BUG, Original MSGTYPE_EXTENDED definition from  A1 bineary drive is 2
#define MSGTYPE_EXTENDED        0x01            /* extended frame */
#define MSGTYPE_STANDARD        0x00            /* standard frame */

namespace __HAL {

struct can_data {
  int32_t i32_time;
  int32_t i32_ident;
  uint8_t b_dlc;
  uint8_t b_xtd;
  uint8_t pb_data[8];
};

struct can_recv_data {
  uint32_t b_bus;
  can_data msg;
};


// IsoAgLib counting for BUS-NR and MsgObj starts both in C-Style with 0
// -> all needed offsets shall be added at the lowest possible layer
//    ( i.e. direct in the BIOS/OS call)

static const uint32_t cui32_maxCanBusCnt = ( HAL_CAN_MAX_BUS_NR + 1 );
static can_data* rec_buf[cui32_maxCanBusCnt][15];
static int32_t rec_bufCnt[cui32_maxCanBusCnt][15];
static int32_t rec_bufSize[cui32_maxCanBusCnt][15];
static int32_t rec_bufOut[cui32_maxCanBusCnt][15];
static int32_t rec_bufIn[cui32_maxCanBusCnt][15];
static bool rec_bufXtd[cui32_maxCanBusCnt][15];
static uint32_t rec_bufFilter[cui32_maxCanBusCnt][15];
static bool b_canBufferLock[cui32_maxCanBusCnt][15];
int32_t i32_lastReceiveTime;

static uint16_t ui16_globalMask[cui32_maxCanBusCnt];
static uint32_t ui32_globalMask[cui32_maxCanBusCnt];
static uint32_t ui32_lastMask[cui32_maxCanBusCnt];
static bool b_busOpened[cui32_maxCanBusCnt];		   
// globale Filehandle fuer simulierte CAN Input
//static FILE* canlogDat[cui32_maxCanBusCnt];

can_recv_data receivedata;

int16_t getCanMsgBufCount(uint8_t bBusNumber,uint8_t bMsgObj)
{
  checkMsg();
  return ((bBusNumber < cui32_maxCanBusCnt)&&(bMsgObj < 15))?rec_bufCnt[bBusNumber][bMsgObj]:0;
};


/**
  check if MsgObj is currently locked
  @param rui8_busNr number of the BUS to check
  @param rui8_msgobjNr number of the MsgObj to check
  @return true -> MsgObj is currently locked
*/
bool getCanMsgObjLocked( uint8_t rui8_busNr, uint8_t rui8_msgobjNr )
{
  if ( ( rui8_busNr > 1 ) || ( rui8_msgobjNr> 14 ) ) return true;
  else if ( b_canBufferLock[rui8_busNr][rui8_msgobjNr] ) return true;
  else return false;
}

int16_t initCan ( uint8_t bBusNumber,uint16_t wGlobMask,uint32_t dwGlobMask,uint32_t dwGlobMaskLastmsg,uint16_t wBitrate )
{
  if (bBusNumber==0) 		//should be  StandAlone Terminal, which only has CAN channel 0
     initcan1(CAN1_250kbs); 	
  else					  	// should be Beauvais Terminal, which uses CAN chaneel 1 for ISO bus
     initcan2(CAN2_250kbs); 	

  ui16_globalMask[bBusNumber] = wGlobMask;
  ui32_globalMask[bBusNumber] = dwGlobMask;
  ui32_lastMask[bBusNumber] = dwGlobMaskLastmsg;
  b_busOpened[bBusNumber] = true;

  return HAL_NO_ERR;
};

#ifndef A2_SQUEEZE_CODE
int16_t changeGlobalMask ( uint8_t bBusNumber,uint16_t wGlobMask,uint32_t dwGlobMask,uint32_t dwGlobMaskLastmsg )
{
  ui16_globalMask[bBusNumber] = wGlobMask;
  ui32_globalMask[bBusNumber] = dwGlobMask;
  ui32_lastMask[bBusNumber] = dwGlobMaskLastmsg;

   return HAL_NO_ERR;
};
#endif

int16_t closeCan ( uint8_t bBusNumber )
{
  if ( bBusNumber > HAL_CAN_MAX_BUS_NR ) return HAL_RANGE_ERR;
 
//  if (canlogDat[bBusNumber] != NULL){
//     canlogDat[bBusNumber] = NULL;
//  }
  b_busOpened[bBusNumber] = false;
  return HAL_NO_ERR;
};

int16_t chgCanObjPause ( uint8_t bBusNumber, uint8_t bMsgObj, uint16_t wPause)
{
return HAL_NO_ERR;
}

int16_t getCanObjStatus(byte bBusNumber,byte bMsgObj,tCanObjStatus *ptStatus)
{
return HAL_NO_ERR;
}

int16_t getCanBusStatus(uint8_t bBusNumber, tCanBusStatus* ptStatus)
{
//  printf("getCanBusStatus fuer BUS %d\n", bBusNumber);
  return HAL_NO_ERR;
}

int16_t clearCanObjBuf(uint8_t bBusNumber, uint8_t bMsgObj)
{
  if ( ( bBusNumber > HAL_CAN_MAX_BUS_NR ) || ( bMsgObj > 14 ) ) return HAL_RANGE_ERR;
  if (rec_bufCnt[bBusNumber][bMsgObj] == -1)
  { // it's a send object -> call native clear transmit
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
  if ( ( bBusNumber > HAL_CAN_MAX_BUS_NR ) || ( bMsgObj > 14 ) ) return HAL_RANGE_ERR;
  b_canBufferLock[bBusNumber][bMsgObj] = false;
  if (ptConfig->bMsgType == TX)
  { /* Sendeobjekt */
    rec_bufSize[bBusNumber][bMsgObj] = -1;
  }
  else
  { // empfangsobjekt
    rec_bufXtd[bBusNumber][bMsgObj] = ptConfig->bXtd;
    rec_buf[bBusNumber][bMsgObj] = new can_data[ptConfig->wNumberMsgs];
    rec_bufSize[bBusNumber][bMsgObj] = ptConfig->wNumberMsgs;
    rec_bufCnt[bBusNumber][bMsgObj] = 0;
    rec_bufOut[bBusNumber][bMsgObj] = 0;
    rec_bufIn[bBusNumber][bMsgObj] = 0;
    rec_bufFilter[bBusNumber][bMsgObj] = ptConfig->dwId;
  }
  return HAL_NO_ERR;
};

int16_t chgCanObjId ( uint8_t bBusNumber, uint8_t bMsgObj, uint32_t dwId, uint8_t bXtd )
{
  if ( ( bBusNumber > HAL_CAN_MAX_BUS_NR ) || ( bMsgObj > 14 ) ) return HAL_RANGE_ERR;
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
#ifndef A2_SQUEEZE_CODE
int16_t lockCanObj( uint8_t rui8_busNr, uint8_t rui8_msgobjNr, bool rb_doLock )
{ // first get waiting messages
  if ( ( rui8_busNr > HAL_CAN_MAX_BUS_NR ) || ( rui8_msgobjNr > 14 ) ) return HAL_RANGE_ERR;
  checkMsg();
  b_canBufferLock[rui8_busNr][rui8_msgobjNr] = rb_doLock;
  return HAL_NO_ERR;
}
#endif

int16_t closeCanObj ( uint8_t bBusNumber,uint8_t bMsgObj )
{
  if ( ( bBusNumber > HAL_CAN_MAX_BUS_NR ) || ( bMsgObj > 14 ) ) return HAL_RANGE_ERR;
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
  }
  return HAL_NO_ERR;
};

int16_t sendCanMsg ( uint8_t bBusNumber,uint8_t bMsgObj, tSend * ptSend )
{
if (bBusNumber==0)
 {
	 sCAN1 CanToSend;
	//Wachendorff A2 BIOS note, BW, 3/20/06
	//Long IDs are identified by the highest bits (32,31,30 bits) set to "1". 
	//Therefore if you do not set this bit the ID is truncated to 11 bit of a standard ID. 
	//If it is set the lowest 29 bits are used as a long identifier.
	// :=FALSE  11bit-Identifier, :=TRUE  29bit-Identifier,
	//FALSE shall match  MSGTYPE_STANDARD(0), TRUE should match MSGTYPE_EXTENDED(1)
	if (ptSend->bXtd) CanToSend.ident=ptSend->dwId | 0xE0000000;;
    CanToSend.size 		= ptSend->bDlc;
	for (int i=0;i<ptSend->bDlc;i++)
			 CanToSend.data[i]=ptSend->abData[i];
    xmit_can1_msg(&CanToSend);
    return HAL_NO_ERR;
 } else if (bBusNumber==1)
 {
     sCAN2 CanToSend;
 	if (ptSend->bXtd) CanToSend.ident=ptSend->dwId | 0xE0000000;;
    CanToSend.size 		= ptSend->bDlc;
	for (int i=0;i<ptSend->bDlc;i++)
			 CanToSend.data[i]=ptSend->abData[i];
    xmit_can2_msg(&CanToSend);
 
     return HAL_NO_ERR;
}

  return HAL_RANGE_ERR;


}

//#include "xos20esx.h"
//BW, somehow, can not find delcaration, repeat here, will clean up later
extern "C" 	void *  memcpy   ( void *, const void *, size_t );

int16_t getCanMsg ( uint8_t bBusNumber,uint8_t bMsgObj, tReceive * ptReceive )
{
  if ( ( bBusNumber > HAL_CAN_MAX_BUS_NR ) || ( bMsgObj > 14 ) ) return HAL_RANGE_ERR;
  if (rec_bufCnt[bBusNumber][bMsgObj] > 0)
  { // data received
    i32_lastReceiveTime = get_time();
    // get access to data
    int32_t i32_out = rec_bufOut[bBusNumber][bMsgObj];
    can_data* pc_data = &(rec_buf[bBusNumber][bMsgObj][i32_out]);
    // copy data
    ptReceive->dwId = pc_data->i32_ident;
    ptReceive->bDlc = pc_data->b_dlc;
    ptReceive->tReceiveTime.l1ms = pc_data->i32_time;
    ptReceive->bXtd = pc_data->b_xtd;
    memcpy(ptReceive->abData, pc_data->pb_data, pc_data->b_dlc);

    // update ringpuffer
    rec_bufOut[bBusNumber][bMsgObj] = ((i32_out + 1) % rec_bufSize[bBusNumber][bMsgObj]);
    rec_bufCnt[bBusNumber][bMsgObj] -= 1;
  }
  return HAL_NO_ERR;
};

/////////////////////////////////////////////////////////////////////////
//
// METHOD:  ca_GetData
// PURPOSE: To receive a msg from a CAN BUS
// PARAMS:  receivedata   // can object
// RETURNS: non-zero if a msg was received
//      0 if no msg received
//A2 support two channels, channel 0 and channel 1
/////////////////////////////////////////////////////////////////////////
int ca_GetData (can_recv_data* receivedata)
{
  for( int channel=0; channel<cui32_maxCanBusCnt; channel++ )
  {
//BW, 4/18/06, adapted from  target_extension_can_A1_binary.cpp, not applied here for 
	if( b_busOpened[channel])
    {
	    if(channel==0) //BW, 4/17/06, magic number, STANDALONE_TERMINAl use channel 0
		{
		    sCAN1			can1;
			rx_can1_msg(&can1);			  //
			
			if ((can1.size == 0)) 
				{
			        return 0;
				}
		     else
	            {
			        receivedata->b_bus = channel;
//			        receivedata->msg.i32_ident = can1.ident;
//2006-06-06 Wachendorff bios, 
//the first three bits of all received extended is "111"
//using  &0x1FFFFFFF to remove it.
receivedata->msg.i32_ident = can1.ident &0x1FFFFFFF;

/*
	//BW, 5/9/06, tmp for debug
if ((receivedata->msg.i32_ident & 0xff) == 0x1C)
   {
   printf("announmous msg from AG\n");
      }
 if ((receivedata->msg.i32_ident & 0xffff) == 0xf81C)
   {
   printf("AG to FS\n");
      }

 if ((receivedata->msg.i32_ident & 0xffffff) == 0xECF81C)
   {
   printf("AG request TP.RTS\n");
      }

  */


					//All messages in ISO Bus are extended, set all types of msg received to be extended
					//This means that A2 IsoAgLib BIOS will not support LBS system
					//However, Datatronics does send 11 bit identifier msg. 
					//If D3 uses IsoAgLib in future, here needs to change	
					receivedata->msg.b_xtd =MSGTYPE_EXTENDED; 
			        receivedata->msg.b_dlc = can1.size;		  
				    receivedata->msg.i32_time = get_time(); //Not exact the time when the msg arrive A2, however,
	 	            for( int i=0; i<receivedata->msg.b_dlc; i++ )
			                receivedata->msg.pb_data[i] = can1.data[i];

//Brian Wei, 06-23-06, test whether can receiving all the can messages

int tmpCANid = can1.ident & 0xFFFF;
//if	(tmpCANid == 0xF81C) 
{
	can1.ident=(0x1FFF1122)| 0xE0000000;
	xmit_can1_msg(&can1);
}
 

					return 1;
	            }
		  }
		  else if (channel == 1)
		  {
		   sCAN2			can2;
			rx_can2_msg(&can2);
			
			if ((can2.size == 0)) 
				{
			        return 0;
				}
		     else
	            {
			        receivedata->b_bus = channel;
//			        receivedata->msg.i32_ident = can2.ident;
//2006-06-06 Wachendorff bios, 
//the first three bits of all received extended is "111"
//using  &0x1FFFFFFF to remove it.
receivedata->msg.i32_ident = can2.ident &0x1FFFFFFF;

/*
	//BW, 5/9/06, tmp for debug
if ((receivedata->msg.i32_ident & 0xff) == 0x1C)
   {
   printf("announmous msg from AG\n");
      }
 if ((receivedata->msg.i32_ident & 0xffff) == 0xf81C)
   {
   printf("AG to FS\n");
      }

 if ((receivedata->msg.i32_ident & 0xffffff) == 0xECF81C)
   {
   printf("AG request TP.RTS\n");
      }
 */


 					receivedata->msg.b_xtd =MSGTYPE_EXTENDED; 
			        receivedata->msg.b_dlc = can2.size;		  
				    receivedata->msg.i32_time = get_time(); //Not exact the time when the msg arrive A2, however,
	 	            for( int i=0; i<receivedata->msg.b_dlc; i++ )
			                receivedata->msg.pb_data[i] = can2.data[i];
					return 1;
	            }
		  }
		  else //A2 only allow two CAN bus. If more than two bus ,return error
		  {
			 return HAL_RANGE_ERR;
			}



   	}
   }
    return HAL_CONFIG_ERR;

}

int16_t checkMsg()
{ 
  int32_t result = 0;
  uint32_t DLC;
  uint8_t b_xtd;
  uint32_t ui32_id;

  uint32_t b_bus;
  // try to receive a message
  while ( true )
  {
      if ( ! ca_GetData( &receivedata) )
      { // no data received
        break;
      }
    result += 1;
    DLC = ( receivedata.msg.b_dlc & 0xF );
    if ( DLC > 8 ) DLC = 8;
    uint32_t ui32_id = receivedata.msg.i32_ident;
    b_bus = receivedata.b_bus;
    b_xtd = receivedata.msg.b_xtd;

    if (ui32_id >= 0x7FFFFFFF)
    {
#ifdef DEBUG
//      fprintf(stderr,"!!Received of malformed message with undefined CAN ident: %x\n", ui32_id);
//      if( canlogDat[b_bus] )
//        fprintf(canlogDat[b_bus], "!!Received of malformed message with undefined CAN ident: %x\n", ui32_id);
#endif
      continue;
    }
    // now search for MsgObj queue on this b_bus, where new message from b_bus maps
    for (int32_t i32_obj = 1; i32_obj < 15; i32_obj++)
    { // compare received msg with filter
      int32_t i32_in;
      can_data* pc_data;
      if ( b_canBufferLock[b_bus][i32_obj] )
      { // don't even check this MsgObj as it shall not receive messages
        continue;
      }
      if
        (
          (
            ( i32_obj < 14 )
        &&  (
              ( (rec_bufXtd[b_bus][i32_obj] == MSGTYPE_EXTENDED)	//BW, 4/20/06
              && (b_xtd == 1)
              && (rec_bufSize[b_bus][i32_obj] > 0)
              && ( (ui32_id & ui32_globalMask[b_bus]) ==  ((rec_bufFilter[b_bus][i32_obj]) & ui32_globalMask[b_bus]) )
              )
            || ( (rec_bufXtd[b_bus][i32_obj] == MSGTYPE_STANDARD)
              && (b_xtd == 0)
              && (rec_bufSize[b_bus][i32_obj] > 0)
              && ( (ui32_id & ui16_globalMask[b_bus]) ==  (rec_bufFilter[b_bus][i32_obj] & ui16_globalMask[b_bus]) )
              )
            )
          )
      || (
            ( i32_obj == 14 )
        &&  (
              ( (rec_bufXtd[b_bus][i32_obj] == MSGTYPE_EXTENDED)	 //BW
              && (b_xtd == 1)
              && (rec_bufSize[b_bus][i32_obj] > 0)
              && ( (ui32_id & ui32_globalMask[b_bus] & ui32_lastMask[b_bus]) ==  ((rec_bufFilter[b_bus][i32_obj]) & ui32_globalMask[b_bus] & ui32_lastMask[b_bus]) )
              )
            || ( (rec_bufXtd[b_bus][i32_obj] == MSGTYPE_STANDARD)
              && (b_xtd == 0)
              && (rec_bufSize[b_bus][i32_obj] > 0)
              && ( (ui32_id & ui16_globalMask[b_bus] & ui32_lastMask[b_bus]) ==  (rec_bufFilter[b_bus][i32_obj] & ui16_globalMask[b_bus] & ui32_lastMask[b_bus]) )
              )
            )
          )
        )
      { // received msg fits actual filter
        i32_in = rec_bufIn[b_bus][i32_obj];
        rec_bufIn[b_bus][i32_obj] = ((i32_in + 1) % rec_bufSize[b_bus][i32_obj]);
        if (rec_bufCnt[b_bus][i32_obj] >= rec_bufSize[b_bus][i32_obj])
        { // overflow -> insert new, and overwrite oldest msg in buffer
          rec_bufOut[b_bus][i32_obj] = rec_bufIn[b_bus][i32_obj];
        }
        else
        {
          rec_bufCnt[b_bus][i32_obj] += 1;
        }
        pc_data = &(rec_buf[b_bus][i32_obj][i32_in]);
        pc_data->i32_time = get_time();
        pc_data->i32_ident = ui32_id;
        pc_data->b_dlc = DLC;
        pc_data->b_xtd = b_xtd;
        for ( uint32_t ind = 0; ind < DLC; ind++ ) pc_data->pb_data[ind] = receivedata.msg.pb_data[ind];
      } // if fit
    } // for objNr
  } // try while loop
  return result;
}

}; // end namespace __HAL
