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

namespace __IsoAgLib {

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
    @param rc_gtp optional GETY_POS code of Process-Data
    @param rui8_pri PRI code of messages with this process data instance (default 2)
    @param rc_ownerGtp optional GETY_POS code of owner of Process-Data
           ( important if GETY and/or POS differs from identity GTP in rc_gtp; this is the case
             for process data from base data dictionary table (GETY==0), which is managed/owned by device of
             type GETY != 0)
    @param rpc_ownerGtp pointer to the optional GETY_POS var of the owner (for automatic update as soon
            as corresponding device is registered as having claimed address in monitor table list)
  */
  ProcIdent_c::ProcIdent_c(
#ifdef USE_ISO_11783
                           const IsoAgLib::ElementDDI_s* ps_elementDDI,
#endif
#ifdef USE_DIN_9684
                           uint8_t rui8_lis,
                           uint8_t rui8_wert,
                           uint8_t rui8_inst,
                           uint8_t rui8_zaehlnum,
#endif
                           const GetyPos_c& rc_gtp,
                           uint8_t rui8_pri,
                           const GetyPos_c& rc_ownerGtp,
                           const GetyPos_c *rpc_ownerGtp,
                           int ri_singletonVecKey)
  : ClientBase( ri_singletonVecKey )
{
  init(
#ifdef USE_ISO_11783
            ps_elementDDI,
#endif
#ifdef USE_DIN_9684
            rui8_lis,
            rui8_wert,
            rui8_inst,
            rui8_zaehlnum,
#endif
            rc_gtp,
            rui8_pri,
            rc_ownerGtp,
            rpc_ownerGtp);
}

/** copy constructor */
ProcIdent_c::ProcIdent_c( const ProcIdent_c& rrefc_src )
  : ClientBase( rrefc_src )
{
  assignFromSource( rrefc_src );
}

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
    @param rc_gtp GETY_POS code of Process-Data
    @param rui8_pri optional PRI code of messages with this process data instance (default 2)
    @param rc_ownerGtp optional GETY_POS code of owner of Process-Data
           ( important if GETY and/or POS differs from identity GTP in rc_gtp; this is the case
             for process data from base data dictionary table (GETY==0), which is managed/owned by device of
             type GETY != 0)
    @param rpc_ownerGtp pointer to the optional GETY_POS var of the owner (for automatic update as soon
            as corresponding device is registered as having claimed address in monitor table list)
*/
void ProcIdent_c::init(
#ifdef USE_ISO_11783
            const IsoAgLib::ElementDDI_s* ps_elementDDI,
#endif
#ifdef USE_DIN_9684
            uint8_t rui8_lis,
            uint8_t rui8_wert,
            uint8_t rui8_inst,
            uint8_t rui8_zaehlnum,
#endif
            const GetyPos_c& rc_gtp,
            uint8_t rui8_pri,
            const GetyPos_c& rc_ownerGtp,
            const GetyPos_c *rpc_ownerGtp)
{
#ifdef USE_ISO_11783
  setElementDDI(ps_elementDDI);
#endif
#ifdef USE_DIN_9684
  setLis(rui8_lis);
  setWert(rui8_wert);
  setInst(rui8_inst);
  setZaehlnum(rui8_zaehlnum);
#endif

  data.c_gtp = rc_gtp;
  setPri(rui8_pri);
  pc_ownerGtp = rpc_ownerGtp;

  // the GETY_POS of ident is best defined by pointed value of rpc_ownerGtp
  if ( rpc_ownerGtp != 0 ) data.c_ownerGtp = *rpc_ownerGtp;
  // second choicer is explicit (not default) setting in rc_ownerGtp
  else if ( rc_ownerGtp.isSpecified() ) data.c_ownerGtp = rc_ownerGtp;
  // last choice is definition of c_ownerGtp by process data identiy
  else data.c_ownerGtp = rc_gtp;
}

/**
  copy constructor for class instance
  @rrrefcSrc source ProcIdent_c instance
  @return reference to source for cmd like "proc1 = proc2 = proc3;"
*/
ProcIdent_c& ProcIdent_c::operator=(const ProcIdent_c& rrefc_src){
  // first assign base class
  ClientBase::operator=(rrefc_src);
  // now assign flags of this class
  assignFromSource( rrefc_src );

  return *this;
}

/** internal base function for copy constructor and assignement */
void ProcIdent_c::assignFromSource( const ProcIdent_c& rrefc_src )
{
  setLis(rrefc_src.lis());
  data.c_gtp = rrefc_src.data.c_gtp;
  data.c_ownerGtp = rrefc_src.data.c_ownerGtp;
  pc_ownerGtp = rrefc_src.pc_ownerGtp;
#ifdef USE_ISO_11783
  // elementDDI() returns list reference, setElementDDI() expects pointer to list
  setElementDDI(&(rrefc_src.elementDDI()));
#endif
#ifdef USE_DIN_9684
  setWert(rrefc_src.wert());
  setInst(rrefc_src.inst());
  setZaehlnum(rrefc_src.zaehlnum());
#endif
  setPri(rrefc_src.pri());
}

/** default destructor which has nothing to do */
ProcIdent_c::~ProcIdent_c(){
}

/**
  set GETY and POS of owner by giving pointer to owner GETY_POS
  @param rpc_val pointer to owner GETY_POS
*/
void ProcIdent_c::setOwnerGtp(const GetyPos_c* rpc_val)
{
  pc_ownerGtp = rpc_val;
  data.c_ownerGtp = *rpc_val;
}


#ifdef USE_ISO_11783
/**
   check if this item has the same identity as defined by the parameters,
   if rui8_pos is 0xFF a lazy match disregarding pos is done
   (important for matching received process data msg);
   if POS is defined (!= 0xFF) then one of the following conditions must be true:<ul>
   <li>parameter POS == ident POS (pos())
   <li>parameter POS == owner POS ( ownerGtp().getPos() )
   <li>parameter rc_ownerGtp == ownerGtp()
   </ul>

   @param rui8_getyReceiver compared GETY value

   ISO parameter
   @param rui8_getySender compare this parameter with owner gety (only for remote, local calls: 0xFF)
   @param rui16_DDI compared DDI value
   @param rui16_element compared element value

   @return true -> this instance has same Process-Data identity
*/
bool ProcIdent_c::matchISO(
             uint8_t rui8_getyReceiver,
             uint8_t rui8_getySender,
             uint16_t rui16_DDI,
             uint16_t rui16_element
             ) const
{
  // check wether current element/DDI combination matches one list element in process data element/DDI list
  std::list<IsoAgLib::ElementDDI_s>::const_iterator iter = NULL;
  for (iter = data.l_elementDDI.begin(); iter != data.l_elementDDI.end(); iter++)
    if ( (iter->ui16_DDI == rui16_DDI) && (iter->ui16_element == rui16_element) )
      break;

  if (iter == data.l_elementDDI.end())
    return false;

  if (rui8_getySender != 0xFF) {
    // check in remote case: check if gety of ownerGtp in procident matches gety of sender
    if (ownerGtp().getGety() != rui8_getySender) return false;
  } else {
    // check in local case: check if procident gety matches gety of empf
    if (gety() != rui8_getyReceiver) return false;
  }

  // all previous tests are positive -> answer positive match
  return true;
}
#endif


#ifdef USE_DIN_9684
/**
   check if this item has the same identity as defined by the parameters,
   if rui8_pos is 0xFF a lazy match disregarding pos is done
   (important for matching received process data msg);
   if POS is defined (!= 0xFF) then one of the following conditions must be true:<ul>
   <li>parameter POS == ident POS (pos())
   <li>parameter POS == owner POS ( ownerGtp().getPos() )
   <li>parameter rc_ownerGtp == ownerGtp()
   </ul>

   @param rui8_gety compared GETY value

   DIN parameter
   @param rui8_lis compared LIS value
   @param rui8_wert compared WERT value
   @param rui8_inst compared INST value
   @param rui8_zaehlnum compared ZAEHLNUM value (default 0xFF == complete working width)

   @param rui8_pos optional compared POS
   @param rc_ownerGtp optional compared GETY_POS of owner
   @return true -> this instance has same Process-Data identity
*/
bool ProcIdent_c::matchDIN(
             uint8_t rui8_gety,
             uint8_t rui8_lis,
             uint8_t rui8_wert,
             uint8_t rui8_inst,
             uint8_t rui8_zaehlnum ,
             uint8_t rui8_pos ,
             const GetyPos_c& rc_ownerGtp
             ) const
{

  if (zaehlnum() != rui8_zaehlnum) return false;
  if (lis() != rui8_lis) return false;
  if (wert() != rui8_wert) return false;
  if (inst() != rui8_inst) return false;

  if (gety() != rui8_gety) return false;

  // only return false for different pos setting, if rui8_pos != 0xFF -> no lazy match is wanted
  // one of the POS checks must be true to answer positive match
  if ( ( rui8_pos != 0xFF )
    && ( pos() != rui8_pos )
    && (ownerGtp().getPos() != rui8_pos)
    && (ownerGtp() != rc_ownerGtp)) return false;

  // all previous tests are positive -> answer positive match
  return true;
}
#endif

#ifdef USE_ISO_11783
void ProcIdent_c::setElementDDI(const IsoAgLib::ElementDDI_s* ps_elementDDI)
{
  data.l_elementDDI.clear();
  // check if pointer to strcut (array) is set (constructor call with NULL possible!)
  if (ps_elementDDI) {
    // in last struct element == 0xFFFF
    while (ps_elementDDI->ui16_element != 0xFFFF)
      data.l_elementDDI.push_back(*ps_elementDDI++);
  }
}

void ProcIdent_c::setElementDDI(const std::list<IsoAgLib::ElementDDI_s>* pl_elementDDI)
{
  data.l_elementDDI.clear();
  // check if pointer to strcut (array) is set (constructor call with NULL possible!)
  if (pl_elementDDI) {
    for (std::list<IsoAgLib::ElementDDI_s>::const_iterator iter = pl_elementDDI->begin();
         iter != pl_elementDDI->end(); iter++)
        data.l_elementDDI.push_back(*iter);
  }
}

#endif

#if 0
/**
  claculates ident value for quick comparison
  @return single comparison value
*/
int32_t ProcIdent_c::calcIdentVal() const {
  int32_t i32_result = (static_cast<int32_t>(gtp().getGety()) & 0x7F   );

    #ifdef USE_ISO_11783
    #ifdef USE_DIN_9684
    if ( element() != 0xFFFF )
    #endif
    { // take ISO ident settings for compare
      i32_result |=
      (
        // @todo: find algorithm for handling element/DDI list
        (static_cast<int32_t>(element() & 0x1FF) << 7   ) // len 12 bit -> reduce to 9 Bit
      | (static_cast<int32_t>(DDI())             << 16  )
      );
    }
    #else
      i32_result |=
      (
        (static_cast<int32_t>(gtp().getPos())    << 8   )
      | (static_cast<int32_t>(lis())             << 12  )
      | (static_cast<int32_t>(inst())            << 16  )
      | (static_cast<int32_t>(wert())            << 20  )
      | (static_cast<int32_t>(zaehlnum())        << 24  )
      );
    #endif
  return i32_result;
};
#endif


} // end of namespace __IsoAgLib

