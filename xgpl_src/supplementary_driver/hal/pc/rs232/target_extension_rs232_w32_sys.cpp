#include "target_extension_rs232_w32_sys.h"

#include <stdio.h>      /* for fgets()  */
#include <stdlib.h>     /* for atexit() */
#include <conio.h>      /* for KeyXXX() */
#include <windows.h>    /* for SioXXX() */

HANDLE hCom[RS232_INSTANCE_CNT];
std::deque<int8_t> deq_readPuff[RS232_INSTANCE_CNT];

int8_t c_read;
static uint32_t configuredComport = 0xFFFF;


void SioExit(uint32_t comport) {
  if ( comport >= RS232_INSTANCE_CNT ) return 0;
    CloseHandle(hCom[comport]);
}

void SioExit() {
  if ( configuredComport >= RS232_INSTANCE_CNT ) return 0;
    CloseHandle(hCom[configuredComport]);
}

int16_t SioInit(uint32_t comport, uint32_t baudrate) {
  if ( comport >= RS232_INSTANCE_CNT ) return 0;
  DCB dcb;
  COMMTIMEOUTS ct;
  uint8_t com[] = "COMx";

  com[3] = comport + 1 + '0';
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

  // reset read puffer
  deq_readPuff[comport].clear();

  return 1;
}

int16_t SioPutBuffer(uint32_t comport, const uint8_t *p, int16_t n) {
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

  for ( uint16_t ind = 0; ind < tempLen; ind++ ) deq_readPuff[comport].push_back( c_temp[ind] );

  return deq_readPuff[comport].size();
}

int16_t SioGetString(uint32_t comport, uint8_t *p, uint8_t len)
{
  if ( comport >= RS232_INSTANCE_CNT ) return 0;
  SioRecPuffCnt(comport);
  if (deq_readPuff[comport].size() > 0)
  {
    uint16_t ind = 0;
    for ( ; ind < len; ind++ )
    {
      p[ind] = deq_readPuff[comport].front();
      deq_readPuff[comport].pop_front();
    }
    // terminate
    p[ind] = '\0';
    return 1;
  }
  else
  {
    return 0;
  }
}

int16_t SioGetTerminatedString(uint32_t comport, uint8_t *p, uint8_t ui8_terminateChar, uint8_t len)
{
  if ( comport >= RS232_INSTANCE_CNT ) return 0;
  SioRecPuffCnt(comport);
  if (deq_readPuff[comport].size() > 0)
  {
    for ( std::deque<int8_t>::iterator iter = deq_readPuff[comport].begin(); iter != deq_readPuff[comport].end(); iter++ )
    { // check if terminating char is found
      if ( *iter == ui8_terminateChar )
      { // found -> copy area from begin to iterator
        uint16_t ind = 0;
        for ( ; ( ( deq_readPuff[comport].front() != ui8_terminateChar) && ( ind < len ) ); ind++ )
        {
          p[ind] = deq_readPuff[comport].front();
          deq_readPuff[comport].pop_front();
        }
        // lastly pop the termination char and terminate the result string
        deq_readPuff[comport].pop_front();
        p[ind] = '\0';
        return HAL_NO_ERR;
      }
    }
  }
  return HAL_NOACT_ERR;
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

