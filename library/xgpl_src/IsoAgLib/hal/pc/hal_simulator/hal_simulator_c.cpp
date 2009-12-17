/*
  hal_simulator_c.cpp: Base class for the HAL callback functions
    Derive from this class to override the callback functions for the
    Hardware Abstraction layer.

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include "hal_simulator_c.h"

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
