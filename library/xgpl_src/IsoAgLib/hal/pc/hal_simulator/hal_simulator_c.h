/***************************************************************************
                          hal_simulator_c.h - Base class for the HAL callback functions
                                      Derive from this class to override the callback
                                      functions for the Hardware Abstraction layer.
                             -------------------
    begin                :  Feb 16, 2009
    copyright            : (C) 2009 AGCO Corporation
    Author               : Mike Schmidt
    email                : Mike.Schmidt@AgcoCorp.com
 ***************************************************************************/

#ifndef HALEVENTS_C_H
#define HALEVENTS_C_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include <IsoAgLib/hal/pc/config.h>
#include <IsoAgLib/hal/pc/typedef.h>    // for int16_t etc.

////////////////////////////////////////////////////////////////////////////////
// CBaseMfcHalApp:
//
// IMPORTANT!!!
// Rules for usage:
//   1. You should create EXACTLY ONE of these HALSimulator_c in your project.
//   2. It should be created BEFORE calling any HAL functions
//   3. Mo HAL functions should be called after it is destructed
//
// Example:
//  In .h file
//    class CApplicatorApp : public CWinApp, HALSimulator_c { ... }
//
//  At top of .cpp file
//    static CApplicatorApp theApp;     // The one and only CApplicatorApp object
//

class HALSimulator_c
{
// static variables
    public:
        static HALSimulator_c* theHALSimulator_c;

    public:
	    HALSimulator_c()
	        { theHALSimulator_c = this; }

	    virtual ~HALSimulator_c()
		{ theHALSimulator_c = 0; } // NULL

// HAL Callback functions
// Override these functions to update your screen variables
    public:
	// System
		virtual int16_t getOn_offSwitch()
			{ return true; }	// Return D+ (ignition key) is turned ON

	// Actors
        virtual void eventSetPwmFreq(uint8_t bOutputGroup, uint32_t dwFrequency)
            {}
        virtual void eventSetDigout(uint8_t bOutputNo, uint16_t wPWMValue )
            {}

	// Sensors
		virtual void init_digin( uint8_t bInput,uint8_t bMode,uint8_t bAktivhighlow,void (*pfFunctionName)(...) )
			{}
		virtual void init_analogin( uint8_t bNumber, uint8_t bType )
			{}
		virtual int16_t getDiginOnoff( uint8_t bInputNumber )
			{ return false; }
		virtual void getDiginPeriod( uint8_t bInput, uint16_t *pwPeriod, uint16_t *pwImpulse )
			{}
		virtual void getDiginFreq( uint8_t bInput, uint16_t *pwFrequency )
			{}
		virtual int16_t getAdc( uint8_t bKanalnummer )
			{ return 0; }

	// RS-232
		virtual void init_rs232( uint16_t baudrate,uint8_t bMode,uint8_t bStoppbits,bool bitSoftwarehandshake, uint8_t comport )
			{}
		virtual void write_rs232( uint8_t aui8_channel, const uint8_t * lpBuf, uint16_t dwToWrite )
			{}
		virtual int16_t getRs232RxBufCount( uint8_t comport )
			{ return 0; }
		virtual int16_t getRs232Char( uint8_t aui8_channel, uint8_t *pbRead )
			{ return 0; }

	// CAN
		virtual void SendCanMsg( uint8_t bBusNumber, uint32_t dwTimeStamp, uint8_t bXtd, uint32_t dwId, uint8_t bDlc, uint8_t abData[8] )
			{}
		virtual void ReceiveCanMsg( uint8_t bBusNumber, uint32_t dwTimeStamp, uint8_t bXtd, uint32_t dwId, uint8_t bDlc, uint8_t abData[8] )
			{}

	//
	// Warning: This may not work well on startup / shutdown...
	// We'll have to see how it goes.
	// But, if this class is constructed before any HAL callback functions are called,
	// then we won't need to check the validity of theHALSimulator_c before each callback function call.
	// I think the app is usually created as a global static variable which is constructed before main()
	// So, just multiply derive your app from HALSimulator_c and MFC's CWinApp
	// -Mike Schmidt
	// 
	//
	static HALSimulator_c& GetHALSimulator_c()
	{ return *HALSimulator_c::theHALSimulator_c; }
};

#endif  // HALEVENTS_C_H

