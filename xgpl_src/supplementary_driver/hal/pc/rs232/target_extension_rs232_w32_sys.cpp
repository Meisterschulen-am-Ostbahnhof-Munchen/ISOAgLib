#include "target_extension_rs232_w32_sys.h"

#include <stdio.h>      /* for fgets()  */
#include <stdlib.h>     /* for atexit() */
#include <conio.h>      /* for KeyXXX() */
#include <windows.h>    /* for SioXXX() */

HANDLE hCom[RS232_INSTANCE_CNT];

int8_t c_read;
int8_t pc_readPuff[RS232_INSTANCE_CNT][300];
int16_t i16_readPuffCnt[RS232_INSTANCE_CNT];


void SioExit(uint32_t comport) {
  if ( comport >= RS232_INSTANCE_CNT ) return 0;
    CloseHandle(hCom[comport]);
}

int16_t SioInit(uint32_t comport, uint32_t baudrate) {
  if ( comport >= RS232_INSTANCE_CNT ) return 0;
    DCB dcb;
    COMMTIMEOUTS ct;
    uint8_t com[] = "COMx";

    com[3] = comport + '0';
    hCom[comport] = CreateFile(com,GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,0,NULL);
    if (!hCom[comport]) return 0;

    atexit(SioExit);

    if (!SetupComm(hCom[comport],1024,1024)) return 0;
    if (!GetCommState(hCom[comport],&dcb)) return 0;
    dcb.BaudRate    = baudrate;
    dcb.ByteSize    = 8;
    dcb.Parity      = NOPARITY;
    dcb.StopBits    = ONESTOPBIT;
    dcb.fRtsControl = RTS_CONTROL_ENABLE;
    if (!SetCommState(hCom[comport],&dcb)) return 0;
    if (!GetCommTimeouts(hCom[comport],&ct)) return 0;
    ct.ReadIntervalTimeout = MAXDWORD;
    ct.ReadTotalTimeoutMultiplier = 0;
    ct.ReadTotalTimeoutConstant = 0;
    ct.WriteTotalTimeoutMultiplier = 0;
    ct.WriteTotalTimeoutConstant = 0;
    if (!SetCommTimeouts(hCom[comport],&ct)) return 0;

    // wait some time to avoid buffer error
    i32_time = (clock()/(CLOCKS_PER_SEC/1000));
    i32_time_2 = (clock()/(CLOCKS_PER_SEC/1000));
    while ((i32_time + 500) > i32_time_2)
      i32_time_2 = (clock()/(CLOCKS_PER_SEC/1000));

    return 1;
}

int16_t SioPutBuffer(uint32_t comport, uint8_t *p, int16_t n) {
  if ( comport >= RS232_INSTANCE_CNT ) return 0;
    DWORD x;
    return WriteFile(hCom[comport],p,n,&x,NULL);
}

int16_t SioRecPuffCnt(uint32_t comport)
{
  if ( comport >= RS232_INSTANCE_CNT ) return 0;
  int8_t c_temp[300];
  DWORD tempLen;
  ReadFile(hCom[comport],c_temp,299,&tempLen,NULL);

  memcpy((pc_readPuff[comport]+i16_readPuffCnt[comport]), c_temp, tempLen);
  i16_readPuffCnt[comport] += tempLen;

  return i16_readPuffCnt[comport];
}

int16_t SioGetString(uint32_t comport, uint8_t *p, uint8_t len)
{
  if ( comport >= RS232_INSTANCE_CNT ) return 0;
  SioRecPuffCnt(comport);
  if (i16_readPuffCnt[comport] >= len)
  {
    memcpy(p, pc_readPuff[comport], len);
    i16_readPuffCnt[comport] -= len;
    memcpy(pc_readPuff[comport], pc_readPuff[comport]+len, i16_readPuffCnt[comport]);
    return 1;
  }
  else
  {
    return 0;
  }
}

int16_t SioGetByte(uint32_t comport, uint8_t *p)
{
  return SioGetString(comport, p, 1);
}


int16_t SioGetByte(uint32_t comport, uint8_t *p) {
  if ( comport >= RS232_INSTANCE_CNT ) return 0;
    DWORD x;
    if (!ReadFile(hCom[comport],p,1,&x,NULL)) return 0;
    return x > 0;
}

int8_t *KeyGetString(int8_t *buffer, int16_t len)
{
  return fgets(buffer, len, stdin);
}

