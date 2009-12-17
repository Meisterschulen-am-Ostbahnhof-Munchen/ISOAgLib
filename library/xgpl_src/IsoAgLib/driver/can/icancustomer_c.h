/*
  icancustomer_c.h: header file for basic object for customer
    relationship to one or more FilterBox_c (-> CAN IO) instances

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef ICAN_CUSTOMER_H
#define ICAN_CUSTOMER_H

#include "impl/cancustomer_c.h"
#include <IsoAgLib/comm/Part3_DataLink/icanpkgext_c.h>

namespace __IsoAgLib
{ // forward declarations
  class ProprietaryMessageHandler_c;
}

// Begin Namespace IsoAgLib
namespace IsoAgLib {

/** Base class for CAN customer classes
  -> FilterBox_c calls member function
    processMsg() to initiate processing of new received CAN telegram,
    which suits the configured filter/mask combination
  ==> all CAN handling object, which uses the services of FilterBox_c, CAN_IO
      should be derived from this class
  @see __IsoAgLib::CAN_IO
  @see __IsoAgLib::FilterBox
  *@author Dipl.-Inform. Achim Spangler
  */
class iCanCustomer_c : private __IsoAgLib::CanCustomer_c
{
public:
  /**
    virtual function which delivers a pointer to the iCANCustomer
    specific iCanPkgExt_c instance
  */
  virtual iCanPkgExt_c& iDataBase()=0;

  /**
    process a message -> the specialized/derived version of this virtual
    function is called during processing of received CAN telegrams in CanIo_c::processMsg
    @see __IsoAgLib::CanIo_c::processMsg
  */
  virtual bool processMsg() { return CanCustomer_c::processMsg();}

 /**
    process a message -> the specialized/derived version of this virtual
    function can be called during processing of received CAN telegrams in CanIo_c::processMsg
    @see __IsoAgLib::CanIo_c::processMsg
  */
  virtual bool processInvalidMsg() { return CanCustomer_c::processInvalidMsg(); }

  virtual bool isNetworkMgmt() const { return CanCustomer_c::isNetworkMgmt(); }


private:
  /**
    virtual function which delivers a pointer to the CANCustomer
    specific CanPkgExt_c instance
  */
  virtual __IsoAgLib::CanPkgExt_c& dataBase() { return static_cast<__IsoAgLib::CanPkgExt_c&>(iDataBase()); }

  friend class iCanIo_c;
  friend struct iIsoFilter_s;
  friend class iMultiReceive_c;
  friend class __IsoAgLib::ProprietaryMessageHandler_c;
};

/** this typedef is only for some time to provide backward compatibility at API level */
typedef iCanCustomer_c iCANCustomer_c;

} // namespace
#endif
