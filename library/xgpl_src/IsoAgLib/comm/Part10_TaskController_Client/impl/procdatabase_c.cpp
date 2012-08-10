/*
  procdatabase_c.cpp: base class for local or remote process data
    object

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
#include "procdatabase_c.h"
#include "process_c.h"
#include <IsoAgLib/comm/impl/isobus_c.h>

namespace __IsoAgLib {

void ProcDataBase_c::init( uint16_t aui16_ddi, uint16_t aui16_element,
                           const IsoName_c& acrc_isoName,
                           const IsoName_c *apc_externalOverridingIsoName,
                           IsoAgLib::ProcessDataChangeHandler_c *apc_processDataChangeHandler,
                           int ai_multitonInst)
{
  ProcIdent_c::init( aui16_ddi, aui16_element, acrc_isoName, apc_externalOverridingIsoName);

  setMultitonInst(ai_multitonInst);
  mpc_processDataChangeHandler = apc_processDataChangeHandler;
}

const ProcDataBase_c& ProcDataBase_c::operator=(const ProcDataBase_c& acrc_src)
{ // call base class operator
  ProcIdent_c::operator=(acrc_src);

  assignFromSource(acrc_src);

  // return source reference
  return *this;
}

ProcDataBase_c::ProcDataBase_c(const ProcDataBase_c& acrc_src)
   : ProcIdent_c(acrc_src)
{
  assignFromSource(acrc_src);
}

void ProcDataBase_c::assignFromSource( const ProcDataBase_c& acrc_src )
{ // copy element vars
  mpc_processDataChangeHandler = acrc_src.mpc_processDataChangeHandler;
}

ProcDataBase_c::~ProcDataBase_c() {}

void ProcDataBase_c::processMsg( ProcessPkg_c& pkg )
{
  isoaglib_assert( DDI() == pkg.DDI() );

  pkg.resolveCommandTypeForISO( DDI() );

  if (pkg.mc_processCmd.checkIsSetpoint())
    processSetpoint( pkg );
  else
    processProg(pkg);
}

bool ProcDataBase_c::timeEvent( uint16_t* /* pui16_nextTimePeriod */ )
{
  return true;
}

bool ProcDataBase_c::sendValISOName( ProcessPkg_c& pkg, const IsoName_c& /*ac_varISOName*/, int32_t ai32_val) const
{
  setBasicSendFlags( pkg );

  pkg.setData( ai32_val );

  // send the msg
  pkg.flags2String();
  getIsoBusInstance4Comm() << pkg;
  return true;
}


void ProcDataBase_c::setBasicSendFlags( ProcessPkg_c& pkg ) const
{
  // the communicating devices are represented on ISO11783
  pkg.setIsoPri(3);
  pkg.setIsoPgn(PROCESS_DATA_PGN);

  // general command is already set, use these values:
  // set command in ProcessPkg::flags2string
  const ProcessCmd_c::ValueGroup_t men_valueGroup = pkg.mc_processCmd.getValueGroup();
  const bool mb_isSetpoint = pkg.mc_processCmd.checkIsSetpoint();

  pkg.set_Element(0xFFFF);
  pkg.set_DDI(0);

  // we have only one DDI/element pair
  // interface process data init was possibly called with parameter DDI and element and not with ElementDdi_s
  // => we don't have reliable infos about men_valueGroup and mb_isSetpoint
  // => don't check for men_valueGroup and mb_isSetpoint but use this single entry in list
  pkg.set_Element(element());
  pkg.set_DDI(DDI());
}

void ProcDataBase_c::processProg( const ProcessPkg_c& )
{
  return;
}

void ProcDataBase_c::processSetpoint( const ProcessPkg_c& )
{
  return;
}

} // end of namespace __IsoAgLib
