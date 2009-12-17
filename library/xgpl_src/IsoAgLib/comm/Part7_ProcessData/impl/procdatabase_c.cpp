/*
  procdatabase_c.cpp: base class for local or remote process data
    object

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
#include "procdatabase_c.h"
#include "process_c.h"
#include <IsoAgLib/driver/can/impl/canio_c.h>
#include "../Remote/Std/impl/procdataremote_c.h"

namespace __IsoAgLib {

/**
   initialise this ProcDataBase_c instance to a well defined initial state
   ISO parameters:
   @param ps_elementDDI optional pointer to array of structure IsoAgLib::ElementDdi_s which contains DDI, element, isSetpoint and ValueGroup
                        (array is terminated by ElementDdi_s.mui16_element == 0xFFFF)
   @param aui16_element  device element number

   common parameters:
   @param acrc_isoName optional ISOName code of Process-Data
   @param apc_externalOverridingIsoName pointer to updated ISOName variable
   @param apc_processDataChangeHandler optional pointer to handler class of application
   @param ai_singletonVecKey optional key for selection of IsoAgLib instance (default 0)
*/
  void ProcDataBase_c::init( const IsoAgLib::ElementDdi_s* ps_elementDDI, uint16_t aui16_element,
                             const IsoName_c& acrc_isoName,
                             const IsoName_c *apc_externalOverridingIsoName,
                             IsoAgLib::ProcessDataChangeHandler_c *apc_processDataChangeHandler,
                             int ai_singletonVecKey)
  {
    ProcIdent_c::init( ps_elementDDI, aui16_element, acrc_isoName, apc_externalOverridingIsoName);

    setSingletonKey(ai_singletonVecKey);
    mpc_processDataChangeHandler = apc_processDataChangeHandler;
  }


/**
  assignment operator for this base object
  @param acrc_src source instance
  @return reference to source instance for cmd like "prog1 = prog2 = prog3;"
*/
const ProcDataBase_c& ProcDataBase_c::operator=(const ProcDataBase_c& acrc_src)
{ // call base class operator
  ProcIdent_c::operator=(acrc_src);

  assignFromSource(acrc_src);

  // return source reference
  return *this;
}


/**
  copy constructor for ProcDataBase_c
  @param acrc_src source instance
*/
ProcDataBase_c::ProcDataBase_c(const ProcDataBase_c& acrc_src)
   : ProcIdent_c(acrc_src)
{
  assignFromSource(acrc_src);
}


/** base function for assignment of element vars for copy constructor and operator= */
void ProcDataBase_c::assignFromSource( const ProcDataBase_c& acrc_src )
{ // copy element vars
  mpc_processDataChangeHandler = acrc_src.mpc_processDataChangeHandler;
}


/** default destructor which has nothing to do */
ProcDataBase_c::~ProcDataBase_c() {}


/** helper function to get reference to process data pkg very quick */
ProcessPkg_c& ProcDataBase_c::getProcessPkg( void ) const
{
#if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
  // if more than one CAN BUS is used for IsoAgLib
  // no single cache value can be used, as each CAN BUS get its
  // individual ProcessPkg_c (a static functional-variable would
  // be used for ALL instances of ProcDataBase_c - also from
  // different CAN BUS'es)
  return getProcessInstance4Comm().data();
#else
  static ProcessPkg_c& mc_data = getProcessInstance4Comm().data();
  return mc_data;
#endif
}


/**
  process a message, which is adressed for this process data item;
  ProcDataBase_c::processMsg() is responsible to delegate the
  processing of setpoint and measurement messages to the appripriate
  functions processSetpoint and processProg;
  both functions are virtual, so that depending on loacl or remote
  process data the suitable reaction can be implemented
*/
void ProcDataBase_c::processMsg()
{
  if (getProcessInstance4Comm().data().mc_processCmd.checkIsSetpoint())
    processSetpoint();
  else
    processProg();
}


/** perform periodic acoins
  @param pui16_nextTimePeriod calculated new time period, based on current measure progs (only for local proc data)
*/
bool ProcDataBase_c::timeEvent( uint16_t* /* pui16_nextTimePeriod */ )
{
  return true;
}


/**
  send the given int32_t value with variable ISOName ac_varISOName
  (local: receiver; remote: sender)
  (other paramter fixed by ident of process data)

  set general command before sendValISOName !

  possible errors:
      * Err_c::elNonexistent one of resolved EMPF/SEND isn't registered with claimed address in Monitor
      * dependant error in CanIo_c on CAN send problems

  @param ac_varISOName variable ISOName
  @param ai32_val int32_t value to send
  @return true -> sendIntern set successful EMPF and SEND
*/
bool ProcDataBase_c::sendValISOName( const IsoName_c& /*ac_varISOName*/, int32_t ai32_val) const
{
  setBasicSendFlags();

  getProcessPkg().setData( ai32_val );

  // send the msg
  getCanInstance4Comm() << getProcessPkg();
  // check for any error during send resolve, ...
  if ( getILibErrInstance().good(IsoAgLib::iLibErr_c::CanBus, IsoAgLib::iLibErr_c::Can) )
  { // good
    return true;
  }
  else
    return false;
}


void ProcDataBase_c::setBasicSendFlags() const
{
  ProcessPkg_c& mc_data = getProcessPkg();

  // the communicating devices are represented on ISO11783
  mc_data.setIsoPri(3);
  mc_data.setIsoPgn(PROCESS_DATA_PGN);

  // general command is already set, use these values:
  // set command in ProcessPkg::flags2string
  const ProcessCmd_c::ValueGroup_t men_valueGroup = getProcessInstance4Comm().data().mc_processCmd.getValueGroup();
  const bool mb_isSetpoint = getProcessInstance4Comm().data().mc_processCmd.checkIsSetpoint();

  mc_data.set_Element(0xFFFF);
  mc_data.set_DDI(0);

  STL_NAMESPACE::list<IsoAgLib::ElementDdi_s>::const_iterator iter_elementDDI;

  if (elementDDI().size() == 1)
  {
    // we have only one DDI/element pair
    // interface process data init was possibly called with parameter DDI and element and not with ElementDdi_s
    // => we don't have reliable infos about men_valueGroup and mb_isSetpoint
    // => don't check for men_valueGroup and mb_isSetpoint but use this single entry in list
    iter_elementDDI = elementDDI().begin();
    mc_data.set_Element(element());
    mc_data.set_DDI(iter_elementDDI->ui16_DDI);
  }
  else
  {
    // get corresponding element/DDI
    for (iter_elementDDI = elementDDI().begin();
         iter_elementDDI != elementDDI().end(); iter_elementDDI++)
    {
      if ( (iter_elementDDI->en_valueGroup == men_valueGroup) && (iter_elementDDI->b_isSetpoint == mb_isSetpoint) )
      {
        mc_data.set_Element(element());
        mc_data.set_DDI(iter_elementDDI->ui16_DDI);
        break;
      }
    }
  }
}

/**
  process a measure prog message
  -> fully dependent on children type local/remote
*/
void ProcDataBase_c::processProg()
{
  return;
}


/**
  virtual base for processing of a setpoint message
*/
void ProcDataBase_c::processSetpoint()
{
  return;
}


/**
  deliver DDI from last received can pkg
  @return DDI
*/
uint16_t ProcDataBase_c::getDDIfromCANPkg( void ) const
{
  return getProcessPkg().DDI();
}

} // end of namespace __IsoAgLib
