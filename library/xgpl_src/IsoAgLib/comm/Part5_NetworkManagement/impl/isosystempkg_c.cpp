/*
  isosystempkg_c.cpp: object for transformation of CAN uint8_t string
    to and from System data structure

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#include "isosystempkg_c.h"
#include <IsoAgLib/driver/can/impl/ident_c.h>

namespace __IsoAgLib {

/** default constructor */
IsoSystemPkg_c::IsoSystemPkg_c() : mc_isoISOName( getDataUnionConst() ) {
}
/** default destructor */
IsoSystemPkg_c::~IsoSystemPkg_c(){
}

/**
  overloaded virtual function to translate the string data into flag values;
  needed for assigning informations from another CanPkg_c or CANPkgExt
  @see CanPkg_c::operator=
  @see CanPkgExt_c::operator=
*/
void IsoSystemPkg_c::string2Flags()
{
  // for ISO use direct read access with isoPgn, indGroup, etc
  mc_isoISOName.inputUnion( getDataUnionConst() );
};

/**
  overloaded virtual function to translate flag values to data string;
  needed for sending informations from this object via CanIo_c on CAN BUS,
  because CanIo_c doesn't know anything about the data format of this type of msg
  so that it can only use an unformated data string from CANPkg
  @see CanPkg_c::getData
  @see CanPkgExt_c::getData
*/
void IsoSystemPkg_c::flags2String()
{ // for ISO use direct write access with setIsoPgn, setIndGroup, etc
};
} // end of namespace __IsoAgLib
