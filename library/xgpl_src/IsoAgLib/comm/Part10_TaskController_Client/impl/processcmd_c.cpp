/*
  processcmd_c.cpp: extracts general data from ISO process messages

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include "processcmd_c.h"
#include <IsoAgLib/driver/can/impl/ident_c.h>

namespace __IsoAgLib {

/**
  constructor
*/
ProcessCmd_c::ProcessCmd_c()
  : mb_isSetpoint(false), mb_isRequest(false),
    men_valueGroup(noValue),
    men_command(noCommand)
{}

/** set values, called in ProcessPkg_c::resolveCommand() */
void ProcessCmd_c::setValues(bool ab_isSetpoint, bool ab_isRequest, ValueGroup_t ren_valueGroup,
                             CommandType_t ren_command)
{
  this->mb_isSetpoint = ab_isSetpoint;
  this->mb_isRequest = ab_isRequest;
  this->men_valueGroup = ren_valueGroup;
  this->men_command = ren_command;
};


} // end of namespace __IsoAgLib
