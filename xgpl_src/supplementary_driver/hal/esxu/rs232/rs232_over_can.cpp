/***************************************************************************
                          rs232_over_can.cpp - definition of HAL
                                    namesapce layer for ESXu
                             -------------------
    begin                : Fri Aug 26 2005
    copyright            : (C) 2005 Michael D. Schmidt
    email                : mike.schmidt@agcocorp:com
 ***************************************************************************/

#include "rs232.h"
#include <IsoAgLib/driver/can/icanio_c.h>

#ifdef USE_RS232_OVER_CAN

using namespace IsoAgLib;  
namespace HAL
{
  /* ****************************** */
  /** \name RS232 I/O BIOS functions */
/*@{*/

  /**
    init the RS232 interface
    @param wBaudrate wnated Baudrate {75, 600, 1200, 2400, 4800, 9600, 19200}
          as configured in <Application_Config/isoaglib_config.h>
    @param bMode one of (DATA_7_BITS_EVENPARITY = 1, DATA_8_BITS_EVENPARITY = 2,
            DATA_7_BITS_ODDPARITY = 3, DATA_8_BITS_ODDPARITY = 4, DATA_8_BITS_NOPARITY = 5)
    @param bStoppbits amount of stop bits (1,2)
    @param bitSoftwarehandshake true -> use xon/xoff software handshake
    @return HAL_NO_ERR -> o.k. else one of settings incorrect
  */

uint8_t RS232_over_can_initialized = false;
uint8_t RS232_over_can_busnum = 1;

int16_t init_rs232(uint16_t wBaudrate,uint8_t bMode,uint8_t bStoppbits,bool bitSoftwarehandshake, uint8_t rui8_channel)
{
	if( !RS232_over_can_initialized )
	{
#if ( CAN_INSTANCE_CNT > 1 )
	getIcanInstance(RS232_over_can_busnum);
#else
	getIcanInstance();
#endif
	if( getLbsErrInstance().good( LibErr_c::Can ) )
		RS232_over_can_initialized = true;
	}

	return RS232_over_can_initialized ? C_NO_ERR : C_RANGE;
};

/**
send single uint8_t on RS232
@param bByte data uint8_t to send
@return HAL_NO_ERR -> o.k. else send puffer overflow
*/
int16_t put_rs232Char(uint8_t bByte, uint8_t rui8_channel)
{
	return put_rs232NChar(&bByte, 1, rui8_channel);
};
/**
send string of n uint8_t on RS232
@param bpWrite pointer to source data string
@param wNumber number of data uint8_t to send
@return HAL_NO_ERR -> o.k. else send puffer overflow
*/
int16_t put_rs232NChar(const uint8_t *bpWrite,uint16_t wNumber, uint8_t rui8_channel)
{
	uint16_t numLeft = wNumber;

	while( numLeft )
		{
#if ( CAN_INSTANCE_CNT > 1 )
		IsoAgLib::iCANIO_c& c_can = IsoAgLib::getIcanInstance(RS232_over_can_busnum);
#else
		IsoAgLib::iCANIO_c& c_can = IsoAgLib::getIcanInstance();
#endif
		IsoAgLib::iCANPkg_c c_sendData;
		c_sendData.setIdent( 0x7F0+rui8_channel, __IsoAgLib::Ident_c::StandardIdent );
		c_sendData.ui8_len = (numLeft < 8 ? numLeft : 8);
		CNAMESPACE::memcpy( &c_sendData.pb_data[0], &bpWrite[wNumber-numLeft], c_sendData.ui8_len );
		numLeft -= c_sendData.ui8_len;

	    c_can << c_sendData;
		}

	return getLbsErrInstance().good( LibErr_c::Can );
};
/**
send '\0' terminated string on RS232
@param pbString pointer to '\0' terminated (!) source data string
@return HAL_NO_ERR -> o.k. else send puffer overflow
*/
int16_t put_rs232String(const uint8_t *pbString, uint8_t rui8_channel)
{
	return put_rs232NChar((uint8_t*)pbString, CNAMESPACE::strlen( (char*)pbString ), rui8_channel);
};
}
#endif
