#include "target_extension_rs232_w32_sys.h"

#include <stdio.h>      /* for fgets()  */
#include <stdlib.h>     /* for atexit() */
#include <conio.h>      /* for KeyXXX() */
#include <windows.h>    /* for SioXXX() */

HANDLE hCom;

int8_t c_read;
boolean b_getRead;
int8_t pc_readPuff[300];
int16_t i16_readPuffCnt;


void SioExit(void) {
    CloseHandle(hCom);
}

int16_t SioInit(uint32_t comport, uint32_t baudrate) {
    DCB dcb;
    COMMTIMEOUTS ct;
    uint8_t com[] = "COMx";

    com[3] = comport + '0';
    hCom = CreateFile(com,GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,0,NULL);
    if (!hCom) return 0;

    atexit(SioExit);

    if (!SetupComm(hCom,1024,1024)) return 0;
    if (!GetCommState(hCom,&dcb)) return 0;
    dcb.BaudRate    = baudrate;
    dcb.ByteSize    = 8;
    dcb.Parity      = NOPARITY;
    dcb.StopBits    = ONESTOPBIT;
    dcb.fRtsControl = RTS_CONTROL_ENABLE;
    if (!SetCommState(hCom,&dcb)) return 0;
    if (!GetCommTimeouts(hCom,&ct)) return 0;
    ct.ReadIntervalTimeout = MAXDWORD;
    ct.ReadTotalTimeoutMultiplier = 0;
    ct.ReadTotalTimeoutConstant = 0;
    ct.WriteTotalTimeoutMultiplier = 0;
    ct.WriteTotalTimeoutConstant = 0;
    if (!SetCommTimeouts(hCom,&ct)) return 0;

    // wait some time to avoid buffer error
    i32_time = (clock()/(CLOCKS_PER_SEC/1000));
    i32_time_2 = (clock()/(CLOCKS_PER_SEC/1000));
    while ((i32_time + 500) > i32_time_2)
      i32_time_2 = (clock()/(CLOCKS_PER_SEC/1000));

    return 1;
}

int16_t SioPutBuffer(uint8_t *p, int16_t n) {
    DWORD x;
    return WriteFile(hCom,p,n,&x,NULL);
}

int16_t SioRecPuffCnt()
{
  int8_t c_temp[300];
  DWORD tempLen;
  ReadFile(hCom,c_temp,299,&tempLen,NULL);

  memcpy((pc_readPuff+i16_readPuffCnt), c_temp, tempLen);
  i16_readPuffCnt += tempLen;

  return i16_readPuffCnt;
}

int16_t SioGetString(uint8_t *p, uint8_t len)
{
  SioRecPuffCnt();
  if (i16_readPuffCnt >= len)
  {
    memcpy(p, pc_readPuff, len);
    i16_readPuffCnt -= len;
    memcpy(pc_readPuff, pc_readPuff+len, i16_readPuffCnt);
    return 1;
  }
  else
  {
    return 0;
  }
}

int16_t SioGetByte(uint8_t *p)
{
  return SioGetString(p, 1);
}


int16_t SioGetByte(uint8_t *p) {
    DWORD x;
    if (!ReadFile(hCom,p,1,&x,NULL)) return 0;
    return x > 0;
}

int8_t *KeyGetString(int8_t *buffer, int16_t len)
{
  return fgets(buffer, len, stdin);
}

