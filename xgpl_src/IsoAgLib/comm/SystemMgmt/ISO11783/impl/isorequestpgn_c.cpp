/***************************************************************************
                          isomonitor_c.cpp - object for monitoring members
                                              (list of ISOItem_c)
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
#include "isorequestpgn_c.h"
#include "isorequestpgnhandler_c.h"

#include <IsoAgLib/driver/can/impl/canio_c.h>
#include "isomonitor_c.h"

/*
#include "isosystempkg_c.h"
#include <IsoAgLib/util/liberr_c.h>
#include <IsoAgLib/comm/Scheduler/impl/scheduler_c.h>
#include <IsoAgLib/comm/SystemMgmt/impl/systemmgmt_c.h>

#if defined(DEBUG) || defined(DEBUG_HEAP_USEAGE)
  #include <IsoAgLib/util/impl/util_funcs.h>
  #ifdef SYSTEM_PC
    #include <iostream>
  #else
    #include <supplementary_driver/driver/rs232/impl/rs232io_c.h>
  #endif
  #include <IsoAgLib/util/impl/util_funcs.h>
#endif
*/

namespace __IsoAgLib {

/** initialisation for ISORequestPGN_c */
void
ISORequestPGN_c::init (void)
{
  // only init if closed (constructor "closes" it so it gets init'ed initially!
  if (checkAlreadyClosed ())
  {
    // clear state of b_alreadyClosed, so that close() is called one time AND no more init()s are performed!
    clearAlreadyClosed();

    if (getCanInstance4Comm().insertFilter( *this, MASK_TYPE(static_cast<MASK_TYPE>(0x1FF00) << 8), MASK_TYPE(static_cast<MASK_TYPE>(REQUEST_PGN_MSG_PGN) << 8), true, Ident_c::ExtendedIdent));
  }
}


/** every subsystem of IsoAgLib has explicit function for controlled shutdown */
void
ISORequestPGN_c::close (void)
{
  if (!checkAlreadyClosed ())
  {
    setAlreadyClosed();

    getCanInstance4Comm().deleteFilter( *this, MASK_TYPE(static_cast<MASK_TYPE>(0x1FF00) << 8), MASK_TYPE(static_cast<MASK_TYPE>(REQUEST_PGN_MSG_PGN) << 8), Ident_c::ExtendedIdent);
  }
};


/** default destructor which has nothing to do */
ISORequestPGN_c::~ISORequestPGN_c ()
{
  close();
}


/** adds the PGN to the list */
bool
ISORequestPGN_c::registerPGN (ISORequestPGNHandler_c &ref_PGNHandler, const uint32_t cui32_pgnToRegister)
{
  if (checkIfAlreadyRegistered (ref_PGNHandler, cui32_pgnToRegister))
    return false; // false could also mean that the PGN - client pair is already inserted in list

  PGN_s s_pgnToRegister = {cui32_pgnToRegister, &ref_PGNHandler};
  registeredClientsWithPGN.push_back (s_pgnToRegister);

  return true; // PGN - client pair didn't exist, so it was added
};


/** adds n PGN for the client to the list */
bool
ISORequestPGN_c::registerPGN (ISORequestPGNHandler_c &ref_PGNHandler, const uint8_t cui8_pgnCount, const uint32_t *pcui32_pgnToRegister)
{
  uint8_t ui8_index = 0;
  bool b_msgProcessed = false;
  for (;ui8_index < cui8_pgnCount; ui8_index++)
  {
    if (!checkIfAlreadyRegistered (ref_PGNHandler, *pcui32_pgnToRegister))
      b_msgProcessed |= registerPGN (ref_PGNHandler, *pcui32_pgnToRegister);
    pcui32_pgnToRegister++;
  }
  return b_msgProcessed;
};


/** remove PGN from the list */
void
ISORequestPGN_c::unregisterPGN (ISORequestPGNHandler_c &ref_PGNHandler, const uint32_t cui32_pgnToRegister)
{
  for (STL_NAMESPACE::vector<PGN_s>::iterator regPGN_it = registeredClientsWithPGN.begin();
       regPGN_it != registeredClientsWithPGN.end();
      )
  {
    if (cui32_pgnToRegister == 0)
    { // every registered PGN will be deleted
      if (regPGN_it->p_handler == &ref_PGNHandler)
        registeredClientsWithPGN.erase (regPGN_it); // after erase, the iterator points to the next (if available) vector item
      else
        regPGN_it++;
    }
    else
    { // only the cui32_pgnToRegister will be deleted
      if ((regPGN_it->p_handler == &ref_PGNHandler) && (regPGN_it->ui32_pgn == cui32_pgnToRegister))
      {
        registeredClientsWithPGN.erase (regPGN_it); // after erase, the iterator points to the next (if available) vector item
        break; // the PGN is unique for the RequestPGNHandler, so we can leave the loop
      }
      else
        regPGN_it++;
    }
  }
};


/** removes n PGN for the client from the list */
void
ISORequestPGN_c::unregisterPGN (ISORequestPGNHandler_c &ref_PGNHandler, const uint8_t cui8_pgnCount, const uint32_t *pcui32_pgnToUnregister)
{
  if (cui8_pgnCount == 0) return;

  uint8_t ui8_index = 0;
  for (; ui8_index < cui8_pgnCount; ui8_index++, pcui32_pgnToUnregister++)
    unregisterPGN (ref_PGNHandler, *pcui32_pgnToUnregister);
};


/** before adding any further PGN - RequestPGNHandler pair, check if not already existing */
bool
ISORequestPGN_c::checkIfAlreadyRegistered (ISORequestPGNHandler_c &ref_PGNHandler, const uint32_t cui32_pgn)
{
  for (STL_NAMESPACE::vector<PGN_s>::iterator regPGN_it=registeredClientsWithPGN.begin();
       regPGN_it != registeredClientsWithPGN.end();
       regPGN_it++
      )
  {
    if ((regPGN_it->p_handler == &ref_PGNHandler) && (regPGN_it->ui32_pgn == cui32_pgn))
      return true;
  }
  // no match was found before
  return false;
};


/** process system msg
  * @return true -> message processed by ISORequestPGN_c (also possible is NACK); false -> let others process */
bool
ISORequestPGN_c::processMsg ()
{
  /// it is not allowed to have source address 0xFF
  /// it needs to be != 0xFF
  if (data().isoSa() == 0xFF)
    return true;

  uint32_t ui32_reqPgn;
  // since we only insertFilter for REQUEST_PGN_MSG_PGN we don't need further checking
  //  if ((data().isoPgn() & 0x1FF00) == REQUEST_PGN_MSG_PGN)
  //  { // request for PGN
  ui32_reqPgn = (
                (static_cast<uint32_t>(data().operator[](0)))
              | (static_cast<uint32_t>(data().operator[](1)) << 8)
              | (static_cast<uint32_t>(data().operator[](2)) << 16)
              );

  /// in case of ISOItem_c has no address claimed yet it has sa 0xFE
  /// that ISOItem_c is not allowed to send any other REQUEST_PGN_MSG_PGN than ADDRESS_CLAIM_PGN
  /// Mike said: FE can request anything it wants ;-) so we'll do it!
  //     if ((data().isoSa() == 0xFE) && (ui32_reqPgn != ADDRESS_CLAIM_PGN))
  //       return true;
  //     else

  /// if the ISOItem_c is not in the monitor list, ignore this request
  if ((data().isoSa() != 0xFE) && (!getIsoMonitorInstance4Comm().existIsoMemberNr (data().isoSa())))
    return true;

  bool b_distributeToClients = false;
  // if isoPs is 255 let all local item answer
  if (data().isoPs() == 0xFF)
  {
    b_distributeToClients = true;
  }
  else
  {
    if (getIsoMonitorInstance4Comm().existIsoMemberNr (data().isoPs()))
    { // check if local
      if (getIsoMonitorInstance4Comm().isoMemberNr (data().isoPs()).itemState (IState_c::Local))
        b_distributeToClients = true;
    }
  }

  if (b_distributeToClients)
  {
    bool b_processedByAnyClient = false;
    for (STL_NAMESPACE::vector<PGN_s>::iterator regPGN_it = registeredClientsWithPGN.begin();
          regPGN_it != registeredClientsWithPGN.end(); regPGN_it++)
    { // let all local regPGN_it process this request
      if (regPGN_it->ui32_pgn == ui32_reqPgn)
        b_processedByAnyClient |= regPGN_it->p_handler->processMsgRequestPGN(ui32_reqPgn, data().isoSa(), data().isoPs());
    }

    if ((data().isoPs() != 0xFF) && !b_processedByAnyClient) // no client could answer the Request PGN, so NACK it!
    {
      uint32_t ui32_purePgn = ui32_reqPgn;
      if (((ui32_purePgn >> 8) & 0xFF) < 0xF0)
      { // destination specific, so clear the destSA field as we want the PURE PGN!
        ui32_purePgn &= 0x1FF00;
      }

      data().setIsoPri(6);
      data().setIsoDp(0);
      data().setIsoPf(ACKNOWLEDGEMENT_PGN >> 8);
      data().setIsoPs(data().isoSa());
      data().setIsoSa(data().isoPs());
      // set the first four bytes as uint32_t value, where lowest byte equals to ControlByte
      data().setUint32Data ((1-1), 0xFFFFFF01UL); // Control Byte = 1, Negative Acknowledgement: NACK
      // set at lowest byte of second uint32_t value the reserved 0xFF
      // and place at the higher bytes of this second uint32_t
      // the ui32_purePgn
      data().setUint32Data ((5-1), ((ui32_purePgn << 8)|0xFFUL) ); // Control Byte = 1, Negative Acknowledgement: NACK
      data().setLen (8);

      __IsoAgLib::getCanInstance4Comm() << c_data;
    }
  }
  return true;
//  }
// since we only insertFilter for REQUEST_PGN_MSG_PGN we don't need further checking
//  return b_processed; // return if msg was processed by ISORequestPGN_c
}


/** constructor for ISORequestPGN_c */
ISORequestPGN_c::ISORequestPGN_c ()
  : SingletonISORequestPGN_c ()
  , registeredClientsWithPGN ()
{
// functionality moved OUT of the constructor, as the constructor is NOT called in embedded systems for static class instances.
}


/** initialize directly after the singleton instance is created.
  * this is called from singleton.h and should NOT be called from the user again.
  * users please use init(...) instead. */
void
ISORequestPGN_c::singletonInit ()
{
  setAlreadyClosed (); // so init() will init ;-) (but only once!)
  init();
};


#if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
/** C-style function, to get access to the unique ISORequestPGN_c singleton instance
 * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS */
ISORequestPGN_c& getIsoRequestPgnInstance (uint8_t rui8_instance)
{ // if > 1 singleton instance is used, no static reference can be used
  return ISORequestPGN_c::instance(rui8_instance);
};
#else
/** C-style function, to get access to the unique ISORequestPGN_c singleton instance */
ISORequestPGN_c& getIsoRequestPgnInstance (void)
{
  static ISORequestPGN_c& c_isoRequestPgn = ISORequestPGN_c::instance ();
  return c_isoRequestPgn;
};
#endif

} // end of namespace __IsoAgLib
