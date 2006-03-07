/***************************************************************************
                          baseitem_c.h  - base class for member lists;
                                    stores access time
                                    and pointer to root Scheduler_c object
                             -------------------
    begin                : Fri Apr 07 2000
    copyright            : (C) 2000 - 2004 by Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
    type                 : Header
    $LastChangedDate: 2005-01-11 10:55:57 +0100 (Di, 11 Jan 2005) $
    $LastChangedRevision: 805 $
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
#ifndef BASE_COMMON_H
#define BASE_COMMON_H

#include <IsoAgLib/driver/system/impl/system_c.h>
#include <IsoAgLib/util/impl/elementbase_c.h>
#include <IsoAgLib/util/impl/singleton.h>
#include <IsoAgLib/util/impl/getypos_c.h>
#include <IsoAgLib/util/impl/cancustomer_c.h>
#include <IsoAgLib/util/impl/canpkg_c.h>
#include <IsoAgLib/comm/Base/ibasetypes.h>
#ifdef USE_DIN_9684
  #include <IsoAgLib/comm/SystemMgmt/DIN9684/impl/dinmonitor_c.h>
#endif
#ifdef USE_ISO_11783
  #include <IsoAgLib/comm/SystemMgmt/ISO11783/impl/isomonitor_c.h>
#endif

// Begin Namespace __IsoAgLib
namespace __IsoAgLib
{

  class BaseCommon_c : public ElementBase_c
  {
  public:

    /** initialize directly after the singleton instance is created.
        this is called from singleton.h and should NOT be called from the user again.
        users please use init(...) instead.
      */
    void singletonInit();

    /** every subsystem of IsoAgLib has explicit function for controlled shutdown */
    void close( );

    /** initialise element which can't be done during construct;
        above all create the needed FilterBox_c instances
        possible errors:
          * dependant error in CANIO_c problems during insertion of new FilterBox_c entries for IsoAgLibBase
        @param rpc_devKey optional pointer to the DEV_KEY variable of the responsible member instance (pointer enables automatic value update if var val is changed)
        @param rt_identMode either IsoAgLib::IdentModeImplement or IsoAgLib::IdentModeTractor
      */
    virtual void init(const DevKey_c*, IsoAgLib::IdentMode_t rt_identMode = IsoAgLib::IdentModeImplement);

    /** tractor object after init --> store devKey and mode
        @param rpc_devKey pointer to the DEV_KEY variable of the responsible member instance (pointer enables automatic value update if var val is changed)
        @param rt_identMode either IsoAgLib::IdentModeImplement or IsoAgLib::IdentModeTractor
      */
    virtual void config(const DevKey_c* rpc_devKey, IsoAgLib::IdentMode_t rt_identMode = IsoAgLib::IdentModeImplement);

     /** deliver reference to data pkg
         @return reference to the member CanPkg, which encapsulates the CAN send structure
        */
    CANPkgExt_c& data() {return c_data;};

    /**
      virtual function which delivers a pointer to the CANCustomer
      specific CANPkgExt_c instance
    */
    virtual CANPkgExt_c& dataBase();

    /** check if filter boxes shall be created - create only ISO filters based
      on active local idents which has already claimed an address
      --> avoid to much Filter Boxes
    */
    virtual void checkCreateReceiveFilter() = 0;

     /** process received base msg and store updated value for later reading access;
      called by FilterBox_c::processMsg after receiving a base msg
      possible errors:
        * LibErr_c::LbsBaseSenderConflict base msg recevied from different member than before
      @see FilterBox_c::processMsg
      @see CANIO_c::processMsg
      @return true -> message was processed; else the received CAN message will be served to other matching CANCustomer_c
    */
    bool processMsg();

    /** functions with actions, which must be performed periodically
        -> called periodically by Scheduler_c
        ==> sends base msg if configured in the needed rates
        possible errors:
          * dependant error in CANIO_c on CAN send problems
        @see CANPkg_c::getData
        @see CANPkgExt_c::getData
        @see CANIO_c::operator<<
        @return true -> all planned activities performed in allowed time
      */
    virtual bool timeEvent();

    #ifdef USE_ISO_11783
    /** process a ISO11783 base information PGN */
    virtual bool isoProcessMsg() = 0;
    /** send a ISO11783 base information PGN.
      * this is only called when sending ident is configured and it has already claimed an address
      */
    virtual bool isoTimeEvent() = 0;
    #endif
    #ifdef USE_DIN_9684
    /** process a DIN9684 base information PGN */
    virtual bool dinProcessMsg() {return false;}
    /** send a DIN9684 base information PGN
      * this is only called when sending ident is configured and it has already claimed an address
      */
    virtual bool dinTimeEvent() {return false;}
    #endif

    /** Retrieve the last update time of the specified information type*/
    int32_t lastedTimeSinceUpdate() const {return (System_c::getTime() - i32_lastMsgReceived);}
    /** Retrieve the time of last update */
    int32_t lastUpdateTime() const {return i32_lastMsgReceived;}

    /** check if a received message should be parsed */
    bool checkParseReceived(const DevKey_c& rrefc_currentSender) const;

    /** return if you currently are in implement mode or tractor mode*/
    bool checkMode(IsoAgLib::IdentMode_t rt_identMode) const {return (t_identMode == rt_identMode);}
    #ifdef USE_ISO_11783
    /** check if iso filters have alread been created*/
    bool checkIsoFilterCreated() const {return b_isoFilterCreated;}
    /** set b_isoFilterCreated*/
    void setIsoFilterCreated() {b_isoFilterCreated = true;}
    /** clear b_isoFilterCreated*/
    void clearIsoFilterCreated() {b_isoFilterCreated = false;}
    #endif
    #ifdef USE_DIN_9684
      /**
    deliver type of base msg BABO
    @return BABO code of base msg (bit 4-7 in identifier)
    */
    uint8_t dataBabo()const {return (CANPkg_c::ident(0) >> 4);}
    /** check if din filters have alread been created*/
    bool checkDinFilterCreated() const {return b_dinFilterCreated;}
    /** set b_dinFilterCreated*/
    void setDinFilterCreated() {b_dinFilterCreated = true;}
    /** clear b_dinFilterCreated*/
    void clearDinFilterCreated() {b_dinFilterCreated = false;}
    #endif

    /** return sender of a msg*/
    const DevKey_c* getDevKey() const {return pc_devKey;};
    /** return a sender which sends commands as a tractor */
    DevKey_c& getSenderDevKey() {return c_sendDevKey;};
    /** return a sender which sends commands as a tractor */
    const DevKey_c& getSenderDevKeyConst() const {return c_sendDevKey;}


    /** set mode to implement or tractor*/
    void setMode(IsoAgLib::IdentMode_t rt_identMode) {t_identMode = rt_identMode;}
    /** set sender which sends commands as a tractor*/
    void setSenderDevKey(const DevKey_c& senderDevKey){c_sendDevKey = senderDevKey;}
    /** set sender of a msg*/
    void setDevKey(const DevKey_c* devKey){pc_devKey = devKey;}
    /** set last time of data msg [msec]*/
    void setUpdateTime(int32_t updateTime) {i32_lastMsgReceived = updateTime;}

  private:
    /** can be implement mode or tractor mode*/
    IsoAgLib::IdentMode_t t_identMode;
    #ifdef USE_DIN_9684
    /** flag to detect, if receive filters for DIN are created */
    bool b_dinFilterCreated;
    #endif
    #ifdef USE_ISO_11783
    /** flag to detect, if receive filters for ISO are created */
    bool b_isoFilterCreated;
    #endif

    /** last time of data msg [msec] */
    int32_t i32_lastMsgReceived;

    /** devKey which act as sender of a msg: either responses on behalf of implement or commands as tractor.
        This pointer is set in config function
      */
    const DevKey_c* pc_devKey;
    /** DEVKEY of sender which sends commands as a tractor*/
    DevKey_c c_sendDevKey;

    /** temp data where received data is put */
    CANPkgExt_c c_data;
    //BasePkg_c c_data;
  };

}// end namespace __IsoAgLib

#endif
