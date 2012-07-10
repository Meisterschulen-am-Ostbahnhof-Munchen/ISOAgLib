/*
  procident_c.h: identity of a process data instance for easy search

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef PROC_IDENT_H
#define PROC_IDENT_H

#include <IsoAgLib/isoaglib_config.h>
#include "processcmd_c.h"
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isoname_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/elementddi_s.h>
#include <IsoAgLib/util/impl/singleton.h>
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
    @param aps_elementDDI optional pointer to array of structure IsoAgLib::ElementDdi_s which contains DDI, isSetpoint and ValueGroup
                         (array is terminated by ElementDdi_s.ddi == 0xFFFF, default: NULL)
    @param aui16_element device element number (default: 0xFFFF)

    common parameter
    @param acrc_isoName optional ISOName code of Process-Data
    @param apc_externalOverridingIsoName pointer to the optional ISOName var (for automatic update as soon
            as corresponding device is registered as having claimed address in monitor table list)
  */
  ProcIdent_c(
              const IsoAgLib::ElementDdi_s* aps_elementDDI,
              uint16_t aui16_element,
              const IsoName_c& acrc_isoName,
              const IsoName_c *apc_externalOverridingIsoName = NULL,
              int ai_multitonInst = 0);

  /** copy constructor */
  ProcIdent_c( const ProcIdent_c& acrc_src );

  /**
    initialisation which can set this process data instance to a defined intial state
    ISO parameter
    @param aps_elementDDI optional pointer to array of structure IsoAgLib::ElementDdi_s which contains DDI, isSetpoint and ValueGroup
                         (array is terminated by ElementDdi_s.ddi == 0xFFFF)
    @param aui16_element device element number

    common parameter
    @param acrc_isoName ISOName code of Process-Data
    @param apc_externalOverridingIsoName pointer to the optional ISOName var (for automatic update as soon
            as corresponding device is registered as having claimed address in monitor table list)
  */
  void init(
            const IsoAgLib::ElementDdi_s* aps_elementDDI,
            uint16_t aui16_element,
            const IsoName_c& acrc_isoName,
            const IsoName_c *apc_externalOverridingIsoName = NULL);

  /**
    copy constructor for class instance
    @param acrc_src source ProcIdent_c instance
    @return reference to source for cmd like "proc1 = proc2 = proc3;"
  */
  ProcIdent_c& operator=(const ProcIdent_c& acrc_src);
  /** default destructor which has nothing to do */
  ~ProcIdent_c();

#if 0
  /**
    comparison of two process ident instances
    @param acrc_right compared object
    @return true -> this instance is equal to the other
  */
  bool operator==(const ProcIdent_c& acrc_right) const
    {return (calcIdentVal() == acrc_right.calcIdentVal());};
  /**
    differ comparison operator with another ProcIdent_c instance
    @param acrc_right compared object
    @return true -> this indstance is different from the other
  */
  bool operator!=(const ProcIdent_c& acrc_right) const
    {return (calcIdentVal() != acrc_right.calcIdentVal());};
  /**
    lower than comparison with another ProcIdent_c instance
    @param acrc_right compared object
    @return true -> this instance is < than the other
  */
  bool operator<(const ProcIdent_c& acrc_right) const
    {return (calcIdentVal() < acrc_right.calcIdentVal());};
  /**
    greater than comparison with another ProcIdent_c instance
    @param acrc_right compared object
    @return true -> this indstance is > than the other
  */
  bool operator>(const ProcIdent_c& acrc_right) const
    {return (calcIdentVal() > acrc_right.calcIdentVal());};
#endif

  // member variable access

  /**
     deliver list of ElementDdi_s
     @return STL_NAMESPACE::list<IsoAgLib::ElementDdi_s>
  */
  const STL_NAMESPACE::list<IsoAgLib::ElementDdi_s>& elementDDI()const {return mlist_elementDDI;}

  /** check if this ProcIdent_c has the given DDI as element */
  bool hasDDI( uint16_t aui16_checkDDI ) const;
  bool hasType(bool ab_isSetpoint, ProcessCmd_c::ValueGroup_t t_ddiType) const;

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
    deliver value _instance_ (important if more than one machine with equal _device_class_ are active)
    deliver the device class instance of the owner, as this _instance_ is sometimes updated after the creation of this
    process data instance.
    @return POS
  */
  uint8_t devClassInst() const{return isoName().devClassInst();}

  /**
    deliver the isoName (retrieved from pointed isoName value, if valid pointer)
    @return actual ISOName
  */
  const IsoName_c& isoName() const
    { return ((mpc_externalOverridingIsoName != 0)?(*mpc_externalOverridingIsoName):(mc_isoName));}

  /**
    set DDI, value group and setpoint/measure type of process msg
    @param aps_elementDDI
  */
  void setElementDDI(const IsoAgLib::ElementDdi_s* aps_elementDDI);

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
  void setDevClassInst(uint8_t aui8_val){mc_isoName.setDevClassInst(aui8_val);}

  /**
    set pointer to external isoName instances (used by isoName())
    @param apc_val pointer to ISOName
  */
  void setExternalOverridingIsoName(const IsoName_c* apc_val);

  /**
    check if this item has the same identity as defined by the parameters,
    if aui8_devClassInst is 0xFF a lazy match disregarding pos is done
    (important for matching received process data msg);
    if INSTANCE is defined (!= 0xFF) then one of the following conditions must be true:<ul>
    <li>parameter INSTANCE == ident INSTANCE (devClassInst())
    <li>parameter acrc_isoName == isoName()
    </ul>

    ISO parameter
    @param acrc_isoNameSender compare this parameter with owner isoName (only for remote, local calls: IsoNameUnspecified)
    @param acrc_isoNameReceiver compared isoName value
    @param aui16_DDI compared DDI value
    @param aui16_element compared element value

    @return true -> this instance has same Process-Data identity
  */
  bool matchISO( const IsoName_c& acrc_isoNameSender,
                 const IsoName_c& acrc_isoNameReceiver,
                 uint16_t aui16_DDI,
                 uint16_t aui16_element
               ) const;

  bool check4GroupMatch(uint16_t aui16_DDI, uint16_t aui16_element, const IsoName_c& acrc_isoName);

  bool check4GroupMatchExisting(uint16_t aui16_DDI, uint16_t aui16_element, const IsoName_c& acrc_isoName);

  bool checkProprietary4GroupMatch(uint16_t aui_deviceElement, const IsoName_c& acrc_isoName);

  static bool isPair(uint16_t aui16_ElementDDI, uint16_t aui16_DDI);

  bool add2Group(uint16_t aui16_DDI);

  bool addProprietary2Group(uint16_t aui16_DDI, bool mb_isSetpoint, ProcessCmd_c::ValueGroup_t ddiType);

  static void getDDIType(uint16_t aui16_DDI, ProcessCmd_c::ValueGroup_t &r_ddiType, bool &rb_isSetpoint);

  static bool hasDDIType (uint16_t aui16_DDI, ProcessCmd_c::ValueGroup_t t_ddiType);


protected:
   STL_NAMESPACE::list<IsoAgLib::ElementDdi_s> mlist_elementDDI;


private: // Private attributes
  /** internal base function for copy constructor and assignement */
  void assignFromSource( const ProcIdent_c& acrc_src );

  /** DEVCLASS code of process data identity */
  const IsoName_c* mpc_externalOverridingIsoName; // only defined for own local data, otherwise NULL

  /** IsoName_c information for this instance
      ( the _instance_ part is important if more ECU of same _device_class_ are
      parallel active on the BUS)
    */
  IsoName_c mc_isoName;

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
