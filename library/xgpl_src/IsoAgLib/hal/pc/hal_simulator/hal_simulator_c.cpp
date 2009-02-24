/***************************************************************************
                          hal_simulator_c.cpp - Base class for the HAL callback functions
                                      Derive from this class to override the callback
                                      functions for the Hardware Abstraction layer.
                             -------------------
    begin                :  Feb 16, 2009
    copyright            : (C) 2009 AGCO Corporation
    Author               : Mike Schmidt
    email                : Mike.Schmidt@AgcoCorp.com
 ***************************************************************************/

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include "HALSimulator_c.h"

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
////////////////////////////////////////////////////////////////////////////////

// static variables
HALSimulator_c* HALSimulator_c::theHALSimulator_c = NULL;
