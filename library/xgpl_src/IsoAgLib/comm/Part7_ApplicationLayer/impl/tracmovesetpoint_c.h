/*
  tracmovesetpoint_c.h: working on Base Data moving set point; stores,
    updates and delivers all base data informations from CanCustomer_c
    derived for CAN sending and receiving interaction; from
    Scheduler_Task_c derived for interaction with other IsoAgLib
    objects

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef TRACMOVESETPOINT_C_H
#define TRACMOVESETPOINT_C_H

#include "basecommon_c.h"


// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

  class TracMoveSetPoint_c;
  typedef SINGLETON_DERIVED(TracMoveSetPoint_c,BaseCommon_c) SingletonTracMoveSetPoint_c;
  /** working on Base Data Msg;
      stores, updates  and delivers all base data informations;
      Derive from BaseCommon_c some fundamental funktionality for all base data
      Derive from Scheduler_Task_c to register in Scheduler_c for timeEvent trigger
      Derive from CANCustomer to register FilterBox'es in CanIo_c to receive CAN messages
      Derive from SINGLETON to create a Singleton which manages one global accessible singleton
      per IsoAgLib instance (if only one IsoAgLib instance is defined in application config, no overhead is produced).
    */

  class TracMoveSetPoint_c : public SingletonTracMoveSetPoint_c{
  public:// Public methods
    /* ********************************************* */

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
    /** config the TracMoveSetPoint_c object after init -> set pointer to isoName and
        config send/receive of different base msg types
        @param apc_isoName pointer to the ISOName variable of the responsible member instance (pointer enables automatic value update if var val is changed)
      @param at_identMode either IsoAgLib::IdentModeImplement or IsoAgLib::IdentModeTractor
      */
    virtual bool config_base (const IsoName_c* apc_isoName, IsoAgLib::IdentMode_t at_identMode = IsoAgLib::IdentModeImplement, uint16_t aui16_suppressMask = 0);

    /** destructor for TracMove_c which has nothing to do */
    virtual ~TracMoveSetPoint_c() { close();};
    /*@}*/

    /** set commanded direction of the machine
        @param t_val  commanded direction of travel
      */
    void setSelectedDirectionCmd(const IsoAgLib::IsoDirectionFlag_t t_val) {mt_selectedDirectionCmd = t_val;}

    /** set commanded set point value of the machine speed as measured by the selected source
        @param ui16_val  set point value of the machine speed
      */
    void setSelectedSpeedSetPointCmd(const uint16_t ui16_val) {mi16_selectedSpeedSetPointCmd = ui16_val;}

    /** communicate maximum allowed speed to the tractor
        @param ui16_val  maximum allowed speed
      */
    void setSelectedSpeedSetPointLimit(const uint16_t ui16_val) {mi16_selectedSpeedSetPointLimit = ui16_val;}

    /** get commanded direction of the machine
        @return  commanded direction of travel
      */
    IsoAgLib::IsoDirectionFlag_t selectedDirectionCmd() const {return mt_selectedDirectionCmd;}

    /** get commanded set point value of the machine speed as measured by the selected source
        @return  set point value of the machine speed
      */
    uint16_t selectedSpeedSetPointCmd() const {return mi16_selectedSpeedSetPointCmd;}

    /** get communicated maximum allowed speed to the tractor
        @return  maximum allowed speed
      */
    uint16_t selectedSpeedSetPointLimit() const {return mi16_selectedSpeedSetPointLimit;}

  ///  Used for Debugging Tasks in Scheduler_c
  virtual const char* getTaskName() const;

  virtual bool processMsgRequestPGN (uint32_t aui32_pgn, IsoItem_c* apc_isoItemSender, IsoItem_c* apc_isoItemReceiver);

  private:
    // Private methods
    friend class SINGLETON_DERIVED(TracMoveSetPoint_c,BaseCommon_c);
    /** HIDDEN constructor for a TracMoveSetPoint_c object instance which can optional
        set the configuration for send/receive for base msg type 1
        NEVER instantiate a variable of type TracMoveSetPoint_c within application
        only access TracMoveSetPoint_c via getTracMoveSetPointInstance() or getTracMoveSetPointInstance( int riLbsBusNr ) in case more than one ISO11783 BUS is used for IsoAgLib
      */
    TracMoveSetPoint_c() {};

    /** check if filter boxes shall be created - create only filters based
        on active local idents which has already claimed an address
        --> avoid to much Filter Boxes
      */
    virtual void checkCreateReceiveFilter( );

    /** send a ISO11783 base information PGN.
      * this is only called when sending ident is configured and it has already claimed an address
        @pre  function is only called in implement mode
        @see  BaseCommon_c::timeEvent()
        @see  CanIo_c::operator<<
      */
    virtual bool timeEventImplMode( );

    /** process a ISO11783 base information PGN
        @pre  sender of message is existent in monitor list
        @see  CanPkgExt_c::resolveSendingInformation()
      */
    virtual bool processMsg();

  private:
    // private attributes

    /** commanded set point value of the machine speed as measured by the selected source */
    uint16_t mi16_selectedSpeedSetPointCmd;

    /** parameter is used by a machine to communicate it's maximum allowed speed to the tractor */
    uint16_t mi16_selectedSpeedSetPointLimit;

    /** commanded direction of the machine */
    IsoAgLib::IsoDirectionFlag_t mt_selectedDirectionCmd;
  };

  #if defined(PRT_INSTANCE_CNT) && (PRT_INSTANCE_CNT > 1)
  /** C-style function, to get access to the unique TracMoveSetPoint_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an m_index must be given to select the wanted BUS
    */
  TracMoveSetPoint_c& getTracMoveSetPointInstance(uint8_t aui8_instance = 0);
  #else
  /** C-style function, to get access to the unique TracMoveSetPoint_c singleton instance */
  TracMoveSetPoint_c& getTracMoveSetPointInstance(void);
  #endif
}
#endif
