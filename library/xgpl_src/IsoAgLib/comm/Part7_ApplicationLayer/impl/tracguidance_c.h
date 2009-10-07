/***************************************************************************
                 tracguidance_c.cpp - stores, updates and delivers all guidance
                                  data information from CanCustomer_c
                                  derived for CAN sending and receiving
                                  interaction;
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
#ifndef TRAC_GUIDANCE_C_H
#define TRAC_GUIDANCE_C_H

#include "basecommon_c.h"


namespace __IsoAgLib {

class TracGuidance_c;
typedef SINGLETON_DERIVED(TracGuidance_c,BaseCommon_c) SingletonTracGuidance_c;
  /** stores, updates  and delivers all moving data information;
      Derive from BaseCommon_c some fundamental funktionality for all base data
      Derive from Scheduler_Task_c to register in Scheduler_c for timeEvent trigger
      Derive from CANCustomer to register FilterBox'es in CanIo_c to receive CAN messages
      Derive from SINGLETON to create a Singleton which manages one global accessible singleton
      per IsoAgLib instance (if only one IsoAgLib instance is defined in application config, no overhead is produced).
    */
  class TracGuidance_c : public SingletonTracGuidance_c
  {
  public:// Public methods

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
    /** config the TracGuidance_c object after init -> set pointer to isoName and
        config send/receive of a guidance msg type
        @param apc_isoName pointer to the ISOName variable of the responsible member instance (pointer enables automatic value update if var val is changed)
        @param at_identMode either IsoAgLib::IdentModeImplement or IsoAgLib::IdentModeTractor
      */
    virtual bool config_base (const IsoName_c* apc_isoName, IsoAgLib::IdentMode_t at_identMode, uint16_t aui16_suppressMask = 0);

    /** destructor for TracGuidance_c which has nothing to do */
    virtual ~TracGuidance_c() { BaseCommon_c::close();}

    /* ******************************************* */
    /** \name Set Values for periodic send on BUS  */
    /*@{*/
    /** set machine steering system's estimate of the curvature of the current turn
        @param i16_val  estimated value of curvature of current turn
      */
    void setEstCurvature(const int16_t i16_val) {mui16_estCurvature = (i16_val + 8032)  * 4;}

    /** set machine steering system request to the automatic guidance system to change curvature command status
        @param t_val  value of curvature command status
      */
    void setRequestResetCmdStatus(const IsoAgLib::IsoResetFlag_t t_val) {mt_requestResetCmdStatus = t_val;}

    /** set machine steering system indication if manual steering is in correct position for guidance to function
        @param t_val  state of manual steering position
      */
    void setSteeringInputPosStatus(const IsoAgLib::IsoSteerPosFlag_t t_val) {mt_steeringInputPosStatus = t_val;}

    /** set machine steering system indication that it is free from faults that would preclude guidance system operation
        @param t_val  status of machine steering system
      */
    void setSteeringSystemReadiness( const IsoAgLib::IsoSteerReadinessFlag_t t_val) {mt_steeringSystemReadiness = t_val;}

    /** set state of lockout switch that allows operators to disable automatic steering system functions
        @param t_val  status of lockout switch
      */
    void setMechanicalSystemLogout(const IsoAgLib::IsoActiveFlag_t t_val) {mt_mechanicalSystemLogout = t_val;}
    /*@}*/

    /* ****************************************************** */
    /** \name Retrieve Values which are sent from other ECUs  */
    /*@{*/
    /** get machine steering system's estimate of the curvature of the current turn
        @return  estimated value of curvature of current turn
      */
    int16_t estCurvature() const { return ( (mui16_estCurvature / 4 ) - 8032);}

    /** get machine steering system request to the automatic guidance system to change curvature command status
        @return  request reset command status
      */
    IsoAgLib::IsoResetFlag_t requestResetCmdStatus()const { return mt_requestResetCmdStatus;}

    /** get machine steering system indication if manual steering is in correct position for guidance to function
        @return  state of manual steering position
      */
    IsoAgLib::IsoSteerPosFlag_t steeringInputPosStatus() const { return mt_steeringInputPosStatus;}

    /** get machine steering system indication that it is free from faults that would preclude guidance system operation
        @return  status of machine steering system
      */
    IsoAgLib::IsoSteerReadinessFlag_t steeringSystemReadiness() const { return mt_steeringSystemReadiness;}

    /** get state of lockout switch that allows operators to disable automatic steering system functions
        @return  status of lockout switch
      */
    IsoAgLib::IsoActiveFlag_t mechanicalSystemLogout() const { return mt_mechanicalSystemLogout;}
    /*@}*/

  ///  Used for Debugging Tasks in Scheduler_c
  virtual const char* getTaskName() const;
  /** dummy implementation */
  virtual bool processMsgRequestPGN (uint32_t aui32_pgn, IsoItem_c* apc_isoItemSender, IsoItem_c* apc_isoItemReceiver);

  private:
    // Private methods
    friend class SINGLETON_DERIVED(TracGuidance_c,BaseCommon_c);

    /** HIDDEN constructor for a TracGuidance_c object instance which can optional
        set the configuration for send/receive for a guidance msg
        NEVER instantiate a variable of type TracGuidance_c within application
        only access TracGuidance_c via getTracGuidanceInstance() or getTracGuidanceInstance( int riLbsBusNr ) in case more than one ISO11783 BUS is used for IsoAgLib
      */
    TracGuidance_c() {}

    /** check if filter boxes shall be created - create only ISO filters based
        on active local idents which has already claimed an address
        --> avoid to much Filter Boxes
      */
    virtual void checkCreateReceiveFilter( );

    /** send a ISO11783 guidance information PGN.
      * this is only called when sending ident is configured and it has already claimed an address
        @pre  function is only called in tractor mode
        @see  BaseCommon_c::timeEvent()
      */
    virtual bool timeEventTracMode( );

    /** process a ISO11783 base information PGN
        @pre  sender of message is existent in monitor list
        @see  CanPkgExt_c::resolveSendingInformation()
      */
    virtual bool processMsg();

    /** send guidance data
        @see  CanIo_c::operator<<
      */
    void isoSendMessageTracMode( );

  private:
    // Private attributes
    /** machine steering system's estimate of the curvature of the current turn */
    uint16_t mui16_estCurvature;

    /** machine steering system request to the automatic guidance system to change curvature command status */
    IsoAgLib::IsoResetFlag_t mt_requestResetCmdStatus;

    /** machine steering system indicates if manual steering is in correct position for guidance to function */
    IsoAgLib::IsoSteerPosFlag_t mt_steeringInputPosStatus;

    /** machine steering system indicates that it is free from faults that would preclude guidance system operation */
    IsoAgLib::IsoSteerReadinessFlag_t mt_steeringSystemReadiness;

    /** state of lockout switch that allows operators to disable automatic steering system functions */
    IsoAgLib::IsoActiveFlag_t mt_mechanicalSystemLogout;
  };

  #if defined(PRT_INSTANCE_CNT) && (PRT_INSTANCE_CNT > 1)
  /** C-style function, to get access to the unique TracGuidance_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an m_index must be given to select the wanted BUS
    */
  TracGuidance_c& getTracGuidanceInstance(uint8_t aui8_instance = 0);
  #else
  /** C-style function, to get access to the unique TracGuidance_c singleton instance */
  TracGuidance_c& getTracGuidanceInstance(void);
  #endif
}

#endif
