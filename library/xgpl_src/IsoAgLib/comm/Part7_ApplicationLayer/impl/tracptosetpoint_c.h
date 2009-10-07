/***************************************************************************
       tracptosetpoint_c.h  - working on pto set point data;
                              stores, updates  and delivers all pto set point
                              data informations from CanCustomer_c derived for
                              CAN sending and receiving interaction;
                              from Scheduler_Task_c derived for
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
#ifndef TRACPTOSETPOINT_C_H
#define TRACPTOSETPOINT_C_H

#include "basecommon_c.h"


namespace __IsoAgLib { // Begin Namespace __IsoAgLib

  class TracPTOSetPoint_c;
  typedef SINGLETON_DERIVED(TracPTOSetPoint_c,BaseCommon_c) SingletonTracPtoSetPoint_c;
  /** working on pto set point data Type;
      stores, updates and delivers all base data informations;
      Derive from BaseCommon_c some fundamental funktionality for all base data
      Derive from Scheduler_Task_c to register in Scheduler_c for timeEvent trigger
      Derive from CANCustomer to register FilterBox'es in CanIo_c to receive CAN messages
      Derive from SINGLETON to create a Singleton which manages one global accessible singleton
      per IsoAgLib instance (if only one IsoAgLib instance is defined in application config, no overhead is produced).
    */
  class TracPTOSetPoint_c : public SingletonTracPtoSetPoint_c {
  public:// Public methods
    /* ********************************************* */
    /** \name Management Functions for class TracPTO_c  */
    /*@{*/


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
    /** config the TracPTOSetPoint_c object after init -> set pointer to isoName and
        config send/receive of different base msg types
        @param apc_isoName pointer to the ISOName variable of the responsible member instance (pointer enables automatic value update if var val is changed)
      @param at_identMode either IsoAgLib::IdentModeImplement or IsoAgLib::IdentModeTractor
      */
    virtual bool config_base (const IsoName_c* apc_isoName, IsoAgLib::IdentMode_t at_identMode = IsoAgLib::IdentModeImplement, uint16_t aui16_suppressMask = 0);

    /** destructor for TracPTOSetPoint_c which has nothing to do */
    virtual ~TracPTOSetPoint_c() { BaseCommon_c::close();};
    /*@}*/

    /* ******************************************* */
    /** \name Set Values for periodic send on BUS  */
    /*@{*/

    /** allowing the position of the front three-point hitch to be set
        @param ui8_val  command value
      */
    void setFrontHitchPosCmd(const uint8_t ui8_val) {mui8_frontHitchPosCmd = ( ( ui8_val * 5 ) / 2 );}

    /** allowing the position of the rear three-point hitch to be set
        @param ui8_val  command value
      */
    void setRearPtoSetPointCmd(const uint8_t ui8_val) {mui8_rearHitchPosCmd = ( ( ui8_val * 5 ) / 2 );}

    /** set rotational speed of the front power take-off (PTO) output shaft
        @param ui16_val  command value
      */
    void setFrontPtoSetPointCmd(const uint16_t ui16_val) {mui16_frontPtoSetPointCmd = ( ui16_val / 8 );}
    /** set rotational speed of the rear power take-off (PTO) output shaft
        @param ui16_val  command value
      */
    void setRearPtoSetPointCmd(const uint16_t ui16_val) {mui16_rearPtoSetPointCmd = ( ui16_val / 8 );}

    /** set front power take-off (PTO)
        @param t_val  IsoActive -> power take-off is engaged
      */
    void setFrontPtoEngagement(const IsoAgLib::IsoActiveFlag_t t_val) {mt_frontPtoEngagement = t_val;}

    /** set rear power take-off (PTO)
        @param t_val  IsoActive -> power take-off is engaged
      */
    void setRearPtoEngagement(const IsoAgLib::IsoActiveFlag_t t_val) {mt_rearPtoEngagement = t_val;}

    /** set mode of the front power take-off (PTO)
        @param t_val  IsoActive -> 1000 r/min; IsoInactive -> 540 r/min
      */
    void setFrontPtoMode(const IsoAgLib::IsoActiveFlag_t t_val) {mt_frontPtoMode = t_val;}

    /** set mode of the rear power take-off (PTO)
        @param t_val  IsoActive -> 1000 r/min; IsoInactive -> 540 r/min
      */
    void setRearPtoMode(const IsoAgLib::IsoActiveFlag_t t_val) {mt_rearPtoMode = t_val;}

    /** set front power take-off (PTO) economy mode
        @param t_val  IsoActive -> pto economy mode is engaged
      */
    void setFrontPtoEconomyMode(const IsoAgLib::IsoActiveFlag_t t_val) {mt_frontPtoEconomyMode = t_val;}

    /** set rear power take-off (PTO) economy mode
        @param t_val  IsoActive -> pto economy mode is engaged
      */
    void setRearPtoEconomyMode(const IsoAgLib::IsoActiveFlag_t t_val){mt_rearPtoEconomyMode = t_val;}
    /*@}*/

    /* ****************************************************** */
    /** \name Retrieve Values which are sent from other ECUs  */
    /*@{*/

    /** get position of the front three-point hitch
        @return  current position of the three-point hitch
      */
    uint8_t frontHitchPosCmd() const {return ( ( mui8_frontHitchPosCmd * 2 ) / 5 );}

    /** get position of the rear three-point hitch
        @return  current position of the three-point hitch
      */
    uint8_t rearHitchPosCmd() const {return ( ( mui8_rearHitchPosCmd * 2 ) / 5 );}

    /** get rotational speed of the front power take-off (PTO) output shaft
        @return current rotational speed of power take-off
      */
    uint16_t frontPtoSetPointCmd() const {return ( mui16_frontPtoSetPointCmd * 8 );}

    /** get rotational speed of the rear power take-off (PTO) output shaft
        @return  current rotational speed of power take-off
      */
    uint16_t rearPtoSetPointCmd() const {return ( mui16_rearPtoSetPointCmd * 8 );}

    /** get front power take-off (PTO)
        @return  IsoActive -> power take-off is engaged
      */
    IsoAgLib::IsoActiveFlag_t frontPtoEngagement() const {return mt_frontPtoEngagement;}

    /** get rear power take-off (PTO)
        @return  IsoActive -> power take-off is engaged
      */
    IsoAgLib::IsoActiveFlag_t rearPtoEngagement() const {return mt_rearPtoEngagement;}

    /** get mode of the front power take-off (PTO)
        @return  IsoActive -> 1000 r/min; IsoInactive -> 540 r/min
      */
    IsoAgLib::IsoActiveFlag_t frontPtoMode() const {return mt_frontPtoMode;}

    /** get mode of the rear power take-off (PTO)
        @return  IsoActive -> 1000 r/min; IsoInactive -> 540 r/min
      */
    IsoAgLib::IsoActiveFlag_t rearPtoMode() const {return mt_rearPtoMode;}

    /** get front power take-off (PTO) economy mode
        @return  IsoActive -> pto economy mode is engaged
      */
    IsoAgLib::IsoActiveFlag_t frontPtoEconomyMode() const {return mt_frontPtoEconomyMode;}

    /** get rear power take-off (PTO) economy mode
        @return  IsoActive -> pto economy mode is engaged
      */

    ///  Used for Debugging Tasks in Scheduler_c
    virtual const char* getTaskName() const;
    /** dummy implementation */
    virtual bool processMsgRequestPGN (uint32_t aui32_pgn, IsoItem_c* apc_isoItemSender, IsoItem_c* apc_isoItemReceiver);

    IsoAgLib::IsoActiveFlag_t rearPtoEconomyMode() const {return mt_rearPtoEconomyMode;}
    /*@}*/

  private:
    // Private methods
    friend class SINGLETON_DERIVED(TracPTOSetPoint_c,BaseCommon_c);
    /** HIDDEN constructor for a TracPTOSetPoint_c object instance which can optional
        set the configuration for send/receive for base msg type
        NEVER instantiate a variable of type TracPTOSetPoint_c within application
        only access TracPTOSetPoint_c via getTracPtoSetPointInstance() or getTracPtoSetPointInstance( int riLbsBusNr ) in case more than one ISO11783 BUS is used for IsoAgLib
      */
    TracPTOSetPoint_c() {};

    /** check if filter boxes shall be created - create only ISO filters based
        on active local idents which has already claimed an address
        --> avoid to much Filter Boxes
      */
    virtual void checkCreateReceiveFilter();

    /** send a ISO11783 pto information PGN.
      * this is only called when sending ident is configured and it has already claimed an address
        @pre  function is only called in implement mode
        @see  BaseCommon_c::timeEvent()
      */
    virtual bool timeEventImplMode();

    /** send Base2 Group data with hitch and PTO state */
    void isoSendBase2Update( void );

    /** process a ISO11783 base information PGN
        @pre  sender of message is existent in monitor list
        @see  CanPkgExt_c::resolveSendingInformation()
      */
    virtual bool processMsg();

    /** send hitch and pto command
        @see  CanIo_c::operator<<
      */
    void sendPtoMsg();

  private:
    // Private attributes

    /** general */
    /** position of the front three-point hitch*/
    uint8_t mui8_frontHitchPosCmd;

    /** position of the rear three-point hitch*/
    uint8_t mui8_rearHitchPosCmd;

    /** rotational speed of the front power take-off (PTO) output shaft*/
    uint16_t mui16_frontPtoSetPointCmd;

    /** rotational speed of the rear power take-off (PTO) output shaft*/
    uint16_t mui16_rearPtoSetPointCmd;

    /** tate of the front power take-off (PTO)*/
    IsoAgLib::IsoActiveFlag_t mt_frontPtoEngagement;

    /** state of the rear power take-off (PTO)*/
    IsoAgLib::IsoActiveFlag_t mt_rearPtoEngagement;

    /** mode of the front power take-off (PTO): IsoInactive == 1000 r/min; IsoActive == 540 r/min*/
    IsoAgLib::IsoActiveFlag_t mt_frontPtoMode;

    /** mode of the rear power take-off (PTO): IsoInactive == 1000 r/min; IsoActive == 540 r/min*/
    IsoAgLib::IsoActiveFlag_t mt_rearPtoMode;

    /** state of the front power take-off (PTO) economy mode*/
    IsoAgLib::IsoActiveFlag_t mt_frontPtoEconomyMode;

    /** state of the rear power take-off (PTO) economy mode*/
    IsoAgLib::IsoActiveFlag_t mt_rearPtoEconomyMode;
  };

  #if defined(PRT_INSTANCE_CNT) && (PRT_INSTANCE_CNT > 1)
  /** C-style function, to get access to the unique TracPTOSetPoint_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an m_index must be given to select the wanted BUS
    */
  TracPTOSetPoint_c& getTracPtoSetPointInstance(uint8_t aui8_instance = 0);
  #else
  /** C-style function, to get access to the unique TracPTOSetPoint_c singleton instance */
  TracPTOSetPoint_c& getTracPtoSetPointInstance(void);
  #endif
}
#endif
