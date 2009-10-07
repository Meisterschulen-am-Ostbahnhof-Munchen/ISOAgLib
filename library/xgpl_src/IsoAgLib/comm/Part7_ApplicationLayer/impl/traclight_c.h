/***************************************************************************
                     traclight_c.h  - working on lighting data;
                                      stores, updates  and
                                      delivers all base data informations
                                      from CanCustomer_c derived for CAN
                                      sending and receiving interaction;
                                      from BaseCommon_c derived for
                                      interaction with other IsoAgLib objects
                             -------------------
    begin                 Fri Apr 07 2000
    copyright            : (C) 2000 - 2009 by Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
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
#ifndef TRACLIGHT_C_H
#define TRACLIGHT_C_H

#include "basecommon_c.h"
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isorequestpgnhandler_c.h>

#include <map>


namespace __IsoAgLib { // Begin Namespace __IsoAgLib

/** bit data for the parameter group number lighting command and lighting data*/
typedef struct {
//Byte 1
  /** daytime running lights state */
  unsigned int daytimeRunning : 2;
  /** alternate head lights state */
  unsigned int alternateHead : 2;
  /** low-beam head lights state */
  unsigned int lowBeamHead : 2;
  /** high-beam head lights state */
  unsigned int highBeamHead : 2;
//Byte 2
  /** front fog lights state */
  unsigned int frontFog : 2;
  /** beacon lights state */
  unsigned int beacon : 2;
  /** right-turn lights state */
  unsigned int rightTurn : 2;
  /** left-turn lights state */
  unsigned int leftTurn : 2;
//Byte 3
  /** back up light and alarm horn */
  unsigned int backUpLightAlarmHorn : 2;
  /** center stop lights state */
  unsigned int centerStop : 2;
  /** right-stop lights state */
  unsigned int rightStop : 2;
  /** left-stop lights state */
  unsigned int leftStop : 2;
//Byte 4
  /** implement clearance lights state */
  unsigned int implClearance : 2;
  /** tractor clearance lights state */
  unsigned int tracClearance : 2;
  /** implement marker lights state */
  unsigned int implMarker : 2;
  /** tractor marker lights state */
  unsigned int tracMarker : 2;
//Byte 5
  /** rear fog lights state */
  unsigned int rearFog : 2;
  /** tractor underside work lights state */
  unsigned int undersideWork : 2;
  /** tractor rear low mounted work lights state */
  unsigned int rearLowWork : 2;
  /** tractor rear high mounted work lights state */
  unsigned int rearHighWork : 2;
//Byte 6
  /** tractor side low mounted work lights state */
  unsigned int sideLowWork : 2;
  /** tractor side high mounted work lights state */
  unsigned int sideHighWork : 2;
  /** tractor front low mounted work lights state */
  unsigned int frontLowWork : 2;
  /** tractor front high mounted work lights state */
  unsigned int frontHighWork : 2;
//Byte 7
  /** implement OEM option 2 light state */
  unsigned int implOEMOpt2 : 2;
  /** implement OEM option 1 light state */
  unsigned int implOEMOpt1 : 2;
  /** implement right forward work lights state */
  unsigned int implRightForwardWork : 2;
  /** implement left forward work lights state */
  unsigned int implLeftForwardWork : 2;
//Byte 8
  /** lighting data message request state */
  unsigned int dataMsgReq : 2;    //reserved in Lighting data
  /** implement right-facing work lights state */
  unsigned int implRightFacingWork : 2;
  /** implement left-facing work lights state */
  unsigned int implLeftFacingWork : 2;
  /** implement rear work lights state */
  unsigned int implRearWork : 2;

  } lightBitData_t;

  class TracLight_c;
  typedef SINGLETON_DERIVED(TracLight_c,BaseCommon_c) SingletonTracLight_c;
  /** stores, updates  and delivers lighting data information;
      Derive from BaseCommon_c some fundamental funktionality for all base data
      Derive from Scheduler_Task_c to register in Scheduler_c for timeEvent trigger
      Derive from CANCustomer to register FilterBox'es in CanIo_c to receive CAN messages
      Derive from SINGLETON to create a Singleton which manages one global accessible singleton
      per IsoAgLib instance (if only one IsoAgLib instance is defined in application config, no overhead is produced).
    */
  class TracLight_c : public SingletonTracLight_c
  {
  public: // Public methods

 /** initialize directly after the singleton instance is created.
      this is called from singleton.h and should NOT be called from the user again.
      users please use init(...) instead.
    */
    void singletonInit();


    /** initialise element which can't be done during construct;
        above all create the needed FilterBox_c instances
        possible errors:
          * dependant error in CanIo_c problems during insertion of new FilterBox_c entries for IsoAgLibBase
        @param apc_isoName optional pointer to the ISOName variable of the responsible member instance (pointer enables automatic value update if var val is changed)
        @param ai_singletonVecKey singleton vector key in case PRT_INSTANCE_CNT > 1
        @param at_identMode either IsoAgLib::IdentModeImplement or IsoAgLib::IdentModeTractor
      */
    virtual void init_base (const IsoName_c*, int ai_singletonVecKey, IsoAgLib::IdentMode_t at_identMode = IsoAgLib::IdentModeImplement);
    /** config the TracLight_c object after init -> set pointer to isoName, set implementMode,
        store pointer to isoName separately if we send as tractor
        @param apc_isoName pointer to the ISOName variable of the responsible member instance (pointer enables automatic value update if var val is changed)
        @param at_identMode either IsoAgLib::IdentModeImplement or IsoAgLib::IdentModeTractor
        @return true -> configuration was successfull
      */
    virtual bool config_base (const IsoName_c* apc_isoName, IsoAgLib::IdentMode_t at_identMode, uint16_t aui16_suppressMask = 0);

    bool processMsgRequestPGN (uint32_t aui32_pgn, IsoItem_c* apc_isoItemSender, IsoItem_c* apc_isoItemReceiver);

    /** destructor for TracLight_c which has nothing to do */
    virtual ~TracLight_c() { BaseCommon_c::close();};

    /** set a lighting command in tractor mode or set a response on a lighting command in implement mode
        @param t_command  command to set
        @param t_val      command value
      */
    void setCommand(const IsoAgLib::IsoCommandFlag_t t_command, const IsoAgLib::IsoActiveFlag_t t_val);

    /** get the number of responses from implements to a command from the tractor
        @return number of responding implements to a command
      */
    int getCount() const {return mmap_data.size();};

    /** get the status of a light of a special implement
        possible errors:
          * iLibErr_c::Range m_index exceeds map size
        @param acui_index    acui_index of implement
        @param at_command  which light
        @return           status of the light of the implement
      */
    IsoAgLib::IsoActiveFlag_t getByIndex(const unsigned int acui_index, const IsoAgLib::IsoCommandFlag_t at_command);

    /** get the status of a light of a special implement
        possible errors:
          * iLibErr_c::Range the implement with sa has not responded to command from the tractor
        @param sa         source address of implement
        @param t_command  which light
        @return           status of the light of the implement
      */
    IsoAgLib::IsoActiveFlag_t getBySa(const uint8_t sa, const IsoAgLib::IsoCommandFlag_t t_command) const;

    /** get command which the tractor send
        @param t_command  command from the tractor
        @return           value of the command which the tractor send
      */
    IsoAgLib::IsoActiveFlag_t getCommand(const IsoAgLib::IsoCommandFlag_t t_command) const;


    /** find out if a particular implement has responded to a command from the tractor
        @param sa   source address of the sought implement
        @return     has responded to a command (true), else (false)
      */
    bool find(const uint8_t sa) const;

  ///  Used for Debugging Tasks in Scheduler_c
  virtual const char* getTaskName() const;

  private:
    // Private methods
    friend class SINGLETON_DERIVED(TracLight_c,BaseCommon_c);
    /** HIDDEN constructor for a TracLight_c object instance which can optional
        set the configuration for send/receive for a lighting msg type
        NEVER instantiate a variable of type TracLight_c within application
        only access TracLight_c via getTracLightInstance() or getTracLightInstance( int riLbsBusNr ) in case more than one ISO11783 BUS is used for IsoAgLib
      */
    TracLight_c();

    /** check if filter boxes shall be created - create only ISO filters based
        on active local idents which has already claimed an address
        --> avoid to much Filter Boxes
      */
    virtual void checkCreateReceiveFilter( );

    /** helper funktion to get the mode of a light of a specified implement
        @param at_data       bit field where the lighting information is stored
        @param at_command  light to find
        @return           status of the light
      */
    IsoAgLib::IsoActiveFlag_t getInfo(const lightBitData_t& at_data, const IsoAgLib::IsoCommandFlag_t at_command) const;

    /** functions with actions, which must be performed periodically
        -> called periodically by Scheduler_c
        ==> sends msg if configured in the needed rates
        possible errors:
          * dependant error in CanIo_c on CAN send problems
        @pre  function is only called in tractor mode
        @see  BaseCommon_c::timeEvent()
        @return true -> all planned activities performed in allowed time
      */
    virtual bool timeEventTracMode();

    /** functions with actions, which must be performed periodically
        -> called periodically by Scheduler_c
        ==> sends msg if configured in the needed rates
        possible errors:
          * dependant error in CanIo_c on CAN send problems
        @pre  function is only called in implement mode
        @see  BaseCommon_c::timeEvent()
        @return true -> all planned activities performed in allowed time
      */
    virtual bool timeEventImplMode();

    /** process received msg and store updated value for later reading access;
        called by FilterBox_c::processMsg after receiving a msg
        possible errors:
          * iLibErr_c::BaseSenderConflict msg received from different member than before

        @pre  sender of message is existent in monitor list
        @see  CanPkgExt_c::resolveSendingInformation()
        @see FilterBox_c::processMsg
        @see CanIo_c::processMsg
        @return true -> message was processed; else the received CAN message will be served to other matching CanCustomer_c
      */
    virtual bool processMsg();

    /** send light update; there is a difference between implement and tractor mode
        @see  TracLight_c::processMsgRequestPGN
        @see  CanIo_c::operator<<
      */
    void sendMessage();

  private:
    // Private attributes
    /** remember if we received a command, which have to be responded */
    bool mb_cmdWait4Response;

    /** send lighting command if change of lamp between OF/ON state */
    bool mb_changeNeedBeSend;


    /// General
    /** bit field with all lighting information for tractor*/
    lightBitData_t mt_cmd;

    /** stores for each requesting implement (sourceAddress) a bit field with all lighting information*/
    STL_NAMESPACE::map<uint8_t, lightBitData_t> mmap_data; // access mmap_data[sourceAdr].

    /** the last 10 times when a command was send.*/
    int32_t marr_timeStamp[10];

    /** m_index which stores the position where the eldest marr_timeStamp is stored*/
    unsigned int m_index;
  };

  #if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
  /** C-style function, to get access to the unique TracLight_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an m_index must be given to select the wanted BUS
    */
  TracLight_c& getTracLightInstance( uint8_t aui8_instance = 0 );
  #else
  /** C-style function, to get access to the unique TracLight_c singleton instance */
  TracLight_c& getTracLightInstance( void );
  #endif
}
#endif
