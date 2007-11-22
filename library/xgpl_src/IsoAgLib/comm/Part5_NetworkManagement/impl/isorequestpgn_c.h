/***************************************************************************
                              isorequestpgn_c.h
                             -------------------
    begin                : Thu May 16 2006
    copyright            : (C) 2000 - 2004 by Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
    type                 : Header
    $LastChangedDate:  $
    $LastChangedRevision:  $
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
#ifndef ISO_REQUESTPGN_C_H
#define ISO_REQUESTPGN_C_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include <IsoAgLib/typedef.h>
#include <IsoAgLib/util/config.h>

#include <IsoAgLib/util/impl/singleton.h>
#include <IsoAgLib/driver/can/impl/cancustomer_c.h>
#include <IsoAgLib/comm/Part3_DataLink/impl/canpkgext_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement//impl/isomonitor_c.h>
//#include "isoitem_c.h"

#include <cstdlib>	// Include before vector or else CNAMESPACE stuff is screwed up for Tasking
#include <vector>

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

// forward declaration
class IsoRequestPgnHandler_c;

class IsoRequestPgn_c;
typedef SINGLETON_DERIVED(IsoRequestPgn_c, CanCustomer_c) SingletonIsoRequestPgn_c;
/** this object manages a list of all ISO members to react on Request for PGN
    @short Manager for handling of Requests for PGN
    @see
    @author Dipl.-Inf. (FH) Martina Winkler */
class IsoRequestPgn_c : public SingletonIsoRequestPgn_c
{
private:
  struct PGN_s
  {
    uint32_t ui32_pgn;
    IsoRequestPgnHandler_c *p_handler;
  };

public:
  /** register an IsoName_c of a local device, so that RequestPGN messages that are directed to this
      IsoName_c are received and handled.
      This function has to be called during initialisation of a local IsoItem_c / IdentItem_c
    */
  void registerLocalDevice( const __IsoAgLib::IsoName_c& rc_isoName );
  /** unregister an IsoName_c of a local device, so that IsoFilterManager_c stops receiving
      messages for the corresponding IsoName_c.
      This function has to be called during destruction of a local IsoItem_c / IdentItem_c
    */
  void unregisterLocalDevice( const __IsoAgLib::IsoName_c& rc_isoName );

  /** initialisation for IsoRequestPgn_c */
  void init (void);
  /** every subsystem of IsoAgLib has explicit function for controlled shutdown */
  void close (void);

  /** default destructor which has nothing to do */
  virtual ~IsoRequestPgn_c ();

  /** deliver reference to data pkg
      @return reference to the CAN communication member object mc_data (CanPkgExt_c)
      @see CANPkgExt */
  CanPkgExt_c& data() {return mc_data;};

  //  Operation: dataBase
  //!  deliver reference to data pkg as reference to CanPkgExt_c
  //!  to implement the base virtual function correct
  virtual CanPkgExt_c& dataBase() {return mc_data;}

  /** adds the PGN to the list
      @param r_PGNHandler: reference to a IsoRequestPgnHandler_c from IsoMonitor_c, IsoItem_c, TimePosGps_c, TracGeneral_c or IsoTerminalServer
      @param cui32_pgnToRegister: PGN for which the r_PGNHandler's processMsgRequestPGN should be called
      @return true, if PGN for that r_PGNHandler wasn't registered yet and is now added and registered successfully
              false, if PGN for that r_PGNHandler was already registered and needn't be added again */
  bool registerPGN (IsoRequestPgnHandler_c &r_PGNHandler, const uint32_t cui32_pgnToRegister);

  /** adds n PGN for the client to the list
      @param r_PGNHandler: reference to a IsoRequestPgnHandler_c from IsoMonitor_c, IsoItem_c, TimePosGps_c, TracGeneral_c or IsoTerminalServer
      @param cui8_pgnCount: number of PGNs to be added in the list
      @param pcui32_pgnToRegister: array of PGNs for which the r_PGNHandler's processMsgRequestPGN should be called
      @return true, if at least one of the PGNs for that r_PGNHandler wasn't registered yet and is now added and registered successfully
              false, if all PGNs for that r_PGNHandler were already registered and needn't be added again*/
  bool registerPGN (IsoRequestPgnHandler_c &r_PGNHandler, const uint8_t cui8_pgnCount, const uint32_t *pcui32_pgnToRegister);

  /** removes a PGN from the list
      @param r_PGNHandler: reference to a IsoRequestPgnHandler_c from IsoMonitor_c, IsoItem_c, TimePosGps_c, TracGeneral_c or IsoTerminalServer
      @param cui32_pgnToRegister if not given, all PGNs of the r_PGNHandler will be removed
                                 if given, only that single PGN will be removed */
  void unregisterPGN (IsoRequestPgnHandler_c &r_PGNHandler, const uint32_t cui32_pgnToRegister = 0);

  /** removes n PGN for the client from the list
      @param r_PGNHandler: reference to a IsoRequestPgnHandler_c from IsoMonitor_c, IsoItem_c, TimePosGps_c, TracGeneral_c or IsoTerminalServer
      @param cui8_pgnCount: number of PGNs to be removed from the list
      @param pcui32_pgnToRegister: array of PGNs to be removed */
  void unregisterPGN (IsoRequestPgnHandler_c &r_PGNHandler, const uint8_t cui8_pgnCount, const uint32_t *pcui32_pgnToUnregister);

  /** before adding any further PGN - RequestPGNHandler, check if not already existing
      @param r_PGNHandler: reference to a IsoRequestPgnHandler_c from IsoMonitor_c, IsoItem_c, TimePosGps_c, TracGeneral_c or IsoTerminalServer
      @param cui32_pgn: check that PGN, please!
      @return true, if the r_PGNHandler is already registered with that PGN */
  bool checkIfAlreadyRegistered (IsoRequestPgnHandler_c &r_PGNHandler, const uint32_t cui32_pgn);

  /// only interesting for processMsgRequestPGN implementations to get the exact can-pkg time!
  int32_t getTimeOfLastRequest() { return data().time(); }

  /** Only call sendCannotRespondNow(..) when you're about to respond to a requested PGN */
  void answerRequestPGNwithNACK()                       { if (mpc_isoItemDA != NULL) sendAcknowledgePGN (*mpc_isoItemDA, 0x01); } // Control Byte 1 = NOT Acknowledge
  void answerRequestPGNwithCannotRespondNow(IsoItem_c& arc_isoItemSender) { sendAcknowledgePGN (arc_isoItemSender, 0x03); } // Control Byte 3 = Cannot Respond

  ///  Operation:  Funktion for Debugging in Scheduler_c
  virtual const char* getTaskName() const;


protected: // Protected methods
  /** process system msg
    * @return true -> message (Request for PGN) processed by IsoRequestPgn_c; false -> let others process */
  bool processMsg ();

private: // Private methods
  /** constructor for IsoRequestPgn_c */
  IsoRequestPgn_c (void);

  /** initialize directly after the singleton instance is created.
      this is called from singleton.h and should NOT be called from the user again.
      users please use init(...) instead. */
  void singletonInit ();

  void sendAcknowledgePGN (IsoItem_c& arc_isoItemSender, uint8_t aui8_ackType);


  /** clear mb_alreadyClosed so that close() can be called one time */
  void clearAlreadyClosed( void ) { mb_alreadyClosed = false; }

  /** set mb_alreadyClosed so that close() can't be called another time */
  void setAlreadyClosed( void ) { mb_alreadyClosed = true; }

  /** check mb_alreadyClosed to decide if close() can be called */
  bool checkAlreadyClosed( void ) const { return mb_alreadyClosed; }


private: // Private attributes
  /// holds all registered clients with PGN(s)
  STL_NAMESPACE::vector<PGN_s> m_registeredClientsWithPGN;

  friend class SINGLETON_DERIVED (IsoRequestPgn_c,CanCustomer_c);

  /** temp data where received and to be sent data is put */
  CanPkgExt_c mc_data;

  /// The following variables are just kept here as cache in case the user
  /// calls "sendAcknowledgePGN" out and the CAN-Pkg was changed in between
  /// by another ReqPGNHandler...
  IsoItem_c* mpc_isoItemSA;
  IsoItem_c* mpc_isoItemDA;
  uint32_t mui32_requestedPGN;

  bool mb_alreadyClosed;
};

#if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
  /** C-style function, to get access to the unique IsoRequestPgn_c singleton instance
      if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS */
  IsoRequestPgn_c& getIsoRequestPgnInstance (uint8_t aui8_instance = 0);
#else
  /** C-style function, to get access to the unique IsoRequestPgn_c singleton instance */
  IsoRequestPgn_c& getIsoRequestPgnInstance (void);
#endif

}
#endif
