/*
  tracguidance_c.cpp: stores, updates and delivers all guidance data
    information from CanCustomer_c derived for CAN sending and
    receiving interaction; from BaseCommon_c derived for interaction
    with other IsoAgLib objects

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
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

    /** config the TracGuidance_c object after init -> set pointer to isoName and
        config send/receive of a guidance msg type
        @param apc_isoName pointer to the ISOName variable of the responsible member instance (pointer enables automatic value update if var val is changed)
        @param at_identMode either IsoAgLib::IdentModeImplement or IsoAgLib::IdentModeTractor
      */
    virtual bool config_base (const IsoName_c* apc_isoName, IsoAgLib::IdentMode_t at_identMode, uint16_t aui16_suppressMask = 0);

    virtual ~TracGuidance_c() {}

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

#if DEBUG_SCHEDULER
    virtual const char* getTaskName() const;
#endif

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
