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
#include <IsoAgLib/util/impl/getypos_c.h>
#include <IsoAgLib/util/impl/singleton.h>
#include "generalcommand_c.h"
#ifdef USE_ISO_11783
  #include "../elementddi_s.h"
  #include <list>
#endif

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
    @param ps_elementDDI optional pointer to array of structure IsoAgLib::ElementDDI_s which contains DDI, element, isSetpoint and ValueGroup
                         (array is terminated by ElementDDI_s.ui16_element == 0xFFFF)
    DIN parameter
    @param rui8_lis optional LIS code of Process-Data
    @param rui8_wert optional WERT code of Process-Data
    @param rui8_inst optional INST code of Process-Data
    @param rui8_zaehlnum optional ZAEHLNUM  code of Process-Data

    common parameter
    @param rc_devKey optional DEV_KEY code of Process-Data
    @param rui8_pri PRI code of messages with this process data instance (default 2)
    @param rc_ownerDevKey optional DEV_KEY code of owner of Process-Data
           ( important if DEVCLASS and/or DEVCLASSINST differs from identity DEVKEY in rc_devKey; this is the case
             for process data from base data dictionary table (DEVCLASS==0), which is managed/owned by device of
             type DEVCLASS != 0)
    @param rpc_ownerDevKey pointer to the optional DEV_KEY var of the owner (for automatic update as soon
            as corresponding device is registered as having claimed address in monitor table list)
  */
  ProcIdent_c(
#ifdef USE_ISO_11783
              const IsoAgLib::ElementDDI_s* ps_elementDDI = NULL,
#endif
#ifdef USE_DIN_9684
              uint8_t rui8_lis = 0,
              uint8_t rui8_wert = 0,
              uint8_t rui8_inst = 0,
              uint8_t rui8_zaehlnum = 0xFF,
#endif
              const DevKey_c& rc_devKey = DevKey_c::DevKeyInitialProcessData,
              uint8_t rui8_pri = 2,
              const DevKey_c& rc_ownerDevKey = DevKey_c::DevKeyUnspecified,
              const DevKey_c *rpc_ownerDevKey = NULL,
              int ri_singletonVecKey = 0);

  /** copy constructor */
  ProcIdent_c( const ProcIdent_c& rrefc_src );

  /**
    initialisation which can set this process data instance to a defined intial state
    ISO parameter
    @param ps_elementDDI optional pointer to array of structure IsoAgLib::ElementDDI_s which contains DDI, element, isSetpoint and ValueGroup
                         (array is terminated by ElementDDI_s.ui16_element == 0xFFFF)

    DIN parameter
    @param rui8_lis LIS code of Process-Data
    @param rui8_wert WERT code of Process-Data
    @param rui8_inst INST code of Process-Data
    @param rui8_zaehlnum ZAEHLNUM  code of Process-Data

    common parameter
    @param rc_devKey DEV_KEY code of Process-Data
    @param rui8_pri optional PRI code of messages with this process data instance (default 2)
    @param rc_ownerDevKey optional DEV_KEY code of owner of Process-Data
           ( important if DEVCLASS and/or DEVCLASSINST differs from identity DEVKEY in rc_devKey; this is the case
             for process data from base data dictionary table (DEVCLASS==0), which is managed/owned by device of
             type DEVCLASS != 0)
    @param rpc_ownerDevKey pointer to the optional DEV_KEY var of the owner (for automatic update as soon
            as corresponding device is registered as having claimed address in monitor table list)
  */
  void init(
#ifdef USE_ISO_11783
            const IsoAgLib::ElementDDI_s* ps_elementDDI,
#endif
#ifdef USE_DIN_9684
            uint8_t rui8_lis,
            uint8_t rui8_wert,
            uint8_t rui8_inst,
            uint8_t rui8_zaehlnum,
#endif
            const DevKey_c& rc_devKey,
            uint8_t rui8_pri = 2,
            const DevKey_c& rc_ownerDevKey = DevKey_c::DevKeyUnspecified,
            const DevKey_c *rpc_ownerDevKey = NULL);

  /**
    copy constructor for class instance
    @param rrefc_src source ProcIdent_c instance
    @return reference to source for cmd like "proc1 = proc2 = proc3;"
  */
  ProcIdent_c& operator=(const ProcIdent_c& rrefc_src);
  /** default destructor which has nothing to do */
  ~ProcIdent_c();

#if 0
  /**
    comparison of two process ident instances
    @param rrefc_right compared object
    @return true -> this instance is equal to the other
  */
  bool operator==(const ProcIdent_c& rrfec_right) const
    {return (calcIdentVal() == rrfec_right.calcIdentVal());};
  /**
    differ comparison operator with another ProcIdent_c instance
    @param rrefc_right compared object
    @return true -> this indstance is different from the other
  */
  bool operator!=(const ProcIdent_c& rrfec_right) const
    {return (calcIdentVal() != rrfec_right.calcIdentVal());};
  /**
    lower than comparison with another ProcIdent_c instance
    @param rrefc_right compared object
    @return true -> this instance is < than the other
  */
  bool operator<(const ProcIdent_c& rrfec_right) const
    {return (calcIdentVal() < rrfec_right.calcIdentVal());};
  /**
    greater than comparison with another ProcIdent_c instance
    @param rrefc_right compared object
    @return true -> this indstance is > than the other
  */
  bool operator>(const ProcIdent_c& rrfec_right) const
    {return (calcIdentVal() > rrfec_right.calcIdentVal());};
#endif

  // member variable access

#ifdef USE_ISO_11783
  /**
     deliver list of ElementDDI_s
     @return std::list<IsoAgLib::ElementDDI_s>
  */
  const std::list<IsoAgLib::ElementDDI_s>& elementDDI()const {return data.l_elementDDI;};

  /**
    deliver value DDI (only possible if only one elementDDI in list)
    @return DDI
  */
  uint16_t DDI() const{
    if (data.l_elementDDI.size() == 1)
      return data.l_elementDDI.begin()->ui16_DDI;
    else
      return 0;
  };
  /**
    deliver value element (only possible if only one elementDDI in list)
    @return element
  */
  uint16_t element() const{
   if (data.l_elementDDI.size() == 1)
      return data.l_elementDDI.begin()->ui16_element;
    else
      return 0xFFFF;
  };

#endif


  /**
    deliver value PRI of messages with this
    process data instance
    @return PRI
  */
  uint8_t pri() const{return data.ui8_pri;};
  /**
    deliver value LIS (list number)
    @return LIS
  */
  uint8_t lis() const{return data.ui8_lis;};
  /**
    deliver value DEVCLASS (machine type specific table of process data types)
    everytime deliver the identity DEVCLASS (and NOT the possibly differing DEVCLASS of the owner)
    @return DEVCLASS
  */
  uint8_t devClass() const{return data.c_devKey.getDevClass();};
  /**
    deliver value DEV_KEY (machine type specific table of process data types)
    use everytime the _device_class_ from the ident part, and take the _instance_ from the owner.
    Special Case: if DevKey is set to 0,0 -> don't use any logac logic and return
    stored value
    @return DEV_KEY
  */
  const DevKey_c& devKey() const
  #ifdef USE_DIN_9684
  {if( ( data.c_devKey.getDevClass() == 0 ) &&  ( data.c_devKey.getDevClassInst() == 0 ) )
               return   data.c_devKey;
          else
               return           DevKey_c(data.c_devKey.getDevClass(), ownerDevKey().getDevClassInst());};
  #else
  {return ownerDevKey();};
  #endif
  /**
    deliver value WERT (row of process data table)
    @return WERT
  */
  uint8_t wert() const{return data.ui8_wert;};
  /**
    deliver value INST (column of process data table)
    @return INST
  */
  uint8_t inst() const{return data.ui8_inst;};
  /**
    deliver value ZAEHLNUM (0xFF == whole working width; else parts of width)
    @return ZAEHLNUM
  */
  uint8_t zaehlnum() const{return data.ui8_zaehlnum;};
  /**
    deliver value _instance_ (important if more than one machine with equal _device_class_ are active)
    deliver the device class instance of the owner, as this _instance_ is sometimes updated after the creation of this
    process data instance.
    @return POS
  */
  uint8_t devClassInst() const{return ownerDevKey().getDevClassInst();};
  /**
    deliver the owner devKey (retrieved from pointed devKey value, if valid pointer)
    @return actual DEV_KEY of owner
  */
  const DevKey_c& ownerDevKey() const
    { return ((pc_ownerDevKey != 0)?(*pc_ownerDevKey):(data.c_ownerDevKey));};

#ifdef USE_ISO_11783

  /**
    set DDI, element, value group and setpoint/measure type of process msg
    @param rl_elementDDI
  */
  void setElementDDI(const IsoAgLib::ElementDDI_s* ps_elementDDI);

  /**
    set DDI, element, value group and setpoint/measure type of process msg (used in assignFromSource)
    @param pl_elementDDI
  */
  void setElementDDI(const std::list<IsoAgLib::ElementDDI_s>* pl_elementDDI);

#endif

  /**
    set value PRI of messages with this
    process data instance
    @param rui8_val new PRI value
  */
  void setPri(uint8_t rui8_val){data.ui8_pri = rui8_val;};
  /**
    set value LIS (list number)
    @param rui8_val new LIS value
  */
  void setLis(uint8_t rui8_val){data.ui8_lis = rui8_val;};
  /**
    set value DEVCLASS (machine type specific table of process data types)
    @param rui8_val new DEVCLASS val
  */
  void setDevClass(uint8_t rui8_val){data.c_devKey.setDevClass(rui8_val);};
  /**
    set value DEV_KEY (machine type specific table of process data types)
    @param rc_val new DEV_KEY val
  */
  void setDevKey(const DevKey_c& rc_val){data.c_devKey = rc_val;};
  /**
    set value WERT (row of process data table)
    @param rui8_val new WERT val
  */
  void setWert(uint8_t rui8_val){data.ui8_wert = rui8_val;};
  /**
    set value INST (column of process data table)
    @param rui8_val new INST val
  */
  void setInst(uint8_t rui8_val){data.ui8_inst = rui8_val;};
  /**
    set value ZAEHLNUM (0xFF == whole working width; else parts of width)
    @param rui8_val new ZAEHLNUM val
  */
  void setZaehlnum(uint8_t rui8_zaehlnum){data.ui8_zaehlnum = rui8_zaehlnum;};
  /**
    set value _instance_ (important if more than one machine with equal _device_class_ are active)
    set also the _instance_ of the owner as the owner _instance_ shall be always the most actual value
    @param rui8_val new device class inst val
  */
  void setDevClassInst(uint8_t rui8_val){data.c_devKey.setDevClassInst(rui8_val); data.c_ownerDevKey.setDevClassInst(rui8_val);};
  /**
    set the owner devKey
    @param rc_val new DEV_KEY of owner
  */
  void setOwnerDevKey(const DevKey_c& rc_val){data.c_ownerDevKey = rc_val;};
  /**
    set the DEVCLASS of the owner
    @param rui8_val new DEVCLASS of owner
  */
  void setOwnerDevClass(uint8_t rui8_val){data.c_ownerDevKey.setDevClass(rui8_val);};
  /**
    set DEVCLASS and _instance_ of owner by giving pointer to owner DEV_KEY
    @param rpc_val pointer to owner DEV_KEY
  */
  void setOwnerDevKey(const DevKey_c* rpc_val);

  /**
    check if this item has the same identity as defined by the parameters,
    if rui8_devClassInst is 0xFF a lazy match disregarding pos is done
    (important for matching received process data msg);
    if INSTANCE is defined (!= 0xFF) then one of the following conditions must be true:<ul>
    <li>parameter INSTANCE == ident INSTANCE (devClassInst())
    <li>parameter INSTANCE == owner INSTANCE ( ownerDevKey().getDevClassInst() )
    <li>parameter rc_ownerDevKey == ownerDevKey()
    </ul>


    ISO parameter
    @param rui8_devClassReceiver compared DEVCLASS value
    @param rui8_devClassSender compare this parameter with owner devClass (only for remote, local calls: 0xFF)
    @param rui16_DDI compared DDI value
    @param rui16_element compared element value

    DIN parameter
    @param rui8_lis compared LIS value
    @param rui8_wert compared WERT value
    @param rui8_inst compared INST value
    @param rui8_zaehlnum compared ZAEHLNUM value (default 0xFF == complete working width)

    @return true -> this instance has same Process-Data identity
  */
#ifdef USE_ISO_11783
  bool matchISO(
             uint8_t rui8_devClassReceiver,
             uint8_t rui8_devClassSender,
             uint16_t rui16_DDI,
             uint16_t rui16_element
             ) const;
#endif

#ifdef USE_DIN_9684
  bool matchDIN(
             uint8_t rui8_devClass,
             uint8_t rui8_lis,
             uint8_t rui8_wert,
             uint8_t rui8_inst,
             uint8_t rui8_zaehlnum = 0xFF,
             uint8_t rui8_devClassInst = 0xFF,  // default values from DIN
             const DevKey_c& rc_ownerDevKey = DevKey_c::DevKeyUnspecified // default values from DIN
             ) const;
#endif

private: // Private attributes
  /** internal base function for copy constructor and assignement */
  void assignFromSource( const ProcIdent_c& rrefc_src );

  /** DEVCLASS code of process data identity */
  const DevKey_c* pc_ownerDevKey; // only defined for own local data, otherwise NULL
  struct _data {
    /**
      in most cases equivalent with ((devClass << 3) | pos);
      for data with ident devClass==0 this is mostly NOT the same as the devClass of the owner,
      because then this value is of the general base data table
    */
    DevKey_c c_ownerDevKey;
    /** DevKey_c information for this instance
        ( the _instance_ part is important if more ECU of same _device_class_ are
        parallel active on the BUS)
      */
    DevKey_c c_devKey;
    /** zaehlnum code */
    uint16_t ui8_zaehlnum : 8;

    /** WERT code of process data identity */
    uint16_t ui8_wert : 4;
    /** INST code of process data identity */
    uint16_t ui8_inst : 4;

    /** LIS code of process data id */
    uint16_t ui8_lis : 3;
    /** PRI code of process data messages; mostly 2 */
    uint16_t ui8_pri : 3;

#ifdef USE_ISO_11783
     std::list<IsoAgLib::ElementDDI_s> l_elementDDI;
#endif

  } data;

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
