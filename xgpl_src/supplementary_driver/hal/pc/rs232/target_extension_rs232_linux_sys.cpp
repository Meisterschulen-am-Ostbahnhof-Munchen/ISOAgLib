#include "target_extension_rs232_linux_sys.h"
#include <cstdio>
#include <cstdlib>
#include <cctype>
#include <ctime>
#include <cstring>

struct T_BAUD { uint32_t rate; uint32_t flag; } t_baud[] = { 
  {    600L, B600    }, {   1200L, B1200   }, {   2400L, B2400   }, 
  {   4800L, B4800   }, {   9600L, B9600   }, {  19200L, B19200  },
  {  38400L, B38400  }, {  57600L, B57600  }, { 115200L, B115200 } };



struct termios t_0;
struct termios t_com;
int16_t f_com;

#define false 0
#define true 1

int8_t c_read;
boolean b_getRead;
int8_t pc_readPuff[300];
int16_t i16_readPuffCnt;


void SioExit(void)
{
  tcsetattr(0, TCSANOW, &t_0);
  tcsetattr(0, TCSANOW, &t_com);
  close(f_com);
}

int16_t SioInit(uint32_t comport, uint32_t baudrate) 
{
  int32_t i32_time = 0,
      i32_time_2 = 0;
  static char com[] = "/dev/ttySx";
  struct termios t;
  struct T_BAUD *b;
  uint32_t  baudflag;

  b_getRead = false;
  b = t_baud;
  do {
    baudflag = b->flag;
    if (b->rate >= baudrate) break;
  } while (++b < t_baud + sizeof t_baud/sizeof *t_baud);

  com[9] = comport-1+'0';
  if ((f_com = open(com, O_RDWR|O_NDELAY)) < 0) return 0;
  if (tcgetattr(0, &t_0)) return 0;
  if (tcgetattr(0, &t_com)) return 0;
  atexit(SioExit);
  
  t = t_0;
  t.c_iflag = 0;
  t.c_oflag = OPOST|ONLCR;
  t.c_lflag = 0;
  t.c_cc[VMIN] = 0;
  t.c_cc[VTIME] = 0;
  tcsetattr(0, TCSANOW, &t);
  
  t = t_com;
  t.c_iflag = 0;
  t.c_oflag = 0;
  t.c_lflag = 0;
  t.c_cflag = CREAD|CS8|CLOCAL|baudflag;
  t.c_cc[VMIN] = 0;
  t.c_cc[VTIME] = 0;
  tcsetattr(f_com, TCSANOW, &t);

  // wait some time to avoid buffer error
  i32_time = (clock()/(CLOCKS_PER_SEC/1000));
  i32_time_2 = (clock()/(CLOCKS_PER_SEC/1000));
  while ((i32_time + 500) > i32_time_2)
    i32_time_2 = (clock()/(CLOCKS_PER_SEC/1000));
  return 1;
}

int16_t SioPutBuffer(uint8_t *p, int16_t n) 
{
  n = write(f_com, p, n) == n;
  tcdrain(f_com);
  return n;
}

int16_t SioRecPuffCnt()
{
  int8_t c_temp[300];
  int16_t tempLen = read(f_com, c_temp, (299));

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

int8_t *KeyGetString(char *buffer, int16_t len)
{
  struct termios t;
  int8_t *p;
  tcgetattr(0, &t);
  tcsetattr(0, TCSAFLUSH, &t_0);
  p = (int8_t*)fgets(buffer, len, stdin);
  tcsetattr(0, TCSAFLUSH, &t);
  return p;
}

