#ifndef WIN32_RS232_H
#define WIN32_RS232_H
#include <IsoAgLib/hal/pc/config.h>
#include <IsoAgLib/hal/pc/typedef.h>

int16_t SioInit(uint32_t comport, uint32_t baudrate);
int16_t SioPutBuffer(uint8_t *p, int16_t n);
int16_t SioRecPuffCnt();
int16_t SioGetByte(uint8_t *p);
int16_t SioGetString(uint8_t *p, uint8_t len);
int8_t *KeyGetString(int8_t *buffer, int16_t len);


#endif
