#ifndef WIN32_RS232_H
#define WIN32_RS232_H
#include <IsoAgLib/hal/pc/config.h>
#include <IsoAgLib/hal/pc/typedef.h>

int16_t SioInit(uint32_t comport, uint32_t baudrate);
int16_t SioPutBuffer(uint32_t comport, uint8_t *p, int16_t n);
int16_t SioRecPuffCnt(uint32_t comport);
int16_t SioGetByte(uint32_t comport, uint8_t *p);
int16_t SioGetString(uint32_t comport, uint8_t *p, uint8_t len);
int16_t SioGetTerminatedString(uint32_t comport, uint8_t *p, uint8_t ui8_terminateChar, uint8_t len = 0xFF);
int8_t *KeyGetString(uint32_t comport, int8_t *buffer, int16_t len);


#endif
