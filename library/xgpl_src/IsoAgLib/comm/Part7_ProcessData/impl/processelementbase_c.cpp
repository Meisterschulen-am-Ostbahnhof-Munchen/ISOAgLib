/*
  processelementbase_c.cpp: base class for elements of ProcessData

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
#include "processelementbase_c.h"

namespace __IsoAgLib {

/**
  constructor which initialse both pointers if given
  @param apc_processData optional pointer to containing ProcessData instance
*/
ProcessElementBase_c::ProcessElementBase_c(
    ProcDataBase_c *const apc_processData )
    : ClientBase(){
  mpc_processData = apc_processData;
  #if defined(PRT_INSTANCE_CNT) && (PRT_INSTANCE_CNT != 1 )
  // only set singletonKey in ClientBase, if more than one IsoAgLib instance
  // ismanaged by this IsoAgLib
  if ( apc_processData != NULL )
    setSingletonKey( apc_processData->getSingletonVecKey() );
  #endif
}
/**
  constructor which intit the pointers by references to the stored object instances
  @param arc_processData optional reference to containing ProcessData instance
*/
ProcessElementBase_c::ProcessElementBase_c(
    ProcDataBase_c &arc_processData )
    : ClientBase(arc_processData){
  mpc_processData = &arc_processData;
}
/**
  copy constructor
  @param acrc_src source ProcessElementBase_c instance
*/
ProcessElementBase_c::ProcessElementBase_c(const ProcessElementBase_c& acrc_src)
  : ClientBase(acrc_src), mpc_processData(acrc_src.mpc_processData)
{}

/**
  assignment operator for ProcessElementBase
  @param acrc_src source ProcessElementBase_c instance
*/
const ProcessElementBase_c& ProcessElementBase_c::operator=(const ProcessElementBase_c& acrc_src){
  setSingletonKey(acrc_src.getSingletonVecKey());
  // copy element vars
  mpc_processData = acrc_src.mpc_processData;

  // return the source reference
  return acrc_src;
}

/** default destructor which has nothing to do */
ProcessElementBase_c::~ProcessElementBase_c(){
  mpc_processData = NULL;
}

/**
  set the pointer to Scheduler_c and ProcessData by references to the object instances
  @param arc_processData optional reference to containing ProcessData instance
*/
void ProcessElementBase_c::set(ProcDataBase_c& arc_processData )
{
  #if defined(PRT_INSTANCE_CNT) && (PRT_INSTANCE_CNT != 1 )
  ClientBase::setSingletonKey(arc_processData.getSingletonVecKey());
  #endif
  mpc_processData = &arc_processData;
};

/**
  deliver the pointer to the containing ProcessData item
  @param acpc_processData optional pointer to containing ProcessData instance
*/
void ProcessElementBase_c::set(ProcDataBase_c *const acpc_processData)
{
  #if defined(PRT_INSTANCE_CNT) && (PRT_INSTANCE_CNT != 1 )
  if ( acpc_processData != NULL )
    ClientBase::setSingletonKey(acpc_processData->getSingletonVecKey());
  #endif
  mpc_processData = acpc_processData;
};


} // end of namespace __IsoAgLib
