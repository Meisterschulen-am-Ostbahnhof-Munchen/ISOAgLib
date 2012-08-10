/*
  procident_c.cpp: identity of a process data instance for easy search

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include "procident_c.h"
#include "process_c.h"

namespace __IsoAgLib {

ProcIdent_c::ProcIdent_c( uint16_t aui16_ddi, uint16_t aui16_element,
                          const IsoName_c& acrc_isoName, const IsoName_c *apc_externalOverridingIsoName, int ai_multitonInst)
  : ClientBase( ai_multitonInst ),
    mpc_externalOverridingIsoName(NULL),
		mc_isoName(IsoName_c::IsoNameUnspecified())
{
  init( aui16_ddi, aui16_element, acrc_isoName, apc_externalOverridingIsoName);
}

ProcIdent_c::ProcIdent_c( const ProcIdent_c& acrc_src )
  : ClientBase( acrc_src ),
	  mpc_externalOverridingIsoName( acrc_src.mpc_externalOverridingIsoName ),
		mc_isoName( acrc_src.mc_isoName )
{
  assignFromSource( acrc_src );
}

void ProcIdent_c::init( uint16_t aui16_ddi, uint16_t aui16_element,
                        const IsoName_c& acrc_isoName, const IsoName_c *apc_externalOverridingIsoName)
{
  setElementDDI(aui16_ddi);
  setElementNumber(aui16_element);

  mpc_externalOverridingIsoName = apc_externalOverridingIsoName;

  // the ISOName of ident is best defined by pointed value of apc_externalOverridingIsoName
  if ( apc_externalOverridingIsoName != 0 ) mc_isoName = *apc_externalOverridingIsoName;
  // last choice is definition of mc_isoName by process data identiy
  else mc_isoName = acrc_isoName;
}

ProcIdent_c& ProcIdent_c::operator=(const ProcIdent_c& acrc_src){
  // first assign base class
  ClientBase::operator=(acrc_src);
  // now assign flags of this class
  assignFromSource( acrc_src );

  return *this;
}

void ProcIdent_c::assignFromSource( const ProcIdent_c& acrc_src )
{
  mc_isoName = acrc_src.mc_isoName;
  mpc_externalOverridingIsoName = acrc_src.mpc_externalOverridingIsoName;
  // elementDDI() returns list reference, setElementDDI() expects pointer to list
  setElementDDI(acrc_src.DDI());
}

ProcIdent_c::~ProcIdent_c(){
}

void ProcIdent_c::setExternalOverridingIsoName(const IsoName_c* apc_val)
{
  mpc_externalOverridingIsoName = apc_val;
  mc_isoName = *apc_val;
}

bool ProcIdent_c::matchISO( const IsoName_c& acrc_isoNameSender,
                            const IsoName_c& acrc_isoNameReceiver,
                            uint16_t aui16_DDI,
                            uint16_t aui16_element
                          ) const
{
  // check wether current element/DDI combination matches one list element in process data element/DDI list
  if (aui16_element != element()) return false;
  if (aui16_DDI != DDI()) return false;

  if (acrc_isoNameSender.isSpecified())
  { // check in remote case: check if devClass of ownerISOName in procident matches devClass of sender
    if (isoName() != acrc_isoNameSender) return false;
  }
  else
  { // check in local case: check if procident devClass matches devClass of empf
    if (isoName() != acrc_isoNameReceiver) return false;
  }

  // all previous tests are positive -> answer positive match
  return true;
}

} // end of namespace __IsoAgLib

