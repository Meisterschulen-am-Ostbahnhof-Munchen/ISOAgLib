/***************************************************************************
                          isoitem_c.cpp - object which represents an item
                                            in a service monitor list
                             -------------------
    begin                : Tue Jan 01 2001
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
#include "isoitem_c.h"
#include "isosystempkg_c.h"
#include "isomonitor_c.h"
#include <IsoAgLib/comm/Scheduler/impl/scheduler_c.h>
#include <IsoAgLib/driver/can/impl/canio_c.h>

#if defined(DEBUG)
  #ifdef SYSTEM_PC
    #include <iostream>
  #else
    #include <supplementary_driver/driver/rs232/impl/rs232io_c.h>
  #endif
  #include <IsoAgLib/util/impl/util_funcs.h>
#endif

namespace __IsoAgLib {

/** default constructor - all data has to be initialized with a call to "set(..)"
*/
IsoItem_c::IsoItem_c()
  : BaseItem_c(0, IState_c::IstateNull, 0)
  #ifdef USE_WORKING_SET
  , pc_masterItem (NULL)
  , i8_slavesToClaimAddress (0) // idle around
  , i32_timeLastCompletedAnnounceStarted (-1)
  , i32_timeCurrentAnnounceStarted (-1)
  , b_repeatAnnounce (false)
  #endif
  , ui8_nr(0xFE)
  , b_repeatClaim (false) // wouldn't be needed to be set here as it's set when entering AddressClaim
  , c_isoName (IsoName_c::IsoNameUnspecified())
{
}

/** copy constructor for ISOItem
  The copy constructor checks if the source item is
  a master ( i.e. the pc_masterItem pointer points to this )
  -> it doesn't simply copy the pointer, but sets its
  own pointer also to the this-pointer of the new instance
  @param arc_src source IsoItem_c instance
*/
IsoItem_c::IsoItem_c(const IsoItem_c& arc_src)
  : BaseItem_c (arc_src)
#ifdef USE_WORKING_SET
  //, pc_masterItem (NULL) // handled in code below!
  , i8_slavesToClaimAddress (arc_src.i8_slavesToClaimAddress)
  , i32_timeLastCompletedAnnounceStarted (arc_src.i32_timeLastCompletedAnnounceStarted)
  , i32_timeCurrentAnnounceStarted (arc_src.i32_timeCurrentAnnounceStarted)
  , b_repeatAnnounce (arc_src.b_repeatAnnounce)
#endif
  , ui8_nr (arc_src.ui8_nr)
  , b_repeatClaim (arc_src.b_repeatClaim)
  , c_isoName (arc_src.c_isoName)

{
  // mark this item as prepare address claim if local
  setItemState (IState_c::Member);

  #ifdef USE_WORKING_SET
  // no need of setting "i8_slavesToClaimAddress", as it will be set when setting state to CleaimedAddress!
  /// @todo Check if this constructor is really needed and what it should do!
  /// @todo Check if we need to copy the new 3 member variables, too. Also for the below constructors!!!
  // check if the master item pointer of the source item
  // is pointing to itself, as then this new created instance shall
  // not copy the pointer, but set the pointer to itself, as this
  // indicates the Master State
  if ( arc_src.isMaster() )
  { // set our pc_masterItem also to this, to indicate master state
    pc_masterItem = this;
  }
  else
  { // just copy the master pointer from source
    pc_masterItem = arc_src.getMaster ();
  }
  #endif

  updateTime (/*get current time due to default parameter*/);
  if (itemState(IState_c::Local))
  {
    setItemState(IState_c::PreAddressClaim);
    timeEvent(); // call time for further init
  }
}

/** assign constructor for ISOItem
  @param arc_src source IsoItem_c object
*/
IsoItem_c& IsoItem_c::operator=(const IsoItem_c& arc_src)
{
//   MonitorItem_c::operator=(arc_src);
  BaseItem_c::operator=(arc_src);
  setISOName(arc_src.isoName());
  setNr(arc_src.nr());

  setItemState (IState_c::Member);
  #ifdef USE_WORKING_SET
  // no need of setting "i8_slavesToClaimAddress" here as it will be set when setting state to ClaimedAddress
  /** @todo What to do with the pc_masterItem? */
  i8_slavesToClaimAddress = arc_src.i8_slavesToClaimAddress;
  i32_timeLastCompletedAnnounceStarted = arc_src.i32_timeLastCompletedAnnounceStarted;
  i32_timeCurrentAnnounceStarted = arc_src.i32_timeCurrentAnnounceStarted;
  b_repeatAnnounce = arc_src.b_repeatAnnounce;
  #endif
  updateTime (/*get current time due to default parameter*/);
  // mark this item as prepare address claim if local
  if (itemState(IState_c::Local))
  {
    setItemState(IState_c::PreAddressClaim);
    timeEvent(); // call time for further init
  }
  return *this;
}

/**
  lower comparison between left ISOName uint8_t and right MonitorItem
  @param ab_left ISOName uint8_t left parameter
  @param arc_right rigth ServiceItem_c parameter
*/
bool operator<(const IsoName_c& ac_left, const IsoItem_c& arc_right)
{
  return (ac_left < arc_right.isoName())?true:false;
}

/** default destructor */
IsoItem_c::~IsoItem_c()
{
  if ( itemState(IState_c::ClaimedAddress ) )
  { // broadcast to handler classes the event of LOSS of this SA
    getIsoMonitorInstance4Comm().broadcastSaRemove2Clients( isoName(), nr() );
  }
  #ifdef USE_WORKING_SET
  // check if we were a working-set master
  if (isMaster()) // => pointing to ourself => workingset-master!
  { // tell all workingset-slaves now that their master is gone so they are now STANDALONE again
    getIsoMonitorInstance4Comm().notifyOnWsMasterLoss(*this);
  }
  #endif
}

/** deliver name
  @return pointer to the name uint8_t string (8byte)
*/
const uint8_t* IsoItem_c::name() const
{
  return isoName().outputString();
}

/** check if the name field is empty (no name received)
  @return true -> no name received
*/
bool IsoItem_c::isEmptyName() const
{
  return false;
}

/** deliver name as pure ASCII string
  @param pc_name string where ASCII string is inserted
  @param aui8_maxLen max length for name
*/
void IsoItem_c::getPureAsciiName(int8_t *pc_asciiName, uint8_t aui8_maxLen)
{
  char c_temp[30];
  const uint8_t* pb_src = name();
  CNAMESPACE::sprintf(c_temp, "0x%02x%02x%02x%02x%02x%02x%02x%02x", pb_src[0],pb_src[1],pb_src[2],pb_src[3],
      pb_src[4],pb_src[5],pb_src[6],pb_src[7]);

  uint8_t ui8_len = CNAMESPACE::strlen(c_temp);
  if (aui8_maxLen < ui8_len) ui8_len = aui8_maxLen;
  CNAMESPACE::memcpy(pc_asciiName, c_temp, ui8_len );
  pc_asciiName[ui8_len-1] = '\0';
}

/** set all element data with one call
  @param ai32_time creation time of this item instance
  @param ac_isoName ISOName code of this item ((deviceClass << 3) | devClInst )
  @param aui8_nr number of this item
  @param ab_status state of this ident (off, claimed address, ...)
  @param ai_singletonVecKey optional key for selection of IsoAgLib instance (default 0)
*/
void IsoItem_c::set(int32_t ai32_time, const IsoName_c& ac_isoName, uint8_t aui8_nr,
        itemState_t ren_status, int ai_singletonVecKey )
{
  BaseItem_c::set( ai32_time, ren_status, ai_singletonVecKey );
  setISOName(ac_isoName);
  setNr(aui8_nr);
}


/** periodically time evented actions:
    * find free SA or check if last SA is available
    * send adress claim
  possible errors:
    * dependant error in CanIo_c during send
  @return true -> all planned time event activitie performed
*/
bool IsoItem_c::timeEvent( void )
{
  CanIo_c& c_can = getCanInstance4Comm();
  IsoMonitor_c& c_isoMonitor = getIsoMonitorInstance4Comm();
  int32_t i32_time = ElementBase_c::getLastRetriggerTime();

  IsoSystemPkg_c& c_pkg = c_isoMonitor.data();
  if (itemState(IState_c::PreAddressClaim))
  { // this item is in prepare address claim state -> wait for sending first adress claim
    int32_t i32_lastAdrRequestTime = c_isoMonitor.lastIsoSaRequest();
    if (i32_lastAdrRequestTime != -1)
    {
      int32_t i32_wait = 1250 + calc_randomWait();
      if ( ( c_isoMonitor.isoMemberCnt( true ) < 1 ) && (i32_wait < 2500))
      { // no claimed IsoItem_c nodes in the monitor list --> i.e. we received no adr claim
        // --> when we are the only ECU on ISOBUS, it doesn't hurt to wait at least 2500 msec after last Req
        i32_wait = 2500;
      }
      if ((i32_time - i32_lastAdrRequestTime) > i32_wait)
      { // last iso adress claim request is still valid and should have been
        // answered till now
        //check if this item is self conf
        // unifyIsoSa delivers actual SA of this item if free
        // if actual SA isn't free and item is NOT-self-conf -> NACK flag 254
        // is answered
        // if actual SA is not free for self-conf item
        // -> a free SA is searched and answered
        // - if no free SA is found, 254 is answered for NACK
        setNr(c_isoMonitor.unifyIsoSa(this));

        if (nr() == 254)
        { // no success -> send NACK and switch to off | error state
          setItemState(IState_c::itemState_t(IState_c::Off | IState_c::Error));
          clearItemState(IState_c::PreAddressClaim);
        }
        else
        { // success -> start address claim mode
          setItemState(IState_c::AddressClaim);
          b_repeatClaim = false;
        }

        // now nr() has now suitable value
        c_pkg.setIsoPri(6);
        c_pkg.setIsoPgn(ADRESS_CLAIM_PGN);
        c_pkg.setIsoPs(255); // global information
        c_pkg.setMonitorItemForSA( this ); // free SA or NACK flag
        // set NAME to CANPkg
        c_pkg.setDataUnion( outputNameUnion() );
        // now IsoSystemPkg_c has right data -> send
        // if (!(c_pkg.identType() == Ident_c::ExtendedIdent))
        // CanPkg_c::identType() changed to static
        if (!(CanPkg_c::identType() == Ident_c::ExtendedIdent))
        {
          // c_pkg.setIdentType(Ident_c::ExtendedIdent);
          // CanPkg_c::setIdentType changed to static
          CanPkg_c::setIdentType(Ident_c::ExtendedIdent);
        }
        c_can << c_pkg;
        // update timestamp
        updateTime();
      }
    }
    else
    { // no adress claim request sent till now
      getIsoMonitorInstance4Comm().sendRequestForClaimedAddress( true );
    }
  }
  else if (itemState(IState_c::AddressClaim))
  { // item in address claim mode (time between send of claim and
    // check if there is a CAN send conflict during send of adress claim
    // final acceptance of adr claim (wait for possible contention)
    if (c_can.stopSendRetryOnErr())
    { // item was in address claim state and send of claim caused error
      setItemState(IState_c::PreAddressClaim);
    }
    else
    {
      if (checkTime(250))
      {
        // no conflict since sent of adress claim since 250ms
        setItemState(IState_c::ClaimedAddress);
        // if there has been a REQUEST_PGN:ADDRESS_CLAIMED_PGN while being in the 250ms-AddressClaim-phase, answer it NOW
        if (b_repeatClaim)
        { // note: this HAS to be done AFTER setting to ClaimedAddress!
          sendSaClaim();
          b_repeatClaim = false;
        }
        // now inform the ISO monitor list change clients on NEW client use
        getIsoMonitorInstance4Comm().broadcastSaAdd2Clients( isoName(), this );
      }
    }
  }
  #ifdef USE_WORKING_SET
  else if ( itemState(IState_c::ClaimedAddress) )
  { // do stuff if completely announced only
    if (isMaster())
    { // We're master, so check if something has to be done..
      if ( i8_slavesToClaimAddress == 0 )
      { // 0 means successfully sent out the ws-announce.
        // So Wait. Nothing to be done here...
      }
      else
      { // <0 or >0
        bool b_sendOutWsMessage=true;
        if ( i8_slavesToClaimAddress < 0 ) // should be -1, but simply catch all <0 for ws-master sending
        { // Announce WS-Master
          i8_slavesToClaimAddress = getIsoMonitorInstance4Comm().getSlaveCount (this); // slavesToClaimAddress will be 0..numberOfSlaves hopefully

          c_pkg.setIsoPgn (WORKING_SET_MASTER_PGN);
          c_pkg.setUint8Data (0, (i8_slavesToClaimAddress+1));
          c_pkg.setLen8FillUpWithReserved (1);
        }
        else // it must be > 0 here now, so omit: if (i8_slavesToClaimAddress > 0)
        { // Slave announcing needs 100ms interspace!
          if (!checkTime(100))
            b_sendOutWsMessage = false;
          else
          { // Announce WS-Slave(s)
            c_pkg.setIsoPgn (WORKING_SET_MEMBER_PGN);
            c_pkg.setDataUnion (getIsoMonitorInstance4Comm().getSlave (getIsoMonitorInstance4Comm().getSlaveCount(this)-i8_slavesToClaimAddress, this)->outputNameUnion());
            i8_slavesToClaimAddress--; // claimed address for one...
          }
        }
        if (b_sendOutWsMessage)
        { // Really send it out on the bus now!
          c_pkg.setIsoPri (7);
          c_pkg.setMonitorItemForSA (this);
          c_can << c_pkg;
          updateTime();

          // did we send the last message of the announce sequence?
          if (i8_slavesToClaimAddress == 0)
          { // yes, announcing finished!
            i32_timeLastCompletedAnnounceStarted = i32_timeCurrentAnnounceStarted;
            i32_timeCurrentAnnounceStarted = -1; // no announce running right now.
            // now see if we have to repeat the sequence because it was requested while it was sent...
            if (b_repeatAnnounce)
            { // repeat the announce-sequence
              b_repeatAnnounce = false;
              (void) startWsAnnounce();
            }
          }
        }
      }
    }
  }
  #endif

  // nothing to be done
  return true;
}


/** process received CAN pkg to update data and react if needed
  * update settings for remote members (e.g. change of SA)
  * react on adress claims or request for adress claims for local items
  @return true -> a reaction on the received/processed msg was sent
*/
bool IsoItem_c::processMsg()
{
  bool b_result = false;
  IsoSystemPkg_c& c_pkg = getIsoMonitorInstance4Comm().data();
  int32_t i32_pkgTime = c_pkg.time(),
      i32_now = ElementBase_c::getLastRetriggerTime();
  if ((i32_now - i32_pkgTime) > 100) updateTime(i32_now);
  else updateTime(i32_pkgTime);

  switch ((c_pkg.isoPgn() & 0x3FF00))
  {
    case ADRESS_CLAIM_PGN: // adress claim
      // check if this item is local
      if ( itemState(IState_c::Local ) )
      { // IsoItem_c::processMsg() is only called for local item, when
        // this item has higher PRIO, so that we shall reject the SA steal
        // by resending OUR SA CLAIM
        c_pkg.setMonitorItemForSA( this );
        c_pkg.setIsoPri(6);
        c_pkg.setIsoPgn(ADRESS_CLAIM_PGN);
        c_pkg.setIsoPs(255); // global information
          // set NAME to CANPkg
        c_pkg.setDataUnion(outputNameUnion());
          // now IsoSystemPkg_c has right data -> send
        getCanInstance4Comm() << c_pkg;
      }
      else
      { // remote item (( the case with change of isoName should NO MORE HAPPEN as IsoMonitor_c
        // simply removes IsoItem_c instances with same SA and different IsoName_c ))
        const bool b_isChange = ( ( c_pkg.isoSa() != nr() ) || ( isoName() != *(c_pkg.getDataUnionConst()) ) );
        const bool b_wasClaimed = itemState(IState_c::ClaimedAddress);
        if ( b_wasClaimed &&  b_isChange )
        { // the previously using item had already claimed an address
          getIsoMonitorInstance4Comm().broadcastSaRemove2Clients( isoName(), nr() );
        }
        setItemState(IState_c::ClaimedAddress);
        setNr(c_pkg.isoSa());
        inputNameUnion(c_pkg.getDataUnionConst());
        if ( (!b_wasClaimed) || b_isChange )
        { // now inform the ISO monitor list change clients on NEW client use
          getIsoMonitorInstance4Comm().broadcastSaAdd2Clients( isoName(), this );
        }
      }
      b_result = true;
    break;
  } // end switch

  return b_result;
}


/** send a SA claim message
  * - needed to respond on request for claimed SA fomr other nodes
  * - also needed when a local ident triggers a periodic request for SA
  * @return true -> this item has already a claimed SA -> it sent its SA; false -> didn't send SA, as not yet claimed or not local
  */
bool IsoItem_c::sendSaClaim()
{
  // If we're still in AddressClaim, delay the saSending until we're ClaimedAddress
  if (   itemState(IState_c::AddressClaim) ) { b_repeatClaim = true; return false; }
  if ( ! itemState(IState_c::ClaimedAddress) ) return false; ///< send no answer, if not yet ready claimed
  if ( ! itemState(IState_c::Local) ) return false;
  // now this ISOItem should/can send a SA claim
  #ifdef DEBUG
  INTERNAL_DEBUG_DEVICE << "Send SA claim (sendSaClaim())" << INTERNAL_DEBUG_DEVICE_ENDL;
  #endif
  IsoSystemPkg_c& c_pkg = getIsoMonitorInstance4Comm().data();
  c_pkg.setIsoPri(6);
  c_pkg.setIsoPgn(ADRESS_CLAIM_PGN);
  c_pkg.setIsoPs(255); // global information
  c_pkg.setMonitorItemForSA( this );
  // set NAME to CANPkg
  c_pkg.setDataUnion(outputNameUnion());
  // now IsoSystemPkg_c has right data -> send
  getCanInstance4Comm() << c_pkg;
  return true;
}



/** calculate random wait time between 0 and 153msec. from NAME and time
  @return wait offset in msec. [0..153]
*/
uint8_t IsoItem_c::calc_randomWait()
{ // perform some calculation from NAME
  uint16_t ui16_result = outputNameUnion()->getUint8Data(0) * outputNameUnion()->getUint8Data(1);
  if ( ( (outputNameUnion()->getUint8Data(2) +1) != 0 )
    && ( System_c::getTime() != 0 )
    && ( (System_c::getTime() / (outputNameUnion()->getUint8Data(2) +1)) != 0 ) )
    ui16_result /= (System_c::getTime() / (outputNameUnion()->getUint8Data(2) +1));
  ui16_result += outputNameUnion()->getUint8Data(3);
  ui16_result %= (outputNameUnion()->getUint8Data(4) + 1);
  ui16_result -= outputNameUnion()->getUint8Data(5);
  ui16_result *= ((outputNameUnion()->getUint8Data(6) + 1) / (outputNameUnion()->getUint8Data(7) + 1));
  // divide by last uint8_t of name till offset in limit
  for (;ui16_result > 153; ui16_result /= (outputNameUnion()->getUint8Data(7) + 1));
  return uint8_t(ui16_result & 0xFF );
}


/**
  lower comparison between left IsoItem_c and right ISOName uint8_t
  @param arc_left left ServiceItem_c parameter
  @param ab_right ISOName uint8_t right parameter
*/
bool lessThan(const IsoItem_c& arc_left, const IsoName_c& ac_right)
{
  return (arc_left.isoName() < ac_right)?true:false;
}


#ifdef USE_WORKING_SET
int32_t IsoItem_c::startWsAnnounce()
{
  int32_t const ci32_timeNow = HAL::getTime();

  if (i32_timeCurrentAnnounceStarted < 0)
  { // no announce currently running, so start it!
    i32_timeCurrentAnnounceStarted = ci32_timeNow;
    i8_slavesToClaimAddress = -1; // start with announcing the master!
  }
  else
  { // we're right in announcing, so delay a second one till after this one ran through
    b_repeatAnnounce = true;
  }

  // return the Announce-Key
  return ci32_timeNow; // which is the NOW-time!
}


/** returns a pointer to the referenced master ISOItem */
IsoItem_c* IsoItem_c::getMaster() const
{
  return pc_masterItem;
}


/** attaches to a working set master and become slave hereby
  if called with NULL the ISOItem loses working-set membership and becomes STANDALONE again!
 */
void
IsoItem_c::setMaster (IsoItem_c* apc_masterItem)
{
  pc_masterItem = apc_masterItem;
}


/// For checking if the WS-Announce is completed use the "announce key" returned from "startWsAnnounce()".
/// Only check for valid announce keys (i.e. ai32_timeAnnounceStarted).
/// Init your announce key to -1, then you're always fine with calling this function!
bool
IsoItem_c::isWsAnnounced (int32_t ai32_timeAnnounceStarted)
{
  if (ai32_timeAnnounceStarted < 0)
    return false;
  else
    return (i32_timeLastCompletedAnnounceStarted >= ai32_timeAnnounceStarted);
}
#endif


} // end of namespace __IsoAgLib
