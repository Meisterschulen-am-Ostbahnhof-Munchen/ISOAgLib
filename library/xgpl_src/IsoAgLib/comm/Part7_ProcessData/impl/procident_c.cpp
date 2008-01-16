/***************************************************************************
                          procident_c.cpp - identity of a process data instance
                                          for easy search
                             -------------------
    begin                : Fri Apr 07 2000
    copyright            : (C) 2000 - 2004 by Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
    type                 : Source
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 * This file is part of the "IsoAgLib", an object oriented program library *
 * to serve as a software layer between application specific program and   *
 * communication protocol details. By providing simple function calls for  *
 * jobs like starting a measuring program for a process data value on a    *
 * remote ECU, the main program has not to deal with single CAN telegram   *
 * formatting. This way communication problems between ECU's which use     *
 * this library should be prevented.                                       *
 * Everybody and every company is invited to use this library to make a    *
 * working plug and play standard out of the printed protocol standard.    *
 *                                                                         *
 * Copyright (C) 2000 - 2004 Dipl.-Inform. Achim Spangler                  *
 *                                                                         *
 * The IsoAgLib is free software; you can redistribute it and/or modify it *
 * under the terms of the GNU General Public License as published          *
 * by the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This library is distributed in the hope that it will be useful, but     *
 * WITHOUT ANY WARRANTY; without even the implied warranty of              *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       *
 * General Public License for more details.                                *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with IsoAgLib; if not, write to the Free Software Foundation,     *
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA           *
 *                                                                         *
 * As a special exception, if other files instantiate templates or use     *
 * macros or inline functions from this file, or you compile this file and *
 * link it with other works to produce a work based on this file, this file*
 * does not by itself cause the resulting work to be covered by the GNU    *
 * General Public License. However the source code for this file must still*
 * be made available in accordance with section (3) of the                 *
 * GNU General Public License.                                             *
 *                                                                         *
 * This exception does not invalidate any other reasons why a work based on*
 * this file might be covered by the GNU General Public License.           *
 *                                                                         *
 * Alternative licenses for IsoAgLib may be arranged by contacting         *
 * the main author Achim Spangler by a.spangler@osb-ag:de                  *
 ***************************************************************************/

 /**************************************************************************
 *                                                                         *
 *     ###    !!!    ---    ===    IMPORTANT    ===    ---    !!!    ###   *
 * Each software module, which accesses directly elements of this file,    *
 * is considered to be an extension of IsoAgLib and is thus covered by the *
 * GPL license. Applications must use only the interface definition out-   *
 * side :impl: subdirectories. Never access direct elements of __IsoAgLib  *
 * and __HAL namespaces from applications which shouldnt be affected by    *
 * the license. Only access their interface counterparts in the IsoAgLib   *
 * and HAL namespaces. Contact a.spangler@osb-ag:de in case your applicat- *
 * ion really needs access to a part of an internal namespace, so that the *
 * interface might be extended if your request is accepted.                *
 *                                                                         *
 * Definition of direct access:                                            *
 * - Instantiation of a variable with a datatype from internal namespace   *
 * - Call of a (member-) function                                          *
 * Allowed is:                                                             *
 * - Instatiation of a variable with a datatype from interface namespace,  *
 *   even if this is derived from a base class inside an internal namespace*
 * - Call of member functions which are defined in the interface class     *
 *   definition ( header )                                                 *
 *                                                                         *
 * Pairing of internal and interface classes:                              *
 * - Internal implementation in an :impl: subdirectory                     *
 * - Interface in the parent directory of the corresponding internal class *
 * - Interface class name IsoAgLib::iFoo_c maps to the internal class      *
 *   __IsoAgLib::Foo_c                                                     *
 *                                                                         *
 * AS A RULE: Use only classes with names beginning with small letter :i:  *
 ***************************************************************************/

 /**************************************************************************
 *                                                                         *
 *     ###    !!!    ---    ===    IMPORTANT    ===    ---    !!!    ###   *
 * Each software module, which accesses directly elements of this file,    *
 * is considered to be an extension of IsoAgLib and is thus covered by the *
 * GPL license. Applications must use only the interface definition out-   *
 * side :impl: subdirectories. Never access direct elements of __IsoAgLib  *
 * and __HAL namespaces from applications which shouldnt be affected by    *
 * the license. Only access their interface counterparts in the IsoAgLib   *
 * and HAL namespaces. Contact a.spangler@osb-ag:de in case your applicat- *
 * ion really needs access to a part of an internal namespace, so that the *
 * interface might be extended if your request is accepted.                *
 *                                                                         *
 * Definition of direct access:                                            *
 * - Call of a (member-) function                                          *
 * - Instantiation of a variable in a datatype from internal namespace     *
 * Allowed is:                                                             *
 * - Instatiation of a variable in a datatype from interface namespace,    *
 *   even if this is derived from a base class inside an internal namespace*
 * - Call of member functions which are defined in the interface class     *
 *   definition ( header )                                                 *
 * Pairing of internal and interface classes:                              *
 * - Internal implementation in an :impl: subdirectory                     *
 * - Interface in the parent directory of the corresponding internal class *
 * - Interface class name IsoAgLib::iFoo_c maps to the internal class      *
 *   __IsoAgLib::Foo_c                                                     *
 *                                                                         *
 * AS A RULE: Use only classes with names beginning with small letter :i:  *
 ***************************************************************************/
#include "procident_c.h"
#include "process_c.h"

namespace __IsoAgLib {

/**
  constructor which can optional set all member values
    ISO parameter
    @param ps_elementDDI optional pointer to array of structure IsoAgLib::ElementDdi_s which contains DDI, isSetpoint and ValueGroup
                         (array is terminated by ElementDdi_s.ddi == 0xFFFF)
    @param mui16_element device element number

    common parameter
    @param ac_isoName optional ISOName code of Process-Data
    @param apc_externalOverridingIsoName pointer to the optional ISOName var (for automatic update as soon
            as corresponding device is registered as having claimed address in monitor table list)
  */
  ProcIdent_c::ProcIdent_c( const IsoAgLib::ElementDdi_s* aps_elementDDI, uint16_t aui16_element,
                            const IsoName_c& ac_isoName, const IsoName_c *apc_externalOverridingIsoName, int ai_singletonVecKey)
  : ClientBase( ai_singletonVecKey ),
    mpc_externalOverridingIsoName(NULL),
		mc_isoName(IsoName_c::IsoNameUnspecified())
{
  init( aps_elementDDI, aui16_element, ac_isoName, apc_externalOverridingIsoName);
}

/** copy constructor */
ProcIdent_c::ProcIdent_c( const ProcIdent_c& arc_src )
  : ClientBase( arc_src ),
	  mpc_externalOverridingIsoName( arc_src.mpc_externalOverridingIsoName ),
		mc_isoName( arc_src.mc_isoName )
{
  assignFromSource( arc_src );
}


/**
    initialisation which can set this process data instance to a defined intial state
    ISO parameter
    @param ps_elementDDI optional pointer to array of structure IsoAgLib::ElementDdi_s which contains DDI, isSetpoint and ValueGroup
                         (array is terminated by ElementDdi_s.ddi == 0xFFFF)
    @param mui16_element device element number

    common parameter
    @param ac_isoName ISOName code of Process-Data
    @param apc_externalOverridingIsoName pointer to the optional ISOName var (for automatic update as soon
            as corresponding device is registered as having claimed address in monitor table list)
*/
void ProcIdent_c::init( const IsoAgLib::ElementDdi_s* aps_elementDDI, uint16_t aui16_element,
                        const IsoName_c& ac_isoName, const IsoName_c *apc_externalOverridingIsoName)
{
  setElementDDI(aps_elementDDI);
  setElementNumber(aui16_element);

  mpc_externalOverridingIsoName = apc_externalOverridingIsoName;

  // the ISOName of ident is best defined by pointed value of apc_externalOverridingIsoName
  if ( apc_externalOverridingIsoName != 0 ) mc_isoName = *apc_externalOverridingIsoName;
  // last choice is definition of mc_isoName by process data identiy
  else mc_isoName = ac_isoName;
}

/**
  copy constructor for class instance
  @param arc_src ProcIdent_c instance
  @return reference to source for cmd like "proc1 = proc2 = proc3;"
*/
ProcIdent_c& ProcIdent_c::operator=(const ProcIdent_c& arc_src){
  // first assign base class
  ClientBase::operator=(arc_src);
  // now assign flags of this class
  assignFromSource( arc_src );

  return *this;
}

/** internal base function for copy constructor and assignement */
void ProcIdent_c::assignFromSource( const ProcIdent_c& arc_src )
{
  mc_isoName = arc_src.mc_isoName;
  mpc_externalOverridingIsoName = arc_src.mpc_externalOverridingIsoName;
  // elementDDI() returns list reference, setElementDDI() expects pointer to list
  setElementDDI(&(arc_src.elementDDI()));
}

/** default destructor which has nothing to do */
ProcIdent_c::~ProcIdent_c(){
}

/**
  set pointer to external isoName instances (used by isoName())
  @param apc_val pointer to ISOName
*/
void ProcIdent_c::setExternalOverridingIsoName(const IsoName_c* apc_val)
{
  mpc_externalOverridingIsoName = apc_val;
  mc_isoName = *apc_val;
}

/**
   check if this item has the same identity as defined by the parameters,
   if aui8_devClassInst is 0xFF a lazy match disregarding pos is done
   (important for matching received process data msg);
   if INSTANCE is defined (!= 0xFF) then one of the following conditions must be true:<ul>
   <li>parameter INSTANCE == ident INSTANCE (devClassInst())
   <li>parameter arc_isoName == isoName()
   </ul>

   ISO parameter
   @param arc_isoNameSender compare this parameter with owner isoName (only for remote, local calls: IsoNameUnspecified)
   @param arc_isoNameReceiver compared isoName value
   @param aui16_DDI compared DDI value
   @param aui16_element compared element value

   @return true -> this instance has same Process-Data identity
*/
bool ProcIdent_c::matchISO( const IsoName_c& arc_isoNameSender,
                            const IsoName_c& arc_isoNameReceiver,
                            uint16_t aui16_DDI,
                            uint16_t aui16_element
                          ) const
{
  // check wether current element/DDI combination matches one list element in process data element/DDI list
  if (aui16_element != element()) return false;
  STL_NAMESPACE::list<IsoAgLib::ElementDdi_s>::const_iterator iter;
  for (iter = mlist_elementDDI.begin(); iter != mlist_elementDDI.end(); iter++)
    if ( iter->ui16_DDI == aui16_DDI )
      break;

  if (iter == mlist_elementDDI.end())
    return false;

  if (arc_isoNameSender.isSpecified())
  { // check in remote case: check if devClass of ownerISOName in procident matches devClass of sender
    if (isoName() != arc_isoNameSender) return false;
  }
  else
  { // check in local case: check if procident devClass matches devClass of empf
    if (isoName() != arc_isoNameReceiver) return false;
  }

  if (!getProcessInstance4Comm().data().resolveCommandTypeForISO(*iter)) return false;
  // all previous tests are positive -> answer positive match
  return true;
}

/** check if this ProcIdent_c has the given DDI as element */
bool ProcIdent_c::hasDDI( uint16_t aui16_checkDDI ) const
{
  STL_NAMESPACE::list<IsoAgLib::ElementDdi_s>::const_iterator iter;
  for (iter = mlist_elementDDI.begin(); iter != mlist_elementDDI.end(); iter++)
    if (iter->ui16_DDI == aui16_checkDDI)
      break;

  if (iter == mlist_elementDDI.end()) return false;
  else return true;
}

bool ProcIdent_c::hasType(bool ab_isSetpoint, ProcessCmd_c::ValueGroup_t t_ddiType) const
{
  STL_NAMESPACE::list<IsoAgLib::ElementDdi_s>::const_iterator iter;
  for (iter = mlist_elementDDI.begin(); iter != mlist_elementDDI.end(); iter++)
    if ((iter->en_valueGroup == t_ddiType) && (iter->b_isSetpoint == ab_isSetpoint))
      break;

  if (iter == mlist_elementDDI.end()) return false;
  else return true;
}

bool ProcIdent_c::check4GroupMatch(uint16_t aui16_DDI, uint16_t aui16_element, const IsoName_c& ac_isoName)
{
  bool b_foundPair = false;
  // first check if ISOName matches
  if (ac_isoName != mc_isoName) return b_foundPair;

  if (aui16_element != element()) return b_foundPair;

  for (STL_NAMESPACE::list<IsoAgLib::ElementDdi_s>::const_iterator iter = mlist_elementDDI.begin(); iter != mlist_elementDDI.end(); iter++)
  {
    b_foundPair = isPair((*iter).ui16_DDI, aui16_DDI);
    if (b_foundPair) break;
  }
  return b_foundPair;
}

bool ProcIdent_c::check4GroupMatchExisting(uint16_t aui16_DDI, uint16_t aui16_element, const IsoName_c& ac_isoName)
{
  bool b_foundPair = false;
  // first check if ISOName matches
  if (ac_isoName != mc_isoName) return b_foundPair;

  if (aui16_element != element()) return b_foundPair;

  return hasDDI(aui16_DDI);
}

bool ProcIdent_c::checkProprietary4GroupMatch(uint16_t aui16_element, const IsoName_c& ac_isoName)
{
  bool b_foundPair = false;
  // first check if DevClass is the same like Name's DevClass
  if ( mpc_externalOverridingIsoName && (ac_isoName.devClass() != mpc_externalOverridingIsoName->devClass()) ) return b_foundPair;

  // if it is not the same device element continue
  if (aui16_element != element()) return b_foundPair;

  STL_NAMESPACE::list<IsoAgLib::ElementDdi_s>::const_iterator iter = mlist_elementDDI.begin();
  if (iter != mlist_elementDDI.end())
  {
    // device element was found
    b_foundPair = true;
  }
  return b_foundPair;
}


bool ProcIdent_c::isPair(uint16_t aui16_ElementDDI, uint16_t aui16_DDI)
{
  if (aui16_ElementDDI == aui16_DDI) return false; // same DDI -> "new DDI should not be added"
  // check if given DDI is proprietary
  if (aui16_ElementDDI > 0xDFFF) return false; // 0xDFFF is Request Default Process Data
  // calculate group membership of both DDIs
  // germane groups are from DDI 1 to 70 grouped to 5 and DDI 71 to 79 grouped to 3 and DDI 142/143
  uint8_t ui8_groupElementDDI1 = (aui16_ElementDDI / 5);
  uint8_t ui8_groupElementDDI2 = ((1 + aui16_ElementDDI) / 3);

  uint8_t ui8_groupDDI1 = (aui16_DDI / 5);
  uint8_t ui8_groupDDI2 = ((1 + aui16_DDI) / 3);

  // check if both are in same DDI group
  if ((ui8_groupElementDDI1 == ui8_groupDDI1) &&
      ((ui8_groupElementDDI1 <= 13) || ((ui8_groupElementDDI1 >= 20) && (ui8_groupElementDDI1 <= 22))))
    return true;
  if ((ui8_groupElementDDI2 == ui8_groupDDI2) && ((ui8_groupElementDDI2 >= 24) && (ui8_groupElementDDI2 <= 26)))
    return true;
  if (((aui16_ElementDDI == 0x8E) && (aui16_DDI == 0x8F)) || ((aui16_ElementDDI == 0x8F) && (aui16_DDI == 0x8E)))
    return true;
  return false;
}


bool ProcIdent_c::add2Group(uint16_t aui16_DDI)
{
  bool b_foundPair = false;

  for (STL_NAMESPACE::list<IsoAgLib::ElementDdi_s>::const_iterator iter = mlist_elementDDI.begin(); iter != mlist_elementDDI.end(); iter++)
  {
    b_foundPair = isPair(iter->ui16_DDI, aui16_DDI);

    if (b_foundPair)
    {
      ProcessCmd_c::ValueGroup_t ddiType;
      bool mb_isSetpoint;
      getDDIType(aui16_DDI, ddiType, mb_isSetpoint);
      if (ddiType != ProcessCmd_c::noValue)
      {
        IsoAgLib::ElementDdi_s s_DDIToAdd = {aui16_DDI, mb_isSetpoint, ddiType};
        mlist_elementDDI.push_back(s_DDIToAdd);
      }
      break;
    }
  }
  return b_foundPair;
}


bool ProcIdent_c::addProprietary2Group(uint16_t aui16_DDI, bool mb_isSetpoint, ProcessCmd_c::ValueGroup_t ddiType)
{
  IsoAgLib::ElementDdi_s s_DDIToAdd = {aui16_DDI, mb_isSetpoint, ddiType};
  mlist_elementDDI.push_back(s_DDIToAdd);

  return true;
}

void ProcIdent_c::getDDIType(uint16_t aui16_DDI, ProcessCmd_c::ValueGroup_t &r_ddiType, bool &rb_isSetpoint)
{
  // check if given DDI is proprietary
  if (aui16_DDI > 0xDFFF)
  { // 0xDFFF is Request Default Process Data
    // proprietary DDIs are exact per default
    r_ddiType = ProcessCmd_c::exactValue;
    rb_isSetpoint = true;
    return;
  }

  if ((aui16_DDI == 0x8E) || (aui16_DDI == 0x8D))
  {
    r_ddiType = ProcessCmd_c::exactValue;
    rb_isSetpoint = true;
    return;
  }
  if (aui16_DDI == 0x8F)
  {
    r_ddiType = ProcessCmd_c::exactValue;
    rb_isSetpoint = false;
    return;
  }

  if (((aui16_DDI >=0x1) && (aui16_DDI <= 0x46)) || ((aui16_DDI >=0x65) && (aui16_DDI <= 0x73)))
  {
    uint8_t ui8_groupDDI1 = aui16_DDI % 5;

    switch (ui8_groupDDI1)
    {
      case 0:
        r_ddiType = ProcessCmd_c::maxValue;
        rb_isSetpoint = false;
        break;
      case 1:
        r_ddiType = ProcessCmd_c::exactValue;
        rb_isSetpoint = true;
        break;
      case 2:
        r_ddiType = ProcessCmd_c::exactValue;
        rb_isSetpoint = false;
        break;
      case 3:
        r_ddiType = ProcessCmd_c::defaultValue;
        rb_isSetpoint = true;
        break;
      case 4:
        r_ddiType = ProcessCmd_c::minValue;
        rb_isSetpoint = false;
        break;
    }
    return;
  }

  if ((aui16_DDI >=0x47) && (aui16_DDI <= 0x4F))
  {
    uint8_t ui8_groupDDI2 = (1 + aui16_DDI) % 3;

    switch (ui8_groupDDI2)
    {
      case 0:
        r_ddiType = ProcessCmd_c::exactValue;
        rb_isSetpoint = true;
        break;
      case 1:
        r_ddiType = ProcessCmd_c::exactValue;
        rb_isSetpoint = false;
        break;
      case 2:
        r_ddiType = ProcessCmd_c::maxValue;
        rb_isSetpoint = false;
        break;
    }
    return;
  }

  // any other DDI
  r_ddiType = ProcessCmd_c::exactValue;
  rb_isSetpoint = false;
}


bool ProcIdent_c::hasDDIType (uint16_t aui16_DDI, ProcessCmd_c::ValueGroup_t t_ddiType)
{
  bool b_isSetPoint;
  ProcessCmd_c::ValueGroup_t t_retDDIType;

  getDDIType (aui16_DDI, t_retDDIType, b_isSetPoint);

  return (t_retDDIType == t_ddiType);
}



void ProcIdent_c::setElementDDI(const IsoAgLib::ElementDdi_s* ps_elementDDI)
{
  mlist_elementDDI.clear();
  // check if pointer to strcut (array) is set (constructor call with NULL possible!)
  if (ps_elementDDI) {
    // in last struct element == 0xFFFF
    while (ps_elementDDI->ui16_DDI != 0xFFFF)
      mlist_elementDDI.push_back(*ps_elementDDI++);
  }
}

void ProcIdent_c::setElementDDI(const STL_NAMESPACE::list<IsoAgLib::ElementDdi_s>* pl_elementDDI)
{
  mlist_elementDDI.clear();
  // check if pointer to struct (array) is set (constructor call with NULL possible!)
  if (pl_elementDDI) {
    for (STL_NAMESPACE::list<IsoAgLib::ElementDdi_s>::const_iterator iter = pl_elementDDI->begin();
         iter != pl_elementDDI->end(); iter++)
        mlist_elementDDI.push_back(*iter);
  }
}


} // end of namespace __IsoAgLib

