#ifndef LINUX_RS232_H
#define LINUX_RS232_H

#include <IsoAgLib/hal/pc/config.h>
#include <IsoAgLib/hal/pc/typedef.h>

#include <fcntl.h>      /* for open()/read()/write() */
#include <stdio.h>      /* for fgets() */
#include <stdlib.h>  /* for atexit() */
#include <termios.h>    /* for tcgetattr()/tcsetattr() */
#include <unistd.h>     /* for close() */



int16_t SioInit(uint32_t comport, uint32_t baudrate);
int16_t SioPutBuffer(uint8_t *p, int16_t n);
int16_t SioRecPuffCnt();
int16_t SioGetByte(uint8_t *p);
int16_t SioGetString(uint8_t *p, uint8_t len);
int8_t *KeyGetString(char *buffer, int16_t len);

#endif
