/*
  cancustomer_c.cpp: implementation file for basic object for customer
    relationship to one or more FilterBox_c (-> CAN IO) instances

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
#include "cancustomer_c.h"
#include <IsoAgLib/comm/Part5_NetworkManagement//iisoname_c.h>



namespace __IsoAgLib {

/**
  virtual function which delivers a pointer to the CANCustomer
  specific CanPkgExt_c instance
    YOU SHOULD NEVER CALL THIS BASE FUNCTION!
*/
CanPkgExt_c& CanCustomer_c::dataBase()
{ // dummy function - just to allow classes to (indirectly) derive from
  // CanCustomer_c without the need to implement this function
  static CanPkgExt_c c_dummyPkg;
  return c_dummyPkg;
}

/** virtual destructor */
CanCustomer_c::~CanCustomer_c()
{ // dummy function - just to allow classes to (indirectly) derive from
  // CanCustomer_c without the need to implement this function
}

} // end of namespace __IsoAgLib
