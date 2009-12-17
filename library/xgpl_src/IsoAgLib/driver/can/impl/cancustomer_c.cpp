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
  process a message -> the specialized/derived version of this virtual
  function is called during processing of received CAN telegrams in CanIo_c::processMsg
  <!--@param apc_box pointer to the FilterBox_c instances which received the telegram (i.e. which has the telegram in its buffer)-->
  @see __IsoAgLib::CanIo_c::processMsg
  @return true -> message was processed; else the received CAN message will be served to other matching CanCustomer_c
*/
bool CanCustomer_c::processMsg()
{ // dummy function - just to allow classes to (indirectly) derive from
  // CanCustomer_c without the need to implement this function
  return false;
}

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
