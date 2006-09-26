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
#include <IsoAgLib/util/impl/elementbase_c.h>
#include <IsoAgLib/util/impl/canpkgext_c.h>
//#include "isoitem_c.h"

#include <vector>

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

// forward declaration
class ISORequestPGNHandler_c;

class ISORequestPGN_c;
typedef SINGLETON_DERIVED(ISORequestPGN_c, ElementBase_c) SingletonISORequestPGN_c;
/** this object manages a list of all ISO members to react on Request for PGN
    @short Manager for handling of Requests for PGN
    @see
    @author Dipl.-Inf. (FH) Martina Winkler */
class ISORequestPGN_c : public SingletonISORequestPGN_c
{
private:
  struct PGN_s
  {
    uint32_t ui32_pgn;
    ISORequestPGNHandler_c *p_handler;
  };

public:
  /** initialisation for ISORequestPGN_c */
  void init (void);
  /** every subsystem of IsoAgLib has explicit function for controlled shutdown */
  void close (void);

  /** default destructor which has nothing to do */
  virtual ~ISORequestPGN_c ();

  bool timeEvent (void) { return true; };

  /** deliver reference to data pkg
      @return reference to the CAN communication member object c_data (CANPkgExt_c)
      @see CANPkgExt */
  CANPkgExt_c& data() {return c_data;};

  //  Operation: dataBase
  //!  deliver reference to data pkg as reference to CANPkgExt_c
  //!  to implement the base virtual function correct
  virtual CANPkgExt_c& dataBase() {return c_data;}

  /** adds the PGN to the list
      @param ref_PGNHandler: reference to a ISORequestPGNHandler_c from ISOMonitor_c, ISOItem_c, TimePosGps_c, TracGeneral_c or IsoTerminalServer
      @param cui32_pgnToRegister: PGN for which the ref_PGNHandler's processMsgRequestPGN should be called
      @return true, if PGN for that ref_PGNHandler wasn't registered yet and is now added and registered successfully
              false, if PGN for that ref_PGNHandler was already registered and needn't be added again */
  bool registerPGN (ISORequestPGNHandler_c &ref_PGNHandler, const uint32_t cui32_pgnToRegister);

  /** adds n PGN for the client to the list
      @param ref_PGNHandler: reference to a ISORequestPGNHandler_c from ISOMonitor_c, ISOItem_c, TimePosGps_c, TracGeneral_c or IsoTerminalServer
      @param cui8_pgnCount: number of PGNs to be added in the list
      @param pcui32_pgnToRegister: array of PGNs for which the ref_PGNHandler's processMsgRequestPGN should be called
      @return true, if at least one of the PGNs for that ref_PGNHandler wasn't registered yet and is now added and registered successfully
              false, if all PGNs for that ref_PGNHandler were already registered and needn't be added again*/
  bool registerPGN (ISORequestPGNHandler_c &ref_PGNHandler, const uint8_t cui8_pgnCount, const uint32_t *pcui32_pgnToRegister);

  /** removes a PGN from the list
      @param ref_PGNHandler: reference to a ISORequestPGNHandler_c from ISOMonitor_c, ISOItem_c, TimePosGps_c, TracGeneral_c or IsoTerminalServer
      @param cui32_pgnToRegister if not given, all PGNs of the ref_PGNHandler will be removed
                                 if given, only that single PGN will be removed */
  void unregisterPGN (ISORequestPGNHandler_c &ref_PGNHandler, const uint32_t cui32_pgnToRegister = 0);

  /** removes n PGN for the client from the list
      @param ref_PGNHandler: reference to a ISORequestPGNHandler_c from ISOMonitor_c, ISOItem_c, TimePosGps_c, TracGeneral_c or IsoTerminalServer
      @param cui8_pgnCount: number of PGNs to be removed from the list
      @param pcui32_pgnToRegister: array of PGNs to be removed */
  void unregisterPGN (ISORequestPGNHandler_c &ref_PGNHandler, const uint8_t cui8_pgnCount, const uint32_t *pcui32_pgnToUnregister);

  /** before adding any further PGN - RequestPGNHandler, check if not already existing
      @param ref_PGNHandler: reference to a ISORequestPGNHandler_c from ISOMonitor_c, ISOItem_c, TimePosGps_c, TracGeneral_c or IsoTerminalServer
      @param cui32_pgn: check that PGN, please!
      @return true, if the ref_PGNHandler is already registered with that PGN */
  bool checkIfAlreadyRegistered (ISORequestPGNHandler_c &ref_PGNHandler, const uint32_t cui32_pgn);

  /// only interesting for processMsgRequestPGN implementations to get the exact can-pkg time!
  int32_t getTimeOfLastRequest() { return data().time(); }

  /** Only call sendCannotRespondNow(..) when you're about to respond to a requested PGN */
  void answerRequestPGNwithNACK()             { sendAcknowledgePGN (0x01); } // Control Byte 3 = NOT Acknowledge
  void answerRequestPGNwithCannotRespondNow() { sendAcknowledgePGN (0x03); } // Control Byte 3 = Cannot Respond

protected: // Protected methods
  /** process system msg
    * @return true -> message (Request for PGN) processed by ISORequestPGN_c; false -> let others process */
  bool processMsg ();

private: // Private methods
  /** constructor for ISORequestPGN_c */
  ISORequestPGN_c (void);

  /** initialize directly after the singleton instance is created.
      this is called from singleton.h and should NOT be called from the user again.
      users please use init(...) instead. */
  void singletonInit ();

  void sendAcknowledgePGN (uint8_t rui8_ackType);


private: // Private attributes
  /// holds all registered clients with PGN(s)
  STL_NAMESPACE::vector<PGN_s> registeredClientsWithPGN;

  friend class SINGLETON_DERIVED (ISORequestPGN_c,ElementBase_c);

  /** temp data where received and to be sent data is put */
  CANPkgExt_c c_data;

  uint8_t  ui8_saFromRequest;
  uint8_t  ui8_psFromRequest;
  uint32_t ui32_reqPgnFromRequest;
};

#if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
  /** C-style function, to get access to the unique ISORequestPGN_c singleton instance
      if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS */
  ISORequestPGN_c& getIsoRequestPgnInstance (uint8_t rui8_instance = 0);
#else
  /** C-style function, to get access to the unique ISORequestPGN_c singleton instance */
  ISORequestPGN_c& getIsoRequestPgnInstance (void);
#endif

}
#endif
