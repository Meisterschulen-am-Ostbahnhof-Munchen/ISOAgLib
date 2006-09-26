/***************************************************************************
                 tracpto_c.h  - working on pto data;
                                stores, updates  and delivers all base data
                                informations from CANCustomer_c derived for
                                CAN sending and receiving interaction;
                                from ElementBase_c derived for
                                interaction with other IsoAgLib objects
                             -------------------
    begin                 Fri Apr 07 2000
    copyright            : (C) 2000 - 2004 by Dipl.-Inform. Achim Spangler
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
#ifndef TRACPTO_C_H
#define TRACPTO_C_H

#include <IsoAgLib/comm/Base/impl/basecommon_c.h>
#include <IsoAgLib/comm/SystemMgmt/ISO11783/impl/isorequestpgnhandler_c.h>

namespace __IsoAgLib { // Begin Namespace __IsoAgLib

/** decide which pto information to send (front/rear) */
typedef enum
{
  sendFrontPto = 0,
  sendRearPto  = 1
} SendPtoData_t;

/** pto information */
typedef struct
{
  /** last time of pto data msg [msec] */
  int32_t i32_lastPto;
  /** pto [1/8RPM] */
  uint16_t ui16_pto8DigitPerRpm;
  /** measured value of the set point of the rotational speed of the power take-off (PTO) output shaft [1/8RPM] */
  uint16_t ui16_ptoSetPoint8DigitPerRpm;
  /** PTO engaged */
  IsoAgLib::IsoActiveFlag_t t_ptoEngaged : 2;
  /** mode of PTO: true == 1000 n/min; false == 540 n/min */
  IsoAgLib::IsoActiveFlag_t t_pto1000 : 2;
  /** economy mode of PTO */
  IsoAgLib::IsoActiveFlag_t t_ptoEconomy : 2;
  /** report tractor ECU's status of engagement */
  IsoAgLib::IsoReqFlag_t t_ptoEngagementReqStatus : 2;
  /** report tractor ECU's status of mode */
  IsoAgLib::IsoReqFlag_t t_ptoModeReqStatus : 2;
  /** report tractor ECU's status of economy mode */
  IsoAgLib::IsoReqFlag_t t_ptoEconomyModeReqStatus : 2;
  /** present limit status of pto shaft speed */
  IsoAgLib::IsoLimitFlag_t t_ptoShaftSpeedLimitStatus : 3;
} PtoData_t;

  class TracPTO_c;
  typedef SINGLETON_DERIVED(TracPTO_c,BaseCommon_c) SingletonTracPto_c;
  /** working on Base Data Msg;
      stores, updates and delivers all base data informations;
      Derive from ElementBase_c to register in Scheduler_c for timeEvent trigger
      Derive from CANCustomer to register FilterBox'es in CANIO_c to receive CAN messages
      Derive from SINGLETON to create a Singleton which manages one global accessible singleton
      per IsoAgLib instance (if only one IsoAgLib instance is defined in application config, no overhead is produced).
    */

  class TracPTO_c : public SingletonTracPto_c
                  , public ISORequestPGNHandler_c
  {
  public:// Public methods
    /* ********************************************* */
    /** \name Management Functions for class TracPTO_c  */
    /*@{*/
    /** check if filter boxes shall be created - create only filters based
        on active local idents which has already claimed an address
        --> avoid to much Filter Boxes
      */
    void checkCreateReceiveFilter();

    /** config the TracPTO_c object after init -> set pointer to isoName and
        config send/receive of different base msg types
        @param rpc_isoName pointer to the ISOName variable of the responsible member instance (pointer enables automatic value update if var val is changed)
        @param rt_identMode either IsoAgLib::IdentModeImplement or IsoAgLib::IdentModeTractor
        @return true -> configuration was successfull
      */
    bool config(const ISOName_c* rpc_isoName, IsoAgLib::IdentMode_t rt_IdentMode);

    /** destructor for Base_c which has nothing to do */
    virtual ~TracPTO_c() { BaseCommon_c::close();};
    /*@}*/

    /* ******************************************* */
    /** \name Set Values for periodic send on BUS  */
    /*@{*/

    /** set rear PTO
      @param ri16_val value to store as the speed of the rear PTO [1RPM]
      */
    void setPtoRear(const uint16_t rui16_val)
    {
      t_ptoRear.ui16_pto8DigitPerRpm = ( rui16_val * 8 );

      if ( rui16_val == 0 ) setPtoRearEngaged( IsoAgLib::IsoInactive );
      else                  setPtoRearEngaged( IsoAgLib::IsoActive );
    };
    /** set front PTO
        @param ri16_val value to store as the speed of the front PTO [1RPM]
      */
    void setPtoFront(const uint16_t rui16_val)
    {
      t_ptoFront.ui16_pto8DigitPerRpm = ( rui16_val * 8 );

      if ( rui16_val == 0 ) setPtoFrontEngaged( IsoAgLib::IsoInactive );
      else                  setPtoFrontEngaged( IsoAgLib::IsoActive );
    };

    /** set rear PTO with unit [1/8RPM] so that the full resolution of ISOBUS messages can be used with integer arithmetic
    @param ri16_val8DigitPerRpm value to store as the speed of the rear PTO [1/8RPM]
     */
    void setPtoRear8DigitPerRpm(const uint16_t rui16_val8DigitPerRpm)
    {
      t_ptoRear.ui16_pto8DigitPerRpm = rui16_val8DigitPerRpm;

      if ( rui16_val8DigitPerRpm == 0 ) setPtoRearEngaged( IsoAgLib::IsoInactive );
      else                              setPtoRearEngaged( IsoAgLib::IsoActive );
    };
    /** set front PTO with unit [1/8RPM] so that the full resolution of ISOBUS messages can be used with integer arithmetic
    @param ri16_val8DigitPerRpm value to store as the speed of the front PTO [1/8RPM]
     */
    void setPtoFront8DigitPerRpm(const uint16_t rui16_val8DigitPerRpm)
    {
      t_ptoFront.ui16_pto8DigitPerRpm = rui16_val8DigitPerRpm;

      if ( rui16_val8DigitPerRpm == 0 ) setPtoFrontEngaged( IsoAgLib::IsoInactive );
      else                              setPtoFrontEngaged( IsoAgLib::IsoActive );
    };

    bool processMsgRequestPGN (uint32_t rui32_pgn, uint8_t rui8_sa, uint8_t rui8_da);
    /** force a request for pgn for front pto state */
    bool sendRequestUpdateFront();
    /** force a request for pgn for rear pto state */
    bool sendRequestUpdateRear();
    /** send pto data message
        @param t_sendptodata  send pto front or pto rear
        @see  TracCert_c::processMsgRequestPGN
        @see  CANIO_c::operator<<
      */
    void sendMessage(SendPtoData_t t_sendptodata);
    /** set explicit information whether front PTO is engaged
      * @param rt_val IsoActive -> PTO is engaged
      */
    void setPtoFrontEngaged(const IsoAgLib::IsoActiveFlag_t rt_val) { t_ptoFront.t_ptoEngaged = rt_val; }
    /** set explicit information whether rear PTO is engaged
      * @param rt_val IsoActive -> PTO is engaged
      */
    void setPtoRearEngaged(const IsoAgLib::IsoActiveFlag_t rt_val) { t_ptoRear.t_ptoEngaged = rt_val;}
    /** set std RPM of front PTO (1000 n/min; 540 n/min)
      * @param rt_val IsoActive == 1000 n/min; false == 540 n/min
      */
    void setPtoFront1000(const IsoAgLib::IsoActiveFlag_t rt_val) { t_ptoFront.t_pto1000 = rt_val;}
    /** set std RPM of rear PTO (1000 n/min; 540 n/min)
      * @param rt_val IsoActive == 1000 n/min; false == 540 n/min
      */
    void setPtoRear1000(const IsoAgLib::IsoActiveFlag_t rt_val) { t_ptoRear.t_pto1000 = rt_val;}
    /** set economy mode of front PTO
      * @param rt_val IsoActive -> front PTO is in economy mode
      */
    void setPtoFrontEconomy(const IsoAgLib::IsoActiveFlag_t rt_val) { t_ptoFront.t_ptoEconomy = rt_val;}
    /** set economy mode of rear PTO
      * @param rt_val IsoActive -> rear PTO is in economy mode
      */
    void setPtoRearEconomy(const IsoAgLib::IsoActiveFlag_t rt_val) { t_ptoRear.t_ptoEconomy = rt_val;}
    /** set measured value of the set point of the rotational speed of the front power take-off (PTO) output shaft
        @param ri16_val requested RPM [1RPM]
      */
    void setFrontPtoSetPoint(const uint16_t ri16_val)  { t_ptoFront.ui16_ptoSetPoint8DigitPerRpm = ( ri16_val * 8 );}
    /** set measured value of the set point of the rotational speed of the rear power take-off (PTO) output shaft
        @param ri16_val requested RPM [1RPM]
      */
    void setRearPtoSetPoint(const uint16_t ri16_val)  { t_ptoRear.ui16_ptoSetPoint8DigitPerRpm = ( ri16_val * 8 );}
    /** set measured value of the set point of the rotational speed of the front power take-off (PTO) output shaft
        @param ri16_val8DigitPerRpm requested RPM [1/8RPM]
     */
    void setFrontPtoSetPoint8DigitPerRpm(const uint16_t ri16_val8DigitPerRpm)
    { t_ptoFront.ui16_ptoSetPoint8DigitPerRpm = ri16_val8DigitPerRpm;}
    /** set measured value of the set point of the rotational speed of the rear power take-off (PTO) output shaft
        @param ri16_val8DigitPerRpm requested RPM [1/8RPM]
     */
    void setRearPtoSetPoint8DigitPerRpm(const uint16_t ri16_val8DigitPerRpm)
    { t_ptoRear.ui16_ptoSetPoint8DigitPerRpm = ri16_val8DigitPerRpm;}
    /** set reported tractor ECU's status of front engagement
        @param rt_val  status to set
      */
    void setFrontPtoEngagementReqStatus(IsoAgLib::IsoReqFlag_t rt_val) {t_ptoFront.t_ptoEngagementReqStatus = rt_val;}
    /** set reported tractor ECU's status of rear engagement
        @param rt_val  status to set
      */
    void setRearPtoEngagementReqStatus(IsoAgLib::IsoReqFlag_t rt_val) {t_ptoRear.t_ptoEngagementReqStatus = rt_val;}

    /** set reported tractor ECU's status of front mode
        @param rt_val  status to set
      */
    void setFrontPtoModeReqStatus(IsoAgLib::IsoReqFlag_t rt_val) {t_ptoFront.t_ptoModeReqStatus = rt_val;}

    /** set reported tractor ECU's status of rear mode
        @param rt_val  status to set
      */
    void setRearPtoModeReqStatus(IsoAgLib::IsoReqFlag_t rt_val) {t_ptoRear.t_ptoModeReqStatus = rt_val;}

    /** set reported tractor ECU's status of front economy mode
        @param rt_val  status to set
      */
    void setFrontPtoEconomyModeReqStatus(IsoAgLib::IsoReqFlag_t rt_val) {t_ptoFront.t_ptoEconomyModeReqStatus = rt_val;}

    /** set reported tractor ECU's status of rear economy mode
        @param rt_val  status to set
      */
    void setRearPtoEconomyModeReqStatus(IsoAgLib::IsoReqFlag_t rt_val) {t_ptoRear.t_ptoEconomyModeReqStatus = rt_val;}

    /** set present limit status of front pto shaft speed
        @param rt_val  status to set
      */
    void setFrontPtoShaftSpeedLimitStatus(IsoAgLib::IsoLimitFlag_t rt_val) {t_ptoFront.t_ptoShaftSpeedLimitStatus = rt_val;}

    /** set present limit status of rear pto shaft speed
        @param rt_val  status to set
      */
    void setRearPtoShaftSpeedLimitStatus(IsoAgLib::IsoLimitFlag_t rt_val) {t_ptoRear.t_ptoShaftSpeedLimitStatus = rt_val;}
    /*@}*/

    /* ****************************************************** */
    /** \name Retrieve Values which are sent from other ECUs  */
    /*@{*/

    /** get rear pto
        @return actual rpm speed of rear PTO [1RPM]
      */
    uint16_t ptoRear() const { return ( t_ptoRear.ui16_pto8DigitPerRpm / 8 );};
    /** get front pto
        @return actual rpm speed of front PTO [1RPM]
      */
    uint16_t ptoFront() const { return ( t_ptoFront.ui16_pto8DigitPerRpm / 8 );};

    /** get rear pto
      @return actual rpm speed of rear PTO [1/8RPM]
     */
    uint16_t ptoRear8DigitPerRpm() const { return ( t_ptoRear.ui16_pto8DigitPerRpm);};
    /** get front pto
      @return actual rpm speed of front PTO [1/8RPM]
     */
    uint16_t ptoFront8DigitPerRpm() const { return ( t_ptoFront.ui16_pto8DigitPerRpm);};

    /** deliver explicit information whether front PTO is engaged
      * @return IsoActive -> PTO is engaged
      */
    IsoAgLib::IsoActiveFlag_t ptoFrontEngaged() const { return t_ptoFront.t_ptoEngaged;}
    /** deliver explicit information whether rear PTO is engaged
      * @return IsoActive -> PTO is engaged
      */
    IsoAgLib::IsoActiveFlag_t ptoRearEngaged() const { return t_ptoRear.t_ptoEngaged;}
    /** deliver std RPM of front PTO (1000 n/min; 540 n/min)
      * @return IsoActive == 1000 n/min; false == 540 n/min
      */
    IsoAgLib::IsoActiveFlag_t ptoFront1000() const { return t_ptoFront.t_pto1000;}
    /** deliver std RPM of rear PTO (1000 n/min; 540 n/min)
      * @return IsoActive == 1000 n/min; false == 540 n/min
      */
    IsoAgLib::IsoActiveFlag_t ptoRear1000() const { return t_ptoRear.t_pto1000;}
    /** deliver economy mode of front PTO
      * @return IsoActive -> front PTO is in economy mode
      */
    IsoAgLib::IsoActiveFlag_t ptoFrontEconomy() const { return t_ptoFront.t_ptoEconomy;}
    /** deliver economy mode of rear PTO
      * @return IsoActive -> rear PTO is in economy mode
      */
    IsoAgLib::IsoActiveFlag_t ptoRearEconomy() const { return t_ptoRear.t_ptoEconomy;}
    /** get measured value of the set point of the rotational speed of the front power take-off (PTO) output shaft
        @return measured value of the set point [1RPM]
      */
    uint16_t frontPtoSetPoint() const { return ( t_ptoFront.ui16_ptoSetPoint8DigitPerRpm / 8 );}
    /** get measured value of the set point of the rotational speed of the rear power take-off (PTO) output shaft
        @return measured value of the set point [1RPM]
      */
    uint16_t rearPtoSetPoint() const { return ( t_ptoRear.ui16_ptoSetPoint8DigitPerRpm / 8 );}
    /** get measured value of the set point of the rotational speed of the front power take-off (PTO) output shaft
        @return measured value of the set point [1/8RPM]
     */
    uint16_t frontPtoSetPoint8DigitPerRpm() const { return t_ptoFront.ui16_ptoSetPoint8DigitPerRpm;}
    /** get measured value of the set point of the rotational speed of the rear power take-off (PTO) output shaft
        @return measured value of the set point [1/8RPM]
     */
    uint16_t rearPtoSetPoint8DigitPerRpm() const { return t_ptoRear.ui16_ptoSetPoint8DigitPerRpm;}
    /** get reported tractor ECU's status of front engagement
        @return  reported status
      */
    IsoAgLib::IsoReqFlag_t frontPtoEngagementReqStatus() const {return t_ptoFront.t_ptoEngagementReqStatus;}
    /** get reported tractor ECU's status of rear engagement
        @return  reported status
      */
    IsoAgLib::IsoReqFlag_t rearPtoEngagementReqStatus() const {return t_ptoRear.t_ptoEngagementReqStatus;}
    /** get reported tractor ECU's status of front mode
         @return  reported status
      */
    IsoAgLib::IsoReqFlag_t frontPtoModeReqStatus() const {return t_ptoFront.t_ptoModeReqStatus;}
    /** get reported tractor ECU's status of rear mode
        @return  reported status
      */
    IsoAgLib::IsoReqFlag_t rearPtoModeReqStatus() const {return t_ptoRear.t_ptoModeReqStatus;}
    /** get reported tractor ECU's status of front economy mode
        @return  reported status
      */
    IsoAgLib::IsoReqFlag_t frontPtoEconomyModeReqStatus() const {return t_ptoFront.t_ptoEconomyModeReqStatus;}
    /** get reported tractor ECU's status of rear economy mode
        @return  reported status
      */
    IsoAgLib::IsoReqFlag_t rearPtoEconomyModeReqStatus()const {return t_ptoRear.t_ptoEconomyModeReqStatus;}
    /** get present limit status of front pto shaft speed
        @return  present limit status
      */
    IsoAgLib::IsoLimitFlag_t frontPtoShaftSpeedLimitStatus() const {return t_ptoFront.t_ptoShaftSpeedLimitStatus;}
    /** get present limit status of rear pto shaft speed
        @return  present limit status
      */
    IsoAgLib::IsoLimitFlag_t rearPtoShaftSpeedLimitStatus() const {return t_ptoRear.t_ptoShaftSpeedLimitStatus;}
    /*@}*/

  private:
    // Private methods
    friend class SINGLETON_DERIVED(TracPTO_c,BaseCommon_c);
    /**
      HIDDEN constructor for a TracPTO_c object instance which can optional
      set the configuration for send/receive for base msg type 1,2
      and calendar
      NEVER instantiate a variable of type TracPTO_c within application
      only access TracPTO_c via getTracPTOInstance() or getTracPTOInstance( int riLbsBusNr ) in case more than one BUS is used for IsoAgLib
    */
    TracPTO_c() {};

    /** send a ISO11783 base information PGN.
      * this is only called when sending ident is configured and it has already claimed an address
        @pre  function is only called in tractor mode
        @see  BaseCommon_c::timeEvent()
      */
    virtual bool timeEventTracMode( );

    /** Detect stop of PTO update from tractor -> indication for stopped PTO */
    virtual bool timeEventImplMode();

    /** process a ISO11783 base information PGN */
    bool processMsg();

  private:
    // Private attributes
    /** pto front information */
    PtoData_t t_ptoFront;
    /** pto rear information */
    PtoData_t t_ptoRear;
  };

  #if defined(PRT_INSTANCE_CNT) && (PRT_INSTANCE_CNT > 1)
  /** C-style function, to get access to the unique TracPTO_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  TracPTO_c& getTracPtoInstance(uint8_t rui8_instance = 0);
  #else
  /** C-style function, to get access to the unique TracPTO_c singleton instance */
  TracPTO_c& getTracPtoInstance(void);
  #endif
}
#endif
