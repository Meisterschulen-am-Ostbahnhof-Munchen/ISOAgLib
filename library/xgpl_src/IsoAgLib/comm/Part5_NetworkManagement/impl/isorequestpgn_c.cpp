/***************************************************************************
                          isomonitor_c.cpp - object for monitoring members
                                              (list of IsoItem_c)
                             -------------------
    begin                : Fri Apr 07 2000
    copyright            : (C) 2000 - 2009 by Dipl.-Inform. Achim Spangler
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
 * Copyright (C) 2000 - 2009 Dipl.-Inform. Achim Spangler                  *
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
#include "isomonitor_c.h"

#include <IsoAgLib/driver/can/impl/canio_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isofiltermanager_c.h>


namespace __IsoAgLib {

/** initialisation for IsoRequestPgn_c */
void
IsoRequestPgn_c::init (void)
{
  // only init if closed (constructor "closes" it so it gets init'ed initially!
  if (checkAlreadyClosed ())
  {
    // clear state of mb_alreadyClosed, so that close() is called one time AND no more init()s are performed!
    clearAlreadyClosed();

    getCanInstance4Comm().insertStandardIsoFilter(*this,(REQUEST_PGN_MSG_PGN | 0xFF),true);
  }
}


/** every subsystem of IsoAgLib has explicit function for controlled shutdown */
void
IsoRequestPgn_c::close (void)
{
  if (!checkAlreadyClosed ())
  {
    setAlreadyClosed();
    getCanInstance4Comm().deleteFilter( *this, 0x3FFFF00UL, MASK_TYPE(static_cast<MASK_TYPE>(REQUEST_PGN_MSG_PGN | 0xFF) << 8), Ident_c::ExtendedIdent);
  }
};


/** default destructor which has nothing to do */
IsoRequestPgn_c::~IsoRequestPgn_c ()
{
  close();
}


/** adds the PGN to the list */
bool
IsoRequestPgn_c::registerPGN (IsoRequestPgnHandler_c &r_PGNHandler, const uint32_t cui32_pgnToRegister)
{
  if (checkIfAlreadyRegistered (r_PGNHandler, cui32_pgnToRegister))
    return false; // false could also mean that the PGN - client pair is already inserted in list

  PGN_s s_pgnToRegister = {cui32_pgnToRegister, &r_PGNHandler};
  m_registeredClientsWithPGN.push_back (s_pgnToRegister);

  return true; // PGN - client pair didn't exist, so it was added
};


/** adds n PGN for the client to the list */
bool
IsoRequestPgn_c::registerPGN (IsoRequestPgnHandler_c &r_PGNHandler, const uint8_t cui8_pgnCount, const uint32_t *pcui32_pgnToRegister)
{
  uint8_t ui8_index = 0;
  bool b_msgProcessed = false;
  for (;ui8_index < cui8_pgnCount; ui8_index++)
  {
    if (!checkIfAlreadyRegistered (r_PGNHandler, *pcui32_pgnToRegister))
      b_msgProcessed |= registerPGN (r_PGNHandler, *pcui32_pgnToRegister);
    pcui32_pgnToRegister++;
  }
  return b_msgProcessed;
};


/** remove PGN from the list */
void
IsoRequestPgn_c::unregisterPGN (IsoRequestPgnHandler_c &r_PGNHandler, const uint32_t cui32_pgnToRegister)
{
  for (STL_NAMESPACE::vector<PGN_s>::iterator regPGN_it = m_registeredClientsWithPGN.begin();
       regPGN_it != m_registeredClientsWithPGN.end();
      )
  {
    if (cui32_pgnToRegister == 0)
    { // every registered PGN will be deleted
      if (regPGN_it->p_handler == &r_PGNHandler)
        m_registeredClientsWithPGN.erase (regPGN_it); // after erase, the iterator points to the next (if available) vector item
      else
        regPGN_it++;
    }
    else
    { // only the cui32_pgnToRegister will be deleted
      if ((regPGN_it->p_handler == &r_PGNHandler) && (regPGN_it->ui32_pgn == cui32_pgnToRegister))
      {
        m_registeredClientsWithPGN.erase (regPGN_it); // after erase, the iterator points to the next (if available) vector item
        break; // the PGN is unique for the RequestPGNHandler, so we can leave the loop
      }
      else
        regPGN_it++;
    }
  }
};


/** removes n PGN for the client from the list */
void
IsoRequestPgn_c::unregisterPGN (IsoRequestPgnHandler_c &r_PGNHandler, const uint8_t cui8_pgnCount, const uint32_t *pcui32_pgnToUnregister)
{
  if (cui8_pgnCount == 0) return;

  uint8_t ui8_index = 0;
  for (; ui8_index < cui8_pgnCount; ui8_index++, pcui32_pgnToUnregister++)
    unregisterPGN (r_PGNHandler, *pcui32_pgnToUnregister);
};


/** before adding any further PGN - RequestPGNHandler pair, check if not already existing */
bool
IsoRequestPgn_c::checkIfAlreadyRegistered (IsoRequestPgnHandler_c &r_PGNHandler, const uint32_t cui32_pgn)
{
  for (STL_NAMESPACE::vector<PGN_s>::iterator regPGN_it=m_registeredClientsWithPGN.begin();
       regPGN_it != m_registeredClientsWithPGN.end();
       regPGN_it++
      )
  {
    if ((regPGN_it->p_handler == &r_PGNHandler) && (regPGN_it->ui32_pgn == cui32_pgn))
      return true;
  }
  // no match was found before
  return false;
};


/** process REQUEST_PGN_MSG_PGN message
    Since we only insertFilter for REQUEST_PGN_MSG_PGN we don't need further checking
    a la "if ((data().isoPgn() & 0x3FF00) == REQUEST_PGN_MSG_PGN)"
  * @return true -> message processed by IsoRequestPgn_c (also possible is NACK); false -> let others process */
bool
IsoRequestPgn_c::processMsg ()
{
  /// Store incoming information for possible later user-triggered "sendAcknowledgePGN()"
  mpc_isoItemSA = data().getMonitorItemForSA();
  mpc_isoItemDA = data().getMonitorItemForDA();
  mui32_requestedPGN = ( (static_cast<uint32_t>(data().operator[](0)))
                      | (static_cast<uint32_t>(data().operator[](1)) << 8)
                      | (static_cast<uint32_t>(data().operator[](2)) << 16) );

  /// In case a node on the bus has not yet claimed an address (it sends with sa=0xFE, i.e. MonitorItem_c==NULL),
  /// it can still request ANY PGNs according to Mike - so no special check done here!
  /// 1. Distribute to all clients
  bool b_processedByAnyClient = false;
  for (STL_NAMESPACE::vector<PGN_s>::iterator regPGN_it = m_registeredClientsWithPGN.begin();
        regPGN_it != m_registeredClientsWithPGN.end(); regPGN_it++)
  { // let all local regPGN_it process this request
    if (regPGN_it->ui32_pgn == mui32_requestedPGN)
      b_processedByAnyClient |= regPGN_it->p_handler->processMsgRequestPGN (mui32_requestedPGN, mpc_isoItemSA, mpc_isoItemDA);
  }

  /// 2. Check if we have to send a NACK as nobody could answer it
  /// (only for destination-specific, will be checked by answerRequestPGNwithNACK() !)
  if (!b_processedByAnyClient)
  { // no client could answer the Request PGN, so NACK it!
    answerRequestPGNwithNACK ();
  }

  return true;
}



void
IsoRequestPgn_c::sendAcknowledgePGN (IsoItem_c& arc_isoItemSender, uint8_t aui8_ackType)
{
  uint32_t ui32_purePgn = mui32_requestedPGN;
  if (((ui32_purePgn >> 8) & 0xFF) < 0xF0)
  { // destination specific, so clear the destSA field as we want the PURE PGN!
    ui32_purePgn &= 0x3FF00;
  }

  data().setIsoPri(6);
  data().setIsoDp(0);
  data().setIsoPf(ACKNOWLEDGEMENT_PGN >> 8);
  data().setMonitorItemForDA (mpc_isoItemSA);
  data().setMonitorItemForSA (&arc_isoItemSender);
  // set the first four bytes as uint32_t value, where lowest byte equals to ControlByte
  data().setUint32Data ((1-1), (0xFFFFFF00UL | uint32_t (aui8_ackType)));
  // set at lowest byte of second uint32_t value the reserved 0xFF
  // and place at the higher bytes of this second uint32_t
  // the ui32_purePgn
  data().setUint32Data ((5-1), ((ui32_purePgn << 8)|0xFFUL) );
  data().setLen (8);

  __IsoAgLib::getCanInstance4Comm() << mc_data;
}


/** register an IsoName_c of a local device, so that RequestPGN messages that are directed to this
    IsoName_c are received and handled.
    This function has to be called during initialisation of a local IsoItem_c / IdentItem_c
  */
void IsoRequestPgn_c::registerLocalDevice( const __IsoAgLib::IsoName_c& rc_isoName )
{
  if ( getIsoMonitorInstance4Comm().existLocalIsoMemberISOName(rc_isoName) )
  { // local IsoItem_c has finished adr claim
    getIsoFilterManagerInstance4Comm().insertIsoFilter (IsoFilter_s (*this, (0x3FFFF00UL), (REQUEST_PGN_MSG_PGN << 8), &rc_isoName, NULL, 3));
  }
}
/** unregister an IsoName_c of a local device, so that IsoFilterManager_c stops receiving
    messages for the corresponding IsoName_c.
    This function has to be called during destruction of a local IsoItem_c / IdentItem_c
  */
void IsoRequestPgn_c::unregisterLocalDevice( const __IsoAgLib::IsoName_c& rc_isoName )
{
  if ( getIsoMonitorInstance4Comm().existLocalIsoMemberISOName(rc_isoName) )
  { // local IsoItem_c has finished adr claim
    getIsoFilterManagerInstance4Comm().removeIsoFilter (IsoFilter_s (*this, (0x3FFFF00UL), (REQUEST_PGN_MSG_PGN << 8), &rc_isoName, NULL, 3));
  }
}


/** constructor for IsoRequestPgn_c */
IsoRequestPgn_c::IsoRequestPgn_c ()
  : SingletonIsoRequestPgn_c ()
  , m_registeredClientsWithPGN ()
{
// functionality moved OUT of the constructor, as the constructor is NOT called in embedded systems for static class instances.
}


/** initialize directly after the singleton instance is created.
  * this is called from singleton.h and should NOT be called from the user again.
  * users please use init(...) instead. */
void
IsoRequestPgn_c::singletonInit ()
{
  setAlreadyClosed (); // so init() will init ;-) (but only once!)
  init();
};

// Funktion for Debugging in Scheduler_c
const char*
IsoRequestPgn_c::getTaskName() const
{
  return "IsoRequestPgn_c";
}


#if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
/** C-style function, to get access to the unique IsoRequestPgn_c singleton instance
 * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS */
IsoRequestPgn_c& getIsoRequestPgnInstance (uint8_t aui8_instance)
{ // if > 1 singleton instance is used, no static reference can be used
  return IsoRequestPgn_c::instance(aui8_instance);
};
#else
/** C-style function, to get access to the unique IsoRequestPgn_c singleton instance */
IsoRequestPgn_c& getIsoRequestPgnInstance (void)
{
  static IsoRequestPgn_c& c_isoRequestPgn = IsoRequestPgn_c::instance ();
  return c_isoRequestPgn;
};
#endif

} // end of namespace __IsoAgLib
