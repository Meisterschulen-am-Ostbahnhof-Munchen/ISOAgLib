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
    constructor whch can optional set all member values
    @param rui8_lis optional LIS code of Process-Data
    @param rc_gtp optional GETY_POS code of Process-Data
    @param rui8_wert optional WERT code of Process-Data
    @param rui8_inst optional INST code of Process-Data
    @param rui8_zaehlnum optional ZAEHLNUM  code of Process-Data
    @param rui8_pri PRI code of messages with this process data instance (default 2)
    @param rc_ownerGtp optional GETY_POS code of owner of Process-Data
           ( important if GETY and/or POS differs from identity GTP in rc_gtp; this is the case
             for process data from base data dictionary table (GETY==0), which is managed/owned by device of
             type GETY != 0)
    @param rpc_ownerGtp pointer to the optional GETY_POS var of the owner (for automatic update as soon
            as corresponding device is registered as having claimed address in monitor table list)
  */
  ProcIdent_c(uint8_t rui8_lis = 0, GetyPos_c rc_gtp = GetyPos_c(0, 0xF), uint8_t rui8_wert = 0,
          uint8_t rui8_inst = 0, uint8_t rui8_zaehlnum = 0xFF, uint8_t rui8_pri = 2,
          GetyPos_c rc_ownerGtp = GetyPos_c(0xF, 0xF), GetyPos_c *rpc_ownerGtp = 0, int ri_singletonVecKey = 0);
  /** copy constructor */
	ProcIdent_c( const ProcIdent_c& rrefc_src );
  /**
    initialisation which can set this process data instance to a defined intial state
    @param rui8_lis LIS code of Process-Data
    @param rc_gtp GETY_POS code of Process-Data
    @param rui8_wert WERT code of Process-Data
    @param rui8_inst INST code of Process-Data
    @param rui8_zaehlnum ZAEHLNUM  code of Process-Data
    @param rui8_pri optional PRI code of messages with this process data instance (default 2)
    @param rc_ownerGtp optional GETY_POS code of owner of Process-Data
           ( important if GETY and/or POS differs from identity GTP in rc_gtp; this is the case
             for process data from base data dictionary table (GETY==0), which is managed/owned by device of
             type GETY != 0)
    @param rpc_ownerGtp pointer to the optional GETY_POS var of the owner (for automatic update as soon
            as corresponding device is registered as having claimed address in monitor table list)
  */
  void init(uint8_t rui8_lis, GetyPos_c rc_gtp, uint8_t rui8_wert,
          uint8_t rui8_inst, uint8_t rui8_zaehlnum, uint8_t rui8_pri = 2,
          GetyPos_c rc_ownerGtp = GetyPos_c(0xF, 0xF), GetyPos_c *rpc_ownerGtp = 0);
	/**
    copy constructor for class instance
    @param rrefc_src source ProcIdent_c instance
    @return reference to source for cmd like "proc1 = proc2 = proc3;"
  */
  ProcIdent_c& operator=(const ProcIdent_c& rrefc_src);
  /** default destructor which has nothing to do */
  ~ProcIdent_c();

  /**
    comparison of two process ident instances
    @param rrefc_right compared object
    @return true -> this instance is equal to the other
  */
  bool operator==(const ProcIdent_c& rrfec_right) const
    {return (calc_identVal() == rrfec_right.calc_identVal());};
  /**
    differ comparison operator with another ProcIdent_c instance
    @param rrefc_right compared object
    @return true -> this indstance is different from the other
  */
  bool operator!=(const ProcIdent_c& rrfec_right) const
    {return (calc_identVal() != rrfec_right.calc_identVal());};
  /**
    lower than comparison with another ProcIdent_c instance
    @param rrefc_right compared object
    @return true -> this instance is < than the other
  */
  bool operator<(const ProcIdent_c& rrfec_right) const
    {return (calc_identVal() < rrfec_right.calc_identVal());};
  /**
    greater than comparison with another ProcIdent_c instance
    @param rrefc_right compared object
    @return true -> this indstance is > than the other
  */
  bool operator>(const ProcIdent_c& rrfec_right) const
    {return (calc_identVal() > rrfec_right.calc_identVal());};

  // member variable access

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
    deliver value GETY (machine type specific table of process data types)
    everytime deliver the identity GETY (and NOT the possibly differing GETY of the owner)
    @return GETY
  */
  uint8_t gety() const{return data.c_gtp.getGety();};
  /**
    deliver value GETY_POS (machine type specific table of process data types)
    use everytime the GETY from the ident part, and take the POS from the owner.
    Special Case: if GetyPos is set to 0,0 -> don't use any logac logic and return
    stored value
    @return GETY_POS
  */
  GetyPos_c gtp() const {return ( data.c_gtp == GetyPos_c( 0, 0 ) )
                                ?data.c_gtp
                                :GetyPos_c(data.c_gtp.getGety(), ownerGtp().getPos());};
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
    deliver value POS (important if more than one machine with equal GETY are active)
    deliver the POS of the owner, as this POS is sometimes updated after the creation of this
    process data instance.
    @return POS
  */
  uint8_t pos() const{return ownerGtp().getPos();};
  /**
    deliver the owner gtp (retrieved from pointed gtp value, if valid pointer)
    @return actual GETY_POS of owner
  */
  GetyPos_c ownerGtp() const
    { return ((pc_ownerGtp != 0)?(*pc_ownerGtp):(data.c_ownerGtp));};

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
    set value GETY (machine type specific table of process data types)
    @param rui8_val new GETY val
  */
  void setGety(uint8_t rui8_val){data.c_gtp.setGety(rui8_val);};
  /**
    set value GETY_POS (machine type specific table of process data types)
    @param rc_val new GETY_POS val
  */
  void setGtp(GetyPos_c rc_val){data.c_gtp = rc_val;};
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
    set value POS (important if more than one machine with equal GETY are active)
    set also the POS of the owner as the owner POS shall be always the most actual value
    @param rui8_val new POS val
  */
  void setPos(uint8_t rui8_val){data.c_gtp.setPos(rui8_val); data.c_ownerGtp.setPos(rui8_val);};
  /**
    set the owner gtp
    @param rc_val new GETY_POS of owner
  */
  void setOwnerGtp(GetyPos_c rc_val){data.c_ownerGtp = rc_val;};
  /**
    set the GETY of the owner
    @param rui8_val new GETY of owner
  */
  void setOwnerGety(uint8_t rui8_val){data.c_ownerGtp.setGety(rui8_val);};
  /**
    set GETY and POS of owner by giving pointer to owner GETY_POS
    @param rpc_val pointer to owner GETY_POS
  */
  void setOwnerGtp(GetyPos_c* rpc_val);

  /**
    check if this item has the same identity as defined by the parametes,
    if rui8_pos is 0xFF a lazy match disregarding pos is done
    (important for matching received process data msg);
    if POS is defined (!= 0xFF) then one of the following conditions must be true:<ul>
    <li>parameter POS == ident POS (pos())
    <li>parameter POS == owner POS ( ownerGtp().getPos() )
    <li>parameter rc_ownerGtp == ownerGtp()
    </ul>
    @param rui8_lis compared LIS value
    @param rui8_gety compared GETY value
    @param rui8_wert compared WERT value
    @param rui8_inst compared INST value
    @param rui8_zaehlnum compared ZAEHLNUM value (default 0xFF == complete working width)
    @param rui8_pos optional compared POS
    @param rui8_pri PRI code of messages with this process data instance (default 2)
    @param rc_ownerGtp optional compared GETY_POS of owner
    @return true -> this instance has same Process-Data identity
  */
  bool match(uint8_t rui8_lis, uint8_t rui8_gety, uint8_t rui8_wert, uint8_t rui8_inst,
             uint8_t rui8_zaehlnum = 0xFF, uint8_t rui8_pos = 0xFF, uint8_t rui8_pri = 2, GetyPos_c rc_ownerGtp = GetyPos_c(0xF, 0xF)) const;

private: // Private attributes
	/** internal base function for copy constructor and assignement */
	void assignFromSource( const ProcIdent_c& rrefc_src );

  /** GETY code of process data identity */
  GetyPos_c* pc_ownerGtp; // only defined for own local data, otherwise NULL
  struct _data {
    /**
      in most cases equivalent with ((gety << 3) | pos);
      for data with ident gety==0 this is mostly NOT the same as the gety of the owner,
      because then this value is of the general base data table
    */
    GetyPos_c c_ownerGtp;
    /** GetyPos_c information for this instance
        ( the POS part is important if more ECU of same GETY are
        parallel active on the BUS)
      */
    GetyPos_c c_gtp;
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
  } data;
private: // Private methods
  /**
    claculates ident value for quick comparison
    @return single comparison value
  */
  int32_t calc_identVal() const;
};
/**
  claculates ident value for quick comparison with given values
  @param rui8_lis compared LIS value
  @param rui8_gety compared GETY value
  @param rui8_wert compared WERT value
  @param rui8_inst compared INST value
  @param rui8_zaehlnum compared ZAEHLNUM value (default 0xFF == complete working width)
  @param rc_ownerGtp optional compared GETY_POS of owner
  @param rui8_pos optional compared POS
  @param rui8_pri PRI code of messages with this process data instance (default 2)
  @return single comparison value
*/
int32_t calc_identVal(uint8_t rui8_lis, uint8_t rui8_gety, uint8_t rui8_wert, uint8_t rui8_inst,
                    uint8_t rui8_zaehlnum = 0, uint8_t rui8_pos = 0xF, uint8_t rui8_pri = 2);

/**
  claculates ident value for quick comparison with given values
  @param rui8_lis compared LIS value
  @param rc_gtp compared GETY_POS value
  @param rui8_wert compared WERT value
  @param rui8_inst compared INST value
  @param rui8_zaehlnum compared ZAEHLNUM value (default 0xFF == complete working width)
  @param rc_ownerGtp optional compared GETY_POS of owner
  @param rui8_pri PRI code of messages with this process data instance (default 2)
  @return single comparison value
*/
int32_t calc_identVal(uint8_t rui8_lis, GetyPos_c rc_gtp, uint8_t rui8_wert, uint8_t rui8_inst,
                    uint8_t rui8_zaehlnum = 0, uint8_t rui8_pri = 2);

}
#endif
