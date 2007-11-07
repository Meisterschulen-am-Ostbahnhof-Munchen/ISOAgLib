/***************************************************************************
                          procident_c.h - identity of a process data instance
                                         for easy search
                             -------------------
    begin                : Fri Apr 07 2000
    copyright            : (C) 2000 - 2004 by Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
    type                 : Header
    $LastChangedDate$
    $LastChangedRevision$
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
#ifndef PROC_IDENT_H
#define PROC_IDENT_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include <IsoAgLib/typedef.h>
#include <IsoAgLib/comm/SystemMgmt/ISO11783/impl/isoname_c.h>

#include <IsoAgLib/util/impl/singleton.h>
#include "generalcommand_c.h"
#include "../elementddi_s.h"
#include <list>

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

/**
  identity of a process data instances;
  implements search
  @author Dipl.-Inform. Achim Spangler
*/
class ProcIdent_c : public ClientBase {
public:

  /**
    constructor which can optional set all member values
    ISO parameter
    @param ps_elementDDI optional pointer to array of structure IsoAgLib::ElementDdi_s which contains DDI, isSetpoint and ValueGroup
                         (array is terminated by ElementDdi_s.ddi == 0xFFFF)
    @param mui16_element device element number

    common parameter
    @param ac_isoName optional ISOName code of Process-Data
    @param ac_ownerISOName optional ISOName code of owner of Process-Data
           ( important if DEVCLASS and/or DEVCLASSINST differs from identity ISOName in ac_isoName; this is the case
             for process data from base data dictionary table (DEVCLASS==0), which is managed/owned by device of
             type DEVCLASS != 0)
    @param apc_ownerISOName pointer to the optional ISOName var of the owner (for automatic update as soon
            as corresponding device is registered as having claimed address in monitor table list)
  */
  ProcIdent_c(
              const IsoAgLib::ElementDdi_s* ps_elementDDI = NULL,
              uint16_t mui16_element = 0xFFFF,
              const IsoName_c& ac_isoName = IsoName_c::IsoNameInitialProcessData(),
              const IsoName_c& ac_ownerISOName = IsoName_c::IsoNameUnspecified(),
              const IsoName_c *apc_ownerISOName = NULL,
              int ai_singletonVecKey = 0);

  /** copy constructor */
  ProcIdent_c( const ProcIdent_c& arc_src );

  /**
    initialisation which can set this process data instance to a defined intial state
    ISO parameter
    @param ps_elementDDI optional pointer to array of structure IsoAgLib::ElementDdi_s which contains DDI, isSetpoint and ValueGroup
                         (array is terminated by ElementDdi_s.ddi == 0xFFFF)
    @param mui16_element device element number

    common parameter
    @param ac_isoName ISOName code of Process-Data
    @param ac_ownerISOName optional ISOName code of owner of Process-Data
           ( important if DEVCLASS and/or DEVCLASSINST differs from identity ISOName in ac_isoName; this is the case
             for process data from base data dictionary table (DEVCLASS==0), which is managed/owned by device of
             type DEVCLASS != 0)
    @param apc_ownerISOName pointer to the optional ISOName var of the owner (for automatic update as soon
            as corresponding device is registered as having claimed address in monitor table list)
  */
  void init(
            const IsoAgLib::ElementDdi_s* ps_elementDDI,
            uint16_t mui16_element,
            const IsoName_c& ac_isoName,
            const IsoName_c& ac_ownerISOName = IsoName_c::IsoNameUnspecified(),
            const IsoName_c *apc_ownerISOName = NULL);

  /**
    copy constructor for class instance
    @param arc_src source ProcIdent_c instance
    @return reference to source for cmd like "proc1 = proc2 = proc3;"
  */
  ProcIdent_c& operator=(const ProcIdent_c& arc_src);
  /** default destructor which has nothing to do */
  ~ProcIdent_c();

#if 0
  /**
    comparison of two process ident instances
    @param arc_right compared object
    @return true -> this instance is equal to the other
  */
  bool operator==(const ProcIdent_c& rrfec_right) const
    {return (calcIdentVal() == rrfec_right.calcIdentVal());};
  /**
    differ comparison operator with another ProcIdent_c instance
    @param arc_right compared object
    @return true -> this indstance is different from the other
  */
  bool operator!=(const ProcIdent_c& rrfec_right) const
    {return (calcIdentVal() != rrfec_right.calcIdentVal());};
  /**
    lower than comparison with another ProcIdent_c instance
    @param arc_right compared object
    @return true -> this instance is < than the other
  */
  bool operator<(const ProcIdent_c& rrfec_right) const
    {return (calcIdentVal() < rrfec_right.calcIdentVal());};
  /**
    greater than comparison with another ProcIdent_c instance
    @param arc_right compared object
    @return true -> this indstance is > than the other
  */
  bool operator>(const ProcIdent_c& rrfec_right) const
    {return (calcIdentVal() > rrfec_right.calcIdentVal());};
#endif

  // member variable access

  /**
     deliver list of ElementDdi_s
     @return STL_NAMESPACE::list<IsoAgLib::ElementDdi_s>
  */
  const STL_NAMESPACE::list<IsoAgLib::ElementDdi_s>& elementDDI()const {return mlist_elementDDI;}

  /** check if this ProcIdent_c has the given DDI as element */
  bool hasDDI( uint16_t aui16_checkDDI ) const;
  bool hasType(bool ab_isSetpoint, GeneralCommand_c::ValueGroup_t t_ddiType) const;

  /**
    deliver value DDI (only possible if only one elementDDI in list)
    @return DDI
  */
  uint16_t DDI() const{
    if (mlist_elementDDI.size() == 1)
      return mlist_elementDDI.begin()->ui16_DDI;
    else
      return 0;
  }

  /**
    deliver value element (only possible if only one elementDDI in list)
    @return element
  */
  uint16_t element() const{ return mui16_element; }

  /**
    deliver value DEVCLASS (machine type specific table of process data types)
    everytime deliver the identity DEVCLASS (and NOT the possibly differing DEVCLASS of the owner)
    @return DEVCLASS
  */
  uint8_t devClass() const{return mc_isoName.devClass();}

  /**
    deliver value ISOName (machine type specific table of process data types)
    use everytime the _device_class_ from the ident part, and take the _instance_ from the owner.
    Special Case: if ISOName is set to 0,0 -> don't use any logac logic and return
    stored value
    @return ISOName
  */
  const IsoName_c& isoName() const {return ownerISOName();}

  /**
    deliver value _instance_ (important if more than one machine with equal _device_class_ are active)
    deliver the device class instance of the owner, as this _instance_ is sometimes updated after the creation of this
    process data instance.
    @return POS
  */
  uint8_t devClassInst() const{return ownerISOName().devClassInst();}

  /**
    deliver the owner isoName (retrieved from pointed isoName value, if valid pointer)
    @return actual ISOName of owner
  */
  const IsoName_c& ownerISOName() const
    { return ((mpc_ownerISOName != 0)?(*mpc_ownerISOName):(mc_ownerISOName));}

  /**
    set DDI, value group and setpoint/measure type of process msg
    @param al_elementDDI
  */
  void setElementDDI(const IsoAgLib::ElementDdi_s* ps_elementDDI);

  /**
    set DDI, value group and setpoint/measure type of process msg (used in assignFromSource)
    @param pl_elementDDI
  */
  void setElementDDI(const STL_NAMESPACE::list<IsoAgLib::ElementDdi_s>* pl_elementDDI);

  /** set device element number
    * @param  aui16_element */
  void setElementNumber(uint16_t aui16_element) { mui16_element = aui16_element; }

  /**
    set value DEVCLASS (machine type specific table of process data types)
    @param aui8_val new DEVCLASS val
  */
  void setDevClass(uint8_t aui8_val){mc_isoName.setDevClass(aui8_val);}

  /**
    set value ISOName (machine type specific table of process data types)
    @param ac_val new ISOName val
  */
  void setISOName(const IsoName_c& ac_val){mc_isoName = ac_val;}

  /**
    set value _instance_ (important if more than one machine with equal _device_class_ are active)
    set also the _instance_ of the owner as the owner _instance_ shall be always the most actual value
    @param aui8_val new device class inst val
  */
  void setDevClassInst(uint8_t aui8_val){mc_isoName.setDevClassInst(aui8_val); mc_ownerISOName.setDevClassInst(aui8_val);}

  /**
    set the owner isoName
    @param ac_val new ISOName of owner
  */
  void setOwnerISOName(const IsoName_c& ac_val){mc_ownerISOName = ac_val;}

  /**
    set the DEVCLASS of the owner
    @param aui8_val new DEVCLASS of owner
  */
  void setOwnerDevClass(uint8_t aui8_val){mc_ownerISOName.setDevClass(aui8_val);}

  /**
    set DEVCLASS and _instance_ of owner by giving pointer to owner ISOName
    @param apc_val pointer to owner ISOName
  */
  void setOwnerISOName(const IsoName_c* apc_val);

  /**
    check if this item has the same identity as defined by the parameters,
    if aui8_devClassInst is 0xFF a lazy match disregarding pos is done
    (important for matching received process data msg);
    if INSTANCE is defined (!= 0xFF) then one of the following conditions must be true:<ul>
    <li>parameter INSTANCE == ident INSTANCE (devClassInst())
    <li>parameter INSTANCE == owner INSTANCE ( ownerISOName().devClassInst() )
    <li>parameter ac_ownerISOName == ownerISOName()
    </ul>

    ISO parameter
    @param arc_isoNameSender compare this parameter with owner isoName (only for remote, local calls: IsoNameUnspecified)
    @param arc_isoNameReceiver compared isoName value
    @param aui16_DDI compared DDI value
    @param aui16_element compared element value

    @return true -> this instance has same Process-Data identity
  */
  bool matchISO( const IsoName_c& arc_isoNameSender,
                 const IsoName_c& arc_isoNameReceiver,
                 uint16_t aui16_DDI,
                 uint16_t aui16_element
               ) const;

  bool check4GroupMatch(uint16_t aui16_DDI, uint16_t aui16_element, const IsoName_c& ac_isoName);

  bool check4GroupMatchExisting(uint16_t aui16_DDI, uint16_t aui16_element, const IsoName_c& ac_isoName);

  bool checkProprietary4GroupMatch(uint16_t aui_deviceElement, const IsoName_c& ac_isoName);

  static bool isPair(uint16_t aui16_ElementDDI, uint16_t aui16_DDI);

  bool add2Group(uint16_t aui16_DDI);

  bool addProprietary2Group(uint16_t aui16_DDI, bool mb_isSetpoint, GeneralCommand_c::ValueGroup_t ddiType);

  static void getDDIType(uint16_t aui16_DDI, GeneralCommand_c::ValueGroup_t &r_ddiType, bool &rb_isSetpoint);

  static bool hasDDIType (uint16_t aui16_DDI, GeneralCommand_c::ValueGroup_t t_ddiType);

private: // Private attributes
  /** internal base function for copy constructor and assignement */
  void assignFromSource( const ProcIdent_c& arc_src );

  /** DEVCLASS code of process data identity */
  const IsoName_c* mpc_ownerISOName; // only defined for own local data, otherwise NULL
  /**
    in most cases equivalent with ((devClass << 3) | pos);
    for data with ident devClass==0 this is mostly NOT the same as the devClass of the owner,
    because then this value is of the general base data table
  */
  IsoName_c mc_ownerISOName;
  /** IsoName_c information for this instance
      ( the _instance_ part is important if more ECU of same _device_class_ are
      parallel active on the BUS)
    */
  IsoName_c mc_isoName;


   STL_NAMESPACE::list<IsoAgLib::ElementDdi_s> mlist_elementDDI;
   uint16_t mui16_element;

private: // Private methods

#if 0
/**
    claculates ident value for quick comparison
    @return single comparison value
  */
  int32_t calcIdentVal() const;
#endif

};

}
#endif
