/*
  isosystempkg_c.h: object for transformation of CAN uint8_t string to
    and from System data structure

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef ISO_SYSTEM_PKG_H
#define ISO_SYSTEM_PKG_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include <IsoAgLib/typedef.h>
#include <IsoAgLib/comm/Part3_DataLink/impl/canpkgext_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isoname_c.h>
#include "isoname_c.h"

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

/**
  IsoSystemPkg_c transforms informations from CAN uint8_t string
  to and from data structure for Scheduler_c SystemMgmt_c messages
  @author Dipl.-Inform. Achim Spangler
*/

class IsoSystemPkg_c : public CanPkgExt_c  {
public:
  /** default constructor */
  IsoSystemPkg_c();
  /** default destructor */
  ~IsoSystemPkg_c();
  /*
    ==> OBSOLETE, because now all can-pkg-data is STATIC!
    ==> REACTIVATE if some NON-STATIC member vars will be added!
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    assign operator to insert informations from one CanPkg_c into another
    @see __IsoAgLib::FilterBox_c::operator>>
    @see CanPkgExt_c::operator=
    @see CanPkgExt_c::getData
    @param acrc_right reference to the source CanPkg_c on the right
    @return reference to the source CanPkg_c to enable assign chains like
        "pkg1 = pkg2 = pkg3 = pkg4;"
  virtual const CanPkg_c& operator=(const CanPkg_c& acrc_right) { return CanPkg_c::operator=(acrc_right);};
  */

  /* ************************** */
  /* ***retreiving of values*** */
  /* ************************** */
  /**
    deliver ISOName of sender
    @return ISOName of sender
  */
  const IsoName_c& isoName() const
  { // read on extended ident ISO msg the ISOName from NAME setting
    return mc_isoISOName;
  };
  /* *********************** */
  /* ***setting of values*** */
  /* *********************** */

  /**
    overloaded virtual function to translate the string data into flag values;
    needed for assigning informations from another CanPkg_c or CANPkgExt
    @see CanPkg_c::operator=
    @see CanPkgExt_c::operator=
  */
  virtual void string2Flags();

  virtual bool isNetworkMgmt() const { return true; } ;
protected: // protected methods

  /**
    overloaded virtual function to translate flag values to data string;
    needed for sending informations from this object via CanIo_c on CAN BUS,
    because CanIo_c doesn't know anything about the data format of this type of msg
    so that it can only use an unformated data string from CANPkg
    @see CanPkg_c::getData
    @see CanPkgExt_c::getData
  */
  virtual void flags2String();
  private:
    IsoName_c mc_isoISOName;
};

/** this typedef is only for some time to provide backward compatibility at API level */
typedef IsoSystemPkg_c ISOSystemPkg_c;

}
#endif
