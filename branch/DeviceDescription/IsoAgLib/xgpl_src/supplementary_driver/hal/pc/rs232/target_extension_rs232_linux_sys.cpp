#include "target_extension_rs232_linux_sys.h"
#include <cstdio>
#include <cstdlib>
#include <cctype>
#include <ctime>
#include <cstring>
#include <deque>

struct T_BAUD { uint32_t rate; uint32_t flag; } t_baud[] = {
  {    600L, B600    }, {   1200L, B1200   }, {   2400L, B2400   },
  {   4800L, B4800   }, {   9600L, B9600   }, {  19200L, B19200  },
  {  38400L, B38400  }, {  57600L, B57600  }, { 115200L, B115200 } };



struct termios t_0;
struct termios t_com;
int16_t f_com[RS232_INSTANCE_CNT];
std::deque<int8_t> deq_readPuff[RS232_INSTANCE_CNT];


#define false 0
#define true 1

int8_t c_read;
boolean b_getRead;


void SioExit(uint32_t comport)
{
  tcsetattr(0, TCSANOW, &t_0);
  tcsetattr(0, TCSANOW, &t_com);
  close(f_com[comport]);
}

int16_t SioInit(uint32_t comport, uint32_t baudrate)
{
  if ( comport >= RS232_INSTANCE_CNT ) return 0;
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

  com[9] = comport+'0';
  if ((f_com[comport] = open(com, O_RDWR|O_NDELAY)) < 0) return 0;
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
  tcsetattr(f_com[comport], TCSANOW, &t);

  // wait some time to avoid buffer error
  i32_time = (clock()/(CLOCKS_PER_SEC/1000));
  i32_time_2 = (clock()/(CLOCKS_PER_SEC/1000));
  while ((i32_time + 500) > i32_time_2)
    i32_time_2 = (clock()/(CLOCKS_PER_SEC/1000));

	// reset read puffer
	deq_readPuff[comport].clear();

  return 1;
}

int16_t SioPutBuffer(uint32_t comport, uint8_t *p, int16_t n)
{
  if ( comport >= RS232_INSTANCE_CNT ) return 0;
  n = write(f_com[comport], p, n) == n;
  tcdrain(f_com[comport]);
  return n;
}

int16_t SioRecPuffCnt(uint32_t comport)
{
  if ( comport >= RS232_INSTANCE_CNT ) return 0;
  int8_t c_temp[300];
  int16_t tempLen = read(f_com[comport], c_temp, (299));

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

